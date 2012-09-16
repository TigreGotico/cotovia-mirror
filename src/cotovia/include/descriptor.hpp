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
 

/** \file descriptor.hpp
 * En el módulo descriptor.hpp se encuentran definidas las clases
 * dedicadas al almacenamiento de la información prosódica y contextual
 * relativa a los segmentos de voz, es decir, Vector descriptor objetivo,
 * Vector semifonema candidato, Vector descriptor candidato, Vector
 * difonema  y Vector sintesis. Cada una de ellas se emplea en una fase
 * diferente del proceso de síntesis de voz. Así, por ejemplo, la clase
 * Vector descriptor candidato se utiliza en la etapa de extracción
 * inicial de los datos de cada segmento de voz del Corpus grabado,
 * mientras que la clase Vector semifonema candidato la información de
 * las unidades de voz candidatas en el momento de la selección de la más
 * apropiada para la síntesis.
 */

#ifndef _VECTOR_DESCRIPTOR

#define _VECTOR_DESCRIPTOR

#include <stdio.h>
#include <stdlib.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "fonemas.hpp"


#define PRIMER_IDENTIFICADOR_DIFONEMAS 30000000 //Se define aquí si no existe difonemas.hpp

// Tamaño de la cabecera de los ficheros binarios donde se almacenan los semifonemas
// candidatos. Se emplea para las variables estáticas de la clase.
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

// Defines para designar la tonicidad de la sílaba a la que pertenece el fonema.

#define TONICA 1
#define ATONA  0

#define GRADOS_ACENTO 2

// Defines para la posición del fonema dentro de su grupo

#define POSICION_INICIAL 0  // Desde el inicio hasta la primera sílaba tónica (incluida).
#define POSICION_MEDIA   1  // Entre la primera sílaba tónica y la última.
#define POSICION_FINAL   2  // Después de la última sílaba tónica (incluida).

#define NUMERO_POSICIONES_FRASE 3

#define UNIDAD_NO_FRONTERA	0	// Unidades intermedias o previas a una pausa que no es punto.
#define UNIDAD_FIN_FRASE	1	// Unidades que quedan entre la última sílaba tónica y un punto.
#define UNIDAD_RUPTURA	2 // Para unidades en la vecindad de una ruptura entonativa o ruptura coma.

#define NUMERO_COEFICIENTES_CEPSTRALES 25 // Éste es el valor por defecto, el real se lee de la base de difonemas.
// Tiene que ser el más grande de los posibles, puesto que los proyectos
// crea_fichero_centroides y organiza_clases espectrales así lo necesitan
// (es una cuestión de velocidad al ejecutarlos)

#define FRECUENCIA_MUESTREO_POR_DEFECTO	16000	// Valor que se emplea para inicializar la variable estática de la clase
// Vector_descriptor_objetivo.

/// Predeclaración de la clase para permitir referencias cruzadas:
class Vector_semifonema_candidato;

/** \class Vector_difonema
 * La clase Vector difonema fue creada para permitir compatibilidad hacia
 * atrás con la anterior versión del conversor, la cual empleaba como
 * unidad básica para la concatenación el difonema. Esta clase adapta la
 * información de la antigua base de datos para que sea posible trabajar
 * con sus segmentos de voz tratándolos como semifonemas.
 */

class Vector_difonema {

  friend class Vector_descriptor_candidato;
  friend class Vector_semifonema_candidato;
  friend class Vector_sintesis;

public:

  static int numero_coeficientes_cepstrales;

  int identificador;                           ///< Análogo al de otras clases amigas; Para
  ///< distinguir si la unidad proviene de un difonema o de un semifonema,
  ///< se empiezan a enumerar a partir del valor
  ///< PRIMER_IDENTIFICADOR_DIFONEMAS definido en el fichero
  ///< difonemas.hpp.

  char fonema_izquierdo[OCTETOS_POR_FONEMA];   ///< Fonema inicial del difonema.
  char fonema_derecho[OCTETOS_POR_FONEMA];     ///< Fonema final del difonema.

  char posicion;                               ///< Indica la posición que ocupa en el logátomo del
  ///< que procede.

  unsigned char validez;                                ///< Indica si es necesario realizar algún procesado
  ///< para modificar la prosodia de la unidad.

