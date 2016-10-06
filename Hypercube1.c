/*

Hypercube of n dimension has n dimensions in which each dimension having 2 processes. We can make
an n dimension hypercube using two (n-1) dimension Hypercubus. I used this concept and used
MPI_Cart_Create to create a communicator for Hypercube of n dimension where each dimension has 2
processes with no reorder.
The step by step process is as follows:
1. MPI_Cart_create is used to create a new communicator for a hypercube of n dimension with
each dimension having 2 processes and no reorder.
2. I used MPI_Scatter to distribute the array to each of the processes where the local sum is
performed.
3. The MPI_Reduce operation is used to perform the summation of the local sums of all the
processes and send the final sum to the source node.

*/
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
main(int argc, char**argv)
{
    int total_arr_length=1000;
    int main_array[total_arr_length];

    int world_rank1,a,m,rank, process_arr_length, j,sum=0, k, x, *par_sum_array, z,size, world_size1, par_sum=0, dim, *processes_per_dim, *period,global_sum=0;

    double total_time,arr_gen_time,send_time, rec_time,arr_add_time;
    total_time=arr_gen_time=send_time=rec_time,arr_add_time=0;

    MPI_Init(&argc,&argv);
    total_time-=MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank1);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size1);

    process_arr_length=total_arr_length/world_size1;
    int com_array[process_arr_length];
    if(world_rank1 ==0)

    {
        int i;
        printf(" The Array is=");
        arr_gen_time-=MPI_Wtime();
        for(i=0; i<total_arr_length; i++)		// assigning array with increasing numbers
        {
            main_array[i]=i;
            printf("%d, ",main_array[i]);
        }
        arr_gen_time+=MPI_Wtime();
        arr_add_time-=MPI_Wtime();
        for(m=(process_arr_length*world_size1); m<total_arr_length; m++)	//adding numbers that are left after distributing becuase of n not being a factor of p
        {
            sum+=main_array[m];
        }

        arr_add_time+=MPI_Wtime();
    }
    dim = 3;							//dimension of hypercube
    processes_per_dim=(int *) malloc(sizeof(int)*dim);
    period= (int *) malloc(sizeof(int)*dim);
    for(a=0; a<dim; a++)
    {
        processes_per_dim[a]=2;		//each dimension has 2 process
        period[a]=1;				// reorder is false
    }
//creating a communicator for the hypercube topology

    MPI_Comm Hypercube;
    MPI_Cart_create(MPI_COMM_WORLD, dim, processes_per_dim, period, 0, &Hypercube);
    MPI_Comm_rank(Hypercube, &rank);
    MPI_Comm_size(Hypercube, &size);
    send_time-=MPI_Wtime();
    MPI_Scatter(main_array,process_arr_length, MPI_INT,com_array,process_arr_length, MPI_INT,0,Hypercube); //distributing the array to all the processors

    send_time+=MPI_Wtime();
    for(z=0; z<process_arr_length; z++)

    {
        printf("\n Processor %d has %d",rank, com_array[z]);
    }
    arr_add_time-=MPI_Wtime();

//finding local sum of all the processors
    for(j=0; j<process_arr_length; j++)
    {
        par_sum+=com_array[j];
    }
    arr_add_time+=MPI_Wtime();

    printf("\n The partial sum is %d \n",par_sum);
    par_sum_array=malloc(sizeof(int)*world_size1);
    MPI_Barrier(MPI_COMM_WORLD);
    rec_time-=MPI_Wtime();
    MPI_Reduce(&par_sum, &global_sum,1, MPI_INT,MPI_SUM,0, Hypercube);  //calculating the final sum and seding it to processor 0
    rec_time+=MPI_Wtime();

//processor 0 receving the final sum from all the processors
    if(rank == 0)
    {
        printf("\n The Final sum is %d \n",global_sum+sum);
        arr_add_time+=MPI_Wtime();
        total_time+=MPI_Wtime();
        printf("----------Timing Analysis-----\n\n");
        printf("Array Generation time %f\n Parital sum generation time %f\n Total send time %f\n Total receive time %f\n The total time taken %f\n ",arr_gen_time,arr_add_time,send_time,rec_time,total_time);
    }

    MPI_Finalize();
}
