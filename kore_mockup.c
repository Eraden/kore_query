#if defined(TEST_KORE_QUERY)

#include "./kore_mockup.h"

#include <sys/param.h>

/* config.c */

static int configure_load(char *);

static FILE *config_file_write(void);

extern u_int8_t asset_builtin_kore_conf[];
extern u_int32_t asset_len_builtin_kore_conf;

static int configure_include(char *);

static int configure_bind(char *);

static int configure_domain(char *);

static int configure_chroot(char *);

static int configure_runas(char *);

static int configure_workers(char *);

static int configure_pidfile(char *);

static int configure_rlimit_nofiles(char *);

static int configure_max_connections(char *);

static int configure_accept_threshold(char *);

static int configure_set_affinity(char *);

static int configure_socket_backlog(char *);

static int configure_certfile(char *);

static int configure_certkey(char *);

static int configure_tls_version(char *);

static int configure_tls_cipher(char *);

static int configure_tls_dhparam(char *);

static int configure_client_certificates(char *);

static int configure_handler(int, char *);

static int configure_static_handler(char *);

static int configure_dynamic_handler(char *);

static int configure_accesslog(char *);

static int configure_http_header_max(char *);

static int configure_http_body_max(char *);

static int configure_http_hsts_enable(char *);

static int configure_http_keepalive_time(char *);

static int configure_http_request_limit(char *);

static int configure_http_body_disk_offload(char *);

static int configure_http_body_disk_path(char *);

static int configure_validator(char *);

static int configure_params(char *);

static int configure_validate(char *);

static int configure_authentication(char *);

static int configure_authentication_uri(char *);

static int configure_authentication_type(char *);

static int configure_authentication_value(char *);

static int configure_authentication_validator(char *);

static int configure_websocket_maxframe(char *);

static int configure_websocket_timeout(char *);

static int configure_pgsql_conn_max(char *);

static int configure_task_threads(char *);

static void domain_sslstart(void);

static void kore_parse_config_file(const char *);

static struct {
  const char *name;

  int (*configure)(char *);
} config_names[] = {
    {"include",                  configure_include},
    {"bind",                     configure_bind},
    {"load",                     configure_load},
    {"domain",                   configure_domain},
    {"chroot",                   configure_chroot},
    {"runas",                    configure_runas},
    {"workers",                  configure_workers},
    {"worker_max_connections",   configure_max_connections},
    {"worker_rlimit_nofiles",    configure_rlimit_nofiles},
    {"worker_accept_threshold",  configure_accept_threshold},
    {"worker_set_affinity",      configure_set_affinity},
    {"pidfile",                  configure_pidfile},
    {"socket_backlog",           configure_socket_backlog},
    {"tls_version",              configure_tls_version},
    {"tls_cipher",               configure_tls_cipher},
    {"tls_dhparam",              configure_tls_dhparam},
    {"certfile",                 configure_certfile},
    {"certkey",                  configure_certkey},
    {"client_certificates",      configure_client_certificates},
    {"static",                   configure_static_handler},
    {"dynamic",                  configure_dynamic_handler},
    {"accesslog",                configure_accesslog},
    {"http_header_max",          configure_http_header_max},
    {"http_body_max",            configure_http_body_max},
    {"http_hsts_enable",         configure_http_hsts_enable},
    {"http_keepalive_time",      configure_http_keepalive_time},
    {"http_request_limit",       configure_http_request_limit},
    {"http_body_disk_offload",   configure_http_body_disk_offload},
    {"http_body_disk_path",      configure_http_body_disk_path},
    {"validator",                configure_validator},
    {"params",                   configure_params},
    {"validate",                 configure_validate},
    {"authentication",           configure_authentication},
    {"authentication_uri",       configure_authentication_uri},
    {"authentication_type",      configure_authentication_type},
    {"authentication_value",     configure_authentication_value},
    {"authentication_validator", configure_authentication_validator},
    {"websocket_maxframe",       configure_websocket_maxframe},
    {"websocket_timeout",        configure_websocket_timeout},
    {"pgsql_conn_max",           configure_pgsql_conn_max},
    {"task_threads",             configure_task_threads},
    {NULL, NULL},
};

char *config_file = NULL;
static u_int8_t current_method = 0;
static struct kore_auth *current_auth = NULL;
static struct kore_module_handle *current_handler = NULL;

extern const char *__progname;
static struct kore_domain *current_domain = NULL;

void kore_parse_config(void) {}

static void kore_parse_config_file(const char *fpath) {}

static int configure_include(char *path) { return (KORE_RESULT_OK); }

static int configure_bind(char *options) { return (KORE_RESULT_OK); }

static int configure_load(char *options) { return (KORE_RESULT_OK); }

static FILE *config_file_write(void) { return (NULL); }

static int configure_tls_version(char *version) { return (KORE_RESULT_OK); }

