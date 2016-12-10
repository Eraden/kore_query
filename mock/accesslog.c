#include "../kore_mockup.h"

#ifdef TEST_KORE_QUERY

struct kore_log_packet {
  u_int8_t method;
  int status;
  u_int16_t time_req;
  u_int16_t worker_id;
  u_int16_t worker_cpu;
  u_int8_t addrtype;
  u_int8_t addr[sizeof(struct in6_addr)];
  char host[KORE_DOMAINNAME_LEN];
  char path[HTTP_URI_LEN];
  char agent[HTTP_USERAGENT_LEN];
};

void kore_accesslog_init(void) {}

void kore_accesslog_worker_init(void) {}

int kore_accesslog_write(const void *data, u_int32_t len) { return (KORE_RESULT_OK); }

void kore_accesslog(struct http_request *req) {}

#endif