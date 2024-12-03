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
 

#ifndef _VITERBI_TEMPLATE_HPP

#define _VITERBI_TEMPLATE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "viterbi_clases.hpp"
#include "matriz_template.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"


#define absoluto(a) (((a) >= 0) ? (a) : -(a))
#define maximo(a, b) ( ((a) > (b)) ? a : b)

/**
 * \class Viterbi_template
 * \brief	template class that implements a generic Viterbi algorithm
 */
template <class T, class U> class Viterbi_template {

	protected:
    
	//! Pointer to the current speaker database
	Locutor *locutor_actual;

    //! Array with the units from the previous iteration
    Coste_Ci<T> *C_i;

    //! Number of elements of the preceding array
	int tamano_C_i;

    //! Current number of positions allocated for C_i
	int memoria_reservada_C_i;

	//! Array of units of the current iteration
    Coste_iteracion<T> *vector_coste;

    //! Number of elements of the preceding array
    int tamano_vector_coste;

    //! Matrix of optimum paths
	Matriz_template<T> matriz;

    //! Flag controlling the pruning of the array vector_coste by number
	bool poda_Ct_numero;

	//! Flag controlling the pruning of the array vector_coste by value
    bool poda_Ct_valor;

    //! Flag controlling the pruning of the array C_i by number
    bool poda_Ci_numero;

    //! Flag controlling the pruning of the array C_i by value
    bool poda_Ci_valor;

    //! Flag controlling the pruning of the array C_t
    bool poda_Ct;

    //! Flag controlling the pruning of the array C_i
    bool poda_Ci;

    //! Flag that indicates if the maximum amount of memory has been allocated (when pruning)
	bool maxima_memoria_reservada;

    //! Minimum current target cost (for pruning)
	float minimo_c_t;

    //! Minimum current accumulated cost (for pruning)
	float minimo_c_i;

    //! varibles that control pruning by value;
    float c_poda;
    float c_red;

    //! maximum number of units in vector_coste (pruning)
    int n_poda_unidad;

	//! maximum number of units in C_i (pruning)
    int n_poda_Ci;

    //! weight for the target cost
    float peso_coste_objetivo;

    //! weight for the concatenation cost
    float peso_coste_concatenacion;

	//! array used for returning the chosen paths
	bool *indices_escogidos;

    void inicia_poda_C_t();

    void inicia_poda_C_i();

	virtual int crea_delta_u(T ***cadena_id, int *tamano, U *objetivo) = 0;

	void calcula_coste_minimo_hasta_unidad(Coste_iteracion<T> *C_concatena);

	void calcula_coste_minimo_hasta_unidades(Coste_iteracion<T> *vector_coste,
    						int numero_candidatos);

	int crea_vector_C_t(Coste_iteracion<T> **costes, int *tamano, T **delta_u,
						U *objetivo);

	virtual float calcula_C_t(T *candidato, U *objetivo) = 0;

	virtual float calcula_C_c(T *unidad1, T *unidad2) = 0;

	int ordena_y_poda_C_t(Coste_iteracion<T> *actual, int posicion);

	int ordena_y_poda_C_i(Coste_Ci<T> *inicio, int posicion);

	int crea_vector_C_i(Coste_iteracion<T> *C_concatena, int tamano);

	int convierte_a_cadena_candidatos(Matriz_struct<T> *inicio, int tamano,
                                T **cadena_salida);

    public:

    //! Basic constructor
    Viterbi_template() {
    	poda_Ct_numero = false;
        poda_Ct_valor = false;
        poda_Ci_numero = false;
        poda_Ci_valor = false;
		poda_Ct = false;
        poda_Ci = false;
        indices_escogidos = NULL;
        peso_coste_objetivo = 0.5;
	    peso_coste_concatenacion = 0.5;

    }

    //! Constructor
    Viterbi_template(bool poda_Ct_n, bool poda_Ct_v, bool poda_Ci_n, bool poda_Ci_v) {
    	poda_Ct_numero = poda_Ct_n;
        poda_Ct_valor = poda_Ct_v;
        poda_Ci_numero = poda_Ci_n;
        poda_Ci_valor = poda_Ci_v;
		poda_Ct = poda_Ct_numero || poda_Ct_valor;
        poda_Ci = poda_Ci_numero || poda_Ci_valor;
        indices_escogidos = NULL;
        peso_coste_objetivo = 0.5;
	    peso_coste_concatenacion = 0.5;
    }

    //! initializes variables related to pruning thresholds
    void introduce_valores_poda(float c_p, int n_coste, int n_C_i);

    //! initializes the weights for target and concatenation costs
    void introduce_pesos_funciones_coste(float p_objetivo, float p_concatenacion);

    //! initializes some variables for an execution of the algorithm
	void inicializa(Locutor * locutor);

    //! implements the first iteration in the Viterbi algorithm, initialising some variables
    int inicia_algoritmo_Viterbi(U *objetivo, int numero_objetivos);

    //! implements an iteration of the Viterbi algorithm
    int siguiente_recursion_Viterbi(U *objetivo);

    //! returns the set of best candidate unit paths
    int devuelve_camino_optimo(unsigned char secuencia_optima, T **unidades,
    					int *numero_unidades, int *numero_real_caminos, float *coste);

    //! Frees the memory used during an interation of the algorithm
    void libera_memoria_algoritmo();

    //! Frees the memory used in the algorithm
    void finaliza();


};



