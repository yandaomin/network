#pragma once
#include <memory>
#include <functional>
#include "loop.h"
#include "socketAddr.h"

class TcpAccepter;
class TcpConnecter;
class TcpConnectionPrivate;
class ConnectionWrapper;
class TcpServerPrivate_;
class TcpClientPrivate_;

class NETWORK_EXPORT TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(Loop* loop, std::string& id, bool isConnected = true);
	virtual ~TcpConnection();

	void close(TcpOnClosedCallback cb);

	int write(const char* buf, long size, OnWritedCallback callback = nullptr);
	void writeInLoop(const char* buf, long size, OnWritedCallback callback);

	void setOnDataCallback(TcpOnDataCallback cb);
	bool isConnected();
	const std::string& id();
	SocketCloseType getCloseType();

	int getLastError();
	std::string getLastErrorMsg();
public:
	void onClosing();
private:
	std::shared_ptr<TcpConnectionPrivate> getPrivate();
	void setWrapper(std::shared_ptr<ConnectionWrapper> wrapper);
	std::shared_ptr<ConnectionWrapper> getWrapper();
	void setOnClosingCallback(TcpOnClosingCallback cb);
	void setOnClosedCallback(TcpOnClosedCallback cb);
private:
	std::weak_ptr<ConnectionWrapper> wrapper_;
	std::shared_ptr<TcpConnectionPrivate> private_ = nullptr;
	friend TcpConnectionPrivate;
	friend TcpAccepter;
	friend TcpConnecter;
	friend TcpServerPrivate_;
	friend TcpClientPrivate_;
};


using ConnectionWrapperPtr = std::shared_ptr<ConnectionWrapper>;
