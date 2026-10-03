#ifndef PROC_H
#define PROC_H

#define MAX 4096

typedef struct {
    int pid;
    char name[64];
    char state;
    long rss_kb;
    unsigned long long ticks;   /* utime + stime */
    double cpu;                 /* percent */
} Proc;

int  read_proc(int pid, Proc *p);                  /* read one process */
int  snapshot(Proc *arr);                          /* scan all of /proc */
void calc_cpu(Proc *before, int n1, Proc *after, int n2, long hz);
void sort_by_cpu(Proc *arr, int n);
int  kill_process(int pid);                        /* 0 = success */

#endif