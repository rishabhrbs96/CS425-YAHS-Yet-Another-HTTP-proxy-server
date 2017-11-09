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


	// dev's code start //

int pid;
static int counter=0;
for(;;)
	{
		if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t*)&addresslength))<0) {
			printf("Problem in accepting connection");
			exit(1);	
		}

		if((pid = fork()) == -1){
			close(clientSocket);
			continue;
		}
		else if(pid > 0){
			close(clientSocket);
			continue;
		}
		else if(pid==0){
		
		    std::cout << "DEBUG: process " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
			acceptRequest();

		// close(clientSocket);
		}

	}


	// dev's code end //

	acceptRequest();
	close(serverSocket);

}

void HTTPServer::acceptRequest() {

	while(1) {
		char httprequest[510],buffer[510];
		char *ts, *method, *url, *v_http;

		// if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t*)&addresslength))<0) {
		// 	printf("Problem in accepting connection");
		// 	exit(1);	
		// }
		
		bzero((char*)httprequest,500);
		recv(clientSocket,httprequest,500,0);
		printf("%s\n", httprequest);
		
		
		strcpy(buffer,httprequest);
		ts = strtok(httprequest," ");
		method = ts;
		ts = strtok(NULL," ");
		url = ts;
		ts = strtok(NULL,"\n");
		v_http = ts;
		
		if((strncmp(v_http,"HTTP/1.1",8) != 0) && ( strncmp(v_http,"HTTP/1.0",8) != 0)) {
			printf("Error http 1.0 / 1.1 requests only\n");
			continue;
		}
		/*if(strncmp(url,"http://",7) != 0) {
			printf("Error https not supported\n");
			continue;
		}*/

		v_http[8] = '\0';

//////////


///////////////
		if(strcmp(method,"GET") == 0) {
			this->httpGET(buffer,url,method,v_http);
		} else if(strcmp(method,"POST") == 0) {
			this->httpPOST(buffer,url,method,v_http);
		} else if(strcmp(method,"HEAD") == 0) {
			this->httpHEAD(buffer,url,method,v_http);
		} else {
			printf("Error invalid http request");
			continue;
		}
	}
}

