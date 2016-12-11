#pragma once

#include <kore/kore.h>
#include <kore/http.h>
#include <kore/pgsql.h>

#include <yajl/yajl_tree.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <wchar.h>
#include <pthread.h>
#include <locale.h>

#include "strings.h"
#include "json.h"

#define DEF_DATABASE_QUERY_ALLOC_START(type) \
type *DatabaseQuery_create##type (void)

#define DATABASE_QUERY_ALLOC_START(type) \
DEF_DATABASE_QUERY_ALLOC_START(type) { \
  type *instance = calloc(sizeof(type), 1);

#define DATABASE_QUERY_ALLOC_END() \
  return instance;\
}

#define DEF_DATABASE_QUERY_DEALLOC_START(type) \
void DatabaseQuery_free##type (type *instance)

#define DATABASE_QUERY_DEALLOC_START(type) \
DEF_DATABASE_QUERY_DEALLOC_START(type) {

#define DATABASE_QUERY_DEALLOC_END() \
  free(instance); \
}

#define DATABASE_QUERY_DEALLOC_LOOP(type, coll) \
  if (coll) { \
    type **pointer = coll; \
    for (unsigned int index = 0; index < coll##Size; index++) { \
      DatabaseQuery_free##type ( *pointer); \
      pointer += 1; \
    } \
    free(coll); \
  }
#define DATABASE_QUERY_DEALLOC_IF_EXISTS(type, instance) \
  if (instance) DatabaseQuery_free##type (instance);

#define DATABASE_QUERY_APPEND(ownerType, childrenType, collectionField) \
static void DatabaseQuery_append_##childrenType##_to_##ownerType##_##collectionField( ownerType *owner, childrenType *child ) { \
  unsigned int oldSize = owner-> collectionField ## Size; \
  unsigned int newSize = oldSize + 1; \
  childrenType **newPointer = calloc(sizeof( childrenType *), newSize ); \
  if ( owner-> collectionField ) {\
    memcpy(newPointer, owner-> collectionField, sizeof( childrenType *) * oldSize ); \
    free( owner-> collectionField ); \
  } \
  newPointer[ oldSize ]  = child; \
  owner-> collectionField = newPointer; \
  owner-> collectionField ## Size = newSize;\
}

/**
 * Escape quotes
 * @param string
 * @return String with escaped quotes
 */
char *SQL_escape_string(const char *string);

/**
 * Prepare SQL string by injecting arguments
 * @param query SQL query
 * @param argsSize number of arguments
 * @param args arguments
 * @return Prepared SQL string
 */
char *SQL_prepare_sql(const char *query, const int argsSize, const char **args);

typedef enum eDatabaseQueryOrderDirection {
  DATABASE_QUERY_ORDER_ASC = 1,
  DATABASE_QUERY_ORDER_DESC = 2,
} DatabaseQueryOrderDirection;

typedef enum eDatabaseQueryType {
  DATABASE_QUERY_TYPE_SELECT = 1,
  DATABASE_QUERY_TYPE_INSERT = 2,
  DATABASE_QUERY_TYPE_UPDATE = 3,
  DATABASE_QUERY_TYPE_DELETE = 4,
} DatabaseQueryType;

typedef enum eDatabaseQueryConditionType {
  DATABASE_QUERY_CONDITION_TYPE_PURE_SQL = 1,
  DATABASE_QUERY_CONDITION_TYPE_VALUE = 2,
  DATABASE_QUERY_CONDITION_TYPE_OTHER_FIELD = 3,
} DatabaseQueryConditionType;

typedef enum eDatabaseQueryJoinType {
  DATABASE_QUERY_JOIN_TYPE_NORMAL = 0,
  DATABASE_QUERY_JOIN_TYPE_INNER,
  DATABASE_QUERY_JOIN_TYPE_LEFT,
  DATABASE_QUERY_JOIN_TYPE_LEFT_OUTER,
  DATABASE_QUERY_JOIN_TYPE_RIGHT,
  DATABASE_QUERY_JOIN_TYPE_RIGHT_OUTER,
  DATABASE_QUERY_JOIN_TYPE_FULL,
  DATABASE_QUERY_JOIN_TYPE_FULL_OUTER,
} DatabaseQueryJoinType;

