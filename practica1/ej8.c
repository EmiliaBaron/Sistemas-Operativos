#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE


int main(int argc, char const *argv[]) {
    int dato = 0;
    __pid_t pid = fork();

    //si no hay error, pid vale 0 para el hijo
    //y el valor del process id del hijo para el padre
    if (pid == -1) {
        //si es -1, hubo un error
        exit(EXIT_FAILURE);
    }

    else if (pid == 0) {
        for (int i = 0; i < 3; i++) {
            dato++;
            printf("Dato hijo: %d\n", dato);
        }
    }
    else {
        for (int i = 0; i < 3; i++) {
            printf("Dato padre: %d\n", dato);
        }
    }

    exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}