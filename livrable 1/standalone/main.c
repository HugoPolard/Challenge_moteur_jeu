#include <stdio.h>
#include <string.h>
#include "avalam.h"
#include "topologie.h"
/* ***** A Faire
	reconnaitre la fin de partie et afficher le score
	paramétrer le nom du data.json
*/

void writePos(char * filename, T_Position p) ; 

int main(int argc, char ** argv){
	int numCase, numCaseD;
	char nom_fichier[50]= "../../avalam-web/", temp[50]; // chaines pour le nom de fichier, temp pour le nom du fichier sans aller dans avalam-web et nom-fichier prend d'abord le chemin d'avalam-web puis le chemin total
	int i = 0; //compteur pour savoir si on est au premier tour ou non
	T_Score s ; 

	T_Position p = getPositionInitiale();
	//afficherPosition(p);
	
	T_ListeCoups l = getCoupsLegaux(p); //liste des coups legaux
	//afficherListeCoups(l);

	while(l.nb > 0) {

		printf("caseO ? : "); 
		scanf("%d",&numCase);

		printf("caseD ? : "); 
		scanf("%d",&numCaseD);

		printf("On joue %d -> %d\n", numCase, numCaseD); 
		p = jouerCoup(p, numCase,numCaseD); 
		//afficherPosition(p);

		if(i == 0) {
			if(argc == 1) {
				strcpy(nom_fichier, "../../avalam-web/data.json");
			}
			else {
				strcpy(temp, argv[1]);
				if(!strstr(temp, ".json")) strcat(temp, ".json"); // on vérifie que le nom du fichier est comme il faut
				strcat(nom_fichier, temp);	
				
			}
		}
		writePos(nom_fichier, p);		
			
		s = evaluerScore(p); 
		afficherScore(s);

		l = getCoupsLegaux(p);
		//afficherListeCoups(l);
		i++;
	}

}

void writePos(char * filename, T_Position p) {
	FILE * fp; 
	int i;

	T_Score s = evaluerScore(p); 


	// Le fichier ne devrait-il pas être préfixé par le PID du process ? 
	// On devrait lui faire porter le nom du groupe, passé en ligne de commandes
	fp = fopen(filename,"w"); 

	// On écrit le trait 
	fprintf(fp, "{\n%s:%d,\n",STR_TURN,p.trait); 

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

	fprintf(fp,"}");

	fclose(fp);
}







