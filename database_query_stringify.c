#include "./database_query_stringify.h"

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryCondition(DatabaseQueryCondition *condition);

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryJoinForUpdate(DatabaseQueryJoin *join) {
  char *joined = NULL;
  DatabaseQueryCondition **conditions = join->conditions;

  joined = append_cstr(joined, "FROM ");
  joined = append_cstr(joined, join->table->name);
  joined = append_cstr(joined, " WHERE ");

  for (unsigned int fieldIndex = 0; fieldIndex < join->conditionsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      joined = append_cstr(joined, " AND ");
    }

    char *condition = DatabaseQuery_stringifyDatabaseQueryCondition(*conditions);
    joined = append_cstr(joined, condition);
    free(condition);

    conditions += 1;
  }

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryField(DatabaseQueryField *field) {
  char *joined = NULL;

  if (field->table) {
    joined = clone_cstr(field->table->name);
    joined = append_cstr(joined, ".");
  }

  joined = append_cstr(joined, field->name);
  if (field->as) {
    joined = append_cstr(joined, " AS ");
    joined = append_cstr(joined, field->as);
  }

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryCondition(DatabaseQueryCondition *condition) {
  char *joined = NULL, *fieldA = NULL, *value = NULL;

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

  joined = clone_cstr(fieldA);
  joined = append_cstr(joined, " ");
  joined = append_cstr(joined, condition->operator);
  joined = append_cstr(joined, " ");
  joined = append_cstr(joined, value);

  free(fieldA);
  free(value);

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryJoin(DatabaseQueryJoin *join) {
  char *joined = NULL;
  DatabaseQueryCondition **conditions = join->conditions;

  switch (join->type) {
    case DATABASE_QUERY_JOIN_TYPE_NORMAL:
      break;
    case DATABASE_QUERY_JOIN_TYPE_INNER: {
      joined = append_cstr(joined, "INNER ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_LEFT: {
      joined = append_cstr(joined, "LEFT ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_LEFT_OUTER: {
      joined = append_cstr(joined, "LEFT OUTER ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_RIGHT: {
      joined = append_cstr(joined, "RIGHT ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_RIGHT_OUTER: {
      joined = append_cstr(joined, "RIGHT OUTER ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_FULL: {
      joined = append_cstr(joined, "FULL ");
      break;
    }
    case DATABASE_QUERY_JOIN_TYPE_FULL_OUTER: {
      joined = append_cstr(joined, "FULL OUTER ");
      break;
    }
  }

  joined = append_cstr(joined, "JOIN ");
  joined = append_cstr(joined, join->table->name);
  joined = append_cstr(joined, " ON ");

  for (unsigned int fieldIndex = 0; fieldIndex < join->conditionsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      joined = append_cstr(joined, " AND ");
    }

    char *condition = DatabaseQuery_stringifyDatabaseQueryCondition(*conditions);
    joined = append_cstr(joined, condition);
    free(condition);

    conditions += 1;
  }

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDatabaseQueryDistinct(DatabaseQueryDistinct *distinct) {
  if (!distinct) return NULL;
  char *joined = clone_cstr(" DISTINCT ON (");
  DatabaseQueryField **fields = distinct->fields;
  for (unsigned int fieldIndex = 0; fieldIndex < distinct->fieldsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      joined = append_cstr(joined, ", ");
    }
    if ((*fields)->table) {
      joined = append_cstr(joined, (*fields)->table->name);
      joined = append_cstr(joined, ".");
    }

    joined = append_cstr(joined, (*fields)->name);
    fields += 1;
  }

  joined = append_cstr(joined, ") ");
  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifySelect(DatabaseQuery *query) {
  if (!query || !query->table) return NULL;

  char *joined = clone_cstr("SELECT "), *generated = NULL;

  if (query->distinct) {
    generated = DatabaseQuery_stringifyDatabaseQueryDistinct(query->distinct);
    joined = append_cstr(joined, generated);
    free(generated);
  }

  DatabaseQueryField **fields = query->fields;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldsSize; fieldIndex++) {
    if (fieldIndex > 0) {
      joined = append_cstr(joined, ", ");
    }

    generated = DatabaseQuery_stringifyDatabaseQueryField(*fields);
    joined = append_cstr(joined, generated);
    free(generated);
    fields += 1;
  }

  joined = append_cstr(joined, " FROM ");
  joined = append_cstr(joined, query->table->name);

  DatabaseQueryJoin **joins = query->joins;
  for (unsigned int joinIndex = 0; joinIndex < query->joinsSize; joinIndex++) {
    joined = append_cstr(joined, " ");
    generated = DatabaseQuery_stringifyDatabaseQueryJoin(*joins);
    joined = append_cstr(joined, generated);
    free(generated);
    joins += 1;
  }

  if (query->conditionsSize) {
    DatabaseQueryCondition **conditions = query->conditions;
    joined = append_cstr(joined, " WHERE ");

    for (unsigned int conditionIndex = 0; conditionIndex < query->conditionsSize; conditionIndex++) {
      if (conditionIndex > 0) {
        joined = append_cstr(joined, " AND ");
      }

      generated = DatabaseQuery_stringifyDatabaseQueryCondition(*conditions);
      joined = append_cstr(joined, generated);
      free(generated);
      conditions += 1;
    }
  }

  if (query->ordersSize) {
    joined = append_cstr(joined, " ORDER BY ");
    DatabaseQueryOrder **orders = query->orders;
    for (unsigned int i = 0; i < query->ordersSize; i++) {
      if (i > 0) joined = append_cstr(joined, ", ");

      if ((*orders)->pure) {
        joined = append_cstr(joined, (*orders)->pure);
      } else if ((*orders)->field) {
        generated = DatabaseQuery_stringifyDatabaseQueryField((*orders)->field);
        joined = append_cstr(joined, generated);
        free(generated);
        switch ((*orders)->direction) {
          case DATABASE_QUERY_ORDER_ASC: {
            joined = append_cstr(joined, " ASC");
            break;
          }
          case DATABASE_QUERY_ORDER_DESC: {
            joined = append_cstr(joined, " DESC");
            break;
          }
        }
      }

      orders += 1;
    }
  }

  if (query->limit) {
    joined = append_cstr(joined, " LIMIT ");
    joined = append_cstr(joined, query->limit->limit);
  }

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyInsert(DatabaseQuery *query) {
  if (!query || !query->table || !query->fieldValues) return NULL;

  char *joined = clone_cstr("INSERT INTO ");

  DatabaseQueryFieldValue **fields;
  DatabaseQueryField **returning;

  joined = append_cstr(joined, query->table->name);
  joined = append_cstr(joined, " (");

  fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) joined = append_cstr(joined, ", ");
    joined = append_cstr(joined, fields[0]->field->name);
    fields += 1;
  }
  joined = append_cstr(joined, ") VALUES (");
  fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) joined = append_cstr(joined, ", ");
    joined = append_cstr(joined, fields[0]->value);
    fields += 1;
  }
  joined = append_cstr(joined, ")");
  returning = query->returning;
  if (returning) {
    joined = append_cstr(joined, " RETURNING ");
    for (unsigned int fieldIndex = 0; fieldIndex < query->returningSize; fieldIndex++) {
      if (fieldIndex > 0) joined = append_cstr(joined, ", ");
      joined = append_cstr(joined, returning[0]->as);
      returning += 1;
    }
  }

  return joined;
}

static char __attribute__((__used__)) *
DatabaseQuery_stringifyUpdate(DatabaseQuery *query) {
  if (query == NULL || query->fieldValues == NULL) return NULL;
  char *sql = clone_cstr("UPDATE ");
  sql = append_cstr(sql, query->table->name);

  sql = append_cstr(sql, " SET ");

  DatabaseQueryFieldValue **fields = query->fieldValues;
  for (unsigned int fieldIndex = 0; fieldIndex < query->fieldValuesSize; fieldIndex++) {
    if (fieldIndex > 0) sql = append_cstr(sql, ", ");
    sql = append_cstr(sql, (*fields)->field->name);
    sql = append_cstr(sql, " = ");
    sql = append_cstr(sql, (*fields)->value);
    fields += 1;
  }

  DatabaseQueryJoin **joins = query->joins;
  if (joins) {
    sql = append_cstr(sql, " ");
    char *generated = DatabaseQuery_stringifyDatabaseQueryJoinForUpdate(*joins);
    sql = append_cstr(sql, generated);
    free(generated);
  }

  DatabaseQueryCondition **conditions = query->conditions;
  if (conditions) {
    if (joins) {
      sql = append_cstr(sql, " AND ");
    } else {
      sql = append_cstr(sql, " WHERE ");
    }
  }
  for (unsigned int conditionIndex = 0; conditionIndex < query->conditionsSize; ++conditionIndex) {
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

static char __attribute__((__used__)) *
DatabaseQuery_stringifyDelete(DatabaseQuery *query) {
  if (query == NULL) return NULL;

  char *sql = clone_cstr("DELETE FROM ");
  sql = append_cstr(sql, query->table->name);
  if (query->conditions) {
    DatabaseQueryCondition **conditions = query->conditions;
    if (conditions) sql = append_cstr(sql, "WHERE ");
    for (unsigned int conditionIndex = 0; conditionIndex < query->conditionsSize; ++conditionIndex) {
      if (conditionIndex > 0) sql = append_cstr(sql, " AND ");
      char *condition = DatabaseQuery_stringifyDatabaseQueryCondition(*conditions);
      sql = append_cstr(sql, condition);
      free(condition);
      conditions += 1;
    }
  }
  return sql;
}

char __attribute__((__used__)) *
DatabaseQuery_stringify(DatabaseQuery *query) {
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

void __attribute__((__used__))
DatabaseQuery_freeSQL(char *sql) {
  free((void *) sql);
}
