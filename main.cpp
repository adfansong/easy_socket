#include "Socket.h"

EASY_NS_USING;

#define TEST_CLASS	1

class Server;

class Client {
public:
	Client(Socket *_ps) {
		stop = false;
		pserv = 0;

		if (!_ps) {
			_ps = new Socket();
		}

		ps = _ps;
		ps->on(sConnected, EASY_BIND_CLASS(&Client::onConnect, this));
		ps->on(sError, EASY_BIND_CLASS(&Client::onError, this));
		ps->on(sData, EASY_BIND_CLASS(&Client::onData, this));
		ps->on(sClose, EASY_BIND_CLASS(&Client::onClose, this));
	}
	Client(int port, const char *ip)
		: Client(0)
	{		
		EASY_LOG("connect: %s:%d", ip, port);
		//ps->setConnectTimeout(3);
		ps->connect(port, ip);

		while (!stop) {
			ps->update();
		}
	}

	~Client() {
		if (ps) {
			ps->release();
		}
	}

	void setServer(Server* s) {
		pserv = s;
	}
private:
	void onConnect(void *p) {
		EASY_LOG("connected!");
	}

	void onError(void *p) {
		SocketError *pe = (SocketError *)p;
		EASY_LOG("error: %s", pe->msg.c_str());
	}

	void onData(void *p) {
		Buffer *buffer = (Buffer*)p;
		if (buffer) {
			EASY_LOG("data: %s", buffer->toString());
		}
	}

	void onClose(void *p);

	Socket *ps;
	Server *pserv;

	bool stop;
};

class Server {
public:
	Server(int port, const char *ip = 0) {
		s.on(sListening, EASY_BIND_CLASS(&Server::onListening, this));
		s.on(sConnection, EASY_BIND_CLASS(&Server::onConnection, this));
		s.on(sError, EASY_BIND_CLASS(&Server::onError, this));
		
		s.listen(port, ip);

		for (;;) {
			s.update();
		}
	}

	~Server() {
		for (vector<Client*>::iterator itr = cs.begin(); itr != cs.end(); ++itr) {
			delete (*itr);
		}
		cs.clear();
	}

	void clientClosed(Client *c) {
		for (vector<Client*>::iterator itr = cs.begin(); itr != cs.end(); ++itr) {
			if ((*itr) == c) {
				cs.erase(itr);
				EASY_LOG("client closed.");
				break;
			}
		}
	}
private:
	void onListening(void *p) {
		EASY_LOG("listening..");
	}

	void onConnection(void *p) {
		EASY_LOG("new connection!");

		Socket *ps = (Socket*)p;
		
		char buf[1024] = { 0 };
		sprintf(buf, "Hello, your index is %d.", s.getConnections().size());
		ps->send(buf);
		
		Client *pc = new Client(ps);
		pc->setServer(this);
		cs.push_back(pc);
	}

	void onError(void *p) {
		SocketError *pe = (SocketError *)p;
		EASY_LOG("error: %s", pe->msg.c_str());
	}

	Socket s;
	vector<Client*> cs;
};

void Client::onClose(void *p)
{
	EASY_LOG("close");
	if (pserv) {
		pserv->clientClosed(this);
	}

	stop = true;
}

int main(int argc, char *argv[]) {
	char ip[] = "127.0.0.1";
	//char ip[] = "::1";
	int port = 9191;

	bool client = true;
	if (argc == 1) {
		client = false;
	}	

#if TEST_CLASS != 1
	
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
#else

	if (!client) {
		Server s(port);
	} else {		
		Client c(port, ip);
	}
	

#endif

	return 0;
}