void HTTPServer::httpGET(char buffer[510], char *url, char *method, char *v_http) {
	printf("GET method called :: %s\n", url);
	
	struct sockaddr_in host_addr;
		int flag=0,newsockfd1,n,port=0,i,sockfd1;
		struct hostent* host;
			char t1[300],t2[300],t3[10];
	char* temp=NULL;
	   
	sscanf(buffer,"%s %s %s",t1,t2,t3);

		flag=0;
		 
		for(i=7;i<strlen(t2);i++) {
			if(t2[i]==':') {
				flag=1;
				break;
			}
		}
   
		temp=strtok(t2,"//");

		if(flag==0) {
			port=80;
			temp=strtok(NULL,"/");
		} else {
			temp=strtok(NULL,":");
		}
   		
		sprintf(t2,"%s",temp);
		
		//printf("host = %s",t2);
		

		host=gethostbyname(t2);
   		
		if(flag==1) {
			temp=strtok(NULL,"/");
			port=atoi(temp);
		}

    	strcat(t1,"^]");
		temp=strtok(t1,"//");
		temp=strtok(NULL,"/");
		if(temp!=NULL)
			temp=strtok(NULL,"^]");
		//printf("\npath = %s\nPort = %d\n",temp,port);
   		printf("@@@@@@@@@@@@@@@ %s\n",t1);
   		printf("@@@@@@@@@@@@@@@ %s\n",t2);
   		printf("@@@@@@@@@@@@@@@ %s\n",t3);
   		printf("@@@@@@@@@@@@@@@ %s\n",method);
   		//char *rtemp = strtok(url,"//");
   		//url = strtok(NULL,"//");
   		char *cptr;
	
		char substr[] = "://";
		char sub2[] = "/";
		url = strstr(url, substr)+3;
		printf("%s\n", url);
		
		temp = strstr(url, "/")+1;
		
		url = strtok(url,"/");
		printf("%s\n", url);
		printf("%s\n", temp);

   		printf("@@@@@@@@@@@@@@@ %s\n",url);
   		printf("@@@@@@@@@@@@@@@ %s\n",temp);

   		//printf("<<<<<<<<<<<<<<< %s %s %s\n",url,v_http,method);
   		
   		//continue;
		bzero((char*)&host_addr,sizeof(host_addr));
		host_addr.sin_port=htons(port);
		host_addr.sin_family=AF_INET;
		bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
		   
		sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
		//sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
		if(newsockfd1<0)
			printf("Error in connecting to remote server");
   
		//printf("\n%s\n",buffer);
		//send(newsockfd,buffer,strlen(buffer),0);
		char b2[510];
		bzero((char*)buffer,sizeof(buffer));
				bzero((char*)b2,sizeof(b2));

		if(temp!=NULL) {
			//sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
			sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
			sprintf(b2,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
		}
		else {
			//sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);
			sprintf(b2,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
			sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
			
		//	exit(0);
		}
 		int ris;
 		

 		
 		printf("final request %s\n", buffer);
 		
// 		exit(0);

		n=send(sockfd1,buffer,strlen(buffer),0);
		/*printf("************exiting\n************exiting\n\n%s\n",buffer);
printf("************temp \n************exiting\n\n%s\n",temp);
printf("************t2 \n************exiting\n\n%s\n",t2);
printf("************t3 \n************exiting\n\n%s\n",t3);
printf("************final\n");*/

		
		if(n<0)
			printf("Error writing to socket");
		else {
			do {

				bzero((char*)buffer,500);

				n=recv(sockfd1,buffer,500,0);
				//printf("*************** %s\n",buffer);

				if(!(n<=0))
				send(clientSocket,buffer,n,0);
				//printf("%s",buffer);
			} while(n>0);
		}

		//continue;


}

void HTTPServer::httpPOST(char buffer[510], char *url, char *method, char *v_http) {
	printf("POST method called :: %s\n", url);
	char *sss = strstr(buffer,"\n");
		printf("%s\n", sss);
	struct sockaddr_in host_addr;
	int flag=0,newsockfd1,n,port=0,i,sockfd1;
	struct hostent* host;
	char t1[300],t2[300],t3[10];
	char* temp=NULL;
	   
	sscanf(buffer,"%s %s %s",t1,t2,t3);

		flag=0;
		 
		for(i=7;i<strlen(t2);i++) {
			if(t2[i]==':') {
				flag=1;
				break;
			}
		}
   
		temp=strtok(t2,"//");

		if(flag==0) {
			port=80;
			temp=strtok(NULL,"/");
		} else {
			temp=strtok(NULL,":");
		}
   		
		sprintf(t2,"%s",temp);
		
		//printf("host = %s",t2);
		

		host=gethostbyname(t2);
   		
		if(flag==1) {
			temp=strtok(NULL,"/");
			port=atoi(temp);
		}

    	strcat(t1,"^]");
		temp=strtok(t1,"//");
		temp=strtok(NULL,"/");
		if(temp!=NULL)
			temp=strtok(NULL,"^]");
		
		/*
		printf("1 %s\n", url);

   		char *rtemp = strtok(url,":");
   		printf("2 %s\n", url);
   		printf("%s\n", rtemp);
   		url = strtok(NULL,":");
   		
   		printf("3 %s\n", url);
   		printf("%s\n", rtemp);
   		*/
   		char *cptr;
	
		char substr[] = "://";
		char sub2[] = "/";
		url = strstr(url, substr)+3;
		printf("%s\n", url);
		
		temp = strstr(url, "/")+1;
		
		url = strtok(url,"/");
		printf("%s\n", url);
		printf("%s\n", temp);
		
		
		//return;
		bzero((char*)&host_addr,sizeof(host_addr));
		host_addr.sin_port=htons(port);
		host_addr.sin_family=AF_INET;
		bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
		   
		sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
		//sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
		if(newsockfd1<0)
			printf("Error in connecting to remote server");
   
		//printf("\n%s\n",buffer);
		//send(newsockfd,buffer,strlen(buffer),0);
		char b2[510];
		bzero((char*)buffer,sizeof(buffer));
				bzero((char*)b2,sizeof(b2));

		/*if(temp!=NULL) {
			sprintf(buffer,"POST /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
		}
		else {
			sprintf(buffer,"POST / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
			
		//	exit(0);
		}
 		printf("%s\n", buffer);*/
		if(temp!=NULL) {
			sprintf(buffer,"POST /%s %s",temp,v_http);
		}
		else {
			sprintf(buffer,"POST / %s",v_http);
			
		//	exit(0);
		}

 		strcat(buffer, sss);
 		printf("vioooolllaaa\n%s\n", buffer);
 		
 		
 		
 		n=send(sockfd1,buffer,strlen(buffer),0);
		
		
		if(n<0)
			printf("Error writing to socket");
		else {
			do {

				bzero((char*)buffer,500);

				n=recv(sockfd1,buffer,500,0);
				//printf("*************** %s\n",buffer);

				if(!(n<=0))
				send(clientSocket,buffer,n,0);
				//printf("%s",buffer);
			} while(n>0);
		}
}

void HTTPServer::httpHEAD(char buffer[510], char *url, char *method, char *v_http) {
	printf("HEAD method called :: %s\n", url);
	struct sockaddr_in host_addr;
	int flag=0,newsockfd1,n,port=0,i,sockfd1;
	struct hostent* host;
	char t1[300],t2[300],t3[10];
	char* temp=NULL;
	   
	sscanf(buffer,"%s %s %s",t1,t2,t3);

		flag=0;
		 
		for(i=7;i<strlen(t2);i++) {
			if(t2[i]==':') {
				flag=1;
				break;
			}
		}
   
		temp=strtok(t2,"//");

		if(flag==0) {
			port=80;
			temp=strtok(NULL,"/");
		} else {
			temp=strtok(NULL,":");
		}
   		
		sprintf(t2,"%s",temp);
		
		//printf("host = %s",t2);
		

		host=gethostbyname(t2);
   		
		if(flag==1) {
			temp=strtok(NULL,"/");
			port=atoi(temp);
		}

    	strcat(t1,"^]");
		temp=strtok(t1,"//");
		temp=strtok(NULL,"/");
		if(temp!=NULL)
			temp=strtok(NULL,"^]");
		
   		char *rtemp = strtok(url,"//");
   		url = strtok(NULL,"//");
   		

		bzero((char*)&host_addr,sizeof(host_addr));
		host_addr.sin_port=htons(port);
		host_addr.sin_family=AF_INET;
		bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
		   
		sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
		//sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
		if(newsockfd1<0)
			printf("Error in connecting to remote server");
   
		//printf("\n%s\n",buffer);
		//send(newsockfd,buffer,strlen(buffer),0);
		char b2[510];
		bzero((char*)buffer,sizeof(buffer));
				bzero((char*)b2,sizeof(b2));

		if(temp!=NULL) {
			sprintf(buffer,"HEAD /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,v_http,url);
		}
		else {
			sprintf(buffer,"HEAD / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v_http,url);
			
		//	exit(0);
		}
 		
 		n=send(sockfd1,buffer,strlen(buffer),0);
		
		
		if(n<0)
			printf("Error writing to socket");
		else {
			do {

				bzero((char*)buffer,500);

				n=recv(sockfd1,buffer,500,0);
				//printf("*************** %s\n",buffer);

				if(!(n<=0))
				send(clientSocket,buffer,n,0);
				//printf("%s",buffer);
			} while(n>0);
		}
}

int main(int argc,char* argv[]) {
	cout << "\n\nYAHS: Yet Another HTTP-proxy Server\n\n";
	HTTPServer httpServer;
	httpServer.runServer(atoi(argv[1]));
	return 0;
}
