#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "proc.h"

#define CPU_ALERT 50.0

static Proc before[MAX], after[MAX];

static void usage(const char *prog) {
    printf("Usage:\n"
           "  %s [-s cpu|mem|pid] [-f name] [-n count]   monitor\n"
           "  %s kill <pid>                               send SIGTERM\n"
           "  %s renice <pid> <nice>                      change priority\n",
           prog, prog, prog);
}

int main(int argc, char *argv[]) {
    char mode = 'c';
    char filter[64] = "";
    int top = 15;

    /* ---- one-shot commands ---- */
    if (argc == 3 && strcmp(argv[1], "kill") == 0) {
        int pid = atoi(argv[2]);
        if (kill_process(pid) == 0) printf("Sent SIGTERM to %d\n", pid);
        else perror("kill");
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "renice") == 0) {
        int pid = atoi(argv[2]), nice = atoi(argv[3]);
        if (renice_process(pid, nice) == 0) printf("Set nice of %d to %d\n", pid, nice);
        else perror("renice");
        return 0;
    }

    /* ---- options for monitor mode ---- */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "mem") == 0) mode = 'm';
            else if (strcmp(argv[i], "pid") == 0) mode = 'p';
            else mode = 'c';
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            snprintf(filter, sizeof filter, "%s", argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            top = atoi(argv[++i]);
            if (top < 1) top = 15;
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    long hz = sysconf(_SC_CLK_TCK);          /* usually 100 */

    for (;;) {
        int n1 = snapshot(before);
        sleep(1);
        int n2 = snapshot(after);

        calc_cpu(before, n1, after, n2, hz);
        log_high_cpu(after, n2, CPU_ALERT);
        n2 = filter_procs(after, n2, filter);
        sort_procs(after, n2, mode);

        printf("\033[H\033[J");              /* clear screen */
        printf("Linux Process Monitor   Processes: %d   Sort: %s   Filter: %s   (Ctrl+C to quit)\n\n",
               n2, mode == 'm' ? "MEM" : mode == 'p' ? "PID" : "CPU",
               filter[0] ? filter : "none");
        printf("%-8s %-10s %-20s %-6s %8s %10s\n",
               "PID", "USER", "NAME", "STATE", "CPU%", "MEM(MB)");
        printf("------------------------------------------------------------------\n");
        for (int i = 0; i < n2 && i < top; i++)
            printf("%-8d %-10.10s %-20.20s %-6c %8.1f %10.1f\n",
                   after[i].pid, after[i].user, after[i].name, after[i].state,
                   after[i].cpu, after[i].rss_kb / 1024.0);
    }
    return 0;
}