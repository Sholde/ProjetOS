#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "navalmap.h"
#include "info.h"
#include "calcul.h"
#include "action.h"

void libere_pere(info_t* info, int** tube) {
	for(int j = 0; j < (info->nmap->nbShips + 1); j++)
	{
		close(tube[j][0]);
		close(tube[j][1]);
		free(tube[j]);
	}
	free(tube);
	free_info(info);
}

void envoie_info_fils(info_t* info, int** tube, message_t* m) {
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		m[j].enVie = 1;
		m[j].tourMax = info->nbTours;
		m[j].maPos = info->nmap->shipPosition[j];
		m[j].monID = j;
		m[j].monKerosen = info->shipKerosen[j];
		m[j].maCoque = info->shipCoque[j];
		m[j].maTarget = to_coord(-1, -1);
		if(m[j].maTarget.x == -1 || m[j].maTarget.y == -1)
			m->dist = -1;
		else
			m->dist = calcul_distance(m[j].maTarget, m[j].maPos);
		m[j].monAction = '0';
		m[j].servTube = tube[info->nmap->nbShips];
		m[j].monTube = tube[j];
		write(tube[j][1], m+j, sizeof(message_t));
	}
}

void verif_enVie(info_t* info, message_t* m, int nbShipsEnVie1, int* nbShipsEnVie2) {
	for(int i = 0; i < nbShipsEnVie1; i++)
	{
		m [i] .maCoque = info->shipCoque[ m[i].monID ];
		m [i] .monKerosen = info->shipKerosen[ m[i].monID ];
		
		if(m[i].enVie && (info->shipCoque[m[i].monID] <= 0 || info->shipKerosen[m[i].monID] <= 0))
		{
			(*nbShipsEnVie2)--;
			info->nmap->map [info->nmap->shipPosition[ m[i].monID ].y] [info->nmap->shipPosition[ m[i].monID ].x] .type = ENT_SEA;
			info->nmap->map [info->nmap->shipPosition[ m[i].monID ].y] [info->nmap->shipPosition[ m[i].monID ].x] .id = -1;
			m [i] .enVie = 0;
			m [i] .maCoque= -1;
			m [i] .monKerosen = -1;
			m [i] .monAction = 'x';
			m [i] .maPos.x = -1;
			m [i] .maPos.y = -1;
		}
	}
}

void liste_survivant(info_t* info, message_t* m, int nbShipsEnVie, int nbTeamEnVie) {
	printf("\n###############################\n");
	printf("\nFIN DE LA PARTIE !!!\n");
	printf("\nListe survivant :\n");
	int tmp = 0;
	for(int j = 0; j < nbShipsEnVie; j++)
	{
		if(m[j].enVie)
		{
			printf("  - J%d (team %d) avec %3.dC et %3.dK\n", m[j].monID, info->equipe[m[j].monID], m[j].maCoque, m[j].monKerosen);
			tmp++;
		}
	}
	if(!tmp)
		printf("  - aucun survivant\n");
	
	if(nbTeamEnVie == 1)
	{
		for(int i = 0; i < info->nb_equipe; i++)
		{
			int nbShipsEnVie = 0;
			for(int j = 0; j < info->nmap->nbShips; j++)
			{
				if(info->equipe[m[j].monID] == i && m[j].enVie)
				{
					nbShipsEnVie++;
				}
			}
			if(nbShipsEnVie)
			{
				printf("\nL'équipe %d gagne !!!\n", i);
				i = info->nb_equipe;
			}
		}
	}
	else
	{
		printf("\nAucune team gagne !!!\n");
	}
}

int verif_teamVie(info_t* info, message_t* m) {
	int equipe_enVie = 0;
	for(int i = 0; i < info->nb_equipe; i++)
	{
		int nbShipsEnVie = 0;
		for(int j = 0; j < info->nmap->nbShips; j++)
		{
			if(info->equipe[m[j].monID] == i && m[j].enVie)
			{
				nbShipsEnVie++;
			}
		}
		if(nbShipsEnVie)
		{
			equipe_enVie++;
		}
	}
	return equipe_enVie;
}			

int compte_action (message_t* m, int n, char c) {
	int res = 0;
	for(int j = 0; j < n; j++)
	{
		if(m[j].monAction == c && m[j].enVie)
		{
			res++;
		}
	}
	return res;
}

void serveur_gere_attaque (serveur_t* s, int nbShipsEnVie) {
	info_t* info = s->info;
	message_t* m = s->m;
	if( compte_action(m, info->nmap->nbShips, 'a') )
		printf("\n   Attaque :\n");
				
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		if(m[j].monAction == 'a' && m[j].enVie)
		{
			pthread_create (&(s [j] .tid), NULL, ATK, &(s[j]));
		}
	}
	
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		if(m[j].monAction == 'a' && m[j].enVie)
		{
			pthread_join ( s [j] .tid, NULL);
		}
	}
	verif_enVie(info, m, info->nmap->nbShips, &nbShipsEnVie);
}

