#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUTFILE "jizerska50.txt"
#define OUTPUTFILE "vysledkova_listina.txt"
#define DELIMITERS ";:,()"
#define MAX 100

typedef struct {
    int minuty;
    int sekundy;
    int setiny;
} CAS;

typedef struct {
    int cislo;
    char prijmeni[15];
    char jmeno[15];
    int rocnik;
    char stat[5];
    CAS cas;
} SOUTEZICI;

SOUTEZICI *readFile(SOUTEZICI *soutezici, int *pocet) {
    FILE *fr = fopen(INPUTFILE, "r");
    if (fr == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", INPUTFILE);
    }
    char radek[MAX];
    int i = 0;
    int j;
    char *token = NULL;
    SOUTEZICI *pomocny = NULL;
    while (fgets(radek, MAX, fr) != NULL) {
        if (i != 0) {
            pomocny = (SOUTEZICI *) realloc(soutezici, sizeof(SOUTEZICI) * i);
            if (pomocny == NULL) {
                printf("Neuspesna realokace.\n");
                free(soutezici);
            } else {
                soutezici = pomocny;
                token = strtok(radek, DELIMITERS);
                j = 0;
                while (token != NULL) {
                    switch (j) {
                        case 0:
                            soutezici[i - 1].cislo = atoi(token);
                            break;
                        case 1:
                            strcpy(soutezici[i - 1].prijmeni, token);
                            break;
                        case 2:
                            strcpy(soutezici[i - 1].jmeno, token);
                            break;
                        case 3:
                            soutezici[i - 1].rocnik = atoi(token);
                            break;
                        case 4:
                            strcpy(soutezici[i - 1].stat, token);
                            break;
                        case 5:
                            soutezici[i - 1].cas.minuty = atoi(token);
                            break;
                        case 6:
                            soutezici[i - 1].cas.sekundy = atoi(token);
                            break;
                        case 7:
                            soutezici[i - 1].cas.setiny = atoi(token);
                            break;
                    }
                    j++;
                    token = strtok(NULL, DELIMITERS);
                }
            }
        }
        i++;
    }
    if (fclose(fr) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n");
    }
    *pocet = i - 1;
    return soutezici;
}

int cas(int rocnik) {
    time_t t = time(NULL);
    struct tm *tim = localtime(&t);
    return (tim->tm_year + 1900) - rocnik;
}

void vypis(SOUTEZICI *soutezici, int pocet) {
    int cze = 0;
    int rocnik = 0;
    int index = 0;
    printf("S T A R T O V N I  L I S T I N A  -  J I Z E R S K A  5 0\n");
    printf("---------------------------------------------------------------\n");
    printf("startovni cislo     prijmeni        jmeno  rocnik  stat\n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < pocet; ++i) {
        if (strcmp(soutezici[i].stat, "CZE") == 0) {
            cze++;
        }
        if (soutezici[i].rocnik > rocnik) {
            rocnik = soutezici[i].rocnik;
            index = i;
        }
        printf("%10d %15s %15s %5d %5s\n", soutezici[i].cislo, soutezici[i].prijmeni, soutezici[i].jmeno,
               soutezici[i].rocnik, soutezici[i].stat);
    }
    printf("---------------------------------------------------------------\n");
    printf("Pocet zavodniku: %d\n", pocet);
    printf("Pocet Cechu:     %d\n", cze);
    printf("Nejmladsim zavodnikem je %s %s z %s. Je mu %d let\n", soutezici[index].jmeno, soutezici[index].prijmeni,
           soutezici[index].stat, cas(rocnik));
}

int prevodCasu(CAS cas) {
    return cas.minuty * 6000 + cas.sekundy * 100 + cas.setiny;
}

void swap(SOUTEZICI *temp1, SOUTEZICI *temp2) {
    SOUTEZICI xp = *temp1;
    *temp1 = *temp2;
    *temp2 = xp;
}

void bubbleSort(SOUTEZICI *soutezici, int pocet) {
    for (int i = 0; i < pocet - 1; ++i) {
        for (int j = 0; j < pocet - 1 - i; ++j) {
            if (prevodCasu(soutezici[j].cas) > prevodCasu(soutezici[j + 1].cas)) {
                swap(&soutezici[j], &soutezici[j + 1]);
            }
        }
    }
}

void doSouboru(SOUTEZICI *soutezici, int pocet) {
    int ztrata;
    FILE *fw = fopen(OUTPUTFILE, "w");
    if (fw == NULL) {
        printf("Nepodarilo se otevrit soubor %s\n", OUTPUTFILE);
    }
    fprintf(fw, "V Y S L E D K O V A  L I S T I N A  -  J I Z E R S K A  5 0\n");
    fprintf(fw, "-----------------------------------------------------------------------\n");
    fprintf(fw, "startovni cislo     prijmeni        jmeno  rocnik  stat   cas    ztrata\n");
    fprintf(fw, "-----------------------------------------------------------------------\n");
    bubbleSort(soutezici, pocet);
    for (int i = 0; i < pocet; ++i) {
        ztrata = prevodCasu(soutezici[i].cas) - prevodCasu(soutezici[0].cas);
        if (ztrata != 0) {
            fprintf(fw, "%10d %15s %15s %5d %5s %02d:%02d:%02d +%02d:%02d:%02d\n", soutezici[i].cislo,
                    soutezici[i].prijmeni,
                    soutezici[i].jmeno,
                    soutezici[i].rocnik, soutezici[i].stat, soutezici[i].cas.minuty, soutezici[i].cas.sekundy,
                    soutezici[i].cas.setiny, ztrata / 6000, ztrata % 6000 / 100, ztrata % 100);
        } else {
            fprintf(fw, "%10d %15s %15s %5d %5s %02d:%02d:%02d\n", soutezici[i].cislo, soutezici[i].prijmeni,
                    soutezici[i].jmeno,
                    soutezici[i].rocnik, soutezici[i].stat, soutezici[i].cas.minuty, soutezici[i].cas.sekundy,
                    soutezici[i].cas.setiny);
        }
    }
    fprintf(fw, "-----------------------------------------------------------------------\n");
    if (fclose(fw) == EOF) {
        printf("Nepodarilo se zavrit soubor %s", OUTPUTFILE);
    }
}


int main() {
    SOUTEZICI *soutezici = NULL;
    int pocet = 0;
    soutezici = readFile(soutezici, &pocet);
    vypis(soutezici, pocet);
    doSouboru(soutezici, pocet);
    free(soutezici);
    return 0;
}
