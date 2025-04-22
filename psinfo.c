#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

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
    int valido;
} Proceso;

void mostrar_uso() {
    fprintf(stderr, "Uso: psinfo [-l|-r] <listado PIDs>\n");
}

int archivo_existe(const char *ruta) {
    struct stat buffer;
    return (stat(ruta, &buffer) == 0);
}

void obtener_valor(const char *archivo, const char *clave, char *resultado) {
    FILE *fp = fopen(archivo, "r");
    if (!fp) {
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

    if (!archivo_existe(ruta)) {
        fprintf(stderr, "Error: el proceso con PID %s no existe.\n", p->pid);
        p->valido = 0;
        return;
    }

    p->valido = 1;
    obtener_valor(ruta, "Name:", p->name);
    obtener_valor(ruta, "State:", p->state);
    obtener_valor(ruta, "VmSize:", p->vmSize);
    obtener_valor(ruta, "VmExe:", p->vmExe);
    obtener_valor(ruta, "VmData:", p->vmData);
    obtener_valor(ruta, "VmStk:", p->vmStk);
    obtener_valor(ruta, "voluntary_ctxt_switches:", p->ctxtVol);
    obtener_valor(ruta, "nonvoluntary_ctxt_switches:", p->ctxtNoVol);
}

void imprimir_info(FILE *salida, const Proceso *p) {
    if (!p->valido) return;

    fprintf(salida, "Pid: %s\n", p->pid);
    fprintf(salida, "Nombre del proceso: %s\n", p->name);
    fprintf(salida, "Estado: %s\n", p->state);
    fprintf(salida, "Tamaño total de la imagen de memoria: %s KB\n", p->vmSize);
    fprintf(salida, "Tamaño de la memoria TEXT: %s KB\n", p->vmExe);
    fprintf(salida, "Tamaño de la memoria DATA: %s KB\n", p->vmData);
    fprintf(salida, "Tamaño de la memoria STACK: %s KB\n", p->vmStk);
    fprintf(salida, "Número de cambios de contexto (voluntarios - no voluntarios): %s - %s\n\n", p->ctxtVol, p->ctxtNoVol);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: parámetros insuficientes.\n");
        mostrar_uso();
        return 1;
    }

    Proceso procesos[MAX_PIDS];
    int numProcesos = 0;
    int modo_lista = 0;
    int modo_reporte = 0;

    if (strcmp(argv[1], "-l") == 0) {
        modo_lista = 1;
    } else if (strcmp(argv[1], "-r") == 0) {
        modo_reporte = 1;
    }

    if (modo_lista || modo_reporte) {
        if (argc < 3) {
            fprintf(stderr, "Error: debe proporcionar al menos un PID después de %s\n", argv[1]);
            mostrar_uso();
            return 1;
        }

        for (int i = 2; i < argc; i++) {
            strncpy(procesos[numProcesos].pid, argv[i], sizeof(procesos[numProcesos].pid) - 1);
            recolectar_info(&procesos[numProcesos]);
            numProcesos++;
        }

        if (modo_lista) {
            printf("-- Información recolectada!!!\n");
            for (int i = 0; i < numProcesos; i++) {
                if (procesos[i].valido)
                    imprimir_info(stdout, &procesos[i]);
            }
        } else if (modo_reporte) {
            // Crear nombre del archivo
            char nombreArchivo[256] = "psinfo-report";
            for (int i = 2; i < argc; i++) {
                strcat(nombreArchivo, "-");
                strcat(nombreArchivo, argv[i]);
            }
            strcat(nombreArchivo, ".info");

            FILE *f = fopen(nombreArchivo, "w");
            if (!f) {
                perror("Error al crear archivo");
                return 1;
            }

            for (int i = 0; i < numProcesos; i++) {
                if (procesos[i].valido)
                    imprimir_info(f, &procesos[i]);
            }

            fclose(f);
            printf("Archivo de salida generado: %s\n", nombreArchivo);
        }

    } else {
        // Un solo PID
        if (argc != 2) {
            fprintf(stderr, "Error: formato incorrecto.\n");
            mostrar_uso();
            return 1;
        }

        Proceso p;
        strncpy(p.pid, argv[1], sizeof(p.pid) - 1);
        recolectar_info(&p);
        if (!p.valido) {
            mostrar_uso();
            return 1;
        }
        imprimir_info(stdout, &p);
    }

    return 0;
}
