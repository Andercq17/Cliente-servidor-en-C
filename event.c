#include "event.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "eventList.h"

Event *CreateEvent(char *name)
{
    Event *evento = malloc(sizeof(Event));
    evento->next = NULL;
    evento->cant_clientes=0;
    name[15]=0;
    sscanf(name,"%s",evento->eventName);
    return evento;

}

void DestroyEvent(Event *this)
{
    free(this);
}





