#pragma once

#include "Common.h"

enum EventType {
	None,
	Listen,
	Accept,
	Connect,
};

typedef void EventFunc(EventType type, void *param);

struct ParamAcceptConnect {
	ParamAcceptConnect(int socket, sockaddr *addr) : socket(socket), addr(addr) {}

	int socket;
	sockaddr *addr;
};

class SocketBase {
public:

	SocketBase(string &ip, int port);
	virtual ~SocketBase() = 0;

	virtual bool create(int protocol) = 0;
	virtual void close() {}
	virtual bool send(const void *buf, size_t len) = 0;
	virtual bool recv(void *buf, size_t len) = 0;
	
	virtual bool bind() = 0;
	virtual bool listen() = 0;
	virtual bool accept() = 0;

	virtual bool connect() = 0;

	bool isClient() { return client; }
protected:
	virtual sockaddr* getSockAddr() = 0;
	virtual void emit(EventType type, void *param);

	string ip;
	int port;
	// 4, 6
	int protocol;
	sockaddr *addr;
	bool client;
};