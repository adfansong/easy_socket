#include "SocketBase.h"


SocketBase::SocketBase(string &ip, int port)
	: ip(ip), port(port), protocol(4), addr(0), client(true)
{

}

SocketBase::~SocketBase()
{
	if (addr) {
		delete addr;
		addr = 0;
	}

	close();
}

bool SocketBase::bind()
{
	client = false;
}

void SocketBase::emit(EventType type, void *param)
{
	EASY_LOG("SocketBase::emit: %d", (int)type);
}
