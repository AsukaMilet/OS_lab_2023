#ifndef SCHDULER_H
#define SCHDULER_H

#define MAXNUM 10

typedef enum Policy { FIFO, SJF, RR, MLFQ } Policy;
typedef struct Job {
  unsigned int pid;      // Simulate the process id in the system
  unsigned int runtime;  // Total runtime of the process
} Job;

#define i_type JobDeque
#define i_key struct Job *
#define i_less(a, b) a->runtime < b->runtime
#include "stc/cdeq.h"

Job *init_joblist(int jobnum);

void print_joblist(Job *p, int jobnum);

enum Policy get_policy(const char *policy);

void fifo_statistics(Job *joblist, int jobnum);

void sjf_sort(Job *joblist, int jobnum);

void rr_statistics(Job *joblist, int jobnum, int time_slice);

void mlfq_statistics(Job *joblist, int jobnum, int numQueues, int time_slice, int boost);

#endif
