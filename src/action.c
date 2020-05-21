#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "navalmap.h"
#include "nm_rect.h"
#include "info.h"
#include "calcul.h"
#include "action.h"

int inMap(navalmap_t* nmap, coord_t a) {
	return (a.x >= 0
		 && a.x < nmap->size.x
		 && a.y >= 0
		 && a.y < nmap->size.y);
}

void NON(
	info_t* info,
	const int shipID,
	message_t* m){
	
	if(!(info->shipKerosen[shipID]<=0)) 
	{
		printf("\t- J%d (team %d)    NON\n", shipID, info->equipe[shipID]);
		info->shipKerosen[shipID] -= 1;
		m->monKerosen -= 1;
	}
	else
	{
		printf("\t- J%d (team %d) mort\n", shipID, info->equipe[shipID]);
	}
}

int bouge (
	info_t								* info,
	const int							shipID,
	const coord_t						target,
	liste_t*							liste,
	int									n) {
	if( !(inMap (info->nmap, target) ) )
		return 0;
		
	if(info->nmap->map [target.y] [target.x].type == ENT_SHIP)
	{
		int id = info->nmap->map [target.y] [target.x].id;
		for(int i = 0; i < n; i++)
		{
			if(id == liste[i].id)
			{
				return bouge(info, id, liste[i].target, liste, n);
			}
			else
				return 1;
		}
		
	}
	return 0;
}

coord_t calcul_moveVec(coord_t maPos, coord_t maTarget) {
	coord_t moveVec;
	moveVec.x = 0; moveVec.y = 0;
	if(maPos.x - maTarget.x < 0)
		moveVec.x = 1;
	if(maPos.x - maTarget.x > 0)
		moveVec.x = -1;
	if(maPos.y - maTarget.y < 0)
		moveVec.y = 1;
	if(maPos.y - maTarget.y > 0)
		moveVec.y = -1;
	return moveVec;
}

void* MOV(
	void* arg) {
	
	serveur_t* s = (serveur_t*) arg;
	
	info_t * info = s->info;
	int shipID = s->m->monID;
	coord_t maTarget  = s->m->maTarget;
	message_t* m = s->m;
	int n = s->n;
	liste_t* liste = s->liste;
	
	coord_t moveVec = calcul_moveVec(m->maPos, maTarget);
	coord_t tmp;
	tmp.x = info->nmap->shipPosition [shipID].x + moveVec.x;
	tmp.y = info->nmap->shipPosition [shipID].y + moveVec.y;
	
	int distance = calcul_distance(info->nmap->shipPosition[shipID], tmp);
	
		
	if (!(info->nmap->isMovePossible (info->nmap, shipID, moveVec))
		|| !( distance > 0 && distance < 3)
		|| info->shipKerosen[shipID] < 2)
	{
		NON(info, shipID, m);
	}
	else
	{
		info->shipKerosen[shipID] -= 2;
		int id = -1;
		int autre_veux_bouger = 0;
		coord_t target;
		target.x = tmp.x; target.y = tmp.y; 
		int ennemie_target = bouge(info, shipID, target, liste, n);
		
		if(info->nmap->map [target.y] [target.x].type == ENT_SHIP)
		{
			id = info->nmap->map [target.y] [target.x].id;
		}
		for(int i = 0; i < n; i++)
		{
			if(i != m->monID && liste[i].target.x == target.x && liste[i].target.y == target.y)
			{
				autre_veux_bouger = 1;
			}
		}
		
		if(id == -1 || !ennemie_target)
		{
			
			if(!autre_veux_bouger)
			{
				// LOCK
				pthread_mutex_lock (s->mutex_move);
				
				moveShip(info->nmap, shipID, moveVec);
				printf("\t- J%d (team %d)    MOV (%d,%d) -> se déplace\n", shipID, info->equipe[shipID], target.x, target.y);
				m->maPos = info->nmap->shipPosition[m->monID];
				if(m->maTarget.x != -1 && m->maTarget.y != -1)
					m->dist = calcul_distance(m->maPos, m->maTarget);
				else
					m->dist = -1;
				
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK
			}
			else if (autre_veux_bouger)
			{
				// LOCK
				pthread_mutex_lock (s->mutex_move);
				
				info->shipCoque [shipID] -= 10;
				
				printf("\t- J%d (team %d)    MOV (%d,%d) ->", shipID, info->equipe[shipID], info->nmap->shipPosition[shipID].x, info->nmap->shipPosition[shipID].y);
				
				if(info->shipCoque [shipID] <= 0 || info->shipKerosen [shipID] <= 0)
					printf(" J%d (team %d) s'est suicidé\n", shipID, info->equipe[shipID]);
				else
					printf(" J%d (team %d) -10 C\n",  shipID, info->equipe[shipID]);
					
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK
			}
		}
		else
		{
				// LOCK
				pthread_mutex_lock (s->mutex_move);

				info->shipCoque [shipID] -= 5;
				info->shipCoque [id] -= 5;
				printf("\t- J%d (team %d)    MOV (%d,%d) ->", shipID, info->equipe[shipID], info->nmap->shipPosition[shipID].x, info->nmap->shipPosition[shipID].y);
				
				if(info->shipCoque [shipID] <= 0 || info->shipKerosen [shipID] <= 0)
					printf(" J%d (team %d) s'est suicidé", shipID, info->equipe[shipID]);
				else
					printf(" J%d (team %d) -5 C",  shipID, info->equipe[shipID]);
					
				printf(" |");
				if(info->shipCoque [id] <= 0)
					printf(" J%d (team %d) est mort\n", id, info->equipe[id]);
				else
					printf(" J%d (team %d) -5 C\n", id, info->equipe[id]);
					
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK

		}
	}
	return NULL;
}

