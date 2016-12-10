#pragma once

#ifdef TEST_KORE_QUERY

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <poll.h>
#include <time.h>
#include <sys/param.h>

#include "./vendor/kore/includes/kore.h"
#include "./vendor/kore/includes/http.h"
#include "./vendor/kore/includes/pgsql.h"
#include "./vendor/kore/includes/tasks.h"

#include <libpq-fe.h>

static FILE *config_file_write(void);

void kore__main();

void kore__terminate();

#endif
