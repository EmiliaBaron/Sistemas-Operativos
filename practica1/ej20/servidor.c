#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> // wait

#include <string.h> // para strcpy
#include <sys/socket.h> // para los sockets
#include <sys/un.h> // para el sockaddr_un
#include <poll.h> //para poll
#include <errno.h> 

#define MAX_CLIENTS 6
#define MAX_PROC 3
#define BACKLOG 64

#define READ 0
#define WRITE 1

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

int chequear_es_primo(int numero, int cota_inf, int cota_sup){

    for(int i = cota_inf; i<=cota_sup; i++){
        if(numero % i == 0 && i != 1 && i != numero){
            return 0;
        }

    }

    return 1;

}

void handle_client(int fd_cliente){

    int numero;
    ssize_t bytes_recieved = recv(fd_cliente, &numero, sizeof(int), 0);

    if(bytes_recieved<0){
        if (errno == EINTR)
        exit(EXIT_FAILURE);
    }	

    if (bytes_recieved == 0)
        exit(EXIT_SUCCESS);      /* el cliente cerró prolijamente */

    
    
    int pipes[3][2];
    
    for (int i = 0; i<3; i++){
        pipe(pipes[i]);
    }

    long rango = (long) numero / 3; //-> debería guardar un entero
    long cota_inf = 1;
    long cota_sup = rango;

    for(int i =0; i<3; i++){
        int hijo = fork();

        if (hijo == 0){

            for(int j = 0; j<3; j++){
                if(j != i){
                    close(pipes[j][READ]);
                    close(pipes[j][WRITE]);
                }

            }   
            close(pipes[i][READ]);
            int res = 0;

            if(i != 2){
                res = chequear_es_primo(numero, cota_inf, cota_sup);
            }
            if (i == 2){
                res = chequear_es_primo(numero, cota_inf, numero);
            }

            write(pipes[i][WRITE], &res, sizeof(res));
            printf("estoy calculando el rango %ld hasta %ld del numero %d con resultado %d\n", cota_inf, cota_sup, numero, res);
            fflush(stdout);
            close(pipes[i][WRITE]);
            exit(EXIT_SUCCESS);

        }
        cota_inf += rango;
        cota_sup +=rango;
    }

    for(int j = 0; j<3; j++ ){
        close(pipes[j][WRITE]);
    }

    int res1;
    int res2;
    int res3;

    read(pipes[0][READ], &res1, sizeof(res1));
    read(pipes[1][READ], &res2, sizeof(res1));
    read(pipes[2][READ], &res3, sizeof(res1));
    
    int es_primo = res1 && res2 && res3;
    printf("resultado 1 = %d, resultado2 = %d, resutlado3 = %d\n", res1, res2, res3);
    fflush(stdout);
    printf("le mando al cliente que el resultado es %d\n", es_primo);
    fflush(stdout);


    for (int i = 0; i<2; i++){
        wait(NULL);
    }

    send(fd_cliente, &es_primo, sizeof(es_primo), 0);
    
    close(fd_cliente);
    exit(EXIT_SUCCESS);
}



int main(){

    int listen_fd = crear_server_socket("servidor");

    for(;;){

        int res = accept(listen_fd, NULL, NULL); // por qué NULL NULL?

        if(res!=-1){
			//Hacemos un fork para que el proceso que responde al cliente i corra en otro proceso y no colgar al padre.
			pid_t pid= fork();
			if(pid==0){
				//Cerramos el FD del padre en el contexto del hijo ya que no lo va a usar.
				close(listen_fd);
				//Lo que hace el servidor para gestionar cada cliente.
				handle_client(res);
				exit(EXIT_SUCCESS);
			}
			//Cerramos el descriptor resultante de la conexión. El child se hace cargo de todo. 
            //Al padre no le interesa. No lo va a usar.
			close(res);
		}
    }   
    close(listen_fd);
    exit(EXIT_SUCCESS);
}

