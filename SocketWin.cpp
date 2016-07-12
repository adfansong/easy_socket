#include "SocketWin.h"


#if EASY_WIN

EASY_NS_BEGIN

LPVOID lpMsgBuf = 0;

#ifdef errno
	#undef errno
#endif
#define errno WSAGetLastError()

// Note: the \r\n is removed
// LANG_NEUTRAL is for local language.
#define strerror(x)	\
	(FormatMessage(\
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
	NULL, \
	x, \
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), \
	(LPTSTR)&lpMsgBuf, \
	0, NULL), ((char*)lpMsgBuf)[strlen((const char*)lpMsgBuf) - 1] = 0, lpMsgBuf)

int SocketWin::__inited = 0;

SocketWin::SocketWin()
	: sock(-1)
{
	if (__inited++ == 0) {
		WSADATA wsdata;
		WSAStartup(MAKEWORD(2, 2), &wsdata);
	}
}

SocketWin::~SocketWin()
{
	close();

	if (--__inited == 0) {
		WSACleanup();
	}
}

bool SocketWin::create(int protocol)
{
	close();

	if (protocol == -1) {
		protocol = this->protocol;
	}

	if (protocol == 4 || protocol == 6) {
		this->protocol = protocol;

		int p = protocol == 4 ? PF_INET : PF_INET6;
		sock = ::socket(p, SOCK_STREAM, 0);
		if (sock != -1) {
			// default: unblock, reuse addr
			if (set_unblock) {
				unblock();
			}
			if (set_reuseAddr) {
				reuseAddr(true);
			}
			setSendBufferSize(65535);
			return true;
		} else {
			EASY_LOG("SocketWin::create error: %s", strerror(errno));
			return false;
		}
	} else {
		EASY_LOG("SocketWin::create error: invalid protocol %d", protocol);
		return false;
	}
}

void SocketWin::close(bool hasError)
{
	if (sock != -1) {
		closesocket(sock);
		sock = -1;
		
		SocketBase::close(hasError);
	}
}

bool SocketWin::send(const char *buf, size_t len)
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::send error: invalid socket");
		return false;
	}

	size_t ret = ::send(sock, buf, len, 0);
	if (ret == -1) {
		// unblock
		int error = errno;
		if (error == WSAEWOULDBLOCK) {
			EASY_LOG("SocketWin::send warning: send buffer is full.");
		} else {
			setError(error);
			EASY_LOG("SocketWin::send error: %s", strerror(error));
		}
		return false;
	}
	
	return ret == len;
}

int SocketWin::recv(char *buf, size_t len)
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::recv error: invalid socket");
		return -1;
	}

	size_t ret = ::recv(sock, buf, len, 0);	
	if (ret == -1) {
		int error = errno;
		if (error == WSAEWOULDBLOCK) {
			ret = -2;
		} else {
			setError(error);
			EASY_LOG("SocketWin::recv error: %s", strerror(error));
		}
	}
	return ret;
}

void SocketWin::shutdown()
{
	::shutdown(sock, SD_BOTH);
}

bool SocketWin::bind(int port, const char *ip)
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::bind error: invalid socket");
		return false;
	}
	
	SockAddr *p = getSockAddr(port, ip);
	int ret = ::bind(sock, p->pointer(), p->length());
	if (ret != -1) {
		return true;
	} else {
		int error = errno;
		setError(error);
		EASY_LOG("SocketWin::bind error: %s", strerror(error));
		return false;
	}
}

bool SocketWin::bind(addrinfo *addrInfo)
{
	if (addr) {
		addr->release();
	}

	addr = new SockAddr(addrInfo);

	return bind(0, 0);
}

bool SocketWin::listen()
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::listen error: invalid socket");
		return false;
	}

	int ret = ::listen(sock, 5);
	if (ret != -1) {
		if (del) {
			del->onListening();
		}
		return true;
	} else {
		int error = errno;
		setError(error);
		EASY_LOG("SocketWin::listen error: %s", strerror(error));
		return false;
	}
}

bool SocketWin::accept(SockAddr *p)
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::accept error: invalid socket");
		return false;
	}

	SockAddr *temp = 0;
	if (!p) {
		temp = new SockAddr(protocol);
		p = temp;
	}

	socklen_t len = p->length();
	int s = ::accept(sock, p->pointer(), &len);
	bool ret = true;
	if (s != -1) {
		SocketWin *conn = new SocketWin();
		conn->sock = s;
		conn->setSockAddr(p);
		if (del) {
			del->onConnection(conn);
		} else {
			delete conn;
			conn = 0;
		}

	} else {
		// unblock
		int error = errno;
		if (error == WSAEWOULDBLOCK) {
			// nop
		} else {
			setError(error);
			EASY_LOG("SocketLinux::accept error: %s", strerror(error));
			ret = false;
		}
	}

	if (temp) {
		temp->release();
	}

	return ret;
}

bool SocketWin::connect(int port, const char *ip)
{
	if (sock == -1) {
		setError(0, SocketInvalid);
		EASY_LOG("SocketWin::connect error: invalid socket");
		return false;
	}

	SockAddr *p = getSockAddr(port, ip);
	int ret = ::connect(sock, p->pointer(), p->length());

	if (ret == 0) {
		if (del) {
			del->onConnect();
		}
		return true;
	} else {
		// unblock
		int error = errno;
		if (ret == -1 && error == WSAEWOULDBLOCK) {
			return true;
		} else {
			setError(error);
			EASY_LOG("SocketLinux::connect error: %s", strerror(error));
			return false;
		}
	}
}

bool SocketWin::connect(addrinfo *addrInfo)
{
	if (addr) {
		addr->release();
	}

	addr = new SockAddr(addrInfo);
	
	return connect(0, 0);
}

const char* SocketWin::formatError(int error)
{
	return (const char*)strerror(error);
}

bool SocketWin::unblock()
{
	unsigned long ul = 1;
	int ret = ioctlsocket(sock, FIONBIO, (unsigned long*)&ul);
	if (ret == -1) {
		EASY_LOG("SocketWin::unblock.fcntl error: %s", strerror(errno));
		return false;
	}
	
	return true;
}


EASY_NS_END

#endif