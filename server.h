// header file for server.c

#ifndef SERVER_H
#define SERVER_H
#include <stdbool.h>

#define MAX_HEADER_COUNT 10
enum HTTP_VERBS { GET, POST, PUT, DELETE, INVALID = -1 };

typedef struct http_request {
  int verb;
  char *uri;
  char *headers[MAX_HEADER_COUNT];
  char *body;
  int header_count;
} request;

typedef struct http_response {
  int status_code;
  char *status_line;
  char *headers[MAX_HEADER_COUNT];
  char *body;
} response;

bool validate_status_line(char *buffer, request *req);
int get_http_verb(char *buffer);
bool parse_headers(char *buffer, request *req);
bool handle_request(char *buffer, request *req);
#endif // SERVER_H
