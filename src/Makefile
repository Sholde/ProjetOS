BIBLIO.c = navalmap.c nm_rect.c
BIBLIO.o = navalmap.o nm_rect.o

MES_FUNC.c = fichier.c info.c action.c serveur.c equipe.c init.c calcul.c
MES_FUNC.o = fichier.o info.o action.o serveur.o equipe.o init.o calcul.o

.PHONY: all lib debug

all: lib

lib:
	gcc -g -c -fPIC ${BIBLIO.c} ${MES_FUNC.c} -lpthread
	ar rcs libnm.a ${BIBLIO.o} ${MES_FUNC.o}

debug:
	gcc -g -o prog main.c -L. -lnm
	
main:
	gcc -I. -o SoD main.c -L. -lnm -lm -lpthread -g3 -Wall
	
clean:
	rm *.o
	rm *.a

