/*Program to distribute an array to a ring network and perform addition of the local sum of the numbers on each processor
Processor 0 receives the local sums from all the processors and performs the final addition

The step by step process is as follows:
1. Processor 0 otherwise called the source node, generates an array of size ‘n’ and calculates the
count of numbers that needs to be distributed to each process. After the calculation it sends the
rest of the array to processor 1.
2. Processor 1, now receives the rest of the array from process 0. It now calculates the local sum of
its share and sends the local sum to process 0. It then forwards the rest of the array to process 2
and it goes on until p-1.
3. At process p-1, it gets the remaining array and performs the local sum operation and sends it to
process 0.
4. Process 0 collects all the local sums from all the processes and it adds all the local sums to get
the global sum of the entire array.
*/
#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv)
{

    int sum, par_sum,par_sum1,source,destination,source_tag=0,dest_tag=0,source_tag1=1,dest_tag1=1;;
    int rank,j, i, world_size, id, arr_length_to_receive, process_arr_length, arr_length_received, start_len, end_len, arr_length_to_send;
    MPI_Status status;
    int  total_arr_length=100;
    int arr[total_arr_length];
    int arr2[total_arr_length];

    double total_time,arr_gen_time,send_time, rec_time,arr_add_time;
    total_time=arr_gen_time=send_time=rec_time,arr_add_time=0;

    sum = 0;

    MPI_Init(&argc, &argv);

    total_time-=MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    process_arr_length = total_arr_length / world_size;

    //for first processor which is the source node

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
        for( j=(world_size*process_arr_length); j<total_arr_length; j++) {	//adding numbers that are left after distributing becuase of n not being a factor of p
            sum+=arr[j];
        }
        arr_add_time+=MPI_Wtime();

        printf("\n\nThe partial sum of process 0 is %d\n", sum);

        start_len =process_arr_length ;
        end_len   = total_arr_length-1;

        arr_length_to_send = end_len - start_len+1;
        send_time-=MPI_Wtime();
        MPI_Send( &arr_length_to_send, 1 , MPI_INT,destination, dest_tag, MPI_COMM_WORLD);

        MPI_Send( &arr[start_len], arr_length_to_send, MPI_INT,destination, dest_tag, MPI_COMM_WORLD);
        send_time+=MPI_Wtime();

    }

    // Below if for 1 to k-1 processors
    else
    {
        if(rank < world_size-1)
        {
            source = rank-1;
            rec_time-=MPI_Wtime();
            MPI_Recv( &arr_length_to_receive, 1, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);

            MPI_Recv( &arr2, arr_length_to_receive, MPI_INT,source, source_tag, MPI_COMM_WORLD, &status);

            rec_time+=MPI_Wtime();
            arr_length_received = arr_length_to_receive;

            par_sum = 0;
            arr_add_time-=MPI_Wtime();
            int arr1[total_arr_length];
            for(i = 0; i <process_arr_length; i++) {
                par_sum += arr2[i];
            }
            arr_add_time+=MPI_Wtime();
            destination=rank+1;

            start_len = process_arr_length;
            end_len   = total_arr_length;

            arr_length_to_send = end_len - start_len;
            send_time-=MPI_Wtime();
            MPI_Send( &arr_length_to_send, 1 , MPI_INT,destination, dest_tag, MPI_COMM_WORLD);

            MPI_Send( &arr2[start_len], arr_length_to_send, MPI_INT,destination, dest_tag, MPI_COMM_WORLD);
            MPI_Send(&par_sum, 1, MPI_INT,0, dest_tag1, MPI_COMM_WORLD);
            send_time+=MPI_Wtime();
        }

//	below is for kth processor to received data from k-1 processor
        else
        {
            source=rank-1;

            rec_time-=MPI_Wtime();
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

//k-1 processor sending sum to 0 th processor

            destination=0;
            send_time-=MPI_Wtime();
            MPI_Send( &par_sum, 1, MPI_INT,0, dest_tag1, MPI_COMM_WORLD);
        }

        send_time+=MPI_Wtime();
    }
    MPI_Barrier(MPI_COMM_WORLD);

// processor 0 receiveing data from k-1 processor
    if(rank==0)
    {
        int sum1,k;
        source=world_size-1;
        for(k=1; k<world_size; k++)
        {
            rec_time-=MPI_Wtime();
            MPI_Recv(&sum1, 1, MPI_INT,k, source_tag1, MPI_COMM_WORLD, &status);
            rec_time+=MPI_Wtime();
            printf("The received partial sums from %d is=%d\n",k,sum1);
            sum+=sum1;
        }
        printf("Final sum is =%d\n\n",sum);

        total_time+=MPI_Wtime();
        printf("--------Timing Analysis-------\n\n");
        printf("Array Generation time %f\n Parital sum generation time %f\n Total send time %f\n Total receive time %f\n The total time taken %f\n ",arr_gen_time,arr_add_time,send_time,rec_time,total_time);
    }
    MPI_Finalize();
}
