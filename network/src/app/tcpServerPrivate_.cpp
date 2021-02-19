#include "tcpServerPrivate_.h"
#include "logWriter.h"
#include <iostream>
#include "tcpConnection.h"
#include "tcpConnectionWrapper.h"

TcpServerPrivate_::TcpServerPrivate_(Loop* loop)
	: loop_(loop),
	timerWheel_(loop)
{
	accepter_ = std::make_shared<TcpAccepter>(loop_);
	accepter_->setOnConnectinonCallback(std::bind(&TcpServerPrivate_::doOnConnection, this, std::placeholders::_1));
}

TcpServerPrivate_::~TcpServerPrivate_()
{
}

void TcpServerPrivate_::doOnConnection(TcpConnectionPtr conn)
{
	sockets_[conn->id()] = conn;
	if (timerWheel_.getTimeout() > 0) {
		auto wrapper = std::make_shared<ConnectionWrapper>(conn);
		conn->setWrapper(wrapper);
		timerWheel_.insert(wrapper);
	}
	conn->setOnClosingCallback(std::bind(&TcpServerPrivate_::doOnDisconnect, this, std::placeholders::_1));
	conn->setOnDataCallback(std::bind(&TcpServerPrivate_::doOnData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	if (on_connection_cb_)
		on_connection_cb_(conn->id());
}

void TcpServerPrivate_::doOnData(TcpConnectionPtr conn, const char * buf, long size)
{
	if (on_data_cb_)
		//on_data_cb_(conn->id(), buf, size);
		on_data_cb_(conn, buf, size);
	if (timerWheel_.getTimeout() > 0)
	{
		timerWheel_.insert(conn->getWrapper());
	}
	return;
}

// void TcpServer::onWrite(TcpConnectionPtr conn, WriteResult & writeResult)
// {
// }

void TcpServerPrivate_::doOnDisconnect(std::string& id)
{
	TcpConnectionPtr conn = getConnnection(id);
	if (nullptr != conn)
	{
		conn->close([this](std::string& id)
		{
			TcpConnectionPtr conn = getConnnection(id);
			if (nullptr != conn) {
				if (on_disconnection_cb_) {
					on_disconnection_cb_(id, conn->getCloseType(), conn->getLastErrorMsg());
				}
				sockets_.erase(id);
			}
		});
	}
}

std::map<std::string, TcpConnectionPtr>& TcpServerPrivate_::sockets()
{
// 	for (auto socket : sockets_) {
// 		clients.push_back(socket.second);
// 	}
	return sockets_;
}

Loop * TcpServerPrivate_::getLoop() {
	return loop_;
}

std::shared_ptr<TcpConnection> TcpServerPrivate_::getConnnection(const std::string& id)
{
    auto rst = sockets_.find(id);
    if(rst == sockets_.end()) {
        return nullptr;
    }
    return rst->second;
}

void TcpServerPrivate_::setAddr(std::string addr) {
	addr_ = addr;
}

std::string TcpServerPrivate_::getAddr() {
	return addr_;
}

void TcpServerPrivate_::setPort(int port) {
	port_ = port;
}

int TcpServerPrivate_::getPort() {
	return port_;
}

void TcpServerPrivate_::setIPV(SocketAddr::IPV ipv)
{
	ipv_ = ipv;
}

SocketAddr::IPV TcpServerPrivate_::getIPV()
{
	return ipv_;
}

void TcpServerPrivate_::setTimeout(int secs) {
	timerWheel_.setTimeout(secs);
}

int TcpServerPrivate_::write(TcpConnectionPtr connection, const char * buf, unsigned int size, OnWritedCallback cb)
{
	int rlt = -1;
	if (nullptr != connection) {
		rlt = connection->write(buf, size, cb);
	}
	else if (cb) {
		WriteResult info;
		info.buf = const_cast<char*>(buf);
		info.size = size;
		info.id = "";
		info.status = WriteResult::result_disconnected;
		cb(info);
	}
	return rlt;
}

void TcpServerPrivate_::writeInLoop(TcpConnectionPtr connection, const char * buf, unsigned int size, OnWritedCallback cb)
{
	if (nullptr != connection)
	{
		connection->writeInLoop(buf, size, cb);
	}
	else if (cb)
	{
		LogWriter::instance().warn("try write disconnected connection.");
		WriteResult info;
		info.buf = const_cast<char*>(buf);
		info.size = size;
		info.id = "";
		info.status = WriteResult::result_disconnected;
		cb(info);
	}
}

int TcpServerPrivate_::write(std::string & id, const char * buf, unsigned int size, OnWritedCallback cb)
{
	auto connection = getConnnection(id);
	return write(connection, buf, size, cb);
}

void TcpServerPrivate_::writeInLoop(std::string & id, const char * buf, unsigned int size, OnWritedCallback cb)
{
	auto connection = getConnnection(id);
	writeInLoop(connection, buf, size, cb);
}

int TcpServerPrivate_::start() {
	int rlt = 0;
	SocketAddr addr(addr_, port_, ipv_);
	rlt = accepter_->bind(addr);
	if (rlt) {
		return rlt;
	}
	rlt = accepter_->listen();
	if (rlt) {
		return rlt;
	}
	timerWheel_.start();
	return rlt;
}

void TcpServerPrivate_::close(ActionCallback callback)
{
	if (accepter_)
		accepter_->close([this, callback]()
	{
		for (auto& connection : sockets_)
		{
			connection.second->onClosing();
		}
		callback();
	});
}

int TcpServerPrivate_::getClientCount() {
	return sockets_.size();
}

void TcpServerPrivate_::setOnConnectionCallback(TcpOnConnectionCallback cb) {
	on_connection_cb_ = cb;
}

void TcpServerPrivate_::setOnDisconnectCallback(TcpOnDisconnectCallback cb) {
	on_disconnection_cb_ = cb;
}

void TcpServerPrivate_::setOnDataCallback(TcpOnDataCallback cb) {
	on_data_cb_ = cb;
}



// void TcpServer::onTime(Timer * timer)
// {
// }
