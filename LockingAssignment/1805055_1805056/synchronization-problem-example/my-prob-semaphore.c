#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#include"zemaphore.h"
#define WORK_TIME 3
#define INTERVAL 2

zem_t mutex,customer,barber;
int number_of_chair, number_of_customer, lobby_seat=0, barber_kill=0;

void* barber_bot(void *arg)
{
	while(1)
	{
		zem_down(&customer);           
		zem_down(&mutex);
        if(lobby_seat == 0) {
            continue;
            zem_up(&mutex);
        }
		lobby_seat--;
        printf("Barber Done. Current Lobby: %d/%d\n", lobby_seat, number_of_chair);
		zem_up(&mutex);
		zem_up(&barber);
		sleep(WORK_TIME); 
        if(barber_kill)
            break;      
	}
}

void* customer_bot(void *arg)
{
    zem_down(&mutex);
    if(lobby_seat < number_of_chair){
        lobby_seat++;
        printf("Customer Took a seat. Current Lobby: %d/%d\n", lobby_seat, number_of_chair);
        zem_up(&mutex);
        zem_up(&customer);
        zem_down(&barber);
    }
    else{
        printf("Customer Left. Current Lobby: %d/%d\n", lobby_seat, number_of_chair);
        zem_up(&mutex);
    }
        
}

int main(int argc,char *argv[])
{
    if(argc < 5){
        printf("Wrong format");
        exit(0);
    }
    zem_init(&mutex,1);
	zem_init(&customer,0);
	zem_init(&barber,1);

    pthread_t barber_bot_thread;
    pthread_t* customer_bot_threads;
    number_of_chair = atoi(argv[1]);
    number_of_customer = atoi(argv[2]);
    customer_bot_threads = (pthread_t *)malloc(sizeof(pthread_t) * number_of_customer);

	pthread_create(&barber_bot_thread,NULL,barber_bot,NULL);
    for(int i = 0; i < number_of_customer; i++){
        pthread_create(&customer_bot_threads[i],NULL,customer_bot,NULL);
        sleep(INTERVAL);
    }

    for(int i = 0; i < number_of_customer; i++)
	    pthread_join(customer_bot_threads[i],NULL);
    
    while(1)
    {
        zem_down(&mutex);
        if(lobby_seat != 0){
            zem_up(&mutex);
            continue;
        }
        else{
            barber_kill = 1;
            zem_up(&mutex);
            break;
        }
    }
	pthread_join(barber_bot_thread,NULL);

    free(customer_bot_threads);
	exit(0);
}