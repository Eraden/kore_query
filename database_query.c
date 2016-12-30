#include "database_query.h"

char *
SQL_escape_string(const char *string) {
  if (string == NULL) return NULL;

  char *sql = NULL;
  u_long size = 0;
  for (u_long i = 0, len = strlen(string); i < len; i++) {
    char c = string[i];
    switch (c) {
      case '\\':
      case '\'': {
        size += 2;
        char *buff = calloc(sizeof(char), size + 1);
        if (sql) {
          strcpy(buff, sql);
          free(sql);

        }
        buff[size - 2] = c;
        buff[size - 1] = c;
        buff[size] = 0;
        sql = buff;
        break;

      }
      default: {
        size += 1;
        char *buff = calloc(sizeof(char), size + 1);
        if (sql) {
          strcpy(buff, sql);
          free(sql);

        }
        buff[size - 1] = c;
        buff[size] = 0;
        sql = buff;
        break;

      }

    }

  }
  if (sql == NULL) return NULL;

  char *buff = calloc(sizeof(char), size + 1);
  strcpy(buff, sql);
  free(sql);
  sql = buff;
  return sql;
}

static void
SQL_fetchArgIndex(const char *query, u_long len, u_long *i, char **indexStr) {
  (*indexStr) = NULL;
  u_long indexStrLen = 0;
  volatile char lookupIndexValue = 1;
  while ((*i) < len && lookupIndexValue) {
    if ((*i) >= len) break;
    char n = query[(*i) + 1];
    switch (n) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        (*i) += 1;
        char *b = calloc(sizeof(char), indexStrLen + 1 + 1);
        if (*indexStr) {
          strcpy(b, (*indexStr));
          free((*indexStr));
        }
        b[indexStrLen] = n;
        (*indexStr) = b;
        indexStrLen += 1;
        break;

      }
      default: {
        lookupIndexValue = 0;
        break;
      }
    }
  }
}

char *
SQL_prepare_sql(const char *query, const int argsSize, const char **args) {
  char *sql = NULL;
  u_long size = 0;
  for (u_long i = 0, len = strlen(query); i < len; i++) {
    char c = query[i];
    switch (c) {
      case '$': {
        char *indexStr;
        SQL_fetchArgIndex(query, len, &i, &indexStr);

        int index = atoi(indexStr) - 1;
        free(indexStr);
        if (index > -1 && index < argsSize) {
          char *escaped = SQL_escape_string(args[index]);
          if (!escaped) break;
          u_long escapedLen = strlen(escaped);
          char *b = calloc(sizeof(char), size + 2 + escapedLen + 1);
          if (sql) {
            strcpy(b, sql);
            free(sql);
          }
          strcat(b, "'");
          strcat(b, escaped);
          strcat(b, "'");
          size += escapedLen + 2;
          sql = b;
          free(escaped);
        }
        break;
      }
      default: {
        size += 1;
        char *buff = calloc(sizeof(char), size + 1);
        if (sql) {
          strcpy(buff, sql);
          free(sql);
        }
        buff[size - 1] = c;
        buff[size] = 0;
        sql = buff;
        break;
      }
    }
  }
  return sql;
}

/// DatabaseQueryTable
DATABASE_QUERY_ALLOC_START(DatabaseQueryTable)
  instance->name = NULL;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryTable)
  if (instance->name) free(instance->name);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryField
DATABASE_QUERY_ALLOC_START(DatabaseQueryField)
  instance->name = NULL;
  instance->as = NULL;
  instance->table = NULL;
  instance->jsonType = JSON_STRING;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryField)
  if (instance->name) free(instance->name);
  if (instance->as) free(instance->as);
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryTable, instance->table);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryLimit
DATABASE_QUERY_ALLOC_START(DatabaseQueryLimit)
  instance->limit = NULL;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryLimit)
  if (instance->limit) free((void *) instance->limit);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryCondition
