#include <stdio.h> //printf
#include <unistd.h> // para fork() 
#include <stdlib.h> // para EXIT_SUCCESS y EXIT_FAILURE
#include <sys/wait.h> //para los wait()
#include <sys/signal.h> // para señales


int main(){

    

    __pid_t pidJuliOrZero = fork();

 if (pidJuliOrZero != 0){
    printf("Soy Juan\n");
    int q = sleep(1);

    wait(NULL);

    __pid_t pidJorgeOrZero = fork();

    if(pidJorgeOrZero == 0){

        printf("Soy Jorge\n");
        int q = sleep(1);
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);


}else{
    printf("Soy Julieta\n");
    int q = sleep(1);

    __pid_t pidJenniOrZero = fork();

    if (pidJenniOrZero == 0){

        printf("Soy Jennifer\n");
        int q = sleep(1);
        exit(EXIT_SUCCESS); //está bien? Parece que Julieta hace exit antes SI


    }

    exit(EXIT_SUCCESS);


}

    // if (pidJuliOrZero == 0){
    //     printf("Soy Julieta\n");

    //     __pid_t pidJenniOrZero = fork();

    //     if (pidJenniOrZero == 0){

    //         printf("Soy Jennifer\n");

    //     }

    //     exit(EXIT_SUCCESS);

    // }
    // else{
    //     int estado;
    //     wait(&estado);

    //     __pid_t pidJorgeOrZero = fork();

    //     if(pidJorgeOrZero == 0){

    //         printf("Soy Jorge\n");
    //     }

    //     exit(EXIT_SUCCESS);
    // }




}

//strace
// execve("./ej10", ["./ej10"], 0x7ffffcb88d18 /* 49 vars */) = 0
// brk(NULL)                               = 0x27009000
// brk(0x27009d00)                         = 0x27009d00
// arch_prctl(ARCH_SET_FS, 0x27009380)     = 0
// set_tid_address(0x27009650)             = 26574
// set_robust_list(0x27009660, 24)         = 0
// rseq(0x27009ca0, 0x20, 0, 0x53053053)   = 0
// prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
// readlinkat(AT_FDCWD, "/proc/self/exe", "/home/emilia/Desktop/Sistemas Op"..., 4096) = 75
// getrandom("\x2a\xe5\x43\xbb\x2b\xc4\x3e\xc1", 8, GRND_NONBLOCK) = 8
// brk(NULL)                               = 0x27009d00
// brk(0x2702ad00)                         = 0x2702ad00
// brk(0x2702b000)                         = 0x2702b000
// mprotect(0x4a6000, 20480, PROT_READ)    = 0
// clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLDstrace: Process 26575 attached
// , child_tidptr=0x27009650) = 26575
// [pid 26575] set_robust_list(0x27009660, 24 <unfinished ...>
// [pid 26574] fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
// [pid 26575] <... set_robust_list resumed>) = 0
// [pid 26574] write(1, "Soy Juan\n", 9 <unfinished ...>                -> POR QUÉ JUAN ESTÁ SIN TERMINAR?
// [pid 26575] fstat(1, Soy Juan                                        -> QUÉ SIGNIFICA ESTO?
//  <unfinished ...>
// [pid 26574] <... write resumed>)        = 9
// [pid 26574] clock_nanosleep(CLOCK_REALTIME, 0, {tv_sec=1, tv_nsec=0},  <unfinished ...>
// [pid 26575] <... fstat resumed>{st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
// [pid 26575] write(1, "Soy Julieta\n", 12Soy Julieta
// ) = 12
// [pid 26575] clock_nanosleep(CLOCK_REALTIME, 0, {tv_sec=1, tv_nsec=0},  <unfinished ...>
// [pid 26574] <... clock_nanosleep resumed>0x7ffe6acf5c10) = 0
// [pid 26575] <... clock_nanosleep resumed>0x7ffe6acf5c10) = 0
// [pid 26574] wait4(-1,  <unfinished ...>
// [pid 26575] clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLDstrace: Process 26576 attached
//  <unfinished ...>
// [pid 26576] set_robust_list(0x27009660, 24) = 0                --> robust list?
// [pid 26576] write(1, "Soy Jennifer\n", 13Soy Jennifer
// ) = 13
// [pid 26575] <... clone resumed>, child_tidptr=0x27009650) = 26576
// [pid 26576] clock_nanosleep(CLOCK_REALTIME, 0, {tv_sec=1, tv_nsec=0},  <unfinished ...>
// [pid 26575] exit_group(0)               = ?
// [pid 26575] +++ exited with 0 +++
// [pid 26574] <... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 26575
// [pid 26574] --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=26575, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
// [pid 26574] clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLDstrace: Process 26577 attached
// , child_tidptr=0x27009650) = 26577
// [pid 26577] set_robust_list(0x27009660, 24 <unfinished ...>
// [pid 26574] exit_group(0 <unfinished ...>
// [pid 26577] <... set_robust_list resumed>) = 0
// [pid 26574] <... exit_group resumed>)   = ?
// [pid 26577] write(1, "Soy Jorge\n", 10 <unfinished ...>
// [pid 26574] +++ exited with 0 +++
// Soy Jorge
// [pid 26577] <... write resumed>)        = 10
// [pid 26577] clock_nanosleep(CLOCK_REALTIME, 0, {tv_sec=1, tv_nsec=0},  <unfinished ...>
// [pid 26576] <... clock_nanosleep resumed>0x7ffe6acf5c10) = 0
// [pid 26577] <... clock_nanosleep resumed>0x7ffe6acf5c10) = 0
// [pid 26577] exit_group(0)               = ?
// [pid 26577] +++ exited with 0 +++
// exit_group(0)                           = ?
// +++ exited with 0 +++