/**
 * \brief	initializes variables related to pruning thresholds
 * \param[in]	c_p	threshold for pruning by value
 * \param[in]	n_coste	maximum number of units in an iteration
 * \param[in]	n_C_i	maximum number of paths from previous iteration
 */
template <class T, class U> void Viterbi_template<T,U>::introduce_valores_poda(float c_p, int n_coste, int n_C_i) {

	c_poda = c_p;
    c_red = 2*c_poda;
    n_poda_unidad = n_coste;
    n_poda_Ci = n_C_i;

}


/**
 * \brief	initilises the weights for target and concatenation costs
 * \param[in]	p_objetivo	weight of the target cost function
 * \param[in]	p_concatenacion	weight of the concatenation cost function
 */
template <class T, class U> void Viterbi_template<T,U>::introduce_pesos_funciones_coste(float p_objetivo, float p_concatenacion) {

	peso_coste_objetivo = p_objetivo;
    peso_coste_concatenacion = p_concatenacion;

}


/**
 * \class	Viterbi_template
 * \brief	initializes some variables for an execution of the algorithm
 * \param[in] locutor	pointer to the speaker database
 */
template <class T, class U> void Viterbi_template<T, U>::inicializa(Locutor * locutor){

	locutor_actual = locutor;

	C_i = NULL;
	tamano_C_i = 0;
	memoria_reservada_C_i = 0;

	vector_coste = NULL;
	tamano_vector_coste = 0;

	maxima_memoria_reservada = 0;


}


/**
 * \class	Viterbi_template
 * \brief	implements the first iteration in the Viterbi algorithm, initialising some variables
 * \param[in]	objetivo	array of target units
 * \param[in]	numero_objetivos	number of units of the array
 * \return	0, in absence of errors
 */
template <class T, class U> int Viterbi_template<T, U>::inicia_algoritmo_Viterbi(U *objetivo, int numero_objetivos) {

    // El vector descriptor se corresponde con el inicio de la frase, es decir, silencio con
    // contexto.

    T **delta_u;
    int numero_candidatos;
    int contador;
    Coste_iteracion<T> *correcaminos;

    if (poda_Ct_valor)
	    inicia_poda_C_t();

    if (poda_Ci_valor)
	    inicia_poda_C_i();

    // Creamos la cadena de unidades candidatas.

    if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
       return 1;

    // Calculamos el coste a cada unidad. En este caso, no se considera el coste de concatenación.

    if ( (vector_coste = (Coste_iteracion<T> *) malloc(numero_candidatos*sizeof(Coste_iteracion<T>)))
                       == NULL) {
         fprintf(stderr, "Error al asignar memoria en inicia_algoritmo_Viterbi.\n");
         return 1;
    }

    if (crea_vector_C_t(&vector_coste, &numero_candidatos, delta_u, objetivo))
       return 1;

    tamano_vector_coste = numero_candidatos;

    // Iniciamos la matriz que se emplea para memorizar los caminos de coste mínimo a cada unidad.

    if (matriz.inicializa_matriz(numero_candidatos, numero_objetivos))
       return 1;

    correcaminos = vector_coste;

    // Creamos la cadena de costes.

    for(contador = 0; contador < numero_candidatos; contador++) {
        correcaminos->coste_c = correcaminos->coste_unidad;
        correcaminos->camino = contador;
        correcaminos++;
    }

    // Creamos el vector C_i

    if (crea_vector_C_i(vector_coste, numero_candidatos))
       return 1;

    // Liberamos la memoria reservada.

    free(delta_u);

    return 0;

}