void* ATK(
	void* arg) {
		
	serveur_t* s = (serveur_t*) arg;
	
	info_t * info = s->info;
	int shipID = s->m->monID;
	coord_t target  =s->m->maTarget;
	message_t* m = s->m;
		
	int distance =  calcul_distance(info->nmap->shipPosition[shipID], target);
	int shipTargetID;
	coord_t tmp = target;
	int nbCible = 0;
	
	if( info->shipKerosen[shipID] >= 5
		&& (distance > 1 && distance < 5) )
	{
		printf("\t- J%d (team %d)    ATK (%d,%d) ->", shipID, info->equipe[shipID], target.x, target.y);

		if(inMap(info->nmap, tmp) && info->nmap->map[target.y][target.x].type == ENT_SHIP)
		{
			shipTargetID = info->nmap->map[target.y][target.x].id;
			
			if(info->equipe[shipTargetID] != info->equipe[shipID])
			{
				// LOCK
				pthread_mutex_lock (s->mutex_attack);
				
				info->shipCoque[shipTargetID] -= 40;
				if(info->shipCoque[shipTargetID] <= 0)
				{
					m->maTarget = to_coord(-1, -1);
					printf(" J%d (team %d) est mort", shipTargetID, info->equipe[shipTargetID]);
				}
				else
				{
					printf(" J%d (team %d) -40C", shipTargetID, info->equipe[shipTargetID]);
				}
				
				pthread_mutex_unlock (s->mutex_attack);
				// UNLOCK
				nbCible++;
			}
			else
				m->maTarget = to_coord(-1, -1);
		}
		else
			m->maTarget = to_coord(-1, -1);
		
		tmp.x = target.x + 1; 
		if(inMap(info->nmap, tmp) && info->nmap->map[tmp.y][tmp.x].type == ENT_SHIP)
		{
			shipTargetID = info->nmap->map[tmp.y][tmp.x].id;
			
			if(info->equipe[shipTargetID] != info->equipe[shipID])
			{
				// LOCK
				pthread_mutex_lock (s->mutex_attack);
				
				info->shipCoque[shipTargetID] -= 20;
				if(nbCible)
				{
					printf(" |");
				}
				if(info->shipCoque[shipTargetID] <= 0)
					printf(" J%d (team %d) est mort", shipTargetID, info->equipe[shipTargetID]);
				else
					printf(" J%d (team %d) -20C", shipTargetID, info->equipe[shipTargetID]);
				
				pthread_mutex_unlock (s->mutex_attack);
				// UNLOCK
				nbCible++;
			}
		}
		
		tmp.x = target.x - 1;
		if(inMap(info->nmap, tmp) && info->nmap->map[tmp.y][tmp.x].type == ENT_SHIP)
		{
			shipTargetID = info->nmap->map[tmp.y][tmp.x].id;
			
			if(info->equipe[shipTargetID] != info->equipe[shipID])
			{
				// LOCK
				pthread_mutex_lock (s->mutex_attack);
				
				info->shipCoque[shipTargetID] -= 20;
				if(nbCible)
				{
					printf(" |");
				}
				if(info->shipCoque[shipTargetID] <= 0)
					printf(" J%d (team %d) est mort", shipTargetID, info->equipe[shipTargetID]);
				else
					printf(" J%d (team %d) -20C", shipTargetID, info->equipe[shipTargetID]);
				
				pthread_mutex_unlock (s->mutex_attack);
				// UNLOCK
				nbCible++;
			}
		}
		tmp.x = target.x;
		
		tmp.y = target.y + 1;
		if(inMap(info->nmap, tmp) && info->nmap->map[tmp.y][tmp.x].type == ENT_SHIP)
		{
			shipTargetID = info->nmap->map[tmp.y][tmp.x].id;
			
			if(info->equipe[shipTargetID] != info->equipe[shipID])
			{
				// LOCK
				pthread_mutex_lock (s->mutex_attack);
				
				info->shipCoque[shipTargetID] -= 20;
				if(nbCible)
				{
					printf(" |");
				}
				if(info->shipCoque[shipTargetID] <= 0)
					printf(" J%d (team %d) est mort", shipTargetID, info->equipe[shipTargetID]);
				else
					printf(" J%d (team %d) -20C", shipTargetID, info->equipe[shipTargetID]);
				
				pthread_mutex_unlock (s->mutex_attack);
				// UNLOCK
				nbCible++;
			}
		}
		
		tmp.y = target.y - 1;
		if(inMap(info->nmap, tmp) && info->nmap->map[tmp.y][tmp.x].type == ENT_SHIP)
		{
			shipTargetID = info->nmap->map[tmp.y][tmp.x].id;
			
			if(info->equipe[shipTargetID] != info->equipe[shipID])
			{
				// LOCK
				pthread_mutex_lock (s->mutex_attack);
				
				info->shipCoque[shipTargetID] -= 20;
				if(nbCible)
				{
					printf(" |");
				}
				if(info->shipCoque[shipTargetID] <= 0)
					printf(" J%d (team %d) est mort", shipTargetID, info->equipe[shipTargetID]);
				else
					printf(" J%d (team %d) -20C", shipTargetID, info->equipe[shipTargetID]);
				
				pthread_mutex_unlock (s->mutex_attack);
				// UNLOCK
				nbCible++;
			}
		}

		if(!nbCible)
		{
			printf(" aucune cible touchée");
		}
		printf("\n");
		info->shipKerosen[shipID] -= 5;
	}
	else
	{
		NON(info, shipID, m);
		m->maTarget = to_coord(-1, -1);
	}
	return NULL;
}

