#include "../kore_mockup.h"

#ifdef TEST_KORE_QUERY

volatile sig_atomic_t sig_recv;

struct listener_head listeners;
u_int8_t nlisteners;
pid_t kore_pid = -1;
u_int16_t cpu_count = 1;
int foreground = 0;
int kore_debug = 0;
u_int8_t worker_count = 0;
int skip_chroot = 0;
char *chroot_path = NULL;
int skip_runas = 0;
char *runas_user = NULL;
u_int32_t kore_socket_backlog = 5000;
char *kore_pidfile = KORE_PIDFILE_DEFAULT;
char *kore_tls_cipher_list = KORE_DEFAULT_CIPHER_LIST;

static void usage(void);

static void version(void);

static void kore_server_start(void);

static void kore_write_kore_pid(void);

static void kore_server_sslstart(void);

static void
usage(void) {}

static void
version(void) { exit(0); }

int
kore_server_bind(const char *ip, const char *port, const char *ccb) { return (KORE_RESULT_OK); }

void
kore_listener_cleanup(void) {}

void
kore_signal(int sig) {}

static void
kore_server_sslstart(void) {}

static void kore_worker__init() {
  worker_count = 1;
  kore_pgsql_init();
}

static void
kore_server_start(void) {
  u_int32_t tmp;
  void (*preload)(void);

  kore_pid = getpid();

  *(void **) &(preload) = kore_module_getsym("kore_preload");
  if (preload != NULL)
    preload();

  kore_platform_proctitle("kore [parent]");
  kore_msg_init();
  kore_worker__init();

  /* Set worker_max_connections for kore_connection_init(). */
  tmp = worker_max_connections;
  worker_max_connections = worker_count;

  net_init();
  worker_max_connections = tmp;
}

static void
kore_write_kore_pid(void) {}

void kore__main() {
  kore_mem_init();
  kore_pid = getpid();
  nlisteners = 0;
  LIST_INIT(&listeners);
  kore_log_init();
  kore_auth_init();
  kore_validator_init();
  kore_domain_init();
  kore_module_init();
  kore_server_sslstart();

  if (config_file == NULL)
    usage();

  kore_parse_config();
  kore_platform_init();

  kore_server_start();
}

void kore__terminate() {
  kore_platform_event_cleanup();
  kore_connection_cleanup();
  kore_domain_cleanup();
  net_cleanup();
  kore_listener_cleanup();
}

#endif