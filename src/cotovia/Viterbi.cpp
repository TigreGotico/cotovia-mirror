/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ramón Piñeiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades,
                     Xunta de Galicia, Santiago de Compostela, Spain

License: GPL-3.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 .
 On Debian systems, the complete text of the GNU General
 Public License version 3 can be found in /usr/share/common-licenses/GPL-3.

______________________________________________________________________________*/
 


/** \file Viterbi.cpp
 
    <H1>El módulo Viterbi</H1>
 
    <P> Este modulo está dedicado a la implementación del algoritmo de Viterbi, 
    adaptado a la selección de unidades acústicas almacenadas en objetos de la 
    clase Vector semifonema candidato. Permite escoger entre diferentes 
    posibilidades para la función de coste de objetivo, pasando por redes 
    neuronales y regresión lineal. <P>

    <H2>Poda</H2>

    <P> Casi toda la carga computacional del algoritmo de viterbi recae en el 
    cálculo de los costes de concatenación entre las unidades candidatas de dos 
    iteraciones consecutivas. Para reducir este problema se puede recurrir al uso 
    de una caché en la que se encuentren ya precalculados dichos costes. Sin 
    embargo, como parece evidente, el espacio ocupado por una tabla en la que se 
    almacenasen todas las posibles combinaciones de unidades de la base de datos 
    podría llegar a ser excesivamente grande, lo cual obligaría a seleccionar un 
    subconjunto de los productos, los más comunes, y calcular en tiempo de 
    ejecución el resto. Otra alternativa completamente diferente, denominada poda, 
    consiste en no considerar todas las unidades posibles en cada iteración, 
    disminuyendo de esta forma el número de costes de concatenación que se deben 
    calcular.

    <P> Básicamente, hay dos tipos de poda:

    <UL>
  
    <LI><B>Poda por valor:</B> consiste en eliminar todos aquellos caminos cuyo 
    coste acumulado supere en una cierta cantidad al mínimo encontrado hasta el 
    momento. además, tiene la ventaja de que si todas las funciones de coste están 
    acotadas, existe un valor <B>c_poda</B> tal que todos aquellos caminos que 
    superen al mínimo en él, podrán ser podados con la garantía de que no se 
    perderá la secuencia óptima.
  
    <LI><B>Poda por número:</B> en lugar de eliminar posibilidades según su 
    diferencia de coste, se limita el máximo número de caminos que se van a 
    considerar a <B>n</B> de menor valor hasta el momento. dado que no hay un 
    control directo sobre los pesos de cada camino, es probable que se descarte 
    alguno que sí que podría ser óptimo al final del algoritmo, lo que supone que 
    sea preciso llegar a un compromiso entre la reducción de la complejidad 
    computacional y la calidad del camino seleccionado (al inicio del fichero 
    Viterbi.hpp se encuentran las macros C_PODA_C_i, C_PODA_UNIDAD, N_PODA_UNIDAD y N_PODA_C_i 
    con las que se controlan estos factores).
    </UL>

    <P> A la vista del algoritmo, hay dos puntos en los que se puede realizar una 
    poda. El primero, en la cadena de unidades candidatas para cada iteración, 
    donde se pueden realizar una poda por valor o por número según los valores de 
    la función de coste de objetivo, y el segundo, en la cadena de caminos óptimos 
    de cada iteración del algoritmo, a partir del coste acumulado para cada 
    secuencia de unidades.

    <H3>Modos de funcionamiento</H3>

    <P>La elección del tipo de poda, así como el lugar en que se desea efectuar, se 
    realiza en el fichero Viterbi.hpp, por medio de las 
    siguientes macros:
    <UL>
    <LI> PODA_C_t_POR_NUMERO: poda en la cadena de unidades candidatas, por número.
    <LI> PODA_C_t_POR_VALOR: poda en la cadena de unidades candidatas, por valor.
    <LI> PODA_C_i_POR_NUMERO: poda en la cadena de caminos óptimos. 
    <LI> PODA_C_i_POR_VALOR: poda en la cadena de caminos óptimos, por valor.
    </UL>

    <P> De esta forma, por ejemplo, si se desease realizar una poda por valor en la 
    cadena de caminos óptimos y una poda por número en las unidades candidatas, 
    llegaría con definir las macros pertinentes. Ninguna de ellas es excluyente.  

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <set>

#include "modos.hpp"
#include "fonemas.hpp"
#include "info_corpus_voz.hpp"
//#include "matriz.hpp"
#include "path_list.hpp"
#include "cache.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "info_estructuras.hpp"
#include "Viterbi.hpp"

using namespace std;

unsigned char Viterbi::fuerza_fronteras_entonativas = 0; // Si vale cero, deja que el algoritmo de selección decida si se introduce o no una frontera entonativa. En caso contrario, fuerza que se introduzca la frontera objetivo.

#if (defined(_UTILIZA_CENTROIDES) || defined(_UTILIZA_REGRESION) ||	\
     defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
     defined(_UTILIZA_RED_NEURONAL) ||					\
     defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ) &&	\
  !defined(_CALCULA_FRECUENCIA_COSTES_CONCATENACION)


#endif

/**
 * \remarks Modifica la variable miembro modifica_fronteras_entonativas
 * \param[in] fuerza Valor que se le quiere asignar a dicha variable
 */
void Viterbi::modifica_fuerza_fronteras_entonativas(unsigned char fuerza) {
  fuerza_fronteras_entonativas = fuerza;
}

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

/**
 * Función:   establece_valores_identificadores                                     
 * \remarks Entrada:
 *			- identificador_menor: identificador más bajo de las unidades que       
 *            pertenecen a la frase original del Corpus.                            
 *          - identificador_mayor: identificador más alto de las unidades de dicha  
 *            frase.                                                                
 */

void Viterbi::establece_valores_identificadores(int identificador_menor, int identificador_mayor) {

  menor_identificador = identificador_menor;
  mayor_identificador = identificador_mayor;

}

#endif


/**
 * Función:   calcula_distancia_euclidea_optimizada                                       
 * \remarks Entrada:
 *			- vector1, vector2: vectores de coeficientes cepstrales.                      
 * \remarks Valor devuelto:                                                                        
 *          - La distancia euclídea.                                                      
 */

inline float Viterbi::calcula_distancia_euclidea_optimizada(Vector_cepstrum vector1, Vector_cepstrum vector2) {

  float *cepstrum1 = (float *) vector1;
  float *cepstrum2 = (float *) vector2;
  float diferencia;
  float acumulado = 0.0;


  diferencia = cepstrum1[0] - cepstrum2[0];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[1] - cepstrum2[1];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[2] - cepstrum2[2];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[3] - cepstrum2[3];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[4] - cepstrum2[4];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[5] - cepstrum2[5];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[6] - cepstrum2[6];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[7] - cepstrum2[7];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[8] - cepstrum2[8];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[9] - cepstrum2[9];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[10] - cepstrum2[10];
  acumulado += diferencia*diferencia;

  diferencia = cepstrum1[11] - cepstrum2[11];
  acumulado += diferencia*diferencia;
  /*
    diferencia = cepstrum1[12] - cepstrum2[12];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[13] - cepstrum2[13];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[14] - cepstrum2[14];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[15] - cepstrum2[15];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[16] - cepstrum2[16];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[17] - cepstrum2[17];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[18] - cepstrum2[18];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[19] - cepstrum2[19];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[20] - cepstrum2[20];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[21] - cepstrum2[21];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[22] - cepstrum2[22];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[23] - cepstrum2[23];
    acumulado += diferencia*diferencia;

    diferencia = cepstrum1[24] - cepstrum2[24];
    acumulado += diferencia*diferencia;
  */
  return (float) sqrt(acumulado);

}



//#ifdef _MODO_NORMAL

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

/**
 * Función:   devuelve_red                                                    
 * \remarks Entrada:
 *			- clase: clase espectral del semifonema.						  
 *	\remarks Salida:
 *			- red: puntero a la red neuronal solicitada.                      
 *          - estadisticos: estadísticos (media y varianza) de los parámetros 
 *            de entrada de la red.                                           
 * \remarks Valor devuelto:															  
 *			- En ausencia de error, devuelve un cero.						  
 */

inline int Viterbi::devuelve_red(clase_espectral clase, Red_neuronal **red,
				 Media_varianza **estadisticos) {



  if ( (clase >= 0) && (clase < NUMERO_ELEMENTOS_CLASE_ESPECTRAL) ) {

    *red = &(locutor_actual->red_contextual[clase]);
    *estadisticos = locutor_actual->estadisticos_contextual[clase];
    return 0;

  } // if ( (clase >= 0) ...

  fprintf(stderr, "Error en devuelve_red: clase (%d) no contemplada.\n", (int) clase);
  return 1;

}

#endif

#ifdef _UTILIZA_REGRESION

/**
 * Función:   devuelve_vector_pesos                                           
 * \remarks Entrada:
 *			- semifonema: alófono central del semifonema.                     
 * Valor_devuelto:                                                            
 *          - Puntero al vector de pesos de la regresión lineal. Si hay algún 
 *			  error, devuelve NULL.							                  
 */

inline float *Viterbi::devuelve_vector_pesos(clase_espectral clase) {

  if ( (clase < 0) || (clase >= NUMERO_ELEMENTOS_CLASE_ESPECTRAL) ) {
    fprintf(stderr, "Error en devuelve_vector_pesos: clase (%d) no contemplado.\n", clase);
    return NULL;
  }  // if ( (clase < 0) || ...

  return (float *) locutor_actual->pesos_regresion[(int) clase];

}

#endif


#if (defined(_UTILIZA_CENTROIDES) || defined(_UTILIZA_REGRESION) ||	\
     defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
     defined(_UTILIZA_RED_NEURONAL)  ||					\
     defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ) &&	\
  !defined(_CALCULA_FRECUENCIA_COSTES_CONCATENACION)



#endif

/**
 * Función:   caracteristicas_de_fonema                                             
 * \remarks Entrada:
 *			- fonema: representación gráfica de un alófono.                         
 *	\remarks Salida:
 *			- clase_alofono: tipo de fonema.                                        
 *          - sordo_sonoro: SORDO, o SONORO.                                        
 *          - vocal_consonante: VOCAL, CONSONANTE o SILENCIO.                       
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un 0.                                  
 */

inline int Viterbi::caracteristicas_de_fonema(char *fonema, clase_duracion *tipo_alofono, char *sordo_sonoro, char *vocal_consonante) {


  char alofono = fonema[0];

  if (alofono == 'a') {
    //        *tipo_alofono = VOC_ABIER;
    *tipo_alofono = VOC_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = VOCAL;
    return 0;
  } // if (alofono == 'a')

  if ( (alofono == 'e') || (alofono == 'E') ||
       (alofono == 'o') || (alofono == 'O') ) {
    *tipo_alofono = VOC_MED_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = VOCAL;
    return 0;
  } // if ( (alofono == 'e') ...

  if ( (alofono == 'i') || (alofono == 'u') ) {
    *tipo_alofono = VOC_CER_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = VOCAL;
    return 0;
  } // if ( (alofono == 'i') ...

  if ( (alofono == 'S') || (alofono == 'T') ||
       (alofono == 'f') || (alofono == 's') || (alofono == 'x') ) {
    *tipo_alofono = FRIC_SOR_DUR;
    *sordo_sonoro = SORDO;
    *vocal_consonante = CONSONANTE;
    return 0;
  } // if ( (alofono == 'S') ...

  if ( (alofono == 'Z') || (alofono == 'l') ) {
    *tipo_alofono = LIQUIDA_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = CONSONANTE;
    return 0;
  } // if ( (alofono == 'Z') ...

  if ( (alofono == 'J') || (alofono == 'N') ||
       (alofono == 'm') || (alofono == 'n') ) {
    *tipo_alofono = NASAL_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = CONSONANTE;
    return 0;
  } // if ( (alofono == 'J') ...

  if ( (alofono == 'B') || (alofono == 'D') || (alofono == 'G') ||
       (alofono == 'b') || (alofono == 'd') || (alofono == 'g') ) {
    *tipo_alofono = OCLU_SON_DUR;
    *sordo_sonoro = SONORO;
    *vocal_consonante = CONSONANTE;
    return 0;
  } // if ( (alofono == 'B') ...

  if ( (alofono == 'k') || (alofono == 'p') || (alofono == 't') ) {
    *tipo_alofono = OCLU_SOR_DUR;
    *sordo_sonoro = SORDO;
    *vocal_consonante = CONSONANTE;
    return 0;
  } // if ( (alofono == 'k')

  if (alofono == 'r') {
    *sordo_sonoro = SONORO;
    *vocal_consonante = CONSONANTE;
    *tipo_alofono = VIBRANTE_DUR;
    return 0;
  } // if (alofono == 'r')

  if (alofono == '#') {
    *tipo_alofono = SILENCIO_DUR;
    *sordo_sonoro = SORDO;
    *vocal_consonante = SILENCIO_DUR;
    return 0;
  } // if (alofono == '#')

  fprintf(stderr, "Error en caracteristicas_de_fonema: Tipo de fonema (%s) no válido.\n",
	  fonema);

  return 1;

}


/**
 * Función:   carga_tabla_cache                                                         
 * \remarks Valor devuelto:                                                                      
 *           - Si no existe el fichero de caché, o bien hay algún error al cargarlo en  
 *             memoria, devuelve un 1.                                                  
 * \remarks Objetivo:  Carga el contenido del archivo indicado en el define FICHERO_CACHE en     
 *            memoria.                                                                  
 */
/*
  int carga_tabla_cache() {

  return memoria_cache.inicia_cache(FICHERO_CACHE);

  }
*/

/**
 * Función:   inicia_poda_C_t                                                           
 * \remarks Objetivo:  Inicializa la variable minimo_c_t, necesaria para el cálculo del umbral   
 *            para la poda por valor del array de costes de unidad. Es necesario llamar 
 *            a esta función cada vez que se inicia un nuevo paso del algoritmo de      
 *            Viterbi.                                                                  
 */

#ifdef PODA_C_t_POR_VALOR

void Viterbi::inicia_poda_C_t() {

  minimo_c_t = FLT_MAX - C_PODA_UNIDAD;

}

#endif

/**
 * Función:   inicia_poda_C_i                                                           
 * \remarks Objetivo:  Inicializa la variable minimo_c_i, necesaria para el cálculo del umbral   
 *            para la poda por valor del array de caminos de coste mínimo. Es necesario 
 *            llamar a esta función cada vez que se inicia un nuevo paso del algoritmo  
 *            de Viterbi.                                                               
 */

#ifdef PODA_C_i_POR_VALOR

void Viterbi::inicia_poda_C_i() {

  minimo_c_i = FLT_MAX - C_PODA_C_i;

}

#endif

/**
 * Función:   comprueba_entrada_en_cache                                                 
 * \remarks Entrada:
 *			- identificador1: identificador de la unidad que proporciona un camino de    
 *            coste mínimo en el paso anterior.                                          
 *          - identificador2: identificador de la unidad considerada en el paso actual.  
 * \remarks Objetivo:  Comprueba si el coste de concatenación entre ambas unidades se encuentra   
 *            almacenado en la caché. En caso afirmativo, devuelve su valor. Si no,      
 *            devuelve un número negativo.                                               
 */
/*
  float comprueba_entrada_en_cache(int identificador1, int identificador2) {

  float resultado;

  // Comprobamos si está en la caché. Si está, ya devolvemos el resultado.
  if ( (resultado = memoria_cache.comprueba_existencia(identificador1, identificador2))
  >= 0)
  return resultado;

  // De momento, para las pruebas.

  return 50.0; // Así forzamos que haya poda.
  }
*/

/**
 * Función:   crea_delta_u                                                                
 * \remarks Entrada:
 *			- objetivo: vector descriptor de la unidad que se plantea como objetivo.      
 * Salida   - *cadena_id: cadena de identificadores de las unidades preseleccíonadas.     
 *          - tamano: número de elementos de dicha cadena.
 * \remarks Valor devuelto:
 *          - Devuelve 0 en caso de que no se produzca error.
 * \remarks Objetivo:  Almacena en *delta_u las candidatas para representar a la unidad objetivo
 *            que se le pasa como parámetro.
 */

int Viterbi::crea_delta_u(Vector_semifonema_candidato ***cadena_id, int *tamano, Vector_descriptor_objetivo &objetivo) {

  char central[3*OCTETOS_POR_FONEMA], derecho[2*OCTETOS_POR_FONEMA],
    izquierdo[OCTETOS_POR_FONEMA];
  estructura_unidades *unidad_seleccionada;

  objetivo.devuelve_fonemas(central, izquierdo, derecho);

  izq_der == IZQUIERDO ? unidad_seleccionada = devuelve_puntero_a_unidades(izquierdo, central, IZQUIERDO, locutor_actual):
    unidad_seleccionada = devuelve_puntero_a_unidades(central,derecho,
						      DERECHO, locutor_actual);
  if (unidad_seleccionada == NULL)
    izq_der == IZQUIERDO ? unidad_seleccionada = reglas_sustitucion_semifonemas(izquierdo, central, IZQUIERDO, locutor_actual):
      unidad_seleccionada = reglas_sustitucion_semifonemas(central, derecho, DERECHO, locutor_actual);
  if (unidad_seleccionada == NULL) {
    if (izq_der == IZQUIERDO)
      fprintf(stderr, "Error en crea_delta_u: no hay candidatos para %s.%s.\n", izquierdo, central);
    else
      fprintf(stderr, "Error en crea_delta_u: no hay candidatos para %s.%s.\n", central, derecho);
    return 1;
  }
    
#ifdef _SELECCION_ANTERIOR
  *cadena_id = crea_cadena_unidades_candidatas(unidad_seleccionada, objetivo.frase, objetivo.posicion, objetivo.acento, tamano, tipo_candidatos);
#else
  *cadena_id = crea_cadena_unidades_candidatas_tonicidad(unidad_seleccionada, objetivo.acento, tamano);

#endif

// #ifdef _DEPURA_VITERBI
//   if (izq_der == IZQUIERDO) 
//     fprintf(fichero_viterbi, "---> En crea_delta_u (%d): (%s) (%s) Izquierdo (%d candidatos).\n", iteracion, izquierdo, central, *tamano);
//   else
//     fprintf(fichero_viterbi, "---> En crea_delta_u (%d): (%s) (%s) Derecho (%d candidatos).\n", iteracion, central, derecho, *tamano);

//   Vector_semifonema_candidato *apunta_candidato = **cadena_id;

//   for (int contador = 0; contador < *tamano; contador++, apunta_candidato++)
//     fprintf(fichero_viterbi, "------> %d\n", apunta_candidato->identificador);

//   fprintf(fichero_viterbi, "\n");
// #endif

  return 0;

}


/**
 * Función:   escribe_subcostes_C_t                                                      
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 *          - fichero: fichero en el que se va a escribir el valor de cada subcoste de   
 *            la función.                                                                
 * \remarks Valor devuelto:                                                                       
 *          - En ausencia de error devuelve un 0.                                        
 * \remarks NOTA:
 *			- No incluye al final el salto de línea. De esta forma, se pueden añadir más 
 *            datos en la misma línea.                                                   
 */

int Viterbi::escribe_subcostes_C_t(Vector_semifonema_candidato *candidato, Vector_semifonema_candidato *objetivo, char izq_der, FILE *fichero) {

  // De momento

  //    int subcoste = 0;
  //    char sordo_sonoro_cand, sordo_sonoro_obj;
  //    char vocal_consonante_cand, vocal_con/sonante_obj;
  //    clase_alofono tipo_alofono_cand, tipo_alofono_obj;
  float duracion_obj;
  /*
    float frec_inicial_obj, frec_cent_obj, frec_final_obj;
    float frec_inicial_cand, frec_final_cand;
  */
  float duracion_cand;

  /*
    float frec_izda_obj;
    float frec_cent_obj;
    float frec_dcha_obj;
    float frec_izda_cand, frec_cent_cand, frec_dcha_cand;
  */
  //    char *apunta_candidato;
  //    char *apunta_objetivo;

  duracion_cand = candidato->duracion;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion;

    /*
      if (caracteristicas_de_fonema(candidato->contexto_derecho, &tipo_alofono_cand,
      &sordo_sonoro_cand, &vocal_consonante_cand))
      return 1;

      if (caracteristicas_de_fonema(objetivo->contexto_derecho, &tipo_alofono_obj,
      &sordo_sonoro_obj, &vocal_consonante_obj))
      return 1;

      if (tipo_alofono_obj != tipo_alofono_cand)
      subcoste = 1;
      if (sordo_sonoro_obj != sordo_sonoro_cand)
      subcoste += 1;
      if (vocal_consonante_obj != vocal_consonante_cand)
      subcoste += 1;

      fprintf(fichero, "%d\t", subcoste);

      /*
      apunta_candidato = (char *) candidato->contexto_derecho;
      apunta_objetivo = (char *) objetivo->contexto_derecho;

      if ( (*apunta_candidato != *apunta_objetivo) ||
      (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
      fprintf(fichero, "1\t");
      else
      fprintf(fichero, "0\t");
    */

    // En el punto de coarticulación.

    fprintf(fichero, "%f\t", absoluto((objetivo->frecuencia_inicial - candidato->frecuencia_inicial))
	    / objetivo->frecuencia_inicial);

    // En la parte estacionaria.

    fprintf(fichero, "%f\t", absoluto((objetivo->frecuencia_final - candidato->frecuencia_final))
	    / objetivo->frecuencia_final);

  } // if (izq_der == IZQUIERDO)

  else {

    duracion_obj = objetivo->duracion;

    /*
      if (caracteristicas_de_fonema(candidato->contexto_izquierdo, &tipo_alofono_cand,
      &sordo_sonoro_cand, &vocal_consonante_cand))
      return 1;

      if (caracteristicas_de_fonema(objetivo->contexto_izquierdo, &tipo_alofono_obj,
      &sordo_sonoro_obj, &vocal_consonante_obj))
      return 1;

      if (tipo_alofono_obj != tipo_alofono_cand)
      subcoste = 1;
      if (sordo_sonoro_obj != sordo_sonoro_cand)
      subcoste += 1;
      if (vocal_consonante_obj != vocal_consonante_cand)
      subcoste += 1;

      fprintf(fichero, "%d\t", subcoste);
      /*
      apunta_candidato = (char *) candidato->contexto_izquierdo;
      apunta_objetivo = (char *) objetivo->contexto_izquierdo;

      if ( (*apunta_candidato != *apunta_objetivo) ||
      (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
      fprintf(fichero, "1\t");
      else
      fprintf(fichero, "0\t");
    */
    // En el punto de coarticulación.
    fprintf(fichero, "%f\t", absoluto((objetivo->frecuencia_final - candidato->frecuencia_final))
	    / objetivo->frecuencia_final);

    // En la zona estacionaria.
    fprintf(fichero, "%f\t", absoluto((objetivo->frecuencia_inicial - candidato->frecuencia_inicial))
	    / objetivo->frecuencia_inicial);

    // Por analogía, en este caso habría que darle más importancia al extremo derecho.
  } // else (DERECHO)

    // Coste de inicio y final de palabra:

    
  fprintf(fichero, "%d\t", absoluto(objetivo->frontera_inicial - candidato->frontera_inicial));

  fprintf(fichero, "%d\t", absoluto(objetivo->frontera_final - candidato->frontera_final));

  // Pruebas:

  if (candidato->frase != objetivo->frase)
    fprintf(fichero, "1\t");
  else
    fprintf(fichero, "0\t");

  if (candidato->posicion != objetivo->posicion)
    fprintf(fichero, "1\t");
  else
    fprintf(fichero, "0\t");

  if (candidato->acento != objetivo->acento)
    fprintf(fichero, "1\t");
  else
    fprintf(fichero, "0\t");
  /*
    if (caracteristicas_de_fonema(candidato->contexto_derecho_2, &tipo_alofono_cand,
    &sordo_sonoro_cand, &vocal_consonante_cand))
    return 1;

    if (caracteristicas_de_fonema(objetivo->contexto_derecho_2, &tipo_alofono_obj,
    &sordo_sonoro_obj, &vocal_consonante_obj))
    return 1;

    subcoste = 0;

    if (tipo_alofono_obj != tipo_alofono_cand)
    subcoste = 1;
    if (sordo_sonoro_obj != sordo_sonoro_cand)
    subcoste += 1;
    if (vocal_consonante_obj != vocal_consonante_cand)
    subcoste += 1;

    fprintf(fichero, "%d\t", subcoste);

    /*
    apunta_candidato = (char *) candidato->contexto_derecho_2;
    apunta_objetivo = (char *) objetivo->contexto_derecho_2;

    if ( (*apunta_candidato != *apunta_objetivo) ||
    (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
    fprintf(fichero, "1\t");
    else
    fprintf(fichero, "0\t");


    if (caracteristicas_de_fonema(candidato->contexto_izquierdo_2, &tipo_alofono_cand,
    &sordo_sonoro_cand, &vocal_consonante_cand))
    return 1;

    if (caracteristicas_de_fonema(objetivo->contexto_izquierdo_2, &tipo_alofono_obj,
    &sordo_sonoro_obj, &vocal_consonante_obj))
    return 1;

    subcoste = 0;

    if (tipo_alofono_obj != tipo_alofono_cand)
    subcoste = 1;
    if (sordo_sonoro_obj != sordo_sonoro_cand)
    subcoste += 1;
    if (vocal_consonante_obj != vocal_consonante_cand)
    subcoste += 1;

    fprintf(fichero, "%d\t", subcoste);

    /*
    apunta_candidato = (char *) candidato->contexto_izquierdo_2;
    apunta_objetivo = (char *) objetivo->contexto_izquierdo_2;

    if ( (*apunta_candidato != *apunta_objetivo) ||
    (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
    fprintf(fichero, "1\t");
    else
    fprintf(fichero, "0\t");
  */

  if (duracion_obj != 0)
    fprintf(fichero, "%f\t", absoluto( (duracion_obj - duracion_cand) / duracion_obj));
  else
    fprintf(fichero, "%f\t", duracion_cand);


  return 0;
    
}

