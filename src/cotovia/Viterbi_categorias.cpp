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
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "categorias_analisis_morfosintactico.hpp"
#include "perfhash.hpp"
#include "modelo_lenguaje.hpp"
//#include "analisis_morfosintactico.hpp"
#include "info_corpus_voz.hpp"
#include "matriz.hpp"
#include "Viterbi_categorias.hpp"



/**
 * Función:   inicia_vector_C_i                                                        
 * \remarks Entrada:
 *			- palabra: lista de categorías posible para la primera palabra de la       
 *            frase.                                                                   
 * \remarks Valor devuelto:                                                                     
 *          - Devuelve 0 en ausencia de error.                                         
 * \remarks Objetivo:  iniciar el vector C_i de caminos óptimos.                                
 */

int Viterbi_categorias::inicia_vector_C_i(estructura_categorias *palabra) {

    unsigned char bi_grama[2];

    Estructura_C_i_categorias *recorre_C_i;
    unsigned char *recorre_categorias = (unsigned char *) palabra->categoria;

    tamano_C_i = palabra->numero_categorias;

#ifdef _PODA_C_i_POR_NUMERO_CATEGORIAS

    if ( (C_i = (Estructura_C_i_categorias *) malloc((TAMANO_MAXIMO_C_i + 1)*sizeof(Estructura_C_i_categorias))) == NULL) {
            puts("Error al asignar memoria.");
            return 1;
        } // if ( (C_i = (Estructura_C_i_categorias...

#else
    if ( (C_i = (Estructura_C_i_categorias *) malloc(tamano_C_i*sizeof(Estructura_C_i_categorias)))
         == NULL) {
         puts("Error en inicia_vector_C_i, al asignar memoria.");
         return 1;
    } // if ( (Estructura_C_i
#endif

    recorre_C_i = C_i;

    bi_grama[0] = (unsigned char) PUNT;

    for (int contador = 0; contador < tamano_C_i; contador++, recorre_C_i++, recorre_categorias++) {

        recorre_C_i->categorias[0] = (unsigned char) PUNT;
        recorre_C_i->categorias[1] = (unsigned char) *recorre_categorias;
        bi_grama[1] = (unsigned char) *recorre_categorias;
        recorre_C_i->C_i = modelo_lenguaje->calcula_prob_w2_dado_w1(bi_grama) +
                           palabra->probabilidad[*recorre_categorias];
    } // for (int contador...

    return 0;
}

/**
 * Función:   inicia_algoritmo_Viterbi_categorias                                      
 * \remarks Entrada:
 *			- palabra: lista de categorías posible para la primera palabra de la       
 *            frase.                                                                   
 *          - numero_elementos: número de elementos de la frase.                       
 * \remarks Valor devuelto:                                                                     
 *          - Devuelve 0 en ausencia de error.                                         
 * \remarks Objetivo:  Lleva a cabo la primera etapa del algoritmo de Viterbi, inicializando    
 *            las variables necesarias para las siguientes iteraciones.                
 */

int Viterbi_categorias::inicia_algoritmo_Viterbi_categorias(estructura_categorias *palabra,
											int numero_elementos) {

    int contador;
    unsigned char *categoria = (unsigned char *) palabra->categoria;

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS

    inicia_poda_C_i_categorias();

#endif

    if (inicia_vector_C_i(palabra))
        return 1;

    // Sumamos 1 por el PUNTO que añadimos al principio.

    if (matriz.inicializa_matriz(1, numero_elementos + 1, 5))
        return 1;

    if (matriz.introduce_elemento(0, PUNT, 0,0))
        return 1;

    matriz.avanza_columna();

    matriz.reinicia_ciclo(palabra->numero_categorias, (unsigned char) palabra->numero_categorias);

    for (contador = 0; contador < palabra->numero_categorias; contador++, categoria++)
       if (matriz.introduce_elemento(contador, *categoria, 0,0))
            return 1;

    return 0;

}

