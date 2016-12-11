# Kore Query

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

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
DatabaseQuery_select(query, "posts", "id", "id");
DatabaseQuery_select(query, "posts", "title", "title");
DatabaseQuery_select(query, "posts", "content", "content");
DatabaseQuery_select(query, "posts", "created_at", "posted_at");
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
char *json = JSON_stringify(root); //=> {"users":[{},{}]}
JSON_free(root);
free(json);
```
