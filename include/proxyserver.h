#include "headers.h"



class HTTPServer {
public:
	struct sockaddr_in clientAddress, serevAddress;
	int serverSocket, clientSocket;
	unsigned int addresslength;

	void runServer(int);
	void httpGET(char buffer[510], char *url, char *method, char *v_http);
	void httpPOST(char buffer[510], char *url, char *method, char *v_http);
	void httpHEAD(char buffer[510], char *url, char *method, char *v_http);
	void acceptRequest();
};
