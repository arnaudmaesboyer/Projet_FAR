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

void *threadFichier(int *arg){
	char nomFichier[100];
	char contenueFich[20000];
	if(*arg==1){
		int resR1 = recv(dSocketClient1,nomFichier,sizeof(nomFichier),0);
		if(resR1==-1){
			perror("Erreur de reception du message du client 1");
			pthread_exit(NULL);
		}
		else if(resR1==0){
			perror("Socket fermé du client 1");
			pthread_exit(NULL);
		}
		int resS1 = send(dSocketClient2,nomFichier,strlen(nomFichier)+1,0);
		if(resS1==-1){
			perror("Erreur d'envoie du message pour le client 2");
			pthread_exit(NULL);
		}
		/*On recoit le nom du fichier et on l'envoie à lautre client pour quil crée un fichier*/
		printf("Nom du fichier reçu.\n");

		resR1 = recv(dSocketClient1,contenueFich,sizeof(contenueFich),0);
		if(resR1==-1){
			perror("Erreur de reception du message du client 1");
			pthread_exit(NULL);
		}
		else if(resR1==0){
			perror("Socket fermé du client 1");
			pthread_exit(NULL);
		}

		printf("Contenue du fichier reçu\n");

		resS1 = send(dSocketClient2,contenueFich,strlen(contenueFich)+1,0);
		if(resS1==-1){
			perror("Erreur d'envoie du message pour le client 2");
			pthread_exit(NULL);
		}
		/*On recoit le contenue du fichier et on lenvoie à lautre client*/
		printf("Contenue du fichier envoyé au client 2\n");

	}
	else if(*arg==2){
		int resR1 = recv(dSocketClient2,nomFichier,sizeof(nomFichier),0);
		if(resR1==-1){
			perror("Erreur de reception du message du client 1");
			pthread_exit(NULL);
		}
		else if(resR1==0){
			perror("Socket fermé du client 1");
			pthread_exit(NULL);
		}
		int resS1 = send(dSocketClient1,nomFichier,strlen(nomFichier)+1,0);
		if(resS1==-1){
			perror("Erreur d'envoie du message pour le client 2");
			pthread_exit(NULL);
		}
		/*On recoit le nom du fichier et on l'envoie à lautre client pour quil crée un fichier*/
		printf("Nom du fichier reçu.\n");


		resR1 = recv(dSocketClient2,contenueFich,sizeof(contenueFich),0);
		if(resR1==-1){
			perror("Erreur de reception du message du client 1");
			pthread_exit(NULL);
		}
		else if(resR1==0){
			perror("Socket fermé du client 1");
			pthread_exit(NULL);
		}

		printf("Contenue du fichier reçu\n");

		resS1 = send(dSocketClient1,contenueFich,strlen(contenueFich)+1,0);
		if(resS1==-1){
			perror("Erreur d'envoie du message pour le client 2");
			pthread_exit(NULL);
		}
		/*On recoit le contenue du fichier et on lenvoie à lautre client*/
		printf("Contenue du fichier envoyé au client 2\n");

	}
	pthread_exit(NULL);
}


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
			if(strcmp(msg,"/file\n")==0){
				pthread_t fichier;
				int resS1 = send(dSocketClient2,msg,strlen(msg)+1,0);
				if(resS1==-1){
					perror("Erreur d'envoie du message pour le client 2");
					pthread_exit(NULL);
				}
				int a=1;
				if(pthread_create(&fichier,NULL,(void*)&threadFichier,&a)==-1){
					perror("erreur dans la création du thread fichier");
					pthread_exit(NULL);
				}

			int ret= pthread_join(fichier,NULL);
			}
			else{

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

			if(strcmp(msg,"/file\n")==0){
				pthread_t fichier;
				int resS1 = send(dSocketClient1,msg,strlen(msg)+1,0);
				if(resS1==-1){
					perror("Erreur d'envoie du message pour le client 2");
					pthread_exit(NULL);
				}
				int a=2;
				if(pthread_create(&fichier,NULL,(void*)&threadFichier,&a)==-1){
					perror("erreur dans la création du thread fichier");
					pthread_exit(NULL);
				}
				
			int ret= pthread_join(fichier,NULL);
			}

			else{

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

	/*On a bind le serveur pour permettre aux clients de se connecter à lui et on initialise 2 structures sockaddr_in pour les paires de clients qui vont se connecter*/
	
	pthread_t client1;
	pthread_t client2;

	while(1){
		/*Ici on rentre dans un while(1) pour permettre au serveur de rester allumer même lors de la déconnection des clients*/
		dSocketClient1 = accept(dSocket,(struct sockaddr *) &adClient1,&lgA1);

		dSocketClient2 = accept(dSocket,(struct sockaddr *) &adClient2,&lgA2);
		/*On met en place deux sockets clients pour permettre l'échange de message bien structuré*/

		printf("Les deux clients sont connectés, ils peuvent maintenant communiquer.\n");
		
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
