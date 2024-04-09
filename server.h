// header file for server.c

#ifndef SERVER_H
#define SERVER_H
#include <stdbool.h>

#define MAX_HEADER_COUNT 20
#define MAX_HEADER_LENGTH 1000
enum HTTP_VERBS { GET, POST, PUT, DELETE, INVALID = -1 };

typedef struct http_request_metadata {
  int verb;
  char *uri;
  char headers[MAX_HEADER_COUNT][MAX_HEADER_LENGTH];
  int header_count;
} request;

typedef struct http_response {
  int status_code;
  char *status_line;
  char *headers[MAX_HEADER_COUNT];
  char *body;
} response;

bool parse_and_validate_request(char *buffer, request *req);
int get_http_verb(char *buffer);
bool handle_request(char *buffer, request *req);
#endif // SERVER_H
