#ifndef TCP_SERVER__H
#define TCP_SERVER__H
#include "loop.h"
#include <map>
#include <list>

class TcpServerPrivate_;

class NETWORK_EXPORT TcpServer{
public:
	TcpServer(Loop* loop);
	~TcpServer();
public:
	void setAddr(std::string addr);
	std::string getAddr();
	void setPort(int port);
	int getPort();
	//ipv4:0, ipv6:1
	void setIPV(IPV ipv);
	IPV getIPV();

	int start();
	void close(ActionCallback callback);
	void setTimeout(int secs);
	int write(std::string& id, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void writeInLoop(std::string& id, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);

	int getClientCount();

	void setOnConnectionCallback(TcpOnConnectionCallback cb);
	void setOnDisconnectCallback(TcpOnDisconnectCallback cb);
	void setOnDataCallback(TcpOnDataCallback cb);
	Loop* getLoop();
	//void getSockets(std::list<TcpConnectionPtr>& clients);
	std::map<std::string, TcpConnectionPtr>& sockets();
private:
	std::atomic<uint64_t> cnt_;
	TcpServerPrivate_* private_;
};
#endif