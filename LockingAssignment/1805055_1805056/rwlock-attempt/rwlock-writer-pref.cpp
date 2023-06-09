#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->num_of_readers = 0;
  rw->num_of_writers = 0;
  pthread_mutex_init(&rw->all_lock_mutex, NULL);
  pthread_mutex_init(&rw->writer_lock_mutex, NULL);
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&rw->all_lock_mutex);
  while(rw->num_of_writers != 0){
    pthread_cond_wait(&rw->read_available, &rw->all_lock_mutex);
  }
  rw->num_of_readers++;
  if(rw->num_of_readers == 1)
    pthread_mutex_lock(&rw->writer_lock_mutex);

  pthread_mutex_unlock(&rw->all_lock_mutex);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->all_lock_mutex);
  rw->num_of_readers--;
  if(rw->num_of_readers == 0) {
    pthread_mutex_unlock(&rw->writer_lock_mutex);
  }
  pthread_mutex_unlock(&rw->all_lock_mutex);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&rw->all_lock_mutex);

  rw->num_of_writers++;
  pthread_mutex_unlock(&rw->all_lock_mutex);

  pthread_mutex_lock(&rw->writer_lock_mutex);

  
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&rw->all_lock_mutex);

  rw->num_of_writers--;
  pthread_mutex_unlock(&rw->writer_lock_mutex);
  if(rw->num_of_writers == 0)
    pthread_cond_broadcast(&rw->read_available);

  pthread_mutex_unlock(&rw->all_lock_mutex);
}
