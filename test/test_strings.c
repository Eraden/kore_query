#if defined(TEST_KORE_QUERY)

#include "test_strings.h"

START_TEST(test_cstr2wcstr)
  wchar_t *w = NULL;
  w = cstr2wcstr("example text");
  ck_assert_ptr_ne(w, NULL);
  ck_assert(wcscmp(w, L"example text") == 0);
  free(w);
END_TEST

START_TEST(test_wcstr2cstr)
  char *w = NULL;
  w = wcstr2cstr((const wchar_t *) L"example w string");
  ck_assert_ptr_ne(w, NULL);
  ck_assert(strcmp(w, "example w string") == 0);
  free(w);
END_TEST

START_TEST(test_join_cstr)
  char *joined = join_cstr("hello", " world");
  ck_assert_ptr_ne(joined, NULL);
  ck_assert(strcmp(joined, "hello world") == 0);
  free(joined);
END_TEST

START_TEST(test_clone_cstr)
  char *original = "hello";
  char *clone = clone_cstr(original);
  ck_assert_ptr_ne(original, NULL);
  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(original, clone);
  ck_assert(strcmp(original, clone) == 0);
  free(clone);
END_TEST

START_TEST(test_clone_wstr)
  wchar_t *original = (wchar_t *) L"hello";
  wchar_t *clone = clone_wstr(original);
  ck_assert_ptr_ne(original, NULL);
  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(original, clone);
  ck_assert(wcscmp(original, clone) == 0);
  free(clone);
END_TEST

START_TEST(test_append_char_to_cstr)
  char *src = "hello";
  char *dest = append_char_to_cstr(src, '!');
  ck_assert_ptr_ne(src, NULL);
  ck_assert_ptr_ne(dest, NULL);
  ck_assert_ptr_ne(src, dest);
  ck_assert(strcmp(dest, "hello!") == 0);
  free(dest);
END_TEST

START_TEST(test_append_cstr)
  char *src = NULL;

  /* Success */
  src = calloc(sizeof(char), strlen("hello") + 1);
  strcat(src, "hello");
  src = append_cstr(src, " my friend!");
  ck_assert_ptr_ne(src, NULL);
  ck_assert(strcmp(src, "hello my friend!") == 0);
  free(src);

  /* Nothing to add */
  src = calloc(sizeof(char), strlen("hello") + 1);
  strcat(src, "hello");
  src = append_cstr(src, NULL);
  ck_assert_ptr_ne(src, NULL);
  ck_assert(strcmp(src, "hello") == 0);
  free(src);

  /* Nothing to add */
  src = calloc(sizeof(char), strlen("hello") + 1);
  strcat(src, "hello");
  char *clone = append_cstr(NULL, src);
  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(clone, src);
  ck_assert(strcmp(clone, "hello") == 0);
  free(src);
  free(clone);

  /* Nothing to do */
  ck_assert_ptr_eq(append_cstr(NULL, NULL), NULL);
END_TEST

void test_strings(Suite *s) {
  TCase *testCaseStrings = tcase_create("strings");
  tcase_add_test(testCaseStrings, test_cstr2wcstr);
  tcase_add_test(testCaseStrings, test_wcstr2cstr);
  tcase_add_test(testCaseStrings, test_join_cstr);
  tcase_add_test(testCaseStrings, test_clone_cstr);
  tcase_add_test(testCaseStrings, test_clone_wstr);
  tcase_add_test(testCaseStrings, test_append_char_to_cstr);
  tcase_add_test(testCaseStrings, test_append_cstr);
  suite_add_tcase(s, testCaseStrings);
}

#endif
