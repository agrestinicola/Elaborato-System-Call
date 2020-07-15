#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../inc/errExit.h"

int main (int argc, char *argv[]) {
    printf("\nHai richiesto il servizio SALVA su file.\n");

    // creo e apro il file in modalità scrittura/lettura
    int file = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (file == -1) {
        printf("Il file %s non è stato creato\n", argv[1]);
    }
    
    // scrivo sul file creato gli argomenti
    for(int i=4; i<argc; i++)
        write(file, argv[i], strlen(argv[i]));

    printf("\nFile %s creato!\n\n", argv[3]);

    // chiudo il descrittore del file
    close(file);

    return 0;
}
