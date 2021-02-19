#pragma once
#include <memory>
#include <functional>
#include "loop.h"
#include "socketAddr.h"
#include "ref.h"

class TcpConnecter;
using TcpClientPtr = std::shared_ptr<TcpConnecter>;

class TcpConnecter
{
public:
	using OnConnectionCallback = std::function<void(TcpConnectionPtr)>;
	using ConnectCallback = std::function<void(bool, std::string&)>;
public:
	TcpConnecter(Loop* loop, bool tcpNoDelay = true);
	virtual ~TcpConnecter();

	bool isTcpNoDelay();
	void setTcpNoDelay(bool isNoDelay);
	void connect(SocketAddr& addr);

	void setOnConnectionCallback(OnConnectionCallback cb);
	void setConnectCallback(ConnectCallback cb);
	Loop* loop();
// 	void setSink(ITcpConnectorSink* sink);
// 	ITcpConnectorSink* getSink();
protected:
	Loop* loop_;

	void onConnect(int status);
// 	void onConnectClose(std::string& name);
// 	void onMessage(TcpConnectionPtr connection, const char* buf, ssize_t size);
// 	void afterConnectFail();
private:
	UVTcpPtr socket_;
	uv_connect_t* connector_;
	//SocketAddr::IPV ipv;
	SocketAddr addr_;
	bool tcpNoDelay_;

	//ConnectStatusCallback connectCallback_;
	//NewMessageCallback onMessageCallback_;

	//TcpConnectionPtr connection_;
	//ITcpConnectorSink* sink_;
	OnConnectionCallback on_connection_cb_ = nullptr;
	ConnectCallback connect_cb_ = nullptr;
};