/**
 * Función:   siguiente_recursion_Viterbi_categorias                                   
 * \remarks Entrada:
 *			- palabra: lista de categorías posible para palabra actual de la frase.    
 *          - indice: posición de la palabra en la frase.                              
 * \remarks Valor devuelto:                                                                     
 *          - Devuelve 0 en ausencia de errores.                                       
 * \remarks Objetivo:  Ejecuta una iteración del algoritmo de Viterbi, actualizando las         
 *            probabilidades de cada secuencia de categorías.                          
 */

int Viterbi_categorias::siguiente_recursion_Viterbi_categorias(estructura_categorias *palabra,
									int indice) {

    Estructura_coste_categorias *recorre_vector_coste;
    Estructura_C_i_categorias *recorre_C_i;
    unsigned char *recorre_categorias = palabra->categoria;
    int contador, cuenta;

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS

    inicia_poda_C_i_categorias();

#endif

    tamano_vector_coste = tamano_C_i*palabra->numero_categorias;

    if ( (vector_coste = (Estructura_coste_categorias *) malloc(tamano_vector_coste*sizeof(Estructura_coste_categorias)))
              == NULL) {
              puts("Error en siguiente_recursion_Viterbi_categorias, al asignar memoria.");
              return 1;
        } // if ( (vector_coste...

    recorre_vector_coste = vector_coste;

    matriz.avanza_columna();

    for (contador = 0; contador < palabra->numero_categorias; contador++, recorre_categorias++)
        for (recorre_C_i = C_i, cuenta = 0; cuenta < tamano_C_i; cuenta++, recorre_C_i++) {
            calcula_probabilidad_secuencia(palabra->probabilidad[*recorre_categorias],
            *recorre_categorias, indice, recorre_C_i, cuenta, recorre_vector_coste);
            recorre_vector_coste++;
        } // for (recorre_C_i = ...

    if (crea_vector_C_i_categorias(vector_coste, tamano_vector_coste, indice,
                                  (unsigned char) palabra->numero_categorias))
        return 1;

    free (vector_coste);
    vector_coste = NULL;

    return 0;

}

/**
 * Función:   calcula_probabilidad_secuencia                                             
 * \remarks Entrada:
 *			- probabilidad: probabilidad de que la palabra actual tenga la categoría     
 *            considerada.                                                               
 *          - categoria: categoría posible de la palabra actual.                         
 *          - indice: posición de la palabra en la frase.                                
 *          - recorre_C_i: camino hasta el paso anterior.                                
 *          - posicion: índice de recorre_C_i en C_i.                                    
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_categorias *C_ii: estructura en la que se almacenan las   
 *            probabilidades máximas, la categoría de la palabra actual, la del paso     
 *            anterior que proporciona máxima probabilidad y el índice al camino óptimo. 
 */

