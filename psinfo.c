#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

// Función para leer el contenido de un archivo
void read_proc_file(const char *pid, const char *file, char *buffer) {
    char path[BUFFER_SIZE];
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%s/%s", pid, file);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    fgets(buffer, BUFFER_SIZE, fp);
    fclose(fp);
}

// Función principal
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return 1;
    }

    char *pid = argv[1];
    char buffer[BUFFER_SIZE];
    char name[BUFFER_SIZE], state[BUFFER_SIZE];
    int total_memory, vmexe, vmdata, vmstk, voluntary, nonvoluntary;

    // Obtener el nombre y el estado del proceso desde /proc/[pid]/status
    read_proc_file(pid, "status", buffer);
    sscanf(buffer, "Name:\t%s", name);
    sscanf(buffer, "State:\t%s", state);

    // Obtener la memoria total, TEXT, DATA y STACK desde /proc/[pid]/smaps
    read_proc_file(pid, "smaps", buffer);
    sscanf(buffer, "VmSize:\t%d kB", &total_memory);
    sscanf(buffer, "VmExe:\t%d kB", &vmexe);
    sscanf(buffer, "VmData:\t%d kB", &vmdata);
    sscanf(buffer, "VmStk:\t%d kB", &vmstk);

    // Obtener el número de cambios de contexto voluntarios y no voluntarios
    read_proc_file(pid, "stat", buffer);
    sscanf(buffer, "%*d (%*s) %*c %*d %*d %*d %*d %*d %*d %*d %d %d",
           &voluntary, &nonvoluntary);

    // Mostrar la información del proceso
    printf("Nombre del proceso: %s\n", name);
    printf("Estado: %s\n", state);
    printf("Tamaño total de la imagen de memoria: %d KB\n", total_memory);
    printf("Tamaño de la memoria TEXT: %d KB\n", vmexe);
    printf("Tamaño de la memoria DATA: %d KB\n", vmdata);
    printf("Tamaño de la memoria STACK: %d KB\n", vmstk);
    printf("Número de cambios de contexto (voluntarios - no voluntarios): %d - %d\n",
           voluntary, nonvoluntary);

    return 0;
}