  char frontera_inicial_izquierda;                         ///< = Clasificación según el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final_izquierda;                            ///< = Clasificación según el tipo de frontera al final del fonema (ver el inicio de este fichero)
  char frontera_inicial_derecha;                         ///< = Clasificación según el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final_derecha;                            ///< = Clasificación según el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad está en posición final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posición intermedia.

  float duracion_izquierda;                    ///< Duración de la parte izquierda del difonema.
  float duracion_derecha;                      ///< Duración de la parte derecha del difonema.

  float frecuencia_izquierda;                  ///< Frecuencia fundamental estimada en la parte inicial del difonema.
  float frecuencia_central;                    ///< Frecuencia fundamental estimada en la mitad del difonema.
  float frecuencia_derecha;                    ///< Frecuencia fundamental estimada en la parte final del difonema.

  float potencia_izquierda;                    ///< Energía estimada en la parte inicial del difonema.
  float potencia_central;                      ///< Energía estimada en la mitad del difonema.
  float potencia_derecha;                      ///< Energía estimada en la parte final del difonema.

  short int numero_marcas_izquierda;           ///< Número de marcas de pitch de la parte izquierda de la unidad.
  short int numero_marcas_derecha;             ///< Número de marcas de pitch de la parte derecha de la unidad.

  int indice;                                  ///< Índice de la primera muestra del segmento de voz del
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

  /// Almacena los valores de la duración de las dos mitades del difonema.
  void anhade_duraciones(float dur1, float dur2);

  /// Almacena los valores de la energía al inicio, la mitad y el final del difonema.
  void anhade_potencia(float estacionaria, float inicial, float final);

  /// Introduce las cadenas de marcas de pitch, así como su número de elementos.
  void anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
			   short int num_derecha, int *marcas_dcha);


  int anhade_vectores_cepstrum(float *cepstrum_medio_izdo, float *cepstrum_medio_dcho,
			       float *cepstrum_medio_pas, float *cepstrum_medio_fut);

  int anhade_vectores_cepstrum_union(float *cepstrum_inicio_izdo,
				     float *cepstrum_final_izdo,
				     float *cepstrum_inicio_dcho,
				     float *cepstrum_final_dcho);

  /// Introduce el valor del índice de la unidad en el fichero de sonido de difonemas.
  void anhade_indice(int indice_fichero);

  /// Almacena la posición del difonema dentro del logátomo.
  void anhade_posicion(char pos);

  /// Devuelve los nombres de los fonemas que forman el difonema.
  void devuelve_fonemas(char fonema_izdo[], char fonema_dcho[]);

  /// Devuelve el parámetro de modificación prosódica del difonema en el
  /// contexto en el que se va a sintetizar.
  unsigned char devuelve_validez();

  /// Devuelve el valor del identificador numérico del difonema.
  int devuelve_identificador();

  /// Análoga a la de las clases similares.
  void libera_memoria();

  /// Análoga a la de las clases similares.
  void escribe_parte_estatica(FILE *fichero);

  /// Análoga a la de las clases similares.
  void escribe_parte_dinamica(FILE *fichero);

  /// Análoga a la de las clases similares.
  void lee_parte_estatica(FILE *fichero);

  /// Análoga a la de las clases similares.
  void lee_parte_dinamica(char **cadena);

  /// Análoga a la de las clases similares.
  void escribe_datos_txt(FILE *fichero, int frecuencia_muestreo);

  /// Escribe las variables estáticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables estáticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_descriptor_candidato
 *
 * La clase Vector descriptor candidato se emplea en la primera fase del
 * proceso de extracción de la información de las frases del Corpus de
 * voz grabado. Al igual que la clase anterior, toma como unidad básica
 * el fonema, dado que de esta forma no se duplican en memoria los datos
 * que comparten ambos semifonemas.
 *
 * Como es natural, esta clase comparte todas las variables de la
 * anterior, con la diferencia de que, en este caso, las frecuencias, energías y
 * duraciones se corresponden con los valores reales para cada fonema,
 * obtenidos a partir de los ficheros de marcas de pitch, formas de onda
 * y límites de fonemas, respectivamente.
 */


class Vector_descriptor_candidato {

  friend class Vector_sintesis;
  friend class Vector_difonema;

public:

  static int numero_coeficientes_cepstrales;
  static int frecuencia_muestreo;

  int identificador;                           ///< Identificador numérico único del fonema, dentro de la base de datos.

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.
  char contexto_derecho[OCTETOS_POR_FONEMA];   ///< Identificador del contexto por la derecha.

