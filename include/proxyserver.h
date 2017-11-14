#include "headers.h"

int *countProcessed = (int *)mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
int *countFiltered = (int *)mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
int *countError = (int *)mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
int serverSocket, clientSocket;

class HTTPServer {
private:
	std::vector<char *> filteringDomains;
	bool filterURL(char *url);

public:
	struct sockaddr_in clientAddress, serevAddress;
	unsigned int addresslength;

	void runServer(int, int, char**);
	void httpGET(char buffer[2048], char *url, char *method, char *v_http);
	void httpPOST(char buffer[2048], char *url, char *method, char *v_http);
	void httpHEAD(char buffer[2048], char *url, char *method, char *v_http);
	void acceptRequest();
	void printStats();

};
