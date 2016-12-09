#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "strings.h"

typedef struct sJSON JSON;
typedef enum eJSONType JSONType;

typedef enum eJSONType {
  JSON_STRING = 1,
  JSON_NUMBER = 1 << 1,
  JSON_OBJECT = 1 << 2,
  JSON_ARRAY = 1 << 3
} JSONType;

typedef struct sJSON {
  JSONType type;

  union {
    char *string;
    float value;
  };

  struct {
    JSON **objects;
    char **keys;
    unsigned int len;
  } children;

  struct {
    JSON **objects;
    unsigned int len;
  } array;
} JSON;

JSON *JSON_alloc(JSONType type);

JSON *JSON_string(char *str);

JSON *JSON_wstring(wchar_t *str);

JSON *JSON_number(float n);

void JSON_free(JSON *object);

void JSON_set(JSON *parent, wchar_t *key, JSON *json);

void JSON_append(JSON *array, JSON *entry);

char *JSON_stringify(JSON *root);

JSON *JSON_at_path(JSON *root, char **path);
