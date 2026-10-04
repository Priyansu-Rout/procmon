# procmon: Linux Process Monitoring Tool
## Final Project Report

**Student:** Priyansu Rout
**Program:** Embedded Systems Project (Wipro)
**Guide:** [Name]
**Date:** [Date]

## 1. Abstract
procmon is a lightweight command-line Linux process monitor written in C. It reads process data from the /proc filesystem and displays PID, user, name, state, CPU % and memory, refreshing every second. It supports sorting, filtering, killing, renicing and logging of high-CPU events.

## 2. Introduction
See 01_introduction.md. The tool helps learners understand how Linux exposes process information and is small enough for embedded systems.

## 3. Requirements
Summary of the functional and non-functional requirements from 02_PRD.md (FR1 to FR9, NFR1 to NFR6).

## 4. Design
Architecture, data structure and UML diagrams are in 03_design.md. The program has three layers: user interface (main.c), process logic (proc.c) and the Linux kernel (/proc and system calls).

## 5. Implementation
- **Process discovery:** numeric folders in /proc are PIDs
- **Data sources:** /proc/[pid]/stat (name, state, utime, stime), /proc/[pid]/statm (resident pages), owner of /proc/[pid] (user)
- **CPU %:** `(ticks_after - ticks_before) * 100 / ticks_per_second` over two snapshots 1 second apart
- **Memory:** resident pages x page size, shown in MB
- **Actions:** kill() with SIGTERM and setpriority()
- **Logging:** processes above 50% CPU appended to procmon.log with timestamp
- **Robustness:** every file open and parse is checked; a process that exits mid-scan is skipped

## 6. Testing
Unit, integration, system and negative tests with results are in 05_testing.md. All unit tests pass. Defects found were fixed and are recorded in the progress log.

## 7. Results
Insert screenshots:
- Monitor sorted by CPU with `yes` at the top
- Sorted by memory and filtered output
- procmon.log content
- Unit test output (ALL TESTS PASSED)

## 8. Achievements
- Working modular tool with no external libraries
- Full set of planned features implemented
- Unit tests and documented test results
- Clear Git history and documentation for every stage

## 9. Limitations
- Linux only
- CPU % is per core, so it can exceed 100%
- Only SIGTERM supported
- Negative nice values need root
- Output appears after about 1 second

## 10. Future Improvements
- ncurses interface with scrolling and key controls
- Process tree view using parent PID
- System-wide CPU and memory summary
- Configurable refresh interval and alert threshold
- Full command line from /proc/[pid]/cmdline

## 11. Conclusion
The project followed the full cycle of requirements, design, implementation, testing and delivery, and produced a working tool that shows how Linux process monitoring works internally.

## 12. References
- Linux man pages: proc(5), kill(2), setpriority(2), sysconf(3)
- Project repository: github.com/[username]/procmon