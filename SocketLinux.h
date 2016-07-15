#pragma once

#include "Common.h"

#if EASY_LINUX

#include "SocketBase.h"

EASY_NS_BEGIN

class SocketLinux : public SocketBase {
public:
	SocketLinux();
	virtual ~SocketLinux();

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
	virtual int recvfrom(char *buf, size_t len, SockAddr **pp);

	virtual bool checkConnected();

	virtual const char* formatError(int error);
protected:
	
	virtual bool unblock();
	virtual int getSockFd() { return sock; }

	// ios need this
	bool noSigPipe(bool no);

	int sock;
};

EASY_NS_END

#endif