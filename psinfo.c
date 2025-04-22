#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
} Proceso;

void obtener_valor(const char *archivo, const char *clave, char *resultado) {
    FILE *fp = fopen(archivo, "r");
    if (!fp) {
        perror("Error al abrir el archivo");
        strcpy(resultado, "No disponible");
        return;
    }

    char linea[MAX_LINE];
    while (fgets(linea, MAX_LINE, fp)) {
        if (strncmp(linea, clave, strlen(clave)) == 0) {
            sscanf(linea + strlen(clave), "%s", resultado);
            break;
        }
    }

    fclose(fp);
}

void recolectar_info(Proceso *p) {
    char ruta[64];
    snprintf(ruta, sizeof(ruta), "/proc/%s/status", p->pid);

    obtener_valor(ruta, "Name:", p->name);
    obtener_valor(ruta, "State:", p->state);
    obtener_valor(ruta, "VmSize:", p->vmSize);
    obtener_valor(ruta, "VmExe:", p->vmExe);
    obtener_valor(ruta, "VmData:", p->vmData);
    obtener_valor(ruta, "VmStk:", p->vmStk);
    obtener_valor(ruta, "voluntary_ctxt_switches:", p->ctxtVol);
    obtener_valor(ruta, "nonvoluntary_ctxt_switches:", p->ctxtNoVol);
}

void mostrar_info(const Proceso *p) {
    printf("Pid: %s\n", p->pid);
    printf("Nombre del proceso: %s\n", p->name);
    printf("Estado: %s\n", p->state);
    printf("Tamaño total de la imagen de memoria: %s KB\n", p->vmSize);
    printf("Tamaño de la memoria TEXT: %s KB\n", p->vmExe);
    printf("Tamaño de la memoria DATA: %s KB\n", p->vmData);
    printf("Tamaño de la memoria STACK: %s KB\n", p->vmStk);
    printf("Número de cambios de contexto (voluntarios - no voluntarios): %s - %s\n\n", p->ctxtVol, p->ctxtNoVol);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s [-l] <PID> [PID ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    Proceso procesos[MAX_PIDS];
    int numProcesos = 0;

    if (strcmp(argv[1], "-l") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Debe proporcionar al menos un PID después de -l.\n");
            return EXIT_FAILURE;
        }

        for (int i = 2; i < argc; i++) {
            strncpy(procesos[numProcesos].pid, argv[i], sizeof(procesos[numProcesos].pid) - 1);
            recolectar_info(&procesos[numProcesos]);
            numProcesos++;
        }

        printf("-- Información recolectada!!!\n");
        for (int i = 0; i < numProcesos; i++) {
            mostrar_info(&procesos[i]);
        }

    } else {
        Proceso p;
        strncpy(p.pid, argv[1], sizeof(p.pid) - 1);
        recolectar_info(&p);
        mostrar_info(&p);
    }

    return EXIT_SUCCESS;
}
