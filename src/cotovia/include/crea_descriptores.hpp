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
 

#ifndef _CREA_DESCRIPTORES

#define _CREA_DESCRIPTORES

#include <stdio.h>

#include "modos.hpp"
#include "energia.hpp"
#include "tip_var.hpp"
#include "modelo_duracion.hpp"
#include "descriptor.hpp"
#include "minor_phrasing.hpp"

#if defined(_ESTADISTICA) || defined (_GENERA_CORPUS)
#include "estadistica.hpp"
#endif

#if defined(_GRUPOS_ACENTUALES) || defined(_MODO_NORMAL)

#include "grupos_acentuales.hpp"
#include "frecuencia.hpp"

#endif

#ifdef _PARA_ELISA

#define NUMERO_FRASES_ELISA 812
#define TAMANO_MAXIMO_FRASE 1000

/**
  Cadena de caracteres en la que se almacena una frase.
*/

typedef char frase_silabas[TAMANO_MAXIMO_FRASE];

/** \struct estructura_duracion

 Estructura en la que se almacena
 un conjunto de fonemas consecutivos y su duración.
 */

typedef struct {

    char unidad[30];  ///< Nombre de la unidad; Puede tratarse de un fonema, una sílaba,...

    float duracion;   ///< Duración de la unidad anterior, tal y como la pronunció el locutor.

} estructura_duracion;

#endif

#ifdef _SECUENCIAS_FONETICAS

typedef struct {
	clase_espectral fonema;
//    int numero_ocurrencias;
} Estructura_fonema_estacionario;

typedef struct {
	clase_espectral primer_fonema;
    clase_espectral segundo_fonema;
//    int numero_ocurrencias;
} Estructura_fonema_transicion;

#endif

#ifdef _CORPUS_PROSODICO


/** \struct Estructura_umbrales

  Estructura en la que se almacena el rango de palabras tónicas que se desea
  encontrar en un grupo entre pausas (#ifdef _CORPUS_PROSODICO).
*/

typedef struct {

   int minimo;     ///< Mínimo número de palabras tónicas que puede
                   ///< tener el grupo entre pausas para ser considerado como válido.

   int maximo;     ///< Máximo número de palabras tónicas que puede
                   ///< tener el grupo entre pausas para ser considerado como válido.

} Estructura_umbrales;


/** \struct  Estructura_prosodica

  Estructura empleada en el modo _CORPUS_PROSODICO para definir el número y tipo de frases que
  se desean extraer de un fichero para crear un corpus prosódico.
*/


typedef struct {

   int ocurrencias;    ///< Número de ocurrencias del tipo de frase definido en
                       ///< los otros campos de la estructura que se desean encontrar.

   int numero_grupos;  ///< Número de grupos entre pausas de los que debe constar
                       ///< el tipo de frase buscado.

   unsigned char tipo_frase;  ///< Tipo de frase, dentro de la clasificación considerada
                              ///< en el módulo descriptor, es decir: FRASE_ENUNCIATIVA,
                              ///< FRASE_EXCLAMATIVA, FRASE_INTERROGATIVA y FRASE_INACABADA.

   Estructura_umbrales *numero_tonicas;  ///< Cadena de estructuras de tipo Estructura_umbrales
                                         ///< que almacenan el rango de sílabas tónicas que debe
                                         ///< haber en cada grupo entre pausas del tipo de frase deseado.
} Estructura_prosodica;

#endif

/// Extrae el nombre del fichero en el que se encuentra la unidad, eliminando
/// la ruta, para que ésta no tenga que ser fija.

void crea_nombre_fichero_sonido(char nombre_salida[]);

/// detecta si una palabra consta únicamente de signos de puntuación. Es necesaria dado que la
/// fase de preprocesado del sintetizador no considera todos los separadores posibles
/// formados por signos de puntuación.

int palabra_solo_signos_puntuacion(char *palabra);

/// Comprueba si la frase de entrada consta únicamente de signos de puntuación.

int frase_solo_signos_de_puntuacion(t_frase_separada *frase);

int cuenta_numero_palabras(t_frase_separada *frase);

/// Según el parámetro de entrada, devuelve el tipo de frase al que pertenece el fonema.

unsigned char halla_tipo_de_frase(unsigned char codigo);

short int cuenta_fonemas_en_silaba(char silaba[]);

/// Devuelve la posición de la vocal fuerte dentro de la sílaba.

int encuentra_vocal_fuerte(char *silaba);

