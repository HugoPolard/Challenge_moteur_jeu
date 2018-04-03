#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "avalam.h"
#include "topologie.h"



int main(int argc, char ** argv){

	char nom_fichier[50]= "./diagrammes/", temp[50] = "base.json";	//nom du fichier à créer (temp) et le chemin du dossier des diagrammes (nom_fichier)
	char description[300];				//description qui va avec l'instantanné du jeu
	char *FEN;
	T_Position pos= positionInitiale;			// l'instantanné du jeu
	int num_diag;						//le numéro de diagramme

	//Entrées des paramètres pris en ligne de commande (position et num_diag)
	if(argc != 3) {	// Si tous les paramètres nécessaires ont TOUS été saisis, on les récupère, sinon on ne fait rien
		printf("***** ERREUR ! Veuillez saisir le numéro de diagramme et le FEN en paramètres *****\n");
		return 1;
	}
	else {		printf("\nJsuis là");

		num_diag= atoi(argv[1]); // on récupère num_diag qui est une chaine de caractère
		strcpy(FEN, argv[2]); // on récupère ensuite le code FEN pour interpretation
	}
		
	// Entrées des paramètres au clavier (description et nom_fichier)
	printf("Nom du fichier à créer :\n");
	scanf("%s", temp);
	if(!strstr(temp, ".json")) strcat(temp, ".json"); // on vérifie que le nom du fichier est comme il faut
	strcat(nom_fichier, temp); //on ajoute le chemin du dossier des diagrammes avant le nom du fichier, nom_fichier contient ainsi tout le chemin du fichier
	viderBuffer();

	printf("Description de la situation de jeu : \n");
	fgets(description, sizeof description, stdin); // saisie de la description avec fgets car constituée de plusieurs mots
	forme_desc(description); // on enlève le saut de ligne en fin de ligne dû à fgets
	printf("ICI\n");
	interpretation(FEN, &(pos)); // On transforme le code FEN en un T_position (position)
	
	crea_fichier(nom_fichier,&(pos), num_diag, description); // création du fichier json avec tous les paramètres saisis
	
	printf("Fichier créé !\n");
	return 0;
}
	
void viderBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

void crea_fichier(char * filename, T_Position *p, int num_diag, char* description) {
	FILE * fp; 
	int i;

	fp = fopen(filename,"w"); 

	// On écrit le trait 
	fprintf(fp, "{\n%s:%d,\n",STR_TURN,(*p).trait); 

	// On écrit le numéro de diagramme
	fprintf(fp, "%s:%d,\n",STR_NUMDIAG, num_diag); 

	// On écrit la description du diagramme
	fprintf(fp, "%s:\"%s\",\n",STR_NOTES, description); 

	// Les colonnes // attention aux "," ?
	fprintf(fp, "%s:[\n",STR_COLS);

	// première 
	fprintf(fp, "\t{%s:%d, %s:%d}",STR_NB,(*p).cols[0].nb, STR_COULEUR,(*p).cols[0].couleur); 	

	// autres
	for(i=1;i<NBCASES;i++) {
		fprintf(fp, ",\n\t{%s:%d, %s:%d}",STR_NB,(*p).cols[i].nb, STR_COULEUR,(*p).cols[i].couleur); 	
	}
	fprintf(fp,"\n]\n"); // avec ou sans "," ? 

	fprintf(fp,"}");

	fclose(fp);
}

void forme_desc(char* chaine) {
	if(chaine[strlen(chaine)-1] == '\n')
		chaine[strlen(chaine)-1] = '\0';
}


void interpretation(char *FEN,T_Position *position){
/* On prend le FEN et on remplit le T_position avec */
	printf("LA\n");
	int aux = 0,i = 0;
do{
	aux = 0;
	if (FEN[i] < 91 && FEN[i] >64)
	{
		position->trait = 'J';
		switch(FEN[i]){
			case 85:/*U*/
			position->cols[i].nb = 1;
			position->cols[i].couleur = 1;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);
			break;
			case 68:/*D*/
			position->cols[i].nb = 2;
			position->cols[i].couleur = 1;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 84: /*T*/
			position->cols[i].nb = 3;
			position->cols[i].couleur = 1;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 81:/*Q*/
			position->cols[i].nb = 4;
			position->cols[i].couleur = 1;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 67:/*C*/
			position->cols[i].nb = 5;
			position->cols[i].couleur = 1;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
		}
	}else if (FEN[i] < 123 && FEN[i] > 96)
	{
		position->trait = 'R';
		switch(FEN[i]){
			case 117:/*u*/
			position->cols[i].nb = 1;
			position->cols[i].couleur = 2;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 100:/*d*/
			position->cols[i].nb = 2;
			position->cols[i].couleur = 2;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 116: /*t*/
			position->cols[i].nb = 3;
			position->cols[i].couleur = 2;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 113:/*q*/
			position->cols[i].nb = 4;
			position->cols[i].couleur = 2;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
			case 99:/*c*/
			position->cols[i].nb = 5;
			position->cols[i].couleur = 2;
			printf("J'ai trouve un %c donc je mets %d dans le nb et %d dans la couleur\n",FEN[i],position->cols[i].nb,position->cols[i].couleur);

			break;
		}
	}else if((FEN[i] > 48 && FEN[i] < 57)&&
			 (FEN[i+1] > 48 && FEN[i+1] < 57)){
		printf("bibi\n");
		aux = FEN[i] + FEN[i+1] - 87;
		printf("%d\n",aux );
		for (int j = 0; j < aux; ++j)
		{
			position->cols[j].nb = 1;
			position->cols[j].couleur = 0;
			printf("J'ai trouve un %d donc je mets %d dans le nb et %d dans la couleur\n",aux,position->cols[i].nb,position->cols[i].couleur);
		}

	}else if(FEN[i] > 48 && FEN[i] < 57){
		printf("coucou\n");
		aux = FEN[i] - 48;
		printf("%d\n",aux );
		for (int j = 0; j < aux; ++j)
		{
			position->cols[j].nb = 1;
			position->cols[j].couleur = 0;
			printf("J'ai trouve un %d donc je mets %d dans le nb et %d dans la couleur\n",aux,position->cols[i].nb,position->cols[i].couleur);
		}

	}
	i++;
}while(FEN[i] != '\0');



	

}