/**
 * Función:   calcula_C_t                                                                
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

inline float Viterbi::calcula_C_t(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der) {

  // De momento

  float coste = 0.0;
  float coste_frecuencia;

  float duracion_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;

  float duracion_cand;
  //    float frec_izda_cand, frec_cent_cand, frec_dcha_cand;

  char *apunta_candidato;
  char *apunta_objetivo;
  //    int contador;
  unsigned char validez = 0;


#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) ) {
    candidato->validez = 0;
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).
  } // if ( (candidato->identificador ...

#endif

  duracion_cand = candidato->duracion;

  if ( (frec_cent_obj = objetivo.frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo.duracion_1;

    if ( (frec_izda_obj = objetivo.frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

#ifndef _CALCULA_INDICES
    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;
    /*
      apunta_candidato = (char *) candidato->contexto_derecho + OCTETOS_POR_FONEMA - 1;
      apunta_objetivo = (char *) objetivo.contexto_derecho + OCTETOS_POR_FONEMA - 1;

      for (contador = 0; contador < OCTETOS_POR_FONEMA;
      contador++, apunta_candidato--, apunta_objetivo--)
      if (*apunta_candidato != *apunta_objetivo) {
      coste += 0.2;
      break;
      } // if (*apunta_candidato != *apunta_objetivo);

    */
    // Lo anterior es lo general y lo correcto, pero de momento vamos a aprovechar que como mucho
    // los alófonos son de dos caracteres:

    apunta_candidato = (char *) candidato->contexto_derecho;
    apunta_objetivo = (char *) objetivo.contexto_derecho;

    if ( (*apunta_candidato != *apunta_objetivo) ||
	 (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
      coste += 0.2;


    coste_frecuencia = 1.5*absoluto((frec_izda_obj - candidato->frecuencia_inicial)) / frec_izda_obj;

    coste_frecuencia += absoluto((frec_cent_obj - candidato->frecuencia_final)) / frec_cent_obj;
    
    if ( ( (frec_izda_obj >= frec_cent_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) )  ||
	 ( (frec_izda_obj <= frec_cent_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;
      
    // Supongo que lo más lógico sería darle un peso mayor a la frecuencia del extremo izquierdo,
    // puesto que es donde menos estacionaria es la señal.

  } // if (izq_der == IZQUIERDO)

  else {

    duracion_obj = objetivo.duracion_2;

    if ( (frec_dcha_obj = objetivo.frecuencia_derecha) == 0)
      frec_dcha_obj = 100;
 
#ifndef _CALCULA_INDICES

    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;

    /*
      apunta_candidato = (char *) candidato->contexto_izquierdo + OCTETOS_POR_FONEMA - 1;
      apunta_objetivo = (char *) objetivo.contexto_izquierdo + OCTETOS_POR_FONEMA - 1;

      for (contador = 0; contador < OCTETOS_POR_FONEMA;
      contador++, apunta_candidato--, apunta_objetivo--)
      if (*apunta_candidato != *apunta_objetivo) {
      coste += 0.2;
      break;
      } // if (*apunta_candidato != *apunta_objetivo...
    */

    // Lo anterior es lo general y lo correcto, pero de momento vamos a aprovechar que como mucho
    // los alófonos son de dos caracteres:

    apunta_candidato = (char *) candidato->contexto_izquierdo;
    apunta_objetivo = (char *) objetivo.contexto_izquierdo;

    if ( (*apunta_candidato != *apunta_objetivo) ||
	 (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
      coste += 0.2;

    coste_frecuencia =  1.5*absoluto((frec_dcha_obj - candidato->frecuencia_final)) / frec_dcha_obj;
    coste_frecuencia += absoluto((frec_cent_obj - candidato->frecuencia_inicial)) / frec_cent_obj;

    if ( ( (frec_dcha_obj >= frec_cent_obj) && (candidato->frecuencia_final >= candidato->frecuencia_inicial) )  ||
	 ( (frec_dcha_obj <= frec_cent_obj) && (candidato->frecuencia_final <= candidato->frecuencia_inicial) ) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;


    // Por analogía, en este caso habría que darle más importancia al extremo derecho.
  } // else (DERECHO)

    // Coste de inicio y final de palabra:

  coste += absoluto(objetivo.frontera_inicial - candidato->frontera_inicial);
  coste += absoluto(objetivo.frontera_final - candidato->frontera_final);

  // Coste de posición prosódica:

  if (objetivo.frontera_prosodica != candidato->frontera_prosodica)
    coste += 3;
        
  // Pruebas:

  if (candidato->frase != objetivo.frase)
    coste += 0.5;
  if (candidato->posicion != objetivo.posicion)
    coste += 0.2;
  if (candidato->acento != objetivo.acento)
    coste += 0.8;
  /*
    if (candidato->palabra != NULL)
    if (strcmp(candidato->palabra, objetivo.palabra))
    coste += 0.2;
    else
    coste += 0.2;
  */

  /*
    apunta_candidato = (char *) candidato->contexto_derecho_2 + OCTETOS_POR_FONEMA - 1;
    apunta_objetivo = (char *) objetivo.contexto_derecho_2 + OCTETOS_POR_FONEMA - 1;

    for (contador = 0; contador < OCTETOS_POR_FONEMA;
    contador++, apunta_candidato--, apunta_objetivo--)
    if (*apunta_candidato != *apunta_objetivo) {
    coste += 0.1;
    break;
    } // if (*apunta_candidato...

    apunta_candidato = (char *) candidato->contexto_izquierdo_2 + OCTETOS_POR_FONEMA - 1;
    apunta_objetivo = (char *) objetivo.contexto_izquierdo_2 + OCTETOS_POR_FONEMA -1;

    for (contador = 0; contador < OCTETOS_POR_FONEMA;
    contador++, apunta_candidato--, apunta_objetivo--)
    if (*apunta_candidato != *apunta_objetivo) {
    coste += 0.1;
    break;
    } // if (*apunta_candidato != ...

  */

  // Lo anterior es lo general y lo correcto, pero de momento vamos a aprovechar que como mucho
  // los alófonos son de dos caracteres:

  apunta_candidato = (char *) candidato->contexto_derecho_2;
  apunta_objetivo = (char *) objetivo.contexto_derecho_2;

  if ( (*apunta_candidato != *apunta_objetivo) ||
       (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
    coste += 0.1;

  apunta_candidato = (char *) candidato->contexto_izquierdo_2;
  apunta_objetivo = (char *) objetivo.contexto_izquierdo_2;

  if ( (*apunta_candidato != *apunta_objetivo) ||
       (*(apunta_candidato + 1) != *(apunta_objetivo + 1) ) )
    coste += 0.1;

  coste += 2*absoluto( (duracion_obj - duracion_cand) / duracion_obj);

  return coste;

}

#ifdef _UTILIZA_CENTROIDES

/**
 * Función:   calcula_C_t_centroide                                                      
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

inline float Viterbi::calcula_C_t_centroide(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo,
					    char izq_der) {

  // De momento

  int indice_central, indice_pas1_obj, indice_fut1_obj, indice_fut2_obj, indice_pas2_obj;
  int indice_pas1_cand, indice_fut1_cand, indice_fut2_cand, indice_pas2_cand;
  int izquierdo_derecho = izq_der - '0';
  float coste = 0.0;
  float coste_frecuencia;

  float duracion_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;

  float duracion_cand;
  float frec_izda_cand, frec_cent_cand, frec_dcha_cand;

  char *apunta_candidato;
  char *apunta_objetivo;
  //    int contador;
  unsigned char validez = 0;


  indice_central = devuelve_indice_fonema(candidato->semifonema);
  indice_fut1_cand = devuelve_indice_fonema(candidato->contexto_derecho);
  indice_fut2_cand = devuelve_indice_fonema(candidato->contexto_derecho_2);
  indice_pas1_cand = devuelve_indice_fonema(candidato->contexto_izquierdo);
  indice_pas2_cand = devuelve_indice_fonema(candidato->contexto_izquierdo_2);
  indice_fut1_obj = devuelve_indice_fonema(objetivo->contexto_derecho);
  indice_fut2_obj = devuelve_indice_fonema(objetivo->contexto_derecho_2);
  indice_pas1_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo);
  indice_pas2_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo_2);

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) )
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).

#endif

  duracion_cand = candidato->duracion;

  if ( (frec_cent_obj = objetivo->frecuencia_mitad) == 0)
    frec_cent_obj = 100;


  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion_1;

    if ( (frec_izda_obj = objetivo->frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

#ifndef _CALCULA_INDICES
    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;

    if (indice_fut1_obj != indice_fut1_cand)
      coste = calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central, indice_fut1_obj, 1)].vector,
					 locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central, indice_fut1_cand, 1)].vector,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales);
    else
      coste = 0.0;
        
    
    coste_frecuencia = 6*absoluto((frec_izda_obj - candidato->frecuencia_inicial)) / frec_izda_obj;
    
    coste_frecuencia += 4*absoluto((frec_cent_obj - candidato->frecuencia_final)) / frec_cent_obj;

    if ( (frec_izda_obj >= frec_cent_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	 (frec_izda_obj <= frec_cent_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;
      
    // Supongo que lo más lógico sería darle un peso mayor a la frecuencia del extremo izquierdo,
    // puesto que es donde menos estacionaria es la señal.

  } // if (izq_der == IZQUIERDO)

  else {

    duracion_obj = objetivo->duracion_2;

    if ( (frec_dcha_obj = objetivo->frecuencia_derecha) == 0)
      frec_dcha_obj = 100;

#ifndef _CALCULA_INDICES

    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;

    if (indice_pas1_obj != indice_pas1_cand)
      coste = calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central, 0)].vector,
					 locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_cand, indice_central, 0)].vector,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales);
    else
      coste = 0.0;

    coste_frecuencia = 6*absoluto((frec_dcha_obj - candidato->frecuencia_final)) / frec_dcha_obj;
    coste_frecuencia += 4*absoluto((frec_cent_obj - candidato->frecuencia_inicial)) / frec_cent_obj;
    // Por analogía, en este caso habría que darle más importancia al extremo derecho.

    if ( (frec_dcha_obj >= frec_cent_obj) && (candidato->frecuencia_final => candidato->frecuencia_inicial) ||
	 (frec_dcha_obj <= frec_cent_obj) && (candidato->frecuencia_final <= candidato->frecuencia_inicial) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;
  } // else (DERECHO)

    // Coste de inicio y final de palabra:

  coste += absoluto(objetivo->frontera_inicial - candidato->frontera_inicial);
  coste += absoluto(objetivo->frontera_final - candidato->frontera_final);

  // Coste de final de frase:

  if (objetivo->final_frase == 1)
    if (candidato->final_frase != 1)
      coste += 3;

  // Pruebas:

  if (candidato->frase != objetivo->frase)
    coste += 0.5;
  if (candidato->posicion != objetivo->posicion)
    coste += 0.2;
  if (candidato->acento != objetivo->acento)
    coste += 0.8;

  coste += calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_fut1_obj, indice_fut2_obj, 1)].vector,
				      locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_fut1_cand, indice_fut2_cand, 1)].vector,;
				      Vector_semifonema_candidato::numero_coeficientes_cepstrales);
  coste += calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2_obj, indice_pas1_obj, 0)].vector,
				      locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2_cand, indice_pas1_cand, 0)].vector,;
				      Vector_semifonema_candidato::numero_coeficientes_cepstrales);
  coste += 8*absoluto( (duracion_obj - duracion_cand) / duracion_obj);

  return coste;

}

#endif

#ifdef _UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS

/**
 * Función:   calcula_C_t_centroide_vector_medio                                         
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

inline float Viterbi::calcula_C_t_centroide_vector_medio(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo,
							 char izq_der) {

  // De momento

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int izquierdo_derecho = izq_der - '0';
  float coste = 0.0;
  float coste_frecuencia;

  float duracion_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;

  float duracion_cand;
  float frec_izda_cand, frec_cent_cand, frec_dcha_cand;

  char *apunta_candidato;
  char *apunta_objetivo;
  unsigned char validez = 0;

  indice_central_obj = devuelve_indice_fonema(objetivo->semifonema);
  indice_fut1_obj = devuelve_indice_fonema(objetivo->contexto_derecho);
  indice_pas1_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo);

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) )
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).

#endif

  duracion_cand = candidato->duracion;

  if ( (frec_cent_obj = objetivo->frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion_1;

    if ( (frec_izda_obj = objetivo->frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

#ifndef _CALCULA_INDICES
    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;


    // Coste del anterior fonema

    coste =
      calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    // Coste del propio semifonema

    coste +=
      calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    // Coste del siguiente fonema

    coste += calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    coste_frecuencia = 6*absoluto((frec_izda_obj - candidato->frecuencia_inicial)) / frec_izda_obj;
    coste_frecuencia += 4*absoluto((frec_cent_obj - candidato->frecuencia_final)) / frec_cent_obj;

    // Supongo que lo más lógico sería darle un peso mayor a la frecuencia del extremo izquierdo,
    // puesto que es donde menos estacionaria es la señal.
    if ( (frec_izda_obj >= frec_cent_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	 (frec_izda_obj <= frec_cent_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;

  } // if (izq_der == IZQUIERDO)

  else { // DERECHO

    duracion_obj = objetivo->duracion_2;

    if ( (frec_dcha_obj = objetivo->frecuencia_derecha) == 0)
      frec_dcha_obj = 100;

#ifndef _CALCULA_INDICES

    if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > FREC_UMBRAL) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) )
      validez |= 1;
#else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

    if ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > umbral_frecuencia) )
      validez |= 1;
#endif

    candidato->validez = validez;

    // Coste del anterior fonema

    coste =
      calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales);
    
    // Coste del propio semifonema

    coste =
      calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    // Coste del siguiente fonema

    coste += calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    coste_frecuencia = 6*absoluto((frec_dcha_obj - candidato->frecuencia_final)) / frec_dcha_obj;
    coste_frecuencia += 4*absoluto((frec_cent_obj - candidato->frecuencia_inicial)) / frec_cent_obj;
    // Por analogía, en este caso habría que darle más importancia al extremo derecho.

    if ( (frec_dcha_obj >= frec_cent_obj) && (candidato->frecuencia_final >= candidato->frecuencia_inicial) ||
	 (frec_dcha_obj <= frec_cent_obj) && (candidato->frecuencia_final <= candidato->frecuencia_inicial) )
      coste += coste_frecuencia;
    else
      coste += 1.2*coste_frecuencia;
  } // else (DERECHO)

    // Coste de inicio y final de palabra:

  coste += absoluto(objetivo->frontera_inicial - candidato->frontera_inicial);
  coste += absoluto(objetivo->frontera_final - candidato->frontera_final);

  // Coste de frontera prosódica:

  if (objetivo->frontera_prosodica != candidato->frontera_prosodica)
    coste += 3;

  // Coste de final de frase:

  if (objetivo->final_frase == 1)
    if (candidato->final_frase != 1)
      coste += 3;

  // Pruebas:

  if (candidato->frase != objetivo->frase)
    coste += 0.5;
  if (candidato->posicion != objetivo->posicion)
    coste += 0.2;
  if (candidato->acento != objetivo->acento)
    coste += 0.8;

  coste += 8*absoluto( (duracion_obj - duracion_cand) / duracion_obj);

  return coste;

}

#endif


#ifdef _UTILIZA_REGRESION

/**
 * Función:   calcula_C_t_regresion                                                      
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

inline float Viterbi::calcula_C_t_regresion(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo,
					    char izq_der) {

  // De momento

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int izquierdo_derecho = izq_der - '0';
  float *pesos;
  float coste_inteligibilidad, coste_prosodia = 0.0;

  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur;

  float duracion_obj;
  float potencia_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;

  float duracion_cand, duracion_auxiliar, potencia_cand;

  //    char *apunta_candidato;
  //    char *apunta_objetivo;
  unsigned char validez = 0;

  pesos = devuelve_vector_pesos((clase_espectral) candidato->tipo_espectral);

  coste_inteligibilidad = *pesos++;

  indice_central_obj = devuelve_indice_fonema(candidato->semifonema);
  indice_fut1_obj = devuelve_indice_fonema(objetivo->contexto_derecho);
  indice_pas1_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo);

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) )  {
    candidato->validez = 0;
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).
  } // if ( (candidato->identificador ...
#endif

  duracion_cand = candidato->duracion;
  potencia_obj = objetivo->potencia;

  if ( (frec_cent_obj = objetivo->frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion_1;

    potencia_cand = candidato->potencia_final;

    if ( (frec_izda_obj = objetivo->frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

#ifndef _CALCULA_INDICES
    if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ||
					       (absoluto(frec_cent_obj - candidato->frecuencia_final) > FREC_UMBRAL) ) ) {
      // Hay que modificar la frecuencia fundamental.
      validez = 1;

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj,
							       locutor_actual->frecuencia_muestreo);

      if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
	validez = 3;
    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
	validez = 2;

    if (absoluto(potencia_obj - potencia_cand) > ENERGIA_UMBRAL)
      validez |= 4;
#else

    if ( (candidato->sonoridad == SONORO) && (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ||
	 (absoluto(frec_cent_obj - candidato->frecuencia_final) > umbral_frecuencia) ) ) {
    // Hay que modificar la frecuencia fundamental.
    validez = 1;

    // Comprobamos si la modificación de frecuencia provoca modificación de duración.

    duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj);

    if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
      validez = 3;
  } // if ( (absoluto(...
  else
    if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
      validez = 2;

  if (absoluto(potencia_obj - potencia_cand) > ENERGIA_UMBRAL)
    validez |= 4;

#endif

  candidato->validez = validez;

  // Anterior fonema:

  coste_inteligibilidad += *pesos++ *
    calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

  // Semifonema:

  coste_inteligibilidad += *pesos++ *
    calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

  // Siguiente fonema:

  coste_inteligibilidad += *pesos++ *
    calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales);


  diferencia_frec_coart = absoluto((frec_izda_obj - candidato->frecuencia_inicial));

  // Frecuencia en la zona de coarticulación:

  coste_inteligibilidad += *pesos++*(diferencia_frec_coart/frec_izda_obj);

  diferencia_frec_est = absoluto((frec_cent_obj - candidato->frecuencia_final));

  // Frecuencia en la zona estacionaria:

  coste_inteligibilidad += *pesos++*(diferencia_frec_est/frec_cent_obj);


} // if (izq_der == IZQUIERDO)

 else { // DERECHO

   duracion_obj = objetivo->duracion_2;

   potencia_cand = candidato->potencia_inicial;

   if ( (frec_dcha_obj = objetivo->frecuencia_derecha) == 0)
     frec_dcha_obj = 100;

#ifndef _CALCULA_INDICES

   if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > FREC_UMBRAL) ||
					      (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ) ) {
     //Hay que modificar la frecuencia fundamental.
     validez = 1;

     // Comprobamos si la modificación de frecuencia supone modificación de duración:

     duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj, locutor_actual->frecuencia_muestreo);

     if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
       validez = 3;

   } // if ( (absoluto(...
   else
     if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
       validez = 2;

   if (absoluto(potencia_obj - potencia_cand) > ENERGIA_UMBRAL)
     validez |= 4;

#else

   if ( (candidato->sonoridad == SONORO) && (absoluto(frec_dcha_obj - candidato->frecuencia_final) > umbral_frecuencia) ||
	(absoluto(frec_cent_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ) ) {
  //Hay que modificar la frecuencia fundamental.
  validez = 1;

  // Comprobamos si la modificación de frecuencia provoca modificación de duración.

  duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj);

  if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
    validez = 3;

} // if ( (absoluto(...
 else
   if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
     validez = 2;

if (absoluto(potencia_obj - potencia_cand) > ENERGIA_UMBRAL)
  validez |= 4;

#endif

candidato->validez = validez;


// Anterior fonema:

coste_inteligibilidad += *pesos++ *
  calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

// Semifonema:

coste_inteligibilidad += *pesos++ *
  calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

// Siguiente fonema:

coste_inteligibilidad += *pesos++ *
  calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

diferencia_frec_coart = absoluto(frec_dcha_obj - candidato->frecuencia_final);

// Frecuencia en la zona de coarticulación:

coste_inteligibilidad += *pesos++*(diferencia_frec_coart/frec_dcha_obj);

diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_inicial);

// Frecuencia en la zona estacionaria.

coste_inteligibilidad += *pesos++*(diferencia_frec_est/frec_cent_obj);


} // else (DERECHO)

// Coste de inicio y final de palabra:

// Esto ya no es correcto por el cambio de inicio de palabra a frontera...
if (objetivo->frontera_inicial != candidato->frontera_inicial)
  coste_inteligibilidad += *pesos++;
 else
   coste_inteligibilidad -= *pesos++;

if (objetivo->frontera_final != candidato->frontera_final)
  coste_inteligibilidad += *pesos++;
 else
   coste_inteligibilidad -= *pesos++;

if (candidato->frase != objetivo->frase)
  coste_inteligibilidad += *pesos++;
 else
   coste_inteligibilidad -= *pesos++;

if (candidato->posicion != objetivo->posicion)
  coste_inteligibilidad += *pesos++;
 else
   coste_inteligibilidad -= *pesos++;

if (candidato->acento != objetivo->acento)
  coste_inteligibilidad += *pesos++;
 else
   coste_inteligibilidad -= *pesos++;


diferencia_dur = absoluto(duracion_obj - duracion_cand);

coste_inteligibilidad += *pesos++*(diferencia_dur/duracion_obj);

if (izq_der == IZQUIERDO) {
  coste_inteligibilidad += *pesos++*locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].varianza;
  coste_inteligibilidad -= *pesos;
 } // if (izq_der == IZQUIERDO)
 else { // DERECHO
   coste_inteligibilidad += *pesos++*locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].varianza;
   coste_inteligibilidad += *pesos;
 } // else // DERECHO


// Coste de final de frase:

if (objetivo->final_frase == 1)
  if (candidato->final_frase != 1)
    coste_inteligibilidad += 5;

// Ahora consideramos el coste debido a la prosodia:

if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
  coste_prosodia = MAXIMO_COSTE_SUBFUNCION;
 else
   if (diferencia_frec_coart > FREC_UMBRAL)
     coste_prosodia = (diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
//            izq_der == IZQUIERDO ? coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_izda_obj:
//                                   coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_dcha_obj;

if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
  coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
 else
   if (diferencia_frec_est > FREC_UMBRAL)
     coste_prosodia += (diferencia_frec_est - FREC_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
//            coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_cent_obj;

if (izq_der == IZQUIERDO) {
  if ( !( (frec_izda_obj >= frec_cent_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_mitad) ||
	 (frec_izda_obj <= frec_cent_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_mitad) )  )
    coste_prosodia *= 1.2;

 }
 else {
   if ( !( (frec_cent_obj >= frec_dcha_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	   (frec_cent_obj <= frec_dcha_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
     coste_prosodia *= 1.2;

 }

if (objetivo->semifonema[0] != '#')
  if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_dur > DUR_UMBRAL)
      coste_prosodia += (diferencia_dur - DUR_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL);

/*
  float diferencia_potencia = absoluto(potencia_cand - potencia_obj);

  if (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION)
  coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
  if (diferencia_potencia > ENERGIA_UMBRAL)
  coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);
*/

return coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD;

}

#endif


#ifdef _UTILIZA_RED_NEURONAL

/**
 * Función:   calcula_C_t_red_neuronal                                                   
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

inline float Viterbi::calcula_C_t_red_neuronal(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo,
					       char izq_der) {

  // De momento

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int izquierdo_derecho = izq_der - '0';
  float coste_prosodia = 0.0;
  double coste_inteligibilidad;
  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur;

  float duracion_obj, duracion_auxiliar;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;
  float potencia_obj;

  float duracion_cand, potencia_cand;

  char *apunta_candidato;
  char *apunta_objetivo;
  double datos_red[NUMERO_ENTRADAS_RED_ESPECTRAL], *apunta_datos_red;
  Red_neuronal *apunta_red;
  Media_varianza *apunta_media_var;
  unsigned char validez = 0;

  apunta_datos_red = (double *) datos_red;

  devuelve_red((clase_espectral) candidato->tipo_espectral, &apunta_red, &apunta_media_var);

  indice_central_obj = devuelve_indice_fonema(candidato->semifonema);
  indice_fut1_obj = devuelve_indice_fonema(objetivo->contexto_derecho);
  indice_pas1_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo);

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) )
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).

#endif

  duracion_cand = candidato->duracion;
  potencia_obj = objetivo->potencia;

  if ( (frec_cent_obj = objetivo->frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion_1;

    potencia_cand = candidato->potencia_final;

    if ( (frec_izda_obj = objetivo->frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

#ifndef _CALCULA_INDICES
    if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ||
					       (absoluto(frec_cent_obj - candidato->frecuencia_final) > FREC_UMBRAL) ) ) {
      // Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj,
							       locutor_actual->frecuencia_muestreo);

      if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
	validez = 3;
    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
	validez = 2;

    if (absoluto(objetivo->potencia - candidato->potencia_final) > ENERGIA_UMBRAL)
      validez |= 4;

#else

    if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_izda_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ||
					       (absoluto(frec_cent_obj - candidato->frecuencia_final) > umbral_frecuencia) ) ) {
      // Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj);

      if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
	validez = 3;
    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
	validez = 2;

    if (absoluto(objetivo->potencia - candidato->potencia_final) > ENERGIA_UMBRAL)
      validez |= 4;

#endif

    candidato->validez = validez;

    diferencia_frec_coart = absoluto((frec_izda_obj - candidato->frecuencia_inicial));

    // Frecuencia en la zona de coarticulación:

    *apunta_datos_red++ = (frec_izda_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;


    diferencia_frec_est = absoluto((frec_cent_obj - candidato->frecuencia_final));

    // Frecuencia en la zona estacionaria:

    *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Inicio de palabra:

    *apunta_datos_red++ = (objetivo->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Fin de palabra:

    *apunta_datos_red++ = (objetivo->frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Frase:

    *apunta_datos_red++ = (objetivo->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Posición

    *apunta_datos_red++ = (objetivo->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Acento:

    *apunta_datos_red++ = (objetivo->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Duración:

    diferencia_dur = absoluto(duracion_obj - duracion_cand);

    *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Izquierdo:

    *apunta_datos_red++ = (0.0 - apunta_media_var->media) / apunta_media_var->desviacion;

    apunta_media_var++;

    // Anterior fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Semifonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Siguiente fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Varianza:

    *apunta_datos_red =  (locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].varianza
			  - apunta_media_var->media) / apunta_media_var->desviacion;

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    duracion_obj = objetivo->duracion_2;
    potencia_cand = candidato->potencia_inicial;
        
    if ( (frec_dcha_obj = objetivo->frecuencia_derecha) == 0)
      frec_dcha_obj = 100;

#ifndef _CALCULA_INDICES

    if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > FREC_UMBRAL) ||
					       (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > FREC_UMBRAL) ) ) {
      //Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj,
							       locutor_actual->frecuencia_muestreo);

      if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
	validez = 3;

    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
	validez = 2;

    if (absoluto(objetivo->potencia - candidato->potencia_inicial) > ENERGIA_UMBRAL)
      validez |= 4;

#else

    if ( (candidato->sonoridad == SONORO) && ( (absoluto(frec_dcha_obj - candidato->frecuencia_final) > umbral_frecuencia) ||
					       (absoluto(frec_cent_obj - candidato->frecuencia_inicial) > umbral_frecuencia) ) ) {
      //Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj);

      if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
	validez = 3;

    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
	validez = 2;

    if (absoluto(objetivo->potencia - candidato->potencia_inicial) > ENERGIA_UMBRAL)
      validez |= 4;

#endif

    candidato->validez = validez;

    diferencia_frec_coart = absoluto(frec_dcha_obj - candidato->frecuencia_final);

    // Frecuencia en la zona de coarticulación:

    *apunta_datos_red++ = (frec_dcha_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_inicial);

    // Frecuencia en la zona estacionaria:

    *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Inicio de palabra:

    *apunta_datos_red++ = (objetivo->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Fin de palabra:

    *apunta_datos_red++ = (objetivo->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->fin_palabra - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Frase:

    *apunta_datos_red++ = (objetivo->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Posición

    *apunta_datos_red++ = (objetivo->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Acento:

    *apunta_datos_red++ = (objetivo->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Duración:

    diferencia_dur = absoluto(duracion_obj - duracion_cand);

    *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Derecho:

    *apunta_datos_red++ = (1.0 - apunta_media_var->media) / apunta_media_var->desviacion;

    apunta_media_var++;

    // Anterior fonema

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Semifonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Siguiente fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Varianza:

    *apunta_datos_red = (locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].varianza
			 - apunta_media_var->media) / apunta_media_var->desviacion;

  } // else (DERECHO)

  apunta_red->simula_red(datos_red, &coste_inteligibilidad);


  // Coste de final de frase:

  if (objetivo->final_frase == 1)
    if (candidato->final_frase != 1)
      coste_inteligibilidad += 5;

  // Ahora consideramos el coste debido a la prosodia:


  if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia = MAXIMO_COSTE_SUBFUNCION;
  else                                                       
    if (diferencia_frec_coart > FREC_UMBRAL)
      coste_prosodia = (diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            izq_der == IZQUIERDO ? coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_izda_obj:
  //                                   coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_dcha_obj;

  if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_frec_est > FREC_UMBRAL)
      coste_prosodia += (diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_cent_obj;

  if (izq_der == IZQUIERDO) {
    if ( !( (frec_izda_obj >= frec_cent_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	    (frec_izda_obj <= frec_cent_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
      coste_prosodia *= 1.2;
  }
  else {
    if ( !( (frec_cent_obj >= frec_dcha_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	    (frec_cent_obj <= frec_dcha_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
      coste_prosodia *= 1.2;
  }

  if (candidato->semifonema[0] != '#')
    if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
      coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
    else
      if (diferencia_dur > DUR_UMBRAL)
	coste_prosodia += (diferencia_dur - DUR_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL);

  /*
    float diferencia_potencia = absoluto(potencia_cand - potencia_obj);

    if (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION)
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
    else
    if (diferencia_potencia > ENERGIA_UMBRAL)        	coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);

  */


  return coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD;

}

#endif


#ifdef _UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION

/**
 * \brief	Variante de la función calcula_C_t_red_neuronal, en la que se emplea un semifonema del
 * corpus como objetivo para el contexto fonético.
 * \remarks	Cuando no hay un semifonema adecuado se recurre al modelo de redes neuronales.
 * \param[in]	candidato unidad candidata.
 * \param[in]	objetivo unidad objetivo.
 * \param[in]	semifonema_contexto	unidad del corpus que se emplea como objetivo para el contexto fonético.
 * \param[in]	izq_der IZQUIERDO o DERECHO.
 * \param[out]  costes_c vector con los costes correspondientes a cada contorno
 * \param[in]   mascara_contornos cuando se calculan varios contornos en paralelo, indica cuáles de esos contornos no se han podado.
 */

float Viterbi::calcula_C_t_red_neuronal_y_modelo_coarticulacion(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *semifonema_contexto, char izq_der, vector<float> *costes_c, char *mascara_contornos) {

  // De momento

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int izquierdo_derecho = izq_der - '0';
  int iteraciones;
  int numero_contornos;
  int frontera_prosodica_objetivo;
  float coste_prosodia = 0;
  float coste_prosodia_basico;
  double coste_inteligibilidad;
  double coste_inteligibilidad_basico = 0;
  float coste_duracion = 0;

  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur;

  float duracion_obj, duracion_auxiliar;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;
  float frec_inicial_obj, frec_final_obj;

  float potencia_obj;

  float duracion_cand, potencia_cand;

  float peso_coarticulacion;

  char *apunta_candidato;
  char *apunta_objetivo;
  double datos_red[NUMERO_ENTRADAS_RED_ESPECTRAL], *apunta_datos_red;
  Red_neuronal *apunta_red;
  Media_varianza *apunta_media_var, *original_media_var;
  unsigned char validez = 0;

  //  if ( (candidato->identificador == 885084) )
  //    	 (candidato->identificador == 378033) )
  //       puts("");

  peso_coarticulacion = decide_peso_coarticulacion(objetivo, candidato);

  if (semifonema_contexto == NULL) {

    apunta_datos_red = (double *) datos_red;

    devuelve_red((clase_espectral) candidato->tipo_espectral, &apunta_red, &apunta_media_var);
    original_media_var = apunta_media_var;

    indice_central_obj = devuelve_indice_fonema(candidato->semifonema);
    indice_fut1_obj = devuelve_indice_fonema(objetivo.contexto_derecho);
    indice_pas1_obj = devuelve_indice_fonema(objetivo.contexto_izquierdo);

  }
    
#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (candidato->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (candidato->identificador >= menor_identificador) &&
	 (candidato->identificador <= mayor_identificador)) )
    return 1000; // Por poner un valor (alto, para que no se seleccione esa unidad).

#endif

  duracion_cand = candidato->duracion;
  potencia_obj = objetivo.potencia;

  if ( (frec_cent_obj = objetivo.frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo.duracion_1;
    diferencia_dur = absoluto(duracion_obj - duracion_cand);
        
    potencia_cand = candidato->potencia_final;

    if ( (frec_izda_obj = objetivo.frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

    diferencia_frec_coart = absoluto(frec_izda_obj - candidato->frecuencia_inicial);
    diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_final);

#ifndef _CALCULA_INDICES
    if ( (candidato->sonoridad == SONORO) && ( (diferencia_frec_coart > FREC_UMBRAL) ||
					       (diferencia_frec_est > FREC_UMBRAL)) ) {
      // Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj, locutor_actual->frecuencia_muestreo);

      if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
	validez = 3;
    } // if ( (absoluto(...
    else
      if (diferencia_dur > DUR_UMBRAL)
	validez = 2;

    if (absoluto(objetivo.potencia - candidato->potencia_final) > ENERGIA_UMBRAL)
      validez |= 4;

#else

    if ( (candidato->sonoridad == SONORO) && ( (diferencia_frec_coart > umbral_frecuencia) ||
					       (diferencia_frec_est > umbral_frecuencia))  ) {
      // Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_izda_obj, frec_cent_obj);

      if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
	validez = 3;
    } // if ( (absoluto(...
    else
      if (diferencia_dur > umbral_duracion)
	validez = 2;

    if (absoluto(objetivo.potencia - candidato->potencia_final) > ENERGIA_UMBRAL)
      validez |= 4;

#endif

    candidato->validez = validez;

    if (semifonema_contexto) {

      coste_inteligibilidad_basico = peso_coarticulacion*calcula_coste_coarticulacion(semifonema_contexto, candidato);
      coste_inteligibilidad_basico = ( (float) round(coste_inteligibilidad_basico*10)) / 10;

    }
    else {

      // Frecuencia en la zona de coarticulación:

      *apunta_datos_red++ = (frec_izda_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;


      // Frecuencia en la zona estacionaria:

      *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Inicio de palabra:

      *apunta_datos_red++ = (objetivo.frontera_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Fin de palabra:

      *apunta_datos_red++ = (objetivo.frontera_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frontera_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Frase:

      *apunta_datos_red++ = (objetivo.frase - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Posición

      *apunta_datos_red++ = (objetivo.posicion - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Acento:

      *apunta_datos_red++ = (objetivo.acento - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Duración:

      *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Izquierdo:

      *apunta_datos_red++ = (0.0 - apunta_media_var->media) / apunta_media_var->desviacion;

      apunta_media_var++;

      // Anterior fonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	// apunta_media_var->desviacion;
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;
      
      // Semifonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Siguiente fonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Varianza:

      *apunta_datos_red =  (locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].varianza - apunta_media_var->media) / apunta_media_var->desviacion;

    } // else // if (semifonema_contexto)

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    duracion_obj = objetivo.duracion_2;
    diferencia_dur = absoluto(duracion_obj - duracion_cand);

    if ( (frec_dcha_obj = objetivo.frecuencia_derecha) == 0)
      frec_dcha_obj = 100;

    diferencia_frec_coart = absoluto(frec_dcha_obj - candidato->frecuencia_final);
    diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_inicial);

    potencia_cand = candidato->potencia_inicial;


#ifndef _CALCULA_INDICES

    if ( (candidato->sonoridad == SONORO) && ( (diferencia_frec_coart > FREC_UMBRAL) || (diferencia_frec_est > FREC_UMBRAL) ) ) {
      //Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj, locutor_actual->frecuencia_muestreo);

      if (absoluto(duracion_auxiliar - duracion_obj) > DUR_UMBRAL)
	validez = 3;

    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > DUR_UMBRAL)
	validez = 2;

    if (absoluto(objetivo.potencia - candidato->potencia_inicial) > ENERGIA_UMBRAL)
      validez |= 4;

#else

    if ( (candidato->sonoridad == SONORO) && ( (diferencia_frec_coart > umbral_frecuencia) || (diferencia_frec_est > umbral_frecuencia) ) ) {
      //Hay que modificar la frecuencia fundamental.
      validez = 1;

      // Comprobamos si la modificación de frecuencia supone modificación de duración:

      duracion_auxiliar = candidato->calcula_duracion_efectiva(frec_cent_obj, frec_dcha_obj);

      if (absoluto(duracion_auxiliar - duracion_obj) > umbral_duracion)
	validez = 3;

    } // if ( (absoluto(...
    else
      if (absoluto(duracion_cand - duracion_obj) > umbral_duracion)
	validez = 2;

    if (absoluto(objetivo.potencia - candidato->potencia_inicial) > ENERGIA_UMBRAL)
      validez |= 4;

#endif

    candidato->validez = validez;

    if (semifonema_contexto) {

      coste_inteligibilidad_basico = peso_coarticulacion*calcula_coste_coarticulacion(semifonema_contexto, candidato);

      coste_inteligibilidad_basico = ( (float) round(coste_inteligibilidad_basico*10)) / 10;

    }
    else {

      // Frecuencia en la zona de coarticulación:

      *apunta_datos_red++ = (frec_dcha_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Frecuencia en la zona estacionaria:

      *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Inicio de palabra:

      *apunta_datos_red++ = (objetivo.frontera_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Fin de palabra:

      *apunta_datos_red++ = (objetivo.frontera_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frontera_final - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Frase:

      *apunta_datos_red++ = (objetivo.frase - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Posición

      *apunta_datos_red++ = (objetivo.posicion - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Acento:

      *apunta_datos_red++ = (objetivo.acento - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Duración:

      *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Derecho:

      *apunta_datos_red++ = (1.0 - apunta_media_var->media) / apunta_media_var->desviacion;

      apunta_media_var++;

      // Anterior fonema:

      // Semifonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Semifonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Siguiente fonema:

      *apunta_datos_red++ =
	// (calcula_distancia_euclidea(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
	(calcula_distancia_euclidea_optimizada(locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final) - apunta_media_var->media) /
	apunta_media_var->desviacion;

      apunta_media_var++;

      // Varianza:

      *apunta_datos_red = (locutor_actual->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].varianza
			   - apunta_media_var->media) / apunta_media_var->desviacion;

    } // else // if (semifonema_contexto)

  } // else (DERECHO)


  if (izq_der == IZQUIERDO) {
    if (is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) !=
	is_silence_or_voiceless_plosive(candidato->contexto_derecho_2) )
      coste_inteligibilidad_basico += 2;
    if (is_silence_or_voiceless_plosive(objetivo.contexto_derecho) !=
	is_silence_or_voiceless_plosive(candidato->contexto_derecho) )
      coste_inteligibilidad_basico += 4;
  }
  else {
    if (is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) !=
	is_silence_or_voiceless_plosive(candidato->contexto_derecho_2))
      coste_inteligibilidad_basico += 3;
    if (is_silence_or_voiceless_plosive(objetivo.contexto_izquierdo) !=
	is_silence_or_voiceless_plosive(candidato->contexto_izquierdo) )
      coste_inteligibilidad_basico += 6;
  }

  if (peso_coarticulacion) {

    if (objetivo.frontera_inicial != candidato->frontera_inicial)
      coste_inteligibilidad_basico += peso_coarticulacion/3*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
    
    if (objetivo.frontera_final != candidato->frontera_final)
      coste_inteligibilidad_basico += peso_coarticulacion*abs(objetivo.frontera_final - candidato->frontera_final);

  }
  else {

    if (objetivo.frontera_inicial != candidato->frontera_inicial)
      coste_inteligibilidad_basico += 0.5*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
    
    if (objetivo.frontera_final != candidato->frontera_final)
      coste_inteligibilidad_basico += abs(objetivo.frontera_final - candidato->frontera_final);

  }

  if (candidato->acento != objetivo.acento)
    coste_inteligibilidad_basico += 6;

  if ( (objetivo.posicion == POSICION_FINAL) && (candidato->posicion != POSICION_FINAL) )
    coste_inteligibilidad_basico += 3;

  if ( (objetivo.posicion != POSICION_FINAL) && (candidato->posicion == POSICION_FINAL) )
    coste_inteligibilidad_basico += 3;

#ifdef _SOLO_COSTE_INTELIGIBILIDAD
  return coste_intelibilidad_basico;
#endif

  if (candidato->semifonema[0] != '#') {

    float porcentaje;
    if (izq_der == IZQUIERDO)
      porcentaje = maximo(objetivo.duracion_1, candidato->duracion) /
	minimo(objetivo.duracion_1, candidato->duracion);
    else
      porcentaje = maximo(objetivo.duracion_2, candidato->duracion) /
	minimo(objetivo.duracion_2, candidato->duracion);

    if (diferencia_dur < DUR_UMBRAL) {
      if (porcentaje > 3)
	coste_duracion = porcentaje;
      else
	if (porcentaje < 0.5)
	  coste_duracion = 1/porcentaje;
    }
    else {
      if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
	coste_duracion = 1.5*MAXIMO_COSTE_SUBFUNCION;
      else
	coste_duracion = (diferencia_dur - DUR_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL); 

      if (porcentaje > 3)
	coste_duracion *= porcentaje;
      else
	if (porcentaje < 0.5)
	  coste_duracion *= (1/porcentaje);
    }

  }

  coste_prosodia += 1.5*coste_duracion;

  float diferencia_potencia = absoluto(potencia_cand - potencia_obj);
  
  if (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION)
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_potencia > ENERGIA_UMBRAL)        	
      coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);

  if (mascara_contornos == NULL)
    iteraciones = 1;
  else {
    iteraciones = objetivo.fronteras_prosodicas.size();
    costes_c->clear();
  }

  //  coste_inteligibilidad_basico; // = coste_inteligibilidad;
  coste_prosodia_basico = coste_prosodia;

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---------> (%f\t%f)\n", coste_inteligibilidad_basico, coste_prosodia_basico);
#endif
  
  for (numero_contornos = 0; numero_contornos < iteraciones; numero_contornos++) {

    if (semifonema_contexto == NULL) {
      if (mascara_contornos != NULL) {
	if (izq_der == IZQUIERDO) {
	  datos_red[0] = (objetivo.frecuencias_inicial[numero_contornos] - original_media_var[0].media) /
	    original_media_var[0].desviacion;
	  datos_red[2] = (objetivo.frecuencias_mitad[numero_contornos] - original_media_var[2].media) /
	    original_media_var[2].desviacion;
	} // if (izq_der == IZQUIERDO)
	else {
	  datos_red[0] = (objetivo.frecuencias_final[numero_contornos] - original_media_var[0].media) /
	    original_media_var[0].desviacion;
	  datos_red[2] = (objetivo.frecuencias_mitad[numero_contornos] - original_media_var[2].media) /
	    original_media_var[2].desviacion;
	}
      }

      apunta_red->simula_red(datos_red, &coste_inteligibilidad);
      coste_inteligibilidad *= peso_coarticulacion;
      coste_inteligibilidad = ( (float) round(coste_inteligibilidad*10)) / 10;

      // Multiplicar aquí por el peso_coarticulacion??

      coste_inteligibilidad += coste_inteligibilidad_basico;

    } // if (semifonema_contexto == NULL)
    else
      coste_inteligibilidad = coste_inteligibilidad_basico;

    coste_prosodia = coste_prosodia_basico;
    
    
    if (candidato->sonoridad == SONORO) {
      // Coste de posición prosódica:
    
      if (mascara_contornos == NULL)
	frontera_prosodica_objetivo = objetivo.frontera_prosodica;
      else
	frontera_prosodica_objetivo = objetivo.fronteras_prosodicas[numero_contornos];
      
      if ( (frontera_prosodica_objetivo == UNIDAD_FIN_FRASE) || (candidato->frontera_prosodica == UNIDAD_FIN_FRASE) ) {
	if (frontera_prosodica_objetivo != candidato->frontera_prosodica)
	  coste_inteligibilidad += 6;
      }
      else
	if (candidato->frontera_prosodica != frontera_prosodica_objetivo)
	coste_inteligibilidad += 0.1;
    }

    if (candidato->sonoridad == SORDO) {
      coste_prosodia *= 1.5;
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "---------> (%d) => %f\t%f\n", numero_contornos, coste_inteligibilidad, coste_prosodia);
#endif
      costes_c->push_back(coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD);
      
    } // if (candidato->sonoridad == SORDO)
    else {
      
      if (mascara_contornos) {
	if (mascara_contornos[numero_contornos] == 0) {
#ifdef _DEPURA_VITERBI
	  fprintf(fichero_viterbi, "---------> (%d) => podado\n", numero_contornos);
#endif
	  costes_c->push_back(CAMINO_PODADO); // Este valor no debería influir más adelante
	  continue;
	}

	if (izq_der == IZQUIERDO) {
	  frec_inicial_obj = objetivo.frecuencias_inicial[numero_contornos];
	  frec_final_obj = objetivo.frecuencias_mitad[numero_contornos];
	}
	else {
	  frec_inicial_obj = objetivo.frecuencias_mitad[numero_contornos];
	  frec_final_obj = objetivo.frecuencias_final[numero_contornos];
	}
      } // if (mascara_contornos)
      else {
	if (izq_der == IZQUIERDO) {
	  frec_inicial_obj = objetivo.frecuencia_izquierda;
	  frec_final_obj = objetivo.frecuencia_mitad;
	}
	else {
	  frec_inicial_obj = objetivo.frecuencia_mitad;
	  frec_final_obj = objetivo.frecuencia_derecha;
	}
      } //
      
      if (izq_der == IZQUIERDO) {
	diferencia_frec_coart = absoluto(frec_inicial_obj - candidato->frecuencia_inicial);
	diferencia_frec_est = absoluto(frec_final_obj - candidato->frecuencia_final);
      } // if (izq_der == IZQUIERDO)
      else {
	diferencia_frec_est = absoluto(frec_inicial_obj - candidato->frecuencia_inicial);
	diferencia_frec_coart = absoluto(frec_final_obj - candidato->frecuencia_final);
      }
	
      if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
	coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
      else
	if (diferencia_frec_coart > FREC_UMBRAL)
	  coste_prosodia += 3*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
      
      if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
	coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
      else
	if (diferencia_frec_est > FREC_UMBRAL)
	  coste_prosodia += 3*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
      
      if ( !( (frec_inicial_obj >=  frec_final_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
	      (frec_inicial_obj <= frec_final_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
	coste_prosodia *= 1.3;

      if (diferencia_frec_coart > 0.4*candidato->frecuencia_inicial)
	coste_prosodia *= 4;
      if (diferencia_frec_est > 0.4*candidato->frecuencia_final)
	coste_prosodia *= 4;
      
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "---------> (%d) => %f\t%f\t\t%f(%f <-> %f)\t(%f <-> %f)\n", numero_contornos, coste_inteligibilidad, coste_prosodia, coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD, frec_inicial_obj, candidato->frecuencia_inicial, frec_final_obj, candidato->frecuencia_final);
#endif
      costes_c->push_back(coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD);
      
    } // if SONORO
      
  }  // for (int numero_contornos = 0; ...
  
  return coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD;
  
}

#endif

#ifdef _UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES

/**
 * \brief	Variante de la función calcula_C_t_red_neuronal_y_modelo_coarticulacion, en la que no se emplean medidas espectrales, sino un peso de coarticulacion calculado a partir de la diferencia de contextos fonéticos.
 * \param[in]	candidato unidad candidata.
 * \param[in]	objetivo unidad objetivo.
 * \param[in]	izq_der IZQUIERDO o DERECHO.
 * \param[out]  costes_c vector con los costes correspondientes a cada contorno
 * \param[in]   mascara_contornos cuando se calculan varios contornos en paralelo, indica cuáles de esos contornos no se han podado.
 */

float Viterbi::calcula_C_t_peso_coarticulacion_texto(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der, vector<float> *costes_c, char *mascara_contornos) {

  // De momento

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int izquierdo_derecho = izq_der - '0';
  int iteraciones;
  int numero_contornos;
  int frontera_prosodica_objetivo;
  float coste_prosodia = 0;
  float coste_prosodia_basico;
  double coste_inteligibilidad;
  double coste_inteligibilidad_basico = 0;
  float coste_duracion = 0;

  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur;
  float coef_frec_est, coef_frec_coart;

  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;
  float frec_inicial_obj, frec_final_obj;

  float potencia_obj;

  float duracion_cand, potencia_cand;

  float peso_coarticulacion;

  candidato->validez = 0;

  // if (candidato->identificador == 890004)
  //   puts("");

  peso_coarticulacion = decide_peso_coarticulacion(objetivo, candidato);

  coste_inteligibilidad_basico = peso_coarticulacion;

  duracion_cand = candidato->duracion;
  potencia_obj = objetivo.potencia;

  if (izq_der == IZQUIERDO) {

    diferencia_dur = absoluto(objetivo.duracion_1 - duracion_cand);
    potencia_cand = candidato->potencia_final;

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    diferencia_dur = absoluto(objetivo.duracion_2 - duracion_cand);
    potencia_cand = candidato->potencia_inicial;

  } // else (DERECHO)

  coste_inteligibilidad_basico += decide_peso_fronteras(objetivo, candidato);
  
  coste_inteligibilidad_basico += decide_peso_posicion_y_tipo_frase(objetivo, candidato);

  if (candidato->acento != objetivo.acento)
    coste_inteligibilidad_basico += 100;

  if (candidato->semifonema[0] != '#') {

    float porcentaje;
    if (izq_der == IZQUIERDO)
      porcentaje = objetivo.duracion_1 / candidato->duracion;
    else
      porcentaje = objetivo.duracion_2 / candidato->duracion;

    if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
      coste_duracion = 1.5*MAXIMO_COSTE_SUBFUNCION;
    else if (diferencia_dur > DUR_UMBRAL)
      coste_duracion = (diferencia_dur - DUR_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL); 
    else
      coste_duracion = 0;

    if (porcentaje > DUR_DIF_COEF_1)
      coste_duracion += 30*(porcentaje - DUR_DIF_COEF_3);
    else if (porcentaje > DUR_DIF_COEF_2)
      coste_duracion += 15*(porcentaje - DUR_DIF_COEF_3);
    else if (porcentaje > DUR_DIF_COEF_3)
      coste_duracion += porcentaje - DUR_DIF_COEF_3;
    else if (porcentaje < DUR_DIF_COEF_MENOR_3)
      coste_duracion += 50*(DUR_DIF_COEF_MENOR_1 - porcentaje);
    else if (porcentaje < DUR_DIF_COEF_MENOR_2)
      coste_duracion += 30*(DUR_DIF_COEF_MENOR_1 - porcentaje);
    else if (porcentaje < DUR_DIF_COEF_MENOR_1)
      coste_duracion += 20*(DUR_DIF_COEF_MENOR_1 - porcentaje);
    
  }

  if (candidato->sonoridad == SONORO)
    coste_prosodia += 1.5*coste_duracion;
  else
    coste_prosodia += 4*coste_duracion;

  if (objetivo.frontera_prosodica != UNIDAD_FIN_FRASE) {
    // El modelo de energía falla mucho, así que es mejor forzar con las otras funciones de coste unidades que estén al final de la oración, y pasar de él en estos casos.
    if ( (candidato->tipo_espectral != OCLU_SOR_ESP) && (candidato->tipo_espectral != SILENCIO_ESP) && (candidato->tipo_espectral != FRIC_SOR_ESP) ) {
      float diferencia_potencia = absoluto(potencia_cand - potencia_obj);
      
      if ( (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION) ||
	   (maximo(potencia_cand, potencia_obj) / minimo(potencia_cand, potencia_obj) > 2))
	coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
      else
	if (diferencia_potencia > ENERGIA_UMBRAL)        	
	  coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);
      
    } // if ( (candidato->tipo_espectral != ...

  } // if (objetivo.frontera_prosodica...

  if (mascara_contornos == NULL)
    iteraciones = 1;
  else {
    iteraciones = objetivo.fronteras_prosodicas.size();
    costes_c->clear();
  }

  //  coste_inteligibilidad_basico; // = coste_inteligibilidad;
  coste_prosodia_basico = coste_prosodia;

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---------> (%f\t%f\t%f)\n", coste_inteligibilidad_basico, coste_prosodia_basico, coste_duracion);
#endif
  
  for (numero_contornos = 0; numero_contornos < iteraciones; numero_contornos++) {

    coste_inteligibilidad = coste_inteligibilidad_basico;

    coste_prosodia = coste_prosodia_basico;

    if (mascara_contornos == NULL)
      frontera_prosodica_objetivo = objetivo.frontera_prosodica;
    else
      frontera_prosodica_objetivo = objetivo.fronteras_prosodicas[numero_contornos];

    // Coste de posición prosódica:
    coste_inteligibilidad += decide_peso_frontera_prosodica(frontera_prosodica_objetivo, candidato);

    if (candidato->sonoridad == SORDO) {
      coste_prosodia *= 1.5;
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "---------> (%d) => %f\t%f\n", numero_contornos, coste_inteligibilidad, coste_prosodia);
#endif
      costes_c->push_back(coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD);
      
    } // if (candidato->sonoridad == SORDO)
    else {
      
      if (mascara_contornos) {
	if (mascara_contornos[numero_contornos] == 0) {
#ifdef _DEPURA_VITERBI
	  fprintf(fichero_viterbi, "---------> (%d) => podado\n", numero_contornos);
#endif
	  costes_c->push_back(CAMINO_PODADO); // Este valor no debería influir más adelante
	  continue;
	}

	if (izq_der == IZQUIERDO) {
	  frec_inicial_obj = objetivo.frecuencias_inicial[numero_contornos];
	  frec_final_obj = objetivo.frecuencias_mitad[numero_contornos];
	}
	else {
	  frec_inicial_obj = objetivo.frecuencias_mitad[numero_contornos];
	  frec_final_obj = objetivo.frecuencias_final[numero_contornos];
	}
      } // if (mascara_contornos)
      else {
	if (izq_der == IZQUIERDO) {
	  frec_inicial_obj = objetivo.frecuencia_izquierda;
	  frec_final_obj = objetivo.frecuencia_mitad;
	}
	else {
	  frec_inicial_obj = objetivo.frecuencia_mitad;
	  frec_final_obj = objetivo.frecuencia_derecha;
	}
      } //
      
      if (izq_der == IZQUIERDO) {
	diferencia_frec_coart = frec_inicial_obj - candidato->frecuencia_inicial;
	if (diferencia_frec_coart >= 0)
	  coef_frec_coart = frec_inicial_obj / candidato->frecuencia_inicial;
	else
	  coef_frec_coart = candidato->frecuencia_inicial / frec_inicial_obj;
  
	diferencia_frec_est = frec_final_obj - candidato->frecuencia_final;
	if (diferencia_frec_est >= 0)
	  coef_frec_est = frec_final_obj / candidato->frecuencia_final;
	else
	  coef_frec_est = candidato->frecuencia_final / frec_final_obj;
      } // if (izq_der == IZQUIERDO)
      else {
	diferencia_frec_est = frec_inicial_obj - candidato->frecuencia_inicial;
	
	if (diferencia_frec_est >= 0)
	  coef_frec_est = frec_inicial_obj / candidato->frecuencia_inicial;
	else
	  coef_frec_est = candidato->frecuencia_inicial / frec_inicial_obj;

	diferencia_frec_coart = frec_final_obj - candidato->frecuencia_final;

	if (diferencia_frec_coart >= 0)
	  coef_frec_coart = frec_final_obj / candidato->frecuencia_final;
	else
	  coef_frec_coart = candidato->frecuencia_final / frec_final_obj;
      }

#ifdef _COSTE_F0_RELATIVO

      if (coef_frec_coart > FREC_DIF_COEF_4)
	coste_prosodia += 500*(coef_frec_coart - FREC_DIF_COEF);
      else
	if (coef_frec_coart > FREC_DIF_COEF_3)
	  coste_prosodia += 200*(coef_frec_coart - FREC_DIF_COEF);
	else
	  if (coef_frec_coart > FREC_DIF_COEF_2)
	    coste_prosodia += 100*(coef_frec_coart - FREC_DIF_COEF);
 	  else
	    if (coef_frec_coart > FREC_DIF_COEF_1)
	      coste_prosodia += 20*(coef_frec_coart - FREC_DIF_COEF);
	    else
	      if (coef_frec_coart > FREC_DIF_COEF)
		coste_prosodia += 10*(coef_frec_coart - FREC_DIF_COEF);
#else

      diferencia_frec_coart = absoluto(diferencia_frec_coart);

#ifndef _MODIFICACION_F0_SOLO_ZONA_ESTACIONARIA

      if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
      	coste_prosodia += 30*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION*(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
      else
	if (diferencia_frec_coart > FREC_UMBRAL_4)
	  coste_prosodia += 20*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
	else
	  if (diferencia_frec_coart > FREC_UMBRAL_3)
	    coste_prosodia += 10*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //15
	  else
	    if (diferencia_frec_coart > FREC_UMBRAL_2)
	      coste_prosodia += 7*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //10
	    else
	      if (diferencia_frec_coart > FREC_UMBRAL_1)
		coste_prosodia += 5*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //7
	      else
		if (diferencia_frec_coart > FREC_UMBRAL)
		  coste_prosodia += 2*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); // 4

#else
      if (objetivo.frontera_prosodica == UNIDAD_FIN_FRASE) {
      if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
      	coste_prosodia += 30*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION*(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
      else
	if (diferencia_frec_coart > FREC_UMBRAL_4)
	  coste_prosodia += 20*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
	else
	  if (diferencia_frec_coart > FREC_UMBRAL_3)
	    coste_prosodia += 10*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //15
	  else
	    if (diferencia_frec_coart > FREC_UMBRAL_2)
	      coste_prosodia += 7*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //10
	    else
	      if (diferencia_frec_coart > FREC_UMBRAL_1)
		coste_prosodia += 5*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //7
	      else
		if (diferencia_frec_coart > FREC_UMBRAL)
		  coste_prosodia += 2*(diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); // 4

      }

#endif

#endif
 
#ifdef _COSTE_F0_RELATIVO

      if (coef_frec_est > FREC_DIF_COEF_4)
	coste_prosodia += 500*(coef_frec_est - FREC_DIF_COEF);
      else
	if (coef_frec_est > FREC_DIF_COEF_3)
	  coste_prosodia += 200*(coef_frec_est - FREC_DIF_COEF);
	else
	  if (coef_frec_est > FREC_DIF_COEF_2)
	    coste_prosodia += 100*(coef_frec_est - FREC_DIF_COEF);
	  else
	    if (coef_frec_est > FREC_DIF_COEF_1)
	      coste_prosodia += 20*(coef_frec_est - FREC_DIF_COEF);
	    else
	      if (coef_frec_est > FREC_DIF_COEF)
		coste_prosodia += 10*(coef_frec_est - FREC_DIF_COEF);
#else
      diferencia_frec_est = absoluto(diferencia_frec_est);

      if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
      	coste_prosodia += 30*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION*(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
      else
	if (diferencia_frec_est > FREC_UMBRAL_4)
	  coste_prosodia += 20*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
	else
	  if (diferencia_frec_est > FREC_UMBRAL_3)
	    coste_prosodia += 10*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //15
	  else
	    if (diferencia_frec_est > FREC_UMBRAL_2)
	      coste_prosodia += 7*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //10
	    else
	      if (diferencia_frec_est > FREC_UMBRAL_1)
		coste_prosodia += 5*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //7
	      else
		if (diferencia_frec_est > FREC_UMBRAL)
		  coste_prosodia += 2*(diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL); //4

      if (izq_der == IZQUIERDO) {
	if (frec_inicial_obj <= FREC_MINIMA_DISTORSION) {
	  if (coef_frec_coart > FREC_DIF_COEF_2)
	    coste_prosodia += 30;
	}
	if (frec_final_obj <= FREC_MINIMA_DISTORSION) {
	  if (coef_frec_est > FREC_DIF_COEF_2)
	    coste_prosodia += 25;
	}
      }
      else {
	if (frec_inicial_obj <= FREC_MINIMA_DISTORSION) {
	  if (coef_frec_est > FREC_DIF_COEF_2)
	    coste_prosodia += 25;
	}
	if (frec_final_obj <= FREC_MINIMA_DISTORSION) {
	  if (coef_frec_coart > FREC_DIF_COEF_2)
	    coste_prosodia += 30;
	}
      }


#endif
      
      if (coste_prosodia)  {
	if ( !( (frec_inicial_obj >=  frec_final_obj) && (candidato->frecuencia_inicial >= candidato->frecuencia_final) ||
		(frec_inicial_obj <= frec_final_obj) && (candidato->frecuencia_inicial <= candidato->frecuencia_final) ) )
	  //	coste_prosodia *= 1.3;
	  coste_prosodia += 2;
      }

#ifndef _COSTE_F0_RELATIVO

      if (izq_der == IZQUIERDO) {
      	if (diferencia_frec_coart > 0.4*candidato->frecuencia_inicial)
      	  coste_prosodia += 25; // 5
      	else if (diferencia_frec_coart > 0.2*candidato->frecuencia_inicial)
      	  coste_prosodia += 15; // 2
	
      	if (diferencia_frec_est > 0.4*candidato->frecuencia_final)
      	  coste_prosodia += 20; // 5
      	else if (diferencia_frec_est > 0.2*candidato->frecuencia_final)
      	  coste_prosodia += 10; // 2
      	  //	  coste_prosodia *= 2;
      }
      else {
      	if (diferencia_frec_coart > 0.4*candidato->frecuencia_final)
      	  coste_prosodia += 25; // 5
      	else if (diferencia_frec_coart > 0.2*candidato->frecuencia_final)
      	  coste_prosodia += 15; // 2
	
      	if (diferencia_frec_est > 0.4*candidato->frecuencia_inicial)
      	  coste_prosodia += 20; // 5
      	else if (diferencia_frec_est > 0.2*candidato->frecuencia_inicial)
      	  coste_prosodia += 10; // 2
      }
#endif
   
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "---------> (%d) => %f\t%f\t(%f <-> %f)\t(%f <-> %f)\n", numero_contornos, coste_inteligibilidad, coste_prosodia, frec_inicial_obj, candidato->frecuencia_inicial, frec_final_obj, candidato->frecuencia_final);
#endif
      costes_c->push_back(coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD);
      
    } // if SONORO
      
  }  // for (int numero_contornos = 0; ...
  
  return coste_prosodia*COEFICIENTE_PROSODIA + coste_inteligibilidad*COEFICIENTE_INTELIGIBILIDAD;
  
}

#endif


/**
 * Función:   calcula_C_c
 * \remarks Entrada:
 *			- unidad1: unidad del paso anterior.
 *          - unidad2: unidad del paso actual.
 * \remarks Objetivo:  Calcula el coste de concatenación entre dos unidades.
 */

float Viterbi::calcula_C_c(Vector_semifonema_candidato *unidad1, Vector_semifonema_candidato *unidad2) {

  float frec_union2, frec_union1, dif_frecuencia;
  float pot_union1, pot_union2, dif_potencia;

  char clase_1, clase_2;

  int cont1, cont2;

  float coste, coste_cepstral = 0; //, coste_fase;

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (unidad1->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (unidad1->identificador >= menor_identificador) &&
	 (unidad1->identificador <= mayor_identificador))  )
    return 1000;
  if ( (unidad2->identificador > MAXIMO_INDICE_FREIRE_800) ||
       ( (unidad2->identificador >= menor_identificador) &&
	 (unidad2->identificador <= mayor_identificador)) )
    return 1000;
#endif

  cont1 = unidad1->identificador;
  cont2 = unidad2->identificador;

  //if ( (cont1 == 11761515) && (cont2 == 8473506) )
  //  puts("");
        
  if ( (cont1 == cont2) || (cont2 == cont1 + 1) ) {
    return 0.0; // Las unidades son consecutivas. Su coste de concatenación es nulo.

    //        return 1000; // Las unidades son consecutivas. Su coste de concatenación es nulo.
  }
  //    else
  //    	return 1000;

  // Culio

  if ( (unidad1->semifonema[0] == '#') && 
       (unidad2->semifonema[0] == '#') )
    return 0;
  
  if ( (unidad1->tipo_espectral == FRIC_SOR_ESP) &&
       (unidad2->tipo_espectral == FRIC_SOR_ESP) )
    return 0;
  
  if ( (unidad1->tipo_espectral == OCLU_SOR_ESP) &&
       (unidad2->tipo_espectral == OCLU_SOR_ESP) )
    return 0;

  if (unidad1->izquierdo_derecho == DERECHO)  {// Parte menos estable
    if (unidad1->tipo_espectral == SILENCIO_ESP) {
      if (unidad2->tipo_espectral == OCLU_SOR_ESP)
	coste = 0.0;
      else if (unidad2->tipo_espectral == OCLU_SON_ESP)
	coste = 1.0;
      else if (unidad2->tipo_espectral == FRIC_SOR_ESP)
	coste = 1.0;
      else
	coste = 10.0;
    }
    else
      coste = 10.0; // 5.0 // 10.0
  //		coste = 1000;
  }
  else
    coste = 0.0; // 0

#ifdef _CONSIDERA_SEMIVOCALES
  if (unidad1->izquierdo_derecho == DERECHO) {

    if ( (unidad1->tipo_espectral == SEMI_VOCAL_ESP) ||
	 (unidad2->tipo_espectral == SEMI_VOCAL_ESP) ) {
      
      if ( (unidad1->tipo_espectral == VOC_AB_ESP) ||
	   (unidad1->tipo_espectral == VOC_MED_ESP) ||
	   (unidad1->tipo_espectral == VOC_CER_ESP) ||
	   (unidad2->tipo_espectral == VOC_AB_ESP) ||
	   (unidad2->tipo_espectral == VOC_MED_ESP) ||
	   (unidad2->tipo_espectral == VOC_CER_ESP) ) { 
	if ( (unidad1->frontera_final == SIN_FRONTERA) || (unidad2->frontera_inicial == SIN_FRONTERA) ) 
	  coste += 100.0; // 10.0
	else
	  coste += 6.0; // 6.0
      }
      else
	coste += 2.0;
    }
    else {
      if ( (unidad2->tipo_espectral == VOC_AB_ESP) ||
	   (unidad2->tipo_espectral == VOC_MED_ESP) ||
	   (unidad2->tipo_espectral == VOC_CER_ESP) ||
	   (unidad2->tipo_espectral == SEMI_VOCAL_ESP) )
	coste += 2.0;
    }
    
  } // if (unidad1->izquierdo_derecho == DERECHO)
  else { // Parte más estable

    if (unidad1->tipo_espectral == SEMI_VOCAL_ESP) 
      coste += 4;
    else
      if ( (unidad1->tipo_espectral == VOC_AB_ESP) ||
	   (unidad1->tipo_espectral == VOC_MED_ESP) ||
	   (unidad1->tipo_espectral == VOC_CER_ESP) )
	coste += 2.0;
    
  }
#else // Sin semivocales
  if (unidad1->izquierdo_derecho == DERECHO) {

    if ( (unidad1->tipo_espectral == VOC_AB_ESP) ||
	 (unidad1->tipo_espectral == VOC_MED_ESP) ||
	 (unidad1->tipo_espectral == VOC_CER_ESP) ) {
      
      if ( (unidad2->tipo_espectral == VOC_AB_ESP) ||
	   (unidad2->tipo_espectral == VOC_MED_ESP) ||
	   (unidad2->tipo_espectral == VOC_CER_ESP) ) {
	if ( (unidad1->frontera_final == SIN_FRONTERA) || (unidad2->frontera_inicial == SIN_FRONTERA) ) 
	  coste += 100.0; // 10.0
	else
	  coste += 6.0; // 6.0
      }
      else
	coste += 2.0;
    }
    else {
      if ( (unidad2->tipo_espectral == VOC_AB_ESP) ||
	   (unidad2->tipo_espectral == VOC_MED_ESP) ||
	   (unidad2->tipo_espectral == VOC_CER_ESP) )
	coste += 2.0;
    }
    
  } // if (unidad1->izquierdo_derecho == DERECHO)
  else { // Parte más estable
    
    if ( (unidad1->tipo_espectral == VOC_AB_ESP) ||
	 (unidad1->tipo_espectral == VOC_MED_ESP) ||
	 (unidad1->tipo_espectral == VOC_CER_ESP) ) 
      coste += 2.0;

  }

#endif

  if (unidad1->marca_inventada == 1)
    coste += 10;

  // Si la unión se realiza por la parte de la coarticulación, damos más importancia al peso:

  if ( (unidad1->sonoridad == SONORO) && (unidad2->sonoridad == SONORO) ) {
    frec_union2 = unidad2->frecuencia_inicial;
    frec_union1 = unidad1->frecuencia_final;
    dif_frecuencia = absoluto(frec_union2 - frec_union1);

    if (dif_frecuencia > FREC_DIF_UMBRAL)
      if (dif_frecuencia > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION)
	coste += 10*dif_frecuencia / maximo(frec_union2, frec_union1);
      else
	coste += 2*dif_frecuencia / maximo(frec_union2, frec_union1);

  } // if ( (unidad1->sonoridad == SONORO)

  float *cepstrum1;
  float *cepstrum2;

  if (unidad1->izquierdo_derecho == IZQUIERDO) {
    cepstrum1 = (float *) unidad1->cepstrum_mitad;
    cepstrum2 = (float *) unidad2->cepstrum_mitad ;
  }
  else {
    cepstrum1 = (float *) unidad1->cepstrum_final;
    cepstrum2 = (float *) unidad2->cepstrum_inicial;
  }

  //        float diferencia;

  // coste_cepstral = calcula_distancia_euclidea(cepstrum1, cepstrum2, Vector_semifonema_candidato::numero_coeficientes_cepstrales) /
  //   Vector_semifonema_candidato::numero_coeficientes_cepstrales;
  coste_cepstral = calcula_distancia_euclidea_optimizada(cepstrum1, cepstrum2) /
    Vector_semifonema_candidato::numero_coeficientes_cepstrales;

#ifndef _CONSIDERA_SEMIVOCALES
  if (unidad1->tipo_espectral == SEMI_VOCAL_ESP)
    clase_1 = VOC_CER_ESP;
  else
    clase_1 = unidad1->tipo_espectral;
#endif

  if (unidad1->izquierdo_derecho == DERECHO) { //(izq_der == IZQUIERDO) {

#ifndef _CONSIDERA_SEMIVOCALES
    if (unidad2->tipo_espectral == SEMI_VOCAL_ESP)
      clase_2 = VOC_CER_ESP;
    else
      clase_2 = unidad2->tipo_espectral;
#else
      clase_2 = unidad2->tipo_espectral;
#endif

    //    coste += 10*locutor_actual->indices_continuidad_coart[unidad1->tipo_espectral][unidad2->tipo_espectral]*
    coste += 10*locutor_actual->indices_continuidad_coart[clase_1][clase_2]*
      coste_cepstral;

    //            coste_fase *= locutor_actual->indices_continuidad_fase_coart[unidad1->tipo_espectral][unidad2->tipo_espectral];

  } // if (unidad1->izquierdo_derecho == DERECHO) //if (izq_der == IZQUIERDO)
  else {

    //    coste += 10*locutor_actual->indices_continuidad_est[unidad1->tipo_espectral]*
    coste += 10*locutor_actual->indices_continuidad_est[clase_1]*
      coste_cepstral;

    //			coste_fase *= locutor_actual->indices_continuidad_fase_est[unidad1->tipo_espectral];

  } // else

  //		coste += coste_fase*2;

  pot_union2 = unidad2->potencia_inicial;
  pot_union1 = unidad1->potencia_final;
  dif_potencia = absoluto(pot_union2 - pot_union1);

  if (pot_union2 != pot_union1) // Antes 1.8
    if ( (unidad1->izquierdo_derecho == IZQUIERDO) && // (izq_der == IZQUIERDO) && 
	 ( (unidad1->tipo_espectral == OCLU_SOR_ESP) || (unidad1->tipo_espectral == OCLU_SON_ESP)) ) {
      coste += 0.2*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));
    }
    else
      if ( (unidad1->izquierdo_derecho == DERECHO) && 
	   (unidad1->tipo_espectral == SILENCIO_ESP) && 
	   ( (unidad2->tipo_espectral == OCLU_SOR_ESP) || (unidad2->tipo_espectral == OCLU_SON_ESP) ) ) {
      coste += 0.2*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));
      }
      else
	if (dif_potencia > ENERGIA_UMBRAL_CONCATENACION)
	  if (dif_potencia < ENERGIA_UMBRAL_CONCATENACION_2)
	    coste += 2*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));
	  else if (dif_potencia < ENERGIA_UMBRAL_CONCATENACION_3)
	    coste += 10*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));	  
	  else if (dif_potencia < ENERGIA_UMBRAL_CONCATENACION_4)
	    coste += 16*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));	  
	  else
	    coste += 20*dif_potencia / maximo(absoluto(pot_union2), absoluto(pot_union1));
  
  /*
    if (izq_der == IZQUIERDO) {
    
    fonema = *unidad2->semifonema;
    
    if ( (fonema != 'p') && (fonema != 't') && (fonema != 'k') && (fonema != 'b') &&
    (fonema != 'B') && (fonema != 'd') && (fonema != 'D') && (fonema != 'g') &&
    (fonema != 'G') ) { // es decir, si no es una oclusiva...
    if ( (pot_union2 < MINIMA_ENERGIA) || (pot_union1 < MINIMA_ENERGIA) )
    coste += 20;
    if (pot_union2 != pot_union1)
    coste += absoluto((pot_union2 - pot_union1)) /
    maximo(absoluto(pot_union2), absoluto(pot_union1));
    } // if ( (fonema != 'p') && ...

    }
    else { // DERECHO

    if (unidad1->semifonema[0] != '#')
    if ( (pot_union1 < MINIMA_ENERGIA) || (pot_union2 < MINIMA_ENERGIA) )
    coste += 20;

    if (pot_union2 != pot_union1) // Antes 1.8
    coste += absoluto((pot_union2 - pot_union1)) /
    maximo(absoluto(pot_union2), absoluto(pot_union1));
    } // else // DERECHO
  */

  return coste;
  // De momento vamos a pasar de la caché.

}


/**
 * Función:   calcula_coste_minimo_hasta_unidad.                                         
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste *C_ii: estructura en la que se almacena el coste mínimo,  
 *            el identificador de la propia unidad, el de aquella del paso anterior que  
 *            proporciona el coste mínimo, el propio coste y al puntero al camino óptimo.
 * \remarks Objetivo:  Calcula mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).                           
 */

void Viterbi::calcula_coste_minimo_hasta_unidad(Estructura_coste *C_ii) {

  Vector_semifonema_candidato *unidad_actual =  C_ii->unidad;
  int id_coste_minimo;
  int tamano_C_i = C_i.size();
  float coste_concatenacion;
  register float minimo = FLT_MAX;
  register float coste_actual;
  register int camino = 0;
  register Estructura_C_i *correcaminos; // = C_i;

  // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la
  // iteración anterior (i.e., los elementos de la cadena de estructuras C_i).

  for (register int contador = 0; contador < tamano_C_i; contador++) {
    correcaminos = &C_i[contador];

    // Calculamos el coste total hasta la unidad considerada en el paso actual.
    coste_concatenacion = calcula_C_c(correcaminos->unidad, unidad_actual);
    coste_actual = PESO_COSTE_CONCATENACION*coste_concatenacion + correcaminos->C_i;
    //        coste_actual = calcula_C_c(correcaminos->unidad, unidad_actual) + correcaminos->C_i;
    // Si el coste el menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.
    if (coste_actual < minimo) {
      //      id_coste_minimo = correcaminos->unidad->identificador;
      C_ii->coste_c = coste_actual;
      minimo = coste_actual;
      camino = contador;
    }

    //    C_ii->id_coste_minimo.push_back(id_coste_minimo);
    correcaminos++;

  }

  // Apuntamos qué camino proporciona el coste mínimo y su valor.
  //  C_ii->camino.push_back(camino);
  C_ii->camino.push_back(camino);
  C_ii->coste_c += PESO_COSTE_OBJETIVO*C_ii->coste_unidad;
  //    C_ii->coste_c += C_ii->coste_unidad;

}


/**
 * Función:   calcula_coste_minimo_hasta_unidades                                        
 * \remarks Entrada:
 *	    - numero_candidatos: número de estructuras de vector_coste.                  
 *          - mascara_contornos: opcional. Se emplea para el cálculo en paralelo de los diversos contornos, e indica cuáles de ellos siguen activos.
 *          - numero_maximo_contornos: opcional. Número de elementos de la cadena anterior.
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste *vector_coste: estructura en la que se almacenan el coste 
 *            mínimo, el identificador de la propia unidad, el de aquella del paso       
 *            anterior que proporciona el coste mínimo, el propio coste y al puntero al  
 *            camino óptimo.                                                             
 * \remarks Valor devuelto:                                                                       
 *          - En ausencia de error devuelve un cero.                                     
 * \remarks Objetivo:  Calcula mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).                           
 */

int Viterbi::calcula_coste_minimo_hasta_unidades(vector<Estructura_coste> &vector_coste, int numero_candidatos, char *mascara_contornos, int numero_maximo_contornos) {

  Vector_semifonema_candidato *unidad_actual, *anterior_unidad;
  float coste_concatenacion, coste_unico, mejor_coste_concatenacion;
  int numero_iteraciones;
  int cuenta_contornos;
  int contador;
  int id_coste_min;
  int sonoridad;
  int tamano_C_i;
  vector<int> id_coste_minimo;
  register float minimo;
  register float coste_actual;
  register int camino;
  vector<Estructura_C_i>::iterator correcaminos;
  vector<Estructura_coste>::iterator recorre_costes, vector_costes_final;

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---> En calcula_coste_minimo_hasta_unidades (%d): (%s)\n", iteracion, vector_coste[0].unidad->semifonema);
#endif

  sonoridad = vector_coste[0].unidad->sonoridad;

  if (mascara_contornos != NULL) {
    // if (sonoridad == SONORO)
    numero_iteraciones = numero_maximo_contornos;
    // else
    //   numero_iteraciones = 1;
    ///numero_iteraciones = vector_coste[0].costes_unidad.size();
  }
  else
    numero_iteraciones = 1;
  
  id_coste_minimo.reserve(numero_iteraciones);
  
  vector_costes_final = vector_coste.end();

  if ( (C_i[0].indice_contorno == CONTORNO_SORDO) && (mascara_contornos != NULL) ) {
    
    for (recorre_costes = vector_coste.begin(); recorre_costes != vector_costes_final; ++recorre_costes) {
      
      unidad_actual = recorre_costes->unidad;
      anterior_unidad = NULL;
      recorre_costes->costes_c.clear();
      recorre_costes->costes_c.reserve(numero_iteraciones);

      minimo = FLT_MAX;
      correcaminos = C_i.begin();

      tamano_C_i = C_i.size();

      for (contador = 0; contador < tamano_C_i; contador++, ++correcaminos) {
	
	if (correcaminos->unidad != anterior_unidad) {
	  coste_concatenacion = calcula_C_c(correcaminos->unidad, unidad_actual);
	  anterior_unidad = correcaminos->unidad;
	} // if (correcaminos->unidad != anterior_unidad)
	
	coste_actual = PESO_COSTE_CONCATENACION*coste_concatenacion + correcaminos->C_i;

	if (coste_actual < minimo) {
	  recorre_costes->coste_c = coste_actual;
	  minimo = coste_actual;
	  camino = contador;
	  id_coste_min = correcaminos->unidad->identificador;
	  mejor_coste_concatenacion = coste_concatenacion;
	} // if (coste_actual < minimo)

      } // for (contador = 0; ...

      // Actualizamos cada camino

      if (sonoridad == SONORO) {
	for (cuenta_contornos = 0; cuenta_contornos < numero_iteraciones; cuenta_contornos++) {
	  recorre_costes->costes_c.push_back(minimo + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[cuenta_contornos]);
	  recorre_costes->camino.push_back(camino);
	  id_coste_minimo.push_back(id_coste_min);
	}
      } // if (sonoridad == SONORO)
      else {
	recorre_costes->costes_c.push_back(minimo + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[0]);
	recorre_costes->camino.push_back(camino);
	id_coste_minimo.push_back(id_coste_min);

      } // if (sonoridad == SONORO

    } // if (recorre_costes = ...
  } // if (C_i[0].indice_contorno == CONTORNO_SORDO)
  else {

    for (recorre_costes = vector_coste.begin(); recorre_costes != vector_costes_final; ++recorre_costes) {
      
      // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la iteración anterior (i.e., los elementos de la cadena de estructuras C_i).
      
      unidad_actual = recorre_costes->unidad;
      anterior_unidad = NULL;
      
      recorre_costes->costes_c.clear();
      recorre_costes->costes_c.reserve(numero_iteraciones);
      
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "------> %d\n", unidad_actual->identificador);
#endif
      
      id_coste_minimo.clear();
      
      for (cuenta_contornos = 0; cuenta_contornos < numero_iteraciones; cuenta_contornos++) {
	
	// Si el contorno ya está podado, pasamos
	if (mascara_contornos != NULL)
	  if (mascara_contornos[cuenta_contornos] == 0) {
	    recorre_costes->costes_c.push_back(CAMINO_PODADO);
	    recorre_costes->camino.push_back(CAMINO_PODADO);
	    id_coste_minimo[cuenta_contornos] = CAMINO_PODADO;
	    //	  recorre_costes->camino.push_back(-1);
	    continue;
	  }
	
	minimo = FLT_MAX;
	correcaminos = C_i.begin();
	
	tamano_C_i = C_i.size();

	for (contador = 0; contador < tamano_C_i; contador++, ++correcaminos) {
	  
	  if (correcaminos->indice_contorno != CONTORNO_SORDO)
	    if (correcaminos->indice_contorno != cuenta_contornos)
	      continue;
	  
	  //Calculamos el coste total hasta la unidad considerada en el paso actual.
	  if (correcaminos->unidad != anterior_unidad) {
	    coste_concatenacion = calcula_C_c(correcaminos->unidad, unidad_actual);
	    anterior_unidad = correcaminos->unidad;
	  }
	  
	  coste_actual = PESO_COSTE_CONCATENACION*coste_concatenacion + correcaminos->C_i;
	  // Si el coste es menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.
	  if (coste_actual < minimo) {
	    //	  id_coste_minimo = correcaminos->unidad->identificador;
	    recorre_costes->coste_c = coste_actual;
	    minimo = coste_actual;
	    camino = contador;
	    id_coste_minimo[cuenta_contornos] = correcaminos->unidad->identificador;
	    mejor_coste_concatenacion = coste_concatenacion;
	  }
	  
	} // for (int contador = 0; // Recorriendo el vector C_i anterior
	
	// Actualizamos los costes de ese contorno:
	if (recorre_costes->unidad->sonoridad == SONORO)
	  recorre_costes->costes_c.push_back(minimo + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[cuenta_contornos]);
	else // Tengo que cambiar esto cuando incluya la variación de fronteras prosódicas
	  recorre_costes->costes_c.push_back(minimo + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[0]);
	recorre_costes->camino.push_back(camino);
	
      } // for (int cuenta_contornos = 0; ...
      
      // Apuntamos qué camino proporciona el coste mínimo y su valor.
      //    recorre_costes->camino = camino;
      recorre_costes->coste_c += PESO_COSTE_OBJETIVO*recorre_costes->coste_unidad;
      //    recorre_costes->id_coste_minimo.push_back(id_coste_minimo);
#ifdef _DEPURA_VITERBI
      for (int i = 0; i < recorre_costes->costes_c.size(); i++)
	fprintf(fichero_viterbi, "---------> %d, (%d %f %f)\n", id_coste_minimo[i], i, recorre_costes->costes_c[i], mejor_coste_concatenacion);
#endif
      
    } // for (vector<Estructura_coste>::iterator...
    
  } // else (if (C_i[0].indice_contorno == CONTORNO_SORDO)

  return 0;

}

