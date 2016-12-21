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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SWAP_CSTR(a, b) \
  a = (char *) (((intptr_t) a) ^ ((intptr_t) b)); \
  b = (char *) (((intptr_t) b) ^ ((intptr_t) a)); \
  a = (char *) (((intptr_t) a) ^ ((intptr_t) b));

/**
 * Create `wchar_t *` from `char *`
 * @param cstr
 * @return
 */
wchar_t *cstr2wcstr(const char *cstr);

/**
 * Create `char *` from `wchar_t *`
 * @param wcstr
 * @return
 */
char *wcstr2cstr(const wchar_t *wcstr);

/**
 * Create new string from two
 * @param left
 * @param right
 * @return
 */
char *join_cstr(const char *left, const char *right);

/**
 * Clone string
 * @param str
 * @return
 */
char *clone_cstr(const char *str);

/**
 * Clone `wchar_t *`
 * @param str
 * @return
 */
wchar_t *clone_wstr(const wchar_t *str);

/**
 * Append char to existing string and return pointer (memory block can be moved).
 * No free is required.
 * @example
 * @code
 *  char *hello = "hell";
 *  hello = append_char_to_cstr(hello, 'o');
 *
 * @param str
 * @param c
 * @return
 */
char *append_char_to_cstr(char *str, char c);

/**
 * Concatenate two strings and return pointer (memory block can be moved).
 * No free is required.
 * @example
 * @code
 *  char *helloWorld = "hello";
 *  helloWorld = append_cstr(helloWorld, " world");
 *
 * @param str
 * @param part
 * @return
 */
char *append_cstr(char *str, char *part);
