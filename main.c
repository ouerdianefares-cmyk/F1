#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "types.h"
#include "game.h"
#include "display.h"
#include "input.h"
#include "save.h"

/*
    Cette fonction affiche le menu principal.

    Elle retourne :
    1 si le joueur veut commencer une nouvelle partie
    2 si le joueur veut charger une sauvegarde
    3 si le joueur veut quitter
*/
static int main_menu(void) {
    printf("===== CY-CONNECT =====\n");
    printf("1. Nouvelle partie\n");
    printf("2. Reprendre une partie sauvegardee\n");
    printf("3. Quitter\n");

    /*
        read_integer demande une saisie à l'utilisateur.
        Ici, il doit entrer un nombre entre 1 et 3.
    */
    return read_integer("Votre choix : ", 1, 3);
}

/*
    Cette fonction prépare une nouvelle partie.
*/
static void start_new_game(Game *game) {
    int player_count;

    /*
        On demande le nombre de joueurs.
        Le joueur doit choisir entre 2 et 3.
    */
    player_count = read_integer("Nombre de joueurs entre 2 et 3 : ",
                                MIN_PLAYERS,
                                MAX_PLAYERS);

    /*
        On initialise la partie avec ce nombre de joueurs.
    */
    initialize_game(game, player_count);
}

/*
    Cette fonction affiche le résultat de la partie.

    winners[] contient les joueurs gagnants.
    winner_count indique combien il y a de gagnants.
*/
static void display_result(const Game *game, int winners[], int winner_count) {
    int i;

    /*
        S’il y a un seul gagnant, on affiche sa victoire.
    */
    if (winner_count == 1) {
        printf("\nVictoire du joueur %d (%c) !\n",
               winners[0],
               get_player_symbol(winners[0]));
    } else {
        /*
            S’il y a plusieurs gagnants en même temps,
            le jeu annonce une égalité.
        */
        printf("\nEgalite entre plusieurs joueurs : ");

        for (i = 0; i < winner_count; i++) {
            printf("Joueur %d (%c)",
                   winners[i],
                   get_player_symbol(winners[i]));

            if (i < winner_count - 1) {
                printf(", ");
            }
        }

        printf("\n");
    }

    printf("\nPartie terminee en %d tours.\n", game->turn_number);
}

/*
    Cette fonction contient la boucle principale du jeu.

    Tant que la partie n’est pas terminée :
    - le joueur joue son tour
    - on vérifie s’il y a un gagnant
    - sinon on passe au joueur suivant
*/
static void game_loop(Game *game) {
    int winners[MAX_PLAYERS];
    int winner_count;
    int finished;

    finished = FALSE;

    while (!finished) {
        /*
            Le joueur actuel joue un tour complet :
            choix colonne, chute, rotation, gravité.
        */
        play_turn(game);

        /*
            Après le tour, on vérifie s’il y a un ou plusieurs gagnants.
        */
        winner_count = check_winners(game, winners);

        if (winner_count > 0) {
            /*
                Il y a au moins un gagnant.
                La partie se termine.
            */
            clear_screen();
            display_game(game);
            display_result(game, winners, winner_count);
            finished = TRUE;
        } else if (is_board_full(game)) {
            /*
                Si le plateau est plein et qu’il n’y a pas de gagnant,
                la partie se termine sur une égalité.
            */
            clear_screen();
            display_game(game);
            printf("\nMatch nul : plus aucune colonne jouable.\n");
            finished = TRUE;
        } else {
            /*
                Aucun gagnant et le plateau n’est pas plein.
                On passe au joueur suivant.
            */
            change_player(game);
        }
    }
}

/*
    Fonction principale du programme.

    C’est ici que le programme commence.
*/
int main(void) {
    Game game;
    int choice;
    int loaded;

    /*
        srand initialise l’aléatoire.

        On l’utilise pour que la taille de rotation 3x3 ou 5x5
        change selon les parties.
    */
    srand((unsigned int) time(NULL));

    /*
        On efface l’écran au début.
    */
    clear_screen();

    /*
        On affiche le menu et on récupère le choix du joueur.
    */
    choice = main_menu();

    if (choice == 1) {
        /*
            Nouvelle partie.
        */
        start_new_game(&game);
        game_loop(&game);
    } else if (choice == 2) {
        /*
            Chargement d’une partie sauvegardée.
        */
        loaded = load_game(&game);

        if (loaded) {
            game_loop(&game);
        } else {
            printf("Impossible de charger la sauvegarde.\n");
        }
    } else {
        /*
            Le joueur a choisi de quitter.
        */
        printf("Fin du programme.\n");
    }

    return 0;
}
