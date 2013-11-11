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
 

/** \file descriptor.hpp
 * En el m�dulo descriptor.hpp se encuentran definidas las clases
 * dedicadas al almacenamiento de la informaci�n pros�dica y contextual
 * relativa a los segmentos de voz, es decir, Vector descriptor objetivo,
 * Vector semifonema candidato, Vector descriptor candidato, Vector
 * difonema  y Vector sintesis. Cada una de ellas se emplea en una fase
 * diferente del proceso de s�ntesis de voz. As�, por ejemplo, la clase
 * Vector descriptor candidato se utiliza en la etapa de extracci�n
 * inicial de los datos de cada segmento de voz del Corpus grabado,
 * mientras que la clase Vector semifonema candidato la informaci�n de
 * las unidades de voz candidatas en el momento de la selecci�n de la m�s
 * apropiada para la s�ntesis.
 */

#ifndef _VECTOR_DESCRIPTOR

#define _VECTOR_DESCRIPTOR

#include <stdio.h>
#include <stdlib.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "fonemas.hpp"


#define PRIMER_IDENTIFICADOR_DIFONEMAS 30000000 //Se define aqu� si no existe difonemas.hpp

// Tama�o de la cabecera de los ficheros binarios donde se almacenan los semifonemas
// candidatos. Se emplea para las variables est�ticas de la clase.
#define TAMANO_CABECERA_FICHERO_UNIDADES_ACUSTICAS	sizeof(int)

// Defines para los tipos de frontera de los fonemas.
#define SIN_FRONTERA 0
#define FRONTERA_SILABA 3
#define FRONTERA_PALABRA 4
#define FRONTERA_GRUPO_ENTONATIVO 6

#define TIPOS_FRONTERAS_FONEMA 4

// Defines para el tipo de frase al que pertenece la palabra en la que se
// encuentra el fonema.

#define FRASE_ENUNCIATIVA   0
#define FRASE_EXCLAMATIVA   1
#define FRASE_INTERROGATIVA 2
#define FRASE_INACABADA     3

#define NUMERO_TIPOS_PROPOSICIONES 4

// Defines para designar la tonicidad de la s�laba a la que pertenece el fonema.

#define TONICA 1
#define ATONA  0

#define GRADOS_ACENTO 2

// Defines para la posici�n del fonema dentro de su grupo

#define POSICION_INICIAL 0  // Desde el inicio hasta la primera s�laba t�nica (incluida).
#define POSICION_MEDIA   1  // Entre la primera s�laba t�nica y la �ltima.
#define POSICION_FINAL   2  // Despu�s de la �ltima s�laba t�nica (incluida).

#define NUMERO_POSICIONES_FRASE 3

#define UNIDAD_NO_FRONTERA	0	// Unidades intermedias o previas a una pausa que no es punto.
#define UNIDAD_FIN_FRASE	1	// Unidades que quedan entre la �ltima s�laba t�nica y un punto.
#define UNIDAD_RUPTURA	2 // Para unidades en la vecindad de una ruptura entonativa o ruptura coma.

#define NUMERO_COEFICIENTES_CEPSTRALES 25 // �ste es el valor por defecto, el real se lee de la base de difonemas.
// Tiene que ser el m�s grande de los posibles, puesto que los proyectos
// crea_fichero_centroides y organiza_clases espectrales as� lo necesitan
// (es una cuesti�n de velocidad al ejecutarlos)

#define FRECUENCIA_MUESTREO_POR_DEFECTO	16000	// Valor que se emplea para inicializar la variable est�tica de la clase
// Vector_descriptor_objetivo.

/// Predeclaraci�n de la clase para permitir referencias cruzadas:
class Vector_semifonema_candidato;

/** \class Vector_difonema
 * La clase Vector difonema fue creada para permitir compatibilidad hacia
 * atr�s con la anterior versi�n del conversor, la cual empleaba como
 * unidad b�sica para la concatenaci�n el difonema. Esta clase adapta la
 * informaci�n de la antigua base de datos para que sea posible trabajar
 * con sus segmentos de voz trat�ndolos como semifonemas.
 */

class Vector_difonema {

  friend class Vector_descriptor_candidato;
  friend class Vector_semifonema_candidato;
  friend class Vector_sintesis;

public:

  static int numero_coeficientes_cepstrales;

  int identificador;                           ///< An�logo al de otras clases amigas; Para
  ///< distinguir si la unidad proviene de un difonema o de un semifonema,
  ///< se empiezan a enumerar a partir del valor
  ///< PRIMER_IDENTIFICADOR_DIFONEMAS definido en el fichero
  ///< difonemas.hpp.

  char fonema_izquierdo[OCTETOS_POR_FONEMA];   ///< Fonema inicial del difonema.
  char fonema_derecho[OCTETOS_POR_FONEMA];     ///< Fonema final del difonema.

  char posicion;                               ///< Indica la posici�n que ocupa en el log�tomo del
  ///< que procede.

  unsigned char validez;                                ///< Indica si es necesario realizar alg�n procesado
  ///< para modificar la prosodia de la unidad.

  char frontera_inicial_izquierda;                         ///< = Clasificaci�n seg�n el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final_izquierda;                            ///< = Clasificaci�n seg�n el tipo de frontera al final del fonema (ver el inicio de este fichero)
  char frontera_inicial_derecha;                         ///< = Clasificaci�n seg�n el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final_derecha;                            ///< = Clasificaci�n seg�n el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad est� en posici�n final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posici�n intermedia.

