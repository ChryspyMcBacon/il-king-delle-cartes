/**
 * @file IlKingDelleCartes.c
 * @brief Gioco di carte con effetti speciali
 * @author Christian Occhiogrosso, Mattia Rosin, Leonardo Serpelloni
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/** Colori ANSI per la stampa colorata nel terminale */
#define RED     "\x1b[31m" ///< Testo rosso
#define GREEN   "\x1b[32m" ///< Testo verde
#define YELLOW  "\x1b[33m" ///< Testo giallo
#define BLUE    "\x1b[34m" ///< Testo blu
#define MAGENTA "\x1b[35m" ///< Testo magenta
#define CYAN    "\x1b[36m" ///< Testo ciano
#define RESET   "\x1b[0m"  ///< Reset del colore
#define BOLD    "\x1b[1m"  ///< Testo in grassetto

/** Macro per pulire lo schermo, compatibile con Windows e Unix */
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN() system("cls")
#else
    #include <unistd.h>
    #define CLEAR_SCREEN() system("clear")
#endif

/**
 * @struct Carta
 * @brief Rappresenta una carta da gioco con Seme e Valore
 *
 * I semi sono codificati da 0 a 3:     CUORI = 0, FIORI = 1, QUADRI = 2, PICCHE = 3
 * 
 * I valori sono mappati da 10 a 19 per distinguerli dai numeri semplici:
 * - 10–16 corrispondono alle carte numeriche da 1 a 7
 * - 17 = J, 18 = Q, 19 = K
 */
typedef struct{
    int seme;     
    int valore;
} Carta;

/**
 * @struct in_mano
 * @brief Rappresenta le due carte in mano a un giocatore
 *
 * Ogni giocatore ha due carte: una scoperta e una coperta
 * `scoperta2` indica se la seconda carta è già stata rivelata
 */
typedef struct{
    Carta carta1;
    Carta carta2;
    bool scoperta2;
} in_mano;

/**
 * @struct Giocatore
 * @brief Rappresenta un giocatore del gioco
 *
 * Contiene un ID univoco, i Pv attuali, il nome del giocatore e le carte in mano
 */
typedef struct{
    int id;
    int puntiVita;
    char nome[20];
    in_mano mano;
} Giocatore;

/**
 * @brief Mette in pausa l'esecuzione fino alla pressione del tasto INVIO
 */
void attendiInput() {
    printf(BOLD "\nPremi INVIO per continuare...\n" RESET);
    while (getchar() != '\n');
}

/**
 * @brief Svuota il buffer
 */
void svuotaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Stampa la carta in mano al giocatore nell'IU
 */
void stampaCarta(Carta c, int i) {
    const char *semi[] = {RED "Cuori ♥" RESET, GREEN "Fiori ♣" RESET, 
                         BLUE "Quadri ♦ " RESET, MAGENTA "Picche ♠" RESET};
    const char *valori[] = {"Asso", "2", "3", "4", "5", "6", "7", "J", "Q", "K"};
    printf(BOLD "  Carta %d) " RESET " %s di %s\n", i, valori[c.valore-10], semi[c.seme]);
}

/**
 * @brief Stampa separatore nell'IU
 */
void stampaSeparatore() {
    printf(CYAN "════════════════════════════════════════════════════════════════════════════════\n" RESET);
}

/**
 * @brief Stampa regolamento nell'IU
 */
void stampaRegolamento() {
    stampaSeparatore();
    printf(BOLD YELLOW "BENVENUTI NEL GIOCO DEL KING DELLE CARTES!\n" RESET);
    stampaSeparatore();
    printf(BOLD YELLOW "REGOLE DEL GIOCO:\n" RESET);
    printf("1. Ogni giocatore inizia con tot punti vita\n");
    printf("2. Ogni turno ricevi 2 carte\n");
    printf("3. Gli effetti delle carte sono:\n");
    printf("   - Asso: Perdi 1 punto vita\n");
    printf("   - 7: Scopri la carta del giocatore successivo\n");
    printf("   - J: Dai 1 punto vita al giocatore precedente\n");
    printf("   - Q: Dai 1 punto vita al secondo giocatore successivo\n");
    printf("   - K: Prendi tutti i punti vita dal campo\n");
    printf("4. Se perdi tutti i punti vita, sei eliminato\n");
    printf("5. L'ultimo giocatore rimasto vince\n");
    stampaSeparatore();
    attendiInput();
}

