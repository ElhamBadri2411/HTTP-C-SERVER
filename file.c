#include "file.h"
#include <string.h>
#include <util.h>

char *get_file_extension(char *filename) {

  char *dot = strrchr(filename, '.');
  if (!dot || dot == filename) {
    return "";
  }
  return dot + 1;
}
char *get_mime_type(char *filename) {

  char *file_extension = get_file_extension(filename);

  if (strcmp(file_extension, "html") == 0 ||
      strcmp(file_extension, "htm") == 0) {
    return "text/html";
  }
  if (strcmp(file_extension, "jpg") == 0 ||
      strcmp(file_extension, "jpeg") == 0) {
    return "image/jpg";
  }
  if (strcmp(file_extension, "css") == 0) {
    return "text/css";
  }
  if (strcmp(file_extension, "json") == 0) {
    return "application/json";
  }
  if (strcmp(file_extension, "js") == 0) {
    return "application/javascript";
  }
  if (strcmp(file_extension, "txt") == 0) {
    return "text/plain";
  }
  if (strcmp(file_extension, "png") == 0) {
    return "image/png";
  }
  if (strcmp(file_extension, "gif") == 0) {
    return "image/gif";
  }
  if (strcmp(file_extension, "pdf") == 0) {
    return "application/pdf";
  }
  if (strcmp(file_extension, "svg") == 0) {
    return "image/svg+xml";
  }
  if (strcmp(file_extension, "xml") == 0) {
    return "application/xml";
  }
  if (strcmp(file_extension, "zip") == 0) {
    return "application.zip";
  }
  return "application/octet-stream";
}
