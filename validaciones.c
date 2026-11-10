/*
 * validaciones.c - Modulo de validaciones e interfaz
 * Funciones de ingreso seguro (enteros, flotantes y caracteres) y
 * mensajes de advertencia.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include "validaciones.h"

/* ------------------------------ Mensajes -------------------------------- */

/* Tabla de mensajes indexada por CodigoError */
static const char *MENSAJES_ERROR[NUM_ERRORES] = {
    "Operacion exitosa.",
    "Error interno: puntero nulo.",
    "Uno o mas parametros estan fuera del rango permitido.",
    "La altitud de apagado h debe ser mayor que la altitud de desacople y1. "
    "Aumente h, o reduzca a1 o t1.",
    "El calculo produjo un valor no valido (desborde o raiz negativa).",
    "El historial esta lleno. Use la opcion 5 para limpiarlo.",
    "La linea ingresada es demasiado larga.",
    "Se cerro la entrada de datos."
};

void mostrar_error(CodigoError e) {
    if (e > OK && e < NUM_ERRORES) {
        printf("  [ERROR] %s\n", MENSAJES_ERROR[e]);
    }
}

/* ------------------------- Lectura de bajo nivel ------------------------ */

/* Lee una linea completa. Descarta el exceso si es demasiado larga. */
CodigoError leer_linea(char *buf, size_t n) {
    size_t len;
    int c;

    if (buf == NULL) return ERR_PUNTERO_NULO;
    if (fgets(buf, (int)n, stdin) == NULL) return ERR_ENTRADA_CERRADA;

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return OK;
    }
    /* No hubo salto de linea: la entrada excedio el buffer */
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar el resto */ }
    return ERR_LINEA_LARGA;
}

/*
 * Convierte texto a double. Admite coma o punto decimal.
 * Rechaza: vacio, letras, hexadecimales, inf/nan, basura al final, desborde.
 * Devuelve 1 si es valido, 0 si no.
 */
static int parsear_real(const char *texto, double *salida) {
    char copia[TAM_LINEA];
    char *fin;
    double v;
    size_t i;

    if (texto == NULL || salida == NULL) return 0;

    strncpy(copia, texto, TAM_LINEA - 1);
    copia[TAM_LINEA - 1] = '\0';

    for (i = 0; copia[i] != '\0'; i++) {
        if (copia[i] == ',') copia[i] = '.';
        if (!strchr("0123456789+-.eE \t", copia[i])) return 0;
    }

    errno = 0;
    v = strtod(copia, &fin);
    if (fin == copia) return 0;                    /* nada convertido */
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0') return 0;                    /* basura al final */
    if (errno == ERANGE || !isfinite(v)) return 0; /* desborde */

    *salida = v;
    return 1;
}

/* Convierte texto a entero. Devuelve 1 si es valido, 0 si no. */
static int parsear_entero(const char *texto, int *salida) {
    char *fin;
    long v;

    if (texto == NULL || salida == NULL) return 0;

    errno = 0;
    v = strtol(texto, &fin, 10);
    if (fin == texto) return 0;
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0' || errno == ERANGE || v < -1000000L || v > 1000000L) {
        return 0;
    }

    *salida = (int)v;
    return 1;
}

/* --------------------------- Lectura validada --------------------------- */

/* Pide un real en (minimo, maximo]. Repite hasta que sea valido. */
CodigoError leer_real(const Campo *campo) {
    char buf[TAM_LINEA];
    double valor;
    CodigoError e;

    if (campo == NULL || campo->destino == NULL) return ERR_PUNTERO_NULO;

    for (;;) {
        printf("  %s (%.2f < x <= %.2f): ", campo->etiqueta,
               campo->minimo, campo->maximo);

        e = leer_linea(buf, sizeof(buf));
        if (e == ERR_ENTRADA_CERRADA) return e;
        if (e != OK) { mostrar_error(e); continue; }

        if (!parsear_real(buf, &valor)) {
            printf("  [ERROR] Ingrese un numero valido (ej. 20 o 20,5).\n");
            continue;
        }
        if (valor <= campo->minimo || valor > campo->maximo) {
            printf("  [ERROR] El valor esta fuera del rango permitido.\n");
            continue;
        }
        *campo->destino = valor;
        return OK;
    }
}

