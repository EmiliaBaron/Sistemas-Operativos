#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE

#include <string.h> // para strcpy
#include <sys/socket.h> // para los sockets
#include <sys/un.h> // para el sockaddr_un

//Tuve que haber hecho la técnica accept + fork?

int crear_socket_server(char* nombreSocket){

    int server_socket;
    struct sockaddr_un server_addr;
    socklen_t slen = sizeof(server_addr);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, nombreSocket);
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 1);

    return server_socket;
}


int main(){

    //importante poner distintos nombres num1 num2 num3 porque si no parece que leen y modifican lo mismo

    __pid_t proceso1OCero = fork();

    if(proceso1OCero == 0){ //Proceso1

        int client_socket;
        int server_socket;
        int num1;
        struct sockaddr_un client_addr;
        socklen_t clen = sizeof(client_addr);
        

        server_socket = crear_socket_server("servidor1");
        
        printf("Proceso1: esperando conexión del Proceso2...\n");
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen); //esto es conectarse una sola vez
        


        int servidor3_socket;
        struct sockaddr_un servidor3_addr;

        servidor3_addr.sun_family = AF_UNIX;
        strcpy(servidor3_addr.sun_path, "servidor3");

        servidor3_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        while (connect(servidor3_socket, (struct sockaddr *)&servidor3_addr, sizeof(servidor3_addr)) == -1) {
            sleep(1);
        }
        printf("Proceso1: conectado\n");

        num1 = 0;

        send(client_socket, &num1, sizeof(num1), 0);

        while(1){
            int enviado1 = recv(servidor3_socket, &num1, sizeof(num1), 0);
            
            if (enviado1 == 0){
                printf("Proceso1: servidor3 cerró la conexión.");
                break;
            }
            
            printf("Proceso1: recibí %d del Proceso3!\n", num1);
            
            if(num1 == 50) {
                //num += 1;
                //send(client_socket, &num1, sizeof(num1), 0);
                break;
            }

            num1 += 1;
            int mandado1 = send(client_socket, &num1, sizeof(num1), 0);
            
            if (mandado1 == -1){
                printf("Proceso1: proceso2 cerró la conexión");
                break;
            }
        
        }

        close(server_socket);
        close(client_socket); 

        // el cliente no maneja caso de cierre -> cuando esto se cierra, el cliente sigue esperando
        // respuesta con recv?? distinto a pipes
        
        exit(EXIT_SUCCESS);
        

    }else{
        __pid_t proceso2OCero = fork();

        if(proceso2OCero == 0){ //Proceso2

            int servidor1_socket;
            struct sockaddr_un servidor1_addr;

            servidor1_addr.sun_family = AF_UNIX;
            strcpy(servidor1_addr.sun_path, "servidor1");

            servidor1_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  
            while (connect(servidor1_socket, (struct sockaddr *)&servidor1_addr, sizeof(servidor1_addr)) == -1) {
                sleep(1);
            }
            printf("Proceso2: conectado\n");



            int server_socket;
            int client_socket;
            int num2;
            struct sockaddr_un client_addr;
            socklen_t clen = sizeof(client_addr);

            server_socket = crear_socket_server("servidor2");

            printf("Proceso2: esperando conexión del Proceso3...\n");
            client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen); //esto es conectarse una sola vez


            while(1){
                int enviado2 = recv(servidor1_socket, &num2, sizeof(num2), 0);

                if (enviado2 == 0){
                    printf("Proceso2: servidor1 cerró la conexión\n");
                    break;
                } 
                

                printf("Proceso2: recibí %d del Proceso1!\n", num2);

                if  (num2 >= 50 ){
                //     num2 += 1;
                //     send(server_socket, &num2, sizeof(num2), 0);
                    break;
                }
                
                num2 += 1;
                int mandado2 = send(client_socket, &num2, sizeof(num2), 0);

                if (mandado2 == -1){
                    printf("Proceso2: proceso3 cerró la conexión");
                    break;
                }
            }

            close(server_socket);
            close(client_socket);
            exit(EXIT_SUCCESS);

        }else{

            __pid_t proceso3OCero = fork();

            if(proceso3OCero == 0){ //Proceso3

                int servidor2_socket;
                struct sockaddr_un servidor2_addr;

                servidor2_addr.sun_family = AF_UNIX;
                strcpy(servidor2_addr.sun_path, "servidor2");

                servidor2_socket = socket(AF_UNIX, SOCK_STREAM, 0);


                while (connect(servidor2_socket, (struct sockaddr *)&servidor2_addr, sizeof(servidor2_addr)) == -1) {
                    sleep(1);
                }
                printf("Proceso3: conectado\n");

                int server_socket;
                int client_socket;
                int num3;
                struct sockaddr_un client_addr;
                socklen_t clen = sizeof(client_addr);
                
                server_socket = crear_socket_server("servidor3");

                printf("Proceso3: esperando conexión del Proceso1...\n");
                client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);


                while(1){
                int enviado3 = recv(servidor2_socket, &num3, sizeof(num3), 0);

                if (enviado3 == 0){
                    printf("Proceso3: servidor2 cerró la conexión\n");
                    break;
                } 
                //al final es mejor que uno decida cuando se termina de enviar

                printf("Proceso3: recibí %d del Proceso2!\n", num3);

                if  (num3 >= 50 ){
                //     num3 += 1;
                //     send(server_socket, &num3, sizeof(num3), 0);
                    break;
                }
                
                num3 += 1;
                int mandado3 = send(client_socket, &num3, sizeof(num3), 0);

                if (mandado3 == -1){
                    printf("Proceso3: proceso1 cerró la conexión");
                    break;
                }
            }
            close(server_socket);
            close(client_socket);
            exit(EXIT_SUCCESS);
            } 
        }  

    }

    //si no se queda colgado el padre mientras los hijos terminaron
    wait(NULL);
    wait(NULL);
    wait(NULL);
    exit(EXIT_SUCCESS);
}

// falta refactorizar pero andá a cagar