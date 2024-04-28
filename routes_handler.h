
#ifndef ROUTES_HANDLER_H
#define ROUTES_HANDLER_H

#include "server.h"

typedef void (*request_handler)(request *req);
typedef struct route_entry {
  char *pattern;
  enum HTTP_VERBS verb;
  request_handler handler;
  struct route_entry *next;
} route_entry;

typedef struct route_hashtable {
  int size;
  route_entry **table;
} route_table;

unsigned long hash_rt(char *uri);
route_table *create_route_table(int size);
void add_route(route_table *table, char *pattern, request_handler handler,
               enum HTTP_VERBS verb);
route_entry *get_route(route_table *table, char *pattern, enum HTTP_VERBS verb);
void free_route_table(route_table *rt);

#endif // ROUTES_HANDLER_H
