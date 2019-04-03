#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

/*test test*/
int main(int argc,char* argv[]){
	int dSocket = socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = INADDR_ANY;
	adr.sin_port = htons(atoi(argv[1]));

	int res = bind(dSocket,(struct sockaddr*) &adr,sizeof(adr));
	if(res==-1){
		perror("erreur de nommage");
		exit(0);
	}
	int resL = listen(dSocket,5);
	if(resL==-1){
		perror("erreur dans l'attente de connection");
		exit(0);
	}
	struct sockaddr_in adClient1;
	socklen_t lgA1=sizeof(struct sockaddr_in);
	struct sockaddr_in adClient2;
	socklen_t lgA2=sizeof(struct sockaddr_in);
	int i =0;

	while(1){
		printf("on passe par le while\n");

		int dSocketClient1 = accept(dSocket,(struct sockaddr *) &adClient1,&lgA1);
		int dSocketClient2 = accept(dSocket,(struct sockaddr *) &adClient2,&lgA2);
			char msg[50];	
			if(i%2==0){	
				
				int resR = recv(dSocketClient1,msg,sizeof(msg),0);
				if(resR==-1){
					perror("erreur de reception");
					exit(0);
				}
				else if(resR==0){
					perror("socket fermé");
					exit(0);
				}
				printf("Message recu : %s",msg);
				int resS = send(dSocketClient2,msg,sizeof(msg),0);
				if(resS==-1){
					perror("erreur d'envoie");
					exit(0);
				}
			}
			else 
			{
				int resR = recv(dSocketClient2,msg,sizeof(msg),0);
				if(resR==-1){
					perror("erreur de reception");
					exit(0);
				}
				else if(resR==0){
					perror("socket fermé");
					exit(0);
				}
				printf("Message recu : %s",msg);
				int resS = send(dSocketClient1,msg,sizeof(msg),0);
				if(resS==-1){
					perror("erreur d'envoie");
					exit(0);
				}
			}
			i++;
			close(dSocketClient1);

		
	}
	close(dSocket);
	exit(0);

}
