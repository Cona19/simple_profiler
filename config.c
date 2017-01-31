#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"

const char *full_text_args[NUM_OP_ARG] = {
    "--cmd",
    "--interval_sec",
    "--iter",
    "--warmup_sec",
    "--output",
};

const char *abb_text_args[NUM_OP_ARG] = {
    "-c",
    "-i",
    "-n",
    "-w",
    "-o"
};

void parseCmd(BenchmarkOptions *ops, char *arg);
void parseInterval(BenchmarkOptions *ops, char *arg);
void parseIter(BenchmarkOptions *ops, char *arg);
void parseWarmup(BenchmarkOptions *ops, char *arg);
void parseOutput(BenchmarkOptions *ops, char *arg);

void (* const op_parsers[NUM_OP_ARG])(BenchmarkOptions*, char*) = {
    parseCmd,
    parseInterval,
    parseIter,
    parseWarmup,
    parseOutput
};

int findOptionType(char *arg){
    int i;
    for (i = 0; i < NUM_OP_ARG; i++){
        if (strcmp(arg, full_text_args[i]) == 0 || strcmp(arg, abb_text_args[i]) == 0)
            return i;
    }
    return -1;
}

void parseCmd(BenchmarkOptions *ops, char *arg){
    ops->cmd = arg;
}
void parseInterval(BenchmarkOptions *ops, char *arg){
    ops->interval_sec = atoi(arg);
}
void parseIter(BenchmarkOptions *ops, char *arg){
    ops->iter = atoi(arg);
}
void parseWarmup(BenchmarkOptions *ops, char *arg){
    ops->warmup_sec = atoi(arg);
}
void parseOutput(BenchmarkOptions *ops, char *arg){
    ops->output = arg;
}

int initDefaultBenchmarkOptions(BenchmarkOptions *ops){
    if (!ops->cmd){
        printf("argument error : cmd is not found\n");
        return -1;
    }
    if (!ops->interval_sec)
        ops->interval_sec = DEF_INTERVAL;
    if (!ops->iter)
        ops->iter = DEF_ITER;
    if (!ops->warmup_sec)
        ops->warmup_sec = DEF_WARMUP;
    if (!ops->output)
        ops->output = DEF_OUTPUT;
    return 0;
}

void releaseBenchmarkOptions(BenchmarkOptions *ops){
}

int initBenchmarkOptions(int argc, char *argv[], BenchmarkOptions *ops){
    int i;
    int type;
    memset(ops, 0, sizeof(BenchmarkOptions));
    for (i = 1; i < argc; i+=2){
        type = findOptionType(argv[i]);
        if (type == -1){
            printf("Unrecognized Command : %s\n", argv[i]);
            return -1;
        }
        if (i+1 >= argc){
            printf("Empty Arguments : %s\n", argv[i]);
            return -1;
        }
        op_parsers[type](ops, argv[i+1]);
    }
    return initDefaultBenchmarkOptions(ops);
}

void printBenchmarkOptions(BenchmarkOptions *ops){
    printf("cmd : %s\n", ops->cmd);
    printf("interval_sec : %d\n", ops->interval_sec);
    printf("iter : %d\n", ops->iter);
    printf("warmup_sec : %d\n", ops->warmup_sec);
    printf("output : %s\n", ops->output);
}