void* SCN (void* arg) {
	serveur_t* s = (serveur_t*) arg;
	if(s->info->shipKerosen[s->m->monID] < 3)
	{
		NON(s->info, s->m->monID, s->m);
	}
	
	s->info->shipKerosen[s->m->monID] -= 3;
	
	int k;
	int nbShipsTrouver = 0, id = -1;
	int* list = NULL;
	int dist = s->info->nmap->size.x + s->info->nmap->size.y + 1;
	
	for(k = 1; k < dist; k++)
	{
		
		list = rect_getTargets( s->info->nmap, s->info->nmap->shipPosition[s->m->monID], k, &nbShipsTrouver);
		
		if(nbShipsTrouver > 0 )
		{
			for(int i = 0; i < nbShipsTrouver; i++)
			{
				if( s->info->equipe[ list[i] ] != s->info->equipe[s->m->monID] )
				{
					id = list[i];
					k = dist;
				}
			}
		}
	}
	free(list);
	
	// Affichage
	printf("\t- J%d (team %d)    SCN (%d,%d) ->", s->m->monID, s->info->equipe[s->m->monID], s->info->nmap->shipPosition[s->m->monID].x, s->info->nmap->shipPosition[s->m->monID].y);
	if(id > -1)
	{
		s->m->maTarget = s->info->nmap->shipPosition[id];
		s->m->dist = calcul_distance(s->m->maTarget, s->m->maPos);
		printf(" J%d (team %d), (%d,%d), %3.d C\n", id, s->info->equipe[id], s->info->nmap->shipPosition[id].x, s->info->nmap->shipPosition[id].y, s->info->shipCoque[id]);
	}
	else
	{
		s->m->maTarget = to_coord(-1, -1);
		s->m->dist = -1;
		printf(" aucun joueur trouvé\n");
	}
	return NULL;
}