/* Pide un entero en [minimo, maximo]. Repite hasta que sea valido. */
CodigoError leer_entero(const char *etiqueta, int minimo, int maximo,
                        int *destino) {
    char buf[TAM_LINEA];
    int valor;
    CodigoError e;

    if (etiqueta == NULL || destino == NULL) return ERR_PUNTERO_NULO;

    for (;;) {
        printf("  %s (%d-%d): ", etiqueta, minimo, maximo);

        e = leer_linea(buf, sizeof(buf));
        if (e == ERR_ENTRADA_CERRADA) return e;
        if (e != OK) { mostrar_error(e); continue; }

        if (!parsear_entero(buf, &valor)) {
            printf("  [ERROR] Ingrese un numero entero.\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("  [ERROR] Elija un numero entre %d y %d.\n", minimo, maximo);
            continue;
        }
        *destino = valor;
        return OK;
    }
}

/*
 * Pide un unico caracter que pertenezca a 'validos' (en minusculas).
 * No distingue mayusculas de minusculas. Devuelve el caracter en minuscula.
 */
CodigoError leer_caracter(const char *pregunta, const char *validos,
                          char *destino) {
    char buf[TAM_LINEA];
    char *p;
    CodigoError e;

    if (pregunta == NULL || validos == NULL || destino == NULL) {
        return ERR_PUNTERO_NULO;
    }

    for (;;) {
        printf("  %s: ", pregunta);

        e = leer_linea(buf, sizeof(buf));
        if (e == ERR_ENTRADA_CERRADA) return e;
        if (e != OK) { mostrar_error(e); continue; }

        /* Saltear espacios al inicio y al final */
        p = buf;
        while (isspace((unsigned char)*p)) p++;
        {
            size_t len = strlen(p);
            while (len > 0 && isspace((unsigned char)p[len - 1])) p[--len] = '\0';
        }

        if (strlen(p) != 1) {
            printf("  [ERROR] Ingrese un unico caracter (%s).\n", validos);
            continue;
        }
        *p = (char)tolower((unsigned char)*p);
        if (strchr(validos, *p) == NULL) {
            printf("  [ERROR] Opcion no valida. Use: %s.\n", validos);
            continue;
        }
        *destino = *p;
        return OK;
    }
}

/* Completa Datos recorriendo un arreglo de Campo */
CodigoError ingresar_datos(Datos *d) {
    Campo campos[] = {
        { "Aceleracion fase 1, a1 [m/s^2]",  0.0,   A_MAX, NULL },
        { "Tiempo de desacople SRB, t1 [s]", 0.0,   T_MAX, NULL },
        { "Aceleracion fase 2, a2 [m/s^2]",  0.0,   A_MAX, NULL },
        { "Altitud de apagado, h [m]",       0.0,   H_MAX, NULL },
        { "Gravedad, g [m/s^2]",             G_MIN, G_MAX, NULL }
    };
    const size_t n = sizeof(campos) / sizeof(campos[0]);
    size_t i;
    CodigoError e;

    if (d == NULL) return ERR_PUNTERO_NULO;

    campos[0].destino = &d->a1;
    campos[1].destino = &d->t1;
    campos[2].destino = &d->a2;
    campos[3].destino = &d->h;
    campos[4].destino = &d->g;

    printf("\n  Ingrese los datos (acepta coma o punto decimal):\n");
    for (i = 0; i < n; i++) {
        e = leer_real(&campos[i]);
        if (e != OK) return e;
    }
    return OK;
}
