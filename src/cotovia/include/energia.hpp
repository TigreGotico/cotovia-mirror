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
 

#ifndef _ENERGIA_HH

#define _ENERGIA_HH

#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "distancia_espectral.hpp"
#include "frecuencia.hpp"
#include "descriptor.hpp"

#define NUMERO_ENTRADAS_MODELO_ENERGIA	15

#define FICHERO_RN_ENERGIA_SILENCIO "mod_energia_silencio.txt"
#define FICHERO_RN_ENERGIA_VOCAL_ABIERTA "mod_energia_vocabier.txt"
#define FICHERO_RN_ENERGIA_VOCAL_CERRADA "mod_energia_voccerr.txt"
#define FICHERO_RN_ENERGIA_VOCAL_MEDIA "mod_energia_vocmed.txt"
#define FICHERO_RN_ENERGIA_NASAL "mod_energia_nasal.txt"
#define FICHERO_RN_ENERGIA_LATERAL "mod_energia_lateral.txt"
#define FICHERO_RN_ENERGIA_APROXIMANTE "mod_energia_aproximante.txt"
#define FICHERO_RN_ENERGIA_OCLU_SON "mod_energia_ocluson.txt"
#define FICHERO_RN_ENERGIA_OCLU_SOR "mod_energia_oclusor.txt"
#define FICHERO_RN_ENERGIA_VIBRANTE "mod_energia_vibrante.txt"
#define FICHERO_RN_ENERGIA_FRIC_SOR "mod_energia_fricsor.txt"
#define FICHERO_RN_ENERGIA_SEMI_VOCAL "mod_energia_semivocal.txt"

#define FICHERO_RN_MEDIA_VARIANZA_SILENCIO "media_varianza_energia_silencio.txt"
#define FICHERO_RN_MEDIA_VARIANZA_VOCAL_ABIERTA "media_varianza_energia_vocabier.txt"
#define FICHERO_RN_MEDIA_VARIANZA_VOCAL_CERRADA "media_varianza_energia_voccerr.txt"
#define FICHERO_RN_MEDIA_VARIANZA_VOCAL_MEDIA "media_varianza_energia_vocmed.txt"
#define FICHERO_RN_MEDIA_VARIANZA_NASAL "media_varianza_energia_nasal.txt"
#define FICHERO_RN_MEDIA_VARIANZA_LATERAL "media_varianza_energia_lateral.txt"
#define FICHERO_RN_MEDIA_VARIANZA_APROXIMANTE "media_varianza_energia_aproximante.txt"
#define FICHERO_RN_MEDIA_VARIANZA_OCLU_SON "media_varianza_energia_ocluson.txt"
#define FICHERO_RN_MEDIA_VARIANZA_OCLU_SOR "media_varianza_energia_oclusor.txt"
#define FICHERO_RN_MEDIA_VARIANZA_VIBRANTE "media_varianza_energia_vibrante.txt"
#define FICHERO_RN_MEDIA_VARIANZA_FRIC_SOR "media_varianza_energia_fricsor.txt"
#define FICHERO_RN_MEDIA_VARIANZA_SEMI_VOCAL "media_varianza_energia_semivocal.txt"

#define FICHERO_RED_ENERGIA "pesos_red_energia.txt"
#define FICHERO_MEDIA_VARIANZA_ENERGIA "media_varianza_energia.txt"
#define FICHERO_MODELO_ENERGIA "mod_energia.txt"

#define FICHERO_RED_ENERGIA_FREIRE "pesos_datos_energia_fre_perceptron_14_9.txt"
#define FICHERO_RED_ENERGIA_PAULINO "pesos_datos_energia_pau_perceptron_14_9.txt"

#define FICHERO_MEDIA_VARIANZA_ENERGIA_FREIRE "media_varianza_energia_fre.txt"
#define FICHERO_MEDIA_VARIANZA_ENERGIA_PAULINO "media_varianza_energia_pau.txt"

#define FICHERO_MODELO_ENERGIA_FREIRE "mod_energia_freire.txt"
#define FICHERO_MODELO_ENERGIA_PAULINO "mod_energia_paulino.txt"

#define UMBRAL_DIFERENCIA_ENERGIA	10.0F	///< Máxima diferencia de energía entre dos unidades sin que haya
///< modificación.

//#define PERIODO_MUESTREO 0.0000625F // 1/16KHZ
//#define TASA_MUESTREO 16000L

#define TAMANO_VENTANA 0.010F ///< Se corresponde con 10 milisegundos de voz (Antes 30) 
#define MINIMA_ENERGIA_INF 0.000001F ///< Valor que consideramos como mínimo para la energía, para evitar
                                     ///< situaciones en las que se obtenga -INF.

#define ENERGIA_MEDIA_FRASE 49.0F ///< Energía media de una frase típica, para el modelo de estimación
///< de la energía.

#define NUMERO_MAXIMO_CARACTERES_VARIABLE_PREDICCION 16 ///< Número de caracteres más alto de los
///< nombres de las variables de predicción del modelo.

#define NUMERO_PREDICTORES_MODELO_ENERGIA 15 ///< Número de variables de predicción.

#define TAMANO_VARIABLE_DATOS_ENERGIA 100 ///< Tamaño de la variable de datos para la estimación del
///< modelo de estimación de energía basado en regresión lineal.


