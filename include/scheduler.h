#ifndef SCHDULER_H
#define SCHDULER_H

#define MAXNUM 10

typedef enum Policy{ 
    FIFO, 
    SJF, 
    RR
} Policy;

typedef struct Job {
    unsigned int pid;   // Simulate the process id in the system
    unsigned int runtime;   // Total runtime of the process
} Job;

Job* init_joblist(int jobnum);

void print_joblist(Job* p, int jobnum);

enum Policy get_policy(const char* policy);

#endif
