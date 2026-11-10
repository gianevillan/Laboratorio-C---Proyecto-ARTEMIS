Algoritmos y Estructuras de Datos - Laboratorio de C
Comisión: 1.4

Integrantes:
  Arias, Alejo Manuel
  Paniagua Mántaras, Morena Iliana
  Romero, Nayhara Soledad
  Vargas, Raúl Alejandro
  Villan, Gianella Agostina

Proyecto ARTEMIS
Escenario 1: Ascenso y Propulsión con el SLS (MRUV y Desprendimiento)
  El proyecto consiste en simular la primera fase de despegue de la nave Orion propulsada
  por el cohete Space Launch System (SLS) desde el Centro Espacial Kennedy.
Reglas de la simulación:
* Se desprecia la resistencia del aire y la variación de g con la altura.
* El desacople de los SRB es instantáneo y limpio: no altera la velocidad de la nave.
* La velocidad y la posición son continuas en el desacople.
* El programa solo acepta datos coherentes:
  a₁ y a₂ en (0; 100] m/s² ; t₁ en (0; 600] s ; h en (0; 10 000 000] m ; g en [0,1; 30] m/s² 
  h debe ser mayor que la altitud alcanzada al desacople (y₁); de lo contrario la fase 2 no existe.

Detalle de funcionamento:
  Al ejecutar el programa se muestra un menú:
  1. Nueva simulacion (ingresar datos)
  2. Simulacion con los valores del enunciado
  3. Ver historial (0/10)
  4. Estadisticas del historial
  5. Limpiar historial
  0. Salir
----------------------------------------------
Opción	Qué hace
1	Pide a₁, t₁, a₂, h y g. Cada dato se valida y se vuelve a pedir si es incorrecto. Luego calcula, muestra las cuatro respuestas y guarda la simulación.
2	Ejecuta la simulación con los valores del enunciado, sin ingresar datos.
3	Muestra en una tabla las simulaciones guardadas (hasta 10).
4	Calcula el promedio de v₂ y señala qué simulación tuvo la mayor v₂ y la mayor altitud máxima.
5	Vacía el historial, previa confirmación (s/n).
0	Sale del programa.

Instrucciones para compilar y ejecutar:
  Utilizamos gcc como compilador donde tendrán que cargar desde la carpeta del proyecto en la terminal el siguiente comando para su ejecución:
    gcc main.c simulador.c validaciones.c -o artemis -lm -Wall -Wextra
.\artemis.exe (para Windows);   ./artemis (para Linux o macOS)

Uso de Inteligencia Artificial:
  




  
