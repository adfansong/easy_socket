#pragma once

#include "Common.h"

EASY_NS_BEGIN

class SockAddr {
public:
	SockAddr(int protocol);
	~SockAddr();

	int length();
	sockaddr* pointer() { return ptr.addr; }

	// Note: delete the returned ptr when you don't use it any more.
	const char* getIp() const;
	int getPort() const;

	// null means any
	void setIp(const char* ip);	
	void setPort(int port);

protected:
	// 4, 6
	int protocol;

	union {
		sockaddr *addr;
		sockaddr_in *addr_in;
		sockaddr_in6 *addr_in6;
	} ptr;
};

EASY_NS_END