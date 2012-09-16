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
 

/** \file grupos_acentuales.hpp
 *
 * En el m�dulo grupos_acentuales se encuentran todas las funciones relacionadas 
 * con la manipulaci�n de los grupos acentuales, la unidad b�sica para la 
 * generaci�n de contornos de frecuencia en el sintetizador.
 */ 

#ifndef _GRUPOS_ACENTUALES_HPP

#define _GRUPOS_ACENTUALES_HPP

#include <stdio.h>
#include "modos.hpp"

#define NOMBRE_FICHERO_ACENTUALES "clusters_acentuales_"
#define FICHERO_ACENTUALES "clusters_acentuales.bin"
#define FICHERO_MEDIAS_GRUPOS_ACENTUALES "medias_grupos.bin"
#define FICHERO_CONTEXTOS_GRAMATICALES "contextos_gramaticales.txt"
#define FICHERO_INSERCION_R_E "insercion_rupturas.txt"

#define INDICE_MAXIMO_FREIRE 4348   ///< Identificador m�s alto de los grupos 
                                    ///< acentuales del locutor Freire.
                                    
#define INDICE_MAXIMO_PAULINO 4346  ///< Identificador m�s alto de los grupos 
                                    ///< acentuales del locutor Paulino.


#define MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS 2.0F	///< M�xima diferencia de frecuencia fundamental entre
///< los contornos de dos grupos acentuales.

#define MAXIMA_VARIACION_CONTORNO_ZONA_NO_ACENTUADA 50.0F ///< M�xima variaci�n de f0 de una s�laba a la siguiente en la
///< zona del contorno que no se corresponde con el acento. Si es mayor, es muy probable que
///< se trate de un error en la estimaci�n de la f0.
                            
#define SIN_PAUSA 0
#define PAUSA_COMA 1L
#define PAUSA_PUNTO 2L                ///< Tambi�n consideramos punto y coma; Los dos puntos los pasamos a otro_signo_puntuacion
#define PAUSA_PUNTOS_SUSPENSIVOS 3L
#define PAUSA_RUPTURA_ENTONATIVA	4L
#define PAUSA_RUPTURA_COMA			5L
#define PAUSA_OTRO_SIGNO_PUNTUACION  6L ///< Aqu� vamos a englobar comillas (simples y dobles),
                                        ///< guiones...
                                        ///< En la versi�n actual se supone que las comillas no introducen pausa.

#define NUMERO_POSIBLES_PAUSAS 7


#define UMBRAL_GRUPO_ACENTUAL	0.3F	    ///< Umbral de variaci�n respecto a la media del
///< propio grupo para considerar que es un grupo.
///< v�lido.
                                        
#define UMBRAL_PENDIENTE_GRUPO_ACENTUAL 2.0F	///< Umbral semejante al anterior referido a la
///< diferencia entre el primer valor y el �ltimo.
												                      
#define GRUPO_AGUDO 0
#define GRUPO_LLANO 1
#define GRUPO_ESDRUJULO 2
#define GRUPO_HUERFANO 3

#define TIPOS_GRUPO_SEGUN_ACENTO 3 // Vamos a obviar de momento los grupos hu�rfanos, ya que se consideran parte del grupo anterior.

#define GRUPO_INICIAL 0
#define GRUPO_MEDIO 1
#define GRUPO_FINAL 2
#define GRUPO_INICIAL_Y_FINAL 3

#define TIPOS_GRUPO_SEGUN_POSICION 4

// Grupos para considerar la estructura de la s�laba.

// #define OCLUSIVAS_SORDAS_CODA	0
// #define OCLUSIVAS_SORDAS_ONSET	1
// #define OCLUSIVAS_SORDAS_CODA_Y_ONSET	2
// #define RESTO_SILABAS	3

#define SORDAS_CODA	0
#define SORDAS_ONSET	1
#define SORDAS_CODA_Y_ONSET	2
#define RESTO_SILABAS	3


#define TIPOS_ESTRUCTURAS_SILABICAS 4

