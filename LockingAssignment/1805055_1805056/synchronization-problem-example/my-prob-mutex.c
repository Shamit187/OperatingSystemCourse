#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "Position.h"
#include "ClientQueue.h"

pthread_mutex_t gettingCut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barberCutting = PTHREAD_COND_INITIALIZER;
pthread_mutex_t accessChairs = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t accessBarber = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clientsAvailable = PTHREAD_COND_INITIALIZER;

Position *barberQueue = NULL;
Position *resignedClients = NULL;

int cutting = 0;
int numberOfChairs;
int totalNumberOfClients = 0;
int currentlyServedClient = 0;
int numberOfWaitingClients = 0;
int numberOfResignedClients = 0;

void initializeClientThread(pthread_t *id2, int numberOfClientThreads, int threadWaitTime);

void initializeBarberThread(pthread_t *id1);

Position *AllowClientIn() {
    Position *tmp = barberQueue;
    barberQueue = barberQueue->next;
    return tmp;
}

void PrintStats() {
    if (currentlyServedClient != 0) {
        printf("Res: %d wRoom: %d/%d In: %d\n", numberOfResignedClients, numberOfWaitingClients, numberOfChairs,
               currentlyServedClient);
    } else {
        printf("Res: %d wRoom: %d/%d In: -\n", numberOfResignedClients, numberOfWaitingClients, numberOfChairs);
    }
}


void Customer() {
    int id;

    pthread_mutex_lock(&accessChairs);

    totalNumberOfClients++;

    id = totalNumberOfClients;

    if (numberOfWaitingClients < numberOfChairs) {

        numberOfWaitingClients++;

        Position *client = AddNewClientToQueue(&barberQueue, totalNumberOfClients);

        printf("New client got into shop!\n");

        PrintStats();

        pthread_cond_signal(&clientsAvailable);
        pthread_mutex_unlock(&accessChairs);
        pthread_mutex_lock(&accessBarber);

        while (currentlyServedClient != id) {
            pthread_cond_wait(&client->turn, &accessBarber);
        }

        pthread_mutex_unlock(&accessBarber);
        pthread_mutex_lock(&gettingCut);

        while (cutting) {
            pthread_cond_wait(&barberCutting, &gettingCut);
        }

        pthread_mutex_unlock(&gettingCut);
    } else {
        AddNewClientToQueue(&resignedClients, totalNumberOfClients);

        numberOfResignedClients++;

        printf("New client couldn't get into lobby!\n");
        PrintStats();

        pthread_mutex_unlock(&accessChairs);
    }
}

void Barber() {
    while (1) {
        int i = rand() % 4;
        pthread_mutex_lock(&accessChairs);

        while (numberOfWaitingClients == 0) {
            currentlyServedClient = 0;
            pthread_cond_wait(&clientsAvailable, &accessChairs);
        }

        numberOfWaitingClients--;

        Position *client = AllowClientIn();
        currentlyServedClient = client->id;

        printf("New client for the cut!\n");

        PrintStats();

        pthread_mutex_unlock(&accessChairs);
        pthread_cond_signal(&client->turn);
        pthread_mutex_lock(&gettingCut);

        cutting = 1;
        sleep(i);
        cutting = 0;

        pthread_cond_signal(&barberCutting);
        pthread_mutex_unlock(&gettingCut);
    }
}

int main(int argc, char *argv[]) {
    pthread_t id1, id2;
    srand(time(NULL));

    if (argc <= 3) {

        switch (argc) {
            case 0: {
                printf("Please provide number of chair");
                exit(-1);
            }
            case 1: {
                printf("Please provide number of clients and thread interval time");
                exit(-1);
            }
            case 2: {
                printf("Please provide thread interval time");
                exit(-1);
            }
            default: {
                exit(-1);
            }
        }
    } else {
        numberOfChairs = atoi(argv[1]);
        int numberOfClientThreads = atoi(argv[2]);
        int threadInterLivings = atoi(argv[3]);

        initializeBarberThread(&id1);
        initializeClientThread(&id2, numberOfClientThreads, threadInterLivings);
    }
}

void initializeBarberThread(pthread_t *id1) {
    int status = pthread_create(id1, NULL, (void *) Barber, NULL);
    if (status != 0) {
        printf("Barber couldn't start!'\n");
        exit(status);
    }
}

void initializeClientThread(pthread_t *id2, int numberOfClientThreads, int threadWaitTime) {
    while (numberOfClientThreads >= 0) {
        sleep(threadWaitTime);

        int creationStatus = pthread_create(id2, NULL, (void *) Customer, NULL);
        if (creationStatus != 0) {
            printf("Client couldn't start!\n");
            exit(creationStatus);
        }
        numberOfClientThreads--;
    }
}
