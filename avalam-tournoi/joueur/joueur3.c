/********* Moteur de tournoi : joueur ***************/

#include <stdio.h>
#include "avalam.h"
#include "moteur.h"


void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups) {
	// Cette fonction peut appeler la fonction ecrireIndexCoup(coupChoisi);
	// Pour sélectionner l'index d'un coup à jouer dans la liste l 
	int boule = 0, compteur = 0;
	int coup= 0;
	while(boule == 0) {
		if(currentPosition.cols[listeCoups.coups[compteur].origine].couleur 
		   == currentPosition.trait) {
		   	if(currentPosition.cols[listeCoups.coups[compteur].destination].couleur 
		   != currentPosition.trait)
			boule = 1;
			coup= compteur;
		}
		compteur++;
	}
	ecrireIndexCoup(coup); // dernier coup 
}
