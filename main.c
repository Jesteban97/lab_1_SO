#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

void show_guide() {
    fprintf(stderr, "Uso: psinfo [-l|-r] <listado PIDs>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: parámetros insuficientes.\n");
        show_guide();
        return 1;
    }

    Process processes[MAX_PIDS];
    int numprocesses = 0;
    int list_mode = 0;
    int report_mode = 0;

    if (strcmp(argv[1], "-l") == 0) list_mode = 1;
    else if (strcmp(argv[1], "-r") == 0) report_mode = 1;

    if (list_mode || report_mode) {
        if (argc < 3) {
            fprintf(stderr, "Error: debe proporcionar al menos un PID después de %s\n", argv[1]);
            show_guide();
            return 1;
        }

        for (int i = 2; i < argc; i++) {
            strncpy(processes[numprocesses].pid, argv[i], sizeof(processes[numprocesses].pid) - 1);
            collect_information(&processes[numprocesses]);
            numprocesses++;
        }

        if (list_mode) {
            printf("-- Información recolectada!!!\n");
            for (int i = 0; i < numprocesses; i++)
                if (processes[i].valid) print_information(stdout, &processes[i]);
        } else if (report_mode) {
            char namefile[256] = "psinfo-report";
            for (int i = 2; i < argc; i++) {
                strcat(namefile, "-");
                strcat(namefile, argv[i]);
            }
            strcat(namefile, ".info");

            FILE *f = fopen(namefile, "w");
            if (!f) {
                perror("Error al crear file");
                return 1;
            }

            for (int i = 0; i < numprocesses; i++)
                if (processes[i].valid) print_information(f, &processes[i]);

            fclose(f);
            printf("file de output generado: %s\n", namefile);
        }

    } else {
        if (argc != 2) {
            fprintf(stderr, "Error: formato incorrecto.\n");
            show_guide();
            return 1;
        }

        Process p;
        strncpy(p.pid, argv[1], sizeof(p.pid) - 1);
        collect_information(&p);
        if (!p.valid) {
            show_guide();
            return 1;
        }
        print_information(stdout, &p);
    }

    return 0;
}
