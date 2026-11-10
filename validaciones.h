/*
 * validaciones.h - Modulo de validaciones e interfaz
 * Ingreso seguro de enteros, flotantes y caracteres, y mensajes de
 * advertencia al usuario.
 */

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <stddef.h>
#include "simulador.h"

#define TAM_LINEA 64   /* tamano maximo de una linea de entrada */

/* Descripcion de un campo de entrada real (se recorre como arreglo) */
typedef struct {
    const char *etiqueta;
    double      minimo;    /* excluido */
    double      maximo;    /* incluido */
    double     *destino;   /* puntero al campo de Datos a completar */
} Campo;

/* Mensajes al usuario */
void mostrar_error(CodigoError e);

/* Lectura segura */
CodigoError leer_linea(char *buf, size_t n);
CodigoError leer_real(const Campo *campo);
CodigoError leer_entero(const char *etiqueta, int minimo, int maximo,
                        int *destino);
CodigoError leer_caracter(const char *pregunta, const char *validos,
                          char *destino);

/* Ingreso completo de los datos del problema */
CodigoError ingresar_datos(Datos *d);

#endif /* VALIDACIONES_H */
