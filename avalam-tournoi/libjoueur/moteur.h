#include <semaphore.h>

#define PATH_RUN "./run"
#define PATH_WEB "./web"
#define PID_FILE "moteur.pid"
#define MAXLEN 100
#define MAXPROCESS 256
#define DELAICHOIXCOUP 1

typedef enum statusJ {ATTEND,JOUE} T_StatutJoueur; 
typedef enum statusP {ATTENTE,ENCOURS,TERMINE} T_StatutPartie; 
typedef enum etat {ATTENTE_APPARIEMENT,ATTENTE_DEBUT, ATTENTE_TOUR_ADVERSAIRE, MON_TOUR} T_Etat;

typedef unsigned char octet; 

typedef struct {
	int pid; 
	char nom[MAXLEN];
	T_StatutJoueur status;
	float nbPoints;
	sem_t sem; // new 10/04 
} T_ProcessJoueur;

typedef struct {
	octet nb;
	T_ProcessJoueur tab[MAXPROCESS];
} T_TabJoueurs; 

typedef struct {
	int nb; 
	T_Coup coups[48];// normalement 48-48/5
} T_FeuillePartie; 

typedef struct {
	char ihm[MAXLEN];
	int pidJ; char nomJ[MAXLEN]; int iJ; 
	int pidR; char nomR[MAXLEN]; int iR; 
	T_StatutPartie status;
	octet scoreJ; octet scoreJ5; 
	octet scoreR; octet scoreR5; 
	octet vainqueur; // JAU(1) ou ROU(2) ou EGALITE(0)
	octet ronde;
	sem_t sem; 
	pthread_t thread;
	octet nbCoups; 
	T_FeuillePartie feuille; 
	// Ajouter le nom du fichier json associé a cette partie, peut-être <index>.json ?
	// Il faudrait lui associer une liste de coups joués 
}	T_Partie;

typedef struct {
	octet nbRondes;
	octet nbParties;
	T_Partie * parties;
} T_TabParties; 


void deroute(int, siginfo_t *, void *);

void addJoueur(int pid, char * nom, T_StatutJoueur status); 
void initTournoi(); 
void listerTournoi(); 

void addPartie(int idP, octet ronde, int iJ,  int iR); 
void listerPartie(T_FeuillePartie f); 
void listerParties();
void terminerPartie(int numP, int nbJ, int nbJ5, int nbR, int nbR5);
void * thread_partie(void * i);

char * lireFichierJoueur(int pid); 
void ecrireFichierJoueur(int pid, char * s); 
void ecrireCoup(octet origine, octet destination); 
void ecrireIndexCoup(int index);
void writePos(char * filename, T_Position p) ; 

 