/// Devuelve el tipo de la siguiente pausa, dentro de la clasificación PAUSA_PUNTO,
/// PAUSA_COMA o PAUSA_PUNTOS_SUSPENSIVOS.

char encuentra_tipo_siguiente_pausa(t_frase_separada *recorre_frase_separada);

/// Dado que cada elemento de la variable global frase_separada se puede desdoblar en más de
/// una palabra, esta función se encarga de extraer la palabra adecuada
/// a la que pertenece el fonema actual.

int inicia_ciclo_transformada(char *palabra_transformada, char *salida, int numero_palabra);

/// Devuelve el carácter fuerte o débil del fonema de entrada.

inline int tipo_vocal(char vocal);

int devuelve_caracteristicas_parte_silaba(char *parte_silaba, char *caracteristica);

int devuelve_onset(char *silaba, char *onset);

int devuelve_coda(char *silaba, char *coda);

int devuelve_tipo_vocal_fuerte(char *silaba, clase_espectral *tipo_vocal);

char decide_categoria_morfosintactica_grupo_acentual(t_frase_separada *frase_separada);

char decide_sintagma_grupo_acentual(unsigned char tipo_sintagma);

void devuelve_siguiente_sintagma_no_puntuacion(t_frase_sintagmada *sintagma_actual, int indice_palabra_actual,
											   unsigned char *tipo_sintagma);

int cuenta_elementos_tonicos_sintagma(t_frase_sintagmada *frase_sintagmada, t_frase_separada *frase_separada_inicio,
									  t_frase_separada *frase_separada);


#ifdef _WIN32
class __declspec(dllexport) Crea_vectores_descriptor {
#else
class Crea_vectores_descriptor {
#endif

#if defined(_GRUPOS_ACENTUALES) || defined(_MODO_NORMAL)

	short int grupo_acentual_actual; ///< Índice del grupo acentual actual dentro
                                         ///< del grupo entre pausas al que pertenece.

	short int numero_elementos_grupo_acentual; ///< Número de elementos significativos de los
                                                   ///< fonemas de la variable global.

	short int numero_silabas_grupo_acentual; ///< Número de sílabas del grupo acentual actual.

	short int numero_grupos_acentuales; ///< Número de grupos acentuales del grupo entre pausas actual.

	char posicion_acento;  ///< Posición del acento en el grupo acentual actual.
                               ///< Se consideran tres opciones: GRUPO_AGUDO, GRUPO_LLANO
                               ///< y GRUPO_ESDRUJULO.

	short int grupos_fonicos_total; ///< Número de grupos de palabras entre pausas que hay en la frase considerada.
        
	short int grupo_fonico_actual; ///< Número de fragmento entre pausas en el que nos encontramos.

	short int grupo_prosodico_actual; ///< Índice del grupo entonativo en el que nos encontramos.

	short int grupos_prosodicos_total; ///< Número de grupos entonativos que hay en la frase considerada.

#endif

#ifdef _PARA_ELISA

	frase_silabas frases_elisa[NUMERO_FRASES_ELISA];  ///< Cadena en la que se almacenan las frases
                                                          ///< de las que se desea calcular la duración
                                                          ///< silábica.

#endif

#if defined(_SCRIPT_DURACION) || defined(_MODO_NORMAL) || defined(_SCRIPT) || \
	defined(_CALCULA_DATOS_MODELO_ENERGIA) || defined(_PARA_ELISA)

	short int posicion_en_silaba; ///< Posición del alófono en la sílaba a la que pertenece.
	short int silabas_totales;    ///< Número de sílabas del grupo entre pausas.
	short int tonicas_totales;    ///< Número de sílabas tónicas del grupo entre pausas.
	short int tonica_actual;      ///< Número de tónicas desde el inicio hasta la sílaba actual.
	short int numero_fonemas_silaba;  ///< Número de fonemas en la sílaba actual.

	short int numero_silabas_palabra; ///< Tamaño de la palabra, en sílabas.
//#endif
  short int indice_silaba_palabra; ///< Índice de la sílaba en la palabra.
#endif

#ifdef _GRUPOS_ACENTUALES
	short int silabas_totales;    ///< Número de sílabas del grupo entre pausas.
	short int tonicas_totales;    ///< Número de sílabas tónicas del grupo entre pausas.
#endif

#ifdef _GRUPOS_ACENTUALES
    estructura_fonemas *cadena_fonemas;	 ///< Cadena con la segmentación fonética.
    int numero_fonemas;			 ///< número de elementos...
#endif

#ifdef _CALCULA_DATOS_MODELO_ENERGIA

