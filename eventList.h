#ifndef _EVENTLIST_H
#define _EVENTLIST_H

#include "event.h"


// EventList class
typedef struct _eventList
{
    char isEmpty;
    Event *head;
    Event *last;
} EventList;

EventList *CreateEventList(void);
void AddEvent(EventList *this, Event *event);
Event *SearchEvent(EventList *this, char *name);
void DestroyEventList(EventList *this);
void RemoveEvent(EventList *this, char *name);
void ListEvents(EventList *this);

#endif