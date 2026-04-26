socketsClienteYServidor

Cliente

Este código en C representa un **cliente** básico que utiliza **Unix Domain Sockets** (IPC - Inter-Process Communication). A diferencia de los sockets de red (que usan IP y puertos), estos sockets se comunican dentro de la misma máquina utilizando un archivo en el sistema de archivos como punto de encuentro.

Aquí tienes el desglose paso a paso:

---

## 1. Configuración de la dirección (`struct sockaddr_un`)
```c
server_addr.sun_family = AF_UNIX;
strcpy(server_addr.sun_path, "unix_socket");
```
* **`AF_UNIX`**: Especifica que la familia de protocolos es para comunicación local (Unix).
* **`sun_path`**: Define la "dirección" del servidor, que en este caso es un archivo llamado `"unix_socket"` en el directorio actual.

## 2. Creación del Socket
```c
server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
```
* Se crea el punto de conexión. 
* **`SOCK_STREAM`** indica que la conexión será orientada a flujo (como TCP), lo que garantiza que los datos lleguen en orden y sin errores.

## 3. Conexión al Servidor
```c
if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Error");
    exit(1);
}
```
* El cliente intenta conectarse al servidor que está "escuchando" en el archivo `unix_socket`. 
* Si el servidor no está encendido o el archivo no existe, el programa imprime el error y se cierra.



## 4. Intercambio de Datos
```c
int num = 42;
write(server_socket, &num, sizeof(num)); // Envía 42
read(server_socket, &num, sizeof(num));  // Espera respuesta
printf("Cliente: recibí %d del servidor!\n", num);
```
Aquí ocurre la "magia" de la comunicación:
1.  **`write`**: Envía el número entero `42` binariamente a través del socket.
2.  **`read`**: Se bloquea (espera) hasta que el servidor envíe algo de vuelta. El valor recibido se guarda en la misma variable `num`.
3.  **`printf`**: Muestra el resultado final.

## 5. Finalización
```c
close(server_socket);
exit(0);
```
* Se cierra el descriptor del socket para liberar los recursos del sistema operativo y el programa termina exitosamente.

---

### Resumen Técnico
| Función | Propósito |
| :--- | :--- |
| **`socket()`** | Crea el "teléfono" para la comunicación. |
| **`connect()`** | "Marca el número" del servidor usando la ruta del archivo. |
| **`write()` / `read()`** | Envía y recibe datos (E/S de bajo nivel). |
| **`AF_UNIX`** | Indica que la comunicación es interna en el host local. |



servidor 

---

## 1. Preparación del terreno: `unlink`
```c
unlink(server_addr.sun_path);
```
Esta línea es **crucial** en Unix Domain Sockets. En Linux/Unix, el socket es un archivo físico. Si el programa se cerró mal antes o el archivo ya existe, la función `bind` fallará. `unlink` borra cualquier rastro del archivo `"unix_socket"` antes de intentar crearlo de nuevo.

## 2. El ritual de inicio (Bind y Listen)
Para que un servidor funcione, debe seguir estos pasos:
1.  **`socket()`**: Crea el punto final de comunicación.
2.  **`bind()`**: Amarra el socket a una dirección específica (el archivo `unix_socket`). Aquí es donde el archivo aparece realmente en tu carpeta.
3.  **`listen()`**: Pone al servidor en "modo espera". El `1` indica que solo permitiremos una conexión en la cola de espera.



## 3. El ciclo infinito (`while(1)`)
El servidor entra en un bucle para estar siempre disponible:

* **`accept()`**: El servidor se queda "dormido" (bloqueado) en esta línea hasta que un cliente intenta conectarse. Cuando alguien llega, `accept` crea un nuevo socket (`client_socket`) específico para hablar con ese cliente.
* **`recv()`**: Recibe el número que envió el cliente. Es equivalente a `read`.
* **El proceso**: Toma el número y lo duplica (`num *= 2`).
* **`send()`**: Envía el resultado de vuelta al cliente. Es equivalente a `write`.
* **`close(client_socket)`**: Despide al cliente y cierra esa conexión específica, pero el servidor sigue vivo (vuelve al inicio del `while`) para esperar al siguiente.

