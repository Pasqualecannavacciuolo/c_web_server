#!/bin/bash
gcc -c webserver.c -o bin/webserver.o -pthread
gcc -c utils.c -o bin/utils.o -pthread
gcc -c connection_handler.c -o bin/connection_handler.o -pthread