/**
 * \class	Viterbi_template
 * \brief	implements an iteration of the Viterbi algorithm
 * \param[in]	objetivo	target unit
 * \return	0, in absence of error
 */
template <class T, class U> int Viterbi_template<T, U>::siguiente_recursion_Viterbi(U *objetivo) {

    T **delta_u;
    int numero_candidatos;

	if (poda_Ct_valor)
	    inicia_poda_C_t();

	if (poda_Ci_valor)
	    inicia_poda_C_i();

    // Avanzamos la columna de la matriz de caminos óptimos sobre la que estamos trabajando.

    matriz.avanza_columna();

    // Creamos la cadena de unidades candidatas.

    if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
       return 1;

    // Calculamos el coste de distorsión de unidad de cada candidata.

    if (tamano_vector_coste < numero_candidatos) {

        if (vector_coste)
            free(vector_coste);

        if ( (vector_coste = (Coste_iteracion<T> *) malloc(numero_candidatos*sizeof(Coste_iteracion<T>)))
                           == NULL) {
             fprintf(stderr, "Error al asignar memoria.");
             return 1;
        }

        tamano_vector_coste = numero_candidatos;
    }


    if (crea_vector_C_t(&vector_coste, &numero_candidatos, delta_u, objetivo))
       return 1;

//    tamano_vector_coste = numero_candidatos;

/*
    // Reservamos espacio en la matriz de memoria de caminos óptimos.

    if (matriz.reinicia_ciclo(numero_candidatos))
       return 1;

    // Actualizamos el vector de costes mínimos.

    calcula_coste_minimo_hasta_unidades(vector_coste, numero_candidatos);
*/

    if (crea_vector_C_i(vector_coste, numero_candidatos))
       return 1;

    // Liberamos la memoria reservada.

    free(delta_u);

    return 0;

}


/**
 * \class	Viterbi_template
 * \brief	Converts a path of candidate units to an array of candidate units
 * \param[in]	inicio	row of the matrix of paths
 * \param[in]	tamano	number of units
 * \param[out]	cadena_salida	array of candidate units
 * \return	0, in absence of errors
 */
template <class T, class U> int Viterbi_template<T, U>::convierte_a_cadena_candidatos(Matriz_struct<T> *inicio, int tamano,
                                T **cadena_salida) {

    T *correcaminos;
    int contador;

	if (*cadena_salida == NULL) {

        if ( (correcaminos = (T *) malloc(tamano*sizeof(T))) == NULL) {

              fprintf(stderr, "Error en devuelve_cadena_candidatos, al asignar memoria.\n");
              return 1;
        }

    	*cadena_salida = correcaminos;

    } // if (*cadena_salida == NULL)
    else
    	correcaminos = *cadena_salida;

    for (contador = 0; contador < tamano; contador++, inicio++, correcaminos++)
        memcpy(correcaminos, inicio->unidad, sizeof(T));

    return 0;

}



/**
 * \class	Viterbi_template
 * \brief	Frees the memory used during the execution of the algorithm
 */
template <class T, class U> void Viterbi_template<T, U>::libera_memoria_algoritmo() {

    matriz.libera_memoria();

    if (C_i != NULL) {
        free(C_i);
        C_i = NULL;
        tamano_C_i = 0;
        memoria_reservada_C_i = 0;
    }

    if (vector_coste != NULL) {
        free(vector_coste);
        vector_coste = NULL;
        tamano_vector_coste = 0;
    }

    maxima_memoria_reservada = 0;

}

/**
 * \class	Viterbi_template
 * \brief	returns the set of best candidate unit paths
 * \param[in]	secuencia_optima	selects the n-th best sequence (being 0 the best)
 * \param[out]	numero_unidades	number of units comprising the sequence
 * \param[out]	unidades	array with the chosen units
 * \param[ou]	numero_unidades	number of elements of the preceding array
 * \param[out]	numero_real_caminos	real number of best paths that can be considered
 * \param[out]	coste	total cost of the set of candite units
 * \return	0, in absence of error
 * \remarks	After the Viterbi search, the user should call this function with secuencia_optima = 0, then 1, and so on
 */

