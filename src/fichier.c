// open, close ...
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h> // exit();
#include <stdio.h> // printf();

#include "navalmap.h"
#include "info.h"
#include "calcul.h"
#include "fichier.h"


#define taille_fichier 256
#define taille_typeMap 9
#define taille_entier 3
#define limite_taille_map 30
#define limite_nbTours 30
#define limite_joueur 99

fichier ouverture_du_fichier(const char* chemin) {
	fichier f;
	f.path = chemin;
	f.acces = O_RDONLY;
	f.fd = open(f.path, f.acces);
	if(!f.fd)
	{
		printf("ERREUR:erreur ouverture !\n");
		exit(1);
	}
	else
		return f;
}

void lire_fichier(fichier f, char* caractere) {
	read(f.fd, caractere, taille_fichier - 1);
}

int verif(char* caractere, int* tete_lecture, int deb, int fin) {
	char c;
	c = caractere[*tete_lecture];
	if(c - deb < 0 || c - fin > fin - deb)
	{
		return 0;
	}
	return 1;
}

int lire_chaine(char* caractere, char* charType, int* tete_lecture) {
	
	int j = 0;
	if(!verif(caractere, tete_lecture, 'a', 'z'))
			return -1;
	
	while( j < taille_typeMap && caractere[*tete_lecture] != ';')
	{
		if(!verif(caractere, tete_lecture, 'a', 'z'))
			return -1;
		
		charType[j] = caractere[*tete_lecture];
		j++;
		*tete_lecture = *tete_lecture + 1;
	}
	return 0;
}

int lire_entier(char* caractere, int* tete_lecture)
{
	int j = 0;
	char* entier;
	entier = calloc(taille_entier + 1, sizeof(char));
	*tete_lecture = *tete_lecture + 1;

	if(!verif(caractere, tete_lecture, '0', '9'))
		return -1;
	
	while( j < taille_entier 
		&& caractere[*tete_lecture] != ';' 
		&& caractere[*tete_lecture] != '\n')
	{
		if(!verif(caractere, tete_lecture, '0', '9'))
			return -1;
		
		entier[j] = caractere[*tete_lecture];
		j++;
		*tete_lecture = *tete_lecture + 1;
	}
	
	int res = atoi(entier);
	free(entier);
	return res;
}


void fermeture(fichier f)
{
	if(close(f.fd) == -1)
	{
		printf("ERREUR:échec de fermeture :/\n");
		exit(1); 
	}
}

int compare_type(char* a, char* b)
{
	int i;
	for(i = 0; i < taille_typeMap; i++)
	{
		if(a[i] != b[i])
		{
			return 0;
		}
	}
	return 1;
}

info_t* lecture_fichier(char* path)
{
	coord_t size;
	map_t mapType;
	int nbJoueur, coque, kerosen, nbTours, tete_lecture; 
	
	fichier f;
	char charType[taille_typeMap+1] = { '\0' };
	char caractere[taille_fichier] = { '\0' };
	
	tete_lecture = 0;
	
	f = ouverture_du_fichier(path);
	lire_fichier(f, caractere);
	fermeture(f);
	
	if(lire_chaine(caractere, charType, &tete_lecture) == -1)
	{
		printf("\nArgument non valide :\n - Le nb de caractere du type de la map doit être > 0 et < %d\n - Les entiers doivent être compris entre 1 et %d\n", taille_fichier, puissance(10, taille_entier)-1 );
		exit(1);
	}
	char* rectangle = "rectangle";
	
	if(compare_type(charType, rectangle))
	{
		mapType = MAP_RECT;
	}
	else
	{
		mapType = MAP_TOTAL;
	}
	
	size.x = lire_entier(caractere, &tete_lecture);
	size.y = lire_entier(caractere, &tete_lecture);
	if(size.x > limite_taille_map || size.y > limite_taille_map)
		printf("\nErreur :Les dimension de la map ne doivent pas dépasser %dx%d\n", limite_taille_map, limite_taille_map);
	
	nbJoueur = lire_entier(caractere, &tete_lecture);
	if( size.x*size.y < nbJoueur )
		printf("Erreur :\nLe nombre de joueur est supérieur au nombre de case de la map\n");
	if( nbJoueur > limite_joueur)
		printf("Erreur :\nLimite de joueur à %d\n", limite_joueur);
	
	// Autre structure
	coque = lire_entier(caractere, &tete_lecture);
	kerosen = lire_entier(caractere, &tete_lecture);
	
	nbTours = lire_entier(caractere, &tete_lecture);
	if(nbTours > limite_nbTours)
		printf("\nErreur :Le nombre de tours doit être < à %d\n", limite_nbTours);
	
	
	if(size.x <= 0 || size.y <= 0 || nbJoueur <= 0 || coque <= 0 || kerosen <= 0 || nbTours <= 0)
	{
		printf("\nArgument du fichier non valide :\n - Le nb de caractere du type de la map doit être > 0 et < %d\n - Les entiers doivent être compris entre 1 et 999\n", taille_fichier);
		exit(1);
	}
	
	if(caractere[++tete_lecture] != '\0')
	{
		printf("\nArgument non valide :\n - Le nb de caractere du type de la map doit être > 0 et < %d\n - Les entiers doivent être compris entre 1 et %d\n", taille_fichier, puissance(10, taille_entier)-1 );
		exit(1);
	}
	
	navalmap_t* nmap = init_navalmap(mapType, size, nbJoueur);
	info_t* info = init_info(nmap, coque, kerosen, nbTours);
	
	return info;
}
