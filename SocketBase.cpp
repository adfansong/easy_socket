#include "SocketBase.h"

EASY_NS_BEGIN

SocketBase::SocketBase()
: protocol(4), addr(0), del(0), errorCode(0), set_unblock(true), set_reuseAddr(true)
{

}

SocketBase::~SocketBase()
{
	if (addr) {
		delete addr;
		addr = 0;
	}

	if (del) {
		del->release();
	}
}

void SocketBase::close(bool hasError /*= false*/)
{
	if (del) {
		del->onClose(hasError);
	}	
}

int SocketBase::select(int ms)
{	
	timeval tv;
	timeval *pt = &tv;
	if (ms < 0) {
		pt = 0;
	} else {
		pt->tv_sec = ms / 1000;
		pt->tv_usec = ms % 1000;
	}

	fd_set rfd, wfd;
	FD_ZERO(&rfd);
	FD_ZERO(&wfd);
	
	int fd = getSockFd();
	FD_SET(fd, &rfd);
	FD_SET(fd, &wfd);

	int ret = 0;
	if (::select(fd + 1, &rfd, &wfd, 0, pt) > 0) {
		if (FD_ISSET(fd, &rfd)) {
			ret |= 1;
		}
		if (FD_ISSET(fd, &wfd)) {
			ret |= 2;
		}
	}

	return ret;
}

bool SocketBase::canRead()
{
	return select() & 1;
}

bool SocketBase::canWrite()
{
	return select() & 2;
}

int SocketBase::getError()
{
	int cur = errorCode;
	errorCode = 0;
	return cur;
}

int SocketBase::getInternalError()
{
	int cur = internalError;
	internalError = 0;
	return cur;
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
		del->release();
	}

	del = p;
	if (del) {
		del->retain();
	}
}

void SocketBase::setError(int e, int ie /*= 0*/)
{
	errorCode = e != 0 ? e : 0;
	internalError = ie != 0 ? ie : 0;
}

void SocketBase::emitError()
{
	if (del) {
		del->onError(errorCode, internalError);
	}
	
	close(true);
}



EASY_NS_END