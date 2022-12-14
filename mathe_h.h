#ifndef MATHE_H
#define MATHE_H

#ifdef __cplusplus
extern "C" {
#endif

// if some idiot made a min / max function in stdlib.h
// -> happens on windows
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

int Klammern(char text[], char saveto[]);
long double rechnen(char text[6]);
double hoch(double basis, int exponent);
int max(int arr[], int len);
int min(int arr[], int len);
double Random(int MIN, int MAX, int Komma);

#ifdef __cplusplus
}
#endif

#endif