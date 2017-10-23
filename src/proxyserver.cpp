#include "headers.h"
#include "proxyserver.h"

using namespace std;

#define MAX_ALLOWED_CONNECTIONS 100

void HTTPServer::runServer(int port) {
	cout << "\n\ncreating server\n\n";

	bzero((char *)&serevAddress, sizeof(serevAddress));
	bzero((char *)&clientAddress, sizeof(clientAddress));

	addresslength = sizeof(serevAddress);
	
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverSocket < 0) {
		printf("Failed to create socket\n");
		exit(1);
	}

	serevAddress.sin_family = AF_INET;
	serevAddress.sin_addr.s_addr=INADDR_ANY;
	serevAddress.sin_port = htons(port);

	int bindstat = bind(serverSocket,(struct sockaddr*)&serevAddress,addresslength);
	if(bindstat < 0) {
		printf("Failed to bind\n");
		exit(1);
	}

	if(listen(serverSocket, MAX_ALLOWED_CONNECTIONS) < 0) {
		printf("Failed to listen\n");
		exit(1);
	}

	//acceptRequest();
	close(serverSocket);

}

void HTTPServer::acceptRequest() {

	while(1) {
		if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t*)&addresslength))<0) {
			printf("Problem in accepting connection");
			exit(1);	
		}
	}
}

void HTTPServer::httpGET() {
	cout << "\n\nYAHS: Yet Another HTTP-proxy Server\n\n";
}

void HTTPServer::httpPOST() {
	cout << "\n\nYAHS: Yet Another HTTP-proxy Server\n\n";
}

void HTTPServer::httpHEAD() {
	cout << "\n\nYAHS: Yet Another HTTP-proxy Server\n\n";
}

int main(int argc,char* argv[]) {
	HTTPServer httpServer;
	httpServer.runServer(atoi(argv[1]));
	return 0;
}
