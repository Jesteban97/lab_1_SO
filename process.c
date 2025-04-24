#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "process.h"

int file_exist(const char *path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void get_value(const char *file, const char *key, char *result)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        strcpy(result, "No disponible");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, fp))
    {
        if (strncmp(line, key, strlen(key)) == 0)
        {
            sscanf(line + strlen(key), "%s", result);
            break;
        }
    }

    fclose(fp);
}

void collect_information(Process *p)
{
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/status", p->pid);

    if (!file_exist(path))
    {
        fprintf(stderr, "Error: el proceso con PID %s no existe.\n", p->pid);
        p->valid = 0;
        return;
    }

    p->valid = 1;
    get_value(path, "Name:", p->name);
    get_value(path, "State:", p->state);
    get_value(path, "VmSize:", p->vmSize);
    get_value(path, "VmExe:", p->vmExe);
    get_value(path, "VmData:", p->vmData);
    get_value(path, "VmStk:", p->vmStk);
    get_value(path, "voluntary_ctxt_switches:", p->ctxtVol);
    get_value(path, "nonvoluntary_ctxt_switches:", p->ctxtNoVol);
}

void print_information(FILE *output, const Process *p)
{
    if (!p->valid)
        return;

    fprintf(output, "Pid: %s\n", p->pid);
    fprintf(output, "name del process: %s\n", p->name);
    fprintf(output, "Estado: %s\n", p->state);
    fprintf(output, "Tamaño total de la imagen de memoria: %s KB\n", p->vmSize);
    fprintf(output, "Tamaño de la memoria TEXT: %s KB\n", p->vmExe);
    fprintf(output, "Tamaño de la memoria DATA: %s KB\n", p->vmData);
    fprintf(output, "Tamaño de la memoria STACK: %s KB\n", p->vmStk);
    fprintf(output, "Número de cambios de contexto (voluntarios - no voluntarios): %s - %s\n\n", p->ctxtVol, p->ctxtNoVol);
}
