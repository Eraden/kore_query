#include <kore_query/strings.h>
#include <kore_query/json.h>

JSON *JSON_alloc(JSONType type) {
  JSON *json = calloc(sizeof(JSON), 1);
  json->jsonValue.string = NULL;
  json->type = type;
  json->children.len = 0;
  json->children.keys = NULL;
  json->children.objects = NULL;
  json->array.len = 0;
  json->array.objects = NULL;
  json->parent = NULL;
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
  if (object->array.objects) free(object->array.objects);

  children = object->children.objects;
  keys = object->children.keys;
  while (children && *children) {
    JSON_free(*children);
    children += 1;
    free(*keys);
    keys += 1;
  }
  if (keys) free(object->children.keys);
  if (children) free(object->children.objects);

  char *str = JSON_AS_STRING(object);
  if ( str && object->type == JSON_STRING)
    free( str );

  free(object);
}

JSON *JSON_set(JSON *parent, wchar_t *key, JSON *json) {
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
  json->parent = parent;
  return json;
}

JSON *JSON_append(JSON *array, JSON *entry) {
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
  entry->parent = array;
  return entry;
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
      buffer = clone_cstr("\"");
      char *str = JSON_AS_STRING(root);
      char *escaped = JSON_escape(str);
      if (escaped) buffer = append_cstr(buffer, escaped);
      buffer = append_cstr(buffer, "\"");
      if (escaped) free(escaped);
      break;
    }
    case JSON_NUMBER: {
      buffer = calloc(sizeof(char), 24);
      sprintf(buffer, "%3.2f", JSON_AS_NUMBER(root));
      char *ptr = buffer;
      while (*ptr) {
        if (*ptr == ',') *ptr = '.';
        ptr += 1;
      }
      break;
    }
    case JSON_UNDEFINED: {
      break;
    }
    case JSON_NULL: {
      return clone_cstr("null");
    }
  }
  return buffer;
}

JSON *JSON_string(char *str) {
  JSON *json = JSON_alloc(JSON_STRING);
  JSON_AS_STRING(json) = clone_cstr(str);
  return json;
}

JSON *JSON_number(float f) {
  JSON *json = JSON_alloc(JSON_NUMBER);
  JSON_AS_NUMBER(json) = f;
  return json;
}

JSON *JSON_find(JSON *source, JSONPath *path) {
  if (!source) return NULL;
  if (!path) return source;

  JSONPath *ptr = path;
  JSON *currentSource = source;

  while (ptr->type != JSON_UNDEFINED) {
    switch (ptr->type) {
      case JSON_NUMBER: {
        if (currentSource->type != JSON_ARRAY) {
          return NULL;
        }
        unsigned int index = ptr->index;
        if (currentSource->array.len <= index) {
          return NULL;
        }
        currentSource = currentSource->array.objects[index];
        break;
      }
      case JSON_STRING: {
        if (currentSource->type != JSON_OBJECT) {
          return NULL;
        }
        const char *key = ptr->name;
        unsigned int index = 0;
        char **keys = currentSource->children.keys;
        if (!keys) {
          return NULL;
        }
        JSON *found = NULL;
        while (*keys) {
          if (strcmp(*keys, key) == 0) {
            found = currentSource = currentSource->children.objects[index];
            break;
          }
          index += 1;
          keys += 1;
        }
        if (found == NULL) {
          return NULL;
        }
        break;
      }
      default: {
        break;
      }
    }
    ptr += 1;
  }

  if (!currentSource) {
    return NULL;
  } else {
    return currentSource;
  }
}

JSON *JSON_clone(JSON *obj, JSONCloneType deep) {
  if (!obj) return NULL;
  JSON *clone = JSON_alloc(obj->type);
  switch (clone->type) {
    case JSON_NUMBER:
      JSON_AS_NUMBER(clone) = JSON_AS_NUMBER(obj);
      break;
    case JSON_STRING:
      JSON_AS_STRING(clone) = clone_cstr(JSON_AS_STRING(obj));
      break;
    case JSON_OBJECT: {
      if (deep == JSON_DEEP) {
        JSON **children = obj->children.objects;
        char **keys = obj->children.keys;
        while (children && *children) {
          wchar_t *w = cstr2wcstr(*keys);
          JSON_set(clone, w, JSON_clone(*children, deep));
          free(w);
          children += 1;
          keys += 1;
        }
      }
      break;
    }
    case JSON_ARRAY: {
      if (deep == JSON_DEEP) {
        JSON **children = obj->array.objects;
        while (children && *children) {
          JSON_append(clone, JSON_clone(*children, deep));
          children += 1;
        }
      }
      break;
    }
    default:
      break;
  }
  return clone;
}

int JSON_mergeJSON(JSON *target, JSON *source, JSONCloneType deep) {
  if (target->type != source->type) return 0;
  switch (target->type) {
    case JSON_ARRAY: {
      JSON **children = source->array.objects;
      while (children && *children) {
        switch ((*children)->type) {
          case JSON_OBJECT:
          case JSON_ARRAY: {
            JSON *child = deep ? JSON_clone(*children, deep) : JSON_alloc((*children)->type);
            JSON_append(target, child);
            break;
          }
          default: {
            JSON_append(target, JSON_clone(*children, deep));
            break;
          }
        }
        children += 1;
      }
      break;
    }
    case JSON_OBJECT: {
      JSON **children = source->children.objects;
      char **keys = source->children.keys;
      while (children && *children) {
        wchar_t *w = cstr2wcstr(*keys);
        switch ((*children)->type) {
          case JSON_OBJECT:
          case JSON_ARRAY: {
            JSON *child = deep ? JSON_clone(*children, deep) : JSON_alloc((*children)->type);
            JSON_set(target, w, child);
            break;
          }
          default: {
            JSON_set(target, w, JSON_clone(*children, deep));
            break;
          }
        }
        free(w);
        children += 1;
        keys += 1;
      }
      break;
    }
    case JSON_STRING: {
      JSON_AS_STRING(target) = append_cstr(JSON_AS_STRING(target), JSON_AS_STRING(source));
      break;
    }
    case JSON_NUMBER: {
      JSON_AS_NUMBER(target) = JSON_AS_NUMBER(source);
      break;
    }
    default:
      return 0;
  }

  return 1;
}

JSONValue JSON_valueOf(JSON *obj, char *key) {
  JSON_EACH_PAIR(obj, fieldName, field)
        if (strcmp(key, fieldName) == 0)
          return field->jsonValue;
        JSON_EACH_PAIR_NEXT
  JSON_END_EACH
  JSONValue blank;
  blank.value = 0;
  return blank;
}

char JSON_hasProperty(JSON *obj, const char *key) {
  JSON_EACH_PAIR(obj, fieldName, fieldObject)
        if (strcmp(key, fieldName) == 0)
          return 1;
        JSON_EACH_PAIR_NEXT
  JSON_END_EACH
  return 0;
}

char JSON_renameNode(JSON *root, const char *name, JSONPath *path) {
  JSON *node = JSON_find(root, path);
  if (node == NULL) return 0;
  JSON *parent = node->parent;
  char *key = NULL;
  while ((*path).type != JSON_UNDEFINED) {
    key = path->name;
    path += 1;
  }
  char **keys = parent->children.keys;
  while (keys && *keys && strcmp(*keys, key) != 0) {
    keys += 1;
  }
  if (keys == NULL || *keys == NULL) return 0;
  free(*keys);
  *keys = clone_cstr(name);
  return 1;
}
