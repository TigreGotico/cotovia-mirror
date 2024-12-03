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
 
#ifndef MODELO_LENGUAJE_HPP
#define MODELO_LENGUAJE_HPP


#define FICHERO_HASH_5 "pentagramas.dat"            ///< Nombre del fichero con las probabilidades condicionadas.
#define FICHERO_HASH_5_real "pentagramas_real.dat"  ///< Nombre del fichero con las probabilidades conjuntas.
#define FICHERO_HASH_4 "tetragramas.dat"            ///< Nombre del fichero con las probabilidades condicionadas.
#define FICHERO_HASH_4_real "tetragramas_real.dat"  ///< Nombre del fichero con las probabilidades conjuntas.
#define FICHERO_HASH_3 "trigramas.dat"              ///< Nombre del fichero con las probabilidades condicionadas.
#define FICHERO_HASH_3_real "trigramas_real.dat"    ///< Nombre del fichero con las probabilidades conjuntas.
#define FICHERO_HASH_2 "bigramas.dat"               ///< Nombre del fichero con las probabilidades condicionadas.
#define FICHERO_HASH_2_real "bigramas_real.dat"     ///< Nombre del fichero con las probabilidades conjuntas.
#define FICHERO_UNIGRAMAS "unigramas.dat"           ///< Nombre del fichero con las probabilidades.
#define FICHERO_CLASES_AMBIGUEDAD "clases_ambiguedad.dat" ///< Nombre del fichero en el que se almacenan las clases
///< de ambigüedad del análisis morfosintáctico.

#define PENALIZACION_4_GRAMAS -3.0F
#define PENALIZACION_3_GRAMAS -1.0F
#define PENALIZACION_2_GRAMAS  0.0F

#define TAMANO_MAXIMO_N_GRAMA 5



/** \struct estructura_probabilidades_generica
 *
 * Estructura en la que se almacena una clave formada por 4 enteros junto
 * con su probabilidad de aparición.
 * 
 */   

typedef struct {

	unsigned char clave[4];  ///< Clave que representa una secuencia de categorías.

	float probabilidad;      ///< Probabilidad de la secuencia anterior.

} estructura_probabilidades_generica;


/** \struct estructura_probabilidades_2
 *
 * Estructura semejante a estructura_probabilidades_generica en la que la clave, 
 * formada por 2 enteros, corresponde a bigramas
 * 
 */  

typedef struct {

	unsigned char clave[2];   ///< Clave formada por 2 enteros que representa un bigrama.
	float probabilidad;       ///< Probabilidad de aparición del bigrama.
	float back_off;           ///< Back-off del bigrama.

} estructura_probabilidades_2;


/** \struct estructura_probabilidades_3
 *
 * Estructura semejante a estructura_probabilidades_generica en la que la clave, 
 * formada por 3 enteros, corresponde a trigramas.
 *
 */  

typedef struct {

	unsigned char clave[3];   ///< Clave formada por 3 enteros que representa un trigrama.
	float probabilidad;       ///< Probabilidad de aparición del trigrama.
	float back_off;           ///< Back-off del trigrama.

} estructura_probabilidades_3;



/** \struct estructura_probabilidades_4
 *
 * Estructura semejante a estructura_probabilidades_generica en la que la clave, 
 * formada por 4 enteros, corresponde a tetragramas.
 *
 */  

typedef struct {

	unsigned char clave[4];   ///< Clave formada por 4 enteros que representa un tetragrama.
	float probabilidad;       ///< Probabilidad de aparición del tetragrama.
	float back_off;           ///< Back-off del tetragrama.

} estructura_probabilidades_4;

/** \struct estructura_unigramas
 *
 * Estructura semejante a estructura_probabilidades_genérica pero referida a
 * unigramas.
 *  
 */  

typedef struct {

	Categorias_modelo_enum categoria;   ///< Categoría de la palabra.
	float probabilidad;            ///< Probabilidad del unigrama
	float back_off;                ///< Back-off del unigrama.

} estructura_unigramas;


/** \struct estructura_probabilidades_4
 *
 * Estructura semejante a estructura_probabilidades_generica en la que la clave, 
 * formada por 5 enteros, corresponde a pentagramas.
 *
 */  

typedef struct {
	unsigned char clave[5];    ///< Clave formada por 5 enteros que representa un pentagrama.
	float probabilidad;        ///< Probabilidad de aparición del pentagrama.

} estructura_probabilidades_5;

//typedef struct {
//	unsigned char clave[LONG_CLAVE_CLASE_AMBIGUEDAD];
//	float probabilidad;
//} estructura_probabilidades_clase_ambiguedad;


/** \class Modelo_lenguaje
 *
 *
 */  

class Modelo_lenguaje {

