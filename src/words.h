#ifndef WORDS_H
#define WORDS_H

#ifndef NO_STD_LIB
    #include<stdio.h>
    #include<stdlib.h>
    #include<stdarg.h>

    #define WORDS_H_MALLOC(size) malloc(size)
    #define WORDS_H_FREE(p) free(p)
    #define WORDS_H_PRINTF(...) printf(__VA_ARGS__)
    #define WORDS_H_VA_LIST             va_list
    #define WORDS_H_VA_ARG(ap, type)    va_arg(ap, type)
    #define WORDS_H_VA_START(ap, param) va_start(ap, param)
    #define WORDS_H_VA_END(ap)          va_end(ap)
#endif

int find(const char text[], const char search[]);
int findIndex(const char text[], const char search[], int * saveto);
int findWords(const int found[], int size, int searchNumber, int wordsFound, int words[][2]);
int replace(char text[], const char search[], const char repl[]);
int split(const char text[], const char search[], char *** saveto);
int join(const char ** wordArr, int words, const char search[], char saveto[]);
int replaceS(char text[], const char search[], const char repl[]);
int splitFirst(const char text[], const char search[], char *** saveto);
int replaceFirst(char text[], const char search[], const char repl[]);

void printIntArr(const int arr[], const int size);
void printDoubleArr(const double arr[], int size, const char after[2]);
void printWordArr(const char ** arr, int words);

void freeWordArr(char ** arr, int words);

void word_copy(char dest[], const char word[]);
int word_len(const char word[]);
int word_compare(const char word1[], const char word2[]);
int word_compare_n(const char word1[], const char word2[], unsigned int size);
void word_add(char dest[], const char word[]);
int zwischen(const char text[], const char word1[], const char word2[], char saveto[]);

int intLen(int zahl);
void intToString(int zahl, char * saveto);
void doubleToString(float zahl, int afterpoint, char * saveto);

const char * word_seek(const char * word, const char * search);
const char * word_seek_after(const char * word, const char * search);
const char * word_seek_first(const char * word, unsigned int number, ...);
const char * word_seek_first_v(const char * word, unsigned int number, WORDS_H_VA_LIST args);
unsigned int word_len_until(const char * word, const char * search);
const char * word_copy_until(char * dest, const char * word, const char * search);

