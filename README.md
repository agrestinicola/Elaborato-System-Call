# Elaborato-System-Call
Il mio elaborato per il corso di Sistemi Operativi.

1) Aprire tre terminali

2) Su due terminali andare nella cartella ../clientReq-server e su uno in ../clientExec

3) Compilare tutto con make

4) Lanciare ./server

5) Lanciare ./clientReq

6) Inserire nico e stampa

7) Lanciare ./clientReq

8) Inserire nico e salva

9) Lanciare ./clientReq

10) Inserire nico e invia

11) Verificare che KeyManager stampi corretamente e controlli la memoria condivisa

12) Verificare con ipcs che sia stata creata la SHM e il SEM

13) Lanciare `./clientExec nico 61001 provo a stampare questo`

14) Verificare che lo stampa a video e che sparisca dalla SHM controllando KeyManager

15) Lanciare `./clientExec nico 58002 nomefile salvo nel file questo`

16) Verificare la creazione del file e il contenuto se corrisponde agli argomenti

17) Lanciare `./clientExec nico 40003 idMessageQueue invio questo alla coda di messaggi` (idMessageQueue = numero a caso)

18) Verificare che abbia creato la coda di messaggi con ipcs

19) Terminare con il comando kill -15 PID

(OPZIONALE) Diminuire i tempi di alarm con alarm(10) (SIGALRM) in KeyManager e i 300 secondi a tipo 20 (nell'if) del timestamp per controllare se viene azzerato il segmento dalla SHM nel caso sia più vecchio
(OPZIONALE) Modificare in ../inc/shared_memory.h la SHMSIZE aumentando e diminuendo la grandezza e vedere se funziona lo stesso