void Viterbi_categorias::calcula_probabilidad_secuencia(float probabilidad, unsigned char categoria,
                                    int indice, Estructura_C_i_categorias *recorre_C_i,
                                    int posicion,
                                    Estructura_coste_categorias *C_ii) {

    unsigned char *n_grama;
    unsigned char nuevo_n_grama[TAMANO_MAXIMO_N_GRAMA];
    int contador;
    register float probabilidad_actual;


    n_grama = (unsigned char *) recorre_C_i->categorias;

    C_ii->categoria = categoria;
    C_ii->camino = posicion;

    if (indice > 3) {
        for (contador = 0; contador < 4; contador++)
            nuevo_n_grama[contador] = n_grama[contador + 1];
        nuevo_n_grama[4] = categoria;
        probabilidad_actual = probabilidad +
                            modelo_lenguaje->calcula_prob_w5_dados_w4_w3_w2_w1(nuevo_n_grama) +
    //                       calcula_prob_1_grama(categoria) +
                            recorre_C_i->C_i;
        C_ii->probabilidad = probabilidad_actual;
    } // if (indice > 3)
    else
        if (indice == 3) {
                 // Calculamos el coste total hasta la unidad considerada en el paso actual.
                n_grama[4] = categoria;
                probabilidad_actual = probabilidad +
                            modelo_lenguaje->calcula_prob_w5_dados_w4_w3_w2_w1(n_grama) +
    //                        calcula_prob_1_grama(categoria) +
                            recorre_C_i->C_i;
                C_ii->probabilidad = probabilidad_actual;
        } // if (indice > 2)
        else
            if (indice == 2) {
                    // Calculamos el coste total hasta la unidad considerada en el paso actual.
                    n_grama[3] = categoria;
                    probabilidad_actual = probabilidad +
                                modelo_lenguaje->calcula_prob_w4_dados_w3_w2_w1(n_grama) +
    //                            calcula_prob_1_grama(categoria) +
                                recorre_C_i->C_i;
                    C_ii->probabilidad = probabilidad_actual;
            } // if (indice == 2)
            else
                if (indice == 1) {
                        // Calculamos el coste total hasta la unidad considerada en el paso actual.
                        n_grama[2] = categoria;
                        probabilidad_actual = probabilidad +
                            modelo_lenguaje->calcula_prob_w3_dados_w2_w1(n_grama) +
//                            calcula_prob_1_grama(categoria) +
                            recorre_C_i->C_i;
                        C_ii->probabilidad = probabilidad_actual;
                } // if (indice == 1)
                else {
                        n_grama[1] = categoria;
                        probabilidad_actual = probabilidad +
                            modelo_lenguaje->calcula_prob_w2_dado_w1(n_grama) +
//                            calcula_prob_1_grama(categoria) +
                            recorre_C_i->C_i;
                        C_ii->probabilidad = probabilidad_actual;
                        C_ii->categoria = categoria;
                } // else

}

/**
 * Función:   crea_vector_C_i_categorias                                                  
 * \remarks Entrada:
 *			- *vector_coste: array con las probabilidades de las secuencias de categorías 
 *             más probables.                                                             
 *          - tamano: contiene la longitud de vector_coste.                               
 *          - indice: posición de la palabra actual dentro de la frase.                   
 *          - numero_categorias: número de categorías posibles de la palabra actual.      
 * \remarks Valor devuelto:                                                                        
 *           - Devuelve 0 si no se produce ningún error.                                  
 * \remarks Objetivo:  crea la cadena de caminos de probabilidad máxima hasta el paso actual del   
 *            algoritmo.                                                                  
 */

