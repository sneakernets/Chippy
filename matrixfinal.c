/**********************************************************
 *matrixfinal.c
 *
 *USE: matrixfinal (n, where n is n * n matrix) (threads)
 *Features: Timer!
 *TODO: make >8192 faster. Or give a warning.
 *
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define ELEMENT float /*each element is a float*/
ELEMENT ** matrix_a, ** matrix_b, **result; /*matrices*/


void * generate_matrix( void * id ); /*declaring methods*/
void *dotter(void *);


int num_threads, matrix_size;




void *dotter(void *arg){
  long column;
  long row;
  long chunk = (long)arg;
  long i;
  ELEMENT t;
/*it doesn't get any simpler*/
  for (column=0; column < matrix_size; column++){ /*for each column*/
    for (row = chunk; row < matrix_size; row += num_threads){/*for each row chunk*/
    t = 0.0;
    for (i=0; i<matrix_size; i++)
      t += matrix_a[row][i] * matrix_b[i][column]; /*the main math is here*/
    result[row][column] = t; /*throw the result in*/
    }
  }
  return NULL;
}

/*matrix generator*/
void * generate_matrix(void * id) {
  long inner_id = (long)id;
  long i;
  for (i=inner_id; i < matrix_size; i += num_threads){
    int j;
    for (j=0; j<matrix_size; j++){
      matrix_a[i][j]=rand();
      matrix_b[i][j]=rand();
    }
  }
  return NULL;
}

/*main*/
int main(int argc, const char * argv[]){
  struct timeval start, end; /*timer!*/
  long totaltime, seconds, useconds;
  long i, j;
  int elmt_size;
  matrix_size = atoi(argv[1]);
if (matrix_size > 4096) printf("%d size? Are you crazy?! Running anyway!\n", matrix_size);
  num_threads = atoi(argv[2]);
  printf("%d by %d matrix pair with %d threads running\n", matrix_size, matrix_size, num_threads);

  gettimeofday(&start, NULL);
  /*init matrix*/
  srand(time(NULL));
  printf("Creating matrix pairs\n");

  matrix_a = malloc(elmt_size = matrix_size * sizeof(ELEMENT *));
  matrix_b = malloc(elmt_size);
  result = malloc(elmt_size);

  for (i=0; i < matrix_size; i++){
    matrix_a[i]	= malloc( elmt_size = matrix_size * sizeof(ELEMENT) );
    matrix_b[i]	= malloc( elmt_size );
    result[i]= malloc( elmt_size );
  }  

  /*init pthreads here*/
  pthread_t * generate = malloc(elmt_size=num_threads*sizeof(pthread_t));
  pthread_t * thread_work = malloc(elmt_size);
  printf("Randomizing matrix pairs\n");
  /*making matrix pairs here*/
  for (i=0; i<num_threads; i++){
    pthread_create(&generate[i], NULL, generate_matrix, (void *) i);
  }
  /*joining the randomness back together*/
  for (i=0; i<num_threads; i++){
    pthread_join(generate[i], NULL);
  }

  /*doing actual work here*/
  printf("Doing work now\n");

  for (i=0; i<num_threads; i++)
      pthread_create(&thread_work[i],NULL,dotter,(void *)i);

  for (i=0; i<num_threads; i++)
    pthread_join(thread_work[i], NULL);

  gettimeofday(&end, NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  totaltime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf("Total time: %ld milliseconds.\n", totaltime);
      }

