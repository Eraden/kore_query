/**
 * @file database_query.h
 * @author Adrian Eraden Woźniak
 * @date 21.12.2016
 * @brief Creating new query
 * @example
 * @code{.c}
 * DatabaseQuery *query NULL;
 *
 * // insert
 * query = DatabaseQuery_startInsert("posts");
 * DatabaseQuery_insert(query, "title", "O'Connor Memory", JSON_STRING);
 * DatabaseQuery_insert(query, "content", "Nothing special", JSON_STRING);
 *
 * // select
 * query = DatabaseQuery_startSelect("posts");
 * DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
 * DatabaseQuery_select(query, "posts", "title", "title", JSON_STRING);
 * DatabaseQuery_whereField(query, "title", "LIKE", "%hello%", JSON_STRING);
 *
 * // update
 * query = DatabaseQuery_startUpdate("posts");
 * DatabaseQuery_update(query, "title", "O'Connor Memory", JSON_STRING);
 * DatabaseQuery_update(query, "content", "Nothing special", JSON_STRING);
 *
 * // delete
 * query = DatabaseQuery_startDelete("posts");
 * DatabaseQuery_whereField(query, "id", "=", 10, JSON_NUMBER);
 * @endcode
 */

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

/** start allocation definition */
#define DEF_DATABASE_QUERY_ALLOC_START(type) \
type *DatabaseQuery_create##type (void)

/** start allocation definition body */
#define DATABASE_QUERY_ALLOC_START(type) \
DEF_DATABASE_QUERY_ALLOC_START(type) { \
  type *instance = calloc(sizeof(type), 1);


/** end allocation definition body */
#define DATABASE_QUERY_ALLOC_END() \
  return instance;\
}

/** start de-allocation definition */
#define DEF_DATABASE_QUERY_DEALLOC_START(type) \
void DatabaseQuery_free##type (type *instance)

/** start de-allocation definition body */
#define DATABASE_QUERY_DEALLOC_START(type) \
DEF_DATABASE_QUERY_DEALLOC_START(type) {

/** end de-allocation definition */
#define DATABASE_QUERY_DEALLOC_END() \
  free(instance); \
}

/** de-allocation loop for coll */
#define DATABASE_QUERY_DEALLOC_LOOP(type, coll) \
  if (coll) { \
    type **pointer = coll; \
    for (unsigned int index = 0; index < coll##Size; index++) { \
      DatabaseQuery_free##type ( *pointer); \
      pointer += 1; \
    } \
    free(coll); \
  }

/** de-allocation for instance with type */
#define DATABASE_QUERY_DEALLOC_IF_EXISTS(type, instance) \
  if (instance) DatabaseQuery_free##type (instance);

/** define append child to collection */
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
 * @param string sql to sanitize
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

/**
 * Sorting direction
 */
typedef enum eDatabaseQueryOrderDirection {
  /** sort asc */
  DATABASE_QUERY_ORDER_ASC = 1,
  /** sort desc */
  DATABASE_QUERY_ORDER_DESC = 2,
} DatabaseQueryOrderDirection;

/**
 * Type of SQL query
 */
typedef enum eDatabaseQueryType {
  /** query is select */
  DATABASE_QUERY_TYPE_SELECT = 1,
  /** query is insert */
  DATABASE_QUERY_TYPE_INSERT = 2,
  /** query is update */
  DATABASE_QUERY_TYPE_UPDATE = 3,
  /** query is delete */
  DATABASE_QUERY_TYPE_DELETE = 4,
} DatabaseQueryType;

/**
 * Condition type
 */
typedef enum eDatabaseQueryConditionType {
  /** condition data is pure sql */
  DATABASE_QUERY_CONDITION_TYPE_PURE_SQL = 1,
  /** condition data is value */
  DATABASE_QUERY_CONDITION_TYPE_VALUE = 2,
  /** condition data is other field */
  DATABASE_QUERY_CONDITION_TYPE_OTHER_FIELD = 3,
} DatabaseQueryConditionType;

/**
 * Join type
 */
typedef enum eDatabaseQueryJoinType {
  /** join without special rules */
  DATABASE_QUERY_JOIN_TYPE_NORMAL = 0,
  /** inner join */
  DATABASE_QUERY_JOIN_TYPE_INNER,
  /** left inner join */
  DATABASE_QUERY_JOIN_TYPE_LEFT,
  /** left outer join */
  DATABASE_QUERY_JOIN_TYPE_LEFT_OUTER,
  /** right join */
  DATABASE_QUERY_JOIN_TYPE_RIGHT,
  /** right outer join */
  DATABASE_QUERY_JOIN_TYPE_RIGHT_OUTER,
  /** full join */
  DATABASE_QUERY_JOIN_TYPE_FULL,
  /** full outer join */
  DATABASE_QUERY_JOIN_TYPE_FULL_OUTER,
} DatabaseQueryJoinType;