#define PARTE_SONORA	0
#define PARTE_OCLUSIVA		1
#define	PARTE_SORDA			2
#define PARTE_OCLUSIVA_Y_SONORA	3
#define PARTE_SONORA_Y_SORDA 4
#define PARTE_OCLUSIVA_Y_SORDA 5
#define VACIO	6


// Tipos de grupos acentuales seg�n la categor�a morfosint�ctica de la palabra acentuada.

#define GA_NOMBRE	0
#define	GA_ADJETIVO	1
#define GA_VERBO	2
#define	GA_PRONOMBRE	3
#define GA_ADVERBIO	4
#define GA_INTERROGATIVO_EXCLAMATIVO	5
//#define GA_EXCLAMATIVO	6
#define GA_DETERMINANTE 6 // Acabo de a�adirlo.
#define GA_OTROS	7

#define CLASES_GRUPOS_ACENTUALES_SEGUN_CATEGORIA_MORFOSINTACTICA	8

// Clasificaci�n sintagm�tica empleada en el modelo entonativo.

#define GA_S_NOMINAL	0
#define	GA_S_ADJETIVAL	1
#define GA_S_VERBAL	2
#define	GA_S_PREPOSICIONAL	3
#define GA_S_ADVERBIAL	4
#define GA_COMA		5
#define GA_RUPTURA_ENTONATIVA 6
#define GA_RUPTURA_COMA	7
#define GA_PUNTO	8 // Incluimos ., :, ;, ...
#define GA_RELATIVO 9
#define GA_CONJ_COOR_ADVERS 10
#define GA_CONJ_COOR_COPU 11
#define GA_CONJ_COOR_DIS 12
#define GA_CONJ_SUBOR 13
#define GA_APERTURA_PAR 14
#define GA_CIERRE_PAR 15
#define GA_APERTURA_INT_EXCLA 16
#define GA_CIERRE_INT_EXCLA 17
#define GA_NO_CLASIFICADO 18
#define GA_DOBLES_COMILLAS 19
#define GA_OTROS_SINTAGMAS	20

#define NUMERO_SINTAGMAS_MODELO_ENTONATIVO	21


#define NUMERO_GRUPOS_POSIBLES 48  ///< �ndice m�s alto, seg�n la macro calcula_indice_grupo (+1)

#define NUMERO_CLUSTERES 48       ///< Ojo, hay que modificar esta macro y NUMERO_GRUPOS_POSIBLES 
                                  ///< al cambiar los grupos.
                                  
// En este caso, coindiden los valores de las dos �ltimas macros, pero no tiene por qu�
// ser as�. Por ejemplo, si s�lo hubiese 3 posibles valores para el tipo de posici�n (ver macro
// calcula_indice_grupo, variable b), en ese caso NUMERO_GRUPOS_POSIBLES SER�A 44, y
// NUMERO_CLUSTERES, 36 (4*3*3).

#define calcula_indice_grupo(a, b, c)  ((c)<<4) | ((b)<<2) | (a)

// a y c -> POS de los grupos candidato y objetivo. (8 posibilidades)
// b y d -> siguiente sintagma de los grupos candidato y objetivo (21 posibilidades)
// e -> Tipo de pausa del grupo candidato. (4 posibilidades).
#define crea_clave_hash_contextos(a,b,c,d,e) ((e) +  10*(c) + 1000*(a) + 100000*(d) + 100000000*(b))

// a -> sintagma actual
// b -> siguiente sintagma
#define crea_clave_hash_inserciones(a,b) ((a) + 100*(b))

/** \struct estructura_marcas_frecuencia
 *
 * Estructura que almacena un valor de frecuencia y el instante temporal en que 
 * �ste fue medido. Tambi�n guarda informaci�n acerca de la validez de dicho valor 
 * de frecuencia.
 */ 

typedef struct {

  float tiempo;      ///< Instante de c�lculo de la frecuencia, en segundos.
  float frecuencia;  ///< Valor del contorno de frecuencia (Hz) en dicho instante.
#ifdef _INCLUYE_SILABAS
  char silaba[10];
#endif
  char tipo_silaba;
  char sonoridad_pobre; ///< Indica si hemos tomado un valor de pitch no realmente estimado
  ///< (== 0 si es un valor v�lido, 1 en otro caso)
} estructura_marcas_frecuencia;


