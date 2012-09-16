/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ram�n Pi�eiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigaci�n en Humanidades,
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
 

#ifndef _INFO_CORPUS_VOZ

#define _INFO_CORPUS_VOZ

#include "descriptor.hpp"
#include "grupos_acentuales.hpp"

#define TIPOS_UNIDADES 24L
#define IZQUIERDO '0'
#define DERECHO '1'
#define NUMERO_INDICES 256
//#define NUMERO_FONEMAS 31
#define NUMERO_DIFONEMAS 679
//#define INDICE_tS 29
//#define INDICE_rr 30

#define TAMANO_MAXIMO_FICHERO 7000000//2500000 // Se emplea al crear el fichero �nico de unidades.7000000 para Rocio

#define CONTORNO_SORDO -1 ///< Identificador que se emplea para indicar cu�ndo un contorno que se est� propagando todav�a no ha pasado por una unidad sonora

#define CAMINO_PODADO -666 ///< Valor que se da a los campos de los caminos podados


/** \struct estructura_indices
 *
 * Estructura en la que se almacena la posici�n dentro de una tabla y el n�mero 
 * de unidades que hay del tipo especificado.
 */  
 
typedef struct {
  short int posicion;           ///< �ndice de la tabla en la que se encuentran las unidades del tipo que se desea.
  short int numero_unidades;    ///< N�mero de unidades de ese tipo.
} estructura_indices;


/** \struct estructura_unidades
 *
 * Estructura en la que se almacenan todos los semifonemas del mismo nombre,
 * ordenados de acuerdo a sus caracter�sticas de tipo de frase, posici�n y
 *  car�cter t�nico.
 */

typedef struct {
  estructura_indices puntero_a_unidades[TIPOS_UNIDADES];  ///< Tabla de �ndices a los tipos de unidades; Se emplea
  ///< la macro conjunto_unidades para convertir la informaci�n
  ///< de tipo de frase, posici�n y car�cter t�nico en un �ndice
  ///< de esta tabla; El contenido de la estructura indexada indica
  ///< la posici�n dentro de la cadena unidades de los semifonemas
  ///< de ese tipo, as� como su n�mero.
  int numero_unidades;                    ///< N�mero total de semifonemas de la cadena unidades.
#ifndef _x86_64
  int dummy_1;
#endif
  Vector_semifonema_candidato *unidades;  ///< Cadena con los semifonemas ordenados.
} estructura_unidades;

typedef estructura_unidades Tabla_semifonemas[NUMERO_FONEMAS][NUMERO_FONEMAS];

/** \struct estructura_difonemas
 *
 *  Estructura en la que se almacena la informaci�n de todos los difonemas 
 *  del mismo nombre.
 *
 */

typedef struct {
  char nombre[2*OCTETOS_POR_FONEMA + 1];  ///< Nombre del difonema.
  int numero_unidades;                    ///< N�mero de difonemas del mismo nombre.
  Vector_difonema *unidades;              ///< Cadena con todos los difonemas del mismo nombre.
} estructura_difonemas;

typedef char nombre_difonema[2*OCTETOS_POR_FONEMA]; ///< Cadena de caracteres en la que se almacena
                                                    ///< el nombre de un difonema.
typedef struct {
  estructura_difonemas **tabla_difonemas;
  estructura_difonemas *difonemas;
  nombre_difonema *indice_semifonemas;
  int numero_semifonemas;
} Corpus_difonemas;


/** \struct ficheros_unidades
 *
 *  Estructura en la que se almacena el nombre del fichero en el que se 
 *  encuentran almacenados todos los semifonemas del mismo nombre.
 *
 */  

typedef struct {
   
  char nombre_fichero[260]; ///< Nombre del fichero. La longitud del array es el valor de FILENAME_MAX en Windows. 
  ///< Al depender su valor del sistema y utilizarse esta estructura en un array
  ///< no podemos dejarlo simplemente con la macro.
  char primer_fonema[3];    ///< Primer fonema del semifonema.
  char segundo_fonema[3];   ///< Segundo fonema del semifonema.
  unsigned char izq_der;    ///< Indica si es un semifonema por la derecha o por la izquierda.
    
} ficheros_unidades;

/** \struct indice_ficheros
 *
 * Estructura que se emplea para crear una tabla con el tama�o y la posici�n de 
 * cada fichero de unidades de un mismo nombre dentro del fichero �nico de unidades.
 * 
 */   

