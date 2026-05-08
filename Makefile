# ============================================================
# Makefile du projet CY-Connect
# ============================================================
# Ce fichier sert à compiler automatiquement le projet.
#
# Commandes utiles :
#   make        -> compile le programme
#   make run    -> compile puis lance le programme
#   make clean  -> supprime les fichiers de compilation
# ============================================================


# Compilateur utilisé
CC = gcc

# Options de compilation
# -Wall   : affiche les avertissements importants
# -Wextra : affiche encore plus d’avertissements
# -Werror : transforme les avertissements en erreurs
# -std=c99 : utilise la norme C99 du langage C
CFLAGS = -Wall -Wextra -Werror -std=c99

# Nom du programme final
TARGET = cy-connect

# Liste des fichiers .c du projet
SOURCES = main.c game.c display.c input.c save.c

# Transformation automatique des fichiers .c en fichiers .o
# Exemple :
# main.c devient main.o
OBJECTS = $(SOURCES:.c=.o)


# ============================================================
# Règle principale
# ============================================================
# Quand on tape simplement "make", cette règle est exécutée.
# Elle demande la création du programme final.
all: $(TARGET)


# ============================================================
# Création du programme final
# ============================================================
# Pour créer cy-connect, il faut d’abord créer tous les fichiers .o.
# Ensuite, on les rassemble pour fabriquer l’exécutable.
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)


# ============================================================
# Compilation des fichiers .c en fichiers .o
# ============================================================
# Cette règle dit comment transformer n’importe quel fichier .c
# en fichier .o.
#
# Exemple :
# main.c  -> main.o
# game.c  -> game.o
# save.c  -> save.o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Lancer le jeu
# ============================================================
# make run compile le projet puis lance le programme.
run: all
	./$(TARGET)


# ============================================================
# Nettoyer le dossier
# ============================================================
# make clean supprime :
# - les fichiers .o
# - le programme final cy-connect
clean:
	rm -f $(OBJECTS) $(TARGET)


# ============================================================
# Aide
# ============================================================
# make help affiche les commandes disponibles.
help:
	@echo "Commandes disponibles :"
	@echo "  make        : compile le projet"
	@echo "  make run    : compile puis lance le jeu"
	@echo "  make clean  : supprime les fichiers de compilation"
	@echo "  make help   : affiche cette aide"
