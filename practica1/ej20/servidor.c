#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> // wait

#include <string.h> // para strcpy
#include <sys/socket.h> // para los sockets
#include <sys/un.h> // para el sockaddr_un
#include <poll.h> //para poll

#define MAX_CLIENTS 6
#define MAX_PROC 3


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
    struct sockaddr_un server_addr;
    socklen_t slen = sizeof(server_addr);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, nombreSocket);
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 6);

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
    // si se manda uno atrás de otro llegan echos percha porque se leen como misma secuencia de bytes, es decir, 
    // no se diferencia un mensaje de otro y llegan mal

    send(servidor_proc, &datos_para_proc, sizeof(datos_para_proc), 0);
    // send(servidor_proc, &cota_inf, sizeof(cota_inf),0);
    // send(servidor_proc, &cota_sup, sizeof(cota_sup),0);
    // send(servidor_proc, &numero_cliente, sizeof(numero_cliente), 0);

}

int main(){

   //COMIENZO PROCESOS

    // hacer que los hijos creen sus propios servidores y esperen a accept mientras que el padre paralelamente intenta
    // conectarse a los servidores crea una condición de carrera
    //chatgpt
    //2. Condición de Carrera en la Conexión
    //En el main, los hijos llaman a crear_server_socket y el padre inmediatamente intenta conectarse_al_servidor.
    //Aunque usaste un while(connect(...) == -1) sleep(1), si el hijo aún no llegó al accept, la conexión queda pendiente.
    //En el strace se ve que el Servidor principal está enviando datos (sendto) a los descriptores de los hijos, 
    //pero si un hijo está ocupado o no leyó en el orden exacto, el buffer se llena.

    int servidor_proc1 = crear_server_socket("servidor_proc1");
    int servidor_proc2 = crear_server_socket("servidor_proc2");
    int servidor_proc3 = crear_server_socket("servidor_proc3");


    __pid_t proceso1OCero = fork();

    if(proceso1OCero == 0){  //Proceso 1


        realizar_tarea_proceso(servidor_proc1, 1);

        exit(EXIT_SUCCESS);

    }else{

        __pid_t proceso2OCero = fork();

        if(proceso2OCero == 0){ //Proceso 2

            realizar_tarea_proceso(servidor_proc2, 2);

            exit(EXIT_SUCCESS);

        }else{

            __pid_t proceso3OCero = fork();

            if(proceso3OCero == 0){ //Proceso 3

                realizar_tarea_proceso(servidor_proc3,3);

                exit(EXIT_SUCCESS);

            }else{

                //FIN PROCESOS

                int i;
                long potencial_primo;
                //int mensaje;
                struct sockaddr_un client_addr;
                socklen_t clen = sizeof(client_addr);

                // conecto los tres procesos
                int cliente_servidor_proc1 = conectarse_al_servidor("servidor_proc1");
                int cliente_servidor_proc2 = conectarse_al_servidor("servidor_proc2");
                int cliente_servidor_proc3 = conectarse_al_servidor("servidor_proc3");

                int server_socket = crear_server_socket("servidor");

                struct pollfd fds[MAX_CLIENTS + 1];
                // El índice 0 será para el socket del servidor (nuevas conexiones)
                fds[0].fd = server_socket;
                fds[0].events = POLLIN; // Queremos saber cuando hay datos de entrada
                
                // Inicializar el resto de los fds como vacíos (-1 es ignorado por poll)
                for (i = 1; i <= MAX_CLIENTS; i++) fds[i].fd = -1;

                //meto los otros sockets en 1 2 y 3
                // esto hace que el poll atienda a todos y no recurrir a lecturas y escrituras bloqueantes
                fds[1].fd = cliente_servidor_proc1;
                fds[1].events = POLLIN;
                
                fds[2].fd = cliente_servidor_proc2;
                fds[2].events = POLLIN;

                fds[3].fd = cliente_servidor_proc3;
                fds[3].events = POLLIN;

                //matriz de registro de respuestas de clientes
                // me sobran 3 espacios pero a la mierda todo
                int respuestas_clientes[MAX_CLIENTS + 1][4];

                //pongo -1 en todo para diferenciar resultado de nada
                for (i =0; i <= MAX_CLIENTS; i++){
                    for(int j = 0; j <= 3; j++) respuestas_clientes[i][j] = -1;
                }

                printf("Servidor: esperando conexión del cliente...\n");

                int a = 0;
                while(a <=2) {
            
                    poll(fds, MAX_CLIENTS + 1, 5);


                    if (fds[0].revents & POLLIN) {
                        int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
                        printf("Nueva conexión aceptada (FD: %d)\n", client_socket);

                        // Añadir el nuevo socket al array de fds
                        for (i = 1; i <= MAX_CLIENTS; i++) {
                            if (fds[i].fd == -1) {
                                fds[i].fd = client_socket;
                                fds[i].events = POLLIN;
                                break;
                            }
                        }
                    }

                    // Revisar si algún cliente envió datos
                    for (i = 1; i <= MAX_CLIENTS; i++) {
                        if (fds[i].fd > 0 && (fds[i].revents & POLLIN)) {

                            //mensaje de procesos
                            if (fds[i].fd == cliente_servidor_proc1 || fds[i].fd == cliente_servidor_proc2 || fds[i].fd == cliente_servidor_proc3 ){ 

                                respuesta_proceso respuesta;

                                // no se usan pipes porque su read y write son bloqueantes, y nosotros queremos que el 
                                //servidor pueda atender varios clientes a la vez
                                int respuesta_para_cliente;

                                int recibido = recv(fds[i].fd, &respuesta, sizeof(respuesta), 0);
                                if(recibido <= 0){
                                    printf("Error en el mensaje del proceso de índice %d", i);

                                }
                            
                                if(i == 1) respuestas_clientes[respuesta.indice_cliente][1] = respuesta.es_potencial_primo;
                                if(i == 2) respuestas_clientes[respuesta.indice_cliente][2] = respuesta.es_potencial_primo;
                                if(i == 3) respuestas_clientes[respuesta.indice_cliente][3] = respuesta.es_potencial_primo;

                                printf("Servidor: respuesta del proceso%d para %ld = %d\n", i, respuesta.potencial_primo, respuesta.es_potencial_primo);

                                    
                                if (respuestas_clientes[respuesta.indice_cliente][1] != -1 &&
                                respuestas_clientes[respuesta.indice_cliente][2] != -1 &&
                                respuestas_clientes[respuesta.indice_cliente][3] != -1){

                                    respuesta_para_cliente = respuestas_clientes[respuesta.indice_cliente][1] 
                                                            && respuestas_clientes[respuesta.indice_cliente][2] 
                                                            && respuestas_clientes[respuesta.indice_cliente][3];

                                    send(fds[respuesta.indice_cliente].fd, &respuesta_para_cliente, sizeof(respuesta_para_cliente), 0);
                                    
                                    //reseteo las respuestas para que no se mezcle dos pedidos de un cliente
                                    for(int m = 0; m <= 3; m++ ) respuestas_clientes[respuesta.indice_cliente][m] = -1;

                                }
                                

                            }else{

                                int valread = recv(fds[i].fd, &potencial_primo, sizeof(potencial_primo),0);

                                if (valread == 0) {
                                    // El cliente se desconectó
                                    printf("Cliente desconectado (FD: %d)\n", fds[i].fd);
                                    close(fds[i].fd);
                                    fds[i].fd = -1;
                                
                                }else{ 
                                    //mensaje de cliente
                                    printf("Recibido de cliente indice %d: %ld\n", i, potencial_primo);

                                    // *calculo de primo*

                                    long rango = potencial_primo / 3;
                                    long cota_inf = 1;
                                    long cota_sup = rango;

                                    mandar_a_servidor_proceso(cliente_servidor_proc1, potencial_primo, cota_inf, cota_sup, i);

                                    cota_inf = rango + 1;
                                    cota_sup = rango + rango;

                                    mandar_a_servidor_proceso(cliente_servidor_proc2, potencial_primo, cota_inf, cota_sup, i);

                                    cota_inf = rango + rango + 1;
                                    cota_sup = potencial_primo;

                                    mandar_a_servidor_proceso(cliente_servidor_proc3, potencial_primo, cota_inf, cota_sup, i);

                                } 

                            }
                        }
                    }
                    


                    a++;
                }
                close(server_socket);

                wait(NULL);
                wait(NULL);
                wait(NULL);

                exit(EXIT_SUCCESS);

            }
        }
    }

}