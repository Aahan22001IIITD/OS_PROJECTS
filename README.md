# Process Scheduler

This project implements a fundamental C-based process scheduler that emulates CPU time-sharing. Operating on a priority-based policy, it facilitates managing multiple processes across a limited core count. The system allows priority assignment on a scale of 1 to 4, providing statistical insights into the impact of priority levels on job scheduling and execution.

## Overview

For detailed information about the project, refer to the attached PDF document.

## Usage

### Running the Scheduler

To run the scheduler, follow these steps:

1. Save the code as `os.c`.
2. Compile the code to generate the executable using the following commands in WSL:
```make clean```
```make```

3. Start the scheduler by running:
```./os $NCPU $TIMESLICE```

Replace `$NCPU` with the number of CPUs and `$TIMESLICE` with the time slice.

### Interacting with the Scheduler

Once the scheduler is running, you can interact with it using the integrated shell. Here are some commands:

- **Submit Process**: Use the `submit ./pi` command to add processes (e.g., `p1`, `p2`, etc.) to the queue with a default priority of 1.

### Running Processes

The scheduler will execute processes in the queue approximately every 30 seconds and display the output on the STDOUT.

## Contributing

Contributions ~ ```@kartikeya22241```