  char contexto_izquierdo[OCTETOS_POR_FONEMA]; ///< Identificador del contexto por la izquierda.

  char contexto_derecho_2[OCTETOS_POR_FONEMA];    ///< Fonema siguiente al derecho.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA];  ///< Fonema anterior al izquierdo.

  unsigned char acento;                        ///< Indicador de si el fonema está acentuado.
  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  unsigned char posicion;                      ///< Posición del semifonema en la frase (o grupo entre pausas).
  char frontera_inicial;                         ///< = Clasificación según el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Clasificación según el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad está en posición final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posición intermedia.

  char tipo_espectral;                         ///< Clasificación del semifonema según el tipo
  ///< enumerado clase_espectral definido en
  ///< distancia_espectral.hpp
  char tipo_duracion;                          ///< Clasificación del semifonema según el tipo
  ///< enumerado clase_duracion definido en
  ///< modelo_duracion.hpp

  unsigned char validez_izda;                           ///< Marca de validez del semifonema izquierdo. Se
  ///< emplea en la fase de extracción de la información para señalar
  ///< aquellas unidades que son incorrectas por algún motivo y que, por lo
  ///< tanto, no deberían ser empleadas para la síntesis.

  unsigned char validez_dcha;                           ///< Marca de validez del semifonema derecho.

  short int numero_marcas_derecha;             ///< Número de marcas de pitch de la parte derecha de la unidad.
  short int numero_marcas_izquierda;           ///< Número de marcas de pitch de la parte izquierda de la unidad.

  float duracion_1;                              ///< Duración del semifonema de la parte izquierda
  float duracion_2;                             ///< Duración del semifonema de la parte derecha
  float frecuencia_mitad;                      ///< Frecuencia en la mitad del fonema.
  float frecuencia_derecha;                    ///< Frecuencia en el contexto derecho.
  float frecuencia_izquierda;                  ///< Frecuencia en el contexto izquierdo.

  float potencia;                              ///< Potencia en la zona estacionaria de la unidad.
  float potencia_inicial;                      ///< Potencia al inicio de la unidad.
  float potencia_final;                        ///< Potencia al final de la unidad.

  int indice_fichero;                          ///< Índice al inicio de la forma de onda de la frase a
  ///< a la que pertenece la unidad dentro del fichero único de sonido.

  int indice_inicial;                          ///< Índice a la primera posición de la unidad en el fichero de
  ///< sonido. (contexto por la izquierda). Índice al inicio de la forma
  ///< de onda del fonema en el fichero de voz al que éste pertenece.

  int indice_mitad;                            ///< Índice a la mitad de la unidad (parte estacionaria). Índice al
  ///< inicio del semifonema derecho dentro del fichero de voz al
  ///< que pertenece el fonema.


  int anterior_pitch;   ///< Valor del pitch entre la primera marca
  ///< del fonema y la última del anterior.
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
  ///< cadena, y ésta apunta a la primera marca que le corresponde.
#ifndef _x86_64
  int dummy_3;
#endif
  int *marcas_izquierda;                  ///< Vector de marcas de la parte izquierda de la unidad. Vector de marcas de pitch del
  ///< semifonema izquierdo de la unidad, necesario para aplicar el
  ///< algoritmo de modificación prosódica en la etapa final de procesado de señal. Cada
  ///< marca de pitch consta de dos valores, el primero referido al
  ///< instante temporal de la marca en el fichero, mientras que el segundo
  ///< se refiere al carácter sordo-sonoro de la propia marca. Aunque en la
  ///< versión actual ya no se emplea esta última información, se sigue
  ///< manteniendo por compatibilidad con la versión inicial de difonemas
  ///< del sintetizador.

#ifndef _x86_64
  int dummy_4;
#endif
  float *cepstrum; // [4*NUMERO_COEFICIENTES_CEPSTRALES]; ///< Vectores mel-cepstrum de 12 coeficientes,
  ///< centrados en las marcas de pitch más cercanas
  ///< al inicio, mitad yfinal del fonema, con una ventana
  ///< de 16 milisegundos. Éstos se calculan fuera de tiempo de
  ///< ejecución por medio de la herramienta sig2fv del Edimburgh
  ///< Speech Tools.

#ifndef _x86_64
  int dummy_5;
#endif

#ifndef _x86_64
  int dummy_10; // padding del objeto completo
#endif


  Vector_descriptor_candidato();

