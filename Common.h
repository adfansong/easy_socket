#pragma once

#if defined(_WINDOWS)
	#define EASY_WIN 1
#else
	#define EASY_LINUX 1
#endif

#include <stdio.h>
#include <string.h>

#include <string>
using std::string;

//#define EASY_LOG	printf

#define EASY_LOG(...)		\
	{	\
	char _str[1024] = {0};	\
	sprintf(_str, __VA_ARGS__);	\
	printf("%s\n", _str); \
	}

#if EASY_LINUX

	#include <unistd.h>
	#include <errno.h>

	#include <sys/types.h>
	#include <sys/socket.h>

	#include <netinet/in.h>
	
	#include <arpa/inet.h>
	#include <netdb.h>

#else

#endif