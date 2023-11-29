#include "utils.h"
#include "constants.h"
#include "connection_handler.h"

#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>

pthread_mutex_t mutex_main = PTHREAD_MUTEX_INITIALIZER;

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

        // Creazione di un thread per la gestione della connessione
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_connection, (void *)&newsock) != 0)
        {
            perror("webserver (pthread_create)");
            close(newsock);
            continue;
        }

        pthread_detach(thread); // Per evitare memory leaks
    }
    pthread_mutex_destroy(&mutex_main);
    return 0;
}
