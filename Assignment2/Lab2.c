#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

/* ttype: type to use for representing time */
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

struct timespec begin, end;
double time_spent;

int N;
int A[15][15];
int rank;
int size;
int rec_buf[15][15];
int AP[15][15];
int *sendcounts;
int *displs;
int *temp_sendcounts ;
int *temp_displs;
int *updated_sendcounts;
int *updated_displs;

//initializing the array 
int* initialize_data(int N)
{
	int number;
	srand(1);
	
	if(rank == 0)
	{
		printf("original data\n");
		for(int i =0; i<N;i++)
		{
			for(int j =0; j < N ;j++)
			{
				number = (rand() % (255 + 1));
				A[i][j]= number;
				printf("%d\t",A[i][j]);
			}
			printf("\n");
		}	
	}
	return *A;
			
}

void distribute_data(int N)
{
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	sendcounts = malloc(sizeof(int)*size);
         // array describing how many elements to send to each process
	displs = malloc(sizeof(int)*size);
         // array describing the displacements where each segment begins

	int sum =0;
	int rem = ((N)%size); 
	temp_sendcounts = malloc(sizeof(int)*size);
	temp_displs= malloc(sizeof(int)*size);
	updated_sendcounts= malloc(sizeof(int)*size);
	updated_displs = malloc(sizeof(int)*size);
	
	for (int i = 0; i < size; i++) 
	{
        temp_sendcounts[i] = ((N)/size);
		
        if (rem > 0) 
		{
          temp_sendcounts[i]++;
          rem--;
        }
        
        temp_displs[i] = sum;
        sum += temp_sendcounts[i];
		displs[i] = temp_displs[i] * N;
		sendcounts[i]= temp_sendcounts[i]*N;
		
    
		if (i == 0)
		{
			updated_sendcounts[i]= sendcounts[i] + N;
			updated_displs[i] =  displs[i] ;
		}
		
		else if(i == (size-1))
		{
			updated_sendcounts[i]= (sendcounts[i] + N);
		    updated_displs[i] =  ((displs[i]) - (int)N) ;
		}
		else
		{
			updated_sendcounts[i]= sendcounts[i] + 2*N;
		    updated_displs[i] = ((displs[i]) - (int)N)  ;
		}
    }
	if (rank == 0)
	{
		for (int i = 0; i < size; i++) 
		{
			
            printf("New sendcounts[%d] = %d\t New displs[%d] = %d\n ", i, updated_sendcounts[i], i, updated_displs[i]);
			printf("old sendcounts[%d] = %d\t old displs[%d] = %d\n", i, sendcounts[i], i, displs[i]);
		}
	}
	
	
	MPI_Scatterv(&A, updated_sendcounts, updated_displs, MPI_INT, &rec_buf, 15*15, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	 //print what each process received
		//printf("%d: ", rank);
		for (int i = 0; i < N; i++) 
		{
			for(int j = 0; j < N; j++)
			{
				//printf("%d\t", rec_buf[i][j]);
			}
			//printf("\n");
		}
		//printf("\n");
	}
	


void mask_operation(int N)
{
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//printf("%d",updated_sendcounts[rank] / 15);
	
		for ( int i = 1; i<((updated_sendcounts[rank] / 15)-1); i++)
		{
			for (int j =1; j<(N-1); j++)
			{
				AP[i][j] = ((2*rec_buf[i][j]) + rec_buf[i-1][j-1] 
				+ rec_buf[i-1][j]+ rec_buf[i-1][j+1] +rec_buf[i][j-1] 
				+ rec_buf[i][j+1] + rec_buf[i+1][j-1] + rec_buf[i+1][j] 
				+ rec_buf[i+1][j+1])/10; 
				//printf("%d\t",rec_buf[i][j]);
			}
		//printf("\n");
		}
		
	//printf("%d: ", rank);
	for ( int i = 1; i<((updated_sendcounts[rank] / 15)-1); i++)
	{
		for (int j =1; j<(N-1); j++)
		{
			rec_buf[i][j] = AP[i][j];
			//printf("%d\t",AP[i][j]);
		}
		//printf("\n");
	}
	
	
	
	printf("%d: \n", rank);
	for ( int i = 0; i<N; i++)
	{
		for (int j =0; j<N; j++)
		{
					
			printf("%d\t",rec_buf[i][j]);
		}
		printf("\n");
	}
	
	

}
void collect_results(void)
{
	int row = sizeof(A)/sizeof(A[0]);
	int column = sizeof(A[0])/sizeof(A[0][0]); 
	
	
	
	//MPI_Gatherv(&rec_buf, sendcounts[rank], MPI_INT, &A, sendcounts, displs, MPI_INT,  0, MPI_COMM_WORLD);
	MPI_Gatherv(&rec_buf, sendcounts[rank], MPI_INT, &A, sendcounts, displs, MPI_INT,  0, MPI_COMM_WORLD);
	if (0 == rank) {
        printf ("Updated data array: %d %d \n",row,column );
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) 
			{
               printf ("%d\t", A [i][j]);
            }
			printf ("\n");
        }
    }   
	
}

int main(int argc, char **argv)
{ 
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	N = atof(argv[1]);
	
	initialize_data(N);
	
	distribute_data (N); // use scatterv
	
	mask_operation(N);
	
	collect_results(); // use gatherv
	
	MPI_Finalize();
	free(sendcounts);
    free(displs);
	return 0;
}






