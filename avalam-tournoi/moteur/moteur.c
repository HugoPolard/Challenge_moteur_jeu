/********** Moteur de tournoi : moteur ************/

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <semaphore.h>
#include <pthread.h>
#include "avalam.h"
#include "topologie.h"
#include "moteur.h"

T_TabJoueurs joueurs; 
T_TabParties tournoi;
int delaiChoixCoup = DELAICHOIXCOUP; 


int main(int argc, char ** argv)
{
	struct sigaction newact;	// déclaration de la structure 
	char buffer[MAXLEN];
	FILE * fp; 
	DIR * dp;
	struct dirent * pDir; 
	int r,i;

	if (argc == 2) {
		delaiChoixCoup = atoi(argv[1]); 
		printf("Délai choix coup : %d secondes\n", delaiChoixCoup);
	}

	printf("PID process : %d\n",getpid());
	// On enregistre le PID du moteur dans le fichier "moteur"

	// On demande à tous les processus du répertoire run de se terminer 
	
	dp = opendir(PATH_RUN); 
	if (dp != NULL) {
		while (	(pDir = readdir(dp)) != NULL ) {
			if (pDir->d_type == DT_REG)
			if (strcmp(pDir->d_name,PID_FILE) != 0) {
				printf("suppression du processus %s \n", pDir->d_name);
				// NB : on pourrait le faire avec un appel direct à kill()
				sprintf(buffer,"kill %s",pDir->d_name);
				CHECK_IF(system(buffer),-1,"suppression processus");
	
				sprintf(buffer,"rm -rf \"%s/%s\"",PATH_RUN,pDir->d_name);
				printf("%s\n",buffer);
				CHECK_IF(system(buffer),-1,"supression contenu de .../run");
			}
		}
		closedir(dp);

	}

	// On crée le répertoire run si nécessaire 
	sprintf(buffer,"mkdir -p \"%s\"",PATH_RUN);
	printf("%s\n",buffer);
	CHECK_IF(system(buffer),-1,"creation de .../run");

	sprintf(buffer,"%s/%s",PATH_RUN,PID_FILE);
	CHECK_IF(fp = fopen(buffer,"w"),NULL, "fopen");
	fprintf(fp,"%d",getpid()); 
	fclose(fp);

	newact.sa_sigaction = deroute;	
	CHECK_IF(sigemptyset(&newact.sa_mask),-1,"problème sigemptyset");
	newact.sa_flags = SA_SIGINFO | SA_RESTART;

	// on installe le handler deroute
	// A partir de maintenant, toute occurence d'un signal SIGUSR1 déclenche l'appel de la fonction deroute  
	CHECK_IF(sigaction(SIGUSR1, &newact, NULL),-1,"problème sigaction");
	CHECK_IF(sigaction(SIGUSR2, &newact, NULL),-1,"problème sigaction");
	CHECK_IF(sigaction(SIGALRM, &newact, NULL),-1,"problème sigaction");

	printf("Attente de connexion des joueurs\n");
	printf("Appuyez sur ENTREE pour démarrer\n"); 
	getchar();

	printf("Demarrage\n");

	printf("Nombre de joueurs : %d\n", joueurs.nb);
	if (joueurs.nb %2 ==1) {
		printf("Nombre impair : on ajoute un joueur baptisé 'exempt'\n");
		addJoueur(0,"exempt", ATTEND); // souci lié au pid : si on kill 0, souci ! 
	}

	initTournoi();

	//listerTournoi();
	
	// Il faut jouer les parties du tournoi, ronde par ronde
	// Il faudra pouvoir relancer une partie lorsqu'elle se terminera

	// états d'un joueur : 
	// en attente de commencer une partie 
	// Moteur envoie à chaque joueur des infos dans un fichier texte portant son pid 
	// reçoit SIGUSR1 pour commencer partie 
	// pendant une partie : en attente du coup adverse
	// reception du coup adverse
	// en train de calculer son coup 
	// fin du temps pour calculer son coup 
	// envoi du coup 


	// code du moteur : 
	// pendant une ronde, en attente d'un sigal d'un joueur 



/*
Lorsque deux process sont dispo, il leur envoie un signal SIGUSR1 pour indiquer que la partie démarre
Il indique à chaque process sa couleur en utilisant des fichiers portant comme nom le PID du process

Chaque process doit accuser réception de cette info
Le moteur de tournoi envoie alors un signal SIGUSR1 au process de qui c'est le tour, ce qui déclenche son chrono. 
On joue à 30s /coup

Chaque groupe devra juste produire une fonction appelant régulièrement la fonction choisirCoup(origine,destination) pour mettre à jour le coup sélectionné
Par défaut, le coup sera le premier listé par la fonction listercoupslegaux
dans un fichier portant comme nom son PID
Cette fonction sera destinataire de la position courante du jeu

Lorsque le temps imparti sera atteint, le moteur de tournoi envoie un signal au process de jeu qui supprime le thread de jeu, en attendant sa prochaine exécution

Le moteur de jeu valide alors le coup pour l'enregistrer dans un fichier, puis envoie ce coup au processus qui joue pour l'autre joueur 
Ce processus récupère le coup, préparer la position, et invoque un nouveau thread de jeu, et ainsi de suite
*/

	listerTournoi();
	for(r=0;r< tournoi.nbRondes; r++)
	{
		printf("---- Ronde %d -----\n", r+1);
		for(i=(joueurs.nb/2) * r ;i<(joueurs.nb/2) * (r+1);i++){
			// init partie i : 
			// On écrit à chaque joueur quelle couleur il joue dans son fichier pid, 
			printf("Lancement partie %d (%s - %s)\n", i, tournoi.parties[i].nomJ , tournoi.parties[i].nomR);

			sem_init(&(tournoi.parties[i].sem),0,0); 
			tournoi.parties[i].status = ENCOURS; 

			if ( (tournoi.parties[i].pidJ != 0) && (tournoi.parties[i].pidR != 0) )
			{
				// partie entre deux moteurs
				// Il faudrait créer un thread de traitement pour chaque partie, permettant de gérer le temps de jeu de chaque joueur 
				pthread_create(&(tournoi.parties[i].thread), NULL,thread_partie, (void*)i);
				// TODO : CHECK()
 			} else {
				// un exempt : l'autre gagne sur le plus petit des scores
				// On pourrait aussi ne pas compter la partie...
				sem_post(&(tournoi.parties[i].sem));
				if (tournoi.parties[i].pidJ == 0)	terminerPartie(i,0,0,1,0);
				else terminerPartie(i,1,0,0,0);
			}
			// On va créer autant de semaphores qu'il y a de parties 
			// Chaque semaphore de partie sera libéré par le thread qui vient de terminer 
			// On pourra donc bloquer et poursuivre le programme principal

			// et on lui envoie un signal SIGUSR1. 
			//printf("jaunes:%s rouges:%s\n", tournoi.parties[i].nomJ,tournoi.parties[i].nomR);
		}

		


		// On attend la fin des parties
		for(i=(joueurs.nb/2) * r ;i<(joueurs.nb/2) * (r+1);i++){
			sem_wait(&(tournoi.parties[i].sem)); 
			printf("Partie %d terminee\n",i);
			listerPartie(tournoi.parties[i].feuille);
			pthread_join(tournoi.parties[i].thread, NULL);
		}

		listerTournoi();
	}

	exit(0);
}

