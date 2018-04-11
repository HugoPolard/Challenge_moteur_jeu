#define FAUX 0
#define VRAI 1

#define EGALITE 0
#define VIDE 0
#define JAU 1
#define ROU 2

// Pour les exports JSON
#define STR_NB "\"nb\""
#define STR_COULEUR "\"couleur\""
#define STR_TURN "\"trait\""
#define STR_COLS "\"cols\""

#define STR_SCORE_J "\"scoreJ\""
#define STR_SCORE_J5 "\"scoreJ5\""
#define STR_SCORE_R "\"scoreR\""
#define STR_SCORE_R5 "\"scoreR5\""

#define STR_NUMDIAG "\"numDiag\""
#define STR_NOTES "\"notes\""


//#define __DEBUG__

#ifdef __DEBUG__
	#define printf0(p) printf(p)
	#define printf1(p,q) printf(p,q)
	#define printf2(p,q,r) printf(p,q,r)
	#define printf3(p,q,r,s) printf(p,q,r,s)
#else
	#define printf0(p)
	#define printf1(p,q)
	#define printf2(p,q,r)
	#define printf3(p,q,r,s)
#endif

//verif appels systèmes 

#define CHECK_IF(sts,val,msg) \
if ((sts) == (val)) {perror(msg); exit(-1); }

#define CHECK_DIF(sts,val,msg) \
if ((sts) != (val)) {perror(msg); exit(-1); }

#define NBCASES 48

#define COLNAME(c) ((c==ROU) ? "rouge" : "jaune")

typedef unsigned char octet; 

typedef struct {
	octet nbJ; 	octet nbJ5; // total et piles de 5
	octet nbR;	octet nbR5;
} T_Score;

typedef struct {
	octet nb; 
	octet cases[8]; // il peut y en avoir moins... ils vaudront 0
} T_Voisins; 

typedef struct {
	octet nb; 
	octet couleur; // couleur du sommet 
} T_Colonne; 

typedef struct { // Un tableau de colonnes
	octet trait; 
	// octet numCoup; // A ajouter
	T_Colonne cols[NBCASES];
} T_Position;

typedef struct {
	octet origine; 
	octet destination;
} T_Coup;

typedef struct {
	int nb; 
	T_Coup coups[8*NBCASES]; 
} T_ListeCoups; 

octet nbVoisins(octet numCase); 
T_Voisins getVoisins(octet numCase); 
void listerVoisins(octet numCase); 
T_Position getPositionInitiale();
void afficherPosition(T_Position p); 
void addCoup(T_ListeCoups * pL, octet origine, octet destination);
void afficherListeCoups(T_ListeCoups l);
T_Position jouerCoup(T_Position p, octet origine, octet destination) ;
T_ListeCoups getCoupsLegaux(T_Position p) ; 
T_Score evaluerScore(T_Position p);
void afficherScore(T_Score s); 

