#include <stdio.h>   /* Permet d'utiliser printf pour afficher du texte */
#include <stdlib.h>  /* Bibliothèque standard du C, utile pour certaines fonctions générales */
#include <time.h>    /* Permet d'utiliser time(NULL) pour initialiser l'aléatoire */

#include "constants.h" /* Contient les constantes du jeu : tailles, TRUE/FALSE, nombre de joueurs... */
#include "type.h"      /* Contient les structures principales comme Game et Position */
#include "game.h"      /* Contient les fonctions principales du jeu : initialisation, tour, victoire... */
#include "display.h"   /* Contient les fonctions d'affichage du plateau */
#include "input.h"     /* Contient les fonctions de saisie sécurisée */
#include "save.h"      /* Contient les fonctions de sauvegarde et de chargement */


/* MENU PRINCIPAL */

/*
    Cette fonction affiche le menu principal.

    Elle permet au joueur de choisir entre :
    - commencer une nouvelle partie
    - reprendre une partie sauvegardée
    - quitter le programme

    Elle retourne :
    - 1 si le joueur veut commencer une nouvelle partie
    - 2 si le joueur veut charger une sauvegarde
    - 3 si le joueur veut quitter
*/
static int main_menu(void)
{
    printf("===== CY-CONNECT =====\n"); /* Affiche le titre du jeu dans le menu */
    printf("1. Nouvelle partie\n"); /* Option pour créer une nouvelle partie */
    printf("2. Reprendre votre dernière partie sauvegardee\n"); /* Option pour charger save.txt */
    printf("3. Quitter\n"); /* Option pour quitter le programme */

    /*
        read_integer sécurise la saisie.

        Ici, le joueur doit obligatoirement entrer :
        - 1
        - 2
        - ou 3

        Cela évite qu'il entre une valeur impossible.
    */
    return read_integer("Votre choix : ", 1, 3);
}


/* CRÉATION D'UNE NOUVELLE PARTIE */

/*
    Cette fonction prépare une nouvelle partie.

    Elle demande d'abord le nombre de joueurs,
    puis elle initialise la structure Game.
*/
static void start_new_game(Game *game)
{
    int player_count; /* Stocke le nombre de joueurs choisi */

    /*
        On demande le nombre de joueurs.

        Les limites MIN_PLAYERS et MAX_PLAYERS sont définies dans constants.h.
        Cela permet de modifier facilement les règles sans changer ce fichier.
    */
    player_count = read_integer("Nombre de joueurs entre 2 et 3 : ",
                                MIN_PLAYERS,
                                MAX_PLAYERS);

    /*
        initialize_game prépare la partie :
        - nombre de joueurs
        - joueur actuel
        - numéro du tour
        - plateau vide avec blocs dans les coins
    */
    initialize_game(game, player_count);
}


/* AFFICHAGE DU RÉSULTAT */

/*
    Cette fonction affiche le résultat final de la partie.

    winners[] contient les numéros des joueurs gagnants.
    winner_count indique combien de joueurs ont gagné.

    Il peut y avoir plusieurs gagnants en même temps,
    car une rotation peut créer plusieurs alignements.
*/
static void display_result(const Game *game, int winners[], int winner_count)
{
    int i; /* Sert à parcourir le tableau des gagnants */

    /*
        Cas simple :
        un seul joueur a gagné.
    */
    if (winner_count == 1)
    {
        printf("\nVictoire du joueur %d (%c) !\n",
               winners[0],
               get_player_symbol(winners[0])); /* Affiche le numéro et le symbole du gagnant */
    }
    else
    {
        /*
            Cas particulier :
            plusieurs joueurs gagnent après la même rotation.

            Dans ce cas, le jeu annonce une égalité entre ces joueurs.
        */
        printf("\nEgalite entre plusieurs joueurs : ");

        for (i = 0; i < winner_count; i++) /* Parcourt tous les gagnants */
        {
            printf("Joueur %d (%c)",
                   winners[i],
                   get_player_symbol(winners[i])); /* Affiche chaque gagnant */

            if (i < winner_count - 1) /* Si ce n'est pas le dernier gagnant */
            {
                printf(", "); /* Ajoute une virgule entre les joueurs */
            }
        }

        printf("\n"); /* Retour à la ligne après la liste des gagnants */
    }

    printf("\nPartie terminee en %d tours.\n", game->turn_number); /* Affiche le nombre total de tours */
}


