#pragma once

#include "Common.h"

#if EASY_LINUX

#include "SocketBase.h"

class SocketLinux : public SocketBase {
public:
	SocketLinux(string &ip, int port);
	virtual ~SocketLinux();

	virtual bool create(int protocol);
	virtual void close();
	virtual bool send(const void *buf, size_t len);
	virtual bool recv(void *buf, size_t len);

	virtual bool bind();
	virtual bool listen();
	virtual bool accept();

	virtual bool connect();

protected:
	virtual sockaddr* getSockAddr();

	int sock;
};

#endif