void serveur_gere_deplacement (serveur_t* s, info_t* info, message_t* m, int nbShipsEnVie) {
	liste_t* liste = NULL;
	int n = compte_action(m, info->nmap->nbShips, 'm') + compte_action(m, info->nmap->nbShips, 'b');
		
	if( n > 0)
	{
		printf("\n   Déplacement :\n");
			
		liste = malloc( n * sizeof(liste_t) );
		int k = 0;
		for(int i = 0; i < info->nmap->nbShips; i++)
		{
			if( m[i].monAction == 'm' && m[i].enVie)
			{
				liste [k] .id = m [i] .monID;
				liste [k] .target = calcul_moveVec( m [i] .maPos, m [i] .maTarget);
				k++;
			}
			if( m[i].monAction == 'b' && m[i].enVie)
			{
				liste[k].id = m[i].monID;
				liste[k].target = m[i].maTarget;
				k++;
			}
		}
	}
		
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		// MOV
		if(m[j].monAction == 'm' && m[j].enVie)
		{	
			s [j] .n = n;
			s [j] .liste = liste;
			pthread_create (&(s [j] .tid), NULL, MOV, &(s[j]) );
		}
			
		// BST
		if(m[j].monAction == 'b' && m[j].enVie)
		{		
			s [j] .n = n;
			s [j] .liste = liste;
			pthread_create (&(s [j] .tid), NULL, BST, &(s[j]) );
		}
	}
	
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		if( (m[j].monAction == 'm' || m[j].monAction == 'b') && m[j].enVie)
		{
			pthread_join ( s [j] .tid, NULL);
			s [j] .n = 0;
			s [j] .liste = NULL;
		}
	}
	free(liste);
		
	verif_enVie(info, m, info->nmap->nbShips, &nbShipsEnVie);
}

void serveur_gere_support (serveur_t* s, int nbShipsEnVie) {
	if( compte_action(s->m, s->info->nmap->nbShips, 'r') + compte_action(s->m, s->info->nmap->nbShips, 'e') )
		printf("\n   Support :\n");
	
	for(int j = 0; j < s->info->nmap->nbShips; j++)
	{
		// Radar
		if(s->m[j].monAction == 'r' && s->m[j].enVie)
		{
			pthread_create (&(s [j] .tid), NULL, SCN, &(s[j]));
		}
			
		// Repair
		if(s->m[j].monAction == 'e' && s->m[j].enVie)
		{
			pthread_create (&(s [j] .tid), NULL, RPR, &(s[j]));
		}
	}
	
	for(int j = 0; j < s->info->nmap->nbShips; j++)
	{
		if( (s->m[j].monAction == 'r' && s->m[j].enVie) 
			|| (s->m[j].monAction == 'e' && s->m[j].enVie) )
		{
			pthread_join (s [j] .tid, NULL);
		}
	}

	verif_enVie(s->info, s->m, s->info->nmap->nbShips, &nbShipsEnVie);
}

void renvoie_info_fils (info_t* info, message_t* m, int nbTeamEnVie) {
	if(nbTeamEnVie <= 1)
	{
		for(int j = 0; j < info->nmap->nbShips; j++)
		{
			m[j].monAction = 'q';
			write(m[j].monTube[1], m+j, sizeof(message_t));
		}
	}
	else
	{
		for(int j = 0; j < info->nmap->nbShips; j++)
		{
			m[j].maCoque = info->shipCoque[m[j].monID];
			m[j].monKerosen = info->shipKerosen[m[j].monID];
			write(m[j].monTube[1], m+j, sizeof(message_t));
		}
	}
}

void serveur(info_t* info, int** tube) {
	// Traitement
	
	message_t* m = malloc((info->nmap->nbShips) * sizeof(message_t));
	serveur_t* s = malloc (info->nmap->nbShips * sizeof(serveur_t));
	pthread_mutex_t atk, dep;
	pthread_mutex_init (&atk, NULL);
	pthread_mutex_init (&dep, NULL);
	
	for(int j = 0; j < info->nmap->nbShips; j++)
		{
			s [j] .info = info;
			s [j] .m = NULL;
			s [j] .n = 0;
			s [j] .liste = NULL;
			s [j] .mutex_attack = &atk;
			s [j] .mutex_move = &dep;
		}
	// Envoie info
	envoie_info_fils(info, tube, m);
	
	// Jeu
	int nbShipsEnVie = info->nmap->nbShips;
	int nbTeamEnVie = info->nb_equipe;
	
	for(int tours = 0; tours < info->nbTours && nbTeamEnVie > 1; tours++)
	{
		printf("\n---------------------------------\n");
		printf("\nTours %d :\n", tours+1);
		
		
		// Récupère info
		for(int j = 0; j < info->nmap->nbShips; j++)
		{
			read(tube[info->nmap->nbShips][0], m+j, sizeof(message_t));
		}
		
		// Initialise la structure serveur
		for(int j = 0; j < info->nmap->nbShips; j++)
		{
			s [j] .m = &(m[j]);
		}
		
		// ATTACK
		serveur_gere_attaque (s, nbShipsEnVie);
		
		// Déplacement
		serveur_gere_deplacement (s, info, m, nbShipsEnVie);
		
		// Support
		serveur_gere_support (s, nbShipsEnVie);
		
		// Compte nb de team en vie
		nbTeamEnVie = verif_teamVie(info, m);
		
		// Renvoie info au fils	
		renvoie_info_fils (info, m, nbTeamEnVie);
	}
	
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		m[j].monAction = 'q';
		write(m[j].monTube[1], m+j, sizeof(message_t));
	}
	
	
	verif_enVie(info, m, info->nmap->nbShips, &nbShipsEnVie);
	nbTeamEnVie = verif_teamVie(info, m);
	
	// Attends ses fils
	for(int j = 0; j < 1; j++)
	{
		wait(NULL);
	}
	
	liste_survivant(info, m, info->nmap->nbShips, nbTeamEnVie);
	
	printf("\n");
	pthread_mutex_destroy (&atk);
	pthread_mutex_destroy (&dep);
	free(s);
	free(m);
	libere_pere(info, tube);
}
