#include "eventList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
EventList *CreateEventList(void)
{
    EventList *eventlist = malloc(sizeof(EventList));
    eventlist->isEmpty = 0;
    eventlist->head = NULL;
    eventlist->last = NULL;
    return eventlist;
}

void DestroyEventList(EventList *this)
{
    free(this);
}

Event *SearchEvent(EventList *this, char *name)
{
    Event *evento = this->head;
    if (this->isEmpty == 0){
        printf("no se encuentra ningun evento\n");
        return NULL;}
    else{
        while (evento != NULL){
            if (strcmp((evento->eventName),(name))==0){
                printf("evento encontrado\n");
                return evento;
            }
            evento = evento->next;
        }
    }
    return NULL;
}

void AddEvent(EventList *this, Event *event)
{
    if (this->isEmpty != 1){
        this->head = event;
        this->last = event;
        this->isEmpty = 1;
        printf("Primer evento adicionado\n");
    }
    else{  
        this->last->next = event;
        this->last = event;
        printf("Evento adicionado\n");
    }
}

void RemoveEvent(EventList *this, char *name)
{
    Event *evento = this->head;
    Event *remover = SearchEvent(this, name);
    if(remover!=NULL){
        while (evento != NULL){
            if(strcmp((this->head->eventName),(name))==0){
                    this->head = this->head->next;
                    break;
            }else if (strcmp((evento->next->eventName),(name))==0){
                    evento->next = evento->next->next;
                    break;
            }
             evento = evento->next;
        }
        printf("evento removido\n");
    }else{
        printf("evento no encontrado\n");
    }
    if (this->head == NULL){
        this->isEmpty = 0;
    }
}

void ListEvents(EventList *this)
{
    Event *evento = this->head;
    if (this->isEmpty == 0){printf("empty\n");}
    else{
        while (evento != NULL){
            printf("%s\n", evento->eventName);
            evento = evento->next;
        }
    }
}
