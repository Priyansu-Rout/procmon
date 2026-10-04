# procmon: Linux Process Monitoring Tool

A lightweight command-line process monitor for Linux, written in C. It reads process information directly from the `/proc` filesystem and shows the top processes by CPU or memory, refreshing every second. It is similar in spirit to `top`, but small enough to read and understand in one sitting.

## Features

- Lists running processes with PID, user, name, state, CPU % and memory (MB)
- Sorts by CPU, memory or PID
- Filters processes by name
- Limits how many rows are displayed
- Auto-refreshes every second
- Kills a process by PID (`SIGTERM`)
- Changes process priority (renice)
- Logs high-CPU events (above 50%) to `procmon.log` with timestamps
- Handles processes that exit during a scan
- Handles process names that contain spaces or parentheses
- Includes unit tests
- No external libraries, only the C standard library and Linux system calls

## Project Structure

```
procmon/
├── Makefile
├── proc.h          # Proc struct and function declarations
├── proc.c          # Reads /proc, CPU %, sort, filter, kill, renice, logging
├── main.c          # Display loop and command-line handling
├── tests/
│   └── test_proc.c # Unit tests
├── docs/           # Project documents, UML diagrams, report
└── README.md
```

## Requirements

- Linux (Ubuntu, Debian, WSL2, Alpine, etc.)
- `gcc` and `make`

On Ubuntu / WSL2:

```bash
sudo apt update
sudo apt install -y build-essential
```

## Build

```bash
git clone https://github.com/<your-username>/procmon.git
cd procmon
make
```

Clean build files:

```bash
make clean
```

## Usage

```
./procmon [-s cpu|mem|pid] [-f name] [-n count]   monitor
./procmon kill <pid>                              send SIGTERM
./procmon renice <pid> <nice>                     change priority
```

| Option | Meaning | Default |
|--------|---------|---------|
| `-s`   | Sort by `cpu`, `mem` or `pid` | `cpu` |
| `-f`   | Show only processes whose name contains this text | none |
| `-n`   | Number of rows to display | 15 |

### Examples

```bash
./procmon                     # top 15 by CPU
./procmon -s mem              # sort by memory
./procmon -f bash -n 5        # names containing "bash", top 5 rows
./procmon renice 1234 10      # lower priority of PID 1234
sudo ./procmon renice 1234 -5 # raising priority needs root
./procmon kill 1234           # send SIGTERM to PID 1234
```

Press `Ctrl+C` to quit the monitor.

### Sample output

```
Linux Process Monitor   Processes: 24   Sort: CPU   Filter: none   (Ctrl+C to quit)

PID      USER       NAME                 STATE      CPU%    MEM(MB)
------------------------------------------------------------------
1234     user       yes                  R          99.0        0.5
567      user       bash                 S           0.0        3.2
1        root       init                 S           0.0        1.1
```

### Testing with dummy load

In a second terminal:

```bash
yes > /dev/null &
sleep 500 &
```

`yes` should appear at the top with about 100% CPU and will also be written to `procmon.log`. Stop it with:

```bash
pkill yes
```

### Log file

Any process above 50% CPU is appended to `procmon.log`:

```
[2025-01-15 21:30:12] HIGH CPU pid=1234 name=yes cpu=99.0%
```

## Running the Tests

```bash
gcc -Wall tests/test_proc.c proc.c -o test_proc && ./test_proc
```

Expected output:

```
read_proc(PID 1) succeeds           PASS
read_proc(invalid PID) fails        PASS
read_proc(own PID) succeeds         PASS
calc_cpu 50 ticks/100hz = 50%       PASS
filter_procs keeps 2 of 3           PASS
kill_process(invalid) fails         PASS

ALL TESTS PASSED
```

| Test type | What is checked |
|-----------|-----------------|
| Unit | `read_proc()` on valid, invalid and own PID |
| Unit | `calc_cpu()` with known tick values |
| Unit | `filter_procs()` keeps only matching names |
| Negative | `kill_process()` on an invalid PID returns an error |

## How It Works

| Data | Source |
|------|--------|
| Name, state, CPU time | `/proc/[pid]/stat` |
| Resident memory | `/proc/[pid]/statm` |
| Owner (user) | Owner of the `/proc/[pid]` directory, resolved with `getpwuid()` |
| Process list | Directory scan of `/proc` (numeric folders are PIDs) |

**CPU % calculation**

1. Take a snapshot of every process's `utime + stime` (CPU ticks used).
2. Wait 1 second.
3. Take a second snapshot.
4. `CPU% = (ticks_after - ticks_before) * 100 / ticks_per_second`

`ticks_per_second` comes from `sysconf(_SC_CLK_TCK)` and is usually 100.

**Memory** is the resident set size (pages in RAM) multiplied by the page size, shown in MB.

**Process names** sit between `(` and the last `)` in `/proc/[pid]/stat`, so the code searches for the last `)` to handle names with spaces or parentheses.

**Actions** use the system calls `kill()` and `setpriority()`.

## Architecture

```
+--------------------------------------------+
|                 main.c (UI)                |
|   display loop | arguments | options       |
+---------------------+----------------------+
                      |
              +-------v--------+
              |     proc.c     |
              | snapshot, CPU%, |
              | sort, filter,   |
              | kill, renice,   |
              | logging         |
              +-------+--------+
                      |
       +--------------v---------------+
       |  Linux kernel: /proc, syscalls |
       +------------------------------+
```

## Limitations

- CPU % is per core, so a process using two cores can show above 100%
- Reads only `/proc`, so it works on Linux only
- Only `SIGTERM` is supported for killing
- Raising priority (negative nice) needs root
- Inside Docker, only processes in the container are visible unless run with `--pid=host`
- First screen takes about 1 second because two snapshots are needed

## Possible Improvements

- ncurses interface with scrolling and key controls
- Process tree view using the parent PID
- Configurable refresh interval and CPU alert threshold
- Show full command line from `/proc/[pid]/cmdline`
- System-wide CPU and memory summary from `/proc/stat` and `/proc/meminfo`

## Development Process

This project followed a staged approach: introduction, requirements, design, prototype, testing, and final delivery. Project documents are in the `docs/` folder.

Git commit convention: `feat:`, `fix:`, `docs:`, `test:`.

## Technologies Used

- C (C99 or later)
- Linux `/proc` filesystem
- POSIX system calls: `kill()`, `setpriority()`, `sysconf()`, `opendir()`, `readdir()`, `stat()`
- GNU Make
- Git and GitHub

## Author

**Priyansu Rout**
Embedded Systems Project (Wipro)

## License

This project is licensed under the MIT License. Add a `LICENSE` file if you want to publish it under this license.