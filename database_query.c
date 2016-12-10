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

char *
SQL_prepare_sql(const char *query, const int argsSize, const char **args) {
  char *sql = NULL;
  u_long size = 0;
  for (u_long i = 0, len = strlen(query); i < len; i++) {
    char c = query[i];
    switch (c) {
      case '$': {
        char *indexStr = NULL;
        u_long indexStrLen = 0;
        volatile char lookupIndexValue = 1;
        while (i < len && lookupIndexValue) {
          if (i >= len) break;
          char n = query[i + 1];
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
              i += 1;
              char *b = calloc(sizeof(char), indexStrLen + 1 + 1);
              if (indexStr) {
                strcpy(b, indexStr);
                free(indexStr);

              }
              b[indexStrLen] = n;
              indexStr = b;
              indexStrLen += 1;
              break;

            }
            default: {
              lookupIndexValue = 0;
              break;

            }
          }
        }

        int index = atoi(indexStr) - 1;
        free(indexStr);
        if (index > -1 && index < argsSize) {
          char *escaped = SQL_escape_string(args[index]);
          u_long escapedLen = strlen(escaped);
          if (!escaped) break;
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
DatabaseQuery_start(char *tableName, DatabaseQueryType type) {
  DatabaseQuery *q = DatabaseQuery_createDatabaseQuery();
  q->table = DatabaseQuery_createDatabaseQueryTable();
  q->table->name = clone_cstr(tableName);
  q->type = type;
  return q;
}

DatabaseQuery *DatabaseQuery_startSelect(char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_SELECT);
}

DatabaseQuery *DatabaseQuery_startInsert(char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_INSERT);
}

DatabaseQuery *DatabaseQuery_startUpdate(char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_UPDATE);
}

DatabaseQuery *DatabaseQuery_startDelete(char *tableName) {
  return DatabaseQuery_start(tableName, DATABASE_QUERY_TYPE_DELETE);
}

