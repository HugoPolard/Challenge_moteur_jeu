#include <stdio.h>
#include <string.h>

int main (/*char * FEN, T_position position*/){
	//par convention on met le trait en premier et les positions après
	//il reste a integrer le trait dans position et mettre dans la bonne forme les chiffres (voir poly cours)
	//pour le moment on défini FEN et position mais ils seront en paramètres plus tard
	int i, j;
	int ctoi, ctoi2, ctoidiz;
	char FEN[50];
	char trait;
	char position[500];
	position[0]='\0';
	printf("Donner les positions en FEN \n");
	scanf("%s", FEN);
	trait = FEN[0];
	if(trait == 114){ //tour du rouge
		strcat(position,"'trait':2,\n");
	}else{ 
		if(trait == 106){ //tour du jaune
			strcat(position, "'trait':1,\n");
		}
	}
	for(i=1; i<strlen(FEN); i++){
		ctoi = FEN[i];
		//on récupère toutes les positions et on fait des cas
		if(ctoi > 47 && ctoi < 58){
		//si c'est un chiffre 
			ctoi2 = FEN[i+1];
			if(ctoi2 > 47 && ctoi2 < 58){
			//si c'est un nombre
				ctoidiz = FEN[i] + FEN[i+1] - 87;
				printf("%d \n", ctoidiz);
				for(j=1; j<=ctoidiz; j++){
					strcat(position, "{'nb':1, 'couleur':0},\n");
				}
				i++;
			}else{
				ctoi -= 48;
				printf("%d \n", ctoi);
				for(j=1; j<=ctoi; j++){
					strcat(position, "{'nb':1, 'couleur':0},\n");
				}
			}
		}else{
			switch(ctoi){
			//si c'est les lettres
				case 85:
					strcat(position, "{'nb':1, 'couleur':2},\n");
					break;
				case 68:
					strcat(position, "{'nb':2, 'couleur':2},\n");
					break;
				case 84:
					strcat(position, "{'nb':3, 'couleur':2},\n");
					break;
				case 81:
					strcat(position, "{'nb':4, 'couleur':2},\n");
					break;
				case 67:
					strcat(position, "{'nb':5, 'couleur':2},\n");
					break;
				case 117:
					strcat(position, "{'nb':1, 'couleur':1},\n");
					break;
				case 100:
					strcat(position, "{'nb':2, 'couleur':1},\n");
					break;
				case 116:
					strcat(position, "{'nb':3, 'couleur':1},\n");
					break;
				case 113:
					strcat(position, "{'nb':4, 'couleur':1},\n");
					break;
				case 99:
					strcat(position, "{'nb':5, 'couleur':1},\n");
					break;
				default:
					printf("ERREUR");
					break;
			}
		}
	}
	printf("\n\n %s \n\n", position);
	//je suis pas sur de la syntaxe de position, faudra voir avec la fonction d'Hugo
	

	// position initale jUuUuUuuUuUuUUuUuUuUuUuUuuUuUuUuUuUuUUuUuUuuUuUuU
}
	
