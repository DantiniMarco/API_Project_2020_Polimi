#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

int change(char **struttura, int inf, int sup);

void print(char **struttura, int inf, int sup);

void mydelete(int inf, int sup);

void undo();

void puliscicoda(int *corrente, int premax);

void ricostruisci();

int *cronologia[10000000];
int coda = 1;
int corrente[10000000];
int numcod = 0;
int max = 0;
bool flag = false;
int numuor = 0; // numero di undo(+) o redo(-)
int sparti = 1;
int max_cod = 0;
FILE *input;
FILE *output;
char *trash;
bool ric = false, fatto = false;


int main() {

    char **struttura;
    struttura = (char **) malloc(sizeof(char *) * 10000000);
    struttura[0] = ".\n";
    char comando[51];
    char lettera;
    char *ptr;
    long inf, sup;
    int old_numuor;


    output = stdout;
    input = stdin;
    trash = (char *) malloc(sizeof(char) * 10);


    while (1) {

        fgets(comando, 50, input);
        lettera = comando[strlen(comando) - 2];
        inf = strtol(comando, &ptr, 10);
        sup = strtol(ptr + 1, &ptr, 10);
        //caso c
        switch (lettera) {

            case 'c':
                if (flag == true) {
                    undo();
                    flag = false;
                    if (numcod - numuor < max_cod && numcod - numuor > 0)
                        numcod = numcod - numuor;
                    else if (numcod - numuor >= max_cod)
                        numcod = max_cod;
                    else
                        numcod = 0;
                    numuor = 0;
                }
                ++numcod;

                coda = change(struttura, inf, sup);
                max_cod = numcod;
                break;

            case 'd':
                ric = true;
                if (ric == true && fatto == false)//se c'è da fare ricostruisci ma non è stato ancora mai fatto, fallo
                {
                    ricostruisci();
                }

                if (flag == true) {
                    flag = false;
                    undo();
                    if (numcod - numuor < max_cod && numcod - numuor > 0)
                        numcod = numcod - numuor;
                    else if (numcod - numuor >= max_cod)
                        numcod = max_cod;
                    else
                        numcod = 0;
                    numuor = 0;
                }
                ++numcod;
                mydelete(inf, sup);
                max_cod = numcod;

                break;

            case 'p':
                if (flag == true) {
                    flag = false;
                    undo();
                }
                print(struttura, inf, sup);

                break;

            case 'u':
                ric = true;
                if (ric == true && fatto == false)//se c'è da fare ricostruisci ma non è stato ancora mai fatto, fallo
                {
                    ricostruisci();
                }
                if (numcod - numuor - inf > 0)
                    numuor += inf;
                else
                    numuor = numcod;

                flag = true;


                break;
            case ('r'):
                ric = true;
                if (ric == true && fatto == false)//se c'è da fare ricostruisci ma non è stato ancora mai fatto, fallo
                {
                    ricostruisci();
                }
                if (numuor > 0) {
                    if (numcod - numuor + inf <= max_cod)
                        numuor -= inf; //se posso fare tutte le redo decremento di inf
                    else
                        numuor = -(max_cod - numcod); //altrimenti decremento solo del numero di redo che posso fare
                }
                flag = true;


                break;

            case 'q':
                return 0;

            default:
                return 0;
        }

    }

}

