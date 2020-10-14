This is original work of Harsh Rawat (CSLogin: harsh-rawat and netid: hrawat2) and Sidharth Gurbani (CSLogin: gurbani and netid: gurbani) for Assignment 2 (CS 537 Fall 2020).

Problem Description-
-------------------
The goal of this assignment was to get experience in writing a program that actually runs in parallel on Linux using threads (pthreads) and synchronization.
We had a problem similar to producer-comsumer where 4 threads are running and they process the data from the previous thread.

Reader => Munch1 => Munch2 => Writer

Reader reads from stdin (only if the line size is less that total buffer size).
Munch1 converts spaces of the line into *
Munch2 converts lower case letters to upper case
Writer writes the processed string to stdout

Usage
-----
Run the following command in the source folder-
make all

Then run the executable using-
prodcom < {input_file or omit this for directly using stdin}

Problem Solution-
----------------
We have divided the code into 4 modules-
1. Queue module - It implements the synchronized queue functionality
2. Statistics module - It is used to keep a track of queue statistics
3. Error module - All error handling functionality is present in this module. For our project, in case of error, we print an error message to stderr and exit with failure code.
4. Threads module - Reader, Munch1, Munch2 and Writer functionality is implemented in this module.

main
----
main method creates the shared queues and use them to create the corresponding objects of Reader, Munch1, Munch2 and Writer.
All this is accomplished using function calls to the Queue and Threads module.
main then creates the respective threads and waits for them to finish. Once these threads finish, the queue stats are printed and our program terminates.

Queue module
------------
The synchronization is achieved using semaphores (semaphore.h). We have a full semaphore initialized to 0 initially and an empty sem initialized to size.
When we enqueue a string then empty is decremented and full is incremented.
Opposite happens during dequeue.
More details can be found in queue module itself!

Statistics Module
-----------------
This module is used to keep track of queue stats. We store enqueue count, dequeue count, enqueue time and dequeue time.
The access is synchronized using semaphore.

Error Module
------------
All the error handling functionality is present in this module. For the purpose of our project, we print a message to stderr and then exit with failure code.
However, if we want to include any complex error management logic then that can be incorporated without changes to other module.

Threads module
--------------
Reader, Munch1, Munch2 and Writer functionality is implemented in this module. We can create the appropriate structs using methods of this module.
The functionality of each component is also implemented in this module.