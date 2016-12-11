#pragma once

#include "./database_query_stringify.h"
#include "./database_query.h"
#include "./json.h"
#include "./strings.h"

#define DB_QUERY_IS_NON_SELECT(query) query->fieldsSize == 0
#define DB_QUERY_HAS_NO_JOINS(query) query->joinsSize == 0
#define DB_QUERY_ONLY_ONE(query) (query->limit == NULL || strcmp(query->limit->limit, "1") == 0)

/**
 * Execute query synchronously (good only for web-sockets)
 * @param query
 * @return Parsed response
 */
JSON *Database_execQuery(DatabaseQuery *query);

/**
 * Execute SQL and parse response
 * @param sql c string database query
 * @param fieldsCount number of fields to parse
 * @param fields names
 * @param argc number of arguments
 * @param argv arguments
 * @return Parsed response
 */
JSON __attribute__((__used__)) *
Database_execSql(
    const char *sql,
    const int fieldsCount,
    const char **fields,
    const int argc,
    const char **argv
);
