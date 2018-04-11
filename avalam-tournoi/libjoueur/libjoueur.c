/********* Moteur de tournoi : joueur ***************/

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "avalam.h"
#include "moteur.h"
#include "topologie.h"

T_TabJoueurs joueurs; 
char * nom ="";
int pidMoteur; 
int myPid; 
T_Etat etat; 
FILE * fp; 
octet couleur; 
pthread_t thread;

T_Position p; 
T_Score s ;
T_ListeCoups l; 

extern void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups);


void whoami() {
	if (couleur == ROU) 
		printf("\t\t\t\t\t\t\t%s (%d) : " , nom, myPid); 
	else  
		printf("%s(%d) : ", nom, myPid);
}

void whop(char * s) {
	if (couleur == ROU) 
		printf("\t\t\t\t\t\t\t%s (%d) : %s\n" , nom, myPid, s); 
	else  
		printf("%s(%d) : %s\n", nom, myPid, s);
}

// PROGRAMME 'JOUEUR'


void * thread_coup(void * i) {	
	octet coupParDefaut = 0; 

	whoami(); printf("Lancement thread de recherche d'un coup (coul=%d)\n", couleur); 
	// On écrit dans le fichier pid le coup choisi 
	// par défaut le premier

	if (l.nb>0) {
		
		whoami(); printf("Choix coup %d -> %d\n",l.coups[coupParDefaut].origine,l.coups[coupParDefaut].destination); 
		//ecrireCoup(l.coups[0].origine,l.coups[0].destination);
		ecrireIndexCoup(coupParDefaut);//TODO:  a changer en 0 
	}
	else {
		printf("plus aucun coup jouable\n");
	}

	// Appel de la fonction des étudiants
	choisirCoup(p, l);

	// Attente de terminaison du thread sur occurence signal 
	pause();
	
	return NULL;  
}

int main(int argc, char ** argv)
{
	struct sigaction newact;	// déclaration de la structure 
	char buffer[MAXLEN];

	srand(time(NULL));

	// Recup NOM du joueur 
	
	if (argc != 2) {
		printf("Il faut saisir votre nom !\n"); 
		return 1;
	}

	newact.sa_sigaction = deroute;	
	CHECK_IF(sigemptyset(&newact.sa_mask),-1,"problème sigemptyset");
	newact.sa_flags = SA_SIGINFO;

	// Si SA_SIGINFO est indiqué dans sa_flags, alors sa_sigaction (plutôt que sa_handler) 
	// pointe vers le gestionnaire  de  signal pour signum.

	// on installe le handler deroute
	// A partir de maintenant, toute occurence d'un signal SIGUSR1 déclenche l'appel de la fonction deroute  
	CHECK_IF(sigaction(SIGUSR1, &newact, NULL),-1,"problème sigaction");
	CHECK_IF(sigaction(SIGUSR2, &newact, NULL),-1,"problème sigaction");
	CHECK_IF(sigaction(SIGALRM, &newact, NULL),-1,"problème sigaction");

	myPid = getpid();
	nom = argv[1];
	printf("Process joueur : pid=%d, nom=%s\n", myPid, nom);
	
	// printf("PID process : %d\n",getpid());
	// On enregistre le PID du moteur dans le fichier "moteur"

	sprintf(buffer,"%s/%d",PATH_RUN, myPid);
	CHECK_IF(fp = fopen(buffer,"w"),NULL, "fopen");
	fprintf(fp,"%s",nom); 
	fclose(fp);

	// recup PID moteur
	sprintf(buffer,"%s/moteur.pid",PATH_RUN);
	CHECK_IF(fp = fopen(buffer,"r"),NULL, "fopen");
	fgets(buffer,MAXLEN,fp); 
	pidMoteur = atoi(buffer);
	printf("PID moteur = %d\n", pidMoteur);
	fclose(fp);

	etat = ATTENTE_APPARIEMENT; 

	// envoi signal au moteur
	kill(pidMoteur, SIGUSR2);
	

	while(1) {
		pause();
	}

	// TODO : supprimer le fichier lié au process... 
	// C'est fait automatiquement lorsqu'on relance 
	exit(0);
}


