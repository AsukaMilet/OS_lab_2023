/**
 * @file scheduler.c
 * @author 计21-1 戴杰 20101050226
 * @brief 允许模拟测试三种不同的调度策略：FIFO, RR, SJF
 * @version 0.1
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>

#define PROJECT_NAME "OS_lab"

int main(int argc, char **argv) {
    if(argc != 1) {
        printf("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    printf("This is project %s.\n", PROJECT_NAME);
    return 0;
}
