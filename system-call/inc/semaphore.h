#ifndef _SEMAPHORE_HH
#define _SEMAPHORE_HH

#define SEMKEY 1234 // key del semaforo

int SEMID;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// funzione che modifica il valore del semaforo
void semOp (int semid, unsigned short sem_num, short sem_op);

// funzione che restituisce il valore di un semaforo
int getSemaphoresValue (int semid);

#endif
