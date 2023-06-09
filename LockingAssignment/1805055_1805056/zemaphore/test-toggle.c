#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include"zemaphore.h"

#define NUM_THREADS 150
#define NUM_ITER 32

zem_t sync_lock[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for(int i=0; i < NUM_ITER; i++)
    {
      zem_down(&sync_lock[thread_id]);
      printf("This is thread %d\n", thread_id);
      zem_up(&sync_lock[(thread_id+1)%NUM_THREADS]);
    }
  return 0;
}

int main(int argc, char *argv[])
{
  zem_init(&sync_lock[0], 1);
  for(int i=1; i < NUM_THREADS; i++){
    zem_init(&sync_lock[i], 0);
  }

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
