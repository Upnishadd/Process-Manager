# Process-Manager


A process manager capable of allocating memory to processes and scheduling them for execution. This project has two phases. In the first phase, a scheduling algorithm will allocate the CPU to processes, assuming that memory requirements are always satisfied. In the second phase, a memory allocation algorithm will be used to allocate memory to processes, before the scheduling occurs. Both the memory allocation and the process scheduling are simulated. There process manager is also assigned the task of controlling the execution of real processes during the process scheduling. Process scheduling decisions will be based on a scheduling algorithm assuming a single CPU (i.e., only one process can be running at a time).


How to run:

./allocate -f {INPUTFILE} -s {SJF/RR} -m {infinite/best-fit} -q {QUANTUM TIME}