  float duracion_izquierda;                    ///< Duraci�n de la parte izquierda del difonema.
  float duracion_derecha;                      ///< Duraci�n de la parte derecha del difonema.

  float frecuencia_izquierda;                  ///< Frecuencia fundamental estimada en la parte inicial del difonema.
  float frecuencia_central;                    ///< Frecuencia fundamental estimada en la mitad del difonema.
  float frecuencia_derecha;                    ///< Frecuencia fundamental estimada en la parte final del difonema.

  float potencia_izquierda;                    ///< Energ�a estimada en la parte inicial del difonema.
  float potencia_central;                      ///< Energ�a estimada en la mitad del difonema.
  float potencia_derecha;                      ///< Energ�a estimada en la parte final del difonema.

  short int numero_marcas_izquierda;           ///< N�mero de marcas de pitch de la parte izquierda de la unidad.
  short int numero_marcas_derecha;             ///< N�mero de marcas de pitch de la parte derecha de la unidad.

  int indice;                                  ///< �ndice de la primera muestra del segmento de voz del
  ///< difonema en el fichero de sonido.

#ifndef _x86_64
  int dummy_1[2];
#endif
  int *marcas_derecha;                    ///< Vector de marcas de la parte derecha de la unidad.

#ifndef _x86_64
  int dummy_2;
#endif
  int *marcas_izquierda;                  ///< Vector de marcas de la parte izquierda de la unidad.


#ifndef _x86_64
  int dummy_3;
#endif
  float *cepstrum_izquierdo_inicio; ///< Vector de coeficientes cepstrales
  ///< al inicio de la parte izquierda del difonema.

#ifndef _x86_64
  int dummy_4;
#endif
  float *cepstrum_izquierdo_final; ///< Vector de coeficientes cepstrales
  ///< al final de la parte izquierda del difonema.
#ifndef _x86_64
  int dummy_5;
#endif
  float *cepstrum_derecho_inicio; ///< Vector de coeficientes cepstrales
  ///< al inicio de la parte derecha del difonema.
#ifndef _x86_64
  int dummy_6;
#endif
  float *cepstrum_derecho_final; ///< Vector de coeficientes cepstrales
  ///< al final de la parte derecha del difonema.
#ifndef _x86_64
  int dummy_7;
#endif
  float *cepstrum_medio_parte_izquierda; //< Vector cepstrum medio de la parte izquierda del
  ///< semifonema.
#ifndef _x86_64
  int dummy_8;
#endif
  float *cepstrum_medio_parte_derecha; ///< Vector cepstrum medio de la parte derecha del
  ///< semifonema.
#ifndef _x86_64
  int dummy_9;
#endif
  float *cepstrum_medio_siguiente_semifonema;  ///< Vector cepstrum medio del semifonema semifon-fut1.

#ifndef _x86_64
  int dummy_10;
#endif
  float *cepstrum_medio_anterior_semifonema;


  Vector_difonema();

  /// Asigna a la unidad un identificador.
  void anhade_identificador(int id);

  /// Introduce los nombres de los fonemas que constituyen el difonema.
  void anhade_fonemas(char fonema_izdo[], char fonema_dcho[]);

  /// Introduce los valores de la frecuencia fundamental al inicio, la mitad y el final del difonema.
  void anhade_frecuencia(float media, float inicial, float final);

  /// Almacena los valores de la duraci�n de las dos mitades del difonema.
  void anhade_duraciones(float dur1, float dur2);

  /// Almacena los valores de la energ�a al inicio, la mitad y el final del difonema.
  void anhade_potencia(float estacionaria, float inicial, float final);

  /// Introduce las cadenas de marcas de pitch, as� como su n�mero de elementos.
  void anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
			   short int num_derecha, int *marcas_dcha);


  int anhade_vectores_cepstrum(float *cepstrum_medio_izdo, float *cepstrum_medio_dcho,
			       float *cepstrum_medio_pas, float *cepstrum_medio_fut);

  int anhade_vectores_cepstrum_union(float *cepstrum_inicio_izdo,
				     float *cepstrum_final_izdo,
				     float *cepstrum_inicio_dcho,
				     float *cepstrum_final_dcho);

  /// Introduce el valor del �ndice de la unidad en el fichero de sonido de difonemas.
  void anhade_indice(int indice_fichero);

  /// Almacena la posici�n del difonema dentro del log�tomo.
  void anhade_posicion(char pos);

  /// Devuelve los nombres de los fonemas que forman el difonema.
  void devuelve_fonemas(char fonema_izdo[], char fonema_dcho[]);

  /// Devuelve el par�metro de modificaci�n pros�dica del difonema en el
  /// contexto en el que se va a sintetizar.
  unsigned char devuelve_validez();

  /// Devuelve el valor del identificador num�rico del difonema.
  int devuelve_identificador();

  /// An�loga a la de las clases similares.
  void libera_memoria();

  /// An�loga a la de las clases similares.
  void escribe_parte_estatica(FILE *fichero);

  /// An�loga a la de las clases similares.
  void escribe_parte_dinamica(FILE *fichero);

  /// An�loga a la de las clases similares.
  void lee_parte_estatica(FILE *fichero);

  /// An�loga a la de las clases similares.
  void lee_parte_dinamica(char **cadena);

  /// An�loga a la de las clases similares.
  void escribe_datos_txt(FILE *fichero, int frecuencia_muestreo);

  /// Escribe las variables est�ticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables est�ticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_descriptor_candidato
 *
 * La clase Vector descriptor candidato se emplea en la primera fase del
 * proceso de extracci�n de la informaci�n de las frases del Corpus de
 * voz grabado. Al igual que la clase anterior, toma como unidad b�sica
 * el fonema, dado que de esta forma no se duplican en memoria los datos
 * que comparten ambos semifonemas.
 *
 * Como es natural, esta clase comparte todas las variables de la
 * anterior, con la diferencia de que, en este caso, las frecuencias, energ�as y
 * duraciones se corresponden con los valores reales para cada fonema,
 * obtenidos a partir de los ficheros de marcas de pitch, formas de onda
 * y l�mites de fonemas, respectivamente.
 */


