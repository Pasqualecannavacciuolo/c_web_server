#include <stdbool.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

#define PORT 8080
#define BUFFER_SIZE 1024

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

int main()
{
    char buffer[BUFFER_SIZE];
    // Da implementare allocazione dinamica delle response in base ai contenuti

    // CREATE SOCKET
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEADDR
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");
#endif
    if (sock == -1)
    {
        perror("webserver (socket)");
        return 1;
    }

    // CREATE THE ADDRESS to bind the socket to
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // CREATE CLIENT address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    // Bind the socket to the address
    if (bind(sock, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("webserver (bind)");
        return 1;
    }
    printf("✅ Socket successfully bound to address\n");

    // LISTEN for incoming connections
    if (listen(sock, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return 1;
    }
    printf("⌛️ Server listening for connections...\n");

    for (;;)
    {
        // ACCEPTING incoming connections
        int newsock = accept(sock, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsock < 0)
        {
            perror("webserver (accept)");
            continue;
        }
        // printf("✅ Connection accepted\n");

        // GET CLIENT address
        int sockn = getsockname(newsock, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0)
        {
            perror("webserver (getsockname)");
            continue;
        }

        // READ from socket
        int valread = read(newsock, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
            continue;
        }

        // READ the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        char *clean_uri = stripSlash(uri);

        char file_content[BUFFER_SIZE] = "";
        bool founded = get_file_to_serve(clean_uri, file_content);
        strcat(file_content, "\r\n");

        // Se trovo il file
        if (founded == 1)
        {
            char file_resp[BUFFER_SIZE] = "HTTP/1.0 200 OK\r\n"
                                          "Server: webserver-c\r\n"
                                          "Content-type: text/html\r\n\r\n";
            strcat(file_resp, file_content);
            // WRITE to the socket
            int valwrite = write(newsock, file_resp, strlen(file_resp));
            if (valwrite < 0)
            {
                perror("webserver (write)");
                continue;
            }
        }
        // Se non trovo un riscontro
        else
        {
            get_file_to_serve("not_found.html", file_content);
            char file_resp[BUFFER_SIZE];
            snprintf(file_resp, sizeof(file_resp), "HTTP/1.0 404 NOT FOUND\r\n"
                                                   "Server: webserver-c\r\n"
                                                   "Content-type: text/html\r\n\r\n%s",
                     file_content);

            // WRITE to the socket
            int valwrite = write(newsock, file_resp, strlen(file_resp));
            if (valwrite < 0)
            {
                perror("webserver (write)");
            }
        }

        free(clean_uri);

        close(newsock);
    }
    return 0;
}