---

## Resumen del Flujo Completo

| Cliente | Servidor |
| :--- | :--- |
| Crea socket. | Crea socket y hace `bind` al archivo. |
| Ejecuta `connect()`. | Ejecuta `listen()` y luego `accept()`. |
| **Envía 42** con `write()`. | **Recibe 42** con `recv()`. |
| Espera... | Multiplica $42 \times 2 = 84$. |
| **Recibe 84** con `read()`. | **Envía 84** con `send()`. |
| Cierra y termina. | Cierra el socket del cliente y vuelve a `accept()`. |

---

### ¿Cómo probarlo?
1.  **Compila ambos**: 
    `gcc servidor.c -o servidor`
    `gcc cliente.c -o cliente`
2.  **Ejecuta el servidor en una terminal**: `./servidor` (se quedará esperando).
3.  **Ejecuta el cliente en otra terminal**: `./cliente`.

Verás que el cliente envía un 42 y, casi instantáneamente, recibe un 84. Es una forma extremadamente rápida de comunicar dos programas en la misma computadora. 



## por qué el cliente usa write y read en vez de recv y send?

Esa es una excelente pregunta técnica. La respuesta corta es: **en Unix/Linux, un socket es un tipo de archivo**, y por lo tanto, las funciones estándar de lectura y escritura de archivos funcionan perfectamente con ellos.

---

## 1. La filosofía "Todo es un archivo"
En los sistemas operativos basados en Unix, casi cualquier cosa con la que el sistema operativo interactúa (teclado, pantalla, archivos en disco, tuberías y **sockets**) se maneja a través de un **Descriptor de Archivo** (un simple número entero).

* Tanto `read()` como `write()` son llamadas al sistema genéricas que funcionan con cualquier descriptor de archivo válido.
* Como `socket()` devuelve un descriptor de archivo, puedes usar las herramientas estándar de entrada/salida (I/O).

## 2. La equivalencia funcional
Cuando usas `send` y `recv` con el último parámetro (flags) en **0**, son exactamente lo mismo que `write` y `read`.

$$write(fd, buf, len) \equiv send(fd, buf, len, 0)$$
$$read(fd, buf, len) \equiv recv(fd, buf, len, 0)$$



## 3. Entonces, ¿cuándo usar cada uno?

| Función | ¿Cuándo usarla? | Ventajas |
| :--- | :--- | :--- |
| **`read` / `write`** | Cuando buscas simplicidad o portabilidad con otros tipos de I/O (archivos, pipes). | Código más genérico y familiar para programadores de C estándar. |
| **`recv` / `send`** | Cuando necesitas **control extra** sobre la transmisión de red. | Permite usar **flags** (banderas) especiales. |

### ¿Qué permiten los "flags" de `send` y `recv`?
Si el código usara `send` o `recv`, podría pasar opciones adicionales que `write` o `read` no tienen, por ejemplo:
* **`MSG_OOB`**: Para enviar o recibir datos "fuera de banda" (prioritarios).
* **`MSG_PEEK`**: Para mirar los datos que han llegado sin quitarlos de la cola (como ver quién llama sin contestar).
* **`MSG_DONTWAIT`**: Para que la operación no se bloquee si no hay datos listos.

---

## En resumen
En tu código, el autor probablemente usó `read` y `write` porque:
1.  **Es un Unix Domain Socket:** Al ser una comunicación local (dentro de la misma máquina), a menudo se trata conceptualmente más como una "tubería" (pipe) entre archivos que como una conexión de red compleja.
2.  **Simplicidad:** No se necesitan opciones especiales de red, así que las funciones básicas son suficientes.


#causa SIGPIPE

El SIGPIPE aparece porque:

un proceso cierra el socket
el otro intenta escribir igual
el kernel lo mata con SIGPIPE