int Viterbi_categorias::crea_vector_C_i_categorias(Estructura_coste_categorias *vector_coste,
							int tamano, int indice, unsigned char numero_categorias) {

    int contador;
    int tamano_actual = 0;
    Estructura_coste_categorias *correcaminos = vector_coste;
    unsigned char *apunta_categorias;
    unsigned char n_grama[TAMANO_MAXIMO_N_GRAMA];
    unsigned char *apunta_n_grama;

#ifdef _PODA_C_i_CATEGORIAS
    int poda;
#endif

#ifndef _PODA_C_i_POR_NUMERO_CATEGORIAS

    if (C_i)
        free(C_i);

    // Le asignamos memoria de nuevo.
    if ( (C_i = (Estructura_C_i_categorias *) malloc(tamano*sizeof(Estructura_C_i_categorias))) == NULL) {
        puts("Error al asignar memoria.");
        return 1;
    }

#endif

    Estructura_C_i_categorias *recorre_C_i = C_i;

    // Vamos rellenando los campos de cada estructura del array.

    matriz.poda_matriz();

    for (contador = 0; contador < tamano; contador++) {
        // Leemos el último n-grama.

        if (matriz.camino_podable(correcaminos->camino)) {
            correcaminos++;
            continue;
        } // if (matriz.camino_podable...

        apunta_categorias = (unsigned char *) recorre_C_i->categorias;

        if (matriz.saca_fila(correcaminos->camino, TAMANO_MAXIMO_N_GRAMA, n_grama))
            return 1;

        apunta_n_grama = (unsigned char *) n_grama;

        if (indice > 2) {
            *apunta_categorias++ = *apunta_n_grama++;
            *apunta_categorias++ = *apunta_n_grama++;
            *apunta_categorias++ = *apunta_n_grama++;
            *apunta_categorias++ = *apunta_n_grama;
            *apunta_categorias = correcaminos->categoria;
//            *apunta_categorias = *n_grama;
        } // if (indice > 2)
        else
            if (indice == 1) {
                *apunta_categorias++ = *apunta_n_grama++;
                *apunta_categorias++ = *apunta_n_grama;
//                    *apunta_categorias = *n_grama;
                *apunta_categorias = correcaminos->categoria;
            } // if (indice == 1)
            else {
                *apunta_categorias++ = *apunta_n_grama++;
                *apunta_categorias++ = *apunta_n_grama++;
                *apunta_categorias++ = *apunta_n_grama;
                *apunta_categorias = correcaminos->categoria;
            }

        // El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
        recorre_C_i->camino = correcaminos->camino;
        // El coste mínimo para llegar hasta dicha unidad.
//        recorre_C_i->C_i = correcaminos->probabilidad_acumulada;
        recorre_C_i->C_i = correcaminos->probabilidad;
        tamano_actual++;
#ifdef _PODA_C_i_CATEGORIAS
       // Comprobamos si hay que realizar algún tipo de poda.
       if ( ( poda = ordena_y_poda_C_i_categorias(C_i, tamano_actual)) != 0)
          tamano_actual -= poda;
       else
#endif
           recorre_C_i++;
        correcaminos++;
   }

   // Actualizamos el número de elementos de la cadena C_i.

   tamano_C_i = tamano_actual;


   // Actualizamos la matriz

   int ind;

   switch (indice) {
    case 0:
    case 1:
    case 2:
        ind = indice + 1;
        break;
    default:
        ind = 4;

   } // switch (indice)...

   if (matriz.reinicia_ciclo(tamano_C_i, numero_categorias))
    return 1;

   recorre_C_i = C_i;

   for (contador = 0; contador < tamano_C_i; contador++) {
       if (matriz.introduce_elemento(contador, recorre_C_i->categorias[ind], recorre_C_i->camino,
                                     recorre_C_i->C_i))
          return 1;
       recorre_C_i++;
   }
/*
    FILE *fichero;

    if ( (fichero = fopen("fichero_matriz.txt", "at")) == NULL) {
        puts("Error al intentar abrir el fichero fichero_matriz.txt.");
        return 1;
    }

    matriz.escribe_contenido_matriz(fichero);

    fclose(fichero);
*/
    return 0;

}

/**
 * Función:   devuelve_secuencia_optima                                                
 *	\remarks Salida:
 *			- numero_unidades: numero de unidades que componen el camino óptimo.       
 * \remarks Valor devuelto:                                                                     
 *          - Puntero a la cadena indexada de categorías. NULL, en el caso de que      
 *            estuviesen sin inicializar las variables internas.                       
 * \remarks Objetivo:  Devolver la secuencia de categorías de mayor probabilidad para la frase  
 *            de entrada.                                                              
 * \remarks NOTA:
 *			- La cadena indexada es de tipo unsigned char, y no Categorias_modelo.     
 */

unsigned char *Viterbi_categorias::devuelve_secuencia_optima(int *numero_unidades) {

    int indice_maximo = 0;
    float maximo = -1*FLT_MAX;
    unsigned char *secuencia = NULL;
    Estructura_C_i_categorias *correcaminos = C_i;


    // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.

    for (int contador = 0; contador < tamano_C_i; contador++) {
        if (correcaminos->C_i > maximo) {
           maximo = correcaminos->C_i;
           indice_maximo = contador;
        }
        correcaminos++;
    }

    if (matriz.saca_fila(indice_maximo, numero_unidades, &secuencia))
        return NULL;
    else
        return secuencia;

}

