# procmon: Linux Process Monitoring Tool

A lightweight command-line process monitor for Linux, written in C. It reads process information directly from the `/proc` filesystem and shows the top processes by CPU usage, refreshing every second. It is similar in spirit to `top`, but small enough to read and understand in one sitting.

## Features

- Lists running processes with PID, name, state, CPU % and memory (MB)
- Sorts by CPU usage (highest first) and shows the top 15
- Auto-refreshes every second
- Kills a process from the command line using `kill()`
- Handles processes that exit during a scan
- Handles process names that contain spaces or parentheses
- No external libraries, only the C standard library and Linux system calls

## Project Structure

```
procmon/
├── Makefile
├── proc.h      # Proc struct and function declarations
├── proc.c      # Reads /proc, calculates CPU %, sorts, kills
└── main.c      # Display loop and command-line handling
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

To clean build files:

```bash
make clean
```

## Usage

Start the monitor:

```bash
./procmon
```

Press `Ctrl+C` to quit.

Kill a process by PID (sends `SIGTERM`):

```bash
./procmon kill <pid>
```

### Sample output

```
Linux Process Monitor   Total processes: 24   (Ctrl+C to quit)

PID      NAME                 STATE      CPU%    MEM(MB)
------------------------------------------------------
1234     yes                  R         99.0        0.5
 567     bash                 S          0.0        3.2
   1     init                 S          0.0        1.1
```

### Testing with dummy load

In a second terminal:

```bash
yes > /dev/null &
sleep 500 &
```

`yes` should appear at the top with about 100% CPU. Stop it with:

```bash
pkill yes
```

## How It Works

| Data | Source |
|------|--------|
| Name, state, CPU time | `/proc/[pid]/stat` |
| Resident memory | `/proc/[pid]/statm` |
| Process list | Directory scan of `/proc` (numeric folders are PIDs) |

**CPU % calculation**

1. Take a snapshot of every process's `utime + stime` (CPU ticks used).
2. Wait 1 second.
3. Take a second snapshot.
4. `CPU% = (ticks_after - ticks_before) * 100 / ticks_per_second`

`ticks_per_second` comes from `sysconf(_SC_CLK_TCK)` and is usually 100.

**Memory** is the resident set size (pages in RAM) multiplied by the page size, shown in MB.

**Process names** sit between `(` and the last `)` in `/proc/[pid]/stat`, so the code searches for the last `)` to handle names with spaces or parentheses.

## Limitations

- CPU % is per core, so a process using two cores can show above 100%
- Shows only the top 15 processes
- Only `SIGTERM` is supported for killing
- Inside Docker, only processes in the container are visible unless run with `--pid=host`

## Possible Improvements

- Sort by memory or PID
- Filter by process name
- Renice a process (`setpriority()`)
- Show the user owning each process
- Process tree view using the parent PID
- ncurses interface with scrolling and key controls
- Log high-CPU events to a file

## Technologies Used

- C (C99 or later)
- Linux `/proc` filesystem
- POSIX system calls: `kill()`, `sysconf()`, `opendir()`, `readdir()`
- GNU Make

## Author

**Priyansu Rout**
Embedded Systems Project (Wipro)

## License

This project is licensed under the MIT License. Add a `LICENSE` file if you want to publish it under this license.