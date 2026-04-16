#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales

/*
SIGPIPE es una señal POSIX enviada a un proceso cuando intenta escribir en una tubería (pipe), 
socket o FIFO que no tiene un proceso lector en el otro extremo
*/

int main(){

    //se crean los pipes antes del fork porque esto evita que se creen dos pipes para el hijo y otros dos para el padre
    //comunicacion bidireccional -> 2 pipes
    int padreAHijo[2];
    int hijoAPadre[2];
	char buf[30];
    pipe(padreAHijo);
    pipe(hijoAPadre);

    __pid_t pidHijoOCero = fork();
   

    if (pidHijoOCero != 0){

        printf("Padre, escribiendo en el pipe\n");

        //maniobras
        close(padreAHijo[0]);
        dup2(padreAHijo[1],1);
        //hay que cerrar descriptores duplicados
        //close(padreAHijo[1]);

        close(hijoAPadre[1]);
        //dup2(oldfd, newfd) duplica oldfd en newfd
        dup2(hijoAPadre[0],0);
        //close(hijoAPadre[0]);

        //cuando ya hubo dup2 se puede usar la salida 1
        write(1, "0" , 2); //son 2 bytes porque es uno para el '0' y otro para el '0\' null que indica
        // que se terminó el string. Hay que escribir en el pipe
    
        //dup2 para que vuelva a escribir en terminal
        dup2(padreAHijo[1],1);

        printf("Padre, leyendo del pipe\n");
        read(0, &buf , 2);
        printf("Mensaje del hijo: \"%s\" \n", buf);

        // printf("Padre, escribiendo en el pipe\n");
        // write(pfds[1],"2", 0);        

        // wait(NULL);

        // printf("Padre, leyendo del pipe\n");
        // read(pfds[0], &buf , 0);
        // printf("Mensaje del hijo: \"%s\" \n", buf);

        wait(NULL);
        exit(EXIT_SUCCESS);

    }else{
        //maniobras
        close(padreAHijo[1]);
        dup2(padreAHijo[0],0);
        close(padreAHijo[0]);

        close(hijoAPadre[0]);
        
        printf("Hijo, leyendo el pipe\n");
        
        //cuando ya hubo dup2 se puede usar la entrada 0
        read(0, buf, 2); //2 bytes en vez de uno para recibir el '0\'
        printf("Mensaje del padre: \"%s\" \n", buf);

        printf("Hijo, escribiendo el pipe\n");
        
        //no se hace este dup2 antes porque el printf va al pipe no a la terminal
        dup2(hijoAPadre[1],1);
        close(hijoAPadre[1]);
        
        write(1, "1" , 2);

        // //int q = sleep(10);
        // //como hacer para que los hijos esperen a sus padres?

        // printf("Hijo, leyendo el pipe\n");
        // read(pfds[0], &buf, 0);
        // printf("Mensaje del padre: \"%s\" \n", buf);

        // printf("Hijo, escribiendo el pipe\n");
        // write(pfds[1], "3" , 0);

        // close(pfds[0]);
        // close(pfds[1]);
        exit(EXIT_SUCCESS);

    }
}