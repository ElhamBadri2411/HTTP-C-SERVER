
#include "json.h"
#include <string.h>

char *get_val_from_key(char *key, keyval kv) {
  if (strcmp(kv.key, key) == 0) {
    return kv.value;
  }
  return NULL;
}
