/*
Name: Claude Freeman
mpifinal.c
Computes the intersection of 4 circles.
Uses Bcast and Reduce.
Because of the use of Bcast, this program does not exit "cleanly".
This does not compromise the performance of the program,
but it does look a little ugly. using broadcasts actually
speeds up the process, from my experience.

This will run on 1 processor, as well.
*/

//#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
int main(int argc,char *argv[])
{
  
  long int n, i;
  long int privn = 0;
  int numprocs;
  int myid = 0;
  double area, maxval;
  double sum_n, in_circle, sum_c;
  double starttime, endtime;
  double oldarea = 0.0;
  double olderarea = 0.0;
  int notdone = 1;
 // MPI_Init(&argc,&argv);
 // MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
//  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  
  maxval = (double)RAND_MAX;    /* The largest value returned by random() */
  srand(time(NULL));                /* Seed the random number generator */

  /* Read the number of random samples in each process */
  /*if (myid == 0) {*/
    n = 0;
  /*}*/
  /* Send n to all processes */
//  MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
/*  if (myid == 0) */
//    printf("Number of darts to start: %llu\n", n);
//  fflush(stdout);
  
  /* Start measuring time */
  /*if (myid == 0) starttime = MPI_Wtime();*/
  in_circle = 0.0;
  /* Draw n random points and count how many are inside the circle */
  while(notdone){
    //in_circle = 0.0;
    privn = n; // make "n" if speed is a must
    n+=500;
  for (i=privn; i<n; i++) {
    double x, y;
    x = (double)rand()/maxval*3 + 3; /* Draw a random point */
    y = (double)rand()/maxval*3 + 3;
    if ((x - 3)*(x - 3)+(y - 3)*(y - 3) <= 9){
      if((x - 6)*(x - 6)+(y - 3)*(y - 3) <= 9){
	if((x - 3)*(x - 3)+(y - 6)*(y - 6) <= 9){
	  if((x - 6)*(x - 6)+(y - 6)*(y - 6) <= 9){
	    in_circle++;
	  }
	}
      }
    }
  }
  /* Sum all the samples inside the circle */
  /*MPI_Reduce(&in_circle, &sum_c, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);*/
    sum_c = in_circle;
  if (myid == 0) {
    sum_n = (double) (n);  /* Total nr of random points */
    olderarea = oldarea;
    oldarea = area;
    area = (sum_c/sum_n)*9.0;
    sum_c = 0.0;
    sum_n = 0.0;
    printf("Area : %f  Previous Area: %f  Number of darts: %d   \r", area,  oldarea, n);
    fflush(stdout);
    if(fabs(area - oldarea) < 0.00001){
      if(fabs(oldarea - olderarea)< 0.0000001){
         notdone = 0;
      /*endtime = MPI_Wtime();*/
      printf("\n");
      printf("The area of the intersection of circles is %2.8f\n", area);
      /*printf("Total time = %f s\n", endtime-starttime);	       */
      fflush(stdout);
      /*MPI_Abort(MPI_COMM_WORLD,0);*/

	}
     }
  }
  } /*while end*/

/*MPI_Finalize();*/
return 0;
}

