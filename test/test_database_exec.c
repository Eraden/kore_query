#include "./test_database_exec.h"

#if defined(TEST_KORE_QUERY)

START_TEST(simple_query_exec)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id");
  DatabaseQuery_select(query, "users", "login", "login");
  DatabaseQuery_select(query, "users", "updated_at", "updated_at");
  DatabaseQuery_select(query, "users", "created_at", "created_at");
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  JSON_free(root);
END_TEST

START_TEST(join_query_exec)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_join(query, "posts", "user_id", "users", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "comments", "post_id", "posts", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_select(query, "users", "id", "id");
  DatabaseQuery_select(query, "users", "login", "login");
  DatabaseQuery_select(query, "users", "updated_at", "updated_at");
  DatabaseQuery_select(query, "users", "created_at", "created_at");
  DatabaseQuery_select(query, "posts", "id", "id");
  DatabaseQuery_select(query, "posts", "title", "title");
  DatabaseQuery_select(query, "posts", "content", "content");
  DatabaseQuery_select(query, "posts", "created_at", "created_at");
  DatabaseQuery_select(query, "comments", "id", "id");
  DatabaseQuery_select(query, "comments", "title", "title");
  DatabaseQuery_select(query, "comments", "content", "content");
  DatabaseQuery_select(query, "comments", "created_at", "created_at");
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  JSON_free(root);
END_TEST

START_TEST(join_query_exec_stringify)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_join(query, "posts", "user_id", "users", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "comments", "post_id", "posts", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_select(query, "users", "id", "id");
  DatabaseQuery_select(query, "users", "login", "login");
  DatabaseQuery_select(query, "users", "updated_at", "updated_at");
  DatabaseQuery_select(query, "users", "created_at", "created_at");
  DatabaseQuery_select(query, "posts", "id", "id");
  DatabaseQuery_select(query, "posts", "title", "title");
  DatabaseQuery_select(query, "posts", "content", "content");
  DatabaseQuery_select(query, "posts", "created_at", "created_at");
  DatabaseQuery_select(query, "comments", "id", "id");
  DatabaseQuery_select(query, "comments", "title", "title");
  DatabaseQuery_select(query, "comments", "content", "content");
  DatabaseQuery_select(query, "comments", "created_at", "created_at");
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  char *json = JSON_stringify(root);
  free(json);
  JSON_free(root);
END_TEST

void test_database_exec(Suite *s) {
  TCase *tc_query = tcase_create("Database query exec");
  tcase_add_test(tc_query, simple_query_exec);
  tcase_add_test(tc_query, join_query_exec);
  tcase_add_test(tc_query, join_query_exec_stringify);
  suite_add_tcase(s, tc_query);
}

#endif
