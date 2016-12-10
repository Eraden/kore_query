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

START_TEST(simple_json_stringify)
  char *json = NULL;
  JSON *root = JSON_alloc(JSON_OBJECT);
  json = JSON_stringify(root);
  free(json);
  JSON_free(root);

  JSON *array = JSON_alloc(JSON_ARRAY);
  json = JSON_stringify(array);
  free(json);
  JSON_free(array);

  JSON *string = JSON_string("hello");
  json = JSON_stringify(string);
  free(json);
  JSON_free(string);

  JSON *number = JSON_number(1.0);
  json = JSON_stringify(number);
  free(json);
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

#define JSON_SMALL_MAX 10

START_TEST(json_complex)
  JSON *root = JSON_alloc(JSON_OBJECT);
  for (int i = 0; i < JSON_SMALL_MAX; i++) {
    JSON *array = JSON_alloc(JSON_ARRAY);
    char tmp[20];
    sprintf(tmp, "key-%i", i);
    wchar_t *key = cstr2wcstr(tmp);
    JSON_set(root, key, array);

    for (int j = 0; j < JSON_SMALL_MAX; j++) {
      JSON *entry = JSON_alloc(JSON_OBJECT);
      JSON_append(array, entry);
      for (int k = 0; k < JSON_SMALL_MAX; k++) {
        JSON *e = JSON_alloc(JSON_OBJECT);
        char string[20];
        sprintf(string, "key-%i", k);
        wchar_t *wcstr = cstr2wcstr(string);
        JSON_set(entry, wcstr, e);
        free(wcstr);
      }
    }
    free(key);
  }
  JSON_free(root);
END_TEST

START_TEST(json_complex_stringify)
  JSON *root = JSON_alloc(JSON_OBJECT);
  for (int i = 0; i < JSON_SMALL_MAX; i++) {
    JSON *array = JSON_alloc(JSON_ARRAY);
    char tmp[20];
    sprintf(tmp, "key-%i", i);
    wchar_t *key = cstr2wcstr(tmp);
    JSON_set(root, key, array);

    for (int j = 0; j < JSON_SMALL_MAX; j++) {
      JSON *entry = JSON_alloc(JSON_OBJECT);
      JSON_append(array, entry);
      for (int k = 0; k < JSON_SMALL_MAX; k++) {
        JSON *e = JSON_alloc(JSON_OBJECT);
        char string[20];
        sprintf(string, "key-%i", k);
        wchar_t *wcstr = cstr2wcstr(string);
        JSON_set(entry, wcstr, e);
        free(wcstr);
      }
    }
    free(key);
  }
  char *json = JSON_stringify(root);
  free(json);
  JSON_free(root);
END_TEST

void test_json(Suite *s) {
  TCase *tc_json = tcase_create("JSON");
  tcase_add_test(tc_json, simple_json);
  tcase_add_test(tc_json, simple_json_stringify);
  tcase_add_test(tc_json, json_object);
  tcase_add_test(tc_json, json_array);
  tcase_add_test(tc_json, json_complex);
  tcase_add_test(tc_json, json_complex_stringify);
  suite_add_tcase(s, tc_json);
}

#endif
