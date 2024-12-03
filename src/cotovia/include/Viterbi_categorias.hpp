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
 

#ifndef _VITERBI_CATEGORIAS_HH

#define _VITERBI_CATEGORIAS_HH


#define TAMANO_MAXIMO_C_i 1000
#define FRACCION_C_i -4 // log10(1/10000)

//#define _PODA_C_i_POR_NUMERO_CATEGORIAS
//#define _PODA_C_i_POR_VALOR_CATEGORIAS

#if defined(_PODA_C_i_POR_NUMERO_CATEGORIAS) || defined(_PODA_C_i_POR_VALOR_CATEGORIAS)
#define _PODA_C_i_CATEGORIAS
#endif

typedef struct {
        unsigned char categorias[TAMANO_MAXIMO_N_GRAMA]; // Historial de la secuencia de categorías
                                                    // óptima hasta la unidad actual.
        int camino;         // Índice a la fila de la matriz de secuencias óptimas.
        float C_i;           // Probabilidad acumulada hasta la unidad actual.
} Estructura_C_i_categorias;

typedef struct {
        unsigned char   categoria; // categoría gramatical.
//        unsigned char categoria_anterior; // categoría del paso anterior que proporciona
                                //máxima probabilidad.
        float probabilidad;  // Probabilidad de que la unidad actual tenga la categoría mencionada.
        int camino;          // Índice a la fila de la matriz de caminos óptimos.
//        float probabilidad_acumulada;       // Coste acumulado hasta la unidad actual.
} Estructura_coste_categorias;

class Viterbi_categorias {

	Estructura_C_i_categorias *C_i;
	int tamano_C_i;

	Estructura_coste_categorias *vector_coste;
	int tamano_vector_coste;

	Matriz_categorias matriz;
    Modelo_lenguaje * modelo_lenguaje;

#ifdef _PODA_C_i_POR_VALOR_CATEGORIAS
	float maximo_c_i;
#endif

int inicia_vector_C_i(estructura_categorias *palabra);

int crea_vector_C_i_categorias(Estructura_coste_categorias *vector_coste, int tamano, int indice,
                               unsigned char categorias);

void calcula_probabilidad_secuencia(float probabilidad, unsigned char categoria,
                                    int indice, Estructura_C_i_categorias *recorre_C_i,
                                    int posicion, Estructura_coste_categorias *C_ii);

int ordena_y_poda_C_i_categorias(Estructura_C_i_categorias *inicio, int posicion);

void inicia_poda_C_i_categorias();

public:

    void inicializa(Modelo_lenguaje * mod_leng);
    
	int inicia_algoritmo_Viterbi_categorias(estructura_categorias *palabra,
    		int numero_elementos);

	int siguiente_recursion_Viterbi_categorias(estructura_categorias *palabra, int indice);

	unsigned char *devuelve_secuencia_optima(int *numero_unidades);
    
	void libera_memoria_algoritmo_categorias();

}; // Class Viterbi_categorias

#endif
