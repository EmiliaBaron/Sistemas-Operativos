#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()

//man system

// execl("/bin/sh", "sh", "-c", command, (char *) NULL);

// usa la familia exec para reemplazar el proceso actual por un intérprete de comandos (sh) que ejecuta una instrucción.

// execl(...) → reemplaza el proceso actual (no crea uno nuevo)
// "/bin/sh" → ruta al shell
// "sh" → argv[0] (nombre del programa)
// "-c" → le dice al shell: “ejecutá el siguiente comando como string”
// command → string con el comando que querés ejecutar
// (char *) NULL → fin de argumentos


//es necesario hacer esto también?
// RETURN VALUE
//        The return value of system() is one of the following:

//        •  If command is NULL, then a nonzero value if a shell is available, or 0 if no shell is available.

//        •  If a child process could not be created, or its status could not be retrieved, the return value is -1 and errno is
//           set to indicate the error.

//        •  If a shell could not be executed in the child process, then the return value is as though the child  shell  termi‐
//           nated by calling _exit(2) with the status 127.

//        •  If  all  system  calls succeed, then the return value is the termination status of the child shell used to execute
//           command.  (The termination status of a shell is the termination status of the last command it executes.)

//        In the last two cases, the return value is a "wait status" that can be examined using the macros described  in  wait‐
//        pid(2).  (i.e., WIFEXITED(), WEXITSTATUS(), and so on).

//        system() does not affect the wait status of any other children.


void sistema(const char*arg){

    if (arg == NULL){

        exit(EXIT_FAILURE);

    }

    __pid_t pidOrZero = fork();

    if (pidOrZero == 0) {

        // no existe exec en POSIX
        //exec(arg);
        execl("/bin/sh", "sh", "-c", arg, (char *) NULL);
        exit(EXIT_SUCCESS);

    }else{
        //wait_for_child(pidOrZero);
        int status;
        waitpid(pidOrZero, &status, 0);
        exit(EXIT_SUCCESS);

    }

}

int main(){

    //sistema("mkdir prueba"); CRIMINAL
    sistema("echo hola");
 
    exit(EXIT_SUCCESS);
}