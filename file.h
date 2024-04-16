#ifndef FILES_H
#define FILES_H

#include "server.h"
char *get_file_extension(char *filename);
char *get_mime_type(char *filename);
char *load_file(char);
void serve_file(request *req, char *name);
void write_to_db(request *req);

#endif // FILES_H
