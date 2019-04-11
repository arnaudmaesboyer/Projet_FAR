#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc,char* argv[]){
	int dSocket = socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = INADDR_ANY;
	adr.sin_port = htons(atoi(argv[1]));

	int res = bind(dSocket,(struct sockaddr*) &adr,sizeof(adr));
	if(res==-1){
		perror("Erreur de nommage de la socket serveur");
		return 0;
	}
	int resL = listen(dSocket,5);
	if(resL==-1){
		perror("Erreur dans l'attente de connection des clients");
		return 0;
	}
	struct sockaddr_in adClient1;
	socklen_t lgA1=sizeof(struct sockaddr_in);
	struct sockaddr_in adClient2;
	socklen_t lgA2=sizeof(struct sockaddr_in);


	while(1){

		int dSocketClient1 = accept(dSocket,(struct sockaddr *) &adClient1,&lgA1);
		int dSocketClient2 = accept(dSocket,(struct sockaddr *) &adClient2,&lgA2);
		char msg[50];
		while(1){
					
			int resR1 = recv(dSocketClient1,msg,sizeof(msg),0);
			if(resR1==-1){
				perror("Erreur de reception du message du client 1");
				return 0;
			}
			else if(resR1==0){
				perror("Socket fermé du client 1");
				return 0;
			}
			if(strcmp(msg,"fin\n")==0){
				printf("Les deux clients sont déconnectés");
				close(dSocketClient2);
				close(dSocketClient1);
				break;
			}

			printf("Message recu du Client 1: %s",msg);
			int resS1 = send(dSocketClient2,msg,strlen(msg)+1,0);
			if(resS1==-1){
				perror("Erreur d'envoie du message pour le client 2");
				return 0;
			}
			
			int resR2 = recv(dSocketClient2,msg,sizeof(msg),0);
			if(resR2==-1){
				perror("Erreur de reception du message du client 2");
				return 0;		
			}
			else if(resR2==0){
				perror("Socket fermé du client 2");
				return 0;	
			}

			if(strcmp(msg,"fin\n")==0){
				printf("Les deux clients sont déconnectés");
				close(dSocketClient2);
				close(dSocketClient1);
				break;
			}

			printf("Message recu du Client 2: %s",msg);
			int resS2 = send(dSocketClient1,msg,strlen(msg)+1,0);
			if(resS2==-1){
				perror("Erreur d'envoie du message pour le client 1");
				return 0;
			}
		}
	}
	close(dSocket);
	return 0;
}
