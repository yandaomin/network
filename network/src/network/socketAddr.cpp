#include "socketAddr.h"
#include <iostream>
#include "ref.h"


SocketAddr::SocketAddr(const std::string&& ip, unsigned short port, IPV ipv)
    :ip_(ip),
    port_(port),
    ipv_(ipv)
{
    if (Ipv6 == ipv)
    {
        ::uv_ip6_addr(ip.c_str(), port, &ipv6_);
    }
    else
    {
        ::uv_ip4_addr(ip.c_str(), port, &ipv4_);
    }
}

SocketAddr::SocketAddr(const std::string& ip, unsigned short port, IPV ipv)
    :SocketAddr(std::move(ip), port, ipv)
{

}

SocketAddr::SocketAddr(const sockaddr* addr, IPV ipv)
    :ipv_(ipv)
{
    if (ipv_ == Ipv4)    {
        ipv4_ = *(reinterpret_cast<const sockaddr_in*>(addr));
    } else {
        ipv6_ = *(reinterpret_cast<const sockaddr_in6*>(addr));
    }
    port_ = getIpAndPort((const sockaddr_storage *)(addr), ip_, ipv);
}

SocketAddr::SocketAddr(const SocketAddr & addr) {
	ipv_ = addr.ipv();
	if (ipv_ == Ipv4) {
		ipv4_ = *(reinterpret_cast<const sockaddr_in*>(addr.addr()));
	} else {
		ipv6_ = *(reinterpret_cast<const sockaddr_in6*>(addr.addr()));
	}
	port_ = getIpAndPort((const sockaddr_storage *)(addr.addr()), ip_, ipv_);
}

SocketAddr::SocketAddr()
{
}

const sockaddr* SocketAddr::addr() const
{
    return (Ipv6 == ipv_) ? reinterpret_cast<const sockaddr*>(&ipv6_) : reinterpret_cast<const sockaddr*>(&ipv4_);
}

std::string SocketAddr::toString()
{
    std::string str = ip_ + ":" + std::to_string(port_);
    return str;
}

SocketAddr::IPV SocketAddr::ipv() const
{
    return ipv_;
}

void SocketAddr::socketAddrToString(void* t, std::string& addrStr, IPV ipv)
{
	uv_tcp_t* socket = (uv_tcp_t*)t;
    struct sockaddr_storage addr;
    int len = sizeof(struct sockaddr_storage);
    ::uv_tcp_getpeername(socket, (struct sockaddr *)&addr, &len);
    uint16_t port = getIpAndPort(&addr, addrStr, ipv);
    addrStr += ":" + std::to_string(port);
}

uint16_t SocketAddr::getIpAndPort(const sockaddr_storage* addr, std::string& out, IPV ipv) {
    auto inet = (Ipv6 == ipv) ? AF_INET6 : AF_INET;
    if (Ipv6 == ipv) {
        char ip[64];
        struct sockaddr_in6* addr6 = (struct sockaddr_in6 *)addr;
        //低版本windows可能找不到inet_ntop函数。
#if    _MSC_VER
        DWORD size = sizeof(ip);
        WSAAddressToString((LPSOCKADDR)addr6, sizeof(sockaddr_in6), NULL, ip, &size);
        out = std::string(ip);
        auto index = out.rfind(":");
        if (index >= 0) {
            out.resize(index);
        }
        return (htons(addr6->sin6_port));
#else
        std::string str(inet_ntop(inet, (void *)&(addr6->sin6_addr), ip, 64));
        out.swap(str);
        return(htons(addr6->sin6_port));
#endif
    } else {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        std::string str(inet_ntoa(addr4->sin_addr));
        out.swap(str);
        return htons(addr4->sin_port);
    }
}