    estructura_fonemas *cadena_fonemas;  ///< Cadena con la segmentación fonética.

    int numero_fonemas;			 ///< Número de elementos...

    short int *muestras_tiempo;	         ///< Cadena con las muestras de la forma de onda.
    int numero_muestras;		 ///< Número de elementos...

    marca_struct *original_marcas;					// cadena con las marcas de pitch.
    int numero_marcas;								// número de elementos...

    estructura_energia *cadena_energia;	 ///< Cadena con la información de la energía
                                         ///< y el nombre de cada fonema.

#endif

#ifdef _SCRIPT_DURACION

    estructura_fonemas *cadena_fonemas;   ///< Cadena con la segmentación fonética.
    int numero_fonemas;			  ///< Número de elementos...

#endif

#ifdef _PARA_ELISA

    estructura_fonemas *cadena_fonemas_fichero;	///< Cadena con la segmentación fonética.
    int numero_fonemas;			        ///< Número de elementos...

#endif

#ifdef _SCRIPT

	Vector_cepstrum_tiempo *cadena_vectores_cepstrum;	// cadena con los vectores cepstrum.
    int numero_vectores_cepstrum;						// número de elementos de la cadena anterior.

    short int *muestras_tiempo;						// cadena con las muestras de la forma de onda.
    int numero_muestras;                            // número de elementos de la cadena anterior.

    estructura_fonemas *cadena_fonemas;				// cadena con la segmentación fonética.
    int numero_fonemas;								// número de elementos...

    marca_struct *original_marcas;					// cadena con las marcas de pitch.
    int numero_marcas;								// número de elementos...

    float *cadena_cepstrum;				// cadena con los coeficientes cepstrales calculados
    									// en ventanas centradas en marcas de pitch
    int numero_cepstrum;				// número de elementos...

#endif

#ifndef _GRUPOS_ACENTUALES

	char tonicidad; ///< Variable en la que se almacena si una sílaba es tónica (1) o átona (0).
	unsigned char tamano_silaba; ///< Tamaño de la sílaba a la que pertenece el fonema que se considera.

//#ifndef _CORPUS_PROSODICO
	short int tamano_palabra; ///< Tamaño, en fonemas, de la palabra actual.

	short int cuenta_silabas; ///< Número de sílabas del grupo entre pausas.

	char *frase; ///< Secuencia de caracteres que forman el grupo entr pausas,
                     ///< extraída de la variable global frase_fonetica.
#endif

	int primer_acento; ///< Posición de la primera sílaba tónica en el grupo de palabras entre pausas.
	int segundo_acento; ///< Posición de la última sílaba tónica en el grupo de palabras entre pausas.
	int fragmentos_entre_pausas; ///< Número de grupos de palabras entre pausas que hay en la frase considerada.
	int fragmento_actual; ///< Índice del grupo entre pausas actua dentro de la frase.
	int numero_elementos; ///< Número de caracteres significativos (todos menos ' ', '^' y '-' en frase_fonetica.

public:
        /// Cuando se emplea en el modo _ENTRENA_CORPUS, se encarga de calcular las características
        /// de los fonemas que se encuentran en el Corpus, a partir de diferentes ficheros en los que
        /// se encuentra la información de los parámetros cepstrales, la forma de onda, la duración
        /// y las marcas de pitch; Por el contrario, en el modo _MODO_NORMAL se encarga de calcular
        /// las características de los fonemas de la frase de entrada, con el fin de crear la secuencia
        /// de objetos Vector_descriptor_objetivo que se establecerán como objetivo en la fase de
        /// selección de unidades.
        
