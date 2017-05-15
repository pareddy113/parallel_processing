# parallel_processing
parallel processing course project 1


To introduce to run very simple MPI-2 programs (use C) on the EXTREME cluster and familiarize with simple MPI-2 calls, and how to execute programs on clusters.

The goal is to create a large array A of n numbers at one node (let us call it the source node), distribute blocks of the array across the processors, and find the global sum of the numbers in A.

Use and familiarize with different topologies such as Ring, Tree and Hypercube with different configurations of sending data between Processors.

Uses MPI to communicate with processors and submit the job to the Server.


RUN:
.pbs files are the job files that are submitted to the cluster where we specify the number of nodes, name of the executable file, and all other configurations for the job to run.

.pl files are the script files that automate the whole process by executing the c program and submitting the .pbs job to the cluster.

.err files give any error while executing the job

.out files are the output files of the job being executed

Run the .pl file to start the program execution on the server.