int find(const char text[], const char search[]) {
    int size = word_len(text);
    int searchSize = word_len(search);

    int found = 0;
    int trueN = 0; // to remember the position of the process / how far it already is. 
    // otherwise it would reapeat the process at always the same position (0)
    for (int i = 0; i < size; i++) {
        for (int n = trueN; n < searchSize; n++) {
            if (text[i] == search[n]) {
                if (n == searchSize - 1) {
                    found = 2;
                } else {
                    found = 1;
                    trueN = trueN + 1;
                    break;
                }
            } else {
                found = 0;
                trueN = 0;
                break;
            }
        }
        if (found == 2) {
            break;
        }
    }
    if (found == 2) {
        return 1;
    }
    return 0;
}
int findIndex(const char text[], const char search[], int * saveto) {
    int size = word_len(text);
    int searchSize = word_len(search);

    int found = 0; // found-Code: 0 = no, 1 = yes (2 = word found)
    int trueFound = 0; // number of words found
    int fArr[size];
    for (int z = 0; z < size; z++) {
        fArr[z] = 0;
    }
    int lastWordEnd = -1;
    int thisWordStart = 0;
    int trueN = 0; // to remember the position of the process / how far it already is. 
    // otherwise it would reapeat the process at always the same position (0)
    for (int i = 0; i < size; i++) {
        for (int n = trueN; n < searchSize; n++) {
            if (text[i] == search[n]) {
                if (n == searchSize - 1) {
                    // reset, word was found
                    found = 2;
                    trueN = 0;
                    trueFound += 1;
                    for (int m = lastWordEnd+1; m < thisWordStart; m++) {
                        // WORDS_H_PRINTF("m: %d %c\n", m, text[m]);
                        fArr[m] = 0;
                    }
                    lastWordEnd = i;
                    fArr[i] = 1;
                } else {
                    // a character was found
                    if (found == 0) {
                        thisWordStart = i-1;
                        if (i == 0) {
                            thisWordStart = 0;
                        }
                        // WORDS_H_PRINTF("start: %d\n", thisWordStart);
                    }
                    found = 1;
                    fArr[i] = 1;
                    trueN = trueN + 1;
                    break;
                }
            } else {
                // this character was not found
                found = 0;
                fArr[i] = 0;
                trueN = 0;
                break;
            }
        }
    }
    if (trueFound != 0) {
        for (int z = 0; z < size; z++) {
            saveto[z] = fArr[z];
        }
        return trueFound;
    }
    // WORDS_H_PRINTF("\n");
    return 0;
}
int findWords(const int found[], int size, int searchSize, int wordsFound, int words[][2]) {
    int trueI = 0;
    int wordPos = 0;

    for (int w = 0; w < wordsFound; w++) {
        int startPos = 0;
        for (int i = trueI; i < size; i++) {
            // word starts
            if (found[i] == 1 && wordPos == 0) {
                startPos = i;
            }
            // word ends
            if (i == size-1) {
                int endPos = i;
                wordPos = 0;
                trueI = i + 1;
                int word[] = {startPos, endPos};
                // printIntArr(word, 2);
                words[w][0] = word[0];
                words[w][1] = word[1];
                break;
            }
            if (found[i] == 1 && (found[i+1] != 1 || wordPos + 1 == searchSize) ) {
                int endPos = i;
                wordPos = 0;
                trueI = i + 1;
                int word[] = {startPos, endPos};
                // printIntArr(word, 2);
                words[w][0] = word[0];
                words[w][1] = word[1];
                break;
            } 
            // word continues
            else if (found[i] == 1) {
                wordPos += 1;
            }
        }
    }
    return wordsFound;
}
int replace(char text[], const char search[], const char repl[]) {
    int size = word_len(text);
    int searchSize = word_len(search);
    int replSize = word_len(repl);
    if (replSize != searchSize) {
        return 0;
    }

    int fArr[size];
    int found = findIndex(text, search, fArr);

    if (found != 0) {
        int rSchritt = 0;
        for (int z = 0; z < size; z++) {
            if (fArr[z] == 1) {
                if (rSchritt == replSize) {
                    rSchritt--;
                }
                text[z] = repl[rSchritt];
                rSchritt++;
            } else {
                text[z] = text[z];
            }
        }
        text[size] = '\0';
        return 1;
    }
    return 0;
}
int split(const char text[], const char search[], char *** saveto) {
    int size = word_len(text);
    int searchSize = word_len(search);

    if (searchSize == 0) {
        // WORDS_H_PRINTF("searchSize: %d\n", searchSize);
        (*saveto) = (char **) WORDS_H_MALLOC(size * sizeof(char *));
        for (int i = 0; i < size; i++) {
            (*saveto)[i] = (char *) WORDS_H_MALLOC(2 * sizeof(char));
            (*saveto)[i][0] = text[i];
            (*saveto)[i][1] = '\0';
        }
        return size-1;
    }
    
    int Index[size];
    int wordAnzahl = findIndex(text, search, Index);

    /* WORDS_H_PRINTF("text: %s\n", text);
    WORDS_H_PRINTF("search: %s\n", search);
    WORDS_H_PRINTF("searchSize: %d\n", searchSize);
    printIntArr(Index, size);
    WORDS_H_PRINTF("found: %d\n", wordAnzahl);
    // WORDS_H_PRINTF("\n"); */

    if (wordAnzahl <= 0) {
        (*saveto) = (char **) WORDS_H_MALLOC(1 * sizeof(char *));
        (*saveto)[0] = (char *) WORDS_H_MALLOC((1+size) * sizeof(char));
        word_copy((*saveto)[0], text);
        return 0;
    }
    int savetoSize = wordAnzahl + 2;
    int wordArr[savetoSize][2];
    findWords(Index, size, searchSize, wordAnzahl, wordArr);
    // printIntArr(wordArr[0], 2);
    // WORDS_H_PRINTF("\n");

    int teilMaxSize = size - (wordAnzahl * searchSize) + 1;
    char Teile[savetoSize][teilMaxSize];
    int trueI = 0;

    (*saveto) = (char **) WORDS_H_MALLOC(savetoSize * sizeof(char *));
    for (int w = 0; w < wordAnzahl+1; w++) {
        int start = wordArr[w][0];
        int ende = wordArr[w][1];
        if (w == wordAnzahl) {
            start = size;
            ende = size;
            trueI = wordArr[w-1][1] + 1;
        }
        int teil_i = 0;
        for(int i = trueI; i < size; i++) {
            if (i < start) {
                Teile[w][teil_i] = text[i];
                teil_i++;
            } else if (i >= ende) {
                trueI = i+1;
                break;
            }
        }
        Teile[w][teil_i] = '\0';

        int elemSize = 1;
        for (int n = 0; n < teilMaxSize; n++) {
            if (Teile[w][n] != '\0') {
                elemSize++;
            } else {
                break;
            }
        }
        // WORDS_H_PRINTF("elemSize: %d\n", elemSize);
        int TrueN = 0;
        (*saveto)[w] =  (char *) WORDS_H_MALLOC(elemSize * sizeof(char));
        if (elemSize == 1) {
            (*saveto)[w][0] = '\0';
            continue;
        }
        for (int n = 0; n < teilMaxSize; n++) {
            if (Teile[w][n] != '\0') {
                (*saveto)[w][TrueN] = Teile[w][n];
                TrueN++;
            } else {
                (*saveto)[w][TrueN] = '\0';
                break;
            }
            if (n + 1 == teilMaxSize) {
                (*saveto)[w][TrueN] = '\0';
            }
        }
        // WORDS_H_PRINTF("nahezu: \"%d\"\n    ", (int) word_len(Teile[w]) - (int) word_len(fertig[w]));
        // WORDS_H_PRINTF("wordLen: %d\n", TrueN);
        // word_copy((*saveto)[w], fertig[w]);
        // WORDS_H_PRINTF("Size: %d\n", (int) sizeof((*saveto)[w]));
        // WORDS_H_PRINTF("nahezu: %s\n", Teile[w]);
        // WORDS_H_PRINTF("fertig: %s\n", (*saveto)[w]);
    }
    // printWordArr((*saveto), wordAnzahl);
    // WORDS_H_PRINTF("\n");

    return wordAnzahl;
}
int join(const char ** wordArr, int words, const char search[], char * saveto) {
    int TrueW = 0;
    int searchLen = word_len(search);
    // WORDS_H_PRINTF("sep: %s\n", search);
    for (int w = 0; w < words + 1; w++) {
        int len = word_len(wordArr[w]);
        // WORDS_H_PRINTF("%d\n", len);
        // WORDS_H_PRINTF("%s\n", wordArr[w]);
        for (int i = 0; i < len; i++) {
            saveto[TrueW] = wordArr[w][i];
            TrueW++;
        }
        // strcat(saveto, wordArr[w]);
        if (w < words) {
            for (int n = 0; n < searchLen; n++) {
                saveto[TrueW] = search[n];
                TrueW++;
            }
        }
        if (w == words) {
            saveto[TrueW] = '\0';
        }
    }
    // WORDS_H_PRINTF("%s\n", saveto);
    return 0;
}
int replaceS(char text[], const char search[], const char repl[]) {
    int len = word_len(text);
    int searchLen = word_len(search);
    int replLen = word_len(repl);
    int intArr[len];
    findIndex(text, search, intArr);

    if (find(text, search) == 0 && searchLen != 0) {
        return 0;
    }

    char ** spl;
    int teile = split(text, search, &spl);
    int size = (len + 1 + (replLen * (teile))) - (searchLen * (teile));
    char str[size];

    join((const char **) spl, teile, repl, str);
    word_copy(text, str);
    freeWordArr(spl, teile);

    return teile;
}
int splitFirst(const char text[], const char search[], char *** saveto) {
    int size = word_len(text);
    int searchSize = word_len(search);

    if (searchSize == 0) {
        (*saveto) = (char **) WORDS_H_MALLOC(size * sizeof(char *));
        for (int i = 0; i < size; i++) {
            (*saveto)[i] = (char *) WORDS_H_MALLOC(2 * sizeof(char));
            (*saveto)[i][0] = text[i];
            (*saveto)[i][1] = '\0';
        }
        return size-1;
    }
    
    int Index[size];
    int wordAnzahl = findIndex(text, search, Index);

    if (wordAnzahl <= 0) {
        (*saveto) = (char **) WORDS_H_MALLOC(1 * sizeof(char *));
        (*saveto)[0] = (char *) WORDS_H_MALLOC((1+size) * sizeof(char));
        word_copy((*saveto)[0], text);
        return 0;
    }
    int savetoSize = wordAnzahl + 2;
    int wordArr[savetoSize][2];
    findWords(Index, size, searchSize, wordAnzahl, wordArr);

    int teil1Size = wordArr[0][0];
    int teil2Size = size - wordArr[0][1];

    (*saveto) = (char **) WORDS_H_MALLOC(2 * sizeof(char *));
    (*saveto)[0] = (char *) WORDS_H_MALLOC(1 * sizeof(char));
    (*saveto)[1] = (char *) WORDS_H_MALLOC(1 * sizeof(char));
    for (int i = 0; i < teil1Size; i++) {
        (*saveto)[0][i] = text[i];
    }
    int s = wordArr[0][1] + 1;
    for (int i = 0; i < teil2Size; i++) {
        (*saveto)[1][i] = text[s];
        s++;
    }

    return 1;
}
int replaceFirst(char text[], const char search[], const char repl[]) {
    int len = word_len(text);
    int searchLen = word_len(search);
    int replLen = word_len(repl);

    if (find(text, search) == 0 && searchLen != 0) {
        return 0;
    }

    char ** spl;
    int teile = splitFirst(text, search, &spl);

    int size = (len + 1 + (replLen * (teile))) - (searchLen * (teile));
    char str[size];

    join((const char **) spl, teile, repl, str);
    word_copy(text, str);

    freeWordArr(spl, teile);
    return teile;
}

