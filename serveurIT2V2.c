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
#include <pthread.h>

socklen_t lgA=sizeof(struct sockaddr_in);

struct socketClient{
	int socketC;
	struct sockaddr_in adresseC;
	pthread_t thread;
	char pseudo[40];
};

struct socketClient tabSocketClient[100];

int nombreClient;

 /*On définit à 100 le nombre max de client qui peuvent se connecter*/

void *clientVersAutre(int i){
	int j;
	int k;
	while(1){

		    char msg[50];
	
			int resR1 = recv(tabSocketClient[i].socketC,msg,sizeof(msg),0);
			/*On recoit le message du client 1*/
			if(resR1==-1){
				perror("Erreur de reception du message du client 1");
				pthread_exit(NULL);
			}
			else if(resR1==0){
				perror("Socket fermé du client 1");
				pthread_exit(NULL);
			}
			if(strcmp(msg,"fin\n")==0){
				for(k=0;k<nombreClient;k++){
					send(tabSocketClient[k].socketC,msg,strlen(msg)+1,0);
					
					if(k!=i){
						pthread_cancel(tabSocketClient[k].thread);

					}
				
					
				  	/*pthread_cancel(tabSocketClient[k].thread);*/
				}
				printf("Les clients sont déconnectés");
				pthread_exit(NULL);
			}

			/*Si le message est "fin" alors le serveur s'occupe de fermer les deux sockets clients*/

			char message[60]="";

			strcat(message,"Message recu de ");
			strcat(message,tabSocketClient[i].pseudo);
			strcat(message," : ");
			strcat(message,msg);



			printf("Message recu du Client 1: %s",message);
			for(j=0;j<nombreClient;j++){
				if(j!=i){
					int resS1 = send(tabSocketClient[j].socketC,message,strlen(message)+1,0);

					if(resS1==-1){
					perror("Erreur d'envoie du message pour les clients ");
					pthread_exit(NULL);
					}
				}
			}
			/*le serveur envoie le message du client 1 au client 2*/
	}
}


int main(int argc,char* argv[]){

	nombreClient = atoi(argv[2]); /*On définit le nombre de client avec l'argument 2*/
	int i;
	int k;
	int l;
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
	
	printf("on est avant le while\n");

	/*On a bind le serveur pour permettre aux clients de se connecter à lui et on initialise 2 structures sockaddr_in pour les paires de clients qui vont se connecter*/

	while(1){
		/*Ici on rentre dans un while(1) pour permettre au serveur de rester allumer même lors de la déconnection des clients*/

		for(k=0;k<nombreClient;k++){
			char pseudo[40];
			tabSocketClient[k].socketC = accept(dSocket,(struct sockaddr *) &(tabSocketClient[k].adresseC),&lgA);
			int resR1 = recv(tabSocketClient[k].socketC,tabSocketClient[k].pseudo,sizeof(tabSocketClient[k].pseudo),0);
		}
		puts("tout le monde est co");
		for(i=0;i<nombreClient;i++){
			puts("tout le monde est co2");
			if(pthread_create(&(tabSocketClient[i].thread),NULL,(void*)&clientVersAutre,i)==-1){
				perror("erreur dans la création du thread 1");
			return EXIT_FAILURE;
			}
			puts("tout le monde est co3");
		}

		printf("cest le print de votre vie0");
		int ret= pthread_join(tabSocketClient[0].thread,NULL);
		printf("cest le print de votre vie1");
		for(l=0;l<nombreClient;l++){
			close(tabSocketClient[l].socketC);
		}
		printf("cest le print de votre vie");
	}


	close(dSocket);
	return EXIT_SUCCESS;
}
