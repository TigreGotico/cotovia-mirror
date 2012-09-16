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
 

#ifndef _MATRIZ_TEMPLATE_HPP

#define _MATRIZ_TEMPLATE_HPP

#include "viterbi_clases.hpp"

/**
 * \class	Matriz_struct
 * \brief	Basic class used in the Matriz_template
 * \remarks	contains the information of a node in the matrix of paths
 */
template <class T> class Matriz_struct {

	public:

    //! Pointer to the unit contained in the node
    T *unidad;

};


/**
 * \class	Matriz_template
 * \brief	Template class that stores the cost of the different paths in the
 * Viterbi algorithm
 */
template <class T> class Matriz_template {

	//! Current number of rows in the matrix. It can change in each iteration
	int filas;

    //! Number of columns. Its value is fixed at the beginning of the algorithm, and it coincides with the number of elements
    int columnas;

    //! Current column
    int columna_actual;

	//! Array of pointers to the beginning of each row
	Matriz_struct<T> **matriz;

    //! Matrix of optimum paths of the previous iteration
    Matriz_struct<T> **antigua_matriz;

    //! Number of rows of the previous matrix
    int antigua_filas;

    public:

    //! Basic constructor of the class
    Matriz_template() {
        matriz = NULL;
        antigua_matriz = NULL;
    }

	//! Allocates memory for the next execution of the algorithm, and frees the previously allocated one
    int inicializa_matriz(int numero_filas, int numero_columnas);

    //! Allocates memory for the next iteration, and frees the previously allocated one
    int reinicia_ciclo(int numero_filas);

    //! Updates number of rows
    int actualiza_numero_de_filas(int nuevo_numero_filas);

	//! Inserts a new unit in the matrix
    int introduce_elemento(int pos_fila, T *unidad, int posicion);

    //! Version of the preceding function that updates a column in the matrix
    int introduce_conjunto_elementos(Coste_Ci<T> *C_i, int numero_unidades);

    //! Returns a row
    Matriz_struct<T> *saca_fila(int fila, int *numero_unidades, int *numero_real_caminos);

    //! Updates the current column
    void avanza_columna();

    //! Returns current number of rows
    int devuelve_numero_filas();
    
	//! Frees the memory of the object
    void libera_memoria();

};

#endif
