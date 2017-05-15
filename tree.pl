
#!usr/bin/perl
system('module load tools/mpich2-1.5-gcc');
system('rm tree.out tree.err');
system('mpicc -o tree tree.c');
system('qsub tree.pbs');
