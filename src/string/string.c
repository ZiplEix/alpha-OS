#include "string.h"

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

bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

int tonumericdigit(char c)
{
    return c - '0';
}