  /// Función similar a la de la clase Vector_descriptor_objetivo , en la que se almacenan
  /// todos los datos del fonema no relativos a la prosodia (también se declara la unidad como
  /// inicialmente válida para la síntesis).

  int introduce_datos(int ident, char *semifon, char *contexto_der,
		      char *contexto_izdo, char *contexto_der2, char *contexto_izdo2,
		      unsigned char ac, char *pal, unsigned char fr,
		      unsigned char posicion_frase, char primera_letra_palabra,
		      char ultima_letra_palabra);

  /// Análoga a la de Vector_descriptor_objetivo.

  void anhade_frecuencia(float media, float inicial, float final);

  /// Análoga a la de Vector_descriptor_objetivo. pero
  /// con un sólo dato de duración (aquí se pierde la posibilidad de que
  /// los dos semifonemas de la unidad puedan tener diferente duración,
  /// así que si se quisiese tener esto en cuenta, habría que cambiarlo).

  void anhade_duracion(float dur1, float dur2);

  /// Análoga a la de Vector_descriptor_objetivo.

  void anhade_potencia(float estacionaria, float inicial, float final);

  int anhade_nombre_fichero(char *fichero);

  /// Almacena los índices del fonema, en muestras.
  void anhade_indices(int origen, int inicio, int mitad);

  ///  Guarda los datos relativos a las  marcas de pitch, es decir, los vectores
  ///  y su número de elementos.

  void anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
			   short int num_derecha, int *marcas_dcha);

  /// Permite modificar el valor de las variables de validez del fonema.

  void anhade_validez(unsigned char modificable, char izq_der);

  /// Libera la memoria asignada a las variables dinámicas del objeto.

  void libera_memoria();

  /// Análoga a la de Vector_descriptor_objetivo.

  int devuelve_fonemas(char central[], char izquierdo[], char derecho[]);

  /// Análoga a la de Vector_descriptor_objetivo.

  void devuelve_palabra(char palabra_contexto[]);

  ///Devuelve el valor de la duración estimada del fonema.

  void devuelve_duracion(float *duracion_unidad, char izquierdo_derecho);

  /// Devuelve los valores estimados de las frecuencias al inicio, la mitad y el final del fonema.

  void devuelve_frecuencias(float *inicial, float *mitad, float *final);

  void devuelve_potencias(float *pot_inicial, float *pot_central, float *pot_final);

  /// Devuelve el tipo de frase a la que pertenece el fonema.

  unsigned char devuelve_tipo_frase();

  /// Devuelve la posición que ocupa el fonema dentro del grupo prosódico al que pertenece.

  unsigned char devuelve_posicion();

  /// Devuelve el tipo de acento de la sílaba a la que pertenece el fonema.

  unsigned char devuelve_acento();

  /// Análoga a la de Vector_descriptor_objetivo

  void devuelve_frase_posicion_acento(unsigned char *fr, unsigned char *pos,
				      unsigned char *ac);

  /// Devuelve el identificador numérico único de la unidad dentro de la base de datos.

  int devuelve_identificador();

  /// Devuelve el contador de posición de la unidad.
  int devuelve_contador();

  /// Devuelve la validez de la parte del fonema que se le pasa como parámetro.

  unsigned char devuelve_validez(char izda_dcha);

  /// Genera el fonema a partir de los semifonemas
  void crea_fonema(Vector_semifonema_candidato *izquierdo, Vector_semifonema_candidato *derecho);

  /// Función que permite aprovechar la base de difonemas de la versión inicial del sintetizador,
  /// adaptando la información del difonema que se pasa como parámetro a un objeto de
  /// esta clase.

  int adapta_difonema(Vector_difonema *difonema, char izq_der);

  /// Análoga a la de Vector_descriptor_objetivo.

  int escribe_datos_txt(FILE *salida, int frecuencia_muestreo);

  // Escribe la información del fonema en una sola línea

  int escribe_datos_txt_simplificado(FILE *salida);

  /// Escribe los valores de la energía al principio y al final del semifonema
  /// que se le pasa como parámetro( Se emplea para escribir en un fichero el contorno
  /// de energía de las unidades seleccionadas para la síntesis).

  void escribe_energia(FILE *fichero, char izq_der);
  int lee_datos_txt(FILE *entrada);

  /// Escribe en formato binario la información del objeto; Debido a la presencia de variables
  /// dinámicas, no se escribe todo el objeto con un único fwrite, sino que se van escribiendo
  /// las variables una por una, lo que supone que cada vez que se añade una nueva variable
  /// miembro a la clase, sea necesario modificar esta función.
      
  int escribe_datos_bin(FILE *salida);

  /// Escribe en el fichero que se le pasa como parámetro la información estática del objeto.

  int escribe_datos_parte_estatica(FILE *salida);

  /// Escribe en el fichero que se le pasa como parámetro la información dinámica del objeto.

  int escribe_datos_parte_dinamica(FILE *salida);

  /// Lee del fichero cuyo nombre se le pasa como parámetro, la información dinámica del objeto.

  void lee_datos_parte_dinamica(char **cadena);

  /// Lee del fichero que se le pasa como parámetro la información del fonema, en el mismo
  /// formato generado por escribe_datos_bin, lo que supone que también sea
  /// necesario modificarla cuando se añadan nuevas variables miembro a la
  /// clase.

  int lee_datos_bin(FILE *entrada);

  /// Según el tipo de parámetro que se le pase, copia en el objeto actual la información
  /// del objeto de entrada (es necesaria por la presencia de variables dinámicas).

  int copia_contenido(Vector_descriptor_candidato vector_original);

  /// Según el tipo de parámetro que se le pase, copia en el objeto actual la información
  /// del objeto de entrada (es necesaria por la presencia de variables dinámicas).

  int copia_contenido(Vector_difonema *vector_original, char izq_der);


  int lee_datos_de_cadena(char **cadena_datos);

  /// Escribe las variables estáticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables estáticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_descriptor_objetivo

    Esta clase es la encargada de recoger los datos prosódicos y
    contextuales deseados para cada fonema. Dada una frase de entrada, se
    descompone en una cadena de objetos de esta clase que almacenan la
    información prosódica y contextual de cada uno de sus fonemas.

