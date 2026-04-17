#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales

int main(){

    int pipeLsAWc[2]; 
    //pipe siempre son ints
    pipe(pipeLsAWc);

    __pid_t pidLSOCero = fork();

    if(pidLSOCero != 0){ //Proceso shell

        __pid_t pidWcOCero = fork();

        if(pidWcOCero == 0){ //proceso wc

            close(pipeLsAWc[1]);
            dup2(pipeLsAWc[0], 0);
            close(pipeLsAWc[0]);

            //como ya hice dup2 no se debe hacer un read manual
            //read(0, &lineasParaContar, sizeof(lineasParaContar)); 

            //execlp mal hechos
            //execlp("ls", "ls", NULL);
            //char cantLineas = execvp("wc", {"wc", "-l", NULL});

            //se repite el "wc" poruqe es como el programa wc se nombra a sí mismo
            char *args[] = {"wc", "-l", NULL};
            execvp("wc", args);

            //printf(cantLineas); //argumento debe ser tipo string (char *)

            exit(EXIT_SUCCESS);

        }else{
            // si no se coloca esto se cuelga
            // creo que es porque si no wc no detecta cuando ls terminó de escribir, porque el sistema operativo no manda endOfFile
            //porque hay referencias hacia el descriptor
            close(pipeLsAWc[0]);
            close(pipeLsAWc[1]);

            wait(NULL);
            wait(NULL);

            exit(EXIT_SUCCESS);
        }

    
    }else{ //proceso ls

        close(pipeLsAWc[0]);
        //dup2(oldfd, newfd) duplica oldfd en newfd
        dup2(pipeLsAWc[1],1);
        //hay que cerrar descriptores duplicados
        close(pipeLsAWc[1]);

        //execvp no devuelve salida del comando, solo retorna si hay error. Y sus argumentos se deben escribir así.
        //char lineas = execvp("ls", {"ls", "-al", NULL});

        char *args[] = {"ls", "-al", NULL};

        execvp("ls", args);

        //lineas tiene que ser un buffer, en vez de char.
        //como ya se hizo dup2 no se escribe manualmente
        //write(1, lineas, sizeof(lineas));

        exit(EXIT_SUCCESS);

    }

}