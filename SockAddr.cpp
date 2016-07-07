#include "SockAddr.h"

EASY_NS_BEGIN

SockAddr::SockAddr(int protocol)
	: protocol(protocol)
{
	if (protocol == 4) {
		sockaddr_in *p = new sockaddr_in();
		ptr.addr_in = p;

		memset(p, 0, sizeof(sockaddr_in));
		p->sin_family = AF_INET;
	} else {
		sockaddr_in6 *p = new sockaddr_in6();
		ptr.addr_in6 = p;

		memset(p, 0, sizeof(sockaddr_in6));
		p->sin6_family = AF_INET6;
	}
}

SockAddr::~SockAddr()
{
	if (ptr.addr) {
		delete ptr.addr;
		ptr.addr = 0;
	}
}

int SockAddr::length()
{
	return protocol == 4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
}

const char* SockAddr::getIp() const
{
	int len = protocol == 4 ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
	char* p = new char[len];
	const char *ret = 0;

	if (protocol == 4) {
		ret = inet_ntop(AF_INET, &ptr.addr_in->sin_addr, p, len);
	} else {
		ret = inet_ntop(AF_INET6, &ptr.addr_in6->sin6_addr, p, len);
	}

	if (ret == 0) {
		EASY_LOG("SockAddr::getIp.inet_ntop error: %s", strerror(errno));
	}

	return ret;
}

int SockAddr::getPort() const
{
	return ntohs(protocol == 4 ? ptr.addr_in->sin_port : ptr.addr_in6->sin6_port);
}

void SockAddr::setIp(const char* ip)
{
	int ret = 1;
	if (protocol == 4) {
		if (ip) {
			ret = inet_pton(AF_INET, ip, &ptr.addr_in->sin_addr);
		} else {
			ptr.addr_in->sin_addr.s_addr = INADDR_ANY;
		}
	} else {
		if (ip) {
			ret = inet_pton(AF_INET6, ip, &ptr.addr_in6->sin6_addr);
		} else {
			ptr.addr_in6->sin6_addr = in6addr_any;
		}
	}
	
	if (ret == 0) {
		EASY_LOG("SockAddr::setIp.inet_pton error: %s", strerror(errno));
	}
}

void SockAddr::setPort(int port)
{
	if (protocol == 4) {
		ptr.addr_in->sin_port = htons(port);
	} else {
		ptr.addr_in6->sin6_port = htons(port);
	}
}

EASY_NS_END