typedef struct sDatabaseQueryTable DatabaseQueryTable;
typedef struct sDatabaseQueryField DatabaseQueryField;
typedef struct sDatabaseQueryLimit DatabaseQueryLimit;
typedef struct sDatabaseQueryCondition DatabaseQueryCondition;
typedef struct sDatabaseQueryJoin DatabaseQueryJoin;
typedef struct sDatabaseQuery DatabaseQuery;

typedef struct sDatabaseQueryTable {
  char *name;
} DatabaseQueryTable;

typedef struct sDatabaseQueryField {
  char *name;
  char *as;
  DatabaseQueryTable *table;
  JSONType jsonType;
} DatabaseQueryField;

typedef struct sDatabaseQueryLimit {
  char *limit;
} DatabaseQueryLimit;

typedef struct sDatabaseQueryCondition {
  DatabaseQueryTable *table;
  DatabaseQueryField *field;
  union {
    char *value;
    char *pure;
    DatabaseQueryField *otherField;
  };
  char *operator;
  DatabaseQueryConditionType type;
} DatabaseQueryCondition;

typedef struct sDatabaseQueryDistinct {
  DatabaseQueryField **fields;
  unsigned int fieldsSize;
} DatabaseQueryDistinct;

/// alias for insert and update
typedef struct sDatabaseQueryCondition DatabaseQueryFieldValue;

typedef struct sDatabaseQueryJoin {
  DatabaseQueryTable *table;
  DatabaseQueryCondition **conditions;
  unsigned int conditionsSize;
  DatabaseQueryJoinType type;
} DatabaseQueryJoin;

typedef struct sDatabaseQueryOrder {
  DatabaseQueryField *field;
  DatabaseQueryOrderDirection direction;
  char *pure;
} DatabaseQueryOrder;

typedef struct sDatabaseQuery {
  /**
   * Main table
   */
  DatabaseQueryTable *table;
  /**
   * Distinct statement
   */
  DatabaseQueryDistinct *distinct;
  /**
   * Select fields
   */
  DatabaseQueryField **fields;
  /**
   * Number of fields to select
   */
  unsigned int fieldsSize;
  /**
   * Fields to update/insert
   */
  DatabaseQueryFieldValue **fieldValues;
  /**
   * Number fields to update/insert
   */
  unsigned int fieldValuesSize;
  /**
   * Conditions
   */
  DatabaseQueryCondition **conditions;
  /**
   * Number of conditions
   */
  unsigned int conditionsSize;
  /**
   * Table to join
   */
  DatabaseQueryJoin **joins;
  /**
   * Number of table to join
   */
  unsigned int joinsSize;
  /**
   * Columns to return after insert
   */
  DatabaseQueryField **returning;
  /**
   * Number of columns to return after insert
   */
  unsigned int returningSize;
  /**
   * Order statements
   */
  DatabaseQueryOrder **orders;
  /**
   * Number of order statements
   */
  unsigned int ordersSize;
  /**
   * Limit statements
   */
  DatabaseQueryLimit *limit;
  /**
   * Type of query
   */
  DatabaseQueryType type;
} DatabaseQuery;

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQuery);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQuery);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryJoin);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryJoin);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryCondition);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryCondition);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryFieldValue);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryFieldValue);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryLimit);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryLimit);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryField);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryField);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryDistinct);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryDistinct);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryOrder);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryOrder);

DEF_DATABASE_QUERY_ALLOC_START(DatabaseQueryTable);

DEF_DATABASE_QUERY_DEALLOC_START(DatabaseQueryTable);

/**
 * Start query to select for table
 * @param tableName
 * @return database query
 */
DatabaseQuery *DatabaseQuery_startSelect(char *tableName);

/**
 * Start insert query for table
 * @param tableName
 * @return database query
 */
DatabaseQuery *DatabaseQuery_startInsert(char *tableName);

