/***************************************************
 *barrier.c
 *creates a number of threads, with a barrier to 
 *collect n threads and block the rest until the
 *barrier code increments a value and releases the
 *thread. Then, freed threads will increment another
 *counter, and the barrier counter and thread count
 *must match to ensure all threads were handled.
 **************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BARRIER 2
int thread_count;
int barrier_thread_count = 0;
pthread_mutex_t barrier_mutex;
pthread_cond_t ok_to_proceed;

void *Thread_work(void* rank);

int main(int argc, char* argv[]){
  long thread;
  pthread_t* thread_handles;
  thread_count = strtol(argv[1], NULL, 10);

  thread_handles = malloc (thread_count*sizeof(pthread_t)); 
  pthread_mutex_init(&barrier_mutex, NULL);
  pthread_cond_init(&ok_to_proceed, NULL);

  for (thread = 0; thread < thread_count; thread++)
    pthread_create(&thread_handles[thread], NULL, Thread_work, (void*) thread);

  for (thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }
  printf("Operation completed successfully with %d threads and %d barriers.\n", thread_count, BARRIER);

  pthread_mutex_destroy(&barrier_mutex);
  pthread_cond_destroy(&ok_to_proceed);
  free(thread_handles);
  return 0;

}

void *Thread_work(void* rank) {
  long my_rank = (long) rank;
  int i;

  for (i = 0; i < BARRIER; i++) {
    pthread_mutex_lock(&barrier_mutex);
    barrier_thread_count++;
    if (barrier_thread_count == thread_count){
      barrier_thread_count = 0;
      printf("Thread %ld > notifying other threads in barrier %d\n", my_rank, i);
      fflush( stdout );
      pthread_cond_broadcast(&ok_to_proceed);
    } else {
      while (pthread_cond_wait(&ok_to_proceed, &barrier_mutex) != 0 );
      printf("Thread %ld > alerted in barrier %d\n", my_rank, i);
      fflush(stdout);
    }
    pthread_mutex_unlock(&barrier_mutex);
    if (my_rank == 0){
      printf("All threads went through barrier %d\n", i);
      fflush(stdout);
    }
  }
  return NULL; /*seriously?*/
}
