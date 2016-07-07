#include "Socket.h"

EASY_NS_USING;

int main(int argc, char *argv[]) {
	bool client = true;
	char ip[] = "127.0.0.1";
	int port = 9999;

	if (argc == 1) {
		client = false;
	}
	
	Socket s;
	
	if (!client) {
		s.listen(port, 0);
		
		EASY_LOG("start update..");
		for (;;) {
			s.update();
		}
	} else {
		s.connect(port, ip);

		EASY_LOG("start update..");
		for (;;) {
			s.update();
		}
	}

	return 0;
}