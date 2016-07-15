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
	start = time(0);
}

void SocketStateConnecting::update()
{	
	float timeout = socket->getConnectTimeout();
	if (timeout > 0 && (time(0) - start) >= timeout) {
		// Note: after setState, this object is released!
		Socket *s = socket;
		s->setState(sDisconnected);
		s->shutdown();
		return;
	}

	if (socket->canWrite()) {
		if (socket->checkConnected()) {
			socket->onConnect();
		} else {
			Socket *s = socket;
			s->setState(sDisconnected);
			s->emitError();
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
	if (!socket->isUdp()) {
		if (socket->getConnections().size() < socket->getMaxConnections()) {
			if (!socket->accept()) {
				Socket *s = socket;
				s->setState(sDisconnected);
				s->emitError();
			}
		}
	} else {
		if (socket->canRead()) {
			// do read
			socket->recvfrom();
		}
	}
}

EASY_NS_END