/**
 * \remarks Calcula los caminos de coste mínimo hasta las unidades de la iteración anterior, para cada uno de los contornos de f0 que se están considerando en paralelo.
 * \brief Calcula mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).
 * \param[in] vector_coste estructura en la que se almacenan el coste mínimo, el identificador de la propia unidad, el de aquella del paso anterior que proporciona el coste mínimo, el propio coste y al puntero al camino óptimo.
 * \param[in] numero_candidatos número de elementos de la cadena anterior.
 * \param[in] numero_candidatos número de estructuras de vector_coste.        
 * \param[in] mascara_contornos array que indica qué contornos están todavía activos en esa iteración del algoritmo. 
 * \param[in] numero_maximo_contornos número de elementos de la cadena anterior.
 * \return En ausencia de error devuelve un cero.                            
 */

int Viterbi::calcula_coste_minimo_hasta_unidades_paralelo(vector<Estructura_coste> &vector_coste, int numero_candidatos, char *mascara_contornos, int numero_maximo_contornos) {

  Vector_semifonema_candidato *unidad_actual;
  float coste_concatenacion, coste_unico;
  int cuenta_contornos;
  int contador, cuenta;
  int sigue_contorno_sordo;
  int tamano_C_i;
  float coste_actual;

  vector<int> id_coste_minimo;
  vector<float> minimo;
  vector<int> camino;
  float *mejor_coste_concatenacion;

  vector<Estructura_C_i>::iterator correcaminos;
  vector<Estructura_coste>::iterator recorre_costes, vector_costes_final;

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---> En calcula_coste_minimo_hasta_unidades_paralelo (%d): (%s)\n", iteracion, vector_coste[0].unidad->semifonema);
#endif

  minimo.reserve(numero_maximo_contornos);

  if ( (mejor_coste_concatenacion = (float *) malloc(numero_maximo_contornos*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en calcula_coste_minimo_hasta_unidades_paralelo, al asignar memoria.\n");
    return 1;
  }

  camino.reserve(numero_maximo_contornos);

#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
  if (vector_coste[0].unidad->semifonema[0] == '#')
    sigue_contorno_sordo = 1;
  else
    sigue_contorno_sordo = 0;
#else
  sigue_contorno_sordo = !vector_coste[0].unidad->sonoridad;
#endif

  id_coste_minimo.reserve(numero_maximo_contornos);
  
  vector_costes_final = vector_coste.end();

  if (C_i[0].indice_contorno == CONTORNO_SORDO) {
    
    minimo.push_back(FLT_MAX);
    camino.push_back(-1);

    for (recorre_costes = vector_coste.begin(); recorre_costes != vector_costes_final; ++recorre_costes) {
      

      unidad_actual = recorre_costes->unidad;
      recorre_costes->costes_c.clear();
      recorre_costes->costes_c.reserve(numero_maximo_contornos);

      minimo[0] = FLT_MAX;

      correcaminos = C_i.begin();

      tamano_C_i = C_i.size();

#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "------> %d\n", unidad_actual->identificador);
#endif

      for (contador = 0; contador < tamano_C_i; contador++, ++correcaminos) {
	
	coste_concatenacion = calcula_C_c(correcaminos->unidad, unidad_actual);
	
	coste_actual = PESO_COSTE_CONCATENACION*coste_concatenacion + correcaminos->datos_camino[0].C_i;
	  
	  if (coste_actual < minimo[0]) {
	    recorre_costes->coste_c = coste_actual;
	    minimo[0] = coste_actual;
	    camino[0] = contador;
	    id_coste_minimo[0] = correcaminos->unidad->identificador;
	    mejor_coste_concatenacion[0] = coste_concatenacion;
	  } // if (coste_actual < minimo[0])

      } // for (contador = 0; ...

      // Actualizamos cada camino

      if (sigue_contorno_sordo == 0) {
	for (cuenta_contornos = 0; cuenta_contornos < numero_maximo_contornos; cuenta_contornos++) {
	  recorre_costes->costes_c.push_back(minimo[0] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[cuenta_contornos]);
	  recorre_costes->camino.push_back(camino[0]);
	  //	  id_coste_minimo.push_back(id_coste_min[0]);
	}
      } // if (sigue_contorno_sordo == 0)
      else {
	recorre_costes->costes_c.push_back(minimo[0] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[0]);
	recorre_costes->camino.push_back(camino[0]);
	//	id_coste_minimo.push_back(id_coste_min[0]);

      } // if (sigue_contorno_sordo == 0)

#ifdef _DEPURA_VITERBI
      for (int i = 0; i < recorre_costes->costes_c.size(); i++)
	fprintf(fichero_viterbi, "---------> %d, (%d %f %f)\n", id_coste_minimo[0], i, recorre_costes->costes_c[i], mejor_coste_concatenacion[0]);
#endif
  
    } // if (recorre_costes = ...
  } // if (C_i[0].indice_contorno == CONTORNO_SORDO)
  else {

    for (contador = 0; contador < numero_maximo_contornos; contador++) {
      camino.push_back(-1);
      id_coste_minimo.push_back(CAMINO_PODADO);
      minimo.push_back(0);
    }

    for (recorre_costes = vector_coste.begin(); recorre_costes != vector_costes_final; ++recorre_costes) {
      
      // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la iteración anterior (i.e., los elementos de la cadena de estructuras C_i).
      
      unidad_actual = recorre_costes->unidad;
      
      recorre_costes->costes_c.clear();
      recorre_costes->costes_c.reserve(numero_maximo_contornos);
      
#ifdef _DEPURA_VITERBI
      fprintf(fichero_viterbi, "------> %d\n", unidad_actual->identificador);
#endif
      
      id_coste_minimo.clear();
	
      for (contador = 0; contador < numero_maximo_contornos; contador++) {
	minimo[contador] = FLT_MAX;
	//	camino.push_back(-1);
	//	id_coste_minimo[contador] = CAMINO_PODADO;
      }
      
      correcaminos = C_i.begin();
      
      tamano_C_i = C_i.size();

      for (contador = 0; contador < tamano_C_i; contador++, ++correcaminos) {
	
	// if (correcaminos->indice_contorno != CONTORNO_SORDO)
	//   if (correcaminos->indice_contorno != cuenta_contornos)
	//     continue;
	
	coste_concatenacion = calcula_C_c(correcaminos->unidad, unidad_actual);	  
	for (cuenta_contornos = 0; cuenta_contornos < numero_maximo_contornos; cuenta_contornos++) {
	  
	  // Si el contorno ya está podado, pasamos
	  if (mascara_contornos[cuenta_contornos] == 0) {
	    minimo[cuenta_contornos] = CAMINO_PODADO;
	    id_coste_minimo[cuenta_contornos] = CAMINO_PODADO;
	    camino[cuenta_contornos] = -1;
	    mejor_coste_concatenacion[cuenta_contornos] = CAMINO_PODADO;
	    //	    recorre_costes->costes_c.push_back(CAMINO_PODADO);
	    //	    id_coste_minimo[cuenta_contornos] = CAMINO_PODADO;
	    //	  recorre_costes->camino.push_back(-1);
	    continue;
	  }
	  
	  //Calculamos el coste total hasta la unidad considerada en el paso actual.
	  coste_actual = PESO_COSTE_CONCATENACION*coste_concatenacion + correcaminos->datos_camino[cuenta_contornos].C_i;
	  // Si el coste es menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.

	  if (coste_actual < minimo[cuenta_contornos]) {
	    //	  id_coste_minimo = correcaminos->unidad->identificador;
	    recorre_costes->coste_c = coste_actual;
	    minimo[cuenta_contornos] = coste_actual;
	    camino[cuenta_contornos] = contador;
	    id_coste_minimo[cuenta_contornos] = correcaminos->unidad->identificador;
	    mejor_coste_concatenacion[cuenta_contornos] = coste_concatenacion;
	  }
	  
	} // for (cuenta_contornos = 0; ...

      } // for (contador = 0; ...
      
      for (cuenta_contornos = 0; cuenta_contornos < numero_maximo_contornos; cuenta_contornos++) {

	if (mascara_contornos[cuenta_contornos] != 0) {

#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
	  if (recorre_costes->unidad->semifonema[0] == '#')
	    recorre_costes->costes_c.push_back(minimo[cuenta_contornos] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[0]);
	  else
	    recorre_costes->costes_c.push_back(minimo[cuenta_contornos] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[cuenta_contornos]);
#else
	  // Actualizamos los costes de esa unidad
	  if (recorre_costes->unidad->sonoridad == SONORO)
	    recorre_costes->costes_c.push_back(minimo[cuenta_contornos] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[cuenta_contornos]);
	  else // Tengo que cambiar esto cuando incluya la variación de fronteras prosódicas
	    recorre_costes->costes_c.push_back(minimo[cuenta_contornos] + PESO_COSTE_OBJETIVO*recorre_costes->costes_unidad[0]);
#endif
	}
	else
	  recorre_costes->costes_c.push_back(CAMINO_PODADO);
      
      } // for (cuenta_contornos = 0; ...

      recorre_costes->camino = camino;

      // Apuntamos qué camino proporciona el coste mínimo y su valor.
      //    recorre_costes->camino = camino;
      recorre_costes->coste_c += PESO_COSTE_OBJETIVO*recorre_costes->coste_unidad;
      //    recorre_costes->id_coste_minimo.push_back(id_coste_minimo);
#ifdef _DEPURA_VITERBI
      for (int i = 0; i < recorre_costes->costes_c.size(); i++)
	fprintf(fichero_viterbi, "---------> %d, (%d %f %f)\n", id_coste_minimo[i], i, recorre_costes->costes_c[i], mejor_coste_concatenacion[i]);
#endif
      
    } // for (vector<Estructura_coste>::iterator...
    
  } // else (if (C_i[0].indice_contorno == CONTORNO_SORDO)

  free(mejor_coste_concatenacion);

  return 0;

}


/**
 * Función:   crea_vector_C_t
 * \remarks Entrada:
 *	    - delta_u: array con las unidades candidatas para una unidad objetivo.        
 *          - objetivo: identificador de la unidad considerada como objetivo.             
 *          - mascara_contornos: cuando se consideran varios contornos en paralelo, indica cuáles de ellos no han sido podados todavía
 *	\remarks Salida:
 *			- Estructura_coste **costes: array con los costes de unidad.                  
 * \remarks Entrada y Salida:                                                                      
 *          - *tamano: inicialmente contiene el número de unidades almacenadas en delta_u.
 *            A la salida, almacena el tamaño de *costes, ya que puede haber poda.        
 * \remarks Objetivo:  Crea el array de estructuras de coste de unidad, incluyendo la posibilidad  
 *            de realizar poda, tanto por número como por valor.                          
 */

int Viterbi::crea_vector_C_t(vector<Estructura_coste> &costes, int *tamano, Vector_semifonema_candidato **delta_u, Vector_descriptor_objetivo &objetivo, char *mascara_contornos) { 

  Estructura_coste correcaminos;
  Vector_semifonema_candidato **puntero;
  int tamano_actual = 0;
  int indice_minimo = 0;
  float coste_minimo;
  //  int c = 1;

  set<int>::iterator fin_set_C_i = set_C_i.end();

#ifdef PODA_C_t
  int poda;
#endif

#ifdef _DEPURA_CREA_VECTOR_COSTE
  FILE *f = NULL;
  char nombre[FILENAME_MAX];

  if (iteracion == 125) {
    sprintf(nombre, "bor_%d.txt", iteracion);
    f = fopen(nombre, "wt");
  }
#endif

  //  correcaminos = *costes;
  puntero = delta_u;

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---> En crea_vector_C_t (%d): (%s %s - %s - %s %s) [%d, %d, %d]\n", iteracion, objetivo.contexto_izquierdo_2, objetivo.contexto_izquierdo, objetivo.semifonema, objetivo.contexto_derecho, objetivo.contexto_derecho_2, objetivo.frontera_inicial, objetivo.frontera_final, objetivo.frontera_prosodica);
#endif

#ifdef _UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION

  Vector_semifonema_candidato *semifonema_contexto;
  encuentra_candidato_con_coarticulacion_adecuada(objetivo, locutor_actual, izq_der, &semifonema_contexto);
    
#endif

  // Repetimos el bucle para cada elemento de la cadena de unidades candidatas.

  for (int contador = 0; contador < *tamano; contador++) {

#ifdef _DEPURA_VITERBI
    fprintf(fichero_viterbi, "------> %d (%s %s - %s - %s %s) [%d, %d, %d]\n", (*puntero)->identificador, (*puntero)->contexto_izquierdo_2, (*puntero)->contexto_izquierdo, (*puntero)->semifonema, (*puntero)->contexto_derecho, (*puntero)->contexto_derecho_2, (*puntero)->frontera_inicial, (*puntero)->frontera_final, (*puntero)->frontera_prosodica);
#endif

    // Calculamos el coste de unidad respecto al objetivo.
    //    correcaminos = &costes[contador];

    //    correcaminos.costes_c.clear();
    correcaminos.costes_unidad.clear();
    //    correcaminos.id_coste_minimo.clear();

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
    correcaminos->coste_unidad = 0.0;
    // Apuntamos a la unidad.
    correcaminos->unidad = *puntero;
    // Incrementamos la variable tamano_actual.
    tamano_actual++;
#else

#ifdef _UTILIZA_CENTROIDES
    correcaminos->coste_unidad = calcula_C_t_centroide(*puntero, objetivo, izq_der);
#elif defined(_UTILIZA_REGRESION)
    correcaminos->coste_unidad = calcula_C_t_regresion(*puntero, objetivo, izq_der);
#elif defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS)
    correcaminos->coste_unidad = calcula_C_t_centroide_vector_medio(*puntero, objetivo, izq_der);
#elif defined(_UTILIZA_RED_NEURONAL)
    correcaminos->coste_unidad = calcula_C_t_red_neuronal(*puntero, objetivo, izq_der);
#elif defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)
    if (mascara_contornos)
      correcaminos.coste_unidad = calcula_C_t_red_neuronal_y_modelo_coarticulacion(*puntero, objetivo, semifonema_contexto, izq_der, &correcaminos.costes_unidad, mascara_contornos);
    else
      correcaminos.coste_unidad = calcula_C_t_red_neuronal_y_modelo_coarticulacion(*puntero, objetivo, semifonema_contexto, izq_der, &correcaminos.costes_unidad, NULL);
#elif defined(_UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES)
    correcaminos.coste_unidad = calcula_C_t_peso_coarticulacion_texto(*puntero, objetivo, izq_der, &correcaminos.costes_unidad, mascara_contornos);
#else
    correcaminos.coste_unidad = calcula_C_t(*puntero, objetivo, izq_der);
#endif
    // Apuntamos a la unidad.
    correcaminos.unidad = *puntero;
    // Incrementamos la variable tamano_actual.
    tamano_actual++;
#endif

    // Aproximación para ver si es podable: en lugar de recorrer toda la matriz de caminos óptimos, comprobamos si la unidad tiene por el otro lado el contexto deseado:

    if (correcaminos.unidad->izquierdo_derecho == IZQUIERDO) {
      // if ( (strcmp(correcaminos.unidad->contexto_izquierdo_2, objetivo.contexto_izquierdo_2) == 0) ||
      // 	   (strcmp(correcaminos.unidad->contexto_derecho, objetivo.contexto_derecho) == 0) )

      if ( (set_C_i.find(correcaminos.unidad->identificador - 1) != fin_set_C_i) ||
	   (strcmp(correcaminos.unidad->contexto_derecho, objetivo.contexto_derecho) == 0) )
    	correcaminos.podable = 0;
      else
    	correcaminos.podable = 1;
    }
    else {
      // if ( (strcmp(correcaminos.unidad->contexto_izquierdo, objetivo.contexto_izquierdo) == 0) ||
      // 	   (strcmp(correcaminos.unidad->contexto_derecho_2, objetivo.contexto_derecho_2) == 0) )
      if ( (set_C_i.find(correcaminos.unidad->identificador) != fin_set_C_i) ||
	   (strcmp(correcaminos.unidad->contexto_derecho_2, objetivo.contexto_derecho_2) == 0) )
    	correcaminos.podable = 0;
      else
    	correcaminos.podable = 1;
    }

    //    correcaminos.podable = 1;

    // Vemos su coste para la poda. Tengo que cambiar esto... el funcionamiento va a depender mucho de lo que aquí se haga...
    if (mascara_contornos) {
      int numero_costes = correcaminos.costes_unidad.size();
      coste_minimo = FLT_MAX;
      for (int cuenta = 0; cuenta < numero_costes; cuenta++) {
	if (correcaminos.costes_unidad[cuenta] < 0)
	  continue;
	if (coste_minimo > correcaminos.costes_unidad[cuenta]) {
	  coste_minimo = correcaminos.costes_unidad[cuenta];
	  indice_minimo = cuenta;
	}
      } // for (int cuenta = 0; 
      correcaminos.coste_poda = correcaminos.costes_unidad[indice_minimo];
    }
    else
      correcaminos.coste_poda = correcaminos.coste_unidad;

    costes.push_back(correcaminos);

#ifdef _DEPURA_VITERBI
    fprintf(fichero_viterbi, "------------>");
    for (int i = 0; i < correcaminos.costes_unidad.size(); i++)
      fprintf(fichero_viterbi, " (%d %f)", i, correcaminos.costes_unidad[i]);
    fprintf(fichero_viterbi, "\n");
    fprintf(fichero_viterbi, "------------> Coste poda = %f (%d).\n", correcaminos.coste_poda, indice_minimo);
#endif

#ifdef PODA_C_t



#ifdef _DEPURA_CREA_VECTOR_COSTE
    if (f)
      fprintf(f, "\nAñadimos %d (%f, %d).\n\n", correcaminos.unidad->identificador, correcaminos.coste_poda, tamano_actual);
    //    fprintf(stderr, "Poda Ct => tamano_actual = %d.\n", tamano_actual);
#endif

    //   // Comprobamos si la unidad puede formar parte del camino óptimo.
    if ( (poda = ordena_y_poda_C_t(costes, tamano_actual)) != 0) {
      tamano_actual -= poda; // En este caso, no puede.
      costes.resize(tamano_actual);
      //      correcaminos = *costes + tamano_actual;
    }
    
#ifdef _DEPURA_CREA_VECTOR_COSTE
    if (f)
      muestra_contenido_vector_costes(costes, f);
#endif

    // if ( (iteracion == 54) && (iteracion_contorno == 1) ) {
    //   printf("Nueva unidad = %d.\n", correcaminos.unidad->identificador);
    //   muestra_contenido_vector_costes(costes);
    // }

// #ifdef _DEPURA_VITERBI
//     fprintf(fichero_viterbi, "------------> Vector de costes: (tamaño = %d)\n", tamano_actual);
//     for (int i = 0; i < tamano_actual; i++)
//       fprintf(fichero_viterbi, "------------> (%d %d %f)\n", i, costes[i].unidad->identificador, costes[i].coste_poda);
// #endif

#endif

    puntero++;

  }

#ifdef _DEPURA_VITERBI
    fprintf(fichero_viterbi, "------------> Vector de costes: (tamaño = %d)\n", tamano_actual);
    for (int i = 0; i < tamano_actual; i++)
      fprintf(fichero_viterbi, "------------> (%d %d %f)\n", i, costes[i].unidad->identificador, costes[i].coste_poda);
#endif

  *tamano = tamano_actual;

#ifdef _DEPURA_CREA_VECTOR_COSTE
  if (f)
    fclose(f);
#endif

  return 0;
}

#ifdef PODA_C_i

/**
 * Función:   ordena_y_poda_C_i                                                          
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - inicio: array ordenado según el coste total de sus  elementos.             
 * \remarks Valor devuelto:                                                                       
 *           - Si no hay poda se devuelve un 0. Un 1 en caso contrario.                  
 * \remarks Objetivo:  ordena y realiza la poda del array por número.                             
 * \remarks NOTA:      De momento no se consideran como no podables aquellos elementos que tienen 
 *            C_c nulo en el siguiente paso.                                             
 */

int Viterbi::ordena_y_poda_C_i(vector<Estructura_C_i> &inicio, int posicion) {

  char podable;
  int poda_numero = 0, poda_valor = 0; //, posicion_poda;

  //Estructura_C_i *correcaminos; // = inicio;

  // La posición del nuevo elemento es la del tamaño - 1.

  posicion -= 1;

  register float coste = inicio[posicion].C_i;

  podable = inicio[posicion].podable;

#ifdef PODA_C_i_POR_VALOR

  register float umbral = minimo_c_i + C_PODA_C_i;

  if (posicion >=  MIN_NUMERO_PODA_VALOR_C_i) {
    if (coste > inicio[MIN_NUMERO_PODA_VALOR_C_i - 1].C_i)
      if (coste > umbral)
	// no puede formar parte de la secuencia óptima. Lo eliminamos.
	return 1;
  }

#ifdef PODA_C_i_POR_NUMERO
  if (posicion >= N_PODA_Ci)
    if (inicio[posicion].C_i > inicio[N_PODA_Ci - 1].C_i)
      if (podable)
	return 1;
#endif

  // Comprobamos si el valor es menor que el mínimo actual.
  if (coste < minimo_c_i) {
    umbral = coste + C_PODA_C_i;
    minimo_c_i = coste;
  }

#endif

  int contador = 0, cuenta;

  // Ordenamos el array.
  // Esto lo tengo que cambiar por una búsqueda binaria.

  while ( (inicio[contador].C_i <= coste) && (contador < posicion) ) {
    contador++;
    //    correcaminos++;
  }

  if (contador != posicion) {
    Estructura_C_i nuevo = inicio[posicion];

    //    correcaminos = inicio + posicion;

    for (int cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef PODA_C_i_POR_VALOR
      if (posicion >= MIN_NUMERO_PODA_VALOR_C_i) {
	if (inicio[cuenta].C_i > umbral) { //(correcaminos->C_i > umbral)
	  poda_valor = posicion - cuenta;
	  //	posicion_poda = cuenta;
	}
      }
#endif
      inicio[cuenta] = inicio[cuenta - 1];

    }
    inicio[contador] = nuevo; // comprobar esto
    //    *correcaminos = nuevo;

#ifdef PODA_C_i_POR_VALOR
    if (posicion >= MIN_NUMERO_PODA_VALOR_C_i) {
      if (inicio[MIN_NUMERO_PODA_VALOR_C_i - 1].C_i > umbral) { //(correcaminos->C_i > umbral)
	//	poda_valor = posicion - contador;
	poda_valor = posicion - MIN_NUMERO_PODA_VALOR_C_i + 1;
	//	posicion_poda = cuenta;
      }
    }
#endif

  }

  if (poda_valor)
    return poda_valor;

#ifdef PODA_C_i_POR_NUMERO
  if (posicion >= N_PODA_Ci) { // El >= viene del posicion -= 1.
    poda_numero = 0;
    for (cuenta = N_PODA_Ci; cuenta <= posicion; cuenta++) {
      if ( (inicio[cuenta].podable == 1) && (inicio[N_PODA_Ci].C_i < inicio[cuenta].C_i) ) {
	poda_numero++;
	for (int cuenta_2 = cuenta; cuenta_2 < posicion; cuenta_2++)
	  inicio[cuenta_2] = inicio[cuenta_2 + 1];
      }
    }
  }
  
#endif

  return poda_numero;

}

#endif

#ifdef PODA_C_t

/**
 * Función:   ordena_y_poda_C_t                                                          
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste *inicio: array ordenado según el coste de unidad de sus   
 *            elementos.                                                                 
 * \remarks Valor devuelto:                                                                       
 *           - Se devuelve el número de elementos podados.                               
 * \remarks Objetivo:  ordena y realiza la poda del array, ya sea por número o por valor.         
 * \remarks NOTA:      De momento no se consideran como no podables aquellos elementos que tienen 
 *            C_c nulo en el siguiente paso.                                             
 */

int Viterbi::ordena_y_poda_C_t(vector<Estructura_coste> &inicio, int posicion) {

  char podable;
  int poda_numero = 0, poda_valor = 0; //, posicion_poda;
  //  Estructura_coste *correcaminos = inicio;
  
  // La posición en la que está situado el nuevo elemento es la del tamaño menos 1.
  posicion -= 1;

  register float coste = inicio[posicion].coste_poda;

#ifdef PODA_C_t_POR_VALOR

  register float umbral;

  if (inicio.size() > 1) // En ocasiones hay una unidad muy dominante, y condiciona demasiado la búsqueda
    umbral = (minimo_c_t + inicio[1].coste_poda) / 2 + C_PODA_UNIDAD;
  else
    umbral = minimo_c_t + C_PODA_UNIDAD;

  podable = inicio[posicion].podable;

  //  fprintf(stderr, "Añadiendo %d.\n", inicio[posicion].unidad->identificador);

  if (posicion >= MIN_NUMERO_PODA_VALOR_C_t) {
    if (coste > inicio[MIN_NUMERO_PODA_VALOR_C_t - 1].coste_poda)
      if (coste > umbral)
      // no puede formar parte de la secuencia óptima. Lo eliminamos.
      return 1;
  }

#ifdef PODA_C_t_POR_NUMERO
  if (posicion >= N_PODA_UNIDAD)
    if (inicio[posicion].coste_poda > inicio[N_PODA_UNIDAD - 1].coste_poda)
      if (podable)
	return 1;
#endif

  // Si el coste es menor que el mínimo, actualizamos su valor.

  if (coste < minimo_c_t) {
    minimo_c_t = coste;
    if (inicio.size() > 1) // En ocasiones hay una unidad muy dominante, y condiciona demasiado la búsqueda
      umbral = (coste + inicio[0].coste_poda) / 2 + C_PODA_UNIDAD;
    else
      umbral = coste + C_PODA_UNIDAD;
  }

#endif

  int contador = 0, cuenta;

  // Ordenamos el array.
  // Esto lo tengo que cambiar por una búsqueda binaria.

  while ( (inicio[contador].coste_poda <= coste) && (contador < posicion) ) {
    contador++;
    //    correcaminos++;
  } // OJO: esto ahora no va a funcionar bien, puesto que cada unidad tiene múltiples caminos. Quizás podríamos hacerlo en función del mínimo coste ...

  if (contador != posicion) {
    Estructura_coste nuevo = inicio[posicion];

    //    correcaminos = inicio + posicion;

    for (cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef PODA_C_t_POR_VALOR
      if (posicion >= MIN_NUMERO_PODA_VALOR_C_t) {
	if (inicio[cuenta].coste_poda > umbral) {
	  poda_valor = posicion - cuenta;
	  //	posicion_poda = cuenta;
	}
      }
#endif
      inicio[cuenta] = inicio[cuenta - 1];

    }

    inicio[contador] = nuevo;

#ifdef PODA_C_t_POR_VALOR
    if (posicion >= MIN_NUMERO_PODA_VALOR_C_t) {
      if (inicio[MIN_NUMERO_PODA_VALOR_C_t - 1].coste_poda > umbral) {
	//	poda_valor = posicion - contador;
	poda_valor = posicion - MIN_NUMERO_PODA_VALOR_C_t + 1;
	//	posicion_poda = cuenta;
      }
    }
#endif

    //    *correcaminos = nuevo;

  }

  if (poda_valor)
    return poda_valor;

#ifdef PODA_C_t_POR_NUMERO
  if (posicion >= N_PODA_UNIDAD) {
    poda_numero = 0;
    for (cuenta = N_PODA_UNIDAD; cuenta <= posicion; cuenta++) {
      if ( (inicio[cuenta].podable == 1) && (inicio[N_PODA_UNIDAD -1].coste_poda < inicio[cuenta].coste_poda) ) {
	poda_numero++;
	for (int cuenta_2 = cuenta; cuenta_2 < posicion; cuenta_2++) {
	  inicio[cuenta_2] = inicio[cuenta_2 + 1];
	}
      }
    }
  }
    
#endif

  //  fprintf(stderr, "Saliendo\n");

  return poda_numero;

}

#endif

/**
 * Función:   crea_vector_C_i                                                             
 * \remarks Entrada:
 *			- Estructura_coste *C_concatena: array, ya podado, con los costes de caminos  
 *            más cortos a cada unidad.                                                   
 *          - tamano: contiene la longitud de C_concatena.                                
 *          - objetivo: Vector descriptor objetivo con las características de la unidad deseada
 *          - ultima_unidad: indica si se trata de la última unidad de la búsqueda actual.
 *          - mascara_contornos: (opcional). Para el caso del cómputo en paralelo de varios contornos, indica cuáles de ellos aún no se han podado.

 * \remarks Valor devuelto:                                                                       
 *           - Devuelve 0 si no se produce ningún error.                                  
 * \remarks Objetivo:  crea la cadena de caminos mínimos hasta el paso actual del algoritmo.       
 */

int Viterbi::crea_vector_C_i(vector<Estructura_coste> &C_concatena, int tamano, Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad, char *mascara_contornos) {

  int contador;
  int tamano_actual = 0;
 int contorno_sordo, sonoridad;
  vector<Estructura_coste>::iterator correcaminos = C_concatena.begin(); // = C_concatena;

#ifdef PODA_C_i
  int poda;
#endif

  set_C_i.clear();

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---> En crea_vector_C_i (%d): (%s)\n", iteracion, C_concatena[0].unidad->semifonema);
#endif

  if (C_i[0].indice_contorno == CONTORNO_SORDO)
    contorno_sordo = 1;
  else
    contorno_sordo = 0;

  sonoridad = C_concatena[0].unidad->sonoridad;

  C_i.clear();

#ifndef PODA_C_i_POR_NUMERO

  if (tamano > C_i.size()) { //|| (tamano > memoria_reservada_C_i) ) {
    // Liberamos la memoria que estaba reservada anteriormente.
    C_i.reserve(tamano);
    //    memoria_reservada_C_i = tamano;
  }
#else

  if (  (maxima_memoria_reservada == 0) && (tamano > C_i.size()) ) {

    C_i.reserve(N_PODA_Ci + 1);

    maxima_memoria_reservada = 1;

  }

#endif

  Estructura_C_i recorre_C_i;

  // Vamos rellenando los campos de cada estructura del array.
  int numero_costes = correcaminos->costes_c.size();

  for (contador = 0; contador < tamano; contador++, correcaminos++) {

    for (int cuenta_contornos = 0; cuenta_contornos < numero_costes; cuenta_contornos++) {

      if (correcaminos->costes_c[cuenta_contornos] < 0)
	continue;

      if (mascara_contornos != NULL)
	if ( (mascara_contornos[cuenta_contornos] == 0) && 
	     (contorno_sordo == 0) )
	  continue;

      // Añadimos la propia unidad.
      recorre_C_i.unidad = correcaminos->unidad;
      // El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
      recorre_C_i.camino = correcaminos->camino[cuenta_contornos]; //correcaminos->camino[cuenta_contornos];
      // El coste mínimo para llegar hasta dicha unidad.
      recorre_C_i.C_i = correcaminos->costes_c[cuenta_contornos];

      // El contorno que le corresponde:
#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
      if ( (contorno_sordo == 1) && (correcaminos->unidad->semifonema[0] == '#') )
	recorre_C_i.indice_contorno = CONTORNO_SORDO;
      else
	recorre_C_i.indice_contorno = cuenta_contornos;
#else
      if ( (sonoridad == SORDO) && (contorno_sordo == 1) )
	recorre_C_i.indice_contorno = CONTORNO_SORDO;
      else
	recorre_C_i.indice_contorno = cuenta_contornos;
#endif
      tamano_actual++;
 
      // Aproximación para ver si es podable: comprobamos si la unidad tiene por el otro lado el contexto deseado:
      
      if (ultima_unidad == 0) {

	if (recorre_C_i.unidad->izquierdo_derecho == IZQUIERDO) {
	  if (strcmp(recorre_C_i.unidad->contexto_derecho, objetivo.contexto_derecho) == 0)
	    recorre_C_i.podable = 0;
	  else
	    recorre_C_i.podable = 1;
	}
	else {
	  if (strcmp(recorre_C_i.unidad->contexto_derecho_2, objetivo.contexto_derecho_2) == 0)
	    recorre_C_i.podable = 0;
	  else
	    recorre_C_i.podable = 1;
	}
      
      } // if (ultima_unidad == 0)
      else
	recorre_C_i.podable = 1;

      C_i.push_back(recorre_C_i);

      //      fprintf(stderr, "tamano_actual = %d.\n", tamano_actual);

#ifdef PODA_C_i
      // Comprobamos si hay que realizar algún tipo de poda.
      if ( ( poda = ordena_y_poda_C_i(C_i, tamano_actual)) != 0) {
	tamano_actual -= poda; // Comprobar esto
	C_i.resize(tamano_actual);
	//	recorre_C_i = C_i + tamano_actual;
      }
#endif

    } // for (int cuenta_contornos = 0; ...

    //    correcaminos++;

  } // for (contador = 0; ...

  // Actualizamos el número de elementos de la cadena C_i.

  //  tamano_C_i = tamano_actual;

  // Actualizamos la matriz

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "------> tamaño = %d. [Índice (identificador coste indice_contorno camino)  ]\n", C_i.size());
#endif

#ifndef _CALCULA_FRECUENCIA_COSTES_CONCATENACION

  if (caminos_optimos.siguiente_iteracion(C_i.size()))
    return 1;

  vector<Estructura_C_i>:: iterator apunta_C_i = C_i.begin();

  for (contador = 0; contador < C_i.size(); contador++, ++apunta_C_i) {
    //    if (matriz.introduce_elemento(contador, C_i[contador].unidad, C_i[contador].camino, izq_der))

    set_C_i.insert(apunta_C_i->unidad->identificador);

    if (mascara_contornos == NULL) {
      if (caminos_optimos.anhade_nodo(contador, apunta_C_i->unidad, apunta_C_i->camino, izq_der))
	return 1;
    }
    else
      for (int cuenta = 0; cuenta < numero_costes; cuenta++)
	if (mascara_contornos[cuenta] != 0)
	  if (caminos_optimos.anhade_nodo(contador, apunta_C_i->unidad, apunta_C_i->datos_camino[contador].camino, izq_der, apunta_C_i->datos_camino[cuenta].indice_contorno))
	    return 1;

#ifdef _DEPURA_VITERBI
    fprintf(fichero_viterbi, "---------> %d (%d %f %d %d)\n", contador, apunta_C_i->unidad->identificador, apunta_C_i->C_i, apunta_C_i->indice_contorno, apunta_C_i->camino);
#endif

  }

#endif

  return 0;

}


/**
 * \remarks  Función encargada de crear el vector C_i, en el que se almacenan los caminos óptimos a cada unidad de la iteración actual. Se emplea en el modo de cálculo de contornos en paralelo
 * \param[in] C_concatena array, ya podado, con los costes de caminos más cortos a cada unidad.                                                   
 * \param[in] tamano contiene la longitud de C_concatena.
 * \param[in] mascara_contornos array que indica qué contornos han sido ya podados.
 * \param[in] objetivo Vector descriptor objetivo con la información deseada para dicha unidad
 * \param[in] ultima_unidad Indica si es la última unidad de la búsqueda actual
 * \return Devuelve 0 si no se produce ningún error.
 */

int Viterbi::crea_vector_C_i_paralelo(vector<Estructura_coste> &C_concatena, int tamano, char *mascara_contornos, Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad) {

  int contador, cuenta_contornos;
  int tamano_actual = 0;
  int contorno_sordo, sonoridad;
  int mejor_camino, mejor_contorno;
  int numero_costes;
  float minimo;

  Datos_mejor_camino datos_camino;
  vector<Datos_mejor_camino> mejores_caminos;

  set_C_i.clear();

#ifdef _DEPURA_CREA_VECTOR_C_I
  FILE *fichero_C_i = NULL;

  if (iteracion == 125)
    if ( (fichero_C_i = fopen("fichero_C_i.txt", "wt")) == NULL) {
      fprintf(fichero_C_i, "Error al crear el fichero de creación del vector C_i.\n");
      return 1;
    }

#endif

  vector<Estructura_coste>::iterator correcaminos = C_concatena.begin(); // = C_concatena;

#ifdef PODA_C_i
  int poda;
#endif

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "---> En crea_vector_C_i_paralelo (%d): (%s)\n", iteracion, C_concatena[0].unidad->semifonema);
#endif

  if (C_i[0].indice_contorno == CONTORNO_SORDO)
    contorno_sordo = 1;
  else
    contorno_sordo = 0;

  sonoridad = C_concatena[0].unidad->sonoridad;

  C_i.clear();

#ifndef PODA_C_i_POR_NUMERO

  if (tamano > C_i.size()) { //|| (tamano > memoria_reservada_C_i) ) {
    // Liberamos la memoria que estaba reservada anteriormente.
    C_i.reserve(tamano);
    //    memoria_reservada_C_i = tamano;
  }
#else

  if (  (maxima_memoria_reservada == 0) && (tamano > C_i.size()) ) {

    C_i.reserve(N_PODA_Ci + 1);

    maxima_memoria_reservada = 1;

  }

#endif

  Estructura_C_i recorre_C_i;

  // Vamos rellenando los campos de cada estructura del array.
  numero_costes = correcaminos->costes_c.size();
  mejores_caminos.reserve(numero_costes);

  for (contador = 0; contador < tamano; contador++, correcaminos++) {

    mejores_caminos.clear();
    
    // Añadimos la propia unidad.
    recorre_C_i.unidad = correcaminos->unidad;

    // Los caminos (filas de la matriz de caminos óptimos) que proporcionan coste mínimo para cada contorno


#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
    if ( (contorno_sordo == 0) || (correcaminos->unidad->semifonema[0] != '#') ) {
#else
    if ( (contorno_sordo == 0) || (sonoridad == 1) ) {
#endif
      minimo = FLT_MAX;
      
      for (cuenta_contornos = 0; cuenta_contornos < numero_costes; cuenta_contornos++) {
	
	datos_camino.camino = correcaminos->camino[cuenta_contornos];
	datos_camino.indice_contorno = cuenta_contornos;

	if (mascara_contornos[cuenta_contornos] == 0) {
	  datos_camino.C_i = CAMINO_PODADO;
	  mejores_caminos.push_back(datos_camino);
	  continue;
	}

	datos_camino.C_i = correcaminos->costes_c[cuenta_contornos];
	mejores_caminos.push_back(datos_camino);
	if (datos_camino.C_i < minimo) {
	  minimo = datos_camino.C_i;
	  mejor_contorno = cuenta_contornos;
	  mejor_camino = datos_camino.camino;
	}

      } // for (cuenta_contornos = 0;...
    } 
    else {
      datos_camino.indice_contorno = CONTORNO_SORDO;
      datos_camino.C_i = correcaminos->costes_c[0];
      //      mejor_contorno = CONTORNO_SORDO;
      mejor_camino = correcaminos->camino[0];
      datos_camino.camino = mejor_camino;
      minimo = datos_camino.C_i;
      mejores_caminos.push_back(datos_camino);
    }

    recorre_C_i.datos_camino = mejores_caminos;

    recorre_C_i.camino = mejor_camino;

    // El coste mínimo para llegar hasta dicha unidad.
    recorre_C_i.C_i = minimo;
    
    // El contorno que le corresponde:
    if ( (sonoridad == SORDO) && (contorno_sordo == 1) )
      recorre_C_i.indice_contorno = CONTORNO_SORDO;
    else
      recorre_C_i.indice_contorno = mejor_contorno;
    
    tamano_actual++;
  
    // Aproximación para ver si es podable: comprobamos si la unidad tiene por el otro lado el contexto deseado:
    
    if (ultima_unidad == 0) {

      if (recorre_C_i.unidad->izquierdo_derecho == IZQUIERDO) {
	if (strcmp(recorre_C_i.unidad->contexto_derecho, objetivo.contexto_derecho) == 0)
	  recorre_C_i.podable = 0;
	else
	  recorre_C_i.podable = 1;
      }
      else {
	if (strcmp(recorre_C_i.unidad->contexto_derecho_2, objetivo.contexto_derecho_2) == 0)
	  recorre_C_i.podable = 0;
	else
	  recorre_C_i.podable = 1;
      }

    } // if (ultima_unidad == 0)
    else
      recorre_C_i.podable = 1;
      
	     
  
    C_i.push_back(recorre_C_i);
    
    //      fprintf(stderr, "tamano_actual = %d.\n", tamano_actual);

#ifdef _DEPURA_CREA_VECTOR_C_I
    if (fichero_C_i != NULL)
      fprintf(fichero_C_i, "Añadimos la unidad %d, con coste %f (podable = %d).\n", recorre_C_i.unidad->identificador, recorre_C_i.C_i, recorre_C_i.podable);
#endif


#ifdef PODA_C_i
    // Comprobamos si hay que realizar algún tipo de poda.
    if ( ( poda = ordena_y_poda_C_i(C_i, tamano_actual)) != 0) {
      tamano_actual -= poda; // Comprobar esto
      C_i.resize(tamano_actual);
      //	recorre_C_i = C_i + tamano_actual;
    }
#endif
    
#ifdef _DEPURA_CREA_VECTOR_C_I
    if (fichero_C_i != NULL)
      muestra_contenido_vector_C_i(C_i, fichero_C_i);
#endif

  } // for (contador = 0; ...
  
  // Actualizamos el número de elementos de la cadena C_i.
  
  //  tamano_C_i = tamano_actual;

  // Actualizamos la matriz

#ifdef _DEPURA_VITERBI
  fprintf(fichero_viterbi, "------> tamaño = %d. [Índice (identificador coste indice_contorno camino)  ]\n", C_i.size());
#endif

#ifndef _CALCULA_FRECUENCIA_COSTES_CONCATENACION

#ifdef _DEPURA_VITERBI
  vector<Datos_mejor_camino>::iterator apunta_datos;
#endif

  if (caminos_optimos.siguiente_iteracion(C_i.size()))
    return 1;

  vector<Estructura_C_i>::iterator apunta_C_i = C_i.begin();

  for (contador = 0; contador < C_i.size(); contador++, apunta_C_i++) {
    //    if (matriz.introduce_elemento(contador, apunta_C_i->unidad, apunta_C_i->camino, izq_der))

    set_C_i.insert(apunta_C_i->unidad->identificador);

    for (cuenta_contornos = 0; cuenta_contornos < numero_costes; cuenta_contornos++)
      if ( (mascara_contornos[cuenta_contornos] == 1) || (numero_costes == 1) )
	if (caminos_optimos.anhade_nodo(contador, apunta_C_i->unidad, apunta_C_i->datos_camino[cuenta_contornos].camino, izq_der, apunta_C_i->datos_camino[cuenta_contornos].indice_contorno)
	    )
	  return 1;
#ifdef _DEPURA_VITERBI

    fprintf(fichero_viterbi, "---------> %d (%d %f %d %d)\n", contador, apunta_C_i->unidad->identificador, apunta_C_i->C_i, apunta_C_i->indice_contorno, apunta_C_i->camino);
    
    if (numero_costes > 1) {
      apunta_datos = apunta_C_i->datos_camino.begin();
      for (cuenta_contornos = 0; cuenta_contornos < numero_costes; cuenta_contornos++, apunta_datos++)
	fprintf(fichero_viterbi, "------------> %d (%f %d %d)\n", cuenta_contornos, apunta_datos->C_i, apunta_datos->indice_contorno, apunta_datos->camino);
    }

#endif
      
  }

#endif


#ifdef _DEPURA_CREA_VECTOR_C_I
  if (fichero_C_i)
    fclose(fichero_C_i);
#endif

  return 0;

}

/**
 * Función:   inicia_algoritmo_Viterbi                                                 
 * \remarks Entrada:
 *			- \remarks Objetivo: primera unidad de la secuencia que se debe sintetizar.         
 *          - numero_objetivos: número de elementos que se van a sintetizar.           
 *          - izquierdo_derecho: indica el tipo de contexto (IZQUIERDO o DERECHO) a    
 *            partir del cual se comienza la selección de unidades.                     
 *          - mascara_contornos: cuando se consideran varios contornos en paralelo, indica cuáles de ellos no han sido completamente podados todavía.
 *          - numero_maximo_contornos: número de elementos de la cadena anterior.

 * \remarks Valor devuelto:                                                                     
 *          - Devuelve 0 en ausencia de error.                                         
 * \remarks Objetivo:  Lleva a cabo la primera etapa del algoritmo de Viterbi, inicializando    
 *            las variables necesarias para las siguientes iteraciones.                
 */

int Viterbi::inicia_algoritmo_Viterbi(Vector_descriptor_objetivo &objetivo, int numero_objetivos, char izquierdo_derecho, char *mascara_contornos, int numero_maximo_contornos) {

  // El vector descriptor se corresponde con el inicio de la frase, es decir, silencio con contexto.

  Vector_semifonema_candidato **delta_u;
  int numero_candidatos;
  int contador;
  vector<Estructura_coste>::iterator it, vector_coste_final;

#ifdef _DEPURA_VITERBI
  char nombre_fichero[FILENAME_MAX];

  iteracion++;

  sprintf(nombre_fichero_depura, "seleccion_acustica_%d", iteracion_contorno);

  sprintf(nombre_fichero, "%s_%d_%s.txt", nombre_fichero_depura, iteracion, objetivo.semifonema);

  filtra_nombre_fichero(nombre_fichero);

  if ( (fichero_viterbi = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en inicia_algoritmo_Viterbi, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero_viterbi...

  fprintf(fichero_viterbi, "Inicio del algoritmo de Viterbi (%d)\n\n", iteracion_contorno);
#endif

  izq_der = izquierdo_derecho;
    
#ifdef PODA_C_t_POR_VALOR

  inicia_poda_C_t();

#endif

#ifdef PODA_C_i_POR_VALOR

  inicia_poda_C_i();

#endif

  // Creamos la cadena de unidades candidatas.

  if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
    return 1;

  if (izq_der == IZQUIERDO)
    numero_candidatos = 1; // Suponemos que la primera unidad siempre va a ser #.#.
    
  // Calculamos el coste a cada unidad. En este caso, no se considera el coste de concatenación.

  vector_coste.clear();
#ifdef _PODA_C_t_POR_NUMERO
  vector_coste.reserve(N_PODA_Ct);
#else
  vector_coste.reserve(numero_candidatos);
#endif

  if (crea_vector_C_t(vector_coste, &numero_candidatos, delta_u, objetivo, mascara_contornos))
    return 1;

  //  tamano_vector_coste = numero_candidatos;
    
  // Iniciamos la matriz que se emplea para memorizar los caminos de coste mínimo a cada unidad.
       
  if (caminos_optimos.inicia_path_list(numero_objetivos))
    return 1;

  //  correcaminos = vector_coste;

  // Creamos la cadena de costes.

  contador = 0;
  vector_coste_final = vector_coste.end();
  for (it = vector_coste.begin(); it != vector_coste_final; it++, contador++) {

    it->coste_c = it->coste_unidad*PESO_COSTE_OBJETIVO;
#ifndef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
    it->camino.push_back(0);
    it->costes_c.push_back(it->costes_unidad[0]*PESO_COSTE_OBJETIVO);
#else
    for (int cuenta = 0; cuenta < it->costes_unidad.size(); cuenta++) {
      it->camino.push_back(0);
      it->costes_c.push_back(it->costes_unidad[cuenta]*PESO_COSTE_OBJETIVO);
    }
#endif
    //    float coste_C_t = it->costes_unidad[0];
    // for (int cuenta = 0; cuenta < numero_maximo_contornos; cuenta++) {
    //   it->costes_c.push_back(coste_C_t);
    //   it->camino.push_back(0);
    //   //      it->camino.push_back(contador);
    // }
  }


  Estructura_C_i recorre_C_i;

#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
  if (objetivo.semifonema[0] == '#')
    recorre_C_i.indice_contorno = CONTORNO_SORDO;
  else
    recorre_C_i.indice_contorno = 0;
#else
  recorre_C_i.indice_contorno = CONTORNO_SORDO;
#endif

  C_i.push_back(recorre_C_i);

  if (mascara_contornos == NULL) {
    if (crea_vector_C_i(vector_coste, numero_candidatos, objetivo, 0, NULL))
      return 1;
  }
  else
    if (crea_vector_C_i_paralelo(vector_coste, numero_candidatos, mascara_contornos, objetivo, 0))
      return 1;

  // Liberamos la memoria reservada.

  //    free(vector_coste);
  free(delta_u);

#ifdef _DEPURA_VITERBI
  fclose(fichero_viterbi);
#endif

  return 0;
    
}


/**
 * Función:   siguiente_recursion_Viterbi                                              
 * \remarks Entrada:
 *			- objetivo: vector descriptor de la unidad que hay que sintetizar.         
 *          - última_unidad: == 1, si es la última unidad del grupo prosódico. == 0,   
 *            en otro caso.                                                            
 *          - mascara_contornos: en el caso de considerar varios contornos en paralelo, indica cuáles de esos contornos no se han podado
 *          - numero_maximo_contornos: en el caso de considerar varios contornos en paralelo, indica el tamano de mascara_contornos
 * \remarks Valor devuelto:                                                                     
 *          - costes_acumulados: array con los costes acumulados para cada contorno (para el cálculo en paralelo)
 *          - Devuelve 0 en ausencia de errores.                                       
 * \remarks Objetivo:  Ejecuta una iteración del algoritmo de Viterbi, actualizando los caminos 
 *            óptimos para cada unidad candidata.                                      
 */

int Viterbi::siguiente_recursion_Viterbi(Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad, char *mascara_contornos, int numero_maximo_contornos, float *costes_acumulados) {

  Vector_semifonema_candidato **delta_u;
  int numero_candidatos;
  int contador;

#ifdef _DEPURA_VITERBI
  iteracion++;
#endif
  //  fprintf(stderr, "En siguiente_recursion...\n");
 
#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
  unsigned char frase, posicion, acento;
  static unsigned char frase_anterior;
  static unsigned char posicion_anterior;
  static unsigned char acento_anterior;

  objetivo->devuelve_frase_posicion_acento(&frase, &posicion, &acento);

#endif

  if (izq_der == IZQUIERDO)
    izq_der = DERECHO;
  else
    izq_der = IZQUIERDO;

#ifdef _DEPURA_VITERBI
  char nombre_fichero[FILENAME_MAX];
  sprintf(nombre_fichero, "%s_%d_%s_%s.txt", nombre_fichero_depura, iteracion, objetivo.semifonema, objetivo.palabra);

  filtra_nombre_fichero(nombre_fichero);

  if ( (fichero_viterbi = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en siguiente_recursion_Viterbi, al abrir el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero_viterbi...

  if (izq_der == DERECHO)
    fprintf(fichero_viterbi, "Siguiente recursión: %d\t %s Derecho (%s).\n\n", iteracion, objetivo.semifonema, objetivo.palabra);
  else
    fprintf(fichero_viterbi, "Siguiente recursión: %d\t %s Izquierdo (%s).\n\n", iteracion, objetivo.semifonema, objetivo.palabra);
#endif

#ifdef PODA_C_t_POR_VALOR

  inicia_poda_C_t();

#endif

#ifdef PODA_C_i_POR_VALOR

  inicia_poda_C_i();

#endif

  // Creamos la cadena de unidades candidatas.

  if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
    return 1;

  if ( (ultima_unidad == 1) && (objetivo.semifonema[0] == '#') && (objetivo.contexto_derecho[0] == '#') )
    numero_candidatos = 1;

  vector_coste.clear();

#ifndef PODA_C_t_POR_NUMERO
  if (numero_candidatos > vector_coste.size()) {

    vector_coste.reserve(numero_candidatos);

    //    tamano_vector_coste = numero_candidatos;

  }
#endif

  // Calculamos el coste de distorsión de unidad de cada candidata.

  if (mascara_contornos) {
    if (crea_vector_C_t(vector_coste, &numero_candidatos, delta_u, objetivo, mascara_contornos))
      return 1;
  }
  else
    if (crea_vector_C_t(vector_coste, &numero_candidatos, delta_u, objetivo, NULL))
      return 1;

  //  muestra_contenido_vector_costes(vector_coste);

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
  //    if (calcula_frecuencia_costes_concatenacion(vector_coste, numero_candidatos))
  //        return 1;
  if (anhade_conjunto_unidades_coste_concatenacion(vector_coste->unidad, C_i, frase, posicion, acento, frase_anterior, posicion_anterior, acento_anterior))
    return 1;
#else
  if (mascara_contornos == NULL)
    calcula_coste_minimo_hasta_unidades(vector_coste, numero_candidatos, NULL, numero_maximo_contornos);
  else {
    calcula_coste_minimo_hasta_unidades_paralelo(vector_coste, numero_candidatos, mascara_contornos, numero_maximo_contornos);
  }
#endif

  // Actualizamos el vector de costes mínimos.

  if (mascara_contornos == NULL) {
    if (crea_vector_C_i(vector_coste, numero_candidatos, objetivo, ultima_unidad, NULL))
      return 1;
  }
  else
    if (crea_vector_C_i_paralelo(vector_coste, numero_candidatos, mascara_contornos, objetivo, ultima_unidad))
      return 1;
  


  // if ( (mascara_contornos != NULL) && (ultima_unidad) ) {

  //   if ( (C_i.size() >= N_PODA_PARALELO) && (sordo_sonoro(objetivo.semifonema) == SONORO) ) {

  //     char *nueva_mascara_contornos;
  //     if ( (nueva_mascara_contornos = (char *) malloc(numero_maximo_contornos*sizeof(char))) == NULL) {
  // 	fprintf(stderr, "Error en siguiente_recursion_Viterbi, al asignar memoria.\n");
  // 	return 1;
  //     }
      
  //     if (C_i[0].indice_contorno != CONTORNO_SORDO) {
	
  // 	float coste_poda = C_i[0].C_i + C_PODA_PARALELO; // Suponemos que C_i está ordenado, y que el mejor coste está en C_i[0].
	
  // 	vector<Estructura_C_i>::iterator C_i_final = C_i.end();
	
  // 	// Vemos qué contornos se pueden podar:
  // 	for (contador = 0; contador < numero_maximo_contornos; contador++)
  // 	  nueva_mascara_contornos[contador] = 0;
	
  // 	vector<Estructura_C_i>::iterator it;
	
  // 	for (it = C_i.begin(); it != C_i_final; it++) {
  // 	  nueva_mascara_contornos[it->indice_contorno] = 1;
  // 	  //	for (contador = 0; contador < 
  // 	}
	
  // 	// Comprobamos los caminos "no principales" (los que no son el mejor para cada unidad de C_i)
	
  // 	for (contador = 0; contador < numero_maximo_contornos; contador++) {
	  
  // 	  if ((*mascara_contornos)[contador] == 0)
  // 	    continue;
	  
  // 	  if (nueva_mascara_contornos[contador] == 1)
  // 	    continue;
	  
  // 	  for (it = C_i.begin(); it != C_i_final; it++) {
  // 	    if (it->datos_camino[contador].C_i < coste_poda) {
  // 	      nueva_mascara_contornos[contador] = 1;
  // 	      break;
  // 	    }
	    
  // 	  } // for (it = 0; ...
	  
  // 	} // for (contador = 0; contador < numero_maximo_contornos
	
  // 	free (*mascara_contornos);
  // 	*mascara_contornos = nueva_mascara_contornos;
	
  // 	//    }
  //     }
  //   }
  // 


// #ifdef _DEPURA_VITERBI
//   if (mascara_contornos != NULL)  {
//     fprintf(fichero_viterbi, "------> Caminos podados:");
//     for (contador = 0; contador < numero_maximo_contornos; contador++) {
//       if ((*mascara_contornos)[contador] == 0)
// 	fprintf(fichero_viterbi, "\t%d", contador);
//     } // for (contador = 0;
//     fprintf(fichero_viterbi, "\n");
//   }
// #endif

  // Liberamos la memoria reservada.

  free(delta_u);

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
  frase_anterior = frase;
  posicion_anterior = posicion;
  acento_anterior =acento;
#endif

#ifdef _DEPURA_VITERBI
  fclose(fichero_viterbi);
#endif

  return 0;

}

/**
 * Función:   devuelve_camino_optimo_sintesis                                          
 *	\remarks Salida:
 * \retval	numero_unidades: numero de unidades que componen el camino óptimo.
 * \retval	coste_minimo: coste de dicha secuencia de unidades.
 * \retval	camino_vectores: cadena de Vectores_síntesis.
 * \retval	numero_real_caminos: número real de caminos óptimos que se consideran (para
 * el caso en el que haya menos caminos en la matriz que los que se piden)
 * \return	En caso de error, se devuelve un uno.
 * \remarks Objetivo:  Devolver la secuencia de unidades que proporciona un camino de coste     
 *            mínimo en la síntesis de la frase objetivo.                              
 */

int Viterbi::devuelve_camino_optimo_sintesis(int *numero_unidades, float *coste_minimo, Vector_sintesis **camino_vectores, int *numero_real_caminos, int *indice_contorno) {

  int indice_minimo = 0;
  int contador = 0;
  estructura_matriz *camino_optimo;

  float minimo = FLT_MAX;
  //  Estructura_C_i *correcaminos = C_i;


  // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.
  vector<Estructura_C_i>::iterator C_i_final = C_i.end();

  for (vector<Estructura_C_i>::iterator it = C_i.begin(); it != C_i_final; it++) {
    if (it->C_i < minimo) {
      minimo = it->C_i;
      indice_minimo = contador;
    }
    contador++;
    //    correcaminos++;
  }

  if (coste_minimo)
    *coste_minimo = minimo;

  if (indice_contorno) {
    *indice_contorno = C_i[indice_minimo].indice_contorno;
  //  if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
    if ( (camino_optimo = caminos_optimos.devuelve_camino(indice_minimo, numero_unidades, numero_real_caminos, *indice_contorno)) == NULL)
      return 1;
  }
  else 
    if ( (camino_optimo = caminos_optimos.devuelve_camino(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
      return 1;

  
  if (convierte_a_cadena_vectores(camino_optimo, *numero_unidades, camino_vectores))
    return 1;
  
  free(camino_optimo);

  return 0;

}

/**
 * \remarks Poda los contornos que pasen de un cierto coste (cálculo de contornos en paralelo)
 * \param[in] mascara_contornos Array que muestra los caminos que se han podado y los que siguen considerándose
 * \param[in] costes_acumulados Array con los costes de cada camino
 * \param[in] numero_maximo_contornos Número de elementos de los arrays anteriores
 * \param[in] objetivo Vector descriptor objetivo
 * \return En ausencia de error, devuelve un cero.
 */
int Viterbi::poda_contornos_paralelo(char *mascara_contornos, float *costes_acumulados, int numero_maximo_contornos, Vector_descriptor_objetivo &objetivo) {

  int contador;
  float coste_poda = FLT_MAX;
  float mejor_coste = FLT_MAX;

  // Buscamos el coste mínimo:
  for (contador = 0; contador < numero_maximo_contornos; contador++) {

    if (mascara_contornos[contador] == 0)
      continue;

    if (costes_acumulados[contador] < mejor_coste) {
      mejor_coste = costes_acumulados[contador];
      coste_poda = mejor_coste + C_PODA_PARALELO;
    }
    else
      if (costes_acumulados[contador] > coste_poda)
	mascara_contornos[contador] = 0;

  } // for (contador = 0; contador < numero_maximo_contornos...

  // Eliminamos los contornos que se alejan demasiado del mejor caso
  for (contador = 0; contador < numero_maximo_contornos; contador++) {
    
    if (mascara_contornos[contador] == 0)
      continue;
    
    if (costes_acumulados[contador] > coste_poda)
      mascara_contornos[contador] = 0;
    
  }

#ifdef _DEPURA_VITERBI

  char nombre_fichero[FILENAME_MAX];
  sprintf(nombre_fichero, "%s_%d_%s_%s.txt", nombre_fichero_depura, iteracion, objetivo.semifonema, objetivo.palabra);
  
  filtra_nombre_fichero(nombre_fichero);
  
  if ( (fichero_viterbi = fopen(nombre_fichero, "at")) == NULL) {
    fprintf(stderr, "Error en siguiente_recursion_Viterbi, al abrir el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero_viterbi...

  if (mascara_contornos != NULL)  {
    fprintf(fichero_viterbi, "------> Costes acumulados:\n");
    for (contador = 0; contador < numero_maximo_contornos; contador++) {
      if (mascara_contornos[contador] == 0)
	fprintf(fichero_viterbi, "%d (%f - podado)\n", contador, costes_acumulados[contador]);
      else
	fprintf(fichero_viterbi, "%d (%f)\n", contador, costes_acumulados[contador]);
    }

   fprintf(fichero_viterbi, "\n------> Caminos podados:");
    for (contador = 0; contador < numero_maximo_contornos; contador++) {
      if (mascara_contornos[contador] == 0)
	fprintf(fichero_viterbi, "\t%d", contador);
    } // for (contador = 0;
    fprintf(fichero_viterbi, "\n");
  }

  fclose(fichero_viterbi);

#endif

  return 0;

}


/**
 * \remarks Esta función se emplea en el modo de selección de unidades dividida en segmentos, y con todos los contornos en paralelo
 * \param[in] indice_camino índice del contorno acentual
 * \param[in] mascara_contornos array de caracteres que indica qué caminos siguen en activo
 * \param[out]	numero_unidades numero de unidades que componen el camino óptimo
 * \param[out]	coste_minimo coste de dicha secuencia de unidades
 * \param[out]	camino_vectores cadena de Vectores_síntesis
 * \param[out] numero_real_caminos número real de caminos óptimos que se consideran (para el caso en el que haya menos caminos en la matriz que los que se piden)
 * \return	En caso de error, se devuelve un uno.
 * \remarks Objetivo:  Devolver la secuencia de unidades que proporciona un camino de coste mínimo en la síntesis de la frase objetivo, pero para el contorno indicado en los parámetros de entrada.                              
 */

int Viterbi::devuelve_camino_optimo_sintesis_paralelo(int *numero_unidades, float *coste_minimo, Vector_sintesis **camino_vectores, int *numero_real_caminos, int indice_camino, char *mascara_contornos) {

  int indice_minimo = 0;
  int contador = 0;
  estructura_matriz *camino_optimo;
  Vector_semifonema_candidato *correcaminos;

  float minimo = FLT_MAX;
  //  Estructura_C_i *correcaminos = C_i;


  // Calculamos el camino óptimo en esa iteración para el contorno "indice_camino" como aquél cuyo coste es mínimo en C_i.
  vector<Estructura_C_i>::iterator C_i_final = C_i.end();

  for (vector<Estructura_C_i>::iterator it = C_i.begin(); it != C_i_final; it++) {
    // if ( (it->indice_contorno != indice_camino) &&
    // 	 (it->indice_contorno != CONTORNO_SORDO) ) {
    //   contador++;
    //   continue;
    // }

    if (it->indice_contorno == CONTORNO_SORDO) {
      if (it->C_i < minimo) {
	minimo = it->C_i;
	indice_minimo = contador;
      }
    }
    else {
      if (it->datos_camino[indice_camino].C_i < minimo) {
	minimo = it->datos_camino[indice_camino].C_i;
	indice_minimo = contador;
      }
      
    }

    contador++;
    //    correcaminos++;
  }

  if (minimo == FLT_MAX) {
    // Ese camino ha sido podado
    mascara_contornos[indice_camino] = 0;
    return 0;
  }

  // if (indice_contorno)
  //   *indice_contorno = C_i[indice_minimo].indice_contorno;

  if (coste_minimo)
    *coste_minimo = minimo;
  
  //  if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
  if ( (camino_optimo = caminos_optimos.devuelve_camino(indice_minimo, numero_unidades, numero_real_caminos, indice_camino)) == NULL)
    return 1;
  
  if (convierte_a_cadena_vectores(camino_optimo, *numero_unidades, camino_vectores))
    return 1;

  free(camino_optimo);

  return 0;

}


/**
 * Función:   devuelve_camino_optimo_candidato                                          
 * \retval	numero_unidades: numero de unidades que componen el camino óptimo.
 * \retval	coste_minimo: coste de dicha secuencia de unidades.
 * \retval	camino_vectores: cadena de Vectores_síntesis.
 * \retval	numero_real_caminos: número real de caminos óptimos que la matriz puede
 * devolver (para el caso en el que haya menos que los que se buscan)
 * \return	En caso de error, se devuelve un uno.                                    
 * \remarks Objetivo:  Devolver la secuencia de unidades que proporciona un camino de coste     
 *            mínimo en la síntesis de la frase objetivo.                              
 */

int Viterbi::devuelve_camino_optimo_candidato(int *numero_unidades, float *coste_minimo, Vector_semifonema_candidato **camino_vectores, int *numero_real_caminos) {

  int indice_minimo = 0;
  int contador = 0;
  estructura_matriz *camino_optimo;

  float minimo = FLT_MAX;
  //  Estructura_C_i *correcaminos = C_i;


  // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.
  vector<Estructura_C_i>::iterator C_i_final = C_i.end();

  for (vector<Estructura_C_i>::iterator correcaminos = C_i.begin(); correcaminos != C_i_final; correcaminos++) {
    if (correcaminos->C_i < minimo) {
      minimo = correcaminos->C_i;
      indice_minimo = contador;
    }
    contador++;
  }

  if (coste_minimo)
    *coste_minimo = minimo;

  //  if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
  if ( (camino_optimo = caminos_optimos.devuelve_camino(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
    return 1;

  if (convierte_a_cadena_vectores(camino_optimo, *numero_unidades, camino_vectores))
    return 1;

  free(camino_optimo);

  return 0;

}

/**
 * Función:   devuelve_camino_optimo_candidato                                         
 * \retval	numero_unidades: numero de unidades que componen el camino óptimo.
 * \retval	numero_real_caminos: número real de caminos óptimos que hay en la matriz (para
 * el caso en el que haya menos que los que se buscan)
 * \return	Puntero a la cadena indexada de unidades. NULL, en el caso de que
 *            estuviesen sin inicializar las variables internas.                       
 * \remarks Objetivo:  Devolver la secuencia de unidades que proporciona un camino de coste     
 *            mínimo en la síntesis de la frase objetivo.                              
 * \remarks NOTA:      Se diferencia de la función anterior en que ésta devuelve una cadena de  
 *            vectores candidato, mientras que la otra la devolvía de vectores de      
 *            síntesis.                                                                
 */

Vector_semifonema_candidato * Viterbi::devuelve_camino_optimo_candidato(int *numero_unidades, int *numero_real_caminos) {

  int indice_minimo = 0;
  int contador = 0;
  estructura_matriz *camino_optimo;
  Vector_semifonema_candidato *camino_vectores;

  float minimo = FLT_MAX;

  // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.
  vector<Estructura_C_i>::iterator C_i_final = C_i.end();

  for (vector<Estructura_C_i>::iterator correcaminos = C_i.begin(); correcaminos != C_i_final; correcaminos++) {
    if (correcaminos->C_i < minimo) {
      minimo = correcaminos->C_i;
      indice_minimo = contador;
    }
    contador++;
  }

  //  if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
  if ( (camino_optimo = caminos_optimos.devuelve_camino(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
    return NULL;

  if (convierte_a_cadena_vectores(camino_optimo, *numero_unidades, &camino_vectores))
    return NULL;

  free(camino_optimo);

  return camino_vectores;

}


/**
 * Función:   libera_memoria_algoritmo                                                 
 * \remarks Objetivo:  Permite que el usuario libere la memoria empleada en los diferentes      
 *            pasos del algoritmo.                                                     
 */

void Viterbi::libera_memoria_algoritmo() {

  //  matriz.libera_memoria();
  caminos_optimos.libera_memoria();

  C_i.clear();

  // if (C_i != NULL) {

  //   free(C_i);
  //   C_i = NULL;
  //   tamano_C_i = 0;
  //   memoria_reservada_C_i = 0;
  // }

  vector_coste.clear();
  // if (vector_coste != NULL) {
  //   free(vector_coste);
  //   vector_coste = NULL;
  //   tamano_vector_coste = 0;
  // }
  /*
    if (fichero_c != NULL) {
    fclose(fichero_c);
    fichero_c = NULL;
    }
  */
#ifdef PODA_C_i_POR_NUMERO
  maxima_memoria_reservada = 0;
#endif

}

/**
 * Función:   escribe_cadena_escogida                                          
 * \param	numero_unidades: número de unidades de la cadena.
 * \param	nombre_fichero: nombre del fichero en el que se desea introducir
 *            la información.
 * \retval  numero_real_caminos: número real de caminos que la matriz puede devolver.
 * \return	En ausencia de error se devuelve un cero.
 */

int Viterbi::escribe_cadena_escogida(int numero_unidades, char *nombre_fichero, int *numero_real_caminos) {

  FILE *fichero;

  Vector_semifonema_candidato *cadena_escogida, *correcaminos;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_cadena_escogida, al intentar crear el fichero %s.\n",
            nombre_fichero);
    return 1;
  }

  if ( (cadena_escogida = devuelve_camino_optimo_candidato(&numero_unidades, numero_real_caminos)) == NULL)
    return 1;

  correcaminos = cadena_escogida;

  for (int contador = 0; contador < numero_unidades; contador++, correcaminos++)
    correcaminos->escribe_datos_txt(fichero);

  free(cadena_escogida);

  fclose(fichero);

  return 0;

}

#ifdef _CALCULA_INDICES
/**
 * Función:   especifica_umbrales                                              
 * \remarks Entrada:
 *			- umbr_duracion: umbral a partir del cual se realizará alguna	   
 *			  modoficación prosódica.										   		
 *            para modificar la duración de las unidades.                      
 *          - umbr_frecuencia: igual, pero referido a la frecuencia.           
 */

void Viterbi::especifica_umbrales(float umbr_duracion, float umbr_frecuencia) {

  umbral_duracion = umbr_duracion;
  umbral_frecuencia = umbr_frecuencia;

}

#endif


#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION


/**
 * Función:   anhade_conjunto_unidades_coste_concatenacion                     
 * \remarks Entrada:
 *			- unidad_actual: primera unidad de la iteración actual del         
 *            algoritmo de Viterbi.                                            
 *          - C_i: cadena de estructuras con la información del algoritmo      
 *            hasta la iteración anterior.                                     
 *          - frase: tipo de frase de la unidad actual.                        
 *          - posicion: posicion en la frase de la unidad actual.              
 *          - acento: acento de la unidad actual.                              
 *          - frase_anterior: tipo de frase de la unidad anterior.             
 *          - posicion_anterior: posición en la frase de la unidad anterior.   
 *          - acento_anterior: acento de la unidad anterior.                   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                                
 * \remarks NOTA:
 *			- La información de frase, posición y acento se refiere a las      
 *            buscadas, no a las que realmente tienen las unidades             
 *            seleccionadas.                                                   
 */

int Viterbi::anhade_conjunto_unidades_coste_concatenacion(Vector_semifonema_candidato *unidad_actual,
							  Estructura_C_i *C_i,
							  unsigned char frase,
							  unsigned char posicion,
							  unsigned char acento,
							  unsigned char frase_anterior,
							  unsigned char posicion_anterior,
							  unsigned char acento_anterior) {

  int fonema_1_anterior, fonema_2_anterior;
  int fonema_1_actual, fonema_2_actual;
  int tipo_unidad_anterior, tipo_unidad_actual;
  estructura_indices indice_anterior, indice_actual;
  estructura_unidades *puntero_unidades_anterior, *puntero_unidades_actual;
  unsigned int clave, clave1 = 0, clave2 = 0;
  int parametros;
  Vector_semifonema_candidato *unidad_anterior = C_i->unidad;

  tipo_unidad_anterior = conjunto_unidades(frase_anterior, posicion_anterior, acento_anterior);
  tipo_unidad_actual = conjunto_unidades(frase, posicion, acento);

  if (unidad_anterior->izquierdo_derecho == IZQUIERDO) {
    puntero_unidades_anterior = devuelve_puntero_a_unidades(unidad_anterior->contexto_izquierdo,
							    unidad_anterior->semifonema,
							    IZQUIERDO);

    if ( (fonema_1_anterior = devuelve_indice_fonema(unidad_anterior->contexto_izquierdo))
	 == -1)
      return 1;

    if ( (fonema_2_anterior = devuelve_indice_fonema(unidad_anterior->semifonema))
	 == -1)
      return 1;

  } // if (unidad_anterior->izquierdo_derecho == ...
  else {
    puntero_unidades_anterior = devuelve_puntero_a_unidades(unidad_anterior->semifonema,
							    unidad_anterior->contexto_derecho,
							    DERECHO);

    if ( (fonema_1_anterior = devuelve_indice_fonema(unidad_anterior->semifonema))
	 == -1)
      return 1;

    if ( (fonema_2_anterior = devuelve_indice_fonema(unidad_anterior->contexto_derecho))
	 == -1)
      return 1;

  } // else // DERECHO


  if (unidad_actual->izquierdo_derecho == IZQUIERDO) {

    puntero_unidades_actual = devuelve_puntero_a_unidades(unidad_actual->contexto_izquierdo,
							  unidad_actual->semifonema,
							  IZQUIERDO);

    if ( (fonema_1_actual = devuelve_indice_fonema(unidad_actual->contexto_izquierdo))
	 == -1)
      return 1;

    if ( (fonema_2_actual = devuelve_indice_fonema(unidad_actual->semifonema))
	 == -1)
      return 1;

  } // if (unidad_anterior->izquierdo_derecho == ...
  else {

    puntero_unidades_actual = devuelve_puntero_a_unidades(unidad_actual->semifonema,
							  unidad_actual->contexto_derecho,
							  DERECHO);

    if ( (fonema_1_actual = devuelve_indice_fonema(unidad_actual->semifonema))
	 == -1)
      return 1;

    if ( (fonema_2_actual = devuelve_indice_fonema(unidad_actual->contexto_derecho))
	 == -1)
      return 1;

  } // else // DERECHO


  if (puntero_unidades_anterior != NULL) {

    indice_anterior =
      puntero_unidades_anterior->puntero_a_unidades[tipo_unidad_anterior];

    if (indice_anterior.numero_unidades == 0)
      parametros = 1;
    else
      parametros = 0;

    parametros <<= 2;

  } // if (puntero_unidades_anterior != NULL)
  else {
    // Se trata de un difonema:

    parametros = 1;
    parametros <<= 3;

  } // else

  if (puntero_unidades_actual != NULL) {

    indice_actual =
      puntero_unidades_actual->puntero_a_unidades[tipo_unidad_actual];


    if (indice_actual.numero_unidades == 0)
      parametros |= 1;

  } // if (puntero_unidades_actual != NULL)
  else {

    // Se trata de un difonema:

    parametros |= 2;


  } // else

    // Construimos los parámetros:

  /*
  // Construimos la clave:

  clave = fonema_1_anterior;
  clave <<= 5;
  clave |= fonema_2_anterior;
  clave <<= 5;
  clave |= tipo_unidad_anterior;
  clave <<= 1;
  clave |= (unidad_anterior->izquierdo_derecho - '0');
  clave <<= 5;
  clave |= fonema_1_actual;
  clave <<= 5;
  clave |= fonema_2_actual;
  clave <<= 5;
  clave |= tipo_unidad_actual;
  clave <<= 1;
  clave |= (unidad_actual->izquierdo_derecho - '0');

  if (anhade_clave_concatenacion(clave, parametros))
  return 1;
  */

  // Construimos la clave:

  empaqueta_clave(fonema_1_anterior, fonema_2_anterior, tipo_unidad_anterior,
		  unidad_anterior->izquierdo_derecho, &clave1);

  clave = clave1 << 16;

  empaqueta_clave(fonema_1_actual, fonema_2_actual, tipo_unidad_actual,
		  unidad_actual->izquierdo_derecho, &clave2);

  clave |= clave2;
    
  if (anhade_clave_concatenacion(clave, parametros))
    return 1;

  return 0;

}


/**
 * Función:   anhade_clave_concatenacion                                       
 * \remarks Entrada:
 *			- clave: entero en el que se encuentra la información de la unidad.
 *          - parametros: entero en el que se encuentra la información         
 *            relativa a si el semifonema proviene de un difonema, o bien al   
 *            grupo de unidades que se han seleccionado de ese semifonema.     
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 */

int Viterbi::anhade_clave_concatenacion(unsigned int clave, int parametros) {

  clave_parametros nueva_clave;
  estructura_concatenacion *correcaminos, *anterior;
  estructura_concatenacion *nuevo_nodo;

  nueva_clave.clave = clave;
  nueva_clave.parametros = parametros;
    
  if (inicio_cache == NULL) {

    numero_bloques_cache = 1;

    if ( (bloques_cache = (estructura_concatenacion **) malloc(sizeof(estructura_concatenacion *)))
	 == NULL) {
      fprintf(stderr, "Error en anhade_nodo_a_cache, al asignar memoria.\n");
      return 1;
    } // if ( (bloques_cache = ...

    if ( (*bloques_cache = (estructura_concatenacion *)
	  malloc(TAMANO_BLOQUE_CACHE*sizeof(estructura_concatenacion)))
	 == NULL) {
      fprintf(stderr, "Error en anhade_nodo_a_cache, al asignar memoria.\n");
      return 1;
    } // if ( (*bloques_cache = ...

    memoria_reservada_cache = *bloques_cache;
    indice_bloque_actual = 1;
    inicio_cache = memoria_reservada_cache;
    numero_elementos_cache = 1;
    inicio_cache->clave_compleja.clave = clave;
    inicio_cache->clave_compleja.parametros = parametros;
    inicio_cache->numero_ocurrencias = 1;
    inicio_cache->siguiente = NULL;

    return 0;

  } // if (inicio_cache == NULL)

  anterior = inicio_cache;
  correcaminos = inicio_cache;

  while (correcaminos != NULL) {

    if (compara_claves_complejas(correcaminos->clave_compleja, nueva_clave) == -1) {
      anterior = correcaminos;
      correcaminos = correcaminos->siguiente;
      continue;
    } // if (compara_claves_complejas...

    if (compara_claves_complejas(correcaminos->clave_compleja, nueva_clave) == 0) {
      (correcaminos->numero_ocurrencias)++;
      return 0;
    } // if (compara_claves_complejas)

    break;

  } // while (correcaminos != NULL)

    // Tenemos que insertar el nuevo nodo:

  numero_elementos_cache++;

  if (numero_elementos_cache == TAMANO_BLOQUE_CACHE*numero_bloques_cache) {

    indice_bloque_actual = 0;

    numero_bloques_cache++;

    if ( (bloques_cache = (estructura_concatenacion **) realloc(bloques_cache,
								numero_bloques_cache*sizeof(estructura_concatenacion *)))
	 == NULL) {
      fprintf(stderr, "Error en anhade_nodo_a_cache, al reasignar memoria.\n");
      return 1;
    } // if ( (bloques_cache = ...

    if ( (bloques_cache[numero_bloques_cache - 1] =
	  (estructura_concatenacion *) malloc(TAMANO_BLOQUE_CACHE*sizeof(estructura_concatenacion)))
	 == NULL) {
      fprintf(stderr, "Error en anhade_nodo_a_cache, al reasignar memoria.\n");
      return 1;
    } // if ( (bloques_cache[numero_bloques_cache - 1] = ...

    memoria_reservada_cache = *(bloques_cache + numero_bloques_cache - 1);
  } // if (++numero_elementos_cache == indice_bloque_actual*


  nuevo_nodo = memoria_reservada_cache + indice_bloque_actual++;

  nuevo_nodo->clave_compleja = nueva_clave;
  nuevo_nodo->clave_compleja.parametros = parametros;
  nuevo_nodo->numero_ocurrencias = 1;

  if (correcaminos == inicio_cache) {
    nuevo_nodo->siguiente = inicio_cache;
    inicio_cache = nuevo_nodo;
  } // if (correcaminos == inicio_cache[indice]
  else {
    anterior->siguiente = nuevo_nodo;
    nuevo_nodo->siguiente = correcaminos;
  } // else

  return 0;

  /*
  // Lo añadimos al final:

  numero_elementos_cache++;

  if (numero_elementos_cache == TAMANO_BLOQUE_CACHE*numero_bloques_cache) {

  indice_bloque_actual = 0;

  numero_bloques_cache++;

  if ( (bloques_cache = (estructura_concatenacion **) realloc(bloques_cache,
  numero_bloques_cache*sizeof(estructura_concatenacion *)))
  == NULL) {
  puts("Error en anhade_nodo_a_cache, al reasignar memoria.");
  return 1;
  } // if ( (bloques_cache = ...

  if ( (bloques_cache[numero_bloques_cache - 1] =
  (estructura_concatenacion *) malloc(TAMANO_BLOQUE_CACHE*sizeof(estructura_concatenacion)))
  == NULL) {
  puts("Error en anhade_nodo_a_cache, al reasignar memoria.");
  return 1;
  } // if ( (memoria_reservada_cache = ...

  memoria_reservada_cache = *(bloques_cache + numero_bloques_cache - 1);
  } // if (++numero_elementos_cache == indice_bloque_actual*

  correcaminos = inicio_cache;

  while (correcaminos->siguiente != NULL)
  correcaminos = correcaminos->siguiente;

  nuevo_nodo = memoria_reservada_cache + indice_bloque_actual++;

  nuevo_nodo->clave_compleja.clave = clave;
  nuevo_nodo->clave_compleja.parametros = parametros;
  nuevo_nodo->numero_ocurrencias = 1;
  nuevo_nodo->siguiente = NULL;

  correcaminos->siguiente = nuevo_nodo;

  return 0;
  */
}


/**
 * Función:   imprime_frecuencia_costes_concatenacion                          
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero en el que se va a almacenar   
 *            la información de los costes de concatenación.                   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 * \remarks NOTA:      Escribe la información en un fichero de texto.                   
 */

int Viterbi::imprime_frecuencia_costes_concatenacion(char *nombre_fichero) {

  unsigned int clave;
  int parametros;
  char grupo_completo_1, grupo_completo_2;
  char difonema_1, difonema_2;
  char izq_der_1, izq_der_2;
  char fonema_1_1[OCTETOS_POR_FONEMA], fonema_1_2[OCTETOS_POR_FONEMA];
  char fonema_2_1[OCTETOS_POR_FONEMA], fonema_2_2[OCTETOS_POR_FONEMA];
  char frase_1, posicion_1, acento_1;
  char frase_2, posicion_2, acento_2;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en imprime_frecuencia_costes_concatenacion, al intentar abrir \
            el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  estructura_concatenacion *correcaminos = inicio_cache;

  while (correcaminos != NULL) {

    // Primero desempaquetamos la clave:

    clave = correcaminos->clave_compleja.clave;
    parametros = correcaminos->clave_compleja.parametros;

    if (desempaqueta_clave(clave, parametros, &izq_der_2, &acento_2, &posicion_2,
			   &frase_2, fonema_2_1, fonema_2_2, &grupo_completo_2, &difonema_2))
      return 1;

    clave >>= 16;
    parametros >>= 2;

    if (desempaqueta_clave(clave, parametros, &izq_der_1, &acento_1, &posicion_1,
			   &frase_1, fonema_1_1, fonema_1_2, &grupo_completo_1, &difonema_1))
      return 1;

    fprintf(fichero, "%s\t%s\t\%c\t%c\t%c\t%c\t%c\t%c\t\t", fonema_1_1, fonema_1_2, frase_1, posicion_1,
	    acento_1, izq_der_1, grupo_completo_1, difonema_1);
    fprintf(fichero, "%s\t%s\t\%c\t%c\t%c\t%c\t%c\t%c\t\t", fonema_2_1, fonema_2_2, frase_2, posicion_2,
	    acento_2, izq_der_2, grupo_completo_2, difonema_2);
    fprintf(fichero, "%d\n", correcaminos->numero_ocurrencias);

    correcaminos = correcaminos->siguiente;
  }

  fclose(fichero);

  return 0;

}


/**
 * Función:   imprime_frecuencia_costes_concatenacion_bin                      
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero en el que se va a almacenar   
 *            la información de los costes de concatenación.                   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 * \remarks NOTA:      Escribe la información en formato binario.                       
 */

int Viterbi::imprime_frecuencia_costes_concatenacion_bin(char *nombre_fichero) {

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, "wb")) == NULL) {
    fprintf(stderr, "Error en imprime_frecuencia_costes_concatenacion_bin, al intentar abrir \
            el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...


  fwrite(&numero_elementos_cache, sizeof(int), 1, fichero);

  estructura_concatenacion *correcaminos = inicio_cache;

  while (correcaminos != NULL) {

    fwrite(&correcaminos->clave_compleja, sizeof(clave_parametros), 1, fichero);
    fwrite(&correcaminos->numero_ocurrencias, sizeof(int), 1, fichero);

    correcaminos = correcaminos->siguiente;

  } // while (correcaminos != NULL)

  fclose(fichero);

  return 0;

}


/**
 * Función:   borra_cache_frecuencia_costes                                    
 * \remarks Objetivo:                                                                   
 *          - Liberar la memoria reservada para la memoria caché de costes de  
 *            concatenación más comunes.                                        
 */

void Viterbi::borra_cache_frecuencia_costes() {

  int contador;

  inicio_cache = NULL;
  numero_elementos_cache = 0;
  indice_bloque_actual = 0;
  memoria_reservada_cache = NULL;

  if (bloques_cache != NULL) {

    for (contador = 0; contador < numero_bloques_cache; contador++)
      free(bloques_cache[contador]);

    free(bloques_cache);
    bloques_cache = NULL;

  } // if (bloques_cache != NULL)


}


/**
 * Función:   compara_claves_complejas                                         
 * \remarks Entrada:
 *			- operando1: primer operando.                                      
 *          - operando2: segundo operando.                                     
 * \remarks Valor devuelto:                                                             
 *          - 1: si operando1 > operando2.                                     
 *          - 0: si son iguales.                                               
 *          - -1: si operando2 > operando1.                                    
 * \remarks Objetivo:                                                                   
 *          - Comparar los valores de las dos claves para poder ordenar la     
 *            lista de claves.                                                            
 */

inline int Viterbi::compara_claves_complejas(clave_parametros operando1, clave_parametros operando2) {

  if (operando1.clave > operando2.clave)
    return 1;

  if (operando1.clave < operando2.clave)
    return -1;

  if (operando1.parametros > operando2.parametros)
    return 1;

  if (operando1.parametros < operando2.parametros)
    return -1;

  return 0;

}


/**
 * Función:   empaqueta_clave                                                  
 * \remarks Entrada:
 *			- clave: entero con la información empaquetada según la función    
 *            empaqueta_clave.                                                 
 *          - parametros: entero con la información de difonema y grupo de     
 *            unidades seleccionado.                                           
 *	\remarks Salida:
 *			- izq_der: IZQUIERDO o DERECHO.                                    
 *          - acento: tónico (1) o átono (0).                                  
 *          - posicion: inicial, media o final (0, 1 ó 2).                     
 *          - frase: enunciativa, exclamativa, interrogativa o inacabada (0-4).
 *          - fonema_1: identificador numérico del primer alófono.             
 *          - fonema_2: identificador numérico del segundo alófono.            
 *          - grupo_completo: si vale 1, indica que se cogieron todas las      
 *            copias del semifonema. Si  vale 0, sólo las del tipo             
 *            especificado por frase, posición y acento.                       
 *          - difonema: indica si la unidad provino de un difonema.            
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 */

int Viterbi::desempaqueta_clave(unsigned int clave, int parametros, char *izq_der, char *acento,
				char *posicion, char *frase, char *fonema_1, char *fonema_2,
				char *grupo_completo, char *difonema) {

  *izq_der = (char) ((clave & 0x1) + '0');
  clave >>= 1;

  *acento = (char) ((clave & 0x1) + '0');
  clave >>= 1;

  *posicion = (char) ((clave & 0x3) + '0');
  clave >>= 2;

  *frase  = (char) ((clave & 0x3) + '0');
  clave >>= 2;

  if (devuelve_nombre_fonema(clave & 31, fonema_2))
    return 1;
  clave >>=5;

  if (devuelve_nombre_fonema(clave & 31, fonema_1))
    return 1;

  *grupo_completo = (char) ((parametros & 0x1) + '0');

  parametros >>= 1;

  *difonema = (char) ((parametros & 0x1) + '0');

  return 0;

}


/**
 * Función:   empaqueta_clave                                                  
 * \remarks Entrada:
 *			- fonema_1: identificador numérico del primer alófono.             
 *          - fonema_2: identificador numérico del segundo alófono.            
 *          - tipo_unidad: entero en el que aparece empaquetada la información 
 *            de tipo de frase, posición y acento, según la macro              
 *            conjunto_unidades, definida en info_estructuras.hpp.             
 *          - izq_der: IZQUIERDO o DERECHO.                                    
 * \remarks Entrada y Salida:                                                           
 *          - clave: la clave con la información anterior empaquetada.         
 * \remarks NOTA:
 *			- Se tiene en cuenta el valor de entrada de la variable clave.      
 */

void Viterbi::empaqueta_clave(int fonema_1, int fonema_2, int tipo_unidad,
			      char izq_der, unsigned int *clave) {

  unsigned int clave_local = *clave;

  clave_local |= fonema_1;
  clave_local <<= 5;
  clave_local |= fonema_2;
  clave_local <<= 5;
  clave_local |= tipo_unidad;
  clave_local <<= 1;
  clave_local |= (izq_der - '0');

  *clave = clave_local;

}


#endif

/**
 * \author 
 * \remarks
 *
 * \param loc  
 *
 * \return 
 */
int Viterbi::inicializa(Locutor * locutor, char tipo_cand){

  locutor_actual = locutor;
  tipo_candidatos = tipo_cand;

  C_i.clear();

  // tamano_C_i = 0;
  //  memoria_reservada_C_i = 0;

  vector_coste.clear();

  //  vector_coste = NULL;
  //tamano_vector_coste = 0;

  izq_der = IZQUIERDO;

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION

  memoria_reservada_cache = NULL;
  numero_bloques_cache = 0;
  inicio_cache = NULL;
  numero_elementos_cache = 0;
  indice_bloque_actual = 0;
  bloques_cache = NULL;

#endif

#ifdef _DEPURA_VITERBI
  iteracion_contorno = -1;
  iteracion = -1;
#endif


#ifdef PODA_C_i_POR_NUMERO
  maxima_memoria_reservada = 0;
#endif

	
  return 0;
}


/**
 * \brief	Función que calcula el coste de coarticulación de un semifonema.
 * \remarks	Toma como objetivo la unidad del corpus que más se aproxima al contexto
 * fonético deseado.
 * \param[in]	vector_1	Primer semifonema.
 * \param[in]	vector_2	Segundo semifonema.
 * \return	El coste de coarticulación.
 */

float Viterbi::calcula_coste_coarticulacion(Vector_semifonema_candidato *vector_1, Vector_semifonema_candidato *vector_2) {

  // Hacer la prueba cambiando el cepstrum medio de los extremos por cepstrum_inicio y cepstrum_final.

  float coste = 0;

  coste += PESO_COSTE_SEMIFONEMA*calcula_distancia_euclidea(vector_1->cepstrum_mitad, vector_2->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales);
 
  //  coste += PESO_COSTE_TRIFONEMA*calcula_distancia_euclidea(vector_1->cepstrum_medio_trifonema, vector_2->cepstrum_medio_trifonema, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

  coste += PESO_COSTE_TRANSICION*calcula_distancia_euclidea(vector_1->cepstrum_inicial , vector_2->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

  coste += PESO_COSTE_OTRO_EXTREMO*calcula_distancia_euclidea(vector_1->cepstrum_final, vector_2->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

  return coste / Vector_semifonema_candidato::numero_coeficientes_cepstrales;
    
}

/**
 * \brief Función que decide el peso que se le da al coste de coarticulación según los fonemas circundantes
 * \param[in] objetivo Vector descriptor objetivo
 * \param[in] candidato Vector candidato
 * \return El peso asignado al coste
 */

float Viterbi::decide_peso_coarticulacion(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato) {

  unsigned char decidido = 0;
  float coste = 0.0;
  clase_espectral clase_objetivo, clase_candidato;


  if (candidato->izquierdo_derecho == IZQUIERDO) {
    if ( (strcmp(candidato->contexto_derecho, objetivo.contexto_derecho) == 0) &&
	 (strcmp(candidato->contexto_derecho_2, objetivo.contexto_derecho_2) == 0) &&
	 (strcmp(candidato->contexto_izquierdo_2, objetivo.contexto_izquierdo_2) == 0) )
      return 0;
  }
  else { // DERECHO
    if ( (strcmp(candidato->contexto_izquierdo, objetivo.contexto_izquierdo) == 0) &&
	 (strcmp(candidato->contexto_derecho_2, objetivo.contexto_derecho_2) == 0) &&
	 (strcmp(candidato->contexto_izquierdo_2, objetivo.contexto_izquierdo_2) == 0) )
      return 0;
  }
  

  if (candidato->izquierdo_derecho == IZQUIERDO) {
    if (strcmp(candidato->contexto_izquierdo, objetivo.contexto_izquierdo)) {
      tipo_fon_espectral_enumerado(candidato->contexto_izquierdo, &clase_candidato);
      tipo_fon_espectral_enumerado(objetivo.contexto_izquierdo, &clase_objetivo);
      if (clase_candidato == clase_objetivo)
	coste += 500000; // Sólo para la poda offline...
      else
	coste += 1000000;
    }

  } // if (candidato->izquierdo_derecho == IZQUIERDO)
  else { // DERECHO
    if (strcmp(candidato->contexto_derecho, objetivo.contexto_derecho)) {
      tipo_fon_espectral_enumerado(candidato->contexto_derecho, &clase_candidato);
      tipo_fon_espectral_enumerado(objetivo.contexto_derecho, &clase_objetivo);
      if (clase_candidato == clase_objetivo)
	coste += 500000;
      else
	coste += 1000000;
    }
  } // DERECHO

  if ( (candidato->tipo_espectral == FRIC_SOR_ESP) || (candidato->tipo_espectral == OCLU_SOR_ESP) || (objetivo.semifonema[0] == '#') )
    return coste;

  if (candidato->izquierdo_derecho == IZQUIERDO) {
    if ( (objetivo.contexto_izquierdo_2[0] == '#') || (candidato->contexto_izquierdo_2[0] == '#') )
      if (objetivo.contexto_izquierdo_2[0] != candidato->contexto_izquierdo_2[0])
	coste += 1;
    
    if ( (objetivo.contexto_derecho[0] == '#') &&
	 (candidato->contexto_derecho[0] == '#') &&
	 (objetivo.contexto_derecho_2[0] == '#') ) 
	  if (candidato->contexto_derecho_2[0] != '#')
	      coste += 10;
  } // if (candidato->izquierdo_derecho == IZQUIERDO)
  else { // DERECHO
    if ( (objetivo.contexto_izquierdo[0] != '#') && (candidato->contexto_izquierdo[0] != '#') ) {
      if ( (objetivo.contexto_izquierdo_2[0] == '#') || (candidato->contexto_izquierdo_2[0] == '#') )
	if (objetivo.contexto_izquierdo_2[0] != candidato->contexto_izquierdo_2[0])
	  coste += 1;
    }
    if ( (objetivo.contexto_derecho_2[0] == '#') || (candidato->contexto_derecho_2[0] == '#') )
      if (objetivo.contexto_derecho_2[0] != candidato->contexto_derecho_2[0])
	if (candidato->sonoridad == SONORO) 
	  coste += 20;
	else
	  coste += 5;
  }

  if (candidato->izquierdo_derecho == IZQUIERDO) {
    if (strcmp(candidato->contexto_derecho, objetivo.contexto_derecho) == 0) {
      coste += 1;
      decidido = 1;
    }
  }
  else {
    if (strcmp(candidato->contexto_izquierdo, objetivo.contexto_izquierdo) == 0) {
      coste += 1;
      decidido = 1;
    }
  }

  // if (candidato->izquierdo_derecho == IZQUIERDO)
  //   if (candidato->tipo_espectral == OCLU_SOR_ESP)
  //     return 0.5;

  if (decidido == 0) {

    if (candidato->izquierdo_derecho == IZQUIERDO) {
      // Consideramos el contexto por la derecha
      
      tipo_fon_espectral_enumerado(objetivo.contexto_derecho, &clase_objetivo);
      
      //      if (candidato->contexto_derecho[0] == '?')
      //	clase_candidato = SILENCIO_ESP;
      // else
      tipo_fon_espectral_enumerado(candidato->contexto_derecho, &clase_candidato);
      
      if (is_vowel(objetivo.contexto_derecho) == is_vowel(candidato->contexto_derecho)) {
	if (is_vowel(objetivo.contexto_derecho)) {
	  if (clase_candidato == clase_objetivo)
	    coste += 2;
	  else
	    coste += 3;
	} // if (is_vowel(objetivo.contexto_derecho)
	else { // Dos consonantes
	  if ( (is_silence_or_voiceless_plosive(objetivo.contexto_derecho) == 1) && (is_silence_or_voiceless_plosive(candidato->contexto_derecho) == 1) ) {
	    coste += 1;
	  }
	  else {
	    if (clase_candidato == clase_objetivo)
	      coste += 2; // += 5
	    else
	      if (sordo_sonoro(objetivo.contexto_derecho) == sordo_sonoro(candidato->contexto_derecho))
		coste += 10;
	      else
		if ( ( (clase_objetivo == OCLU_SOR_ESP) && (clase_candidato == OCLU_SON_ESP) ) || ( (clase_objetivo == OCLU_SON_ESP) && (clase_candidato == OCLU_SOR_ESP) ) )
		  coste += 20;
		else
		  coste += 30;
	  }
	}
      } // if (is_vowel(objetivo.contexto_derecho) == is_vowel...
      else { // Una es vocal, y la otra, consonante
	if (sordo_sonoro(objetivo.contexto_derecho) == sordo_sonoro(candidato->contexto_derecho))
	  coste += 20;
	else
	  coste += 40;
      }
    } // if (candidato->izquierdo_derecho == IZQUIERDO)
    else { // DERECHO
      // Consideramos el contexto por la izquierda
      
      tipo_fon_espectral_enumerado(objetivo.contexto_izquierdo, &clase_objetivo);
      
      //      if (candidato->contexto_izquierdo[0] == '?')
      //	clase_candidato = SILENCIO_ESP;
      // else
      tipo_fon_espectral_enumerado(candidato->contexto_izquierdo, &clase_candidato);
      
      if (is_vowel(objetivo.contexto_izquierdo) == is_vowel(candidato->contexto_izquierdo)) {
	if (is_vowel(objetivo.contexto_izquierdo)) {
	  if (clase_candidato == clase_objetivo)
	    coste += 2;
	  else
	    coste += 3;
	} // if (is_vowel(objetivo->contexto_izquierdo)
	else { // Dos consonantes
	  if (clase_objetivo == clase_candidato)
	    coste += 2; // += 5
	  else
	    if (sordo_sonoro(objetivo.contexto_izquierdo) == sordo_sonoro(candidato->contexto_izquierdo))
	      coste += 10;
	    else
	      coste += 30;
	}
      } // if (is_vowel(objetivo.contexto_izquierdo) == is_vowel(candidato->contexto_izquierdo
      else { // Una es vocal, y la otra, consonante
	if (sordo_sonoro(objetivo.contexto_izquierdo) == sordo_sonoro(candidato->contexto_izquierdo))
	  coste += 20;
	else
	  coste += 40;
      }
    } // if (candidato->izquierdo_derecho == DERECHO)

  } // if (decidido == 0)

  // Penalización para el caso de las nasales:
  
  if (sordo_sonoro(candidato->semifonema) == SONORO) {
    if (candidato->izquierdo_derecho == IZQUIERDO) {
      //      if (candidato->contexto_derecho[0] == '?')
      //	clase_candidato = SILENCIO_ESP;
      // else
      tipo_fon_espectral_enumerado(candidato->contexto_derecho, &clase_candidato);
      tipo_fon_espectral_enumerado(objetivo.contexto_derecho, &clase_objetivo);
    } // if (candidato->izquierdo_derecho == IZQUIERDO)
    else {
      //      if (candidato->contexto_izquierdo[0] == '?')
      //	clase_candidato = SILENCIO_ESP;
      // else {
      tipo_fon_espectral_enumerado(candidato->contexto_izquierdo, &clase_candidato);
      //  }
      tipo_fon_espectral_enumerado(objetivo.contexto_izquierdo, &clase_objetivo); 
    }
    
    if (clase_objetivo != clase_candidato)
      if ( (clase_objetivo == NASAL_ESP) || (clase_candidato == NASAL_ESP) )
	coste += 4; //8

    // if (candidato->izquierdo_derecho == DERECHO) {
    //   if (candidato->contexto_derecho[0] != '?')
    // 	tipo_fon_espectral_enumerado(candidato->contexto_derecho, &clase_candidato);
    //   else
    // 	clase_candidato = SILENCIO_ESP;

    //   tipo_fon_espectral_enumerado(objetivo.contexto_derecho_2, &clase_objetivo);

    //   if ( ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) ) &&
    // 	   ( (clase_candidato == VOC_AB_ESP) || (clase_candidato == VOC_MED_ESP) || (clase_candidato == VOC_CER_ESP) ) ) {
	
    // 	if (candidato->contexto_derecho_2[0] != '?')
    // 	  tipo_fon_espectral_enumerado(candidato->contexto_derecho_2, &clase_candidato);
    // 	else
    // 	  clase_candidato = SILENCIO_ESP;

    // 	if (clase_objetivo != clase_candidato)
    // 	  if ( (clase_objetivo == NASAL_ESP) || (clase_candidato == NASAL_ESP) )
    // 	    coste += 6;
    //   } // if ( ( (candidato->tipo_espectral

    // } // if (candidato->izquierdo_derecho == DERECHO)
   
  } // if (sordo_sonoro(candidato->semifonema) == SONORO)
    
  return coste;

}


/**
 * \brief Función que decide el peso que se le da al coste de coarticulación según las fronteras del fonema
 * \param[in] objetivo Vector descriptor objetivo
 * \param[in] candidato Vector candidato
 * \return El peso asignado al coste
 */

 float Viterbi::decide_peso_fronteras(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato) {

  float coste = 0.0;
  float coste_parte_izquierda, coste_parte_derecha;
  clase_espectral clase_objetivo, clase_candidato;
  clase_espectral tipo_fonema, tipo_fonema_contexto, tipo_fonema_contexto_izquierdo_candidato, tipo_fonema_contexto_derecho_candidato;

  tipo_fon_espectral_enumerado(candidato->contexto_izquierdo, &tipo_fonema_contexto_izquierdo_candidato);
  tipo_fon_espectral_enumerado(candidato->contexto_derecho, &tipo_fonema_contexto_derecho_candidato);
 
  if (candidato->izquierdo_derecho == IZQUIERDO) {
    coste_parte_izquierda = COSTE_FRONTERA_ADYACENTE;
    coste_parte_derecha = COSTE_FRONTERA_CONTRARIA;
  }
  else {
    coste_parte_izquierda = COSTE_FRONTERA_CONTRARIA;
    coste_parte_derecha = COSTE_FRONTERA_ADYACENTE;
  }

  if (objetivo.frontera_inicial != candidato->frontera_inicial) {
    
    tipo_fon_espectral_enumerado(objetivo.semifonema, &tipo_fonema);
    tipo_fon_espectral_enumerado(objetivo.contexto_izquierdo, &tipo_fonema_contexto);
    

    if ( 
#ifdef _CONSIDERA_SEMIVOCALES
	( (tipo_fonema_contexto == SEMI_VOCAL_ESP) && ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) )) ||
	( (tipo_fonema == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto == VOC_AB_ESP) || (tipo_fonema_contexto == VOC_MED_ESP) || (tipo_fonema_contexto == VOC_CER_ESP) ) ) ||
	( (tipo_fonema_contexto_izquierdo_candidato == SEMI_VOCAL_ESP) && ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) ) ) ||
	( (candidato->tipo_espectral == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto_izquierdo_candidato == VOC_AB_ESP) || (tipo_fonema_contexto_izquierdo_candidato == VOC_MED_ESP) || (tipo_fonema_contexto_izquierdo_candidato == VOC_CER_ESP) ) )
#else
	( (tipo_fonema_contexto == VOC_CER_ESP) && ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) ) ) ||
	( (tipo_fonema == VOC_CER_ESP) && ( (tipo_fonema_contexto == VOC_AB_ESP) || (tipo_fonema_contexto == VOC_MED_ESP) || (tipo_fonema_contexto == VOC_CER_ESP) ) ) ||
	( (tipo_fonema_contexto_izquierdo_candidato == VOC_CER_ESP) && ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) ) ) ||
	( (candidato->tipo_espectral == VOC_CER_ESP) && ( (tipo_fonema_contexto_izquierdo_candidato == VOC_AB_ESP) || (tipo_fonema_contexto_izquierdo_candidato == VOC_MED_ESP) || (tipo_fonema_contexto_izquierdo_candidato == VOC_CER_ESP) ) )

#endif 
	 )
      coste += 4*coste_parte_izquierda*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
    else
      if ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) || (candidato->tipo_espectral == SEMI_VOCAL_ESP) )
	coste += 3*coste_parte_izquierda*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
      else if (candidato->tipo_espectral == VIBRANTE_ESP)
	  coste += 2*coste_parte_izquierda*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
	else
	  coste += coste_parte_izquierda*abs(objetivo.frontera_inicial - candidato->frontera_inicial);
      
  } // if (objetivo.frontera_inicial != candidato->frontera_inicial)
  
  if (objetivo.frontera_final != candidato->frontera_final) {
    
    tipo_fon_espectral_enumerado(objetivo.semifonema, &tipo_fonema);
    tipo_fon_espectral_enumerado(objetivo.contexto_derecho, &tipo_fonema_contexto);
    
    if ( 
#ifdef _CONSIDERA_SEMIVOCALES
	( (tipo_fonema_contexto == SEMI_VOCAL_ESP) && ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) ) ) ||
	( (tipo_fonema == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto == VOC_AB_ESP) || (tipo_fonema_contexto == VOC_MED_ESP) || (tipo_fonema_contexto == VOC_CER_ESP) ) ) ||
	( (tipo_fonema_contexto_derecho_candidato == SEMI_VOCAL_ESP) && ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) ) ) ||
	( (candidato->tipo_espectral == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto_derecho_candidato == VOC_AB_ESP) || (tipo_fonema_contexto_derecho_candidato == VOC_MED_ESP) || (tipo_fonema_contexto_derecho_candidato == VOC_CER_ESP) ) ) 
