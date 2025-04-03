#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>

// Función que se ejecuta en el thread N del proceso hijo
void *printKey(void *arg) {
    char key = *((char *)arg);
    pthread_t threadId = pthread_self();
    
    // Imprime el key press y su código ASCII
    printf("Thread %ld: Key pressed '%c' (Code: %d)\n", (long)threadId, key, key);
    return NULL;
}

int main(int argc, const char *argv[]) {
    char key;
    pid_t pid;
    
    printf("Insertar key: ");
    
    // Leer un keypress desde STDIN
    if (read(STDIN_FILENO, &key, 1) <= 0) {
        perror("Error reading key");
        return 1;
    }
    
    // Crear un proceso hijo utilizando fork()
    pid = fork();
    if (pid < 0) {
        perror("Fork error");
        return 1;
    }
    
    if (pid == 0) {  // Proceso hijo
        pthread_t thread;
        // Creamos un thread para imprimir el key code
        if (pthread_create(&thread, NULL, printKey, &key) != 0) {
            perror("Error creating thread");
            exit(1);
        }
        
        // Esperamos a que el thread termine
        if (pthread_join(thread, NULL) != 0) {
            perror("Error joining thread");
            exit(1);
        }
        
        // Termina el proceso hijo
        exit(0);
    } else {  // Proceso padre
        // Esperamos a que el proceso hijo termine
        wait(NULL);
        printf("Main process finished.\n");
    }
    
    return 0;
}
