#include "test.h"
#include <stdio.h>
#include <malloc.h>

EXPORT void malloc10()
{
    malloc(10);
}

EXPORT void malloc11()
{
    malloc(11);
}

// static void __attribute__((constructor)) swap_hook()
// {
//     malloc(100);
// }