#include "SocketLinux.h"


#if EASY_LINUX

SocketLinux::SocketLinux(string &ip, int port)
	: SocketBase(ip, port), sock(-1)
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

void SocketLinux::close()
{
	if (sock != -1) {
		::close(sock);
		sock = -1;
	}
}

bool SocketLinux::send(const void *buf, size_t len)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::send error: invalid socket");
		return false;
	}

	size_t ret = ::send(sock, buf, len, 0);
	if (ret == -1) {
		EASY_LOG("SocketLinux::send error: %s", strerror(errno));
	}
	return ret == len;
}

bool SocketLinux::recv(void *buf, size_t len)
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::recv error: invalid socket");
		return false;
	}

	// TODO
	size_t ret = ::recv(sock, buf, len, 0);
	if (ret == -1) {
		EASY_LOG("SocketLinux::recv error: %s", strerror(errno));
	}
	return ret == len;
}

bool SocketLinux::bind()
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::bind error: invalid socket");
		return false;
	}

	
	SocketBase::bind();

	sockaddr *p = getSockAddr();
	int ret = ::bind(sock, p, sizeof(*p));
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
		emit(Listen, 0);
		return true;
	} else {
		EASY_LOG("SocketLinux::listen error: %s", strerror(errno));
		return false;
	}
}

bool SocketLinux::accept()
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::accept error: invalid socket");
		return false;
	}

	sockaddr *p = 0;
	if (protocol == 4) {
		p = (sockaddr*)new sockaddr_in();
	} else {
		p = (sockaddr*)new sockaddr_in6();
	}
	
	// TODO: test sizeof(*p)
	socklen_t len = sizeof(*p);
	int s = ::accept(sock, p, &len);
	bool ret = true;
	if (s != -1) {
		ParamAcceptConnect param(s, p);
		emit(Accept, &param);
	} else {
		ret = false;
		EASY_LOG("SocketLinux::accept error: %s", strerror(errno));
	}

	if (p) {
		delete p;
	}

	return ret;
}

bool SocketLinux::connect()
{
	if (sock == -1) {
		EASY_LOG("SocketLinux::connect error: invalid socket");
		return false;
	}

	sockaddr *p = getSockAddr();

	int s = ::connect(sock, p, sizeof(*p));
	bool ret = true;
	if (s != -1) {
		ParamAcceptConnect param(s, p);
		emit(Connect, &param);
	} else {
		ret = false;
		EASY_LOG("SocketLinux::connect error: %s", strerror(errno));
	}

	return ret;
}

sockaddr* SocketLinux::getSockAddr()
{
	if (!addr) {
		if (protocol == 4) {
			sockaddr_in *sock_addr = new struct sockaddr_in();
			bzero(sock_addr, sizeof(sock_addr));

			sock_addr->sin_family = AF_INET;
			sock_addr->sin_port = htons(port);

			addr = (sockaddr*)sock_addr;
			if (client) {
				int ret = inet_pton(AF_INET, ip.c_str(), &sock_addr->sin_addr);
				if (ret == 0) {
					addr = 0;
					EASY_LOG("SocketLinux::getSockAddr.inet_pton error: %s", strerror(errno));
				}
			} else {
				sock_addr->sin_addr.s_addr = INADDR_ANY;
			}
		} else {
			sockaddr_in6 *sock_addr = new struct sockaddr_in6();
			bzero(sock_addr, sizeof(sock_addr));

			sock_addr->sin6_family = AF_INET6;
			sock_addr->sin6_port = htons(port);

			addr = (sockaddr*)sock_addr;
			if (client) {
				int ret = inet_pton(AF_INET6, ip.c_str(), &sock_addr->sin6_addr);
				if (ret == 0) {
					addr = 0;
					EASY_LOG("SocketLinux::getSockAddr.inet_pton error: %s", strerror(errno));
				}
			} else {
				sock_addr->sin6_addr = in6addr_any;
			}
		}
	}

	return addr;
}

#endif