#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "proc.h"

static Proc before[MAX], after[MAX];

int main(int argc, char *argv[]) {
    /* usage: ./procmon kill <pid> */
    if (argc == 3 && strcmp(argv[1], "kill") == 0) {
        int pid = atoi(argv[2]);
        if (kill_process(pid) == 0) printf("Sent SIGTERM to %d\n", pid);
        else perror("kill");
        return 0;
    }

    long hz = sysconf(_SC_CLK_TCK);          /* usually 100 */

    for (;;) {
        int n1 = snapshot(before);
        sleep(1);
        int n2 = snapshot(after);

        calc_cpu(before, n1, after, n2, hz);
        sort_by_cpu(after, n2);

        printf("\033[H\033[J");              /* clear screen */
        printf("Linux Process Monitor   Total processes: %d   (Ctrl+C to quit)\n\n", n2);
        printf("%-8s %-20s %-6s %8s %10s\n", "PID", "NAME", "STATE", "CPU%", "MEM(MB)");
        printf("------------------------------------------------------\n");
        for (int i = 0; i < n2 && i < 15; i++)
            printf("%-8d %-20.20s %-6c %8.1f %10.1f\n",
                   after[i].pid, after[i].name, after[i].state,
                   after[i].cpu, after[i].rss_kb / 1024.0);
    }
    return 0;
}