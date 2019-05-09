#include <pthread.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "navalmap.h"
#include "info.h"
#include "calcul.h"
#include "action.h"// temporaire
#include "serveur.h" // temporaire jutse pour desalloc a la fin
#include "equipe.h" 


void libere_fils (info_t* info, int** tube, int n) {
	for(int j = 0; j < info->nmap->nbShips; j++)
	{
		if(j != n)
		{
			close( tube [j] [0] );
			close( tube [j] [1]);
			free( tube [j] );
		}
	}
	free_info(info);
}

void* bateau (void* arg) {
	// Traitement
	equipe_t* argTeam = (equipe_t*) arg;
	
	// recup info
	message_t tmp;
	message_t* m = &tmp;
	read(argTeam->fd, &tmp, sizeof(message_t));
	
	// thread
	
	int radar = 0;
	
	for(int tours = 0; tours < m->tourMax; tours++)
	{
		pthread_mutex_lock (argTeam->mutex);
		
		if( m->maTarget.x == -1 || m->maTarget.y == -1 )
		{
			for(int i = 0; i < argTeam->nbTeam; i++)
			{
				if( argTeam->posAdv [i].x == -1 || argTeam->posAdv [i].y == -1 )
				{
					int dist = calcul_distance( argTeam->posAdv [i], m->maPos );
					if( dist > 1 && dist < 5 )
					{
						m->maTarget = argTeam->posAdv [i];
					}
				}
			}
		}
		
		pthread_mutex_unlock (argTeam->mutex);
		
		if(!m->enVie)
		{
			m->monAction = 'n';
			m->typeAction = 'n';
		}
		else if(m->maCoque < 30 && m->monKerosen > 20)
		{
			m->monAction = 'e';
			m->typeAction = 's';
			radar++;
		}
		else if(m->maTarget.x == -1 || m->maTarget.y == -1 || radar > 1)
		{
			m->monAction = 'r';
			m->typeAction = 's';
			radar = 0;
		}
		else if(m->dist < 6 && m->dist > 3 
			&& (m->maTarget.x == m->maPos.x || m->maTarget.y == m->maPos.y) )
		{
			m->monAction = 'b';
			m->typeAction = 'm';
			radar++;
		}
		else if(m->dist < 5 && m->dist > 1)
		{
			m->monAction = 'a';
			m->typeAction = 'a';
			radar++;
		}
		else
		{
			m->monAction = 'm';
			m->typeAction = 'm';
			radar++;
		}
		write(m->servTube[1], m, sizeof(message_t));
		read(m->monTube[0], m, sizeof(message_t));
		
		pthread_mutex_lock (argTeam->mutex);
		argTeam->posAdv [argTeam->id] = m->maTarget;
		pthread_mutex_unlock (argTeam->mutex);

		if(m->monAction == 'q')
			return NULL;
	}
	return NULL;
}

void equipe(info_t* info, int** tube, int j) {
	
	pthread_t tid [ info->nbShips_equipe[j] ];
	
	equipe_t* argTeam = malloc ( info->nbShips_equipe[j] * sizeof(equipe_t) );
	pthread_mutex_t mutex;
	pthread_mutex_init (&mutex, NULL);
	
	coord_t* tmp = malloc ( info->nbShips_equipe[j] * sizeof(equipe_t) );
	for(int i = 0; i < info->nbShips_equipe[j]; i++)
	{
		tmp[i].x = -1; tmp[i].y = -1;
	}
	
	int k = 0;
	for(int i = 0; i < info->nmap->nbShips; i++)
	{
		if(info->equipe[i] == j)
		{
			argTeam [k] .id = k;
			argTeam [k] .nbTeam = info->nbShips_equipe[j];
			argTeam [k] .fd = tube[i][0];
			argTeam [k] .posAdv = tmp;
			argTeam [k] .mutex = &mutex;
			pthread_create (&(tid[k]), NULL, bateau, &( argTeam[k] ) );
			k++;
		}
	}
	
	k = 0;
	for(int i = 0; i < info->nmap->nbShips; i++)
	{
		if(info->equipe[i] == j)
		{
			pthread_join (tid[k], NULL);
			k++;
		}
	}
	
	pthread_mutex_destroy (&mutex);
	free (argTeam);
	libere_pere (info, tube);
}
