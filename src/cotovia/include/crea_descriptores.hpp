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
 un conjunto de fonemas consecutivos y su duraci�n.
 */

typedef struct {

    char unidad[30];  ///< Nombre de la unidad; Puede tratarse de un fonema, una s�laba,...

    float duracion;   ///< Duraci�n de la unidad anterior, tal y como la pronunci� el locutor.

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

  Estructura en la que se almacena el rango de palabras t�nicas que se desea
  encontrar en un grupo entre pausas (#ifdef _CORPUS_PROSODICO).
*/

typedef struct {

   int minimo;     ///< M�nimo n�mero de palabras t�nicas que puede
                   ///< tener el grupo entre pausas para ser considerado como v�lido.

   int maximo;     ///< M�ximo n�mero de palabras t�nicas que puede
                   ///< tener el grupo entre pausas para ser considerado como v�lido.

} Estructura_umbrales;


/** \struct  Estructura_prosodica

  Estructura empleada en el modo _CORPUS_PROSODICO para definir el n�mero y tipo de frases que
  se desean extraer de un fichero para crear un corpus pros�dico.
*/


typedef struct {

   int ocurrencias;    ///< N�mero de ocurrencias del tipo de frase definido en
                       ///< los otros campos de la estructura que se desean encontrar.

   int numero_grupos;  ///< N�mero de grupos entre pausas de los que debe constar
                       ///< el tipo de frase buscado.

   unsigned char tipo_frase;  ///< Tipo de frase, dentro de la clasificaci�n considerada
                              ///< en el m�dulo descriptor, es decir: FRASE_ENUNCIATIVA,
                              ///< FRASE_EXCLAMATIVA, FRASE_INTERROGATIVA y FRASE_INACABADA.

   Estructura_umbrales *numero_tonicas;  ///< Cadena de estructuras de tipo Estructura_umbrales
                                         ///< que almacenan el rango de s�labas t�nicas que debe
                                         ///< haber en cada grupo entre pausas del tipo de frase deseado.
} Estructura_prosodica;

#endif

/// Extrae el nombre del fichero en el que se encuentra la unidad, eliminando
/// la ruta, para que �sta no tenga que ser fija.

void crea_nombre_fichero_sonido(char nombre_salida[]);

/// detecta si una palabra consta �nicamente de signos de puntuaci�n. Es necesaria dado que la
/// fase de preprocesado del sintetizador no considera todos los separadores posibles
/// formados por signos de puntuaci�n.

int palabra_solo_signos_puntuacion(char *palabra);

/// Comprueba si la frase de entrada consta �nicamente de signos de puntuaci�n.

int frase_solo_signos_de_puntuacion(t_frase_separada *frase);

int cuenta_numero_palabras(t_frase_separada *frase);

/// Seg�n el par�metro de entrada, devuelve el tipo de frase al que pertenece el fonema.

unsigned char halla_tipo_de_frase(unsigned char codigo);

short int cuenta_fonemas_en_silaba(char silaba[]);

/// Devuelve la posici�n de la vocal fuerte dentro de la s�laba.

int encuentra_vocal_fuerte(char *silaba);

/// Devuelve el tipo de la siguiente pausa, dentro de la clasificaci�n PAUSA_PUNTO,
/// PAUSA_COMA o PAUSA_PUNTOS_SUSPENSIVOS.

char encuentra_tipo_siguiente_pausa(t_frase_separada *recorre_frase_separada);

/// Dado que cada elemento de la variable global frase_separada se puede desdoblar en m�s de
/// una palabra, esta funci�n se encarga de extraer la palabra adecuada
/// a la que pertenece el fonema actual.

int inicia_ciclo_transformada(char *palabra_transformada, char *salida, int numero_palabra);

/// Devuelve el car�cter fuerte o d�bil del fonema de entrada.

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

	short int grupo_acentual_actual; ///< �ndice del grupo acentual actual dentro
                                         ///< del grupo entre pausas al que pertenece.

	short int numero_elementos_grupo_acentual; ///< N�mero de elementos significativos de los
                                                   ///< fonemas de la variable global.

	short int numero_silabas_grupo_acentual; ///< N�mero de s�labas del grupo acentual actual.

	short int numero_grupos_acentuales; ///< N�mero de grupos acentuales del grupo entre pausas actual.

	char posicion_acento;  ///< Posici�n del acento en el grupo acentual actual.
                               ///< Se consideran tres opciones: GRUPO_AGUDO, GRUPO_LLANO
                               ///< y GRUPO_ESDRUJULO.

	short int grupos_fonicos_total; ///< N�mero de grupos de palabras entre pausas que hay en la frase considerada.
        
	short int grupo_fonico_actual; ///< N�mero de fragmento entre pausas en el que nos encontramos.

	short int grupo_prosodico_actual; ///< �ndice del grupo entonativo en el que nos encontramos.

	short int grupos_prosodicos_total; ///< N�mero de grupos entonativos que hay en la frase considerada.

#endif

#ifdef _PARA_ELISA

	frase_silabas frases_elisa[NUMERO_FRASES_ELISA];  ///< Cadena en la que se almacenan las frases
                                                          ///< de las que se desea calcular la duraci�n
                                                          ///< sil�bica.

#endif

#if defined(_SCRIPT_DURACION) || defined(_MODO_NORMAL) || defined(_SCRIPT) || \
	defined(_CALCULA_DATOS_MODELO_ENERGIA) || defined(_PARA_ELISA)

	short int posicion_en_silaba; ///< Posici�n del al�fono en la s�laba a la que pertenece.
	short int silabas_totales;    ///< N�mero de s�labas del grupo entre pausas.
	short int tonicas_totales;    ///< N�mero de s�labas t�nicas del grupo entre pausas.
	short int tonica_actual;      ///< N�mero de t�nicas desde el inicio hasta la s�laba actual.
	short int numero_fonemas_silaba;  ///< N�mero de fonemas en la s�laba actual.

	short int numero_silabas_palabra; ///< Tama�o de la palabra, en s�labas.
//#endif
  short int indice_silaba_palabra; ///< �ndice de la s�laba en la palabra.
#endif

#ifdef _GRUPOS_ACENTUALES
	short int silabas_totales;    ///< N�mero de s�labas del grupo entre pausas.
	short int tonicas_totales;    ///< N�mero de s�labas t�nicas del grupo entre pausas.
#endif

#ifdef _GRUPOS_ACENTUALES
    estructura_fonemas *cadena_fonemas;	 ///< Cadena con la segmentaci�n fon�tica.
    int numero_fonemas;			 ///< n�mero de elementos...
#endif

#ifdef _CALCULA_DATOS_MODELO_ENERGIA

    estructura_fonemas *cadena_fonemas;  ///< Cadena con la segmentaci�n fon�tica.

    int numero_fonemas;			 ///< N�mero de elementos...

    short int *muestras_tiempo;	         ///< Cadena con las muestras de la forma de onda.
    int numero_muestras;		 ///< N�mero de elementos...

    marca_struct *original_marcas;					// cadena con las marcas de pitch.
    int numero_marcas;								// n�mero de elementos...

    estructura_energia *cadena_energia;	 ///< Cadena con la informaci�n de la energ�a
                                         ///< y el nombre de cada fonema.

#endif

#ifdef _SCRIPT_DURACION

    estructura_fonemas *cadena_fonemas;   ///< Cadena con la segmentaci�n fon�tica.
    int numero_fonemas;			  ///< N�mero de elementos...

#endif

#ifdef _PARA_ELISA

    estructura_fonemas *cadena_fonemas_fichero;	///< Cadena con la segmentaci�n fon�tica.
    int numero_fonemas;			        ///< N�mero de elementos...

#endif

#ifdef _SCRIPT

	Vector_cepstrum_tiempo *cadena_vectores_cepstrum;	// cadena con los vectores cepstrum.
    int numero_vectores_cepstrum;						// n�mero de elementos de la cadena anterior.

    short int *muestras_tiempo;						// cadena con las muestras de la forma de onda.
    int numero_muestras;                            // n�mero de elementos de la cadena anterior.

    estructura_fonemas *cadena_fonemas;				// cadena con la segmentaci�n fon�tica.
    int numero_fonemas;								// n�mero de elementos...

    marca_struct *original_marcas;					// cadena con las marcas de pitch.
    int numero_marcas;								// n�mero de elementos...

    float *cadena_cepstrum;				// cadena con los coeficientes cepstrales calculados
    									// en ventanas centradas en marcas de pitch
    int numero_cepstrum;				// n�mero de elementos...

#endif

#ifndef _GRUPOS_ACENTUALES

	char tonicidad; ///< Variable en la que se almacena si una s�laba es t�nica (1) o �tona (0).
	unsigned char tamano_silaba; ///< Tama�o de la s�laba a la que pertenece el fonema que se considera.

//#ifndef _CORPUS_PROSODICO
	short int tamano_palabra; ///< Tama�o, en fonemas, de la palabra actual.

	short int cuenta_silabas; ///< N�mero de s�labas del grupo entre pausas.

	char *frase; ///< Secuencia de caracteres que forman el grupo entr pausas,
                     ///< extra�da de la variable global frase_fonetica.
#endif

	int primer_acento; ///< Posici�n de la primera s�laba t�nica en el grupo de palabras entre pausas.
	int segundo_acento; ///< Posici�n de la �ltima s�laba t�nica en el grupo de palabras entre pausas.
	int fragmentos_entre_pausas; ///< N�mero de grupos de palabras entre pausas que hay en la frase considerada.
	int fragmento_actual; ///< �ndice del grupo entre pausas actua dentro de la frase.
	int numero_elementos; ///< N�mero de caracteres significativos (todos menos ' ', '^' y '-' en frase_fonetica.

public:
        /// Cuando se emplea en el modo _ENTRENA_CORPUS, se encarga de calcular las caracter�sticas
        /// de los fonemas que se encuentran en el Corpus, a partir de diferentes ficheros en los que
        /// se encuentra la informaci�n de los par�metros cepstrales, la forma de onda, la duraci�n
        /// y las marcas de pitch; Por el contrario, en el modo _MODO_NORMAL se encarga de calcular
        /// las caracter�sticas de los fonemas de la frase de entrada, con el fin de crear la secuencia
        /// de objetos Vector_descriptor_objetivo que se establecer�n como objetivo en la fase de
        /// selecci�n de unidades.
        
	Crea_vectores_descriptor();
    ~Crea_vectores_descriptor();

#ifdef _GENERA_CORPUS

/// Realiza las mismas funciones que calcula_estadistica_2_0, pero adem�s comprueba que en la
/// frase entrada haya alguna de las unidades que son requeridas en la variable de entrada
/// lista_unidades; En el caso de que s� que las haya, a�ade al fichero fichero_corpus la frase
/// y actualiza el n�mero de ocurrencias de todas las unidades. En caso contrario, a�ade la frase
/// al fichero fichero_rechazo y no modifica las estad�sticas.

int genera_corpus(char *frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
    char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
		  t_frase_sintagmada *frase_sintagmada, int numero_de_elementos_de_frase_separada,
    Estadistica *semifonema_izquierdo_est,
    Estadistica *semifonema_derecho_est, Estadistica *trifonema_est, Estadistica *lista_unidades,
    FILE *fichero_corpus, FILE *fichero_rechazo);


#endif

#ifdef _GRUPOS_ACENTUALES

/// Extrae las caracter�sticas de los grupos acentuales de la frase del Corpus considerada.

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

/// Extrae las porciones de frase entre pausas, dejando s�lo la informaci�n relativa a los grupos
/// acentuales.

int extrae_grupo_acentual(char *frase_total, unsigned char *alofono_acento, char *tipo_pausa);

/// Calcula el n�mero de grupos pros�dicos existentes en la frase que se le pasa como
/// par�metro.

void calcula_numero_grupos_prosodicos(char *frase_total);

void calcula_numero_grupos_acentuales(char *grupo_prosodico);

/// reinicia las variables del m�dulo empleadas para el c�lculo de las caracter�sticas de los
///  grupos acentuales de cada frase; Se llama al inicio de cada frase.

void reinicia_grupos_acentuales();

/// Avisa al usuario del final de un grupo acentual, y calcula los par�metros caracter�sticos del
/// siguiente.

int fin_grupo_acentual(char *frase, char *alofono, char *nuevo_grupo, char *alofono_acento,
                       char *tipo_de_pausa);

/// Calcula la posici�n de la s�laba sobre la que recae el acento.

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

/// Calcula las estad�sticas de frecuencia de aparici�n de los fonemas, semifonemas y trifonemas de la frase
/// de entrada; No incluye la informaci�n de contexto, car�cter t�nico y tipo de proposici�n.

int calcula_estadistica_1_0(t_prosodia *frase_prosodica, Estadistica *fonema_est,
         Estadistica *semifonema_izquierdo_est, Estadistica *semifonema_derecho_est,
         Estadistica *trifonema_est);

/// Calcula las estad�sticas de frecuencia de aparici�n de los semifonemas y trifonemas de la frase
/// de entrada, incluyendo la informaci�n de car�cter t�nico, posici�n en la frase y
/// tipo de proposici�n.

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

/// Determina si una frase debe pertenecer o no al corpus pros�dico, en funci�n de su n�mero
/// de grupos entre pausas y su distribuci�n de s�labas t�nicas en ellos.

int genera_corpus_prosodico(char *frase_extraida, char *frase_fonetica,
	t_frase_separada *frase_separada,
    Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
    FILE *fichero_corpus, FILE *fichero_rechazo);

/// Carga en memoria el contenido del fichero en el que se especifican las estructuras pros�dicas
/// que se desea que formen parte del corpus pros�dico.

int lee_corpus_prosodico(Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
                         FILE *fichero);

/// Una vez que ya se ha completado el n�mero de ocurrencias deseadas de un tipo de frase en
/// el corpus pros�dico, se emplea para eliminar de la lista de estructuras pros�dicas
/// buscadas el nodo correspondiente.

int elimina_estructura_prosodia(Estructura_prosodica **inicial, int posicion, int *tamano);

/// Escribe en el fichero que se le pasa como par�metro aquellos tipos de frases que no se han
/// encontrado.

void escribe_corpus_prosodico(Estructura_prosodica *inicial, int numero_estructuras, FILE *fichero);

void libera_corpus_prosodico(Estructura_prosodica *corpus_prosodico, int numero_estructuras);

#endif

/// Cuenta el n�mero de grupos entre pausas de una frase, as� como el n�mero de palabras t�nicas de cada
/// uno de ellos.

int cuenta_grupos_y_tonicas(char *frase_fonetica, int *numero_grupos, short int **tonicas);

/// Extrae las caracter�sticas de frase posici�n en la frase y car�cter t�nico decada al�fono.

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

/// Extrae el grupo entre pausas actual y adem�s calcula el n�mero de s�labas totales
/// y t�nicas del grupo.

char *extrae_frase_y_posiciones(char *frase_total);

///  Inicializa el valor de las restantes variables del m�dulo relacionadas con el
///  c�lculo de la posici�n y el car�cter t�nico; a diferencia de la
///  anterior, se emplea al inicio de cada grupo entre pausas.

void reinicia_variables_posicion_y_tonicidad();

///  Se emplea al inicio de cada frase, para inicializar los valores de las variables
///  del m�dulo fragmentos_entre_pausas, fragmento_actual, numero_elementos y frase.

void inicializa_calculo_posicion_y_tonicidad();

///  Inicia los valores relacionados con la posici�n sil�bica dentro del grupo
///  entre pausas.

void reinicia_variables_posicion_silabica();

/// Comprueba si la diferencia entre la transcripci�n del sintetizador y la del fichero
/// de fonemas se debe simplemente a un problema de timbre, o bien es consecuencia
/// de una p�rdida de sincronismo, lo cual supondr�a un error grave.

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

/// Utiliza la informaci�n del fichero de fonemas para generar una cadena de tipo
/// estructura_duracion en la que se almacenan las s�labas de la frase de entrada y
/// sus respectivas duraciones.

int calcula_duraciones_silabicas(t_prosodia *frase_prosodica, char *frase_fonetica,
                                 FILE *fichero_fonemas, FILE *fichero_silabas,
                                 estructura_duracion *cadena_silabas, int *numero_silabas,
                                 estructura_duracion *cadena_fonemas, int *numero_fonemas,
                                 char nombre_fichero_sfs[], char nombre_fichero_fonemas[],
                                 int correccion_manual);

/// Carga en la variable del m�dulo frases_elisa las frases contenidas en el fichero que
/// se le pasa como par�metro.

void carga_fichero_frases_elisa(FILE *fichero);

/// Obtiene el n�mero de frase de un fichero a partir del nombre, que tiene que seguir
/// el formato indicado en el c�digo.

int consigue_numero_frase(char *nombre_fichero);

/// Extrae la frase que se le indica como par�metro de la variable del m�dulo frases_elisa.

void extraer_frase(int numero_frase, char *frase_extraida);

/// Crea el nombre del fichero de silabas a partir del nombre del fichero de fonemas original.

int crea_nombre_fichero_silabas(char *fichero_original, char *nombre_salida);

/// Crea el nombre del fichero de duraciones a partir del nombre del fichero de fonemas original.

int crea_nombre_fichero_duraciones(char *fichero_original, char *nombre_salida);

/// Calcula el n�mero de caracteres significativos de la siguiente silaba de la frase de entrada.

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


