

#!usr/bin/perl
system('module load tools/mpich2-1.5-gcc');
system('rm rings.out rings.err');
system('mpicc -o rings rings.c');
system('qsub rings.pbs');
