# Sustentación os-2022-10-u3-ev1-Andercq17

## Video en Youtube

- https://youtu.be/d7OsSFgr0T8

## ¿Vectores cumplidos?
Se han cumplido todos los vectores, excepto el vector 14, debido que, aunque el comando exit realiza correctamente la función de terminar tanto el servidor como los clientes y destruye la cola de mensajes, este también, detecta un smash de stack.

## ¿Cómo se compila la aplicación?

Como el archivo del servidor implementa codigo de otros archivos y a su vez estos implementan de otros *.h, entonces, se tiene que generar un archivo *.o para cada uno de ellos, y a su vez se tiene que realizar un ejecutable en donde se comprimen todos los *.o generados en un solo archivo, que es el que se va a ejecutar con todos los codigos implementados, este en el programa es finalmente llamado como el archivo principal "server".

Por lo tanto, primero hay que identificar los archivos que hay que hacer *.o, estos serían las clases que se van a implementar, en este caso las que se usan para un solo codigo son, Event, EventList y Server. y como "server" hace uso de hilos, entonces estos en su codigo de compilación debe vincular lpthread. Siendo así, los comandos de compilación quedarían de la siguiente manera:

1. gcc -Wall -c event.c -o event.o
2. gcc -Wall -c eventList.c -o eventList.o
3. gcc -Wall -c server.c -o server.o -lpthread

A su vez, se tienen que reunir todos los archivos *.o en un solo ejecutable, así:

1. gcc -Wall server.o event.o eventList.o -o servidor -lpthread

También existe un archivo del cliente, llamado client.c, este se compila normalmente pero guardando la vinculación de lpthread ya que este también hace uso de hilos.

1. gcc -Wall client.c -o client -lpthread

## ¿Cómo se ejecuta el codigo?

Después de realizar la compilación del codigo correctamente, significa que en nuestra carpeta quedó dos archivos ejecutables, uno llamado server y el otro client, estos dos archivos se ejecutan en el siguiente orden: 

1. primero el servidor se ejecuta así: "./server"
2. segundo el cliente se ejecuta así: "./client"

## ¿Cómo se resolvió cada requisito de la aplicación?


### cliente
Se generó un listado de los codigos con los cuales se van a identificar los mensajes enviados desde el cliente al servidor, siendo estos los siguientes:

### Tipo   ->   Petición_mensaje
###   2     ->       sub
###   3     ->       unsub
###   4     ->       list
###   5     ->       ask
###   10    ->       exponer id

Cuando se ejecuta la aplicación del cliente, entonces, el cliente va a conectarse a la cola de mensajes que creó el servidor y va a exponer su id de proceso padre, este con el mensaje estructurado así:

Tipo = 10
Mensaje = Hola padre
id_proceso = id de proceso padre propio

Y ejecuta un hilo que es el encargado de escuchar los que se le está escribiendo en la terminal, comandos como sub, unsub, list y ask, que luego por cada comando escrito se va a generar un mensaje hacia el servidor con las mismas estructuras, así:

-------------------------------------------------------

//sub
Tipo = 2 //
Mensaje = "evento al cual quiere suscribirse" //
id_proceso = id de proceso padre propio

-------------------------------------------------------

//unsub
Tipo = 3 //
Mensaje = "evento al cual quiere desuscribirse" //
id_proceso = id de proceso padre propio

-------------------------------------------------------

//list
Tipo = 4 //
Mensaje = list //
id_proceso = id de proceso padre propio

-------------------------------------------------------

//ask
Tipo = 5 //
Mensaje = ask //
id_proceso = id de proceso padre propio

---------------------------------------------------------------------------------------

Dentro del hilo que se ejecutó se va a ejecutar otro hilo el cual es el encargado de leer los mensajes que le envía el servidor, se queda pendiente a la cola de mensajes por si existe uno dentro de la cola dirigido a el cliente en especifico. El servidor lo unico que va a hacer es responder a las peticiones que el cliente le ha enviado, y por consiguiente el cliente va a exponer en su terminal lo que el servidor le está diciendo.

### ¿Cómo sabe el cliente que los mensajes van dirigidos a el?.

Esto es sencillo, el servidor lo que va a hacer para responder a las peticiones de los clientes en especifico, es que va a enviar mensajes marcados en los que su tipo es el id de proceso del que va a recibir, es por esto que al ejecutar cada cliente, ellos exponen su id de proceso enviando un mensaje tipo 10, para que el servidor sepa quienes se están comunicando con el y los almacene en una lista, además, también es por esto que cuando cada cliente envía un mensaje se va con un long donde dice quién envió la petición y qué es lo que quiere.

Entonces, en resumen, el cliente sabe que los mensajes van dirigidos a el porque el solo va a agarrar los mensajes de la cola en que en su tipo está su numero de id de proceso.



### servidor

El servidor cuando se ejecuta crea una lista enlazada vacía, una lista de tipos long en donde almacenará los id expuestos por los clientes, va a guardar el id de la cola de mensajes para poder enviar mensajes satisfactoriamente y a su vez, creará la cola de mensajes, esta la crea con una llave unica generada en el directorio actual. Una vez generada la cola de mensajes, entonces va a tener los mismos procedimientos que el cliente, va a tener un hilo que va a escuchar lo que se le escribe por terminal, y dentro de este va a generar otro hilo que se va a encargar de leer los mensajes en la cola.

