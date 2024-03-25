#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

  // LOAD UP ADRESS STRUCTS WITH getaddrinfo()
  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(argv[1], "8080", &hints, &result);

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
  bind(socket_fd, result->ai_addr, result->ai_addrlen);

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

  // RECEIVE DATA (LOOP THROUGH UNTIL NO MORE DATA)
  char buffer[100];
  int response;
  while ((response = recv(incoming_socket_fd, buffer, 100, 0)) > 0) {
    printf("buffer %s\n", buffer);
  }
}
