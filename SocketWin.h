#pragma once

#include "Common.h"

#if EASY_WIN

#include "SocketBase.h"

EASY_NS_BEGIN

class SocketWin : public SocketBase {
public:
	SocketWin();
	virtual ~SocketWin();

	virtual bool create(int protocol);
	virtual void close(bool hasError = false);
	virtual bool send(const char *buf, size_t len);
	virtual int recv(char *buf, size_t len);
	virtual void shutdown();

	virtual bool bind(int port, const char *ip);
	virtual bool bind(addrinfo *addrInfo);
	virtual bool listen();
	virtual bool accept(SockAddr *p = 0);

	virtual bool connect(int port, const char *ip);
	virtual bool connect(addrinfo *addrInfo);
	
	// udp
	virtual int sendto(const char *buf, size_t len, int port, const char *ip);
	virtual int sendto(const char *buf, size_t len, addrinfo *addrInfo);
	virtual int recvfrom(char *buf, size_t len, SockAddr **pp = 0);

	virtual const char* formatError(int error);
protected:
	
	virtual bool unblock();
	virtual int getSockFd() { return sock; }

	static int __inited;

	SOCKET sock;	
};

EASY_NS_END

#endif