class Vector_descriptor_candidato {

  friend class Vector_sintesis;
  friend class Vector_difonema;

public:

  static int numero_coeficientes_cepstrales;
  static int frecuencia_muestreo;

  int identificador;                           ///< Identificador num�rico �nico del fonema, dentro de la base de datos.

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.
  char contexto_derecho[OCTETOS_POR_FONEMA];   ///< Identificador del contexto por la derecha.

  char contexto_izquierdo[OCTETOS_POR_FONEMA]; ///< Identificador del contexto por la izquierda.

  char contexto_derecho_2[OCTETOS_POR_FONEMA];    ///< Fonema siguiente al derecho.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA];  ///< Fonema anterior al izquierdo.

  unsigned char acento;                        ///< Indicador de si el fonema est� acentuado.
  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  unsigned char posicion;                      ///< Posici�n del semifonema en la frase (o grupo entre pausas).
  char frontera_inicial;                         ///< = Clasificaci�n seg�n el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Clasificaci�n seg�n el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad est� en posici�n final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posici�n intermedia.

  char tipo_espectral;                         ///< Clasificaci�n del semifonema seg�n el tipo
  ///< enumerado clase_espectral definido en
  ///< distancia_espectral.hpp
  char tipo_duracion;                          ///< Clasificaci�n del semifonema seg�n el tipo
  ///< enumerado clase_duracion definido en
  ///< modelo_duracion.hpp

  unsigned char validez_izda;                           ///< Marca de validez del semifonema izquierdo. Se
  ///< emplea en la fase de extracci�n de la informaci�n para se�alar
  ///< aquellas unidades que son incorrectas por alg�n motivo y que, por lo
  ///< tanto, no deber�an ser empleadas para la s�ntesis.

  unsigned char validez_dcha;                           ///< Marca de validez del semifonema derecho.

  short int numero_marcas_derecha;             ///< N�mero de marcas de pitch de la parte derecha de la unidad.
  short int numero_marcas_izquierda;           ///< N�mero de marcas de pitch de la parte izquierda de la unidad.

  float duracion_1;                              ///< Duraci�n del semifonema de la parte izquierda
  float duracion_2;                             ///< Duraci�n del semifonema de la parte derecha
  float frecuencia_mitad;                      ///< Frecuencia en la mitad del fonema.
  float frecuencia_derecha;                    ///< Frecuencia en el contexto derecho.
  float frecuencia_izquierda;                  ///< Frecuencia en el contexto izquierdo.

  float potencia;                              ///< Potencia en la zona estacionaria de la unidad.
  float potencia_inicial;                      ///< Potencia al inicio de la unidad.
  float potencia_final;                        ///< Potencia al final de la unidad.

  int indice_fichero;                          ///< �ndice al inicio de la forma de onda de la frase a
  ///< a la que pertenece la unidad dentro del fichero �nico de sonido.

  int indice_inicial;                          ///< �ndice a la primera posici�n de la unidad en el fichero de
  ///< sonido. (contexto por la izquierda). �ndice al inicio de la forma
  ///< de onda del fonema en el fichero de voz al que �ste pertenece.

  int indice_mitad;                            ///< �ndice a la mitad de la unidad (parte estacionaria). �ndice al
  ///< inicio del semifonema derecho dentro del fichero de voz al
  ///< que pertenece el fonema.


  int anterior_pitch;   ///< Valor del pitch entre la primera marca
  ///< del fonema y la �ltima del anterior.
  int ultimo_pitch;	    ///< Valor del pitch entre la ultima marca
                            ///< del fonema y la primera del siguiente.

#ifndef _x86_64
  int dummy_1;
#endif
  char *palabra;                               ///< Palabra a la que pertenece.


#ifndef _x86_64
  int dummy_2;
#endif
  int *marcas_derecha;                    ///< Vector de marcas de pitch del semifonema derecho de la unidad.
  ///< En realidad, todas las marcas de
  ///< pitch del fonema se encuentran almacenadas consecutivamente en
  ///< memoria, de tal forma que la varible anterior apunta al inicio de la
  ///< cadena, y �sta apunta a la primera marca que le corresponde.
#ifndef _x86_64
  int dummy_3;
#endif
  int *marcas_izquierda;                  ///< Vector de marcas de la parte izquierda de la unidad. Vector de marcas de pitch del
  ///< semifonema izquierdo de la unidad, necesario para aplicar el
  ///< algoritmo de modificaci�n pros�dica en la etapa final de procesado de se�al. Cada
  ///< marca de pitch consta de dos valores, el primero referido al
  ///< instante temporal de la marca en el fichero, mientras que el segundo
  ///< se refiere al car�cter sordo-sonoro de la propia marca. Aunque en la
  ///< versi�n actual ya no se emplea esta �ltima informaci�n, se sigue
  ///< manteniendo por compatibilidad con la versi�n inicial de difonemas
  ///< del sintetizador.

#ifndef _x86_64
  int dummy_4;
#endif
  float *cepstrum; // [4*NUMERO_COEFICIENTES_CEPSTRALES]; ///< Vectores mel-cepstrum de 12 coeficientes,
  ///< centrados en las marcas de pitch m�s cercanas
  ///< al inicio, mitad yfinal del fonema, con una ventana
  ///< de 16 milisegundos. �stos se calculan fuera de tiempo de
  ///< ejecuci�n por medio de la herramienta sig2fv del Edimburgh
  ///< Speech Tools.

#ifndef _x86_64
  int dummy_5;
#endif

#ifndef _x86_64
  int dummy_10; // padding del objeto completo
#endif


