#pragma once

#if defined(TEST_KORE_QUERY)

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <poll.h>
#include <time.h>

#include <kore/kore.h>
#include <kore/http.h>
#include <kore/pgsql.h>
//#include "./kore/includes/jsonrpc.h"
#include <kore/tasks.h>

#include <libpq-fe.h>

static FILE *config_file_write(void);

#endif
