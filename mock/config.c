#include "../kore_mockup.h"

#ifdef TEST_KORE_QUERY

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

void kore_parse_config(void) { kore_parse_config_file(NULL); }

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

#endif