typedef struct sDatabaseQueryTable DatabaseQueryTable;
typedef struct sDatabaseQueryField DatabaseQueryField;
typedef struct sDatabaseQueryLimit DatabaseQueryLimit;
typedef struct sDatabaseQueryCondition DatabaseQueryCondition;
typedef struct sDatabaseQueryJoin DatabaseQueryJoin;
typedef struct sDatabaseQuery DatabaseQuery;

/**
 * Queried table data
 */
typedef struct sDatabaseQueryTable {
  /** table name */
  char *name;
} DatabaseQueryTable;

/**
 * Queried table column data.
 */
typedef struct sDatabaseQueryField {
  /** column name */
  char *name;
  /** used by exec to fetch data */
  char *as;
  /** column table name */
  DatabaseQueryTable *table;
  /** how data will be fetched */
  JSONType jsonType;
} DatabaseQueryField;

/**
 * Query rows limit
 */
typedef struct sDatabaseQueryLimit {
  /** limit value */
  char *limit;
} DatabaseQueryLimit;

/**
 * Condition data
 */
typedef struct sDatabaseQueryCondition {
  /** condition table */
  DatabaseQueryTable *table;
  /** condition column */
  DatabaseQueryField *field;
  union {
    /** condition value */
    char *value;
    /** condition sql */
    char *pure;
    /** condition second column */
    DatabaseQueryField *otherField;
  };
  /** operator */
  char *operator;
  /**
   * condition type
   * @see DatabaseQueryConditionType
   */
  DatabaseQueryConditionType type;
} DatabaseQueryCondition;

/**
 * Distinct on data
 */
typedef struct sDatabaseQueryDistinct {
  /** Distinct on columns */
  DatabaseQueryField **fields;
  /** Number of distinct on columns */
  unsigned int fieldsSize;
} DatabaseQueryDistinct;

/// alias for insert and update
typedef struct sDatabaseQueryCondition DatabaseQueryFieldValue;

/**
 * Join data
 */
typedef struct sDatabaseQueryJoin {
  /** join table */
  DatabaseQueryTable *table;
  /** join `on` conditions */
  DatabaseQueryCondition **conditions;
  /** number of join `on` conditions */
  unsigned int conditionsSize;
  /** join type */
  DatabaseQueryJoinType type;
} DatabaseQueryJoin;

/**
 * Order data
 */
typedef struct sDatabaseQueryOrder {
  /** order columns */
  DatabaseQueryField *field;
  /** order direction */
  DatabaseQueryOrderDirection direction;
  /** pure sql */
  char *pure;
} DatabaseQueryOrder;

/**
 * Full query data
 */
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
 * @param tableName table name
 * @return database query new database query
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("accounts");
 * @endcode
 */
DatabaseQuery *DatabaseQuery_startSelect(char *tableName);

/**
 * Start insert query for table
 * @param tableName table name
 * @return database query new database query
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startInsert("accounts");
 * @endcode
 */
DatabaseQuery *DatabaseQuery_startInsert(char *tableName);

/**
 * Start update query for table
 * @param tableName table name
 * @return database query new database query
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startUpdate("accounts");
 * @endcode
 */
DatabaseQuery *DatabaseQuery_startUpdate(char *tableName);

/**
 * Start delete query for table
 * @param tableName table name
 * @return database query new database query
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startDelete("accounts");
 * @endcode
 */
DatabaseQuery *DatabaseQuery_startDelete(char *tableName);

/**
 * Add where condition to query
 * @param query database query object
 * @param field table column
 * @param operator sql valid operator
 * @param value any valid sql value to compare
 * @param type if string add quotes
 * @return new database query condition
 *
 * @code{.c}
 *  char *password = "secret";
 *  DatabaseQuery *query = DatabaseQuery_startSelect("accounts");
 *  DatabaseQuery_select(query, "accounts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_whereField(query, "pass", "=", password, JSON_STRING);
 * @endcode
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
 *
 * @param query database query object
 * @param field table column
 * @param operator
 * @param value value
 * @param caller database function to call
 * @param type if string then will add quotes
 * @return new database query condition instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("posts");
 *  DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_whereFieldWithCall(query, "title", "=", "Hello", "lower", JSON_STRING);
 *  // posts.title = lower('Hello')
 * @endcode
 */
