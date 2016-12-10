#if defined(TEST_KORE_QUERY)

#include "test_json.h"

START_TEST(simple_json)
  JSON *root = JSON_alloc(JSON_OBJECT);
  JSON_free(root);
  JSON *array = JSON_alloc(JSON_ARRAY);
  JSON_free(array);
  JSON *string = JSON_string("hello");
  JSON_free(string);
  JSON *number = JSON_number(1.0);
  JSON_free(number);
END_TEST

START_TEST(json_object)
  JSON *root = JSON_alloc(JSON_OBJECT);
  for (int i = 0; i < 10000; i++) {
    JSON *child = JSON_alloc(JSON_OBJECT);
    char tmp[20];
    sprintf(tmp, "key-%i", i);
    wchar_t *key = cstr2wcstr(tmp);
    JSON_set(root, key, child);
    free(key);
  }
  JSON_free(root);
END_TEST

START_TEST(json_array)
  JSON *root = JSON_alloc(JSON_ARRAY);
  for (int i = 0; i < 10000; i++) {
    JSON *child = JSON_alloc(JSON_OBJECT);
    JSON_append(root, child);
  }
  JSON_free(root);
END_TEST

void test_json(Suite *s) {
  TCase *tc_json = tcase_create("JSON");
  tcase_add_test(tc_json, simple_json);
  tcase_add_test(tc_json, json_object);
  tcase_add_test(tc_json, json_array);
  suite_add_tcase(s, tc_json);
}

#endif
