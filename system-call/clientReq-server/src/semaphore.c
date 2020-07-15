#include <sys/sem.h>
#include <stdio.h>

#include "../../inc/semaphore.h"
#include "../../inc/errExit.h"

void semOp (int semid, unsigned short sem_num, short sem_op){
    struct sembuf sop = {.sem_num = sem_num, 
                         .sem_op = sem_op, 
                         .sem_flg = 0};
    if (semop(semid, &sop, 1) == -1)
        errExit("semop failed");
}

int getSemaphoresValue (int semid) {
    int value;
    value = semctl(semid, 0, GETVAL, 0);
    if(value == -1)
        errExit("semctl GETALL failed");
        
    return value;
}
