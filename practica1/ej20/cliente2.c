#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int server_socket;
    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "servidor");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    while (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        sleep(1);
    }

    long num = 45;
    int res;
    write(server_socket, &num, sizeof(num));
    read(server_socket, &res, sizeof(res));

    if(res == 0){
        printf("Cliente2: recibí que %ld NO es primo del servidor!\n", num);
    }
    else if(res == 1){
        printf("Cliente2: recibí que %ld es primo del servidor!\n", num);
    }else{
        printf("Cliente2: recibí %d. Respuesta incorrecta del servidor!\n", res);
    }

    close(server_socket);
    exit(0);
}