#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int rank,size;
int *sendcounts;
int *sc;
int *displs;
int N;

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
double ts;

void initialize_data(long long int N,int A[N][N])
{
    srand(1);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int no;
    if (0 == rank) 
    {
        printf ("original data :\n");
        for (int i = 0; i < N; i++) 
        {
            for (int j = 0; j < N; j++) 
			{
               no=(rand()%256);
               A[i][j]=no;
               printf ("%d\t", A[i][j]);
            }
			printf ("\n");
        }
    }  
}

void distribute_data(long long int N,int A[N][N], int rec_buf[N][N])
{
    int rank, size;     // for storing this process' rank, and the number of processes
    int rem;            // elements remaining after division among processes
    int sum = 0; 
    int i, j; 
    int k = 0, l=0;   
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // added
    //printf("distributed data ----------------\n");
    for (i=0; i<N; i++)
    {
        for (j=0;j<N;j++)
        {
            //printf("%d\t",A[i][j]);
            rec_buf[i][j]=0;
        }
        //printf("\n");
    }

    rem = (N-2)%size;
    sendcounts = malloc(sizeof(int)*size);
    sc = malloc(sizeof(int)*size);
    displs = malloc(sizeof(int)*size);



    // calculate send counts and displacements
    for (i = 0; i < size; i++) 
    {
        sc[i]=(N-2)/size;
        if (rem > 0) 
        {
          sc[i]++;
          //sendcounts[i]=(sc[i]+2)*N;
          rem--;
        }

        displs[i] = sum*N;
        //printf("%d",sc[i]);
        sum += sc[i];
    }

    for (i=0;i<size;i++)
    {
        sendcounts[i]=(sc[i]+2)*N;   
    }
    // print calculated send counts and displacements for each process  
    

    if(0 == rank)
    {
        for (i = 0; i < size; i++) 
            {
                printf("\n sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
            }
        // print what each process received
        
    }
    MPI_Scatterv(A, sendcounts,displs, MPI_INT, rec_buf,N*N,MPI_INT, 0, MPI_COMM_WORLD);
    
    
        printf("matrix part sent to rank: %d \n\n", rank);
        for (i = 0; i < N; i++) 
        {
            for(int j = 0; j < N; j++)
			{
                printf("%d\t", rec_buf[i][j]);
            }
            printf("\n");
        }
}

void mask_operation (long long int N, int rec_buf[N][N], int Ap[N][N], int B[N][N])
{
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
    
    int i,j;
    //printf("Mask process for %d\n",rank);
    for (i=1;i<((sendcounts[rank] / N)-1);i++)
    {
        for(j=1;j<N-1;j++)
        {   
            Ap[i][j] = (rec_buf[i-1][j-1]+rec_buf[i-1][j]+rec_buf[i-1][j+1]+rec_buf[i][j-1]+
                        (rec_buf[i][j]*2)+rec_buf[i][j+1]+rec_buf[i+1][j-1]+rec_buf[i+1][j]+rec_buf[i+1][j+1])/10;
            //printf("%d\t",rec_buf[i][j]);
        }
        //printf("\n");
    }
    
    
    for(i =1;i<((sendcounts[rank] / N)-1);i++)
    {
        for (j = 1; j < N-1; j++) 
        {
            rec_buf[i][j] = Ap[i][j];
        }
    }

    for(i=0; i<=(sendcounts[rank] / N);i++)
    {
        for(j=0;j<N;j++)
        {
            //printf("%d\t",rec_buf[i][j]);
        }
        //printf("\n");
    }

    if (rank == 0)
	{   
        printf("updated values for rank :%d\n",rank);
		for(int i = 0; i<(sendcounts[rank]/N)-1; i++)
		{
			for(int j = 0; j<N; j++)
			{
				B[i][j] = rec_buf[i][j];
                printf("%d\t",B[i][j]);
			}	
            printf("\n");
		}
	}

    else if(rank == (size-1))
	{
        printf("updated values for rank:%d\n",rank);
		for(int i = 1; i<(sendcounts[rank]/N); i++)
		{
			for(int j = 0; j<N; j++)
			{
				B[i-1][j] = rec_buf[i][j];
                printf("%d\t",B[i-1][j]);
			}
            printf("\n");				
		}

	}
    else
	{
        printf("updated values for rank :%d\n",rank);
		for(int i = 1; i<(sendcounts[rank]/N)-1; i++)
		{
			for(int j = 0; j<N; j++)
			{
				B[i-1][j] = rec_buf[i][j];
                printf("%d\t",B[i-1][j]);				
			}	
            printf("\n");			
		}
	}

    
}

void collect_results (long long int N, int B[N][N], int A[N][N], int Ap[N][N])
{   

    for(int i=0;i<size;i++)
        {
            if(i==0)
            {
                sendcounts[i]=sendcounts[i]-N;
            }

            else if(i==(size-1))
            {
                sendcounts[i]=sendcounts[i]-N;
                displs[i]=displs[i]+N;
            }

            else
            {
                sendcounts[i]=sendcounts[i]-N*2;
                displs[i]=displs[i]+N;
            }
        }

    MPI_Gatherv(B, sendcounts[rank], MPI_INT, A, sendcounts, displs, MPI_INT,  0, MPI_COMM_WORLD);
    if (0 == rank) 
    {
        printf ("Updated data array:  \n");
        for (int i = 0; i < N; i++) 
        {
            for (int j = 0; j < N; j++) 
			{
               printf ("%d\t", A[i][j]);
            }
			printf ("\n");
        }
    }    
}


int main(int argc, char *argv[])
{
MPI_Init(&argc, &argv);
long long int N = atof(argv[1]);
int A[N][N], rec_buf[N][N], B[N][N], Ap[N][N];

begin=now();
initialize_data(N,A);
distribute_data (N,A,rec_buf);
mask_operation(N,rec_buf,Ap,B);
collect_results(N,B,A,Ap); 
end=now();
ts = tdiff(begin, end);
if(rank == 0)
{
printf("\n \n total time : %.8f sec\n", ts); 
}
MPI_Finalize();

free(sendcounts);
free(sc);
free(displs);
return 0;

}