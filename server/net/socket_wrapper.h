#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8088

int create_server_socket();
void bind_socket(int socket_fd, int server_port);
void listen_for_connections(int socket_fd);
int accept_connection(int socket_fd);
void send_data(int socket_fd, const char *data);
void receive_data(int socket_fd, char *buffer);
void close_socket(int socket_fd);

#endif // SOCKET_WRAPPER_H