void whoami(octet numP) {
	printf("Partie %d : %s(%d) - %s(%d) : " , numP,tournoi.parties[numP].nomJ,tournoi.parties[numP].pidJ, tournoi.parties[numP].nomR, tournoi.parties[numP].pidR); 
}

void * thread_partie(void * i) {
	int numP = (int) i; 
	char bNom[MAXLEN]; 

	printf("Lancement thread de gestion de la partie %d\n", numP); 
	sprintf(bNom,"%s/%d.js",PATH_WEB, numP);

	ecrireFichierJoueur(tournoi.parties[numP].pidJ, "j");
	kill(tournoi.parties[numP].pidJ, SIGUSR1);
	ecrireFichierJoueur(tournoi.parties[numP].pidR, "r");
	kill(tournoi.parties[numP].pidR, SIGUSR1);

	// Chaque thread va gérer une partie, comme avalam_standalone
	T_Position p = getPositionInitiale();
	writePos(bNom, p);

	T_ListeCoups l = getCoupsLegaux(p);
	T_Score score ;
	char * s;
	int indexCoupJoue; 

	while(l.nb > 0) {
		// On indique à qui c'est le tour 
		if (p.trait == JAU) kill(tournoi.parties[numP].pidJ, SIGUSR1);
		else kill(tournoi.parties[numP].pidR, SIGUSR1);
		
		// On attend 30s
		sleep(delaiChoixCoup);

		// On lui dit d'arrêter en lui réenvoyant un signal
		// On récupère son coup et on le joue 
		if (p.trait == JAU) {
			kill(tournoi.parties[numP].pidJ, SIGUSR1);
			s = lireFichierJoueur(tournoi.parties[numP].pidJ); 
		}
		else {
			kill(tournoi.parties[numP].pidR, SIGUSR1);
			s = lireFichierJoueur(tournoi.parties[numP].pidR);
		}

		//printf("Fic = %s\n",s);
		//printf("Nb = %d\n",l.nb);
		indexCoupJoue = atoi(s);
		//printf("Coup converti = %d\n", indexCoupJoue);
 
		if ((indexCoupJoue>= 0) && (indexCoupJoue<l.nb)) {
			whoami(numP); printf("Coup %d\n", tournoi.parties[numP].nbCoups);
			whoami(numP); printf("%s a choisi le coup %d\n",COLNAME(p.trait), indexCoupJoue );
			whoami(numP); printf("Soit : %d -> %d\n", l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination);
			

			tournoi.parties[numP].feuille.coups[tournoi.parties[numP].nbCoups].origine = l.coups[indexCoupJoue].origine; 
			tournoi.parties[numP].feuille.coups[tournoi.parties[numP].nbCoups].destination = l.coups[indexCoupJoue].destination; 
			tournoi.parties[numP].feuille.nb++; 
			tournoi.parties[numP].nbCoups++; 

			p = jouerCoup(p, l.coups[indexCoupJoue].origine, l.coups[indexCoupJoue].destination) ;
			l = getCoupsLegaux(p);
		} else {
			whoami(numP); printf("Saisie incorrecte ! \n"); 
			whoami(numP); printf("Fin de la partie %d\n",numP);
			return NULL; // On quitte le thread 
		}

		// On informe l'adversaire du coup en préparant un fichier pour lui 
		if (p.trait == JAU) {
			ecrireFichierJoueur(tournoi.parties[numP].pidJ,s) ;
		}
		else {
			ecrireFichierJoueur(tournoi.parties[numP].pidR,s) ;
		}

		free(s);

		// on enregistre la nouvelle position 
		writePos(bNom, p); 

		// et on recommence 
	}

	whoami(numP); printf("Partie terminée !\n");
	
	// On envoie SIGUSR2 aux deux processus
	// Il faut attendre que les joueurs accusent réception de ces signaux 
	// Pour que l'on soit sûr que la partie est bien terminée 
	kill(tournoi.parties[numP].pidJ, SIGUSR2);
	// On doit se synchroniser avec un processus, correspondant à un joueur 
	// On crée autant de sem. que de joueurs 
	printf("attente terminaison partie par joueur %d\n", tournoi.parties[numP].pidJ);
	sem_wait(&(joueurs.tab[tournoi.parties[numP].iJ].sem)); 
	printf("ok\n");


	kill(tournoi.parties[numP].pidR, SIGUSR2);
	printf("attente terminaison partie par joueur %d\n", tournoi.parties[numP].pidR);
	sem_wait(&(joueurs.tab[tournoi.parties[numP].iR].sem)); 
	printf("ok\n");

	score = evaluerScore(p);
	terminerPartie(numP,score.nbJ,score.nbJ5,score.nbR,score.nbR5);

	// On libère le sémaphore associé 
	sem_post(&(tournoi.parties[numP].sem)); 

	//listerTournoi();

	return NULL;  
}


