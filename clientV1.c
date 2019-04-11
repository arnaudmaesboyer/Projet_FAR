#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char* argv[]){

	int dSock=socket(PF_INET,SOCK_STREAM,0);
	if (dSock<0){
		perror("Problème de création de socket");
		exit(0);
	}
	struct sockaddr_in adServ;
	adServ.sin_family=AF_INET;
	inet_pton(AF_INET,"162.38.111.88",&(adServ.sin_addr));
	adServ.sin_port=htons(atoi(argv[1])); /* L'argument 1 est le port du serveur */ 
	socklen_t lgA = sizeof(struct sockaddr_in);
	int resC = connect(dSock,(struct sockaddr*)&adServ,lgA);
	if (resC==-1){
		perror("Erreur de connection au serveur");
		close(dSock);
		return 0;
	}

	while(1){
		if(atoi(argv[2])==1){ /*L'argument 2 est le numéro du client soit 1 pour le client 1 et 2 pour le client 2 */
			/*Ici nous sommes dans le traitement pour le client 1, celui ci envoie un message écrit dans le terminal et l'envoie au serveur */
			char buffer[50];
			fgets(buffer,50,stdin);

			int resS = send(dSock,buffer,strlen(buffer)+1,0);
			if(resS==-1){
				perror("Erreur dans l'envoie du message du client 1 vers le serveur");
				return 0;
			}
			if(resS<strlen(buffer)+1){
				perror("Erreur dans l'envoie du message du client 2 vers le serveur --> Le message n'a pas été envoyé entièrement");
				return 0;
			}
			/*Puis il reçoit le message du client 2 via le serveur et l'affiche*/
			int resR = recv(dSock,&buffer,sizeof(buffer),0);
			if(resR==0){
				perror("Le client 1 est fermé");
				close(dSock);
				return 0;
			}
			else if(resR==-1){
				perror("Erreur dans la réception du message du client 2");
				close(dSock);
				return 0;
			}
			printf("%s",buffer);
		}
		else if(atoi(argv[2])==2){
			/*Ici c'est le traitement du client 2, celui ci reçoit d'abord le premier message puis en écrit un et l'envoi */
			char buffer[50];

			int resR = recv(dSock,&buffer,sizeof(buffer),0);
			if(resR==0){
				perror("Le client 2 est fermé");
				close(dSock);
				return 0;
			}
			else if(resR==-1){
				perror("Erreur dans la réception du message du client 1");
				close(dSock);
				return 0;
			}
			printf("%s",buffer);
	
			fgets(buffer,50,stdin);
			
			int resS = send(dSock,buffer,strlen(buffer)+1,0);
			if(resS==-1){
				perror("Erreur dans l'envoie du message du client 2 vers le serveur");
				return 0;
			}
			if(resS<strlen(buffer)+1){
				perror("Erreur dans l'envoie du message du client 2 vers le serveur --> Le message n'a pas été envoyé entièrement");
				return 0;
			}	
		}
	}	

	close(dSock);
	return 0;
}