  Vector_descriptor_candidato();

  /// Funci�n similar a la de la clase Vector_descriptor_objetivo , en la que se almacenan
  /// todos los datos del fonema no relativos a la prosodia (tambi�n se declara la unidad como
  /// inicialmente v�lida para la s�ntesis).

  int introduce_datos(int ident, char *semifon, char *contexto_der,
		      char *contexto_izdo, char *contexto_der2, char *contexto_izdo2,
		      unsigned char ac, char *pal, unsigned char fr,
		      unsigned char posicion_frase, char primera_letra_palabra,
		      char ultima_letra_palabra);

  /// An�loga a la de Vector_descriptor_objetivo.

  void anhade_frecuencia(float media, float inicial, float final);

  /// An�loga a la de Vector_descriptor_objetivo. pero
  /// con un s�lo dato de duraci�n (aqu� se pierde la posibilidad de que
  /// los dos semifonemas de la unidad puedan tener diferente duraci�n,
  /// as� que si se quisiese tener esto en cuenta, habr�a que cambiarlo).

  void anhade_duracion(float dur1, float dur2);

  /// An�loga a la de Vector_descriptor_objetivo.

  void anhade_potencia(float estacionaria, float inicial, float final);

  int anhade_nombre_fichero(char *fichero);

  /// Almacena los �ndices del fonema, en muestras.
  void anhade_indices(int origen, int inicio, int mitad);

  ///  Guarda los datos relativos a las  marcas de pitch, es decir, los vectores
  ///  y su n�mero de elementos.

  void anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
			   short int num_derecha, int *marcas_dcha);

  /// Permite modificar el valor de las variables de validez del fonema.

  void anhade_validez(unsigned char modificable, char izq_der);

  /// Libera la memoria asignada a las variables din�micas del objeto.

  void libera_memoria();

  /// An�loga a la de Vector_descriptor_objetivo.

  int devuelve_fonemas(char central[], char izquierdo[], char derecho[]);

  /// An�loga a la de Vector_descriptor_objetivo.

  void devuelve_palabra(char palabra_contexto[]);

  ///Devuelve el valor de la duraci�n estimada del fonema.

  void devuelve_duracion(float *duracion_unidad, char izquierdo_derecho);

  /// Devuelve los valores estimados de las frecuencias al inicio, la mitad y el final del fonema.

  void devuelve_frecuencias(float *inicial, float *mitad, float *final);

  void devuelve_potencias(float *pot_inicial, float *pot_central, float *pot_final);

  /// Devuelve el tipo de frase a la que pertenece el fonema.

  unsigned char devuelve_tipo_frase();

  /// Devuelve la posici�n que ocupa el fonema dentro del grupo pros�dico al que pertenece.

  unsigned char devuelve_posicion();

  /// Devuelve el tipo de acento de la s�laba a la que pertenece el fonema.

  unsigned char devuelve_acento();

  /// An�loga a la de Vector_descriptor_objetivo

  void devuelve_frase_posicion_acento(unsigned char *fr, unsigned char *pos,
				      unsigned char *ac);

  /// Devuelve el identificador num�rico �nico de la unidad dentro de la base de datos.

  int devuelve_identificador();

  /// Devuelve el contador de posici�n de la unidad.
  int devuelve_contador();

  /// Devuelve la validez de la parte del fonema que se le pasa como par�metro.

  unsigned char devuelve_validez(char izda_dcha);

  /// Genera el fonema a partir de los semifonemas
  void crea_fonema(Vector_semifonema_candidato *izquierdo, Vector_semifonema_candidato *derecho);

  /// Funci�n que permite aprovechar la base de difonemas de la versi�n inicial del sintetizador,
  /// adaptando la informaci�n del difonema que se pasa como par�metro a un objeto de
  /// esta clase.

  int adapta_difonema(Vector_difonema *difonema, char izq_der);

  /// An�loga a la de Vector_descriptor_objetivo.

  int escribe_datos_txt(FILE *salida, int frecuencia_muestreo);

  // Escribe la informaci�n del fonema en una sola l�nea

  int escribe_datos_txt_simplificado(FILE *salida);

  /// Escribe los valores de la energ�a al principio y al final del semifonema
  /// que se le pasa como par�metro( Se emplea para escribir en un fichero el contorno
  /// de energ�a de las unidades seleccionadas para la s�ntesis).

  void escribe_energia(FILE *fichero, char izq_der);
  int lee_datos_txt(FILE *entrada);

  /// Escribe en formato binario la informaci�n del objeto; Debido a la presencia de variables
  /// din�micas, no se escribe todo el objeto con un �nico fwrite, sino que se van escribiendo
  /// las variables una por una, lo que supone que cada vez que se a�ade una nueva variable
  /// miembro a la clase, sea necesario modificar esta funci�n.
      
  int escribe_datos_bin(FILE *salida);

  /// Escribe en el fichero que se le pasa como par�metro la informaci�n est�tica del objeto.

  int escribe_datos_parte_estatica(FILE *salida);

  /// Escribe en el fichero que se le pasa como par�metro la informaci�n din�mica del objeto.

  int escribe_datos_parte_dinamica(FILE *salida);

  /// Lee del fichero cuyo nombre se le pasa como par�metro, la informaci�n din�mica del objeto.

  void lee_datos_parte_dinamica(char **cadena);

  /// Lee del fichero que se le pasa como par�metro la informaci�n del fonema, en el mismo
  /// formato generado por escribe_datos_bin, lo que supone que tambi�n sea
  /// necesario modificarla cuando se a�adan nuevas variables miembro a la
  /// clase.

  int lee_datos_bin(FILE *entrada);

  /// Seg�n el tipo de par�metro que se le pase, copia en el objeto actual la informaci�n
  /// del objeto de entrada (es necesaria por la presencia de variables din�micas).

  int copia_contenido(Vector_descriptor_candidato vector_original);

  /// Seg�n el tipo de par�metro que se le pase, copia en el objeto actual la informaci�n
  /// del objeto de entrada (es necesaria por la presencia de variables din�micas).

  int copia_contenido(Vector_difonema *vector_original, char izq_der);


  int lee_datos_de_cadena(char **cadena_datos);

  /// Escribe las variables est�ticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables est�ticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_descriptor_objetivo

    Esta clase es la encargada de recoger los datos pros�dicos y
    contextuales deseados para cada fonema. Dada una frase de entrada, se
    descompone en una cadena de objetos de esta clase que almacenan la
    informaci�n pros�dica y contextual de cada uno de sus fonemas.

