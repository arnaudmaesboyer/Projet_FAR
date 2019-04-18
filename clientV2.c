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

void *envoie(void *arg){
	puts("salut envoie");
	char buffer[50];
		while(1){

			/*Ici nous sommes dans le traitement pour le client 1, celui ci envoie un message écrit dans le terminal et l'envoie au serveur */

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
			/*if(strcmp(buffer,"fin\n")==0){
				break;
			}*/
		}
	/*pthread_cancel(env);
	pthread_cancel(recep);*/
	pthread_exit(NULL);
}

void *reception(void *arg){
	puts("salut reception");
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
		
	printf("on est apres la décla\n");

	if(pthread_create(&env,NULL,(void*)&envoie,NULL)==-1){
		perror("erreur dans la création du thread 1");
		return EXIT_FAILURE;
	}
	if(pthread_create(&recep,NULL,(void*)&reception,NULL)==-1){
		perror("erreur dans la création du thread 2");
		return EXIT_FAILURE;
	}
	puts("on a fini");
	
	int ret= pthread_join(env,NULL);
	int ret2= pthread_join(recep,NULL);

	close(dSock);
	return EXIT_SUCCESS;
}
