#include <stdlib.h>
#include <time.h>

#include "navalmap.h"
#include "info.h"

info_t* init_info(navalmap_t* nmap, int coque, int kerosen, int nbTours) {
	int nbShips = nmap->nbShips;
	info_t* info = malloc(sizeof(info_t));
	info->shipCoque = malloc(nbShips * sizeof(int));
	info->shipKerosen = malloc(nbShips * sizeof(int));
	info->equipe = malloc(nbShips * sizeof(int));
	
	srand( time(NULL) );
	if(nbShips <= 1)
	{
		info->nb_equipe = 1;
	}
	else if(nbShips == 2)
	{
		info->nb_equipe = 2;
	}
	else
	{
		info->nb_equipe = rand()%(nbShips-2) + 2;
	}
	info->nbShips_equipe = malloc(info->nb_equipe * sizeof(int));
	for(int j = 0; j < info->nb_equipe; j++)
		info->nbShips_equipe[j] = 0;
	
	for(int j = 0; j < nbShips; j++)
	{
		info->equipe[j] = j % info->nb_equipe;
		info->nbShips_equipe[ info->equipe[j] ]++;
		info->shipCoque[j] = coque;
		info->shipKerosen[j] = kerosen;
	}
	
	info->Kmax = kerosen;
	info->Cmax = coque;
	info->nmap = nmap;
	info->nbTours = nbTours;
	return info;
}

void free_info(info_t* info) {
	free_navalmap(info->nmap);
	free(info->nbShips_equipe);
	free(info->equipe);
	free(info->shipCoque);
	free(info->shipKerosen);
	free(info);
}
