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

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "scheduler.h"

/**
 * @brief Initialize the list of jobs
 * 
 * @param jobnum 
 * @return Job* 
 */
Job* init_joblist(int jobnum) {
    struct Job* p = (struct Job*)malloc(jobnum * sizeof(struct Job));
    for(int i = 0; i < jobnum; i++) {
        // Initialize a job with random runtime, runtime between 0 and 20000
        Job job = { .pid = i, .runtime = rand()%20000};
        p[i] = job;
    }
    return p;
}

void print_joblist(Job* p, int jobnum) {
    for(int i = 0; i < jobnum; i++) {
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
