#include "server.h"
#include "file.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

  // LOAD UP ADRESS STRUCTS WITH getaddrinfo()
  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(argv[1], PORT, &hints, &result);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  struct addrinfo *c;
  // iterate through the addrinfo results from getaddrinfo
  for (c = result; c != NULL; c = c->ai_next) {
    if (c->ai_family == AF_INET) { // IPv4
      // cast to IPv4 struct
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)c->ai_addr;

      // get the pointer to the address
      void *addr = &(ipv4->sin_addr);

      // space to store the ip string
      char ipstr[INET_ADDRSTRLEN];

      // convert the IP to a string
      inet_ntop(c->ai_family, addr, ipstr, sizeof(ipstr));

      // print port number + ip
      printf("PORT: %d\n", ntohs(ipv4->sin_port));
      printf("IPV4: %s\n", ipstr);
    } else { // IPv6

      // cast the addr to IPv6 struct
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)c->ai_addr;

      // get the pointer to the address
      void *addr = &(ipv6->sin6_addr);

      // space to store the ip
      char ipstr[INET6_ADDRSTRLEN];

      // covert the ip to a a string
      inet_ntop(c->ai_family, addr, ipstr, sizeof(ipstr));

      // print the port number + ip
      printf("PORT: %d\n", ntohs(ipv6->sin6_port));
      printf("IPV6: %s\n", ipstr);
    }
  }

  // CREATE SOCKET

  int socket_fd =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  int option_value = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
  // BIND SOCKET TO PORT
  int bind_status = bind(socket_fd, result->ai_addr, result->ai_addrlen);
  if (bind_status == -1) {
    printf("Error bindings socket to port\n");
    return 1;
  }

  freeaddrinfo(result);

  // LISTENT TO PORT
  int listen_status = listen(socket_fd, 10);
  if (listen_status == -1) {
    printf("Error listening on port\n");
    return 1;
  }

  while (1) {
    // ACCEPT CONNECTION
    int incoming_socket_fd;

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addr_size = sizeof(their_addr);
    incoming_socket_fd =
        accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);

    // while (1) {

    // SEND DATA

    char buffer[BUFFER_SIZE];
    ssize_t bytes_recived;

    bytes_recived = recv(incoming_socket_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_recived < 0) {
      printf("error reciving data\n");
      return 1;
    }

    *(buffer + bytes_recived + 1) = '\0';

    request req;
    req.response_fd = incoming_socket_fd;
    if (handle_request(buffer, &req) == false) {
      printf("ERROR INVALID REQUEST\n");
    }

    // if(strcmp(req.uri, "/") == 0){

    // }
    // char *content =
    //     "<h1 style=\"color:blue;font-size:21rem\" >Hello world!</h1>";
    // int content_length = strlen(content) + 100;
    // char *response;
    // asprintf(&response,
    //          "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
    //          "%d\n\n%s",
    //          content_length, content);

    // printf("client_socket: %d\n", incoming_socket_fd);
    // int bytes_sent = send(incoming_socket_fd, response, strlen(response), 0);
    // send(incoming_socket_fd,
    //      "<h2>"
    //      "122222222222222222222222222222222222222222222222222222222222222222222"
    //      "222222222222222222222</h2>",
    //      100, 0);
    // printf("bytes_sent: %d\n", bytes_sent);
    // printf("bytes_received: %d\n", bytes_recived);
    // if (bytes_sent < 0) {
    //   printf("Error sending data\n");
    //   return 1;
    // }

    // CLOSE CONNECTION
    close(incoming_socket_fd);
  }
  return 0;
}

int get_http_verb(char *buffer) {
  if (strncmp(buffer, "GET", 3) == 0) {
    return GET;
  } else if (strncmp(buffer, "POST", 4) == 0) {
    return POST;
  } else if (strncmp(buffer, "PUT", 3) == 0) {
    return PUT;
  } else if (strncmp(buffer, "DELETE", 6) == 0) {
    return DELETE;
  }
  return INVALID;
}

bool parse_and_validate_request(char *buffer, request *req) {
  char *token;
  char *saveptr;

  //======= HANDLE REQ LINE =========
  token = strtok_r(buffer, " ", &saveptr);

  if (token == NULL) {
    perror("Blank request line\n");
    return false; // No tokens found
  }
  enum HTTP_VERBS verb = get_http_verb(token);
  if (verb == INVALID) {
    perror("Invalid HTTP Verb\n");
    return false; // Invalid HTTP Verb
  }

  req->verb = verb;
  token = strtok_r(NULL, " ", &saveptr);
  if (token == NULL || strlen(token) == 0) {
    perror("Empty or missing Request Target\n");
    return false; // Empty or missing Request Target
  }
  req->uri = token;

  token = strtok_r(NULL, "\r\n", &saveptr);

  if (token == NULL || strncmp(token, "HTTP/1.1", 8) != 0) {
    perror("Invalid or missing HTTP Version, please ensure that you're sending "
           "an HTTP/1.1 request\n");
    return false; // Invalid or missing HTTP Version
  }

  //======HANDLE HEADERS========

  // need this because of wierd bug where strtok_r will skip 2 consecutive
  // delimiters in this case it skips \r\n\r\n and treats the body as a header
  // https://stackoverflow.com/questions/65916201/strtok-skips-more-than-one-delimiter-how-to-make-it-skip-only-one
  char *saveptr_trailing = saveptr;
  token = strtok_r(NULL, "\r\n", &saveptr);
  strlcpy(req->headers[req->header_count], token, MAX_HEADER_LENGTH);
  req->header_count++;

  while (token != NULL) {
    // add 1 here because saveptr off by 1 issues
    if (token != (saveptr_trailing + 1)) {
      break;
    } else {
      strlcpy(req->headers[req->header_count], token, MAX_HEADER_LENGTH);
      req->header_count++;
      saveptr_trailing = saveptr;
      token = strtok_r(NULL, "\r\n", &saveptr);
    }
  }
  //======== HANDLE BODY =======
  // printf("typefromreq : %s\n", get_mime_type_from_req(req));
  return true;
}

void handle_get_request(request *req) {
  char *filename = malloc(200);
  char *mime_type = get_mime_type(req->uri);

  struct stat file_stat;
  asprintf(&filename, "files%s", req->uri);

  if (stat(filename, &file_stat) == -1) {
    return;
  }

  if (!S_ISREG(file_stat.st_mode)) { 
    return;
  }

  long long content_length = (long long)file_stat.st_size;

  FILE *f = fopen(filename, "rb");
  free(filename);

  if (f == NULL) {
    perror("failed to open file");
  } else {
    char buffer[1024];
    char *response_header;
    asprintf(&response_header,
             "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: "
             "%lld\n\n",
             mime_type, content_length);
    int sent =
        send(req->response_fd, response_header, strlen(response_header), 0);
    printf(" sent %d\n", sent);

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, 1024, f)) > 0) {
      if (send(req->response_fd, buffer, bytes_read, 0) == -1) {
        perror("Error sending file");
        break;
      }
    }
  }

  fclose(f);
}

bool handle_request(char *buffer, request *req) {
  printf("\n\n%s\n\n", buffer);
  req->header_count = 0;
  if (parse_and_validate_request(buffer, req) == false) {
    return false;
  }

  // execute request
  switch (req->verb) {
  case GET:
    handle_get_request(req);
    break;
  default:
    return false;
  }

  print_http_request(req);
  return true;
}
