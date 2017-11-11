#include "headers.h"



class HTTPServer {
private:
	int countFiltered;
	std::vector<std::string> filteringDomains;

	bool filterURL(char *url);
public:
	struct sockaddr_in clientAddress, serevAddress;
	int serverSocket, clientSocket;
	unsigned int addresslength;

	void runServer(int, int, char**);
	void httpGET(char buffer[510], char *url, char *method, char *v_http);
	void httpPOST(char buffer[510], char *url, char *method, char *v_http);
	void httpHEAD(char buffer[510], char *url, char *method, char *v_http);
	void acceptRequest();
};
