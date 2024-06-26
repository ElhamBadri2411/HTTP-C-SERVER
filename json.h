#ifndef JSON_H
#define JSON_H

typedef struct key_value_pair {
  char *key;
  char *value;
} keyval;

char *get_val_from_key(char *key, keyval kv);
char *create_json_string(keyval kv[], int kv_len, int *json_size);
keyval *create_keyvals_from_json_string(char *json_string, int *count);
void free_keyvals(keyval *kv, int count);

#endif // JSON_H
