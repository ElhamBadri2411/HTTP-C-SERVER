#include "utils.h"
#include "server.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *lstrip(char *str) {
  if (!str) {
    return NULL;
  }

  while (isspace(*str)) {
    str++;
  }
  return str;
}

char *rstrip(char *str) {
  if (!str) {
    return NULL;
  }
  char *b = str + strlen(str);

  while (isspace(*b))
    b--;

  *(b + 1) = '\0';

  return str;
}

char *strip(char *str) {

  if (!str) {
    return NULL;
  }
  return lstrip(rstrip(str));
}

char *strlower(char *str) {
  if (!str) {
    return NULL;
  }
  char *c;

  for (c = str; c != '\0'; c++) {
    tolower(*c);
  }

  return str;
}

void print_http_request(request *req) {

  printf("header_count: %d\n", req->header_count);
  printf("HTTP Request\n");
  printf("Verb: %d\n", req->verb);
  printf("URI: %s\n", req->uri);
  printf("====Headers:====\n");

  for (int i = 0; i < req->header_count; i++) {

    printf("header %d: %s\n", i, req->headers[i]);
  }
}
