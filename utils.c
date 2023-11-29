#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE 1024

/**
 * @brief Strip the first slash of URI
 *
 * @param uri The actual URI
 */
char *stripSlash(char uri[100])
{
    int uri_len = strlen(uri);
    if (uri_len == 1)
    {
        printf("URI: %s\n", uri);
        return strdup(uri);
    }
    char sanitized_uri[100];
    strcpy(sanitized_uri, uri);

    for (int i = 0; i < uri_len; i++)
    {
        sanitized_uri[i] = sanitized_uri[i + 1];
    }

    printf("URI: %s\n", sanitized_uri);

    // Alloca dinamicamente la memoria per la stringa risultante
    char *result = strdup(sanitized_uri);
    return result;
}

/**
 * @brief Show HTDOCS dir content
 *
 */
bool get_file_to_serve(char *clean_uri, char *file_content)
{
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir("./htdocs");
    if (folder == NULL)
    {
        perror("webserver (folder)");
        return false;
    }

    // HTDOCS list folder
    while ((entry = readdir(folder)))
    {
        files++;
        // Ottengo contenuto del file
        char complete_path[50];
        strcpy(complete_path, "./htdocs/");
        strcat(complete_path, clean_uri);
        // Se trova in htdocs il file richiesto con nome uguale a clean_uri
        if (strcmp(clean_uri, entry->d_name) == 0)
        {
            FILE *file = fopen(complete_path, "r");
            if (file == NULL)
            {
                perror("webserver (file)");
            }
            size_t bytesRead = fread(file_content, 1, BUFFER_SIZE - 1, file);
            file_content[bytesRead] = '\0'; // Aggiungi il terminatore di stringa
            // Leggi e stampa il contenuto del file
            /*int character;
            while ((character = fgetc(file)) != EOF)
            {
                putchar(character);
            }*/

            fclose(file); // Ricorda di chiudere il file quando hai finito di usarlo
            // printf("File %3d: %s", files, color_print(ANSI_COLOR_GREEN, entry->d_name, ANSI_COLOR_RESET));
            closedir(folder);
            return true;
        }
        // Il file richiesto non e' stato trovato
        /*else
        {
            printf("File %3d: %s", files, color_print(ANSI_COLOR_BLUE, entry->d_name, ANSI_COLOR_RESET));
        }*/
    }

    closedir(folder);
    return false;
}

char *color_print(char *color, char *text, char *reset)
{
    char message[100];
    snprintf(message, sizeof(message), "%s%s%s\n", color, text, reset);
    char *result = strdup(message);
    return result;
}
