#if defined(TEST_KORE_QUERY)

#include <stdlib.h>
#include <check.h>
#include "../strings.h"
#include "test_json.h"
#include "test_database_query.h"

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  Suite *s = suite_create("TEST");

  test_json(s);
  test_database_query(s);

  SRunner *sr;
  int number_failed = 0;

  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed += srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif