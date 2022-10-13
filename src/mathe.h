// #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
#include <time.h>

#include "words.h"

// if some idiot made a min / max function in stdlib.h
// -> happens on windows
#undef max
#undef min

int Klammern(char text[], char saveto[]);
long double rechnen(char text[6]);
double hoch(double basis, int exponent);
int max(int arr[], int len);
int min(int arr[], int len);
double Random(int MIN, int MAX, int Komma);

int Klammern(char text[], char saveto[]) {
    if (find(text, "(") != 0) {
        int size = word_len(text);
        int AnfangArr[size];
        int EndeArr[size];
        findIndex(text, "(", AnfangArr);
        findIndex(text, ")", EndeArr);
        int AnfangWord[1][2];
        int EndeWord[1][2];
        findWords(AnfangArr, size, 1, 1, AnfangWord);
        findWords(EndeArr, size, 1, 1, EndeWord);
        int TrueI = 0;
        for (int i = AnfangWord[0][0]; i <= EndeWord[0][0]; i++) {
            saveto[TrueI] = text[i];
            TrueI++;
        }
        saveto[TrueI] = '\0';
        return 1;
    }
    return 0;
}
long double rechnen(char text[]) {
    int size = word_len(text);
    char newText[size];
    word_copy(newText, text);

    char klam[size];
    if (Klammern(newText, klam) != 0) {
        char Klrepl[size];
        char newKlam[size];
        word_copy(newKlam, klam);
        replaceS(klam, "(", "");
        replaceS(klam, ")", "");
        double Klerg = rechnen(klam);
        // sprintf(Klrepl, "%lf", Klerg);
        doubleToString(Klerg, 6, Klrepl);
        replaceS(newText, newKlam, Klrepl);
    }

    replaceS(newText, "--", "+");
    replaceS(newText, "-", "+-");

    replaceS(newText, "*+-", "*-");
    replaceS(newText, "/+-", "/-");
    replaceS(newText, "%+-", "%-");
    replaceS(newText, "^+-", "^-");

    replaceS(newText, "*+", "*");
    replaceS(newText, "/+", "/");
    replaceS(newText, "%+", "%");
    replaceS(newText, "^+", "^");

    // printf("%s\n", newText);

    char ** ad;
    int teileAD = split(newText, "+", &ad);
    long double erAD = 0;
    for (int i = 0; i < teileAD + 1; i++) {
        char ** mu;
        int teileMU = split(ad[i], "*", &mu);
        if (teileMU == 0) {
            word_copy(mu[0], ad[i]);
        }
        long double erMU;

        for (int j = 0; j < teileMU + 1; j++) {
            // printf("mu[j]: %s\n", mu[j]);
            char ** di;
            int teileDI = split(mu[j], "/", &di);
            if (teileDI == 0) {
                word_copy(di[0], mu[j]);
            }
            long double erDI;

            for (int m = 0; m < teileDI + 1; m++) {
                // printf("di[m]: %s\n", di[m]);
                char ** mo;
                int teileMO = split(di[m], "mod", &mo);
                if (teileMO == 0) {
                    word_copy(mo[0], di[m]);
                }
                long double erMO;

                for (int n = 0; n < teileMO + 1; n++) {
                    // printf("mo[n]: %s\n", mo[n]);
                    char ** ho;
                    int teileHO = split(mo[n], "^", &ho);
                    if (teileHO == 0) {
                        word_copy(ho[0], mo[n]);
                    }
                    long double erHO;

                    for (int h = 0; h < teileHO + 1; h++) {
                        long double newH = atof(ho[h]);
                        // printf("newH: %ld\n", newH);
                        if (h == 0) {
                            erHO = newH;
                        } else {
                            erHO = hoch(erHO, newH);
                        }
                        // printf("%ld\n\n", erHO);
                    }
                    if (n == 0) {
                        erMO = erHO;
                    } else {
                        long int zErMo = (long int) erMO % (long int) erHO;
                        erMO = (long double) zErMo;
                    }
                    // printf("%ld\n\n", erMO);
                    freeWordArr(ho, teileHO);
                }

                if (m == 0) {
                    erDI = erMO;
                } else {
                    erDI /= erMO;
                }
                // printf("%lf\n\n", (double) erDI);
                freeWordArr(mo, teileMO);
            }

            if (j == 0) {
                erMU = erDI;
            } else {
                erMU *= erDI;
            }
            // printf("%lf\n\n", (double) erMU);
            freeWordArr(di, teileDI);
        }
        erAD += erMU;
        // printf("%lf\n\n", (double) erAD);
        freeWordArr(mu, teileMU);
    }
    freeWordArr(ad, teileAD);

    return erAD;
}
double hoch(double basis, int exponent) {
    if (exponent == 0) {
        return 1;
    }
    double erg = basis;
    for (int nh = 1; nh < exponent; nh++) {
        erg *= basis;
    }
    return erg;
}
int max(int arr[], int len) {
    int Max = 0;
    for (int i = 0; i < len; i++) {
        for (int n = 0; n < len; n++) {
            if (arr[i] >= arr[n] && arr[i] >= arr[Max]) {
                Max = i;
            }
        }
    }
    return Max;
}
int min(int arr[], int len) {
    int Min = 0;
    for (int i = 0; i < len; i++) {
        for (int n = 0; n < len; n++) {
            if (arr[i] <= arr[n] && arr[i] <= arr[Min]) {
                Min = i;
            }
        }
    }
    return Min;
}
double Random(int MIN, int MAX, int Komma) {
    // srand(time(0));
    MIN = MIN * hoch(10, Komma);
    MAX = MAX * hoch(10, Komma);
    double zahl = (rand() % (MAX - MIN +1)) + MIN;
    zahl = zahl / hoch(10, Komma);
    return zahl;
}