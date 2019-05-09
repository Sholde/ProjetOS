
// Structure principale
typedef struct {
	navalmap_t* nmap;			//< Carte navale
	int* shipKerosen;			//< Kerosen des bateaux
	int Kmax;					//< Kerosen Max
	int Cmax;					//< Coque Max
	int* shipCoque;				//< coque des bateaux
	int nbTours;				//< Nombre de tour au plus à jouer
	int* equipe;				//< Equipe des bateau
	int nb_equipe;				//< Nombre d'équipe
	int* nbShips_equipe;		//< Nombre de bateau par équipe
}info_t;

// Initialise la structure info
// \param		nmap			Carte navale
// \param		coque			Coque 
// \param		kerosen			Kerosen
// \param		nbTours			Nombre de tour
// \return						structure info initialisé
info_t* init_info(
	navalmap_t* nmap,
	int coque,
	int kerosen,
	int nbTours);

// Désalloc la structure info
// \param		info			struct info
void free_info(
	info_t* info);

// Structure message qui permet de communiquer entre serveur et bateau
typedef struct {
	coord_t maPos;				//< Position du bateau
	int monKerosen;				//< Kerosen du bateau
	int maCoque;				//< Coque du bateau
	coord_t maTarget;			//< Cible du bateau
	char monAction;				//< Action prochain tour
	char typeAction;			//< Type d'action prochain tour
	int monID;					//< ID du bateau
	int* monTube;				//< tube du bateau (lis)
	int* servTube;				//< tube du serveur (écris)
	int enVie;					//< 1 si il est en vie, 0 sinon
	int dist;					//< distance de la pos du bateau et de la cible
	int tourMax;				//< nb de tours max à jouer
}message_t;


typedef struct {
	int id;
	coord_t target;
} liste_t;
