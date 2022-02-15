/* Monte_carlo.c
  Author : Mohit P Sathyaseelan
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"


int mt = 2;
/*   ttype: type to use for representing time */
typedef double ttype;
ttype tdiff(struct timespec a, struct timespec b)
/* Find the time difference. */
{
  ttype dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
  return dt;
}
#define MASTER 0 

struct timespec now()
/* Return the current time. */
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}

struct timespec begin, end;
double ts;

int nt ,taskid ;


void init_rand_seed(void)
{
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	srand((unsigned int)taskid);
}

double estimate_g(double lower_bound, double upper_bound, long long int N)
{
    
	MPI_Comm_size(MPI_COMM_WORLD,&nt);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	
	int nortask = N/nt,etasks = N%nt;
    
	float val1,val2, tsum = 0;
	int abs;
    float c = 8*sqrt(2*3.14); 
	abs = (taskid <= etasks) ? nortask+1 : nortask;
		
		for (int i = 0; i <= abs; i++)
			{
				val1 = (upper_bound - lower_bound) * ((float)rand() / RAND_MAX) + lower_bound;
				val2 = ((float)(upper_bound - lower_bound)/(float)N)  * (c) / (exp((2*val1)*(2*val1)));		
				tsum += val2;	
			}
		printf("Sum: %.10f > task: %d\n",tsum,taskid);
	return(tsum);
}

void collect_results(double *result)
{
	
	MPI_Comm_size(MPI_COMM_WORLD,&nt);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	
		int dest = 0;
		double task_result = *result;
		double value=0;
	
	MPI_Reduce(&task_result,&value,1,MPI_DOUBLE,MPI_SUM,dest,MPI_COMM_WORLD);
	
		printf("DATA from task  %d reduced \n",&taskid);
        if(value != 0)
        {
		printf("\n----------------total sum: %.8f  --------------------\n", value);
        }
        else 
        {
            printf( "\n");
        }     
}

int main(int argc, char **argv)
{
	double result;
	float lower_bound = atof(argv[1]);
    float upper_bound = atof(argv[2]);
    long long int N = atof(argv[3]);
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&nt);
	
	init_rand_seed();
	
	begin = now();
	result = estimate_g(lower_bound, upper_bound, N);
	
   
	collect_results(&result);

    end = now();
    ts = tdiff(begin, end);
    printf("total time : %.8f sec\n", ts); 
	
	MPI_Finalize();
    return 0;
}