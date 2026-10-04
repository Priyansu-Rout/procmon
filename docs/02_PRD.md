# Stage 2: Project Requirements Document (PRD)

## 1. Purpose
Define what procmon must do, how well it must do it, and how it will be delivered.

## 2. Functional Requirements
| ID | Requirement | Priority |
|----|-------------|----------|
| FR1 | List all running processes (PID, user, name, state, memory) | High |
| FR2 | Calculate per-process CPU % | High |
| FR3 | Auto-refresh every second | High |
| FR4 | Sort by CPU, memory or PID | Medium |
| FR5 | Filter by process name | Medium |
| FR6 | Kill a process by PID | High |
| FR7 | Change process priority (renice) | Medium |
| FR8 | Log processes above 50% CPU to a file | Medium |
| FR9 | Limit number of displayed rows | Low |

## 3. Non-Functional Requirements
| ID | Category | Requirement |
|----|----------|-------------|
| NFR1 | Performance | One scan completes in under 100 ms |
| NFR2 | Reliability | No crash if a process exits during a scan |
| NFR3 | Portability | Runs on any Linux (Ubuntu, WSL2, Alpine) |
| NFR4 | Usability | Clear table output, simple commands |
| NFR5 | Maintainability | Modular code, comments, Makefile |
| NFR6 | Resource use | Minimal CPU and memory |

## 4. Modules
| Module | File | Responsibility |
|--------|------|----------------|
| Data collection | proc.c / proc.h | Read /proc, snapshot, CPU % |
| Processing | proc.c | Sort, filter |
| Actions | proc.c | Kill, renice |
| Logging | proc.c | Write high-CPU events to procmon.log |
| Interface | main.c | Arguments, refresh loop, table output |
| Build | Makefile | Compile and clean |
| Tests | tests/test_proc.c | Unit tests |

## 5. Deliverables
- Source code (proc.h, proc.c, main.c, Makefile)
- Unit tests
- README, PRD, design document with UML, test report
- Git repository with commit history
- Final report and presentation

## 6. Timeline
| Week | Work | Stage |
|------|------|-------|
| 1 | Introduction and PRD | 1, 2 |
| 2 | Architecture, UML, repository setup | 3 |
| 3 | Prototype: /proc reading, CPU % | 4 |
| 4 | Sort, filter, kill, renice, logging | 4, 5 |
| 5 | Testing and bug fixes | 5 |
| 6 | Report, demo, presentation | 6 |

## 7. Assumptions and Constraints
- Runs on Linux only
- Raising priority (negative nice) needs root
- Developed on Ubuntu (WSL2) with gcc and make

## Next Stage
Stage 3: architecture, UML diagrams, data structures, repository and tools setup.