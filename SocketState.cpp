#include "SocketState.h"
#include "Socket.h"

EASY_NS_BEGIN

SocketState::SocketState(Socket *s, void *p)
	: socket(s), param(p)
{

}

SocketState::~SocketState()
{
	if (param) {
		delete param;
		param = 0;
	}
}

SocketState* SocketState::create(Socket *s, StateType t, void *p)
{
	SocketState *state = 0;
	switch (t) {
	case sConnecting:
		state = new SocketStateConnecting(s, p);
		break;
	case sConnected:
		state = new SocketStateConnected(s, p);
		break;
	case sListening:
		state = new SocketStateListening(s, p);
		break;
	case sClose:
	default:
		state = new SocketState(s, p);
		break;
	}

	if (state) {
		state->type = t;
	}
	return state;
}

SocketStateConnecting::SocketStateConnecting(Socket *s, void *p)
	: SocketState(s, p)
{
	
}

void SocketStateConnecting::update()
{
	if (socket->canWrite()) {
		if (socket->checkConnected()) {
			socket->onConnect();
		} else {
			socket->setState(sDisconnected);
			socket->emitError();
		}
	}
}

SocketStateConnected::SocketStateConnected(Socket *s, void *p)
	: SocketState(s, p)
{

}

void SocketStateConnected::update()
{
	int ret = socket->select();
	if (ret & 1) {
		// do read
		socket->recv();
	}
}

SocketStateListening::SocketStateListening(Socket *s, void *p)
	: SocketState(s, p)
{

}

void SocketStateListening::update()
{
	if (!socket->accept()) {
		socket->setState(sDisconnected);
		socket->emitError();
	}
}


EASY_NS_END
