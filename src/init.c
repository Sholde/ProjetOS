#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "navalmap.h"
#include "info.h"
#include "calcul.h" // tmp
#include "fichier.h"

void verif_argument(int argc) {
	if(argc != 2 ){
		printf("ERREUR:ARGUMENT NON VALIDE:essayer ./Sod fichier\n");
		exit(1);
	}
}

info_t* initialisation_de_la_carte(char* path)
{
	initNavalMapLib ();
	info_t* info = lecture_fichier(path);
	placeRemainingShipsAtRandom(info->nmap);
	return info;
}

int** init_tube(navalmap_t* nmap) {
	int** a = malloc((nmap->nbShips + 1) * sizeof(int*));
	for(int i = 0; i < (nmap->nbShips + 1); i++)
	{
		a[i] = malloc(2 * sizeof(int));
		pipe(a[i]);
	}
	return a;
}
