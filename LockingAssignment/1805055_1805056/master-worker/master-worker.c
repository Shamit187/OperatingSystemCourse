#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size, item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;
pthread_cond_t empty, fill;
pthread_mutex_t mutex;
int head,tail;

int *buffer;

void print_produced(int num, int master) {
  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {
  printf("Consumed %d by worker %d\n", num, worker);
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data){
  
  int thread_id = *((int *)data);

  while(1){
    pthread_mutex_lock(&mutex);
    
    if(item_to_produce >= total_items){
      //printf("Master thread: %d found every item to be produced.\n", thread_id);
      pthread_mutex_unlock(&mutex);
      break;
    }

    while(((max_buf_size != 1) && (head + 1) % max_buf_size == tail) || (max_buf_size == 1 && head == 1)) {
      //printf("Master thread: %d waiting for an empty size in buffer. Head: %d Tail: %d.\n", thread_id, head, tail);
      pthread_cond_wait(&empty, &mutex);
      //printf("Master thread: %d got awake. Head: %d Tail: %d Going to produce: %d\n", thread_id, head, tail, item_to_produce);
      if(item_to_produce >= total_items){
        pthread_mutex_unlock(&mutex);
        return 0;
      }
    }

    //printf("Master thread: %d is going to produce %d.\n",thread_id, item_to_produce);
    buffer[head] = item_to_produce;
    print_produced(item_to_produce, thread_id);
    item_to_produce++;

    if(max_buf_size != 1)
      head = (head + 1) % max_buf_size;
    else
      head = 1;
    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&mutex);
  }

  pthread_cond_broadcast(&empty);
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

void *response_requests_loop(void *data){
  int thread_id = *((int *)data);
  int consumed_data;

  while(1){
    pthread_mutex_lock(&mutex);

    if(item_to_consume >= total_items){
      //printf("Worker thread: %d found every item to be consumed.\n", thread_id);
      pthread_mutex_unlock(&mutex);
      break;
    }

    while( (max_buf_size != 1 && tail == head) || (max_buf_size == 1 && head == 0)) {
      //printf("Worker thread: %d waiting for an filled size in buffer. Head: %d Tail: %d.\n", thread_id, head, tail);
      pthread_cond_wait(&fill, &mutex);
      //printf("Worker thread: %d got awake. Head: %d Tail: %d Going to consume: %d\n", thread_id, head, tail, item_to_consume);
      if(item_to_consume >= total_items){
        pthread_mutex_unlock(&mutex);
        return 0;
      }
    }

    //printf("Worker thread: %d is going to consume %d.\n",thread_id, item_to_consume);
    consumed_data = buffer[tail];
    print_consumed(consumed_data, thread_id);
    item_to_consume++;

    if(max_buf_size != 1)
      tail = (tail + 1) % max_buf_size;
    else
      head = 0;
    
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int *master_thread_id, *worker_thread_id;
  pthread_t *master_thread, *worker_thread;
  pthread_mutex_init(&mutex, NULL);
  item_to_produce = 0;
  item_to_consume = 0;
  curr_buf_size = 0;
  head = 0;
  tail = 0;
  
  int i;
  
  if(argc < 5){
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    
  buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers+1);
  
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, response_requests_loop, (void *)&worker_thread_id[i]);
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
  {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
  }
  pthread_cond_broadcast(&fill);
  for (i = 0; i < num_workers; i++)
  {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
  }
  
  
  
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);
  
  return 0;
}
