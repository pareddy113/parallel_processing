/*Program to distribute an array to a ring network and perform addition of the local sum of the numbers on each processor
Partial sum is performed of all the local sums and the final sum is returned to the Processor 0

1. Processor 0 otherwise called the source node, generates an array of size ‘n’ and calculates the
count of numbers that needs to be distributed to each process. After the calculation it sends the
local sum of its share and the rest of the array to processor 1.
2. Processor 1, now receives the local sum and rest of the array from process 0. It now calculates
the local sum of its share and adds it to the local sum of process 0, to get the partial sum of
process 0 and 1. It then forwards the partial sum and rest of the array to process 2 and it goes
on until p-2.
3. At process p-1, it gets the partial sum of all the process before that and also the remaining array,
it now adds its local sum to the partial sum to get the final sum. It now sends this final sum to
Process 0 which concludes the summation operation of the array.

*/
#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv)
{

    int sum, par_sum,par_sum1,source,destination,source_tag=0,dest_tag=0,source_tag1=1,dest_tag1=1;;
    int rank,j, i, world_size, id, arr_length_to_receive, process_arr_length, arr_length_received, start_len, end_len, arr_length_to_send;

    double total_time,arr_gen_time,send_time, rec_time,arr_add_time;
    total_time=arr_gen_time=send_time=rec_time,arr_add_time=0;

    MPI_Status status;
    int  total_arr_length=1230;
    int arr[total_arr_length];
    int arr2[total_arr_length];
    sum = 0;

    MPI_Init(&argc, &argv);
    total_time-=MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    process_arr_length = total_arr_length / world_size;

//Processor 0 sending the local sum and the array to the next processor i.e. processor 1
    if(rank == 0) {
        destination = rank+1;
        printf("The elements in the array are=\n");
        arr_gen_time-=MPI_Wtime();

        for(i = 0; i < total_arr_length; i++) {	// assigning array with increasing numbers
            arr[i] = i;
            printf("%d,",arr[i]);
        }
        arr_gen_time+=MPI_Wtime();

        arr_add_time-=MPI_Wtime();
        for(i = 0; i < process_arr_length; i++) {
            sum += arr[i];
        }
        for( j=(world_size*process_arr_length); j<total_arr_length; j++) {		//adding numbers that are left after distributing becuase of n not being a factor of p
            sum+=arr[j];
        }
        arr_add_time+=MPI_Wtime();

        printf("\n\nPartial sum of process 0 is %d\n\n", sum);

        start_len =process_arr_length ;
        end_len   = total_arr_length-1;

        arr_length_to_send = end_len - start_len+1;
        send_time-=MPI_Wtime();
        MPI_Send( &arr_length_to_send, 1 , MPI_INT,destination, dest_tag, MPI_COMM_WORLD);

        MPI_Send( &arr[start_len], arr_length_to_send, MPI_INT,destination, dest_tag, MPI_COMM_WORLD);

        MPI_Send(&sum,1,MPI_INT,destination, dest_tag1, MPI_COMM_WORLD);
        send_time+=MPI_Wtime();
    }

    // Processor 1 sending the partial sum and the rest of the array to Processor k-1
    else
    {
        if(rank < world_size-1)
        {
            source = rank-1;
            rec_time-=MPI_Wtime();

            MPI_Recv( &par_sum1, 1, MPI_INT,source, source_tag1, MPI_COMM_WORLD, &status);

            MPI_Recv( &arr_length_to_receive, 1, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);

            MPI_Recv( &arr2, arr_length_to_receive, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);

            rec_time+=MPI_Wtime();
            arr_length_received = arr_length_to_receive;

            par_sum = 0;

            int arr1[total_arr_length];

            arr_add_time-=MPI_Wtime();
            for(i = 0; i <process_arr_length; i++) {
                par_sum += arr2[i];
            }
            arr_add_time+=MPI_Wtime();

            sum=par_sum+par_sum1;
            printf("Partial sum of %d process is =%d\n",rank,par_sum);
            destination=rank+1;

            printf("Partial Aggregate until %d processor is %d\n\n",rank, sum);
            start_len = process_arr_length;
            end_len   = total_arr_length;

            arr_length_to_send = end_len - start_len;

            send_time-=MPI_Wtime();
            MPI_Send( &arr_length_to_send, 1 , MPI_INT,destination, dest_tag, MPI_COMM_WORLD);

            MPI_Send( &arr2[start_len], arr_length_to_send, MPI_INT,destination, dest_tag, MPI_COMM_WORLD);
            MPI_Send(&sum, 1, MPI_INT,destination, dest_tag1, MPI_COMM_WORLD);
            send_time+=MPI_Wtime();
        }

// kth processor to received data from k-1 processor
        else
        {
            source=rank-1;
            rec_time-=MPI_Wtime();
            MPI_Recv(&par_sum1, 1, MPI_INT,source,source_tag1, MPI_COMM_WORLD, &status);

            MPI_Recv( &arr_length_to_receive, 1, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);

            MPI_Recv( &arr2, arr_length_to_receive, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);
            rec_time+=MPI_Wtime();
            arr_length_received = arr_length_to_receive;

            par_sum = 0;

            arr_add_time-=MPI_Wtime();
            for(i = 0; i <process_arr_length; i++) {
                par_sum += arr2[i];
            }
            arr_add_time+=MPI_Wtime();
            sum=par_sum1+par_sum;

            printf("Partial sum of %d  process is =%d\n",rank,par_sum);

            printf("Total Aggregate until %d processor is%d\n\n",rank,rank,sum);
        }

    }

//Processor k-1 sending the total sum to Processor 0

    if (rank== (world_size-1))
    {
        destination=0;
        int sum1=sum;
        send_time-=MPI_Wtime();
        MPI_Send( &sum1, 1, MPI_INT,destination, dest_tag1, MPI_COMM_WORLD);
        send_time+=MPI_Wtime();
    }

//Processor 0 receving the total sum from Processor k-1
    if(rank==0)
    {
        int sum1;
        source=world_size-1;
        rec_time-=MPI_Wtime();
        MPI_Recv(&sum1, 1, MPI_INT,source, source_tag1, MPI_COMM_WORLD, &status);
        rec_time+=MPI_Wtime();
        printf("Final sum is =%d\n\n",sum1);

        total_time+=MPI_Wtime();
        printf("----------Timing Analysis-----\n\n");
        printf("Array Generation time %f\n Parital sum generation time %f\n Total send time %f\n Total receive time %f\n The total time taken %f\n ",arr_gen_time,arr_add_time,send_time,rec_time,total_time);
    }
    MPI_Finalize();

}
