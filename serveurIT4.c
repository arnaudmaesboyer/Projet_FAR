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
	char salon[40];
};




struct salon{
	char salon[40];
	int nbClient;
};


struct salon tabSalon[10];

struct salon chien;
struct salon chat;
struct salon souris;
struct salon chenille;
struct salon singe;
struct salon tigre;
struct salon ecureuil;
struct salon dauphin;
struct salon panda;
struct salon elephant;


int dSocket;


/*On définit une structure permettant de stocker toutes les informations de chaque client connectés comme
son pseudo, son adresse, son socket et le thread utilisé par le serveur pour ce client*/

struct socketClient tabSocketClient[100];
/*On crée ensuite un tableau qui contiendra toutes les informations pour chaque client*/

int nombreClient;

 /*On définit à 100 le nombre max de client qui peuvent se connecter*/
/* thread de qiui envoi les message aux autres clients*/
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
			/*if(strcmp(msg,"fin\n")==0){
				int a;
				for(a=0;a<10;a++){
					if(strcmp(tabSalon[a].salon,tabSocketClient[j].salon)==0){
						tabSalon[k].nbClient -= 1;
					}
				}
				printf("Le client est déconnecté");
				pthread_exit(NULL);
			}*/

			char message[60]="";
			strcat(message,"Message recu de ");
			strcat(message,tabSocketClient[i].pseudo);
			strcat(message," : ");
			strcat(message,msg);
			/*On s'occupe ici de changer le message reçu du client pour y mettre devant le pseudo de celui-ci*/

			int nombreClient;
			for(k=0;k<10;k++){
				if(strcmp(tabSalon[k].salon,tabSocketClient[i].salon)==0){
					nombreClient = tabSalon[k].nbClient;
				}
			}


			printf("%s",message);
			for(j=0;j<100;j++){
				if(j!=i && strcmp(tabSocketClient[i].salon,tabSocketClient[j].salon)==0){
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


/* thread de qui tourne en permannence pour la connexion de nouveaux clients*/

void *connexion(){
	int i = 0;
	while(1){
		struct socketClient client;
		client.socketC = accept(dSocket,(struct sockaddr *) &(client.adresseC),&lgA);

		int res = recv(client.socketC,client.pseudo,sizeof(client.pseudo),0);
		printf("Le client %s est connecté\n",client.pseudo);
		send(client.socketC,tabSalon,sizeof(tabSalon),0);
		printf("Envoi de la liste des salons au client %s\n",client.pseudo);

		// on met le nom du salon choisit dans le client
		res = recv(client.socketC,client.salon,sizeof(client.salon),0);
		printf("Le client %s veut se connecter sur le salon %s\n",client.pseudo,client.salon);

		int k;
		int valide = 1;
		for(k=0;k<10;k++){
			if(strcmp(tabSalon[k].salon,client.salon)==0 && tabSalon[k].nbClient<3){
				tabSalon[k].nbClient += 1;
				valide = 0;
			}
		}

		while(valide == 1){
			char msg[50]="Ce salon est plein veuillez en saisir un autre ! ";
			send(client.socketC,msg,strlen(msg)+1,0);
			// on met le nom du salon choisit dans le client
			res = recv(client.socketC,client.salon,sizeof(client.salon),0);
			printf("%s\n",client.salon);
			for(k=0;k<10;k++){
			if(strcmp(tabSalon[k].salon,client.salon)==0 && tabSalon[k].nbClient<3){
				tabSalon[k].nbClient += 1;
				valide = 0;
			}
		}
		}
		printf("Le client %s s'est bien connecté au salon %s\n",client.pseudo,client.salon);
		
		tabSocketClient[i] = client;

		if(pthread_create(&(tabSocketClient[i].thread),NULL,(void*)&clientVersAutre,i)==-1){
				perror("erreur dans la création du thread 1");
			pthread_exit(NULL);
		}
		i += 1;	


	}
	

}



int main(int argc,char* argv[]){

	strcpy(chien.salon,"chien\n"); // car on va le comparer avec un fgets
	chien.nbClient= 0;
	tabSalon[0] = chien;

	strcpy(chat.salon,"chat\n"); // car on va le comparer avec un fgets
	chat.nbClient= 0;
	tabSalon[1] = chat;

	strcpy(souris.salon,"souris\n"); // car on va le comparer avec un fgets
	souris.nbClient= 0;
	tabSalon[2] = souris;

	strcpy(chenille.salon,"chenille\n"); // car on va le comparer avec un fgets
	chenille.nbClient= 0;
	tabSalon[3] = chenille;

	strcpy(souris.salon,"souris\n"); // car on va le comparer avec un fgets
	souris.nbClient= 0;
	tabSalon[4] = souris;

	strcpy(tigre.salon,"tigre\n"); // car on va le comparer avec un fgets
	tigre.nbClient= 0;
	tabSalon[5] = tigre;

	strcpy(ecureuil.salon,"ecureuil\n"); // car on va le comparer avec un fgets
	ecureuil.nbClient= 0;
	tabSalon[6] = ecureuil;

	strcpy(dauphin.salon,"dauphin\n"); // car on va le comparer avec un fgets
	dauphin.nbClient= 0;
	tabSalon[7] = dauphin;

	strcpy(panda.salon,"panda\n"); // car on va le comparer avec un fgets
	panda.nbClient= 0;
	tabSalon[8] = panda;

	strcpy(elephant.salon,"elephant\n"); // car on va le comparer avec un fgets
	elephant.nbClient= 0;
	tabSalon[9] = elephant;



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

	/*On a bind le serveur pour permettre aux clients de se connecter et on le met en listen avec le nombreMax de clients possible*/

		/*Ici on rentre dans un while(1) pour permettre au serveur de rester allumer même lors de la déconnection des clients*/

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
