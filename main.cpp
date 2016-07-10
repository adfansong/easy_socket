#include "Socket.h"

EASY_NS_USING;

int main(int argc, char *argv[]) {
	bool client = true;
	char ip[] = "127.0.0.1";
	//char ip[] = "::1";
	int port = 9999;

	if (argc == 1) {
		client = false;
	}	
	
	Socket s;	
	if (!client) {		
		s.on(sListening, ([](void* p)->void{
			EASY_LOG("Listening..");
		}));

		s.on(sConnection, [&s](void* p)->void{
			EASY_LOG("Connection..");

			Socket *ps = (Socket*)p;
			ps->send("Hello, this is server!");

			ps->on(sData, [](void* p)->void{
				Buffer *buffer = (Buffer*)p;
				if (buffer) {
					EASY_LOG("Data: %s", buffer->toString());
				}
			});

			ps->on(sClose, [&s](void *p)->void {
				EASY_LOG("Connection closed");
			});
		});

		s.on(sError, [](void*)->void{
			EASY_LOG("Error..");
		});

		s.listen(port, 0);
		
		EASY_LOG("start update..");
		for (;;) {
			s.update();
		}
	} else {				
		s.on(sConnected, [&s](void*)->void{
			EASY_LOG("Connection..");
			s.send("Hello, this is client!");
		});

		s.on(sError, [](void*)->void{
			EASY_LOG("Error..");
		});

		s.on(sData, [](void* p){
			Buffer *buffer = (Buffer*)p;
			if (p) {
				EASY_LOG("Data: %s", buffer->toString());
			}
		});

		s.on(sClose, [](void *p)->void {
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