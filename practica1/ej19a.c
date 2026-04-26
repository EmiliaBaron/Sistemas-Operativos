#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE

#include <string.h> // para strcpy
#include <sys/socket.h> // para los sockets
#include <sys/un.h> // para el sockaddr_un

int main(){

    __pid_t proceso1OCero = fork();

    if(proceso1OCero == 0){

        int client_socket;
        int server_socket;
        struct sockaddr_un server_addr;
        struct sockaddr_un client_addr;
        uint slen = sizeof(server_addr);
        uint clen = sizeof(client_addr);
        int num;

        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, "unix_socket");
        unlink(server_addr.sun_path);

        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        bind(server_socket, (struct sockaddr *) &server_addr, slen);
        listen(server_socket, 1);
        
        printf("Servidor: esperando conexión del cliente...\n");
        
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
        recv(client_socket, &num, sizeof(num), 0);
        printf("Servidor: recibí %d del cliente!\n", num);
        num += 1;
        send(client_socket, &num, sizeof(num), 0);

        recv(client_socket, &num, sizeof(num), 0);
        printf("Servidor: recibí %d del cliente!\n", num);
        num += 1;
        send(client_socket, &num, sizeof(num), 0);

        close(client_socket);
       
       
        

    }else{
        __pid_t proceso2OCero = fork();

        if(proceso2OCero == 0){
            int server_socket;
            struct sockaddr_un server_addr;

            server_addr.sun_family = AF_UNIX;
            strcpy(server_addr.sun_path, "unix_socket");

            server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
            if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
                perror("No hay server");
                exit(1);
            }

            int num = 0;
            send(server_socket, &num, sizeof(num), 0);
            recv(server_socket, &num, sizeof(num), 0);
            printf("Cliente: recibí %d del servidor!\n", num);

            num = 2;
            send(server_socket, &num, sizeof(num), 0);
            recv(server_socket, &num, sizeof(num), 0);
            printf("Cliente: recibí %d del servidor!\n", num);

            exit(EXIT_SUCCESS);

        }


    }


}