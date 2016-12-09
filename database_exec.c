#include "json.h"
#include "./database_exec.h"

static void Database_setObjectValue(
    JSON *object,
    const char *currentFieldAs,
    char *value
) {
  wchar_t *fieldName = cstr2wcstr(currentFieldAs);
  JSON_set(object, fieldName, JSON_string(value));
  free(fieldName);
}

static JSON *Database_findCurrent(JSON *array, const char *id);

static DatabaseQueryField **Database_orderedFields(const DatabaseQuery *query);

static JSON *Database_flatSerialization(const DatabaseQuery *query, struct kore_pgsql *kore_sql);

static JSON *
Database_nestedSerialization(const DatabaseQuery *query, DatabaseQueryField **fields, struct kore_pgsql *kore_sql);

static JSON *Database_findCollection(JSON *root, const char *name) {
  if (root == NULL) {
    kore_log(LOG_CRIT, "Attempt to looking in NULL, terminating!");
    exit(1);
  }
  kore_log(LOG_INFO, "Looking for collection '%s'", name);
  JSON **children = root->children.objects;
  char **keys = root->children.keys;
  JSON *object = NULL;

  while (*children) {
    if (strcmp(*keys, name) == 0) {
      object = *children;
    }
    keys += 1;
    children += 1;
  }

  if (object == NULL) {
    wchar_t *tmp = cstr2wcstr(name);
    JSON_set(root, tmp, object = JSON_alloc(JSON_ARRAY));
    free(tmp);
  }
  return object;
}

static JSON *Database_findCurrent(JSON *array, const char *id) {
  if (array == NULL) {
    kore_log(LOG_CRIT, "array is NULL!!!!");
    exit(100);
  }
  JSON *found = NULL;
  JSON **children = array->array.objects;
  while (children && *children) {
    JSON *object = *children;
    JSON **fields = object->children.objects;
    char **keys = object->children.keys;
    while (*keys && strcmp(*keys, "id") != 0) {
      keys += 1;
      fields += 1;
    }
    if (*fields && strcmp((*fields)->string, id) == 0) {
      found = object;
      break;
    }
    children += 1;
  }
  return found;
}

static unsigned char Database_hasAnyField(const char *name, DatabaseQueryField **fields, unsigned long fieldsSize) {
  DatabaseQueryField **it = fields;
  for (unsigned long i = 0; i < fieldsSize; i++) {
    DatabaseQueryField *field = it[0];
    if (field->table && field->table->name && strcmp(field->table->name, name) == 0) {
      return 1;
    }
    it += 1;
  }
  return 0;
}

static JSON *
Database_nestedSerialization(const DatabaseQuery *query, DatabaseQueryField **fields, struct kore_pgsql *kore_sql) {
  kore_log(LOG_INFO, "Database_nestedSerialization");

  int rows = kore_pgsql_ntuples(kore_sql);
  kore_log(LOG_INFO, "Parsing %i rows", rows);

  char *value = NULL;
  const char *rootTable = query->table->name;
  wchar_t *rootTableName = cstr2wcstr(rootTable);
  const char *currentTableName = NULL, *currentFieldName = NULL, *currentFieldAs = NULL;
  JSON *array = JSON_alloc(JSON_ARRAY), *root = JSON_alloc(JSON_OBJECT);
  JSON_set(root, rootTableName, array);
  free(rootTableName);

  unsigned long size = query->fieldsSize ? query->fieldsSize : query->returningSize;

  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    const char *lastTableName = NULL;
    JSON *current = root;

    DatabaseQueryField **ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < size; fieldIndex++) {
      currentTableName =  (*ptr)->table ? (*ptr)->table->name : NULL;
      currentFieldName =  (*ptr)->name;
      currentFieldAs =    (*ptr)->as;

      if (!currentTableName) {
        kore_log(LOG_INFO, "  missing table name, skipping...");
        ptr += 1;
        continue;
      }
      if (!currentFieldName) {
        kore_log(LOG_INFO, "  missing field name, skipping...");
        ptr += 1;
        continue;
      }
      if (!currentFieldAs) {
        kore_log(LOG_INFO, "  missing field as, skipping...");
        ptr += 1;
        continue;
      }

      value = kore_pgsql_getvalue(kore_sql, rowIndex, fieldIndex);

      if (!Database_hasAnyField(currentTableName, fields, size)) {
        ptr += 1;
        continue;
      }

      JSON *old = NULL;
      if (lastTableName == NULL || strcmp(lastTableName, currentTableName) != 0) {
        array = Database_findCollection(current, currentTableName);
        old = current;
        current = Database_findCurrent(array, value);
        lastTableName = currentTableName;
      }

      if (value == NULL || strlen(value) == 0) {
        if (old != NULL && current == NULL) current = old;
        ptr += 1;
        continue;
      }

      if (current == NULL)
        JSON_append(array, current = JSON_alloc(JSON_OBJECT));

      Database_setObjectValue(current, currentFieldAs, value);
      ptr += 1;
    }
  }

  free(fields);

  return root;
}

