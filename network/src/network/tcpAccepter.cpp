#include "tcpAccepter.h"
#include <iostream>
#include <string>
#include "logWriter.h"
#include "tcpConnection.h"
#include "tcpConnectionPrivate.h"

/////////////////////TcpAccepter///////////////////////
TcpAccepter::TcpAccepter(Loop* loop, bool tcpNoDelay)
	:isListened_(false),
	tcpNoDelay_(tcpNoDelay),
	loop_(loop),
	onConnectionCallback_(nullptr),
	onCloseCallback_(nullptr)
{
	::uv_tcp_init((uv_loop_t*)(loop_->handle()), &server_);
	if (tcpNoDelay_)
		::uv_tcp_nodelay(&server_, 1);
	server_.data = (void*)this;
}

TcpAccepter:: ~TcpAccepter() {

}

Loop* TcpAccepter::loop() {
	return loop_;
}

void TcpAccepter::onConnection(UVTcpPtr socket) {
	//异常处理
	if (!onConnectionCallback_) {
		::uv_close((uv_handle_t*)socket.get(), NULL);
		return;
	}

	std::string key;
	SocketAddr::socketAddrToString(socket.get(), key, addr_.ipv());
	std::shared_ptr<TcpConnection> connection(new TcpConnection(loop_, key));
	//私有对象的赋值及开发读取数据
	std::shared_ptr<TcpConnectionPrivate> conPrivate = connection->getPrivate();
	conPrivate->setHandle(socket);
	conPrivate->setConnection(connection);
	conPrivate->start();
 	if (!connection) {
 		LogWriter::instance().error("create connection fail. :" + key);
 	}
	if (onConnectionCallback_) {
		onConnectionCallback_(connection);
	}
}

void TcpAccepter::onClose() {
	if (onCloseCallback_)
		onCloseCallback_();
}

int TcpAccepter::bind(SocketAddr& addr) {
	addr_ = addr;
	return ::uv_tcp_bind(&server_, addr.addr(), 0);
}

int TcpAccepter::listen() {
	auto rlt = ::uv_listen((uv_stream_t*)&server_, 1280,
		[](uv_stream_t *listenSocket, int status)
	{
		if (status < 0) {
			LogWriter::instance().error(std::string("New connection error :") + Loop::getErrorMessage(status));
			return;
		}
		TcpAccepter* accepter = static_cast<TcpAccepter*>(listenSocket->data);
		UVTcpPtr socket = std::make_shared<uv_tcp_t>();
		::uv_tcp_init((uv_loop_t*)(accepter->loop()->handle()), socket.get());
		if (accepter->isTcpNoDelay())
			::uv_tcp_nodelay(socket.get(), 1);

		if (0 == ::uv_accept(listenSocket, (uv_stream_t*)socket.get())) {
			accepter->onConnection(socket);
		} else {
			::uv_close((uv_handle_t*)socket.get(), NULL);
		}
	});
	if (rlt == 0) {
		isListened_ = true;
	}
	return rlt;
}


bool TcpAccepter::isListen() {
	return isListened_;
}

void TcpAccepter::close(ActionCallback callback) {
	onCloseCallback_ = callback;
	auto ptr = &server_;
	if (::uv_is_active((uv_handle_t*)ptr)) {
		::uv_read_stop((uv_stream_t*)ptr);
	}
	if (::uv_is_closing((uv_handle_t*)ptr) == 0) {
		::uv_close((uv_handle_t*)ptr,
			[](uv_handle_t* handle)
		{
			auto accept = static_cast<TcpAccepter*>(handle->data);
			accept->onClose();
		});
	}else {
		onClose();
	}
}

bool TcpAccepter::isTcpNoDelay() {
	return tcpNoDelay_;
}

void TcpAccepter::setOnConnectinonCallback(OnConnectionCallback callback) {
	onConnectionCallback_ = callback;
}