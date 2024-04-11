
#ifndef ROUTES_HANDLER_H
#define ROUTES_HANDLER_H

#include "server.h"

// request handler structs/types/functions
typedef void (*request_handler)(request *req);
typedef struct route_entry {
  char *pattern;
  request_handler handler;
  struct route_entry *next;
} route_entry;

typedef struct route_hashtable {
  int size;
  route_entry **table;
} route_table;

unsigned long hash_rt(char *uri);
route_table *create_route_table(int size);
void add_route(route_table *table, char *pattern, request_handler handler);
route_entry *get_route(route_table *table, char *pattern);

#endif // ROUTES_HANDLER_H