DATABASE_QUERY_ALLOC_START(DatabaseQueryCondition)
  instance->table = NULL;
  instance->field = NULL;
  instance->value = NULL;
  instance->operator = NULL;
  instance->type = DATABASE_QUERY_CONDITION_TYPE_VALUE;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryCondition)
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryTable, instance->table);
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryField, instance->field);
  switch (instance->type) {
    case DATABASE_QUERY_CONDITION_TYPE_PURE_SQL:
      if (instance->pure) free(instance->pure);
      break;
    case DATABASE_QUERY_CONDITION_TYPE_VALUE:
      if (instance->value) free(instance->value);
      break;
    case DATABASE_QUERY_CONDITION_TYPE_OTHER_FIELD:
      if (instance->otherField) DatabaseQuery_freeDatabaseQueryField(instance->otherField);
      break;
  }

  if (instance->operator) free(instance->operator);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryFieldValue
DATABASE_QUERY_ALLOC_START(DatabaseQueryFieldValue)
  instance->table = NULL;
  instance->field = NULL;
  instance->value = NULL;
  instance->operator = NULL;
  instance->type = DATABASE_QUERY_CONDITION_TYPE_VALUE;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryFieldValue)
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryTable, instance->table);
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryField, instance->field);
  if (instance->value) free(instance->value);
  if (instance->operator) free(instance->operator);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryJoin
DATABASE_QUERY_ALLOC_START(DatabaseQueryJoin)
  instance->table = NULL;
  instance->conditions = NULL;
  instance->conditionsSize = 0;
  instance->type = DATABASE_QUERY_JOIN_TYPE_NORMAL;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryJoin)
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryTable, instance->table);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryCondition, instance->conditions);
DATABASE_QUERY_DEALLOC_END()


/// DatabaseQueryDistinct
DATABASE_QUERY_ALLOC_START(DatabaseQueryDistinct)
  instance->fields = NULL;
  instance->fieldsSize = 0;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryDistinct)
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryField, instance->fields);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQueryOrder
DATABASE_QUERY_ALLOC_START(DatabaseQueryOrder)
  instance->field = NULL;
  instance->direction = DATABASE_QUERY_ORDER_DESC;
  instance->pure = NULL;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQueryOrder)
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryField, instance->field);
  if (instance->pure) free(instance->pure);
DATABASE_QUERY_DEALLOC_END()

/// DatabaseQuery
DATABASE_QUERY_ALLOC_START(DatabaseQuery)
  instance->table = NULL;
  instance->distinct = NULL;
  instance->fields = NULL;
  instance->fieldsSize = 0;
  instance->conditions = NULL;
  instance->conditionsSize = 0;
  instance->fieldValues = NULL;
  instance->fieldValuesSize = 0;
  instance->joins = NULL;
  instance->joinsSize = 0;
  instance->returning = NULL;
  instance->returningSize = 0;
  instance->orders = NULL;
  instance->ordersSize = 0;
  instance->limit = NULL;
  instance->type = DATABASE_QUERY_TYPE_SELECT;
DATABASE_QUERY_ALLOC_END()

DATABASE_QUERY_DEALLOC_START(DatabaseQuery)
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryTable, instance->table);
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryDistinct, instance->distinct);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryField, instance->fields);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryFieldValue, instance->fieldValues);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryCondition, instance->conditions);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryJoin, instance->joins);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryField, instance->returning);
  DATABASE_QUERY_DEALLOC_LOOP(DatabaseQueryOrder, instance->orders);
  DATABASE_QUERY_DEALLOC_IF_EXISTS(DatabaseQueryLimit, instance->limit);
DATABASE_QUERY_DEALLOC_END()

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryField, fields);

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryField, returning);

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryFieldValue, fieldValues);

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryCondition, conditions);

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryJoin, joins);

DATABASE_QUERY_APPEND(DatabaseQueryJoin, DatabaseQueryCondition, conditions);

DATABASE_QUERY_APPEND(DatabaseQueryDistinct, DatabaseQueryField, fields);

DATABASE_QUERY_APPEND(DatabaseQuery, DatabaseQueryOrder, orders);

static DatabaseQuery *
DatabaseQuery_start(const char *tableName, DatabaseQueryType type) {
  DatabaseQuery *q = DatabaseQuery_createDatabaseQuery();
  q->table = DatabaseQuery_createDatabaseQueryTable();
  q->table->name = clone_cstr(tableName);
  q->type = type;
  return q;
}

DatabaseQuery *DatabaseQuery_startSelect(const char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_SELECT);
}

DatabaseQuery *DatabaseQuery_startInsert(const char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_INSERT);
}

DatabaseQuery *DatabaseQuery_startUpdate(const char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_UPDATE);
}

