#ifndef JSON_H
#define JSON_H

typedef struct key_value_pair {
  char *key;
  char *value;
} keyval;

char *get_val_from_key(char *key, keyval kv);
char *create_json_string(keyval kv[], int kv_len);
#endif // JSON_H
