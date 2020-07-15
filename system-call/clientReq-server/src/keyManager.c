#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h> // segnali
#include <string.h>
#include <time.h> // time types

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h> // shared memory facility
#include <sys/sem.h> // semaphore facility

#include "../../inc/errExit.h"
#include "../../inc/shared_memory.h" // gestire più ordinatamente la SHM
#include "../../inc/keyManager.h"
#include "../../inc/semaphore.h" // gestire più ordinatamente il SEM

int shmidKeyManager;
struct KeyManager *check;

int shmidCountKm;
int *countKm;

void esciKeyManager(){
    _exit(0);
}

void checkTimestamp(int sig){
    if(getSemaphoresValue(SEMID) == 0) // visualizzo un messaggio se la SHM è occupata in altri processi
        printf("\n<KeyManager> La memoria condivisa è occupata\n"); 
    semOp(SEMID, 0, -1); // funzione lock del semaforo
    
    printf("\n<KeyManager> Ora controllo i timestamp\n");
    
    int i;
    for(i=0; i<SHMSIZE; i++){
        if(check[i].chiave != 0){ // se il segmento non è vuoto
            printf("\nSto controllando %d: %s - %d - %lld\n", i+1, check[i].userId, check[i].chiave, (long long)check[i].timestamp);
            if(time(NULL) - check[i].timestamp >= 300){ // se il timestamp è vecchio di 5 minuti
                strcpy(check[i].userId, "");            // azzero il segmento
                check[i].chiave=0;
                check[i].timestamp=0;
                countKm[0]--; // diminuisco il contatore dei segmenti occupati
                printf("\n<KeyManager> Segmento %d deallocato\n", i+1);
            }   
        }else if(check[i].chiave == 0){
            printf("\n%d: %s - %d - %lld\n", i+1,check[i].userId, check[i].chiave, (long long)check[i].timestamp);
        }
    }
    semOp(SEMID, 0, 1); // funzione unlock del semaforo
}

void keyManager(){
    // ottengo l'identificatore del semaforo già esistente
    SEMID = semget(SEMKEY, 1, S_IRUSR | S_IWUSR);
    if (SEMID == -1)
        errExit("<KeyManager> semget failed");

    shmidKeyManager = alloc_shared_memory(SHSERVERKEY, (sizeof(struct KeyManager) * SHMSIZE));
    check = (struct KeyManager *)get_shared_memory(shmidKeyManager, 0);

    shmidCountKm = alloc_shared_memory(SHMCOUNT, sizeof(int));
    countKm = (int *)get_shared_memory(shmidCountKm, 0);

    // imposto un gestore del segnale per SIGALRM
    if (signal(SIGALRM, checkTimestamp) == SIG_ERR){
        errExit("<KeyManager> SIGALRM assegnazione signal handler fallita");
    }    

    // imposto un gestore del segnale per SIGTERM
    if (signal(SIGTERM, esciKeyManager) == SIG_ERR){
        errExit("<KeyManager> SIGTERM assegnazione signal handler fallita");
    }

    // ogni 30 secondi SIGALRM per controllare i timestamp
    while(1){
        alarm(30);
        pause();
    }
}
