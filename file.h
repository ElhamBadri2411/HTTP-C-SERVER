#ifndef FILES_H
#define FILES_H

#include "json.h"
#include "server.h"
char *get_file_extension(char *filename);
char *get_mime_type(char *filename);
char *load_file(char);
void serve_file(request *req, char *name);
void write_to_db(request *req);
db_response get_from_db(int id);

#endif // FILES_H