void listerPartie(T_FeuillePartie f) {
	octet i; 
	for(i=0;i<f.nb;i++) {
		printf("%2d. %2d->%2d\n", i, f.coups[i].origine,  f.coups[i].destination); 
	}
}

void listerTournoi(){
	int i,j, r; 
	printf("------------------ TOURNOI -----------------\n");
	printf("Nb rondes : %d\n", tournoi.nbRondes);
	printf("%d parties (%d joueurs)\n", tournoi.nbParties, joueurs.nb);
	for(r=0;r< tournoi.nbRondes; r++)
	{
		printf("---- Ronde %d -----\n", r+1);
		for(i=(joueurs.nb/2) * r ;i<(joueurs.nb/2) * (r+1);i++){
			printf("%d : ", i);			
			if (tournoi.parties[i].status != TERMINE) {
				printf("[%s] - [%s]\n", tournoi.parties[i].nomJ,tournoi.parties[i].nomR);			
			} else {
				printf("[%d(%d) %s] ", tournoi.parties[i].scoreJ, tournoi.parties[i].scoreJ5, tournoi.parties[i].nomJ);
				if (tournoi.parties[i].vainqueur == EGALITE) printf("1/2 - 1/2");
				else if (tournoi.parties[i].vainqueur == JAU) printf("1 - 0");
				else printf("0 - 1");
				printf(" [%s %d(%d)]\n", tournoi.parties[i].nomR,tournoi.parties[i].scoreR, tournoi.parties[i].scoreR5);
			}
		}
	}

	for(j=0;j<joueurs.nb;j++) {
		printf("Joueur %d : %s : %.1f points\n", j, joueurs.tab[j].nom, joueurs.tab[j].nbPoints);
	}
} 

