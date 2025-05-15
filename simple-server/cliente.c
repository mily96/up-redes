#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HEARTBEAT_PORT 9999 //Puerto al que el cliente enviara HEARTBEAT
#define HEARTBEAT_INTERVAL 5 // Intervalo en segundos entre cada HEARTBEAT
#define MAX_ATTEMPTS 3        // Intentos antes de considerar caída del servidor

int main(int argc, char* argv[]) {
    //Valida si se ingreso la IP del servidor, de lo contrario devuelve error
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <IP del servidor>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //Creacion socket UDP
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    //Si falla la creacion, devuelve error y termina el programa
    if (udp_socket < 0) {
        perror("Error al crear socket UDP");
        return EXIT_FAILURE;
    }

    //Se establece un timeout de 5 segs para evitar bloqueos en recvfrom() mejorando la deteccion de fallos
    struct timeval timeout;
    timeout.tv_sec = 5; 
    timeout.tv_usec = 0;
    setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    //Se declara e inicializa en 0 server_addr para almacenar la direccion del servidor
    struct sockaddr_in server_addr = {0};
    //Se establece la familia de direcciones en AF_INET
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HEARTBEAT_PORT);
    //Define la dirección del servidor, el puerto y convierte la IP de texto a binario
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int attempts = 0; //Inicializa contador en 0
    //LOOP
    while (1) {
        //Envia HEARTBEAT al servidor
        printf("Enviando mensaje: '%s' (Tamaño: %lu bytes)\n", "HEARTBEAT", strlen("HEARTBEAT"));
        sendto(udp_socket, "HEARTBEAT", 9, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        printf("Heartbeat enviado al servidor.\n");


        char buffer[16] = {0};

        struct sockaddr_in from_addr;
        socklen_t addr_len = sizeof(from_addr);

        int bytes = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from_addr, &addr_len);
        buffer[bytes] = '\0';
        printf("Bytes recibidos: %d, Mensaje: %s\n", bytes, buffer);

        //Si recibo OK, el servidor esta operativo
        if (bytes > 0 && strcmp(buffer, "OK") == 0) {
            printf("✅ Servidor operativo.\n");
            attempts = 0;
        //No hubo respuesta y se va incrementando el contador
        } else if (bytes <= 0) {
            attempts++;
     
            printf("Sin respuesta del servidor (Intento %d)\n", attempts);
        }
        //Si supera la cantidad de intentos devuelve este mensaje
        if (attempts >= MAX_ATTEMPTS) {

            printf("⚠️ No hay respuesta del servidor tras %d intentos. Posible caída.\n", MAX_ATTEMPTS);
        }

        sleep(HEARTBEAT_INTERVAL); //Pausa el programa por HEARTBEAT_INTERVAL segundos antes de enviar el siguiente HEARTBEAT
    }
    //Se cierra el socket
    close(udp_socket);
    return 0;
}