*/

class Vector_descriptor_objetivo {


public:

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.
  char contexto_derecho[OCTETOS_POR_FONEMA];   ///< Identificador del contexto por la derecha (fonema posterior al actual).
  char contexto_izquierdo[OCTETOS_POR_FONEMA]; ///< Identificador del contexto por la izquierda (fonema anterior al actual)..

  char *palabra;                               ///< Palabra a la que pertenece.

  char contexto_derecho_2[OCTETOS_POR_FONEMA];   ///< nombre del fonema situado dos unidades a la derecha del actual.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA]; ///< nombre del fonema situado dos unidades a la izquierda del actual.

  unsigned char acento;                        /**< \brief Indicador de si el fonema est� acentuado.

Marca que indica si el fonema pertenece
a una s�laba acentuada l�xicamente o no. Actualmente s�lo se consideran dos
posibles valores (t�nica o �tona), aunque probablemente en un futuro
pr�ximo se a�adan nuevos niveles.  */

  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  /**< Tipo de frase a la que pertenece el fonema, dentro de la clasificaci�n
     enunciativas, interrogativas, exclamativas e inacabadas.
     #define FRASE_ENUNCIATIVA   0
     #define FRASE_EXCLAMATIVA   1
     #define FRASE_INTERROGATIVA 2
     #define FRASE_INACABADA     3 */

  unsigned char posicion;                      ///< Posici�n del semifonema en la frase (o grupo entre pausas).
  char info_silaba;                            ///< 1 si primer fonema de s�laba, 2 �dem en �ltima s�laba de palabra, 0 resto 
  char frontera_inicial;                         ///< = Clasificaci�n seg�n el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Clasificaci�n seg�n el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad est� en posici�n final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posici�n intermedia.

  vector<char> fronteras_prosodicas;           ///< Vector en el que se indican las fronteras pros�dicas de dicha unidad seg�n los diferentes contornos de frecuencia considerados (para el c�lculo en paralelo)

  float duracion_1;                            ///< Duraci�n de la primera parte del semifonema.
  float duracion_2;                            ///< Duraci�n de la segunda parte del semifonema.

  float frecuencia_izquierda;                  ///< Frecuencia en el contexto izquierdo.
  float frecuencia_mitad;                      ///< Frecuencia en la mitad del fonema.
  float frecuencia_derecha;                    ///< Frecuencia en el contexto derecho.

  vector<float> frecuencias_inicial;    ///< Vector para todas las posibles frecuencias objetivo en el contexto izquierdo.
  vector<float> frecuencias_mitad;        ///< Vector para todas las posibles frecuencias objetivo en la mitad del fonema.
  vector<float> frecuencias_final;        ///< Vector para todas las posibles frecuencias objetivo en el contexto derecho.
  
#ifdef _INCLUYE_POTENCIA
  float potencia;                              ///< Potencia en la zona estacionaria de la unidad.
  //      float potencia_inicial;                      // Potencia al inicio de la unidad.
  //      float potencia_final;                        // Potencia al final de la unidad.
#endif

//HTS


