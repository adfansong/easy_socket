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
		s.on(sListening, [](void*){
			EASY_LOG("Listening..");
		});
		s.on(sConnection, [](void*){
			EASY_LOG("Connection..");
		});

		s.on(sError, [](void*){
			EASY_LOG("Error..");
		});

		s.listen(port, 0);
		
		EASY_LOG("start update..");
		for (;;) {
			s.update();
		}
	} else {
		s.on(sConnected, [](void*){
			EASY_LOG("Connected..");
		});

		s.on(sError, [](void*){
			EASY_LOG("Error..");
		});

		s.connect(port, ip);

		EASY_LOG("start update..");
		for (;;) {
			s.update();
		}
	}

	return 0;
}