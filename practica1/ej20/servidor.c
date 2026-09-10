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

typedef struct {
    long potencial_primo;
    long cota_inf;
    long cota_sup;
    int cliente;

} datos_para_proceso;

typedef struct {
    int indice_cliente;
    long potencial_primo;
    int es_potencial_primo;
}respuesta_proceso;


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

void realizar_tarea_proceso(int servidor_del_proceso, int numero_proceso){

    datos_para_proceso datos;
    respuesta_proceso respuesta;
    int client_socket;
    int cliente_prendido = 1; 
    //int socket_servidor = conectarse_al_servidor("servidor");
    //printf("Proceso%d: conectado al servidor");

    struct sockaddr servidor_addr;
    socklen_t sevidorlen = sizeof(servidor_addr);

    //servidor_proc = crear_server_socket(nombre_servidor_del_proceso);
    client_socket = accept(servidor_del_proceso, (struct sockaddr *) &servidor_addr, &sevidorlen);
    printf("Proceso%d: servidor%d aceptado.\n", numero_proceso, numero_proceso);



    while(cliente_prendido){
        respuesta.es_potencial_primo = 1;

        cliente_prendido = recv(client_socket, &datos, sizeof(datos), 0);
        if(cliente_prendido == 0) break;
        if(datos.cota_inf == 0){
            printf("Proceso%d: cota inferior del rango es igual a cero", numero_proceso);
            break;
        }

        respuesta.potencial_primo = datos.potencial_primo;
        respuesta.indice_cliente = datos.cliente;

        // cliente_prendido = recv(client_socket, &potencial_primo, sizeof(potencial_primo), 0);
        // // hay que validar que cliente_prendido != 0 en cada lectura
        // if(cliente_prendido == 0) break;
        // cliente_prendido = recv(client_socket, &cota_inferior_rango, sizeof(cota_inferior_rango), 0);
        
        // if(cota_inferior_rango == 0){
        //     printf("Proceso%d: cota inferior del rango es igual a cero", numero_proceso);
        //     break;
        // }
        
        // if(cliente_prendido == 0) break;
        // cliente_prendido = recv(client_socket, &cota_superior_rango, sizeof(cota_superior_rango), 0);
        // if(cliente_prendido == 0) break;
        // cliente_prendido = recv(client_socket, &numero_cliente, sizeof(numero_cliente), 0);
        // if(cliente_prendido == 0) break;

        for (int i = datos.cota_inf; i <= datos.cota_sup; i++){

            if (datos.potencial_primo % i == 0 && i != 1 && i != datos.potencial_primo){
                respuesta.es_potencial_primo = 0;
                break;
            }
        }

        //write(pipe[1], &es_potencial_primo, sizeof(es_potencial_primo));

        send(client_socket, &respuesta, sizeof(respuesta), 0);
    }   

    //close(servidor_proc);
    close(client_socket);

}

void mandar_a_servidor_proceso(int servidor_proc, long potencial_primo, long cota_inf, long cota_sup, int numero_cliente){

    datos_para_proceso datos_para_proc;

    datos_para_proc.potencial_primo = potencial_primo;
    datos_para_proc.cliente = numero_cliente;
    datos_para_proc.cota_inf = cota_inf;
    datos_para_proc.cota_sup = cota_sup;

    //se debe mandar un send o recv con un struct con todos los datos para asegurarse que llegan completos;
    // si se manda uno atrás de otro llegan hechos percha porque se leen como misma secuencia de bytes, es decir, 
    // no se diferencia un mensaje de otro y llegan mal

    send(servidor_proc, &datos_para_proc, sizeof(datos_para_proc), 0);
    // send(servidor_proc, &cota_inf, sizeof(cota_inf),0);
    // send(servidor_proc, &cota_sup, sizeof(cota_sup),0);
    // send(servidor_proc, &numero_cliente, sizeof(numero_cliente), 0);

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