/**
 * Start update query for table
 * @param tableName
 * @return database query
 */
DatabaseQuery *DatabaseQuery_startUpdate(char *tableName);

/**
 * Start delete query for table
 * @param tableName
 * @return database query
 */
DatabaseQuery *DatabaseQuery_startDelete(char *tableName);

/**
 * Add where condition to query
 * @param query
 * @param field table column
 * @param operator
 * @param value
 * @param type if string add quotes
 * @return
 */
DatabaseQueryCondition *
DatabaseQuery_whereField(
    DatabaseQuery *query,
    const char *field,
    const char *operator,
    const char *value,
    JSONType type
);

/**
 * Add where condition to query.
 * Example result:
 *  posts.title = lower('Hello');
 * @param query
 * @param field table column
 * @param operator
 * @param value value
 * @param caller database function to call
 * @param type if string then will add quotes
 * @return
 */
DatabaseQueryCondition __attribute__((__used__)) *
DatabaseQuery_whereFieldWithCall(
    DatabaseQuery *query,
    const char *field,
    const char *operator,
    const char *value,
    const char *caller,
    JSONType type
);

/**
 * Add sql where condition
 * @param query
 * @param pure sql where part eq. "posts.title ILIKE 'hello'"
 * @return
 */
DatabaseQueryCondition __attribute__((__used__)) *
DatabaseQuery_whereSQL(DatabaseQuery *query, char *pure);

/**
 * Add join statement
 * @param query
 * @param joinTableName table to join
 * @param joinFieldName join ON
 * @param queriedTableName table which already was joined or is selected
 * @param queriedFieldName
 * @param type join type
 * @return
 */
DatabaseQueryJoin *
DatabaseQuery_join(
    DatabaseQuery *query,
    char *joinTableName,
    char *joinFieldName,
    char *queriedTableName,
    char *queriedFieldName,
    DatabaseQueryJoinType type
);

/**
 * Add limit statement
 * @param query
 * @param value number of rows
 * @return
 */
DatabaseQueryLimit __attribute__((__used__)) *
DatabaseQuery_limit(DatabaseQuery *query, char *value);

/**
 * Add returning statement
 * @param query
 * @param tableName table
 * @param fieldName column
 * @return
 */
DatabaseQueryField __attribute__((__used__)) *
DatabaseQuery_returning(DatabaseQuery *query, char *tableName, char *fieldName);

/**
 * Add column to select
 * @param query
 * @param tableName table
 * @param fieldName column
 * @param as used by parser
 * @return
 */
DatabaseQueryField __attribute__((__used__)) *
DatabaseQuery_select(DatabaseQuery *query, char *tableName, char *fieldName, char *as, JSONType type);

/**
 * Add insert value
 * @param query
 * @param fieldName column name
 * @param value value
 * @param type if string add quotes
 * @return
 */
DatabaseQueryFieldValue __attribute__((__used__)) *
DatabaseQuery_insert(DatabaseQuery *query, char *fieldName, char *value, JSONType type);

/**
 * Add distinct on statement
 * @param query
 * @param tableName
 * @param fieldName
 * @return
 */
DatabaseQueryDistinct __attribute__((__used__)) *
DatabaseQuery_distinctOn(DatabaseQuery *query, char *tableName, char *fieldName);

/**
 * Add order by statement
 * @param query
 * @param tableName table name
 * @param fieldName column
 * @param direction sort direction
 * @return
 */
DatabaseQueryOrder __attribute__((__used__)) *
DatabaseQuery_order(DatabaseQuery *query, char *tableName, char *fieldName, DatabaseQueryOrderDirection direction);

/**
 * Add update value
 * @param query
 * @param fieldName column name
 * @param value to set
 * @param type if string add quotes
 * @return
 */
DatabaseQueryFieldValue __attribute__((__used__)) *
DatabaseQuery_update(DatabaseQuery *query, char *fieldName, char *value, JSONType type);

/**
 * Check if value can be harmful for database
 * @param value
 * @return
 */
char DatabaseQuery_isDirty(const char *value);

