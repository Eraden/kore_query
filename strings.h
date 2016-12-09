#pragma once

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <wchar.h>
#include <pthread.h>
#include <locale.h>

#define SWAP_CSTR(a, b) \
  a = (char *) (((intptr_t) a) ^ ((intptr_t) b)); \
  b = (char *) (((intptr_t) b) ^ ((intptr_t) a)); \
  a = (char *) (((intptr_t) a) ^ ((intptr_t) b));

wchar_t *cstr2wcstr(const char *cstr);

char *wcstr2cstr(const wchar_t *wcstr);

char *join_cstr(const char *left, const char *right);

char *clone_cstr(const char *str);

wchar_t *clone_wstr(const wchar_t *str);

char *append_char_to_cstr(char *str, char c);

char *append_cstr(char *str, char *part);
