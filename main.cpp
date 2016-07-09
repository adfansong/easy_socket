#include "Socket.h"

EASY_NS_USING;

Socket s;

int main(int argc, char *argv[]) {
	bool client = true;
	char ip[] = "127.0.0.1";
	int port = 9999;

	if (argc == 1) {
		client = false;
	}	
	
	if (!client) {
		s.on(sListening, [](void*){
			EASY_LOG("Listening..");
		});

		s.on(sConnection, [](void* p){
			EASY_LOG("Connection..");

			Socket *ps = (Socket*)p;
			ps->send("Hello, this is server!");

			ps->on(sData, [](void* p){
				Buffer *buffer = (Buffer*)p;
				if (buffer) {
					EASY_LOG("Data: %s", buffer->toString());
				}
			});

			ps->on(sClose, [](void *p) {
				EASY_LOG("Connection closed");
			});
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
			EASY_LOG("Connection..");
			s.send("Hello, this is client!");
		});

		s.on(sError, [](void*){
			EASY_LOG("Error..");
		});

		s.on(sData, [](void* p){
			Buffer *buffer = (Buffer*)p;
			if (p) {
				EASY_LOG("Data: %s", buffer->toString());
			}
		});

		s.on(sClose, [](void *p) {
			EASY_LOG("Connection closed.");
		});

		s.connect(port, ip);

		EASY_LOG("start update..");
		for (;;) {
			s.update();

			if (s.isClosed()) {
				break;
			}
		}
	}

	return 0;
}