	Crea_vectores_descriptor();
    ~Crea_vectores_descriptor();

#ifdef _GENERA_CORPUS

/// Realiza las mismas funciones que calcula_estadistica_2_0, pero además comprueba que en la
/// frase entrada haya alguna de las unidades que son requeridas en la variable de entrada
/// lista_unidades; En el caso de que sí que las haya, añade al fichero fichero_corpus la frase
/// y actualiza el número de ocurrencias de todas las unidades. En caso contrario, añade la frase
/// al fichero fichero_rechazo y no modifica las estadísticas.

int genera_corpus(char *frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
    char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
		  t_frase_sintagmada *frase_sintagmada, int numero_de_elementos_de_frase_separada,
    Estadistica *semifonema_izquierdo_est,
    Estadistica *semifonema_derecho_est, Estadistica *trifonema_est, Estadistica *lista_unidades,
    FILE *fichero_corpus, FILE *fichero_rechazo);


#endif

#ifdef _GRUPOS_ACENTUALES

/// Extrae las características de los grupos acentuales de la frase del Corpus considerada.

#ifdef _MINOR_PHRASING_TEMPLATE

int calcula_grupos_acentuales(char *frase_extraida,t_prosodia *frase_prosodica,
			      char *frase_fonetica, 
                              t_frase_separada *frase_separada,
                              int numero_elementos_frase_separada,
                              t_frase_sintagmada *frase_sintagmada,
                              Grupo_acentual_candidato **acentuales, 
			      int *numero_acentuales,
                              Frase_frontera &frase_frontera,
                              Frecuencia_estilizada *frecuencia_estilizada,
                              FILE *fichero_fonemas,
                              FILE *fichero_silabas,
                              char *nombre_fichero,
                              int correccion_manual);
#else

int calcula_grupos_acentuales(char *frase_extraida,t_prosodia *frase_prosodica,
							  char *frase_fonetica,
                              t_frase_separada *frase_separada,
                              int numero_elementos_frase_separada,
                              t_frase_sintagmada *frase_sintagmada,
                              Grupo_acentual_candidato **acentuales, int *numero_acentuales,
                              Frecuencia_estilizada *frecuencia_estilizada,
                              FILE *fichero_fonemas,
                              FILE *fichero_silabas,
                              char *nombre_fichero,
                              int correccion_manual);
#endif

#endif

#if defined(_GRUPOS_ACENTUALES) || defined(_MODO_NORMAL) || defined(_SCRIPT)

/// Extrae las porciones de frase entre pausas, dejando sólo la información relativa a los grupos
/// acentuales.

int extrae_grupo_acentual(char *frase_total, unsigned char *alofono_acento, char *tipo_pausa);

/// Calcula el número de grupos prosódicos existentes en la frase que se le pasa como
/// parámetro.

void calcula_numero_grupos_prosodicos(char *frase_total);

void calcula_numero_grupos_acentuales(char *grupo_prosodico);

/// reinicia las variables del módulo empleadas para el cálculo de las características de los
///  grupos acentuales de cada frase; Se llama al inicio de cada frase.

void reinicia_grupos_acentuales();

/// Avisa al usuario del final de un grupo acentual, y calcula los parámetros característicos del
/// siguiente.

int fin_grupo_acentual(char *frase, char *alofono, char *nuevo_grupo, char *alofono_acento,
                       char *tipo_de_pausa);

/// Calcula la posición de la sílaba sobre la que recae el acento.

char calcula_posicion_acento(char *grupo_acentual, unsigned char *alofono_acento);

/// Extrae el nombre del fichero en el que se encuentra la unidad y crea el nombre del fichero en el
/// que se van a almacenar los grupos acentuales.

void crea_nombre_fichero_grupo(char *nombre_entrada, char *nombre_salida);



#endif


#ifdef _MODO_NORMAL

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

int crea_vectores_descriptor(char *frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, float esc_temp, float esc_pitch,char difo, int numero_de_elementos_de_frase_separada, struct Modelo_energia * modelo_energia, struct Modelo_duracion * modelo_duracion, vector<Vector_descriptor_objetivo> &vectores, int *numero_unidades, 
#ifdef _MINOR_PHRASING_TEMPLATE
			     Frase_frontera *frase_frontera,
#endif
			     Grupo_acentual_objetivo **acentuales, int *numero_acentuales);

#else

