#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include "proc.h"

#define LOG_FILE "procmon.log"

/* Read one process from /proc/[pid]. Returns 1 on success. */
int read_proc(int pid, Proc *p) {
    char path[64], buf[1024];
    FILE *f;

    snprintf(path, sizeof path, "/proc/%d/stat", pid);
    f = fopen(path, "r");
    if (!f) return 0;                       /* process may have exited */
    if (!fgets(buf, sizeof buf, f)) { fclose(f); return 0; }
    fclose(f);

    /* name sits between '(' and the LAST ')' and may contain spaces */
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

    /* owner: the uid that owns the /proc/[pid] directory */
    strcpy(p->user, "?");
    snprintf(path, sizeof path, "/proc/%d", pid);
    struct stat sb;
    if (stat(path, &sb) == 0) {
        struct passwd *pw = getpwuid(sb.st_uid);
        if (pw) snprintf(p->user, sizeof p->user, "%s", pw->pw_name);
        else    snprintf(p->user, sizeof p->user, "%u", (unsigned)sb.st_uid);
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
                if (after[i].ticks >= before[j].ticks)
                    after[i].cpu = (after[i].ticks - before[j].ticks) * 100.0 / hz;
                break;
            }
        }
    }
}

static char sort_mode = 'c';

static int cmp(const void *a, const void *b) {
    const Proc *x = a, *y = b;
    if (sort_mode == 'm') return (y->rss_kb > x->rss_kb) - (y->rss_kb < x->rss_kb);
    if (sort_mode == 'p') return x->pid - y->pid;
    return (y->cpu > x->cpu) - (y->cpu < x->cpu);
}

void sort_procs(Proc *arr, int n, char mode) {
    sort_mode = mode;
    qsort(arr, n, sizeof(Proc), cmp);
}

/* Keep only processes whose name contains f. Returns new count. */
int filter_procs(Proc *arr, int n, const char *f) {
    if (!f || !f[0]) return n;
    int k = 0;
    for (int i = 0; i < n; i++)
        if (strstr(arr[i].name, f))
            arr[k++] = arr[i];
    return k;
}

int kill_process(int pid) {
    return kill(pid, SIGTERM);
}

int renice_process(int pid, int nice) {
    return setpriority(PRIO_PROCESS, (id_t)pid, nice);
}

/* Append a line to procmon.log for every process above the threshold */
void log_high_cpu(const Proc *arr, int n, double threshold) {
    FILE *f = NULL;
    char ts[32];
    time_t t = time(NULL);
    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", localtime(&t));

    for (int i = 0; i < n; i++) {
        if (arr[i].cpu > threshold) {
            if (!f && !(f = fopen(LOG_FILE, "a"))) return;
            fprintf(f, "[%s] HIGH CPU pid=%d name=%s cpu=%.1f%%\n",
                    ts, arr[i].pid, arr[i].name, arr[i].cpu);
        }
    }
    if (f) fclose(f);
}