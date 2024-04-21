#include "server.h"
#include "file.h"
#include "json.h"
#include "routes_handler.h"
#include "utils.h"
#include <arpa/inet.h>
#include <math.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT "8080"
#define BUFFER_SIZE 4096

void serve_json(char *json_string, request *req);
bool handle_request(char *buffer, request *req, route_table *rt);
void get_hello(request *req) { serve_file(req, "hello.html"); }
void post_stuff(request *req) {
  ;
  write_to_db(req);
}
void get_test(request *req) { serve_file(req, "test.html"); }
void param_test(request *req) {
  char *filename;

  for (int i = 0; i < req->param_count; i++) {
    char *val = get_val_from_key("filename", req->params[i]);
    if (val != NULL)
      filename = val;
  }

  serve_file(req, filename);
}
void get_css(request *req) { serve_file(req, "index.css"); }
void get_json(request *req) {
  int id;

  for (int i = 0; i < req->param_count; i++) {
    char *val = get_val_from_key("id", req->params[i]);
    printf("val = %s\n", val);
    if (val != NULL) {
      id = atoi(val);
      printf("id : %d\n", id);

      db_response dbr;
      dbr = get_from_db(id);

      int json_size;
      char *json_string =
          create_json_string(dbr.body, dbr.body_count, &json_size);

      print_db_response(dbr);
      serve_json(json_string, req);

      return;
    }
  }
}
void notfound(request *req) { serve_file(req, "notfound.html"); }
void delete_test(request *req) {

  int id;

  for (int i = 0; i < req->param_count; i++) {
    char *val = get_val_from_key("id", req->params[i]);
    printf("val = %s\n", val);
    if (val != NULL) {
      id = atoi(val);
      printf("id : %d\n", id);

      delete_from_db(id);

      return;
    }
  }
}
// Parse JSON line (assuming you have a JSON parsing function)
int main(int argc, char *argv[]) {

  // LOAD UP ADRESS STRUCTS WITH getaddrinfo()
  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (argc < 2) {
    status = getaddrinfo("localhost", PORT, &hints, &result);
  } else {
    status = getaddrinfo(argv[1], PORT, &hints, &result);
  }

  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }
  // PRINT ADDRINFO
  print_addr_info(result);

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

  route_table *rt = create_route_table(20);
  add_route(rt, "/hello", get_hello, GET);
  add_route(rt, "/test", get_test, GET);
  add_route(rt, "/params", param_test, GET);
  add_route(rt, "/index.css", get_css, GET);
  add_route(rt, "/user", post_stuff, POST);
  add_route(rt, "/json", get_json, GET);
  add_route(rt, "/json", delete_test, DELETE);

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
    if (handle_request(buffer, &req, rt) == false) {
      printf("ERROR INVALID REQUEST\n");
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

  //================HANDLE PARAMS====================
  int param_location = has_params(token);

  if (param_location != -1) {

    token[param_location] = '\0';
    req->uri = token;

    char *params = token + param_location + 1;

    char *param = strtok(params, "&");

    while (param != NULL) {
      char *key_value_pair = strdup(param); // Create a copy
      char *equals_sign = strchr(key_value_pair, '=');

      if (equals_sign != NULL) {
        *equals_sign = '\0';
        keyval new_param;
        new_param.key = key_value_pair;
        new_param.value = equals_sign + 1;
        req->params[req->param_count] = new_param;
        req->param_count++;
      } else {
        free(key_value_pair);
      }

      param = strtok(NULL, "&");
    }
  } else {
    req->uri = token;
  }

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

  char *colon = strchr(token, ':');

  if (colon != NULL) {
    char *key_value_pair = strdup(token);
    *colon = '\0';
    keyval new_header;
    new_header.key = key_value_pair;
    new_header.value = colon + 1;
    req->headers[req->header_count] = new_header;
    req->header_count++;
  }

  // strlcpy(req->headers[req->header_count], token, MAX_HEADER_LENGTH);
  // req->header_count++;

  while (token != NULL) {
    // add 1 here because saveptr off by 1 issues
    if (token != (saveptr_trailing + 1)) {
      break;
    } else {
      int colon = get_char_pos(token, ':');

      if (colon != -1) {
        char *key_value_pair = strdup(token);
        key_value_pair[colon] = '\0';
        keyval new_header;
        new_header.key = key_value_pair;
        new_header.value = strip(key_value_pair + colon + 1);
        req->headers[req->header_count] = new_header;
        req->header_count++;
      }
      saveptr_trailing = saveptr;
      token = strtok_r(NULL, "\r\n", &saveptr);
    }
  }
  //======== HANDLE BODY =======

  keyval *kv = create_keyvals_from_json_string(token, &req->body_count);
  req->body = kv;

  return true;
}

