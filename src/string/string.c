#include "string.h"

char tolower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }

    return c;
}

int strlen(const char *str)
{
    int len = 0;

    while (str[len]) {
        len++;
    }

    return len;
}

int strnlen(const char *str, int max)
{
    int len = 0;

    while (str[len] && len < max) {
        len++;
    }

    return len;
}

int strnlen_terminator(const char *str, int max, char terminator)
{
    int i = 0;

    for (i = 0; i < max; i++) {
        if (str[i] == '\0' || str[i] == terminator) {
            break;
        }
    }

    return i;
}

int istrncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;

    while (n-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;

        if (u1 != u2 && tolower(u1) != tolower(u2)) {
            return u1 - u2;
        }

        if (u1 == '\0') {
            return 0;
        }
    }

    return 0;
}

int strncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;

    while (n-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;

        if (u1 != u2) {
            return u1 - u2;
        }

        if (u1 == '\0') {
            return 0;
        }
    }

    return 0;
}

char *strcpy(char *dest, const char *src)
{
    char *res = dest;

    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';

    return res;
}

char *strncpy(char *dest, const char *src, int n)
{
    int i = 0;

    for (i = 0; i < n - 1; i++) {
        if (src[i] == 0) {
            break;
        }
        dest[i] = src[i];
    }

    dest[i] = 0;

    return dest;
}

bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

int tonumericdigit(char c)
{
    return c - '0';
}