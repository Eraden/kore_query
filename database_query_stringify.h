/**
 * @file database_query_stringify.h
 * @author Adrian Eraden Woźniak
 * @date 21.12.2016
 * @brief Create sql string from database query
 * @example
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("accounts");
 *  DatabaseQuery_select(query, "accounts", "id", "id", JSON_NUMBER);
 *  char *sql = DatabaseQuery_stringify(query);
 * @endcode
 */

#pragma once

#include "./database_query.h"

/**
 * Transform query to SQL string
 * @param query database query to stringify
 * @return sql c string
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("accounts");
 *  DatabaseQuery_select(query, "accounts", "id", "id", JSON_NUMBER);
 *  char *sql = DatabaseQuery_stringify(query);
 *  printf("%s", sql); // => SELECT accounts.id AS id FROM accounts;
 * @endcode
 */
char *DatabaseQuery_stringify(DatabaseQuery *query);

/**
 * Deallocate SQL string
 * @param sql c string to free, it's totally ok to use free instead
 */
void DatabaseQuery_freeSQL(char *sql);
