#ifndef UDPNETWORK_PRIVATE_H
#define UDPNETWORK_PRIVATE_H
#include <memory>
#include "ref.h"
#include "loop.h"
#include "socketAddr.h"

class UdpNetworkPrivate: public std::enable_shared_from_this<UdpNetworkPrivate> {
public:
	UdpNetworkPrivate(Loop* loop);
	~UdpNetworkPrivate();
public:
	void setAddr(std::string addr);
	std::string getAddr();
	void setPort(int port);
	int getPort();
	void setIPV(SocketAddr::IPV ipv);
	SocketAddr::IPV getIPV();

	int start();
	void close(ActionCallback cb = nullptr);
	int write(const std::string& ip, int port, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void writeInLoop(const std::string& ip, int port, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void setOnDataCallback(UdpOnDataCallback cb);
	int getLastError();
	std::string getLastErrorMsg();
private:
	static void onDataReceive(uv_udp_t* socket, ssize_t size, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
	char* resizeReadBuffer(size_t size);
	int sockAddrConvert(sockaddr_storage *addr, socklen_t len, std::string &str, int& port);
	void doOnData(const std::string& addr, int port, const char* buf, long size);
	void doOnClose();
private:
	Loop* loop_;
	std::string addr_;
	int port_;
	UdpOnDataCallback on_data_cb_ = nullptr;
	ActionCallback on_closed_cb_ = nullptr;
	int lastError_ = 0;
	std::string lastErrorMsg_ = "";

	SocketAddr::IPV ipv_ = SocketAddr::Ipv4;
	uv_udp_t* socket_;
	std::string readBuffer_;
};
#endif