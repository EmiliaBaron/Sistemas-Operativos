#include <unistd.h> 
#include <stdio.h> // para printf

int main() {

    printf("Hola SO!\n");
    fork();

    //al hacer un fork, los dos procesos hacen los mensajes de abajo y por lo tanto se repiten

    printf("Nos vemos!\n");
    return 0;
}