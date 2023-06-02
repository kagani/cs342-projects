# CS342: Operating Systems Projects
Projects for Bilkent University's OS course taught in the spring of 2023.

## Project 1
In this project, you will develop an application that will find
the K most frequently occurring words, i.e., top-K words, in a given input
data set (K is an positive integer). In part A, the application will use multiple
child processes to process the data set. In part B, it will use multiple threads.
Hence, you will develop two programs, that will essentially do the same
thing. The data set will contain N input files. Child processes will use shared
memory to pass information to the parent. The shared memory segment will
be created by the parent. Each child process will access and use a portion of
the shared memory.

## Project 2
In this project you will implement a program that will simulate multiprocessor
scheduling. The program will simulate two approaches for multiprocessor
scheduling: single-queue approach, and multi-queue approach.
In single-queue approach, we have a single common ready queue
(runqueue) that is used by all processors. In multi-queue approach, each
processor has its own ready queue. Your program will simulate the following
scheduling algorithms: FCFS (first-come first-served), SJF (shortest job first -
nonpreemptive), and RR(Q) (round robin with time quantum Q).

## Project 3
In this project you will develop a resource manager library, that will
manage a set of resources for a multithreaded program (application). A
program that is linked with the library may create multiple threads and each
thread may request resources from the library, use them, and release them, as
many times as it wishes, during its lifetime. 

## Project 4
In this project you will implement a Linux program, in C, that will give
memory related information for a process and for the physical memory of the
system. The program will be called pvm. It will take the memory related
information from the /proc virtual file system, which is an interface to the
kernel for user-space programs (applications). In the /proc directory, there
are a lot of files that can be read by a user program or viewed by a user to get
various information about the system state and processes. The related
information is retrieved from various kernel data structures and variables
(from kernel space). Hence, the content of these files are derived from kernel
memory, not from disk. In /proc directory, there is a sub-directory for each
process, to get process specific information maintained by the kernel.
