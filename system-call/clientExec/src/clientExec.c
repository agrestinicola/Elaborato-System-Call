#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "../../inc/errExit.h"
#include "../../inc/request_response.h"
#include "../../inc/shared_memory.h"
#include "../../inc/semaphore.h"

int shmidClientExec;
struct KeyManager *check;

int shmidCountExec;
int *countExec;

int main (int argc, char *argv[]) {

    printf("\nBenvenuto nel ClientExec!\n");

    // ottengo l'identificatore del semaforo già esistente
    SEMID = semget(SEMKEY, 1, S_IRUSR | S_IWUSR);
    if (SEMID == -1)
        errExit("<ClientExec> semget failed");

    // alloco in shmidServer la memoria condivisa
    shmidClientExec = alloc_shared_memory(SHSERVERKEY, sizeof(struct KeyManager) * SHMSIZE);
    // assegno l'indirizzo a check
    check = (struct KeyManager *)get_shared_memory(shmidClientExec, 0);

    shmidCountExec = alloc_shared_memory(SHMCOUNT, sizeof(int));
    countExec = (int *)get_shared_memory(shmidCountExec, 0);

    int i, codiceServizio;
    char *id = argv[1];
    int chiaveInserita = atoi(argv[2]);

    if(getSemaphoresValue(SEMID) == 0){ // visualizzo un messaggio se la SHM è occupata in altri processi e termino
        printf("\n<ClientExec> La memoria condivisa è occupata\n");
        return 0;
    }
    semOp(SEMID, 0, -1); // funzione lock del semaforo
    
    for(i=0; i<SHMSIZE; i++){
        if(strcmp(id, check[i].userId) == 0 && chiaveInserita == check[i].chiave){  // controllo che esistano l'utente e la chiave inseriti
            
            printf("\nLogin: %s - %d\n", check[i].userId, check[i].chiave);
            codiceServizio = check[i].chiave/1000;  // salvo il codice del servizio richiesto
            
            // resetto il segmento di memoria condiviso appena usato
            strcpy(check[i].userId, "");
            check[i].chiave=0;
            check[i].timestamp=0;

            countExec[0]--;// diminuisco il contatore dei segmenti occupati

            if(codiceServizio == 61){   // servizio STAMPA 
                semOp(SEMID, 0, 1); // funzione unlock del semaforo
                if (execv("stampa", argv) == -1) // eseguo il programma stampa
                    errExit("<ClientExec> execv fallita");
            }

            if(codiceServizio == 58){   // servizio SALVA
                semOp(SEMID, 0, 1); // funzione unlock del semaforo
                if (execv("salva", argv) == -1) // eseguo il programma salva
                    errExit("<ClientExec> execv fallita");
            }

            if(codiceServizio == 40){   // servizio INVIA
                semOp(SEMID, 0, 1); // funzione unlock del semaforo
                if (execv("invia", argv) == -1) // eseguo il programma invia
                    errExit("<ClientExec> execv fallita");
            }
            return 0;
        }
    }
    printf("\nLa coppia non è presente nella memoria condivisa\n\n");
    semOp(SEMID, 0, 1); // funzione unlock del semaforo
    return 1;
}