  char silaba_hts[LONX_MAX_PAL_SIL_E_ACENTUADA]; //para HTS
  t_frase_separada * palsep;


  Vector_descriptor_objetivo();

  Vector_descriptor_objetivo(const Vector_descriptor_objetivo &origen);
  
  ~Vector_descriptor_objetivo() {
    if (palabra) {
      free(palabra);
      palabra = NULL;
    }
  }

  /// Versi�n sobrecargada del operador de igualaci�n

  Vector_descriptor_objetivo &operator= (const Vector_descriptor_objetivo &origen);

  /// Inicializa las variables de tipo cadena o puntero para evitar en alg�n acceso a
  /// ellas si est�n sin inicializar; tiene la misma que el constructor, pero permite que
  /// podamos emplear la clase con realloc(), que no llama constructor.

  void inicializa();

  /// Carga en las variables miembro toda la informaci�n del fonema no relativa a la prosodia.

  int introduce_datos(char *semifon, char *contexto_der, char *contexto_izdo,
		      char *contexto_der2, char *contexto_izdo2, unsigned char ac,
		      char *pal, unsigned char fr, unsigned char posicion_frase,
		      char primera_letra_palabra, char ultima_letra_palabra, char silaba);

  /// Almacena en las variables
  /// correspondientes la informaci�n relacionada con la evoluci�n de la
  /// frecuencia fundamental en el fonema, en hertzios.

  void anhade_frecuencia(float media, float inicial, float final);

  ///  Equivalente a anhade_frecuencia, pero
  ///  relativa a la duraci�n de ambas partes del fonema, en segundos;
  ///  Y si fuese m�s larga?

  void anhade_duracion(float dur1, float dur2);

#ifdef _INCLUYE_POTENCIA
  //      void anhade_potencia(float estacionaria, float inicial, float final);

  /// Equivalente a anhade_frecuencia y anhade duracion, pero
  ///referida a la potencia. La recibe como par�metro en unidades naturales, pero internamente se almacena en unidades
  ///logar�tmicas.

  void anhade_potencia(float estacionaria);
#endif

  /// Libera la memoria reservada para las variables miembro din�micas.
  void libera_memoria();

  /// Escribe en un fichero de texto la informaci�n del fonema, en un formato f�cilmente legible por el usuario.

  int escribe_datos_txt(FILE *salida);

  /// Lee de un fichero de texto, con el mismo formato generado por escribe_datos_txt funci�n,
  /// la informaci�n de un fonema.

  int lee_datos_txt(FILE *entrada);

  /// Similar a escribe_datos_txt pero en formato binario.

  int escribe_datos_bin(FILE *salida);

  /// Similar a lee_datos_txt pero en formato binario.

  int lee_datos_bin(FILE *entrada);

  /// Devuelve el valor de las variables semifonema, contexto derecho y contexto izquierdo.

  void devuelve_fonemas(char central[], char izquierdo[], char derecho[]);

  /// Devuelve la palabra a la que pertenece el fonema.

  void devuelve_palabra(char palabra_contexto[]);

  /// Devuelve la duraci�n de la parte izquierda o derecha del fonema, seg�n se indique.

  void devuelve_duracion(char izq_der, float *duracion_unidad);

  /// Devuelve los valores de la frecuencia fundamental al inicio, la mitad y al final del fonema.

  void devuelve_frecuencias(float *inicial, float *mitad, float *final);

  /// Devuelve los valores de duraci�n de ambas partes del fonema.

  void devuelve_duraciones(float *dur1, float *dur2);

  /// Devuelve el tipo de frase a la que pertenece el fonema,
  /// su posici�n dentro de �sta y el tipo de acento que tiene la s�laba a la que la unidad
  /// pertenece.
      
  void devuelve_frase_posicion_acento(unsigned char *fr, unsigned char *pos,
				      unsigned char *ac);

  /// Crea un vector objetivo a partir de un candidato.
  int crea_vector_objetivo(Vector_descriptor_candidato *vector_candidato);

  /// Crea un vector_objetivo a partir de un semifonema candidato.
  int crea_vector_objetivo(Vector_semifonema_candidato *vector_candidato, unsigned char izquierdo_derecho);

};


/** \class Vector_semifonema_candidato
 * La clase Vector semifonema candidato se emplea para almacenar la
 * informaci�n de las unidades susceptibles de ser seleccionadas en el
 * momento de la s�ntesis de voz. Al contrario que las dos clases
 * anteriores, en este caso los datos almacenados se refieren al
 * semifonema, la unidad b�sica para la concatenaci�n en la versi�n
 * actual del conversor.
 */

class Vector_semifonema_candidato {

  friend class Vector_difonema;

public:
    
  static int numero_coeficientes_cepstrales;		/// N�mero de coeficientes cepstrales.


  int identificador;                             ///< Identificador num�rico del semifonema; Dado que
  ///< en la versi�n actual no se reorganizan los identificadores
  ///< de las unidades, y por tanto se mantiene el secuenciamiento
  ///< en la base de datos original, no resulta necesario tener otra
  ///< variable diferente como en el caso de la clase anterior.

  char semifonema[OCTETOS_POR_FONEMA];           ///< Nombre del al�fono central; nombre del fonema del que fue
  ///< extra�do el semifonema.

  char contexto_izquierdo[OCTETOS_POR_FONEMA];   ///< Nombre del al�fono por la izquierda.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA]; ///< Nombre del al�fono anterior al de la izquierda.