void addPartie(int idP, octet ronde, int iJ,  int iR) {
	// On fournit l'id de partie pour pouvoir les organiser chronologiquement
	tournoi.parties[idP].iJ= iJ; 
	tournoi.parties[idP].pidJ= joueurs.tab[iJ].pid; 
	strcpy(tournoi.parties[idP].nomJ, joueurs.tab[iJ].nom);

	tournoi.parties[idP].iR= iR; 
	tournoi.parties[idP].pidR= joueurs.tab[iR].pid; 
	strcpy(tournoi.parties[idP].nomR, joueurs.tab[iR].nom);

	tournoi.parties[idP].status = ATTENTE; 
	tournoi.parties[idP].ronde = ronde; 

	tournoi.parties[idP].feuille.nb = 0; 
	tournoi.parties[idP].nbCoups = 0;

	printf3("ajout partie %d entre %s et %s\n",idP,joueurs.tab[iJ].nom, joueurs.tab[iR].nom);
}

void initTournoi() {

	// On cherche deux processus n'ayant pas joué ensemble
	// Il faut etre sur de les trouver tous... 
	// On parcourt la table des joueurs 
	// On prend le premier dispo, puis on reparcourt la table des joueurs jusqu'à en trouver un autre que l'on a pas encore rencontré
	// s'il n'y en a pas ? 

	// Mieux : on peut générer la liste de toutes les parties à mener, et les jouer l'une après l'autre... 
	// Le pb est de ne pas boucler en attendant que des joueurs aient terminé... 
	// On peut attendre que toutes les parties en cours soient terminées, puis passer aux suivantes
	// Mais il faut s'assurer qu'il n'y a jamais deux parties de la même ronde exploitant le même adversaire

	// existe-t-il un algo pour cela ? 
	// On classe les joueurs, on ajoute un "exempt" pour assurer la parité
	// On utilise le principe des tournois de blitz : 
	// Une table fixe, les autres tournent autour
	// On joue deux parties à chaque fois 

	// X et 1 2 3 4 5 6 7
	// X joue 1, puis les parties se répartissent à chaque fois avec des joueurs calculés à partir de 1 : 
	// -> de manière croissante pour les premiers : 1+1, 1+2, ...
	// -> de manière décroissante pour les suivants, modulo le nombre : (n-1 + 1-1)%(n-1), (n-1 + 1-2)%(n-1)

	// X 2 3 4
	// 1 7 6 5

	// X 3 4 5
	// 2 1 7 6

	// X 4 5 6
	// 3 2 1 7

	// X 5 6 7
	// 4 3 2 1

	// X 6 7 1
	// 5 4 3 2

	// X 7 1 2
	// 6 5 4 3

	// X 1 2 3
	// 7 6 5 4

	// n joueurs =2 * (n-1) parties 
	// On crée chaque partie entre les deux mêmes joueurs même s'ils ne jouent pas leurs deux parties de suite. 
 
	tournoi.nbParties = joueurs.nb *  (joueurs.nb-1); 
	tournoi.parties = (T_Partie *) malloc( tournoi.nbParties *  sizeof(T_Partie));
	

	// premier joueur : pivot = 0
	int ronde; //ad du pivot = num ronde
	int n=joueurs.nb; //nb joueurs du tournoi (pair)
	int nbP; // nb de parties dans une ronde 
	int nextH; 
	int nextB; 
	int numP=0;

	tournoi.nbRondes = 2*(joueurs.nb-1); 

	for(ronde=1;ronde<n;ronde++) {

		//printf("\n\nronde %d\n", ronde);
		//printf("partie 0 entre 0 et %d\n",ronde);

		addPartie(numP, ronde, 0,  ronde);
		// il faut ajouter une autre partie couleurs inversées, (n-1) rondes plus tard
		// Or, il y a n/2 parties dans chaque ronde 

		addPartie((n/2)*(ronde+n-2),ronde+n-1, ronde, 0);

		numP++;

		nextH = (ronde+1 <n) ? ronde+1: 1;
		nextB = (ronde-1==0) ? n-1 : ronde-1; 

		for(nbP=1;nbP<n/2; nbP++){			
			//printf("partie %d entre %d et %d\n",nbP, nextH, nextB);
			addPartie(numP, ronde, nextH, nextB);
			addPartie((n/2)*(ronde+n-2)+nbP, ronde+n-1, nextB, nextH);

			nextH = (nextH+1 <n) ? nextH+1 : 1; 
			nextB = (nextB-1>0) ? nextB-1:n-1;

			numP++;
			 
		}
	}

}