void deroute (int signal_number, siginfo_t * info, void * context)
{
	int i;
	char * col;
	char * s; 
	int indexCoupJoue; 
	void * res; 

	// printf("PID emetteur : %d !!\n",info->si_pid);
	switch (signal_number) {
		case SIGUSR1 : 
			whoami(); 
			printf("Signal SIGUSR1 reçu.\n"); 

			switch(etat) {
				case ATTENTE_APPARIEMENT : 
					// Une nouvelle partie va commencer
					col = lireFichierJoueur(myPid);
					//printf("col lue : .%s.\n",col);
					if (strcmp(col,"j") == 0) couleur = JAU; 
					else couleur = ROU; 

					whop("Nouvelle partie");
					whoami();printf("Je suis %s\n", COLNAME(couleur));

					// On initialise la partie 
					p = getPositionInitiale();
					l = getCoupsLegaux(p);

					// On ne commence pas tout de suite, on attend que le moteur nous renvoie un signal de départ					
					if (couleur == JAU) {
						etat = ATTENTE_DEBUT;
						whop("C'est mon tour !");
					}
					else {
						etat = ATTENTE_TOUR_ADVERSAIRE;
						whop("C'est le tour de mon adversaire !"); 
					}
				break;

				case ATTENTE_DEBUT :
					// On commence depuis la position initiale 
					// On lance un thread de recherche du prochain coup 
					// Ce thread devra écrire régulièrement le coup choisi dans un fichier portant le nom du pid du process 
					// On devra le terminer lorsqu'on recevra un signal SIGUSR1
					etat = MON_TOUR;
					whop("La partie commence !");
					pthread_create(&thread, NULL,thread_coup, NULL);
				break;

				case ATTENTE_TOUR_ADVERSAIRE : 
					// On reçoit un signal : il faut jouer le coup sélectionné par l'adversaire sur la position
					// Celui-ci est contenu dans le fichier pid.
					s = lireFichierJoueur(myPid); 
					whoami();printf("Saisie adversaire : %s\n", s); 
					indexCoupJoue = atoi(s);
					free(s); 
					if ((indexCoupJoue>= 0) && (indexCoupJoue<l.nb)) {
						whoami(); printf("L'adversaire a choisi le coup %d\n",indexCoupJoue );
						whoami(); printf("Soit : %d -> %d\n", l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination);
						p = jouerCoup(p, l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination) ;
						l = getCoupsLegaux(p);
					} else {
						whoami(); printf("Saisie incorrecte ! \n"); 
						return; 
					}
					
					etat = MON_TOUR;
					whop("C'est mon tour !");
					pthread_create(&thread, NULL,thread_coup, NULL);
					// On lance un thread de recherche du prochain coup 
					// Ce thread devra écrire régulièrement le coup choisi dans un fichier portant le nom du pid du process 
				break;

				case MON_TOUR  : 
					// On reçoit un signal : il faut jouer le coup sélectionné par le moteur sur la position

					s = lireFichierJoueur(myPid); 
					whoami();printf("Saisie moteur : %s\n", s); 
					indexCoupJoue = atoi(s);
					free(s); 
					if ((indexCoupJoue>= 0) && (indexCoupJoue<l.nb)) {
						whoami(); printf("Je joue le coup %d\n",indexCoupJoue );
						whoami(); printf("Soit : %d -> %d\n", l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination);
						p = jouerCoup(p, l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination) ;
						l = getCoupsLegaux(p);
					} else {
						whoami(); printf("Saisie incorrecte ! \n"); 
						return; 
					}
					
					// puis on attend l'adversaire 
					etat = ATTENTE_TOUR_ADVERSAIRE;
					whop("C'est le tour de mon adversaire !");
				break;
			}

		break;
		case SIGUSR2 : 
			whoami(); 
			printf("Signal SIGUSR2 reçu.\n"); 
			whop("Partie terminée\n");
			// Il faut supprimer le thread 
			CHECK_DIF(pthread_cancel(thread),0,"pthread_cancel");
			CHECK_DIF(pthread_join(thread,&res),0, "pthread_join"); 
			if (res == PTHREAD_CANCELED)
				whop("Thread terminé\n");

			etat = ATTENTE_APPARIEMENT; 
			// devrait relancer une nouvelle partie 

			// On renvoie un SIGUSR1 au moteur 
			kill(pidMoteur, SIGUSR1);
	
		break;
		case SIGALRM : 
			whoami(); 
			printf("Signal SIGALARM reçu.\n"); 
			break;
	}
}



// jamais aucun joueur ne doit ecrire dans un autre fichier que le sien 

char * lireFichierJoueur (int pid) {
	FILE * fp; 
	char buffer[MAXLEN];
	sprintf(buffer,"%s/%d",PATH_RUN, pid);
	CHECK_IF(fp = fopen(buffer,"r"),NULL, "fopen");
	fgets(buffer,MAXLEN,fp);  			
	fclose(fp);
	return strdup(buffer);
}

void ecrireFichierJoueur(int pid, char * s) {
	// on n'ajoute pas de saut de ligne 
	FILE * fp; 
	char buffer[MAXLEN];
	sprintf(buffer,"%s/%d",PATH_RUN, pid);
	CHECK_IF(fp = fopen(buffer,"w"),NULL, "fopen");
	fprintf(fp,"%s",s);  			
	fclose(fp);
}

void ecrireCoup(octet origine, octet destination) {
	char buffer[6]; 
	sprintf(buffer, "%02d:%02d", origine, destination);
	ecrireFichierJoueur(myPid, buffer); 
}

void ecrireIndexCoup(int index) {
	char buffer[6]; 

	if (index>=0 && index<l.nb) {
		sprintf(buffer, "%d", index);
		ecrireFichierJoueur(myPid, buffer); 
	} else {
		printf("Index coup %d invalide : non pris en compte\n", index); 
	}
}





