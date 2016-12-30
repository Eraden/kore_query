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

START_TEST(test_valueOf)
  JSON *root = JSON_alloc(JSON_OBJECT);
  JSON_set(root, (wchar_t *) L"id", JSON_number(123.0));
  JSON_set(root, (wchar_t *) L"name", JSON_string("John"));

  JSONValue val;

  val = JSON_valueOf(root, "id");
  ck_assert(val.value == 123.0);
  val = JSON_valueOf(root, "name");
  ck_assert_cstr_contains(val.string, "John");
  val = JSON_valueOf(root, "not-existing");
  ck_assert(val.value == 0);
  JSON_free(root);
END_TEST

START_TEST(test_stringify_null)
  JSON *root = JSON_alloc(JSON_NULL);
  char *json = JSON_stringify(root);
  ck_assert_cstr_contains(json, "null");
  free(json);
  JSON_free(root);
END_TEST

START_TEST(test_stringify_undefined)
  JSON *root = JSON_alloc(JSON_UNDEFINED);
  char *json = JSON_stringify(root);
  ck_assert_ptr_eq(json, NULL);
  JSON_free(root);
END_TEST

START_TEST(test_find_path)
  JSON *root = JSON_alloc(JSON_OBJECT);
  JSON *a = JSON_set(root, (wchar_t *) L"a", JSON_alloc(JSON_OBJECT));
  JSON *b = JSON_set(a, (wchar_t *) L"b", JSON_alloc(JSON_ARRAY));
  JSON *string = JSON_append(b, JSON_string("c"));
  JSONPath path[4] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_STRING, .name="b"},
      {.type=JSON_NUMBER, .name=0},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  JSON *found = JSON_find(root, path);
  ck_assert_ptr_ne(found, NULL);
  ck_assert_ptr_eq(found, string);
  JSON_free(root);
END_TEST

START_TEST(test_find_path_incompatible)
  JSON *root = JSON_alloc(JSON_OBJECT);
  JSON *a = JSON_set(root, (wchar_t *) L"a", JSON_alloc(JSON_OBJECT));
  JSON *b = JSON_set(a, (wchar_t *) L"b", JSON_alloc(JSON_ARRAY));
  JSON *c = JSON_set(a, (wchar_t *) L"c", JSON_alloc(JSON_OBJECT));
  JSON_append(b, JSON_string("c"));
  JSONPath pathToObjectGoesToArray[4] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_STRING, .name="b"},
      {.type=JSON_STRING, .name="missing"},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  JSON *found = NULL;

  found = JSON_find(root, pathToObjectGoesToArray);
  ck_assert_ptr_eq(found, NULL);

  JSONPath pathToArrayGoesToObject[3] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_NUMBER, .index=0},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  found = JSON_find(root, pathToArrayGoesToObject);
  ck_assert_ptr_eq(found, NULL);

  JSONPath arrayPathIndexTooLarge[4] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_STRING, .name="b"},
      {.type=JSON_NUMBER, .index=100},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  found = JSON_find(root, arrayPathIndexTooLarge);
  ck_assert_ptr_eq(found, NULL);

  JSONPath notExistingKeyPath[3] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_STRING, .name="-----"},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  found = JSON_find(root, notExistingKeyPath);
  ck_assert_ptr_eq(found, NULL);

  JSONPath noKeysPath[4] = {
      {.type=JSON_STRING, .name="a"},
      {.type=JSON_STRING, .name="c"},
      {.type=JSON_STRING, .name="d"},
      {.type=JSON_UNDEFINED, .name=NULL}
  };
  found = JSON_find(root, noKeysPath);
  ck_assert_ptr_eq(found, NULL);

  JSON_free(root);
END_TEST

START_TEST(test_escape_json)
  char *buffer = "\"hello\"\nworld";
  char *json = JSON_escape(buffer);
  ck_assert_ptr_ne(json, NULL);
  ck_assert_ptr_ne(json, buffer);
  ck_assert_cstr_contains(json, "\\\"hello\\\"\\nworld");
  free(json);
END_TEST