DatabaseQuery *DatabaseQuery_startDelete(const char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_DELETE);
}

DatabaseQueryCondition __attribute__((__used__)) *
DatabaseQuery_whereField(
    DatabaseQuery *query,
    const char *field,
    const char *operator,
    const char *value,
    JSONType type
) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->field = DatabaseQuery_createDatabaseQueryField();
  condition->field->name = clone_cstr(field);

  char *v = NULL;
  if (value) {
    v = SQL_escape_string(value);
    if (type == JSON_STRING) {
      char *tmp = clone_cstr("'");
      tmp = append_cstr(tmp, v);
      tmp = append_cstr(tmp, "'");
      free(v);
      v = tmp;
    }
  } else {
    v = clone_cstr("NULL");
  }
  condition->value = v;
  condition->operator = clone_cstr(operator);
  condition->type = DATABASE_QUERY_CONDITION_TYPE_VALUE;
  DatabaseQuery_append_DatabaseQueryCondition_to_DatabaseQuery_conditions(query, condition);
  return condition;
}

DatabaseQueryCondition __attribute__((__used__)) *
DatabaseQuery_whereFieldWithCall(
    DatabaseQuery *query,
    const char *field,
    const char *operator,
    const char *value,
    const char *caller,
    JSONType type
) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->field = DatabaseQuery_createDatabaseQueryField();
  condition->field->name = clone_cstr(field);

  char *v = NULL;
  if (value) {
    if (type == JSON_STRING) {
      char *escaped = SQL_escape_string(value);
      v = clone_cstr(caller);
      v = append_cstr(v, "('");
      v = append_cstr(v, escaped);
      v = append_cstr(v, "')");
      free(escaped);
    } else {
      v = SQL_escape_string(value);
    }
  } else {
    v = clone_cstr("NULL");
  }

  condition->value = v;
  condition->operator = clone_cstr(operator);
  condition->type = DATABASE_QUERY_CONDITION_TYPE_VALUE;
  DatabaseQuery_append_DatabaseQueryCondition_to_DatabaseQuery_conditions(query, condition);
  return condition;
}

DatabaseQueryCondition __attribute__((__used__)) *
DatabaseQuery_whereSQL(DatabaseQuery *query, const char *pure) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->pure = SQL_escape_string(pure);
  condition->type = DATABASE_QUERY_CONDITION_TYPE_PURE_SQL;
  DatabaseQuery_append_DatabaseQueryCondition_to_DatabaseQuery_conditions(query, condition);
  return condition;
}

DatabaseQueryJoin *
DatabaseQuery_join(
    DatabaseQuery *query,
    const char *joinTableName,
    const char *joinFieldName,
    const char *queriedTableName,
    const char *queriedFieldName,
    DatabaseQueryJoinType type
) {
  DatabaseQueryJoin *join = DatabaseQuery_createDatabaseQueryJoin();

  DatabaseQueryTable *table = DatabaseQuery_createDatabaseQueryTable();
  table->name = clone_cstr(joinTableName);
  join->table = table;
  join->type = type;

  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();

  condition->field = DatabaseQuery_createDatabaseQueryField();
  condition->field->name = clone_cstr(joinFieldName);
  condition->field->table = DatabaseQuery_createDatabaseQueryTable();
  condition->field->table->name = clone_cstr(joinTableName);

  condition->otherField = DatabaseQuery_createDatabaseQueryField();
  condition->otherField->name = clone_cstr(queriedFieldName);
  condition->otherField->table = DatabaseQuery_createDatabaseQueryTable();
  condition->otherField->table->name = clone_cstr(queriedTableName);

  condition->operator = clone_cstr("=");

  condition->type = DATABASE_QUERY_CONDITION_TYPE_OTHER_FIELD;
  DatabaseQuery_append_DatabaseQueryCondition_to_DatabaseQueryJoin_conditions(join, condition);
  DatabaseQuery_append_DatabaseQueryJoin_to_DatabaseQuery_joins(query, join);
  return join;
}

DatabaseQueryField *
DatabaseQuery_select(
    DatabaseQuery *query,
    const char *tableName,
    const char *fieldName,
    const char *as, JSONType type
) {
  DatabaseQueryTable *table = DatabaseQuery_createDatabaseQueryTable();
  table->name = clone_cstr(tableName);
  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  field->as = clone_cstr(as == NULL ? fieldName : as);
  field->table = table;
  field->jsonType = type;
  DatabaseQuery_append_DatabaseQueryField_to_DatabaseQuery_fields(query, field);
  return field;
}