/**
 * Función:   libera_memoria_algoritmo_categorias                                      
 * \remarks Objetivo:  Permite que el usuario libere la memoria empleada en los diferentes      
 *            pasos del algoritmo.                                                     
 */

void Viterbi_categorias::libera_memoria_algoritmo_categorias() {

    matriz.libera_memoria();

    if (C_i != NULL) {
        free(C_i);
        C_i = NULL;
        tamano_C_i = 0;
    } // if (C_i != NULL)

    if (vector_coste != NULL) {
        free(vector_coste);
        vector_coste = NULL;
        tamano_vector_coste = 0;
    } // if (vector_coste != NULL

}

#ifdef _PODA_C_i_CATEGORIAS

/**
 * Función:   ordena_y_poda_C_i_categorias                                               
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - inicio: array ordenado según la probabilidad de sus elementos.             
 * \remarks Valor devuelto:                                                                       
 *           - Si no hay poda se devuelve un 0. Un 1 en caso contrario.                  
 * \remarks Objetivo:  ordena y realiza la poda del array por número.                             
 */

int Viterbi_categorias::ordena_y_poda_C_i_categorias(Estructura_C_i_categorias *inicio,
													 int posicion) {

    int poda_numero = 0, poda_valor = 0;
    Estructura_C_i_categorias *correcaminos = inicio;

    // La posición del nuevo elemento es la del tamaño - 1.

    posicion -= 1;

    register float probabilidad = correcaminos[posicion].C_i;

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS

    register float umbral = maximo_c_i + FRACCION_C_i;

    if (probabilidad < umbral)
       // no puede formar parte de la secuencia óptima. Lo eliminamos.
       return 1;

    // Comprobamos si el valor es menor que el mínimo actual.
    if (probabilidad > maximo_c_i) {
       umbral = probabilidad + FRACCION_C_i;
       maximo_c_i = probabilidad;
    }

#endif

    int contador = 0;

// Ordenamos el array.
// Esto lo tengo que cambiar por una búsqueda binaria.

    while ( (correcaminos->C_i > probabilidad) && (contador < posicion) ) {
          contador++;
          correcaminos++;
    }

    if (contador != posicion) {
       Estructura_C_i_categorias nuevo = inicio[posicion];

       correcaminos = inicio + posicion;

       for (int cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS
           if (correcaminos->C_i < umbral)
              poda_valor = posicion - cuenta;
#endif
           *correcaminos = *(correcaminos - 1);
           correcaminos--;
       }

       *correcaminos = nuevo;

    }

#ifdef _PODA_C_i_POR_NUMERO_CATEGORIAS
    if (posicion >= TAMANO_MAXIMO_C_i) // El >= viene del posicion -= 1.
//       poda_numero = posicion - N_PODA_UNIDAD;
        poda_numero = 1;
#endif

    // Devolvemos el valor que produce una poda mayor.

    if (poda_valor > poda_numero)
       return poda_valor;
    return poda_numero;

}

/**
 * Función:   inicia_poda_C_i_categorias                                                
 * \remarks Objetivo:  Inicializa la variable maximo_c_i, necesaria para el cálculo del umbral   
 *            para la poda por valor del array de secuencias de categorías de máxima    
 *            probabilidad.                                                             
 */

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS

void Viterbi_categorias::inicia_poda_C_i_categorias() {

     maximo_c_i = -1*FLT_MAX;

}

#endif


#endif

/**
 * \author
 * \remarks
 *
 * \param loc
 *
 * \return
 */

void Viterbi_categorias::inicializa(Modelo_lenguaje * mod_leng){

	C_i = NULL;
	tamano_C_i = 0;

	vector_coste = NULL;
	tamano_vector_coste = 0;

    modelo_lenguaje = mod_leng;
}