template <class T, class U> int Viterbi_template<T,U>::devuelve_camino_optimo(unsigned char secuencia_optima,
                T **unidades, int *numero_unidades,
                int *numero_real_caminos, float *coste) {

    int indice_minimo;
	int filas  = matriz.devuelve_numero_filas();
    Matriz_struct<T> *camino_optimo;

    if (indices_escogidos == NULL) {

        if ( (indices_escogidos = (bool *) malloc(filas*sizeof(bool))) == NULL) {
        	fprintf(stderr, "Error en devuelve_camino_optimo, al asignar memoria.\n");
            return 1;
        }

        for (int contador = 0; contador < filas; contador++)
        	indices_escogidos[contador] = false;

    } // if (indices_escogidos == NULL)

    if (poda_Ci == false) {

        char elimina;
        int cuenta;
        float minimo = FLT_MAX;

        Coste_Ci<T> *correcaminos = C_i;

        indice_minimo = 0;

        // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.

        for (int contador = 0; contador < tamano_C_i; contador++, correcaminos++) {

            if (correcaminos->C_i < minimo) {

                cuenta = 0;
                elimina = 0;

                while (cuenta < filas)
                    if (contador == indices_escogidos[cuenta++]) {
                        elimina = 1;
                        break;
                    }

                if (elimina)
                    continue;

                minimo = correcaminos->C_i;
                indice_minimo = contador;
            } // if (correcaminos->C_i...

        } // for (int contador ...
    }
	else
		indice_minimo = secuencia_optima;

    *coste = C_i[indice_minimo].C_i;
    
    indices_escogidos[indice_minimo] = true;

    if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
       return 1;


    if (convierte_a_cadena_candidatos(camino_optimo, *numero_unidades, unidades))
        return 1;

    return 0;

}


/**
 * \class	Viterbi_template
 * \brief	Frees the memory of some variables
 */

template <class T, class U> void Viterbi_template<T, U>::finaliza() {

	if (indices_escogidos) {
    	free(indices_escogidos);
        indices_escogidos = NULL;
    }

}

/**
 * \class	Viterbi_template
 * \brief	initializes the variable minimo_c_t, used to prune by value the array vector_coste
 */
template <class T, class U> void Viterbi_template<T, U>::inicia_poda_C_t() {

     minimo_c_t = FLT_MAX - c_red;

}

/**
 * \class	Viterbi_template
 * \brief	initializes the variable minimo_c_i, used to prune by value the array C_i
 */
template <class T, class U> void Viterbi_template<T, U>::inicia_poda_C_i() {

     minimo_c_i = FLT_MAX - c_poda;

}


/**
 * \class	Viterbi_template
 * \brief	sorts and prunes the vector C_i
 * \param[in]	posicion: index of the unit which is going to be inserted
 * \param[in]	inicio	sorted array with the total cost until each unit
 * \return	number of pruned units
 */
template <class T, class U> int Viterbi_template<T,U>::ordena_y_poda_C_i(Coste_Ci<T> *inicio, int posicion) {

    unsigned char poda_numero = 0, poda_valor = 0;
    float umbral;

    Coste_Ci<T> *correcaminos = inicio;

    // La posición del nuevo elemento es la del tamaño - 1.

    posicion -= 1;

    float coste = correcaminos[posicion].C_i;

    if (poda_Ci_valor) {

	    umbral = minimo_c_i + c_poda;

	    if (coste > umbral)
	       // no puede formar parte de la secuencia óptima. Lo eliminamos.
	       return 1;

    	// Comprobamos si el valor es menor que el mínimo actual.
	    if (coste < minimo_c_i) {
	       umbral = coste + c_red;
	       minimo_c_i = coste;
	    }

    } // if (poda_Ci_valor)

    int contador = 0;

// Ordenamos el array.
// Esto lo tengo que cambiar por una búsqueda binaria.

    while ( (correcaminos->C_i <= coste) && (contador < posicion) ) {
          contador++;
          correcaminos++;
    }

    if (contador != posicion) {
       Coste_Ci<T> nuevo = inicio[posicion];

       correcaminos = inicio + posicion;

       for (int cuenta = posicion; cuenta > contador; cuenta--) {

       		if (poda_Ci_valor) {
	           if (correcaminos->C_i > umbral)
    	          poda_valor = (short int) (posicion - cuenta);
			}
           *correcaminos = *(correcaminos - 1);
           correcaminos--;
       }

       *correcaminos = nuevo;

        if (poda_Ci_valor) {
        	if (contador == 0)
            	if (inicio[1].C_i > umbral)
                	poda_valor++;
        }

    }

    if (poda_Ci_numero) {
	    if (posicion >= n_poda_Ci) // El >= viene del posicion -= 1.
	//       poda_numero = posicion - N_PODA_UNIDAD_acentual;
	        poda_numero = 1;
	}

    // Devolvemos el valor que produce una poda mayor.

    if (poda_valor > poda_numero)
       return poda_valor;

    return poda_numero;

}



