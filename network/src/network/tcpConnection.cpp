#include "tcpConnection.h"
#include <iostream>
#include <string>
#include "logWriter.h"
#include "tcpConnectionPrivate.h"

TcpConnection::~TcpConnection()
{
}

TcpConnection::TcpConnection(Loop* loop, std::string& id, bool isConnected) {
	private_ = std::make_shared<TcpConnectionPrivate>(loop, id, isConnected);
	//private_.reset(new TcpConnectionPrivate(loop, id,isConnected));
}

void TcpConnection::close(TcpOnClosedCallback cb)
{
	private_->close(cb);
}

int TcpConnection::write(const char* buf, long size, OnWritedCallback cb) {
	return private_->write(buf, size, cb);
}

void TcpConnection::writeInLoop(const char* buf, long size, OnWritedCallback cb) {
	private_->writeInLoop(buf, size, cb);
}

void TcpConnection::setWrapper(ConnectionWrapperPtr wrapper) {
	wrapper_ = wrapper;
}

std::shared_ptr<ConnectionWrapper> TcpConnection::getWrapper() {
	return wrapper_.lock();
}

void TcpConnection::setOnDataCallback(TcpOnDataCallback cb)
{
	private_->setOnDataCallback(cb);
}

void TcpConnection::setOnClosingCallback(TcpOnClosingCallback cb)
{
	private_->setOnClosingCallback(cb);
}

void TcpConnection::setOnClosedCallback(TcpOnClosedCallback cb)
{
	private_->setOnClosedCallback(cb);
}

SocketCloseType TcpConnection::getCloseType()
{
	return private_->getCloseType();
}

int TcpConnection::getLastError()
{
	return private_->getLastError();
}

std::string TcpConnection::getLastErrorMsg()
{
	return private_->getLastErrorMsg();
}

void TcpConnection::onClosing()
{
	private_->onClosing();
}

std::shared_ptr<TcpConnectionPrivate> TcpConnection::getPrivate()
{
	return private_;
}

bool TcpConnection::isConnected()
{
	return private_->isConnected();
}

const std::string& TcpConnection::id()
{
	return private_->id();
}