*/

class Vector_descriptor_objetivo {


public:

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.
  char contexto_derecho[OCTETOS_POR_FONEMA];   ///< Identificador del contexto por la derecha (fonema posterior al actual).
  char contexto_izquierdo[OCTETOS_POR_FONEMA]; ///< Identificador del contexto por la izquierda (fonema anterior al actual)..

  char *palabra;                               ///< Palabra a la que pertenece.

  char contexto_derecho_2[OCTETOS_POR_FONEMA];   ///< nombre del fonema situado dos unidades a la derecha del actual.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA]; ///< nombre del fonema situado dos unidades a la izquierda del actual.

  unsigned char acento;                        /**< \brief Indicador de si el fonema está acentuado.

Marca que indica si el fonema pertenece
a una sílaba acentuada léxicamente o no. Actualmente sólo se consideran dos
posibles valores (tónica o átona), aunque probablemente en un futuro
próximo se añadan nuevos niveles.  */

  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  /**< Tipo de frase a la que pertenece el fonema, dentro de la clasificación
     enunciativas, interrogativas, exclamativas e inacabadas.
     #define FRASE_ENUNCIATIVA   0
     #define FRASE_EXCLAMATIVA   1
     #define FRASE_INTERROGATIVA 2
     #define FRASE_INACABADA     3 */

  unsigned char posicion;                      ///< Posición del semifonema en la frase (o grupo entre pausas).
  char info_silaba;                            ///< 1 si primer fonema de sílaba, 2 ídem en última sílaba de palabra, 0 resto 
  char frontera_inicial;                         ///< = Clasificación según el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Clasificación según el tipo de frontera al final del fonema (ver el inicio de este fichero)

  char frontera_prosodica;					 ///< Campo que indica si la unidad está en posición final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posición intermedia.

  vector<char> fronteras_prosodicas;           ///< Vector en el que se indican las fronteras prosódicas de dicha unidad según los diferentes contornos de frecuencia considerados (para el cálculo en paralelo)

  float duracion_1;                            ///< Duración de la primera parte del semifonema.
  float duracion_2;                            ///< Duración de la segunda parte del semifonema.

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

  Vector_descriptor_objetivo();

  Vector_descriptor_objetivo(const Vector_descriptor_objetivo &origen);
  
  ~Vector_descriptor_objetivo() {
    if (palabra) {
      free(palabra);
      palabra = NULL;
    }
  }

  /// Versión sobrecargada del operador de igualación

  Vector_descriptor_objetivo &operator= (const Vector_descriptor_objetivo &origen);

  /// Inicializa las variables de tipo cadena o puntero para evitar en algún acceso a
  /// ellas si están sin inicializar; tiene la misma que el constructor, pero permite que
  /// podamos emplear la clase con realloc(), que no llama constructor.

