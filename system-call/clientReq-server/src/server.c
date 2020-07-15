#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "../../inc/errExit.h"
#include "../../inc/request_response.h"
#include "../../inc/shared_memory.h"
#include "../../inc/keyManager.h"
#include "../../inc/semaphore.h"

///////////////// FIFO /////////////////////
char *path2ServerFIFO = "/tmp/fifo_server";
char *baseClientFIFO = "/tmp/fifo_client.";
int serverFIFO, serverFIFO_extra;
struct Response response;
////////////////////////////////////////////

pid_t kManager; // PID del processo figlio KeyManager

int serviceCounter; // variabile per contare il numero dei servizi e rendere sempre univoca la chiave

///////////////////////////////DATI DELLA MEMORIA CONDIVISA////////////////////////////////////////////
int shmidServer; // ID della memoria che verrà allocata per il KeyManager, globale perché la userò in più processi
struct KeyManager *row; // struttura che verrà contenuta nella memoria condivisa

int shmidCount; // ID della memoria che verrà allocata per il contatore dei segmenti occupati
int *count; // variabile che userò per contare i segmenti occupati
///////////////////////////////////////////////////////////////////////////////////////////////////////

////////CHIUDE I DESCRITTORI DELLE FIFO, LE RIMUOVE E TERMINA IL PROCESSO///////////////
////////RIMUOVE I 3 SEMAFORI CREATI, DEALLOCA E RIMUOVE LA MEMORIA CONDIVISA////////////
void esci(int sig) {
    if (sig == SIGTERM){
        printf("\nOh no è arrivato SIGTERM\n\n");
        kill(kManager, SIGTERM); // se arriva SIGTERM termino il processo figlio kManager
    }

    // chiude le FIFO
    if (serverFIFO != 0 && close(serverFIFO) == -1)
        errExit("<Server> close fallita");
    if (serverFIFO_extra != 0 && close(serverFIFO_extra) == -1)
        errExit("<Server> close fallita");

    // rimuove le FIFO
    if (unlink(path2ServerFIFO) != 0)
        errExit("<Server> unlink fallita");

    // rimuovo il semaforo
    if (semctl(SEMID, 0, IPC_RMID, NULL) == -1)
        errExit("<Server> semctl IPC_RMID failed");

    // dealloca l'indirizzo della memoria condivisa del KeyManager
    free_shared_memory(row);
    // rimuove la memoria condivisa del KeyManager
    remove_shared_memory(shmidServer);

    // dealloca l'indirizzo della memoria condivisa del contatore dei segmenti occupati
    free_shared_memory(count);
    // rimuove la memoria condivisa del contatore dei segmenti occupati
    remove_shared_memory(shmidCount);

    // termina il processo
    _exit(0);
}
///////////////////////////////////////////////////////////////////////////////////////

//FUNZIONE PER GENERARE UN RISULTATO USA E GETTA CON RICONOSCIMENTO DEL SERVIZIO//
//////////////CODICE 58 SALVA - CODICE 61 STAMPA - CODICE 40 INVIA////////////////
int hashCode(char *str) {
    int tmp = 0, reminder = 0, hash = 0;
    for (int i = 0; i < strlen(str); i++){  // scorro tutta la stringa
        tmp = str[i];   // salvo il valore del codice ASCII della lettera
        while(tmp != 0){ // sommo cifra per cifra il valore del codice ASCII, per esempio 'a' (ASCII 115) risultato 5+1+1
            reminder = tmp % 10;
            hash += reminder;
            tmp = tmp / 10;
        }
    }
    serviceCounter++; // a ogni richiesta da parte di clientReq aumento il contatore
    return (hash*1000)+serviceCounter; // restituisco un codice univoco con il riconoscimento del servizio
}
//////////////////////////////////////////////////////////////////////////////////

