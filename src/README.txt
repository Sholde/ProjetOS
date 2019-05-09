########
## Contenu de l'archive
########

L'archive est constituée des fichiers suivants :
- un fichier README.txt
- un fichier Makefile pour la compilation de la bibliothèque
- des fichiers d'en-tête contenant les prototypes des fonctions à utiliser, la structure de données ainsi que leur documentation
- des fichiers source

########
## Compilation de la bibliothèque
########

La bibliothèque nm se compile à l'aide de la commande 'make lib'. Une archive libnm.a sera alors créée et vous devrez la lier à votre projet

########
## Compilation de votre programme
########

Pour compiler votre programme en liant la bibliothèque vous devez :
1) vous assurer que les fichiers d'en-tête de la bibliothèque sont accessibles (option -I de gcc)
2) vous assurer que la bibliothèque est accessible (option -L de gcc)
3) lier la bibliothèque à votre programme (option -l de gcc)

En supposant que vous compiliez votre programme dans '~' que la bibliothèque se trouve dans '~/lib' et les fichiers d'en-tête dans '~/lib/include', vous devrez exécuter la commande suivante :
$ gcc -o prog -I./lib/include main.c -L./lib -lnm

########
## Autres questions
########

Pour toute autre question ou bug à l'utilisation de la bibliothèque, envoyez les à l'adresse sebastien.gougeaud@uvsq.fr avec en pièce jointe les informations nécessaires à une réponse ou à une résolution rapide de votre problème (morceaux de code utilisés, erreurs obtenus, etc.)
