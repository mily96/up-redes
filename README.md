<h1 align="center">
  <br>
  <a href="https://www.palermo.edu"><img src="https://www.palermo.edu/images/header/logo@2x.png" alt="up logo" width="130"></a>
  <br>
  TP Programación en Redes 
</h1>


Repositorio del TP integrador Programacion en Redes
<br>
**Alumno: Milena Venditto**
</br>
**Profesor: Diego Marafetti**



## Requerimientos 🚀

- Descargar e instalar [Visual Studio Code](https://code.visualstudio.com/Download)
- Descargar e instalar [Docker](https://www.docker.com/). Esto puede variar según el sistema operativo.
- Instalar las siguiente extensiones de VSCode
  - C/C++
  - C/C++ Extension Pack
  - CMake
  - CMake Tools
  - Docker
  - Dev Containers

## Entornos de Desarrollo

> [!NOTE]
> Se recomienda la configuración utilizando Docker.

### Docker

Se provee un docker container con todas las tools necesarias para compilación y debugging de aplicaciónes. Para instalar el entorno ejecutar el docker compose detached en el root del proyecto. 

```bash
 docker compose up --build -d
```

Una vez finalizado se puede conectarse por TTY al container con el siguiente comando:

```bash
 docker exec -it ubuntu-redes bash
```



## Entregas
### Iteracion 1
1) Compilar el servidor desde una terminal: **gcc -o servidor servidor.c -pthread**
2) Compilar el cliente desde otra terminal: **gcc -o cliente cliente.c**
3) Ejecutar el servidor: **./servidor 8080**
4) Ejecutar el cliente: **./cliente 127.0.0.1 8080**
### Iteracion 2
1) Compilar el servidor desde una terminal: **gcc -o servidor servidor.c**
2) Compilar el cliente desde otra terminal: **gcc -o cliente cliente.c**
3) Ejecutar el servidor: **./servidor 8080**
4) Ejecutar el cliente: **./cliente 127.0.0.1 8080**
### Iteracion 3
1) Compilar el servidor desde una terminal: **gcc -o servidor servidor.c -lpthread**
2) Compilar el cliente desde otra terminal: **gcc -o cliente cliente.c**
3) Ejecutar el servidor: **./servidor 8080**
4) Ejecutar el cliente: **./cliente 127.0.0.1 8080**
5) Extra => Ejecutar desde otra terminal: **telnet 127.0.0.1 8080** o **nc 127.0.0.1 8080** y escribir PING y devolvera PONG
### Iteracion 4
1) Compilar el servidor desde una terminal: **gcc -o servidor servidor.c**
3) Ejecutar el servidor: **./servidor 8080**
4) Ir al navegador y poner la siguiente url: **localhost:8080/imagen.jpg**, debera aparecer la siguiente imagen

<img src="./simple-server/Dogs.jpg">