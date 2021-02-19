#include "tcpClient.h"
#include <iostream>
#include "tcpClientPrivate_.h"
#include "tcpConnection.h"

TcpClient::TcpClient(Loop* loop)
{
	private_ = new TcpClientPrivate_(loop);
}

TcpClient::~TcpClient()
{
	if (private_) {
		delete private_;
		private_ = nullptr;
	}
}

Loop * TcpClient::getLoop() {
	return private_->getLoop();
}

TcpConnectionPtr TcpClient::getConnection() {
	return private_->getConnection();
}

void TcpClient::connect(const std::string& ip, int port) {
	private_->connect(ip, port);
}

void TcpClient::setTcpOnDataCallback(TcpOnDataCallback cb)
{
	private_->setTcpOnDataCallback(cb);
}

void TcpClient::setTcpOnDisconnectCallback(TcpOnDisconnectCallback cb)
{
	private_->setTcpOnDisconnectCallback(cb);
}

void TcpClient::setTcpOnConnectCallback(TcpOnConnectCallback cb)
{
	private_->setTcpOnConnectCallback(cb);
}

int TcpClient::write(const char * buf, long size, OnWritedCallback cb)
{
	return private_->write(buf, size, cb);
}

void TcpClient::writeInLoop(const char * buf, long size, OnWritedCallback cb)
{
	private_->writeInLoop(buf, size, cb);
}
