#include "json.h"
#include "./strings.h"

JSON *JSON_alloc(JSONType type) {
  JSON *json = calloc(sizeof(JSON), 1);
  json->string = NULL;
  json->type = type;
  json->children.len = 0;
  json->children.keys = NULL;
  json->children.objects = NULL;
  json->array.len = 0;
  json->array.objects = NULL;
  return json;
}

void JSON_free(JSON *object) {
  JSON **children = NULL;
  char **keys = NULL;

  children = object->array.objects;
  while (children && *children) {
    JSON_free(*children);
    children += 1;
  }

  children = object->children.objects;
  keys = object->children.keys;
  while (children && *children) {
    JSON_free(*children);
    children += 1;
    free(*keys);
    keys += 1;
  }

  if (object->string && object->type == JSON_STRING)
    free(object->string);

  free(object);
}

void JSON_set(JSON *parent, wchar_t *key, JSON *json) {
  if (parent->children.len == 0) {
    parent->children.len = 1;
    parent->children.objects = calloc(sizeof(JSON *), 2);
    parent->children.objects[0] = json;
    parent->children.objects[1] = 0;
    parent->children.keys = calloc(sizeof(char *), 2);
    parent->children.keys[0] = wcstr2cstr(key);
    parent->children.keys[1] = 0;
  } else {
    parent->children.len += 1;
    parent->children.keys = realloc(parent->children.keys, sizeof(char *) * (parent->children.len + 1));
    parent->children.keys[parent->children.len - 1] = wcstr2cstr(key);
    parent->children.keys[parent->children.len] = 0;
    parent->children.objects = realloc(parent->children.objects, sizeof(JSON *) * (parent->children.len + 1));
    parent->children.objects[parent->children.len - 1] = json;
    parent->children.objects[parent->children.len] = 0;
  }
}

void JSON_append(JSON *array, JSON *entry) {
  if (array->array.len == 0) {
    array->array.len = 1;
    array->array.objects = calloc(sizeof(JSON *), 2);
    array->array.objects[0] = entry;
    array->array.objects[1] = 0;
  } else {
    array->array.len += 1;
    array->array.objects = realloc(array->array.objects, sizeof(JSON *) * (array->array.len + 1));
    array->array.objects[array->array.len - 1] = entry;
    array->array.objects[array->array.len] = 0;
  }
}
char *JSON_escape(char *string) {
  if (string == NULL) return NULL;
  size_t index = 0, len = 0;

  char *ptr = string;
  while (*ptr) {
    switch (*ptr) {
      case '\n':
      case '\"': {
        len += 2;
        break;
      }
      default: {
        len += 1;
        break;
      }
    }
    ptr += 1;
  }
  char *buffer = calloc(sizeof(char), len + 1);

  ptr = string;
  while (*ptr) {
    switch (*ptr) {
      case '\n': {
        buffer[index] = '\\';
        index += 1;
        buffer[index] = 'n';
        break;
      }
      case '\"': {
        buffer[index] = '\\';
        index += 1;
        buffer[index] = *ptr;
        break;
      }
      default: {
        buffer[index] = *ptr;
        break;
      }
    }
    ptr += 1;
    index += 1;
  }
  return buffer;
}

char *JSON_stringify(JSON *root) {
  if (root == NULL) return NULL;

  char *buffer = NULL;
  switch (root->type) {
    case JSON_OBJECT: {
      buffer = calloc(sizeof(char), 2);
      buffer[0] = '{';
      JSON **children = root->children.objects;
      char **keys = root->children.keys;
      while (children && keys && *children && *keys) {
        char *part = JSON_stringify(*children);
        char *key = *keys;
        buffer = append_cstr(buffer, "\"");
        buffer = append_cstr(buffer, key);
        buffer = append_cstr(buffer, "\":");
        buffer = append_cstr(buffer, part);
        if (*(children + 1)) buffer = append_cstr(buffer, ",");
        free(part);

        children += 1;
        keys += 1;
      }
      buffer = append_cstr(buffer, "}");
      break;
    }
    case JSON_ARRAY: {
      buffer = calloc(sizeof(char), 2);
      buffer[0] = '[';
      JSON **children = root->array.objects;
      while (children && *children) {
        char *part = JSON_stringify(*children);
        buffer = append_cstr(buffer, part);
        if (*(children + 1)) buffer = append_cstr(buffer, ",");
        free(part);
        children += 1;
      }
      buffer = append_cstr(buffer, "]");
      break;
    }
    case JSON_STRING: {
      buffer = calloc(sizeof(char), strlen(root->string) + 3);
      strcat(buffer, "\"");
      char *escaped = JSON_escape(root->string);
      strcat(buffer, escaped);
      strcat(buffer, "\"");
      break;
    }
    case JSON_NUMBER: {
      buffer = calloc(sizeof(char), 24);
      sprintf(buffer, "%0.2f", root->value);
      break;
    }
  }
  return buffer;
}

JSON *JSON_string(char *str) {
  JSON *json = JSON_alloc(JSON_STRING);
  json->string = clone_cstr(str);
  return json;
}

JSON *JSON_wstring(wchar_t *str) {
  JSON *json = JSON_alloc(JSON_STRING);
  json->string = wcstr2cstr(str);
  return json;
}

JSON *JSON_number(float f) {
  JSON *json = JSON_alloc(JSON_NUMBER);
  json->value = f;
  return json;
}

JSON *JSON_at_path(JSON *root, char **path) {
  JSON *found = NULL;
  JSON *current = root;
  while (*path) {
    JSON **children = current->children.objects;
    char **keys = current->children.keys;

    JSON *foundChild = NULL;
    while (*keys) {
      if (strcmp(*keys, *path) == 0) {
        foundChild = current = *children;
        break;
      }
      keys += 1;
      children += 1;
    }

    if ((path + 1) == NULL) found = foundChild;

    path += 1;
  }
  return found;
}
