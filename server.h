// header file for server.c

#ifndef SERVER_H
#define SERVER_H
#include <stdbool.h>

#define MAX_HEADER_COUNT 20
#define MAX_HEADER_LENGTH 1000
#define MAX_PARAMS_COUNT 10
#define MAX_PARAMS_LENGTH 200
enum HTTP_VERBS { GET, POST, PUT, DELETE, INVALID = -1 };

typedef struct key_value_pair {
  char *key;
  char *value;
} keyval;

typedef struct http_request_metadata {
  int verb;
  char *uri;
  keyval headers[MAX_HEADER_COUNT];
  int header_count;
  keyval params[MAX_PARAMS_COUNT];
  int param_count;
  int response_fd;
} request;

typedef struct http_response {
  int status_code;
  char *status_line;
  char *headers[MAX_HEADER_COUNT];
  char *body;
} response;

// http parsing / validation functions
bool parse_and_validate_request(char *buffer, request *req);
int get_http_verb(char *buffer);
#endif // SERVER_H
