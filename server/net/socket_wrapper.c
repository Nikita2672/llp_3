// socket_wrapper.c
#include "socket_wrapper.h"

int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int create_client_socket(const char *server_ip, int server_port) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));

    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    return client_socket;
}

void bind_socket(int socket_fd, int server_port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void listen_for_connections(int socket_fd) {
    listen(socket_fd, 5);
}

int accept_connection(int socket_fd) {
    return accept(socket_fd, NULL, NULL);
}

void connect_to_server(int socket_fd, const char *server_ip, int server_port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));

    connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void send_data(int socket_fd, const char *data) {
    printf("Sended data: %s", data);
    send(socket_fd, data, strlen(data), 0);
}

void receive_data(int socket_fd, char *buffer) {
    ssize_t bytes_received = recv(socket_fd, buffer, MAX_BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0) {
        // Handle disconnection or error
        if (bytes_received == 0) {
            printf("Client disconnected.\n");
        } else {
            perror("recv");
        }

        // Optionally, you may close the socket or take other actions as needed
    } else {
        // Null-terminate the received data
        buffer[bytes_received] = '\0';
    }
}
void close_socket(int socket_fd) {
    close(socket_fd);
}
