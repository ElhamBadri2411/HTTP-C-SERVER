#include "server.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define BUFFER_SIZE 1024

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
  for (c = result; c != NULL; c = c->ai_next) {
    printf("count: %d\n", ++count);
    if (c->ai_family == AF_INET) { // IPv4
      struct sockaddr_in *ipv4 =
          (struct sockaddr_in *)c->ai_addr; // cast to IPv4 struct
      void *addr = &(ipv4->sin_addr);       // get the pointer to the address
      char ipstr[INET_ADDRSTRLEN];          // space to store the string
      inet_ntop(c->ai_family, addr, ipstr,
                sizeof(ipstr)); // convert the IP to a string
                                // print port number too
      printf("PORT: %d\n", ntohs(ipv4->sin_port));
      printf("IPV4: %s\n", ipstr);
    } else { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)c->ai_addr;
      void *addr = &(ipv6->sin6_addr);
      char ipstr[INET6_ADDRSTRLEN];
      inet_ntop(c->ai_family, addr, ipstr, sizeof(ipstr));
      printf("PORT: %d\n", ntohs(ipv6->sin6_port));
      printf("IPV6: %s\n", ipstr);
    }
  }

  // CREATE SOCKET
  int socket_fd;

  socket_fd =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  // BIND SOCKET TO PORT
  int bind_status = bind(socket_fd, result->ai_addr, result->ai_addrlen);
  if (bind_status == -1) {
    printf("Error bindings socket to port\n");
    return 1;
  }

  // LISTENT TO PORT
  int listent_status = listen(socket_fd, 10);
  if (listent_status == -1) {
    printf("Error listening on port\n");
    return 1;
  }

  // ACCEPT CONNECTION
  int incoming_socket_fd;

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  addr_size = sizeof(their_addr);
  incoming_socket_fd =
      accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);

  while (1) {

    // SEND DATA
    handle_request(incoming_socket_fd);

    // CLOSE CONNECTION
    close(incoming_socket_fd);

    return 0;
  }
}

void handle_request(int client_fd) {
  char buffer[BUFFER_SIZE];
  ssize_t bytes_recived;

  bytes_recived = recv(client_fd, buffer, BUFFER_SIZE, 0);
  if (bytes_recived < 0) {
    printf("Error reciving data\n");
    return;
  }

  printf("Response:\n\n %s\n", buffer);

  char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
                   "12\n\nHello world!";

  printf("client_socket: %d\n", client_fd);
  int bytes_sent = send(client_fd, response, strlen(response), 0);
  printf("bytes_sent: %d\n", bytes_sent);
  if (bytes_sent < 0) {
    printf("Error sending data\n");
    return;
  }
}

int get_method(char *buffer) {
  if (strncmp(buffer, "GET", 3) == 0) {
    return GET;
  } else if (strncmp(buffer, "POST", 4) == 0) {
    return POST;
  } else if (strncmp(buffer, "PUT", 3) == 0) {
    return PUT;
  } else if (strncmp(buffer, "DELETE", 6) == 0) {
    return DELETE;
  }
  return -1;
}

int parse_headers(char *buffer, char *headers[]) { return 0; }
