#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales
#include <asm-generic/signal-defs.h>
#include <sys/stat.h> //para fstat()
#include <time.h> //para nanosleep()
#include <sys/mman.h> //mmap

void handlerPadreInt(int sig){
    (void) sig;

}

void handlerHijoHup(int sig){
    (void) sig;

    write(1, "Me voy a mirar crecer las flores desde abajo.",46);
}

void handlerHijoInt(int sig){
    (void) sig;
    write(1, "Dejame Pensarlo...\n", 19);
}

int main(){

    int pipe1[2];
    // accesing 8 bytes in a region of size 4 -> puse pipe[1] en vez de pipe[2]
    pipe(pipe1);

    int pidHijoOCero = fork();

    if (pidHijoOCero != 0){

        close(pipe1[1]);
        signal(SIGINT, handlerPadreInt);

        // sigset_t conjunto;
        // sigset_t conjuntoVacio;

        // // chatgpt
        // sigemptyset(&conjunto);
        // sigemptyset(&conjuntoVacio);
        // sigaddset(&conjunto, SIGCHLD);
        // //ahora el argumento &conjunto es del tipo const sigset_t *__restrict __set
        // sigprocmask(SIG_BLOCK, &conjunto , &conjuntoVacio);

        // struct sigaction actual;
        // sigaction(SIGCHLD, NULL, &actual);
        // sigemptyset(&conjuntoVacio);
        //sigprocmask(SIG_SETMASK, &conjuntoVacio , NULL);  
        signal(SIGCHLD,NULL);
    
        sleep(1); //hace clock_nanosleep en vez de solo nanosleep pero bueno

        //ftat y mmap no se hacen
        
        write(1, "¿Cuál es el significado de la vida?", 38 ); 
        // \n es un byte -> longitud 39
        // letras comunes -> 1 byte
        // caracteres acentuados (á) y ¿? -> 2 bytes
        kill(pidHijoOCero, SIGINT);

        
        char buf[3];

        read(3, &buf, sizeof(buf));
        write(1, "Mirá vos. El significado de la vida es 42.",44);
        //
        write(1, "¡Bang Bang, estás liquidado!\n",31);

        kill(pidHijoOCero, SIGHUP);
            
        // se supone que debo meter el read y writes adentro del SIGINT??
        // porque el read se queda bloqueado y es función no handler-safe
        // si no, para que se usaría el handler de SIGINT del padre???

        // cosas satánicas de manejo de máscaras
    
        sleep(10);

        write(1,"Te voy a buscar en la oscuridad.\n",33);
        close(pipe1[0]);

    }else{

        close(pipe1[0]);
        int parentID = getppid();
        signal(SIGINT, handlerHijoInt);
        signal(SIGHUP, handlerHijoHup);

        // hay cosas de sigpromask que es pecado capital hacerlas

        sleep(5);

        write(1,"Ya sé el significado de la vida.",34);
        //4 es la parte de escritura del pipe?
        write(4,"42",2);
        kill(parentID, SIGINT);

           
        //Como hago para que el hijo espere el SIGHUP si no hay más syscalls? tipo sleep o pause
        // es porque tenía que poner el read(3, ..), los writes y el kill(SIGHUP) en el handler del padre?

        close(pipe1[1]);

    }




}


/*
chatgpt

rt_sigprocmask(SIG_BLOCK, [CHLD],  <... rt_sigprocmask resumed> [], 8) = 0

La función de C que genera ese rastro en strace es sigprocmask().

Si estás trabajando en un entorno multihilo (usando POSIX threads), la función equivalente es pthread_sigmask(). 
Ambas terminan invocando la llamada al sistema rt_sigprocmask en Linux.

¿Qué está pasando exactamente en ese trace?
El prefijo rt_ significa "Real Time", que es la versión moderna de la llamada al sistema para manejar conjuntos de 
señales más grandes. Aquí te detallo los argumentos que viste:

SIG_BLOCK: Indica que las señales en el conjunto deben añadirse a la máscara actual de señales bloqueadas.

[CHLD]: Es el conjunto de señales que se quiere bloquear (en este caso, SIGCHLD, la señal que envía un proceso hijo al terminar).

[]: Es donde se guardaría la máscara de señales previa (en tu caso, parece que el programador pasó NULL o el conjunto anterior estaba vacío).

8: Es el tamaño en bytes del tipo sigset_t (típico en arquitecturas de 64 bits).




sighandler_t is a pointer to a function that takes an integer argument and returns nothing.



que funcion c genera este trace? nanosleep({1, 0}, 0x7ffdd87913d0) = 0

La función de C que genera ese rastro es **`nanosleep()`**.

A diferencia de la función clásica `sleep()`, que solo entiende de segundos enteros, `nanosleep()` permite pausar la ejecución del programa con precisión de nanosegundos.

### Desglose del trace

Cuando ves `nanosleep({1, 0}, 0x7ffdd87913d0) = 0`, el sistema te está diciendo:

* **`{1, 0}`**: Es la estructura `timespec` que indica el tiempo solicitado para dormir. El primer número son **segundos** (`1`) y el segundo son **nanoseconds** (`0`). Es decir, el programa pidió dormir exactamente 1 segundo.
* **`0x7ffdd87913d0`**: Es un puntero a otra estructura `timespec` (el argumento `rem`). Si el proceso es despertado por una señal antes de tiempo, el kernel escribe aquí cuánto tiempo le faltaba por dormir.
* **`= 0`**: Significa que la llamada tuvo éxito y el proceso durmió todo el tiempo solicitado sin interrupciones.

### ¿Cómo se usa en C?

Para poder usarla, necesitas incluir la cabecera `<time.h>`. Aquí tienes el código exacto que generaría ese trace:

> **Un detalle técnico:** En tu rastro de `strace` ves una dirección de memoria (`0x7ffdd87913d0`) en el segundo parámetro en lugar de `NULL`. Eso significa que el programador pasó una variable para capturar el tiempo restante por si acaso la función era interrumpida por una señal, algo así: `nanosleep(&req, &rem);`.

### ¿Por qué se prefiere `nanosleep` sobre `sleep`?
1. **Precisión:** Te deja bajar a nivel de nanosegundos.
2. **Hilos (Threads):** Es segura para usar en entornos multihilo (Thread-safe), a diferencia de algunas implementaciones antiguas de `sleep`.
3. **Señales:** Si una señal interrumpe a `nanosleep`, te devuelve `-1` y te dice exactamente cuánto tiempo le faltó por dormir, permitiéndote reanudar la pausa fácilmente si lo deseas.

*/