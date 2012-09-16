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
 
/** \file analisis_morfosintactico.hpp
  
  <H1>El módulo de análisis morfosintáctico</H1>

<H2>Diagrama de flujo</H2>

<P> La entrada del análisis morfosintáctico consiste en todas las posibles 
categorías que puede tener cada palabra de la frase, las cuales se han extraído 
de diccionarios en una fase anterior.

<P> En primer lugar se aplica un pequeño juego de reglas lingüísticas seguras que 
eliminan aquellas secuencias de categorías que no son posibles en el idioma. 
Posteriormente, se reducen las categorías resultantes a un conjunto más general,
 que es el que aparece contemplado en el modelo estadístico. Además, también se 
le asigna a cada categoría una probabilidad léxica, calculada a partir del 
concepto de la clase de ambigüedad. El siguiente paso consiste en la aplicación 
de un modelo de lenguaje estadístico que considera la probabilidad de aparición 
de cada secuencia de categorías consecutivas dentro del idioma, el cual decide, 
conjuntamente con la información léxica de la etapa anterior, cuál es el 
conjunto de categorías más probable para la frase de entrada. Por último, se 
realiza la conversión de las categorías del modelo del lenguaje a las 
originales.

<H2> Modelos estadísticos</H2>

<P>Dentro de la etapa del modelo del lenguaje, consideramos cuatro métodos
diferentes:

<UL> 
    <LI> <B> Método v1:</B> Ventana deslizante. Se va desplazando a lo largo de la 
    frase una ventana de tamaño cinco, y se busca el pentagrama más probable de 
    todos los posibles en función de las categorías que pueden tener todas las 
    palabras de la ventana. Se decide la categoría de una palabra a partir del 
    pentagrama más probable centrado en ella, ponderado por la probabilidad léxica 
    de que esa palabra tenga esa categoría. De esta forma, la decisión se va 
    haciendo palabra por palabra, independientemente de las categorías ya 
    escogidas. Evidentemente, de esta forma se pierden influencias sintácticas que 
    estén fuera del pentagrama, pero también se evita arrastrar errores del 
    pasado.

    <LI> <B> Método v2:</B> Algoritmo de Viterbi. Se trata del método clásico de 
    progamaci ón dinámica, similar al descrito en la sección 1.2, en el cual se 
    busca la secuencia de estados (en este caso, categorías) de mayor probabilidad. 
    En este caso, las probabilidades de transición entre estados vendrían modeladas 
    por el modelo de lenguaje, mientras que la probabilidad de observación de cada 
    estado vendría dada por la probabilidad léxica.
    
    <LI> <B> Método v3:</B> Ventana deslizante mejorado. Variante del método de la 
    ventana deslizante, consistente en considerar las probabilidades léxicas de 
    todas las palabras de la ventana, y no sólo de aquélla sobre la que se decide. 
    De esta forma, aumenta la importancia del modelo léxico respecto al método v1.
    
    <LI> <B>Método v4:</B> Variante del método anterior, en la que se modifica la 
    importancia del modelo léxico frente al contextual, en función cada clase de 
    ambigüedad en concreto.
</UL>


<H2>Las clases de ambigüedad</H2>

<P> Una clase de ambigüedad se define como el grupo de palabras del Corpus de 
entrenamiento que comparten un mismo conjunto de etiquetas que se les pueden 
asignar a priori como, por ejemplo, nombre-adjetivo. De esta forma, se puede 
emplear el concepto de clase de ambigüedad para calcular la probabilidad de que 
una palabra dada tenga una categoría determinada, independientemente de su 
contexto, lo que se conoce como modelo léxico. Aunque, evidentemente, la 
situación ideal sería calcular dicha probabilidad para cada palabra en concreto,
 la gran cantidad de texto correctamente etiquetado que sería necesario para 
estimar dichas probabilidades hace que la aproximación basada en clases de 
ambigüedad sea muy razonable.
  
  */
  
   
#ifndef _ANALISIS_MORFOSINTACTICO_HPP
    #define _ANALISIS_MORFOSINTACTICO_HPP

#include "modos.hpp"
#include "tip_var.hpp"
#include "perfhash.hpp"
#include "interfaz_ficheros.hpp"

/* Tenemos que acordarnos de eliminar el #define del tip_var.h N_CAT_ */


#define MAX_LENGUAJES 10

//#define LAMBDA 0.375

//#define PESO_MODELO_LENGUAJE 0.8F

#define puntuacion_sin_final(cat)  ( (cat == APERTURA_PARENTESE) || (cat == PECHE_PARENTESE) ||  \
                                     (cat == APERTURA_CORCHETE) || (cat == PECHE_CORCHETE) || \
                                     (cat == GUION) || (cat == DOBLES_COMINNAS) || \
                                     (cat == SIMPLES_COMINNAS) || (cat == GUION_BAIXO) )

#define maximo(a, b) ( ((a) > (b)) ? a : b)
#define minimo(a, b) ( ((a) < (b)) ? a : b)


