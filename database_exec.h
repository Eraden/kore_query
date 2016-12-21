#pragma once

#include "./database_query_stringify.h"
#include "./database_query.h"
#include "./json.h"
#include "./strings.h"

#define DB_QUERY_IS_NON_SELECT(query) query->fieldsSize == 0
#define DB_QUERY_HAS_NO_JOINS(query) query->joinsSize == 0
#define DB_QUERY_ONLY_ONE(query) (query->limit == NULL || strcmp(query->limit->limit, "1") == 0)

/**
 * Single join chain
 * @example
 * @code
 *  | accounts | posts | comments |
 */
typedef struct sDatabaseJoinChain {
  char **chain;
  size_t len;
} DatabaseJoinChain;

/**
 * Join chains like
 * @example
 * @code
 *  | accounts | posts | comments |
 *  | accounts | posts | files    |
 */
typedef struct sDatabaseJoinChains {
  DatabaseJoinChain **chains;
  size_t len;
} DatabaseJoinChains;

/**
 * Execute query synchronously (good only for web-sockets)
 * @param query
 * @return Parsed response
 */
JSON *Database_execQuery(DatabaseQuery *query);

/**
 * Execute SQL and parse response
 *
 * @param sql c string database query
 * @param fieldsCount number of fields to parse
 * @param fields names
 * @param argc number of arguments
 * @param argv arguments
 * @return Parsed response
 *
 * @example
 * @code
 *  DatabaseQuery *query = DatabaseQuery_startSelect("users");
 *  DatabaseQuery_select(query, "users", "id", "id", JSON_INTEGER);
 *  JSON *root = Database_execSql(query);
 */
JSON __attribute__((__used__)) *
Database_execSql(
    const char *sql,
    const int fieldsCount,
    const char **fields,
    const int argc,
    const char **argv
);

/**
 * Check if table as any fields declared to select.
 *
 * @param name table name
 * @param fields select or returns
 * @param fieldsSize
 * @return 1 if table has fields to select, otherwise 0
 *
 * @example
 * @code
 *  DatabaseQuery *query = DatabaseQuery_startSelect("users");
 *  DatabaseQuery_select(query, "users", "id", "id", JSON_INTEGER);
 *  Database_hasAnyField("users", query->fields, query->fieldsSize); // 1
 *
 *  DatabaseQuery *query = DatabaseQuery_startInsert("users");
 *  DatabaseQuery_insert(query, "login", "Josh", JSON_STRING);
 *  DatabaseQuery_returning(query, "users", "id");
 *  Database_hasAnyField("users", query->returning, query->returningSize); // 1
 */
unsigned char
Database_hasAnyField(
    const char *name,
    DatabaseQueryField **fields,
    unsigned long fieldsSize
);

