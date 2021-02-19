#ifndef TCP_CLIENT_PRIVATE__H
#define TCP_CLIENT_PRIVATE__H
#include "tcpConnecter.h"
#include <vector>
#include "common.h"

class TcpClientPrivate_{
public:
	TcpClientPrivate_(Loop* loop);
	~TcpClientPrivate_();
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
	void onConnection(TcpConnectionPtr conn);
	void onData(TcpConnectionPtr conn, const char *buf, int size);
	void onConnect(bool isConnected, std::string& errMsg);
	void onDisConnect(std::string& id);
private:
	std::string addr_;
	int port_;
	std::shared_ptr<TcpConnecter> connecter_ = nullptr;
	TcpConnectionPtr connection_ = nullptr;
	Loop* loop_ = nullptr;
	//////////////////////////////////////////////////////////////////////////
	TcpOnDataCallback on_data_cb_ = nullptr;
	TcpOnDisconnectCallback on_disconnect_cb_ = nullptr;
	TcpOnConnectCallback on_connect_cb_ = nullptr;
};
#endif