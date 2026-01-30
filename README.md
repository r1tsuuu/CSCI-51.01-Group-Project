# CSCI-51.01-Group-Project

## Overview
This project involves creating a C++ program that simulates 5 CPU scheduling algorithms for a uniprocessor system. The simulator examines a ready queue and selects processes for execution based on specific algorithm rules.

## Grouping
* **Members:** 2 to 3 members per group.
* **Defense:** Onsite defense with major and minor questions.

## Scoring
* **Base Score:** 60 points for a working program.
* **Test Cases:** 10 to 11 test cases provided. (-5 points per incorrect/lacking test case).
* **Defense:** 2 major questions (20 points each) + variable minor questions.
* **Peer Eval:** Individual scores adjusted based on peer evaluation.

## Supported Algorithms
1.  **FCFS** — First Come First Served
2.  **SJF** — Shortest Job First (non-preemptive)
3.  **SRTF** — Shortest Remaining Time First (SJF preemptive)
4.  **P** — Priority (preemptive). Uses "Nice level" (-20 to +20).
5.  **RR** — Round-Robin. Requires a time quantum `Q`.

## Specifications
1.  **System:** Uniprocessor, zero overhead (instant context-switching).
2.  **Indexing:** Process indices start at **1**.
3.  **Tie-Breaking Rule:**
    * If multiple processes are eligible: Select the one with the **earlier arrival time**.
    * If arrival times are equal: Select the one with the **lower index**.
4.  **Round-Robin Logic:**
    * Mimics FCFS behavior.
    * Preempted processes move to the tail of the ready queue.
    * **Crucial:** If a process is preempted at the same time a new process arrives, the **new arrival enters the queue first**, followed by the preempted process.

## Input File Format
The input file contains integers only (no fractions).

* **Line 1:** Number of test cases.
* **Line 2:** Start of first test case.
* **Test Case Header:** `[Number of Processes X] [Algorithm]`
    * *Note: If Algorithm is `RR`, a Quantum `Q` follows the code (e.g., `4 RR 25`).*
* **Next X Lines (Process Data):** `[Arrival Time] [Burst Time] [Nice Level]`
    * *Note: Arrival time is $\ge 0$. Burst time $> 0$. Nice level is -20 to +20.*

### Example Input
```text
2
4 SRTF
0 50 2
40 2 3
20 3 1
30 55 1
2 FCFS
100 10 1
10 70 1
