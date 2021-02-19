#ifndef TCP_SERVER_PRIVATE__H
#define TCP_SERVER_PRIVATE__H
#include "tcpAccepter.h"
#include <map>
#include <list>
#include <iostream>
#ifndef _Win32
#include <string.h>
#endif

#include "timerWheel.h"

class ConnectionWrapper;
class TcpServer;

class TcpServerPrivate_{
public:
	TcpServerPrivate_(Loop* loop);
	~TcpServerPrivate_();
public:
	Loop* getLoop();

	void setAddr(std::string addr);
	std::string getAddr();
	void setPort(int port);
	int getPort();
	void setIPV(SocketAddr::IPV ipv);
	SocketAddr::IPV getIPV();
	

	int start();
	void close(ActionCallback callback);
	void setTimeout(int secs);
	int write(std::string& id, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void writeInLoop(std::string& id, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);

	int getClientCount();

	void setOnConnectionCallback(TcpOnConnectionCallback cb);
	void setOnDisconnectCallback(TcpOnDisconnectCallback cb);
	void setOnDataCallback(TcpOnDataCallback cb);
private:
	int write(TcpConnectionPtr connection, const char* buf, unsigned int size, OnWritedCallback cb = nullptr);
	void writeInLoop(TcpConnectionPtr connection, const char* buf, unsigned int size, OnWritedCallback cb);
private:
	std::shared_ptr<TcpConnection> getConnnection(const std::string& id);
	void doOnConnection(TcpConnectionPtr conn);
	void doOnData(TcpConnectionPtr conn, const char *buf, long size);
	void doOnDisconnect(std::string& id);
private:
	TcpOnConnectionCallback on_connection_cb_ = nullptr;
	TcpOnDisconnectCallback on_disconnection_cb_ = nullptr;
	TcpOnDataCallback on_data_cb_ = nullptr;
private:
	std::map<std::string, TcpConnectionPtr>& sockets();
	//void sockets(std::list<TcpConnectionPtr>& clients);
private:
	std::string addr_;
	int port_;
	SocketAddr::IPV ipv_ = SocketAddr::Ipv4;
	std::shared_ptr<TcpAccepter> accepter_ = nullptr;
	//Timer* timer_ = nullptr;
	Loop* loop_ = nullptr;
	
	std::atomic<uint64_t> cnt_;
	TimerWheel<ConnectionWrapper> timerWheel_;
	std::map<std::string, TcpConnectionPtr> sockets_;
	friend TcpServer;
};

#endif