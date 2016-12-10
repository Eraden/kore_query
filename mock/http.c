#include "../kore_mockup.h"

#ifdef TEST_KORE_QUERY

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

#endif
