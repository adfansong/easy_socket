#include "Socket.h"
#include "SocketLinux.h"


EASY_NS_BEGIN

Socket::Socket()
	: state(0)
{
#if EASY_LINUX
	impl = new SocketLinux();
#else
#endif

	impl->setDelegate(this);

	// TODO: choose tcp protocol: 4 or 6.
	impl->create(4);
	
	setState(sDisconnected);
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
	if (isClosed()) {
		EASY_LOG("Socket::connect error: closed.");
		return false;
	}

	if (state && state->getType() == sConnecting) {
		EASY_LOG("Socket::connect warning: connecting.");
		return true;
	}

	if (impl) {
		setState(sConnecting);

		return impl->connect(port, ip);
	}

	return false;
}

bool Socket::listen(int port, const char *ip)
{
	if (isClosed()) {
		EASY_LOG("Socket::listen error: closed.");
		return false;
	}

	if (impl) {
		if (!impl->bind(port, ip)) {
			return false;
		}

		if (impl->listen()) {
			return true;
		}
	}

	return false;
}

bool Socket::send(const char *buf, int len)
{
	if (isClosed()) {
		EASY_LOG("Socket::send error: closed.");
		return false;
	}

	// TODO
	return true;
}

void Socket::close()
{
	if (impl) {
		impl->close();
	}
}

void Socket::update()
{
	if (state) {
		state->update();
	}
}

bool Socket::isConnected()
{
	return state && state->getType() == sConnected;
}

bool Socket::isClosed()
{
	return state && state->getType() == sClose;
}

void Socket::onError(int error, int internalError)
{
	emit(sError);
}

void Socket::onClose(bool hasError /*= false*/)
{
	setState(sClose);
	emit(sClose);
}

void Socket::onListening()
{
	setState(sListening);
	emit(sListening);
}

void Socket::onConnection(SocketBase *s)
{
	emit(sConnection, s);
}

void Socket::onConnect()
{
	setState(sConnected);
	emit(sConnected);
}

void Socket::onData()
{
	emit(sData);
}

int Socket::select(int ms)
{
	return impl->select(ms);
}

bool Socket::canRead()
{
	return impl ? impl->canRead() : false;
}

bool Socket::canWrite()
{
	return impl ? impl->canWrite() : false;
}

void Socket::on(int name, EventFunc cb)
{
	EventMap::iterator itr = events.find(name);
	EventFuncVec *p = 0;
	if (itr == events.end()) {
		p = new EventFuncVec();
		events[name] = p;
	} else {
		p = itr->second;
	}

	if (p) {
		p->push_back(cb);
	}
}

void Socket::off(int name, EventFunc cb)
{
	EventMap::iterator itr = events.find(name);
	if (itr != events.end()) {
		EventFuncVec *p = itr->second;
		if (p) {
			EventFuncVec::iterator it = p->begin();
			for (; it != p->end(); ++it) {
				if (*it == cb) {
					break;
				}
			}

			if (it != p->end()) {
				p->erase(it);
			}
		}

		if (!p || p->empty()) {
			events.erase(itr);
		}
	}
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
		return impl->accept(0);
	}

	return true;
}

bool Socket::checkConnected()
{
	return impl ? impl->checkConnected() : false;
}

void Socket::emitError()
{
	if (impl) {
		impl->emitError();
	}
}

void Socket::emit(int name, void *p)
{
	EventMap::iterator itr = events.find(name);
	if (itr != events.end()) {
		EventFuncVec *arr = itr->second;
		
		EventFuncVec::iterator it = arr->begin();
		for (; it != arr->end(); ++it) {
			(*(*it))(p);
		}
	}
}

EASY_NS_END
