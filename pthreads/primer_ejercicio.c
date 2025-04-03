//EJERCICIO

//Crear una aplicación de línea de comando en C que lea del STDIN un keypress e imprima el
//key code en un thread.

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Función que ejecutará el thread N
void *print_key(void *arg) {
    char *key = (char *)arg;
    printf("Key pressed: %c (Key code: %d)\n", *key, *key);
    return NULL;
}

int main() {
    char key;
    pthread_t thread;


    
    printf("Insertar key:\n");

    
    // Leer un keypress desde STDIN
    if (read(STDIN_FILENO, &key, 1) == -1) {
        perror("Error reading keypress");
        return 1;
    }


    // Crear un thread para imprimir el key code
    if (pthread_create(&thread, NULL, print_key, &key) != 0) {
        perror("Error creating thread");
        return 1;
    }
    
    
    // Esperar a que el thread termine
    if (pthread_join(thread, NULL) != 0) {
        perror("Error joining thread");
        return 1;
    }

    return 0;
}