#pragma once

#include "Common.h"
#include "SockAddr.h"
#include "ISocketBaseEvent.h"

EASY_NS_BEGIN


class SocketBase {
public:	

	SocketBase();
	virtual ~SocketBase();

	virtual bool create(int protocol) = 0;
	virtual void close(bool hasError = false);
	virtual bool send(const void *buf, size_t len) = 0;
	// -2 again, -1 error, 0 close, > 0 ok
	virtual int recv(void *buf, size_t len) = 0;
	
	virtual bool bind(int port, const char *ip) = 0;
	virtual bool listen() = 0;
	virtual bool accept(SockAddr *p = 0) = 0;

	virtual bool connect(int port, const char *ip) = 0;
	
	// ret: 0 no, 1 read, 2 write
	int select(int ms = 0);
	bool canRead();
	bool canWrite();

	virtual bool checkConnected() { return true; }	

	SockAddr* getSockAddr(int port, const char *ip);
	int getProtocol() { return protocol; }
	int getError();
	int getInternalError();
	bool isUnblock() { return set_unblock; }
	bool isReuseAddr() { return set_reuseAddr; }

	void setSockAddr(SockAddr *p);
	void setDelegate(ISocketBaseEvent *p);
	void setUnblock(bool un) { set_unblock = un; }
	void setReuseAddr(bool re) { set_reuseAddr = re; }	

	void emitError();
protected:
	
	virtual bool unblock() = 0;
	virtual bool noDelay(bool no) = 0;
	virtual bool reuseAddr(bool use) = 0;
	virtual int getSockFd() { return -1; }
	virtual bool setSendBufferSize(int size) = 0;

	void setError(int e, int ie = 0);

	// 4, 6
	int protocol;
	SockAddr *addr;

	ISocketBaseEvent *del;
	int errorCode;
	int internalError;

	bool set_unblock;
	bool set_reuseAddr;
};

EASY_NS_END