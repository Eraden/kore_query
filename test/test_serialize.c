#if defined(TEST_KORE_QUERY)

#include "test_serialize.h"

START_TEST(test_kore_serialization_mergePaths)
  JSON *src = JSON_alloc(JSON_OBJECT);
  JSON *array = JSON_set(src, (wchar_t *) L"array", JSON_alloc(JSON_ARRAY));
  JSON_append(array, JSON_number(123));
  JSON_append(array, JSON_string("hello world"));

  JSONPath srcPath[2] = {
      { .type=JSON_STRING, .name="array" },
      { .type=JSON_UNDEFINED, .name=NULL }
  };
  JSONPath destPath[1] = {
      { .type=JSON_UNDEFINED, .name=NULL }
  };

  JSON *dest = JSON_alloc(JSON_ARRAY);
  int res = kore_serialization_mergePaths(dest, src, srcPath, destPath);

  ck_assert_int_eq(res, 1);
  ck_assert_uint_eq(dest->array.len, 2);
  ck_assert_ptr_ne(dest->array.objects, NULL);

  JSON_free(src);
  JSON_free(dest);
END_TEST

START_TEST(test_kore_serialization_scrapeAndMerge)
  JSON *src = JSON_alloc(JSON_OBJECT);
  JSON *array = JSON_set(src, (wchar_t *) L"array", JSON_alloc(JSON_ARRAY));
  JSON_append(array, JSON_number(123));
  JSON_append(array, JSON_string("hello world"));

  JSONPath srcPath[2] = {
      { .type=JSON_STRING, .name="array" },
      { .type=JSON_UNDEFINED, .name=NULL }
  };

  JSON *dest = JSON_alloc(JSON_ARRAY);
  int res = kore_serialization_scrapeAndMerge(dest, src, srcPath, JSON_DEEP);

  ck_assert_int_eq(res, 1);
  ck_assert_uint_eq(dest->array.len, 2);
  ck_assert_ptr_ne(dest->array.objects, NULL);

  JSON_free(src);
  JSON_free(dest);
END_TEST

void test_serialize(Suite *s) {
  TCase *testCaseSerialize = tcase_create("serialize");
  tcase_add_test(testCaseSerialize, test_kore_serialization_mergePaths);
  tcase_add_test(testCaseSerialize, test_kore_serialization_scrapeAndMerge);
  suite_add_tcase(s, testCaseSerialize);
}

#endif
