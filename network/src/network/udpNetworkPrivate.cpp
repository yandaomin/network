#include "udpNetworkPrivate.h"
#include "logWriter.h"

struct UdpWriteReq {
	uv_udp_send_t req;
	uv_buf_t buf;
	std::string ip;
	unsigned int port;
	OnWritedCallback callback;
};

UdpNetworkPrivate::UdpNetworkPrivate(Loop* loop)
	: socket_(new uv_udp_t())
{
	loop_ = loop;
	::uv_udp_init((uv_loop_t*)(loop->handle()), socket_);
	socket_->data = this;
}

UdpNetworkPrivate::~UdpNetworkPrivate() {
	loop_ = nullptr;
	if (socket_) {
		delete socket_;
		socket_ = nullptr;
	}
}

void UdpNetworkPrivate::setAddr(std::string addr) {
	addr_ = addr;
}

std::string UdpNetworkPrivate::getAddr() {
	return addr_;
}

void UdpNetworkPrivate::setPort(int port) {
	port_ = port;
}

int UdpNetworkPrivate::getPort() {
	return port_;
}

void UdpNetworkPrivate::setIPV(SocketAddr::IPV ipv) {
	ipv_ = ipv;
}

SocketAddr::IPV UdpNetworkPrivate::getIPV() {
	return ipv_;
}

int UdpNetworkPrivate::start() {
	SocketAddr addr(addr_, port_, ipv_);
	int lastError_ = uv_udp_bind(socket_, addr.addr(), 0);
	if (0 != lastError_) {
		lastErrorMsg_ = uv_err_name(lastError_);
		return lastError_;
	}
	lastError_ = uv_udp_recv_start(socket_,
		[](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
	{
		auto conn = static_cast<UdpNetworkPrivate*>(handle->data);
		buf->base = conn->resizeReadBuffer(suggested_size);
#if _MSC_VER
		buf->len = (ULONG)suggested_size;
#else
		buf->len = suggested_size;
#endif
	},
		&UdpNetworkPrivate::onDataReceive);
}

void UdpNetworkPrivate::close(ActionCallback cb) {
	on_closed_cb_ = cb;
	if (uv_is_active((uv_handle_t*)socket_)) {
		uv_udp_recv_stop(socket_);
	}
	if (uv_is_closing((uv_handle_t*)socket_) == 0) {
		::uv_close((uv_handle_t*)socket_, [](uv_handle_t* handle)
		{
			UdpNetworkPrivate* ptr = static_cast<UdpNetworkPrivate*>(handle->data);
			ptr->doOnClose();
		});
	} else {
		doOnClose();
	}
}

int UdpNetworkPrivate::write(const std::string & ip, int port, const char * buf, unsigned int size, OnWritedCallback cb) {
	UdpWriteReq* req = new UdpWriteReq();
	req->buf = uv_buf_init(const_cast<char*>(buf), size);
	req->ip = ip;
	req->port = port;
	req->callback = cb;

	SocketAddr addr(ip, port, ipv_);
	int rlt = 0;
	rlt = ::uv_udp_send(&req->req, socket_, &req->buf, 1, addr.addr(),
		[](uv_udp_send_t* req_t, int status)
	{
		UdpWriteReq* wr = (UdpWriteReq*)req_t;
		if (nullptr != wr->callback) {
			struct WriteResult info;
			info.buf = const_cast<char*>(wr->buf.base);
			info.size = wr->buf.len;
			info.status = status;
			if (status) {
				info.errMsg = uv_err_name(status);
			} else {
				info.errMsg = "success";
			}
			wr->callback(info);
		}
		
		delete wr;
	});

	if (0 != rlt)
	{
		if (nullptr != cb)
		{
			struct WriteResult info;
			info.status = rlt;
			info.id = "";
			info.errMsg = "";
			info.buf = const_cast<char*>(buf);
			info.size = static_cast<unsigned long>(size);
			cb(info);
		}
		delete req;
	}
	return rlt;
}

void UdpNetworkPrivate::writeInLoop(const std::string& ip, int port, const char * buf, unsigned int size, OnWritedCallback cb) {
	std::weak_ptr<UdpNetworkPrivate> conn = shared_from_this();
	loop_->runInLoop(
		[conn, ip, port, buf, size, cb]()
	{
		std::shared_ptr<UdpNetworkPrivate> ptr = conn.lock();
		if (ptr != nullptr) {
			ptr->write(ip, port, buf, size, cb);
		} else {
			struct WriteResult info;
			info.status = WriteResult::result_disconnected;
			info.buf = const_cast<char*>(buf);
			info.size = static_cast<unsigned long>(size);
			cb(info);
		}
	});
}

void UdpNetworkPrivate::setOnDataCallback(UdpOnDataCallback cb) {
	on_data_cb_ = cb;
}

int UdpNetworkPrivate::getLastError() {
	return lastError_;
}

std::string UdpNetworkPrivate::getLastErrorMsg() {
	return lastErrorMsg_;
}

void UdpNetworkPrivate::onDataReceive(uv_udp_t * socket, ssize_t size, 
	const uv_buf_t * buf, const sockaddr * addr, unsigned flags) {
	if (size < 0) {
		UdpNetworkPrivate* udp = static_cast<UdpNetworkPrivate*>(socket->data);
		if (!udp) return;
		udp->lastError_ = size;
		udp->lastErrorMsg_ = "udp socket read error :";
		udp->lastErrorMsg_.append( Loop::getErrorMessage((int)size));
		LogWriter::instance().error(udp->lastErrorMsg_);
	} else if (size > 0) {
		UdpNetworkPrivate* udp = static_cast<UdpNetworkPrivate*>(socket->data);
		if (!udp) return;
		std::string addr_s;
		int port = 0;
		udp->sockAddrConvert((sockaddr_storage *)addr, sizeof(sockaddr), addr_s, port);
		udp->doOnData(addr_s, port, buf->base, size);
	}
}

char * UdpNetworkPrivate::resizeReadBuffer(size_t size) {
	readBuffer_.resize(size);
	return const_cast<char*>(readBuffer_.c_str());
}

int UdpNetworkPrivate::sockAddrConvert(sockaddr_storage * addr, socklen_t len, std::string & str, int& port) {
	char ipstr[INET6_ADDRSTRLEN];

	if (addr->ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)addr;
		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
	}
	else { // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = (int)ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
	}

	str.assign(ipstr);
	return 0;
}

void UdpNetworkPrivate::doOnData(const std::string & addr, int port, const char * buf, long size) {
	if (on_data_cb_)
		on_data_cb_(addr, port, buf, size);
}

void UdpNetworkPrivate::doOnClose() {
	if (on_closed_cb_)
		on_closed_cb_();
}

