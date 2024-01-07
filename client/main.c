#include "net/socket_wrapper.h"
#include "parser/input.h"
#include "transfer/ast_to_xml.h"

#include <stdio.h>


int main() {
    const char *server_ip = "127.0.0.1";
    int client_socket = create_client_socket(server_ip);

    char buffer[MAX_BUFFER_SIZE];

    int times = 5;
    while (times) {
//        times--;
        printf("Enter a message to send to the server: ");
        input();
        char * xml = to_xml();

        strncpy(buffer, xml, sizeof(buffer));
        freeAstTree();
        send_data(client_socket, buffer);
        receive_data(client_socket, buffer);
    }
    close_socket(client_socket);


    return 0;
}
