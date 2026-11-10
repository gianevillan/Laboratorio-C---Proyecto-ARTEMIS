/*
 * simulador.h - Modulo de cabecera
 * Constantes (#define), tipos estructurados (struct, typedef, enum)
 * y prototipos de las funciones publicas de la simulacion.
 */

#ifndef SIMULADOR_H
#define SIMULADOR_H

/* ------------------------------ Constantes ------------------------------ */

#define MAX_SIM        10      /* capacidad del historial */
#define G_DEFECTO      9.80    /* gravedad terrestre [m/s^2] */

/* Rangos admitidos para los datos de entrada (minimo excluido, maximo incluido) */
#define A_MAX          100.0   /* aceleracion [m/s^2] */
#define T_MAX          600.0   /* tiempo [s] */
#define H_MAX          1.0e7   /* altitud [m] */
#define G_MIN          0.1     /* gravedad minima [m/s^2] (incluida) */
#define G_MAX          30.0    /* gravedad maxima [m/s^2] */

/* Separadores para la interfaz */
#define SEP_DOBLE      "==============================================\n"
#define SEP_SIMPLE     "----------------------------------------------\n"

/* ------------------------------- Tipos ---------------------------------- */

/* Codigos de error compartidos por todos los modulos */
typedef enum {
    OK = 0,
    ERR_PUNTERO_NULO,
    ERR_PARAMETRO,
    ERR_H_MENOR_Y1,
    ERR_NUMERICO,
    ERR_HISTORIAL_LLENO,
    ERR_LINEA_LARGA,
    ERR_ENTRADA_CERRADA,
    NUM_ERRORES
} CodigoError;

/* Datos de entrada del problema */
typedef struct {
    double a1;   /* aceleracion fase 1 [m/s^2] */
    double t1;   /* instante de desacople de los SRB [s] */
    double a2;   /* aceleracion fase 2 [m/s^2] */
    double h;    /* altitud de apagado de motores [m] */
    double g;    /* gravedad [m/s^2] */
} Datos;

/* Resultados del problema */
typedef struct {
    double v1;       /* rapidez al desacople [m/s] */
    double y1;       /* altitud al desacople [m] */
    double v2;       /* rapidez al apagado [m/s] */
    double hmax;     /* altitud maxima en caida libre [m] */
    double a1_en_g;  /* a1 como multiplo de g */
    double a2_en_g;  /* a2 como multiplo de g */
} Resultados;

/* Una simulacion guardada en el historial */
typedef struct {
    Datos      datos;
    Resultados res;
} Simulacion;

/* ------------------------ Prototipos (logica) --------------------------- */

CodigoError validar_datos(const Datos *d);
CodigoError calcular(const Datos *d, Resultados *r);
void        cargar_datos_enunciado(Datos *d);

CodigoError registrar(Simulacion *hist, int *n, const Datos *d,
                      const Resultados *r);
CodigoError ejecutar_simulacion(const Datos *d, Simulacion *hist, int *n);

void mostrar_resultados(const Datos *d, const Resultados *r);
void listar_historial(const Simulacion *hist, int n);
void mostrar_estadisticas(const Simulacion *hist, int n);

#endif /* SIMULADOR_H */
