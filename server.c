#include "server.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
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

  int count = 0;
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
    if (handle_request(buffer, &req) == false) {
      printf("ERROR INVALID REQUEST\n");
    }

    char *content =
        "<h1 style=\"color:blue;font-size:21rem\" >Hello world!</h1>";
    int content_length = strlen(content);
    char *response;
    asprintf(&response,
             "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
             "%d\n\n%s",
             content_length, content);

    printf("client_socket: %d\n", incoming_socket_fd);
    int bytes_sent = send(incoming_socket_fd, response, strlen(response), 0);
    printf("bytes_sent: %d\n", bytes_sent);
    printf("bytes_received: %d\n", bytes_recived);
    if (bytes_sent < 0) {
      printf("Error sending data\n");
      return 1;
    }

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

bool parse_headers(char *buffer, request *req) {

  char *header = strtok(buffer, "\r\n"); // Split by newline to separate headers
  while (header != NULL) {
    if (strlen(header) > 0) {
      strcpy(req->headers[req->header_count++], header);
    }
    header = strtok(NULL, "\r\n"); // Move to the next header
  }
  return true;
}

bool validate_status_line(char *buffer, request *req) {
  char *token;

  token = strtok(buffer, " ");

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
  token = strtok(NULL, " ");

  if (token == NULL || strlen(token) == 0) {
    perror("Empty or missing Request Target\n");
    return false; // Empty or missing Request Target
  }
  req->uri = token;

  token = strtok(NULL, " ");

  if (token == NULL || strncmp(token, "HTTP/1.1", 8) != 0) {
    perror("Invalid or missing HTTP Version, please ensure that you're sending "
           "an HTTP/1.1 request\n");
    return false; // Invalid or missing HTTP Version
  }

  return true;
}

bool handle_request(char *buffer, request *req) {
  printf("\n\n%s\n\n", buffer);
  req->header_count = 0;
  if (validate_status_line(buffer, req) == false) {
    return false;
  }
  if (parse_headers(buffer, req) == false) {
    return false;
  }

  print_http_request(req);
  return true;
}
