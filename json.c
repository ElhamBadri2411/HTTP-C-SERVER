
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_val_from_key(char *key, keyval kv) {
  if (strcmp(kv.key, key) == 0) {
    return kv.value;
  }
  return NULL;
}

char *create_json_string(keyval kv[], int kv_len) {
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
    offset += sprintf(json_string + offset, "\"%s\": \"%s\"%s", kv[i].key,
                      kv[i].value, end);
  }

  json_string[offset] = '\0';

  return json_string;
}
