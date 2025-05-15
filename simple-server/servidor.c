#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>

#define MAX_CLIENTS 10 //Define 10 como numero maximo de clientes
#define HEARTBEAT_PORT 9999  // Puerto UDP para heartbeat

void *heartbeat_server(void *arg) {
    //Crea un socket UDP para enviar y recibir HEARTBEAT del cliente
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("Error al crear socket UDP");
        return NULL;
    }
    //Configuracion de direccion y puerto donde el server escuchara
    struct sockaddr_in udp_addr = {0};
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(HEARTBEAT_PORT);

    //Se linkea el socket al puerto, si falla devuelve error y cierra socket
    if (bind(udp_socket, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("Error en bind UDP");
        close(udp_socket);
        return NULL;
    }

    printf("Servidor UDP de heartbeat activo en el puerto %d...\n", HEARTBEAT_PORT);

    //Define buffer para recibir msjs y estructura para almacenar la direccion del cliente
    char buffer[16];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    //LOOP
    while (1) {
        //Recibe un msj UDP del cliente y lo almacena en buffer
        int bytes = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        buffer[bytes] = '\0'; // Agrega terminador nulo
        if (bytes > 0) {
            printf("Mensaje recibido en el servidor UDP (%d bytes): %s\n", bytes, buffer);
        }
        //Si recibimos HEARTBEAT, entonces que devuelva OK
        if (strcmp(buffer, "HEARTBEAT") == 0) {
            printf("Enviando respuesta OK al cliente...\n");  // Depuración
            sendto(udp_socket, "OK", 2, 0, (struct sockaddr*)&client_addr, addr_len);
        }
        

    }

    close(udp_socket);
    return NULL;
}

int main(int argc, char* argv[]) {
    //Valida si se ingreso el numero correcto de argumentos, de lo contrario devuelve error
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    //Crea el socket TCP para aceptar clientes
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear socket");
        return EXIT_FAILURE;
    }

    //Permite reutilizar la direccion evitando errores al reiniciar el servidor
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    //Se linkea el socket al puerto, si falla devuelve error y cierra socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind TCP");
        close(server_socket);
        return EXIT_FAILURE;
    }
    //Coloca el socket en listen permitiendo recibir conexiones
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error en listen");
        close(server_socket);
        return EXIT_FAILURE;
    }

    fd_set readfds;
    int client_sockets[MAX_CLIENTS] = {0};
    int max_fd = server_socket;

    printf("Servidor TCP activo en el puerto %d...\n", port);

    // **Inicia el servidor de heartbeat en un thread**
    pthread_t hb_thread;
    pthread_create(&hb_thread, NULL, heartbeat_server, NULL);
    pthread_detach(hb_thread);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
            }
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("Error en select");
            break;
        }
        //Si el socket del servidor tiene actividad acepta nueva conexion
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket = accept(server_socket, NULL, NULL);
            if (new_socket < 0) {
                perror("Error en accept");
                continue;
            }

            printf("Nueva conexión aceptada: %d\n", new_socket);

            int keepalive = 1;
            setsockopt(new_socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

        //Añade el nuevo cliente a la lista de clientes activos
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }
        //Revisa si algun cliente ha enviado datos
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {
                char buffer[16] = {0};
                int bytes = recv(client_sockets[i], buffer, sizeof(buffer), 0);

                printf("Bytes recibidos: %d\n", bytes);
                printf("Mensaje recibido: '%s'\n", buffer);

                if (bytes <= 0) {
                    printf("Cliente desconectado: %d\n", client_sockets[i]);
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } else {
                    printf("Recibido de %d: %s\n", client_sockets[i], buffer);
                    buffer[strcspn(buffer, "\r\n")] = '\0';
                    //Si el cliente envia PING, el servidor responde PONG
                    if (strcmp(buffer, "PING") == 0) {
                        send(client_sockets[i], "PONG", 4, 0);
                    }
                }
            }
        }
    }

    close(server_socket);
    return 0;
}