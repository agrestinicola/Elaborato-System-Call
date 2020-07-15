#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>
#include <time.h>

#define SHSERVERKEY 9999 // key della memoria condivisa per il KeyManager
#define SHMSIZE 5 // numero di struct KeyManager che contiene la memoria condivisa

#define SHMCOUNT 6666 // key della memoria condivisa per il contatore dei segmenti occupati

// questa struttura definisce una richiesta da parte del client
struct KeyManager {
    char userId[20];
    int chiave;
    time_t timestamp;
};

// La funzione alloc_shared_memory crea, nel caso non esistesse, un segmento
// di memoria condivisa di grandezza size e chiave shmKey.
// restituisce shmid in caso di successo, altrimenti termina il processo chiamante
int alloc_shared_memory(key_t key, size_t size);

// La funzione get_shared_memory alloca il segmento di memoria condivisa nell'indirizzo logico
// del processo chiamante.
// Restituisce un puntatore al segmento di memoria condivisa allocato,
// altrimenti termina il processo chiamante
void *get_shared_memory(int shmid, int shmflg);

// La funzione free_shared_memory dealloca il segmento di memoria condiviso dall'indirizzo logico
// del processo chiamante.
// Nel caso non avesse successo, viene terminato il processo chiamante
void free_shared_memory(void *ptr_sh);

// La funzione remove_shared_memory rimuove il segmento di memoria condiviso
// Nel caso non avesse successo, viene terminato il processo chiamante
void remove_shared_memory(int shmid);

#endif
