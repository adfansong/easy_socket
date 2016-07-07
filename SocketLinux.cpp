#include "SocketLinux.h"


#if EASY_LINUX

EASY_NS_BEGIN

SocketLinux::SocketLinux()
	: sock(-1)
{
	
}

SocketLinux::~SocketLinux()
{

}

bool SocketLinux::create(int protocol)
{
	close();

	if (protocol == 4 || protocol == 6) {
		this->protocol = protocol;

		int p = protocol == 4 ? PF_INET : PF_INET6;
		sock = ::socket(p, SOCK_STREAM, 0);
		if (sock != -1) {
			// default: unblock, reuse addr
			unblock();
			reuseAddr(true);
			return true;
		} else {
			EASY_LOG("SocketLinux::create error: %s", strerror(errno));
			return false;
		}
	} else {
		EASY_LOG("SocketLinux::create error: invalid protocol %d", protocol);
		return false;
	}
}

void SocketLinux::close(bool hasError)
{
	if (sock != -1) {
		::close(sock);
		sock = -1;
	}

	SocketBase::close(hasError);
}

int SocketLinux::send(const void *buf, size_t len)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::send error: invalid socket");
		return -1;
	}

	size_t ret = ::send(sock, buf, len, 0);
	if (ret == -1) {
		EASY_LOG("SocketLinux::send error: %s", strerror(errno));
	}
	return ret;
}

int SocketLinux::recv(void *buf, size_t len)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::recv error: invalid socket");
		return -1;
	}

	size_t ret = ::recv(sock, buf, len, 0);
	if (ret == -1) {
		EASY_LOG("SocketLinux::recv error: %s", strerror(errno));
	}
	return ret;
}

bool SocketLinux::bind(int port, const char *ip)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::bind error: invalid socket");
		return false;
	}
	
	SockAddr *p = getSockAddr(port, ip);
	int ret = ::bind(sock, p->pointer(), p->length());
	if (ret != -1) {
		return true;
	} else {
		EASY_LOG("SocketLinux::bind error: %s", strerror(errno));
		return false;
	}
}

bool SocketLinux::listen()
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::listen error: invalid socket");
		return false;
	}

	int ret = ::listen(sock, 5);
	if (ret != -1) {
		if (del) {
			del->onListening();
		}
		return true;
	} else {
		EASY_LOG("SocketLinux::listen error: %s", strerror(errno));
		return false;
	}
}

bool SocketLinux::accept(SockAddr *p)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::accept error: invalid socket");
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
		// TODO: save connections..
		SocketLinux *conn = new SocketLinux();
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
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// nop
		} else {
			EASY_LOG("SocketLinux::accept error: %s", strerror(errno));
			ret = false;
		}
	}

	if (temp) {
		delete temp;
	}

	return ret;
}

bool SocketLinux::connect(int port, const char *ip)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::connect error: invalid socket");
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
		if (ret == -1 && errno == EINPROGRESS) {
			return true;
		} else {
			EASY_LOG("SocketLinux::connect error: %s", strerror(errno));
			return false;
		}
	}
}

bool SocketLinux::canRead()
{
	fd_set fds;
	timeval tv;

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select(sock + 1, &fds, 0, 0, &tv) > 0) {
		if (FD_ISSET(sock, &fds)) {
			return true;
		}
	}

	return false;
}

bool SocketLinux::canWrite()
{
	fd_set fds;
	timeval tv;

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select(sock + 1, 0, &fds, 0, &tv) > 0) {
		if (FD_ISSET(sock, &fds)) {
			return true;
		}
	}

	return false;
}

bool SocketLinux::checkConnected()
{
	int error;
	socklen_t len = sizeof(error);
	int ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
	if (ret == -1) {
		EASY_LOG("SocketLinux::checkConnected.getsockpot error: %s", strerror(errno));
		return false;
	}

	if (error == ECONNREFUSED || error == ETIMEDOUT) {
		EASY_LOG("SocketLinux::checkConnected failed: %s", strerror(error));
		return false;
	}

	return true;
}

bool SocketLinux::unblock()
{
	int flags = fcntl(sock, F_GETFL, 0);
	int ret = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	if (ret == -1) {
		EASY_LOG("SocketLinux::unblock.fcntl error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketLinux::noDelay(bool no)
{
	int d = no ? 1 : 0;
	int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&d, sizeof(d));
	if (ret == -1) {
		EASY_LOG("SocketLinux::noDelay.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketLinux::reuseAddr(bool use)
{
	int d = use ? 1 : 0;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&d, sizeof(d));
	if (ret == -1) {
		EASY_LOG("SocketLinux::reuseAddr.setsockopt error: %s", strerror(errno));
		return false;
	}

	return true;
}

bool SocketLinux::noSigPipe(bool no)
{
#if EASY_IOS || EASY_MAC
	int d = no ? 1 : 0;
	int ret = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (const void *)&d, sizeof(d));
	if (ret == -1) {
		EASY_LOG("SocketLinux::noSigPipe.setsockopt error: %s", strerror(errno));
		return false;
	}

#endif
	return true;
}


EASY_NS_END

#endif