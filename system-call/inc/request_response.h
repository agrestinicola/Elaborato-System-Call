#ifndef _REQUEST_RESPONSE_HH
#define _REQUEST_RESPONSE_HH

#include <sys/types.h>

// richiesta da clientReq.c -> server.c
struct Request {   
    pid_t cPid; // PID del clientReq
    char userId[20]; // stringa per userId
    char serviceName[20]; // stringa per serviceName
};

// risposta del server.c -> clientReq.c
struct Response {
    int chiave; // risultato usa e getta e con un riconoscimento del servizio richiesto
};

#endif
