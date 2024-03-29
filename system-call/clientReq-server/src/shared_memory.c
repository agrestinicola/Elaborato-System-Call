#include <stdio.h>

#include <sys/shm.h>
#include <sys/stat.h>

#include "../../inc/errExit.h"
#include "../../inc/shared_memory.h"

int alloc_shared_memory(key_t key, size_t size) {
    // ottiene o crea un segmento di memoria condiviso
    int shmid = shmget(key, size, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (shmid == -1)
        errExit("shmget failed");

    return shmid;
}

void *get_shared_memory(int shmid, int shmflg) {
    // alloca il segmento di memoria condivisa
    void *ptr_sh = shmat(shmid, NULL, shmflg);
    if (ptr_sh == (void *)-1)
        errExit("shmat failed");

    return ptr_sh;
}

void free_shared_memory(void *ptr_sh) {
    // dealloca il segmento di memoria condivisa
    if (shmdt(ptr_sh) == -1)
        errExit("shmdt failed");
}

void remove_shared_memory(int shmid) {
    // rimuove il segmento di memoria condivisa
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        errExit("shmctl failed");
}
