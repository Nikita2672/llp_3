//#include "socket_wrapper.h"

//int main() {
//    int server_socket = create_server_socket();
//    bind_socket(server_socket);
//    listen_for_connections(server_socket);
//
//    printf("Server waiting for connections...\n");
//
//    int client_socket = accept_connection(server_socket);
//    printf("Connection established with client.\n");
//
//    int times = 5;
//    while (times) {
//        times--;
//        char buffer[MAX_BUFFER_SIZE];
//        receive_data(client_socket, buffer);
//
//        printf("Received from client: %s\n", buffer);
//
//        // Echo the received data back to the client
//        send_data(client_socket, buffer);
//    }
//
//    close_socket(client_socket);
//    close_socket(server_socket);

//    return 0;
//}
