# Hey! I'm Filing Here

This program creates an ext2 file system with one directory, one file, and one symbolic link.

## Building

1. `make`

## Running

Compiling
1. `./ext2-create`

Mounting
1. `mkdir mnt`
2. `sudo mount -o loop cs111-base.img mnt`

Example output of `ls -ain`

```
total 7
         2 drwxr-xr-x 3    0    0 1024 Nov 28 20:42 .
    942243 drwxr-xr-x 4 1000 1000 4096 Nov 28 20:58 ..
        13 lrw-r--r-- 1 1000 1000   11 Nov 28 20:42 hello -> hello-world
        12 lrw-r--r-- 1 1000 1000   12 Nov 28 20:42 hello-world
        11 drwxr-xr-x 2    0    0 1024 Nov 28 20:42 lost+found
```

## Cleaning up

Unmounting and removing mnt
1. `sudo umount mnt`
2. `rmdir mnt`

Cleaning up binary files
1. `make clean`
