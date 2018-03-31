#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "avalam.h"
#include "topologie.h"

void viderBuffer();
void crea_fichier(char * filename, T_Position p, int num_diag, char* description) ;
void forme_desc(char* description);

int main(int argc, char ** argv){

	char nom_fichier[50]= "./diagrammes/", temp[50] = "base.json";	//nom du fichier à créer (temp) et le chemin du dossier des diagrammes (nom_fichier)
	char description[300];				//description qui va avec l'instantanné du jeu
	char fen[100];
	T_Position position= positionInitiale;			// l'instantanné du jeu
	int num_diag;						//le numéro de diagramme

	//Entrées des paramètres pris en ligne de commande (position et num_diag)
	if(argc != 3) {	// Si tous les paramètres nécessaires ont TOUS été saisis, on les récupère, sinon on ne fait rien
		printf("***** ERREUR ! Veuillez saisir le numéro de diagramme et le FEN en paramètres *****\n");
		return 1;
	}
	else {
		num_diag= atoi(argv[1]); // on récupère num_diag qui est une chaine de caractère
		strcpy(fen, argv[2]); // on récupère ensuite le code fen pour interpretation
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
	
	//interpretation(fen, position); // On transforme le code fen en un T_position (position)
	
	crea_fichier(nom_fichier, position, num_diag, description); // création du fichier json avec tous les paramètres saisis
	
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

void crea_fichier(char * filename, T_Position p, int num_diag, char* description) {
	FILE * fp; 
	int i;

	fp = fopen(filename,"w"); 

	// On écrit le trait 
	fprintf(fp, "{\n%s:%d,\n",STR_TURN,p.trait); 

	// On écrit le numéro de diagramme
	fprintf(fp, "%s:%d,\n",STR_NUMDIAG, num_diag); 

	// On écrit la description du diagramme
	fprintf(fp, "%s:\"%s\",\n",STR_NOTES, description); 

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

void forme_desc(char* chaine) {
	if(chaine[strlen(chaine)-1] == '\n')
		chaine[strlen(chaine)-1] = '\0';
}