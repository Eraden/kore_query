#include "./test_database_query_stringify.h"

#ifdef TEST_KORE_QUERY

START_TEST(complex_select_stringify)
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
  DatabaseQuery_whereField(query, "posts.content", "LIKE", "%home%", JSON_STRING);
  DatabaseQuery_whereField(query, "comments.content", "LIKE", "%home%", JSON_STRING);
  DatabaseQuery_limit(query, "10");
  DatabaseQuery_distinctOn(query, "posts", "id");
  char *sql = DatabaseQuery_stringify(query);
  ck_assert_cstr_contains(sql,
                          " LEFT JOIN posts ON posts.user_id = users.id LEFT JOIN comments ON comments.post_id = posts.id");
  ck_assert_cstr_contains(sql, " WHERE posts.content LIKE '%home%' AND comments.content LIKE '%home%'");
  ck_assert_cstr_contains(sql, " LIMIT 10");
  free(sql);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(insert_stringify)
  DatabaseQuery *query = DatabaseQuery_startInsert("users");
  DatabaseQuery_insert(query, "login", "John' Obein", JSON_STRING);
  char *sql = DatabaseQuery_stringify(query);
  ck_assert_cstr_contains(sql, "INSERT INTO users (login) VALUES ('John'' Obein')");
  free(sql);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(update_stringify)
  DatabaseQuery *query = DatabaseQuery_startUpdate("users");
  DatabaseQuery_update(query, "login", "John' Obein", JSON_STRING);
  char *sql = DatabaseQuery_stringify(query);
  ck_assert_cstr_contains(sql, "UPDATE users SET login = 'John'' Obein'");
  free(sql);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

void test_database_query_stringify(Suite *s) {
  TCase *tc_query = tcase_create("Database query stringify");
  tcase_add_test(tc_query, complex_select_stringify);
  tcase_add_test(tc_query, insert_stringify);
  tcase_add_test(tc_query, update_stringify);
  suite_add_tcase(s, tc_query);
}

#endif
