#include "Socket.h"
#include "SocketLinux.h"

EASY_NS_BEGIN

Socket::Socket()
	: state(0), maxConnections(10)
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

Socket::Socket(SocketBase *p)
	: impl(p)
{
	impl->setDelegate(this);
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

	SocketVec::iterator itr = connections.begin();
	for (; itr != connections.end(); ++itr) {
		Socket *p = *itr;
		if (p) {
			delete p;
		}
	}
	connections.clear();

	EventMap::iterator it = events.begin();
	for (; it != events.end(); ++it) {
		EventFuncVec *p = it->second;
		if (p) {
			delete p;
		}
	}
	events.clear();
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
	
	if (len == 0) {
		len = strlen(buf) + 1;
	}

	return impl ? impl->send(buf, len) : false;
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

	// update connections
	SocketVec::iterator itr = connections.begin();
	for (; itr != connections.end(); ) {
		(*itr)->update();

		// remove closed
		if ((*itr)->isClosed()) {
			itr = connections.erase(itr);
			EASY_LOG("total connections: %d", connections.size());
		} else {
			++itr;
		}
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

int Socket::getState()
{
	return state ? state->getType() : -1;
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
	Socket *ps = new Socket(s);
	ps->setState(sConnected);
	connections.push_back(ps);

	emit(sConnection, ps);
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

void Socket::setState(StateType type, void *param)
{
	if (state && state->getType() == type) {
		return;
	}

	if (state) {
		delete state;
	}

	state = SocketState::create(this, type, param);
}

void Socket::recv()
{
	if (impl) {
		char p[65535];
		int ret = impl->recv(p, sizeof(p));
		if (ret > 0) {
			Buffer* buffer = new Buffer(ret);
			buffer->write(p, 0, ret);

			//listener need delete the buffer.
			emit(sData, buffer);

			buffer->release();
		} else if (ret == 0) {
			impl->close();
		} else if (ret == -1) {
			emitError();
			impl->close(true);
		}
	}
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
			(*it)(p);
		}
	}
}

Socket* Socket::getConnection(int i)
{
	if (i >= 0 && i < connections.size()) {
		return connections[i];
	} else {
		return 0;
	}
}


EASY_NS_END