/** \struct Frecuencia_estilizada
 *
 * Estructura en la que se almacena la informaci�n sobre la forma del contorno 
 * de frecuencia del grupo acentual.
 */ 

typedef struct {

#ifndef _x86_64
  int dummy_1;
#endif
  estructura_marcas_frecuencia *marcas;   ///< Cadena de estructuras del tipo anterior en la
  ///< que se encuentra la informaci�n del contorno
  ///< de frecuencia. Se toma un valor por s�laba,
  ///< calculado en el punto medio de la vocal fuerte,
  ///< y otros dos que marcan el inicio y al final del grupo.

  float tiempo_inicio;  ///< Instante de inicio del grupo acentual en
  ///< la frase de la que fue extra�do.

  float tiempo_final;   ///< Instante final del grupo acentual en
  ///< la frase de la que fue extra�do.

  int numero_marcas;    ///< N�mero de elementos del campo marcas.
#ifndef _x86_64
  int dummy_2; // padding del objeto completo
#endif

} Frecuencia_estilizada;


/** \struct estructura_silaba
 *
 * Estructura en la que se almacenan datos relativos a la s�laba.
 */
   
typedef struct {
  unsigned char fonemas_por_silaba;  ///< N�mero de fonemas de la s�laba.
    
  unsigned char vocal_fuerte;        ///< Posici�n de la vocal fuerte.
    
  unsigned char tipo_silaba;         ///< Estructura de la s�laba seg�n los 
  ///< fonemas que la componen.
                                       
  float tiempo_silaba;               ///< Duraci�n estimada de la s�laba.
    
  float tiempo_hasta_nucleo;         ///< Duraci�n de la s�laba hasta la
  ///< mitad de la vocal fuerte.
} estructura_silaba;



/** \class Grupo_acentual_candidato
 *
 * Esta clase es formalmente equivalente a Vector_descriptor_candidato. Si �sta
 * �ltima se empleaba para almacenar la informaci�n de las unidades ac�sticas
 * del Corpus, la clase Grupo_acentual_candidato est� dedicada a las unidades
 * b�sicas del sintetizador para la generaci�n de los contornos de frecuencia.
 * En esta clase se almacena la informaci�n de los grupos acentuales extra�dos
 * del Corpus.
 */

class Grupo_acentual_candidato {

public:

  int identificador; ///< Identificador num�rico �nico del grupo acentual.

  unsigned char tipo_grupo_prosodico_1; ///< Tipo de grupo pros�dico, tal y como viene
                     
  ///< en la variable global frase_prosodica.

  unsigned char tipo_grupo_prosodico_2; ///< Tipo de grupo pros�dico, dentro de una clasificaci�n
  ///< m�s general, despu�s de pasar la variable anterior por
  ///< la funci�n adaptar_grupo_prosodico.

  unsigned char tipo_grupo_prosodico_3; ///< Tipo de grupo pros�dico reducido a la clasificaci�n 
  ///< de los tipos de frase, es decir, enunciativo, interrogativo,
  ///< exclamativo o inacabado.

  unsigned char tipo_pausa; ///< Tipo de pausa en la que termina el grupo acentual.

  unsigned char tipo_pausa_anterior; ///< Tipo de pausa anterior al grupo acentual.
    
  char posicion_silaba_acentuada; ///< Indica la posici�n de la s�laba acentuada, desde el final
  ///< del grupo. (0, 1 � 2).
                                  
  char posicion_grupo_prosodico;  ///< Posici�n del grupo pros�dico al que pertenece
  ///< el grupo acentual dentro de la frase.

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL

  unsigned char etiqueta_morfosintactica;	///< Etiqueta morfosint�ctica de la palabra que lleva el acento.

  unsigned char sintagma_actual;			      ///< Identificador del sintagma al que pertenece el grupo.

  unsigned char siguiente_sintagma;		  ///< Identificador del siquiente sintagma.
    
#endif
    
