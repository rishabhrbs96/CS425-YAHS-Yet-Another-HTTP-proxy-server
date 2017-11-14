#include "headers.h"
#include "proxyserver.h"

using namespace std;

#define MAX_ALLOWED_CONNECTIONS 100

extern int *countProcessed;
extern int *countFiltered;
extern int *countError;

HTTPServer httpServer;
vector <int> procid;

void handle_sigusr1(int signum)  {
	printf("Received SIGUSR1...reporting status:\n");
	httpServer.printStats();
}

void handle_sigusr2(int signum)  {
	printf("Received SIGUSR2\n");
	for(vector <int> :: reverse_iterator it = procid.rbegin(); it != procid.rend(); it++)
		kill(*it,SIGKILL);
}

void HTTPServer::printStats() {
	printf("-- Processed %d requests successfully\n", countProcessed[0]);
	printf("-- Filtering:");
	for(int i=0; i<filteringDomains.size(); i++)
		printf("%s; ", filteringDomains[i]);
	printf("\n-- Filtered %d requests\n", countFiltered[0]);
	printf("-- Encountered %d requests in error\n", countError[0]);
}

bool HTTPServer::filterURL(char *url){
	int siz = filteringDomains.size();
	bool flag = true; 
	
	for(int i=0;i<siz;i++){
		if(strstr(url, filteringDomains[i]) != NULL) {
			flag = false;
			break;
		}
	}
	return flag;
}

void HTTPServer::runServer(int port, int argc, char **argv) {
	cout << "Server process running on pid : " << getpid() << endl;
	
	struct sigaction psa1,psa2;
	psa1.sa_handler = handle_sigusr1;
	psa2.sa_handler = handle_sigusr2;
	sigaction(SIGUSR1, &psa1, NULL);
	sigaction(SIGUSR2, &psa2, NULL);
	
	procid.push_back(getpid());
	
	countProcessed[0] = 0;
	countFiltered[0] = 0;
	countError[0] = 0;
	filteringDomains.clear();

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

	// Initialising the number of filtered requests 
	for(int i=0;i<argc-2;i++)
		filteringDomains.push_back(argv[2+i]);

	printf("Server Created Successfully\n");

	int pid;
	while(1) {
		if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t*)&addresslength))<0) {
			continue;	
		}

		if((pid = fork()) == -1) {
			close(clientSocket);
			continue;
		}
		else if(pid > 0) {
			close(clientSocket);
			continue;
		}
		else if(pid==0) {
			procid.push_back(getpid());
			acceptRequest();
			close(clientSocket);
			//procid.erase(remove(procid.begin(), procid.end(), pid), procid.end());
		}

	}

	close(serverSocket);
	return;
}

void HTTPServer::acceptRequest() {
	char httprequest[2048],buffer[2048];
	char *ts, *method, *url, *v_http;
	
	bzero((char*)httprequest,500);
	recv(clientSocket,httprequest,500,0);
	strcpy(buffer,httprequest);
	
	ts = strtok(httprequest," ");
	method = ts;
	ts = strtok(NULL," ");
	url = ts;
	ts = strtok(NULL,"\n");
	v_http = ts;

	if(filterURL(url) == false){
		countFiltered[0]++;
		printf("%s %s [FILTERED]\n", method, url);
		send(clientSocket,"<html><head>403 Forbidden</head><body><h1>HTTP Error: 403 Forbidden</h1><p>AccessDenied: You don't have permission to access this website.</p></body></html>",156,0);
		return;
	}


	if((strncmp(v_http,"HTTP/1.1",8) != 0) && ( strncmp(v_http,"HTTP/1.0",8) != 0)) {
		countError[0]++;
		send(clientSocket,"<html><head>501 Not Implemented</head><body><h1>HTTP Error: 501 Not Implemented</h1><p>HTTP/1.0 or HTTP/1.1 requests allowed only.</p></body></html>",148,0);
		return;
	}
		
	if(strncmp(url,"http://",7) != 0) {
		countError[0]++;
		send(clientSocket,"<html><head>501 Not Implemented</head><body><h1>HTTP Error: 501 Not Implemented</h1><p>HTTPS requests not allowed.</p></body></html>",132,0);
		return;
	}

	if(strstr(buffer, "\nHost") == NULL) {
		countError[0]++;
		send(clientSocket,"<html><head>400 Bad Request</head><body><h1>HTTP Error: 400 Bad Request</h1><p>Your browser sent a request that this server could not understand.</p></body></html>",163,0);
		return;
	}

	if(strstr(buffer, "\nContent-Length") == NULL && strcmp(method,"POST") == 0) {
		countError[0]++;
		send(clientSocket,"<html><head>400 Bad Request</head><body><h1>HTTP Error: 400 Bad Request</h1><p>Your browser sent a request that this server could not understand.</p></body></html>",163,0);
		return;
	}

	v_http[8] = '\0';
		
	if(strcmp(method,"GET") == 0) {
		this->httpGET(buffer,url,method,v_http);
	} else if(strcmp(method,"POST") == 0) {
		this->httpPOST(buffer,url,method,v_http);
	} 
	else if(strcmp(method,"HEAD") == 0) {
		this->httpHEAD(buffer,url,method,v_http);
	} 
	else {
		countError[0]++;
		send(clientSocket,"<html><head>405 Method not allowed</head><body><h1>HTTP Error: 405 Method not allowed</h1><p>Only HTTP GET, HEAD and POST methods are allowed on this server.</p></body></html>",175,0);
		return;
	}
	
	return;
}

