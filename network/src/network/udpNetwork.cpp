#include "udpNetwork.h"
#include "logWriter.h"
#include "udpNetworkPrivate.h"

UdpNetwork::UdpNetwork(Loop* loop)
{
	private_ = std::make_shared<UdpNetworkPrivate>(loop);
}

UdpNetwork::~UdpNetwork() {
	
}

void UdpNetwork::setAddr(std::string addr) {
	private_->setAddr(addr);
}

std::string UdpNetwork::getAddr() {
	return private_->getAddr();
}

void UdpNetwork::setPort(int port) {
	private_->setPort(port);
}

int UdpNetwork::getPort() {
	return private_->getPort();
}

void UdpNetwork::setIPV(IPV ipv) {
	private_->setIPV((ipv == IPV::ipv4) ? SocketAddr::Ipv4 : SocketAddr::Ipv6);
}

IPV UdpNetwork::getIPV() {
	return (private_->getIPV() == SocketAddr::Ipv4) ? IPV::ipv4 : IPV::ipv6;
}

int UdpNetwork::start() {
	return private_->start();
}

void UdpNetwork::close(ActionCallback cb) {
	private_->close(cb);
}

int UdpNetwork::write(const std::string & ip, int port, const char * buf, unsigned int size, OnWritedCallback cb) {
	return private_->write(ip, port, buf, size, cb);
}

void UdpNetwork::writeInLoop(const std::string& ip, int port, const char * buf, unsigned int size, OnWritedCallback cb) {
	return private_->writeInLoop(ip, port, buf, size, cb);
}

void UdpNetwork::setOnDataCallback(UdpOnDataCallback cb) {
	private_->setOnDataCallback(cb);
}

int UdpNetwork::getLastError() {
	return private_->getLastError();
}

std::string UdpNetwork::getLastErrorMsg() {
	return private_->getLastErrorMsg();
}