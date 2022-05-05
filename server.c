#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "eventList.h"


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
long lista[200];
int cant_clientes=0;
long proccesid;
EventList *eventList = NULL;

struct mesg_buffer {
    long msg_type;
    char mensaje[100];
    long processid;
}message;

void* leer_mensajes(void *param){
    int flag=0;
    while(flag==0){
        struct mesg_buffer message;
        int e=msgrcv(msgid, &message, sizeof(message), -10,0);    
        //cuando expongan id
        if(e>0){
            if(message.msg_type==10){//exposicion id
                cant_clientes=cant_clientes+1;
                lista[cant_clientes-1]=message.processid;
                printf("id recibido: %ld\n",message.processid);
                printf("cantidad de clientes en la lista: %d\n", cant_clientes);
            }
            if(message.msg_type==2){//sub
                printf("cliente: %ld, quiere suscribirse a: %s \n",message.processid, message.mensaje);
                struct mesg_buffer msg;
                msg.msg_type=message.processid;
                Event *peticion = SearchEvent(eventList, message.mensaje);
                if(peticion!=NULL){

                    peticion->clientes[peticion->cant_clientes]=message.processid;
                    peticion->cant_clientes=peticion->cant_clientes+1;              

                    Event *evento = eventList->head;

                    while (evento != NULL){
                        if(strcmp((eventList->head->eventName),(message.mensaje))==0){
                            eventList->head = peticion;
                            break;
                        }
                        if(strcmp(evento->next->eventName,message.mensaje)==0){
                            evento->next = peticion;
                            break;
                        }
                        evento = evento->next;
                    }
                    (void)strcpy(msg.mensaje, "te has suscrito");
                    msg.processid=proccesid;
                    msgsnd(msgid,&msg,sizeof(msg),0);
                }else{
                    (void)strcpy(msg.mensaje, "no existe ese evento");
                    msg.processid=proccesid;
                    msgsnd(msgid,&msg,sizeof(msg),0);
                }
            }
            if(message.msg_type==3){//unsub
                printf("cliente: %ld, quiere desuscribirse a: %s \n",message.processid, message.mensaje);
                struct mesg_buffer msg;
                msg.msg_type=message.processid;
                Event *peticion = SearchEvent(eventList, message.mensaje);
                if(peticion!=NULL){ 
                    int encontro,posicion;
                    encontro=0;
                    for(int contador=0;contador<peticion->cant_clientes;contador++){
                        if(message.processid == peticion->clientes[contador]){
                            encontro = 1;
                            posicion = contador;
                            break;
                        }
                    }
                    if(encontro==1){
                        struct mesg_buffer msga;
                        msga.msg_type=message.processid;
                        (void)strcpy(msga.mensaje, "Se encontró la suscripción");
                        msga.processid=proccesid;
                        msgsnd(msgid,&msga,sizeof(msga),0);
                        for(int contador = posicion; contador < peticion->cant_clientes; contador++){
                            if(peticion->clientes[contador+1]!=0){
                                peticion->clientes[contador]=peticion->clientes[contador + 1];
                            }  	
                        }
                        peticion->cant_clientes=peticion->cant_clientes-1;
                        Event *evento = eventList->head;
                        while (evento != NULL){
                            if(strcmp((eventList->head->eventName),(message.mensaje))==0){
                                eventList->head = peticion;
                                break;
                            }
                            if(strcmp(evento->next->eventName,message.mensaje)==0){
                                evento->next = peticion;
                                break;
                            }
                            evento = evento->next;
                        }
                        (void)strcpy(msg.mensaje, "te has desuscrito");
                        msg.processid=proccesid;
                        msgsnd(msgid,&msg,sizeof(msg),0);
                    }else if(encontro!=1){
                        struct mesg_buffer msga;
                        msga.msg_type=message.processid;
                        (void)strcpy(msga.mensaje, "no se encuentra la suscripción");
                        msga.processid=proccesid;
                        msgsnd(msgid,&msga,sizeof(msga),0);
                    }
                    encontro=0;
                }else{
                    (void)strcpy(msg.mensaje, "no existe ese evento para desuscribirse");
                    msg.processid=proccesid;
                    msgsnd(msgid,&msg,sizeof(msg),0);
                }
            }
            if(message.msg_type==4){//list
                printf("cliente: %ld, quiere ver a donde está suscrito: %s \n",message.processid, message.mensaje);  
                Event *evento = eventList->head;
                if (eventList->isEmpty == 0){
                    struct mesg_buffer msga;
                    msga.msg_type=message.processid;
                    (void)strcpy(msga.mensaje, "no se encuentran eventos");
                    msga.processid=proccesid;
                    msgsnd(msgid,&msga,sizeof(msga),0);
                }
                else{
                    char *suscripciones[200];
                    int bandera=0;
                    int conteo=0;
                    while (evento != NULL){
                        for (int contador=0; contador<evento->cant_clientes;contador++){
                            if(evento->clientes[contador] == message.processid){
                                suscripciones[conteo]= evento->eventName;
                                conteo=conteo+1;
                                bandera=1;
                            }
                        }
                        evento = evento->next;
                    }
                    if(bandera==0){
                        struct mesg_buffer msgb;
                        msgb.msg_type=message.processid;
                        (void)strcpy(msgb.mensaje, "No ha hecho ninguna suscripción");
                        msgb.processid=proccesid;
                        msgsnd(msgid,&msgb,sizeof(msgb),0);
                    }else{
                        struct mesg_buffer msgb;
                        msgb.msg_type=message.processid;
                        (void)strcpy(msgb.mensaje, "estas son sus sucripciones");
                        msgb.processid=proccesid;
                        msgsnd(msgid,&msgb,sizeof(msgb),0);
                        suscripciones[conteo]=NULL;
                        int i=0;
                        while(suscripciones[i]!=NULL){
                            struct mesg_buffer msga;
                            msga.msg_type=message.processid;
                            (void)strcpy(msga.mensaje, suscripciones[i]);
                            msga.processid=proccesid;
                            msgsnd(msgid,&msga,sizeof(msga),0);
                            i++;
                        }
                    }
                }
            }
            if(message.msg_type==5){//ask
                printf("cliente: %ld, quiere ver los eventos disponibles\n",message.processid);  
                Event *evento = eventList->head;
                if (eventList->isEmpty == 0){
                    struct mesg_buffer msg;
                    msg.msg_type=message.processid;
                    (void)strcpy(msg.mensaje, "no hay eventos disponibles, lo lamentamos");
                    msg.processid=proccesid;
                    msgsnd(msgid,&msg,sizeof(msg),0);
                }
                else{
                    struct mesg_buffer msga;
                    msga.msg_type=message.processid;
                    (void)strcpy(msga.mensaje, "Los eventos son los siguientes");
                    msga.processid=proccesid;
                    msgsnd(msgid,&msga,sizeof(msga),0);
                    while (evento != NULL){
                        struct mesg_buffer msg;
                        msg.msg_type=message.processid;
                        (void)strcpy(msg.mensaje, evento->eventName);
                        msg.processid=proccesid;
                        msgsnd(msgid,&msg,sizeof(msg),0);
                        evento = evento->next;
                    }
                }
            }
        }
    }
}