int change(char **struttura, int inf, int sup) {
    char frase[1025];
    int i = coda + (sup - inf), j;
    int mem = inf;
    int premax = max;

    if (numcod == 1)
        max = sup;

    if (inf <= premax)
        ric = true;

    if (ric == true && fatto == false)//se c'è da fare ricostruisci ma non è stato ancora mai fatto, fallo
    {
        numcod = numcod - 1;
        ricostruisci();
        numcod = numcod + 1;
    }


    while (coda <= i) {  //inseriesce le frasi da frase a struttura[coda]

        fgets(frase, 1025, input);
        struttura[coda] = (char *) malloc((strlen(frase) + 2));
        strcpy(struttura[coda], frase);

        corrente[mem] = coda;
        mem++;
        coda++;

    }
    if (mem > max)
        max = mem - 1;


    if (inf == premax + 1 && ric == false) { // caso di change consecutive
        cronologia[numcod] = (int *) malloc(sizeof(int));
    } else {//change interne
        cronologia[numcod] = (int *) malloc((max + 1) * sizeof(int));
        ric = true; // è da fare ricostruisci, ma una sola volta!
    }


    if (ric == true) {
        for (j = 1; j <= max; j++) {
            cronologia[numcod][j] = corrente[j];
        }
    }


    cronologia[numcod][0] = max;
    fgets(trash, 10, input);

    return coda;

}


void print(char **struttura, int inf, int sup) {
    int j;


    for (j = inf; j <= sup; j++) {
        if (j > max || j == 0)
            fputs(".\n", output);
        else if (j <= max)
            fputs(struttura[corrente[j]], output);

    }
}

void mydelete(int inf, int sup) {
    int i, j, k;
    int valid, legit;

    if (inf == 0) { inf = 1; }

    if (sup <= max) { //distinguo i vari casi
        legit = 0;
        valid = sup - inf + 1;

    } else if (sup == 0)
        legit = 2;
    else {
        if (inf <= max) {
            legit = 0;
            valid = max - inf + 1;
        } else {
            legit = 2;
        }
    }
    if (numcod == 1) { //se sono alla prima istruzione e faccio delete rientro nel caso 2
        legit = 2;
    }

    switch (legit) {
        case (0):

            if (inf + valid <= max) {//delete interne
                for (i = inf; i < inf + max - sup; i++)
                    corrente[i] = corrente[i + valid];
                max = max - valid;
            } else {//delete in coda
                max = max - valid;
            }


            cronologia[numcod] = (int *) malloc(sizeof(int) * (max + 1));
            for (k = 1; k <= max; k++) {
                cronologia[numcod][k] = corrente[k];
            }//copia degli elementi nel nuovo stato
            cronologia[numcod][0] = max;


            break;


        case (2):

            cronologia[numcod] = (int *) malloc(sizeof(int) * (max +1));
            // le delete che non fanno nulla semplicemente copiano lo stato corrente in cronologia[numcod]
            for (j = 1; j <= max; j++) {
                cronologia[numcod][j] = corrente[j];


                cronologia[numcod][0] = max;
            }
            break;
    }
}

void undo() {
    int i, k, premax, new_cod;  //uso premax per tenere traccia di quante frasi c'erano nel vettore corrente
    premax = max;


    if (numcod - numuor > 0) {//mi dice se stiamo sforando con le undo

        if (numcod - numuor < max_cod)
            new_cod = numcod - numuor;
        else
            new_cod = max_cod;
        max = cronologia[new_cod][0];
        for (i = 1; i <= max; i++)//pesco in cronologia lo stato della undo che voglio e lo copio in corrente
            corrente[i] = cronologia[new_cod][i];
    } else {  //caso in cui faccio più undo di quante operazioni ho fatto, allora riempo corrente con 0
        max = 0;
    }
    puliscicoda(corrente, premax);
}


void puliscicoda(int *corrente, int premax) {
    int i;
    for (i = max + 1; i <= premax; i++) {
        corrente[i] = 0;
    }
}


void ricostruisci() {
    int i, k, act_max;;
    fatto = true;
    for (i = 1; i <= numcod; i++) { //per tutti i numcod
        act_max = cronologia[i][0];
        free(cronologia[i]);
        cronologia[i] = (int *) malloc((act_max + 1) * sizeof(int));
        cronologia[i][0] = act_max;
        for (k = 1; k <= act_max; k++) {
            cronologia[i][k] = corrente[k];
        }
    }
}
