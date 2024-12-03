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

#include "matriz_template.hpp"


/**
 * \class	Matriz_template
 * \brief	Allocates memory for the next execution of the algorithm, and frees the previously allocated one
 * \param[in]	numero_filas	number of rows
 * \param[in]	numero_columnas	number of columns
 * \return	0, in absence of errors
 */
template <class T> int Matriz_template<T>::inicializa_matriz(int numero_filas, int numero_columnas) {

    if (antigua_matriz) {
      free(antigua_matriz);
      antigua_matriz = NULL;
    }

    if (matriz)
      free(matriz);

    antigua_filas = 0;
    columna_actual = 0;

    // Asignamos memoria al array de filas.

    if ( (matriz = (Matriz_struct<T> **) malloc(numero_filas*sizeof(Matriz_struct<T> *))) == NULL) {
       fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
       return 1;
    }

    // Asignamos memoria a cada fila del array.

    if ( (matriz[0] = (Matriz_struct<T> *) malloc(numero_columnas*numero_filas*sizeof(Matriz_struct<T>)))
          == NULL) {
          fprintf(stderr, "Error en inicializa_matriz, al asignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < numero_filas; contador++)
        matriz[contador] = matriz[0] + contador*numero_columnas;

    filas = numero_filas;
    columnas = numero_columnas;

    return 0;

}

/**
 * \class	Matriz_template
 * \brief	Allocates memory for the next iteration, and frees the previously allocated one
 * \param[in]	numero_filas	number of rows
 * \return	0, in absence of errors
 */
template <class T> int Matriz_template<T>::reinicia_ciclo(int numero_filas) {

    // Si la antigua matriz es distinta de NULL, tenemos que liberar la memoria que habíamos
    // reservado para ella.

    if (antigua_matriz) {
        free(antigua_matriz[0]);
        free(antigua_matriz);
    }

    // Apuntamos antigua_matriz a matriz.

    antigua_matriz = matriz;

    // Asignamos memoria para la matriz nueva.

    if ( (matriz = (Matriz_struct<T> **) malloc(numero_filas*sizeof(Matriz_struct<T> *))) == NULL) {
       fprintf(stderr, "Error en reinicia_ciclo, al asignar memoria.\n");
       return 1;
    }

    if ( (matriz[0] = (Matriz_struct<T> *) malloc(numero_filas*(columna_actual + 1)*sizeof(Matriz_struct<T>)))
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
 * \class	Matriz_template
 * \brief	Updates number of rows
 * \param[in]	numero_filas	number of rows
 * \return	0, in absence of errors
 */
template <class T> int Matriz_template<T>::actualiza_numero_de_filas(int nuevo_numero_filas) {

	filas = nuevo_numero_filas;

    if ( (matriz = (Matriz_struct<T> **) realloc(matriz, filas*sizeof(Matriz_struct<T> *))) == NULL) {
       fprintf(stderr, "Error en actualiza_numero_de_filas, al reasignar memoria.\n");
       return 1;
    }

    if ( (matriz[0] = (Matriz_struct<T> *) realloc(matriz[0], filas*(columna_actual + 1)*sizeof(Matriz_struct<T>)))
          == NULL) {
          fprintf(stderr, "Error en actualiza_numero_de_filas, al reasignar memoria.\n");
          return 1;
    }

    for (int contador = 1; contador < filas; contador++)
        matriz[contador] = matriz[0] + contador*(columna_actual + 1);

	return 0;

}


/**
 * \class	Matriz_template
 * \brief	Inserts a new unit in the matrix
 * \param[in]	pos_fila	row of the matrix
 * \param[in]	unidad	new unit to be inserted
 * \param[in]	posicion	row of the preceding unit
 * \return	0, in absence of errors
 */
template <class T> int Matriz_template<T>::introduce_elemento(int pos_fila, T *unidad, int posicion) {

    int contador;
    Matriz_struct<T> nuevo_elemento;
    Matriz_struct<T> *recorre_nueva;
    Matriz_struct<T> *recorre_antigua;

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

    nuevo_elemento.unidad = unidad;

    if (antigua_matriz)
        *recorre_nueva = nuevo_elemento;
    else
        matriz[pos_fila][columna_actual] = nuevo_elemento;

    return 0;

}


/**
 * \class	Matriz_template
 * \brief	Inserts a set of units in the matrix
 * \param[in]	C_i
 * \param[in]	numero_unidades
 * \return	0, in absence of errors
 */
template <class T> int Matriz_template<T>::introduce_conjunto_elementos(Coste_Ci<T> *C_i, int numero_unidades) {

    int contador;
    Coste_Ci<T> *recorre_C_i = C_i;
    Matriz_struct<T> nuevo_elemento, *recorre_nueva, *recorre_antigua;

    for (int cuenta = 0; cuenta < numero_unidades; cuenta++, recorre_C_i++) {

        // Leemos el camino óptimo hasta la unidad actual.

        if (antigua_matriz) {
            recorre_nueva = *(matriz + cuenta);
            recorre_antigua = *(antigua_matriz + recorre_C_i->camino);
            for (contador = 0; contador < columna_actual; contador++)
                *recorre_nueva++ = *recorre_antigua++;
        }

        // Añadimos la propia unidad al camino óptimo.

        nuevo_elemento.unidad = recorre_C_i->unidad;

        if (antigua_matriz)
            *recorre_nueva = nuevo_elemento;
        else
            matriz[cuenta][columna_actual] = nuevo_elemento;

    } // for (int cuenta...

    return 0;

}


/**
 * \class	Matriz_template
 * \param[in]	fila	row of the matrix
 * \param[out]	numero_unidades	number of units comprising the path
 * \param[out]	numero_real_caminos	real number of paths in the matrix
 * \return	A pointer to the path. Null, in case of error	
 */
template <class T> Matriz_struct<T> *Matriz_template<T>::saca_fila(int fila, int *numero_unidades, int *numero_real_caminos) {

    // Comprobamos que el valor de la fila es válido.

    if (fila < 0) {
       fprintf(stderr, "Posición no válida en Matriz::saca_fila.");
       return NULL;
    }

    if (fila == filas - 1)
    	*numero_real_caminos = filas;

    *numero_unidades = columna_actual + 1;

    return matriz[fila];

}


/**
 * \class	Matriz_template
 * \brief	Updates the current column
 */
template <class T> void Matriz_template<T>::avanza_columna() {

     columna_actual++;

}


/**
 * \class	Matriz_template
 * \brief	returns the current number of rows
 * \return	number of rows
 */
template <class T> int Matriz_template<T>::devuelve_numero_filas() {

	return filas;

}


/**
 * \class	Matriz_template
 * \brief	Frees the memory of the object
 */
template <class T> void Matriz_template<T>::libera_memoria() {

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

