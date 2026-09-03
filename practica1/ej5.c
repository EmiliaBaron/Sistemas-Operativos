#include <stdio.h>
#include <unistd.h> 
// si no se agrega este include, el sistema asume que fork() es una función cualquiera que devuelve int
#include <stdlib.h>
// este include permite tener las constantes EXIT_SUCCESS y EXIT_FAILURE (para errores)
#include <sys/wait.h> //para los wait()


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
                int statusBart;
                int statusLisa;
                int statusMaggie;

                waitpid(PidBartOrZero, &statusBart, 0);
                waitpid(PidLisaOrZero, &statusLisa, 0);
                waitpid(PidMaggieOrZero, &statusMaggie, 0);

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
    int statusHomero;
    waitpid(pidOrZero, &statusHomero, 0);
    exit(EXIT_SUCCESS);
    }
}
// otro strace strace -f -e trace=%signal,%process,write ./ej5


//Ejercicio 7, waitpid agregado
// emilia@emilia-HP-Laptop-14-dk1xxx:~/Desktop/Sistemas Operativos/Sistemas-Operativos/practica1$ strace -q ./ej5
// execve("./ej5", ["./ej5"], 0x7ffdcc9fdfe8 /* 49 vars */) = 0
// brk(NULL)                               = 0x25c1c000
// brk(0x25c1cd00)                         = 0x25c1cd00
// arch_prctl(ARCH_SET_FS, 0x25c1c380)     = 0
// set_tid_address(0x25c1c650)             = 23907
// set_robust_list(0x25c1c660, 24)         = 0
// rseq(0x25c1cca0, 0x20, 0, 0x53053053)   = 0
// prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
// readlinkat(AT_FDCWD, "/proc/self/exe", "/home/emilia/Desktop/Sistemas Op"..., 4096) = 74
// getrandom("\x21\x81\xad\xa6\x79\xd9\x17\xde", 8, GRND_NONBLOCK) = 8
// brk(NULL)                               = 0x25c1cd00
// brk(0x25c3dd00)                         = 0x25c3dd00
// brk(0x25c3e000)                         = 0x25c3e000
// mprotect(0x4a6000, 20480, PROT_READ)    = 0
// getpid()                                = 23907
// fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x1), ...}) = 0
// write(1, "Soy Abraham, mi pID es:23907\n", 29Soy Abraham, mi pID es:23907
// ) = 29
// clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLDSoy Homero, mi pID es:23908
// , child_tidptr=0x25c1c650) = 23908
// Soy Bart, mi pID es:23909
// wait4(23908, Soy Maggie, mi pID es:23911
// Soy Lisa, mi pID es:23910
// [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 23908
// --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=23908, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
// exit_group(0)                           = ?
// +++ exited with 0 +++

//porqué Maggie se ejecuta antes que Lisa? los procesos son así