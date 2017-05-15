#!usr/bin/perl
system('module load tools/mpich2-1.5-gcc');
system('rm rings1.out rings1.err');
system('mpicc -o rings1 rings1.c');
system('qsub rings1.pbs');