void printIntArr(const int arr[], int size) {
    for (int i = 0; i < size; i++) {
        WORDS_H_PRINTF("%d ", arr[i]);
    }
    WORDS_H_PRINTF("\n");
}
void printDoubleArr(const double arr[], int size, const char after[2]) {
    char text[] = "%.02f ";
    text[2] = after[0];
    text[3] = after[1];
    for (int i = 0; i < size; i++) {
        WORDS_H_PRINTF(text, arr[i]);
    }
    WORDS_H_PRINTF("\n");
}
void printWordArr(const char ** arr, int words) {
    for (int w = 0; w < words + 1; w++) {
        WORDS_H_PRINTF("Word %d: \"%s\"\n", w + 1, arr[w]);
    }
}
void freeWordArr(char ** arr, int words) {
    for (int w = 0; w < words + 1; w++) {
        // WORDS_H_PRINTF("\"%s\"\n", arr[w]);
        WORDS_H_FREE(arr[w]);
    }
    WORDS_H_FREE(arr);
    // WORDS_H_PRINTF("\"%s\"\n", arr[0]);
}
void word_copy(char dest[], const char word[]) {
    int i = 0;
    while (word[i] != '\0') {
        dest[i] = word[i];
        i++;
    }
    dest[i] = '\0';
}
int word_len(const char word[]) {
    int i = 0;
    while (word[i] != '\0') {
        i++;
    }
    return i;
}
int word_compare(const char word1[], const char word2[]) {
    int i = 0;
    int wahr = 0;
    int size = word_len(word1);
    int size2 = word_len(word2);
    if (size > size2) {
        return 1;
    } else if (size < size2) {
        return -1;
    }
    while (i < size) {
        if (word1[i] == word2[i]) {
            wahr++;
        }
        i++;
    }
    if (wahr == size) {
        return 0;
    }
    return 2;
}
int word_compare_n(const char word1[], const char word2[], unsigned int size) {
    int i = 0;
    int wahr = 0;
    while (i < size) {
        if (word1[i] == word2[i]) {
            wahr++;
        }
        i++;
    }
    if (wahr == i) {
        return 0;
    }
    return 2;
}