/**
 * \class	Viterbi_template
 * \brief	sorts and prunes the vector C_t
 * \param[in]	inicio	array sorted in terms of the target cost
 * \param[in]	posicion	index of the unit to be inserted
 * \return	Number of pruned elements
 */
template <class T, class U> int Viterbi_template<T, U>::ordena_y_poda_C_t(Coste_iteracion<T> *inicio, int posicion) {

    unsigned char poda_numero = 0, poda_valor = 0;
	float umbral;
    Coste_iteracion<T> *correcaminos = inicio;

    // La posición en la que está situado el nuevo elemento es la del tamaño menos 1.
    posicion -= 1;

    float coste = correcaminos[posicion].coste_unidad;

    if (poda_Ct_valor) {

		umbral = minimo_c_t + c_red;

	    if (coste > umbral)
	       // no puede formar parte de la secuencia óptima. Lo eliminamos.
	       return 1;

    	// Si el coste es menor que el mínimo, actualizamos su valor.

	    if (coste < minimo_c_t) {
	       umbral = coste + c_red;
	       minimo_c_t = coste;
	    }
    } // if (poda_Ct_valor)

    int contador = 0;

// Ordenamos el array.
// Esto lo tengo que cambiar por una búsqueda binaria.

    while ( (correcaminos->coste_unidad <= coste) && (contador < posicion) ) {
          contador++;
          correcaminos++;
    }

    if (contador != posicion) {

    	Coste_iteracion<T> nuevo = inicio[posicion];

       	correcaminos = inicio + posicion;

		for (int cuenta = posicion; cuenta > contador; cuenta--) {

        	if (poda_Ct_valor) {

            	if (correcaminos->coste_unidad > umbral)
//                	poda_valor = (short int) (posicion - cuenta);
					poda_valor++;

			}
           	*correcaminos = *(correcaminos - 1); // YA QUE SON POSICIONES CONSECUTIVAS EN MEMORIA,
           	correcaminos--;  // DEBERÍA INTENTAR CAMBIARLO POR UN SIMPLE MEMMOVE.
       	}

        *correcaminos = nuevo;

        if (poda_Ct_valor) {
        	if (contador == 0)
            	if (inicio[1].coste_unidad > umbral)
                	poda_valor++;
        }
	}

    if (poda_Ct_numero) {

	    if (posicion >= n_poda_unidad) // El >= viene del posicion -= 1.
	       poda_numero = (char) 1;
	}

    // Devolvemos el valor que produce una poda mayor.

    if (poda_valor > poda_numero)
       return poda_valor;

    return poda_numero;

}


/**
 * \class	Viterbi_template
 * \brief	Updates the array C_i to the current iteration
 * \param[in]	C_concatena	pruned array with the shortest paths to each unit
 * \param[in]	tamano number of elements of the previous array
 * \param[in]	objetivo	target unit
 * \return	0, in absence of error
 * \remarks	this function considers more than one optimum path to each unit
 */
