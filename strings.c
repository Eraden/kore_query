#include "./strings.h"

wchar_t *cstr2wcstr(const char *cstr) {
  if (!cstr) return NULL;
  u_long len = strlen(cstr);
  wchar_t *wcstr = calloc(sizeof(wchar_t), len + 1);
  mbstowcs(wcstr, cstr, len);
  return wcstr;
}

char *wcstr2cstr(const wchar_t *wcstr) {
  if (!wcstr) return NULL;
  u_long len = wcslen(wcstr);
  char *cstr = calloc(sizeof(char), len + 1);
  wcstombs(cstr, wcstr, len);
  return cstr;
}

char *clone_cstr(const char *str) {
  if (str == NULL) return NULL;
  char *clone = calloc(sizeof(char), strlen(str) + 1);
  strcpy(clone, str);
  return clone;
}

wchar_t *clone_wstr(const wchar_t *str) {
  if (str == NULL) return NULL;
  wchar_t *clone = calloc(sizeof(wchar_t), wcslen(str) + 1);
  wcscpy(clone, str);
  return clone;
}

char *
__attribute__((malloc))
join_cstr(const char *left, const char *right) {
  unsigned long len = 0;
  if (left) len += strlen(left);
  if (right) len += strlen(right);
  char *joined = calloc(sizeof(char), len + 1);
  if (left) strcat(joined, left);
  if (right) strcat(joined, right);
  return joined;
}

char *append_char_to_cstr(char *token, char c) {
  unsigned long len = 0;
  char *tmp = NULL;
  len = token ? strlen(token) : 0;
  tmp = calloc(sizeof(char), len + 2);
  if (token) {
    strcat(tmp, token);
  }
  tmp[len] = c;
  return tmp;
}

char *append_cstr(char *str, char *part) {
  if (str == NULL && part == NULL) {
    return NULL;
  }
  if (part == NULL) {
    return str;
  }
  if (str == NULL) {
    return JSON_clone_str(part);
  }
  char *res = realloc(str, sizeof(char) * (strlen(str) + strlen(part) + 1));
  if (res == NULL) {
    exit(10);
  } else {
    str = res;
  }
  strcat(str, part);
  return str;
}
