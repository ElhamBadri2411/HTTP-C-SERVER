
#include "json.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_val_from_key(char *key, keyval kv) {
  printf("kv.key = %s, key = %s\n", kv.key, key);
  if (strcmp(kv.key, key) == 0) {
    return kv.value;
  }
  return NULL;
}

char *create_json_string(keyval kv[], int kv_len, int *json_size) {
  int size = 2; // for the {}

  for (int i = 0; i < kv_len; i++) {
    size += strlen(kv[i].key) + strlen(kv[i].value) + 7;
    if (i != kv_len - 1) {
      size += 1; // for ,
    }
  }

  char *json_string = malloc(size + 1);
  if (json_string == NULL) {
    return NULL;
  }

  int offset = 0;

  offset += sprintf(json_string, "{");

  for (int i = 0; i < kv_len; i++) {
    char *end = i == kv_len - 1 ? "}" : ",";
    offset +=
        sprintf(json_string + offset, "%s: %s%s", kv[i].key, kv[i].value, end);
  }

  json_string[offset] = '\0';

  *json_size = offset;
  return json_string;
}

keyval *create_keyvals_from_json_string(char *json_str, int *count) {
  // we need to duplicate the string becuase string literals are stored in read
  // only memory in C
  char *json_string = malloc(strlen(json_str) + 1);
  if (json_string == NULL) {
    return NULL;
  }
  strncpy(json_string, json_str, strlen(json_str) + 1);
  char *curr = json_string; // Pointer to traverse the string
  int num_keyvals = 0;
  keyval *kvs = NULL;

  // Skip initial whitespace and opening brace
  while (isspace(*curr) || *curr == '{')
    curr++;

  while (*curr != '}') {

    char *key_start = curr;
    while (*curr != ':' && *curr != '\0')
      curr++;
    if (*curr != ':')
      return NULL;
    *curr = '\0'; // Terminate the key
    curr++;

    while (isspace(*curr) || *curr == ':')
      curr++;

    char *val_start = curr;
    while (*curr != ',' && *curr != '}')
      curr++;
    if (*curr == '}')
      *(curr + 1) = '}';
    *(curr) = '\0'; // Terminate the value string

    while (isspace(*curr) || *curr == ',')
      curr++;
    curr++;

    // Allocate new keyval and store
    kvs = realloc(kvs, (num_keyvals + 1) * sizeof(keyval));
    kvs[num_keyvals].key = strdup(key_start);
    kvs[num_keyvals].value = strdup(val_start);
    num_keyvals++;
  }

  *count = num_keyvals;
  return kvs;
}

// void free_keyvals(keyval *kv, int count) {
//   keyval *curr = kv;
//   for (int i = 0; i < count; i++) {
//   }
// }