  int crea_vectores_descriptor(char * frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, float esc_temp, float esc_pitch, char difo, int numero_de_elementos_de_frase_separada, Red_neuronal *red_energia, Media_varianza_energia *medias_varianzas_energia, struct Modelo_duracion * modelo_duracion, vector<Vector_descriptor_objetivo> &vectores, int *numero_unidades,
#ifdef _MINOR_PHRASING_TEMPLATE
			       Frase_frontera *frase_frontera,
#endif
			       Grupo_acentual_objetivo **acentuales,
			       int *numero_acentuales);

#endif

#endif

#ifdef _SCRIPT

int crea_vectores_descriptor(t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, char * locutores, t_frase_sintagmada *frase_sintagmada, char *fichero_origen, char *nombre_fonemas, char *nombre_txt, char *nombre_txt_morf, char *nombre_fichero_sfs, FILE *fichero_fonemas, FILE *fichero_marcas, FILE *fichero_sonido, FILE *fichero_cepstrum, FILE *fichero_cepstrum_bin, int frecuencia_muestreo, float frecuencia_minima_locutor, float frecuencia_maxima_locutor, int numero_de_elementos_de_frase_separada, FILE *fichero_bin, int correccion_manual, char *nombre_fichero_origen_configuracion);

#endif

#ifdef _ESTADISTICA

/// Calcula las estadísticas de frecuencia de aparición de los fonemas, semifonemas y trifonemas de la frase
/// de entrada; No incluye la información de contexto, carácter tónico y tipo de proposición.

int calcula_estadistica_1_0(t_prosodia *frase_prosodica, Estadistica *fonema_est,
         Estadistica *semifonema_izquierdo_est, Estadistica *semifonema_derecho_est,
         Estadistica *trifonema_est);

/// Calcula las estadísticas de frecuencia de aparición de los semifonemas y trifonemas de la frase
/// de entrada, incluyendo la información de carácter tónico, posición en la frase y
/// tipo de proposición.

int calcula_estadistica_2_0(t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
    char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
    t_frase_sintagmada *frase_sintagmada, int numero_de_elementos_de_frase_separada,
    Estadistica *semifonema_izquierdo_est,
    Estadistica *semifonema_derecho_est, Estadistica *trifonema_est);

#endif

#ifdef _SECUENCIAS_FONETICAS

int lee_estructuras_foneticas(Estructura_fonema_estacionario **fonemas_estacionario,
                                                        int *numero_fonemas_estacionario,
                                                        Estructura_fonema_transicion **fonemas_transicion,
                                                        int *numero_fonemas_transicion,
                                                        int *minimo_numero_palabras,
                                                        int *maximo_numero_palabras,
                                                        int *porcentaje_minimo_ocurrencias, FILE *fichero);

void libera_estructuras_foneticas(Estructura_fonema_estacionario *fonema_estacionario,
															Estructura_fonema_transicion *fonema_transicion);

int seleccion_frases_secuencias_foneticas(char *frase_extraida, t_prosodia *frase_prosodica,
	t_frase_separada *frase_separada,
    Estructura_fonema_estacionario *fonema_estacionario, int numero_fonemas_estacionario,
    Estructura_fonema_transicion *fonema_transicion, int numero_fonemas_transicion,
    int minimo_numero_palabras, int maximo_numero_palabras,
    float porcentaje_fonemas_buscados,
    FILE *fichero_corpus, FILE *fichero_rechazo);


#endif

#ifdef _CORPUS_PROSODICO

/// Determina si una frase debe pertenecer o no al corpus prosódico, en función de su número
/// de grupos entre pausas y su distribución de sílabas tónicas en ellos.

int genera_corpus_prosodico(char *frase_extraida, char *frase_fonetica,
	t_frase_separada *frase_separada,
    Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
    FILE *fichero_corpus, FILE *fichero_rechazo);

/// Carga en memoria el contenido del fichero en el que se especifican las estructuras prosódicas
/// que se desea que formen parte del corpus prosódico.

int lee_corpus_prosodico(Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
                         FILE *fichero);

/// Una vez que ya se ha completado el número de ocurrencias deseadas de un tipo de frase en
/// el corpus prosódico, se emplea para eliminar de la lista de estructuras prosódicas
/// buscadas el nodo correspondiente.

int elimina_estructura_prosodia(Estructura_prosodica **inicial, int posicion, int *tamano);

/// Escribe en el fichero que se le pasa como parámetro aquellos tipos de frases que no se han
/// encontrado.

void escribe_corpus_prosodico(Estructura_prosodica *inicial, int numero_estructuras, FILE *fichero);

void libera_corpus_prosodico(Estructura_prosodica *corpus_prosodico, int numero_estructuras);

#endif

/// Cuenta el número de grupos entre pausas de una frase, así como el número de palabras tónicas de cada
/// uno de ellos.

int cuenta_grupos_y_tonicas(char *frase_fonetica, int *numero_grupos, short int **tonicas);

/// Extrae las características de frase posición en la frase y carácter tónico decada alófono.

int calcula_posicion_frase_y_tonicidad(char *f_fonetica, char alof[],
                                       unsigned char *posicion, unsigned char *tonica);

int calcula_posicion_frase_tonicidad_silabas(char *f_fonetica, char alof[],
                                             unsigned char *posicion, unsigned char *tonica,
                                             short int *fonemas_en_silaba,
                                             short int *fonemas_en_palabra,
                                             short int *silabas_en_palabra,
                                             short int *posicion_silaba, short int *posicion_inicio,
                                             short int *posicion_final, short int *tonicas_inicio,
                                             short int *tonicas_final,
                                             char *inicio_silaba,
                                             char *fin_silaba,
                                             char *silaba_actual, 
					     char *frontera_inicial,
                                             char *frontera_final,
                                             char *final_frase,
					     unsigned char *indice_silaba);
/// Extrae el grupo entre pausas actual.

char *extrae_frase(char *frase_total);

/// Extrae el grupo entre pausas actual y además calcula el número de sílabas totales
/// y tónicas del grupo.

char *extrae_frase_y_posiciones(char *frase_total);

///  Inicializa el valor de las restantes variables del módulo relacionadas con el
///  cálculo de la posición y el carácter tónico; a diferencia de la
///  anterior, se emplea al inicio de cada grupo entre pausas.

void reinicia_variables_posicion_y_tonicidad();

///  Se emplea al inicio de cada frase, para inicializar los valores de las variables
///  del módulo fragmentos_entre_pausas, fragmento_actual, numero_elementos y frase.

void inicializa_calculo_posicion_y_tonicidad();

///  Inicia los valores relacionados con la posición silábica dentro del grupo
///  entre pausas.

void reinicia_variables_posicion_silabica();

/// Comprueba si la diferencia entre la transcripción del sintetizador y la del fichero
/// de fonemas se debe simplemente a un problema de timbre, o bien es consecuencia
/// de una pérdida de sincronismo, lo cual supondría un error grave.

int comprueba_error_etiquetado(char *alof_cotovia, char *alof_real, int correccion_manual);

///  Crea el nombre del fichero "sfs" para generar el script de ficheros con errores de
///  etiquetado; toma la ruta y el inicio del nombre de la cadena de
///  entrada.

void construye_nombre_sfs(char **cadena);

int devuelve_tipo_estructura_silaba(char *silaba, int posicion_vocal_fuerte, char *tipo_silaba);

#ifdef _CALCULA_DATOS_MODELO_ENERGIA