static JSON *Database_flatSerialization(const DatabaseQuery *query, struct kore_pgsql *kore_sql) {
  kore_log(LOG_INFO, "Database_flatSerialization");
  int rows = kore_pgsql_ntuples(kore_sql);
  unsigned char useArray = 1;

  if (DB_QUERY_IS_NON_SELECT(query) || DB_QUERY_ONLY_ONE(query)) {
    useArray = 0;
  }

  char *value = NULL;
  const char *rootTable = query->table->name;
  wchar_t *rootTableName = cstr2wcstr(rootTable);
  const char *currentTableName = NULL, *currentFieldName = NULL, *currentFieldAs = NULL;
  JSON
      *root = JSON_alloc(JSON_OBJECT),
      *array = NULL,
      *current = NULL;

  if (useArray) {
    array = JSON_alloc(JSON_ARRAY);
    JSON_set(root, rootTableName, array);
  } else {
    current = JSON_alloc(JSON_OBJECT);
    JSON_set(root, rootTableName, current);
  }
  free(rootTableName);

  unsigned long size = query->fieldsSize ? query->fieldsSize : query->returningSize;
  DatabaseQueryField **fields = query->fields ? query->fields : query->returning;
  if (fields == NULL) rows = 0;

  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    if (useArray) {
      current = JSON_alloc(JSON_OBJECT);
      JSON_append(array, current);
    }

    DatabaseQueryField **ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < size; fieldIndex++) {
      currentTableName = (*ptr)->table ? (*ptr)->table->name : NULL;
      currentFieldName = (*ptr)->name;
      currentFieldAs = (*ptr)->as;
      value = kore_pgsql_getvalue(kore_sql, rowIndex, fieldIndex);

      if (!currentTableName) continue;
      if (!currentFieldName) continue;
      Database_setObjectValue(current, currentFieldAs, value);
      ptr += 1;
    }
  }

  return root;
}

