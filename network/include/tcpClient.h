#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <vector>
#include "common.h"
#include "loop.h"

class TcpClientPrivate_;

class NETWORK_EXPORT TcpClient{
public:
	TcpClient(Loop* loop);
	~TcpClient();
public:
	Loop* getLoop();
	TcpConnectionPtr getConnection();
	void connect(const std::string& ip, int port);
	void setTcpOnDataCallback(TcpOnDataCallback cb);
	void setTcpOnDisconnectCallback(TcpOnDisconnectCallback cb);
	void setTcpOnConnectCallback(TcpOnConnectCallback cb);

	int write(const char* buf, long size, OnWritedCallback cb = nullptr);
	void writeInLoop(const char* buf, long size, OnWritedCallback cb);
private:
	TcpClientPrivate_* private_;
};
#endif