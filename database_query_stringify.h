#pragma once

#include "./database_query.h"

/**
 * Transform query to SQL string
 * @param query
 * @return
 */
char *DatabaseQuery_stringify(DatabaseQuery *query);

/**
 * Deallocate SQL string
 * @param sql
 */
void DatabaseQuery_freeSQL(char *sql);
