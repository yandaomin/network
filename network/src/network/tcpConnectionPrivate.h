#pragma once
#include <memory>
//#include <chrono>
#include <functional>
#include "loop.h"
#include "socketAddr.h"
#include "ref.h"
/*#include "tcpConnection.h"*/
class TcpConnection;

class TcpConnectionPrivate : public std::enable_shared_from_this<TcpConnectionPrivate>
{
public:
	TcpConnectionPrivate(Loop* loop, std::string& id, bool isConnected = true);
	virtual ~TcpConnectionPrivate();

	//void close(std::function<void(std::string&)> callback);
	void close(TcpOnClosedCallback cb);

	int write(const char* buf, ssize_t size, OnWritedCallback callback = nullptr);
	void writeInLoop(const char* buf, ssize_t size, OnWritedCallback callback);

// 	void setWrapper(std::shared_ptr<ConnectionWrapper> wrapper);
// 	std::shared_ptr<ConnectionWrapper> getWrapper();

	void setOnDataCallback(TcpOnDataCallback cb);
	void setOnClosingCallback(TcpOnClosingCallback cb);
	void setOnClosedCallback(TcpOnClosedCallback cb);

	bool isConnected();

	const std::string& id();

// 	void setSink(ITcpConnectionSink* sink);
// 	ITcpConnectionSink* getSink();
	SocketCloseType getCloseType();

	int getLastError();
	std::string getLastErrorMsg();
	/////////////////
	void setHandle(UVTcpPtr socket);
	void start();
public:
	void setConnection(std::shared_ptr<TcpConnection> conn);
	std::shared_ptr<TcpConnection> getConnection();
public:
	void onClosing();
private:
	void setIsConnected(bool connected);

private:
	void onData(const char* buf, ssize_t size);
	
	void onClosed();
	char* resizeReadBuffer(size_t size);
	static void onDataReceive(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf);
private:
	std::string id_;
	std::atomic<bool> isConnected_;
	Loop* loop_;
	UVTcpPtr handle_;
	std::string readBuffer_;
	//std::weak_ptr<ConnectionWrapper> wrapper_;

	TcpOnDataCallback on_data_cb_ = nullptr;
	TcpOnClosingCallback on_closing_cb = nullptr;
	TcpOnClosedCallback on_closed_cb_ = nullptr;

	//ITcpConnectionSink* sink_ = nullptr;
	int lastError_ = 0;
	std::string lastErrorMsg_ = "";
	SocketCloseType socketCloseType_ = SocketCloseType::Unknow;
	//////
	std::weak_ptr<TcpConnection> connection_;

	//TcpConnectionPrivate* private_ = nullptr;
};
