#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

//Funcion para manejar cliente
void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    //Recibe datos del cliente y los guarda en el buffer
    char buffer[16] = {0};
    recv(client_socket, buffer, sizeof(buffer), 0);
    //Si recibimos PING, entonces que devuelva PONG
    if (strcmp(buffer, "PING") == 0) {
        send(client_socket, "PONG", 4, 0);
    }
    //Se cierra la conexion con el cliente
    close(client_socket);
    //Finaliza el hilo
    return NULL;
}

int main(int argc, char* argv[]) {
     //Valida si se ingreso el numero correcto de argumentos, de lo contrario devuelve error
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //Convierte el puerto de cadena a entero
    int port = atoi(argv[1]);
    //Se crea un socket server utilizando TCP
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    //Se configura SO_REUSEADDR para reutilizar direcciones
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in server_addr = {0};
    //AF_INET: Familia de direcciones IPV4
    server_addr.sin_family = AF_INET;
    //INADDR_ANY: Acepta conexiones de cualquier IP
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //htons(port): Convierte el puerto al formato de red
    server_addr.sin_port = htons(port);
    //Se linkea el socket a la direccion y puerto configurados
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //El socket escucha con una cola para 5 conexiones
    listen(server_socket, 5);

    //LOOP
    while (1) {
        //malloc: Reserva memoria para el descriptor del socket cliente
        int* client_socket = malloc(sizeof(int));
        //accept: Acepta la conexion y se almacena el descriptor
        *client_socket = accept(server_socket, NULL, NULL);
        //Se crea el hilo para manejar el cliente
        pthread_t thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &attr, handle_client, client_socket);
        pthread_attr_destroy(&attr);
    }

    return 0;
}