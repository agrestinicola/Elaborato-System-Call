#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/msg.h>
#include <sys/stat.h>

#include "../../inc/errExit.h"

struct mymsg {  //struttura della mia coda di messaggi
    long mtype;
    char mtext[100];
};

int main (int argc, char *argv[]) {
    printf("\nHai richiesto il servizio INVIA.\n");

    int keyMsgKey = atoi(argv[3]);  // converto in intero l'argv[3]
    int msqid = msgget(keyMsgKey, IPC_CREAT | S_IRUSR | S_IWUSR); // creo la coda di messaggi con l'argv[3]
    if (msqid == -1)
        errExit("msgget failed");

    struct mymsg m; // inizializzo la mia struttura
    m.mtype = 1;    // che avrà il tipo 1
    for(int i=4; i<argc; i++){  // concateno in un'unica stringa gli argomenti
        strcat(m.mtext, argv[i]);
    }
    size_t mSize = sizeof(struct mymsg) - sizeof(long); // calcolo la grandezza della stringa
    if (msgsnd(msqid, &m, mSize, 0) == -1)  // invio il messaggio alla coda di messaggi
        errExit("msgsnd failed");

    printf("\nArgomenti inviati: %s\n\n", m.mtext);    // stampo per controllare se è corretto il messaggio

    return 0;
}
