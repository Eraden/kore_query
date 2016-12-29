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

START_TEST(test_order)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_order(query, "users", "id", DATABASE_QUERY_ORDER_DESC);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(test_where_pure)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_whereSQL(query, "users.login ILIKE '%some%'");
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(test_where_with_call)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_whereFieldWithCall(query, "login", "=", "JOHN", "lower", JSON_STRING);
  DatabaseQuery_whereFieldWithCall(query, "created_at", ">=", "02-02-2016", "date", JSON_UNDEFINED);
  DatabaseQuery_whereFieldWithCall(query, "confirmed_at", ">=", NULL, "date", JSON_UNDEFINED);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(test_where_without_value)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_whereField(query, "login", "NOT", NULL, JSON_UNDEFINED);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(test_returning)
  DatabaseQuery *query = DatabaseQuery_startInsert("users");
  DatabaseQuery_insert(query, "login", "john", JSON_STRING);
  DatabaseQuery_returning(query, "users", "id", JSON_NUMBER);
  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(test_DatabaseQuery_isDirty)
  ck_assert_int_eq(DatabaseQuery_isDirty(""), 0);
  ck_assert_int_eq(DatabaseQuery_isDirty("hello"), 0);
  ck_assert_int_eq(DatabaseQuery_isDirty("hello'"), 1);
END_TEST

START_TEST(test_simple_prepare_sql)
  const char *query = "SELECT * FROM accounts WHERE users.login = $1";
  const int size = 1;
  const char *args[1] = {"john"};
  char *sql = SQL_prepare_sql(query, size, args);
  ck_assert_ptr_ne(sql, NULL);
  ck_assert(strcmp(sql, "SELECT * FROM accounts WHERE users.login = 'john'") == 0);
  free(sql);
END_TEST

START_TEST(test_large_prepare_sql)
  const char *query = "SELECT * "
      "FROM accounts "
      "LEFT JOIN posts ON accounts.id = posts.author_id "
      "LEFT JOIN comments ON comments.post_id = posts.id "
      "WHERE login = $1 AND "
      "accounts.id > $2 AND "
      "accounts.id < $3 AND "
      "(comments.title ILIKE $4 OR posts.title ILIKE $5) AND"
      "comments.rate > $6 AND "
      "posts.visits > $7 AND"
      "posts.content ILIKE $8 AND "
      "comments.visibility = $9 AND"
      "accounts.status != $10 AND"
      "posts.visibility = $11 ";
  const int size = 11;
  const char *args[11] = {"john", "1", "10", "%hello%", "%Yo%", "3", "6", "%high%", "public", "blocked", "blocked"};
  char *sql = SQL_prepare_sql(query, size, args);
  ck_assert_ptr_ne(sql, NULL);
  ck_assert(strcmp(sql, "SELECT * "
      "FROM accounts "
      "LEFT JOIN posts ON accounts.id = posts.author_id "
      "LEFT JOIN comments ON comments.post_id = posts.id "
      "WHERE login = 'john' AND "
      "accounts.id > '1' AND "
      "accounts.id < '10' AND "
      "(comments.title ILIKE '%hello%' OR posts.title ILIKE '%Yo%') AND"
      "comments.rate > '3' AND "
      "posts.visits > '6' AND"
      "posts.content ILIKE '%high%' AND "
      "comments.visibility = 'public' AND"
      "accounts.status != 'blocked' AND"
      "posts.visibility = 'blocked' ") == 0);
  free(sql);
END_TEST

void test_database_query(Suite *s) {
  TCase *tc_query = tcase_create("Database query");
  tcase_add_test(tc_query, empty_select_query);
  tcase_add_test(tc_query, empty_insert_query);
  tcase_add_test(tc_query, empty_update_query);
  tcase_add_test(tc_query, empty_delete_query);
  tcase_add_test(tc_query, simple_select_query);
  tcase_add_test(tc_query, join_select_query);
  tcase_add_test(tc_query, test_order);
  tcase_add_test(tc_query, test_where_pure);
  tcase_add_test(tc_query, test_where_with_call);
  tcase_add_test(tc_query, test_where_without_value);
  tcase_add_test(tc_query, test_returning);
  tcase_add_test(tc_query, test_DatabaseQuery_isDirty);
  tcase_add_test(tc_query, test_simple_prepare_sql);
  tcase_add_test(tc_query, test_large_prepare_sql);
  suite_add_tcase(s, tc_query);
}

#endif