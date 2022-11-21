# Hash Hash Hash

This program simulates and outputs the time metrics for three versions of hash table program: one single-threaded, one multi-threaded with one mutex, and one multi-threaded with multiple mutexes.

## Building

1. `make` to build the executable

## Running
Usage

`./hash-table-tester -t [number of threads] -s [size per thread]`

Example
```
./hash-table-tester -t 4 50000
Generation: 16,429 usec
Hash table base: 76,211 usec
  - 0 missing
Hash table v1: 249,440 usec
  - 0 missing
Hash table v2: 21,517 usec
  - 0 missing
```

## First Implementation

Version 1 implements one mutex defined in `struct hash_table_v1` and intialized in `hash_table_v1_create()`. The mutex locks at the beginning of `hash_table_v1_add_entry()` and unlocks at the end of the function. This implementation is correct because the mutex wraps around the entire critical section and prevents any other thread from accessing it during any read or write actions for all data in the hash table.

### Performance

```
Hash table v1: 249,440 usec
  - 0 missing
```

Version 1 results in a slow down of the base case. In using a single mutex, the program performs worse than the single-threaded version because only one thread can do work at a time. All other threads must wait for the thread holding the mutex to release it. Regardless of how many threads are used, it will not increase speed. Additionally, version 1 has more computational work than the single-threaded version accomplish due to the initialization, locking/unlocking, and destruction of the hash table mutex. Overall, version 1 results in a 3.27x slow down on a machine with 4 cores.

## Second Implementation

Version 2 implements a mutex for each `struct hash_table_entry` which are initialized in `hash_table_v2_create()`. This accomplishes a finer locking granularity than version 1. The logic of this version is due to the structure of a hash table. In order to access data in the hash table, a thread must first compute the entry for a given key. Multiple keys can lead to the same entry, which will result in a race condition. Version 2 avoids this race condition by giving each entry a mutex. If one thread is holding the entry mutex, all other threads wanting to access that same entry must wait for the mutex to be released. 

### Performance

```
Hash table v2: 21,517 usec
  - 0 missing
```

Version 2 results in a speed up of the base case because of the location of mutexes in the code. Because the mutexes are located at a higher level implementation-wise than version 1, they are locked less frequently when threads may or may not to collide at the hash table entries. This increases parallelism because non-colliding threads are able to continue working, and only the ones trying to access the same entry will have a delay. Overall, version 2 results in an 3.54x speedup from the base case when running on a machine with 4 cores.

## Cleaning up

1. `make clean` to clean up the executable