/**
 * @brief Stampa il Titolo della Fase corrente nell'IU
 */
void stampaTitoloFase(int fase, int pv) {
    printf(BOLD YELLOW "════════════════════════════════════════════════════════════════════════════════\n");
    printf("                                  FASE %d\n", fase);
    printf("════════════════════════════════════════════════════════════════════════════════\n" RESET);
    printf(BOLD "                          PUNTI VITA SUL CAMPO: " GREEN "%d\n" RESET, pv);
}

/**
 * @brief Ad ogni turno, stampa Nome, Id, PV e carte in mano nell'IU
 */
void stampaStatoGiocatore(Giocatore g) {
    printf(BOLD CYAN "╔══════════════════════════════════════════════════════════════════════════════╗\n" RESET);
    printf(BOLD YELLOW "                            TURNO DI: " RESET BOLD GREEN "%s" RESET "\n", g.nome);
    printf(BOLD "                        ID: " RESET BLUE "%2d" RESET "   " BOLD "Punti Vita: " RESET RED "%2d" RESET "\n", g.id, g.puntiVita);
    printf(BOLD "Carte in mano:" RESET "\n");
    stampaCarta(g.mano.carta1, 1);
    if(g.mano.scoperta2)
        stampaCarta(g.mano.carta2, 2);
    else
        printf(BOLD "  Carta 2) " RESET RED " COPERTA!" RESET "\n");
    printf(BOLD CYAN "╚══════════════════════════════════════════════════════════════════════════════╝\n" RESET);
}

/**
 * @brief Stampa il vincitore nell'IU
 */
void vittoria(const char* nomeVincitore) {
    printf(BOLD GREEN "\n════════════════════════════════════════════════════════════════════════════════\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n" );
    printf("                                     VITTORIA!\n" RESET);
    printf(BOLD MAGENTA"\nIl vincitore è %s, complimenti!\n" RESET, nomeVincitore);
    printf(BOLD GREEN "════════════════════════════════════════════════════════════════════════════════\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n" RESET);
}

/**
 * @brief Chiede quanti giocatori, il loro nome e li stampa nell'IU
 */
void n_giocatori(Giocatore giocatori[], int *n){
    bool found = false;
    printf("Inserire il numero di giocatori: ");
    do{
        if(scanf("%d", n) != 1 || *n < 2 || *n > 20) {
            printf(RED "\nErrore - possono giocare dalle 2 alle 20 persone, reinserire il numero di giocatori: " RESET);
            while(getchar() != '\n');
        }else
            found = true;
    }while(!found);
    stampaSeparatore();
    printf(BOLD MAGENTA "INSERIMENTO NOMI GIOCATORI\n" RESET);
    for(int i = 0; i < *n; i++){
        giocatori[i].id = i;
        do{
            found = false;
            printf("\nGiocatore %d, inserisci il tuo Nome: ", i+1);
            scanf("%s", giocatori[i].nome);
            while (getchar() != '\n');
            if (strlen(giocatori[i].nome) == 0 || strlen(giocatori[i].nome) > 14){
                printf(RED "\nErrore - il nome può avere un massimo di 14 caratteri, reinserisci:" RESET);
            }else{
                found = true;
            }
        }while(!found);
    }
    stampaSeparatore();
    printf(BOLD GREEN "OK, SIAMO IN %d GIOCATORI!\n" RESET, *n);
    printf(BOLD "ECCO I GIOCATORI:\n" RESET);
    for(int i = 0; i < *n; i++){
        printf("%d) %s\n", i+1, giocatori[i].nome);
    }
    attendiInput();
}

/**
 * @brief Chiede quanti Pv avrà ogni giocatore
 */
