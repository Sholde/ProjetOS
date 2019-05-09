

// Desaloc la mémoire alloué
// \param 	info
// \param 	tube
void libere_pere(
	info_t* info,
	int** tube);

// Si le bateau n' a plus de coque ou de kerosen, il meurt
// \param 		info				struct info
// \param		tube				tube anonyme
// \param 		m 					struct message contenant les info de tous les bateaux
void envoie_info_fils(
	info_t* info,
	int** tube,
	message_t* m);

// Si le bateau n' a plus de coque ou de kerosen, il meurt
// \param 		info				struct info
// \param 		m 					struct message contenant les info de tous les bateaux
// \param		nbShipsEnVie1 		nb de ship en vie avant que le tour commence
// \param		nbShipsEnVie2 		actualise le nb de ship en vie a chaque fois qu' un bateau est mort
void verif_enVie(
	info_t* info,
	message_t* m,
	int nbShipsEnVie1,
	int* nbShipsEnVie2);

// Liste les bateau qui ont survécu
// \param		info				struct info
// \param 		m					struct message contenant les info de tous les bateaux
// \param		nbShipsEnVie		Nombre de bateau en vie
void liste_survivant(
	info_t* info,
	message_t* m,
	int nbShipsEnVie);

// Calcul le nombre d'équipe en vie
// \param		info				struct info
// \param		m					struct message contenant les info de tous les bateaux
// \return							Nombre d'équipe en vie
int verif_teamVie(
	info_t* info,
	message_t* m);

// Compte le nombre d'action d' un même groupe qui vont s' effectuer
// \param 		m					struct message contenant les info de tous les bateaux
// \param		n					nombre totale de bateau
// \param		c					action
// \return							nombre de l'action c qui va s'effectuer
int compte_action (
	message_t* m,
	int n,
	char c);
	

void serveur_gere_attaque (
	serveur_t* s,
	int nbShipsEnVie);

void serveur_gere_deplacement (
	serveur_t* s,
	info_t* info,
	message_t* m,
	int nbShipsEnVie);

void serveur_gere_support (
	serveur_t* s,
	int nbShipsEnVie);
	
void renvoie_info_fils (
	info_t* info,
	message_t* m,
	int nbTeamEnVie);

// Gere le serveur
// \param		info				struct info
// \param		tube				tube anonyme
void serveur(
	info_t* info,
	int** tube);
