#pragma once

#include "Common.h"
#include "SocketState.h"
#include "ISocketBaseEvent.h"
#include "Buffer.h"

#include <vector>
#include <map>
#include <functional>

EASY_NS_BEGIN

class SocketBase;

class SocketError {
	SocketError(int code, int inter = 0, const char *msg = "")
		: code(code), internalCode(inter), msg(msg) {}

	int code;
	int internalCode;
	string msg;
};

typedef function< void(void*) > EventFunc;

class Socket : public ISocketBaseEvent {
public:
	friend class SocketState;
	friend class SocketStateConnecting;
	friend class SocketStateConnected;
	friend class SocketStateListening;

	typedef vector<Socket*> SocketVec;

	Socket();
	Socket(SocketBase *p);
	virtual ~Socket();

	bool connect(int port, const char *ip);
	bool listen(int port, const char *ip);
	bool send(const char *buf, int len = 0);
	void close();
	
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

	void setMaxConnections(int max) { maxConnections = max; }
protected:
	typedef vector<EventFunc> EventFuncVec;
	typedef map<int, EventFuncVec*> EventMap;

	void setState(StateType type, void *param = 0);
	void recv();
	bool accept();
	bool checkConnected();
	void emitError();

	void emit(int name, void *p = 0);

	SocketBase *impl;
	SocketState *state;

	EventMap events;
	SocketVec connections;
	
	int maxConnections;
};

EASY_NS_END