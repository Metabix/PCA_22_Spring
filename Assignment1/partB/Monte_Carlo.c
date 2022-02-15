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
	begin = now();
	double resul, sumtask;
	MPI_Comm_size(MPI_COMM_WORLD,&nt);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);    

	if(taskid == MASTER)
	{
		resul = *result;
	        double temp;
		for (int z = 1; z < nt; z++)
		{
			MPI_Status status;
			int source = z;
                        printf("Source : %d\n",source);
			MPI_Recv(&temp, 1, MPI_DOUBLE, source,mt, MPI_COMM_WORLD, &status);
			resul += temp;
		}
			end = now();
                ts = tdiff(begin, end);
		
		printf("total sum : %.10f \n",resul);
		printf("total time : %.8f sec\n", ts);
	}
	
	else
	{
		sumtask = *result; 
		MPI_Send(&sumtask, 1, MPI_DOUBLE, MASTER, mt, MPI_COMM_WORLD);
		
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
	
	
	result = estimate_g(lower_bound, upper_bound, N);
	

	collect_results(&result);
	
	MPI_Finalize();
    return 0;
}