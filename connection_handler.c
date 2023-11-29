#include "connection_handler.h"
#include "utils.h"
#include "constants.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *handle_connection(void *arg)
{
    // CREATE CLIENT address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    int newsock = *((int *)arg);
    char buffer[BUFFER_SIZE];

    int valread = read(newsock, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        perror("webserver (read)");
        close(newsock);
        pthread_exit(NULL);
    }

    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    char *clean_uri = stripSlash(uri);

    char file_content[BUFFER_SIZE] = "";
    bool founded;

    pthread_mutex_lock(&mutex);

    founded = get_file_to_serve(clean_uri, file_content);
    strcat(file_content, "\r\n");

    if (founded)
    {
        char file_resp[BUFFER_SIZE] = "HTTP/1.0 200 OK\r\n"
                                      "Server: webserver-c\r\n"
                                      "Content-type: text/html\r\n\r\n";
        strcat(file_resp, file_content);

        int valwrite = write(newsock, file_resp, strlen(file_resp));
        if (valwrite < 0)
        {
            perror("webserver (write)");
        }
    }
    else
    {
        get_file_to_serve("not_found.html", file_content);
        char file_resp[BUFFER_SIZE];
        snprintf(file_resp, sizeof(file_resp), "HTTP/1.0 404 NOT FOUND\r\n"
                                               "Server: webserver-c\r\n"
                                               "Content-type: text/html\r\n\r\n%s",
                 file_content);

        int valwrite = write(newsock, file_resp, strlen(file_resp));
        if (valwrite < 0)
        {
            perror("webserver (write)");
        }
    }

    pthread_mutex_unlock(&mutex);

    free(clean_uri);
    close(newsock);
    pthread_exit(NULL);
}
