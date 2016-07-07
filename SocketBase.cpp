#include "SocketBase.h"

EASY_NS_BEGIN

SocketBase::SocketBase()
: protocol(4), addr(0), del(0)
{

}

SocketBase::~SocketBase()
{
	if (addr) {
		delete addr;
		addr = 0;
	}

	close();

	if (del) {
		delete del;
		del = 0;
	}
}

void SocketBase::close(bool hasError /*= false*/)
{
	if (del) {
		del->onClose(hasError);
	}
}

SockAddr* SocketBase::getSockAddr(int port, const char *ip)
{
	if (!addr) {
		addr = new SockAddr(protocol);
	}
	addr->setPort(port);
	addr->setIp(ip);

	return addr;
}

void SocketBase::setSockAddr(SockAddr *p)
{
	if (addr) {
		delete addr;
	}
	addr = p;
}

void SocketBase::setDelegate(ISocketBaseEvent *p)
{
	if (del) {
		delete del;
	}
	del = p;
}

void SocketBase::emitError(int error)
{
	if (del) {
		del->onError(error);
	}
	
	close(true);
}



EASY_NS_END