template <class T, class U> int Viterbi_template<T, U>::crea_vector_C_i(Coste_iteracion<T> *C_concatena, int tamano) {

	int contador;
    int tamano_actual = 0;
    Coste_iteracion<T> *correcaminos = C_concatena;
    Coste_Ci<T> *nuevo_C_i;
    int tamano_total;
	int poda;
    bool primera_iteracion;

    (tamano_C_i == 0) ? primera_iteracion = true: primera_iteracion = false;

    if (primera_iteracion == true)
        tamano_total = tamano;
    else
        tamano_total = tamano*tamano_C_i;

    if (poda_Ci_numero == false) {

        // Le asignamos memoria de nuevo.
        if ( (nuevo_C_i = (Coste_Ci<T> *) malloc(tamano_total*sizeof(Coste_Ci<T>))) == NULL) {
           fprintf(stderr, "Error al asignar memoria en crea_vector_C_i.");
           return 1;
        }

    } // if (poda_Ci_numero == false)
    else {

        // Le asignamos memoria de nuevo.
        if ( (nuevo_C_i = (Coste_Ci<T> *) malloc((n_poda_Ci + 1)*sizeof(Coste_Ci<T>))) == NULL) {
           fprintf(stderr, "Error al asignar memoria en crea_vector_C_i.");
           return 1;
        }

    } // else // poda_Ci_numero = true


    Coste_Ci<T> *recorre_nuevo_C_i = nuevo_C_i;
    Coste_Ci<T> *recorre_C_i = C_i;

    if (primera_iteracion == true) {

	   // Vamos rellenando los campos de cada estructura del array.

    	for (contador = 0; contador < tamano; contador++) {
	        // Añadimos la propia unidad.
	        recorre_nuevo_C_i->unidad = correcaminos->unidad;
	        // El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
	        recorre_nuevo_C_i->camino = correcaminos->camino;
	        // El coste mínimo para llegar hasta dicha unidad.
	        recorre_nuevo_C_i->C_i = correcaminos->coste_c;
            tamano_actual++;

//            printf("Id = %d.\n", correcaminos->unidad->identificador);
            
            if (poda_Ci) {
		       // Comprobamos si hay que realizar algún tipo de poda.
		       if ( ( poda = ordena_y_poda_C_i(nuevo_C_i, tamano_actual)) != 0) {
		          tamano_actual -= poda;
                  recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
               }
		       else
               	recorre_nuevo_C_i++;

            } // if (poda_Ci)
			else
	          recorre_nuevo_C_i++;

       		correcaminos++;

   		} // for (contador = 0; ...

    } // if (primera_iteracion == true)
	else {

        // Vamos rellenando los campos de cada estructura del array.

        for (int cuenta_C_i = 0; cuenta_C_i < tamano_C_i; cuenta_C_i++, recorre_C_i++) {

            correcaminos = C_concatena;

            for (contador = 0; contador < tamano ; contador++) {
                // Añadimos la propia unidad.
                recorre_nuevo_C_i->unidad = correcaminos->unidad;
                // El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
                recorre_nuevo_C_i->camino = cuenta_C_i;
                // El coste mínimo para llegar hasta dicha unidad.
                recorre_nuevo_C_i->C_i = peso_coste_objetivo*correcaminos->coste_unidad +
                                   peso_coste_concatenacion*calcula_C_c(recorre_C_i->unidad, correcaminos->unidad) +
                                   recorre_C_i->C_i;

                tamano_actual++;

                if (poda_Ci) {

                    // Comprobamos si hay que realizar algún tipo de poda.
                    if ( ( poda = ordena_y_poda_C_i(nuevo_C_i, tamano_actual)) != 0) {
                        tamano_actual -= poda;
                        recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
                    }
                    else {
                        recorre_nuevo_C_i++;
                    }

                } // if (poda_Ci)
                else
                    recorre_nuevo_C_i++;

               correcaminos++;

            } // for (contador = 0; ...

        } // for (cuenta_C_i...

	} // else // if (primera_iteracion == true)

    if (primera_iteracion == true) {
	    if (tamano != tamano_actual)
		   // Actualizamos el número de filas (por si ha habido poda):
        	matriz.actualiza_numero_de_filas(tamano_actual);
	}
    else
		// Reservamos memoria para la matriz de caminos óptimos.
	   	if (matriz.reinicia_ciclo(tamano_actual))
   			return 1;

   	// Actualizamos el número de elementos de la cadena C_i.

   	tamano_C_i = tamano_actual;

	// Actualizamos la matriz

	recorre_C_i = nuevo_C_i;

   	for (contador = 0; contador < tamano_actual; contador++) {
    	if (matriz.introduce_elemento(contador, recorre_C_i->unidad, recorre_C_i->camino))
        	return 1;
       	recorre_C_i++;
   	} // for (contador = 0;


    if (primera_iteracion == false)
		free(C_i);

    C_i = nuevo_C_i;

	return 0;

}

