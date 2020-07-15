#ifndef _ERREXIT_HH
#define _ERREXIT_HH

/* errExit è una funzione di supporto per stampare
 * il messaggio d'errore dell'ultima system-call chiamata.
 * errExit termina anche il processo che l'ha chiamata.
 */
void errExit(const char *msg);

#endif
