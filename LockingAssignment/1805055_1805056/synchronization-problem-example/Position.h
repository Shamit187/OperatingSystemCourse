//
// Created by amimu on 2/6/2023.
//

#include <pthread.h>

#ifndef UNTITLED1_POSITION_H
#define UNTITLED1_POSITION_H

typedef struct Pos_Struct {
    int id;
    pthread_cond_t turn;
    struct Pos_Struct *next;
} Position;

#endif //UNTITLED1_POSITION_H