#include <stdlib.h> //  standard library definitions
#include <stdio.h> // standard buffered input/output
#include <sys/types.h> // data type
#include <sys/stat.h> // informations about files attributes
#include <fcntl.h> // file control options
#include <unistd.h> // standard symbolic constants and types
#include <string.h> // string operations

#include "../../inc/errExit.h" // gestire eventuali errori delle system-call
#include "../../inc/request_response.h" // gestire request e response delle FIFO

/////////////PATH PER LE FIFO//////////////
char *path2ServerFIFO = "/tmp/fifo_server";
char *baseClientFIFO = "/tmp/fifo_client.";
///////////////////////////////////////////

///STRINGHE PER IL CONTROLLO DEL SERVIZIO///
char *stampa = "stampa";
char *salva  = "salva";
char *invia  = "invia";
////////////////////////////////////////////

int main (int argc, char *argv[]) {

    printf("\nBenvenuto nel clientReq!\n");
    printf("Sono disponibili i seguenti servizi:\n");
    printf("- salva\n");
    printf("- stampa\n");
    printf("- invia\n");

    // clientReq crea la sua FIFO in /tmp
    char path2ClientFIFO [25];
    sprintf(path2ClientFIFO, "%s%d", baseClientFIFO, getpid());

    // crea una FIFO con i seguenti permessi:
    // user:  read, write
    // group: write
    // other: no permission
    if (mkfifo(path2ClientFIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1)
        errExit("mkfifo fallita");

    // clientReq apre la FIFO del server per mandare una request
    int serverFIFO = open(path2ServerFIFO, O_WRONLY);
    if (serverFIFO == -1)
        errExit("open fallita");

    // prepariamo una richiesta
    struct Request request;
    request.cPid = getpid();
    printf("Inserire un codice identificativo: ");
    scanf("%s", request.userId);
    do{
        printf("Inserire un servizio: ");
        scanf("%s", request.serviceName);
    }while(strcmp(request.serviceName, stampa) != 0 && strcmp(request.serviceName, salva) != 0 && strcmp(request.serviceName, invia) != 0);

    // clientReq manda la request tramite la FIFO del server
    if (write(serverFIFO, &request, sizeof(struct Request)) != sizeof(struct Request))
        errExit("write fallita");

    // clientReq apre la sua FIFO in modalità di sola lettura per ricevere una response
    int clientFIFO = open(path2ClientFIFO, O_RDONLY);
    if (clientFIFO == -1)
        errExit("open fallita");

    // clientReq legge la respose dal server
    struct Response response;
    if (read(clientFIFO, &response,
        sizeof(struct Response)) != sizeof(struct Response))
        errExit("read fallita");

    // clientReq stampa a video il risultato
    printf("\nCodice Identificativo Utente: %s\nServizio: %s\nChiave rilasciata dal server: %d\n\n", request.userId, request.serviceName, response.chiave);

    // clientReq chiude la sua FIFO
    if (close(serverFIFO) != 0 || close(clientFIFO) != 0)
        errExit("close fallita");

    // clientReq rimuove la sua FIFO dal sistema
    if (unlink(path2ClientFIFO) != 0)
        errExit("unlink fallita");

    return 0;
}
