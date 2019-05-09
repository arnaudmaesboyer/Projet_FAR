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
#include <pthread.h>
#include <signal.h>

int dSock;
pthread_t env;
pthread_t recep;
/*Variable globales utilisés dans le main et les fonctions threads*/

void *envoie(void *arg){
	/*Comme dans la V1 nous sommes dans le thread permettant d'envoyer un message au serveur*/
	char buffer[50];
		while(1){
			fgets(buffer,50,stdin);
			int resS = send(dSock,buffer,strlen(buffer)+1,0);
			if(resS==-1){
				perror("Erreur dans l'envoie du message du client 1 vers le serveur");
				pthread_exit(NULL);
			}
			if(resS<strlen(buffer)+1){
				perror("Erreur dans l'envoie du message du client 2 vers le serveur --> Le message n'a pas été envoyé entièrement");
				pthread_exit(NULL);
			}
			
		}
	pthread_exit(NULL);
}

void *reception(void *arg){
	/*Comme dans la V1 nous sommes dans le thread permettant la réception des messages*/
	char buffer[50];
	while(1){
			int resR = recv(dSock,&buffer,sizeof(buffer),0);
			if(resR==0){
				perror("Le client 1 est fermé");
				pthread_exit(NULL);
			}
			else if(resR==-1){
				perror("Erreur dans la réception du message du client 2");
				pthread_exit(NULL);
			}
			printf("%s",buffer);

			if(strcmp(buffer,"fin\n")==0){
				break;
			}
		}
	pthread_cancel(env);
	pthread_cancel(recep);
	pthread_exit(NULL);

}

int main(int argc,char* argv[]){

	dSock=socket(PF_INET,SOCK_STREAM,0);
	if (dSock<0){
		perror("Problème de création de socket");
		exit(0);
	}
	struct sockaddr_in adServ;
	adServ.sin_family=AF_INET;
	inet_pton(AF_INET,"162.38.111.27",&(adServ.sin_addr));
	adServ.sin_port=htons(atoi(argv[1])); /* L'argument 1 est le port du serveur */ 
	socklen_t lgA = sizeof(struct sockaddr_in);
	int resC = connect(dSock,(struct sockaddr*)&adServ,lgA);
	if (resC==-1){
		perror("Erreur de connection au serveur");
		close(dSock);
		return 0;
	}
	/*On a configurer la connection au serveur en créant les sockets et en se connectant*/
	char pseudo[40];
	strcpy(pseudo,argv[2]);
	send(dSock,pseudo,strlen(pseudo)+1,0);
	/*On définit un pseudo passé en paramètre du terminal ex : ./client 40002 Jean*/
	/*Et on l'envoie au serveur pour qu'il sache comment on s'appelle*/
		
	if(pthread_create(&env,NULL,(void*)&envoie,NULL)==-1){
		perror("erreur dans la création du thread 1");
		return EXIT_FAILURE;
	}
	if(pthread_create(&recep,NULL,(void*)&reception,NULL)==-1){
		perror("erreur dans la création du thread 2");
		return EXIT_FAILURE;
	}
	/*Comme dans la V1 on crée les threads envoie et réception*/
	
	int ret= pthread_join(env,NULL);
	int ret2= pthread_join(recep,NULL);
	/*Et on attend que ces threads soient terminés avant de finir l'exécution du main*/

	close(dSock);
	return EXIT_SUCCESS;
}
