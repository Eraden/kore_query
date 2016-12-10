#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <poll.h>
#include <time.h>

#include "kore.h"
#include "http.h"
#include "pgsql.h"
//#include "./kore/includes/jsonrpc.h"
#include "tasks.h"

#include <libpq-fe.h>

static FILE *config_file_write(void);
