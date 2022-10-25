# You Spin Me Round Robin

This program simulates Round Robin Scheduling given a text file of processes, their arrival time, and burst time.

## Building

1. `make` to compile the executable `rr` and associated binaries.

## Running

Usage: 

`./rr [processes text file] [quantum length]`

In the processes text file, the first line is the number of total processes to be run. Each subsequent line describes the process as follows: `[pid], [arrival time], [burst time]`. The file also ends with a newline.

Example:

processes.txt
```
4
1, 0, 7
2, 2, 4
3, 4, 1
4, 5, 4

```
```
./rr processes.txt 3
Average waiting time: 7.00
Average response time: 2.75
```

## Cleaning up

1. `make clean` to remove the executable and other associated binaries.
