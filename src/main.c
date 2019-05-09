#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


// biblio
#include "navalmap.h"
#include "nm_rect.h"
#include "info.h"

#include "fichier.h"
#include "calcul.h"
#include "action.h"
#include "serveur.h"
#include "equipe.h"
#include "init.h"

int main (int argc, char** argv) {
	
	verif_argument(argc);
	info_t* info = initialisation_de_la_carte(argv[1]);
	printf("\nInfo carte :\n  - size = %d x %d\n  - nb Joueur = %d\n  - nb équipe = %d\n",
		info->nmap->size.x, info->nmap->size.y, info->nmap->nbShips, info->nb_equipe);
	
	// Création tube
	int** tube = init_tube(info->nmap);
	pid_t pid[info->nb_equipe];
	
	for(int j = 0; j < info->nb_equipe; j++)
	{
		pid[j] = fork();
		if(!pid[j])
		{
			equipe(info, tube, j);
			
			return 0;
		}
	}
	
	serveur(info, tube);
	
	return 0;
}
