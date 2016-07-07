#pragma once

#include "Common.h"

EASY_NS_BEGIN

class Socket;


enum StateType {
	Disconnected,
	Connecting,
	Connected,
	Listening,
};

// base state
class SocketState {
public:

	SocketState(Socket *s, void *p);
	virtual ~SocketState();

	virtual void update() {}

	StateType getType() { return type; }

	static SocketState* create(Socket *s, StateType t, void *p = 0);
protected:
	Socket	*socket;
	void *param;
	StateType type;
};

class SocketStateConnecting : public SocketState {
public:
	SocketStateConnecting(Socket *s, void *p);

	virtual void update();
};

class SocketStateConnected : public SocketState {
public:
	SocketStateConnected(Socket *s, void *p);

	virtual void update();
};

class SocketStateListening : public SocketState {
public:
	SocketStateListening(Socket *s, void *p);

	virtual void update();
};

EASY_NS_END