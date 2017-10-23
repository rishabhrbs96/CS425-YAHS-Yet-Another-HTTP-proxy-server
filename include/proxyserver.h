#include "headers.h"



class HTTPServer {
public:
	struct sockaddr_in clientAddress, serevAddress;
	int serverSocket, clientSocket;
	unsigned int addresslength;

	void runServer(int);
	void httpGET();
	void httpPOST();
	void httpHEAD();
	void acceptRequest();
};
