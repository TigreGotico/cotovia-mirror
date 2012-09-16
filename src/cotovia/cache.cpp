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
 

#include <stdlib.h>
#include <stdio.h>

#include "cache.hpp"

/**
 * Función:   inicia_cache                                                      
 * Clase:     Cache                                                             
 * \remarks Entrada:
 *			- nombre_fichero: nombre del archivo, de tipo texto, en el que se   
 *            encuentra la información, con el siguiente formato:               
 *            [identificador] [número de elementos][\n]                         
 *            [id1] [coste1] [id2] [coste2] ...[\n] (con id1 < id2 <...)        
 * \remarks Valor devuelto:                                                              
 *          - En ausencia de error, devuelve un 0.                              
 * \remarks Objetivo:  Cargar en memoria el contenido de la memoria caché, a partir de   
 *            un fichero de texto, fácilmente manipulable por el usuario.       
 */

int Cache::inicia_cache(char *nombre_fichero) {

    FILE *fichero;
    int elementos;
    int contador;
    nodo_cache *correcaminos;
    nodo_lista *corre_lista;
    int numero_real = 0;
    numero_nodos = 100;

    // Abrimos el fichero donde se encuentra la información de la tabla caché.

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
       printf("Error al intentar abrir el fichero de caché %s.\n", nombre_fichero);
       return 1;
    }

    // Asignamos memoria para la tabla. Inicialmente tendrá espacio para 100 nodos, y si es preciso,
    // después le asignamos más.

    if ( (tabla_cache = (nodo_cache *) malloc(numero_nodos*sizeof(nodo_cache))) == NULL) {
       puts("Error al asignar memoria, en inicia_cache.");
       return 1;
    }

    // Vamos leyendo las entradas de la tabla. Dado que no todas las entradas tendrán el mismo tamaño,
    // no nos que da más remedio que ir leyéndolas una por una.

    correcaminos = tabla_cache;

    while (!feof(fichero)) {
          if (fscanf(fichero, "%d %d\n", &correcaminos->id_principal, &elementos) == EOF)
             break;           // Así eliminamos problemas de \n al final de la última línea.

          // Leemos el número de costes de concatenación almacenados relativos a la unidad actual.
          correcaminos->numero_elementos = elementos;

          // Asignamos memoria para dichos costes.
          if ( (correcaminos->lista = (nodo_lista *) malloc(elementos*sizeof(nodo_lista))) == NULL) {
             puts("Error al asignar memoria, en inicia_cache.");
             return 1;
          }

          // Vamos leyendo, uno por uno, los costes de concatenación, así como las unidades relacionadas.
          corre_lista = correcaminos->lista;
          for (contador = 0; contador < elementos; contador++) {
              fscanf(fichero, "%d %f", &corre_lista->id_secundario, &corre_lista->coste);
              corre_lista++;
          }

          // Comprobamos que no nos salimos de nuestra zona de memoria.
          if (++numero_real >= numero_nodos) {
             numero_nodos += 100;
             if ( (tabla_cache = (nodo_cache *) realloc(tabla_cache,
                               numero_nodos*sizeof(nodo_cache))) == NULL) {
                   puts("Error al reasignar memoria, en inicia_cache.");
                   return 1;
             }
             correcaminos = tabla_cache + numero_real;
          }
          else
              correcaminos++;
    }

    // Liberamos la memoria que no necesitamos.

    if (numero_real != numero_nodos) {
       numero_nodos = numero_real;
       if ( (tabla_cache = (nodo_cache *) realloc(tabla_cache,
                               numero_nodos*sizeof(nodo_cache))) == NULL) {
             puts("Error al reasignar memoria, en inicia_cache.");
             return 1;
       }
    }

    fclose(fichero);

    return 0;

}

/**
 * Función:   comprueba_existencia                                                     
 * Clase:     Cache.                                                                   
 * \remarks Entrada:
 *			- identificador1: identificador numérico (exclusivo) de la primera unidad. 
 *          - identificador2: identificador de la segunda unidad.                      
 * \remarks Valor devuelto:                                                                     
 *          - Si se encuentra en la caché, se devuelve el coste de concatenación de    
 *            ambas unidades. En caso contrario, se devuelve -1. (Hay que ver si       
 *            merece la pena meter en la caché unidades consecutivas (Cc == 0)).       
 * \remarks Objetivo:  Busca en la memoria caché el coste de concatenación entre dos unidades,  
 *            para aligerar la carga computacional del proceso.                        
 */