/* BOUCLE PRINCIPALE DU JEU */

/*
    Cette fonction contient la boucle principale d'une partie.

    Tant que la partie n'est pas terminée :
    1. le joueur actuel joue son tour
    2. on vérifie si un joueur a gagné
    3. si personne n'a gagné, on vérifie si le plateau est plein
    4. sinon, on passe au joueur suivant
*/
static void game_loop(Game *game)
{
    int winners[MAX_PLAYERS]; /* Tableau qui stocke les joueurs gagnants */
    int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]; /* Tableau qui indique les cases gagnantes à surligner */
    int winner_count; /* Nombre de gagnants trouvés */
    int finished; /* Indique si la partie est terminée */
    int turn_result; /* Résultat du tour : TRUE si joué, FALSE si sauvegardé/quitté */

    finished = FALSE; /* Au départ, la partie n'est pas terminée */

    while (!finished) /* Continue tant que la partie n'est pas finie */
    {
        turn_result = play_turn(game); /* Joue le tour complet du joueur actuel */

        if (turn_result == FALSE) /* Si le joueur a sauvegardé et quitté pendant son tour */
        {
            printf("\nLa partie a ete sauvegardee. Vous pourrez la reprendre plus tard.\n");
            return; /* Quitte la boucle et retourne au menu/programme */
        }

        winner_count = check_winners(game, winners); /* Vérifie s'il y a un ou plusieurs gagnants */

        if (winner_count > 0) /* Si au moins un joueur a gagné */
        {
            get_winning_cells(game, winning_cells); /* Repère les cases de l'alignement gagnant */

            clear_screen(); /* Nettoie l'écran pour afficher clairement le résultat */
            printf("\n✨✨✨ ALIGNEMENT DETECTE ! ✨✨✨\n"); /* Message visuel de victoire */
            display_game_with_winning_cells(game, winning_cells); /* Affiche le plateau avec les cases gagnantes */
            display_result(game, winners, winner_count); /* Affiche le ou les gagnants */
            finished = TRUE; /* La partie est terminée */
        }
        else if (is_board_full(game)) /* Si personne n'a gagné mais que le plateau est plein */
        {
            clear_screen(); /* Nettoie l'écran */
            display_game(game); /* Affiche le plateau final */
            printf("\nMatch nul : plus aucune colonne jouable.\n"); /* Message d'égalité */
            finished = TRUE; /* La partie est terminée */
        }
        else
        {
            change_player(game); /* Passe au joueur suivant */
        }
    }
}


/* FONCTION PRINCIPALE */

/*
    main est la première fonction exécutée quand le programme démarre.

    Elle sert à :
    - initialiser l'aléatoire
    - afficher le menu
    - lancer une nouvelle partie
    - ou charger une sauvegarde
*/
int main(void)
{
    Game game; /* Structure qui contient toute la partie */
    int choice; /* Choix du menu principal */
    int loaded; /* Indique si le chargement de sauvegarde a réussi */

    /*
        srand initialise le générateur aléatoire.

        time(NULL) permet d'utiliser l'heure actuelle.
        Grâce à ça, la rotation imposée 3x3 ou 5x5
        ne suit pas toujours la même suite à chaque lancement.
    */
    srand((unsigned int) time(NULL));

    clear_screen(); /* Efface l'écran au lancement pour avoir un affichage propre */

    choice = main_menu(); /* Affiche le menu et récupère le choix du joueur */

    if (choice == 1) /* Si le joueur choisit une nouvelle partie */
    {
        start_new_game(&game); /* Prépare une nouvelle partie */
        game_loop(&game); /* Lance la boucle principale du jeu */
    }
    else if (choice == 2) /* Si le joueur choisit de reprendre une sauvegarde */
    {
        loaded = load_game(&game); /* Tente de charger la partie sauvegardée */

        if (loaded) /* Si le chargement a réussi */
        {
            game_loop(&game); /* Reprend la partie */
        }
        else /* Si aucune sauvegarde valide n'a pu être chargée */
        {
            printf("Impossible de charger la sauvegarde.\n");
        }
    }
    else /* Si le joueur choisit de quitter */
    {
        printf("Fin du programme.\n"); /* Message de fermeture */
    }

    return 0; /* Indique que le programme s'est terminé correctement */
}
