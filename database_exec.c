#include "json.h"
#include "./database_exec.h"

static JSON *
Database_findCurrent(JSON *array, const char *id);

static DatabaseQueryField **
Database_orderedFields(const DatabaseQuery *query);

static JSON *
Database_flatSerialization(
    const DatabaseQuery *query,
    struct kore_pgsql *kore_sql
);

static JSON *
Database_nestedSerialization(
    const DatabaseQuery *query,
    DatabaseQueryField **fields,
    struct kore_pgsql *kore_sql
);


static DatabaseJoinChains *
Database_buildJoinChains(
    const DatabaseQuery *query,
    DatabaseQueryField **fields,
    unsigned long fieldsSize
) {
  DatabaseJoinChains *joinChains = calloc(sizeof(DatabaseJoinChains), 1);

  DatabaseQueryJoin **joins = calloc(sizeof(DatabaseQueryJoin *), query->joinsSize);
  DatabaseQueryJoin **release = joins;

  memcpy(joins, query->joins, sizeof(DatabaseQueryJoin *) * query->joinsSize);
  unsigned int len = query->joinsSize;

  while (len) {
    DatabaseQueryJoin *join = *joins;
    if (!join) {
      len -= 1;
      continue;
    }
    *joins = NULL;
    DatabaseQueryCondition *condition = *join->conditions;

    DatabaseJoinChain *joinChain = calloc(sizeof(DatabaseJoinChain), 1);
    joinChain->len = 2;
    joinChain->chain = calloc(sizeof(char **), joinChain->len + 1);

    if (joinChains->chains == NULL) {
      joinChains->len = 1;
      joinChains->chains = calloc(sizeof(DatabaseJoinChain **), joinChains->len + 1);
    } else {
      joinChains->len += 1;
      joinChains->chains = realloc(joinChains->chains, sizeof(DatabaseJoinChain **) * (joinChains->len + 1));
      joinChains->chains[joinChains->len] = 0;
    }
    joinChains->chains[joinChains->len - 1] = joinChain;

    char *queriedTable = condition->otherField->table->name;
    char *joinedTable = condition->field->table->name;
    joinChain->chain[0] = queriedTable;
    joinChain->chain[1] = joinedTable;

    unsigned int nextLen = len - 1;

    while (nextLen) {
      if (*(joins + 1) == NULL) {
        nextLen -= 1;
        continue;
      }
      DatabaseQueryJoin *nextJoin = *(joins + 1);
      DatabaseQueryCondition *nextCondition = *nextJoin->conditions;

      char *nextQueriedTable = nextCondition->otherField->table->name;
      char *nextJoinedTable = nextCondition->field->table->name;

      if (strcmp(joinedTable, nextQueriedTable) == 0) {
        joinChain->len += 1;
        joinChain->chain = realloc(joinChain->chain, sizeof(char **) * (joinChain->len + 1));
        joinChain->chain[joinChain->len - 1] = nextJoinedTable;
        joinChain->chain[joinChain->len] = 0;
        *(joins + 1) = NULL;
      }

      nextLen -= 1;
    }

    joins += 1;
    len -= 1;
  }

  // optimize paths
  DatabaseJoinChain **chains = joinChains->chains;
  while (*chains) {
    char **paths = (*chains)->chain;
    while (*paths) {
      if (!Database_hasAnyField(*paths, fields, fieldsSize)) {
        char **write = paths, **read = paths;
        read += 1;
        while (*write) {
          *write = *read;
          if (*(read + 1) == NULL) *read = NULL;
          read += 1;
          write += 1;
        }
      }
      paths += 1;
    }
    chains += 1;
  }


  free(release);

  return joinChains;
}

static void Database_freeChains(DatabaseJoinChains *chains) {
  if (!chains) return;
  DatabaseJoinChain **ptr = chains->chains;
  while (ptr && *ptr) {
    free((*ptr)->chain);
    free(*ptr);
    ptr += 1;
  }
  free(chains->chains);
  free(chains);
}

static void
Database_setObjectValue(
    JSON *object,
    const char *currentFieldAs,
    char *value,
    DatabaseQueryField *field
) {
  switch (field->jsonType) {
    case JSON_NUMBER: {
      wchar_t *fieldName = cstr2wcstr(currentFieldAs);
      JSON_set(object, fieldName, JSON_number((float) atof(value)));
      free(fieldName);
      break;
    }
    default: {
      wchar_t *fieldName = cstr2wcstr(currentFieldAs);
      JSON_set(object, fieldName, JSON_string(value));
      free(fieldName);
    }
  }
}

