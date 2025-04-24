#include <stdio.h> 
#ifndef PROCESS_H
#define PROCESS_H

#define MAX_LINE 256
#define MAX_PIDS 100

typedef struct {
    char pid[10];
    char name[MAX_LINE];
    char state[MAX_LINE];
    char vmSize[MAX_LINE];
    char vmExe[MAX_LINE];
    char vmData[MAX_LINE];
    char vmStk[MAX_LINE];
    char ctxtVol[MAX_LINE];
    char ctxtNoVol[MAX_LINE];
    int valid;
} Process;

void collect_information(Process *p);
void print_information(FILE *output, const Process *p);

#endif
