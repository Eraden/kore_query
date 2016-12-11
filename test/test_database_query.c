#if defined(TEST_KORE_QUERY)

#include "./test_database_query.h"

START_TEST(empty_select_query)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(empty_insert_query)
  DatabaseQuery *query = DatabaseQuery_startInsert("users");
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(empty_update_query)
  DatabaseQuery *query = DatabaseQuery_startUpdate("users");
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(empty_delete_query)
  DatabaseQuery *query = DatabaseQuery_startDelete("users");
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(simple_select_query)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(join_select_query)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_join(query, "posts", "user_id", "users", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "comments", "post_id", "posts", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "posts", "title", "title", JSON_STRING);
  DatabaseQuery_select(query, "posts", "content", "content", JSON_STRING);
  DatabaseQuery_select(query, "posts", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_select(query, "comments", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "comments", "title", "title", JSON_STRING);
  DatabaseQuery_select(query, "comments", "content", "content", JSON_STRING);
  DatabaseQuery_select(query, "comments", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

void test_database_query(Suite *s) {
  TCase *tc_query = tcase_create("Database query");
  tcase_add_test(tc_query, empty_select_query);
  tcase_add_test(tc_query, empty_insert_query);
  tcase_add_test(tc_query, empty_update_query);
  tcase_add_test(tc_query, empty_delete_query);
  tcase_add_test(tc_query, simple_select_query);
  tcase_add_test(tc_query, join_select_query);
  suite_add_tcase(s, tc_query);
}

#endif