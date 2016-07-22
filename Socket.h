#pragma once

#include "Common.h"
#include "SocketState.h"
#include "ISocketBaseEvent.h"
#include "Buffer.h"
#include "SockAddr.h"

#include <vector>
#include <map>
#include <functional>

EASY_NS_BEGIN

class SocketBase;

class SocketError {
public:
	SocketError(int code, int inter = 0, const char *msg = "", SocketBase* s = 0);
	
	void formatError();	

	int code;
	int internalCode;
	string msg;
	SocketBase* s;
};

class UdpData : public Ref {
public:
	UdpData(Buffer *b, SockAddr *a);
	~UdpData();

	Buffer *buffer;
	SockAddr *addr;
};

typedef function< void(void*) > EventFunc;

class Socket : public ISocketBaseEvent {
public:
	friend class SocketState;
	friend class SocketStateConnecting;
	friend class SocketStateConnected;
	friend class SocketStateListening;

	typedef vector<Socket*> SocketVec;

	static inline const char* getMulticastAddr(int protocol);

	Socket();
	Socket(SocketBase *p);
	virtual ~Socket();

	bool connect(int port, const char *ip);
	bool listen(int port, const char *ip = 0);
	bool send(const char *buf, int len = 0);
	void close();
	void shutdown();
	bool create(int protocol = -1);

	// udp
	bool send(const char *buf, int len, int port, const char *ip);

	// multi cast
	bool addMembership(const char *addr = 0, const char *interface_ = 0);
	bool dropMembership(const char *addr = 0, const char *interface_ = 0);

	// methods
	void closeConnection(int i);
	void closeAllConnections();	
	
	// Note: need update state manually
	void update();
	
	bool isConnected();
	bool isClosed();
	int getState();

	virtual void onError(int error, int internalError);
	virtual void onClose(bool hasError = false);

	virtual void onListening();
	virtual void onConnection(SocketBase *s);

	virtual void onConnect();
	virtual void onData();
	
	int select(int ms = 0);
	bool canRead();
	bool canWrite();

	void on(int name, EventFunc cb);

	Socket* getConnection(int i);
	SocketVec& getConnections() { return connections; }
	int getMaxConnections() { return maxConnections; }
	float getConnectTimeout() { return connectTimeoutSecs; }
	const char* getIp();
	int getPort();
	float getIOTimeout() { return ioTimeoutSecs; }

	bool isUdp() { return socket_type == SOCK_DGRAM; }
	
	void setMaxConnections(int max) { maxConnections = max; }
	void setConnectTimeout(float sec) { connectTimeoutSecs = sec; }
	void setCheckIpv6Only(bool check) { checkIpv6Only = check; }
	void setSocketType(int type);
	void setUdp() { setSocketType(SOCK_DGRAM); }
	void setIOTimeout(float sec) { ioTimeoutSecs = sec; }
protected:
	typedef vector<EventFunc> EventFuncVec;
	typedef map<int, EventFuncVec*> EventMap;

	void setState(StateType type, void *param = 0);
	void recv();
	bool accept();
	bool checkConnected();
	void emitError();
	void recvfrom();
	SockAddr* getUdpAddr();

	void emit(int name, void *p = 0);
	// support ipv6-only
	int checkProtocol(int port, const char *ip, bool passive = false);

	SocketBase *impl;
	SocketState *state;

	EventMap events;
	SocketVec connections;
	
	int maxConnections;
	// -1 means socket default timeout
	float connectTimeoutSecs;

	addrinfo* addrInfo;
	bool checkIpv6Only;

	int socket_type;
	SockAddr *addrUdp;
	SockAddr *addrTemp;

	// -1 means never
	float ioTimeoutSecs;
};

EASY_NS_END