void n_pv(Giocatore giocatori[], int vivi){
    bool found = false;
    int pv;

    stampaSeparatore();
    printf("Inserire il numero di punti vita iniziali per ciascun giocatore: ");
    do{
        if(scanf("%d", &pv) != 1 || pv < 2 || pv > 10 ) {
            printf(RED "\nErrore - i punti vita possono essere dai 2 ai 10\n" RESET);
            while(getchar() != '\n');
        }else
            found = true;
    }while(!found);
    for(int i = 0; i < vivi; i++)
        giocatori[i].puntiVita = pv;

    stampaSeparatore();
    printf(BOLD GREEN "Tutti i giocatori inizieranno con %d punti vita!\n" RESET, pv);
    svuotaBuffer();
    attendiInput();
    CLEAR_SCREEN();
}

/**
 * @brief Mescola il mazzo usando l’algoritmo Fisher-Yates 
 */
void mescolaMazzo(Carta mazzo[], int size) {
    int k = 0;
    for (int seme = 0; seme < 4; seme++) {
        for (int valore = 10; valore <= 19; valore++) {
            mazzo[k].seme = seme;
            mazzo[k].valore = valore;
            k++;
        }
    }
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = mazzo[i];
        mazzo[i] = mazzo[j];
        mazzo[j] = temp;
    }
}

/**
 * @brief Randomizza il primo giocatore ad iniziare la Fase
 */
int scegliPrimoGiocatore(int n) {
    return rand() % n;
}

/**
 * @brief Resetta le carte 2 di ogni giocatore
 */
void resetCarteScoperte(Giocatore giocatori[], int n) {
    for (int i = 0; i < n; i++) {
        giocatori[i].mano.scoperta2 = false;
    }
}

/**
 * @brief Da due carte dal mazzo a ogni giocatore
 */
void daiDueCarte(Giocatore* giocatore, Carta mazzo[], int *nCarte){
    giocatore->mano.carta1 = mazzo[*nCarte - 1];
    giocatore->mano.carta2 = mazzo[*nCarte - 2]; 
    *nCarte -= 2;
}

/**
 * @brief Applica l'effetto speciale di una carta in base al suo valore
 *
 * Gli effetti speciali includono:
 * - **Asso (valore 10)**: il giocatore perde 1 punto vita, che va sul campo di gioco
 * - **7 (valore 16)**: il giocatore successivo scopre una carta e ne applica l'effetto
 * - **J (valore 17)**: il giocatore versa 1 punto vita al precedente
 * - **Q (valore 18)**: il giocatore versa 1 punto vita a chi si trova 2 posizioni avanti (se più di 2 giocatori)
 * - **K (valore 19)**: il giocatore prende tutti i punti vita dal campo di gioco
 *
 */
