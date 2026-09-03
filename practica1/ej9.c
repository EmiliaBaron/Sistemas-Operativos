#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales

volatile sig_atomic_t flag = 0;
// volatile -> Le dice al compilador que no optimice esta variable asumiendo que no cambia sola. Porque la variable
// flag puede cambiar fuera del flujo normal del programa. 
// Es modificada por el handler de señal, no solo por el código principal

// sig_atomic_t -> Es un tipo de dato definido en <signal.h> que garantiza que 
//las operaciones sobre esa variable son atómicas. Es decir, No puede quedar “a medio escribir” si llega una señal.
// Es seguro modificarla dentro de un handler de señales.

// Manejador de señal
void handler(int sig) {
    (void) sig;
    // se pone como parámetro sig aunque no se use porque es como la firma que usan las funciones que manejan señales
    //según la API
    flag = 1;
}

int main(){

    // Asociar el handler a SIGUSR1, que es una señal puesta para que la defina el usuario
    signal(SIGUSR1, handler);

    __pid_t pidOrZero = fork();
    char opcion = 'n';

    while(opcion != 's' && opcion != 'S' ){

        if (pidOrZero == 0){

            pid_t ppid = getppid();

            for (int i = 0; i < 3; i++) {
                // Espera señal del padre
                while (!flag);
                flag = 0;

                __pid_t pidHijo = getpid();
                printf("pong, mi pID es: %d\n", pidHijo);        

                // Avisar al padre
                kill(ppid, SIGUSR1);
            }

        }else{

            for (int i = 0; i < 3; i++) {

                __pid_t pidPadre = getpid();
                printf("ping, mi pID es: %d\n", pidPadre);     

                // Avisar al hijo
                kill(pidOrZero, SIGUSR1);

                // Espera señal del hijo
                while (!flag);
                flag = 0;
            }

            printf("¿Desea finalizar el programa? (s/n): ");
            scanf(" %c", &opcion);  // espacio antes de %c evita problemas con enter

        }
    }

    kill(pidOrZero, SIGKILL);
    wait(NULL);
    exit(EXIT_SUCCESS);

}