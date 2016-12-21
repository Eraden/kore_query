# Kore Query

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./License.md)

## Using

```bash
cd src
git submodule add git@github.com:Eraden/kore_query.git
```

## Kore Database Query

### Building `insert`

```cpp
DatabaseQuery *query = DatabaseQuery_startInsert("posts");
DatabaseQuery_insert(query, "title", "O'Connor Memory", JSON_STRING);
DatabaseQuery_insert(query, "content", "Nothing special", JSON_STRING);
char *sql = DatabaseQuery_stringify(query);
```

Result:

```sql
INSERT INTO posts (title, content) VALUES ('O''Connor Memory', 'Nothing special')
```

### Building `select`

```cpp
DatabaseQuery *query = DatabaseQuery_startSelect("posts");
DatabaseQuery_select(query, "posts", "id", "id", JSON_NUMBER);
DatabaseQuery_select(query, "posts", "title", "title", JSON_STRING);
DatabaseQuery_select(query, "posts", "content", "content", JSON_STRING);
DatabaseQuery_select(query, "posts", "created_at", "posted_at", JSON_STRING);
DatabaseQuery_whereField(query, "title", "LIKE", "%hello%", JSON_STRING);
char *sql = DatabaseQuery_stringify(query);
```

Result:

```sql
SELECT posts.id AS id, posts.title AS title, posts.content AS content, posts.created_at AS since_at FROM posts WHERE title LIKE '%hello%'
```

### Building `update`

```cpp
DatabaseQuery *query = DatabaseQuery_startUpdate("posts");
DatabaseQuery_update(query, "title", "O'Connor Memory", JSON_STRING);
DatabaseQuery_update(query, "content", "Nothing special", JSON_STRING);
char *sql = DatabaseQuery_stringify(query);
```

Result:

```sql
UPDATE posts SET title = 'O''Connor Memory', content = 'Nothing special'
```


### Building `delete`

```cpp
DatabaseQuery *query = DatabaseQuery_startDelete("posts");
DatabaseQuery_whereField(query, "id", "=", 10, JSON_NUMBER);
char *sql = DatabaseQuery_stringify(query);
```

Result:

```sql
DELETE posts WHERE id = 10
```

### Execute Query

Good only for websockets!

```cpp
Database_execQuery(query); // sync
```

## JSON

Only for serialization purpose

```cpp
JSON *root = JSON_alloc(JSON_OBJECT);
JSON *array = JSON_alloc(JSON_ARRAY);
JSON_set(root, L"users", array);

for (int i = 0; i < 2; i++) {
  JSON *child = JSON_alloc(JSON_OBJECT);
  JSON_append(array, child);
}

JSON_EACH_PAIR(root, rootKey, rootChild)
    if (strcmp(rootKey, "some-key") == 0) /* do something */;
    JSON_EACH_PAIR_NEXT
JSON_END_EACH

JSON_EACH(array, arrayIndex, arrayEntry)
    if (arrayIndex != 0) kore_log(LOG_INFO, ", ");
    JSON_EACH_NEXT
JSON_END_EACH

JSONPath path[3] = {
    { .type=JSON_STRING, .name="users" },
    { .type=JSON_NUMBER, .index=1 },
    { .type=JSON_UNDEFINED, .name=NULL },
};
JSON *child = JSON_find(source, path);

char *json = JSON_stringify(root); //=> {"users":[{},{}]}
JSON_free(root);
free(json);
```

## MOCK

If you wish to test your stuff you can use my mockup but be aware it's not greatest stuff in the world.

You just need to define `TEST_KORE_QUERY` macro and `http`, `kore` and many other things will be replaced by empty or
fixed code.

```cmake
SET(TEST_KORE_QUERY, '1')
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST_KORE_QUERY")
```

```c
#include "./src/kore_query/kore_mockup.h"

int main() {
    kore__main();
    /// ... your code
    kore__terminate();
    return 0;
}
```

There is no multi-threads here and database is a little bit override (connection is cleared after every query).

Things you can test:

* Database queries
* Your code

Also please consider adding memory sanitizer:

```cmake
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer")
```

DO NOT add this library code as cmake subdirectory! It has own test database and code too run.
There is no reason to test things which does not belongs to you.