/**
 * \class	Viterbi_template
 * \brief	Creates the array vector_coste, computing the target cost of each unit in the
current iteration
 * \param[in]	delta_u	array of candidate units
 * \param[in]	objetivo: target unit
 * \param[out]	costes	array with the target costs
 * \param	tamano	in the call => original number of candidate units. It can change in case of pruning
 * \return	0, in absence of errors
 */
template <class T, class U> int Viterbi_template<T, U>::crea_vector_C_t(Coste_iteracion<T> **costes, int *tamano, T **delta_u,
                    U *objetivo) {

    Coste_iteracion<T> *correcaminos;
    T **puntero;
    int tamano_actual = 0;
    int poda;


    correcaminos = *costes;
    puntero = delta_u;

    // Repetimos el bucle para cada elemento de la cadena de unidades candidatas.

    for (int contador = 0; contador < *tamano; contador++) {
        // Calculamos el coste de unidad respecto al objetivo.
        correcaminos->coste_unidad = calcula_C_t(*puntero, objetivo);
        // Anotamos el identificador de la unidad.
        correcaminos->unidad = *puntero;
        // Incrementamos la variable tamano_actual.
        tamano_actual++;

        if (poda_Ct) {
	        // Comprobamos si la unidad puede formar parte del camino óptimo.
	        if ( (poda = ordena_y_poda_C_t(*costes, tamano_actual)) != 0) {
	           tamano_actual -= poda; // En este caso, no puede.
               correcaminos = *costes + tamano_actual;
            }
	        else
            	correcaminos++;
		}
        else
        	correcaminos++;

        puntero++;

    }

/*
    // Si ha habido poda, las unidades sobrantes están al final, así que llega con
    // liberar esa memoria.

    if (*tamano != tamano_actual) {
       *tamano = tamano_actual;
       if ( (*costes = (Estructura_coste_acentual *) realloc(*costes,
                       tamano_actual*sizeof(Estructura_coste_acentual))) == NULL) {
          fprintf(stderr, "Error en crea_vector_C_t, al reasignar memoria.");
          return 1;
       }
    }
*/
    // Si ha habido poda, avisamos al programa llamante cambiando el valor de *tamano.
    // No liberamos la memoria, porque sería una pérdida de tiempo (sólo liberaríamos unas
    // posiciones). Ya se libera al final del ciclo por completo.

    *tamano = tamano_actual;

    return 0;
}


/**
 * \class	Viterbi_template
 * \brief	Computes mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).
 * \param	vector_coste	array of Coste_iteracion<T> objects
 * \param[in]	numero_candidatos	number of candidate units
 */
template <class T, class U> void Viterbi_template<T, U>::calcula_coste_minimo_hasta_unidades(Coste_iteracion<T> *vector_coste,
    						int numero_candidatos) {

    T *unidad_actual;
    Coste_iteracion<T> *recorre_costes = vector_coste;
    float minimo;
    float coste_actual;
    int camino;
    Coste_Ci<T> *correcaminos;
    int contador;

    for (int cuenta = 0; cuenta < numero_candidatos; cuenta++, recorre_costes++) {

        // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la
        // iteración anterior (i.e., los elementos de la cadena de estructuras C_i).

        unidad_actual = recorre_costes->unidad;
        camino = 0;
        correcaminos = C_i;
        minimo = FLT_MAX;

        for (contador = 0; contador < tamano_C_i; contador++) {
            // Calculamos el coste total hasta la unidad considerada en el paso actual.
	        coste_actual = calcula_C_c(correcaminos->unidad, unidad_actual) + correcaminos->C_i;
            // Si el coste el menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.
            if (coste_actual < minimo) {
               recorre_costes->id_coste_minimo = correcaminos->unidad->identificador;
               recorre_costes->coste_c = coste_actual;
               minimo = coste_actual;
               camino = contador;
            }
            correcaminos++;
        }

        // Apuntamos qué camino proporciona el coste mínimo y su valor.
        recorre_costes->camino = camino;
        recorre_costes->coste_c += recorre_costes->coste_unidad;
    }

}

#endif
