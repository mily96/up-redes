#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(int argc, char* argv[]) {
    //Valida si se ingresaron 2 argumentos ademas del nombre del programa, de lo contrario devuelve error
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //Creacion del socket
    //AF_INET: Familia de direcciones IPV4
    //SOCK_STREAM: Tipo de socket para conexiones TCP
    //0: Por default TCP
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    //Si falla la creacion, devuelve error y termina el programa
    if (client_socket < 0) {
        perror("Error al crear socket");
        return EXIT_FAILURE;
    }
    //Se declara e inicializa en 0 server_addr para almacenar la direccion del servidor
    struct sockaddr_in server_addr = {0};
    //Se establece la familia de direcciones en AF_INET
    server_addr.sin_family = AF_INET;
    //Convierte el puerto proporcionado como argumento de texto a numero entero mediante la funcion atoi y la transforma al formato de red mediante la funcion htons
    server_addr.sin_port = htons(atoi(argv[2]));
    //Convierte la direccion IP proporcionada de texto a binario mediante la funcion inet_pton y lo almacena en sin_addr
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    //Intenta conectar el socket con el servidor, si falla muestra error y termina programa
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en connect");
        close(client_socket);
        return EXIT_FAILURE;
    }

    printf("Conectado al servidor. Escriba 'PING' para probar.\n");
    //Declara un conjunto de descriptores de archivos que seran monitoreados por select
    fd_set readfds;
    //LOOP
    while (1) {
        //FD_ZERO: Limpia el conjunto de descriptores de archivo
        //FD_SET(STDIN_FILENO, &readfds): Agrega la entrada estandar para monitorear el teclado
        //FD_SET(client_socket, &readfds): Agrega el socket del servidor para monitorear datos recibidos
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(client_socket, &readfds);

        int activity = select(client_socket + 1, &readfds, NULL, NULL, NULL);
        //Si ocurre un error, devuelve mensaje y termina loop
        if (activity < 0) {
            perror("Error en select");
            break;
        }
        //FD_ISSET: Verifica si se ingreso algo por teclado
        //fgets: Lee el msj ingresado
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char buffer[16];
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // Elimina el salto de línea para evitar problemas con el envio
            //Envia el msj al servidor
            send(client_socket, buffer, strlen(buffer), 0);
        }
        //FD_ISSET: Verifica si el servidor envio alguna respuesta
        if (FD_ISSET(client_socket, &readfds)) {
            char buffer[16] = {0};
            //Recibe la respuesta del servidor
            int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes <= 0) {
                printf("Conexión cerrada por el servidor.\n");
                break;
            }
            //Imprime la respuesta del servidor
            printf("Respuesta del servidor: %s\n", buffer);
        }
    }
    //Se cierra el socket para liberar recursos
    close(client_socket);
    return 0;
}