  short int indice_maximo;  ///< �ndice del campo marcas de la estructura frecuencia_estilizada
  ///< que se corresponde con el m�ximo de frecuencia fundamental.
                              
  short int grupos_acentuales_grupo_prosodico; ///< N�mero total de grupos acentuales en el grupo pros�dico.

  short int posicion_grupo_acentual;  ///< Posici�n del grupo acentual en el grupo pros�dico.

  short int numero_silabas;          ///< N�mero de s�labas del grupo acentual.

  short int numero_grupos_prosodicos; ///< N�mero total de grupos pros�dicos en la frase.

  float inicio;   ///< Instante de inicio del grupo acentual en la frase
  ///< de la que fue extra�do, en segundos.

  float final;    ///< Instante de finalizaci�n del grupo acentual en la
  ///< frase de la que fue extra�do, en segundos.

  float frecuencia_media; ///< Frecuencia media del grupo acentual.

  float factor_caida;     ///< Par�metro que mide la ca�da de frecuencia
  ///< fundamental al final del grupo; Se mide a
  ///< partir de la pendiente de la recta entre el
  ///< punto en el que recae el acento y el �ltimo,
  ///< aunque tal vez ser�a interesante medirlo entre
  ///< el punto de frecuencia m�s alta y el �ltimo.

  float salto_f0;	///< Par�metro que mide la diferencia de f0 entre el valor
    			///< en la s�laba acentuada y el de la s�laba anterior.


  int numero_frase;                  ///< N�mero de la frase del archivo original.

  Frecuencia_estilizada frecuencia; ///< Estructura de tipo frecuencia_estilizada en la que se almacena el contorno de frecuencia del grupo acentual.

  //  char *fichero_origen;

  /// Constructor de la clase.
    
  Grupo_acentual_candidato();
    
  /// Devuelve el identificador del grupo acentual.

  int devuelve_identificador();

  /// Reorganiza varias secuencias de grupos acentuales candidatos en funci�n de sus costes
  static void reorganiza_caminos_segun_costes(Grupo_acentual_candidato **contornos_escogidos, int numero_contornos_actuales, float *costes_minimos, int *relacion_original);
  
  /// Selecciona un conjunto de contornos en base al criterio indicado
  static int selecciona_mejores_opciones(Grupo_acentual_candidato **contornos, int numero_grupos_totales, int numero_grupos, int numero_grupos_acentuales, char criterio, float *costes_minimos, int *relacion_original);
  
  /// Copia el contenido de un grupo acentual
  int copia_grupo_acentual(Grupo_acentual_candidato *original);
    
  /// Escribe en el fichero que se le pasa como par�metro la informaci�n 
  /// del grupo acentual, en formato texto.
    
  int escribe_datos_txt(FILE *fichero);

  void escribe_contorno(FILE *fichero);
	  
  /// Asigna un valor al identificador num�rico del grupo.
	  
  void anhade_identificador(int identificador);
    
  //  int anhade_fichero_origen(char *nombre_fichero);

  /// Escribe en el fichero que se le pasa como par�metro la informaci�n del 
  /// grupo acentual, en formato binario.

  int escribe_datos_bin(FILE *salida);
    
  /// Lee del fichero que se le pasa como par�metro la informaci�n del grupo 
  /// acentual, en el formato originado por la funci�n escribe_datos_bin.
    
  int lee_datos_bin(FILE *entrada);
    
  /// Lee del fichero que se le pasa como par�metro la informaci�n del grupo 
  /// acentual, en el formato originado por la funci�n escribe_datos_txt.    
    
  int lee_datos_txt(FILE *fichero);
    
  void escribe_datos_txt_tabla(FILE *salida);
    
  /// Escribe la parte est�tica del objeto en el fichero que se le pasa como par�metro.
    
  void escribe_parte_estatica(FILE *fichero);
    
  /// Escribe la parte din�mica del objeto en el fichero que se le pasa como par�metro.
    
  void escribe_parte_dinamica(FILE *fichero);
    
