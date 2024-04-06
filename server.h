// header file for server.c

#ifndef SERVER_H
#define SERVER_H
#include <stdbool.h>

enum HTTP_VERBS { GET, POST, PUT, DELETE };

bool validate_status_line(char *buffer);
int get_http_verb(char *buffer);
int parse_headers(char *buffer, char *headers[]);
bool validate_request(char *buffer);
#endif // SERVER_H
