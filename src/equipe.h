
//< Structure envoyés lors de la création des threads
typedef struct {
	int id;							//< id du bateau
	int fd;							//< tube de lecture du bateau
	int nbTeam;						//< nombre de joueur de l'équipe du bateau
	coord_t* posAdv;				//< tableau des position ennemie trouvés par l'équipe
	pthread_mutex_t* mutex;			//< mutex
} equipe_t;


// Temporaire
void libere_fils(
	info_t* info,
	int** tube,
	int n);


// Thread qui gere un joueur
// \param		arg			entier
// \return					NULL
void* bateau (
	void* arg);


// Fonction qui gère l'équipe et créé les thread
// \param		info		struct info (voir info.h)
// \param		tube		tube anonyme
// \param		j			numéro de l'équipe
void equipe(
	info_t* info,
	int** tube,
	int j) ;
