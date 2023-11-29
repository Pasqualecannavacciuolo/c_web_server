// utils.h
#include <stdbool.h>

#ifndef UTILS_H
#define UTILS_H

char *stripSlash(char uri[100]);
bool get_file_to_serve(char *clean_uri, char *file_content);
char *color_print(char *color, char *text, char *reset);

#endif // UTILS_H