#else
	( (tipo_fonema_contexto == SEMI_VOCAL_ESP) && ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) ) ) ||
	( (tipo_fonema == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto == VOC_AB_ESP) || (tipo_fonema_contexto == VOC_MED_ESP) || (tipo_fonema_contexto == VOC_CER_ESP) ) ) ||
	( (tipo_fonema_contexto_derecho_candidato == SEMI_VOCAL_ESP) && ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) ) ) ||
	( (candidato->tipo_espectral == SEMI_VOCAL_ESP) && ( (tipo_fonema_contexto_derecho_candidato == VOC_AB_ESP) || (tipo_fonema_contexto_derecho_candidato == VOC_MED_ESP) || (tipo_fonema_contexto_derecho_candidato == VOC_CER_ESP) ) ) 
#endif
	 )
      coste += 4*coste_parte_derecha*abs(objetivo.frontera_final - candidato->frontera_final);	
    else
      if ( (candidato->tipo_espectral == VOC_AB_ESP) || (candidato->tipo_espectral == VOC_MED_ESP) || (candidato->tipo_espectral == VOC_CER_ESP) || (candidato->tipo_espectral == SEMI_VOCAL_ESP) )
	coste += 3*coste_parte_izquierda*abs(objetivo.frontera_final - candidato->frontera_final);
      else if (candidato->tipo_espectral == VIBRANTE_ESP)
	coste += 2*coste_parte_derecha*abs(objetivo.frontera_final - candidato->frontera_final);
      else
	coste += coste_parte_derecha*abs(objetivo.frontera_final - candidato->frontera_final);
    
  } // if (objetivo.frontera_final != ...
  

  if (is_diptongo(tipo_fonema_contexto_izquierdo_candidato, candidato->tipo_espectral, candidato->frontera_inicial))
    if (candidato->izquierdo_derecho == IZQUIERDO) {
      if (candidato->contexto_derecho[0] != objetivo.contexto_derecho[0])
	if (sordo_sonoro(objetivo.contexto_derecho) == SORDO)
	  coste += 5;
	else
	  coste += 15;
    }
    else
      if (candidato->contexto_izquierdo[0] != objetivo.contexto_izquierdo[0])
	if (sordo_sonoro(objetivo.contexto_izquierdo) == SORDO)
	  coste += 5;
	else
	  coste += 15;

  if (is_diptongo(candidato->tipo_espectral, tipo_fonema_contexto_derecho_candidato, candidato->frontera_final))
    if (candidato->izquierdo_derecho == IZQUIERDO) {
      if (candidato->contexto_derecho[0] != objetivo.contexto_derecho[0])
	if (sordo_sonoro(objetivo.contexto_derecho) == SORDO)
	  coste += 5;
	else
	  coste  += 15;
    }
    else 
      if (candidato->contexto_izquierdo[0] != objetivo.contexto_izquierdo[0])
	if (sordo_sonoro(objetivo.contexto_izquierdo) == SORDO)
	  coste += 5;
	else
	  coste += 15;

  return coste;
  
 }


