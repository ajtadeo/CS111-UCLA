# A Kernel Seedling

This program counts the total number of running programs in the system and outputs the value in `/proc/count`.

## Building

1. `make clean` to create a fresh build.
2. `make` to build the proc_count.ko file and other objects.
3. `sudo insmod proc_count.ko` to install the kernel module
4. `sudo lsmod | grep proc_count` should return the module name and other information to verify installation

## Running

After installing, we can view the total number of running processes by running `cat /proc/count`

## Cleaning Up

1. `sudo rmmod proc_count` to remove the kernel module as well as /proc/count
2. `sudo lsmod | grep proc_count` should return nothing to verify cleanup

## Testing

Module was tested on kernel release version 5.14.8-arch1-1. Using the provided test_lab0.py file, the module was tested for valid behavior for insmod calls, rmmod calls, and to ensure the creation and valid reading of /proc/count. Additionally, the number of alive processes outputted from `cat /proc/count` was compared to the output of `ps -A | wc -l` which outputs all current processes in a table and counts the number of lines. This value is not exactly equal to the output of `cat /proc/count`. It is slightly higher due to counting the line containing column titles, as well as the processes running `ps` and `wc` for the currently executing shell command. Since the numbers for the output of these two commands are similar, we satisfy our sanity check.