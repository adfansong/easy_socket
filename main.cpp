#include "SocketLinux.h"

int main(int argc, char *argv[]) {
	bool client = true;
	string ip = "127.0.0.1";
	int port = 9999;

	if (argc == 1) {
		client = false;
	}
	
	SocketLinux s(ip, port);

	s.create(4);

	if (!client) {
		
		s.bind();
		s.listen();
		s.accept();

		char buf[1024] = { 0 };
		s.recv(buf, 1024);
		char str[] = "this is server!";
		s.send(str, sizeof(str));
		EASY_LOG("recved: %s", buf);
	} else {
		s.connect();

		char buf[1024] = "this is client!";
		s.send(buf, strlen(buf) + 1);
		s.recv(buf, 1024);
		EASY_LOG("recved: %s", buf);
	}

	return 0;
}