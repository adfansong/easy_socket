#pragma once

#define EASY_DEBUG	1

#if defined(_WIN32)
	#define EASY_WIN 1
#else
	#define EASY_LINUX 1

	// Apple: Mac and iOS
	#if defined(__APPLE__) && !defined(ANDROID) // execlude android for binding generator.
		#include <TargetConditionals.h>
		#if TARGET_OS_IPHONE // TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
			#define EASY_IOS 1
		#elif TARGET_OS_MAC
			#define EASY_MAC 1
		#endif
	#endif
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <string>
using namespace std;

#define EASY_NS_BEGIN	namespace easy_socket {
#define EASY_NS_END		}
#define EASY_NS_USING	using namespace easy_socket

#define EASY_BIND_CLASS(pfunc, cls)	\
	std::bind(pfunc, cls, std::placeholders::_1)

#if EASY_DEBUG
	#define EASY_LOG(...)		\
		{	\
		char _str[1024] = {0};	\
		sprintf(_str, __VA_ARGS__);	\
		printf("%s\n", _str); \
		}
#else
	#define EASY_LOG
#endif

#if EASY_LINUX

	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>

	#include <sys/syscall.h>

	#include <sys/types.h>
	#include <sys/socket.h>

	#include <netinet/in.h>
	#include <netinet/tcp.h>
	
	#include <arpa/inet.h>
	#include <netdb.h>

#else

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "ws2_32.lib")

#endif

EASY_NS_BEGIN

enum InternalError {
	InternalNone,
	SocketInvalid,
};

class Ref
{
	int  m_refer;

	//this can avoid that invoke release when in destructor
	bool m_lock;

protected:
	//u must invoke this function in your destructor function
	void lock()
	{
		m_lock = true;
	}

public:
	virtual ~Ref()
	{
		//this must be someone call delete directly.
		if (m_refer > 0)
		{
			//EASY_LOG("detect directly call delete in Ref \n");
		}
	}

	Ref()
	{
		m_refer = 1;

		m_lock = false;
	}

	void release()
	{
		if (--m_refer == 0)
		{
			if (!m_lock)
			{
				delete this;
			}
		}
	}

	void retain()
	{
		++m_refer;
	}

	int ref()
	{
		return m_refer;
	}
};

EASY_NS_END