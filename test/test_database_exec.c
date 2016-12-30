#if defined(TEST_KORE_QUERY)

#include "./test_database_exec.h"

static DatabaseJoinChains *
Database_buildJoinChains(
    DatabaseQuery *query,
    DatabaseQueryField **fields,
    unsigned int fieldsSize
) {
  DatabaseJoinChains *joinChains = calloc(sizeof(DatabaseJoinChains), 1);

  DatabaseQueryJoin **joins = calloc(sizeof(DatabaseQueryJoin *), query->joinsSize);
  DatabaseQueryJoin **release = joins;

  memcpy(joins, query->joins, sizeof(DatabaseQueryJoin *) * query->joinsSize);
  unsigned int len = query->joinsSize;

  while (len) {
    DatabaseQueryJoin *join = *joins;
    if (!join) {
      len -= 1;
      continue;
    }
    *joins = NULL;
    DatabaseQueryCondition *condition = *join->conditions;

    DatabaseJoinChain *joinChain = calloc(sizeof(DatabaseJoinChain), 1);
    joinChain->len = 2;
    joinChain->chain = calloc(sizeof(char **), joinChain->len + 1);

    if (joinChains->chains == NULL) {
      joinChains->len = 1;
      joinChains->chains = calloc(sizeof(DatabaseJoinChain **), joinChains->len + 1);
    } else {
      joinChains->len += 1;
      joinChains->chains = realloc(joinChains->chains, sizeof(DatabaseJoinChain **) * (joinChains->len + 1));
      joinChains->chains[joinChains->len] = 0;
    }
    joinChains->chains[joinChains->len - 1] = joinChain;

    char *queriedTable = condition->otherField->table->name;
    char *joinedTable = condition->field->table->name;
    joinChain->chain[0] = queriedTable;
    joinChain->chain[1] = joinedTable;

    unsigned int nextLen = len - 1;

    while (nextLen) {
      if (*(joins + 1) == NULL) {
        nextLen -= 1;
        continue;
      }
      DatabaseQueryJoin *nextJoin = *(joins + 1);
      DatabaseQueryCondition *nextCondition = *nextJoin->conditions;

      char *nextQueriedTable = nextCondition->otherField->table->name;
      char *nextJoinedTable = nextCondition->field->table->name;

      if (strcmp(joinedTable, nextQueriedTable) == 0) {
        joinChain->len += 1;
        joinChain->chain = realloc(joinChain->chain, sizeof(char **) * (joinChain->len + 1));
        joinChain->chain[joinChain->len - 1] = nextJoinedTable;
        joinChain->chain[joinChain->len] = 0;
        *(joins + 1) = NULL;
      }

      nextLen -= 1;
    }

    joins += 1;
    len -= 1;
  }

  // optimize paths
  DatabaseJoinChain **chains = joinChains->chains;
  while (*chains) {
    char **paths = (*chains)->chain;
    while (*paths) {
      if (!Database_hasAnyField(*paths, fields, fieldsSize)) {
        char **write = paths, **read = paths;
        read += 1;
        while (*write) {
          *write = *read;
          if (*(read + 1) == NULL) *read = NULL;
          read += 1;
          write += 1;
        }
      }
      paths += 1;
    }
    chains += 1;
  }


  free(release);

  return joinChains;
}

static void Database_freeChains(DatabaseJoinChains *chains) {
  if (!chains) return;
  DatabaseJoinChain **ptr = chains->chains;
  while (ptr && *ptr) {
    free((*ptr)->chain);
    free(*ptr);
    ptr += 1;
  }
  free(chains->chains);
  free(chains);
}

START_TEST(join_chain)
  DatabaseQuery* query = DatabaseQuery_startSelect("clans");
  DatabaseQuery_join(query, "clan_history", "clan_id", "clans", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "clan_disciplines", "clan_id", "clans", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "disciplines", "id", "clan_disciplines", "discipline_id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_select(query, "clans", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "clans", "name", "name", JSON_STRING);
  DatabaseQuery_select(query, "clans", "overview", "overview", JSON_STRING);
  DatabaseQuery_select(query, "clan_history", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "clan_history", "era", "era", JSON_STRING);
  DatabaseQuery_select(query, "clan_history", "description", "description", JSON_STRING);
  DatabaseQuery_select(query, "disciplines", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "disciplines", "name", "name", JSON_STRING);

  fprintf(stderr, "attempt to build chains...\n");
  DatabaseJoinChains *chains = Database_buildJoinChains(query, query->fields, query->fieldsSize);

  fprintf(stderr, "chains length: %lu\n", chains->len);
  DatabaseJoinChain **chainsPtr = chains->chains;
  while (chainsPtr && *chainsPtr) {
    char **tables = (*chainsPtr)->chain;
    fprintf(stderr, "| ");
    while (tables && *tables) {
      fprintf(stderr, " %-20s |", *tables);
      tables += 1;
    }
    fprintf(stderr, "\n");
    chainsPtr += 1;
  }

  Database_freeChains(chains);

  DatabaseQuery_freeDatabaseQuery(query);
END_TEST

START_TEST(simple_query_exec)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_select(query, "users", "id", "id", JSON_NUMBER);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  JSON_free(root);
END_TEST

START_TEST(join_query_exec)
  DatabaseQuery *query = DatabaseQuery_startSelect("users");
  DatabaseQuery_join(query, "posts", "user_id", "users", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_join(query, "comments", "post_id", "posts", "id", DATABASE_QUERY_JOIN_TYPE_LEFT);
  DatabaseQuery_select(query, "users", "id", "id", JSON_STRING);
  DatabaseQuery_select(query, "users", "login", "login", JSON_STRING);
  DatabaseQuery_select(query, "users", "updated_at", "updated_at", JSON_STRING);
  DatabaseQuery_select(query, "users", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_select(query, "posts", "id", "id", JSON_STRING);
  DatabaseQuery_select(query, "posts", "title", "title", JSON_STRING);
  DatabaseQuery_select(query, "posts", "content", "content", JSON_STRING);
  DatabaseQuery_select(query, "posts", "created_at", "created_at", JSON_STRING);
  DatabaseQuery_select(query, "comments", "id", "id", JSON_STRING);
  DatabaseQuery_select(query, "comments", "title", "title", JSON_STRING);
  DatabaseQuery_select(query, "comments", "content", "content", JSON_STRING);
  DatabaseQuery_select(query, "comments", "created_at", "created_at", JSON_STRING);
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  JSON_free(root);
END_TEST

START_TEST(join_query_exec_stringify)
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
  JSON *root = Database_execQuery(query);
  DatabaseQuery_freeDatabaseQuery(query);
  char *json = JSON_stringify(root);
  free(json);
  JSON_free(root);
END_TEST

void test_database_exec(Suite *s) {
  TCase *tc_query = tcase_create("Database query exec");
  tcase_add_test(tc_query, join_chain);
  tcase_add_test(tc_query, simple_query_exec);
  tcase_add_test(tc_query, join_query_exec);
  tcase_add_test(tc_query, join_query_exec_stringify);
  suite_add_tcase(s, tc_query);
}

#endif