DatabaseQueryCondition *
DatabaseQuery_whereField(
    DatabaseQuery *query,
    const char *field, const char *operator, const char *value,
    unsigned short int isString
) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->field = DatabaseQuery_createDatabaseQueryField();
  condition->field->name = clone_cstr(field);

  char *v = NULL;
  if (value) {
    v = SQL_escape_string(value);
    if (isString) {
      char *tmp;

      tmp = join_cstr("'", v);
      free(v);
      SWAP_CSTR(tmp, v);

      tmp = join_cstr(v, "'");
      free(v);
      SWAP_CSTR(tmp, v);
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

DatabaseQueryCondition *
DatabaseQuery_whereFieldWithCall(
    DatabaseQuery *query,
    const char *field,
    const char *operator,
    const char *value,
    const char *caller,
    unsigned short int isString
) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->field = DatabaseQuery_createDatabaseQueryField();
  condition->field->name = clone_cstr(field);

  char *v = NULL;
  if (value) {
    if (isString) {
      char *tmp, *escaped = SQL_escape_string(value);

      tmp = join_cstr(NULL, caller);
      free(v);
      SWAP_CSTR(tmp, v);

      tmp = join_cstr(v, "('");
      free(v);
      SWAP_CSTR(tmp, v);

      tmp = join_cstr(v, escaped);
      free(v);
      SWAP_CSTR(tmp, v);

      tmp = join_cstr(v, "')");
      free(v);
      SWAP_CSTR(tmp, v);

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

DatabaseQueryCondition *DatabaseQuery_whereSQL(DatabaseQuery *query, char *pure) {
  DatabaseQueryCondition *condition = DatabaseQuery_createDatabaseQueryCondition();
  condition->pure = SQL_escape_string(pure);
  condition->type = DATABASE_QUERY_CONDITION_TYPE_PURE_SQL;
  DatabaseQuery_append_DatabaseQueryCondition_to_DatabaseQuery_conditions(query, condition);
  return condition;
}

DatabaseQueryJoin *
DatabaseQuery_join(DatabaseQuery *query,
                   char *joinTableName, char *joinFieldName,
                   char *queriedTableName, char *queriedFieldName,
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

DatabaseQueryField *DatabaseQuery_select(DatabaseQuery *query, char *tableName, char *fieldName, char *as) {
  DatabaseQueryTable *table = DatabaseQuery_createDatabaseQueryTable();
  table->name = clone_cstr(tableName);
  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  field->as = clone_cstr(as == NULL ? fieldName : as);
  field->table = table;
  DatabaseQuery_append_DatabaseQueryField_to_DatabaseQuery_fields(query, field);
  return field;
}

DatabaseQueryFieldValue *
DatabaseQuery_insert(DatabaseQuery *query, char *fieldName, char *value, unsigned char isString) {
  value = clone_cstr(value);
  if (isString) {
    char *joined = join_cstr("'", value);
    SWAP_CSTR(value, joined);
    free(joined);
    value = append_cstr(value, "'");
  }

  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  DatabaseQueryFieldValue *fieldValue = DatabaseQuery_createDatabaseQueryFieldValue();
  fieldValue->field = field;
  fieldValue->value = value;

  DatabaseQuery_append_DatabaseQueryFieldValue_to_DatabaseQuery_fieldValues(query, fieldValue);
  return fieldValue;
}

DatabaseQueryFieldValue *
DatabaseQuery_update(DatabaseQuery *query, char *fieldName, char *value, unsigned char isString) {
  value = clone_cstr(value);
  if (isString) {
    char *joined = join_cstr("'", value);
    free(value);
    value =  joined;
    value = append_cstr(value, "'");
  }

  DatabaseQueryField *field = DatabaseQuery_createDatabaseQueryField();
  field->name = clone_cstr(fieldName);
  DatabaseQueryFieldValue *fieldValue = DatabaseQuery_createDatabaseQueryFieldValue();
  fieldValue->field = field;
  fieldValue->value = value;

  DatabaseQuery_append_DatabaseQueryFieldValue_to_DatabaseQuery_fieldValues(query, fieldValue);
  return fieldValue;
}

DatabaseQueryLimit *DatabaseQuery_limit(DatabaseQuery *query, char *value) {
  DatabaseQueryLimit *limit = DatabaseQuery_createDatabaseQueryLimit();
  limit->limit = clone_cstr(value);
  query->limit = limit;
  return limit;
}

DatabaseQueryField *DatabaseQuery_returning(DatabaseQuery *query, char *tableName, char *fieldName) {
  DatabaseQueryField *returning = DatabaseQuery_createDatabaseQueryField();
  returning->table = DatabaseQuery_createDatabaseQueryTable();
  returning->table->name = clone_cstr(tableName);
  returning->name = clone_cstr(fieldName);
  returning->as = clone_cstr(fieldName);
  DatabaseQuery_append_DatabaseQueryField_to_DatabaseQuery_returning(query, returning);
  return returning;
}

DatabaseQueryDistinct *
DatabaseQuery_distinctOn(DatabaseQuery *query, char *tableName, char *fieldName) {
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

DatabaseQueryOrder *
DatabaseQuery_order(DatabaseQuery *query, char *tableName, char *fieldName, DatabaseQueryOrderDirection direction) {
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

static char *DatabaseQuery_stringifyDatabaseQueryField(DatabaseQueryField *field) {
  char *joined = NULL, *tmp = NULL;

  if (field->table) {
    tmp = join_cstr(joined, field->table->name);
    free(joined);
    SWAP_CSTR(tmp, joined);

    tmp = join_cstr(joined, ".");
    free(joined);
    SWAP_CSTR(tmp, joined);
  }

  tmp = join_cstr(joined, field->name);
  free(joined);
  SWAP_CSTR(tmp, joined);

  if (field->as) {
    tmp = join_cstr(joined, " AS ");
    free(joined);
    SWAP_CSTR(tmp, joined);

    tmp = join_cstr(joined, field->as);
    free(joined);
    SWAP_CSTR(tmp, joined);
  }

  return joined;
}

static char *DatabaseQuery_stringifyDatabaseQueryCondition(DatabaseQueryCondition *condition) {
  char *joined = NULL, *tmp = NULL, *fieldA = NULL, *value = NULL;

  switch (condition->type) {
    case DATABASE_QUERY_CONDITION_TYPE_PURE_SQL: {
      return clone_cstr(condition->pure);
    }
    case DATABASE_QUERY_CONDITION_TYPE_VALUE: {
      value = clone_cstr(condition->value);
      break;
    }
    case DATABASE_QUERY_CONDITION_TYPE_OTHER_FIELD: {
      value = DatabaseQuery_stringifyDatabaseQueryField(condition->otherField);
      break;
    }
  }

  fieldA = DatabaseQuery_stringifyDatabaseQueryField(condition->field);

  tmp = join_cstr(NULL, fieldA);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, " ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, condition->operator);
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, " ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, value);
  free(joined);
  SWAP_CSTR(tmp, joined);

  free(fieldA);
  free(value);

  return joined;
}

static char *DatabaseQuery_stringifyDatabaseQueryJoin(DatabaseQueryJoin *join) {
  char *joined = NULL, *tmp = NULL;
  DatabaseQueryCondition **conditions = join->conditions;

  switch (join->type) {
    case DATABASE_QUERY_JOIN_TYPE_NORMAL:
      break;
    case DATABASE_QUERY_JOIN_TYPE_INNER: {
      tmp = join_cstr(joined, " INNER");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_LEFT: {
      tmp = join_cstr(joined, " LEFT");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_LEFT_OUTER: {
      tmp = join_cstr(joined, " LEFT OUTER");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_RIGHT: {
      tmp = join_cstr(joined, " RIGHT");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_RIGHT_OUTER: {
      tmp = join_cstr(joined, " RIGHT OUTER");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_FULL: {
      tmp = join_cstr(joined, " FULL");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_FULL_OUTER: {
      tmp = join_cstr(joined, " FULL OUTER");
      free(joined);
      SWAP_CSTR(tmp, joined);
      break;
    }
  }

  tmp = join_cstr(joined, " JOIN ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, join->table->name);
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, " ON ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  for (unsigned int fieldIndex = 0; fieldIndex < join->conditionsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      tmp = join_cstr(joined, " AND ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    tmp = join_cstr(joined, DatabaseQuery_stringifyDatabaseQueryCondition(conditions[0]));
    free(joined);
    SWAP_CSTR(tmp, joined);

    conditions += 1;
  }

  return joined;
}

static char *DatabaseQuery_stringifyDatabaseQueryDistinct(DatabaseQueryDistinct *distinct) {
  if (!distinct) return NULL;
  char *joined = NULL, *tmp = NULL, *generated = NULL;

  tmp = join_cstr(joined, " DISTINCT ON (");
  free(joined);
  SWAP_CSTR(tmp, joined);

  DatabaseQueryField **fields = distinct->fields;
  DatabaseQueryField *field = NULL;
  for (unsigned int fieldIndex = 0; fieldIndex < distinct->fieldsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      tmp = join_cstr(joined, ", ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    field = fields[0];

    if (field->table) {
      tmp = join_cstr(joined, field->table->name);
      free(generated);
      free(joined);
      SWAP_CSTR(tmp, joined);

      tmp = join_cstr(joined, ".");
      free(generated);
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    tmp = join_cstr(joined, field->name);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);

    fields += 1;
  }

  tmp = join_cstr(joined, ") ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  return joined;
}

static char *DatabaseQuery_stringifySelect(DatabaseQuery *query) {
  if (!query || !query->table) return NULL;

  char *joined = join_cstr(NULL, "SELECT "), *tmp = NULL, *generated = NULL;

  if (query->distinct) {
    generated = DatabaseQuery_stringifyDatabaseQueryDistinct(query->distinct);
    tmp = join_cstr(joined, generated);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);
  }

  DatabaseQueryField **fields = query->fields;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      tmp = join_cstr(joined, ", ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    generated = DatabaseQuery_stringifyDatabaseQueryField(fields[0]);
    tmp = join_cstr(joined, generated);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);

    fields += 1;
  }

  tmp = join_cstr(joined, " FROM ");
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, query->table->name);
  free(joined);
  SWAP_CSTR(tmp, joined);

  DatabaseQueryJoin **joins = query->joins;
  for (unsigned int joinIndex = 0; joinIndex < query->joinsSize; joinIndex++) {
    if (joinIndex > 0) {
      tmp = join_cstr(joined, " ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    generated = DatabaseQuery_stringifyDatabaseQueryJoin(joins[0]);
    tmp = join_cstr(joined, generated);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);

    joins += 1;
  }

  if (query->conditionsSize) {
    DatabaseQueryCondition **conditions = query->conditions;
    tmp = join_cstr(joined, " WHERE ");
    free(joined);
    SWAP_CSTR(tmp, joined);

    for (unsigned int conditionIndex = 0; conditionIndex < query->conditionsSize; conditionIndex++) {
      if (conditionIndex > 0) {
        tmp = join_cstr(joined, " AND ");
        free(joined);
        SWAP_CSTR(tmp, joined);
      }

      generated = DatabaseQuery_stringifyDatabaseQueryCondition(conditions[0]);
      tmp = join_cstr(joined, generated);
      free(generated);
      free(joined);
      SWAP_CSTR(tmp, joined);

      conditions += 1;
    }
  }

  if (query->ordersSize) {
    tmp = join_cstr(joined, " ORDER BY ");
    free(joined);
    SWAP_CSTR(tmp, joined);

    DatabaseQueryOrder **orders = query->orders;
    DatabaseQueryOrder *order = NULL;
    for (unsigned int i = 0; i < query->ordersSize; i++) {
      order = orders[0];

      if (i > 0) {
        tmp = join_cstr(joined, ", ");
        free(joined);
        SWAP_CSTR(tmp, joined);
      }

      if (order->pure) {
        tmp = join_cstr(joined, order->pure);
        free(joined);
        SWAP_CSTR(tmp, joined);
      } else if (order->field) {
        generated = DatabaseQuery_stringifyDatabaseQueryField(order->field);
        tmp = join_cstr(joined, generated);
        free(generated);
        free(joined);
        SWAP_CSTR(tmp, joined);

        switch (order->direction) {
          case DATABASE_QUERY_ORDER_ASC: {
            tmp = join_cstr(joined, " ASC");
            free(joined);
            SWAP_CSTR(tmp, joined);
            break;
          }
          case DATABASE_QUERY_ORDER_DESC: {
            tmp = join_cstr(joined, " DESC");
            free(joined);
            SWAP_CSTR(tmp, joined);
            break;
          }
        }
      }

      orders += 1;
    }
  }

  if (query->limit) {
    tmp = join_cstr(joined, " LIMIT ");
    free(joined);
    SWAP_CSTR(tmp, joined);

    tmp = join_cstr(joined, query->limit->limit);
    free(joined);
    SWAP_CSTR(tmp, joined);
  }

  return joined;
}

static char *DatabaseQuery_stringifyInsert(DatabaseQuery *query) {
  if (!query || !query->table || !query->fieldValues) return NULL;

  char *joined = join_cstr(NULL, "INSERT INTO "), *tmp = NULL, *generated = NULL;

  DatabaseQueryFieldValue **fields;
  DatabaseQueryField **returning;

  tmp = join_cstr(joined, query->table->name);
  free(joined);
  SWAP_CSTR(tmp, joined);

  tmp = join_cstr(joined, " (");
  free(joined);
  SWAP_CSTR(tmp, joined);

  fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) {
      tmp = join_cstr(joined, ", ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    tmp = join_cstr(joined, fields[0]->field->name);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);

    fields += 1;
  }


  tmp = join_cstr(joined, ") VALUES (");
  free(joined);
  SWAP_CSTR(tmp, joined);

  fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) {
      tmp = join_cstr(joined, ", ");
      free(joined);
      SWAP_CSTR(tmp, joined);
    }

    tmp = join_cstr(joined, fields[0]->value);
    free(generated);
    free(joined);
    SWAP_CSTR(tmp, joined);

    fields += 1;
  }

  tmp = join_cstr(joined, ")");
  free(joined);
  SWAP_CSTR(tmp, joined);

  returning = query->returning;
  if (returning) {
    tmp = join_cstr(joined, " RETURNING ");
    free(joined);
    SWAP_CSTR(tmp, joined);

    for (unsigned int fieldIndex = 0; fieldIndex < query->returningSize; fieldIndex++) {
      if (fieldIndex > 0) {
        tmp = join_cstr(joined, ", ");
        free(joined);
        SWAP_CSTR(tmp, joined);
      }

      tmp = join_cstr(joined, returning[0]->as);
      free(generated);
      free(joined);
      SWAP_CSTR(tmp, joined);

      returning += 1;
    }
  }

  return joined;
}

static char *DatabaseQuery_stringifyUpdate(DatabaseQuery *query) {
  if (query == NULL || query->fieldValues == NULL) return NULL;
  char *sql = clone_cstr("UPDATE ");
  sql = append_cstr(sql, query->table->name);
  sql = append_cstr(sql, " SET ");

  DatabaseQueryFieldValue **fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) sql = append_cstr(sql, ", ");
    sql = join_cstr(sql, (*fields)->field->name);
    sql = join_cstr(sql, " = ");
    sql = join_cstr(sql, (*fields)->value);
    fields += 1;
  }

  DatabaseQueryCondition **conditions = query->conditions;
  if (conditions) sql = append_cstr(sql, "WHERE ");
  for (int conditionIndex = 0; conditionIndex < query->conditionsSize; ++conditionIndex) {
    if (conditionIndex > 0) sql = append_cstr(sql, " AND ");
    char *condition = DatabaseQuery_stringifyDatabaseQueryCondition(*conditions);
    sql = append_cstr(sql, condition);
    free(condition);
    conditions += 1;
  }

  if (query->limit) {
    sql = append_cstr(sql, "LIMIT ");
    sql = append_cstr(sql, query->limit->limit);
  }

  return sql;
}

static char *DatabaseQuery_stringifyDelete(DatabaseQuery *query) {
  return NULL;
}

char *DatabaseQuery_stringify(DatabaseQuery *query) {
  switch (query->type) {
    case DATABASE_QUERY_TYPE_SELECT:
      return DatabaseQuery_stringifySelect(query);
    case DATABASE_QUERY_TYPE_INSERT:
      return DatabaseQuery_stringifyInsert(query);
    case DATABASE_QUERY_TYPE_UPDATE:
      return DatabaseQuery_stringifyUpdate(query);
    case DATABASE_QUERY_TYPE_DELETE:
      return DatabaseQuery_stringifyDelete(query);
    default:
      return NULL;
  }
}

void DatabaseQuery_freeSQL(char *sql) {
  free((void *) sql);
}
