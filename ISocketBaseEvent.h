#pragma once

#include "Common.h"

EASY_NS_BEGIN

class SocketBase;

class ISocketBaseEvent {
public:
	virtual ~ISocketBaseEvent() {};

	virtual void onError(int error, int internalError) = 0;
	virtual void onClose(bool hasError = false) = 0;

	virtual void onListening() = 0;
	virtual void onConnection(SocketBase *s) = 0;

	virtual void onConnect() = 0;
	virtual void onData() = 0;
	//virtual void onTimeout() = 0;
};

EASY_NS_END