void terminerPartie(int numP, int nbJ, int nbJ5, int nbR, int nbR5) {
	tournoi.parties[numP].status = TERMINE; 
	tournoi.parties[numP].scoreJ = nbJ; 
	tournoi.parties[numP].scoreJ5 = nbJ5; 
	tournoi.parties[numP].scoreR = nbR; 
	tournoi.parties[numP].scoreR5 = nbR5; 

	if ( nbJ > nbR) tournoi.parties[numP].vainqueur = JAU;
	else if ( nbR > nbJ) tournoi.parties[numP].vainqueur = ROU;
	else if ( nbJ5 > nbR5) tournoi.parties[numP].vainqueur = JAU;
	else if ( nbR5 > nbJ5) tournoi.parties[numP].vainqueur = ROU;
	else tournoi.parties[numP].vainqueur = EGALITE;

	if (tournoi.parties[numP].vainqueur == JAU) joueurs.tab[tournoi.parties[numP].iJ].nbPoints++;
	else if (tournoi.parties[numP].vainqueur == ROU) joueurs.tab[tournoi.parties[numP].iR].nbPoints++; 
	else {
		joueurs.tab[tournoi.parties[numP].iJ].nbPoints += 0.5; 
		joueurs.tab[tournoi.parties[numP].iR].nbPoints += 0.5; 
	}
	
}