  int calcula_datos_modelo_energia(t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, char *fichero_origen, char *nombre_fichero_sfs, FILE *fichero_fonemas, FILE *fichero_marcas, FILE *fichero_sonido, int frecuencia_muestreo, int numero_de_elementos_de_frase_separada, Parametros_modelo_energia **cadena_datos_energia, int *numero_fonemas_energia, int correccion_manual);
#endif

#ifdef _PARA_ELISA

/// Utiliza la información del fichero de fonemas para generar una cadena de tipo
/// estructura_duracion en la que se almacenan las sílabas de la frase de entrada y
/// sus respectivas duraciones.

int calcula_duraciones_silabicas(t_prosodia *frase_prosodica, char *frase_fonetica,
                                 FILE *fichero_fonemas, FILE *fichero_silabas,
                                 estructura_duracion *cadena_silabas, int *numero_silabas,
                                 estructura_duracion *cadena_fonemas, int *numero_fonemas,
                                 char nombre_fichero_sfs[], char nombre_fichero_fonemas[],
                                 int correccion_manual);

/// Carga en la variable del módulo frases_elisa las frases contenidas en el fichero que
/// se le pasa como parámetro.

void carga_fichero_frases_elisa(FILE *fichero);

/// Obtiene el número de frase de un fichero a partir del nombre, que tiene que seguir
/// el formato indicado en el código.

int consigue_numero_frase(char *nombre_fichero);

/// Extrae la frase que se le indica como parámetro de la variable del módulo frases_elisa.

void extraer_frase(int numero_frase, char *frase_extraida);

/// Crea el nombre del fichero de silabas a partir del nombre del fichero de fonemas original.

int crea_nombre_fichero_silabas(char *fichero_original, char *nombre_salida);

/// Crea el nombre del fichero de duraciones a partir del nombre del fichero de fonemas original.

int crea_nombre_fichero_duraciones(char *fichero_original, char *nombre_salida);

/// Calcula el número de caracteres significativos de la siguiente silaba de la frase de entrada.

void extrae_tamano_siguiente_silaba(char **frase, int *tamano_silaba);

#endif

#ifdef _SCRIPT_DURACION

int calcula_parametros_duracion(t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
    char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
    t_frase_sintagmada *frase_sintagmada, char *fichero_origen,
    FILE *fichero_fonemas, int numero_de_elementos_de_frase_separada,
    Parametros_duracion_entrenamiento **cadena_duracion, int *numero_elementos_duracion,
    int correccion_manual);

#endif

};

#endif