  /// Lee la informaci�n de la parte din�mica de la cadena que se le pasa como par�metro.
    
  void lee_parte_dinamica(char **cadena);
    
  /// Libera la memoria.
    
  void libera_memoria();
};



/** \struct estructura_grupos_acentuales
 * 
 * Estructura en la que se almacenan todos los grupos acentuales de un mismo tipo.
 * 
 */  
  
typedef struct {

  int numero_grupos;       ///< N�mero de grupos acentuales de un mismo tipo.
#ifndef _x86_64
  int dummy_1;
#endif
  Grupo_acentual_candidato *grupos; ///< Cadena con los grupos acentuales de ese tipo.
#ifndef _x86_64
  int dummy_2;
#endif

} estructura_grupos_acentuales;


/**
 * \struct estructura_grupos_acentuales_compatible
 *
 * Estructura empleada al escribir en disco (por compatibilidad entre 32 y 64 bits)
 *
 */
typedef struct {

#ifndef _x86_64
  int dummy;
#endif
  estructura_grupos_acentuales *puntero;
} estructura_grupos_acentuales_compatible;


/** \class Grupo_acentual_objetivo
 *
 * Esta clase es an�loga a Vector_descriptor_objetivo, con la diferencia 
 * de que se emplea para almacenar la informaci�n de los grupos acentuales de la 
 * frase que se desea sintetizar.
 */  
 
class Grupo_acentual_objetivo {

public:

  float duracion;                 ///< Duraci�n estimada del grupo acentual.
    
  float duracion_hasta_acento;    ///< Duraci�n de la parte del grupo acentual anterior al acento.

  float factor_caida;     ///< Par�metro que mide la ca�da de frecuencia
  ///< fundamental al final del grupo; Se mide a 
  ///< partir de la pendiente de la recta entre el 
  ///< punto en el que recae el acento y el �ltimo,
  ///< aunque tal vez ser�a interesante medirlo entre
  ///< el punto de frecuencia m�s alta y el �ltimo.
 
  float salto_f0;	///< Par�metro que mide la diferencia de f0 entre el valor 
    			///< en la s�laba acentuada y el de la s�laba anterior.

  unsigned char tipo_proposicion; ///< Tipo de proposici�n a la que pertenece el grupo (enun, ...).
    
  char posicion_silaba_acentuada; ///< Indica la posici�n de la s�laba acentuada desde el final
  ///< del grupo (0, 1 � 2).
                                    
  char posicion_grupo_prosodico;  ///< Posici�n del grupo pros�dico dentro de la frase.
    
  unsigned char indice_alofono_tonico; ///< Posici�n del al�fono en el que recae el acento.
       
  short int grupos_acentuales_grupo_prosodico; ///< N�mero total de grupos acentuales en el grupo pros�dico.
    
  short int posicion_grupo_acentual;           ///< Posici�n del grupo actual en el grupo pros�dico.

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
  unsigned char etiqueta_morfosintactica;	///< Etiqueta morfosint�ctica de la palabra que lleva el acento.
  unsigned char sintagma_actual;			///< Identificador del sintagma al que pertenece el grupo.
  unsigned char numero_elementos_sintagma;	///< N�mero de palabras t�nicas que componen el sintagma
  unsigned char siguiente_sintagma;		///< Identificador del siquiente sintagma.
#endif

  short int numero_silabas;           ///< N�mero de s�labas del grupo acentual.
    
  short int numero_grupos_prosodicos; ///< N�mero total de grupos pros�dicos en la frase.

  short int numero_alofonos;         ///< N�mero de fonemas del grupo, preciso para la sincronizaci�n
  ///< entre la cadena de grupos acentuales y la de descriptores.
                                       
  unsigned char tipo_pausa;          ///< Tipo de pausa en la que termina el grupo acentual.
    
  unsigned char tipo_pausa_anterior;	///< Tipo de pausa anterior al grupo acentual.
    
  unsigned char tipo_proposicion2; ///< Tipo de proposici�n, pero tal y como viene en la 
  ///< variable global frase_prosodica, sin limitarlo a 
  ///< los tipos del campo tipo_proposicion.
                                     
