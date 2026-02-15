#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

// --- Variabili Globali Statiche (Stato del gioco) ---
static int gioco_impostato = 0; // Flag per verificare se il gioco è pronto [cite: 86]
static int mappa_chiusa = 0;
static struct Giocatore* giocatori[4]; // Array di puntatori [cite: 58]
static int n_giocatori = 0;

// Teste delle liste
static struct Zona_mondoreale* prima_zona_mr = NULL; // [cite: 65]
static struct Zona_soprasotto* prima_zona_ss = NULL;

// --- Funzioni di Utilità (Statiche) [cite: 42] ---

static int lancia_dado(int facce) {
    return (rand() % facce) + 1;
}

static void pulisci_buffer() {
    while(getchar() != '\n');
}

static const char* nome_zona(Tipo_zona t) {
    switch(t) {
        case bosco: return "Bosco";
        case scuola: return "Scuola";
        case laboratorio: return "Laboratorio";
        case caverna: return "Caverna";
        case strada: return "Strada";
        case giardino: return "Giardino";
        case supermercato: return "Supermercato";
        case centrale_elettrica: return "Centrale Elettrica";
        case deposito_abbandonato: return "Deposito Abbandonato";
        case stazione_polizia: return "Stazione Polizia";
        default: return "Sconosciuto";
    }
}

static const char* nome_nemico(Tipo_nemico n) {
    switch(n) {
        case nessun_nemico: return "Nessuno";
        case billi: return "Billi";
        case democane: return "Democane";
        case demotorzone: return "Demotorzone";
        default: return "???";
    }
}

static const char* nome_oggetto(Tipo_oggetto o) {
    switch(o) {
        case nessun_oggetto: return "Vuoto";
        case bicicletta: return "Bicicletta";
        case maglietta_fuocoinferno: return "Maglietta Fuocoinferno";
        case bussola: return "Bussola";
        case schitarrata_metallica: return "Schitarrata Metallica";
        default: return "???";
    }
}

// Funzione per liberare la memoria della mappa
static void dealloca_mappa() {
    struct Zona_mondoreale *curr_mr = prima_zona_mr;
    while (curr_mr != NULL) {
        struct Zona_mondoreale *temp = curr_mr;
        curr_mr = curr_mr->avanti;
        free(temp);
    }
    prima_zona_mr = NULL;

    struct Zona_soprasotto *curr_ss = prima_zona_ss;
    while (curr_ss != NULL) {
        struct Zona_soprasotto *temp = curr_ss;
        curr_ss = curr_ss->avanti;
        free(temp);
    }
    prima_zona_ss = NULL;
    mappa_chiusa = 0;
}