void applicaEffetto(Carta c, Giocatore giocatori[], int attuale, int* depositati, int vivi, int num_carta){
    bool effettoCompletato = false;
    int next;
    int prev;
    printf(CYAN "\nVerifico gli effetti della carta %d:\n", num_carta);
    while (!effettoCompletato) {
        switch(c.valore-10){
            case 0: //asso
                printf(RED "\nMi spiace %s, per colpa dell'Asso perdi un punto vita, mettilo sul campo di gioco\n" RESET, giocatori[attuale].nome);
                giocatori[attuale].puntiVita--;
                if (giocatori[attuale].puntiVita < 0) giocatori[attuale].puntiVita = 0;
                *depositati += 1;
                effettoCompletato = true;
                break;
            case 6:  // sette
                if(giocatori[attuale].id == vivi - 1){
                    next = 0;
                }else{
                    next = giocatori[attuale].id + 1;
                }
                printf(YELLOW "\nBravo %s hai giocato un 7, scopri la carta di %s e ne applichi l'effetto\n" RESET,giocatori[attuale].nome, giocatori[next].nome);
                if(!giocatori[next].mano.scoperta2){
                    giocatori[next].mano.scoperta2 = true;
                    stampaCarta(giocatori[next].mano.carta2,2);
                    attuale = next;
                    c = giocatori[next].mano.carta2;
                } else {
                    effettoCompletato = true;
                }
                break;
            case 7:  // J
                if(giocatori[attuale].id == 0){
                    prev = vivi - 1;
                }else{
                    prev = giocatori[attuale].id - 1;
                }
                printf(BOLD CYAN "\nMi spiace %s per il J giocato versi un punto vita a %s \n" RESET, giocatori[attuale].nome, giocatori[prev].nome);
                giocatori[attuale].puntiVita--;
                if (giocatori[attuale].puntiVita < 0) giocatori[attuale].puntiVita = 0;
                giocatori[prev].puntiVita++;
                effettoCompletato = true;
                break;
            case 8:  // Q
                if(vivi == 2){
                    printf(RED "\nVisto che siete in 2, l'effetto del Q si annulla\n" RESET);
                    effettoCompletato = true;
                }else{
                    if(giocatori[attuale].id == vivi - 1){
                        next = 1;
                    }else if(giocatori[attuale].id == vivi - 2){
                        next = 0;
                    }else{
                        next = giocatori[attuale].id + 2;
                    }
                    printf(BOLD CYAN "\nMi spiace %s per il Q giocato versi un punto vita a %s\n" RESET, giocatori[attuale].nome, giocatori[next].nome);
                    giocatori[attuale].puntiVita--;
                    if (giocatori[attuale].puntiVita < 0) giocatori[attuale].puntiVita = 0;
                    giocatori[next].puntiVita++;
                    effettoCompletato = true;
                }
                break;
            case 9: // K
                if ((*depositati) != 0){
                    printf(GREEN "\nGrande %s, pescando un K, prendi tutti i punti vita dal campo di gioco, ovvero %d!\n" RESET, giocatori[attuale].nome, *depositati);
                    giocatori[attuale].puntiVita += *depositati;
                    *depositati = 0;
                }else{
                    printf(RED "\nMi dispiace %s il campo di gioco è vuoto, quindi niente punti vita per te!\n" RESET, giocatori[attuale].nome);
                }
                effettoCompletato = true;
                break;
            default:
                printf(CYAN "\nLa carta giocata non ha effetti\n" RESET);
                effettoCompletato = true;
        }
    }
}

/**
 * @brief Gestisce la fase di gioco del giocatore attuale.
 *
 * Durante la fase di un giocatore, viene mostrato il suo stato attuale e
 * viene automaticamente applicato l'effetto della prima carta in mano
 * 
 * Se la seconda carta è ancora coperta, il giocatore può decidere se:
 * - visionarla
 * - scoprirla e applicarne l'effetto
 * - lasciarla coperta
 *
 * Se invece la seconda carta è già stata scoperta in precedenza il gioco lo notifica
 */
void svolgiFase(int attuale, Giocatore giocatori[], int* depositati, int vivi){
    stampaStatoGiocatore(giocatori[attuale]);
    applicaEffetto(giocatori[attuale].mano.carta1, giocatori, attuale, depositati, vivi, 1);
    int scopri;
    if(!giocatori[attuale].mano.scoperta2){
        bool inputValido = false;

        do{
            printf("\nVuoi visionare la carta coperta? (1 = Sì, 0 = No): ");

            if(scanf("%d", &scopri) != 1 || (scopri != 0 && scopri != 1)) {
                printf(RED "\nErrore - inserisci 1 per Sì o 0 per No\n" RESET);
                while(getchar() != '\n');
            } else {
                inputValido = true;
            }
        }while(!inputValido);

        if(scopri){
            stampaCarta(giocatori[attuale].mano.carta2,2);
            inputValido = false;
            do{
                printf("\nVuoi scoprire la carta e applicarne l'effetto? (1 = Sì, 0 = No): ");
                if(scanf("%d", &scopri) != 1 || (scopri != 0 && scopri != 1)) {
                    printf(RED "\nErrore - inserisci 1 per Sì o 0 per No\n" RESET);
                    while(getchar() != '\n');
                } else {
                    inputValido = true;
                }
            }while(!inputValido);

            if(scopri){
                applicaEffetto(giocatori[attuale].mano.carta2, giocatori, attuale, depositati, vivi, 2);
            }
            else
                printf(YELLOW "\nVa bene, allora la carta resterà coperta!\n" RESET);
        }
    }else{
        printf(BOLD MAGENTA "\nLa tua carta 2 è stata scoperta dal giocatore precedente, devi vendicarti!\n" RESET);
    }
}

