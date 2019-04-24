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
#include <dirent.h>
#include <limits.h>

int dSock;
pthread_t env;
pthread_t recep;
/*Ces variables sont globales car nous les utilisons à la fois dans le main et dans les fonctions thread*/

int get_last_tty() {
  FILE *fp;
  char path[1035];
  fp = popen("/bin/ls /dev/pts", "r");

  if (fp == NULL) {
    printf("Impossible d'exécuter la commande\n" );
    exit(1);
  }
  int i = INT_MIN;

  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    if(strcmp(path,"ptmx")!=0){

      int tty = atoi(path);
      if(tty > i) i = tty;
    }
  }

  pclose(fp);
  return i;
}

FILE* new_tty() {
  pthread_mutex_t the_mutex;  
  pthread_mutex_init(&the_mutex,0);
  pthread_mutex_lock(&the_mutex);
  system("gnome-terminal"); sleep(1);
  char *tty_name = ttyname(STDIN_FILENO);    
  int ltty = get_last_tty();   
  char str[2];
  sprintf(str,"%d",ltty);
  
  int i;
  for(i = strlen(tty_name)-1; i >= 0; i--) {
    if(tty_name[i] == '/') break;
  }
  tty_name[i+1] = '\0';  
  strcat(tty_name,str);  
  FILE *fp = fopen(tty_name,"wb+");
  pthread_mutex_unlock(&the_mutex);
  pthread_mutex_destroy(&the_mutex);
  return fp;
}

void *envoie(void *arg){
	/*Ici nous somme dans le thread où le client peut envoyer des messages au serveur*/
	char buffer[50];
		while(1){
			fgets(buffer,50,stdin);

			if(strcmp(buffer,"/file\n")==0){
				  char str[1000];
				  char contenueFichier[1000]="";

				  int resS = send(dSock,buffer,strlen(buffer)+1,0);
				  /*On envoie le message du fichier au serveur*/
				  if(resS==-1){
					  perror("Erreur dans l'envoie du message du fichier du client vers le serveur");
					  pthread_exit(NULL);
				  }
				  if(resS<strlen(buffer)+1){
					  perror("Erreur dans l'envoie du message du fichier du client vers le serveur --> Le message n'a pas été envoyé entièrement");
					  pthread_exit(NULL);
				  }

				  FILE* fp1 = new_tty();
				  fprintf(fp1,"%s\n","Ce terminal sera utilisé uniquement pour l'affichage");

				  /* Demander à l'utilisateur quel fichier afficher*/
				  DIR *dp;
				  struct dirent *ep;     
				  dp = opendir ("./");
				  if (dp != NULL) {
				    fprintf(fp1,"Voilà la liste de fichiers :\n");
				    while (ep = readdir(dp)) {
				      if(strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0) 
					fprintf(fp1,"%s\n",ep->d_name);
				    }    
				    (void) closedir (dp);
				  }
				  else {
				    perror ("Ne peux pas ouvrir le répertoire");
				  }
				  printf("Indiquer le nom du fichier : ");
				  char fileName[1023];
				  fgets(fileName,sizeof(fileName),stdin);
				  fileName[strlen(fileName)-1]='\0';
				  resS = send(dSock,fileName,strlen(fileName)+1,0);
				  /*On envoie le nom du fichier au serveur*/

				  FILE *fps = fopen(fileName, "r");
				  if (fps == NULL){
				    printf("Ne peux pas ouvrir le fichier suivant : %s",fileName);
				  }
				  else {
				        
				    /* Lire et afficher le contenu du fichier*/
				   	while (fgets(str, 1000, fps) != NULL) {
				      fprintf(fp1,"%s",str);
				      strcat(contenueFichier,str);
				    }
				  }
				  printf("%s",contenueFichier);

				  resS = send(dSock,contenueFichier,strlen(contenueFichier)+1,0);
				  /*On envoie le contenue du fichier au serveur*/
				  if(resS==-1){
					  perror("Erreur dans l'envoie du message du fichier du client vers le serveur");
					  pthread_exit(NULL);
				  }
				  if(resS<strlen(contenueFichier)+1){
					  perror("Erreur dans l'envoie du message du fichier du client vers le serveur --> Le message n'a pas été envoyé entièrement");
					  pthread_exit(NULL);
				  }
				  fclose(fps);	
			}
			else{

				int resS = send(dSock,buffer,strlen(buffer)+1,0);
				/*On envoie le message au serveur*/
				if(resS==-1){
					perror("Erreur dans l'envoie du message du client vers le serveur");
					pthread_exit(NULL);
				}
				if(resS<strlen(buffer)+1){
					perror("Erreur dans l'envoie du message du client vers le serveur --> Le message n'a pas été envoyé entièrement");
					pthread_exit(NULL);
				}
			}
			
		}
	pthread_exit(NULL);
}

void *reception(void *arg){
	/*Ici nous sommes dans le thread permettant de recevoir les messages venant du serveur*/
	char buffer[50];
	while(1){

			int resR = recv(dSock,&buffer,sizeof(buffer),0);
			/*On reçoit le message*/
			if(resR==0){
				perror("Le client 1 est fermé");
				pthread_exit(NULL);
			}
			else if(resR==-1){
				perror("Erreur dans la réception du message du client 2");
				pthread_exit(NULL);
			}
			printf("%s",buffer);

			if(strcmp(buffer,"/file\n")==0){
				char nomFichier[100];
				
				resR = recv(dSock,&nomFichier,sizeof(nomFichier),0);
				char cheminFichier[100]="../";
				strcat(cheminFichier,nomFichier);

				FILE* monFich = NULL;
				monFich=fopen(cheminFichier,"w+");

				char contenueFichier[1000];
				resR = recv(dSock,&contenueFichier,sizeof(contenueFichier),0); 
				if(fputs(contenueFichier,monFich)<0){
					perror("probleme ecriture dans fichier");
				}

				fclose(monFich);
				puts("Fichier reçu !");

			}

			if(strcmp(buffer,"fin\n")==0){
				break;
			}
			/*Si le message reçu est fin alors on sort de la boucle*/
		}
	pthread_cancel(env);
	pthread_cancel(recep);
	/*Et on arrete les deux threads envoie() et réception()*/
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
	inet_pton(AF_INET,"162.38.111.14",&(adServ.sin_addr));
	adServ.sin_port=htons(atoi(argv[1])); /* L'argument 1 est le port du serveur */ 
	socklen_t lgA = sizeof(struct sockaddr_in);
	int resC = connect(dSock,(struct sockaddr*)&adServ,lgA);
	if (resC==-1){
		perror("Erreur de connection au serveur");
		close(dSock);
		return 0;
	}
	/*On crée la socket client et on se connecte au serveur*/
	if(pthread_create(&env,NULL,(void*)&envoie,NULL)==-1){
		perror("erreur dans la création du thread 1");
		return EXIT_FAILURE;
	}
	/*Ici on crée notre premier thread permettant d'envoyer les messages au serveur*/
	if(pthread_create(&recep,NULL,(void*)&reception,NULL)==-1){
		perror("erreur dans la création du thread 2");
		return EXIT_FAILURE;
	}
	/*Et ici on crée celui qui permet de recevoir les message du serveur*/
	
	int ret= pthread_join(env,NULL);
	int ret2= pthread_join(recep,NULL);
	/*Ces fonctions permettent de bloquer le main principal et d'attendre que les threads envoie et reception soient terminés
	pour continuer l'exécution du programme*/

	close(dSock);
	return EXIT_SUCCESS;
}