DatabaseQueryFieldValue __attribute__((__used__)) *
DatabaseQuery_insert(DatabaseQuery *query, const char *fieldName, const char *value, JSONType type) {
  char *val = clone_cstr(value);
  if (type == JSON_STRING) {
    char *joined = clone_cstr("'");
    char *escaped = SQL_escape_string(val);
    joined = append_cstr(joined, escaped);
    joined = append_cstr(joined, "'");
    free(val);
    free(escaped);
    val = joined;
  }

  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  DatabaseQueryFieldValue *fieldValue = DatabaseQuery_createDatabaseQueryFieldValue();
  fieldValue->field = field;
  fieldValue->value = val;

  DatabaseQuery_append_DatabaseQueryFieldValue_to_DatabaseQuery_fieldValues(query, fieldValue);
  return fieldValue;
}

DatabaseQueryFieldValue __attribute__((__used__)) *
DatabaseQuery_update(DatabaseQuery *query, const char *fieldName, const char *value, JSONType type) {
  char *val = clone_cstr(value);
  if (type == JSON_STRING) {
    char *joined = clone_cstr("'");
    char *escaped = SQL_escape_string(val);
    joined = append_cstr(joined, escaped);
    joined = append_cstr(joined, "'");
    free(val);
    free(escaped);
    val = joined;
  }

  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  DatabaseQueryFieldValue *fieldValue = DatabaseQuery_createDatabaseQueryFieldValue();
  fieldValue->field = field;
  fieldValue->value = val;

  DatabaseQuery_append_DatabaseQueryFieldValue_to_DatabaseQuery_fieldValues(query, fieldValue);
  return fieldValue;
}

DatabaseQueryLimit __attribute__((__used__)) *
DatabaseQuery_limit(DatabaseQuery *query, const char *value) {
  DatabaseQueryLimit *limit = DatabaseQuery_createDatabaseQueryLimit();
  limit->limit = clone_cstr(value);
  query->limit = limit;
  return limit;
}

DatabaseQueryField __attribute__((__used__)) *
DatabaseQuery_returning(DatabaseQuery *query, const char *tableName, const char *fieldName, JSONType type) {
  DatabaseQueryField *returning = DatabaseQuery_createDatabaseQueryField();
  returning->table = DatabaseQuery_createDatabaseQueryTable();
  returning->table->name = clone_cstr(tableName);
  returning->name = clone_cstr(fieldName);
  returning->as = clone_cstr(fieldName);
  returning->jsonType = type;
  DatabaseQuery_append_DatabaseQueryField_to_DatabaseQuery_returning(query, returning);
  return returning;
}

DatabaseQueryDistinct __attribute__((__used__)) *
DatabaseQuery_distinctOn(DatabaseQuery *query, const char *tableName, const char *fieldName) {
  DatabaseQueryDistinct *distinct = query->distinct;
  if (!distinct) {
    query->distinct = distinct = DatabaseQuery_createDatabaseQueryDistinct();
  }

  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->table = DatabaseQuery_createDatabaseQueryTable();
  field->name = clone_cstr(fieldName);
  field->as = clone_cstr(fieldName);
  field->table->name = clone_cstr(tableName);

  DatabaseQuery_append_DatabaseQueryField_to_DatabaseQueryDistinct_fields(distinct, field);

  return distinct;
}

DatabaseQueryOrder __attribute__((__used__)) *
DatabaseQuery_order(DatabaseQuery *query, const char *tableName, const char *fieldName,
                    DatabaseQueryOrderDirection direction) {
  DatabaseQueryOrder *order = DatabaseQuery_createDatabaseQueryOrder();
  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->table = DatabaseQuery_createDatabaseQueryTable();
  field->name = clone_cstr(fieldName);
  field->table->name = clone_cstr(tableName);
  order->direction = direction;
  order->field = field;
  DatabaseQuery_append_DatabaseQueryOrder_to_DatabaseQuery_orders(query, order);
  return order;
}

char
DatabaseQuery_isDirty(const char *value) {
  const char *ptr = value;
  while (ptr && *ptr) if (*ptr == '\'') return 1; else ptr += 1;
  return 0;
}
