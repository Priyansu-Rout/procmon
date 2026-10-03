#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include "proc.h"

/* Read one process from /proc/[pid]. Returns 1 on success. */
int read_proc(int pid, Proc *p) {
    char path[64], buf[1024];
    FILE *f;

    snprintf(path, sizeof path, "/proc/%d/stat", pid);
    f = fopen(path, "r");
    if (!f) return 0;                       /* process may have exited */
    if (!fgets(buf, sizeof buf, f)) { fclose(f); return 0; }
    fclose(f);

    /* name sits between '(' and the LAST ')', and may contain spaces */
    char *open = strchr(buf, '(');
    char *close = strrchr(buf, ')');
    if (!open || !close) return 0;

    int len = close - open - 1;
    if (len > 63) len = 63;
    memcpy(p->name, open + 1, len);
    p->name[len] = '\0';

    unsigned long long ut, st;
    if (sscanf(close + 2,
               "%c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %llu %llu",
               &p->state, &ut, &st) != 3)
        return 0;

    p->pid = pid;
    p->ticks = ut + st;

    /* memory: 2nd field of statm = resident pages */
    p->rss_kb = 0;
    snprintf(path, sizeof path, "/proc/%d/statm", pid);
    f = fopen(path, "r");
    if (f) {
        long resident = 0;
        if (fscanf(f, "%*d %ld", &resident) == 1)
            p->rss_kb = resident * (sysconf(_SC_PAGESIZE) / 1024);
        fclose(f);
    }
    return 1;
}

/* Scan /proc and fill arr. Returns number of processes. */
int snapshot(Proc *arr) {
    DIR *d = opendir("/proc");
    struct dirent *e;
    int n = 0;
    if (!d) return 0;
    while ((e = readdir(d)) && n < MAX) {
        if (!isdigit((unsigned char)e->d_name[0])) continue;
        if (read_proc(atoi(e->d_name), &arr[n])) n++;
    }
    closedir(d);
    return n;
}

/* CPU% = ticks used between two snapshots (1 sec apart) / ticks per second * 100 */
void calc_cpu(Proc *before, int n1, Proc *after, int n2, long hz) {
    for (int i = 0; i < n2; i++) {
        after[i].cpu = 0;
        for (int j = 0; j < n1; j++) {
            if (before[j].pid == after[i].pid) {
                after[i].cpu = (after[i].ticks - before[j].ticks) * 100.0 / hz;
                break;
            }
        }
    }
}

static int by_cpu(const void *a, const void *b) {
    double x = ((const Proc *)a)->cpu, y = ((const Proc *)b)->cpu;
    return (y > x) - (y < x);
}

void sort_by_cpu(Proc *arr, int n) {
    qsort(arr, n, sizeof(Proc), by_cpu);
}

int kill_process(int pid) {
    return kill(pid, SIGTERM);
}