static JSON *
Database_findCollection(JSON *root, const char *name) {
  if (root == NULL) {
    kore_log(LOG_CRIT, "Attempt to looking in NULL, terminating!");
    exit(1);
  }
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

static char
Database_isChainContains(
    const char *lastTableName,
    const char *name,
    const DatabaseJoinChains *joinChains
) {
  if (lastTableName == NULL)
    return 1;

  DatabaseJoinChain **chains = joinChains->chains;
  DatabaseJoinChain *found = NULL;
  while (*chains && found == NULL) {
    char **paths = (*chains)->chain;
    while (*paths && found == NULL) {
      if (strcmp(*paths, lastTableName) == 0) found = *chains;
      else paths += 1;
    }
    chains += 1;
  }

  if (found == NULL)
    return 0;
  char **paths = found->chain;
  while (*paths) {
    if (strcmp(*paths, name) == 0) return 1;
    else paths += 1;
  }
  return 0;
}

static JSON *
Database_findCurrent(JSON *array, const char *objectId) {
  if (array == NULL) {
    kore_log(LOG_CRIT, "Database_findCurrent in collection failed, collection is NULL");
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
    if (*fields) {
      switch ((*fields)->type) {
        case JSON_NUMBER: {
          float id = (float) atof(objectId);
          if ((*fields)->value == id) found = object;
          break;
        }
        default: {
          if ((*fields)->string && strcmp((*fields)->string, objectId) == 0) found = object;
          break;
        }
      }
      if (found) break;
    }
    children += 1;
  }
  return found;
}

unsigned char
Database_hasAnyField(
    const char *name,
    DatabaseQueryField **fields,
    unsigned long fieldsSize
) {
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
Database_nestedSerialization(
    const DatabaseQuery *query,
    DatabaseQueryField **fields,
    struct kore_pgsql *kore_sql
) {
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

  DatabaseJoinChains *joinChains = Database_buildJoinChains(query, fields, size);

  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    const char *lastTableName = NULL;
    JSON *current = root;

    DatabaseQueryField **ptr = fields;
    for (unsigned int fieldIndex = 0; fieldIndex < size; fieldIndex++) {
      currentTableName = (*ptr)->table ? (*ptr)->table->name : NULL;
      currentFieldName = (*ptr)->name;
      currentFieldAs = (*ptr)->as;

      if (!currentTableName) {
        // kore_log(LOG_INFO, "  missing table name, skipping...");
        ptr += 1;
        continue;
      }

      if (!currentFieldName) {
        // kore_log(LOG_INFO, "  missing field name, skipping...");
        ptr += 1;
        continue;
      }

      if (!currentFieldAs) {
        // kore_log(LOG_INFO, "  missing field as, skipping...");
        ptr += 1;
        continue;
      }

      value = kore_pgsql_getvalue(kore_sql, rowIndex, fieldIndex);

      if (!Database_hasAnyField(currentTableName, fields, size)) {
        // kore_log(LOG_INFO, "table has no fields, skipping...");
        ptr += 1;
        continue;
      }

      JSON *old = NULL;

      if (lastTableName == NULL || strcmp(lastTableName, currentTableName) != 0) {
        if (!Database_isChainContains(lastTableName, currentTableName, joinChains))
          current = old;
        array = Database_findCollection(current, currentTableName);
        old = current;
        current = Database_findCurrent(array, value);
        lastTableName = currentTableName;
      }

      if (value == NULL || strlen(value) == 0) {
        // kore_log(LOG_INFO, "value is empty, skipping...");
        if (old != NULL && current == NULL) current = old;
        ptr += 1;
        continue;
      }

      if (current == NULL) {
        JSON_append(array, current = JSON_alloc(JSON_OBJECT));
      }

      Database_setObjectValue(current, currentFieldAs, value, *ptr);
      ptr += 1;
    }
  }

  Database_freeChains(joinChains);

  free(fields);

  return root;
}

static JSON *
Database_flatSerialization(const DatabaseQuery *query, struct kore_pgsql *kore_sql) {
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
      Database_setObjectValue(current, currentFieldAs, value, *ptr);
      ptr += 1;
    }
  }

  return root;
}

static DatabaseQueryField **
Database_orderedFields(const DatabaseQuery *query) {
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

  for (unsigned int tableIndex = 0; tableIndex < tablesSize; tableIndex++) {
    unsigned short int idFound = 0;
    const char *tableName = tables[tableIndex];
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

JSON __attribute__((__used__)) *
Database_execQuery(DatabaseQuery *query) {
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

JSON __attribute__((__used__)) *
Database_execSql(
    const char *rawSQL,
    const int fieldsCount,
    const char **fields,
    const int argc,
    const char **argv
) {
  JSON *array = JSON_alloc(JSON_ARRAY);
  struct kore_pgsql kore_sql;

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
    int rows = kore_pgsql_ntuples(&kore_sql);

    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      JSON *child = JSON_alloc(JSON_OBJECT);
      JSON_append(array, child);
      for (int fieldIndex = 0; fieldIndex < fieldsCount; fieldIndex++) {
        const char *field = fields[fieldIndex];
        wchar_t *fieldName = cstr2wcstr(field);
        char *value = kore_pgsql_getvalue(&kore_sql, rowIndex, fieldIndex);
        JSON_set(child, fieldName, JSON_string(value));
        free(fieldName);
      }
    }
  }

  kore_pgsql_cleanup(&kore_sql);
  return array;
}
