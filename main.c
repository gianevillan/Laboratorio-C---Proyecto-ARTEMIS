/*
 * main.c - Modulo principal
 * Contiene unicamente main(), el menu de navegacion y la inicializacion
 * del flujo general del programa.
 *
 * Simulador de Misiones Artemis - Problema 1
 * Ascenso y propulsion con el SLS (MRUV y desprendimiento)
 * UTN FRRe - Algoritmos y Estructuras de Datos
 *
 * Compilar:
 *   gcc main.c simulador.c validaciones.c -o artemis -lm -Wall -Wextra
 */

#include <stdio.h>
#include "simulador.h"
#include "validaciones.h"

static void mostrar_menu(int cargadas) {
    printf("\n" SEP_DOBLE);
    printf("  SIMULADOR ARTEMIS - ASCENSO DEL SLS / ORION\n");
    printf(SEP_DOBLE);
    printf("  1. Nueva simulacion (ingresar datos)\n");
    printf("  2. Simulacion con los valores del enunciado\n");
    printf("  3. Ver historial (%d/%d)\n", cargadas, MAX_SIM);
    printf("  4. Estadisticas del historial\n");
    printf("  5. Limpiar historial\n");
    printf("  0. Salir\n");
    printf(SEP_SIMPLE);
}

int main(void) {
    Simulacion historial[MAX_SIM];
    int cantidad = 0;
    int opcion = -1;
    char respuesta;
    Datos d;
    CodigoError e;

    do {
        mostrar_menu(cantidad);

        e = leer_entero("Opcion", 0, 5, &opcion);
        if (e != OK) {              /* entrada cerrada: salir ordenadamente */
            printf("\n");
            break;
        }

        switch (opcion) {
            case 1:
                e = ingresar_datos(&d);
                if (e == ERR_ENTRADA_CERRADA) { opcion = 0; break; }
                if (e == OK) {
                    e = ejecutar_simulacion(&d, historial, &cantidad);
                    if (e != OK) mostrar_error(e);
                }
                break;

            case 2:
                cargar_datos_enunciado(&d);
                e = ejecutar_simulacion(&d, historial, &cantidad);
                if (e != OK) mostrar_error(e);
                break;

            case 3:
                listar_historial(historial, cantidad);
                break;

            case 4:
                mostrar_estadisticas(historial, cantidad);
                break;

            case 5:
                if (cantidad == 0) {
                    printf("\n  El historial ya esta vacio.\n");
                    break;
                }
                e = leer_caracter("Confirma vaciar el historial? (s/n)",
                                  "sn", &respuesta);
                if (e == ERR_ENTRADA_CERRADA) { opcion = 0; break; }
                if (respuesta == 's') {
                    cantidad = 0;
                    printf("  Historial vaciado.\n");
                } else {
                    printf("  Operacion cancelada.\n");
                }
                break;

            case 0:
                break;
        }
    } while (opcion != 0);

    printf("\n  Fin del programa.\n");
    return 0;
}
