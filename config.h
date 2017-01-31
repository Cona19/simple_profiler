#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEF_INTERVAL 1
#define DEF_ITER 20
#define DEF_WARMUP 30
#define DEF_OUTPUT "cpu_result.txt"


typedef struct _BenchmarkOptions{
    char *cmd;
    int interval_sec;
    int iter;
    int warmup_sec;
    char *output;
} BenchmarkOptions;

#define NUM_OP_ARG 5

void releaseBenchmarkOptions(BenchmarkOptions *ops);
int initBenchmarkOptions(int argc, char *argv[], BenchmarkOptions *ops);
void printBenchmarkOptions(BenchmarkOptions *ops);

#define BUF_SIZE 1024

#endif
