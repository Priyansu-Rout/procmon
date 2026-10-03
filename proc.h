#ifndef PROC_H
#define PROC_H

#define MAX 4096

typedef struct {
    int pid;
    char name[64];
    char state;
    long rss_kb;
    unsigned long long ticks;   
    double cpu;                 
} Proc;

int  read_proc(int pid, Proc *p);                  
int  snapshot(Proc *arr);                          
void calc_cpu(Proc *before, int n1, Proc *after, int n2, long hz);
void sort_by_cpu(Proc *arr, int n);
int  kill_process(int pid);                        

#endif