float Cache::comprueba_existencia(int identificador1, int identificador2) {

      nodo_cache *correcaminos;

      if (tabla_cache) {

         // Dado que las entradas de la caché están ordenadas por el índice del descriptor, llega con
         // comparar el identificador con el número de nodos de la caché para saber si el valor buscado
         // se puede encontrar en la memoria.
         if (identificador1 > numero_nodos)
            return -1;

         // Si es posible, buscamos en la lista correspondiente al identificador.

         correcaminos = &tabla_cache[identificador1];

         return busqueda_binaria(correcaminos->lista, correcaminos->numero_elementos, identificador2);
      }

      // Si la tabla no está inicializada, se devuelve un -1. Así podemos comprobar el funcionamiento
      // sin caché.
      
      return -1;

}

/**
 * Función:   busqueda_binaria                                                           
 * Clase:     Cache.                                                                     
 * \remarks Entrada:
 *			- nodo_inicial: puntero al inicio de la cadena de unidades en caché.         
 *          - elementos: número de unidades con las que ya está calculado el Coste de    
 *            concatenación con la unidad a la que pertenece *nodo_inicial.              
 *          - identificador: identificador de la unidad cuyo coste de concatenación con  
 *            la unidad original se quiere comprobar si está en la caché.                
 * \remarks Valor devuelto:                                                                       
 *          - Si se encuentra en la caché, se devuelve el coste de concatenación de      
 *            ambas unidades. En caso contrario, se devuelve -1.                         
 * \remarks Objetivo:  Busca en el nodo de la memoria caché perteneciente a la unidad generadora  
 *            de nodo_inicial si está almacenado el coste de concatenación con la unidad 
 *            identificador.                                                             
 * \remarks NOTA:      Se supone que nodo_lista está ordenada de forma creciente según los        
 *            identificadores.                                                           
 */

float Cache::busqueda_binaria(nodo_lista *nodo_inicial, int elementos, int identificador) {

      int punto_medio;
      int extremo_inferior = 0;
      int extremo_superior = elementos - 1;
      int id;
      char no_encontrado = 1;

      // Si el último elemento es menor que el identificador, entonces no puede estar en
      // la caché (ya que está ordenada de menor a mayor).
      if (nodo_inicial[elementos - 1].id_secundario < identificador)
         return -1;

      // Buscamos el elemento en la lista.

      while (no_encontrado) {

            punto_medio = (int) (extremo_inferior + extremo_superior) / 2;
            id = nodo_inicial[punto_medio].id_secundario;
            if (id == identificador) {
//               return nodo_lista[punto_medio].coste; // Esto es sólo para evitar el warning.
                 no_encontrado = 0;
                 continue;
            }

            if (id > identificador)
            // el nodo, si se encuentra en la caché, está antes.
               extremo_superior = punto_medio - 1;
            else
                 extremo_inferior = punto_medio + 1;

            if (extremo_superior < extremo_inferior)
               return -1;
      }

      return nodo_inicial[punto_medio].coste;

}


/*
int Cache::crea_fichero_cache(char *nombre_fichero_datos, char *nombre_fichero_salida) {

    FILE *fichero_entrada, *fichero_salida;
    nodo_cache *nodo;
    int tamano_nodo = sizeof(nodo_cache);

    if ( (nodo = (nodo_cache *) malloc(tamano_nodo)) == NULL) {
       puts("Error en crea_fichero_cache, al asignar memoria.");
       return 1;
    }

    if ( (fichero_datos = fopen(nombre_fichero_datos, "rt")) == NULL) {
       printf("Error en crea_fichero_cache, al intentar abrir el archivo %s.\n",
                     nombre_fichero_datos);
       return 1;
    }

    if ( (fichero_salida = fopen(nombre_fichero_salida, "wb")) == NULL) {
       printf("Error en crea_fichero_cache, al intentar crear el archivo %s.\n",
                     nombre_fichero_salida);
       return 1;
    }

    while (!feof(fichero_entrada)) {
          fscanf(fichero_entrada, "%d %d %f\n", &nodo->identificador1, &nodo->identificador2,
                                                &nodo->coste);
          if (fwrite(nodo, tamano_nodo, 1, fichero_salida) != tamano_nodo) {
             puts("Error en crea_fichero_cache, al escribir en el fichero de salida.");
             return 1;
          }
    }

    return 0;

}

*/


