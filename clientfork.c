#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char* argv[]){

	int dSock=socket(PF_INET,SOCK_STREAM,0);
	if (dSock<0){
		perror("pb de socket");
		exit(0);
	}
	struct sockaddr_in adServ;
	adServ.sin_family=AF_INET;
	inet_pton(AF_INET,"162.38.111.7",&(adServ.sin_addr));
	adServ.sin_port=htons(atoi(argv[1]));
	socklen_t lgA = sizeof(struct sockaddr_in);
	int resC = connect(dSock,(struct sockaddr*)&adServ,lgA);
	if (resC==-1){
		perror("erreur de connection");
		return 0;
	}
	printf("on est avant le while client");
	while(1){
		if(atoi(argv[2])==1){
			printf("on passe dans le if client 1");
			char buffer[50];
			fgets(buffer,50,stdin);
			
			char message[100];

			int resS = send(dSock,buffer,strlen(buffer)+1,0);
			if(strcmp(buffer,"fin")==0){
				break;
			}
		
			int resR = recv(dSock,&message,sizeof(char)*100,0);
			if(resR==0){
				perror("erreur de reception");
				return 0;
			}
			printf("%d\n",message);
		}

		else{
			printf("on passe dans le if client 2");
			char message[100];
			int resR = recv(dSock,&message,sizeof(char)*100,0);
			if(resR==0){
				perror("erreur de reception");
				return 0;
			}

			char buffer[50];
			fgets(buffer,50,stdin);
			
			int resS = send(dSock,buffer,strlen(buffer)+1,0);
			if(strcmp(buffer,"fin")==0){
				break;
			}
		
			printf("%d\n",message);
		}
	}	

	close(dSock);
	return 0;
}
