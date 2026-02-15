#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"

int main() {
    int scelta;
    
    // Messaggio di benvenuto
    printf("\n--- BENVENUTI A OCCHINZ ---\n");

    do {
        // Stampa menu [cite: 14]
        printf("\nMENU PRINCIPALE:\n");
        printf("1) Imposta gioco\n");
        printf("2) Gioca\n");
        printf("3) Termina gioco\n");
        printf("4) Visualizza i crediti\n");
        printf("Scelta: ");

        // Controllo input intero
        if (scanf("%d", &scelta) != 1) {
            printf("Errore: inserire un numero valido.\n");
            while(getchar() != '\n'); // Pulisce il buffer
            scelta = 0; // Forza ripetizione
            continue;
        }

        // Switch per le scelte [cite: 15, 18]
        switch (scelta) {
            case 1:
                imposta_gioco();
                break;
            case 2:
                gioca();
                break;
            case 3:
                termina_gioco();
                break;
            case 4:
                crediti();
                break;
            default:
                // Gestione errore comando sbagliato [cite: 16]
                printf("Comando sbagliato! Inserire un numero tra 1 e 4.\n");
                break;
        }

    } while (scelta != 3); // Esce con 3 [cite: 23]

    return 0;
}