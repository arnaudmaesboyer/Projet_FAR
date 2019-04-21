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
/*On définit une structure permettant de stocker toutes les informations de chaque client connectés comme
son pseudo, son adresse, son socket et le thread utilisé par le serveur pour ce client*/

struct socketClient tabSocketClient[100];
/*On crée ensuite un tableau qui contiendra toutes les informations pour chaque client*/

int nombreClient;

 /*On définit à 100 le nombre max de client qui peuvent se connecter*/

void *clientVersAutre(int i){
	/*Dans cette version nous avons donc qu'une seule fonction de thread qui permet d'envoyer un message
	à tous les clients sauf celui qui vient d'envoyer le message*/
	int j;
	int k;
	while(1){

		  	char msg[50];
			int resR1 = recv(tabSocketClient[i].socketC,msg,sizeof(msg),0);
			/*On recoit le message du client i*/
			if(resR1==-1){
				perror("Erreur de reception du message du client");
				pthread_exit(NULL);
			}
			else if(resR1==0){
				perror("Socket fermé du client");
				pthread_exit(NULL);
			}
			if(strcmp(msg,"fin\n")==0){
				for(k=0;k<nombreClient;k++){
					send(tabSocketClient[k].socketC,msg,strlen(msg)+1,0);
					/*On envoie le message fin à chaque client (même celui qui a envoyé 'fin') pour qu'ils
					ferment eux-même leur socket et leurs threads*/
					if(k!=i){
						pthread_cancel(tabSocketClient[k].thread);
					}
					/*Et on arrete les threads des clients 1 par 1*/
				
				}
				printf("Les clients sont déconnectés");
				pthread_exit(NULL);
			}

			char message[60]="";

			strcat(message,"Message recu de ");
			strcat(message,tabSocketClient[i].pseudo);
			strcat(message," : ");
			strcat(message,msg);
			/*On s'occupe ici de changer le message reçu du client pour y mettre devant le pseudo de celui-ci*/
			
			printf("%s",message);
			for(j=0;j<nombreClient;j++){
				if(j!=i){
					int resS1 = send(tabSocketClient[j].socketC,message,strlen(message)+1,0);
					if(resS1==-1){
					perror("Erreur d'envoie du message pour les clients ");
					pthread_exit(NULL);
					}
					/*Et on envoie à tout le monde sauf au client qui a envoyé le message précédemment*/
				}
			}
	}
}


int main(int argc,char* argv[]){

	nombreClient = atoi(argv[2]); /*On définit le nombre de client qui peuvent se connecter simultanément avec l'argument 2*/
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
	int resL = listen(dSocket,nombreClient);
	if(resL==-1){
		perror("Erreur dans l'attente de connection des clients");
		return 0;
	}

	/*On a bind le serveur pour permettre aux clients de se connecter et on le met en listen avec le nombreMax de clients possible*/

	while(1){
		/*Ici on rentre dans un while(1) pour permettre au serveur de rester allumer même lors de la déconnection des clients*/

		for(k=0;k<nombreClient;k++){
			char pseudo[40];
			tabSocketClient[k].socketC = accept(dSocket,(struct sockaddr *) &(tabSocketClient[k].adresseC),&lgA);
			int resR1 = recv(tabSocketClient[k].socketC,tabSocketClient[k].pseudo,sizeof(tabSocketClient[k].pseudo),0);
		}
		/*Cette boucle for permet d'accepter la connection des clients au serveur et de recevoir leur premier message qui
		sera leur pseudo, on le stocke donc dans l'attribut pseudo de notre structure socketClient*/
		
		puts("Tout le monde est connecté");
		
		for(i=0;i<nombreClient;i++){
			if(pthread_create(&(tabSocketClient[i].thread),NULL,(void*)&clientVersAutre,i)==-1){
				perror("erreur dans la création du thread 1");
			return EXIT_FAILURE;
			}
		}
		puts("Création des threads terminés");
		/*Cette boucle permet de créer les threads pour chaque client connecté*/
		
		int ret= pthread_join(tabSocketClient[0].thread,NULL);
		/*On fait cette fonction qui permet au main d'attendre qu'un thread soit fini (en sachant que dans notre 
		code, si un thread est terminé alors tous les autres threads clients se terminent)*/
		
		for(l=0;l<nombreClient;l++){
			close(tabSocketClient[l].socketC);
		}
		/*Et on ferme tous les sockets clients*/
	}

	close(dSocket);
	return EXIT_SUCCESS;
}