/**
 * @brief Elimina i giocatori riscrivendo l'array, nel caso morissero tutti in un turno, il vincitore è l'ultimo a morire
 */
int eliminaGiocatori(Giocatore giocatori[], int *n, char ultimoEliminato[]) {
    int vivi = 0;
    int ultimoIdx = -1;
    for(int i = 0; i < *n; i++){
        if(giocatori[i].puntiVita > 0)
            giocatori[vivi++] = giocatori[i];
        else {
            printf(RED "════════════════════════════════════════════════════════════════════════════════\n" RESET);
            printf(RED "════════════════════════════════════════════════════════════════════════════════\n" RESET);
            printf(BOLD "                        %s è stato eliminato dal gioco!\n" RESET, giocatori[i].nome);
            strcpy(ultimoEliminato, giocatori[i].nome); // Salva l'ultimo eliminato
            ultimoIdx = i;
            printf(RED "════════════════════════════════════════════════════════════════════════════════\n" RESET);
            printf(RED "════════════════════════════════════════════════════════════════════════════════\n" RESET);        }
    }
    *n = vivi;
    for(int i = 0; i < *n; i++){
        giocatori[i].id = i;
    }
    if (vivi == 0 && ultimoIdx != -1) return 1;
    return 0;
}

/**
 * @brief Funzione main
 */
int main(){
    srand(time(NULL));

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8); //attiva la visualizzazione dei caratteri speciali su windows
    #endif

    Carta mazzo[40];
    Giocatore giocatori[20];

    stampaRegolamento();

    int pvDepositati = 0;
    int vivi;
    n_giocatori(giocatori, &vivi);
    n_pv(giocatori, vivi);

    int fase = 1;
    char ultimoEliminato[20] = "";
    while(vivi > 1){
        stampaTitoloFase(fase, pvDepositati);

        int nCarte = 40;
        mescolaMazzo(mazzo, nCarte);

        int primo = scegliPrimoGiocatore(vivi);
        resetCarteScoperte(giocatori, vivi);

        printf(BOLD CYAN "════════════════════════════════════════════════════════════════════════════════\n" RESET);
        printf(BOLD YELLOW "Il primo a cominciare questa fase è: " RESET BOLD GREEN "%s" RESET "\n", giocatori[primo].nome);
        printf(BOLD CYAN   "════════════════════════════════════════════════════════════════════════════════\n" RESET);
       
        int turno = primo;

        for(int t = 0; t < vivi; t++){
            daiDueCarte(&giocatori[turno], mazzo, &nCarte);
            if(turno == vivi - 1){
                turno = 0;
            }else{
                turno++;
            }
        }

        turno = primo;
        for(int t = 1; t <= vivi; t++){
            svolgiFase(turno, giocatori, &pvDepositati, vivi);
            if(turno == vivi - 1){
                turno = 0;
            }else{
                turno++;
            }

            if(t == vivi){
                printf(BOLD GREEN "\n════════════════════════════════════════════════════════════════════════════════\n" RESET);
                printf(BOLD MAGENTA "                                  FASE %d CONCLUSA!\n" RESET, fase);
                printf(BOLD GREEN   "════════════════════════════════════════════════════════════════════════════════\n" RESET);
            }   
            attendiInput();
            svuotaBuffer();
            CLEAR_SCREEN();
        }
        int morti = eliminaGiocatori(giocatori, &vivi, ultimoEliminato);
        if (morti) {
            vittoria(ultimoEliminato);
            return 0;
        }
        fase++;
    }
    vittoria(giocatori[0].nome);
    printf(BOLD "\nPremi INVIO per terminare...\n" RESET);
    while (getchar() != '\n');
    return 0;
}

