#include "utils.h"
#include <ctype.h>
#include <string.h>

char *lstrip(char *str) {
  while (isspace(*str)) {
    str++;
  }
  return str;
}

char *rstrip(char *str) {
  char *b = str + strlen(str);

  while (isspace(*b))
    b--;

  *(b + 1) = '\0';

  return str;
}

char *strip(char *str) { return lstrip(rstrip(str)); }
