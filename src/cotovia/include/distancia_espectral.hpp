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
 

#ifndef _DISTANCIA_ESPECTRAL_HH

#define _DISTANCIA_ESPECTRAL_HH

#include "info_corpus_voz.hpp"
#include "descriptor.hpp"
#include "frecuencia.hpp"

#if defined(_SCRIPT) || defined(_CALCULA_DATOS_MODELO_ENERGIA)
#define _CALCULA_DISTANCIAS_CEPSTRALES
#endif

//#define _CALCULA_DISTANCIAS_CEPSTRALES

#define FICHERO_PESOS_CONCATENACION_COART "pesos_continuidad_coart.bin"
#define FICHERO_PESOS_CONCATENACION_EST "pesos_continuidad_est.bin"

#define FICHERO_CENTROIDES "fichero_centroides_var.bin"

#define DESPLAZAMIENTO_VENTANAS_CEPSTRUM  0.008F // Desplazamiento entre las ventanas empleadas
                                                 // para el cálculo de los coeficientes cepstrales.
#ifdef _MODOA_EU //euskera
#define MAXIMO_NUMERO_VECTORES_CEPSTRUM 625 //karolina-eu // Número máximo de vectores de coeficientes cepstrales
#else
#define MAXIMO_NUMERO_VECTORES_CEPSTRUM 225  // Número máximo de vectores de coeficientes cepstrales
                                            // en una unidad. Dado que el espaciado entre ventanas
                                            // es de 0.008 segundos, equivale a 1200 milisegundos
                                            // de unidad (semifonema). Esta restricción se cumple
                                            // para todo el corpus800 de Freire.
#endif

#define TAMANO_CABECERA_HTK_MFCC 12     // Tamaño de la cabecera del fichero de coeficientes cepstrum
                                        // con el formato htk_mfcc

#ifdef _CALCULA_DISTANCIAS_CEPSTRALES

#define MAXIMO_NUMERO_UNIDADES_GRUPO 1400   // Máximo número de ocurrencias de los semifonemas con
                                            // contexto. Se puede extraer de los ficheros
                                            // Estad_derecho_fre.txt y Estad_izquierdo_fre.txt

#define absoluto(a) (((a) >= 0) ? (a) : -(a))

#endif

#ifdef _CONSIDERA_SEMIVOCALES
#define NUMERO_ELEMENTOS_CLASE_ESPECTRAL 12
#else
#define NUMERO_ELEMENTOS_CLASE_ESPECTRAL 11
#endif

#define NUMERO_MAXIMO_PESOS_MODELO_ESPECTRAL 15
#define NUMERO_COEFICIENTES_LSF 12

#define calcula_indice_tabla_centroides(a, b, c) ( (a)*62 + (b)*2 + (c) )
										// El 2 viene de los posibles valores del campo
                                        // izquierdo_derecho. 62 = NUMERO_FONEMAS*2.

typedef enum {SILENCIO_ESP, VOC_AB_ESP, VOC_MED_ESP, VOC_CER_ESP, OCLU_SOR_ESP, OCLU_SON_ESP, APROX_SON_ESP, FRIC_SOR_ESP, NASAL_ESP, VIBRANTE_ESP, LATERAL_ESP, SEMI_VOCAL_ESP} clase_espectral;
              // Dejo pendiente la posible división en las vibrantes y las laterales.

#ifdef _CONSIDERA_SEMIVOCALES
#define is_diptongo(clase_primero, clase_segundo, frontera) ( (frontera == 0) && \
							      ( (clase_primero == VOC_AB_ESP) && (clase_segundo == VOC_CER_ESP) ) || \
							      ( (clase_primero == VOC_CER_ESP) && (clase_segundo == VOC_AB_ESP) ) || \
							      ( (clase_primero == VOC_MED_ESP) && (clase_segundo == VOC_CER_ESP) ) || \
							      ( (clase_primero == VOC_CER_ESP) && (clase_segundo == VOC_MED_ESP) ) || \
							      ( (clase_primero == VOC_CER_ESP) && (clase_segundo == VOC_CER_ESP) ) )
#else
#define is_diptongo(clase_primero, clase_segundo, frontera) ( (frontera == 0) && \
							      ( (clase_primero == VOC_AB_ESP) && (clase_segundo == SEMI_VOCAL_ESP) ) || \
							      ( (clase_primero == SEMI_VOCAL_ESP) && (clase_segundo == VOC_AB_ESP) ) || \
							      ( (clase_primero == VOC_MED_ESP) && (clase_segundo == SEMI_VOCAL_ESP) ) || \
							      ( (clase_primero == SEMI_VOCAL_ESP) && (clase_segundo == VOC_MED_ESP) ) || \
							      ( (clase_primero == SEMI_VOCAL_ESP) && (clase_segundo == VOC_CER_ESP) ) || \
							      ( (clase_primero == VOC_CER_ESP) && (clase_segundo == SEMI_VOCAL_ESP) ) )
#endif

typedef float Vector_pesos_regresion[NUMERO_MAXIMO_PESOS_MODELO_ESPECTRAL];

typedef float Vector_cepstrum[NUMERO_COEFICIENTES_CEPSTRALES];

typedef float Vector_lsf[NUMERO_COEFICIENTES_LSF];

typedef struct {
    Vector_cepstrum vector;
    float varianza;
} Vector_cepstrum_varianza;