### El hilo que se encarga de leer los mensajes en la cola funciona de la siguiente manera:

Se va a establecer que se va a recibir mensajes con el parametro de -10 en el tipo de mensaje a recibir, esto va a hacer que va a recibir los mensajes marcados con tipo de 10 hasta tipo 1, con este parametro estamos asegurando que va a recibir todas las peticiones enviadas por los clientes.

Una vez lea un mensaje va a mirar de qué tipo es el mensaje, y a partir del tipo se generan diferentes acciones:


(Hay que tener en claro que cada evento creado tiene una lista de clientes suscritos, esta lista es de tipo long ya que allí se guardará los id de los que se han suscrito)

//sub
- si es tipo 2, entonces va a buscar si el evento al que se quiere suscribir el cliente existe, si existe va a adicionar el id del cliente a la lista de clientes del evento, y si no existe envía un mensaje al cliente de que ese evento no existe.

//unsub
- si es tipo 3, se procede a hacer proceso parecido al de sub, pero lo que se va a hacer es que se va a buscar el evento, si no existe envía mensaje de respuesta de que el evento no existe, y si existe el evento, entonces analizará la lista de clientes suscritos al evento, si después de recorrer la lista se da cuenta de que el id del cliente no está, entonces enviará un mensaje en respuesta diciendo que no se ha suscrito aún a ese evento. Si en la lista de clientes suscritos al evento, si se encuentra la suscripción del cliente a ese evento, entonces se procede a borrar es id dentro de la lista.

//list (listar eventos a los cuales está suscrito)
- si es tipo 4, entonces se va a recorrer todos los eventos dentro de la lista enlazada, y mientras recorre cada uno va a recorrer la lista de clientes suscritos, si encuentra el id, entonces va a almacenar en un arreglo de char los nombres de los eventos en los que se encuentra dentro de la lista de clientes suscritos. Cuando se termine de recorrer todos los eventos, entonces tantos mensajes como eventos dentro de la lista de char van a ser enviados al que hizo la petición. Cada mensaje enviado dirá el nombre del evento en el cual va la iteración.

//ask
- si es de tipo 5, lo unico que va a hacer es recorrer la lista de eventos, y por cada evento recorrido va a enviar un mensaje al que hizo la petición con el nombre del evento.

### El hilo que va a escuchar la linea de comandos va hacer lo siguiente:

Este hilo al leer la linea de comando va a diferenciar 6 comandos especificos, los cuales son los siguientes:

//add (este comando va acompañado de un argumento, el cual va a hacer el nombre del evento que se quiere crear)
- agarra el nombre del argumento y utiliza la función "AddEvent" de la clase "EventList", la cual recibe dos parametros, la lista a la cual se va a añadir el evento y el nombre del evento, por lo cual solo se le pasa como parametros la lista enlazzada creada y el nombre del argumento. Dentro de la función simplemente se van enlazando los eventos con el nuevo evento añadido el cual tiene como nombre el argumento ingresado en terminal.

//remove (este comando va acompañado de un argumento, el cual va a hacer el nombre del evento que se quiere eliminar)
- Se agarra el argumento (nombre evento) ingresado y se llama la función "RemoveEvent" de la clase "EventList", la cual recibe dos parametros, la lista de eventos y el nombre del evento a ingresar que en este caso es el tomado de argumento de la linea de comando. Dentro de la función lo unico que se va a hacer es recorrer la lista de eventos y una vez encuentre el evento con el nombre que se ingresó, se va a tomar el evento anterior y su sigueinte dentro de la lista va a ser el siguiente del evento que se va a remover.

//trigger (este comando va acompañado de un argumento, el cual va a hacer el nombre del evento que se quiere publicar)
- Este evento no va a llamar ninguna funcion en especifico, solo va a tomar el argumento de la consola el cual contiene el nombre del evento a publicar y va a recorrer la lista de eventos, una vez encuentra el evento con el nombre que se ingresó, va a realizar una iteración por la lista que contiene los id de los clientes y va a generar un mensaje por cliente iterado, en cada uno de esos mensajes su tipo va a hacer el long que se está iterando y en el mensaje dirá el nombre del evento que se publicó.

//list (este comando va acompañado de un argumento, el cual va a hacer el nombre del evento que se quiere mostrar los clientes suscritos)
- En este se va a iterar la lista de eventos, y cuando se encuentre el evento en el que el nombre es igual al ingresado como argumentos, entonces se va a iterar la lista de clientes suscritos de ese evento y se va a imprimir el long dentro de la lista en cada iteración.

//listar
- Este comando mostrará por terminar del servidor los eventos que se han añadido a la lista de eventos, esto con ayuda de una función en "EventList.c", la cual es "ListEvent", la cual va a iterar los eventos e imprimir los nombres.

//exit
- cuando se ingrese este comando, iterará la lista de longs donde se encuentran los id de procesos de los clientes, y por cada iteración enviará un mensaje marcado en su tipo con el long de la iteración para enviarselo a todos los clientes, todos los mensajes que se envíen en esta iteración tendrán como mensaje "exit" para que los clientes sepan que se va a destruir la cola de mensajes. cuando se terminen de enviar los mensajes se procede a destruir la lista de eventos y la cola de mensajes, y termina el programa. (Dentro del cliente, cuando recibe un mensaje que diga "exit", procederá a destruir su extremo de la cola y terminará el programa).