// Funzione per liberare i giocatori
static void dealloca_giocatori() {
    for (int i = 0; i < n_giocatori; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
    n_giocatori = 0;
}

// --- Funzioni Creazione Mappa ---

static void genera_mappa() {
    // Se richiamata, sovrascrive [cite: 72]
    dealloca_mappa();
    
    struct Zona_mondoreale *prec_mr = NULL;
    struct Zona_soprasotto *prec_ss = NULL;
    int ha_demotorzone = 0;

    for (int i = 0; i < 15; i++) { // 15 zone [cite: 68]
        // Allocazione nodi
        struct Zona_mondoreale *nuovo_mr = malloc(sizeof(Zona_mondoreale));
        struct Zona_soprasotto *nuovo_ss = malloc(sizeof(Zona_soprasotto));

        // Tipo zona (uguale per entrambi)
        Tipo_zona tipo = rand() % 10;
        nuovo_mr->tipo = tipo;
        nuovo_ss->tipo = tipo;

        // Nemici e Oggetti (Random con vincoli) [cite: 69, 70]
        // Mondo Reale: Billi (10%), Democane (10%), Nessuno (80%)
        int r_nem = rand() % 100;
        if (r_nem < 10) nuovo_mr->nemico = billi;
        else if (r_nem < 20) nuovo_mr->nemico = democane;
        else nuovo_mr->nemico = nessun_nemico;

        // Oggetto Mondo Reale (30% probabilità)
        if (rand() % 100 < 30) nuovo_mr->oggetto = (rand() % 4) + 1; // 1-4
        else nuovo_mr->oggetto = nessun_oggetto;

        // Soprasotto: Democane (30%), Demotorzone (unico), Nessuno
        // Gestione Demotorzone: ne forziamo uno all'ultima zona se non è uscito, 
        // oppure random (bassa probabilità)
        if (!ha_demotorzone && (i == 14 || (rand() % 100 < 5))) {
            nuovo_ss->nemico = demotorzone;
            ha_demotorzone = 1;
        } else {
            if (rand() % 100 < 30) nuovo_ss->nemico = democane;
            else nuovo_ss->nemico = nessun_nemico;
        }

        // Collegamenti Link Dimensionale
        nuovo_mr->link_soprasotto = nuovo_ss;
        nuovo_ss->link_mondoreale = nuovo_mr;

        // Collegamenti Lista
        nuovo_mr->avanti = NULL;
        nuovo_mr->indietro = prec_mr;
        nuovo_ss->avanti = NULL;
        nuovo_ss->indietro = prec_ss;

        if (prec_mr != NULL) prec_mr->avanti = nuovo_mr;
        else prima_zona_mr = nuovo_mr;

        if (prec_ss != NULL) prec_ss->avanti = nuovo_ss;
        else prima_zona_ss = nuovo_ss;

        prec_mr = nuovo_mr;
        prec_ss = nuovo_ss;
    }
    printf("Mappa generata casualmente (15 zone).\n");
}

static void inserisci_zona() {
    int pos;
    printf("Inserisci la posizione (0 per testa, >0 per successivi): ");
    scanf("%d", &pos);

    struct Zona_mondoreale *nuovo_mr = malloc(sizeof(Zona_mondoreale));
    struct Zona_soprasotto *nuovo_ss = malloc(sizeof(Zona_soprasotto));

    // Generazione casuale tipo
    Tipo_zona tipo = rand() % 10;
    nuovo_mr->tipo = tipo;
    nuovo_ss->tipo = tipo;

    // Input utente per dettagli [cite: 79]
    int input;
    printf("Tipo nemico Mondo Reale (0=Nessuno, 1=Billi, 2=Democane): ");
    scanf("%d", &input);
    if(input == 1) nuovo_mr->nemico = billi;
    else if(input == 2) nuovo_mr->nemico = democane;
    else nuovo_mr->nemico = nessun_nemico;

    printf("Oggetto Mondo Reale (0=Nessuno, 1=Bici, 2=Maglietta, 3=Bussola, 4=Chitarra): ");
    scanf("%d", &input);
    if (input >= 1 && input <= 4) nuovo_mr->oggetto = (Tipo_oggetto)input;
    else nuovo_mr->oggetto = nessun_oggetto;

    printf("Tipo nemico Soprasotto (0=Nessuno, 2=Democane, 3=Demotorzone): ");
    scanf("%d", &input);
    if(input == 3) nuovo_ss->nemico = demotorzone;
    else if(input == 2) nuovo_ss->nemico = democane;
    else nuovo_ss->nemico = nessun_nemico;

    // Link dimensionali
    nuovo_mr->link_soprasotto = nuovo_ss;
    nuovo_ss->link_mondoreale = nuovo_mr;

    // Inserimento in lista (Logica semplificata per brevità: inserimento in testa o dopo)
    if (pos <= 0 || prima_zona_mr == NULL) {
        // Inserimento in testa
        nuovo_mr->avanti = prima_zona_mr;
        nuovo_mr->indietro = NULL;
        if(prima_zona_mr) prima_zona_mr->indietro = nuovo_mr;
        prima_zona_mr = nuovo_mr;

        nuovo_ss->avanti = prima_zona_ss;
        nuovo_ss->indietro = NULL;
        if(prima_zona_ss) prima_zona_ss->indietro = nuovo_ss;
        prima_zona_ss = nuovo_ss;
    } else {
        // Scorrimento
        struct Zona_mondoreale *curr = prima_zona_mr;
        struct Zona_soprasotto *curr_s = prima_zona_ss;
        int i = 0;
        while (curr->avanti != NULL && i < pos - 1) {
            curr = curr->avanti;
            curr_s = curr_s->avanti;
            i++;
        }
        // Inserimento dopo curr
        nuovo_mr->avanti = curr->avanti;
        nuovo_mr->indietro = curr;
        if (curr->avanti) curr->avanti->indietro = nuovo_mr;
        curr->avanti = nuovo_mr;

        nuovo_ss->avanti = curr_s->avanti;
        nuovo_ss->indietro = curr_s;
        if (curr_s->avanti) curr_s->avanti->indietro = nuovo_ss;
        curr_s->avanti = nuovo_ss;
    }
    printf("Zona inserita.\n");
}

static void cancella_zona() {
    int pos;
    printf("Inserisci posizione da cancellare: ");
    scanf("%d", &pos);

    if (prima_zona_mr == NULL) return;

    struct Zona_mondoreale *target_mr = prima_zona_mr;
    struct Zona_soprasotto *target_ss = prima_zona_ss;

    // Caso testa
    if (pos == 0) {
        prima_zona_mr = target_mr->avanti;
        if (prima_zona_mr) prima_zona_mr->indietro = NULL;
        
        prima_zona_ss = target_ss->avanti;
        if (prima_zona_ss) prima_zona_ss->indietro = NULL;
    } else {
        int i = 0;
        while (target_mr != NULL && i < pos) {
            target_mr = target_mr->avanti;
            target_ss = target_ss->avanti;
            i++;
        }
        if (target_mr == NULL) {
            printf("Posizione non valida.\n");
            return;
        }
        // Scollegamento
        if (target_mr->indietro) target_mr->indietro->avanti = target_mr->avanti;
        if (target_mr->avanti) target_mr->avanti->indietro = target_mr->indietro;

        if (target_ss->indietro) target_ss->indietro->avanti = target_ss->avanti;
        if (target_ss->avanti) target_ss->avanti->indietro = target_ss->indietro;
    }

    free(target_mr);
    free(target_ss);
    printf("Zona cancellata.\n");
}

static void stampa_mappa() {
    int scelta;
    printf("Quale mappa stampare? 0=Mondo Reale, 1=Soprasotto: ");
    scanf("%d", &scelta);

    int i = 0;
    if (scelta == 0) {
        struct Zona_mondoreale *curr = prima_zona_mr;
        while(curr) {
            printf("Zona %d: %s | Nemico: %s | Oggetto: %s\n", i++, nome_zona(curr->tipo), nome_nemico(curr->nemico), nome_oggetto(curr->oggetto));
            curr = curr->avanti;
        }
    } else {
        struct Zona_soprasotto *curr = prima_zona_ss;
        while(curr) {
            printf("Zona %d (SS): %s | Nemico: %s\n", i++, nome_zona(curr->tipo), nome_nemico(curr->nemico));
            curr = curr->avanti;
        }
    }
}

static void stampa_zona_fun() {
    int pos;
    printf("Posizione zona da ispezionare: ");
    scanf("%d", &pos);
    
    struct Zona_mondoreale *mr = prima_zona_mr;
    struct Zona_soprasotto *ss = prima_zona_ss;
    int i = 0;
    while(mr != NULL && i < pos) {
        mr = mr->avanti;
        ss = ss->avanti;
        i++;
    }

    if(mr) {
        printf("--- Zona %d ---\n", pos);
        printf("MR: %s, Nem: %s, Ogg: %s\n", nome_zona(mr->tipo), nome_nemico(mr->nemico), nome_oggetto(mr->oggetto));
        printf("SS: %s, Nem: %s\n", nome_zona(ss->tipo), nome_nemico(ss->nemico));
    } else {
        printf("Zona inesistente.\n");
    }
}

static void chiudi_mappa() {
    int count = 0;
    int demo_count = 0;
    struct Zona_soprasotto *curr = prima_zona_ss;
    while (curr) {
        count++;
        if (curr->nemico == demotorzone) demo_count++;
        curr = curr->avanti;
    }

    if (count >= 15 && demo_count == 1) { // [cite: 87]
        mappa_chiusa = 1;
        printf("Mappa chiusa correttamente. Si può giocare.\n");
    } else {
        printf("Impossibile chiudere: servono almeno 15 zone e ESATTAMENTE 1 Demotorzone (Trovati: %d zone, %d demotorzoni).\n", count, demo_count);
        mappa_chiusa = 0;
    }
}

// --- Implementazione Funzioni Pubbliche ---

void imposta_gioco() {
    // Reset se chiamato nuovamente [cite: 44]
    dealloca_giocatori();
    dealloca_mappa();
    gioco_impostato = 0;

    printf("\n--- IMPOSTAZIONE GIOCO ---\n");
    do {
        printf("Inserire numero giocatori (1-4): ");
        scanf("%d", &n_giocatori);
    } while (n_giocatori < 1 || n_giocatori > 4);

    srand(time(NULL)); // [cite: 115]

    int undici_scelto = 0;

    for (int i = 0; i < n_giocatori; i++) {
        giocatori[i] = malloc(sizeof(Giocatore));
        printf("Nome Giocatore %d: ", i+1);
        scanf("%s", giocatori[i]->nome);
        
        // Stats iniziali [cite: 59]
        giocatori[i]->attacco_pischico = lancia_dado(20);
        giocatori[i]->difesa_pischica = lancia_dado(20);
        giocatori[i]->fortuna = lancia_dado(20);
        giocatori[i]->mondo = 0; // Parte nel mondo reale
        giocatori[i]->oggetti_posseduti = 0;
        giocatori[i]->vivo = 1;
        // Inizializza zaino vuoto
        for(int k=0; k<3; k++) giocatori[i]->zaino[k] = nessun_oggetto;

        printf("Stats: Att %d, Dif %d, Fort %d\n", giocatori[i]->attacco_pischico, giocatori[i]->difesa_pischica, giocatori[i]->fortuna);
        
        printf("Scegli classe:\n");
        printf("1) Normale (+3 Att, -3 Dif)\n");
        printf("2) Normale (-3 Att, +3 Dif)\n");
        if (!undici_scelto) printf("3) Undici VirgolaCinque (+4 Att, +4 Dif, -7 Fort)\n");
        
        int scelta;
        scanf("%d", &scelta);
        if (scelta == 1) {
            giocatori[i]->attacco_pischico += 3;
            giocatori[i]->difesa_pischica -= 3; // Può andare negativo? Assumiamo min 0
        } else if (scelta == 2) {
            giocatori[i]->attacco_pischico -= 3;
            giocatori[i]->difesa_pischica += 3;
        } else if (scelta == 3 && !undici_scelto) {
            giocatori[i]->attacco_pischico += 4;
            giocatori[i]->difesa_pischica += 4;
            giocatori[i]->fortuna -= 7;
            strcat(giocatori[i]->nome, " (11.5)");
            undici_scelto = 1;
        }
        // Clamp a minimo 0 per evitare bug
        if(giocatori[i]->attacco_pischico < 0) giocatori[i]->attacco_pischico = 0;
        if(giocatori[i]->difesa_pischica < 0) giocatori[i]->difesa_pischica = 0;
        if(giocatori[i]->fortuna < 0) giocatori[i]->fortuna = 0;
    }

    // Menu creazione mappa
    int scelta_map;
    do {
        printf("\n-- MENU CREAZIONE MAPPA --\n");
        printf("1) Genera mappa casuale\n");
        printf("2) Inserisci zona\n");
        printf("3) Cancella zona\n");
        printf("4) Stampa mappa\n");
        printf("5) Stampa zona\n");
        printf("6) Chiudi mappa e termina impostazioni\n");
        printf("Scelta: ");
        scanf("%d", &scelta_map);

        switch(scelta_map) {
            case 1: genera_mappa(); break;
            case 2: inserisci_zona(); break;
            case 3: cancella_zona(); break;
            case 4: stampa_mappa(); break;
            case 5: stampa_zona_fun(); break;
            case 6: chiudi_mappa(); break;
            default: printf("Scelta errata.\n");
        }
    } while (!mappa_chiusa);

    // Posizionamento iniziale giocatori [cite: 89]
    for(int i=0; i<n_giocatori; i++) {
        giocatori[i]->pos_mondoreale = prima_zona_mr;
        giocatori[i]->pos_soprasotto = prima_zona_ss;
    }

    gioco_impostato = 1; // Flag OK
}

// --- Logica di Gioco (Statiche Helper) ---

static int nemico_presente(Giocatore *g) {
    if (g->mondo == 0) return g->pos_mondoreale->nemico != nessun_nemico;
    else return g->pos_soprasotto->nemico != nessun_nemico;
}

static void combatti(Giocatore *g) {
    if (!nemico_presente(g)) {
        printf("Non c'è nessuno da combattere qui.\n");
        return;
    }

    Tipo_nemico nemico = (g->mondo == 0) ? g->pos_mondoreale->nemico : g->pos_soprasotto->nemico;
    printf("COMBATTIMENTO contro %s!\n", nome_nemico(nemico));
    
    // Semplice sistema di combattimento [cite: 125]
    int forza_nemico = 0;
    if (nemico == billi) forza_nemico = 15;
    else if (nemico == democane) forza_nemico = 25;
    else if (nemico == demotorzone) forza_nemico = 50;

    int dado_att = lancia_dado(20);
    int totale_att = g->attacco_pischico + dado_att;
    
    // Bonus oggetto Maglietta (esempio implementazione utilizza_oggetto implicita o passiva)
    for(int i=0; i<3; i++) {
        if(g->zaino[i] == maglietta_fuocoinferno) totale_att += 5; // Bonus
    }

    printf("Tuo attacco: %d (Base) + %d (Dado) = %d. Difesa Nemico: %d\n", g->attacco_pischico, dado_att, totale_att, forza_nemico);

    if (totale_att >= forza_nemico) {
        printf("Hai sconfitto %s!\n", nome_nemico(nemico));
        // 50% chance che sparisca [cite: 127]
        if (rand() % 2 == 0) {
            printf("Il nemico svanisce definitivamente!\n");
            if (g->mondo == 0) g->pos_mondoreale->nemico = nessun_nemico;
            else g->pos_soprasotto->nemico = nessun_nemico;
        } else {
            printf("Il nemico fugge ma rimane in zona.\n");
        }
        
        // Vittoria Demotorzone
        if (nemico == demotorzone) {
            printf("\n*** %s HA UCCISO IL DEMOTORZONE! VITTORIA! ***\n", g->nome);
            // Salva vincitore (non persistente in questo esempio semplice, ma stampato)
            gioco_impostato = 0; // Termina partita logica
        }
    } else {
        // Danno al giocatore
        int danno = forza_nemico - (g->difesa_pischica + lancia_dado(10));
        if (danno > 0) {
            printf("Sei stato colpito! (Danno calcolato in base alla difesa)\n");
            // Se la difesa + dado è bassa, muori (semplificazione richiesta: "può morire")
            if (lancia_dado(20) > g->fortuna) { // Check morte basato su sfortuna
                printf("%s e' MORTO sotto i colpi del nemico...\n", g->nome);
                g->vivo = 0;
            } else {
                printf("Sei ferito ma sopravvivi per miracolo (Grazie alla Fortuna).\n");
            }
        } else {
            printf("Hai parato il colpo!\n");
        }
    }
}

static void avanza(Giocatore *g, int *azione_fatta) {
    if (nemico_presente(g)) {
        printf("Devi combattere prima di muoverti!\n");
        return;
    }
    
    if (g->mondo == 0) {
        if (g->pos_mondoreale->avanti) {
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            g->pos_soprasotto = g->pos_soprasotto->avanti; // Muove anche il puntatore parallelo
            printf("Avanzato nel Mondo Reale.\n");
            *azione_fatta = 1;
        } else printf("Sei alla fine della mappa.\n");
    } else {
        if (g->pos_soprasotto->avanti) {
            g->pos_soprasotto = g->pos_soprasotto->avanti;
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            printf("Avanzato nel Soprasotto.\n");
            *azione_fatta = 1;
        } else printf("Sei alla fine della mappa.\n");
    }
}

static void indietreggia(Giocatore *g, int *azione_fatta) {
    if (nemico_presente(g)) {
        printf("Devi combattere prima di muoverti!\n");
        return;
    }

    if (g->mondo == 0) {
        if (g->pos_mondoreale->indietro) {
            g->pos_mondoreale = g->pos_mondoreale->indietro;
            g->pos_soprasotto = g->pos_soprasotto->indietro;
            printf("Indietreggiato nel Mondo Reale.\n");
            *azione_fatta = 1;
        } else printf("Sei all'inizio della mappa.\n");
    } else {
        if (g->pos_soprasotto->indietro) {
            g->pos_soprasotto = g->pos_soprasotto->indietro;
            g->pos_mondoreale = g->pos_mondoreale->indietro;
            printf("Indietreggiato nel Soprasotto.\n");
            *azione_fatta = 1;
        } else printf("Sei all'inizio della mappa.\n");
    }
}

static void cambia_mondo(Giocatore *g, int *azione_fatta) {
    if (g->mondo == 0) {
        // Mondo Reale -> Soprasotto
        if (nemico_presente(g)) {
            printf("Non puoi cambiare mondo col nemico presente!\n");
            return;
        }
        g->mondo = 1;
        printf("Sei entrato nel SOPRASOTTO!\n");
        *azione_fatta = 1;
    } else {
        // Soprasotto -> Mondo Reale (richiede check fortuna) [cite: 104]
        int d = lancia_dado(20);
        printf("Tentativo fuga: %d vs Fortuna %d\n", d, g->fortuna);
        if (d < g->fortuna) {
            g->mondo = 0;
            printf("Sei tornato nel Mondo Reale!\n");
            *azione_fatta = 1;
        } else {
            printf("Sei rimasto bloccato nel Soprasotto!\n");
        }
    }
}

static void raccogli_oggetto(Giocatore *g) {
    if (g->mondo == 1) {
        printf("Non ci sono oggetti nel Soprasotto.\n");
        return;
    }
    if (nemico_presente(g)) {
        printf("Sconfiggi il nemico prima!\n");
        return;
    }
    if (g->pos_mondoreale->oggetto == nessun_oggetto) {
        printf("Nessun oggetto qui.\n");
        return;
    }
    
    // Cerca spazio [cite: 130]
    int preso = 0;
    for (int i=0; i<3; i++) {
        if (g->zaino[i] == nessun_oggetto) {
            g->zaino[i] = g->pos_mondoreale->oggetto;
            g->pos_mondoreale->oggetto = nessun_oggetto; // Rimosso dalla zona
            printf("Oggetto raccolto nello slot %d.\n", i);
            preso = 1;
            break;
        }
    }
    if (!preso) printf("Zaino pieno!\n");
}

static void utilizza_oggetto(Giocatore *g) {
    printf("Zaino: 0)%s, 1)%s, 2)%s. Scegli indice: ", 
        nome_oggetto(g->zaino[0]), nome_oggetto(g->zaino[1]), nome_oggetto(g->zaino[2]));
    int idx;
    scanf("%d", &idx);
    if(idx < 0 || idx > 2 || g->zaino[idx] == nessun_oggetto) {
        printf("Slot vuoto o invalido.\n");
        return;
    }

    Tipo_oggetto obj = g->zaino[idx];
    printf("Utilizzi %s...\n", nome_oggetto(obj));
    
    // Effetti [cite: 132]
    switch(obj) {
        case bicicletta:
            printf("Pedali veloce! (Non consuma azione movimento se implementato complesso, qui è flavor).\n");
            break;
        case maglietta_fuocoinferno:
            printf("Ti senti più forte (Attacco passivo aumentato in combattimento).\n");
            break;
        case bussola:
            printf("La bussola impazzisce... il campo magnetico è strano.\n");
            break;
        case schitarrata_metallica:
            printf("Suoni un assolo epico! I nemici sono confusi.\n");
            g->fortuna += 2; // Esempio effetto
            break;
        default: break;
    }
    
    // Rimuovi dopo uso (o dipende dall'oggetto, qui rimuoviamo per semplicità tranne maglietta che potrebbe essere passiva, ma rimuoviamo per standardizzare)
    if (obj != maglietta_fuocoinferno) {
       g->zaino[idx] = nessun_oggetto;
    }
}

// --- Funzione Principale Gioca ---

void gioca() {
    if (!gioco_impostato || !mappa_chiusa) { // [cite: 88]
        printf("Errore: Imposta prima il gioco e chiudi la mappa.\n");
        return;
    }

    int round = 1;
    int vittoria = 0;

    // Loop gioco
    while (!vittoria) {
        printf("\n\n=============== ROUND %d ===============\n", round);
        // Verifica se tutti morti [cite: 138]
        int vivi = 0;
        for(int i=0; i<n_giocatori; i++) if(giocatori[i]->vivo) vivi++;
        if (vivi == 0) {
            printf("TUTTI I GIOCATORI SONO MORTI. GAME OVER.\n");
            break;
        }

        // Shuffle ordine giocatori [cite: 91, 92]
        int ordine[4] = {0,1,2,3}; // Indici max
        for (int i = 0; i < n_giocatori; i++) {
            int r = rand() % n_giocatori;
            int temp = ordine[i];
            ordine[i] = ordine[r];
            ordine[r] = temp;
        }

        // Turni
        for (int k = 0; k < n_giocatori; k++) {
            int idx = ordine[k];
            Giocatore *g = giocatori[idx];
            if (!g->vivo) continue;

            if (!gioco_impostato) { vittoria = 1; break; } // Check vittoria demotorzone avvenuta

            printf("\n--- TURNO DI %s (Mondo: %s) ---\n", g->nome, (g->mondo==0) ? "Reale" : "Soprasotto");
            
            int turno_finito = 0;
            int mossa_movimento_usata = 0; // [cite: 93]

            while (!turno_finito && g->vivo && gioco_impostato) {
                printf("1) Avanza\n2) Indietreggia\n3) Cambia Mondo\n4) Combatti\n");
                printf("5) Stampa Giocatore\n6) Stampa Zona\n7) Raccogli\n8) Usa Oggetto\n9) Passa Turno\nScelta: ");
                
                int azione;
                if(scanf("%d", &azione) != 1) { pulisci_buffer(); continue; }

                switch(azione) {
                    case 1: // Avanza
                        if(!mossa_movimento_usata) avanza(g, &mossa_movimento_usata);
                        else printf("Hai già mosso in questo turno.\n");
                        break;
                    case 2: // Indietreggia
                        if(!mossa_movimento_usata) indietreggia(g, &mossa_movimento_usata);
                        else printf("Hai già mosso in questo turno.\n");
                        break;
                    case 3: // Cambia mondo
                         if(!mossa_movimento_usata) cambia_mondo(g, &mossa_movimento_usata);
                         else printf("Hai già mosso in questo turno.\n");
                         break;
                    case 4: combatti(g); break;
                    case 5: 
                        printf("Stats: Att %d, Dif %d, Fort %d. Zaino: %s %s %s\n", 
                            g->attacco_pischico, g->difesa_pischica, g->fortuna, 
                            nome_oggetto(g->zaino[0]), nome_oggetto(g->zaino[1]), nome_oggetto(g->zaino[2]));
                        break;
                    case 6: 
                        printf("Zona attuale: %s. Nemico: %s.\n", 
                            nome_zona((g->mondo==0)?g->pos_mondoreale->tipo : g->pos_soprasotto->tipo),
                            nome_nemico((g->mondo==0)?g->pos_mondoreale->nemico : g->pos_soprasotto->nemico));
                        break;
                    case 7: raccogli_oggetto(g); break;
                    case 8: utilizza_oggetto(g); break;
                    case 9: turno_finito = 1; break;
                    default: printf("Comando non valido.\n");
                }
            }
        }
        round++;
    }
}

void termina_gioco() {
    printf("Grazie per aver giocato a Cosestrane!\n");
    dealloca_giocatori();
    dealloca_mappa();
    exit(0);
}

void crediti() {
    printf("\n--- CREDITI ---\n");
    printf("Creato da: Studente di Programmazione\n");
    printf("Progetto Esame 2025/2026\n");
}