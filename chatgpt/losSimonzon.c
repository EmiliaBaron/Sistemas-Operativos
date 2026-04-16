#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    // Declaración de pipes: [0] es para lectura, [1] es para escritura
    int pipe_lester[2];
    int pipe_eliza[2];
    
    // Variables para los IDs de los procesos
    pid_t pid_lester, pid_eliza;

    // 1. Humberto crea los pipes ANTES de tener a los hijos
    // para que ellos hereden los descriptores de archivo.
    if (pipe(pipe_lester) == -1 || pipe(pipe_eliza) == -1) {
        perror("Humberto falló al crear los pipes");
        exit(EXIT_FAILURE);
    }

    // 2. Humberto crea a su primer hijo: Lester
    pid_lester = fork();

    if (pid_lester < 0) {
        perror("Error al crear a Lester");
        exit(EXIT_FAILURE);
    }

    if (pid_lester == 0) {
        /* ---------------- PROCESO LESTER ---------------- */
        // Cerramos los extremos de los pipes que Lester no va a usar
        close(pipe_lester[0]); // Lester no lee de su propio pipe
        close(pipe_eliza[0]);  // Lester no usa el pipe de Eliza
        close(pipe_eliza[1]);

        // Inicializamos la semilla aleatoria usando el PID para asegurar aleatoriedad
        srand(time(NULL) ^ getpid());
        
        int dado = (rand() % 6) + 1; // Dado del 1 al 6
        
        // Lester le cuenta a Humberto (Escribe en el pipe)
        write(pipe_lester[1], &dado, sizeof(int));
        
        close(pipe_lester[1]); // Cerramos escritura al terminar
        exit(EXIT_SUCCESS);    // Lester termina su turno
    }

    // 3. Humberto crea a su segunda hija: Eliza
    pid_eliza = fork();

    if (pid_eliza < 0) {
        perror("Error al crear a Eliza");
        exit(EXIT_FAILURE);
    }

    if (pid_eliza == 0) {
        /* ---------------- PROCESO ELIZA ---------------- */
        // Cerramos los extremos de los pipes que Eliza no va a usar
        close(pipe_eliza[0]);  // Eliza no lee de su propio pipe
        close(pipe_lester[0]); // Eliza no usa el pipe de Lester
        close(pipe_lester[1]);

        // Semilla aleatoria
        srand(time(NULL) ^ getpid());
        
        int dado = (rand() % 6) + 1; // Dado del 1 al 6
        
        // Eliza le cuenta a Humberto (Escribe en el pipe)
        write(pipe_eliza[1], &dado, sizeof(int));
        
        close(pipe_eliza[1]); // Cerramos escritura al terminar
        exit(EXIT_SUCCESS);   // Eliza termina su turno
    }

    /* ---------------- PROCESO HUMBERTO (PADRE) ---------------- */
    
    // Humberto solo va a leer, así que cierra los extremos de escritura
    close(pipe_lester[1]);
    close(pipe_eliza[1]);

    int valor_lester, valor_eliza;

    // Humberto espera a que ambos hijos terminen de jugar (sincronización)
    wait(NULL);
    wait(NULL);

    // Humberto escucha a sus hijos (Lee de los pipes)
    read(pipe_lester[0], &valor_lester, sizeof(int));
    read(pipe_eliza[0], &valor_eliza, sizeof(int));

    // Cierra los extremos de lectura ya que no los necesita más
    close(pipe_lester[0]);
    close(pipe_eliza[0]);

    // Anuncios en stdout
    printf("Humberto revisa los dados...\n");
    printf("- Lester sacó un: %d\n", valor_lester);
    printf("- Eliza sacó un: %d\n\n", valor_eliza);

    // Humberto determina al ganador
    if (valor_lester > valor_eliza) {
        printf("Humberto grita: ¡EL GANADOR ES LESTER!\n");
    } else if (valor_eliza > valor_lester) {
        printf("Humberto grita: ¡LA GANADORA ES ELIZA!\n");
    } else {
        printf("Humberto grita: ¡ES UN EMPATE!\n");
    }

    return 0;
}