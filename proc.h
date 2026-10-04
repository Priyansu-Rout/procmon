#ifndef PROC_H
#define PROC_H

#define MAX 4096

typedef struct {
    int pid;
    char name[64];
    char user[32];
    char state;
    long rss_kb;
    unsigned long long ticks;   /* utime + stime */
    double cpu;                 /* percent */
} Proc;

int  read_proc(int pid, Proc *p);
int  snapshot(Proc *arr);
void calc_cpu(Proc *before, int n1, Proc *after, int n2, long hz);
void sort_procs(Proc *arr, int n, char mode);        /* 'c' cpu, 'm' mem, 'p' pid */
int  filter_procs(Proc *arr, int n, const char *f);  /* returns new count */
int  kill_process(int pid);                          /* 0 = success */
int  renice_process(int pid, int nice);              /* 0 = success */
void log_high_cpu(const Proc *arr, int n, double threshold);

#endif