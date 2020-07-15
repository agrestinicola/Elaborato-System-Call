#include "../../inc/errExit.h"

#include <stdio.h>
#include <errno.h> // system error number
#include <stdlib.h>

void errExit(const char *msg) {
    perror(msg);
    exit(1);
}
