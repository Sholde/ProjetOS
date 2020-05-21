SRCDIR = src
OBJDIR = obj
EXE = SoD

SRC = ${wildcard ${SRCDIR}/*.c}
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

.PHONY: all lib clean conf

all: clean conf lib main

lib: ${OBJ}
	@ ar rcs ${OBJDIR}/libnm.a ${OBJ}
	
main:
	@ gcc -I. -o ${EXE} ${SRCDIR}/main.c -L${OBJDIR}/ -lnm -lm -lpthread -g3 -Wall

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ $(CC) -c -o $@ $< -g3 -Wall -lpthread
	
clean:
	@ rm -rf ${OBJDIR}/
	@ rm ${EXE}

conf:
	@ mkdir obj 2> /dev/null
