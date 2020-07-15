#include <stdlib.h>
#include <stdio.h>

#include "../../inc/errExit.h"

int main (int argc, char *argv[]) {
    printf("\nHai richiesto il servizio di STAMPA!\n\nGli argomenti forniti da terminale sono:");
    for(int i=3; i<argc; i++)
        printf(" %s", argv[i]);
    printf("\n\n");

    return 0;
}