  int numero_frase;                  ///< N�mero de la frase en el fichero.

  estructura_silaba *silabas;      ///< Estructura en la que se almacena informaci�n de
  ///< de todas y cada una de las s�labas del grupo.

  void crea_grupo_objetivo(Grupo_acentual_candidato *candidato);
	  
  /// Escribe en el fichero que se le pasa como par�metro la informaci�n
  /// del grupo acentual, en formato texto.
    
  void escribe_datos_txt(FILE *fichero);
    
  /// Libera la memoria asignada a las variables din�micas del objeto.
    
  void libera_memoria();
};
  
/// Devuelve el valor de frecuencia fundamental en el instante que se le pasa 
/// como par�metro, interpolado a partir de los datos de la estructura de tipo 
/// Frecuencia_estilizada.

float interpola(Frecuencia_estilizada *marcas_frecuencia, float x, char *sonoridad);

/// Carga en memoria la informaci�n del fichero de frecuencia estilizada, seg�n 
/// el formato short text File del Praat.

int carga_fichero_frecuencia_estilizada(FILE *fichero, Frecuencia_estilizada *frecuencia_estilizada);

/// Asigna memoria para la tabla en la que se almacenan los grupos acentuales 
/// del locutor que se le indica como par�metro.

int inicia_tabla_grupos();

/// A�ade un grupo acentual al cluster que le corresponde.

int anhade_grupo_acentual(Grupo_acentual_candidato *nuevo_grupo, FILE *fichero, int opcion);

/// Comprueba errores posibles en la estimaci�n del contorno de f0.

int comprueba_error_estimacion_contorno(Grupo_acentual_candidato *grupo);

/// Escribe en uno o varios ficheros la informaci�n de los grupos acentuales 
/// organizados en clusteres.

int escribe_grupos_ordenados(char unico_fichero, char locutor[]);

/// Lee de un fichero la informaci�n de los diferentes clusteres de grupos 
/// acentuales.

int lee_tabla_de_fichero(FILE *fichero);

/// Extrae del parametro parametros la informaci�n de tipo de frase, 
/// posici�n y s�laba acentuada.

void devuelve_parametros_estructura_matriz(char parametros, unsigned char *proposicion,
					   unsigned char *posicion, unsigned char *silaba_acentuada);
                        
/// Empaqueta la informaci�n de tipo de frase, posici�n y s�laba acentuada en 
/// el par�metro parametros.

void crea_parametros_estructura_matriz(char *parametros, unsigned char proposicion,
				       unsigned char posicion, unsigned char silaba_acentuada);
                        
/// Libera la memoria dedicada a los clusteres de grupos acentuales.

void libera_memoria_tabla_grupos();

estructura_grupos_acentuales *devuelve_puntero_a_tabla(int indice);

/// Devuelve un puntero a la tabla de grupos acentuales.

estructura_grupos_acentuales **devuelve_tabla(int *numero_elementos);

/// Devuelve el valor de la frecuencia media de la estructura de tipo.

float calcula_frecuencia_media_grupo(Frecuencia_estilizada *frecuencia);

float encuentra_maxima_variacion_absoluto_grupo(Frecuencia_estilizada *frecuencia);

/// Calcula la frecuencia m�s alta de la estructura de tipo Frecuencia_estilizada
/// que se le pasa como par�metro.

short int calcula_frecuencia_maxima_grupo(Frecuencia_estilizada *frecuencia);

short int calcula_frecuencia_maxima_grupo_absoluto(Frecuencia_estilizada *frecuencia);

/// Calcula el factor de ca�da del grupo acentual que se le pasa como par�metro.

float calcula_factor_caida(Grupo_acentual_candidato *grupo);

/// Calcula el salto de f0 del grupo acentual que se le pasa como par�metro

int calcula_salto_f0(Grupo_acentual_candidato *grupo, float *salto_f0);

/// Devuelve los estad�sticos de media y varianza de los clusteres de grupos 
/// acentuales del Corpus.

void calcula_estadisticas_factor_caida(FILE *fichero_salida);

