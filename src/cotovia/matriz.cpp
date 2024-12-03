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

#include "info_corpus_voz.hpp"
#include "descriptor.hpp"
#include "matriz.hpp"


/**
 * Función:   Constructor de la clase Matriz
 * Clase:     Matriz                                                              
 */

Matriz::Matriz() {

    antigua_matriz = NULL;
    matriz = NULL;

}

Matriz::~Matriz() {
  if (antigua_matriz)
    free(antigua_matriz);

  if (matriz) {
    free(matriz[0]);
    free(matriz);
  }
}

/**
 * Función:   inicializa_matriz                                                   
 * Clase:     Matriz                                                              
 * \remarks Entrada:
 *			- numero_filas: número de unidades candidatas en ese paso.            
 *          - numero_columnas: número de unidades que hay que sintetizar.         
 * \remarks Valor devuelto:                                                                
 *          - En caso de error se devuelve un 1.                                  
 * \remarks Objetivo:  Reservar la memoria para el primer paso del algoritmo de Viterbi.   
 */

int Matriz::inicializa_matriz(int numero_filas, int numero_columnas) {

    if (antigua_matriz) {
      free(antigua_matriz);
      antigua_matriz = NULL;
    }

    if (matriz)
      free(matriz);

    antigua_filas = 0;
    columna_actual = 0;

    // Asignamos memoria al array de filas.

    if ( (matriz = (estructura_matriz **) malloc(numero_filas*sizeof(estructura_matriz *))) == NULL) {
       fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
       return 1;
    }

    // Asignamos memoria a cada fila del array.

    if ( (matriz[0] = (estructura_matriz *) malloc(numero_columnas*numero_filas*sizeof(estructura_matriz)))
          == NULL) {
          fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < numero_filas; contador++)
        matriz[contador] = matriz[0] + contador*numero_columnas;
/*
    for (int contador = 0; contador < numero_filas; contador++)
        if ( (matriz[contador] = (estructura_matriz *) malloc(numero_columnas*sizeof(estructura_matriz))) == NULL) {
           puts("Error al asignar memoria.");
           return 1;
        }
*/

    filas = numero_filas;
    columnas = numero_columnas;

    return 0;

}


/**
 * Función:   reinicia_ciclo                                                      
 * Clase:     Matriz.                                                             
 * \remarks Entrada:
 *			- numero_filas: número de unidades candidatas en el paso actual.      
 * \remarks Valor devuelto:                                                                
 *          - En caso de error se devuelve un 1.                                  
 * \remarks Objetivo:  Reservar la memoria necesaria para almacenar los caminos óptimos de 
 *            cada recursión del algoritmo de Viterbi, y liberar la que sobre.    
 */

