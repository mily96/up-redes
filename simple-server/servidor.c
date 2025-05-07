#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10 //Define 10 como numero maximo de clientes

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
    if (server_socket < 0) {
        perror("Error al crear socket");
        return EXIT_FAILURE;
    }
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
    //Se linkea el socket a la direccion y puerto configurados, si falla devuelve error y cierra socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_socket);
        return EXIT_FAILURE;
    }
    //Coloca el socket en listen permitiendo recibir conexiones
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error en listen");
        close(server_socket);
        return EXIT_FAILURE;
    }
    //Estructura para el conjunto de descriptores de archivo monitoreados
    fd_set readfds;
    //Almacena los sockets de los clientes conectados
    int client_sockets[MAX_CLIENTS] = {0};
    //Almacena el valor del descriptor de archivo mas alto
    int max_fd = server_socket;

    printf("Servidor escuchando en el puerto %d...\n", port);

    //LOOP
    while (1) {
        //Limpia el conjunto de descriptores de archivos
        FD_ZERO(&readfds);
        //Agrega el socket del servidor para monitorear nuevas conexiones
        FD_SET(server_socket, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
            }
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }
        //Mediante select se bloquea la ejecucion hasta que haya actividad en algun descriptor, si hay error se termine el loop
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error en select");
            break;
        }

        if (FD_ISSET(server_socket, &readfds)) {
        //Mediante accept acepta la conexion entrante
            int new_socket = accept(server_socket, NULL, NULL);
            if (new_socket < 0) {
                perror("Error en accept");
                continue;
            }

            printf("Nueva conexión aceptada: %d\n", new_socket);
            //Guarda el nuevo cliente en el primer espacio disponible
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }
        //Verifica si algun cliente envio datos
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {
                char buffer[16] = {0};
                int bytes = recv(client_sockets[i], buffer, sizeof(buffer), 0);

                if (bytes <= 0) {
                    printf("Cliente desconectado: %d\n", client_sockets[i]);
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } else {
                    printf("Recibido de %d: %s\n", client_sockets[i], buffer);
                 //Si recibimos PING, entonces que devuelva PONG
                    if (strcmp(buffer, "PING") == 0) {
                        send(client_sockets[i], "PONG", 4, 0);
                    }
                }
            }
        }
    }
    //Cierra el socket del servidor antes de finalizar
    close(server_socket);
    return 0;
}
