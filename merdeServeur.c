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


struct salon{
	int numSalon;
	struct socketClient tabSocketClient[100];
	int nbClient;
};


struct salon tabSalon[10];


int nombreClient;

struct envoiClient{
	struct salon salonClient;
	int numClient;
};
	
int dSocket;

 /*On définit à 100 le nombre max de client qui peuvent se connecter*/

void *clientVersAutre(struct envoiClient envoi){
	/*Dans cette version nous avons donc qu'une seule fonction de thread qui permet d'envoyer un message
	à tous les clients sauf celui qui vient d'envoyer le message*/
	int j;
	int k;
	while(1){

		puts("cest bon 9");

		  	char msg[50];
			int resR1 = recv(envoi.salonClient.tabSocketClient[envoi.numClient].socketC,msg,sizeof(msg),0);
			/*On recoit le message du client i*/
		puts("cest bon 10");

			if(resR1==-1){
				perror("Erreur de reception du message du client");
				pthread_exit(NULL);
			}
			else if(resR1==0){
				perror("Socket fermé du client");
				pthread_exit(NULL);
			}
		puts("cest bon 11");

			if(strcmp(msg,"fin\n")==0){
				for(k=0;k<nombreClient;k++){
		puts("cest bon 12");

					send(envoi.salonClient.tabSocketClient[k].socketC,msg,strlen(msg)+1,0);
					/*On envoie le message fin à chaque client (même celui qui a envoyé 'fin') pour qu'ils
					ferment eux-même leur socket et leurs threads*/
					if(k!=envoi.numClient){
						pthread_cancel(envoi.salonClient.tabSocketClient[k].socketC);
					}
					/*Et on arrete les threads des clients 1 par 1*/
				
				}
				printf("Les clients sont déconnectés");
				pthread_exit(NULL);
			}
		puts("cest bon 13");


			char message[60]="";
			strcat(message,"Message recu de ");
			strcat(message,envoi.salonClient.tabSocketClient[envoi.numClient].pseudo);
			strcat(message," : ");
			strcat(message,msg);
			/*On s'occupe ici de changer le message reçu du client pour y mettre devant le pseudo de celui-ci*/
			
			printf("%s",message);
			for(j=0;j<nombreClient;j++){
				if(j!=envoi.numClient){
		puts("cest bon 14");

					int resS1 = send(envoi.salonClient.tabSocketClient[j].socketC,message,strlen(message)+1,0);
					if(resS1==-1){
					perror("Erreur d'envoie du message pour les clients ");
					pthread_exit(NULL);
					}
					/*Et on envoie à tout le monde sauf au client qui a envoyé le message précédemment*/
				}
			}
	}
}


void *connexion(){
	while(1){
		puts("on passe là");
		struct socketClient client;
		client.socketC = accept(dSocket,(struct sockaddr *) &(client.adresseC),&lgA);
		puts("on passe 2");

		int res = recv(client.socketC,client.pseudo,sizeof(client.pseudo),0);
		puts("cest bon");
		printf("%s\n",client.pseudo);
		send(client.socketC,tabSalon,sizeof(tabSalon),0);
		puts("cest bon 2");
		char nomSalon;
		res = recv(client.socketC,&nomSalon,sizeof(nomSalon),0);
		printf("%c\n",nomSalon);
		int numSalon= atoi(&nomSalon);
		printf("%d", numSalon);
		int i;		
		for(i=0;i<10;i++){
			if(tabSalon[i].numSalon==numSalon){

				if(tabSalon[i].nbClient < 10){

					tabSalon[i].tabSocketClient[tabSalon[i].nbClient] = client;

					tabSalon[i].nbClient += 1;

					struct envoiClient envoi;

					envoi.salonClient = tabSalon[i];

					envoi.numClient = tabSalon[i].nbClient - 1; 

					if(pthread_create(&(tabSalon[i].tabSocketClient[tabSalon[i].nbClient].thread),NULL,(void*)&clientVersAutre,&envoi)==-1){
						perror("erreur dans la création du thread 1");
						pthread_exit(NULL);
					}
				}
				else{
					char msg[] = "Ce salon est plein";
					send(client.socketC,msg,strlen(msg)+1,0);
				}
			}	
		}
	}
	

}


int main(int argc,char* argv[]){

	nombreClient = atoi(argv[2]); /*On définit le nombre de client qui peuvent se connecter simultanément avec l'argument 2*/
	int i;
	int k;
	int l;
	dSocket = socket(PF_INET,SOCK_STREAM,0);
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

    /*On a bind le serveur pour permettre aux clients de se connecter et on le
met en listen avec le nombreMax de clients possible*/     
	int m;
	for(m=0;m<10;m++){    
    	tabSalon[m].nbClient=0;    
    	tabSalon[m].numSalon=m;
     }

	
		pthread_t thread;
		//création du thread connexion
		if(pthread_create(&(thread),NULL,(void*)&connexion,NULL)==-1){
				perror("erreur dans la création du thread 1");
			return EXIT_FAILURE;
			}

		int ret= pthread_join(thread,NULL);
		/*On fait cette fonction qui permet au main d'attendre qu'un thread soit fini (en sachant que dans notre 
		code, si un thread est terminé alors tous les autres threads clients se terminent)*/
		
		for(l=0;l<nombreClient;l++){
			close(tabSocketClient[l].socketC);
		}
		/*Et on ferme tous les sockets clients*/


	close(dSocket);
	return EXIT_SUCCESS;
}
