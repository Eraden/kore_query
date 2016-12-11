#pragma once

#ifdef TEST_KORE_QUERY

#include <string.h>
#include <stdlib.h>

#define ck_assert_cstr_contains(X, Y) do { \
  const char* _ck_x = (X); \
  const char* _ck_y = (Y); \
  ck_assert_msg(strstr(_ck_x, _ck_y) != NULL, "Assertion '%s' failed:  \n%s == '%s',\n  '%s' == '%s'", #X" contains "#Y, #X, _ck_x, #Y, _ck_y); \
} while (0)

#endif
