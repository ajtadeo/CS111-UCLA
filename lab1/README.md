# Pipe Up

This program implements the `|` shell command to pipe input and output from one shell commands into another.

## Building

1. `make` generates the `pipe` executable

## Running
 
Usage: `./pipe [cmd1] [cmd2] ... [cmdn]`

This program requires at least one argument, a valid shell command.
An example run of the program is as follows:
```
./pipe ls cat wc
    7   7   63
```
An invalid command would issue the following response:
```
./pipe badcmd 
Error 2: Command execution failed
Error 2: Child proceess exited with invalid status
```

## Cleaning up

1. `make clean` cleans up all binary files and removes the `pipe` executable