typedef struct {
  char           chunkWav[4];
  unsigned long  longChunkWav;
  char           dataChunk[4];
  char           chunkFormat[4];
  unsigned long  longChunkFormat;
  unsigned short formatTag;
  unsigned short nChannels;
  unsigned long  nSamplesPerSec;
  unsigned long  nAvgBytesPerSec;
  unsigned short nBlockAlign;
  unsigned short nBitsPerSample;
  char           chunkData[4];
  unsigned long  longChunkData;
} tCabeceraWav;



struct Modelo_energia{
  int numero_variables; ///< Número de variables de predicción en el modelo de regresión.
  char variables[NUMERO_PREDICTORES_MODELO_ENERGIA][NUMERO_MAXIMO_CARACTERES_VARIABLE_PREDICCION]; ///< Nombres de las variables de 
  ///< predicción; Ej: "InicioPalabra".
  int numero_coeficientes; ///< Número de coeficientes del modelo de regresión.
  float *coeficientes;  ///< Coeficientes del modelo.
  int maximo_silabas_inicio;
  int maximo_silabas_final;
  int lee_modelo(char *nombre_fichero); ///< Función que lee todos los parámetros de un modelo desde archivo.
  void libera_memoria();
};

typedef struct {
  float energia;
  float energia_fonema_anterior;
  float energia_media_fichero;
  clase_espectral tipo_fonema;
  clase_espectral tipo_fonema_anterior;
  clase_espectral tipo_fonema_siguiente;
  short int fonemas_palabra;
  short int silabas_palabra;
  int silabas_inicio;
  int silabas_final;
  int silabas_frase;
  char fonema[OCTETOS_POR_FONEMA];
  char frontera_inicial;
  char frontera_final;
  char frontera_prosodica;
  //    char inicio_palabra;
  //  char fin_palabra;
  char final_frase;
  char acento;
  char tipo_proposicion;
  char posicion;
} Parametros_modelo_energia;

typedef Media_varianza Media_varianza_energia[NUMERO_ENTRADAS_MODELO_ENERGIA];

typedef struct {
  float energia;  
  char fonema[OCTETOS_POR_FONEMA + 1]; ///< Nombre del fonema (Se le suma 1 
  ///< por la posible marca de error).
} estructura_energia;

/// Calcula la energía del vector de entrada, enventanándolo con la variable del módulo ventana.
  
int calcula_energia(short int *vector, int tamano, float *salida);

/// Función principal del módulo, encargada de calcular la energía al inicio, 
/// la mitad y el final del fonema; Se mide la energía en unidades naturales.

int calcula_potencias(short int *vector_muestras, float inicio, float fin,
		      marca_struct *cadena_marcas,
		      int frecuencia_muestreo,	
                      Vector_descriptor_candidato *vector);

/// Calcula la energía del fonem a su inicio.

int calcula_potencia_inicial(short int *vector, float *potencia_inicial);

/// Calcula la potencia en la parte central del fonema.

int calcula_potencia_mitad(short int *vector, float *potencia_mitad, float inicio, float fin,
			   int frecuencia_muestreo);
						   
/// Calcula la potencia en la parte final del fonema.

int calcula_potencia_final(short int *vector, float *potencia_final, float inicio, float fin,
			   int frecuencia_muestreo);
            						   
/// Función similar a las anteriores, pero más genérica; Simplemente calcula la 
/// energía del fragmento de voz al que apunta la variable de entrada.

int calcula_potencia(short int *vector, float *potencia);

/// Crea ventana de N muestras.

void hanning(float *window, int N);

/// Crea ventana de N muestras.

void hamming(float *window, int N);

/// Libera la memoria asignada a la ventana de análisis.

void libera_memoria_ventana();

/// Crea una ventana de análisis con el tipo y tamaño especificados; Actualmente 
/// sólo se aceptan las ventanas hanning y las hamming.

int inicia_ventana(const char *tipo, int tamano);

/// Carga en memoria el contenido del fichero de forma de onda, 
/// con formato de datos en bruto.

int carga_vector_muestras(int frecuencia_muestreo, short int **vector,
			  int *tamano,  float *tiempo_final_sonido, FILE *fichero);
						  
/// Comprueba la coherencia entre los datos del fichero de fonemas y la 
/// duración del fichero de forma de onda.

int comprueba_validez_fichero_sonido(int tamano, float tiempo_final_fonemas,
				     int frecuencia_muestreo, float *tiempo_final_sonido);


int calcula_energia_media_y_parte_estacionaria(short int *cadena_muestras, int numero_muestras, estructura_fonemas *cadena_fonemas, int numero_fonemas, marca_struct *cadena_marcas, int numero_marcas, int frecuencia_muestreo, float *energia_media, estructura_energia **cadena_energia);
											   

void escribe_parametros_modelo_energia(FILE *fichero);

int estima_energia(Parametros_modelo_energia *datos, struct Modelo_energia *modelo_temp);

void libera_memoria_modelo_energia();

int estima_energia_red_neuronal(Red_neuronal *red, Media_varianza_energia *estadisticos, Parametros_modelo_energia *datos);

int criterio_modificacion_energia(Vector_descriptor_objetivo &objetivo, int sonido_final, int *modificable);

void escribe_parametros_modelo_energia(FILE *fichero, int opcion_salida);


class Energia {
#ifndef _ESTIMA_ENERGIA_RED_NEURONAL
  Modelo_energia * modelo_energia;// Modelo de estimación de la energía basado en regresión lineal.
#else
  Red_neuronal * red_energia;// Red neuronal para la estimación de la energía.
  Media_varianza * estadisticos_energia;// Medias y desviaciones
#endif

public:
  int escribe_datos_modelo_energia(Parametros_modelo_energia *datos, FILE *fichero,
				   int opcion_salida);

};


#endif

