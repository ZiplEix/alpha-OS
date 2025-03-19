#include "stdlib.h"
#include "alphaos.h"

void *malloc(size_t size)
{
    return __malloc(size);
}

void free(void *ptr)
{

}