	int numero_modelos_cargados;

	public:
	Perfhash *pentagramas,         ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad condicionada de aparición de los pentagramas.

					 *pentagramas_real,    ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad de aparición de los pentagramas.

					 *tetragramas,         ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad condicionada de aparición de los tetragramas.

					 *tetragramas_real,    ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad de aparición de los tetragramas.

					 *trigramas,           ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad condicionada de aparición de los trigramas.

					 *trigramas_real,      ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad de aparición de los trigramas.

					 *bigramas,            ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad condicionada de aparición de los bigramas.

					 *bigramas_real,       ///< Estructura de tipo Perfhash en la que se almacena la información 
					 ///< de la probabilidad de aparición de los bigramas.

					 *clases_ambiguedad;   ///< Estructura de tipo Perfhash en la que se almacena la información de
	///< las probabilidades de cada categoría dentro de su clase de ambigüedad.

	estructura_unigramas *unigramas; ///< Cadena de estructuras de tipo estructura_unigramas en la que se almacena
	///< la informacion de cada una de on de la probabilidad de aparición de cada
	///< una de las categorías.

	int numero_unigramas;            ///< Número de elementos de la cadena unigramas.


	/// Inicia los valores de las variables del módulo pentagramas, tetragramas, trigramas,
	/// bigramas, unigramas y numero_unigramas.

	void asigna_estructuras(Perfhash *est5, Perfhash *est4, Perfhash *est3,
			Perfhash *est2, estructura_unigramas *uni,
			int num_unigramas);

	/// Asigna un conjunto de claves al campo claves de la variable del módulo que se pasa como argumento.

	int cambia_claves_y_separacion(unsigned char *claves, unsigned int separacion, unsigned char n);

	/// Escribe en el fichero que se pasa como parámetro el contenido de una de las variables
	/// del módulo destinadas al almacenamiento de las probabilidades de las secuencias 
	/// de categorías.

	int escribe_estructura_hash_interna(char opcion, FILE *fichero);

	/// Devuelve la probabilidad de aparición de una categoría
	/// condicionada a las cuatro anteriores.

	float calcula_prob_w5_dados_w4_w3_w2_w1(unsigned char *clave);

	/// Devuelve la probabilidad de aparición de una categoría
	/// condicionada a las tres anteriores.

	float calcula_prob_w4_dados_w3_w2_w1(unsigned char *clave);

	/// Devuelve la probabilidad de aparición de una categoría
	/// condicionada a las dos anteriores.

	float calcula_prob_w3_dados_w2_w1(unsigned char *clave);

	/// Devuelve la probabilidad de aparición de una categoría
	/// condicionada a la anterior.

	float calcula_prob_w2_dado_w1(unsigned char *clave);

	/// Calcula la probabilidad conjunta de una secuencia de cinco 
	/// categorías, recurriendo al back-off cuando es necesario.

	float calcula_prob_5_grama(unsigned char *clave);

	/// Calcula la probabilidad conjunta de una secuencia de cuatro 
	/// categorías, recurriendo al back-off cuando es necesario.

	float calcula_prob_4_grama(unsigned char *clave);

	/// Calcula la probabilidad conjunta de una secuencia de tres 
	/// categorías, recurriendo al back-off cuando es necesario.

	float calcula_prob_3_grama(unsigned char *clave);

	/// Calcula la probabilidad conjunta de una secuencia de dos 
	/// categorías, recurriendo al back-off cuando es necesario.

	float calcula_prob_2_grama(unsigned char *clave);

	/// Calcula la probabilidad de una categoría, recurriendo al back-off 
	/// cuando es necesario.

	float calcula_prob_1_grama(unsigned char clave);

	/// Devuelve la probabilidad de que una palabra o conjunto de palabras 
	/// cumpla una determinada función dentro de su clase de ambigüedad.

	float calcula_prob_clase_ambiguedad(unsigned char *clave);

	/// Carga en una estructura hash el contenido del fichero que se
	/// le pasa como parámetro.

	int carga_hash(Perfhash **estructura_hash, FILE *fichero);

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

	void establece_penalizaciones_n_gramas(float pen_tetragramas, float pen_trigramas,
			float pen_bigramas);


#endif

	/// Carga en memoria el contenido de las estructuras de tipo Perfhash,
	/// además de la cadena de unigramas.

	int crea_estructuras_hash(char *ruta_datos);

	/// Libera la memoria asignada las estructuras que almacenan la información de las 
	/// secuencias de categorías, además de la de clases de ambigüedad.

	void libera_hash();


	int numero_usuarios;
	char * nombre;

	Modelo_lenguaje();
	Modelo_lenguaje(char * ruta, char *l);
	~Modelo_lenguaje();


};

#endif

