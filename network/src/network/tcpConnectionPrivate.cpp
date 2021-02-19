#include "tcpConnectionPrivate.h"

#include <iostream>
#include <string>
#include "logWriter.h"

#include "internal.h"
#include "common.h"
#include "tcpConnection.h"

struct WriteReq {
	uv_write_t req;
	uv_buf_t buf;
	//std::string source;
	OnWritedCallback callback;
	//TcpConnectionPtr conn;
};

TcpConnectionPrivate::~TcpConnectionPrivate()
{
}

TcpConnectionPrivate::TcpConnectionPrivate(Loop* loop, std::string& id, bool isConnected)
	: id_(id),
	isConnected_(isConnected),
	loop_(loop)
{
	
	
	// 	if (GlobalConfig::BufferModeStatus == GlobalConfig::ListBuffer)
	// 	{
	// 		buffer_ = std::make_shared<ListBuffer>();
	// 	}
	// 	else if (GlobalConfig::BufferModeStatus == GlobalConfig::CycleBuffer)
	// 	{
	// 		buffer_ = std::make_shared<CycleBuffer>();
	// 	}
}

void TcpConnectionPrivate::onDataReceive(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
{
	auto connection = static_cast<TcpConnectionPrivate*>(socket->data);
	if (nread > 0) {
		connection->onData(buf->base, nread);
	}
	else if (nread < 0) {
		//connection->setIsConnected(false);
		connection->lastError_ = (int)nread;
		//is greater than 0, the error be happened.
		if (connection->lastError_) {
			connection->lastErrorMsg_ = uv_err_name(connection->lastError_);
			//libuv  uv_err_name(), if err = 0, 则存在内存泄露
			//msg = uv_err_name(0);
			//delete[] msg;
		}
		//log
		LogWriter::instance().error(connection->lastErrorMsg_);

		if (nread != UV_EOF) {
			connection->onClosing();
			return;
		}

		uv_shutdown_t* sreq = new uv_shutdown_t;
		sreq->data = static_cast<void*>(connection);
		::uv_shutdown(sreq, (uv_stream_t*)socket,
			[](uv_shutdown_t* req, int status)
		{
			auto connection = static_cast<TcpConnectionPrivate*>(req->data);
			connection->onClosing();
			delete req;
		});
	}
	else {
		/* Everything OK, but nothing read. */
	}
}

void TcpConnectionPrivate::onData(const char* buf, ssize_t size) {
	if (!isConnected_) return;
	if(on_data_cb_)
		on_data_cb_(connection_.lock(), buf, size);
}

void TcpConnectionPrivate::setConnection(std::shared_ptr<TcpConnection> conn)
{
	connection_ = conn;
}

std::shared_ptr<TcpConnection> TcpConnectionPrivate::getConnection()
{
	return connection_.lock();
}

void TcpConnectionPrivate::onClosing()
{
	//ydm 在这里是否需要先通过server将connection删除后再做真正的close，或者每次write时先判断isConnected再说
// 	if (!isClosing_) 
// 	{
// 		close();
// 	}
	if (socketCloseType_ == SocketCloseType::Unknow)
		socketCloseType_ = SocketCloseType::Passive;
	if (on_closing_cb)
		on_closing_cb(id_);
}

void TcpConnectionPrivate::close(TcpOnClosedCallback cb)
{
	setIsConnected(false);
	on_data_cb_ = nullptr;
	on_closing_cb = nullptr;

	on_closed_cb_ = cb;

	if (socketCloseType_ == SocketCloseType::Unknow)
		socketCloseType_ = SocketCloseType::Active;

	uv_tcp_t* ptr = handle_.get();
	if (::uv_is_active((uv_handle_t*)ptr)) {
		::uv_read_stop((uv_stream_t*)ptr);
	}
	if (::uv_is_closing((uv_handle_t*)ptr) == 0) {
		//libuv 在loop轮询中会检测关闭句柄，delete会导致程序异常退出。
		::uv_close((uv_handle_t*)ptr,
			[](uv_handle_t* handle)
		{
			auto connection = static_cast<TcpConnectionPrivate*>(handle->data);
			connection->onClosed();
		});
	}
	else {
		onClosed();
	}
}

int TcpConnectionPrivate::write(const char* buf, ssize_t size, OnWritedCallback callback) {
	int rlt = 0;
	if (isConnected_) {
		WriteReq* req = new WriteReq;
		req->buf = uv_buf_init(const_cast<char*>(buf), static_cast<unsigned int>(size));
		req->callback = callback;
		//req->source = id_;
		//req->conn = shared_from_this();

		auto ptr = handle_.get();
		//0 is success else fail.
		rlt = ::uv_write((uv_write_t*)req, (uv_stream_t*)ptr, &req->buf, 1,
			[](uv_write_t *req, int status)
		{
			WriteReq* wr = (WriteReq*)req;
			if (nullptr != wr->callback) {
				struct WriteResult info;
				info.buf = const_cast<char*>(wr->buf.base);
				info.size = wr->buf.len;
				//info.id = wr->source;
				info.status = status;
				if (status) {
					info.errMsg = uv_err_name(status);
				} else {
					info.errMsg = "success";
				}

				wr->callback(info);
			}
			delete wr;
		});
		if (0 != rlt)
		{
			//LogWriter::Instance()->error(std::string("write data error:" + std::to_string(rst)));
			if (nullptr != callback)
			{
				struct WriteResult info;
				info.status = rlt;
				info.id = "";
				info.errMsg = "";
				info.buf = const_cast<char*>(buf);
				info.size = static_cast<unsigned long>(size);
				callback(info);
			}
			delete req;
		}
	}
	else
	{
		rlt = -1;
		if (nullptr != callback) {
			struct WriteResult info;
			info.status = WriteResult::result_disconnected;
			info.id = "";
			info.errMsg = "";
			info.buf = const_cast<char*>(buf);
			info.size = static_cast<unsigned long>(size);
			//WriteResult info = { WriteResult::result_disconnected,"",const_cast<char*>(buf),"", static_cast<unsigned long>(size) };
			callback(info);
		}
	}
	return rlt;
}

void TcpConnectionPrivate::writeInLoop(const char* buf, ssize_t size, OnWritedCallback callback) {
	std::weak_ptr<TcpConnectionPrivate> conn = shared_from_this();
	loop_->runInLoop(
		[conn, buf, size, callback]()
	{
		std::shared_ptr<TcpConnectionPrivate> ptr = conn.lock();
		if (ptr != nullptr)
		{
			ptr->write(buf, size, callback);
		}
		else
		{
			struct WriteResult info;
			info.status = WriteResult::result_disconnected;
			info.id = "";
			info.errMsg = "";
			info.buf = const_cast<char*>(buf);
			info.size = static_cast<unsigned long>(size);
			//struct WriteResult info = { WriteResult::result_disconnected,"", const_cast<char*>(buf), "", static_cast<unsigned long>(size) };
			callback(info);
		}
	});
}

// void TcpConnectionPrivate::setWrapper(ConnectionWrapperPtr wrapper) {
// 	wrapper_ = wrapper;
// }
// 
// std::shared_ptr<ConnectionWrapper> TcpConnection::getWrapper() {
// 	return wrapper_.lock();
// }

void TcpConnectionPrivate::setOnDataCallback(TcpOnDataCallback cb)
{
	on_data_cb_ = cb;
}

void TcpConnectionPrivate::setOnClosingCallback(TcpOnClosingCallback cb)
{
	on_closing_cb = cb;
}

void TcpConnectionPrivate::setOnClosedCallback(TcpOnClosedCallback cb)
{
	on_closed_cb_ = cb;
}

void TcpConnectionPrivate::onClosed()
{
	if (on_closed_cb_ != nullptr) {
		on_closed_cb_(id_);
	}
// 	if (sink_) {
// 		//std::weak_ptr<TcpConnection> conn = shared_from_this();
// 		//std::shared_ptr<TcpConnection> ptr = conn.lock();
// 		std::shared_ptr<TcpConnection> ptr = shared_from_this();
// 		if (ptr != nullptr) {
// 			//sink_->onDisconnect(ptr);
// 		}
// 	}
}

// void TcpConnection::setSink(ITcpConnectionSink * sink)
// {
// 	sink_ = sink;
// }
// 
// ITcpConnectionSink * TcpConnection::getSink()
// {
// 	return sink_;
// }

SocketCloseType TcpConnectionPrivate::getCloseType()
{
	return socketCloseType_;
}

int TcpConnectionPrivate::getLastError()
{
	return lastError_;
}

std::string TcpConnectionPrivate::getLastErrorMsg()
{
	return lastErrorMsg_;
}

void TcpConnectionPrivate::setHandle(UVTcpPtr socket)
{
	handle_ = socket;
	handle_->data = static_cast<void*>(this);
}

void TcpConnectionPrivate::start()
{
	::uv_read_start((uv_stream_t*)handle_.get(),
		[](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
	{
		//std::cout << "suggested_size = " << suggested_size << endl;
		auto conn = static_cast<TcpConnectionPrivate*>(handle->data);
		buf->base = conn->resizeReadBuffer(suggested_size);
#if _MSC_VER
		buf->len = (ULONG)suggested_size;
#else
		buf->len = suggested_size;
#endif
	},
		&TcpConnectionPrivate::onDataReceive);
}

void TcpConnectionPrivate::setIsConnected(bool connected)
{
	isConnected_ = connected;
}

bool TcpConnectionPrivate::isConnected()
{
	return isConnected_;
}

const std::string& TcpConnectionPrivate::id()
{
	return id_;
}

char* TcpConnectionPrivate::resizeReadBuffer(size_t size)
{
	readBuffer_.resize(size);
	return const_cast<char*>(readBuffer_.c_str());
}

// PacketBufferPtr TcpConnection::getPacketBuffer()
// {
// 	return buffer_;
// }