void* escuchar(void *param){
    int flag=0;  
    pthread_t threadrecibir;
    pthread_create(&threadrecibir,NULL,&leer_mensajes,NULL);
    char buffer[100];
    while(flag==0){
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strlen(buffer) - 1] = 0;
        char *argumentos[10];
        char *token;
        token=strtok(buffer," ");
        argumentos[0]=token;
        int i=0;
        while( token != NULL ){
            argumentos[i]=token;
            token = strtok(NULL, " ");
            i++;
        }
        //comando add
        if(strncmp(argumentos[0],"add",strlen("add"))==0 && strlen(argumentos[0])==3 && argumentos[1]!=NULL){
            AddEvent(eventList, CreateEvent(argumentos[1]));
        }
        //comando remove
        if(strncmp(argumentos[0],"remove",strlen("remove"))==0 && strlen(argumentos[0])==6 &&argumentos[1]!=NULL){
            RemoveEvent(eventList, argumentos[1]);
        }
        //comando listar
        if(strncmp(argumentos[0],"listar",strlen("listar"))==0  && strlen(argumentos[0])==6 && argumentos[1]==NULL){
            ListEvents(eventList);
        }
        //comando list
        if(strncmp(argumentos[0],"list",strlen("list"))==0  && strlen(argumentos[0])==4 && argumentos[1]!=NULL){
            Event *peticion = SearchEvent(eventList, argumentos[1]);
            if(peticion!=NULL){
                for(int i=0; i<peticion->cant_clientes;i++){
                    printf("clientes: %ld\n",peticion->clientes[i]);
                }
            }else{
                printf("No existe ese evento\n");
            }
        }
        //comando trigger
        if(strncasecmp(argumentos[0],"trigger",strlen("trigger"))==0 &&  strlen(argumentos[0])==7 && argumentos[1]!=NULL){
            Event *peticion=SearchEvent(eventList, argumentos[1]);
            if(peticion!=NULL){
                for(int i=0;i<cant_clientes;i++){
                    struct mesg_buffer msga;
                    msga.msg_type=lista[i];
                    (void)strcpy(msga.mensaje, "se ha publicado el siguiente evento");
                    msga.processid=proccesid;
                    msgsnd(msgid,&msga,sizeof(msga),0);
                    
                    struct mesg_buffer msg;
                    msg.msg_type=lista[i];
                    (void)strcpy(msg.mensaje, argumentos[1]);
                    msg.processid=proccesid;
                    msgsnd(msgid,&msg,sizeof(msg),0);   
                }
            }else{
                printf("ese evento no existe pa publicarse\n");
            }
             
        }//comando exit
        else if(strncasecmp(argumentos[0],"exit",strlen("exit"))==0 && strlen(argumentos[0])==4 && argumentos[1]==NULL){
            printf("Lista de eventos va a ser destruida\n");
            DestroyEventList(eventList);
            printf("cola va a ser destruida\n");

            for(int i=0;i<cant_clientes;i++){
                struct mesg_buffer msg;
                msg.msg_type=lista[i];
                (void)strcpy(msg.mensaje, "exit");
                msg.processid=proccesid;
                msgsnd(msgid,&msg,sizeof(msg),0);
            } 
            int a=msgctl(msgid, IPC_RMID, NULL);
            if(a==0){
                printf("cola destruida satisfactoriamente\n");
            }
            break;
        }
        for(int j=0;j<sizeof(argumentos);j++){
            argumentos[j]=NULL;
        }
    }
}

int main(int argc, char *argv[])
{   
    if(argc==1){
        eventList = CreateEventList();
        pthread_t threadID;
        key_t key;
        proccesid=getpid();
        key = ftok(".", 17);
        msgid = msgget(key, 0666 | IPC_CREAT);
        pthread_create(&threadID,NULL,&escuchar,NULL);
        pthread_join(threadID,NULL);
    }
}