typedef struct {
  int indice;    ///< Posici�n dentro del fichero �nico.
  int tamano;    ///< Tama�o, en octetos, del fichero 
} indice_ficheros;


/** \union Identificador
 *
 * Union que almacena un tipo de unidad.
 * 
 */  

typedef union {
  Vector_semifonema_candidato *vector;  ///< Puntero a un objeto de tipo Vector_semifonema_candidato.
  Grupo_acentual_candidato *grupo;      ///< Puntero a un objeto de tipo Grupo_acentual_candidato.
  int contador_posicion;                ///< Identificador de un difonema.
} Identificador;


/** \struct estructura_matriz
 *
 * Estructura en la que se almacena la informaci�n de un nodo dentro de la 
 * matriz de caminos �ptimos.
 * 
 */  

typedef struct {
  char tipo_unidad; ///< semifonema (0) o difonema (1).
  char parametro_1; ///< Si es un semifonema, indica IZQUIERDO (0) o DERECHO (1).
  ///< Si es un difonema, indica el �ndice del primer fonema.
  char parametro_2; ///< Si es un semifonema, indica la validez de la unidad.
  ///< Si es un difonema, contiene el �ndice del segundo fonema.
  char parametro_3; ///< S�lo tiene significado si es un difonema, y contiene codificada la
  ///< informaci�n de la validez y el contexto (IZQUIERDO o DERECHO).
  Identificador localizacion;  ///< Puntero al semifonema o identificador del difonema.

} estructura_matriz;


/** \struct tabla_ficheros
 *
 * Estructura en la que se almacena el nombre del fichero de sonido original y 
 * la posici�n que �ste ocupa dentro del fichero �nico de sonido del sintetizador.
 * 
 */  

typedef struct {
  char nombre_fichero[100]; ///< Nombre del fichero de sonido del Corpus.
  int posicion_fichero; ///< Posici�n de los datos del fichero anterior dentro
  ///< del fichero �nico de sonido.
} tabla_ficheros;


/** \struct Estructura_coste
 
    \remarks Estructura intermedia que se emplea
    para almacenar los costes de objetivo de cada unidad candidata de la
    iteraci�n actual. Sobre ella se realiza la poda basada en el coste
    de objetivo. 

*/

class Estructura_coste {
public:

  Vector_semifonema_candidato *unidad;
  //  vector<int> id_coste_minimo; ///< Identificador de la unidad de la iteraci�n anterior que proporciona coste m�nimo hasta llegar a la actual.
  
  float coste_unidad;  ///< Coste de objetivo de la unidad actual respecto al objetivo.
  vector<float> costes_unidad; ///< Vector de costes de objetivo para el c�lculo de varios contornos en paralelo.
  vector<int> camino;          ///< �ndice a la fila de la matriz de caminos �ptimos;�ndice a la secuencia de unidades que preceden a la actual dentro de la matriz de caminos �ptimos.
  float coste_c;       ///< Coste acumulado hasta la unidad actual. (lo dejamos de momento por compatibilidad hacia atr�s)
  vector<float> costes_c;  ///< Vector de costes para el c�lculo con varios contornos en paralelo
  float coste_poda; ///< Para el c�lculo de costes en paralelo, coste empleado para la poda

  char podable; ///< Se emplea para evitar podar caminos que tienen coste de concatenaci�n cero con alguna unidad de la iteraci�n anterior. Si es cero, no se puede podar.

  Estructura_coste() {
    //    id_coste_minimo.clear();
    costes_unidad.clear();
    costes_c.clear();
    camino.clear();
  }

  Estructura_coste &operator= (const Estructura_coste &origen) {
    if (this != &origen) {
      unidad = origen.unidad;
      //      id_coste_minimo = origen.id_coste_minimo;
      coste_unidad = origen.coste_unidad;
      costes_unidad = origen.costes_unidad;
      camino = origen.camino;
      coste_c = origen.coste_c;
      costes_c = origen.costes_c;
      coste_poda = origen.coste_poda;
      podable = origen.podable;
    }
  }

  Estructura_coste(const Estructura_coste &origen) {
    *this = origen;
  }
};

/** \struct Datos_mejor_camino
 *
 * Estructura que almacena los datos relevantes al mejor camino hasta cada unidad, relacionado con un contorno.
 */
   
typedef struct {
  int camino; ///< �ndice a la fila de la matriz de caminos �ptimos.
  int indice_contorno; ///< �ndice del contorno al que se refiere esa unidad (para el c�lculo en paralelo)
  float C_i; ///< Coste acumulado hasta la unidad actual, con ese contorno de f0.
} Datos_mejor_camino;

