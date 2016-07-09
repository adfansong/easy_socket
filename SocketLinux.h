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
	virtual bool send(const void *buf, size_t len);
	virtual int recv(void *buf, size_t len);

	virtual bool bind(int port, const char *ip);
	virtual bool listen();
	virtual bool accept(SockAddr *p = 0);

	virtual bool connect(int port, const char *ip);

	virtual bool checkConnected();
protected:
	
	virtual bool unblock();
	virtual bool noDelay(bool no);
	virtual bool reuseAddr(bool use);
	virtual int getSockFd() { return sock; }
	virtual bool setSendBufferSize(int size);

	// ios need this
	bool noSigPipe(bool no);

	int sock;
};

EASY_NS_END

#endif