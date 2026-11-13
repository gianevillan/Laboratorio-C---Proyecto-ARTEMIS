/*
 * simulador.c - Modulo de logica
 * Calculos fisicos (MRUV y caida libre), validacion de coherencia de los
 * datos, manejo del historial de simulaciones y presentacion de resultados.
 */

#include <stdio.h>
#include <math.h>
#include "simulador.h"

/* ------------------------------- Calculo -------------------------------- */

/* Valida rangos y coherencia entre datos (h debe superar a y1) */
CodigoError validar_datos(const Datos *d) {
    double y1;

    if (d == NULL) return ERR_PUNTERO_NULO;

    if (!(d->a1 > 0.0 && d->a1 <= A_MAX) ||
        !(d->a2 > 0.0 && d->a2 <= A_MAX) ||
        !(d->t1 > 0.0 && d->t1 <= T_MAX) ||
        !(d->h  > 0.0 && d->h  <= H_MAX) ||
        !(d->g  >= G_MIN && d->g <= G_MAX)) {
        return ERR_PARAMETRO;
    }

    y1 = 0.5 * d->a1 * d->t1 * d->t1;
    if (d->h <= y1) return ERR_H_MENOR_Y1;

    return OK;
}

/* Resuelve las cuatro preguntas del problema */
CodigoError calcular(const Datos *d, Resultados *r) {
    double delta_y, radicando;
    CodigoError e;

    if (d == NULL || r == NULL) return ERR_PUNTERO_NULO;

    e = validar_datos(d);
    if (e != OK) return e;

    /* Fase 1: MRUV desde el reposo */
    r->v1 = d->a1 * d->t1;
    r->y1 = 0.5 * d->a1 * d->t1 * d->t1;

    /* Fase 2: v2^2 = v1^2 + 2*a2*(h - y1) */
    delta_y   = d->h - r->y1;
    radicando = r->v1 * r->v1 + 2.0 * d->a2 * delta_y;
    if (radicando < 0.0) return ERR_NUMERICO;
    r->v2 = sqrt(radicando);

    /* Caida libre: hmax = h + v2^2 / (2g) */
    r->hmax = d->h + (r->v2 * r->v2) / (2.0 * d->g);

    /* Aceleracion como multiplo de g */
    r->a1_en_g = d->a1 / d->g;
    r->a2_en_g = d->a2 / d->g;

    if (!isfinite(r->v1) || !isfinite(r->y1) || !isfinite(r->v2) ||
        !isfinite(r->hmax)) {
        return ERR_NUMERICO;
    }
    return OK;
}

/* Carga los valores del enunciado */
void cargar_datos_enunciado(Datos *d) {
    if (d == NULL) return;
    d->a1 = 20.0;
    d->t1 = 120.0;
    d->a2 = 12.5;
    d->h  = 160000.0;
    d->g  = G_DEFECTO;
}

/* ------------------------------ Historial ------------------------------- */

CodigoError registrar(Simulacion *hist, int *n, const Datos *d,
                      const Resultados *r) {
    if (hist == NULL || n == NULL || d == NULL || r == NULL) {
        return ERR_PUNTERO_NULO;
    }
    if (*n >= MAX_SIM) return ERR_HISTORIAL_LLENO;

    (hist + *n)->datos = *d;
    (hist + *n)->res   = *r;
    (*n)++;
    return OK;
}

/* Calcula, muestra y guarda una simulacion. Devuelve el error si lo hubo. */
CodigoError ejecutar_simulacion(const Datos *d, Simulacion *hist, int *n) {
    Resultados r;
    CodigoError e;

    e = calcular(d, &r);
    if (e != OK) return e;

    mostrar_resultados(d, &r);

    e = registrar(hist, n, d, &r);
    if (e == OK) {
        printf("  Simulacion guardada en el historial (#%d).\n", *n);
    }
    return e;
}

/* ----------------------------- Presentacion ----------------------------- */

void mostrar_resultados(const Datos *d, const Resultados *r) {
    if (d == NULL || r == NULL) return;

    printf("\n" SEP_DOBLE);
    printf("  RESULTADOS\n");
    printf(SEP_DOBLE);
    printf("  1) Desacople de SRB (t = %.1f s):\n", d->t1);
    printf("     v1 = %.2f m/s\n", r->v1);
    printf("     y1 = %.2f m (%.2f km)\n\n", r->y1, r->y1 / 1000.0);

    printf("  2) Apagado de motores (h = %.1f m):\n", d->h);
    printf("     v2 = %.2f m/s\n\n", r->v2);

    printf("  3) Caida libre (g = %.2f m/s^2):\n", d->g);
    printf("     h_max = %.2f m (%.2f km)\n\n", r->hmax, r->hmax / 1000.0);

    printf("  4) Aceleracion maxima (fase 1):\n");
    printf("     a1 = %.2f g  (a2 = %.2f g en fase 2)\n", r->a1_en_g, r->a2_en_g);
    printf("     Incluyendo la gravedad a vencer: %.2f g\n", r->a1_en_g + 1.0);
    printf(SEP_DOBLE);
}

void listar_historial(const Simulacion *hist, int n) {
    const Simulacion *p;
    int i = 1;

    if (hist == NULL) return;
    if (n == 0) {
        printf("\n  El historial esta vacio.\n");
        return;
    }

    printf("\n  %-3s %7s %7s %7s %10s %9s %11s\n",
           "#", "a1", "t1", "a2", "h [m]", "v2 [m/s]", "hmax [km]");
    printf("  --------------------------------------------------------------\n");
    for (p = hist; p < hist + n; p++, i++) {
        printf("  %-3d %7.2f %7.1f %7.2f %10.1f %9.2f %11.2f\n",
               i, p->datos.a1, p->datos.t1, p->datos.a2, p->datos.h,
               p->res.v2, p->res.hmax / 1000.0);
    }
}

void mostrar_estadisticas(const Simulacion *hist, int n) {
    const Simulacion *p, *mayor_v2, *mayor_h;
    double suma_v2 = 0.0;

    if (hist == NULL) return;
    if (n == 0) {
        printf("\n  No hay simulaciones para analizar.\n");
        return;
    }

    mayor_v2 = mayor_h = hist;
    for (p = hist; p < hist + n; p++) {
        suma_v2 += p->res.v2;
        if (p->res.v2   > mayor_v2->res.v2)  mayor_v2 = p;
        if (p->res.hmax > mayor_h->res.hmax) mayor_h  = p;
    }

    printf("\n  Simulaciones cargadas : %d\n", n);
    printf("  Rapidez v2 promedio   : %.2f m/s\n", suma_v2 / n);
    printf("  Mayor v2              : %.2f m/s (simulacion #%d)\n",
           mayor_v2->res.v2, (int)(mayor_v2 - hist) + 1);
    printf("  Mayor altitud maxima  : %.2f km (simulacion #%d)\n",
           mayor_h->res.hmax / 1000.0, (int)(mayor_h - hist) + 1);
}