/*mat_mult.c */

//#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

/*   ttype: type to use for representing time */
typedef double ttype;
ttype tdiff(struct timespec a, struct timespec b)
/* Find the time difference. */
{
  ttype dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
  return dt;
}

struct timespec now()
/* Return the current time. */
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}






int main (int argc, char *argv[])
{
	int NRA = atof(argv[1]);                /* number of rows in matrix A */
	int NCA_NRB = atof(argv[2]);             /* number of rows in matrix B, number of columns in matrix A */    
	int NCB= atof(argv[3]);                 /* number of columns in matrix B */
	int tid;               

	int	A[NRA][NCA_NRB];         /* matrix A to be multiplied */
	int B[NCA_NRB][NCB];        /* matrix B to be multiplied */
	int C[NRA][NCB];           /* result matrix C */


//clock_t begin, end;
  struct timespec begin, end;
  double time_spent; 
               
      printf("Matrix A: #rows %d; #cols %d\n", NRA, NCA_NRB);
      printf("Matrix B: #rows %d; #cols %d\n", NCA_NRB, NCB);
      printf ("\n");

      printf("Initializing arrays...\n");
      for (int i=0; i<NRA; i++)
         for (int j=0; j<NCA_NRB; j++)
            A[i][j]= i+j;  

      printf (" Contents of matrix A\n");
      for (int i=0; i<NRA; i++) {  
        for (int j=0; j<NCA_NRB; j++)
        printf("%d\t", A[i][j]);
        printf("\n");
      }     
            
      for (int i=0; i<NCA_NRB; i++)   
         for (int j=0; j<NCB; j++)
            B[i][j]= i-j;

      printf (" Contents of matrix B\n");
      for (int i=0; i<NCA_NRB; i++) {  
        for (int j=0; j<NCB; j++){
        printf("%d\t", B[i][j]);      
		  } 
		  printf("\n"); 
	  }	  

	    //begin = clock();
	begin = now();
  #pragma omp parallel  private(tid) 
  {
	  tid = omp_get_thread_num();
	  #pragma omp for 
      for (int i=0; i<NRA; i++)
	    {
         for (int j=0; j<NCB; j++)
         {
           C[i][j] = 0;
           for (int k=0; k<NCA_NRB; k++)            
              C[i][j] = C[i][j] + A[i][k] * B[k][j];
         }
	    }
	  if (tid == 0)
      {
		  /* Print results */
        printf ("\n");
        printf("******************************************************\n");
        printf("Result Matrix:\n");
	  

        for (int i=0; i<NRA; i++)
        {
          printf("\n"); 
          for (int j=0; j<NCB; j++) 
              printf("%d\t", C[i][j]);
        }
	  
	  
          //end = clock();
          end = now();
          time_spent = tdiff(begin, end);
        
     
      
        printf("\n******************************************************\n");
        printf ("\n");
        printf("total time: %.8f sec\n", time_spent);
        printf ("\n");
      }  
  }
	 
}

         
