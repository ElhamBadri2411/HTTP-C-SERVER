
#include "routes_handler.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// djb2 hash algorithm
// http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_rt(char *uri) {
  unsigned long hash = 5381;
  int c;

  while ((c = *uri++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}
route_table *create_route_table(int size) {
  route_table *rt = malloc(sizeof(route_table));
  if (rt == NULL) {
    return NULL;
  }

  rt->size = size;
  rt->table = calloc(size, sizeof(route_entry));
  if (rt->table == NULL) {
    return NULL;
  }

  return rt;
}
void add_route(route_table *table, char *pattern, request_handler handler,
               enum HTTP_VERBS verb) {
  unsigned long index = hash_rt(pattern) % table->size;

  route_entry *re = malloc(sizeof(route_entry));
  if (re == NULL) {
    return;
  }
  re->handler = handler;
  re->verb = verb;
  re->pattern = strdup(pattern);

  re->next = *(table->table + index);
  *(table->table + index) = re;
}
route_entry *get_route(route_table *table, char *pattern,
                       enum HTTP_VERBS verb) {
  unsigned long index = hash_rt(pattern) % table->size;

  route_entry *re_bucket = *(table->table + index);

  while (re_bucket != NULL) {
    if (strcmp(re_bucket->pattern, pattern) == 0 && re_bucket->verb == verb) {
      return re_bucket;
    }
    re_bucket = re_bucket->next;
  }

  return NULL;
}