void addJoueur(int pid, char * nom, T_StatutJoueur status){
	joueurs.tab[joueurs.nb].pid = pid; 
	strcpy(joueurs.tab[joueurs.nb].nom, nom);
	joueurs.tab[joueurs.nb].status = status;
	joueurs.tab[joueurs.nb].nbPoints = 0; 
	sem_init(&(joueurs.tab[joueurs.nb].sem),0,0);

	printf("\tAjout du joueur : %s (PID %d)\n", nom, pid);
	joueurs.nb++;

	printf("\t%d joueur(s) actuellement\n", joueurs.nb);
}


void deroute (int signal_number, siginfo_t * info, void * context)
{
	char * nom;
	FILE * fp; 
	int i;
	int j; 

	// printf("PID emetteur : %d !!\n",info->si_pid);
	switch (signal_number) {
		case SIGUSR1 : printf("\tSignal SIGUSR1 reçu.\n"); 
		// C'est une confirmation de fin d'un thread 
		// Il faut retrouver le joueur concerné dans le tableau des joueurs 
		// et incrémenter le sémaphore associé à ce joueur *
		for(i=0;i<joueurs.nb;i++){
			if (joueurs.tab[i].pid == info->si_pid) {
				printf("Signal SIGUSR1 recu depuis %d\n", info->si_pid); 
				sem_post(&(joueurs.tab[i].sem));
			}
		}
		


		break;
		case SIGUSR2 : printf("\tArrivee d'un nouveau joueur : %d\n", info->si_pid); 

			// Peut-être faudrait-il protéger cette zone contre des accès concurrents ?

			if (joueurs.nb == MAXPROCESS) {
					printf("\t\tTrop de joueurs !\n"); 
					return;
			}

			// Il faut vérifier qu'il n'existe pas déjà
			for(i=0;i<joueurs.nb;i++) {
				if (joueurs.tab[i].pid == info->si_pid) {
					printf("\t\tCe joueur est déjà référencé !\n"); 
					return;
				}
			}

			nom = lireFichierJoueur(info->si_pid); 
			addJoueur(info->si_pid, nom, ATTEND);
			free(nom);

			// il doit avoir inscrit son nom dans un fichier portant son PID
			// joueurs.tab[joueurs.nb].status = ATTEND; 
			// joueurs.nb++;

		break;
		case SIGALRM : printf("\tSignal SIGALARM reçu.\n"); break;
	}
}


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

void writePos(char * filename, T_Position p) {
	FILE * fp; 
	int i;

	T_Score s = evaluerScore(p); 


	// Le fichier ne devrait-il pas être préfixé par le PID du process ? 
	// On devrait lui faire porter le nom du groupe, passé en ligne de commandes
	fp = fopen(filename,"w"); 

	// On écrit le trait 
	fprintf(fp, "traiterJson({\n%s:%d,\n",STR_TURN,p.trait); 

	// On écrit les scores
	fprintf(fp, "%s:%d,\n",STR_SCORE_J,s.nbJ); 
	fprintf(fp, "%s:%d,\n",STR_SCORE_J5,s.nbJ5); 
	fprintf(fp, "%s:%d,\n",STR_SCORE_R,s.nbR); 
	fprintf(fp, "%s:%d,\n",STR_SCORE_R5,s.nbR5);



	// Les colonnes // attention aux "," ?
	fprintf(fp, "%s:[\n",STR_COLS);

	// première 
	fprintf(fp, "\t{%s:%d, %s:%d}",STR_NB,p.cols[0].nb, STR_COULEUR,p.cols[0].couleur); 	

	// autres
	for(i=1;i<NBCASES;i++) {
		fprintf(fp, ",\n\t{%s:%d, %s:%d}",STR_NB,p.cols[i].nb, STR_COULEUR,p.cols[i].couleur); 	
	}
	fprintf(fp,"\n]\n"); // avec ou sans "," ? 

	fprintf(fp,"});");

	fclose(fp);
}















