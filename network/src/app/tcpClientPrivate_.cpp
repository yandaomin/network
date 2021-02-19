#include "tcpClientPrivate_.h"
#include <iostream>
#include "tcpConnection.h"

TcpClientPrivate_::TcpClientPrivate_(Loop* loop): loop_(loop)
{
}

TcpClientPrivate_::~TcpClientPrivate_()
{
}

void TcpClientPrivate_::onConnection(TcpConnectionPtr conn)
{
	conn->setOnDataCallback(std::bind(&TcpClientPrivate_::onData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	conn->setOnClosedCallback(std::bind(&TcpClientPrivate_::onDisConnect, this, std::placeholders::_1));
	connection_ = conn;
	//std::string msg = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\r\n";
	//if(count_ >= 3)
	//connection_->write(msg.data(), msg.length());
}

void TcpClientPrivate_::onData(TcpConnectionPtr conn, const char * buf, int size)
{
	//conn->write(buf, size);
}


void TcpClientPrivate_::onConnect(bool isConnected, std::string& errMsg)
{
	if (on_connect_cb_)
		on_connect_cb_(isConnected, errMsg);
}

void TcpClientPrivate_::onDisConnect(std::string& id)
{
	if (!connection_) return;
	connection_->close([this](std::string& id)
	{
		if (nullptr != connection_) {
			if (on_disconnect_cb_)
				on_disconnect_cb_(id, connection_->getCloseType(), connection_->getLastErrorMsg());
			connection_ = nullptr;
		}
	});
}

Loop * TcpClientPrivate_::getLoop() {
	return loop_;
}

TcpConnectionPtr TcpClientPrivate_::getConnection() {
	return connection_;
}

void TcpClientPrivate_::connect(const std::string& ip, int port) {
	if (!connecter_) {
		connecter_ = std::make_shared<TcpConnecter>(loop_);
		connecter_->setConnectCallback(std::bind(&TcpClientPrivate_::onConnect, this, std::placeholders::_1, std::placeholders::_2));
		connecter_->setOnConnectionCallback(std::bind(&TcpClientPrivate_::onConnection, this, std::placeholders::_1));
	}
	addr_ = ip;
	port_ = port;
	SocketAddr addr(addr_, port_);
	connecter_->connect(addr);
}

void TcpClientPrivate_::setTcpOnDataCallback(TcpOnDataCallback cb)
{
	on_data_cb_ = cb;
}

void TcpClientPrivate_::setTcpOnDisconnectCallback(TcpOnDisconnectCallback cb)
{
	on_disconnect_cb_ = cb;
}

void TcpClientPrivate_::setTcpOnConnectCallback(TcpOnConnectCallback cb)
{
	on_connect_cb_ = cb;
}

int TcpClientPrivate_::write(const char * buf, long size, OnWritedCallback cb)
{
	int rlt = -1;
	if (connection_)
		rlt = connection_->write(buf, size, cb);
	return rlt;
}

void TcpClientPrivate_::writeInLoop(const char * buf, long size, OnWritedCallback cb)
{
	if (connection_)
		connection_->writeInLoop(buf, size, cb);
}

// void TcpClientPrivate_::setSockets(std::vector<TcpClient*>* sockets)
// {
// 	sockets_ = sockets;
// }


