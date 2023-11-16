/**
 * @file scheduler.c
 * @author 计21-1 戴杰 20101050226
 * @brief 允许模拟测试四种不同的调度策略：FIFO, RR, SJF, MLFQ
 * 为了简单起见，在MLFQ策略中每个队列的时间片增长500, 不考虑I/O和yield
 * @version 0.1
 * @date 2023-10-30(create)
 * @copyright Copyright (c) 2023
 *
 */

#include "scheduler.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define i_type VecDeque
#define i_keyclass JobDeque
#include "stc/cvec.h"

int find_queue(VecDeque *queues);

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
  if (strcmp(policy, "MLFQ") == 0) {
    return MLFQ;
  } else {
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
  JobDeque jobs = JobDeque_init();
  for (int i = 0; i < jobnum; i++) {
    JobDeque_push(&jobs, joblist[i]);
  }

  while (!JobDeque_empty(&jobs)) {
    Job *job = JobDeque_front(&jobs);  // Get the first process in the queue
    JobDeque_pop_front(&jobs);         // Pop the process from the queue

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
      JobDeque_push_back(&jobs, *job);
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
  JobDeque_drop(&jobs);
  free(turnaround_time);
  free(response_time);
}

void mlfq_statistics(Job *joblist, int jobnum, int numQueues, int time_slice, int boost) {
  srand((unsigned int)time(NULL));
  int finished_jobs = 0;
  int round = 0;
  int index = -1;
  int currtime = 0;
  JobDeque *queue = NULL;
  // Initialize the array of queues
  int *time_slices = (int *)malloc(sizeof(int) * numQueues);
  time_slices[0] = time_slice;

  // Initialize the time slice size for each queue
  for (int i = 1; i < numQueues; i++) {
    time_slices[i] = time_slices[i - 1] + 500;
  }

  // Initialize the response time of each process
  int *response_times = (int *)malloc(sizeof(int) * jobnum);
  memset(response_times, -1, sizeof(int) * jobnum);

  // Initialize the turnaround time of each process
  int *turnaround_times = (int *)malloc(sizeof(int) * jobnum);
  memset(turnaround_times, 0, sizeof(int) * jobnum);

  // Initialize the multilevel queues
  VecDeque queues = VecDeque_init();
  for (int i = 0; i < numQueues; i++) {
    // Avoid stack-use-after-scope
    queue = VecDeque_push_back(&queues, JobDeque_init());
  }

  // Add all jobs to the highest priority queue
  queue = VecDeque_at_mut(&queues, 0);
  for (int i = 0; i < jobnum; i++) {
    JobDeque_push(queue, joblist[i]);
  }

  while (finished_jobs < jobnum) {
    // remove all jobs from queues (except high queue) and put them in high queue
    if (boost > 0 && round != 0) {
      if (round % boost == 0) {
        // Ignore the boost time
        printf("[ Round %d ] BOOST (every %d)\n", round, boost);
        JobDeque *high_queue = VecDeque_at_mut(&queues, 0);
        for (int i = 1; i < numQueues; i++) {
          // Get the rest of queues
          JobDeque *temp = VecDeque_at_mut(&queues, i);
          while (!JobDeque_empty(temp)) {
            // Add the job to the highest priority queue
            JobDeque_push(high_queue, *JobDeque_front(temp));
            JobDeque_pop_front(temp);
          }
        }
      }
    }

    // Find the highest non-empty queue
    index = find_queue(&queues);
    if (index == -1) {
      // Ignore CPU idle time
      printf("[Round %d ] No jobs can be executed\n", round);
      round += 1;
      continue;
    }
    // Get the first process of the queue
    JobDeque *curr_queue = VecDeque_at_mut(&queues, index);
    Job *job = JobDeque_front(curr_queue);
    JobDeque_pop_front(curr_queue);

    if (response_times[job->pid] == -1) {
      response_times[job->pid] = currtime;
    }

    // Current process can't be finished in the current time slice
    if (job->runtime > (unsigned int)time_slices[index]) {
      int is_yield = rand() % 10;  // 10% chance of yielding to simulate I/O
      if (is_yield == 0) {
        // If current process yield. To make life easier, when it runs half of the current time slice, it will yield
        currtime += time_slices[index] / 2;
        job->runtime -= time_slices[index] / 2;
        printf("[Round %d ] Run process %d at priority %d for %d secs\n", round, job->pid, index,
              time_slices[index] / 2);
        printf("process %d yields\n", job->pid);
        JobDeque_push(VecDeque_at_mut(&queues, index), *job);
      } else {
        // Current process doesn't yield
        printf("[Round %d ] Run process %d at priority %d for %d secs\n", round, job->pid, index, time_slices[index]);
        // Current process is not in the lowest priority queue
        if (index != numQueues - 1) {
          currtime += time_slices[index];
          job->runtime -= time_slices[index];
          JobDeque_push(VecDeque_at_mut(&queues, index + 1), *job);
        } else {
          // Current process is in the lowest priority queue
          currtime += time_slices[index];
          job->runtime -= time_slices[index];
          JobDeque_push(VecDeque_at_mut(&queues, index), *job);
        }
      }
    } else {
      // Current process can be finished in the current time slice
      currtime += job->runtime;
      turnaround_times[job->pid] = currtime += job->runtime;
      printf("[Round %d ] Run process %d at priority %d for %d secs (finished at %d)\n", round, job->pid, index,
            job->runtime, currtime);
      finished_jobs++;
    }
    round++;
  }

  printf("\nFinal Statistics:\n");
  int turnaroundSum = 0, responseSum = 0;
  for (int i = 0; i < jobnum; i++) {
    printf("Process %3d -- Response: %6d, Turnaround: %6d\n", joblist[i].pid, response_times[joblist[i].pid],
          turnaround_times[joblist[i].pid]);
    responseSum += response_times[joblist[i].pid];
    turnaroundSum += turnaround_times[joblist[i].pid];
  }
  printf("\nAverage Response: %6d, Average Turnaround: %6d\n", responseSum / jobnum, turnaroundSum / jobnum);

  c_drop(VecDeque, &queues);
  free(time_slices);
  free(response_times);
  free(turnaround_times);
}

/**
 * @brief Find the highest non-empty queue index
 *
 * @param queues
 * @return JobDeque*, return -1 if all queues are empty
 */
int find_queue(VecDeque *queues) {
  for (int i = 0; i < VecDeque_size(queues); i++) {
    // Check whether current queue is empty
    const JobDeque *high_queue = VecDeque_at(queues, i);
    if (!JobDeque_empty(high_queue)) {
      return i;
    }
  }
  return -1;
}
