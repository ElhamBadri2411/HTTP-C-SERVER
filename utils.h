#ifndef UTILS_H
#define UTILS_H

#include "server.h"
#include <netdb.h>
#include <sys/types.h>
char *lstrip(char *str);
char *rstrip(char *str);
char *strip(char *str);
int has_params(char *str);
int get_char_pos(char *str, char c);

char *strlower(char *str);

void print_http_request(request *req);
void print_addr_info(struct addrinfo *result);

#endif // UTILS_H