typedef unsigned char Pentagrama[5];

typedef unsigned char Tetragrama[4];

typedef unsigned char Trigrama[3];

typedef unsigned char Bigrama[2];


/*
 *class Lenguaje {
 *public:
 *  int numero_usuarios;
 *  char * nombre;
 *  Modelo_lenguaje modelo;
 *  Lenguaje(char * ruta, char *l);
 *  ~Lenguaje();
 *}
 */


int adjetivo_sustantivable(t_frase_separada *frase_separada, t_frase_separada *palabra_actual,
						   int numero_elementos);

int infinitivo_sustantivable(t_frase_separada *frase_separada, t_frase_separada *palabra_actual);






int convierte_modelo_lenguaje_a_categorias(t_frase_separada *frase_separada,
                                           int numero_elementos,
                                           Categorias_modelo *categorias, char tipo);

int convierte_modelo_lenguaje_a_categorias_completo(t_frase_separada *frase_separada,
                                           int numero_elementos,
                                           Categorias_modelo *categorias);


float calcula_probabilidad_backoff(unsigned char *clave, int tamano_clave);


Categorias_modelo encuentra_categoria_probabilidad_maxima(unsigned char *probabilidades);

void calcula_probabilidades_homografas(t_frase_separada *palabra, 
                                       float *probabilidad_adjetivo,
                                       float *probabilidad_adverbio,
                                       float *probabilidad_infinitivo,
                                       float *probabilidad_nombre,
                                       float *probabilidad_participio,
                                       float *probabilidad_verbo);


void comprueba_locuciones(t_frase_separada *recorre_frase_separada, char *locucion);

int comprueba_contracciones(t_frase_separada *recorre_frase_separada, int opcion);

void calcula_probabilidad_numero_numerales(t_frase_separada *recorre_frase_separada,
                                           float *prob_singular, float *prob_plural,
                                           float *prob_det, float *prob_pron);

void elimina_categorias_no_contraccion(t_frase_separada *recorre_frase_separada);



char *devuelve_categorias_genericas_cadena(int indice);




class
#ifdef _WIN32
__declspec(dllexport) 
#endif
	Analisis_morfosintactico : public Interfaz {


		Modelo_lenguaje * modelo_lenguaje;


		int convierte_a_categorias_modelo_lenguaje_completo(t_frase_separada *frase_separada,
				int *numero_elementos,
				estructura_categorias *lista_categorias);

		public:

		void calcula_probabilidades_clases_ambiguedad(char *mascara,
				estructura_categorias *categorias);

		int aplica_modelo_lenguaje_v1(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);

		int aplica_modelo_lenguaje_v2(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);

		int aplica_modelo_lenguaje_v3(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);

		int aplica_modelo_lenguaje_v4(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);

		int aplica_modelo_lenguaje_v5(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);

		int aplica_modelo_lexico(estructura_categorias *lista_categorias, int numero_elementos,
				Categorias_modelo *categorias);


		int convierte_a_categorias_modelo_lenguaje(t_frase_separada *frase_separada,
				int *numero_elementos,
				estructura_categorias *lista_categorias);


		static Modelo_lenguaje ** modelos_lenguaje;
		static int numero_modelos_lenguaje;

		char ** lenguajes_cargados;
		int numero_lenguajes_cargados;
		char saca_prob;
		CotoviaEagles *oCE;
		public:


		Analisis_morfosintactico(){lenguajes_cargados = NULL;numero_lenguajes_cargados = 0;};
		//~Analisis_morfosintactico();
		int decide_categorias(t_frase_separada *frase_separada, int numero_elementos,
				int idioma);


		int carga(char * ruta_bd, char * lenguaje);
		void descarga();
		Modelo_lenguaje *lenguaje_cargado(char * l);

		int analisis_morfosintactico(t_frase_separada * frase,
				char * nombre_fichero_analisis, FILE * f_analisis);

		int analisis_morfosintactico(t_frase_separada * frase, int n_elementos, char *directorio_salida,
				char *frase_extraida, char idioma, char tipo);


		int sacar_analisis_morfosintactico( char * frase_extraida,
				t_frase_separada * frase_separada,
				char * frase_procesada,
				char * frase_sil_e_acentuada,
				char * frase_fonetica,
				t_frase_sintagmada * frase_sintagmada,
				t_frase_en_grupos * frase_en_grupos,
				t_prosodia * frase_prosodica,
				FILE * &flin,
				char * fsalida,
				char tipo, char separa_lin, char idioma);

		void selecciona_lenguaje(char * lenguaje);


		int sacar_analisis_morfosintactico( char * frase_extraida, t_frase_separada * frase_separada, 
					FILE * &flin,	char * fsalida, char tipo, char separa_lin, char idioma);
		int sacar_verbos_y_contracciones_expandidas(t_frase_separada * it, FILE * &flin, char * fsalida, char separa_lin, char idioma);
	};
#endif