/**
 * \brief Función que decide el peso que se le da al coste de coarticulación según las fronteras del fonema
 * \param[in] frontera_objetivo frontera prosódica del fonema objetivo
 * \param[in] candidato Vector candidato
 * \return El peso asignado al coste
 */

 float Viterbi::decide_peso_frontera_prosodica(char frontera_objetivo, Vector_semifonema_candidato *candidato) {

   float coste = 0.0;

#ifdef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
    if (candidato->semifonema[0] != '#') {
#else
    if (candidato->sonoridad == SONORO) {
#endif
      
      if ( (frontera_objetivo == UNIDAD_FIN_FRASE) || (candidato->frontera_prosodica == UNIDAD_FIN_FRASE) ) {
	if (frontera_objetivo != candidato->frontera_prosodica)
	  coste += 10;
      }
      else
	if (fuerza_fronteras_entonativas == 0) { 
	  if ( (candidato->frontera_prosodica == UNIDAD_RUPTURA) &&
	    (frontera_objetivo != UNIDAD_RUPTURA) )
	    coste += 2;
	}
	else {
	  if (candidato->frontera_prosodica != frontera_objetivo)
	    coste += 20;
	}
    } // if (candidato->sonoridad == SONORO)
    // else { // SORDO
    //   if (frontera_objetivo != candidato->frontera_prosodica)
    // 	coste_inteligibilidad += 6;
    // }

   return coste;

 }


/**
 * \brief Función que decide el peso que se le da al coste de coarticulación según el tipo de frase y la posición del semifonema en ella
 * \param[in] objetivo Vector descriptor objetivo
 * \param[in] candidato Vector candidato
 * \return El peso asignado al coste
 */

 float Viterbi::decide_peso_posicion_y_tipo_frase(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato) {

   float coste = 0.0;
     
   if (candidato->sonoridad == SONORO) {
     // Suponemos que esto sólo tiene importancia para fonemas sonoros. Las fricativas y las oclusivas sordas "parecen" muy estables. En todo caso, al tener en cuenta la continuidad en el Viterbi, los fonemas sonoros deberían de tirar de los sordos que los rodean en la grabación original.

     if (objetivo.frase != candidato->frase) {

       if ( (objetivo.frase == FRASE_ENUNCIATIVA) && ( (candidato->frase == FRASE_EXCLAMATIVA) || (candidato->frase == FRASE_INTERROGATIVA) ) )
	 coste += 40;
       // }
     }

     if ( (objetivo.posicion == POSICION_INICIAL) && (candidato->posicion != POSICION_INICIAL) )
       if (objetivo.frase != candidato->frase)
	 coste += 3;
       else
	 coste += 1;
     
     if ( (objetivo.posicion != POSICION_INICIAL) && (candidato->posicion == POSICION_INICIAL) )
       if (objetivo.frase != candidato->frase)
	 coste += 3;
       else
	 coste += 1;
     
   } // if (candidato->sonoridad == SONORO)
   
   
   if ( (objetivo.posicion == POSICION_FINAL) && (candidato->posicion != POSICION_FINAL) )
     if (objetivo.frase != candidato->frase)
       coste += 3;
     else
       coste += 1;
   
   if ( (objetivo.posicion != POSICION_FINAL) && (candidato->posicion == POSICION_FINAL) )
     if (objetivo.frase != candidato->frase)
       coste += 3;
     else
       coste += 1;
   
   return coste;
   
 }


 void Viterbi::muestra_contenido_vector_costes(vector<Estructura_coste> &costes, FILE *fichero) {

  vector<Estructura_coste>::iterator it = costes.begin();
  vector<Estructura_coste>::iterator fin = costes.end();

  fprintf(fichero, "Siguiente iteración:\n");
  for (int contador = 0; it != fin; ++it, contador++)
    fprintf(fichero, "%d\t%d\t%f\t%d\n", contador, it->unidad->identificador, it->coste_poda, it->podable);
  fprintf(fichero, "\n");

}

void Viterbi::muestra_contenido_vector_C_i(vector<Estructura_C_i> &C_i, FILE *fichero) {

  vector<Estructura_C_i>::iterator it = C_i.begin();
  vector<Estructura_C_i>::iterator fin = C_i.end();

  fprintf(fichero, "Siguiente iteracion:\n");
  for (int contador = 0; it != fin; ++it, contador++)
    fprintf(fichero, "%d\t%d\t%f\t%d\n", contador, it->unidad->identificador, it->C_i, it->podable);

  fprintf(fichero, "\n");

}


#ifdef _DEPURA_VITERBI
void Viterbi::avanza_contorno() {
  iteracion_contorno++;
  iteracion = 0;
}
#endif
