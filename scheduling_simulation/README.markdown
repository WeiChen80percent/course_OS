# CPU Scheduling Algorithms Simulation

This project implements four CPU scheduling algorithms in C++ to simulate process scheduling in an operating system. Each algorithm is implemented in a separate file, and the simulation generates random processes with attributes such as arrival time, burst time, and priority. The output includes a Gantt chart, average turnaround time, average waiting time, and the number of context switches.

## Algorithms Implemented

1. **Shortest Job First (SJF)** (`SJF.cpp`)
   - A non-preemptive scheduling algorithm that selects the process with the shortest burst time among the processes that have arrived.
   - Processes are sorted based on arrival time and burst time at specific intervals (time = 5 and 10).
   - Suitable for minimizing waiting time but may lead to starvation for long processes.

2. **Round Robin with Non-Preemptive Priority** (`RR_nonpreemptivepriority.cpp`)
   - Combines Round Robin scheduling with priority-based execution.
   - Processes are grouped by priority (1 to 3), and each priority level is processed in a round-robin manner with a time quantum of 5 units.
   - Higher-priority processes are executed first, and within each priority level, processes are scheduled in a round-robin fashion.

3. **Multilevel Feedback Queue** (`Multilevel_feedback_queue.cpp`)
   - Uses multiple queues with different time quanta to prioritize processes dynamically.
   - Processes start in the highest-priority queue (level 1) with a time quantum of 5 units, move to level 2 (quantum of 10 units) if not completed, and finally to level 3, where they are executed until completion.
   - Adjusts process priority based on execution history, preventing starvation.

4. **Customized (Lottery Scheduling)** (`customized.cpp`)
   - Implements a lottery scheduling algorithm where processes are assigned lottery tickets based on their priority.
   - Priorities (1 to 3) map to ticket counts (15, 10, 5 respectively). A random ticket is drawn to select the next process to execute.
   - **Advantage:**
   
      - Fairness Across Priorities:
      Processes are assigned lottery tickets based on their priority (15, 10, or 5 tickets for priorities 1, 2, or 3, respectively). Higher-priority processes have more tickets, increasing their chances of being selected, but lower-priority processes still have a non-zero chance. This ensures probabilistic fairness, preventing starvation of low-priority processes.
      - Prevention of Starvation:
      Unlike SJF or non-preemptive priority scheduling, where low-priority or long-running processes may be indefinitely delayed, lottery scheduling guarantees that every process has a chance to execute as long as it has at least one ticket.

## Common Features

- **Process Generation**:
  - Each simulation generates 10 processes with random attributes:
    - **Identification**: 1 to 10.
    - **Arrival Time**: 0, 5, or 10 (selected randomly).
    - **Burst Time**: Randomly between 6 and 28 units.
    - **Priority**: Randomly between 1 and 3.
  - The random seed is initialized using the system time for varied results.

- **Output**:
  - **Process Details**: Displays the ID, arrival time, burst time, and (where applicable) priority of each process.
  - **Gantt Chart**: Visual representation of process execution over time, with idle periods marked.
  - **Performance Metrics**:
    - **Average Turnaround Time**: Average time from process arrival to completion.
    - **Average Waiting Time**: Average time processes spend waiting in the ready queue.
    - **Total Context Switches**: Number of times the CPU switches between processes or idle states.

## How to Run

1. **Prerequisites**:
   - A C++ compiler (e.g., g++).
   - Standard C++ libraries (included in the code).

2. **Compilation**:
   Compile each file separately using a C++ compiler. For example:
   ```bash
   g++ SJF.cpp -o sjf
   g++ RR_nonpreemptivepriority.cpp -o rr_priority
   g++ Multilevel_feedback_queue.cpp -o multilevel
   g++ customized.cpp -o customized
   ```

3. **Execution**:
   Run the compiled programs:
   ```bash
   ./sjf
   ./rr_priority
   ./multilevel
   ./customized
   ```

4. **Output**:
   Each program will:
   - Print the generated processes.
   - Display the Gantt chart.
   - Show the average turnaround time, average waiting time, and total context switches.

## Notes

- The simulations use random values, so results will vary between runs.
- The algorithms assume non-preemptive execution (except where specified) and do not account for I/O operations or interrupts.
- The code is designed for educational purposes to demonstrate the behavior and performance of different scheduling algorithms.
