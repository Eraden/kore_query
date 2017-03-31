#include <kore_query/json.h>
#include <kore_query/database_exec.h>

#ifdef TEST_KORE_QUERY
static void print_joins(DatabaseJoinChains *joinChains) {
  fprintf(stderr, "\n");
  DatabaseJoinChain **chainsPtr = joinChains->chains;
  while (chainsPtr && *chainsPtr) {
    char **tables = (*chainsPtr)->chain;
    fprintf(stderr, "| ");
    while (tables && *tables) {
      fprintf(stderr, " %-20s |", *tables);
      tables += 1;
    }
    fprintf(stderr, "\n");
    chainsPtr += 1;
  }
  fprintf(stderr, "\n");
}
#define INSPECT_CHAINS(joinChains) print_joins(joinChains);
#define DEBUG_INFO(str) kore_log(LOG_INFO, str);
#else
#define INSPECT_CHAINS(joinChains)
#define DEBUG_INFO(str)
#endif

#ifdef INSPECT_SQL
#define PRINT_SQL(sql) fprintf(stderr, "SQL: %s\n", sql)
#else
#define PRINT_SQL(sql)
#endif

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

static char **Database_chainPaths(
    const DatabaseJoinChains *joinChains,
    const char *name
);

static void Database_appendJoinToJoins(DatabaseJoinChains *joinChains, DatabaseJoinChain *joinChain) {
  if (joinChains->chains == NULL) {
    joinChains->len = 1;
    joinChains->chains = calloc(sizeof(DatabaseJoinChain **), joinChains->len + 1);
  } else {
    joinChains->len += 1;
    joinChains->chains = realloc(joinChains->chains, sizeof(DatabaseJoinChain **) * (joinChains->len + 1));
    joinChains->chains[joinChains->len] = 0;
  }
  joinChains->chains[joinChains->len - 1] = joinChain;
}

static void Database_appendTableToJoin(DatabaseJoinChain *joinChain, char *tableName) {
  if (joinChain->len == 0) {
    joinChain->len = 1;
    joinChain->chain = calloc(sizeof(char **), joinChain->len + 1);
    joinChain->chain[joinChain->len - 1] = tableName;
  } else {
    joinChain->len += 1;
    joinChain->chain = realloc(joinChain->chain, sizeof(char **) * (joinChain->len + 1));
    joinChain->chain[joinChain->len - 1] = tableName;
  }
  joinChain->chain[joinChain->len] = 0;
}

static DatabaseJoinChain *DatabaseJoinChain_clone(DatabaseJoinChain *join) {
  DatabaseJoinChain *clone = calloc(sizeof(DatabaseJoinChain), 1);
  clone->len = 0;
  clone->chain = NULL;

  char **tables = join->chain;
  size_t times = join->len - 1;
  while (times) {
    Database_appendTableToJoin(clone, *tables);
    tables += 1;
    times -= 1;
  }
  return clone;
}