static int configure_tls_cipher(char *cipherlist) { return (KORE_RESULT_OK); }

static int configure_tls_dhparam(char *path) { return (KORE_RESULT_OK); }

static int configure_client_certificates(char *options) { return (KORE_RESULT_OK); }

static int configure_certfile(char *path) { return (KORE_RESULT_OK); }

static int configure_certkey(char *path) { return (KORE_RESULT_OK); }

static int configure_domain(char *options) { return (KORE_RESULT_OK); }

static int configure_static_handler(char *options) { return (KORE_RESULT_OK); }

static int configure_dynamic_handler(char *options) { return (KORE_RESULT_OK); }

static int configure_handler(int type, char *options) { return (KORE_RESULT_OK); }

static int configure_accesslog(char *path) { return (KORE_RESULT_OK); }

static int configure_http_header_max(char *option) { return (KORE_RESULT_OK); }

static int configure_http_body_max(char *option) { return (KORE_RESULT_OK); }

static int configure_http_body_disk_offload(char *option) { return (KORE_RESULT_OK); }

static int configure_http_body_disk_path(char *path) { return (KORE_RESULT_OK); }

static int configure_http_hsts_enable(char *option) { return (KORE_RESULT_OK); }

static int configure_http_keepalive_time(char *option) { return (KORE_RESULT_OK); }

static int configure_http_request_limit(char *option) { return (KORE_RESULT_OK); }

static int configure_validator(char *name) { return (KORE_RESULT_OK); }

static int configure_params(char *options) { return (KORE_RESULT_ERROR); }

static int configure_validate(char *options) { return (KORE_RESULT_OK); }

static int configure_authentication(char *options) { return (KORE_RESULT_OK); }

static int configure_authentication_type(char *option) { return (KORE_RESULT_OK); }

static int configure_authentication_value(char *option) { return (KORE_RESULT_OK); }

static int configure_authentication_validator(char *validator) { return (KORE_RESULT_OK); }

static int configure_authentication_uri(char *uri) { return (KORE_RESULT_OK); }

static int configure_websocket_maxframe(char *option) { return (KORE_RESULT_OK); }

static int configure_websocket_timeout(char *option) { return (KORE_RESULT_OK); }

static int configure_chroot(char *path) { return KORE_RESULT_OK; }

static int configure_runas(char *user) { return (KORE_RESULT_OK); }

static int configure_workers(char *option) { return (KORE_RESULT_OK); }

static int configure_pidfile(char *path) { return (KORE_RESULT_OK); }

static int configure_max_connections(char *option) { return (KORE_RESULT_OK); }

static int configure_rlimit_nofiles(char *option) { return (KORE_RESULT_OK); }

static int configure_accept_threshold(char *option) { return (KORE_RESULT_OK); }

static int configure_set_affinity(char *option) { return (KORE_RESULT_OK); }

static int configure_socket_backlog(char *option) { return (KORE_RESULT_OK); }

static void domain_sslstart(void) {}

static int configure_pgsql_conn_max(char *option) { return (KORE_RESULT_OK); }

static int configure_task_threads(char *option) { return (KORE_RESULT_OK); }

/* http.c */

static int http_body_recv(struct netbuf *);

static int http_body_rewind(struct http_request *);

static void http_error_response(struct connection *, int);

static void http_argument_add(struct http_request *, const char *, char *);

static void http_response_normal(struct http_request *,
                                 struct connection *, int, const void *, size_t);

static void multipart_add_field(struct http_request *, struct kore_buf *,
                                const char *, const char *, const int);

static void multipart_file_add(struct http_request *, struct kore_buf *,
                               const char *, const char *, const char *, const int);

static int multipart_find_data(struct kore_buf *, struct kore_buf *,
                               size_t *, struct http_request *, const void *, size_t);

static int multipart_parse_headers(struct http_request *,
                                   struct kore_buf *, struct kore_buf *,
                                   const char *, const int);

static struct kore_buf *header_buf;
static char http_version[32];
static u_int16_t http_version_len;
static TAILQ_HEAD(, http_request) http_requests;
static TAILQ_HEAD(, http_request) http_requests_sleeping;
static struct kore_pool http_request_pool;
static struct kore_pool http_header_pool;
static struct kore_pool http_host_pool;
static struct kore_pool http_path_pool;
static struct kore_pool http_body_path;

int http_request_count = 0;
u_int32_t http_request_limit = HTTP_REQUEST_LIMIT;
u_int64_t http_hsts_enable = HTTP_HSTS_ENABLE;
u_int16_t http_header_max = HTTP_HEADER_MAX_LEN;
u_int16_t http_keepalive_time = HTTP_KEEPALIVE_TIME;
u_int64_t http_body_max = HTTP_BODY_MAX_LEN;
u_int64_t http_body_disk_offload = HTTP_BODY_DISK_OFFLOAD;
char *http_body_disk_path = HTTP_BODY_DISK_PATH;

