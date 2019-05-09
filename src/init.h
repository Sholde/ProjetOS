
// Vérifie si il y a 1 arguments après ./SoD
// \param 		argc 		Nombre d' argument
void verif_argument(
	int argc);


// Initialise la carte
// \param		path		Chaine correspondant au fichier à lire
// \return					La structure initialisé
info_t* initialisation_de_la_carte(
	char* path);


// Créé les tubes pour communiquer entre le serveur et les bateau
// \param		nmap		Carte navale
// \return					Addresse des tubes
int** init_tube(
	navalmap_t* nmap);
