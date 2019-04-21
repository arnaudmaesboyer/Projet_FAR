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

int dSocketClient1;
int dSocketClient2;
/*Ces variables globales sont à la fois utilisés dans le main et dans les fonctions thread*/

void *c1versc2(void *arg){
	while(1){
		   	char msg[50];
			int resR1 = recv(dSocketClient1,msg,sizeof(msg),0);
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
				send(dSocketClient2,msg,strlen(msg)+1,0);
				send(dSocketClient1,msg,strlen(msg)+1,0);
				printf("Les deux clients sont déconnectés");
				close(dSocketClient2);
				close(dSocketClient1);
			  	pthread_exit(NULL);
			}
			/*Si le message est "fin" alors le serveur s'occupe de fermer les deux sockets clients*/

			printf("Message recu du Client 1: %s",msg);
			int resS1 = send(dSocketClient2,msg,strlen(msg)+1,0);
			if(resS1==-1){
				perror("Erreur d'envoie du message pour le client 2");
				pthread_exit(NULL);
			}
			/*le serveur envoie le message du client 1 au client 2*/

	}
}


void *c2versc1(void *arg){
 	 while(1){
            char msg[50];

			int resR2 = recv(dSocketClient2,msg,sizeof(msg),0);
			if(resR2==-1){
				perror("Erreur de reception du message du client 2");
				pthread_exit(NULL);
			}
			else if(resR2==0){
				perror("Socket fermé du client 2");
				pthread_exit(NULL);
			}
			/*le serveur recoit le message du client 2*/

			if(strcmp(msg,"fin\n")==0){
				send(dSocketClient2,msg,strlen(msg)+1,0);
				send(dSocketClient1,msg,strlen(msg)+1,0);
				printf("Les deux clients sont déconnectés");
				close(dSocketClient2);
				close(dSocketClient1);
			  	pthread_exit(NULL);
		
			}
			/*Si le message est "fin" alors le serveur s'occupe de fermer les deux sockets clients*/

			printf("Message recu du Client 2: %s",msg);
			int resS2 = send(dSocketClient1,msg,strlen(msg)+1,0);
			if(resS2==-1){
				perror("Erreur d'envoie du message pour le client 1");
				pthread_exit(NULL);
			}
			/*le serveur envoie le message du client 2 au client 1*/
  }
	
}

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
	printf("on est avant le while\n");

	/*On a bind le serveur pour permettre aux clients de se connecter à lui et on initialise 2 structures sockaddr_in pour les paires de clients qui vont se connecter*/
	
	pthread_t client1;
	pthread_t client2;

	while(1){
		/*Ici on rentre dans un while(1) pour permettre au serveur de rester allumer même lors de la déconnection des clients*/
		dSocketClient1 = accept(dSocket,(struct sockaddr *) &adClient1,&lgA1);

		dSocketClient2 = accept(dSocket,(struct sockaddr *) &adClient2,&lgA2);
		/*On met en place deux sockets clients pour permettre l'échange de message bien structuré*/
		
		if(pthread_create(&client1,NULL,(void*)&c1versc2,NULL)==-1){
			perror("erreur dans la création du thread 1");
			return EXIT_FAILURE;
		}
		if(pthread_create(&client2,NULL,(void*)&c2versc1,NULL)==-1){
			perror("erreur dans la création du thread 2");
			return EXIT_FAILURE;
		}
		/*On crée les deux fonctions thread permettant de soit envoyer du Client 1 vers le client 2 soit l'inverser*/
		
	}

	int ret= pthread_join(client1,NULL);
	int ret2= pthread_join(client2,NULL);

	close(dSocket);
	return EXIT_SUCCESS;
}
