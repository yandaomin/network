#ifndef UDPNETWORK_H
#define UDPNETWORK_H
#include <memory>
#include "loop.h"
#include "common.h"
#include "export_network.h"

class UdpNetworkPrivate;
class NETWORK_EXPORT UdpNetwork: public std::enable_shared_from_this<UdpNetwork> {
public:
	UdpNetwork(Loop* loop);
	~UdpNetwork();
public:
	void setAddr(std::string addr);
	std::string getAddr();
	void setPort(int port);
	int getPort();
	void setIPV(IPV ipv);
	IPV getIPV();

	int start();
	void close(ActionCallback cb = nullptr);
	int write(const std::string& ip, int port, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void writeInLoop(const std::string& ip, int port, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);

	void setOnDataCallback(UdpOnDataCallback cb);
	int getLastError();
	std::string getLastErrorMsg();
private:
	std::shared_ptr<UdpNetworkPrivate> private_ = nullptr;
};

#endif