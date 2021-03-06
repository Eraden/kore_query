#if defined(TEST_KORE_QUERY)

#include <stdlib.h>
#include <check.h>
#include "../strings.h"
#include "test_json.h"
#include "test_database_query.h"
#include "test_database_exec.h"
#include "test_database_query_stringify.h"
#include "test_strings.h"
#include "test_serialize.h"

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  kore__main();

  kore_pgsql_register("db", "dbname=kore_query_test");

  Suite *s = suite_create("TEST");

  test_strings(s);
  test_json(s);
  test_serialize(s);
  test_database_query(s);
  test_database_query_stringify(s);
  test_database_exec(s);

  SRunner *sr;
  int number_failed = 0;

  sr = srunner_create(s);
  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed += srunner_ntests_failed(sr);
  srunner_free(sr);

  kore__terminate();

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif