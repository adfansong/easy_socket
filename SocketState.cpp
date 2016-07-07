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
	case Connecting:
		state = new SocketStateConnecting(s, p);
		break;
	case Connected:
		state = new SocketStateConnected(s, p);
		break;
	case Listening:
		state = new SocketStateListening(s, p);
		break;
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
			socket->setState(Disconnected);
		}
	}
}

SocketStateConnected::SocketStateConnected(Socket *s, void *p)
	: SocketState(s, p)
{

}

void SocketStateConnected::update()
{
	// TODO: do read, write..
	if (socket->canRead()) {
		
	}

	if (socket->canWrite()) {

	}
}

SocketStateListening::SocketStateListening(Socket *s, void *p)
	: SocketState(s, p)
{

}

void SocketStateListening::update()
{
	if (!socket->accept()) {
		// TODO: emit error
		socket->setState(Disconnected);
	}
}


EASY_NS_END