void http_init(void) {}

void http_cleanup(void) {}

int http_request_new(struct connection *c, const char *host, const char *method, const char *path, const char *version,
                     struct http_request **out) { return (KORE_RESULT_OK); }

void http_request_sleep(struct http_request *req) {}

void http_request_wakeup(struct http_request *req) {}

void http_process(void) {}

void http_process_request(struct http_request *req) {}

void http_response_header(struct http_request *req, const char *header, const char *value) {}

void http_request_free(struct http_request *req) {}

void http_response(struct http_request *req, int status, const void *d, size_t l) {}

void http_response_stream(struct http_request *req, int status, void *base, size_t len, int (*cb)(struct netbuf *),
                          void *arg) {}

int http_request_header(struct http_request *req, const char *header, char **out) { return (KORE_RESULT_OK); }

int http_header_recv(struct netbuf *nb) { return (KORE_RESULT_OK); }

int http_argument_get(struct http_request *req, const char *name, void **out, void *nout,
                      int type) { return (KORE_RESULT_ERROR); }

int http_argument_urldecode(char *arg) { return KORE_RESULT_OK; }

struct http_file *http_file_lookup(struct http_request *req, const char *name) { return (NULL); }

ssize_t http_file_read(struct http_file *file, void *buf, size_t len) { return (0); }

void http_file_rewind(struct http_file *file) {}

void http_populate_post(struct http_request *req) {}

void http_populate_get(struct http_request *req) {}

void http_populate_multipart_form(struct http_request *req) {}

ssize_t http_body_read(struct http_request *req, void *out, size_t len) { return (0); }

int http_state_run(struct http_state *states, u_int8_t elm, struct http_request *req) { return (KORE_RESULT_OK); }

static int multipart_find_data(struct kore_buf *in, struct kore_buf *out, size_t *olen, struct http_request *req,
                               const void *needle, size_t len) { return (KORE_RESULT_OK); }

static int
multipart_parse_headers(struct http_request *req, struct kore_buf *in, struct kore_buf *hbuf, const char *boundary,
                        const int blen) { return (KORE_RESULT_OK); }

static void multipart_add_field(struct http_request *req, struct kore_buf *in, const char *name, const char *boundary,
                                const int blen) {}

static void multipart_file_add(struct http_request *req, struct kore_buf *in, const char *name, const char *fname,
                               const char *boundary, const int blen) {}

static void http_argument_add(struct http_request *req, const char *name, char *value) {}

static int http_body_recv(struct netbuf *nb) { return (KORE_RESULT_OK); }

static int http_body_rewind(struct http_request *req) { return (KORE_RESULT_OK); }

static void http_error_response(struct connection *c, int status) {}

static void
http_response_normal(struct http_request *req, struct connection *c, int status, const void *d, size_t len) {}

const char *http_status_text(int status) { return (""); }

const char *http_method_text(int method) { return (""); }

/* domain.c */

#define SSL_SESSION_ID    "kore_ssl_sessionid"

struct kore_domain_h domains;
struct kore_domain *primary_dom = NULL;
static u_int8_t keymgr_buf[2048];
static size_t keymgr_buflen = 0;
static int keymgr_response = 0;
int tls_version = KORE_TLS_VERSION_1_2;

static void domain_load_crl(struct kore_domain *);

void
kore_domain_init(void) {}

void
kore_domain_cleanup(void) {}

int
kore_domain_new(char *domain) { return 1; }

void
kore_domain_free(struct kore_domain *dom) {}

void
kore_domain_sslstart(struct kore_domain *dom) {}

void
kore_domain_callback(void (*cb)(struct kore_domain *)) {}

struct kore_domain *
kore_domain_lookup(const char *domain) { return (NULL); }

void
kore_domain_closelogs(void) {}

void
kore_domain_load_crl(void) {}

void
kore_domain_keymgr_init(void) {}

static void
domain_load_crl(struct kore_domain *dom) {}

/* kore.c */

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

static void
kore_server_start(void) {}

static void
kore_write_kore_pid(void) {}

/* accesslog.c */

struct kore_log_packet {
  u_int8_t	method;
  int		status;
  u_int16_t	time_req;
  u_int16_t	worker_id;
  u_int16_t	worker_cpu;
  u_int8_t	addrtype;
  u_int8_t	addr[sizeof(struct in6_addr)];
  char		host[KORE_DOMAINNAME_LEN];
  char		path[HTTP_URI_LEN];
  char		agent[HTTP_USERAGENT_LEN];
#if !defined(KORE_NO_TLS)
  char		cn[X509_CN_LENGTH];
#endif
};

void kore_accesslog_init(void) { }

void kore_accesslog_worker_init(void) {}

int kore_accesslog_write(const void *data, u_int32_t len) { return (KORE_RESULT_OK); }

void kore_accesslog(struct http_request *req) { }

#endif