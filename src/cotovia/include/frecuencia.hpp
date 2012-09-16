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
 


/** \file frecuencia.hpp
 *
 * En este módulo se recogen las funciones relacionadas con la estimación del 
 * pitch a lo largo de la evolución de cada unidad. Para ello se emplea la salida 
 * del proyecto de Daniel Esperante, en donde aparece la información del instante 
 * temporal de cada marca de pitch, así como de si se trata de un fonema sordo o 
 * sonoro.
 */

#ifndef _FRECUENCIA_HH

#define _FRECUENCIA_HH

#include "descriptor.hpp"
#include "grupos_acentuales.hpp"


#define FRECUENCIA_MAXIMA_LOCUTOR 170.0F ///< Frecuencia máxima admitida para un locutor; Si la estimación
                                         ///< es mayor, supondremos que hay algún tipo de error (CUIDADO).
                                         
#define FRECUENCIA_MINIMA_LOCUTOR 30.0F  ///< Frecuencia mínima admitida; Si es menor, supondremos que
                                         ///< hay algún tipo de error (Valor más que discutible).
                                         
#define FRECUENCIA_POR_DEFECTO 100.0F    ///< Frecuencia que se adopta ante algún tipo de situación
                                         ///< anómala en la estimación.
                                         
#define DURACION_POR_DEFECTO 0.15F      ///< Duración por defecto para los silencios de final de fichero.

#define DURACION_MINIMA 0.020F          ///< Duración mínima de un semifonema para que no se considere un
                                        ///< caso anómalo.
                                        
#define MAXIMA_DIFERENCIA_HZ 30.0F      ///< Máxima diferencia que puede existir entre dos estimaciones
                                        ///< consecutivas de la frecuencia en un fonema. (i.e, entre
                                        ///< inicial y mitad, o entre mitad y final).

/** \struct estructura_frecuencias
 *
 * Estructura que almacena los valores de frecuencia fundamental de un fonema.
 * 
 */
   
typedef struct {

    float frecuencia_inicial;
    float frecuencia_mitad;
    float frecuencia_final;

} estructura_frecuencias;


/** \struct marca_struct
 *
 * Estructura con la información de la posición temporal de la marca de pitch y 
 * su carácter sordo/sonoro.
 *  
 */

typedef struct {
        float etiqueta;    ///< Posición temporal de la marca de pitch, en
                           ///< segundos, relativa al inicio del fichero de 
                           ///< sonido en el que se encuentra la unidad.
        char sonoridad[2]; 
} marca_struct;

/** \struct estructura_fonemas
 *
 * Estructura con la información de inicio de cada fonema en el fichero de 
 * sonido al que pertenece.
 * 
 */   

typedef struct {
        float inicio;     ///< Instante de inicio, en segundos, del fonema en 
                          ///< el fichero de sonido. 
                          
        char fonema[OCTETOS_POR_FONEMA + 1]; ///< Nombre del fonema (un caracter adicional por
                                             ///< la posible marca de error).
} estructura_fonemas;


  /// Función principal para el cálculo de la frecuencia fundamental de un fonema.

int calcula_frecuencias(marca_struct *cadena, float inicio, float fin,
						            int frecuencia_muestreo, float frecuencia_maxima_locutor,
                        float frecuencia_minima_locutor,
                        Vector_descriptor_candidato *vector);
                        
  /// Calcula la duración de un fonema.
                        
int calcula_duracion(float inicio, float fin, int frecuencia_muestreo, Vector_descriptor_candidato* vector);

 /// Calcula la frecuencia fundamental al inicio del fonema.
 
int calcula_frecuencia_inicial(marca_struct **cadena, float *frecuencia_inicial, float tiempo_inicio,
                                            float tiempo_final);
                                            
 /// Calcula la frecuencia fundamental en la parte central del fonema.                                           
                                            
int calcula_frecuencia_mitad(marca_struct *cadena, float *frecuencia_mitad, float tiempo_medio);

 /// Calcula la frecuencia fundamental al final del fonema.

int calcula_frecuencia_final(marca_struct *cadena, float *frecuencia_final, float tiempo_final);

 /// Estima el valor de la frecuencia fundamental a partir de las primeras 
 /// muestras de la cadena de marcas que se le pasa como parámetro.

void calcula_frecuencia(int *cadena, int num_marcas, int frecuencia_muestreo,
						float *frecuencia);

 /// Añade al vector descriptor que se le pasa como parámetro una cadena con sus
 /// marcas de pitch ajustadas a los máximos de la forma de onda.

int crea_cadena_pitch(marca_struct *cadena, float inicial, float final,
					            int frecuencia_muestreo, float frecuencia_minima_locutor,
                      float frecuencia_maxima_locutor,
                      Vector_descriptor_candidato *vector);
                
 /// Comprueba si el fichero de marcas es válido para el fichero de fonemas.

int comprueba_validez_fichero_marcas(marca_struct *cadena, int tamano, float tiempo_final);

 /// Carga en memoria el contenido del fichero en el que se recogen las marcas 
 /// de pitch de una frase del Corpus.

int crea_cadena_fichero(marca_struct **cadena, FILE *fichero, int *tamano_final, float *tiempo_ultima_marca);

 /// Carga en memoria el contenido del fichero de fonemas. Incluye la 
 /// posibilidad de modificar las etiquetas con las que se representan los fonemas, 
 /// y convertirlas al formato deseado.

int carga_fichero_fonemas(estructura_fonemas **cadena_fonemas, FILE *fichero, int *numero_fonemas, int opcion);

 /// POR COMENTAR

int carga_fichero_fonemas_sintetico(estructura_fonemas **cadena_fonemas, FILE *fichero, int *numero_fonemas);

#endif