void word_add(char dest[], const char word[]) {
    int curlen = word_len(dest);
    int i = 0;
    while (word[i] != '\0') {
        dest[curlen + i] = word[i];
        i++;
    }
    dest[curlen + i] = '\0';
}
int zwischen(const char text[], const char word1[], const char word2[], char saveto[]) {
    if (find(text, word1) != 0) {
        int size = word_len(text);
        int AnfangArr[size];
        int EndeArr[size];
        int AnfangAnzahl = findIndex(text, word1, AnfangArr);
        int EndeAnzahl = findIndex(text, word2, EndeArr);
        int AnfangWord[AnfangAnzahl][2];
        int EndeWord[EndeAnzahl][2];
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

int intLen(int zahl) {
    int len = 0;
    for (; zahl > 0; zahl /= 10) {len += 1;};
    return len;
}
void intToString(int zahl, char * saveto) {
    if (zahl < 10) {
        *saveto = zahl+48; // '0' == 48
        saveto++; *saveto = '\0';
        return;
    }
    int firstZiffer = zahl;
    int oft = 1;
    for (; intLen(firstZiffer) > 1; firstZiffer /= 10) { oft *= 10; };
    *saveto = firstZiffer+48; // '0' == 48
    int next = zahl-(firstZiffer*oft);

    int diffNull = intLen(zahl) - (intLen(next) + 1);
    for (; diffNull > 0; diffNull--) {
        saveto++; *saveto = '0';
    }
    if (intLen(next) == 0) { saveto++; *saveto = '\0'; return; }
    intToString(next, ++saveto);
}
void doubleToString(float zahl, int afterpoint, char * saveto) {
    int ipart = (int) zahl; // Extract integer part
    double fpart = zahl - (double) ipart; // Extract floating part == part after the comma
  
    // convert integer part to string
    intToString(ipart, saveto);
    int i = word_len(saveto);
  
    // check for display option after point
    if (afterpoint != 0) {
        saveto[i] = '.'; // add dot
  
        // multiply by then as long as there needed (afterpoint == max)
        int ergHoch = 1;
        for (int nh = 1; nh < afterpoint; nh++) {
            ergHoch *= 10;
        }
        fpart = fpart * ergHoch;
        // remove unneeded 0
        int before = (int) fpart;
        int temp = before / 10;
        unsigned int rmLast = 0;
        while (temp * 10 == before && temp != 0) {
            rmLast++;
            before = temp;
            temp /= 10;
        }
        for (unsigned int rml = 0; rml < rmLast; rml++) {
            fpart /= 10;
        }
  
        intToString((int) fpart, saveto + i + 1);
    }
}

const char * word_seek(const char * word, const char * search) {
    while (*word != '\0') {
        const char * temp = word;
        const char * tempSearch = search;
        while (*temp == *tempSearch) {
            tempSearch++; temp++;
            if (*tempSearch == '\0') { return word; }
        }
        word++;
    }
    return word;
}
const char * word_seek_after(const char * word, const char * search) {
    while (*word != '\0') {
        const char * temp = word;
        const char * tempSearch = search;
        while (*temp == *tempSearch) {
            tempSearch++; temp++;
            if (*tempSearch == '\0') { return temp; }
        }
        word++;
    }
    return word;
}
const char * word_seek_first(const char * word, unsigned int number, ...) {
    WORDS_H_VA_LIST args;
    WORDS_H_VA_START(args, number);

    const char * ret = word_seek_first_v(word, number, args);

    WORDS_H_VA_END(args);

    return ret;
}
const char * word_seek_first_v(const char * word, unsigned int number, WORDS_H_VA_LIST args) {
    const char ** searches = (const char **) WORDS_H_MALLOC(number * sizeof(const char **));
    for (unsigned int i = 0; i < number; i++) {
        searches[i] = WORDS_H_VA_ARG(args, const char *);
    }

    while (*word != '\0') {
        for (unsigned int i = 0; i < number; i++) {
            const char * search = searches[i];
            if (word_compare_n(word, search, word_len(search)) == 0) { return word; }
        }
        word++;
    }
    return word;
}
unsigned int word_len_until(const char * word, const char * search) {
    int i = 0;
     while (*word != '\0') {
        const char * temp = word;
        const char * tempSearch = search;
        while (*temp == *tempSearch) {
            tempSearch++; temp++;
            if (*tempSearch == '\0') { return i; }
        }
        word++; i++;
    }
    return i;
}
const char * word_copy_until(char * dest, const char * word, const char * search) {
    while (*word != '\0') {
        const char * temp = word;
        const char * tempSearch = search;
        while (*temp == *tempSearch) {
            tempSearch++; temp++;
            if (*tempSearch == '\0') {
                *(dest) = '\0';
                return temp;
            }
        }
        *dest = *word;
        word++; dest++;
    }
    return word;
}

#endif