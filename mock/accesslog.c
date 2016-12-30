/*
 * Copyright (c) 2013-2016 Joris Vink <joris@coders.se>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

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