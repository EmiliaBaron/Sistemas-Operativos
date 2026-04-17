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
        close(hijoAPadre[1]);

        write(padreAHijo[1], "0" , 2); //son 2 bytes porque es uno para el '0' y otro para el '0\' null que indica
        // que se terminó el string. Hay que escribir en el pipe

        printf("Padre, leyendo del pipe\n");
        read(hijoAPadre[0], &buf , 2);
        printf("Mensaje del hijo: \"%s\" \n", buf); // \"%s\" a diferencia de otros, necesita de el '0\'
        printf("\n");

        printf("Padre, escribiendo en el pipe\n");
        write(padreAHijo[1],"2", 2);        

        wait(NULL); //según chatgpt esto está mal ubicado

        printf("Padre, leyendo del pipe\n");
        read(hijoAPadre[0], &buf , 2);
        printf("Mensaje del hijo: \"%s\" \n", buf);
        printf("\n");

        close(padreAHijo[1]);
        close(hijoAPadre[0]);
        exit(EXIT_SUCCESS);

    }else{
        //maniobras
        close(padreAHijo[1]);
        close(hijoAPadre[0]);
        
        printf("Hijo, leyendo el pipe\n");
        
        read(padreAHijo[0], buf, 2); //2 bytes en vez de uno para recibir el '0\'
        printf("Mensaje del padre: \"%s\" \n", buf);
        printf("\n");

        printf("Hijo, escribiendo el pipe\n");
        write(hijoAPadre[1], "1" , 2);

        printf("Hijo, leyendo el pipe\n");
        read(padreAHijo[0], &buf, 2);
        printf("Mensaje del padre: \"%s\" \n", buf);
        printf("\n");

        printf("Hijo, escribiendo el pipe\n");
        write(hijoAPadre[1], "3" , 2);

        close(padreAHijo[0]);
        close(hijoAPadre[1]);
        exit(EXIT_SUCCESS);

    }
}