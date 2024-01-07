// socket_wrapper.c
#include <ctype.h>
#include "socket_wrapper.h"

int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int create_client_socket(const char *server_ip) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));

    connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));

    return client_socket;
}

void bind_socket(int socket_fd) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
}

void listen_for_connections(int socket_fd) {
    listen(socket_fd, 5);
}

int accept_connection(int socket_fd) {
    return accept(socket_fd, NULL, NULL);
}

void connect_to_server(int socket_fd, const char *server_ip) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));

    connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
}

void send_data(int socket_fd, const char *data) {
    send(socket_fd, data, strlen(data), 0);
}

int trimmed_strcmp(const char *str1, const char *str2) {
    while (isspace(*str1)) {
        str1++;
    }

    const char *end1 = str1 + strlen(str1);
    while (end1 > str1 && isspace(*(end1 - 1))) {
        end1--;
    }
    while (isspace(*str2)) {
        str2++;
    }

    const char *end2 = str2 + strlen(str2);
    while (end2 > str2 && isspace(*(end2 - 1))) {
        end2--;
    }

    return strncmp(str1, str2, end1 - str1);
}

char *parseResponse(char *buffer) {
    LIBXML_TEST_VERSION

    xmlDocPtr doc = xmlReadMemory(buffer, strlen(buffer), "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse the input XML.\n");
        return NULL;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        fprintf(stderr, "Empty document.\n");
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNodePtr messageNode = xmlFirstElementChild(root);
    if (messageNode != NULL && xmlStrEqual(messageNode->name, BAD_CAST "Message")) {
        char *messageContent = (char *)xmlNodeGetContent(messageNode);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return messageContent;
    } else {
        xmlFreeDoc(doc);
        xmlCleanupParser();

        return NULL;
    }
}

void receive_data(int socket_fd, char *buffer) {
    memset(buffer, 0, MAX_BUFFER_SIZE);
    recv(socket_fd, buffer, MAX_BUFFER_SIZE, 0);
//    printf("\n\n\n\nBuffer contains: %s\n\n\n\n", buffer);
    char *message = parseResponse(buffer);
    while (trimmed_strcmp(message, "End") != 0) {
        printf("%s\n", message);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        recv(socket_fd, buffer, MAX_BUFFER_SIZE, 0);
//        printf("\n\n\n\nBuffer contains: %s\n\n\n\n", buffer);
        message = parseResponse(buffer);
    }
//    printf("%s\n", message);
}

void close_socket(int socket_fd) {
    close(socket_fd);
}