typedef Vector_cepstrum_varianza Tabla_centroides[NUMERO_FONEMAS][NUMERO_FONEMAS][2];

typedef struct {
    float tiempo;
    Vector_cepstrum vector;
} Vector_cepstrum_tiempo;


typedef struct {
	float tiempo;
    Vector_lsf vector;
} Vector_lsf_tiempo;

typedef struct Ventanas_cepstrum {
    int identificador;
    int numero_ventanas;
    Vector_cepstrum_tiempo ventana[MAXIMO_NUMERO_VECTORES_CEPSTRUM];
    Vector_cepstrum cepstrum_medio;
    struct Ventanas_cepstrum *siguiente;
} Ventanas_cepstrum;

typedef struct {
    Vector_semifonema_candidato *objetivo;
    Vector_semifonema_candidato *candidato;
    float distancia;
} Vector_distancias ;


const char *tipo_fon_espectral_cadena(char *fon);

int tipo_fon_espectral_enumerado(char *fon, clase_espectral *tipo_espectral);

int devuelve_tipo_alofono_espectral(char *nombre, clase_espectral *clase);

const char *devuelve_cadena_alofono_espectral(clase_espectral clase);

int carga_fichero_coeficientes_espectrales(float **cadena_coeficientes, FILE *fichero,
										   int numero_elementos_vector,
                       					   int *numero_vectores);

int carga_fichero_lsf(Vector_lsf_tiempo **cadena_lsf, FILE *fichero,
					  int numero_elementos_vector, int *numero_vectores, char lee_tiempo);

int escribe_fichero_cepstrum_bin(Vector_cepstrum_tiempo *cadena_cepstrum, int numero_vectores,
								 int numero_coeficientes_cepstrales, FILE *fichero);

int carga_fichero_cepstrum_bin(Vector_cepstrum_tiempo **cadena_cepstrum, FILE *fichero, int *numero_vectores, int numero_coeficientes_cepstrum);

int carga_fichero_cepstrum_txt(Vector_cepstrum_tiempo **cadena_cepstrum, FILE *fichero,
                          int *numero_vectores, int numero_coeficientes_cepstrum);

int devuelve_coeficientes(float *cadena_coeficientes, float tiempo_inicial,
						  float tiempo_final, marca_struct *cadena_marcas,
                          int numero_elementos_vector,
                          int numero_coeficientes, float *coeficientes);

int carga_vectores_cepstrum_unidad(Vector_semifonema_candidato *unidad, FILE *fichero,
                                   Vector_cepstrum_tiempo *cadena_cepstrum, int *numero_vectores,
                                   int frecuencia_muestreo);

int carga_vectores_cepstrum_unidad_txt(Vector_semifonema_candidato *unidad, FILE *fichero,
                                       Vector_cepstrum_tiempo *cadena_cepstrum, int *numero_vectores,
                                       int frecuencia_muestreo);

int calcula_distancia_cepstral_media(Vector_cepstrum_tiempo *cepstrum_objetivo, int ventanas_objetivo,
                                     Vector_cepstrum_tiempo *cepstrum_candidato, int ventanas_candidato,
                                     int numero_coeficientes_cepstrales, float *distancia);

float calcula_distancia_euclidea_general(float *vector1, float *vector2,
										 int numero_coeficientes);

float calcula_distancia_euclidea(Vector_cepstrum vector1, Vector_cepstrum vector2, int numero_coeficientes_cepstrales);

int devuelve_ventanas_cepstrum_unidad(Vector_semifonema_candidato *semifonema, char path_cepstrum[],
                                      char *numero_fichero,
                                      Vector_cepstrum_tiempo **cadena_cepstrum,
                                      int *numero_ventanas,
                                      int frecuencia_muestreo,
                                      unsigned int formato = 0);

int anhade_coeficientes_cepstrum_difonema(Vector_semifonema_candidato *semifonema);

void borra_cache_distancias();

int calcula_centroide(Vector_cepstrum centroide);

int calcula_varianza(Vector_cepstrum centroide, int numero_coeficientes_cepstrales, float *varianza);

int encuentra_unidad_proxima_a_centroide(Vector_cepstrum centroide, int numero_coeficientes_cepstrales,
										 int *identificador,
                                         Vector_cepstrum_tiempo **cepstrum_objetivo,
                                         int *numero_ventanas_objetivo);

int anhade_vector_medio_a_cache(Vector_semifonema_candidato *semifonema, char path_cepstrum[],
                                char *numero_fichero, int frecuencia_muestreo);

int calcula_vector_cepstrum_medio(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores,
                                  Vector_cepstrum vector_cepstrum_medio);

int calcula_vector_cepstrum_medio(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores,
                                  float inicio, float final,
                                  Vector_cepstrum vector_cepstrum_medio);

int calcula_vector_lsf_medio(Vector_lsf_tiempo *cadena_vectores, int numero_vectores,
                             Vector_lsf vector_lsf_medio);

int anhade_vector_distancia(Vector_distancias nuevo_vector, int tamano_maximo);

int anhade_vector_distancia(Vector_distancias nuevo_vector);

void reinicia_vector_distancia();

int devuelve_vector_distancia(Vector_distancias **apunta_distancia, int *distancias);

int obten_vector_cepstrum_proximo_a(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores,
                                    float tiempo, char anterior_posterior,
                                    Vector_cepstrum vector_cepstrum);


#endif
 
