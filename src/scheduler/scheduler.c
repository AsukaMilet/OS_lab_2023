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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
}

void fifo_statistics(Job *joblist, int jobnum) {
  int thetime = 0;
  int turnaroundSum = 0, waitSum = 0, responseSum = 0;
  int response = 0, wait = 0, turnaround = 0;  // Current process response, wait, turnaround time

  for (int i = 0; i < jobnum; i++) {
    printf("[time %6d ] Run process %d for %d secs (Finished at %d)\n", thetime, joblist[i].pid, joblist[i].runtime,
          thetime + joblist[i].runtime);
    thetime += joblist[i].runtime;
  }

  thetime = 0;
  printf("\nFinal Statistics:\n");
  for (int i = 0; i < jobnum; i++) {
    response = thetime;
    turnaround = thetime + joblist[i].runtime;
    wait = thetime;
    printf("Process %3d -- Response: %6d, Wait: %6d, Turnaround: %6d\n", joblist[i].pid, response, wait, turnaround);
    responseSum += response;
    turnaroundSum += turnaround;
    waitSum += wait;
    thetime += joblist[i].runtime;
  }
  printf("Average Response: %6d, Average Wait: %6d, Average Turnaround: %6d\n", responseSum / jobnum, waitSum / jobnum,
        turnaroundSum / jobnum);
}