  void inicializa();

  /// Carga en las variables miembro toda la información del fonema no relativa a la prosodia.

  int introduce_datos(char *semifon, char *contexto_der, char *contexto_izdo,
		      char *contexto_der2, char *contexto_izdo2, unsigned char ac,
		      char *pal, unsigned char fr, unsigned char posicion_frase,
		      char primera_letra_palabra, char ultima_letra_palabra, char silaba);

  /// Almacena en las variables
  /// correspondientes la información relacionada con la evolución de la
  /// frecuencia fundamental en el fonema, en hertzios.

  void anhade_frecuencia(float media, float inicial, float final);

  ///  Equivalente a anhade_frecuencia, pero
  ///  relativa a la duración de ambas partes del fonema, en segundos;
  ///  Y si fuese más larga?

  void anhade_duracion(float dur1, float dur2);

#ifdef _INCLUYE_POTENCIA
  //      void anhade_potencia(float estacionaria, float inicial, float final);

  /// Equivalente a anhade_frecuencia y anhade duracion, pero
  ///referida a la potencia. La recibe como parámetro en unidades naturales, pero internamente se almacena en unidades
  ///logarítmicas.

  void anhade_potencia(float estacionaria);
#endif

  /// Libera la memoria reservada para las variables miembro dinámicas.
  void libera_memoria();

  /// Escribe en un fichero de texto la información del fonema, en un formato fácilmente legible por el usuario.

  int escribe_datos_txt(FILE *salida);

  /// Lee de un fichero de texto, con el mismo formato generado por escribe_datos_txt función,
  /// la información de un fonema.

  int lee_datos_txt(FILE *entrada);

  /// Similar a escribe_datos_txt pero en formato binario.

  int escribe_datos_bin(FILE *salida);

  /// Similar a lee_datos_txt pero en formato binario.

  int lee_datos_bin(FILE *entrada);

  /// Devuelve el valor de las variables semifonema, contexto derecho y contexto izquierdo.

  void devuelve_fonemas(char central[], char izquierdo[], char derecho[]);

  /// Devuelve la palabra a la que pertenece el fonema.

  void devuelve_palabra(char palabra_contexto[]);

  /// Devuelve la duración de la parte izquierda o derecha del fonema, según se indique.

  void devuelve_duracion(char izq_der, float *duracion_unidad);

  /// Devuelve los valores de la frecuencia fundamental al inicio, la mitad y al final del fonema.

  void devuelve_frecuencias(float *inicial, float *mitad, float *final);

  /// Devuelve los valores de duración de ambas partes del fonema.

  void devuelve_duraciones(float *dur1, float *dur2);

  /// Devuelve el tipo de frase a la que pertenece el fonema,
  /// su posición dentro de ésta y el tipo de acento que tiene la sílaba a la que la unidad
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
 * información de las unidades susceptibles de ser seleccionadas en el
 * momento de la síntesis de voz. Al contrario que las dos clases
 * anteriores, en este caso los datos almacenados se refieren al
 * semifonema, la unidad básica para la concatenación en la versión
 * actual del conversor.
 */

class Vector_semifonema_candidato {

  friend class Vector_difonema;

public:
    
  static int numero_coeficientes_cepstrales;		/// Número de coeficientes cepstrales.


  int identificador;                             ///< Identificador numérico del semifonema; Dado que
  ///< en la versión actual no se reorganizan los identificadores
  ///< de las unidades, y por tanto se mantiene el secuenciamiento
  ///< en la base de datos original, no resulta necesario tener otra
  ///< variable diferente como en el caso de la clase anterior.

  char semifonema[OCTETOS_POR_FONEMA];           ///< Nombre del alófono central; nombre del fonema del que fue
  ///< extraído el semifonema.

  char contexto_izquierdo[OCTETOS_POR_FONEMA];   ///< Nombre del alófono por la izquierda.
  char contexto_izquierdo_2[OCTETOS_POR_FONEMA]; ///< Nombre del alófono anterior al de la izquierda.

  char contexto_derecho[OCTETOS_POR_FONEMA];     ///< Nombre del alófono por la derecha.
  char contexto_derecho_2[OCTETOS_POR_FONEMA];   ///< Nombre del alófono posterior al de la derecha.

