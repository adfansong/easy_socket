#include "Socket.h"
#include "SocketLinux.h"


EASY_NS_BEGIN

Socket::Socket()
{
#if EASY_LINUX
	impl = new SocketLinux();
#else
#endif

	impl->setDelegate(this);

	// TODO: choose tcp protocol: 4 or 6.
	impl->create(4);

	state = 0;
	setState(Disconnected);
}

Socket::~Socket()
{
	if (impl) {
		delete impl;
		impl = 0;
	}

	if (state) {
		delete state;
		state = 0;
	}
}

bool Socket::connect(int port, const char *ip)
{	
	if (impl) {
		setState(Connecting);

		return impl->connect(port, ip);
	}

	return false;
}

bool Socket::listen(int port, const char *ip)
{
	if (impl) {
		if (!impl->bind(port, ip)) {
			return false;
		}

		if (impl->listen()) {
			setState(Listening);
			return true;
		}
	}

	return false;
}

void Socket::update()
{
	if (state) {
		state->update();
	}
}

bool Socket::isConnected()
{
	return state && state->getType() == Connected;
}

void Socket::onError(int error)
{

}

void Socket::onClose(bool hasError /*= false*/)
{

}

void Socket::onListening()
{
	EASY_LOG(__FUNCTION__);
}

void Socket::onConnection(SocketBase *s)
{
	EASY_LOG(__FUNCTION__);
}

void Socket::onConnect()
{
	EASY_LOG(__FUNCTION__);
	setState(Connected);
}

void Socket::onData()
{
	EASY_LOG(__FUNCTION__);
}

void Socket::setState(StateType type, void *param)
{
	if (state) {
		delete state;
	}

	state = SocketState::create(this, type, param);
}

bool Socket::accept()
{
	if (impl) {
		//SockAddr *p = new SockAddr(impl->getProtocol());
		return impl->accept(0);
	}

	return true;
}

bool Socket::canRead()
{
	return impl ? impl->canRead() : false;
}

bool Socket::canWrite()
{
	return impl ? impl->canWrite() : false;
}

bool Socket::checkConnected()
{
	return impl ? impl->checkConnected() : false;
}



EASY_NS_END
