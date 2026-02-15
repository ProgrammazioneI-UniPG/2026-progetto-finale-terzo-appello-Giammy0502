#ifndef GAMELIB_H
#define GAMELIB_H

#include <stdio.h>

// Definizioni Enum [cite: 38, 39, 40]
typedef enum {
    bosco, scuola, laboratorio, caverna, strada, giardino,
    supermercato, centrale_elettrica, deposito_abbandonato, stazione_polizia
} Tipo_zona;

typedef enum {
    nessun_nemico, billi, democane, demotorzone
} Tipo_nemico;

typedef enum {
    nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica
} Tipo_oggetto;

// Forward declaration per i puntatori incrociati
struct Zona_soprasotto;
struct Zona_mondoreale;

// Struttura Zona Mondo Reale [cite: 28]
typedef struct Zona_mondoreale {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto;
    struct Zona_mondoreale* avanti;
    struct Zona_mondoreale* indietro;
    struct Zona_soprasotto* link_soprasotto; // [cite: 32]
} Zona_mondoreale;

// Struttura Zona Soprasotto [cite: 33]
typedef struct Zona_soprasotto {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    // Nessun campo oggetto nel Soprasotto [cite: 35]
    struct Zona_soprasotto* avanti;
    struct Zona_soprasotto* indietro;
    struct Zona_mondoreale* link_mondoreale; // [cite: 37]
} Zona_soprasotto;

// Struttura Giocatore [cite: 25]
typedef struct Giocatore {
    char nome[50];
    int mondo; // 0 = Mondo Reale, 1 = Soprasotto [cite: 26]
    struct Zona_mondoreale* pos_mondoreale;
    struct Zona_soprasotto* pos_soprasotto;
    int attacco_pischico;
    int difesa_pischica;
    int fortuna;
    Tipo_oggetto zaino[3]; // Array di 3 oggetti [cite: 27]
    int oggetti_posseduti; // Contatore helper per lo zaino
    int vivo; // 1 = vivo, 0 = morto
} Giocatore;

// Funzioni pubbliche [cite: 18]
void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();

#endif