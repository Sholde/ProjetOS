
// Stocke les info du fichier a ouvrir
typedef struct {
	int fd;						//< contient le descripteur de fichier
	int acces;					//< contient l'accès au fichier 
	const char* path;			//< contient le nom du fic
}fichier;

// Ouvre le fichier fichier
// \param 		chemin 			chemin du fichier
// \return 						fichier
fichier ouverture_du_fichier(
	const char* chemin);

// Copie le fichier dans une chaine de caractere
// \param 		f 				fichier contenant l'acces et le descripteur
// \param 		caractere 		chaine ou sera copié le fichier
void lire_fichier(
	fichier f,
	char* caractere);

// Vérifie si le prochain caractère est un nombre ou un lettre minuscule suivant deb et fin
// \param		caractere 		chaine ou est copié le fichier
// \param		tete_lecture	indique la position a lire
// \param		deb				caractere de debut
// \param		fin				caractere de fin
// \return						1 si le caractere est compris entre deb et fin, 0 sinon
int verif(
	char* caractere,
	int* tete_lecture,
	int deb,
	int fin);

// Récupère une chaine de caractere
// \param 		caractere 		chaine ou se trouve le fichier copié
// \param 		charType 		chaine ou sera sauvgarder la chaine de caractere souhaité
// \param 		tete_lecture 	indice de lecture à partir du quel on lit le fichier
// \return						-1 si le type de carte entrée est incorrect, 0 sinon
int lire_chaine(
	char* caractere,
	char* charType,
	int* tete_lecture);

// Récupère un entier 
// \param 		caractere 		chaine ou se trouve le fichier copié
// \param 		tete_lecture 	indice a partir duquel on lit dans caractere
// \return 						-1 si ce n'est pas un entier, sinon entier res ou se trouve l'entier extrait 
int lire_entier(
	char* caractere,
	int* tete_lecture);

// Ferme le fichier
// \param 		f 				fichier ou se trouve le descripteur du fichier à fermée
void fermeture(
	fichier f);

// Compare les deux chaines de caractere
// \param		a				1ere chaine
// \param		b				2eme chaine
// \return						0 si les deux chaines sont différentes, 1 sinon
int compare_type(
	char* a,
	char* b);

// Cette fonction récupère les données dans le fichier 
// \param 		path 			chemin du fichier à charger
// \return 						struct info (voir info.h)
info_t* lecture_fichier(
	char* path);
