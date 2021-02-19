#ifndef COMMON_H
#define COMMON_H
#include <memory>
#include <functional>
#include "export_network.h"

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

struct WriteResult
{
	static const int result_disconnected = -1;
	//0: success, else fail
	int status = 0;
	std::string errMsg = "";
	char* buf;
	std::string id = "";
	unsigned long size;
};

class Timer;
class Async;

using ActionCallback = std::function<void()>;
using AsyncOnCloseCallBack = std::function<void(Async*)>;

enum class IPV {
	ipv4 = 0,
	ipv6
};
/************* tcp begin**************/
enum class SocketCloseType {
	Unknow = 0,
	Active,
	Passive
};
//主动断开和被动断开都会回调此函数（SocketCloseType参数用于区分断开的类型是主动还是被动）
using TcpOnDisconnectCallback = std::function<void(const std::string&, SocketCloseType, const std::string& errmsg)>;
using TcpOnConnectionCallback = std::function<void (const std::string&)>;
using TcpOnDataCallback = std::function<void(TcpConnectionPtr conn, const char *buf, long size)>;
//using OnDataCallback = std::function<void(const std::string&, const char*, long)>;
using OnWritedCallback = std::function<void(WriteResult&)>;
//tcp client
using TcpOnConnectCallback = std::function<void(bool isConnected, std::string& errMsg)>;
////////////////tcp内部使用//////////////////
using TcpOnClosedCallback = std::function<void(std::string&)>;
using TcpOnClosingCallback = std::function<void(std::string&)>;
/************* tcp end**************/
/************* udp begin************/
using UdpOnDataCallback = std::function<void(const std::string& ip, int port, const char *buf, long size)>;
/************* udp end************/
//timer
using OnTimerCallback = std::function<void(Timer*)>;
using TimerOnCloseCallback = std::function<void(Timer*)>;

#endif