////////////////////////FUNZIONE PER MANDARE LA RISPOSTA A clientReq/////////////////////////
void mandaRisposta(struct Request *request) {

    // creiamo il path del ClientFIFO
    char path2ClientFIFO [25];
    sprintf(path2ClientFIFO, "%s%d", baseClientFIFO, request->cPid);

    // apriamo la FIFO
    int clientFIFO = open(path2ClientFIFO, O_WRONLY);
    if (clientFIFO == -1) {
        printf("<Server> open fallita");
        return;
    }

    // creiamo la risposta per clientReq
    response.chiave = hashCode(request->serviceName);

    // scriviamo la response nella FIFO aperta
    if (write(clientFIFO, &response, sizeof(struct Response)) != sizeof(struct Response)) {
        printf("<Server> write fallita");
        return;
    }

    // chiudiamo la FIFO
    if (close(clientFIFO) != 0)
        printf("<Server> close fallita");
}
////////////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]) {

    printf("\nBenvenuto nel Server! Il mio PID è %d\n", getpid());

    // creo un semaforo
    SEMID = semget(SEMKEY, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (SEMID == -1)
        errExit("<Server> semget failed");

    union semun arg;
    arg.val = 1;
    // imposto il semaforo appena creato ad 1
    if(semctl(SEMID, 0, SETVAL, arg) == -1)
        errExit("<Server> semctl failed");

    // alloco in shmidServer la memoria condivisa per il KeyManager
    shmidServer = alloc_shared_memory(SHSERVERKEY, (sizeof(struct KeyManager) * SHMSIZE));
    // assegno l'indirizzo a row
    row = (struct KeyManager *)get_shared_memory(shmidServer, 0);

    // alloco in shmidCount la memoria condivisa per il contatore dei segmenti occupati
    shmidCount = alloc_shared_memory(SHMCOUNT, sizeof(int));
    // assegno l'indirizzo del contatore a count
    count = (int *)get_shared_memory(shmidCount, 0);

    // creo il processo figlio KeyManager
    if((kManager = fork()) == -1){
        errExit("<Server> non ho creato il figlio KeyManager");
    }else if(kManager == 0){
        keyManager();
    }

    // creo la FIFO con i seguenti permessi
    // user:  read, write
    // group: write
    // other: no permission
    if (mkfifo(path2ServerFIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1)
        errExit("<Server> mkfifo fallita");

    sigset_t mySet; // creo un set per contenere i segnali
    sigfillset(&mySet); // inizializzo mySet a contenere tutti i segnali
    sigdelset(&mySet, SIGTERM); // rimuovo SIGTERM da mySet
    sigprocmask(SIG_SETMASK, &mySet, NULL); // blocco tutti i segnali tranne SIGTERM
    
    // imposto un gestore del segnale per il segnale SIGTERM
    if (signal(SIGTERM, esci) == SIG_ERR){
        errExit("<Server> assegnazione signal handler fallita");
    }

    // apro il serverFIFO in modalità sola lettura
    serverFIFO = open(path2ServerFIFO, O_RDONLY);
    if (serverFIFO == -1)
        errExit("<Server> open read-only fallita");

    // apro un descrittore extra, il server non vedrà la fine del file
    // fino a quando tutti i clientReq chiuderanno la write end della FIFO
    serverFIFO_extra = open(path2ServerFIFO, O_WRONLY);
    if (serverFIFO_extra == -1)
        errExit("<Server> open write-only fallita");

    // preparo per leggere la richiesta e rispondere
    struct Request request;
    int bR = -1;
    int pos; // per scorrere la memoria condivisa

    do {      
        printf("\n<Server> sono in attesa di una richiesta\n");
        // leggo la richiesta dalla FIFO
        bR = read(serverFIFO, &request, sizeof(struct Request));

        // controllo il numero di byte letti
        if (bR == -1) {
            printf("<Server> la FIFO non è valida\n");
        }else if (bR != sizeof(struct Request) || bR == 0){
            printf("<Server> non ho ricevuto una richiesta valida\n");
        }else{
            mandaRisposta(&request); // rispondo a clientReq
            if(getSemaphoresValue(SEMID) == 0) // visualizzo un messaggio se la SHM è occupata in altri processi
                printf("\n<Server> La memoria condivisa è occupata\n");
            semOp(SEMID, 0, -1); // funzione lock del semaforo
            for(pos = 0; pos < SHMSIZE; pos++){ // scorro la memoria condivisa
                if(row[pos].chiave == 0){ // se il segmento è vuoto salvo i dati all'interno
                    strcpy(row[pos].userId, request.userId); 
                    row[pos].chiave = response.chiave;
                    row[pos].timestamp = time(NULL);
                    count[0]++; // incremento il contatore dei segmenti occupati
                    break; // al termina del salvataggio esco dal ciclo
                }
            }
            if(count[0] == SHMSIZE){
                printf("\nIl KeyManager è pieno\n");
            }
            semOp(SEMID, 0, 1); // funzione unlock del semaforo
        }
    } while (bR != -1);

    // rimuovo la FIFO e termino i processi
    esci(0);
}