void* RPR (void* arg) {
	serveur_t* s = (serveur_t*) arg;
	int shipID = s->m->monID;
	if( s->info->shipKerosen [shipID] < 20 )
	{
		NON(s->info, s->m->monID, s->m);
	}
	else
	{
		s->info->shipKerosen [shipID] -= 20;
		s->info->shipCoque [shipID] += 25;
		if( s->info->shipCoque [shipID] > s->info->Cmax )
			s->info->shipCoque [shipID] = s->info->Cmax;
		
		s->m->monKerosen = s->info->shipKerosen [shipID];
		s->m->maCoque = s->info->shipCoque [shipID];
		printf("\t- J%d (team %d)    RPR (%d,%d) ->", s->m->monID, s->info->equipe[shipID],  s->info->nmap->shipPosition[shipID].x, s->info->nmap->shipPosition[shipID].y);
		printf(" J%d (team %d) %3.d C\n", s->m->monID, s->info->equipe[shipID], s->info->shipCoque [shipID]);
	}
	return NULL;
}

void* BST (
	void* arg) {
	
	// Recup info
	serveur_t* s = (serveur_t*) arg;
	
	info_t * info = s->info;
	int shipID = s->m->monID;
	coord_t target  = s->m->maTarget;
	message_t* m = s->m;
	int n = s->n;
	liste_t* liste = s->liste;
	
	int dx = target.x - info->nmap->shipPosition [shipID].x;
	int dy = target.y - info->nmap->shipPosition [shipID].y;
	
	int dist = calcul_distance(target, info->nmap->shipPosition [shipID]);
	
	coord_t moveVec;
	moveVec.x = dx;
	moveVec.y = dy;
	
	if( !(info->nmap->isMovePossible (info->nmap, shipID, moveVec) )
		|| info->shipKerosen [shipID] < 3 
		|| !( !(dx == 0 && dy != 0) || !(dx != 0 && dy == 0)
		|| dist < 4 || dist > 5) )
	{
		printf("BST\n");
		NON(info, shipID, m);
	}
	else
	{
		info->shipKerosen [shipID] -= 3;
		printf("\t- J%d (team %d)    BST (%d,%d) ->", shipID, info->equipe[shipID], target.x, target.y);
		
		int id = -1;
		int autre_veux_bouger = 0;
		int ennemie_target = bouge(info, shipID, target, liste, n);
		
		if(info->nmap->map [target.y] [target.x].type == ENT_SHIP)
		{
			id = info->nmap->map [target.y] [target.x].id;
		}
		for(int i = 0; i < n; i++)
		{
			if(i != m->monID && liste[i].target.x == target.x && liste[i].target.y == target.y)
			{
				autre_veux_bouger = 1;
			}
		}
		
		if(id == -1 || !ennemie_target)
		{
			
			if(!autre_veux_bouger)
			{
				// LOCK
				pthread_mutex_lock (s->mutex_move);
				
				moveShip(info->nmap, shipID, moveVec);
				printf(" J%d (team %d) se déplace\n", shipID, info->equipe[shipID]);
				m->maPos = info->nmap->shipPosition[m->monID];
				if(m->maTarget.x != -1 && m->maTarget.y != -1)
					m->dist = calcul_distance(m->maPos, m->maTarget);
				else
					m->dist = -1;
				
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK
			}
			else if(autre_veux_bouger)
			{
				// LOCK
				pthread_mutex_lock (s->mutex_move);
				
				info->shipCoque [shipID] -= 10;
				if(info->shipCoque [shipID] <= 0 || info->shipKerosen [shipID] <= 0)
					printf(" J%d (team %d) s'est suicidé\n", shipID, info->equipe[shipID]);
				else
					printf(" J%d (team %d) -10 C\n", shipID, info->equipe[shipID]);
					
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK
			}
		}
		else
		{
				// LOCK
				pthread_mutex_lock (s->mutex_move);
				
				info->shipCoque [id] -= 60;
				info->shipCoque [shipID] -= 10;
				
				if(info->shipCoque [shipID] <= 0 || info->shipKerosen [shipID] <= 0)
					printf(" J%d (team %d) s'est suicidé", shipID, info->equipe[shipID]);
				else
					printf(" J%d (team %d) -10 C", shipID, info->equipe[shipID]);
				
				printf(" |");
				if(info->shipCoque [id] <= 0 || info->shipKerosen [id] <= 0)
					printf(" J%d (team %d) est mort\n", id, info->equipe[id]);
				else
					printf(" J%d (team %d) -60 C\n", id, info->equipe[id]);
					
				pthread_mutex_unlock (s->mutex_move);
				//UNLOCK
		}
	}
	return NULL;
}