static DatabaseQueryField **Database_orderedFields(const DatabaseQuery *query) {
  kore_log(LOG_INFO, "Database_orderedFields");
  DatabaseQueryField **fields = query->fields ? query->fields : query->returning;
  if (!fields) return NULL;

  unsigned int fieldsSize = query->fieldsSize ? query->fieldsSize : query->returningSize;
  unsigned int tablesSize = 1;

  if (DB_QUERY_HAS_NO_JOINS(query)) {
    return NULL;
  } else {
    tablesSize += query->joinsSize;
  }

  DatabaseQueryField **ordered = calloc(sizeof(DatabaseQueryField *), fieldsSize);
  const char **tables = calloc(sizeof(char *), tablesSize);

  tables[0] = query->table->name;
  for (unsigned int joinsIndex = 0; joinsIndex < query->joinsSize; joinsIndex++) {
    tables[joinsIndex + 1] = query->joins[joinsIndex]->table->name;
  }

  unsigned int orderedIndex = 0;
  DatabaseQueryField **ptr = NULL;
  const char *tableName = NULL;
  unsigned short int idFound = 0;

  for (unsigned int tableIndex = 0; tableIndex < tablesSize; tableIndex++) {
    idFound = 0;
    tableName = tables[tableIndex];
    ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < fieldsSize; fieldIndex++) {
      DatabaseQueryField *field = ptr[0];
      if (strcmp(field->table->name, tableName) == 0 && strcmp(field->name, "id") == 0) {
        ordered[orderedIndex] = field;
        orderedIndex += 1;
        idFound = 1;
      }
      ptr += 1;
    }

    ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < fieldsSize; fieldIndex++) {
      DatabaseQueryField *field = *ptr;
      if (strcmp(field->table->name, tableName) == 0 && strcmp(field->name, "id") != 0 &&
          strstr(field->name, "_at") == NULL) {
        if (idFound != 0) {
          ordered[orderedIndex] = field;
          orderedIndex += 1;
        } else {
          free(ordered);
          ordered = NULL;
          fieldsSize = 0;
        }
      }
      ptr += 1;
    }

    ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < fieldsSize; fieldIndex++) {
      DatabaseQueryField *field = *ptr;
      if (
          strcmp(field->table->name, tableName) == 0 &&
          strcmp(field->name, "id") != 0 &&
          strstr(field->name, "_at") != NULL
          ) {
        ordered[orderedIndex] = field;
        orderedIndex += 1;
      }
      ptr += 1;
    }
  }
  free(tables);
  return ordered;
}

static JSON *
Database_buildResult(const DatabaseQuery *query, struct kore_pgsql *kore_sql) {
  DatabaseQueryField **ordered = Database_orderedFields(query);
  if (ordered) {
    return Database_nestedSerialization(query, ordered, kore_sql);
  } else {
    return Database_flatSerialization(query, kore_sql);
  }
}

JSON *Database_execQuery(DatabaseQuery *query) {
  char *sql = DatabaseQuery_stringify(query);
  JSON *result = JSON_alloc(JSON_OBJECT);
  struct kore_pgsql kore_sql;

  if (!kore_pgsql_query_init(&kore_sql, NULL, "db", KORE_PGSQL_SYNC)) {
    kore_pgsql_logerror(&kore_sql);
    kore_pgsql_cleanup(&kore_sql);
    return result;
  }

  unsigned char queryResult = (unsigned char) kore_pgsql_query(&kore_sql, sql);
  DatabaseQuery_freeSQL(sql);

  if (!queryResult) {
    kore_pgsql_logerror(&kore_sql);
  } else {
    JSON_free(result);
    result = Database_buildResult(query, &kore_sql);
  }

  kore_pgsql_cleanup(&kore_sql);
  return result;
}

JSON *Database_execSql(const char *rawSQL, const int fieldsCount, const char **fields, const int argc,
                       const char **argv) {
  JSON *array = JSON_alloc(JSON_ARRAY);
  struct kore_pgsql kore_sql;
  int rows = 0;
  char *value = NULL;

  if (!kore_pgsql_query_init(&kore_sql, NULL, "db", KORE_PGSQL_SYNC)) {
    kore_pgsql_logerror(&kore_sql);
    kore_pgsql_cleanup(&kore_sql);
    return array;
  }

  char *sql = SQL_prepare_sql(rawSQL, argc, argv);

  unsigned char query_result = (unsigned char) kore_pgsql_query(&kore_sql, sql);
  free(sql);

  if (!query_result) {
    kore_pgsql_logerror(&kore_sql);
  } else {
    rows = kore_pgsql_ntuples(&kore_sql);

    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      JSON *child = JSON_alloc(JSON_OBJECT);
      JSON_append(array, child);
      for (int fieldIndex = 0; fieldIndex < fieldsCount; fieldIndex++) {
        const char *field = fields[fieldIndex];
        wchar_t *fieldName = cstr2wcstr(field);
        value = kore_pgsql_getvalue(&kore_sql, rowIndex, fieldIndex);
        wchar_t *wstr = cstr2wcstr(value);
        JSON_set(child, fieldName, JSON_wstring(wstr));
        free(wstr);
        free(fieldName);
      }
    }
  }

  kore_pgsql_cleanup(&kore_sql);
  return array;
}
