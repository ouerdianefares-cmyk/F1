#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "game.h"
#include "display.h"
#include "save.h"

int main(void) {
    GameState state;
    int choice;
    int running = 1;

    srand((unsigned int)time(NULL));

    while (running) {
        display_welcome();
        display_menu();

        if (!read_int(&choice)) {
            printf("  %sEntree invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        switch (choice) {
            case 1:
                init_board(&state, 2);
                run_game(&state);
                pause_display("Partie terminee !");
                break;

            case 2:
                init_board(&state, 3);
                run_game(&state);
                pause_display("Partie terminee !");
                break;

            case 3:
                if (!save_exists(SAVE_FILE)) {
                    printf("\n  %sAucune sauvegarde trouvee !%s\n", ANSI_RED, ANSI_RESET);
                    pause_display("");
                } else {
                    if (load_game(&state, SAVE_FILE)) {
                        printf("\n  %sPartie chargee avec succes !%s\n", ANSI_GREEN, ANSI_RESET);
                        pause_display("Reprise de la partie...");
                        run_game(&state);
                        pause_display("Partie terminee !");
                    } else {
                        printf("\n  %sErreur lors du chargement de la sauvegarde.%s\n",
                               ANSI_RED, ANSI_RESET);
                        pause_display("");
                    }
                }
                break;

            case 4:
                running = 0;
                printf("\n  %sMerci d'avoir joue a CY-Connect ! A bientot.%s\n\n",
                       ANSI_CYAN, ANSI_RESET);
                break;

            default:
                printf("  %sChoix invalide ! Entrez un nombre entre 1 et 4.%s\n",
                       ANSI_RED, ANSI_RESET);
                pause_display("");
                break;
        }
    }

    return 0;
}