  char contexto_derecho[OCTETOS_POR_FONEMA];     ///< Nombre del al�fono por la derecha.
  char contexto_derecho_2[OCTETOS_POR_FONEMA];   ///< Nombre del al�fono posterior al de la derecha.

  unsigned char izquierdo_derecho;             ///< Indicador de si se trata de un semifonema por la derecha o por
  ///< la izquierda; indica si el semifonema proviene de la
  ///< parte izquierda o derecha del fonema.
  unsigned char acento;                        ///< Indicador de si el fonema est� acentuado.
  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  unsigned char posicion;                      ///< Posici�n del semifonema en la frase (o grupo entre pausas).
  char frontera_inicial;                         ///< = Clasificaci�n seg�n el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Classificaci�n seg�n el tipo de frontera al final del fonema (ver el inicio de este fichero)

  unsigned char validez;                                ///< A diferencia de su significado en otras clases, aqu� se emplea para
  ///< marcar la necesidad de realizar alg�n tipo de modificaci�n pros�dica
  ///< en el semifonema en el caso de que este se escogiese para la
  ///< s�ntesis en un contexto pros�dico determinado.

  char tipo_espectral;                         // Clase del semifonema seg�n el enumerado
  // clase_espectral, definido en
  // distancia_espectral.hpp.
  char tipo_duracion;                          // Clasificaci�n del semifonema seg�n el tipo
  // enumerado clase_duracion definido en
  // modelo_duracion.hpp
  char marca_inventada;						 // Campo en el que se almacena si el semifonema
  // tiene alguna marca de pitch inventada.
  char sonoridad;								 // Campo que indica si el semifonema es sordo o
  // sonoro.
  char frontera_prosodica;						 ///< Campo que indica si la unidad est� en posici�n final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posici�n intermedia.

  short int numero_marcas;                     ///< N�mero de marcas de pitch del semifonema.

  int anterior_pitch;						   // Valor del pitch entre la primera marca
  // del semifonema y la �ltima del anterior.
  int ultimo_pitch;						   // Valor del pitch entre la ultima marca
  // del semifonema y la primera del siguiente.

  float duracion;                              ///< Duraci�n del semifonema.
  float frecuencia_inicial;                    ///< Estimaci�n de la frecuencia fundamental al inicio del semifonema.
  float frecuencia_final;                      ///< Estimaci�n de la frecuencia fundamental al final del semifonema.

  float potencia_inicial;                      ///< Potencia al inicio del semifonema.
  float potencia_final;                        ///< Potencia al final del semifonema.

  int indice_fichero;                          ///< �ndice al inicio del fragmento de voz en el
  ///< fichero �nico de sonido.
  int indice;                                  ///< �ndice al inicio del semifonema en el fichero de sonido
  ///< del que fue extra�do.
  int indice_primera_marca;                    ///< �ndice a la posici�n de la primera marca de
  ///< pitch del semifonema en muestras

#ifndef _x86_64
  int dummy_1;
#endif

  char *palabra;                               ///< Palabra de la que fue extra�do el semifonema, tal y
  ///< como fue pronunciada por el locutor originalmente, para tener en
  ///< cuenta su contexto fon�tico real.
#ifndef _x86_64
  int dummy_2;
#endif

  int *marcas_pitch;                      ///< Vector de marcas de pitch.

#ifndef _x86_64
  int dummy_3;
#endif

  float *cepstrum_inicial; ///< Vector de coeficientes mel-cepstrum centrado en
  ///< en la primera marca del fonema al que pertenece.
#ifndef _x86_64
  int dummy_3b;
#endif

  float *cepstrum_mitad; ///< Vector de coeficientes mel-cepstrum centrado en
  ///< en la marca de la mitad del fonema al que pertenece
#ifndef _x86_64
  int dummy_4;
#endif

  float *cepstrum_final;  ///< Vector de coeficientes mel-cepstrum centrado en
  ///< la �ltima marca de pitch del fonema al que pertenece.

#ifndef _x86_64
  int dummy_9; // padding del objeto completo.
#endif

  Vector_semifonema_candidato();

  /// Extrae del objeto Vector descriptor candidato los datos del semifonema que se le
  ///indica como par�metro.
    
  int crea_semifonema_candidato(Vector_descriptor_candidato *candidato, char izq_der);

  float calcula_duracion_efectiva(float frec_inicial, float frec_final,
				  int frecuencia_muestreo);
  /// En sus diferentes versiones, calcula la duraci�n efectiva del semifonema, en funci�n de los
  /// segmentos de voz que hay alrededor de las marcas de pitch.

  void calcula_duracion_efectiva(int frecuencia_muestreo);

  int copia_contenido(Vector_semifonema_candidato vector_original);
  int copia_contenido(Vector_difonema *vector_original, char izq_der);
  int adapta_difonema(Vector_difonema *difonema, char izq_der);
  void escribe_energia(FILE *fichero, char izq_der);
  int escribe_datos_txt(FILE *salida, const Vector_descriptor_objetivo &objetivo, int excluye_mp_y_cepstrum = 0);
  int escribe_datos_txt(FILE *salida, int exclude_mp_y_cepstrum = 1);

