#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAX_LINE 4096

int main() {

    int desconectarse = 0;

    while(!desconectarse){
        int server_socket;
        struct sockaddr_un server_addr;

        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, "servidor");

        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        while (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            sleep(1);
        }
        
        // char mensaje[8];
        // read(0, &mensaje, sizeof(mensaje));

        char mensaje[MAX_LINE] = "Buenos días";

        send(server_socket, &mensaje, sizeof(mensaje), 0);
        recv(server_socket, &mensaje, sizeof(mensaje), 0);

        printf("Cliente1: recibí que '%s' \n", mensaje);
        fflush(stdout);

        close(server_socket);
    }

    exit(0);
}
