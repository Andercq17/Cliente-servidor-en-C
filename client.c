// C Program for Message Queue (Reader Process)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
  
//codigos de mensajes enviados
// sub - 2 
// unsub - 3
// list -4
// ask - 5
// exponer mi id de proceso - 10
//\codigos de mensajes enviados


//codigos de mensajes recibidos
// servidor a todos los clientes - 1
// servidor a mi - proccessid
//\codigos de mensajes enviados


//variables globales
int msgid;  
long processid;
int bandera=0;

//\variables globales

// estructuraa de message queue
struct mesg_buffer {
    long mesg_type;
    char mensaje[100];
    long processid;
};

//recibe los mensajes del servidor
void* leer_mensajes(void *param){ 
    int flag=0;
    while(flag==0){
        struct mesg_buffer mensaje;
        int b=msgrcv(msgid, &mensaje, sizeof(mensaje), processid, 0);
        if(b>0){
            printf("servidor le dice a: %ld, %s\n",mensaje.mesg_type, mensaje.mensaje);
        }
        
        if(strncasecmp(mensaje.mensaje,"exit",strlen("exit"))==0){
            printf("vos sos mrk? disque matandome eche... oh... siento como un... AAAAAAAAAAA MI CORAZON -explota-\n");
            bandera=1;
            break;
        }
    }
    //\ recibir mensajes
} 
//\recibe los mensajes del servidor

//escucha la linea de comandos
void* escuchar(void *param){
    pthread_t threadrecibir;
    pthread_create(&threadrecibir,NULL,&leer_mensajes,NULL);
    int flag=0;
    while(flag==0){
        char buffer[100];
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strlen(buffer) - 1] = 0;
        
        int i=0;
        char *argumentos[10];
        char *token;

        token=strtok(buffer," ");
        argumentos[0]=token;
        
        while( token != NULL ){
            argumentos[i]=token;
            token = strtok(NULL, " ");
            i++;
        }
        //sub
        if(strncmp(argumentos[0],"sub",strlen("sub"))==0 && strlen(argumentos[0])==3 && argumentos[1]!=NULL){
            struct mesg_buffer mensaje;
            mensaje.mesg_type=2;
            mensaje.processid=processid;
            strcpy(mensaje.mensaje, argumentos[1]);
            printf("yo %ld, quiero subscribirme a: %s\n", mensaje.processid, mensaje.mensaje);
            msgsnd(msgid, &mensaje, sizeof(mensaje),0);
        }
        //\sub

        //unsub
        if(strncmp(argumentos[0],"unsub",strlen("unsub"))==0 && strlen(argumentos[0])==5 && argumentos[1]!=NULL){
            struct mesg_buffer mensaje;
            mensaje.mesg_type=3;
            mensaje.processid=processid;
            strcpy(mensaje.mensaje, argumentos[1]);
            printf("yo %ld, quiero desubscribirme a: %s\n", mensaje.processid, mensaje.mensaje);
            msgsnd(msgid, &mensaje, sizeof(mensaje),0);
        }
        //\unsub

        //list
        if(strncmp(argumentos[0],"list",strlen("list"))==0 && strlen(argumentos[0])==4 && argumentos[1]==NULL){
            struct mesg_buffer mensaje;
            mensaje.mesg_type=4;
            mensaje.processid=processid;
            strcpy(mensaje.mensaje, "list");
            printf("yo %ld, quiero que me muestres los eventos a los que estoy suscrito\n", mensaje.processid);
            msgsnd(msgid, &mensaje, sizeof(mensaje),0);
        }
        //\list

        //ask
        if(strncmp(argumentos[0],"ask",strlen("ask"))==0 && strlen(argumentos[0])==3 && argumentos[1]==NULL){
            struct mesg_buffer mensaje;
            mensaje.mesg_type=5;
            mensaje.processid=processid;
            strcpy(mensaje.mensaje, "ask");
            printf("yo %ld, quiero que me muestres los eventos que hay\n", mensaje.processid);
            msgsnd(msgid, &mensaje, sizeof(mensaje),0);
        }
        //\ask

        //volver argumentos nulos otra vez
        for(int j=0;j<sizeof(argumentos);j++){
            argumentos[j]=NULL;
        }
    }

}
void* exposicion(void *param){

    struct mesg_buffer mensaje;
    mensaje.mesg_type=10;
    mensaje.processid=processid;
    strcpy(mensaje.mensaje, "Hello parent!");
    printf("mi id del proceso: %ld\n", mensaje.processid);
    msgsnd(msgid, &mensaje, sizeof(mensaje),0);
    
}
//\escucha la linea de comandos

int main()
{
    processid=getpid();
    key_t key;
    key = ftok(".", 17);
    msgid = msgget(key, 0666 | IPC_CREAT);

    //enviar mensaje de reconocimiento id
    pthread_t threadexponer;
    pthread_create(&threadexponer,NULL,&exposicion,NULL); 
    pthread_join(threadexponer,NULL);
    //\enviar mensaje de reconocimiento id
    
    //escuchar comandos
    pthread_t threadescuchar;
    pthread_create(&threadescuchar,NULL,&escuchar,NULL); 
    pthread_join(threadescuchar,NULL);
    //\escuchar comandos

}