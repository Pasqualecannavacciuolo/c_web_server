#!/bin/bash
gcc bin/webserver.o bin/utils.o bin/connection_handler.o -o main -pthread