/// Calcula la media y la varianza de la frecuencia de los grupos acentuales 
/// del Corpus.

void calcula_frecuencia_media_grupos_acentuales(double *frecuencia,
                                                double *varianza, int *numero_valores);

int crea_fichero_frecuencia_media_grupos_acentuales_por_grupos(FILE *fichero,
							       char opciones[]);
								                                                
/// Escribe en el fichero que se le pasa como par�metro los datos de frecuencia 
/// media de todos los grupos acentuales del Corpus.								                                                

void crea_fichero_frecuencia_media_grupos_acentuales(FILE *fichero);

/// Escribe la informaci�n de todos los grupos acentuales del Corpus.  

void escribe_datos_grupos_acentuales(FILE *fichero);

/// Escribe las frecuencias de los grupos acentuales

void escribe_frecuencias_corpus_prosodico(FILE *fichero);

/// Adapta el rango din�mico y el valor medio de la base de datos pros�dica

void adapta_valor_medio_y_rango_dinamico(float antigua_media, float nueva_media, float antiguo_rango, float nuevo_rango);

int escribe_frecuencia_inicial_o_final_grupos_entonativos(char *nombre_locutor, int indice,
							  char opcion_f0, char opcion_pausa,
							  char opcion_intervalo);

int calcula_distancia_entre_contornos_cuadratica(Grupo_acentual_candidato *contorno_1,
						 Grupo_acentual_candidato *contorno_2,
						 int numero_grupos_acentuales,
						 float *distancia);

int calcula_distancia_entre_contornos_cuadratica(Grupo_acentual_candidato *contorno_1,
						 Grupo_acentual_candidato *contorno_2,
						 int numero_grupos_acentuales,
						 float *distancia,
						 float *distancia_media);

int calcula_distancia_entre_contornos_grupos(Grupo_acentual_candidato *contorno_1,
					     Grupo_acentual_candidato *contorno_2,
					     int numero_grupos_acentuales,
					     float *distancia);

void asigna_tabla_grupos(estructura_grupos_acentuales **nueva_tabla) ;

void escribe_informacion_corpus_prosodico(FILE *fichero);

int lee_contorno_fichero(Grupo_acentual_candidato **grupos, int *numero_grupos, int opcion, FILE *entrada);

void comprueba_y_asegura_continuidad_contornos(Grupo_acentual_candidato *acentuales, int numero_acentuales);

void modifica_contorno_grupo_acentual(float frecuencia_inicio, float frecuencia_fin, Frecuencia_estilizada *contorno);

void modifica_contorno_grupo_acentual_natural(float frecuencia_inicio, float frecuencia_fin, Frecuencia_estilizada *contorno);
                                      
float coste_estructura_silabica(estructura_silaba *silabas_objetivo, int numero_silabas_objetivo, estructura_marcas_frecuencia *silabas_candidato, int numero_silabas_candidato, int posicion_silaba_acentuada);

int devuelve_alineado_indices(int indice_1, int total_1, int total_2);

int calcula_continuidad_f0_grupos_fonicos(FILE *fichero_distancias, FILE *fichero_no_encontrados);

int encuentra_grupo_acentual_corpus(int identificador, Grupo_acentual_candidato **grupo_acentual);

int devuelve_parametros_salto_f0_silabico(FILE *fichero);

int devuelve_parametros_salto_f0_silabico_y_categoria_morfosintactica(FILE *fichero, char categoria, char opcion, char tipo_pausa);

int escribe_tipo_categoria_morfosintactica_grupo(int categoria, char cadena_categoria[]);

int lee_tipo_categoria_morfosintactica_grupo(char *categoria, unsigned char *valor_categoria);

int escribe_tipo_sintagma_grupo(int sintagma, char cadena_sintagma[]);

int lee_tipo_sintagma_grupo(char *sintagma, unsigned char *valor_sintagma);

int lee_tipo_frontera(char *frontera, unsigned char *valor_frontera);

int lee_tipo_pausa_grupo_acentual(char *pausa, unsigned char *val_pausa);

#endif
