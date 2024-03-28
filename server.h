// header file for server.c

#ifndef SERVER_H
#define SERVER_H

enum HTTP_METHODS { GET, POST, PUT, DELETE };

void handle_request(int client_fd);
int get_method(char *buffer);
int parse_headers(char *buffer, char *headers[]);
#endif // SERVER_H