  unsigned char izquierdo_derecho;             ///< Indicador de si se trata de un semifonema por la derecha o por
  ///< la izquierda; indica si el semifonema proviene de la
  ///< parte izquierda o derecha del fonema.
  unsigned char acento;                        ///< Indicador de si el fonema está acentuado.
  unsigned char frase;                         ///< Tipo de frase a la que pertenece.
  unsigned char posicion;                      ///< Posición del semifonema en la frase (o grupo entre pausas).
  char frontera_inicial;                         ///< = Clasificación según el tipo de frontera al principio del fonema (ver el inicio de este fichero)
  char frontera_final;                            ///< = Classificación según el tipo de frontera al final del fonema (ver el inicio de este fichero)

  unsigned char validez;                                ///< A diferencia de su significado en otras clases, aquí se emplea para
  ///< marcar la necesidad de realizar algún tipo de modificación prosódica
  ///< en el semifonema en el caso de que este se escogiese para la
  ///< síntesis en un contexto prosódico determinado.

  char tipo_espectral;                         // Clase del semifonema según el enumerado
  // clase_espectral, definido en
  // distancia_espectral.hpp.
  char tipo_duracion;                          // Clasificación del semifonema según el tipo
  // enumerado clase_duracion definido en
  // modelo_duracion.hpp
  char marca_inventada;						 // Campo en el que se almacena si el semifonema
  // tiene alguna marca de pitch inventada.
  char sonoridad;								 // Campo que indica si el semifonema es sordo o
  // sonoro.
  char frontera_prosodica;						 ///< Campo que indica si la unidad está en posición final de frase,
  ///< en la proximidad de una ruptura entonativa / ruptura coma, o
  ///< en una posición intermedia.

  short int numero_marcas;                     ///< Número de marcas de pitch del semifonema.

  int anterior_pitch;						   // Valor del pitch entre la primera marca
  // del semifonema y la última del anterior.
  int ultimo_pitch;						   // Valor del pitch entre la ultima marca
  // del semifonema y la primera del siguiente.

  float duracion;                              ///< Duración del semifonema.
  float frecuencia_inicial;                    ///< Estimación de la frecuencia fundamental al inicio del semifonema.
  float frecuencia_final;                      ///< Estimación de la frecuencia fundamental al final del semifonema.

  float potencia_inicial;                      ///< Potencia al inicio del semifonema.
  float potencia_final;                        ///< Potencia al final del semifonema.

  int indice_fichero;                          ///< Índice al inicio del fragmento de voz en el
  ///< fichero único de sonido.
  int indice;                                  ///< Índice al inicio del semifonema en el fichero de sonido
  ///< del que fue extraído.
  int indice_primera_marca;                    ///< Índice a la posición de la primera marca de
  ///< pitch del semifonema en muestras

#ifndef _x86_64
  int dummy_1;
#endif

  char *palabra;                               ///< Palabra de la que fue extraído el semifonema, tal y
  ///< como fue pronunciada por el locutor originalmente, para tener en
  ///< cuenta su contexto fonético real.
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
  ///< la última marca de pitch del fonema al que pertenece.

#ifndef _x86_64
  int dummy_9; // padding del objeto completo.
#endif

  Vector_semifonema_candidato();

  /// Extrae del objeto Vector descriptor candidato los datos del semifonema que se le
  ///indica como parámetro.
    
  int crea_semifonema_candidato(Vector_descriptor_candidato *candidato, char izq_der);

  float calcula_duracion_efectiva(float frec_inicial, float frec_final,
				  int frecuencia_muestreo);
  /// En sus diferentes versiones, calcula la duración efectiva del semifonema, en función de los
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
  /// Escribe las variables estáticas de la clase en la cabecera del fichero binario.
  static void escribe_cabecera(FILE *salida);

  /// Lee las variables estáticas de la clase de la cabecera del fichero binario.
  static void lee_cabecera(FILE *entrada);

};


/** \class Vector_sintesis
 * Esta clase contiene los parámetros de las unidades seleccionadas
 * necesarios para la síntesis, además de permitir una abstracción entre
 * las diferentes clases empleadas en la fase de selección de unidades y
 * el procesado final de la voz.
 */

class Vector_sintesis {

public:

  int identificador;                           ///< Identificador numérico de la unidad de la
  ///< que proviene. No es estrictamente necesario para la síntesis, pero
  ///< simplifica notablemente las labores de depuración.
  ///< pero ayuda para depurar.

  char semifonema[OCTETOS_POR_FONEMA];         ///< Identificador del semifonema.

