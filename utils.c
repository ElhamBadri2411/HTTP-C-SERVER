#include "utils.h"
#include "server.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

char *lstrip(char *str)
{
  if (!str)
  {
    return NULL;
  }

  while (isspace(*str))
  {
    str++;
  }
  return str;
}

char *rstrip(char *str)
{
  if (!str)
  {
    return NULL;
  }
  char *b = str + strlen(str);

  while (isspace(*b))
    b--;

  *(b + 1) = '\0';

  return str;
}

char *strip(char *str)
{

  if (!str)
  {
    return NULL;
  }
  return lstrip(rstrip(str));
}

char *strlower(char *str)
{
  if (!str)
  {
    return NULL;
  }
  char *c;

  for (c = str; c != '\0'; c++)
  {
    tolower(*c);
  }

  return str;
}

int has_params(char *str)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == '?')
    {
      return i;
    }
  }
  return -1;
}

int get_char_pos(char *str, char c)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == c)
    {
      return i;
    }
  }
  return -1;
}
void print_http_request(request *req)
{

  printf("header_count: %d\n", req->header_count);
  printf("HTTP Request\n");
  printf("Verb: %d\n", req->verb);
  printf("URI: %s\n", req->uri);
  printf("====Headers:====\n");

  for (int i = 0; i < req->header_count; i++)
  {

    printf("header %d: %s : %s\n", i, req->headers[i].key,
           req->headers[i].value);
  }

  printf("======Params======\n");
  for (int i = 0; i < req->param_count; i++)
  {

    printf("param %d: %s : %s\n", i, req->params[i].key, req->params[i].value);
  }
  printf("======Body======\n");
  for (int i = 0; i < req->body_count; i++)
  {

    printf("body %d: %s : %s\n", i, req->body[i].key, req->body[i].value);
  }
}

void print_addr_info(struct addrinfo *result)
{
  struct addrinfo *c;
  // iterate through the addrinfo results from getaddrinfo
  for (c = result; c != NULL; c = c->ai_next)
  {
    if (c->ai_family == AF_INET)
    { // IPv4
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
    }
    else
    { // IPv6

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
}

void print_db_response(db_response dbr)
{

  for (int i = 0; i < dbr.body_count; i++)
  {
    printf("body %d: %s : %s\n", i, dbr.body[i].key, dbr.body[i].value);
  }
}