/** \struct Estructura_C_i
 *
 * Estructura en la que se almacenan los caminos �ptimos a cada unidad de la 
 * iteraci�n actual, as� como el coste acumulado.
 *  
 */ 

class Estructura_C_i {

public:

  Vector_semifonema_candidato *unidad;
  int camino;          ///< �ndice a la fila de caminos �ptimos.
  int indice_contorno; ///< �ndice del contorno al que se refiere esa unidad (para el c�lculo en paralelo)
  float C_i;           ///< Coste acumulado hasta la unidad actual.
  vector<Datos_mejor_camino> datos_camino; ///< Vector que almacena los datos de cada mejor camino para cada contorno
  char podable; ///< Se emplea para evitar podar caminos que tienen coste de concatenaci�n cero con alguna unidad de la iteraci�n anterior. Si es cero, no se puede podar.

  Estructura_C_i() {
    datos_camino.clear();
  }

  Estructura_C_i &operator= (const Estructura_C_i &origen) {
    unidad = origen.unidad;
    camino = origen.camino;
    indice_contorno = origen.indice_contorno;
    podable = origen.podable;
    C_i = origen.C_i;
    if (this != &origen) {
      datos_camino = origen.datos_camino;
    }
  }

  Estructura_C_i(const Estructura_C_i &origen) {
    *this = origen;
  }

};

/** \struct Estructura_coste_acentual
 *
 */ 

typedef struct {
  Grupo_acentual_candidato *unidad;
  int id_coste_minimo; ///< Identificador de la unidad del paso anterior que proporciona coste
  ///< m�nimo hasta llegar a la actual.
  float coste_unidad;  ///< Coste de distorsi�n de la unidad id_propio respecto al objetivo.
  int camino;          ///< �ndice a la fila de la matriz de caminos �ptimos.
  float coste_c;       ///< Coste acumulado hasta la unidad actual.
} Estructura_coste_acentual;

/** \struct Estructura_coste_acentual_GF
 *
 */ 

typedef struct {
  Grupo_acentual_candidato *secuencia_unidades;
  int numero_unidades; ///< N�mero de unidades del grupo f�nico
  int id_coste_minimo; ///< Identificador de la unidad del paso anterior que proporciona coste
  ///< m�nimo hasta llegar a la actual.
  float coste_unidad;  ///< Coste de distorsi�n de la unidad id_propio respecto al objetivo.
  int camino;          ///< �ndice a la fila de la matriz de caminos �ptimos.
  float coste_c;       ///< Coste acumulado hasta la unidad actual.
} Estructura_coste_acentual_GF;


/** \struct Estructura_C_i_acentual
  
    Estructura en la que se almacenan los caminos �ptimos a cada unidad de la 
    iteraci�n actual, as� como el coste acumulado.

*/

typedef struct {
  Grupo_acentual_candidato *unidad;
  int camino;          ///< �ndice a la fila de caminos �ptimos; �ndice a la secuencia de unidades que forman la
  ///< secuencia �ptima incluyendo a la unidad actual.
  int id_coste_minimo; ///< Identificador de la unidad del paso anterior que proporciona coste
  float C_i;           ///< Coste acumulado hasta la unidad actual; coste acumulado de la secuencia anterior.
  float coste_unidad; ///< Coste de unidad.
  float coste_concatenacion; ///< Coste de concatenaci�n con la unidad anterior.
  float coste_anterior; ///< Coste hasta la unidad anterior.
} Estructura_C_i_acentual;


/** \struct Estructura_C_i_acentual_GF
 * \brief Estructura similar a la anterior, pero que se emplea en el modo de c�lculo de contornos por grupos f�nicos.
*/

typedef struct {
  Grupo_acentual_candidato *secuencia_unidades;
  int numero_unidades; ///< N�mero de unidades del grupo f�nico
  int camino;          ///< �ndice a la fila de caminos �ptimos; �ndice a la secuencia de unidades que forman la secuencia �ptima incluyendo a la unidad actual.
  int id_coste_minimo; ///< Identificador de la unidad del paso anterior que proporciona coste
  float C_i;           ///< Coste acumulado hasta la unidad actual; coste acumulado de la secuencia anterior.
  float coste_unidad; ///< Coste de unidad.
  float coste_concatenacion; ///< Coste de concatenaci�n con la unidad anterior.
  float coste_anterior; ///< Coste hasta la unidad anterior.
} Estructura_C_i_acentual_GF;

#endif