  int anterior_pitch;						   // pitch entre la primera marca del semifonema
  // y la última del anterior.
  int ultimo_pitch;							   // pitch entre la última marca del semifonea
  // y la primera del siguiente.

  float frecuencia_inicial;             ///< Frecuencia al inicio del semifonema
  float frecuencia_final;              ///< Frecuencia al final del semifonema

  float duracion;                              ///< Duración del semifonema.
  float potencia_inicial;				   		///< Potencia del semifonema en la parte inicial.
  float potencia_final;							///< Potencia del semifonema en la parte final.
  short int numero_marcas_analisis;            ///< Número de marcas de pitch del semifonema del que proviene.
  short int numero_marcas_sintesis;            ///< Número de marcas de pitch tras la etapa de modificación prosódica.

#ifndef _x86_64
  int dummy_1;
#endif
  int *marcas_analisis;                   ///< Marcas de pitch del semifonema del que proviene.
  int pitch_sintesis;					   	             ///< Pitch de síntesis para aquellas unidades con una única marca.	
  int indice;                                  ///< Índice a la primera muestra del semifonema en el fichero de sonido.
  ///< (contexto por la izquierda).
#ifndef _x86_64
  int dummy_2;
#endif
  int *correspondientes;                  ///< cadena con las correspondencias entre las marcas
  ///< de análisis y las de síntesis.
#ifndef _x86_64
  int dummy_3;
#endif
  int *marcas_sintesis;                   ///< cadena con las marcas de síntesis.

  char izquierdo_derecho;                      ///< Indica el tipo de unidad que almacena.
  unsigned char validez;                                ///< Indica si es preciso realizar una manipulación
  ///< de la prosodia de la unidad contenida.
  char sonoridad;				   			   ///< Indica si la unidad es sorda o sonora.
  char tipo_espectral;                         ///< Tipo espectral de la unidad.


  Vector_sintesis();

  float calcula_duracion_efectiva(int frecuencia_muestreo);

  float calcula_duracion_efectiva(float frec_inicial, float frec_final, int frecuencia_muestreo);

  /// Asigna al identificador el valor que se le indica.

  void anhade_identificador(int ident);

  /// Asigna el tipo de modificación prosódica que hay que realizar sobre la unidad
  /// si se escoge para la síntesis en ese contexto.

  void anhade_validez(unsigned char modificable);

  /// Apunta las variables internas a los vectores de marcas de síntesis y
  /// correspondientes que se le pasan como parámetro, además de almacenar sus tamaños.

  void incluye_marcas_sintesis_y_correspondientes(int *marcas, int *marcas_corr,
						  short int num_marcas);

  /// Devuelve el valor de la duración del semifonema.

  float devuelve_duracion();

  /// Devuelve el identificador del semifonema.

  int devuelve_identificador();

  /// Devuelve un puntero al vector de marcas de pitch de la unidad original, así como su número.

  void devuelve_marcas_analisis(int **marc_analisis, short int *numero);

  /// Devuelve punteros a los vectores de marcas de análisis, correspondientes y de síntesis, así como sus
  /// respectivos tamaños.

  void devuelve_marcas(int **marc_analisis, int **marc_corr, int **marc_sintesis,
		       short int *num_analisis, short int *num_sintesis);

  /// Devuelve el índice dentro del fichero de sonido a la primera muestra de la unidad.

  void devuelve_indice(int *indice);

  /// Devuelve el parámetro de modificación prosódica de la unidad.

  unsigned char devuelve_validez();

  /// Crea un objeto de esta clase, adaptando los valores del parámetro de entrada
  /// según el tipo de éste, permitiendo de esta forma separar la información necesaria
  /// para la síntesis de cómo se encuentre ésta organizada en la fase de selección.

  int crea_vector_sintesis(Vector_descriptor_candidato *original, char izq_der, unsigned char val,
			   int frecuencia_muestreo);
  int crea_vector_sintesis(Vector_difonema *original, char izq_der, unsigned char val,
			   int frecuencia_muestreo);
  int crea_vector_sintesis(Vector_semifonema_candidato *original, char izq_der, unsigned char val);

  /// Análoga a la de clases similares.

  void libera_memoria();

  /// Indica si el semifonema proviene de la parte izquierda o derecha del fonema.

  char izquierdo_o_derecho();

  /// Análoga a la de clases similares.

  void escribe_datos_txt(FILE *fichero);


};

#endif
