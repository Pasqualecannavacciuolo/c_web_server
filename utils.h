// utils.h
#include <stdbool.h>
#include "constants.h"

#ifndef UTILS_H
#define UTILS_H

char *stripSlash(char uri[100]);
bool get_file_to_serve(char *clean_uri, char *file_content);
char *color_print(char *color, char *text, char *reset);
void *handle_connection(void *args);

#endif // UTILS_H
