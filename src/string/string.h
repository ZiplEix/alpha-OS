#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

int strlen(const char *str);
int strnlen(const char *str, int max);
bool isdigit(char c);
int tonumericdigit(char c);
char *strcpy(char *dest, const char *src);
int strncmp(const char *str1, const char *str2, int n);
int istrncmp(const char *str1, const char *str2, int n);
int strnlen_terminator(const char *str, int max, char terminator);
char tolower(char c);

#endif // STRING_H
