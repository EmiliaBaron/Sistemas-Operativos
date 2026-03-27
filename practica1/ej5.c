#include <stdio.h>
#include <unistd.h> 
// si no se agrega este include, el sistema asume que fork() es una función cualquiera que devuelve int
#include <stdlib.h>
// este include permite tener las constantes EXIT_SUCCESS y EXIT_FAILURE (para errores)


int ejecutar_proceso_homero(){

    __pid_t pIDHom = getpid();
        printf("Soy Homero, mi pID es:%d\n", pIDHom);
        __pid_t PidBartOrZero = fork();

        if (PidBartOrZero == 0){
            //parece que en PidBartOrZero homero recibe un número distinto de cero

            __pid_t pIDBart = getpid();
            printf("Soy Bart, mi pID es:%d\n", pIDBart);
            exit(EXIT_SUCCESS);

        }else {

            __pid_t PidLisaOrZero = fork();
             if (PidLisaOrZero == 0){
                //en PidLisaOrZero Homero recibe un número distinto de cero, que es el pid de Lisa

                __pid_t pIDLisa = getpid();
                printf("Soy Lisa, mi pID es:%d\n", pIDLisa);
                exit(EXIT_SUCCESS);

            }else {

                __pid_t PidMaggieOrZero = fork();

                if(PidMaggieOrZero == 0){
                     __pid_t pIDMaggie = getpid();
                    printf("Soy Maggie, mi pID es:%d\n", pIDMaggie);
                    exit(EXIT_SUCCESS);

                }
                
                //estas funciones no existen en Linux, pero se pondrían acá para hacer el ejercicio b
                //wait_for_child(PidBartOrZero);
                //wait_for_child(PidLisaOrZero);
                //wait_for_child(PidMaggieOrZero);
                exit(EXIT_SUCCESS);
                
            }

        }

    
}


int main()
{   
    __pid_t pIDAbr = getpid();
    printf("Soy Abraham, mi pID es:%d\n", pIDAbr);
    
    __pid_t pidOrZero = fork();
    
    if (pidOrZero == 0){
       ejecutar_proceso_homero();
    }else{
    // no existe wait_for_child() en Linux
    //wait_for_child(pidOrZero);
    exit(EXIT_SUCCESS);
    }
}