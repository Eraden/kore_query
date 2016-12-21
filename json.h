/**
 * @file json.h
 * @author Adrian Eraden Woźniak
 * @date 21.12.2016
 * @brief JSON response type
 * @example
 * @code
 *  JSON *root = JSON_alloc(JSON_OBJECT);
 *  JSON *array = JSON_alloc(JSON_ARRAY);
 *  JSON_set(root, L"users", array);
 *
 *  for (int i = 0; i < 2; i++) {
 *    JSON *child = JSON_alloc(JSON_OBJECT);
 *    JSON_append(array, child);
 *  }
 *
 *  JSON_EACH_PAIR(root, rootKey, rootChild)
 *      if (strcmp(rootKey, "some-key") == 0) ...;
 *      JSON_EACH_PAIR_NEXT
 *  JSON_END_EACH
 *
 *  JSON_EACH(array, arrayIndex, arrayEntry)
 *      if (arrayIndex != 0) kore_log(LOG_INFO, ", ");
 *      JSON_EACH_NEXT
 *  JSON_END_EACH
 *
 *  JSONPath path[3] = {
 *      { .type=JSON_STRING, .name="users" },
 *      { .type=JSON_NUMBER, .index=1 },
 *      { .type=JSON_UNDEFINED, .name=NULL },
 *  };
 *  JSON *child = JSON_find(source, path);
 *
 *  char *json = JSON_stringify(root); //=> {"users":[{},{}]}
 *  JSON_free(root);
 *  free(json);
 * @endcode
 */

#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "strings.h"

typedef struct sJSON JSON;
typedef struct sJSONPath JSONPath;
typedef enum eJSONType JSONType;
typedef union eJSONValue JSONValue;

