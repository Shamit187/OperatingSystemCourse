//
// Created by amimu on 2/6/2023.
//

#ifndef UNTITLED1_CLIENTQUEUE_H
#define UNTITLED1_CLIENTQUEUE_H

#include <malloc.h>
#include "Position.h"

Position *AddNewClientToQueue(Position **start, int id) {
    Position *new = malloc(sizeof(Position));

    new->id = id;
    new->next = NULL;

    pthread_cond_init(&new->turn, NULL);

    if (*start == NULL) {
        (*start) = new;
    } else {
        Position *tmp = *start;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new;
    }
    return new;
}

#endif //UNTITLED1_CLIENTQUEUE_H
