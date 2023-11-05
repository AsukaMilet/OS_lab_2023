#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "scheduler.h"

int main(int argc, const char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: scheduler --the number of Jobs --random seed --policy\n");
    exit(EXIT_FAILURE);
  }
  int jobnum = 0;
  int seed = 0;
  const char *policy_name = NULL;

  // Parse the command line
  struct argparse_option options[] = {OPT_INTEGER('j', "jobs", &jobnum, "number of jobs", NULL, 0, 0),
                                      OPT_INTEGER('s', "seed", &seed, "random seed", NULL, 0, 0),
                                      OPT_STRING('p', "policy", &policy_name, "policy", NULL, 0, 0), OPT_END()};

  // Convert arguments into number of jobs, random seed, and policy
  struct argparse parse;
  argparse_init(&parse, options, NULL, 0);
  argc = argparse_parse(&parse, argc, argv);

  // Set the random seed from argument
  srand(seed);

  Job *joblist = init_joblist(jobnum);
  Policy policy = get_policy(policy_name);
  switch (policy) {
    case FIFO: {
      char policy[] = "FIFO";
      printf("Current Policy: %s\n", policy);
      print_joblist(joblist, jobnum);
      printf("\n\n");
      fifo_statistics(joblist, jobnum);
    } break;
    case SJF: {
      char policy[] = "SJF";
      printf("Current Policy: %s\n", policy);
      print_joblist(joblist, jobnum);
    } break;
    case RR: {
      char policy[] = "RR";
      printf("Current Policy: %s\n", policy);
      print_joblist(joblist, jobnum);
    } break;
  }
  free(joblist);  // Free the memory to avoid memory leak
  return 0;
}
