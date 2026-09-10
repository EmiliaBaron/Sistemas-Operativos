#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> // wait

#include <string.h> // para strcpy
#include <sys/socket.h> // para los sockets
#include <sys/un.h> // para el sockaddr_un
#include <poll.h> //para poll
#include <errno.h> 

#define MAX_CLIENTS 3
#define MAX_PROC 3
#define BACKLOG 64

#define READ 0
#define WRITE 1
#define MAX_LINE 4096

int crear_server_socket(char* nombreSocket){

    int server_socket;
    struct sockaddr_un server_addr; //yo hago sockaddr_un en vez de sockaddr_in

    strcpy(server_addr.sun_path, nombreSocket);
    unlink(server_addr.sun_path);  // desvincula el socket -> es necesario?
    
    // strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if (server_socket < 0) {
		perror("error de socket");
		return EXIT_FAILURE;
	}
   
    socklen_t slen = sizeof(server_addr);
    server_addr.sun_family = AF_UNIX;

    if (bind(server_socket, (struct sockaddr *) &server_addr, slen)){
    	perror("error del bind");
		return EXIT_FAILURE;
	}
    
    if (listen(server_socket, BACKLOG)){ //hasta BACKLOG conexiones entrantes
    	perror("error del listen");
		return EXIT_FAILURE;
	}

    return server_socket;
}

int conectarse_al_servidor(char* nombreSocket){

    int servidor_socket;
    struct sockaddr_un servidor_addr;

    servidor_addr.sun_family = AF_UNIX;
    strcpy(servidor_addr.sun_path, nombreSocket);

    servidor_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    while (connect(servidor_socket, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1) {
        sleep(1);
    }

    return servidor_socket;

}

void handle_client(int fd_cliente, int num_proc, int pipes[][2], int cant_procesos){

    // cerramos todos los pipes que no use
    int proc_anterior = (num_proc+MAX_CLIENTS-1) %MAX_CLIENTS;

    for(int i= 0; i<MAX_CLIENTS; i++){
        if(i != num_proc && i != proc_anterior){
            close(pipes[i][READ]);
            close(pipes[i][WRITE]);
        } 
    }   
    close(pipes[proc_anterior][WRITE]);
    close(pipes[num_proc][READ]);


    char mensaje[MAX_LINE];
    ssize_t bytes_recieved = recv(fd_cliente, &mensaje, sizeof(mensaje), 0);

    write(pipes[num_proc][WRITE], &mensaje, sizeof(mensaje));

    if(bytes_recieved<0){
        if (errno == EINTR)
        exit(EXIT_FAILURE);
    }	

    if (bytes_recieved == 0)
        exit(EXIT_SUCCESS);      /* el cliente cerró prolijamente */

    
    char mensaje_proc_ant[MAX_LINE];

    read(pipes[proc_anterior][READ], &mensaje_proc_ant, sizeof(mensaje_proc_ant));
    send(fd_cliente, &mensaje_proc_ant, sizeof(mensaje_proc_ant), 0);
    
    close(fd_cliente);
    close(pipes[proc_anterior][READ]);
    close(pipes[num_proc][WRITE]);
    exit(EXIT_SUCCESS);
}



int main(){

    int listen_fd = crear_server_socket("servidor");

    int pipes[MAX_CLIENTS -1][2];

    for (int i = 0; i<MAX_CLIENTS; i++){
        pipe(pipes[i]);
    }

    int clientes_atendidos_numProc = 0;

    for(;;){

        if(clientes_atendidos_numProc < MAX_CLIENTS){
            int res = accept(listen_fd, NULL, NULL); // por qué NULL NULL?

            if(res!=-1){
                clientes_atendidos_numProc++;

                //Hacemos un fork para que el proceso que responde al cliente i corra en otro proceso y
                // no colgar al padre.
                pid_t pid= fork();
                if(pid==0){
                    //Cerramos el FD del padre en el contexto del hijo ya que no lo va a usar.
                    close(listen_fd);

                    //Lo que hace el servidor para gestionar cada cliente.
                    handle_client(res, clientes_atendidos_numProc, pipes);

                    exit(EXIT_SUCCESS);
                }
                //Cerramos el descriptor resultante de la conexión. El child se hace cargo de todo. 
                //Al padre no le interesa. No lo va a usar.
                close(res);
            }

        }else{
            wait(NULL);
            clientes_atendidos_numProc--;

        }




    }   
    close(listen_fd);
    exit(EXIT_SUCCESS);
}

