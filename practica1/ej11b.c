#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales

int main(){
    
    int padreAHijo1[2];
    int hijo1AHijo2[2];
    int hijo2APadre[2];
	int numeroPadre = 0;
    int numeroHijo1 = 0;
    int numeroHijo2 = 0;
    pipe(padreAHijo1);
    pipe(hijo1AHijo2);
    pipe(hijo2APadre);


    __pid_t pidHijo1OCero = fork();

    if (pidHijo1OCero != 0){

        __pid_t pidHijo2OCero = fork();

        if(pidHijo2OCero !=0){ //PADREEE

            //maniobras
            close(padreAHijo1[0]);
            close(hijo1AHijo2[0]);
            close(hijo1AHijo2[1]);
            close(hijo2APadre[1]);

            numeroPadre = 0;

            printf("Padre envia a Hijo1 el valor %d\n", numeroPadre);
            fflush(stdout);
            write(padreAHijo1[1], &numeroPadre , sizeof(int)); 

            while(1){

                //printf("Padre, leyendo del pipe\n");
                int bytes_leidos = read(hijo2APadre[0], &numeroPadre , sizeof(int));
                //printf("Mensaje del hijo2:  %d\n", numeroPadre); 
                //printf("\n");

                if(numeroPadre >=50 || (bytes_leidos == 0)) break;

                numeroPadre = numeroPadre + 1;

                printf("Padre envía al Hijo1 el valor %d\n", numeroPadre);
                fflush(stdout);
                write(padreAHijo1[1], &numeroPadre, sizeof(int));
            }

            close(padreAHijo1[1]);
            close(hijo2APadre[0]); //cierro los pipes antes así los hijos detectan que no hay nada que leer y terminan
            
            wait(NULL);
            wait(NULL); //para evitar que hayan procesos zombie

            exit(EXIT_SUCCESS);
        }else{
            //Hijo2

            close(padreAHijo1[0]);
            close(padreAHijo1[1]);
            close(hijo1AHijo2[1]);
            close(hijo2APadre[0]);

            while(1){

                //printf("Hijo2, leyendo el pipe\n");
            
                int bytes_leidos = read(hijo1AHijo2[0], &numeroHijo2, sizeof(int)); 
                //printf("Mensaje de hijo1: %d \n", numeroHijo2);
                //printf("\n");

                if(numeroHijo2 >= 50 || (bytes_leidos == 0)) break;

                numeroHijo2 = numeroHijo2 + 1;

                printf("Hijo2 envía a Padre el valor %d\n", numeroHijo2);
                fflush(stdout);
                write(hijo2APadre[1], &numeroHijo2 , sizeof(int));

            }

            close(hijo1AHijo2[0]);
            close(hijo2APadre[1]);
            exit(EXIT_SUCCESS);

        }

    }else{
        //maniobras
        close(padreAHijo1[1]);
        close(hijo1AHijo2[0]);
        close(hijo2APadre[0]);
        close(hijo2APadre[1]);

        while(1){

            //printf("Hijo1, leyendo el pipe\n");
            
            int bytes_leidos = read(padreAHijo1[0], &numeroHijo1, sizeof(int)); 
            //printf("Mensaje del padre: %d \n", numeroHijo1);
            //printf("\n");

            if(numeroHijo1 >=50 || (bytes_leidos == 0)) break;

            numeroHijo1 = numeroHijo1 + 1;

            printf("Hijo1 envía a Hijo2 el valor %d\n", numeroHijo1);
            fflush(stdout);
            write(hijo1AHijo2[1], &numeroHijo1 , sizeof(int));

        }

        close(padreAHijo1[0]);
        close(hijo1AHijo2[1]);
        exit(EXIT_SUCCESS);

    }
}