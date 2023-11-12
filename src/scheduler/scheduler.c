/**
 * @file scheduler.c
 * @author 计21-1 戴杰 20101050226
 * @brief 允许模拟测试三种不同的调度策略：FIFO, RR, SJF
 * @version 0.1
 * @date 2023-10-30(create)
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "scheduler.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define i_tag job
#define i_key struct Job
#define i_less(a, b) a->runtime < b->runtime
#include "stc/cdeq.h"

/**
 * @brief Initialize the list of jobs
 *
 * @param jobnum
 * @return Job*
 */
Job *init_joblist(int jobnum) {
  struct Job *p = (struct Job *)malloc(jobnum * sizeof(struct Job));
  for (int i = 0; i < jobnum; i++) {
    // Initialize a job with random runtime, runtime between 0 and 20000
    Job job = {.pid = i, .runtime = rand() % 20000};
    p[i] = job;
  }
  return p;
}

void print_joblist(Job *p, int jobnum) {
  for (int i = 0; i < jobnum; i++) {
    printf("Current process pid: %d, runtime: %d\n", p[i].pid, p[i].runtime);
  }
}

// Get the scheduling policy
Policy get_policy(const char *policy) {
  if (strcmp(policy, "FIFO") == 0) {
    return FIFO;
  }
  if (strcmp(policy, "SJF") == 0) {
    return SJF;
  }
  if (strcmp(policy, "RR") == 0) {
    return RR;
  }
  if(strcmp(policy, "MLFQ") == 0) {
    return MLFQ;
  }
  else {
    fprintf(stderr, "Invalid policy: %s\n", policy);
    exit(EXIT_FAILURE);
  }
}

void fifo_statistics(Job *joblist, int jobnum) {
  int currtime = 0;  // Current time
  int turnaroundSum = 0, responseSum = 0;
  int response = 0, turnaround = 0;  // Current process response, wait, turnaround time

  for (int i = 0; i < jobnum; i++) {
    printf("[time %6d ] Run process %d for %d secs (Finished at %d)\n", currtime, joblist[i].pid, joblist[i].runtime,
          currtime + joblist[i].runtime);
    currtime += joblist[i].runtime;
  }

  currtime = 0;
  printf("\nFinal Statistics:\n");
  for (int i = 0; i < jobnum; i++) {
    response = currtime;
    turnaround = currtime + joblist[i].runtime;
    printf("Process %3d -- Response: %6d, Turnaround: %6d\n", joblist[i].pid, response, turnaround);
    responseSum += response;
    turnaroundSum += turnaround;
    currtime += joblist[i].runtime;
  }
  printf("\nAverage Response: %6d, Average Turnaround: %6d\n", responseSum / jobnum, turnaroundSum / jobnum);
}

void sjf_sort(Job *joblist, int jobnum) {
  for (int i = 1; i < jobnum; i++) {
    Job temp = joblist[i];
    int j = i - 1;

    while (j >= 0 && joblist[j].runtime > temp.runtime) {
      joblist[j + 1] = joblist[j];
      j--;
    }

    joblist[j + 1] = temp;
  }
}

void rr_statistics(Job *joblist, int jobnum, int time_slice) {
  // Initialize the turnaround time of each process
  int *turnaround_time = (int *)malloc(sizeof(int) * jobnum);
  memset(turnaround_time, 0, sizeof(int) * jobnum);

  // Initialize the response time of each process
  int *response_time = (int *)malloc(sizeof(int) * jobnum);
  memset(response_time, -1, sizeof(int) * jobnum);

  int currtime = 0;    // Current time of execution
  int round_time = 0;  // Current round time
  
  //  Initialize the queue of processes
  cdeq_job jobs = cdeq_job_init();
  for (int i = 0; i < jobnum; i++) {
    cdeq_job_push(&jobs, joblist[i]);
  }

  while (!cdeq_job_empty(&jobs)) {
    Job *job = cdeq_job_front(&jobs);  // Get the first process in the queue
    cdeq_job_pop_front(&jobs);         // Pop the process from the queue

    int pid = job->pid;
    int runtime = job->runtime;
    if (response_time[pid] == -1) {
      response_time[pid] = currtime;
    }
    // If process can't be finished in the current time slice
    if (runtime > time_slice) {
      job->runtime = job->runtime - time_slice;
      round_time = time_slice;
      printf("[time %6d ] Run process %d for %d secs\n", currtime, pid, round_time);
      // Push the process to the tail of the queue
      cdeq_job_push_back(&jobs, *job);
    }
    // Process can be finished in the current time slice
    else {
      round_time = runtime;
      turnaround_time[pid] = currtime + round_time;
      printf("[time %6d ] Run process %d for %d secs (Finished at %d)\n", currtime, pid, round_time,
            currtime + round_time);
    }
    currtime += round_time;
  }

  printf("\nFinal Statistics:\n");
  int turnaroundSum = 0, responseSum = 0;
  for (int i = 0; i < jobnum; i++) {
    printf("Process %3d -- Response: %6d, Turnaround: %6d\n", joblist[i].pid, response_time[i], turnaround_time[i]);
    responseSum += response_time[i];
    turnaroundSum += turnaround_time[i];
  }
  printf("\nAverage Response: %6d, Average Turnaround: %6d\n", responseSum / jobnum, turnaroundSum / jobnum);

  // Free the memory to avoid memory leak
  cdeq_job_drop(&jobs);
  free(turnaround_time);
  free(response_time);
}
