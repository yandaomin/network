#include "tcpServer.h"
#include <iostream>
#include "tcpServerPrivate_.h"
#include "ref.h"
#include "tcpConnection.h"
#include "common.h"


TcpServer::TcpServer(Loop* loop){
	private_ = new TcpServerPrivate_(loop);
}

TcpServer::~TcpServer()
{
	if (private_) {
		delete private_;
		private_ = nullptr;
	}
}

Loop* TcpServer::getLoop() {
	return private_->getLoop();
}

std::map<std::string, TcpConnectionPtr>& TcpServer::sockets(){
	return private_->sockets();
}

void TcpServer::setAddr(std::string addr) {
	private_->setAddr(addr);
}

std::string TcpServer::getAddr() {
	return private_->getAddr();
}

void TcpServer::setPort(int port) {
	private_->setPort(port);
}

int TcpServer::getPort() {
	return private_->getPort();
}

void TcpServer::setIPV(IPV ipv)
{
	private_->setIPV((ipv == IPV::ipv4) ? SocketAddr::Ipv4: SocketAddr::Ipv6);
}

IPV TcpServer::getIPV()
{
	return (private_->getIPV() == SocketAddr::Ipv4)? IPV::ipv4: IPV::ipv6;
}

void TcpServer::setTimeout(int secs) {
	private_->setTimeout(secs);
}

int TcpServer::write(std::string & id, const char * buf, unsigned int size, OnWritedCallback cb)
{
	return private_->write(id, buf, size, cb);
}

void TcpServer::writeInLoop(std::string & id, const char * buf, unsigned int size, OnWritedCallback cb)
{
	private_->writeInLoop(id, buf, size, cb);
}

int TcpServer::start() {
	return private_->start();
}

void TcpServer::close(ActionCallback callback)
{
	private_->close(callback);
}

int TcpServer::getClientCount() {
	return private_->getClientCount();
}

void TcpServer::setOnConnectionCallback(TcpOnConnectionCallback cb) {
	private_->setOnConnectionCallback(cb);
}

void TcpServer::setOnDisconnectCallback(TcpOnDisconnectCallback cb) {
	private_->setOnDisconnectCallback(cb);
}

void TcpServer::setOnDataCallback(TcpOnDataCallback cb) {
	private_->setOnDataCallback(cb);
}
