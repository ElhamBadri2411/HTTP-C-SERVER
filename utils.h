#ifndef UTILS_H
#define UTILS_H

#include "server.h"
char *lstrip(char *str);
char *rstrip(char *str);
char *strip(char *str);

void print_http_request(request *req);
#endif // UTILS_H
