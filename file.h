#ifndef FILES_H
#define FILES_H

#include "server.h"
char *get_file_extension(char *filename);
char *get_mime_type(char *filename);
char *get_mime_type_from_req(request *req);

#endif // FILES_H
