/*
The step by step process is as follows:
1. Processor 0 otherwise called the source node, generates an array of size ‘n’ and calculates the
count of numbers that needs to be distributed to each slave process. After the calculation it
sends each slave process with a subset of the array.
2. Each slave process receives the subset of the array, performs the local sum and sends the local
sum back to the master.
3. The master process collects the local sums of all the slave processes and now calculates the
global sum of the array.
*/

#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv)
{
    int sum, par_sum;
    double total_time, total_start_time,total_end_time,arr_gen_time,arr_start_time,arr_end_time,send_time,send_start_time,send_end_time, rec_time,rec_start_time,rec_end_time,arr_add_time,arr_add_start,arr_add_end, total_send_time,send_start_time1,send_end_time1,send_time1,rec_time1,rec_start_time1,rec_end_time1,total_rec_time,arr_add_start1,arr_add_end1,array_add_time;
    int rank, i, world_size, id, arr_length_to_receive, process_arr_length, source, arr_length_received, start_len, end_len, arr_length_to_send;
    MPI_Status status;

    int  total_arr_length=120;
    int arr[total_arr_length];
    int arr2[total_arr_length];
    MPI_Init(&argc, &argv);

    total_start_time=MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

// Processor 0 distributing the other processors with the array
    if(rank == 0) {

        process_arr_length = total_arr_length / world_size;
        printf("The elements in the array are=\n");
        arr_start_time=MPI_Wtime();
        for(i = 0; i < total_arr_length; i++) {	// assigning array with increasing numbers
            arr[i] = i;
            printf("%d,",arr[i]);
        }
        arr_end_time=MPI_Wtime();
        arr_gen_time=arr_end_time-arr_start_time;
        sum = 0;

        arr_add_start=MPI_Wtime();
        for(i = 0; i < process_arr_length ; i++) {
            sum += arr[i];
        }
        arr_add_end=MPI_Wtime();
        arr_add_time=arr_add_end-arr_add_start;

        printf("\n\nThe partial sum of process 0 is %d\n", sum);


        for(id = 1; id < world_size; id++) {
            start_len = id*process_arr_length ;
            end_len   = ((id + 1)*process_arr_length)-1;

            if((total_arr_length - end_len) <= process_arr_length)
                end_len = total_arr_length - 1;

            arr_length_to_send = end_len - start_len + 1;
            send_start_time=MPI_Wtime();
            MPI_Send( &arr_length_to_send, 1 , MPI_INT,id, 1, MPI_COMM_WORLD);

            MPI_Send( &arr[start_len], arr_length_to_send, MPI_INT,id, 1, MPI_COMM_WORLD);

            send_end_time=MPI_Wtime();

            send_time=send_end_time-send_start_time;
        }


        for(id = 1; id < world_size; id++) {
            rec_start_time1=MPI_Wtime();
            MPI_Recv( &par_sum, 1, MPI_INT, MPI_ANY_SOURCE,2, MPI_COMM_WORLD, &status);
            rec_end_time1=MPI_Wtime();
            rec_time1=rec_end_time1-rec_start_time1;
            source = status.MPI_SOURCE;

            printf("\n\nThe partial sum of process %d is %d\n", source,par_sum);

            sum += par_sum;
        }
        total_end_time=MPI_Wtime();
        total_time=total_end_time-total_start_time;
        printf("\n\nFinal added total is: %d\n", sum);

    }

//Processors 1 to k-1 receving the array from procesosr 0, performing local sum and sending it to processor 0
    else {
        rec_start_time=MPI_Wtime();
        MPI_Recv( &arr_length_to_receive, 1, MPI_INT,0, 1, MPI_COMM_WORLD, &status);

        MPI_Recv( &arr2, arr_length_to_receive, MPI_INT,0, 1, MPI_COMM_WORLD, &status);
        rec_end_time=MPI_Wtime();
        rec_time=rec_end_time-rec_start_time;
        arr_length_received = arr_length_to_receive;

        par_sum = 0;


        arr_add_start=MPI_Wtime();
        for(i = 0; i < arr_length_received; i++) {
            par_sum += arr2[i];
        }
        arr_add_end=MPI_Wtime();
        arr_add_time=arr_add_end-arr_add_start;

        send_start_time=MPI_Wtime();
        MPI_Send( &par_sum, 1, MPI_INT, 0,2, MPI_COMM_WORLD);
        send_end_time=MPI_Wtime();
        send_time1=send_end_time-send_start_time;
    }
    total_send_time=send_time+send_time1;
    total_rec_time=rec_time+rec_time1;
    printf("Rank=%d\nArray generation time=  %f\n Partial sum generation time= %f\n Send communication time=%f\n receive communication time=%f\n total parallel run time=%f\n",rank,arr_gen_time,arr_add_time,total_send_time,total_rec_time,total_time);
    MPI_Finalize();
}

