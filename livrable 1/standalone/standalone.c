#include "standalone.h"

int main() {
	int partie_fini = 0; //booléen pour dire si la partie est finie -> boucle
	int joueur= 1; // 1 ou 2 pour le joueur qui peut jouer

	printf("******  JEU DE L'AVALAM  ********\n");
	printf("Debut de la partie\n");					//introduction

	while(partie_fini)
	{
		printf("tour du joueur %d\n", joueur);
		printf("--position d'origine : \n");
		scanf("%d", &pos_origine);
		printf("--position d'arrivée : \n");
		scanf("%d", &pos_arrivee);
	}
	printf("Debut de la partie\n");
<<<<<<< HEAD:standalone.c
}
=======
}
>>>>>>> Alexandre:livrable 1/standalone/standalone.c
