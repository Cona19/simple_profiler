#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#define BUF_SIZE 1024

/* Killobyte */
typedef struct _IOStat{
    double tot_read;
    double act_read;
    double tot_write;
    double act_write;
    inline struct _IOStat operator + (struct _IOStat b){
        return {tot_read + b.tot_read, act_read + b.act_read, tot_write + b.tot_write, act_write + b.act_write};
    }
    inline struct _IOStat operator / (int b){
        return {tot_read / b, act_read / b, tot_write / b, act_write / b};
    }
} IOStat;

double getCpuUsage(char *cmd){
    char buf[BUF_SIZE];
    FILE *fp;
    double tmp = 0.0;
    double cpu_util = 0.0;
    sprintf(buf, "/bin/ps -C %s -o %%cpu", cmd);
    fp = popen(buf, "r");
    if (fp == NULL){
        printf("failed to run ps\n");
        return 0.0;
    }
    fgets(buf, sizeof(buf), fp);
    while(!feof(fp)){
        fscanf(fp, "%lf\n", &tmp);
        cpu_util += tmp;
    }
    fclose(fp);

    return cpu_util;
}

void getIOUsage(IOStat* prof){
    char buf[BUF_SIZE];
    char *p;
    FILE *fp;
    sprintf(buf, "/usr/sbin/iotop -b -o -k -q -n 1");
    fp = popen(buf, "r");
    if (fp == NULL){
        printf("failed to run iotop\n");
    }

    fgets(buf, sizeof(buf), fp);
    p = strchr(buf, ':');
    prof->tot_read = strtod(++p, NULL);
    p = strchr(p, ':');
    prof->tot_write = strtod(++p, NULL);

    fgets(buf, sizeof(buf), fp);
    p = strchr(buf, ':');
    prof->act_read = strtod(++p, NULL);
    p = strchr(p, ':');
    prof->act_write = strtod(++p, NULL);
    fclose(fp);
}

int main(int argc, char *argv[]){
    int i;
    double tot_cpu = 0;
    double max_cpu = 0;
    double min_cpu = 10000000;
    double cur_cpu;
    IOStat tot_io;
    IOStat max_io = {0,};
    IOStat cur_io;
    BenchmarkOptions bop;
    if (initBenchmarkOptions(argc, argv, &bop)){
        printf("option init failed\n");
        return 0;
    }
    sleep(bop.warmup_sec);
    for (i = 0; i < bop.iter ; i++){
        cur_cpu = getCpuUsage(bop.cmd);
        getIOUsage(&cur_io);
        tot_cpu += cur_cpu;
        if (max_cpu < cur_cpu) max_cpu = cur_cpu;
        if (min_cpu > cur_cpu) min_cpu = cur_cpu;
        sleep(bop.interval_sec);
        tot_io = tot_io + cur_io;

        if (max_io.tot_read < cur_io.tot_read)
            max_io.tot_read = cur_io.tot_read;
        if (max_io.act_read < cur_io.act_read)
            max_io.act_read = cur_io.act_read;
        if (max_io.tot_write < cur_io.tot_write)
            max_io.tot_write = cur_io.tot_write;
        if (max_io.act_write < cur_io.act_write)
            max_io.act_write = cur_io.act_write;
    }
    tot_io = tot_io / bop.iter;

    printf("AVG_CPU\t\t%lf\n", tot_cpu / bop.iter);
    printf("MAX_CPU\t\t%lf\n", max_cpu);
    printf("MIN_CPU\t\t%lf\n", min_cpu);
    printf("AVG_TOT_READ\t%lf\n", tot_io.tot_read);
    printf("AVG_ACT_READ\t%lf\n", tot_io.act_read);
    printf("AVG_TOT_WRITE\t%lf\n", tot_io.tot_write);
    printf("AVG_ACT_WRITE\t%lf\n", tot_io.act_write);
    printf("MAX_TOT_READ\t%lf\n", max_io.tot_read);
    printf("MAX_ACT_READ\t%lf\n", max_io.act_read);
    printf("MAX_TOT_WRITE\t%lf\n", max_io.tot_write);
    printf("MAX_ACT_WRITE\t%lf\n", max_io.act_write);

    releaseBenchmarkOptions(&bop);
    return 0;
}