int Matriz::reinicia_ciclo(int numero_filas) {

    // Si la antigua matriz es distinta de NULL, tenemos que liberar la memoria que habíamos
    // reservado para ella.

    if (antigua_matriz) {
        free(antigua_matriz[0]);
        free(antigua_matriz);
    }

    // Apuntamos antigua_matriz a matriz.

    antigua_matriz = matriz;

    // Asignamos memoria para la matriz nueva.

    if ( (matriz = (estructura_matriz **) malloc(numero_filas*sizeof(estructura_matriz *))) == NULL) {
       fprintf(stderr, "Error en reinicia_ciclo, al asignar memoria.\n");
       return 1;
    }

    if ( (matriz[0] = (estructura_matriz *) malloc(numero_filas*(columna_actual + 1)*sizeof(estructura_matriz)))
          == NULL) {
          fprintf(stderr, "Error en reinicia_ciclo, al asignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < numero_filas; contador++)
        matriz[contador] = matriz[0] + contador*(columna_actual + 1);


    antigua_filas = filas;

    filas = numero_filas;

    return 0;

}


/**
 * \brief Actualiza el número de filas de la matriz, para aquellos casos
 * en los que hay una poda posterior a la asignación de la matriz.
 * \param nuevo_numero_filas: nuevo número de filas.
 * \return En ausencia de error, un cero.
 */

int Matriz::actualiza_numero_de_filas(int nuevo_numero_filas) {

	filas = nuevo_numero_filas;

    if ( (matriz = (estructura_matriz **) realloc(matriz, filas*sizeof(estructura_matriz *))) == NULL) {
       fprintf(stderr, "Error en actualiza_numero_de_filas, al reasignar memoria.\n");
       return 1;
    }

    if ( (matriz[0] = (estructura_matriz *) realloc(matriz[0], filas*(columna_actual + 1)*sizeof(estructura_matriz)))
          == NULL) {
          fprintf(stderr, "Error en actualiza_numero_de_filas, al reasignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < filas; contador++)
        matriz[contador] = matriz[0] + contador*(columna_actual + 1);

	return 0;

}


/**
 * Función:   introduce_elemento
 * Clase:     Matriz.
 * \remarks Entrada:
 *			- pos_fila: fila de la matriz en la que se debe meter el elemento.
 *          - unidad: unidad que proporciona coste mínimo.
 *          - posicion: fila en la que se encuentra el identificador anterior.
 *          - izq_der: IZQUIERDO o DERECHO.                                       
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de errores se devuelve un 0.                            
 * \remarks Objetivo:  Actualizar el camino óptimo hasta la unidad actual.                 
 */

int Matriz::introduce_elemento(int pos_fila, Vector_semifonema_candidato *unidad, int posicion,
                               char izq_der) {

    int contador;
    estructura_matriz nuevo_elemento;
    estructura_matriz *recorre_nueva;
    estructura_matriz *recorre_antigua;

    // Comprobamos que las coordenadas no se salen de la zona reservada.

    if ( (pos_fila < 0) || (columna_actual < 0) || (columna_actual >= columnas) || (pos_fila >= filas) ) {
        fprintf(stderr, "Error en Matriz::introduce_elemento: posición no válida.\n");
        return 1;
    }

    // Leemos el camino óptimo hasta la unidad actual.

    if (antigua_matriz) {
       recorre_nueva = *(matriz + pos_fila);
       recorre_antigua = *(antigua_matriz + posicion);
       for (contador = 0; contador < columna_actual; contador++)
          *recorre_nueva++ = *recorre_antigua++;
    }

    // Añadimos la propia unidad al camino óptimo.

    nuevo_elemento.tipo_unidad = 0; // Indica que se trata de un semifonema
    nuevo_elemento.parametro_1 = izq_der;
    nuevo_elemento.parametro_2 = unidad->validez;
    nuevo_elemento.localizacion.vector = unidad;

    if (antigua_matriz)
        *recorre_nueva = nuevo_elemento;
    else
        matriz[pos_fila][columna_actual] = nuevo_elemento;

    return 0;

}


/**
 * Función:   introduce_conjunto_elementos                                        
 * Clase:     Matriz.                                                             
 * \remarks Entrada:
 *			- C_i: estructura con los caminos óptimos a cada unidad en una        
 *            recursión del algoritmo de Viterbi.                                 
 *          - numero_vectores: número de elementos de la cadena anterior.         
 *          - izq_der: Tipo de semifonema: IZQUIERDO, o DERECHO.                  
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de errores se devuelve un 0.                            
 * \remarks Objetivo:  Versión de la función anterior, que actualiza los caminos óptimos a 
 *            todas las unidades de una recursión del algoritmo de Viterbi.       
 */

int Matriz::introduce_conjunto_elementos(Estructura_C_i *C_i, int numero_vectores, char izq_der) {

    int contador;
    Estructura_C_i *recorre_C_i = C_i;
    Vector_semifonema_candidato *unidad;
    estructura_matriz nuevo_elemento;
    estructura_matriz *recorre_antigua;
    estructura_matriz *recorre_nueva;

    for (int cuenta = 0; cuenta < numero_vectores; cuenta++, recorre_C_i++) {

        // Leemos el camino óptimo hasta la unidad actual.

        if (antigua_matriz) {
            recorre_antigua = *(antigua_matriz + recorre_C_i->camino);
            recorre_nueva = *(matriz + cuenta);
            for (contador = 0; contador < columna_actual; contador++)
                *recorre_nueva++ = *recorre_antigua++;
        }

        // Añadimos la propia unidad al camino óptimo.

        unidad = recorre_C_i->unidad;

        nuevo_elemento.tipo_unidad = 0; // Indica que se trata de un semifonema
        nuevo_elemento.parametro_1 = izq_der;
        nuevo_elemento.parametro_2 = unidad->validez;
        nuevo_elemento.localizacion.vector = unidad;

        if (antigua_matriz)
            *recorre_nueva = nuevo_elemento;
        else
            matriz[cuenta][columna_actual] = nuevo_elemento;

    } // for (int cuenta = 0...

    return 0;

}


/**
 * Función:   introduce_elemento                                                  
 * Clase:     Matriz.                                                             
 * \remarks Entrada:
 *			- pos_fila: fila de la matriz en la que se debe meter el elemento.    
 *          - unidad: unidad que proporciona coste mínimo.                        
 *          - posicion: fila en la que se encuentra el identificador anterior.    
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de errores se devuelve un 0.                            
 * \remarks Objetivo:  Actualizar el camino óptimo hasta la unidad actual.                 
 * \remarks NOTA:      Sobrecarga la función anterior.                                     
 */

int Matriz::introduce_elemento(int pos_fila, Grupo_acentual_candidato *unidad, int posicion) {

    int contador;
    estructura_matriz nuevo_elemento;
    estructura_matriz *recorre_nueva;
    estructura_matriz *recorre_antigua;

    // Comprobamos que las coordenadas no se salen de la zona reservada.

    if ( (pos_fila < 0) || (columna_actual < 0) || (columna_actual >= columnas) || (pos_fila >= filas) ) {
        fprintf(stderr, "Error en Matriz::introduce_elemento: posición no válida.\n");
        return 1;
    }
    
    // Leemos el camino óptimo hasta la unidad actual.

    if (antigua_matriz) {
        recorre_nueva = *(matriz + pos_fila);
        recorre_antigua = *(antigua_matriz + posicion);
        for (contador = 0; contador < columna_actual; contador++)
            *recorre_nueva++ = *recorre_antigua++;
    }
    
    // Añadimos la propia unidad al camino óptimo.

    nuevo_elemento.localizacion.grupo = unidad;

    if (antigua_matriz)
        *recorre_nueva = nuevo_elemento;
    else
        matriz[pos_fila][columna_actual] = nuevo_elemento;

    return 0;

}


/**
 * Función:   introduce_conjunto_elementos                                        
 * Clase:     Matriz.                                                             
 * \remarks Entrada:
 *			- C_i: estructura con los caminos óptimos a cada unidad en una        
 *            recursión del algoritmo de Viterbi.                                 
 *          - numero_vectores: número de elementos de la cadena anterior.         
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de errores se devuelve un 0.                            
 * \remarks Objetivo:  Versión de la función anterior, que actualiza los caminos óptimos a 
 *            todas las unidades de una recursión del algoritmo de Viterbi.       
 * \remarks NOTA:      A diferencia de la otra función del mismo nombre, en este caso se   
 *            trabaja sobre estructuras de grupos acentuales.                     
 */

int Matriz::introduce_conjunto_elementos(Estructura_C_i_acentual *C_i, int numero_grupos) {

    int contador;
    Estructura_C_i_acentual *recorre_C_i = C_i;
    estructura_matriz nuevo_elemento, *recorre_nueva, *recorre_antigua;


    for (int cuenta = 0; cuenta < numero_grupos; cuenta++, recorre_C_i++) {

        // Leemos el camino óptimo hasta la unidad actual.

        if (antigua_matriz) {
            recorre_nueva = *(matriz + cuenta);
            recorre_antigua = *(antigua_matriz + recorre_C_i->camino);
            for (contador = 0; contador < columna_actual; contador++)
                *recorre_nueva++ = *recorre_antigua++;
        }

        // Añadimos la propia unidad al camino óptimo.

        nuevo_elemento.localizacion.grupo = recorre_C_i->unidad;

        if (antigua_matriz)
            *recorre_nueva = nuevo_elemento;
        else
            matriz[cuenta][columna_actual] = nuevo_elemento;

    } // for (int cuenta...

    return 0;

}


/**
 * Función:   saca_fila                                                           
 * Clase:     Matriz.                                                             
 * \param	fila: fila en la que se encuentra el camino óptimo  hasta la unidad
 *            actual.
 * \param	numero_unidades: número de unidades que componen el camino óptimo.
 * \param	numero_real_caminos: número real de caminos óptimos que se consideran
 * (para el caso en el que en la matriz haya menos de los que se buscan).
 * \return	En ausencia de error devuelve un puntero a la cadena indexada de
 *            unidades que proporcionan el coste mínimo. En otro caso, NULL       
 * \remarks Objetivo:  Obtener la secuencia de unidades que proporcionan el coste mínimo.  
 */

estructura_matriz  *Matriz::saca_fila(int fila, int *numero_unidades, int *numero_real_caminos) {

    // Comprobamos que el valor de la fila es válido.

    if (fila < 0) {
       fprintf(stderr, "Posición no válida en Matriz::saca_fila.");
       return NULL;
    }

    if (numero_real_caminos)
    	*numero_real_caminos = filas;

    *numero_unidades = columna_actual + 1;

    return matriz[fila];

}


/**
 * Función:   libera_memoria                                                      
 * Clase:     Matriz.                                                             
 * \remarks Objetivo:  Liberar la memoria reservada para las matrices.                     
 */

void Matriz::libera_memoria() {

   // Liberamos la memoria reservada para antigua_matriz.

   if (antigua_matriz) {
        free(antigua_matriz[0]);
        free(antigua_matriz);
   }

   antigua_matriz = NULL;

   // Liberamos la memoria reservada para matriz.

   if (matriz) {
        free(matriz[0]);
        free(matriz);
   }

    matriz = NULL;
}


/**
 * Función:   avanza_columna                                                           
 * Clase:     Matriz                                                                   
 * \remarks Objetivo:  Incrementar en una unidad la columna sobre la que se está trabajando.    
 */

void Matriz::avanza_columna() {

     columna_actual++;

}



/**
 * Función:   Constructor de la clase Matriz_categorias                           *
 * Clase:     Matriz_categorias                                                   
 */

Matriz_categorias::Matriz_categorias() {

    matriz = NULL;
    antigua_matriz = NULL;
    numero_categorias = NULL;
    caminos_poda = NULL;

}

/**
 * Función:   inicializa_matriz                                                   
 * Clase:     Matriz_categorias                                                   
 * \remarks Entrada:
 *			- numero_filas: número de unidades candidatas en ese paso.            
 *          - numero_columnas: número de unidades que hay que sintetizar.         
 *          - tamano_n_grama: tamaño del máximo n_grama considerado.              
 * \remarks Valor devuelto:                                                                
 *          - En caso de error se devuelve un 1.                                  
 * \remarks Objetivo:  Reservar la memoria para el primer paso del algoritmo de Viterbi.   
 */

int Matriz_categorias::inicializa_matriz(int numero_filas, int numero_columnas,
                       int tamano_n_grama) {

    n_grama = tamano_n_grama;

    if (antigua_matriz) {
      free(antigua_matriz);
      antigua_matriz = NULL;
    }

    if (matriz)
      free(matriz);

    if (caminos_poda) {
        free(caminos_poda);
        caminos_poda = NULL;
        tamano_caminos_poda = 0;
    }

    if (numero_categorias)
        free(numero_categorias);

    antigua_filas = 0;
    columna_actual = 0;

    columna_actual = 0;

    // Asignamos memoria al array de filas.

    if ( (matriz = (estructura_pesos **) malloc(numero_filas*sizeof(estructura_pesos *))) == NULL) {
       fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
       return 1;
    }

    // Asignamos memoria a cada fila del array.

    if ( (matriz[0] = (estructura_pesos*) malloc(numero_columnas*numero_filas*sizeof(estructura_pesos)))
          == NULL) {
          fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < numero_filas; contador++)
        matriz[contador] = matriz[0] + contador*numero_columnas;

    filas = numero_filas;
    columnas = numero_columnas;

    // Asignamos memoria al array de número de categorías posibles

    if ( (numero_categorias = (unsigned char *) malloc(columnas*sizeof(unsigned char)))
        == NULL) {
        fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
        return 1;
    }

    return 0;

}


/**
 * Función:   introduce_elemento                                                  
 * Clase:     Matriz_categorias.                                                  
 * \remarks Entrada:
 *			- pos_fila: fila de la matriz en la que se debe meter el elemento.    
 *          - categoria: categoría que proporciona coste mínimo.                  
 *          - posicion: fila en la que se encuentra el identificador anterior.    
 *          - pesos: coste hasta el momento.                                      
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error devuelve un cero.                              
 * \remarks Objetivo:  Actualizar el camino óptimo hasta la unidad actual.                 
 */

int Matriz_categorias::introduce_elemento(int pos_fila, unsigned char unidad, int posicion,
                                          float peso) {

    int contador;
    estructura_pesos *recorre_nueva, *recorre_antigua, nueva_unidad;

    // Comprobamos que las coordenadas no se salen de la zona reservada.

    if ( (pos_fila < 0) || (columna_actual < 0) || (columna_actual >= columnas) || (pos_fila >= filas) ) {
        fprintf(stderr, "Error en Matriz::introduce_elemento: posición no válida.\n");
        return -1;
    }

    nueva_unidad.categoria = unidad;
    nueva_unidad.peso = peso;

    // Leemos el camino óptimo hasta la unidad actual.

    if (antigua_matriz) {
        recorre_nueva = *(matriz + pos_fila);
        recorre_antigua = *(antigua_matriz + posicion);
        for (contador = 0; contador < columna_actual; contador++)
            *recorre_nueva++ = *recorre_antigua++;
    } // if (antigua_matriz...

    // Añadimos la propia unidad al camino óptimo.

    if (antigua_matriz)
        *recorre_nueva = nueva_unidad;
    else
        matriz[pos_fila][columna_actual] = nueva_unidad;

    return 0;

}


/**
 * Función:   saca_fila                                                           
 * Clase:     Matriz_categorias.                                                  
 * \remarks Entrada:
 *			- fila: fila en la que se encuentra el camino óptimo  hasta la unidad 
 *            actual.                                                             
 *			- tamano_maximo_n_grama: tamaño máximo del n-grama.					  	
 * Salida   - secuencia: secuencia de unidades almacenadas en la fila indicada.   
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error, devuelve un cero.                             
 * \remarks Objetivo:  Obtener la secuencia de categorías de mayor probabilidad.           
 */

int Matriz_categorias::saca_fila(int fila, int tamano_maximo_n_grama, unsigned char *secuencia) {

    unsigned char *recorre_fila;
    estructura_pesos *recorre_pesos;
    int numero_unidades = columna_actual;
    // Comprobamos que el valor de la fila es válido.

    if ( (fila < 0) || (fila >= filas) ) {
       fprintf(stderr, "Error en Matriz::saca_fila: posición no válida.\n");
       return 1;
    }

    recorre_fila = secuencia;

    if (numero_unidades >= tamano_maximo_n_grama) {
        recorre_pesos = matriz[fila] + numero_unidades - tamano_maximo_n_grama + 1;
        numero_unidades = tamano_maximo_n_grama - 1;
    }
    else
        recorre_pesos = matriz[fila];

    for (int contador = 0; contador < numero_unidades; contador++, recorre_pesos++)
        *recorre_fila++ = recorre_pesos->categoria;

    return 0;

}


/**
 * Función:   saca_fila                                                           
 * Clase:     Matriz_categorias.                                                  
 * \remarks Entrada:
 *			- fila: fila en la que se encuentra el camino óptimo  hasta la unidad 
 *            actual.                                                             
 *	\remarks Salida:
 *			- numero_unidades: número de unidades que componen el camino óptimo.  
 *          - secuencia: secuencia de unidades almacenadas en la fila indicada.   
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error, devuelve un cero.                             
 * \remarks Objetivo:  Obtener la secuencia de categorías de mayor probabilidad.           
 */

int Matriz_categorias::saca_fila(int fila, int *numero_unidades, unsigned char **secuencia) {

    unsigned char *recorre_fila;
    estructura_pesos *recorre_pesos;
    // Comprobamos que el valor de la fila es válido.

    if ( (fila < 0) || (fila >= filas) ) {
       fprintf(stderr, "Error en Matriz::saca_fila: posición no válida.\n");
       return 1;
    }

    *numero_unidades = columna_actual + 1;

    if (*secuencia == NULL)
        if ( (*secuencia = (unsigned char *) malloc(*numero_unidades*sizeof(unsigned char)))
              == NULL) {
            fprintf(stderr, "Error en saca_fila, al asignar memoria.\n");
            return 1;
        }
        
    recorre_fila = *secuencia;
    recorre_pesos = matriz[fila];

    for (int contador = 0; contador < *numero_unidades; contador++, recorre_pesos++)
        *recorre_fila++ = recorre_pesos->categoria;

    return 0;

}


/**
 * Función:   libera_memoria                                                      
 * Clase:     Matriz_categorias.                                                  
 * \remarks Objetivo:  Liberar la memoria reservada para las matrices.                     
 */

void Matriz_categorias::libera_memoria() {

    // Liberamos la memoria reservada para matriz.

    if (matriz) {
        free(matriz[0]);
        free(matriz);
        matriz = NULL;
    } // if (matriz...

    if (antigua_matriz) {
        free(antigua_matriz[0]);
        free(antigua_matriz);
        antigua_matriz = NULL;
    } // if (antigua_matriz...

    if (numero_categorias) {
        free(numero_categorias);
        numero_categorias = NULL;
    } // if (numero_categorias...

    if (caminos_poda) {
        free(caminos_poda);
        caminos_poda = NULL;
    } // if (caminos_poda)
    
}


/**
 * Función:   avanza_columna                                                           
 * Clase:     Matriz_categorias                                                        
 * \remarks Objetivo:  Incrementar en una unidad la columna sobre la que se está trabajando.    
 */

void Matriz_categorias::avanza_columna() {

     columna_actual++;

}

/**
 * Función:   reinicia_ciclo                                                      
 * Clase:     Matriz_categorias                                                   
 * \remarks Entrada:
 *			- numero_filas: número de unidades candidatas en el paso actual.      
 *          - categorias: número de categorías posibles de la palabra actual.     
 * \remarks Valor devuelto:                                                                
 *          - En caso de error se devuelve un 1.                                  
 * \remarks Objetivo:  Reservar la memoria necesaria para almacenar los caminos óptimos de 
 *            cada recursión del algoritmo de Viterbi, y liberar la que sobre.    
 */

int Matriz_categorias::reinicia_ciclo(int numero_filas, unsigned char categorias) {

    int contador;

    // Si la antigua matriz es distinta de NULL, tenemos que liberar la memoria que habíamos
    // reservado para ella.

    if (antigua_matriz) {
        free(antigua_matriz[0]);
        free(antigua_matriz);
    }

    // Apuntamos antigua_matriz a matriz.

    antigua_matriz = matriz;

    // Asignamos memoria para la matriz nueva.

    if ( (matriz = (estructura_pesos **) malloc(numero_filas*sizeof(estructura_pesos*))) == NULL) {
       fprintf(stderr, "Error en reinicia_ciclo, al asignar memoria.\n");
       return 1;
    } // if ( (matriz = ...


    if ( (matriz[0] = (estructura_pesos*) malloc(columnas*numero_filas*sizeof(estructura_pesos)))
        == NULL) {
       fprintf(stderr, "Error en reinicia_ciclo, al asignar memoria.\n");
       return 1;
    } // if ( (matriz[0] = ....

    for (contador = 1; contador < numero_filas; contador++)
        matriz[contador] = matriz[0] + contador*columnas;

    antigua_filas = filas;

    filas = numero_filas;

    numero_categorias[columna_actual] = categorias;
    
    return 0;

}


/**
 * Función:   poda_matriz                                                              
 * Clase:     Matriz_categorias                                                        
 * \remarks Objetivo:  Podar los caminos que no pueden formar parte de la ruta óptima.          
 * \remarks Valor devuelto:                                                                     
 *          - En ausencia de error devuelve un cero.                                   
 */

int Matriz_categorias::poda_matriz() {

    int columna_poda;
    int fila_inicio = 0, cuenta_seguidas = 0;
    int contador, cuenta_categorias = 0;
    float peso_actual = 0.0;
    unsigned char categoria_considerada = 255;
    estructura_pesos nodo_actual;
    unsigned char bandera_acaba = 0;

    if (columna_actual <= n_grama)
        return 0;

    columna_poda = columna_actual - n_grama;

    if (caminos_poda) {
        if (tamano_caminos_poda < filas) {
            free(caminos_poda);
            if ( (caminos_poda = (short int *) malloc(filas*sizeof(short int))) == NULL) {
                fprintf(stderr, "Error en poda_matriz, al asignar memoria.\n");
                return 1;
            }
        } // if (tamano_caminos_poda...
    } // if (caminos_poda)
    else {
        if ( (caminos_poda = (short int *) malloc(filas*sizeof(short int))) == NULL) {
            fprintf(stderr, "Error en poda_matriz, al asignar memoria.\n");
            return 1;
        }
        tamano_caminos_poda = filas;
    } // else

    if (filas != 1) {
        while ( (numero_categorias[columna_poda] == 1) && (cuenta_categorias++ < n_grama) )
            columna_poda++;
        if (cuenta_categorias == n_grama) {
            columna_poda = columna_actual - 1;
            peso_actual = matriz[0][columna_poda].peso;
            for (contador = 1; contador < filas; contador++) {
                nodo_actual = matriz[contador][columna_poda];
                if (nodo_actual.peso > peso_actual)
                    peso_actual = nodo_actual.peso;
            } // for (contador = 1...

            for (contador = 0; contador < filas; contador++) {
                nodo_actual = matriz[contador][columna_poda];
                if (nodo_actual.peso == peso_actual)
                    caminos_poda[contador] = (short int) contador;
                else
                    caminos_poda[contador] = (short int) -1;
            } // for (contador = 0; contador < filas...

            return 0;

        } // if (cuenta_categorias == n_grama)
    } // if (filas != 1)...
    else {
        caminos_poda[0] = 0;
        return 0;
    } // else

    for (contador = 0; contador < filas; contador++) {

       	if ( (cuenta_categorias == filas) && (bandera_acaba == 1))
        	break;
    
        nodo_actual = matriz[contador][columna_poda];

        if (categoria_considerada == 255) {
            fila_inicio = contador;
            cuenta_seguidas = 1;
            categoria_considerada = nodo_actual.categoria;
            peso_actual = matriz[contador][columna_actual - 1].peso;
            if (contador != filas - 1)
                continue;
        } // if (categoria_considerada...
        else
            if (nodo_actual.categoria == categoria_considerada) {
                cuenta_seguidas++;
                if (matriz[contador][columna_actual - 1].peso > peso_actual)
                    peso_actual = matriz[contador][columna_actual - 1].peso;
                if (contador != filas - 1)
                    continue;
            } // if (nodo_actual.peso > peso_actual;

        if ((contador != filas - 1) || (bandera_acaba == 0) ) {
            if (contador == filas - 1)
                bandera_acaba = 1;
            contador--;
        } // if ((contador != filas - 1...

        for (cuenta_categorias = fila_inicio; cuenta_categorias < fila_inicio + cuenta_seguidas;
                                                        cuenta_categorias++) {
            nodo_actual = matriz[cuenta_categorias][columna_poda];
//            if (nodo_actual.peso == peso_actual)
            if (matriz[cuenta_categorias][columna_actual - 1].peso == peso_actual)
                caminos_poda[cuenta_categorias] = (short int) cuenta_categorias;
            else
                caminos_poda[cuenta_categorias] = -1;
        } // for (cuenta_categorias = fila_inicio...

        categoria_considerada = 255;

    } // for (contador = 0; contador < filas...

    return 0;

}
/*
    for (cuenta_categorias = 0; cuenta_categorias < numero_categorias[columna_poda];
                                                    cuenta_categorias++) {

        for (contador = 0; contador < filas; contador++) {
            nodo_actual = matriz[contador][columna_poda];
            if (categoria_considerada == 255) {
                if (mascara_categorias[nodo_actual.categoria] == '0') {
                    categoria_considerada = nodo_actual.categoria;
                    peso_actual =-1*MAXFLOAT;
                } // if (mascara_categorias...
                else
                    continue;
            } // if (categoria_considerada...
            else
                if (nodo_actual.categoria != categoria_considerada)
                    continue;

            if (nodo_actual.peso <= peso_actual)
                continue;

            if (nodo_actual.peso > peso_actual) {
                peso_actual = nodo_actual.peso;
//                fila_escogida = contador;
            } // if (nodo_actual->peso > peso_actual)

        } // for (contador = 0; contador < filas...

        for (contador = 0; contador < filas; contador++) {
            nodo_actual = matriz[contador][columna_poda];
            if (nodo_actual.categoria != categoria_considerada)
                continue;

            if (nodo_actual.peso == peso_actual)
//                caminos_poda[contador] = (short int) fila_escogida;
                caminos_poda[contador] = (short int) contador;
            else
                caminos_poda[contador] = -1;
        } // for (contador = 0; contador < filas...

        mascara_categorias[categoria_considerada] = '1';
        categoria_considerada = 255;

    } // for (cuenta_categorias...

    return 0;
}
*/


/**
 * Función:   camino_podable                                                           
 * Clase:     Matriz_categorias                                                        
 * \remarks Objetivo:  Indicar al usuario si el camino al que se refiere puede formar parte de  
 *            la ruta óptima.                                                          
 * \remarks Valor devuelto:                                                                     
 *          - Devuelve 1 si el camino no puede formar parte de la ruta óptima, y 0 en  
 *            caso contrario.                                                          
 */

int Matriz_categorias::camino_podable(int posicion) {

    if (caminos_poda == NULL)
        return 0;

    if (caminos_poda[posicion] == -1)
        return 1;

    return 0;

}

/**
 * Función:   escribe_contenido_matriz                                                 
 * Clase:     Matriz_categorias                                                        
 * \remarks Objetivo:  Escribir en un fichero el contenido actual de la matriz de secuencias de 
 *            categorías óptimas.                                                      
 */

void Matriz_categorias::escribe_contenido_matriz(FILE *fichero) {

    estructura_pesos *apunta_filas;

    for (int cuenta_filas = 0; cuenta_filas < filas; cuenta_filas++) {
        apunta_filas = matriz[cuenta_filas];
        for (int cuenta_columnas = 0; cuenta_columnas < columna_actual + 1;
                                        cuenta_columnas++, apunta_filas++)
            if (apunta_filas->categoria < 10)
                fprintf(fichero, " %d ", apunta_filas->categoria);
            else
                fprintf(fichero, "%d ", apunta_filas->categoria);
        fprintf(fichero, "\t%f\n", (apunta_filas - 1)->peso);
    } // for (int cuenta_columnas = 0;

    fprintf(fichero, "\n");

}

/**
 * Función:   crea_parametros_estructura_matriz                               
 * \remarks Entrada:
 *			- izq_der: IZQUIERDO o DERECHO                                    
 *          - acento: tónico o átono.                                         
 *          - frase: tipo de frase.                                           
 *          - posicion: posición dentro de la frase.                          
 *			- vector: unidad cuya información se desea introducir en la 	  
 *			  matriz de caminos óptimos.									  
 *	\remarks Salida:
 *			- estructura: variable de tipo estructura_matriz con la 		  
 *			  información de acento, validez, posición, frase y localización  
 *            codificadas.													  
 * \remarks NOTA:	- Esta función sólo se emplea cuando la unidad proviene de un	  
 *			  difonema.														  
 */

void crea_parametros_estructura_matriz(char izq_der, char acento, char frase,
                                       char posicion,
                                       Vector_semifonema_candidato *vector,
                                       estructura_matriz *estructura) {


	char auxiliar = 0;

  	estructura->tipo_unidad = (char) (vector->validez | 16);

    estructura->localizacion.contador_posicion = vector->identificador;

	if (izq_der == IZQUIERDO) {
    	estructura->parametro_1 = (char) devuelve_indice_fonema(vector->contexto_izquierdo);
        estructura->parametro_2 = (char) devuelve_indice_fonema(vector->semifonema);
    } // if (izq_der == IZQQUIERDO)
    else {
        estructura->parametro_1 = (char) devuelve_indice_fonema(vector->semifonema);
        estructura->parametro_2 = (char) devuelve_indice_fonema(vector->contexto_derecho);
    } // else

   	auxiliar |= (char) ((posicion << 4) | (frase << 2) |
                       (acento << 1) | (izq_der - '0'));

    estructura->parametro_3 = auxiliar;

}

/**
 * Función:   devuelve_parametros_estructura_matriz                           
 * \remarks Entrada:
 *			- estructura: variable de tipo estructura_matriz en la que se 	  
 *			  encuentra la información de la unidad codificada.				  
 *	\remarks Salida:
 *			- izq_der: IZQUIERDO o DERECHO                                    
 *          - acento: tónico o átono.                                         
 *          - frase: tipo de frase.                                           
 *          - posicion: posición dentro de la frase.                          
 *          - validez: validez de la unidad.                                  
 *			- fonema_1: primer fonema del difonema.							  
 *			- fonema_2: segundo fonema del difonema.						  
 * 			- contador_posicion: contador de la unidad.						  	
 * \remarks NOTA:	- Esta función sólo se emplea cuando la unidad proviene de un	  
 *			  difonema.			                                              
 */

void devuelve_parametros_estructura_matriz(estructura_matriz estructura, char *izq_der,
										   char *acento, char *frase,
                                           char *posicion, char *validez, char **fonema_1,
                                           char **fonema_2, int *contador_posicion) {


    *izq_der = (char) ((estructura.parametro_3 & 0x01) + '0');
    *acento = (char) ((estructura.parametro_3 >> 1) & 0x01);
    *frase = (char) ((estructura.parametro_3 >> 2) & 0x03);
    *posicion = (char) ((estructura.parametro_3 >> 4) & 0x03);

    *validez = (char) (estructura.tipo_unidad & 15);

    *fonema_1 = devuelve_puntero_a_nombre_fonema(estructura.parametro_1);
    *fonema_2 = devuelve_puntero_a_nombre_fonema(estructura.parametro_2);

	*contador_posicion = estructura.localizacion.contador_posicion;
        
}
