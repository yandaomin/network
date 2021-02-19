#include "tcpConnecter.h"
#include <iostream>
#include <string>
#include "logWriter.h"
#include "tcpConnection.h"
#include "tcpConnectionPrivate.h"

///////////////////TcpConnector/////////////////////
TcpConnecter::TcpConnecter(Loop* loop, bool tcpNoDelay)
	:loop_(loop),
	connector_(new uv_connect_t()),
	tcpNoDelay_(tcpNoDelay)
{
	connector_->data = static_cast<void*>(this);
}

TcpConnecter::~TcpConnecter() {
	delete connector_;
}

bool TcpConnecter::isTcpNoDelay() {
	return tcpNoDelay_;
}

void TcpConnecter::setTcpNoDelay(bool isNoDelay) {
	tcpNoDelay_ = isNoDelay;
}


void TcpConnecter::connect(SocketAddr& addr) {
	socket_ = std::make_shared<uv_tcp_t>();
	::uv_tcp_init((uv_loop_t*)(loop_->handle()), socket_.get());
	if (tcpNoDelay_)
		::uv_tcp_nodelay(socket_.get(), 1);

	addr_ = addr;
	::uv_tcp_connect(connector_, socket_.get(), addr.addr(), [](uv_connect_t* req, int status)
	{
		auto handle = static_cast<TcpConnecter*>((req->data));
		//0 success else fail.
		handle->onConnect(status);
	});
}

void TcpConnecter::setOnConnectionCallback(OnConnectionCallback cb) {
	on_connection_cb_ = cb;
}

void TcpConnecter::setConnectCallback(ConnectCallback cb) {
	connect_cb_ = cb;
}

void TcpConnecter::onConnect(int status) {
	std::string errMsg = status == 0 ? "success" : uv_err_name(status);
	connect_cb_(status == 0, errMsg);
	if (!status) {
		std::string id;
		SocketAddr::socketAddrToString(socket_.get(), id, addr_.ipv());

		TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_, id);
		//私有对象的赋值及开发读取数据
		std::shared_ptr<TcpConnectionPrivate> conPrivate = conn->getPrivate();
		conPrivate->setHandle(socket_);
		conPrivate->setConnection(conn);
		conPrivate->start();
		on_connection_cb_(conn);
	} else {
		if (::uv_is_active((uv_handle_t*)socket_.get())) {
			::uv_read_stop((uv_stream_t*)socket_.get());
		}
		if (::uv_is_closing((uv_handle_t*)socket_.get()) == 0) {
			socket_->data = static_cast<void*>(this);
			::uv_close((uv_handle_t*)socket_.get(), NULL);
		}
	}
}


Loop* TcpConnecter::loop() {
	return loop_;
}