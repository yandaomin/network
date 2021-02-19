#ifndef TCPACCEPTER_H
#define TCPACCEPTER_H
#include <memory>
#include <functional>
#include "loop.h"
#include "socketAddr.h"
#include "ref.h"

////////TcpAccepter////////

class TcpAccepter
{
public:
	using OnConnectionCallback = std::function<void(TcpConnectionPtr)>;
public:
	TcpAccepter(Loop* loop, bool tcpNoDelay = true);
	virtual ~TcpAccepter();

	int bind(SocketAddr& addr);
	int listen();
	bool isListen();
	void close(ActionCallback callback);
	bool isTcpNoDelay();
	void setOnConnectinonCallback(OnConnectionCallback callback);
	Loop* loop();
private:
	void onConnection(UVTcpPtr client);
	void onClose();
private:
	bool isListened_;
	bool tcpNoDelay_;
	Loop* loop_;
	OnConnectionCallback onConnectionCallback_;
	ActionCallback onCloseCallback_;
	uv_tcp_t server_;
	//ITcpAcceptorSink* sink_;
	SocketAddr addr_;
};

#endif