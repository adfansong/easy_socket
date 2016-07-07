#pragma once

#include "Common.h"
#include "SocketState.h"
#include "ISocketBaseEvent.h"

EASY_NS_BEGIN

class SocketBase;

class Socket : public ISocketBaseEvent {
public:
	friend class SocketState;
	friend class SocketStateConnecting;
	friend class SocketStateListening;

	Socket();
	virtual ~Socket();

	virtual bool connect(int port, const char *ip);
	virtual bool listen(int port, const char *ip);
	
	// Note: need update state
	virtual void update();
	
	bool isConnected();

	virtual void onError(int error);
	virtual void onClose(bool hasError = false);

	virtual void onListening();
	virtual void onConnection(SocketBase *s);

	virtual void onConnect();
	virtual void onData();

	bool canRead();
	bool canWrite();
protected:
	void setState(StateType type, void *param = 0);
	bool accept();
	bool checkConnected();

	SocketBase *impl;
	SocketState *state;
};

EASY_NS_END