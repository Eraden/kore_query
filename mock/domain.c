#include "../kore_mockup.h"

#ifdef TEST_KORE_QUERY

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

#endif
