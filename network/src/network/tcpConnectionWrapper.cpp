#include "tcpConnectionWrapper.h"
#include "tcpConnection.h"

ConnectionWrapper::ConnectionWrapper(TcpConnectionPtr connection)
	:connection_(connection)
{
}

ConnectionWrapper::~ConnectionWrapper() {
	TcpConnectionPtr connection = connection_.lock();
	if (connection) {
		connection->onClosing();
	}
}
