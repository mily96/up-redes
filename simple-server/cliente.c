#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    //Valida si se ingresaron 2 argumentos ademas del nombre del programa, de lo contrario devuelve error
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <Puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //Creacion del socket
    //AF_INET: Familia de direcciones IPV4
    //SOCK_STREAM: Tipo de socket para conexiones TCP
    //0: Por default TCP
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    //Se declara e inicializa en 0 server_addr para almacenar la direccion del servidor
    struct sockaddr_in server_addr = {0};
    //Se establece la familia de direcciones en AF_INET
    server_addr.sin_family = AF_INET;
    //Convierte el puerto proporcionado como argumento de texto a numero entero mediante la funcion atoi y la transforma al formato de red mediante la funcion htons
    server_addr.sin_port = htons(atoi(argv[2]));
    //Convierte la direccion IP proporcionada de texto a binario mediante la funcion inet_pton y lo almacena en sin_addr
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    //Se establece la conexion
    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //Envia PING al servidor
    send(client_socket, "PING", 4, 0);
    printf("Enviando PING al servidor\n");
    //Recibe la respuesta del servidor y lo almacena en response
    char response[16] = {0};
    recv(client_socket, response, sizeof(response), 0);
    //Imprime la respuesta del servidor
    printf("Respuesta del servidor: %s\n", response);
    //Se cierra el socket para liberar recursos
    close(client_socket);
    return 0;
}