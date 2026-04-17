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

        if(pidHijo2OCero !=0){

            printf("Padre, escribiendo en el pipe\n");

            //maniobras
            close(padreAHijo1[0]);
            close(hijo1AHijo2[0]);
            close(hijo1AHijo2[1]);
            close(hijo2APadre[1]);

            numeroPadre = 0;

            write(padreAHijo1[1], &numeroPadre , sizeof(int)); 

            while(1){

                printf("Padre, leyendo del pipe\n");
                read(hijo2APadre[0], &numeroPadre , sizeof(int));
                printf("Mensaje del hijo2:  %d\n", numeroPadre); 
                printf("\n");

                if(numeroPadre >=50) break;

                numeroPadre = numeroPadre + 1;

                printf("Padre, escribiendo en el pipe\n");
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

                printf("Hijo2, leyendo el pipe\n");
            
                read(hijo1AHijo2[0], &numeroHijo2, sizeof(int)); 
                printf("Mensaje de hijo1: %d \n", numeroHijo2);
                printf("\n");

                if(numeroHijo2 >= 50) break;

                numeroHijo2 = numeroHijo2 + 1;

                printf("Hijo2, escribiendo el pipe\n");
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

        while(numeroHijo1 <50){

            printf("Hijo1, leyendo el pipe\n");
            
            read(padreAHijo1[0], &numeroHijo1, sizeof(int)); 
            printf("Mensaje del padre: %d \n", numeroHijo1);
            printf("\n");

            if(numeroHijo1 >=50) break;

            numeroHijo1 = numeroHijo1 + 1;

            printf("Hijo1, escribiendo el pipe\n");
            write(hijo1AHijo2[1], &numeroHijo1 , sizeof(int));

        }
        
        // printf("Hijo, leyendo el pipe\n");
        // read(padreAHijo[0], &numero, 2);
        // printf("Mensaje del padre: \"%s\" \n", numero);
        // printf("\n");

        // printf("Hijo, escribiendo el pipe\n");
        // write(hijoAPadre[1], "3" , 2);

        close(padreAHijo1[0]);
        close(hijo1AHijo2[1]);
        exit(EXIT_SUCCESS);

    }
}