START_TEST(test_simple_clone_array)
  JSON *array = JSON_alloc(JSON_ARRAY);
  JSON_append(array, JSON_string("hello"));
  JSON_append(array, JSON_number(123));
  JSON_append(array, JSON_alloc(JSON_OBJECT));
  JSON *clone = JSON_clone(array, JSON_SIMPLE);

  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(clone, array);
  ck_assert_ptr_eq(clone->array.objects, NULL);
  ck_assert_uint_eq(clone->array.len, 0);
  JSON_free(clone);
  JSON_free(array);
END_TEST

START_TEST(test_deep_clone_array)
  JSON *array = JSON_alloc(JSON_ARRAY);
  JSON_append(array, JSON_string("hello"));
  JSON_append(array, JSON_number(123));
  JSON_append(array, JSON_alloc(JSON_OBJECT));
  JSON *clone = JSON_clone(array, JSON_DEEP);

  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(clone, array);
  ck_assert_ptr_ne(clone->array.objects, NULL);
  ck_assert_uint_eq(clone->array.len, 3);
  JSON_free(clone);
  JSON_free(array);
END_TEST

START_TEST(test_simple_clone_object)
  JSON *object = JSON_alloc(JSON_OBJECT);
  JSON_set(object, (wchar_t *) L"a", JSON_string("hello"));
  JSON_set(object, (wchar_t *) L"b", JSON_number(123));
  JSON_set(object, (wchar_t *) L"c", JSON_alloc(JSON_OBJECT));
  JSON *clone = JSON_clone(object, JSON_SIMPLE);

  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(clone, object);
  ck_assert_ptr_eq(clone->children.objects, NULL);
  ck_assert_uint_eq(clone->children.len, 0);
  JSON_free(clone);
  JSON_free(object);
END_TEST

START_TEST(test_deep_clone_object)
  JSON *object = JSON_alloc(JSON_OBJECT);
  JSON_set(object, (wchar_t *) L"a", JSON_string("hello"));
  JSON_set(object, (wchar_t *) L"b", JSON_number(123));
  JSON_set(object, (wchar_t *) L"c", JSON_alloc(JSON_OBJECT));
  JSON *clone = JSON_clone(object, JSON_DEEP);

  ck_assert_ptr_ne(clone, NULL);
  ck_assert_ptr_ne(clone, object);
  ck_assert_ptr_ne(clone->children.objects, NULL);
  ck_assert_uint_eq(clone->children.len, 3);
  JSON_free(clone);
  JSON_free(object);
END_TEST

START_TEST(test_renameNode)
  JSON *object = JSON_alloc(JSON_OBJECT);
  JSON_set(object, (wchar_t *) L"a", JSON_string("hello"));
  JSONPath path[2] = {
      { .type=JSON_STRING, .name="a" },
      { .type=JSON_UNDEFINED, .name=NULL }
  };
  JSON_renameNode(object, "b", path);
  ck_assert_ptr_ne(object->children.keys, NULL);
  ck_assert_ptr_ne(*object->children.keys, NULL);
  ck_assert_cstr_contains(*object->children.keys, "b");
  JSON_free(object);
END_TEST

void test_json(Suite *s) {
  TCase *tc_json = tcase_create("JSON");
  tcase_add_test(tc_json, simple_json);
  tcase_add_test(tc_json, simple_json_stringify);
  tcase_add_test(tc_json, json_object);
  tcase_add_test(tc_json, json_array);
  tcase_add_test(tc_json, json_complex);
  tcase_add_test(tc_json, json_complex_stringify);
  tcase_add_test(tc_json, test_valueOf);
  tcase_add_test(tc_json, test_stringify_null);
  tcase_add_test(tc_json, test_stringify_undefined);
  tcase_add_test(tc_json, test_find_path);
  tcase_add_test(tc_json, test_find_path_incompatible);
  tcase_add_test(tc_json, test_escape_json);
  tcase_add_test(tc_json, test_simple_clone_array);
  tcase_add_test(tc_json, test_deep_clone_array);
  tcase_add_test(tc_json, test_simple_clone_object);
  tcase_add_test(tc_json, test_deep_clone_object);
  tcase_add_test(tc_json, test_renameNode);
  suite_add_tcase(s, tc_json);
}

#endif