void HTTPServer::httpGET(char buffer[2048], char *url, char *method, char *v_http) {
	printf("GET %s\n", url);

	struct sockaddr_in host_addr;
	int newhostSocket,n,port=80,hostSocket;
	struct hostent* host;
	char* temp=NULL;
	
	url = strstr(url, "://")+3;
	for(n = 0; n<strlen(url); ++n)
		if(url[n] == '/') {
			temp = strstr(url, "/")+1;
			break;
		}
	url = strtok(url,"/");

	host = gethostbyname(url);
	if(host == 0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}

	bzero((char*)&host_addr,sizeof(host_addr));
	host_addr.sin_port = htons(port);
	host_addr.sin_family = AF_INET;
	bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
	hostSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	newhostSocket = connect(hostSocket,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
	
	if(newhostSocket<0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}
	
	bzero((char*)buffer,sizeof(buffer));
	if(temp != NULL) {
		sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
	}
	else {
		sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
	}

	n = send(hostSocket,buffer,strlen(buffer),0);
	if(n<0) {
		countError[0]++;
		send(clientSocket,"<html><head>520 Unknown Error</head><body><h1>HTTP Error: 520 Unknown Error</h1><p>The server is returning unknown error.</p></body></html>",139,0);
	}
	else {
		do {
			bzero((char*)buffer,500);
			n=recv(hostSocket,buffer,500,0);
			if(!(n<=0))
				send(clientSocket,buffer,n,0);
		} while(n>0);
		countProcessed[0]++;
	}
}

void HTTPServer::httpPOST(char buffer[2048], char *url, char *method, char *v_http) {
	printf("POST %s\n", url);

	char *requestBody = strstr(buffer,"\n");
	struct sockaddr_in host_addr;
	int newhostSocket,n,port=80,i,hostSocket;
	struct hostent* host;
	char* temp=NULL;
	   
	url = strstr(url, "://")+3;
	for(n = 0; n<strlen(url); ++n)
		if(url[n] == '/') {
			temp = strstr(url, "/")+1;
			break;
		}
	url = strtok(url,"/");

	host = gethostbyname(url);
	if(host == 0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}
	
	bzero((char*)&host_addr,sizeof(host_addr));
	host_addr.sin_port=htons(port);
	host_addr.sin_family=AF_INET;
	bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);

	hostSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	newhostSocket = connect(hostSocket,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
	if(newhostSocket<0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}

	bzero((char*)buffer,sizeof(buffer));
	if(temp!=NULL) {
		sprintf(buffer,"POST /%s %s",temp,v_http);
	}
	else {
		sprintf(buffer,"POST / %s",v_http);
	}
	strcat(buffer, requestBody);
	
	n = send(hostSocket,buffer,strlen(buffer),0);
		
	if(n<0) {
		countError[0]++;
		send(clientSocket,"<html><head>520 Unknown Error</head><body><h1>HTTP Error: 520 Unknown Error</h1><p>The server is returning unknown error.</p></body></html>",139,0);
	}
	else {
		do {
			bzero((char*)buffer,500);
			n=recv(hostSocket,buffer,500,0);
			if(!(n<=0))
				send(clientSocket,buffer,n,0);
			} while(n>0);
			countProcessed[0]++;
		}
}

void HTTPServer::httpHEAD(char buffer[2048], char *url, char *method, char *v_http) {
	printf("HEAD %s\n", url);

	struct sockaddr_in host_addr;
	int newhostSocket,n,port=80,hostSocket;
	struct hostent* host;
	char* temp=NULL;
	
	url = strstr(url, "://")+3;
	for(n = 0; n<strlen(url); ++n) {
		if(url[n] == '/') {
			temp = strstr(url, "/")+1;
			break;
		}
	}
	url = strtok(url,"/");

	host = gethostbyname(url);
	if(host == 0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}

	bzero((char*)&host_addr,sizeof(host_addr));
	host_addr.sin_port = htons(port);
	host_addr.sin_family = AF_INET;
	bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
	hostSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	newhostSocket = connect(hostSocket,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
	
	if(newhostSocket<0) {
		countError[0]++;
		send(clientSocket,"<html><head>404 Not Found</head><body><h1>HTTP Error: 404 Not Found</h1><p>The requested url was not found.</p></body></html>",125,0);
		return;
	}
	
	bzero((char*)buffer,sizeof(buffer));
	if(temp != NULL) {
		sprintf(buffer,"HEAD /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
	}
	else {
		sprintf(buffer,"HEAD / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
	}

	n = send(hostSocket,buffer,strlen(buffer),0);
	
	if(n<0) {
		countError[0]++;
		send(clientSocket,"<html><head>520 Unknown Error</head><body><h1>HTTP Error: 520 Unknown Error</h1><p>The server is returning unknown error.</p></body></html>",139,0);
	}
	else {
		do {
			bzero((char*)buffer,500);
			n=recv(hostSocket,buffer,500,0);
			if(!(n<=0))
				send(clientSocket,buffer,n,0);
		} while(n>0);
		countProcessed[0]++;
	}

	return;
}

int main(int argc,char* argv[]) {
	cout << "\n\nYAHS: Yet Another HTTP-proxy Server\n\n";
	httpServer.runServer(atoi(argv[1]), argc, argv);
	return 0;
}