#define JSON_EACH_PAIR(expr, ENTRY_KEY, ENTRY_NAME) { \
  JSON *object = expr; \
  if (object) { \
    JSON **childrenPtr = object->children.objects; \
    char **keysPtr = object->children.keys; \
    for (unsigned int childIndex = 0; childIndex < object->children.len; childIndex ++) { \
      JSON *ENTRY_NAME = *childrenPtr; \
      char *ENTRY_KEY = *keysPtr; \

#define JSON_EACH_PAIR_NEXT \
      childrenPtr += 1; \
      keysPtr += 1;

#define JSON_EACH(expr, ENTRY_INDEX, ENTRY_NAME) { \
  JSON *array = expr; \
  if (array) { \
    JSON **childrenPtr = array->array.objects; \
    for (unsigned int ENTRY_INDEX = 0; ENTRY_INDEX < array->array.len; ENTRY_INDEX ++) { \
      JSON *ENTRY_NAME = *childrenPtr;

#define JSON_EACH_NEXT childrenPtr += 1;

#define JSON_END_EACH } } }

#define JSON_AS_NUMBER(obj) obj->jsonValue.value
#define JSON_AS_STRING(obj) obj->jsonValue.string

/**
 * JSON value variants
 */
typedef union eJSONValue {
  char *string;
  float value;
} JSONValue;

/**
 * Type of data stored by JSON
 */
typedef enum eJSONType {
  JSON_UNDEFINED = 0,
  JSON_STRING = 1,
  JSON_NUMBER = 1 << 1,
  JSON_OBJECT = 1 << 2,
  JSON_ARRAY = 1 << 3,
  JSON_NULL = 1 << 4,
} JSONType;

/**
 * How JSON should be cloned
 * Simple indicates that complex object should not clone children
 */
typedef enum eJSONCloneType {
  JSON_SIMPLE = 0,
  JSON_DEEP = 1
} JSONCloneType;

/**
 * JSON data
 */
typedef struct sJSON {
  JSONType type; /** JSON type */
  JSONValue jsonValue; /** stored value */

  struct {
    JSON **objects; /** stored objects */
    char **keys; /** children keys */
    unsigned int len; /** number of children */
  } children; /** object children */

  struct {
    JSON **objects; /** array entities */
    unsigned int len; /** array length */
  } array; /** array data */
} JSON;

/**
 * Path inside json
 *
 * @code{.c}
 *  JSONPath path[3] = {
 *      { .type=JSON_STRING, .name="accounts" }, // for object
 *      { .type=JSON_NUMBER, .index=1 }, // for array
 *      { .type=JSON_UNDEFINED, .name=NULL } // end of path
 *  };
 * @endcode
 *
 * @see JSON_find
 */
typedef struct sJSONPath {
  union {
    char *name;
    unsigned index;
  };
  JSONType type;
} JSONPath;

/**
 * Create new empty json object with given type
 * @param type
 * @return
 */
JSON *JSON_alloc(JSONType type);

/**
 * Create json string from string
 * @param str
 * @return
 */
JSON *JSON_string(char *str);

/**
 * Create json object from float
 * @param n
 * @return
 */
JSON *JSON_number(float n);

/**
 * Free object memory
 * @param object
 */
void JSON_free(JSON *object);

/**
 * Set new value for given key.
 * Multiple values for key can exists and newest value will be at end of keys.
 *
 * @param parent
 * @param key
 * @param json
 * @return
 *
 * @code{.c}
 *  JSON *users = JSON_set(root, L"users", JSON_alloc(JSON_ARRAY));
 * @endcode
 */
JSON *JSON_set(JSON *parent, wchar_t *key, JSON *json);

/**
 * Append object at end of given array
 *
 * @param array
 * @param entry
 * @return
 *
 * @code{.c}
 *  JSON *user = JSON_append(array, JSON_alloc(JSON_OBJECT));
 * @endcode
 */
JSON *JSON_append(JSON *array, JSON *entry);

/**
 * Create string from JSON object
 *
 * @param root
 * @return
 *
 * @code{.c}
 *  char *json = JSON_stringify(root);
 * @endcode
 */
char *JSON_stringify(JSON *root);

/**
 * Lookup for JSON path
 *
 * @param source
 * @param path
 * @return json object or NULL if not found
 *
 * @see JSONPath
 *
 * @code{.c}
 *  JSONPath path[4] = {
 *      { .type=JSON_STRING, .name="hello" },
 *      { .type=JSON_STRING, .name="world" },
 *      { .type=JSON_NUMBER, .index=1 },
 *      { .type=JSON_UNDEFINED, .name=NULL },
 *  };
 *  JSON *child = JSON_find(source, path);
 * @endcode
 */
JSON *JSON_find(JSON *source, JSONPath *path);

/**
 * Escape all special characters
 *
 * @param string
 * @return
 *
 * @code{.c}
 *  char *escaped = JSON_escape("Some paragraph\n Next paragraph");
 *  // #=> "Some paragraph\\n Next paragraph"
 * @endcode
 */
char *JSON_escape(char *string);

/**
 * New object will be created with fallowing rules:
 *
 * @param obj
 * @param deep
 * @return new json object
 *
 * @code{.c}
 *  * object will be cloned with all keys and returned if deep is set to `JSON_DEEP`, otherwise empty object will be returned
 *  * array will be cloned with all children and returned if deep is set to `JSON_DEEP`, otherwise empty array will be returned
 *  * string will be cloned
 *  * value will be preserved
 * @endcode
 */
JSON *JSON_clone(JSON *obj, JSONCloneType deep);

/**
 * Merge two object with fallowing rules:
 *
 * @param target
 * @param source
 * @param deep
 * @return 1 if succeed or 0 if not
 *
 * @code{.c}
 *  * object will be cloned and merged to target if deep is set to `JSON_DEEP`, otherwise empty object will be merged
 *  * array will be cloned and merged to target if deep is set to `JSON_DEEP`, otherwise empty array will be merged
 *  * two string will be concatenated without any separator
 *  * value will be replaced
 * @endcode
 */
int JSON_mergeJSON(JSON *target, JSON *source, JSONCloneType deep);

/**
 * Returns JSONValue for given key if missing empty JSONValue.
 * @param object
 * @param key
 * @return
 */
JSONValue JSON_valueOf(JSON *object, char *key);
