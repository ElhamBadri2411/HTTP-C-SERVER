#include "utils.h"
#include "server.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *lstrip(char *str) {
  while (isspace(*str)) {
    str++;
  }
  return str;
}

char *rstrip(char *str) {
  char *b = str + strlen(str);

  while (isspace(*b))
    b--;

  *(b + 1) = '\0';

  return str;
}

char *strip(char *str) { return lstrip(rstrip(str)); }

void print_http_request(request *req) {
  printf("HTTP Request\n");
  printf("Verb: %d\n", req->verb);
  printf("URI: %s\n", req->uri);
  printf("Headers:\n");
  for (int i = 0; i < req->header_count; i++) {
    printf("%s\n", req->headers[i]);
  }
}
