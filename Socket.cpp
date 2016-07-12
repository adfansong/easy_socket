#include "Socket.h"
#include "SocketLinux.h"
#include "SocketWin.h"

EASY_NS_BEGIN

SocketError::SocketError(int code, int inter /*= 0*/, const char *msg /*= 0*/, SocketBase* s)
	: code(code), internalCode(inter), msg(msg), s(s)
{
	formatError();
}

void SocketError::formatError()
{
	if (code != 0) {
		if (s) {
			msg = s->formatError(code);
		} else {
			msg = "formatError failed: SocketBase invalid.";
		}
	} else {
		switch (internalCode)
		{
		case InternalNone:
			msg = "no internal error.";
			break;
		case SocketInvalid:
			msg = "socket invalid.";
			break;
		default:
			msg = "unknown internal error.";
			break;
		}
	}
}

Socket::Socket()
	: state(0), maxConnections(10), connectTimeoutSecs(5), 
	addrInfo(0), checkIpv6Only(true)
{
#if EASY_LINUX
	impl = new SocketLinux();
#else
	impl = new SocketWin();
#endif

	impl->setDelegate(this);
	
	setState(sClose);
}

Socket::Socket(SocketBase *p)
	: state(0), maxConnections(10), connectTimeoutSecs(-1), 
	addrInfo(0), checkIpv6Only(true), impl(p)
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

	if (addrInfo) {
		freeaddrinfo(addrInfo);
		addrInfo = 0;
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
	int protocol = checkProtocol(port, ip);

	if (isClosed() || protocol != impl->getProtocol()) {
		if (!create(protocol)) {
			EASY_LOG("Socket::connect error: create socket failed.");
			return false;
		}
	}

	if (state && state->getType() == sConnecting) {
		EASY_LOG("Socket::connect warning: connecting.");
		return true;
	}

	if (impl) {
		setState(sConnecting);

		if (addrInfo) {
			return impl->connect(addrInfo);
		} else {
			return impl->connect(port, ip);
		}
	}

	return false;
}

bool Socket::listen(int port, const char *ip)
{
	int protocol = checkProtocol(port, ip, true);

	if (isClosed() || protocol != impl->getProtocol()) {
		if (!create(protocol)) {
			EASY_LOG("Socket::listen error: create socket failed.");
			return false;
		}
	}

	if (impl) {
		if (addrInfo) {
			if (!impl->bind(addrInfo)) {
				return false;
			}
		} else {
			if (!impl->bind(port, ip)) {
				return false;
			}
		}

		if (impl->listen()) {
			return true;
		}
	}

	return false;
}

bool Socket::send(const char *buf, int len)
{
	if (!isConnected()) {
		EASY_LOG("Socket::send error: not connected yet.");
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

void Socket::shutdown()
{
	if (impl) {
		impl->shutdown();
		impl->close();
	}
}

bool Socket::create(int protocol /*= -1*/)
{
	close();
	
	if (impl->create(protocol)) {
		setState(sDisconnected);
		return true;
	} else {
		return false;
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
	SocketError se(error, internalError, "", impl);
	
	emit(sError, &se);
}

void Socket::onClose(bool hasError /*= false*/)
{
	setState(sClose);
	emit(sClose, (void*)hasError);
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

Socket* Socket::getConnection(int i)
{
	if (i >= 0 && i < connections.size()) {
		return connections[i];
	} else {
		return 0;
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

int Socket::checkProtocol(int port, const char *ip, bool passive)
{
	int protocol = 4;
	if (!checkIpv6Only) {
		return protocol;
	}

	struct addrinfo hints, *res;
	
	char port_s[30];
	sprintf(port_s, "%d", port);
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (ip == 0 || passive) {
		if (!ip) {
			ip = "127.0.0.1";
		}
		hints.ai_flags = AI_PASSIVE;
	}

	if (addrInfo) {
		freeaddrinfo(addrInfo);
		addrInfo = 0;
	}
	
	int ret = getaddrinfo(ip, port_s, &hints, &res);
	if (ret == 0) {
		addrInfo = res;
		if (addrInfo->ai_family == AF_INET6) {
			EASY_LOG("Socket::checkProtocol: using ipv6!");
			protocol = 6;
		}
	} else {
		EASY_LOG("Socket::checkProtocol error: getaddrinfo failed.");
	}

	return protocol;
}


EASY_NS_END
