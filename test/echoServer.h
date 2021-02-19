#ifndef ECHO_SERVER
#define ECHO_SERVER
#include "export_network.h"
#include "tcpServer.h"
class EchoServer : public TcpServer {
	EchoServer(Loop* loop);
	~EchoServer();
};
#endif