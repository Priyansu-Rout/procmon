# Stage 1: Project Introduction

## Project Title
procmon: Linux Process Monitoring Tool

## Idea and Objective
Build a lightweight command-line tool in C that reads the Linux `/proc` filesystem and shows live CPU and memory usage for every running process, with the ability to sort, filter, kill, renice and log.

## Problem Statement
Tools like `top` and `htop` are large, and beginners do not see how they get their data. Minimal embedded Linux boards often do not have them installed. A small, readable tool helps learners understand how Linux exposes process information.

## Scope
**In scope**
- Process listing (PID, user, name, state, CPU %, memory)
- Sort by CPU, memory or PID
- Filter by name
- Kill and renice a process
- Log high-CPU events

**Out of scope**
- GUI or web interface
- Remote monitoring
- Network and disk monitoring
- Non-Linux systems

## Expected Outcome
A working, modular, tested command-line tool with documentation and a Git repository.

## Applications
- Learning OS internals (processes, scheduling, `/proc`)
- Finding runaway processes
- Monitoring embedded Linux devices

## Next Stage
Stage 2: write the PRD, define modules and prepare the timeline.