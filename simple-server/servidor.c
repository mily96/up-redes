#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 8080 //Puerto de escucha
#define IMAGE_PATH "Dogs.jpg" //Nombre de la imagen que va a mostrar

void handle_client(int client_socket) {
    char buffer[1024] = {0};

    // Recibe la solicitud del cliente
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    printf("Solicitud recibida:\n%s\n", buffer);

    // Verifica si es una solicitud GET para imagen.jpg
    if (strncmp(buffer, "GET /imagen.jpg", 15) == 0) {
        //Intenta abrir el archivo en modo lectura
        int file_fd = open(IMAGE_PATH, O_RDONLY);
        //Si falla devuelve 404 NOT FOUND
        if (file_fd < 0) {
            char not_found[] = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(client_socket, not_found, strlen(not_found), 0);
        } 
        //Si el archivo se encuentra, envia un encabezado HTTP de exito 200 OK y el tipo de contenido
        else {
            char header[128];
            sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n");
            send(client_socket, header, strlen(header), 0);
            //Lee el archivo en bloques de 4096 bytes y lo envia al cliente
            char file_buffer[4096];
            ssize_t bytes;
            while ((bytes = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
                send(client_socket, file_buffer, bytes, 0);
            }
            close(file_fd);
        }
    } 
    //Si la solicitud no es para imagen.jpg, devuelve 404 BAD REQUEST
    else {
        char bad_request[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_socket, bad_request, strlen(bad_request), 0);
    }
    //Cierra la conexion con el cliente
    close(client_socket);
}

int main() {
    //Crea el socket del servidor y define la estructura de direccion
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {0};

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    //Linkea el socket a la direccion y habilita la escucha de hasta 5 conexiones en espera
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Servidor HTTP activo en el puerto %d...\n", PORT);
    //Espera nuevas conexiones y cuando un cliente se conecta, ejecuta handle_client()
    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket > 0) {
            handle_client(client_socket);
        }
    }
    //Finaliza la ejecucion cuando el proceso se detiene
    close(server_socket);
    return 0;
}