  int escribe_datos_clustering(FILE *salida);
  int escribe_datos_bin(FILE *salida);
  int escribe_datos_parte_dinamica(FILE *salida);
  void lee_datos_parte_dinamica(char **cadena);
  int lee_datos_bin(FILE *entrada);
  void libera_memoria();
  void libera_memoria_menos_marcas_pitch();
  /// Escribe las variables est�ticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables est�ticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_sintesis
 * Esta clase contiene los par�metros de las unidades seleccionadas
 * necesarios para la s�ntesis, adem�s de permitir una abstracci�n entre
 * las diferentes clases empleadas en la fase de selecci�n de unidades y
 * el procesado final de la voz.
 */

class Vector_sintesis {

public:

  int identificador;                           ///< Identificador num�rico de la unidad de la
  ///< que proviene. No es estrictamente necesario para la s�ntesis, pero
  ///< simplifica notablemente las labores de depuraci�n.
  ///< pero ayuda para depurar.

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.

  int anterior_pitch;						   // pitch entre la primera marca del semifonema
  // y la �ltima del anterior.
  int ultimo_pitch;							   // pitch entre la �ltima marca del semifonea
  // y la primera del siguiente.

  float frecuencia_inicial;             ///< Frecuencia al inicio del semifonema
  float frecuencia_final;              ///< Frecuencia al final del semifonema

  float duracion;                              ///< Duraci�n del semifonema.
  float potencia_inicial;				   		///< Potencia del semifonema en la parte inicial.
  float potencia_final;							///< Potencia del semifonema en la parte final.
  short int numero_marcas_analisis;            ///< N�mero de marcas de pitch del semifonema del que proviene.
  short int numero_marcas_sintesis;            ///< N�mero de marcas de pitch tras la etapa de modificaci�n pros�dica.

#ifndef _x86_64
  int dummy_1;
#endif
  int *marcas_analisis;                   ///< Marcas de pitch del semifonema del que proviene.
  int pitch_sintesis;					   	             ///< Pitch de s�ntesis para aquellas unidades con una �nica marca.	
  int indice;                                  ///< �ndice a la primera muestra del semifonema en el fichero de sonido.
  ///< (contexto por la izquierda).
#ifndef _x86_64
  int dummy_2;
#endif
  int *correspondientes;                  ///< cadena con las correspondencias entre las marcas
  ///< de an�lisis y las de s�ntesis.
#ifndef _x86_64
  int dummy_3;
#endif
  int *marcas_sintesis;                   ///< cadena con las marcas de s�ntesis.

  char izquierdo_derecho;                      ///< Indica el tipo de unidad que almacena.
  unsigned char validez;                                ///< Indica si es preciso realizar una manipulaci�n
  ///< de la prosodia de la unidad contenida.
  char sonoridad;				   			   ///< Indica si la unidad es sorda o sonora.
  char tipo_espectral;                         ///< Tipo espectral de la unidad.


  Vector_sintesis();

  float calcula_duracion_efectiva(int frecuencia_muestreo);

  float calcula_duracion_efectiva(float frec_inicial, float frec_final, int frecuencia_muestreo);

  /// Asigna al identificador el valor que se le indica.

  void anhade_identificador(int ident);

  /// Asigna el tipo de modificaci�n pros�dica que hay que realizar sobre la unidad
  /// si se escoge para la s�ntesis en ese contexto.

  void anhade_validez(unsigned char modificable);

  /// Apunta las variables internas a los vectores de marcas de s�ntesis y
  /// correspondientes que se le pasan como par�metro, adem�s de almacenar sus tama�os.

  void incluye_marcas_sintesis_y_correspondientes(int *marcas, int *marcas_corr,
						  short int num_marcas);

  /// Devuelve el valor de la duraci�n del semifonema.

  float devuelve_duracion();

  /// Devuelve el identificador del semifonema.

  int devuelve_identificador();

  /// Devuelve un puntero al vector de marcas de pitch de la unidad original, as� como su n�mero.

  void devuelve_marcas_analisis(int **marc_analisis, short int *numero);

  /// Devuelve punteros a los vectores de marcas de an�lisis, correspondientes y de s�ntesis, as� como sus
  /// respectivos tama�os.

  void devuelve_marcas(int **marc_analisis, int **marc_corr, int **marc_sintesis,
		       short int *num_analisis, short int *num_sintesis);

  /// Devuelve el �ndice dentro del fichero de sonido a la primera muestra de la unidad.

  void devuelve_indice(int *indice);

  /// Devuelve el par�metro de modificaci�n pros�dica de la unidad.

  unsigned char devuelve_validez();

  /// Crea un objeto de esta clase, adaptando los valores del par�metro de entrada
  /// seg�n el tipo de �ste, permitiendo de esta forma separar la informaci�n necesaria
  /// para la s�ntesis de c�mo se encuentre �sta organizada en la fase de selecci�n.

  int crea_vector_sintesis(Vector_descriptor_candidato *original, char izq_der, unsigned char val,
			   int frecuencia_muestreo);
  int crea_vector_sintesis(Vector_difonema *original, char izq_der, unsigned char val,
			   int frecuencia_muestreo);
  int crea_vector_sintesis(Vector_semifonema_candidato *original, char izq_der, unsigned char val);

  /// An�loga a la de clases similares.

  void libera_memoria();

  /// Indica si el semifonema proviene de la parte izquierda o derecha del fonema.

  char izquierdo_o_derecho();

  /// An�loga a la de clases similares.

  void escribe_datos_txt(FILE *fichero);


};

#endif
