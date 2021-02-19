#ifndef SOCKET_ADDR_H
#define SOCKET_ADDR_H

#include <string>
#include <memory>
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include "netinet/in.h"
#endif

class SocketAddr
{
public:
    enum IPV {
        Ipv4 = 0,
        Ipv6
    };

    SocketAddr(const std::string&& ip, unsigned short port, IPV ipv = Ipv4);
    SocketAddr(const std::string& ip, unsigned short port, IPV ipv = Ipv4);
    SocketAddr(const sockaddr* addr, IPV ipv = Ipv4);
	SocketAddr(const SocketAddr& addr);
	SocketAddr();

    const sockaddr* addr() const;
    std::string toString();

    IPV ipv() const;

public:
    static  void socketAddrToString(void* client, std::string& addrStr, IPV ipv = Ipv4);
    static  uint16_t  getIpAndPort(const sockaddr_storage* addr, std::string& out, IPV ipv = Ipv4);

private:
    std::string ip_;
    unsigned short port_;
    IPV ipv_;
    sockaddr_in ipv4_;
    sockaddr_in6 ipv6_;
};

#endif
