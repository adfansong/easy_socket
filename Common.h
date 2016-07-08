#pragma once

#define EASY_DEBUG	1

#if defined(_WINDOWS)
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

#endif

enum InternalError {
	None,
	SocketInvalid,
};