#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#define MAX_BUFFER_SIZE 40096

int create_client_socket(const char *server_ip, int server_port);
void send_data(int socket_fd, const char *data);
void receive_data(int socket_fd, char *buffer);
void close_socket(int socket_fd);

#endif // SOCKET_WRAPPER_H