void serve_file(request *req, char *name) {

  char *mime_type = get_mime_type(name);
  char *filename = malloc(200);
  struct stat file_stat;
  asprintf(&filename, "files/%s", name);

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

bool handle_request(char *buffer, request *req, route_table *rt) {
  req->header_count = 0;
  req->param_count = 0;
  req->body_count = 0;
  if (parse_and_validate_request(buffer, req) == false) {
    return false;
  }

  route_entry *re = get_route(rt, req->uri, req->verb);
  if (re == NULL) {
    notfound(req);
    return true;
  }
  re->handler(req);

  print_http_request(req);
  return true;
}

void write_to_db(request *req) {
  FILE *db = fopen("files/db.txt", "ab");
  if (db == NULL) {
    perror("failed to open db files");
    fclose(db);
  }

  int json_size = 0;
  char *json = create_json_string(req->body, req->body_count, &json_size);
  fwrite(json, json_size, 1, db);
  fwrite("\n", 1, 1, db);
  fclose(db);
  return;
}

db_response get_from_db(int id) {
  int id_str_size = (int)((ceil(log10(id)) + 1) * sizeof(char));
  char id_str[id_str_size];
  snprintf(id_str, id_str_size, "%d", id);
  db_response dbr = {.body = NULL, .body_count = 0};

  FILE *db = fopen("files/db.txt", "rb");
  if (db == NULL) {
    perror("failed to open db files");
    fclose(db);
  }

  char buffer[2048];
  int count = 3;
  while (fgets(buffer, sizeof(buffer), db)) {

    keyval *kv = create_keyvals_from_json_string(buffer, &count);

    for (int i = 0; i < count; i++) {
      if (get_val_from_key("\"id\"", kv[i]) != NULL &&
          strncmp(get_val_from_key("\"id\"", kv[i]), id_str, id_str_size) ==
              0) {
        fclose(db);
        dbr.body = kv;
        dbr.body_count = count;
        return dbr;
      }
    }
    free(kv);
  }
  return dbr;
  fclose(db);
}

void delete_from_db(int id) {
  int id_str_size = (int)((ceil(log10(id)) + 1) * sizeof(char));
  char id_str[id_str_size];
  snprintf(id_str, id_str_size, "%d", id);
  bool found = false;

  FILE *db = fopen("files/db.txt", "rb");
  if (db == NULL) {
    perror("failed to open db files");
    fclose(db);
  }

  FILE *temp_db = fopen("files/temp.txt", "wb");
  if (temp_db == NULL) {
    perror("failed to open db files");
    fclose(db);
  }

  char buffer[2048];
  int count = 3;
  while (fgets(buffer, sizeof(buffer), db)) {
    found = false;

    keyval *kv = create_keyvals_from_json_string(buffer, &count);

    for (int i = 0; i < count; i++) {
      if (get_val_from_key("\"id\"", kv[i]) != NULL &&
          strncmp(get_val_from_key("\"id\"", kv[i]), id_str, id_str_size) ==
              0) {
        found = true;
      }
    }
    if (!found) {
      fwrite(buffer, strlen(buffer), 1, temp_db);
    }
    free(kv);
  }

  fclose(temp_db);
  fclose(db);

  remove("files/db.txt");
  rename("files/temp.txt", "files/db.txt");
}

void serve_json(char *json_string, request *req) {
  int json_len = strlen(json_string);
  char *mime_type = "application/json";

  char *response_header;
  asprintf(&response_header,
           "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: "
           "%d\n\n",
           mime_type, json_len);

  send(req->response_fd, response_header, strlen(response_header), 0);

  if (json_len != 0) {
    if (send(req->response_fd, json_string, json_len, 0) == -1) {
      perror("Error sending file");
    }
  }
}
