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

/**
 * @file strings.h
 * @author Adrian Eraden Woźniak
 * @date 21.12.2016
 * @brief String manipulations
 * @example
 * @code
 *  char *buffer = NULL;
 *  buffer = clone_cstr("Some: ");
 *  buffer = append_cstr(buffer, "\\n");
 * @endcode
 */

/**
 * Create `wchar_t *` from `char *`
 * @param cstr c string to transform
 * @return wchar_t string
 */
wchar_t *cstr2wcstr(const char *cstr);

/**
 * Create `char *` from `wchar_t *`
 * @param wcstr wchar_t string to transform
 * @return c string
 */
char *wcstr2cstr(const wchar_t *wcstr);

/**
 * Create new string from two
 * @param left string which will be concatenated
 * @param right string which will be concatenated
 * @return new c string
 *
 * @code
 *  char *buffer = join_cstr("foo", "bar");
 *  free(buffer); // safety
 * @endcode
 */
char *join_cstr(const char *left, const char *right);

/**
 * Clone string
 * @param str c string to clone
 * @return new c string
 *
 * @code
 *  char *buffer = clone_cstr("hello");
 *  free(buffer);
 * @endcode
 */
char *clone_cstr(const char *str);

/**
 * Clone `wchar_t *`
 * @param str wchar_t string to clone
 * @return new wchar_t string
 *
 * @code
 *  wchar_t *buffer = clone_wstr(L"hello");
 *  free(buffer);
 * @endcode
 */
wchar_t *clone_wstr(const wchar_t *str);

/**
 * Append char to existing string and return pointer (memory block can be moved).
 * No free is required.
 *
 * @param str source c string
 * @param c char to add
 * @return new c string
 *
 * @code
 *  char *hello = "hell";
 *  hello = append_char_to_cstr(hello, 'o');
 *  free(hello);
 * @endcode
 */
char *append_char_to_cstr(char *str, char c);

/**
 * Concatenate two strings and return pointer (memory block can be moved).
 * No free is required.
 *
 * @param str c string which will change
 * @param part c string to concatenate
 * @return pointer to memory with string (can be same as given)
 *
 * @code
 *  char *helloWorld = "hello";
 *  helloWorld = append_cstr(helloWorld, " world");
 * @endcode
 */
char *append_cstr(char *str, char *part);
