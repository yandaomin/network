#ifndef TCPCONNECTION_WRAPPER_H
#define TCPCONNECTION_WRAPPER_H
#include <functional>
#include <memory>
#include "common.h"
class ConnectionWrapper : public std::enable_shared_from_this<ConnectionWrapper>
{
public:
	ConnectionWrapper(TcpConnectionPtr connection);
	~ConnectionWrapper();
private:
	std::weak_ptr<TcpConnection> connection_;
};
#endif