static int
Database_appendTableToEveryTableChain(
    DatabaseJoinChains *joinChains,
    char *toFind,
    char *toAdd
) {
  if (!toFind) return 0;
  if (!toAdd) return 0;

  int found = 0;
  const size_t chainsSize = joinChains->len;
  size_t chainsIndex = 0;
  // Check already if added?
  while (chainsIndex < chainsSize) {
    DatabaseJoinChain *join = *(joinChains->chains + chainsIndex);
    const size_t chainSize = join->len;
    size_t chainIndex = 0;
    while (chainIndex < chainSize) {
      if (
          strcmp(join->chain[chainIndex], toFind) == 0 &&
          join->chain[chainIndex + 1] != NULL &&
          strcmp(join->chain[chainIndex + 1], toAdd) == 0
          ) {
        return 1;
      }
      chainIndex += 1;
    }
    chainsIndex += 1;
  }

  // Append
  chainsIndex = 0;
  while (chainsIndex < chainsSize) {
    DatabaseJoinChain *join = *(joinChains->chains + chainsIndex);
    const size_t chainSize = join->len;
    size_t chainIndex = 0;
    while (chainIndex < chainSize) {
      if (strcmp(join->chain[chainIndex], toFind) == 0) {
        found = 1;

        if (chainIndex + 1 == chainSize) {
          Database_appendTableToJoin(join, toAdd);
        } else {
          DatabaseJoinChain *clone = DatabaseJoinChain_clone(join);
          Database_appendJoinToJoins(joinChains, clone);
          Database_appendTableToJoin(clone, toAdd);
          chainsIndex = chainsSize - 1;
          found = 1;
        }
        break;
      }
      chainIndex += 1;
    }
    chainsIndex += 1;
  }
  return found;
}

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
    if (*(joins) == NULL) {
      len -= 1;
      continue;
    }
    DatabaseQueryCondition *condition = *join->conditions;

    char *queriedTable = condition->otherField->table->name;
    char *joinedTable = condition->field->table->name;
    if (len == query->joinsSize) {
      DatabaseJoinChain *joinChain = calloc(sizeof(DatabaseJoinChain), 1);
      joinChain->len = 0;
      joinChain->chain = NULL;
      Database_appendJoinToJoins(joinChains, joinChain);
      Database_appendTableToJoin(joinChain, queriedTable);
      Database_appendTableToJoin(joinChain, joinedTable);
    } else if (!Database_appendTableToEveryTableChain(joinChains, queriedTable, joinedTable)) {
      len -= 1;
      if (len == 0) break;
      joins += 1;
      continue;
    }
    unsigned int nextLen = len - 1;
    DatabaseQueryJoin **nextJoins = joins + 1;

    while (nextLen) {
      if (*(joins + 1) == NULL) {
        nextLen -= 1;
        continue;
      }
      DatabaseQueryJoin *nextJoin = *nextJoins;
      DatabaseQueryCondition *nextCondition = *nextJoin->conditions;

      char *nextQueriedTable = nextCondition->otherField->table->name;
      char *nextJoinedTable = nextCondition->field->table->name;
      if (strcmp(joinedTable, nextQueriedTable) == 0) {
        Database_appendTableToEveryTableChain(joinChains, nextQueriedTable, nextJoinedTable);
      }

      nextLen -= 1;
      nextJoins += 1;
    }
    len -= 1;
    if (len == 0) break;
    joins += 1;
  }
  // optimize paths
  DatabaseJoinChain **chains = joinChains->chains;
  while (chains && *chains) {
    char **paths = (*chains)->chain;
    while (paths && *paths) {
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
  if (!JSON_hasProperty(object, currentFieldAs)) {
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
}

static JSON *
Database_findCollection(JSON *root, const char *name) {
  if (root == NULL) {
    kore_log(LOG_CRIT, "Attempt to looking in NULL, terminating!");
    exit(1);
  }
  if (name == NULL) {
    kore_log(LOG_CRIT, "name is NULL, terminating!");
    exit(1);
  }
  JSON **children = root->children.objects;
  char **keys = root->children.keys;
  JSON *object = NULL;

  while (children && *children) {
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

static char **
Database_chainPaths(
    const DatabaseJoinChains *joinChains,
    const char *name
) {
  DatabaseJoinChain **chains = joinChains->chains;
  DatabaseJoinChain *found = NULL;
  while (*chains && found == NULL) {
    char **paths = (*chains)->chain;
    while (*paths && found == NULL) {
      if (strcmp(*paths, name) == 0) found = *chains;
      else paths += 1;
    }
    chains += 1;
  }

  if (found == NULL)
    return NULL;
  return found->chain;
}

static char
Database_isChainContains(
    const char *lastTableName,
    const char *name,
    const DatabaseJoinChains *joinChains
) {
  if (lastTableName == NULL)
    return 1;

  char **paths = Database_chainPaths(joinChains, lastTableName);
  if (!paths)
    return 1;

  while (*paths) {
    if (strcmp(*paths, name) == 0) return 1;
    else paths += 1;
  }
  return 0;
}

static JSON *
Database_findLastMatchingCollection(
    JSON *root,
    const DatabaseJoinChains *joinChains,
    const char *currentTableName
) {
  JSON *current = root;
  char **paths = Database_chainPaths(joinChains, currentTableName);
  while (paths && *paths) {
    if (strcmp(*paths, currentTableName) == 0) {
      break;
    }
    JSON *collection = Database_findCollection(current, *paths);
    if (*(paths + 1) && collection->array.objects) {
      current = collection->array.objects[collection->array.len - 1];
    } else {
      current = collection;
    }
    paths += 1;
  }

  return current;
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
          if (JSON_AS_NUMBER((*fields)) == id) found = object;
          break;
        }
        default: {
          if (JSON_AS_STRING((*fields)) && strcmp(JSON_AS_STRING((*fields)), objectId) == 0) found = object;
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
  DEBUG_INFO("Database_nestedSerialization");
  int rows = kore_pgsql_ntuples(kore_sql);
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
        ptr += 1;
        continue;
      }
      if (!currentFieldName) {
        ptr += 1;
        continue;
      }

      if (!currentFieldAs) {
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
        if (!Database_isChainContains(lastTableName, currentTableName, joinChains)) {
          current = Database_findLastMatchingCollection(root, joinChains, currentTableName);
        }

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
  DEBUG_INFO("Database_flatSerialization");
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
  DEBUG_INFO("Database_orderedFields")
  DatabaseQueryField **fields = query->fields ? query->fields : query->returning;
  if (!fields) return NULL;

  unsigned int fieldsSize = query->fieldsSize ? query->fieldsSize : query->returningSize;
  unsigned int tablesSize = 1;

  if (DB_QUERY_HAS_NO_JOINS(query)) {
    DatabaseQueryField **ordered = calloc(sizeof(DatabaseQueryField *), fieldsSize);
    memcpy(ordered, fields, sizeof(DatabaseQueryField *) * fieldsSize);
    return ordered;
//    return NULL;
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
Database_execQuery(const DatabaseQuery *query) {
  char *sql = DatabaseQuery_stringify(query);
  JSON *result = JSON_alloc(JSON_OBJECT);
  struct kore_pgsql kore_sql;

  if (!kore_pgsql_query_init(&kore_sql, NULL, "db", KORE_PGSQL_SYNC)) {
    kore_pgsql_logerror(&kore_sql);
    kore_pgsql_cleanup(&kore_sql);
    return result;
  }

  PRINT_SQL(sql);
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
