// header file for server.c

#ifndef SERVER_H
#define SERVER_H
#include "json.h"
#include <stdbool.h>

#define MAX_HEADER_COUNT 20
#define MAX_PARAMS_COUNT 10
#define BODY_JSON_COUNT 10
#define _GNU_SOURCE 1

enum HTTP_VERBS { GET, POST, PUT, DELETE, INVALID = -1 };
enum RESPONSE_TYPE {
  OK = 200,
  CREATED = 201,
};

typedef struct http_request {
  int verb;
  char *uri;
  keyval headers[MAX_HEADER_COUNT];
  int header_count;
  keyval params[MAX_PARAMS_COUNT];
  int param_count;
  int response_fd;
  keyval *body;
  int body_count;
} request;

typedef struct db_response {
  keyval *body;
  int body_count;
} db_response;

// http parsing / validation functions
bool parse_and_validate_request(char *buffer, request *req);
int get_http_verb(char *buffer);
void get_response_type_string(enum RESPONSE_TYPE response_type,
                              char *response_type_string);
void send_response_start(request *req, enum RESPONSE_TYPE response_type);
#endif // SERVER_H