DatabaseQueryCondition *
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
 *
 * @param query database query object
 * @param pure sql where part eq. "posts.title ILIKE 'hello'"
 * @return
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("posts");
 *  DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_whereSQL(query, "posts.title ILIKE 'hello'");
 * @endcode
 */
DatabaseQueryCondition *
DatabaseQuery_whereSQL(DatabaseQuery *query, char *pure);

/**
 * Add join statement
 *
 * @param query database query object
 * @param joinTableName table to join
 * @param joinFieldName join ON
 * @param queriedTableName table which already was joined or is selected
 * @param queriedFieldName
 * @param type join type
 * @return new database query join instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("accounts");
 *  DatabaseQuery_join(query, "profiles", "account_id", "accounts", "id", DATABASE_QUERY_JOIN_TYPE_INNER);
 * @endcode
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
 *
 * @param query database query object
 * @param value number of rows
 * @return new database query limit instance
 *
 * @note Be careful with joins and limit because it will affect all tables!
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startSelect("posts");
 *  DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_limit(query, "1");
 * @endcode
 */
DatabaseQueryLimit *
DatabaseQuery_limit(DatabaseQuery *query, char *value);

/**
 * Add returning statement
 * @param query database query object
 * @param tableName table
 * @param fieldName column
 * @return new database query returning instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startInsert("posts");
 *  DatabaseQuery_insert(query, "title", "Boom!", JSON_STRING);
 *  DatabaseQuery_returning(query, "posts", "id");
 * @endcode
 */
DatabaseQueryField *
DatabaseQuery_returning(DatabaseQuery *query, char *tableName, char *fieldName);

/**
 * Add column to select
 * @param query database query object
 * @param tableName table
 * @param fieldName column
 * @param as used by parser
 * @return new database query field instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startInsert("posts");
 *  DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_select(query, "posts", "title", "title", JSON_STRING);
 * @endcode
 */
DatabaseQueryField *
DatabaseQuery_select(DatabaseQuery *query, char *tableName, char *fieldName, char *as, JSONType type);

/**
 * Add insert value
 * @param query database query object
 * @param fieldName column name
 * @param value value
 * @param type if string add quotes
 * @return new database query field value instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startInsert("accounts");
 *  DatabaseQuery_insert(query, "login", "Josh", JSON_STRING);
 * @endcode
 */
DatabaseQueryFieldValue *
DatabaseQuery_insert(DatabaseQuery *query, char *fieldName, char *value, JSONType type);

/**
 * Add distinct on statement
 * @param query database query object
 * @param tableName table name
 * @param fieldName column name
 * @return new database query distinct instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_select("accounts");
 *  DatabaseQuery_select(query, "accounts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_distinctOn(query, "accounts", "id");
 * @endcode
 */
DatabaseQueryDistinct *
DatabaseQuery_distinctOn(DatabaseQuery *query, char *tableName, char *fieldName);

/**
 * Add order by statement
 * @param query database query object
 * @param tableName table name
 * @param fieldName column name
 * @param direction sort direction
 * @return new database query order instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_select("accounts");
 *  DatabaseQuery_select(query, "accounts", "id", "id", JSON_NUMBER);
 *  DatabaseQuery_order(query, "accounts", "id", DATABASE_QUERY_ORDER_DESC);
 * @endcode
 */
DatabaseQueryOrder *
DatabaseQuery_order(DatabaseQuery *query, char *tableName, char *fieldName, DatabaseQueryOrderDirection direction);

/**
 * Add update value
 * @param query database query object
 * @param fieldName column name
 * @param value to set
 * @param type if string add quotes
 * @return new database query field value instance
 *
 * @code{.c}
 *  DatabaseQuery *query = DatabaseQuery_startInsert("accounts");
 *  DatabaseQuery_update(query, "login", "Josh", JSON_STRING);
 * @endcode
 */
DatabaseQueryFieldValue *
DatabaseQuery_update(DatabaseQuery *query, char *fieldName, char *value, JSONType type);

/**
 * Check if value can be harmful for database
 *
 * @param value c string to check
 * @return 1 if has invalid characters, otherwise 0
 *
 * @code{.c}
 *  DatabaseQuery_isDirty(";"); // 1
 *  DatabaseQuery_isDirty("'"); // 1
 *  DatabaseQuery_isDirty("\n"); // 0
 * @endcode
 */
char DatabaseQuery_isDirty(const char *value);

