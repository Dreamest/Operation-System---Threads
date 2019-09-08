# Operation-System---Threads

A program that checks Sudoku if Sudoku solutions are valid.

The program was written and compiled in Linux.

The master thread reads the Sudoku from file and store it. 

v1 - 27 threads are created, each thread will be responsible for one line/column or box.
     Every thread saves its result to a unique location.
     Once all results have arrived, the master thread will inform if the solution is valid or not.
     
v2 - The master thread creates a task pool that's protected with a mutex, from which the other threads each takes a task and completes it before taking another.
     Once all tasks are completed, using condition variable we're informing the master thread that results can be printed.
