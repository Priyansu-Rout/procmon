#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../proc.h"

static int fails = 0;
#define CHECK(name, cond) do { \
    int ok = (cond); \
    printf("%-35s %s\n", name, ok ? "PASS" : "FAIL"); \
    if (!ok) fails++; \
} while (0)

int main(void) {
    Proc p;
    CHECK("read_proc(PID 1) succeeds", read_proc(1, &p) == 1);
    CHECK("read_proc(invalid PID) fails", read_proc(99999999, &p) == 0);
    CHECK("read_proc(own PID) succeeds", read_proc(getpid(), &p) == 1);

    Proc b[1] = {{ .pid = 5, .ticks = 100 }};
    Proc a[1] = {{ .pid = 5, .ticks = 150 }};
    calc_cpu(b, 1, a, 1, 100);
    CHECK("calc_cpu 50 ticks/100hz = 50%", a[0].cpu > 49.9 && a[0].cpu < 50.1);

    Proc f[3] = {{ .pid = 1 }, { .pid = 2 }, { .pid = 3 }};
    strcpy(f[0].name, "bash"); strcpy(f[1].name, "sleep"); strcpy(f[2].name, "bashrc");
    CHECK("filter_procs keeps 2 of 3", filter_procs(f, 3, "bash") == 2);

    CHECK("kill_process(invalid) fails", kill_process(99999999) == -1);

    printf("\n%s\n", fails ? "SOME TESTS FAILED" : "ALL TESTS PASSED");
    return fails != 0;
}