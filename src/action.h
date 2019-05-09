
typedef struct {
	info_t*						info;				//< Carte navale
	liste_t*					liste;				//< liste
	int							n;					//< entier
	message_t* 					m;					//< struct message du bateau
	pthread_mutex_t*			mutex_attack;		//< mutex attaque
	pthread_mutex_t*			mutex_move;			//< mutex déplacement
	pthread_t 					tid;				//< id du thread
} serveur_t;

// Vérifie si les coord sont dans la map
// \param		namp		Carte navale
// \param		a			coordonné cible
// \return					1 si a est dans la map, 0 sinon
int inMap(
	navalmap_t* nmap,
	coord_t a);

// Action non-action
// \param		info		struct info (voir info.h)
// \param		shipID		ID du bateau
void NON(
	info_t* 							info,
	const int 							shipID,
	message_t* 							m);

// Retourn 1 il n'y a pas de bateau sur la case cible ou si le bateau sur la case cible bouge peut bouger dans le même tour, 0 sinon
int bouge (
	info_t								* info,
	const int							shipID,
	const coord_t						target,
	liste_t*							liste,
	int									n);
	
// Calcul simplement les coord pour se déplacer
// \param		maPos				position du joueur
// \param		maTarget			position de l' ennemie
// \return							coord du moveVec
coord_t calcul_moveVec(
	coord_t maPos,
	coord_t maTarget);

// Thread MOV
void* MOV(
	void* arg);

// Thread ATK
void* ATK(
	void* arg);
	
// Thread radar
void* SCN (
	void* arg);

// Thread repere
void* RPR (
	void* arg);


// Thread BST
void* BST (
	void* arg);
