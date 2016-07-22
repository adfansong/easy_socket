#include "SocketBase.h"

EASY_NS_BEGIN

SocketBase::SocketBase()
	: protocol(4), addr(0), del(0), errorCode(0), 
	set_unblock(true), set_reuseAddr(true), socket_type(SOCK_STREAM)
{

}

SocketBase::~SocketBase()
{
	if (addr) {
		addr->release();
		addr = 0;
	}
}

void SocketBase::close(bool hasError /*= false*/)
{
	if (del) {
		del->onClose(hasError);
	}	
}

bool SocketBase::addMembership(const char *addr, const char *interface_ /*= 0*/)
{
	int sock = getSockFd();
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketBase::addMembership error: invalid socket");
		return false;
	}

	int ret = -1;
	if (protocol == 4) {
		ip_mreq addrGroup;
		memset(&addrGroup, 0, sizeof(struct ip_mreq));
		addrGroup.imr_multiaddr.s_addr = inet_addr(addr);
		addrGroup.imr_interface.s_addr = interface_ ? inet_addr(interface_) : htonl(INADDR_ANY);

		ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(char*)&addrGroup, sizeof(ip_mreq));
	} else {		
#ifndef EASY_IOS
		// TODO: ios IPV6_ADD_MEMBERSHIP is undefined.
		ipv6_mreq addrGroup;
		memset(&addrGroup, 0, sizeof(struct ip_mreq));
		inet_pton(AF_INET6, addr, &addrGroup.ipv6mr_multiaddr);
		if (ret == -1){
			EASY_LOG("SocketBase::addMembership.inet_pton error: %s", strerror(errno));
			return false;
		}
		addrGroup.ipv6mr_interface = interface_ ? 0 : atoi(interface_);
		ret = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
			(char*)&addrGroup, sizeof(ipv6_mreq));
#endif
	}

	if (ret == -1){
		EASY_LOG("SocketBase::addMembership.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketBase::dropMembership(const char *addr, const char *interface_ /*= 0*/)
{
	int sock = getSockFd();
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketBase::addMembership error: invalid socket");
		return false;
	}

	int ret = -1;
	if (protocol == 4) {
		ip_mreq addrGroup;
		addrGroup.imr_multiaddr.s_addr = inet_addr(addr);
		addrGroup.imr_interface.s_addr = interface_ ? inet_addr(interface_) : htonl(INADDR_ANY);

		ret = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
			(char*)&addrGroup, sizeof(ip_mreq));
	} else {
#ifndef EASY_IOS
		ipv6_mreq addrGroup;
		inet_pton(AF_INET6, addr, &addrGroup.ipv6mr_multiaddr);
		if (ret == -1){
			EASY_LOG("SocketBase::addMembership.inet_pton error: %s", strerror(errno));
			return false;
		}
		addrGroup.ipv6mr_interface = interface_ ? 0 : atoi(interface_);
		ret = setsockopt(sock, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP,
			(char*)&addrGroup, sizeof(ipv6_mreq));
#endif
	}

	if (ret == -1){
		EASY_LOG("SocketBase::dropMembership.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

int SocketBase::select(int ms)
{	
	timeval tv;
	timeval *pt = &tv;
	if (ms < 0) {
		pt = 0;
	} else {
		pt->tv_sec = ms / 1000;
		pt->tv_usec = ms % 1000;
	}

	fd_set rfd, wfd;
	FD_ZERO(&rfd);
	FD_ZERO(&wfd);
	
	int fd = getSockFd();
	FD_SET(fd, &rfd);
	FD_SET(fd, &wfd);

	int ret = 0;
	if (::select(fd + 1, &rfd, &wfd, 0, pt) > 0) {
		if (FD_ISSET(fd, &rfd)) {
			ret |= 1;
		}
		if (FD_ISSET(fd, &wfd)) {
			ret |= 2;
		}
	}

	return ret;
}

bool SocketBase::canRead()
{
	return select() & 1;
}

bool SocketBase::canWrite()
{
	return select() & 2;
}

int SocketBase::getError()
{
	int cur = errorCode;
	errorCode = 0;
	return cur;
}

int SocketBase::getInternalError()
{
	int cur = internalError;
	internalError = 0;
	return cur;
}

SockAddr* SocketBase::getSockAddr(int port, const char *ip)
{
	if (!addr) {
		addr = new SockAddr(protocol);
	}

	if (port != 0 || ip != 0) {
		addr->setPort(port);
		addr->setIp(ip);
	}
	
	return addr;
}

void SocketBase::setSockAddr(SockAddr *p)
{
	if (addr) {
		addr->release();
	}
	addr = p;
	addr->retain();
}

void SocketBase::setDelegate(ISocketBaseEvent *p)
{
	del = p;
}

void SocketBase::setError(int e, int ie /*= 0*/)
{
	errorCode = e != 0 ? e : 0;
	internalError = ie != 0 ? ie : 0;
}

void SocketBase::emitError()
{
	if (del) {
		del->onError(errorCode, internalError);
	}
	
	close(true);
}

bool SocketBase::noDelay(bool no)
{
	int d = no ? 1 : 0;
	int ret = setsockopt(getSockFd(), IPPROTO_TCP, TCP_NODELAY, (const char *)&d, sizeof(d));
	if (ret == -1) {
		EASY_LOG("SocketBase::noDelay.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketBase::reuseAddr(bool use)
{
	int d = use ? 1 : 0;
	int ret = setsockopt(getSockFd(), SOL_SOCKET, SO_REUSEADDR, (const char *)&d, sizeof(d));
	if (ret == -1) {
		EASY_LOG("SocketBase::reuseAddr.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketBase::setSendBufferSize(int size)
{
	int ret = setsockopt(getSockFd(), SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(size));
	if (ret == -1) {
		EASY_LOG("SocketBase::setSendBufferSize.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}


EASY_NS_END