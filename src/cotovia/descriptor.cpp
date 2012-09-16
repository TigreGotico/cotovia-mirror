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
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "modos.hpp"
#include "fonemas.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "descriptor.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "info_estructuras.hpp"

// Inicialización de las variables estáticas con el número de coeficientes cepstrales:
int Vector_semifonema_candidato::numero_coeficientes_cepstrales = NUMERO_COEFICIENTES_CEPSTRALES;
int Vector_difonema::numero_coeficientes_cepstrales = NUMERO_COEFICIENTES_CEPSTRALES;
int Vector_descriptor_candidato::numero_coeficientes_cepstrales = NUMERO_COEFICIENTES_CEPSTRALES;
int Vector_descriptor_candidato::frecuencia_muestreo = FRECUENCIA_MUESTREO_POR_DEFECTO;
/**
 * Función:   constructor de la clase                                                  
 * Clase:     Vector_descriptor_candidato                                              
 * \remarks Objetivo:  Inicializar las variables de tipo cadena o puntero para evitar problemas 
 *            en algún acceso a ellas si están sin inicializar.                        
 */

Vector_descriptor_candidato::Vector_descriptor_candidato() {

  semifonema[0] = '\0';
  contexto_derecho[0] = '\0';
  contexto_izquierdo[0] = '\0';
  contexto_derecho_2[0] = '\0';
  contexto_izquierdo_2[0] = '\0';
  marcas_izquierda = NULL;
  marcas_derecha = NULL;
  numero_marcas_izquierda = 0;
  numero_marcas_derecha = 0;
  palabra = NULL;
  validez_dcha = 1;
  validez_izda = 1;
  cepstrum = NULL;

}

/**
 * Función:   introduce_datos                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- ident: identificador numérico de la unidad.                             
 *          - semifon: identificador del semifonema.                                  
 *          - contexto_der: contexto por la derecha del semifonema.                   
 *          - contexto_izdo: contexto por la izquierda.                               
 *          - contexto_der2: fonema siguiente al de la derecha.                       
 *          - contexto_izdo2: fonema anterior al de la izquierda.                     
 *          - ac: información de acento (0 -> sin acento, 1 -> con acento).           
 *          - pal: palabra a la que pertenece la unidad.                              
 *          - fichero: nombre del fichero en el que se encuentra la unidad.           
 *          - primera_letra_palabra: = 1 si es la primera, 0 en otro caso.            
 *          - ultima_letra_palabra: = 1 si es la última, 0 en otro caso.
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error se devuelve un 0.                                  
 * \remarks Objetivo:  Cargar la función referente a las variables de entrada en el objeto.    
 * \remarks NOTA:    En el campo semifonema se introduce también el contexto, con el siguiente 
 *          formato: [id_semifonema] [\0] [cont_izda] [\0] [cont_dcha] [\0]. De esta  
 *          forma, si queremos crear un cluster por trifonemas, o por contexto por la 
 *          izquierda, no tenemos que comparar varios campos. LLega con hacer un      
 *          memcmp() del campo semifonema, con la longitud adecuada. Análogamente,    
 *          el campo contexto_derecho también tiene concatenado el semifonema central 
 *          con el formato: [contexto_dcha] [\0] [id_semifonema] [\0].                
 * \remarks NOTA2:   Creo que la nota anterior es una absoluta chorrada, si tenemos en cuenta  
 *          cómo va a funcionar en realidad el programa. Voy a dejar la posibilidad   
 *          de que se haga así, con un #define en modos.hpp.                          
 */

int Vector_descriptor_candidato::introduce_datos(int ident, char *semifon, char *contexto_der,
						 char *contexto_izdo, char *contexto_der2, char *contexto_izdo2, unsigned char ac,
						 char *pal, unsigned char fr, unsigned char posicion_frase, char primera_letra_palabra,
						 char ultima_letra_palabra) {

  identificador = ident;

  strcpy(contexto_derecho, contexto_der);
  strcpy(contexto_izquierdo, contexto_izdo);
  strcpy(semifonema, semifon);

  // Copiamos los contextos izquierdo y derecho -+ 2.

  strcpy(contexto_derecho_2, contexto_der2);
  strcpy(contexto_izquierdo_2, contexto_izdo2);

  // Rellenamos los campos acento y posicion.

  acento = ac;
  posicion = posicion_frase;

  // Asignamos espacio para la cadena palabra y anotamos su contenido.

  if ( (palabra = (char *) malloc(strlen(pal) + 1)) == NULL) {
    fprintf(stderr, "Error en introduce_datos, al asignar memoria.\n");
    return 1;
  }
         
  strcpy(palabra, pal);

  // Rellenamos el tipo de frase.

  frase = fr;

  // En principio, todas las unidades son válidas.

  validez_izda = 1;
  validez_dcha = 1;
  // Cambiar esta clasificación
  frontera_inicial = primera_letra_palabra;
  frontera_final = ultima_letra_palabra;
				          
  return 0;
         
}


/**
 * Función:   anhade_frecuencia                                                       
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- media: estimación de la frecuencia en la zona media del fonema.         
 *          - inicial: estimación de la frecuencia en la zona inicial del fonema.     
 *          - final: estimación en la zona final.                                     
 * \remarks Objetivo:  Introducir los valores de frecuencia en las variables del objeto.       
 */

void Vector_descriptor_candidato::anhade_frecuencia(float media, float inicial, float final) {

  frecuencia_mitad = media;
  frecuencia_izquierda = inicial;
  frecuencia_derecha = final;

}

/**
 * Función:   anhade_duracion                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- dur1: duración del semifonema de la parte izquierda.
 *                      - dur2: duración del semifonema de la parte derecha.
 * \remarks Objetivo:  Introducir en la variable del objeto el valor de la duración.           
 */

void Vector_descriptor_candidato::anhade_duracion(float dur1, float dur2) {

  duracion_1 = dur1;
  duracion_2 = dur2;

}

/**
 * Función:   anhade_potencia                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- estacionaria: estimación de la potencia en la zona estacionaria del     
 *            fonema.                                                                 
 *          - inicial: estimación de la potencia en la zona inicial.                  
 *          - final: estimación al final del fonema.                                  
 * \remarks Objetivo:  Introducir en las variables internas del objeto los valores referentes  
 *            a la potencia de la unidad.                                             
 * \remarks NOTA:      Los valores están en dB.                                                
 */

void Vector_descriptor_candidato::anhade_potencia(float estacionaria, float inicial,
						  float final) {
  if (estacionaria)
    potencia = 10*log10(estacionaria);
  else
    potencia = 0; // Por poner algo.
  if (inicial)
    potencia_inicial = 10*log10(inicial);
  else
    potencia_inicial = 0;
  if (final)
    potencia_final = 10*log10(final);
  else
    potencia_final = 0;

}


/**
 * Función:   devuelve_potencias                                                      
 * Clase:     Vector_descriptor_candidato                                             
 *	\remarks Salida:
 *			- pot_inicial: potencia al inicio del fonema.                             
 *          - pot_central: potencia en el punto medio del fonema.                     
 *          - pot_final: potencia al final del fonema.                                
 */

void Vector_descriptor_candidato::devuelve_potencias(float *pot_inicial, float *pot_central,
                                                     float *pot_final) {

  *pot_inicial = potencia_inicial;
  *pot_central = potencia;
  *pot_final = potencia_final;

}


/**
 * Función:   anhade_indices                                                          
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- origen: índice al inicio del fragmento correspondiente a la frase de la 
 *            unidad.                                                                 
 *          - inicio: número de muestra al inicio del fonema.                         
 *          - mitad: número de muestra a la mitad del fonema.                         
 * \remarks Objetivo:  Introducir en las variables internas del objeto los valores referentes  
 *            a los índices que delimitan la posición en el fichero de sonido de la   
 *            unidad.                                                                 
 */

void Vector_descriptor_candidato::anhade_indices(int origen, int inicio, int mitad) {

  indice_fichero = origen;
  indice_inicial = inicio;
  indice_mitad = mitad;

}

/**
 * Función:   anhade_marcas_pitch                                                     
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- num_derecha: número de marcas de pitch de la primera parte del fonema.  
 *          - marcas_derecha: vector con dichas marcas.                               
 *          - num_izquierda: número de marcas de pitch de la segunda parte.           
 *          - marcas_izquierda: vector con dichas marcas.                             
 * \remarks Objetivo:  Cargar los valores antes mencionados en las variables del objeto.       
 */

void Vector_descriptor_candidato::anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
						      short int num_derecha, int *marcas_dcha) {

  numero_marcas_derecha = num_derecha;
  marcas_derecha = marcas_dcha;
  numero_marcas_izquierda = num_izquierda;
  marcas_izquierda = marcas_izda;

}


/**
 * Función:   anhade_validez                                                          
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- modificable: indica si es necesario realizar una manipulación prosódica 
 *            en la unidad durante el proceso de síntesis.                            
 *          - izq_der: indica a qué parte de la unidad se refiere la validez.         
 */

void Vector_descriptor_candidato::anhade_validez(unsigned char modificable, char izq_der) {

  if (izq_der == IZQUIERDO)
    validez_izda = modificable;
  else
    validez_dcha = modificable;

}


/**
 * Función:   libera_memoria                                                          
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Objetivo:  Liberar la memoria reservada para las variables miembro.                
 */

void Vector_descriptor_candidato::libera_memoria() {

  if (palabra) {
    free(palabra);
    palabra = NULL;
  }

  if (marcas_izquierda) {
    free(marcas_izquierda);
    marcas_izquierda = NULL;
    marcas_derecha = NULL;
  }

  if (cepstrum) {
    free(cepstrum);
    cepstrum = NULL;
  }


}



/**
 * Función:   devuelve_fonemas                                                        
 * Clase:     Vector_descriptor_candidato                                             
 *	\remarks Salida:
 *			- central: fonema central de la unidad                                    
 *          - izquierdo: contexto por la izquierda                                    
 *          - derecho: contexto por la derecha                                        
 * \remarks Valor devuelto:                                                                    
 *          - Si el vector no está inicializado, devuelve un 1. En caso contrario, 0  
 * \remarks Objetivo:  Informar al usuario de la unidad contenida en el vector                 
 */

int Vector_descriptor_candidato::devuelve_fonemas(char central[], char izquierdo[], char derecho[]) {

  if (semifonema[0]) {
    strcpy(central, semifonema);
    strcpy(izquierdo, contexto_izquierdo);
    strcpy(derecho, contexto_derecho);
    return 0;
  }

  fprintf(stderr, "Error en devuelve_fonemas: fonemas no inicializados.\n");

  return 1;

}


/**
 * Función:   devuelve_palabra                                                        
 * Clase:     Vector_descriptor_candidato                                             
 *	\remarks Salida:
 *			- palabra_contexto: palabra a la que pertenece la unidad.                 
 */

void Vector_descriptor_candidato::devuelve_palabra(char palabra_contexto[]) {

  if (palabra)
    strcpy(palabra_contexto, palabra);
  else
    strcpy(palabra_contexto, "");

}


/**
 * Función:   devuelve_frase_posicion_acento                                          
 * Clase:     Vector_descriptor_candidato                                             
 *	\remarks Salida:
 *			- fr: tipo de frase.                                                      
 *          - pos: posición (inicial, media, final).                                  
 *          - ac: tonicidad de la sílaba a la que pertenece.                          
 */

void Vector_descriptor_candidato::devuelve_frase_posicion_acento(unsigned char *fr,
                                                                 unsigned char *pos,
                                                                 unsigned char *ac) {
  *fr = frase;
  *pos = posicion;
  *ac = acento;

}

/**
 * Función:   devuelve_duracion                                                       
 * Clase:     Vector_descriptor_candidato                                  
 *           \param[in] izquierdo_derecho: parte izquierda o derecha de la unidad.
 *	\remarks Salida:
 *			- duracion_unidad: duración de la unidad, en segundos.                    
 */

void Vector_descriptor_candidato::devuelve_duracion(float *duracion_unidad, char izquierdo_derecho) {

  if (izquierdo_derecho == IZQUIERDO)
    *duracion_unidad = duracion_1;
  else
    *duracion_unidad = duracion_2;

}


/**
 * Función:   devuelve_frecuencias                                                    
 * Clase:     Vector_descriptor_candidato                                             
 *	\remarks Salida:
 *			- inicial: frecuencia al inicio de la unidad.                             
 *          - mitad: frecuencia en la zona central de la unidad.                      
 *          - final: frecuencia al final de la unidad.                                
 */

void Vector_descriptor_candidato::devuelve_frecuencias(float *inicial, float *mitad,
                                                       float *final) {

  *inicial = frecuencia_izquierda;
  *mitad = frecuencia_mitad;
  *final = frecuencia_derecha;
}


/**
 * Función:   devuelve_validez                                                        
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- izq_dcha: indica si se desea conocer la validez de la parte derecha o   
 *            izquierda de la unidad.                                                 
 * \remarks Valor devuelto:                                                                    
 *          - La validez de la unidad.                                                
 */

unsigned char Vector_descriptor_candidato::devuelve_validez(char izq_dcha) {

  if (izq_dcha == IZQUIERDO)
    return validez_izda;

  return validez_dcha;

}


/**
 * Función:   adapta_difonema
 * Clase:     Vector_descriptor_candidato
 * \remarks Entrada:
 *			- difonema: vector de tipo difonema con la información que se desea
 *            adaptar.
 *          - izq_der: indica la parte del difonema que nos interesa.
 * \remarks Valor devuelto:
 *          - En ausencia de error devuelve un cero.
 */

int Vector_descriptor_candidato::adapta_difonema(Vector_difonema *difonema, char izq_der) {

  int contador;
  float *recorre_coeficientes;

  palabra = NULL;

  frontera_prosodica = difonema->frontera_prosodica;

  strcpy(contexto_derecho_2, "#");
  strcpy(contexto_izquierdo_2, "#");

  identificador = difonema->identificador;

  indice_fichero = difonema->indice;

  anterior_pitch = 160; // Sólo pruebas.
  ultimo_pitch = 160;

  frase = FRASE_ENUNCIATIVA;      // Estos datos son por poner algo.
  posicion = POSICION_MEDIA;

  if ( (cepstrum = (float *) malloc(4*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en adapta_difonema, al asignar memoria.\n");
    return 1;
  }

  recorre_coeficientes = (float *) cepstrum;

  if (izq_der == IZQUIERDO) {

    strcpy(semifonema, difonema->fonema_derecho);
    strcpy(contexto_izquierdo, difonema->fonema_izquierdo);
    strcpy(contexto_derecho, "#");   // Antiguamente, "?"

    frontera_inicial = difonema->frontera_inicial_derecha;
    frontera_final = difonema->frontera_final_derecha;

    duracion_1 = difonema->duracion_derecha;
    duracion_2 = difonema->duracion_derecha;

    frecuencia_mitad = difonema->frecuencia_derecha;
    frecuencia_izquierda = difonema->frecuencia_central;
    frecuencia_derecha = -1; // Por poner algo.

    numero_marcas_izquierda = difonema->numero_marcas_derecha;
    numero_marcas_derecha = 0;

    if (numero_marcas_izquierda == 0) {
      numero_marcas_izquierda = 1;
      marcas_izquierda = difonema->marcas_izquierda + 2*difonema->numero_marcas_izquierda - 2;
    } // if (numero_marcas_izquierda == 0)
    else
      marcas_izquierda = difonema->marcas_derecha;

    marcas_derecha = marcas_izquierda;

    potencia = difonema->potencia_derecha;
    potencia_inicial = difonema->potencia_central;
    potencia_final = 0; // Por poner algo.

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = difonema->cepstrum_derecho_inicio[contador];

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = difonema->cepstrum_derecho_final[contador];

    for (contador = 0; contador < 2*numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = 0.0;

  }
  else { // DERECHO

    strcpy(semifonema, difonema->fonema_izquierdo);
    strcpy(contexto_derecho, difonema->fonema_derecho);
    strcpy(contexto_izquierdo, "#"); // Antiguamente, "?"

    frontera_inicial = difonema->frontera_inicial_izquierda;
    frontera_final = difonema->frontera_final_izquierda;

    duracion_1 = difonema->duracion_izquierda;
    duracion_2 = difonema->duracion_izquierda;

    frecuencia_mitad = difonema->frecuencia_izquierda;
    frecuencia_derecha = difonema->frecuencia_central;
    frecuencia_izquierda = -1;

    numero_marcas_izquierda = 0;
    numero_marcas_derecha = difonema->numero_marcas_izquierda;

    if (numero_marcas_derecha == 0) {
      numero_marcas_derecha = 1;
      marcas_derecha = difonema->marcas_derecha;
    } // if (numero_marcas_derecha == 0)
    else
      marcas_derecha = difonema->marcas_izquierda;

    marcas_izquierda = marcas_derecha;

    potencia = difonema->potencia_izquierda;
    potencia_final = difonema->potencia_central;
    potencia_inicial = 0;

    for (contador = 0; contador < 2*numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = 0.0;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = difonema->cepstrum_izquierdo_inicio[contador];

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      *recorre_coeficientes++ = difonema->cepstrum_izquierdo_final[contador];

  }

  if (semifonema[0] == '#')
    acento = ATONA;
  else
    acento = TONICA;

  if (tipo_fon_espectral_enumerado(semifonema, (clase_espectral *) &tipo_espectral))
    return 1;

  if (tipo_de_fonema_duracion(semifonema, (clase_duracion *) &tipo_duracion))
    return 1;


  return 0;

}


/**
 * Función:   escribe_datos_txt
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:	- frecuencia_muestreo: frecuencia de muestreo de los datos de voz.		  
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero de texto en el que se escribe la información relevante  
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - En caso de error devuelve un 1.                                         
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            fácilmente legible por el usuario.                                      
 */

int Vector_descriptor_candidato::escribe_datos_txt(FILE *salida, int frecuencia_muestreo) {

  const char *nombre_clase;
  float *recorre_coeficientes;
  fprintf(salida, "%d\n", identificador);
  fprintf(salida, "%s\n", semifonema);
  fprintf(salida, "%s %s\n", contexto_izquierdo_2, contexto_izquierdo);
  fprintf(salida, "%s %s\n", contexto_derecho, contexto_derecho_2);

  if (acento)
    fprintf(salida, "Sílaba tónica.\n");
  else
    fprintf(salida, "Sílaba átona.\n");
  fprintf(salida, "%s\n", palabra);

  fprintf(salida, "Frontera inicial: %d.\n", frontera_inicial);
  fprintf(salida, "Frontera final: %d.\n", frontera_final);

  if ( (nombre_clase = devuelve_cadena_alofono_espectral((clase_espectral) tipo_espectral)) == NULL)
    return 1;

  fprintf(salida, "Clase espectral: %s.\n", nombre_clase);

  if ( (nombre_clase = devuelve_cadena_alofono_duracion((clase_duracion) tipo_duracion)) == NULL)
    return 1;

  fprintf(salida, "Clase duración: %s.\n", nombre_clase);

  char *cadena_auxiliar;

  if (devuelve_tipo_frase_cadena(frase, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  if (devuelve_tipo_posicion_cadena(posicion, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  if (devuelve_tipo_frontera_prosodica_cadena(frontera_prosodica, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  fprintf(salida, "%f\t%f\n", duracion_1, duracion_2);
  fprintf(salida, "%f %f %f\n", frecuencia_izquierda, frecuencia_mitad, frecuencia_derecha);
  fprintf(salida, "%f %f %f\n", potencia_inicial, potencia, potencia_final);
  fprintf(salida, "%d %d %d\n", indice_fichero, indice_inicial, indice_mitad);

  fprintf(salida, "%d %d\n\n", anterior_pitch, ultimo_pitch);

  int *correcaminos = marcas_izquierda;
  int i;

  fprintf(salida, "Marcas de pitch de la parte izquierda: %d\n", numero_marcas_izquierda);
  for (i = 0; i < numero_marcas_izquierda; i++) {
    fprintf(salida, "%d (%f) ", (int) *correcaminos, ((float) *correcaminos) / frecuencia_muestreo);
    correcaminos += 2;
  }

  fprintf(salida, "\nMarcas de pitch de la parte derecha: %d\n", numero_marcas_derecha);
  correcaminos = marcas_derecha;
  for (i = 0; i < numero_marcas_derecha; i++) {
    fprintf(salida, "%d (%f) ", (int) *correcaminos, ((float) *correcaminos) / frecuencia_muestreo);
    correcaminos += 2;
  }

  fprintf(salida, "\n\nCoeficientes cepstrales:\n");

  recorre_coeficientes = cepstrum;

  for (i = 0; i < 4; i++) {
    for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
      fprintf(salida, "%f ", *recorre_coeficientes);
    }
    fprintf(salida, "\n");
  } // for (i = 0; i < 4; ...


  fprintf(salida, "\n\n");

  return 0;
    
}

/**
 * Función:   escribe_datos_txt
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero de texto en el que se escribe la información relevante  
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - En caso de error devuelve un 1.                                         
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            fácilmente legible por el usuario.                                      
 */

int Vector_descriptor_candidato::escribe_datos_txt_simplificado(FILE *salida) {

  // Identificador
  fprintf(salida, "%d\t", identificador);

  // Fonemas
  fprintf(salida, "%s\t%s\t%s\t%s\t%s\t", semifonema, contexto_izquierdo, contexto_derecho, contexto_izquierdo_2, contexto_derecho_2);

  // Acento
  if (acento)
    fprintf(salida, "T\t");
  else
    fprintf(salida, "A\t");

  // Frontera Inicial, final y prosódica:
  fprintf(salida, "%d\t%d\t%d\t", frontera_inicial, frontera_final, frontera_prosodica);

  // Frase:
  switch (frase) {
  case FRASE_ENUNCIATIVA:
    fprintf(salida, "EN\t");
    break;
  case FRASE_EXCLAMATIVA:
    fprintf(salida, "EX\t");
    break;
  case FRASE_INTERROGATIVA:
    fprintf(salida, "IN\t");
    break;
  case FRASE_INACABADA:
    fprintf(salida, "SU\t");
    break;
  default:
    fprintf(stderr, "Error en escribe_datos_txt: Tipo de frase no contemplado.\n");
    return 1;
  }

  // F0
  fprintf(salida, "%.1f\t%.1f\t%.1f\t", frecuencia_izquierda, frecuencia_mitad, frecuencia_derecha);

  // Duración
  fprintf(salida, "%.4f\t%.4f\t", duracion_1, duracion_2);

  // Energía
  fprintf(salida, "%.1f\t%.1f\t%.1f\n\n", potencia_inicial, potencia, potencia_final);

  return 0;

}

/**
 * \fn crea_fonema
 * \remarks Crea una versión simplificada del fonema (sin marcas de pitch ni coeficientes cepstrales) a partir de los semifonemas que lo componen
 * \param[in] izquierdo semifonema de la parte izquierda
 * \param[in] derecho semifonema de la parte derecha
 */
void Vector_descriptor_candidato::crea_fonema(Vector_semifonema_candidato *izquierdo, Vector_semifonema_candidato *derecho) {

  this->identificador = izquierdo->identificador;

  strcpy(this->semifonema, izquierdo->semifonema);
  strcpy(this->contexto_izquierdo, izquierdo->contexto_izquierdo);
  strcpy(this->contexto_izquierdo_2, izquierdo->contexto_izquierdo_2);
  strcpy(this->contexto_derecho, izquierdo->contexto_derecho);
  strcpy(this->contexto_derecho_2, izquierdo->contexto_derecho_2);

  this->acento = izquierdo->acento;

  this->frontera_inicial = izquierdo->frontera_inicial;
  this->frontera_final = izquierdo->frontera_final;
  this->frontera_prosodica = izquierdo->frontera_prosodica;

  this->frase = izquierdo->frase;

  this->frecuencia_izquierda = izquierdo->frecuencia_inicial;
  this->frecuencia_mitad = izquierdo->frecuencia_final;
  this->frecuencia_derecha = derecho->frecuencia_final;

  this->duracion_1 = izquierdo->duracion;
  this->duracion_2 = derecho->duracion;

  this->potencia_inicial = izquierdo->potencia_inicial;
  this->potencia = izquierdo->potencia_final;
  this->potencia_final = derecho->potencia_final;

  this->palabra = NULL;
  this->cepstrum = NULL;
  this->marcas_izquierda = NULL;

}

/**
 * Función:   escribe_energia                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- izq_der: indica qué valores de la energía se deben mostrar.             
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la energía de la unidad.                                              
 * \remarks Objetivo:  escribir en un fichero la evolución de la energía según las unidades    
 *            seleccionadas para la síntesis.                                         
 */

void Vector_descriptor_candidato::escribe_energia(FILE *fichero, char izq_der) {

  if (izq_der == IZQUIERDO)
    fprintf(fichero, "%f %f ", potencia_inicial, potencia);
  else
    fprintf(fichero, "%f %f ", potencia, potencia_final);
        
}


/**
 * Función:   escribe_datos_bin                                                       
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            que ocupe poco espacio en memoria.                                      
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Vector_descriptor_candidato::escribe_datos_bin(FILE *salida) {

  unsigned char tamano_char;

  // De golpe, la parte estática. (estamos escribiendo el valor de los punteros a las
  // partes dinámicas, pero merece la pena, con respecto a hacer múltiples fwrite
  // para cada variable y tener que actualizar esta función cada vez que se añade
  // una simple variable estática. El mantenimiento es mucho más sencillo.

  fwrite(this, sizeof(Vector_descriptor_candidato), 1, salida);

  // Luego, la dinámica

  tamano_char = (unsigned char) (strlen(palabra) + 1);
  fwrite(&tamano_char, 1, 1, salida); // Apuntamos el tamano de la palabra contexto.
  fwrite(palabra, 1, tamano_char, salida);

  fwrite(marcas_izquierda, sizeof(int), 2*(numero_marcas_izquierda + numero_marcas_derecha),
	 salida);

  fwrite(cepstrum, sizeof(float), 4*numero_coeficientes_cepstrales, salida);

  return 0;

}

/**
 * Función:   escribe_datos_parte_estatica                                            
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  Escribir de forma rápida el contenido de un vector en un fichero.       
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Vector_descriptor_candidato::escribe_datos_parte_estatica(FILE *salida) {

  fwrite(this, sizeof(Vector_descriptor_candidato), 1, salida);

  return 0;

}

/**
 * Función:   escribe_datos_parte_dinamica                                            
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  Escribir de forma rápida el contenido de un vector en un fichero.       
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Vector_descriptor_candidato::escribe_datos_parte_dinamica(FILE *salida) {

  fwrite(palabra, sizeof(char), strlen(palabra) + 1, salida);

  fwrite(marcas_izquierda, sizeof(int), 2*(numero_marcas_izquierda + numero_marcas_derecha),
         salida);

  fwrite(cepstrum, sizeof(float), 4*numero_coeficientes_cepstrales,
         salida);

  return 0;

}

/**
 * Función:   lee_datos_parte_dinamica                                                
 * Clase:     Vector_descriptor_candidato                                             
 * Entrada y salida:                                                                  
 *          - cadena: cadena de caracteres en la que se encuentra la información de   
 *            la parte dinámica del objeto.                                           
 * \remarks NOTA:      Se supone que la memoria está correctamente reservada. Si no, casca.    
 */

void Vector_descriptor_candidato::lee_datos_parte_dinamica(char **cadena) {

  char *correcaminos = *cadena;
  int tamano = strlen(correcaminos);

  palabra = correcaminos;

  correcaminos += tamano + 1;

  marcas_izquierda = (int *) correcaminos;

  marcas_derecha = marcas_izquierda + 2*numero_marcas_izquierda;

  correcaminos += 2*sizeof(int)*(numero_marcas_izquierda + numero_marcas_derecha);

  cepstrum = (float *) correcaminos;

  correcaminos += 4*numero_coeficientes_cepstrales*sizeof(float);

}

/**
 * Función:   lee_datos_txt                                                           
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- entrada: fichero de texto en el que se encuentra la información de los  
 *            vectores descriptor.                                                    
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  cargar en memoria la información de los vectores descriptor desde un    
 *            fichero de texto.                                                       
 */

// Hay que retocar esta función. No está precisamente actualizada.

int Vector_descriptor_candidato::lee_datos_txt(FILE *entrada) {

  // Liberamos la memoria

  libera_memoria();

  fscanf(entrada, "%d %s\n", &identificador, semifonema);
  fscanf(entrada, "%s %s\n", contexto_izquierdo_2, contexto_izquierdo);
  fscanf(entrada, "%s %s\n", contexto_derecho, contexto_derecho_2);
  fscanf(entrada, "%c\n", &acento);
  fscanf(entrada, "%s\n", palabra);
  fscanf(entrada, "%c\n", &frase);
  fscanf(entrada, "%f\t%f\n", &duracion_1, &duracion_2);
  fscanf(entrada, "%f %f %f\n", &frecuencia_mitad, &frecuencia_izquierda, &frecuencia_derecha);

  fscanf(entrada, "%f %f %f\n\n", &potencia, &potencia_inicial, &potencia_final);

  return 0;

}

/**
 * Función:   lee_datos_de_cadena                                                     
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - cadena_datos: cadena de octetos en la que se encuentra la información.  
 * \remarks Objetivo:  cargar en memoria la información de los vectores descriptor desde una   
 *            cadena de octetos.                                                      
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error, devuelve un cero.
 * \remarks NOTA:
 *			- Se supone que en la cadena hay espacio reservado para la información
 *            que se lee. Si no lo hay, casca.
 */

int Vector_descriptor_candidato::lee_datos_de_cadena(char **cadena_datos) {

  unsigned char tamano_char;
  register char *correcaminos = *cadena_datos;
  int contador;
  float *apunta_cepstrum;

  identificador = *(int *)correcaminos;
  correcaminos += sizeof(int);

  memcpy(semifonema, correcaminos, OCTETOS_POR_FONEMA);

  correcaminos += OCTETOS_POR_FONEMA;

  memcpy(contexto_izquierdo, correcaminos, OCTETOS_POR_FONEMA);
  correcaminos += OCTETOS_POR_FONEMA;

  memcpy(contexto_derecho, correcaminos, OCTETOS_POR_FONEMA);
  correcaminos += OCTETOS_POR_FONEMA;

  memcpy(contexto_izquierdo_2, correcaminos, OCTETOS_POR_FONEMA);
  correcaminos += OCTETOS_POR_FONEMA;

  memcpy(contexto_derecho_2, correcaminos, OCTETOS_POR_FONEMA);
  correcaminos += OCTETOS_POR_FONEMA;

  acento = *correcaminos++;
  tamano_char = *correcaminos++;

  if ( (palabra = (char *) malloc(tamano_char)) == NULL) {
    fprintf(stderr, "Error en lee_datos_de cadena, al asignar memoria.\n");
    return 1;
  }

  sscanf(correcaminos, "%s", palabra);

  correcaminos += tamano_char;

  frase = *correcaminos++;
  posicion = *correcaminos++;

  duracion_1 = *(float *) correcaminos;
  correcaminos += sizeof(float);

  duracion_2 = *(float *) correcaminos;
  correcaminos += sizeof(float);

  frecuencia_izquierda = *(float *) correcaminos;
  correcaminos += sizeof(float);

  frecuencia_mitad = *(float *) correcaminos;
  correcaminos += sizeof(float);

  frecuencia_derecha = *(float*) correcaminos;
  correcaminos += sizeof(float);

  potencia_inicial = *(float *) correcaminos;
  correcaminos += sizeof(float);

  potencia = *(float *) correcaminos;
  correcaminos += sizeof(float);

  potencia_final = *(float *) correcaminos;
  correcaminos += sizeof(float);

  indice_fichero = *(int *) correcaminos;
  correcaminos += sizeof(int);

  indice_inicial = *(int *) correcaminos;
  correcaminos += sizeof(int);

  indice_mitad = *(int *) correcaminos;
  correcaminos += sizeof(int);

  numero_marcas_izquierda = *(short int *) correcaminos;
  correcaminos += sizeof(short int);

  numero_marcas_derecha = *(short int *) correcaminos;
  correcaminos += sizeof(short int);

  if (numero_marcas_izquierda || numero_marcas_derecha) {

    if ( (marcas_izquierda = (int *) malloc(2*(numero_marcas_izquierda+numero_marcas_derecha)
					    *sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en lee_datos_de_cadena, al asignar memoria.\n");
      return 2;
    }

    for (contador = 0; contador < 2*(numero_marcas_izquierda + numero_marcas_derecha);
	 contador++) {
      marcas_izquierda[contador] = *(int *) correcaminos;
      correcaminos += sizeof(int);
    }

    marcas_derecha = marcas_izquierda + 2*numero_marcas_izquierda;
  }
  else
    marcas_derecha = marcas_izquierda = NULL;

  //    numero_ventanas = *correcaminos++;


  if ( (cepstrum = (float * ) malloc(4*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en lee_datos_de_cadena, al asignar memoria.\n");
    return 1;
  }

  apunta_cepstrum = (float *) cepstrum;

  for (contador = 0; contador < 4*numero_coeficientes_cepstrales; contador++) {
    *apunta_cepstrum++ = *(float *) correcaminos;
    correcaminos += sizeof(float);
  } // for (contador = 0; ...

  validez_izda = *correcaminos++;

  validez_dcha = *correcaminos;

  *cadena_datos = correcaminos + sizeof(char);

  return 0;
    
}

/**
 * Función:   lee_datos_bin                                                           
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- entrada: fichero binario en el que se encuentra la información de los   
 *            vectores descriptor.                                                    
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 al llegar a fin de fichero, y un 2 si hay error.          
 * \remarks Objetivo:  cargar en memoria la información de los vectores descriptor desde un    
 *            fichero binario.                                                        
 */

int Vector_descriptor_candidato::lee_datos_bin(FILE *entrada) {

  unsigned char tamano_char;

  // Primero, la parte estática:

  fread(this, sizeof(Vector_descriptor_candidato), 1, entrada);

  if (feof(entrada))
    return 1;

  // Luego, la dinámica:

  fread(&tamano_char, 1, 1, entrada);

  if ( (palabra = (char *) malloc(tamano_char)) == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  }

  fread(palabra, 1, tamano_char, entrada);

  if (numero_marcas_izquierda || numero_marcas_derecha) {

    if ( (marcas_izquierda = (int *) malloc(2*(numero_marcas_izquierda+numero_marcas_derecha)
					    *sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
      return 2;
    }

    fread(marcas_izquierda, sizeof(int), 2*(numero_marcas_izquierda+numero_marcas_derecha),
          entrada);

    marcas_derecha = marcas_izquierda + 2*numero_marcas_izquierda;
  }
  else
    marcas_derecha = marcas_izquierda = NULL;

  if ( (cepstrum = (float *) malloc(4*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  }

  fread(cepstrum, sizeof(float), 4*numero_coeficientes_cepstrales, entrada);

  return 0;

}


/**
 * Función:   copia_contenido                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Entrada:
 *			- vector_original: vector descriptor candidato con la información que se  
 *            desea copiar.                                                           
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 en caso de error.                                         
 * \remarks Objetivo:  copiar las variables miembro de un vector descriptor a otro. No llega   
 *            con una simple igualación, debido a la presencia de punteros. Liberar   
 *            la memoria del vector original provocaría la pérdida de la información  
 *            del segundo.                                                            
 */

int Vector_descriptor_candidato::copia_contenido(Vector_descriptor_candidato vector_original) {

  int contador;
    
  *this = vector_original; // Copiamos de golpe toda la parte estática.

  // Asignamos memoria para las partes dinámicas.
    
  if ( (palabra = (char *) malloc(strlen(vector_original.palabra) + 1)) == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  } // if ( (palabra = ...

  strcpy(palabra, vector_original.palabra);

  if (numero_marcas_izquierda || numero_marcas_derecha) {
    if ( (marcas_izquierda = (int *) malloc(2*(numero_marcas_derecha + numero_marcas_izquierda)*
					    sizeof(int))) == NULL) {
      fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
      return 1;
    } // if ( (marcas_izquierda = ...

    memcpy(marcas_izquierda, vector_original.marcas_izquierda,
	   2*(numero_marcas_derecha + numero_marcas_izquierda)*sizeof(int));

    marcas_derecha = marcas_izquierda + 2*numero_marcas_izquierda;

  } // if (numero_marcas_izquierda || numero_marcas_derecha)
  else {
    marcas_izquierda = NULL;
    marcas_derecha = NULL;
  } // else

  if ( (cepstrum = (float *) malloc(4*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  }

  for (contador = 0; contador < 4*numero_coeficientes_cepstrales; contador++)
    cepstrum[contador] = vector_original.cepstrum[contador];

  return 0;

}


/**
 * Función:   copia_contenido
 * Clase:     Vector_descriptor_candidato
 * \remarks Entrada:
 *			- vector_original
 *          - izq_der: indica qué parte del difonema queremos copiar.
 * \remarks Valor devuelto:
 *          - Devuelve un 1 en caso de error.
 * \remarks Objetivo:  copiar las variables miembro de un vector descriptor a otro. No llega
 *            con una simple igualación, debido a la presencia de punteros. Liberar
 *            la memoria del vector original provocaría la pérdida de la información
 *            del segundo.
 */

int Vector_descriptor_candidato::copia_contenido(Vector_difonema *vector_original, char izq_der) {

  clase_espectral clase_esp;
  clase_duracion clase_dur;

  identificador = vector_original->identificador;

  acento = TONICA; // vector_original->acento;
  frase = ENUNCIATIVA; // vector_original->frase;
  posicion = vector_original->posicion;

  indice_fichero = vector_original->indice;
  indice_mitad = indice_inicial = 0;

  if ( (palabra = (char *) malloc(1)) == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  }

  palabra[0] = '\0';

  *contexto_derecho_2 = 0;
  *contexto_izquierdo_2 = 0;

  if (izq_der == IZQUIERDO) {

    strcpy(semifonema, vector_original->fonema_izquierdo);
    strcpy(contexto_derecho, vector_original->fonema_derecho);
    contexto_izquierdo[0] = '\0';

    duracion_1 = vector_original->duracion_izquierda;
    duracion_2 = vector_original->duracion_izquierda;

    frecuencia_mitad = vector_original->frecuencia_derecha;
    frecuencia_izquierda = vector_original->frecuencia_central;
    frecuencia_derecha = -1;

    potencia_final = -1;
    potencia = vector_original->potencia_derecha;
    potencia_inicial = vector_original->potencia_central;

    numero_marcas_izquierda = vector_original->numero_marcas_derecha;

    if ( (marcas_izquierda = (int *) malloc(2*numero_marcas_izquierda*sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
      return 1;
    }

    int *recorre = vector_original->marcas_derecha;

    for (int contador = 0; contador < 2*numero_marcas_izquierda; contador++)
      marcas_izquierda[contador] = *recorre++;

    numero_marcas_derecha = 0;
    marcas_derecha = NULL;

  }
  else {

    strcpy(semifonema, vector_original->fonema_derecho);
    strcpy(contexto_izquierdo, vector_original->fonema_izquierdo);
    contexto_derecho[0] = '\0';

    duracion_1 = vector_original->duracion_derecha;
    duracion_2 = vector_original->duracion_izquierda;

    frecuencia_mitad = vector_original->frecuencia_izquierda;
    frecuencia_derecha = vector_original->frecuencia_central;
    frecuencia_izquierda = -1;

    potencia_final = vector_original->potencia_central;
    potencia = vector_original->potencia_izquierda;
    potencia_inicial = -1;

    numero_marcas_izquierda = 0;

    numero_marcas_derecha = vector_original->numero_marcas_izquierda;

    int *recorre = vector_original->marcas_izquierda;

    if ( (marcas_derecha = (int *) malloc(2*numero_marcas_derecha*sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
      return 1;
    }

    for (int contador = 0; contador < 2*numero_marcas_derecha; contador++)
      marcas_derecha[contador] = *recorre++;

    marcas_izquierda = marcas_derecha;
  }

  //    cepstrum = NULL;


  if (tipo_fon_espectral_enumerado(semifonema, &clase_esp))
    return 1;

  tipo_espectral = (char) clase_esp;

  if (tipo_de_fonema_duracion(semifonema, &clase_dur))
    return 1;

  tipo_duracion = (char) clase_dur;

  if (izq_der == IZQUIERDO)
    validez_izda = vector_original->validez;
  else
    validez_dcha = vector_original->validez;

  return 0;

}


/**
 * Función:   devuelve_tipo_frase
 * Clase:     Vector_descriptor_candidato
 * \remarks Valor devuelto:
 *          - Devuelve el tipo de frase a la que pertenece la unidad
 */

unsigned char Vector_descriptor_candidato::devuelve_tipo_frase() {

  return frase;

}


/**
 * Función:   devuelve_posicion                                                        
 * Clase:     Vector_descriptor_candidato                                              
 * \remarks Valor devuelto:                                                                     
 *          - Devuelve la posición en la que se encuentra la unidad dentro de la frase 
 */

unsigned char Vector_descriptor_candidato::devuelve_posicion() {

  return posicion;

}

/**
 * Función:   devuelve_acento                                                         
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve la tonicidad de la sílaba a la que pertenece la unidad.        
 */

unsigned char Vector_descriptor_candidato::devuelve_acento() {

  return acento;

}

/**
 * Función:   devuelve_identificador                                                  
 * Clase:     Vector_descriptor_candidato                                             
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve el identificador de la unidad.                                 
 */

int Vector_descriptor_candidato::devuelve_identificador() {

  return identificador;

}


/**
 * \brief	Función que lee los datos de la cabecera de un fichero binario.
 * \remarks	Lee las variables estáticas de la clase.
 * \param	entrada fichero binario
 */

void Vector_descriptor_candidato::lee_cabecera(FILE *entrada) {

  fread(&numero_coeficientes_cepstrales, sizeof(int), 1, entrada);
  fread(&frecuencia_muestreo, sizeof(int), 1, entrada);

}


/**
 * \brief	Función que escribe los datos en la cabecera de un fichero binario.
 * \remarks	Escribe las variables estáticas de la clase.
 * \param	salida fichero binario
 */

void Vector_descriptor_candidato::escribe_cabecera(FILE *salida) {

  fwrite(&numero_coeficientes_cepstrales, sizeof(int), 1, salida);
  fwrite(&frecuencia_muestreo, sizeof(int), 1, salida);

}


/**
 * Función:   constructor de la clase                                                  
 * Clase:     Vector_descriptor_objetivo                                               
 * \remarks Objetivo:  Inicializar las variables de tipo cadena o puntero para evitar problemas 
 *            en algún acceso a ellas si están sin inicializar.                        
 */

Vector_descriptor_objetivo::Vector_descriptor_objetivo() {

  semifonema[0] = '\0';
  contexto_derecho[0] = '\0';
  contexto_izquierdo[0] = '\0';
  contexto_derecho_2[0] = '\0';
  contexto_izquierdo_2[0] = '\0';
  palabra = NULL;

}

/**
 * \brief Constructor de copia de la clase
 */

Vector_descriptor_objetivo::Vector_descriptor_objetivo(const Vector_descriptor_objetivo &origen) {

  *this = origen;

}

/**
 * \brief Sobrecarga del operador de igualación
 */

Vector_descriptor_objetivo &Vector_descriptor_objetivo::operator= (const Vector_descriptor_objetivo &origen) {

  int contador;

  if (this != &origen) {

    // Liberamos memoria...
    // this->~Vector_descriptor_objetivo();
    
    strcpy(semifonema, origen.semifonema);
    strcpy(contexto_derecho, origen.contexto_derecho);
    strcpy(contexto_izquierdo, origen.contexto_izquierdo);
    strcpy(contexto_derecho_2, origen.contexto_derecho_2);
    strcpy(contexto_izquierdo_2, origen.contexto_izquierdo_2);

    if ( (palabra = (char *) malloc((strlen(origen.palabra) + 1)*sizeof(char))) != NULL) // No me convence esto, pero los constructores no devuelven valores :S y no quiero llenarlo todo de excepciones :S
      strcpy(palabra, origen.palabra);
      
    acento = origen.acento;
    frase = origen.frase;
    posicion = origen.posicion;

    info_silaba = origen.info_silaba;

    frontera_inicial = origen.frontera_inicial;
    frontera_final = origen.frontera_final;
    frontera_prosodica = origen.frontera_prosodica;
    fronteras_prosodicas = origen.fronteras_prosodicas;

    duracion_1 = origen.duracion_1;
    duracion_2 = origen.duracion_2;

    frecuencia_izquierda = origen.frecuencia_izquierda;
    frecuencia_mitad = origen.frecuencia_mitad;
    frecuencia_derecha = origen.frecuencia_derecha;
    frecuencias_inicial = origen.frecuencias_inicial;
    frecuencias_mitad = origen.frecuencias_mitad;
    frecuencias_final = origen.frecuencias_final;

#ifdef _INCLUYE_POTENCIA
    potencia = origen.potencia;
#endif
    
  }

}



/**
 * Función:   inicializa                                                               
 * Clase:     Vector_descriptor_objetivo                                               
 * \remarks Objetivo:  Inicializar las variables de tipo cadena o puntero para evitar problemas 
 *            en algún acceso a ellas si están sin inicializar. Tiene la misma función 
 *            que el constructor, pero permite que podamos emplear la clase con el     
 *            realloc(), que no llama al constructor.                                   
 */

void Vector_descriptor_objetivo::inicializa() {

  semifonema[0] = '\0';
  contexto_derecho[0] = '\0';
  contexto_izquierdo[0] = '\0';
  contexto_derecho_2[0] = '\0';
  contexto_izquierdo_2[0] = '\0';
  if (palabra != NULL) {
    free(palabra);
    palabra = NULL;
  }

}

/**
 * Función:   introduce_datos                                                         
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- semifon: identificador del semifonema.                                  
 *          - contexto_der: contexto por la derecha del semifonema.                   
 *          - contexto_izdo: contexto por la izquierda.                               
 *          - contexto_der2: fonema siguiente al de la derecha.                       
 *          - contexto_izdo2: fonema anterior al de la izquierda.                     
 *          - ac: información de acento (0 -> sin acento, 1 -> con acento).           
 *          - pal: palabra a la que pertenece la unidad.                              
 *          - primera_letra_palabra: = 1 si es la primera, 0 en otro caso.            
 *          - ultima_letra_palabra: = 1 si es la última, 0 en otro caso.              
 *          - silaba: 2 si es primer fonema de la última sílaba de la palabra, 
 *          	        1 si es primer fonema de sílaba,
 *          	        0 en otro caso
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error se devuelve un 0.                                  
 * \remarks Objetivo:  Cargar la función referente a las variables de entrada en el objeto.    
 * \remarks NOTA:    En el campo semifonema se introduce también el contexto, con el siguiente 
 *          formato: [id_semifonema] [\0] [cont_izda] [\0] [cont_dcha] [\0]. De esta  
 *          forma, si queremos crear un cluster por trifonemas, o por contexto por la 
 *          izquierda, no tenemos que comparar varios campos. LLega con hacer un      
 *          memcmp() del campo semifonema, con la longitud adecuada. Análogamente,    
 *          el campo contexto_derecho también tiene concatenado el semifonema central 
 *          con el formato: [contexto_dcha] [\0] [id_semifonema] [\0].                
 */

int Vector_descriptor_objetivo::introduce_datos(char *semifon, char *contexto_der,
						char *contexto_izdo, char *contexto_der2, char *contexto_izdo2, unsigned char ac,
						char *pal, unsigned char fr, unsigned char posicion_frase, char primera_letra_palabra,
						char ultima_letra_palabra, char silaba) {

  // Liberamos la memoria

  libera_memoria();

  strcpy(semifonema, semifon);
  strcpy(contexto_derecho, contexto_der);
  strcpy(contexto_izquierdo, contexto_izdo);

  // Copiamos los contextos por la derecha +2 y por la izquierda -2.

  strcpy(contexto_derecho_2, contexto_der2);
  strcpy(contexto_izquierdo_2, contexto_izdo2);
         
  // Rellenamos los campos acento y posicion.

  acento = ac;
  posicion = posicion_frase;

  // Asignamos espacio para la cadena palabra y anotamos su contenido.

  if ( (palabra = (char *) malloc(strlen(pal) + 1)) == NULL) {
    fprintf(stderr, "Error en introduce_datos, al asignar memoria.\n");
    return 1;
  }
  strcpy(palabra, pal);

  // Rellenamos el tipo de frase.

  frase = fr;

  // Asignamos los valores de inicio y fin de palabra

  // Cambiar esta clasificación
  frontera_inicial = primera_letra_palabra;
  frontera_final = ultima_letra_palabra;

  info_silaba = silaba;
  return 0;
         
}


/**
 * Función:   anhade_frecuencia                                                       
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- media: estimación de la frecuencia en la zona media del fonema.         
 *          - inicial: estimación de la frecuencia en la zona inicial del fonema.     
 *          - final: estimación en la zona final.                                     
 * \remarks Objetivo:  Introducir los valores de frecuencia en las variables del objeto.       
 */

void Vector_descriptor_objetivo::anhade_frecuencia(float media, float inicial, float final) {

  frecuencia_mitad = media;
  frecuencia_izquierda = inicial;
  frecuencia_derecha = final;

}

/**
 * Función:   anhade_duracion                                                         
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- dur: duración del semifonema.                                           
 * \remarks Objetivo:  Introducir en la variable del objeto el valor de la duración.           
 */

void Vector_descriptor_objetivo::anhade_duracion(float dur1, float dur2) {

  duracion_1 = dur1;
  duracion_2 = dur2;
}

#ifdef _INCLUYE_POTENCIA

/**
 * Función:   anhade_potencia                                                         
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- estacionaria: estimación de la potencia en la zona estacionaria del     
 *            fonema.                                                                 
 * \remarks Objetivo:  Introducir en las variables internas del objeto los valores referentes  
 *            a la potencia de la unidad.                                             
 * \remarks NOTA:      Los valores están en dB.                                                
 */

void Vector_descriptor_objetivo::anhade_potencia(float estacionaria) {
  //, float inicial, float final) {

  potencia = 10*log10(estacionaria);
  //      potencia_inicial = 10*log10(inicial);
  //      potencia_final = 10*log10(final);

}

#endif

/**
 * Función:   escribe_datos_txt                                                       
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero de texto en el que se escribe la información relevante  
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            fácilmente legible por el usuario.                                      
 */

int Vector_descriptor_objetivo::escribe_datos_txt(FILE *salida) {

  char *cadena_auxiliar;

  fprintf(salida, "%s\n", semifonema);
  fprintf(salida, "%s %s\n", contexto_izquierdo_2, contexto_izquierdo);
  fprintf(salida, "%s %s\n", contexto_derecho, contexto_derecho_2);
    
  if (acento)
    fprintf(salida, "Sílaba tónica.\n");
  else
    fprintf(salida, "Sílaba átona.\n");
  fprintf(salida, "%s\n", palabra);

  if (devuelve_tipo_frase_cadena(frase, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  if (devuelve_tipo_posicion_cadena(posicion, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  if (devuelve_tipo_frontera_prosodica_cadena(frontera_prosodica, &cadena_auxiliar))
    return 1;

  fprintf(salida, "%s.\n", cadena_auxiliar);

  fprintf(salida, "%f %f\n", duracion_1, duracion_2);
  fprintf(salida, "%f %f %f\n", frecuencia_izquierda, frecuencia_mitad, frecuencia_derecha);

#ifdef _INCLUYE_POTENCIA
  //    fprintf(salida, "%f %f %f\n\n", potencia_inicial, potencia, potencia_final);
  fprintf(salida, "%f\n\n", potencia);
#endif

  return 0;
    
}

/**
 * Función:   lee_datos_txt                                                           
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- entrada: fichero de texto en el que se encuentra la información de los  
 *            vectores descriptor.                                                    
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 en caso de error.                                         
 * \remarks Objetivo:  cargar en memoria la información de los vectores descriptor desde un    
 *            fichero de texto.                                                       
 */

// Esta función está sin depurar.

int Vector_descriptor_objetivo::lee_datos_txt(FILE *entrada) {

  char palabra1[LONX_MAX_PALABRA], palabra2[LONX_MAX_PALABRA];

  // Liberamos la memoria

  libera_memoria();

  fscanf(entrada, "%s\n", semifonema);
  fscanf(entrada, "%s %s\n", contexto_izquierdo_2, contexto_izquierdo);
  fscanf(entrada, "%s %s\n", contexto_derecho, contexto_derecho_2);

  fscanf(entrada, "%s %s\n", palabra1, palabra2);
  if (strcmp(palabra2, "átona."))
    acento = TONICA;
  else
    acento = ATONA;

  fscanf(entrada, "%s\n", palabra1);

  if ( (palabra = (char *) malloc(strlen(palabra1) + 1)) == NULL) {
    fprintf(stderr, "Error en lee_datos_txt, al asignar memoria.\n");
    return 1;
  }

  strcpy(palabra, palabra1);

  fscanf(entrada, "%s %s\n", palabra1, palabra2);

  if (!strcmp(palabra2, "enunciativa."))
    frase = FRASE_ENUNCIATIVA;
  else
    if (!strcmp(palabra2, "interrogativa."))
      frase = FRASE_INTERROGATIVA;
    else
      if (!strcmp(palabra2, "exclamativa."))
	frase = FRASE_EXCLAMATIVA;
      else
	frase = FRASE_INACABADA;

  fscanf(entrada, "%s %s\n", palabra1, palabra2);

  if (!strcmp(palabra2, "inicial."))
    posicion = POSICION_INICIAL;
  else
    if (!strcmp(palabra2, "media."))
      posicion = POSICION_MEDIA;
    else
      posicion = POSICION_FINAL;

  fscanf(entrada, "%f %f\n", &duracion_1, &duracion_2);
  fscanf(entrada, "%f %f %f\n", &frecuencia_mitad, &frecuencia_izquierda, &frecuencia_derecha);

#ifdef _INCLUYE_POTENCIA
  //    fscanf(entrada, "%f %f %f\n\n", &potencia, &potencia_inicial, &potencia_final);
  fscanf(entrada, "%f\n\n", &potencia);
#endif

  return 0;

}

/**
 * Función:   libera_memoria                                                          
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Objetivo:  Liberar la memoria reservada para las variables miembro.                
 */

void Vector_descriptor_objetivo::libera_memoria() {

  if (palabra) {
    free(palabra);
    palabra = NULL;
  }

}

/**
 * Función:   devuelve_fonemas                                                        
 * Clase:     Vector_descriptor_objetivo                                              
 *	\remarks Salida:
 *			- central: fonema central de la unidad                                    
 *          - izquierdo: contexto por la izquierda                                    
 *          - derecho: contexto por la derecha                                        
 * \remarks Objetivo:  Mostrar la información fonológica de la unidad, para la selección de    
 *            unidades.                                                               
 */

void Vector_descriptor_objetivo::devuelve_fonemas(char central[], char izquierdo[],
                                                  char derecho[]) {

  strcpy(central, semifonema);
  strcpy(izquierdo, contexto_izquierdo);
  strcpy(derecho, contexto_derecho);

}

/**
 * Función:   devuelve_palabra                                                        
 * Clase:     Vector_descriptor_objetivo                                              
 *	\remarks Salida:
 *			- palabra_contexto: palabra a la que pertenece la unidad.                 
 */

void Vector_descriptor_objetivo::devuelve_palabra(char palabra_contexto[]) {

  strcpy(palabra_contexto, palabra);

}

/**
 * Función:   devuelve_duracion                                                       
 * Clase:     Vector_descriptor_objetivo                                              
 * \remarks Entrada:
 *			- izq_der: indica si nos referimos al semifonema izquierdo o derecho.     
 *	\remarks Salida:
 *			- duracion_unidad: duración de la unidad, en segundos.                    
 */

void Vector_descriptor_objetivo::devuelve_duracion(char izq_der, float *duracion_unidad) {

  if (izq_der == IZQUIERDO)
    *duracion_unidad = duracion_1;
  else
    *duracion_unidad = duracion_2;

}

/**
 * Función:   devuelve_frecuencias                                                    
 * Clase:     Vector_descriptor_objetivo                                              
 *	\remarks Salida:
 *			- inicial: frecuencia al inicio de la unidad.                             
 *          - mitad: frecuencia en la zona central de la unidad.                      
 *          - final: frecuencia al final de la unidad.                                
 */

void Vector_descriptor_objetivo::devuelve_frecuencias(float *inicial, float *mitad,
                                                      float *final) {

  *inicial = frecuencia_izquierda;
  *mitad = frecuencia_mitad;
  *final = frecuencia_derecha;
}

/**
 * Función:   devuelve_duraciones                                                     
 * Clase:     Vector_descriptor_objetivo                                              
 *	\remarks Salida:
 *			- dur1: duración de la parte izquierda del fonema.                        
 *          - dur2: duración de la parte derecha del mismo.                            
 */

void Vector_descriptor_objetivo::devuelve_duraciones(float *dur1, float *dur2) {

  *dur1 = duracion_1;
  *dur2 = duracion_2;

}

/**
 * Función:   devuelve_frase_posicion_acento                                          
 * Clase:     Vector_descriptor_objetivo                                              
 *	\remarks Salida:
 *			- fr: tipo de frase.                                                      
 *          - pos: posición (inicial, media, final).                                  
 *          - ac: tonicidad de la sílaba a la que pertenece.                          
 */

void Vector_descriptor_objetivo::devuelve_frase_posicion_acento(unsigned char *fr,
                                                                unsigned char *pos,
                                                                unsigned char *ac) {

  *fr = frase;
  *pos = posicion;
  *ac = acento;

}


/**
 * \brief	Función que crea un vector objetivo a partir de un candidato.
 * \remarks	Se emplea con la opción de Cotovía -fich_unidades, en la que ya se
 indica en un fichero las unidades candidatas que se quieren usar.
 * \param[in]	vector_candidato	Vector candidato del que se extraen los parámetros
 necesarios para el resto de la síntesis.
 * \return	0, en ausencia de error.
 */

int Vector_descriptor_objetivo::crea_vector_objetivo(Vector_descriptor_candidato *vector_candidato) {


  strcpy(semifonema, vector_candidato->semifonema);
  strcpy(contexto_derecho, vector_candidato->contexto_derecho);
  strcpy(contexto_izquierdo, vector_candidato->contexto_izquierdo);
  strcpy(contexto_derecho, vector_candidato->contexto_derecho);
  strcpy(contexto_izquierdo_2, vector_candidato->contexto_izquierdo_2);
  strcpy(contexto_derecho_2, vector_candidato->contexto_derecho_2);

  if ( (palabra = (char *) malloc((strlen(vector_candidato->palabra) + 1)*sizeof(char))) == NULL) {
    fprintf(stderr, "Error en crea_vector_objetivo, al asignar memoria.\n");
    return 1;
  }

  strcpy(palabra, vector_candidato->palabra);

  acento = vector_candidato->acento;
  frase = vector_candidato->frase;
  posicion = vector_candidato->posicion;

  info_silaba = 0; // No se usa en este modo.

  frontera_inicial = vector_candidato->frontera_inicial;
  frontera_final = vector_candidato->frontera_final;

  frontera_prosodica = vector_candidato->frontera_prosodica;

  duracion_1 = vector_candidato->duracion_1;
  duracion_2 = vector_candidato->duracion_2;

  frecuencia_izquierda = vector_candidato->frecuencia_izquierda;
  frecuencia_mitad = vector_candidato->frecuencia_mitad;
  frecuencia_derecha = vector_candidato->frecuencia_derecha;


#ifdef _INCLUYE_POTENCIA
  potencia = vector_candidato->potencia;
#endif

  return 0;

}

/**
 * \brief Función que crea un vector objetivo a partir de un semifonema candidato.
 * \remarks Se usa en el proyecto compara_candidatos
 * \param[in] vector_candidato	Vector candidato del que se extraen los parámetros necesarios para el resto de la síntesis.
 * \param[in] izquierdo_derecho Indica si el semifonema es izquierdo o derecho
 * \return	0, en ausencia de error.
 */

int Vector_descriptor_objetivo::crea_vector_objetivo(Vector_semifonema_candidato *vector_candidato, unsigned char izquierdo_derecho) {


  strcpy(semifonema, vector_candidato->semifonema);
  strcpy(contexto_derecho, vector_candidato->contexto_derecho);
  strcpy(contexto_izquierdo, vector_candidato->contexto_izquierdo);
  strcpy(contexto_derecho, vector_candidato->contexto_derecho);
  strcpy(contexto_izquierdo_2, vector_candidato->contexto_izquierdo_2);
  strcpy(contexto_derecho_2, vector_candidato->contexto_derecho_2);

  if ( (palabra = (char *) malloc((strlen(vector_candidato->palabra) + 1)*sizeof(char))) == NULL) {
    fprintf(stderr, "Error en crea_vector_objetivo, al asignar memoria.\n");
    return 1;
  }

  strcpy(palabra, vector_candidato->palabra);

  acento = vector_candidato->acento;
  frase = vector_candidato->frase;
  posicion = vector_candidato->posicion;

  info_silaba = 0; // No se usa en este modo.

  frontera_inicial = vector_candidato->frontera_inicial;
  frontera_final = vector_candidato->frontera_final;

  frontera_prosodica = vector_candidato->frontera_prosodica;

  if (izquierdo_derecho == IZQUIERDO) {

    duracion_1 = vector_candidato->duracion;
    duracion_2 = 0;

    frecuencia_izquierda = vector_candidato->frecuencia_inicial;
    frecuencia_mitad = vector_candidato->frecuencia_final;
    frecuencia_derecha = 0;

#ifdef _INCLUYE_POTENCIA
    potencia = vector_candidato->potencia_final;
#endif

  } // if (izquierdo_derecho == IZQUIERDO)
  else { // DERECHO
    
    duracion_1 = 0; 
    duracion_2 = vector_candidato->duracion;
    
    frecuencia_izquierda = 0;
    frecuencia_mitad = vector_candidato->frecuencia_inicial;
    frecuencia_derecha = vector_candidato->frecuencia_final;

#ifdef _INCLUYE_POTENCIA
    potencia = vector_candidato->potencia_inicial;
#endif

  } // DERECHO


  return 0;

}


/**
 * Función:   Constructor                                                             
 * Clase:     Vector_sintesis                                                         
 */

Vector_sintesis::Vector_sintesis() {

  //    fichero_origen = NULL;
  identificador = -1;
  duracion = 0.0;
  marcas_analisis = NULL;
  marcas_sintesis = NULL;
  correspondientes = NULL;
  numero_marcas_sintesis = 0;
  numero_marcas_analisis = 0;
  semifonema[0] = '\0';

}



/**
 * Función:   calcula_duracion_efectiva                                               
 * Clase:     Vector_sintesis			                                              
 * \remarks Entrada:	- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  	
 * \remarks Valor devuelto:																	  
 *		    - Calcula la duración real del semifonema según sus marcas de pitch.      
 */

float Vector_sintesis::calcula_duracion_efectiva(int frecuencia_muestreo) {

  float duracion_auxiliar = marcas_analisis[numero_marcas_analisis - 1];

  // Añadimos la duración debida a la parte inicial de la primera ventana y a la final
  // de la última.

  if (numero_marcas_analisis > 1) {
    duracion_auxiliar += marcas_analisis[1] - marcas_analisis[0];
    duracion_auxiliar += marcas_analisis[numero_marcas_analisis - 1] -
      marcas_analisis[numero_marcas_analisis - 2];
  } // if (numero_marcas > 1)
  else
    duracion_auxiliar += 2*TSORDOS;

  return duracion_auxiliar / frecuencia_muestreo;

}

/**
 * Función:   calcula_duracion_efectiva                                               
 * Clase:     Vector_sintesis                                     
 * \remarks Entrada:
 *			- frec_inicial: valor de frecuencia deseada al inicio del semifonema.     
 *          - frec_final: valor de frecuencia deseada al final del semifonema.        
 *			- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  	
 * \remarks Valor devuelto:                                                                    
 *          - La duración efectiva del semifonema.                                    
 * \remarks Objetivo:  Calcula la duración real del semifonema una vez hecha la modificación   
 *            de frecuencia fundamental.                                              
 */

float Vector_sintesis::calcula_duracion_efectiva(float frec_inicial,
						 float frec_final,
						 int frecuencia_muestreo) {

  int contador;
  int *marcas = marcas_analisis;
  int primera_marca = *marcas_analisis;
  int ultima_marca = marcas[numero_marcas_analisis - 1];
  float frec_auxiliares[PITCH_M_MAX];
  int marc_inter[PITCH_M_MAX];
  float denominador = ultima_marca - primera_marca;
  float duracion_auxiliar;

  if (numero_marcas_analisis == 1) {
    return ( (float )(ultimo_pitch + anterior_pitch) ) / frecuencia_muestreo;
  }
  else
    for (contador = 0; contador < numero_marcas_analisis; contador++, marcas++)
      frec_auxiliares[contador] = (frec_inicial*(ultima_marca - *marcas) -
				   frec_final*(primera_marca - *marcas) )
	/ denominador;

  // Cálculo real de la marcas de pitch intermedias a partir de frec intermedias

  marc_inter[0] = marcas_analisis[0];

  for (contador = 1; contador < numero_marcas_analisis; contador++) {
    if (frec_auxiliares[contador]!=0.0) {
      if (frec_auxiliares[contador]<1E-06)
	frec_auxiliares[contador]=1E-06;
      marc_inter[contador] = marc_inter[contador - 1] + (int) (frecuencia_muestreo/frec_auxiliares[contador]);
    } // if (frec_auxiliares[contador]
    else
      marc_inter[contador] = marc_inter[contador - 1] + (int) (frecuencia_muestreo*TSORDOS);
  } // for (contador = 1; ...

  duracion_auxiliar = marc_inter[numero_marcas_analisis - 1];

  // Ahora añadimos la duración debida a los segmentos de voz anterior a la primera marca y
  // posterior a la última.

  if (numero_marcas_analisis > 1) {
    duracion_auxiliar += marc_inter[1] - marc_inter[0];
    duracion_auxiliar += marc_inter[numero_marcas_analisis - 1] - marc_inter[numero_marcas_analisis - 2];
  } // if (numero_marcas_analisis > 1)
  else
    duracion_auxiliar += TSORDOS;

  return duracion_auxiliar/frecuencia_muestreo;

}


/**
 * Función:   devuelve_duracion                                                       
 * Clase:     Vector_descriptor_sintesis                                              
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve la duración original de la unidad.                             
 */

float Vector_sintesis::devuelve_duracion() {

  return duracion;
    
}

/**
 * Función:   devuelve_identificador                                                  
 * Clase:     Vector_descriptor_sintesis                                              
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve el identificador de la unidad.                                 
 */

int Vector_sintesis::devuelve_identificador() {

  return identificador;

}

/**
 * Función:   anhade_identificador                                                    
 * Clase:     Vector_sintesis                                                         
 * \remarks Entrada:
 *			- ident: identificador de la unidad.                                      
 */

void Vector_sintesis::anhade_identificador(int ident) {

  identificador = ident;

}


/**
 * Función:   anhade_validez
 * Clase:     Vector_sintesis
 * \remarks Entrada:
 *			- modificable: indica si se debe modificar la prosodia de la unidad.
 */

void Vector_sintesis::anhade_validez(unsigned char modificable) {

  validez = modificable;

}


/**
 * Función:   crea_vector_sintesis
 * Clase:     Vector_sintesis
 * \remarks Entrada:
 *			- original: vector_descriptor_candidato que contiene la información que
 *            se desea copiar.
 *          - izq_der: indica si la unidad es por la izquierda o por la derecha.
 *          - val:     modificación prosódica que hay que realizar sobre la unidad.
 *            0-> ninguna, 1->frecuencia, 2->duración, 3->ambas.
 *			- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error, devuelve un cero.                                 
 * \remarks NOTA:      La posición de las marcas se devuelve en muestras.                      
 */

int Vector_sintesis::crea_vector_sintesis(Vector_descriptor_candidato *original, char izq_der, unsigned char val, int frecuencia_muestreo) {

  int contador;
  int origen;

  validez = val;

  izquierdo_derecho = izq_der;

  numero_marcas_sintesis = 0;

  strcpy(semifonema, original->semifonema);

  sonoridad = (char) sordo_sonoro(original->semifonema);

  identificador = original->identificador;

  tipo_espectral = original->tipo_espectral;

  if (izq_der == IZQUIERDO) {

    frecuencia_inicial = original->frecuencia_izquierda;
    frecuencia_final = original->frecuencia_mitad;

    duracion = original->duracion_1;

    potencia_inicial = original->potencia_inicial;
    potencia_final = original->potencia;

    numero_marcas_analisis = original->numero_marcas_izquierda;

    if (numero_marcas_analisis > 1) {
      if ( (marcas_analisis = (int *) malloc(2*numero_marcas_analisis*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }
    }
    else
      if ( (marcas_analisis = (int *) malloc(4*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }



    if (numero_marcas_analisis != 0) {

      origen = original->marcas_izquierda[0];
      indice = original->indice_fichero + origen*sizeof(short int);

      if (numero_marcas_analisis > 1) {
	for (contador = 0; contador < 2*numero_marcas_analisis; contador += 2) {
	  marcas_analisis[contador] = original->marcas_izquierda[contador] - origen;
	  marcas_analisis[contador + 1] = original->marcas_izquierda[contador + 1];
	}
      }
      else {

	if (original->numero_marcas_derecha) {
	  // Tomamos una marca de la segunda parte del fonema.

	  for (contador = 0; contador < 4; contador += 2) {
	    marcas_analisis[contador] = original->marcas_izquierda[contador] - origen;
	    marcas_analisis[contador + 1] = original->marcas_izquierda[contador + 1];
	  }
	}
	else { // No hay marcas en la parte derecha. Nos inventamos una.
	  marcas_analisis[0] = 0;
	  marcas_analisis[1] = 0;
	  marcas_analisis[2] = 160; //FS*original->duracion / 2;
	  marcas_analisis[3] = 0;
	}
	numero_marcas_analisis = 2;
	//                numero_marcas_analisis = 1;
      }
    }
    else { // Nos las inventamos. Añadimos varias a boleo. V.gr, 2, ya que se
      //tratará de un sonido corto. Intentamos tomarlas a partir de la referencia de
      // la parte derecha.

      if (original->numero_marcas_derecha) {
	origen = original->marcas_derecha[0] - (int)(original->duracion_1*frecuencia_muestreo); // Aproximación
	indice = original->indice_fichero + origen*sizeof(short int);
	//                printf("Añadimos dos marcas en la unidad %d, a partir de las de la parte derecha.\n",
	//                        identificador);
      }
      else { // Tampoco hay marcas en la parte derecha. Rellenamos con silencio.
	//                origen = 0;
	indice = 0; // original->indice_fichero;
	//                printf("Añadimos dos marcas de silencio a la unidad %d.\n", identificador);
      }

      numero_marcas_analisis = 2;

      marcas_analisis[0] = 0;
      marcas_analisis[1] = 0; // Este valor no nos importa.
      marcas_analisis[2] = 160; //FS*original->duracion / 2;
      marcas_analisis[3] = 0;
    }
  }
  else { // DERECHO

    frecuencia_inicial = original->frecuencia_mitad;
    frecuencia_final = original->frecuencia_derecha;

    duracion = original->duracion_2;

    potencia_inicial = original->potencia;
    potencia_final = original->potencia_final;

    numero_marcas_analisis = original->numero_marcas_derecha;

    if (numero_marcas_analisis > 1) {
      if ( (marcas_analisis = (int *) malloc(2*numero_marcas_analisis*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }
    }
    else
      if ( (marcas_analisis = (int *) malloc(4*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }

    if (numero_marcas_analisis != 0) {

      origen = original->marcas_derecha[0];
      indice = original->indice_fichero + origen*sizeof(short int);

      if (numero_marcas_analisis > 1)
	for (contador = 0; contador < 2*numero_marcas_analisis; contador += 2) {
	  marcas_analisis[contador] = original->marcas_derecha[contador] - origen;
	  marcas_analisis[contador + 1] = original->marcas_derecha[contador + 1];
	}
      else { // Nos inventamos la segunda marca.

	numero_marcas_analisis = 2;
	//                numero_marcas_analisis = 1;

	marcas_analisis[0] = 0;
	marcas_analisis[1] = 0;
	marcas_analisis[2] = 160;
	marcas_analisis[3] = 0;
      }

    }
    else { // Nos las inventamos. Añadimos varias a boleo. V.gr, 2, ya que se
      //tratará de un sonido corto. Intentamos tomarlas de la parte izquierda.

      numero_marcas_analisis = 2;
      //            numero_marcas_analisis = 1;

      if (original->numero_marcas_izquierda) {
	origen = original->marcas_izquierda[0] + (int)(original->duracion_1*frecuencia_muestreo); // Aproximación
	indice = original->indice_fichero + origen*sizeof(short int);
	//                printf("Añadimos dos marcas en la unidad %d, a partir de las de la parte izquierda.\n",
	//                        identificador);
      }
      else { // Tampoco hay marcas en la parte izquierda. Rellenamos con silencio.
	//                origen = 0;
	indice = original->indice_fichero;
	//                printf("Añadimos dos marcas de silencio a la unidad %d.\n", identificador);
      }

      //            printf("Añadimos dos marcas a la unidad %d.\n", identificador);

      marcas_analisis[0] = 0;
      marcas_analisis[1] = 0; // Este valor no nos importa.
      marcas_analisis[2] = 160; //FS*original->duracion / 2;
      marcas_analisis[3] = 0;
    }
  }

  correspondientes = NULL;
  marcas_sintesis = NULL;
  numero_marcas_sintesis = 0;
    
  return 0;
    
}


/**
 * Función:   crea_vector_sintesis                                                    
 * Clase:     Vector_sintesis                                                         
 * \remarks Entrada:
 *			- original: vector_difonema que contiene la información que se desea      
 *            copiar.                                                                 
 *          - izq_der: indica si la unidad es por la izquierda o por la derecha.      
 *          - val:     modificación prosódica que hay que realizar sobre la unidad.   
 *            0-> ninguna, 1->frecuencia, 2->duración, 3->ambas.                      
 *			- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  	
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error, devuelve un cero.                                 
 * \remarks NOTA:      La posición de las marcas se devuelve en muestras.                      
 * \remarks NOTA2:     Se trata de una adaptación de la función anterior al caso en que la     
 *            entrada se encuentra en un objeto de tipo Vector_difonema.              
 */

int Vector_sintesis::crea_vector_sintesis(Vector_difonema *original, char izq_der, unsigned char val, int frecuencia_muestreo) {

  int contador;
  int origen;

  izquierdo_derecho = izq_der;

  numero_marcas_sintesis = 0;

  if (izq_der == IZQUIERDO)
    strcpy(semifonema, original->fonema_derecho);
  else
    strcpy(semifonema, original->fonema_izquierdo);

  validez = val;

  (izq_der == IZQUIERDO) ?
    sonoridad = (char) sordo_sonoro(original->fonema_izquierdo):
    sonoridad = (char) sordo_sonoro(original->fonema_derecho);

  identificador = original->identificador;

  anterior_pitch = 160; // Sólo pruebas.
  ultimo_pitch = 160;

  if (tipo_fon_espectral_enumerado(semifonema, (clase_espectral *) &tipo_espectral))
    return 1;

  if (izq_der == IZQUIERDO) {

    frecuencia_inicial = original->frecuencia_central;
    frecuencia_final = original->frecuencia_derecha;

    potencia_inicial = original->potencia_central;
    potencia_final = original->potencia_derecha;

    duracion = original->duracion_derecha;

    numero_marcas_analisis = original->numero_marcas_derecha;

    if (numero_marcas_analisis != 0) {

      origen = original->marcas_derecha[0];
      indice = original->indice + origen*sizeof(short int);

      if (numero_marcas_analisis == 1) { // Nos inventamos una marca.
	if ( (marcas_analisis = (int *) malloc(2*sizeof(int)))
	     == NULL) {
	  fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	  return 1;
	}
      }
      else
	if ( (marcas_analisis = (int *) malloc(numero_marcas_analisis*sizeof(int)))
	     == NULL) {
	  fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	  return 1;
	}

      for (contador = 0; contador < numero_marcas_analisis; contador++) {
	marcas_analisis[contador] = original->marcas_derecha[contador] - origen;
      }

      if (numero_marcas_analisis == 1) {
	marcas_analisis[1] = marcas_analisis[0] + (int) (frecuencia_muestreo*TSORDOS) ;
	numero_marcas_analisis = 2;
      }
    }
    else { // Nos las inventamos. Añadimos varias a boleo. V.gr, 2, ya que se
      //tratará de un sonido corto. Intentamos tomarlas a partir de la referencia de
      // la parte derecha.

      if (original->numero_marcas_izquierda) {
	origen = original->marcas_izquierda[0] +
	  (int ) (original->duracion_izquierda*frecuencia_muestreo); // Aproximación
	indice = original->indice + origen*sizeof(short int);
	//                printf("Añadimos dos marcas en la unidad %d, a partir de las de la parte derecha.\n",
	//                        identificador);
      }
      else { // Tampoco hay marcas en la parte derecha. Rellenamos con silencio.
	//                origen = 0;
	indice = 0; // original->indice_fichero;
	//                printf("Añadimos dos marcas de silencio a la unidad %d.\n", identificador);
      }

      numero_marcas_analisis = 2;

      if ( (marcas_analisis = (int *) malloc(numero_marcas_analisis*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }

      marcas_analisis[0] = 0;
      marcas_analisis[1] = (int) (frecuencia_muestreo*original->duracion_izquierda / 2);
    }
  }
  else { // DERECHO

    frecuencia_inicial = original->frecuencia_izquierda;
    frecuencia_final = original->frecuencia_central;

    potencia_inicial = original->potencia_izquierda;
    potencia_final = original->potencia_central;

    duracion = original->duracion_izquierda;

    numero_marcas_analisis = original->numero_marcas_izquierda;

    if (numero_marcas_analisis != 0) {

      origen = original->marcas_izquierda[0];
      indice = original->indice + origen*sizeof(short int);

      if (numero_marcas_analisis == 1) // Cogemos otra de la parte derecha.
	numero_marcas_analisis = 2;

      if ( (marcas_analisis = (int *) malloc(numero_marcas_analisis*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }

      for (contador = 0; contador < numero_marcas_analisis; contador++) {
	marcas_analisis[contador] = original->marcas_izquierda[contador] - origen;
      }

    }
    else { // Nos las inventamos. Añadimos varias a boleo. V.gr, 2, ya que se
      //tratará de un sonido corto. Intentamos tomarlas de la parte izquierda.

      if (original->numero_marcas_izquierda) {
	origen = original->marcas_izquierda[0] + (int)(original->duracion_derecha*frecuencia_muestreo); // Aproximación
	indice = original->indice + origen*sizeof(short int);
	//                printf("Añadimos dos marcas en la unidad %d, a partir de las de la parte izquierda.\n",
	//                        identificador);
      }
      else { // Tampoco hay marcas en la parte derecha. Rellenamos con silencio.
	//                origen = 0;
	indice = original->indice;
	//                printf("Añadimos dos marcas de silencio a la unidad %d.\n", identificador);
      }

      //            printf("Añadimos dos marcas a la unidad %d.\n", identificador);

      numero_marcas_analisis = 2;

      if ( (marcas_analisis = (int *) malloc(numero_marcas_analisis*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_sintesis, al asignar memoria.\n");
	return 1;
      }

      marcas_analisis[0] = 0;
      marcas_analisis[1] = (int) (frecuencia_muestreo*original->duracion_derecha / 2);

    }
  }

  correspondientes = NULL;
  marcas_sintesis = NULL;
  numero_marcas_sintesis = 0;

  return 0;
    
}


/**
 * Función:   crea_vector_sintesis                                                    
 * Clase:     Vector_sintesis                                                         
 * \remarks Entrada:
 *			- original: vector_semifonema_candidato que contiene la información que   
 *            se desea copiar.                                                        
 *          - izq_der: indica si la unidad es por la izquierda o por la derecha.      
 *          - val:     modificación prosódica que hay que realizar sobre la unidad.   
 *            0-> ninguna, 1->frecuencia, 2->duración, 3->ambas.                      
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error, devuelve un cero.                                 
 * \remarks NOTA:      La posición de las marcas se devuelve en muestras.                      
 */

int Vector_sintesis::crea_vector_sintesis(Vector_semifonema_candidato *original, char izq_der, unsigned char val) {

  validez = val;

  izquierdo_derecho = izq_der;

  frecuencia_inicial = original->frecuencia_inicial;
  frecuencia_final = original->frecuencia_final;

  strcpy(semifonema, original->semifonema);
    
  sonoridad = original->sonoridad;

  tipo_espectral = original->tipo_espectral;
    
  potencia_inicial = original->potencia_inicial;
  potencia_final = original->potencia_final;

  anterior_pitch = original->anterior_pitch;
  ultimo_pitch = original->ultimo_pitch;
    
  numero_marcas_analisis = original->numero_marcas;

  identificador = original->identificador;

  duracion = original->duracion;

  marcas_analisis = original->marcas_pitch;

  indice = original->indice_primera_marca; // (En bytes)

  correspondientes = NULL;
  marcas_sintesis = NULL;
  numero_marcas_sintesis = 0;

  return 0;

}


/**
 * Función:   izquiero_o_derecho                                                      
 * Clase:     Vector_sintesis                                                         
 * \remarks Objetivo:  Indica el tipo de unidad cuya información se almacena, es decir,        
 *            semifonema con contexto por la izquierda, o con contexto por la derecha.
 *            También hay la posibilidad de indicar a partir de este campo si se      
 *            trata de una unidad del corpus original.                                
 */

char Vector_sintesis::izquierdo_o_derecho() {

  return izquierdo_derecho;

}


/**
 * Función:   devuelve_marcas_analisis                                                
 * Clase:     Vector_sintesis                                                         
 *	\remarks Salida:
 *			- marc_analisis: vector con las marcas de análisis.                       
 *          - numero: número de marcas de análisis.                                   
 */

void Vector_sintesis::devuelve_marcas_analisis(int **marc_analisis, short int *numero) {

  *marc_analisis = marcas_analisis;
  *numero = numero_marcas_analisis;
}

/**
 * Función:   devuelve_indice                                                         
 * Clase:     Vector_sintesis                                                         
 *	\remarks Salida:
 *			- indice_unidad: índice al inicio de la unidad.                           
 */

void Vector_sintesis::devuelve_indice(int *indice_unidad) {

  *indice_unidad = indice;

}

/**
 * Función:   devuelve_marcas                                                         
 * Clase:     Vector_sintesis                                                         
 *	\remarks Salida:
 *			- marc_analisis: vector con las marcas de análisis.                       
 *          - num_analisis: número de marcas de análisis.                             
 *          - marc_corr: correspondencia entre las marcas de análisis y las de        
 *            síntesis.                                                               
 *          - marc_sintesis: marcas de síntesis.                                      
 *          - num_sintesis: número de marcas de síntesis.                             
 */

void Vector_sintesis::devuelve_marcas(int **marc_analisis, int **marc_corr, int **marc_sintesis,
                                      short int *num_analisis, short int *num_sintesis) {

  *marc_analisis = marcas_analisis;
  *marc_corr = correspondientes;
  *marc_sintesis = marcas_sintesis;
  *num_analisis = numero_marcas_analisis;
  *num_sintesis = numero_marcas_sintesis;

}


/**
 * Función:   devuelve_validez
 * Clase:     Vector_sintesis
 * \remarks Valor devuelto:
 *          - La validez de la unidad.
 */

unsigned char Vector_sintesis::devuelve_validez() {

  return validez;

}


/**
 * Función:   incluye_marcas_sintesis                                                 
 * Clase:     Vector_sintesis                                                         
 * \remarks Entrada:
 *			- marcas_sint: vector con las marcas de sintesis.                         
 *          - marcas_corr: vector con las correspondencias con las marcas de análisis.
 *          - num_marcas: número de marcas.                                           
 */

void Vector_sintesis::incluye_marcas_sintesis_y_correspondientes(int *marcas_sint,
								 int *marcas_corr, short int num_marcas) {

  marcas_sintesis = marcas_sint;
  correspondientes = marcas_corr;
  numero_marcas_sintesis = num_marcas;

}

/**
 * Función:   libera_memoria                                                          
 * Clase:     Vector_sintesis                                                         
 */

void Vector_sintesis::libera_memoria() {

  if (correspondientes) {
    free(correspondientes);
    correspondientes = NULL;
  }

  if (marcas_sintesis) {
    free(marcas_sintesis);
    marcas_sintesis = NULL;
  }

}


/**
 * Función:   escribe_datos_txt                                                       
 * Clase:     Vector_sintesis                                                         
 * \remarks Objetivo:  Escribe en el fichero que se le pasa como parámetro el contenido de la  
 *            unidad.                                                                 
 */

void Vector_sintesis::escribe_datos_txt(FILE *fichero) {

  int contador;

  fprintf(fichero, "Identificador = %d.\n", identificador);

  fprintf(fichero, "Fonema = %s.\n", semifonema);
    
  if (izquierdo_derecho  == IZQUIERDO)
    fprintf(fichero, "%s\n", "Izquierdo.");
  else
    fprintf(fichero, "%s\n", "Derecho.");

  fprintf(fichero, "Índice = %d.\n", indice);

  fprintf(fichero, "Duración: %f.\n", duracion);

  fprintf(fichero, "Validez: %x.\n", validez);
        
  fprintf(fichero, "Número de marcas de análisis: %d.\n", numero_marcas_analisis);

  if (numero_marcas_analisis) {
    for (contador = 0; contador < numero_marcas_analisis; contador++) {
      fprintf(fichero, "%d\t", (int) marcas_analisis[2*contador]);
    }
  }


  fprintf(fichero, "\n");

  if (numero_marcas_sintesis) {
    fprintf(fichero, "Número de marcas de síntesis: %d.\n", numero_marcas_sintesis);
    fprintf(fichero, "Marcas de síntesis:\n");
    for (contador = 0; contador < numero_marcas_sintesis; contador++) {
      fprintf(fichero, "%d\t", (int) marcas_sintesis[contador]);
    }

    fprintf(fichero, "\nMarcas de correspondencia:\n");
    for (contador = 0; contador < numero_marcas_sintesis; contador++) {
      fprintf(fichero, "%d\t", (int) correspondientes[contador]);
    }

  }

  fprintf(fichero, "\n\n");
    
}


/**
 * Función:   Constructor                                                             
 * Clase:     Vector_difonema                                                         
 */

Vector_difonema::Vector_difonema() {

  marcas_izquierda = NULL;
  numero_marcas_izquierda = 0;

  marcas_derecha = NULL;
  numero_marcas_derecha = 0;

  cepstrum_izquierdo_inicio = NULL;
    
}


/**
 * Función:   anhade_identificador                                                    
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- id: identificador numérico único de la unidad.                          
 */

void Vector_difonema::anhade_identificador(int id) {

  identificador = id;

}


/**
 * Función:   anhade_fonemas                                                          
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- fonema_izdo: parte izquierda del semifonema.                            
 *          - fonema_dcho: parte derecha del semifonema.                              
 */

void Vector_difonema::anhade_fonemas(char fonema_izdo[], char fonema_dcho[]) {

  strcpy(fonema_izquierdo, fonema_izdo);
  strcpy(fonema_derecho, fonema_dcho);

}

/**
 * Función:   anhade_frecuencia                                                       
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- media: frecuencia en la parte central del difonema.                     
 *          - inicial: frecuencia en la parte inicial del difonema.                   
 *          - final: frecuencia en la parte final del difonema.                       
 */

void Vector_difonema::anhade_frecuencia(float media, float inicial, float final) {

  frecuencia_central = media;
  frecuencia_derecha = final;
  frecuencia_izquierda = inicial;

}


/**
 * Función:   anhade_duraciones                                                       
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- dur1: duración de la primera parte del difonema.                        
 *          - dur2: duración de la segunda parte del difonema.                        
 */

void Vector_difonema::anhade_duraciones(float dur1, float dur2) {

  duracion_izquierda = dur1;
  duracion_derecha = dur2;

}


/**
 * Función:   anhade_potencia                                                         
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- estacionaria: potencia en la parte central del difonema.                
 *          - inicial: potencia en la parte inicial del difonema.                     
 *          - final: potencia en la parte final del difonema.                         
 */

void Vector_difonema::anhade_potencia(float estacionaria, float inicial, float final)  {

  if (estacionaria)
    potencia_central = 10*log10(estacionaria);
  else
    potencia_central = 0;

  if (inicial)
    potencia_izquierda = 10*log10(inicial);
  else
    potencia_izquierda = 0;

  if (final)
    potencia_derecha = 10*log10(final);
  else
    potencia_derecha = 0;

}


/**
 * Función:   anhade_marcas_pitch                                                     
 * Clase:     Vector_difonema                                                         
 * Entrada  - marcas_izda: marcas de pitch de la primera parte del difonema.          
 *          - num_izquierda: número de marcas de la parte izquierda.                  
 *          - marcas_dcha: marcas de pitch de la parte derecha del difonema.          
 *          - num_derecha: número de marcas de la parte derecha.                      
 * \remarks NOTA:      La reserva de memoria se hace fuera de la función.                            
 */

void Vector_difonema::anhade_marcas_pitch(short int num_izquierda, int *marcas_izda,
                                          short int num_derecha, int *marcas_dcha) {

  numero_marcas_derecha = num_derecha;
  numero_marcas_izquierda = num_izquierda;

  marcas_izquierda = marcas_izda;
  marcas_derecha = marcas_dcha;

}


/**
 * Función:   anhade_indice                                                           
 * Clase:     Vector_difonema
 * \remarks Entrada:
 *			- indice_fichero: índice a la posición del fichero en que se encuentra la 
 *            forma de onda de la unidad.                                             
 */

void Vector_difonema::anhade_indice(int indice_fichero) {

  indice = indice_fichero;
    
}


/**
 * Función:   anhade_posicion                                                         
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- pos: posición del difonema en el logátomo original.                     
 */

void Vector_difonema::anhade_posicion(char pos) {

  posicion = pos;

}

/**
 * Función:   anhade_vectores_cepstrum                                                
 * Clase:     Vector_difonema			                                              
 * \remarks Entrada:
 *			- cepstrum_medio_izdo: vector cepstrum medio de la parte izquierda del    
 *            difonema.                                                               
 *          - cepstrum_medio_dcho: vector cepstrum medio de la parte derecha del      
 *            difonema.                                                               
 *          - cepstrum_medio_pas: vector cepstrum medio del semifonema anterior.      
 *          - cepstrum_medio_fut: vector cepstrum medio del semifonema posterior.
 * \return	En ausencia de error devuelve un cero.
 */

int Vector_difonema::anhade_vectores_cepstrum(float *cepstrum_medio_izdo,
					      float *cepstrum_medio_dcho,
					      float *cepstrum_medio_pas,
					      float *cepstrum_medio_fut) {

  if ( (cepstrum_medio_parte_izquierda = (float * ) malloc(4*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en anhade_vectores_cepstrum, al asignar memoria.\n");
    return 1;
  }

  cepstrum_medio_parte_derecha = cepstrum_medio_parte_izquierda + numero_coeficientes_cepstrales;
  cepstrum_medio_siguiente_semifonema = cepstrum_medio_parte_derecha + numero_coeficientes_cepstrales;
  cepstrum_medio_anterior_semifonema = cepstrum_medio_siguiente_semifonema + numero_coeficientes_cepstrales;

  for (int contador = 0; contador < numero_coeficientes_cepstrales; contador++) {
    cepstrum_medio_parte_izquierda[contador] = *cepstrum_medio_izdo++;
    cepstrum_medio_parte_derecha[contador] = *cepstrum_medio_dcho++;
    cepstrum_medio_siguiente_semifonema[contador] = *cepstrum_medio_fut++;
    cepstrum_medio_anterior_semifonema[contador] = *cepstrum_medio_pas++;
  } // for (int contador = 0...

  return 0;

}


/**
 * Función:   anhade_vectores_cepstrum_union                                          
 * Clase:     Vector_difonema			                                              
 * \remarks Entrada:
 *			- cepstrum_inicio_izdo: vector cepstrum al inicio de la parte izquierda   
 *			  del difonema.															  
 *          - cepstrum_final_izdo: vector cepstrum al final de la parte izquierda del 
 *			  difonema.																  
 *          - cepstrum_inicio_dcho: vector cepstrum al inicio de la parte derecha del 
 *			  difonema.																  
 *          - cepstrum_final_dcho: vector cepstrum al final de la parte derecha del   
 *			  difonema.
 * \return	En ausencia de error devuelve un cero.																  				
 */

int Vector_difonema::anhade_vectores_cepstrum_union(float *cepstrum_inicio_izdo,
						    float *cepstrum_final_izdo,
						    float *cepstrum_inicio_dcho,
						    float *cepstrum_final_dcho) {

  if ( (cepstrum_izquierdo_inicio = (float * ) malloc(numero_coeficientes_cepstrales*4*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en anhade_vectores_cepstrum_union, al asignar memoria.\n");
    return 1;
  }

  cepstrum_izquierdo_final = cepstrum_izquierdo_inicio + numero_coeficientes_cepstrales;
  cepstrum_derecho_inicio = cepstrum_izquierdo_final + numero_coeficientes_cepstrales;
  cepstrum_derecho_final = cepstrum_derecho_inicio + numero_coeficientes_cepstrales;

  for (int contador = 0; contador < numero_coeficientes_cepstrales; contador++) {
    cepstrum_izquierdo_inicio[contador] = *cepstrum_inicio_izdo++;
    cepstrum_izquierdo_final[contador] = *cepstrum_final_izdo++;
    cepstrum_derecho_inicio[contador] = *cepstrum_inicio_dcho++;
    cepstrum_derecho_final[contador] = *cepstrum_final_dcho++;
  } // for (int contador = 0...

  return 0;
    
}

/**
 * Función:   libera_memoria                                                          
 * Clase:     Vector_difonema                                                         
 */

void Vector_difonema::libera_memoria() {

  if (posicion == 0) {
    if (marcas_izquierda != NULL) {
      free(marcas_izquierda);
      marcas_izquierda = NULL;
      marcas_derecha = NULL;
    }
        
  }
}


/**
 * Función:   escribe_parte_estatica                                                  
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- fichero: fichero en el que se va a escribir la información estática del 
 *            difonema.                                                               
 * \remarks Objetivo:  Escribir en un fichero la parte estática de la unidad. En conjunto con  
 *            la siguiente función miembro permite una lectura posterior rápida y     
 *            eficaz de los datos de un fichero.                                      
 */

void Vector_difonema::escribe_parte_estatica(FILE *fichero) {

  fwrite(this, sizeof(Vector_difonema), 1, fichero);

}


/**
 * Función:   escribe_parte_dinamica                                                  
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- fichero: fichero en el que se va a escribir la información dinámica del 
 *            difonema.                                                               
 * \remarks Objetivo:  Escribir en un fichero la parte dinámica de la unidad. En conjunto con  
 *            la anterior función miembro permite una lectura posterior rápida y      
 *            eficaz de los datos de un fichero.                                      
 */

void Vector_difonema::escribe_parte_dinamica(FILE *fichero) {

  fwrite(marcas_izquierda, sizeof(int), numero_marcas_izquierda + numero_marcas_derecha,
	 fichero);

  fwrite(cepstrum_izquierdo_inicio, sizeof(float), 4*numero_coeficientes_cepstrales, fichero);

  fwrite(cepstrum_medio_parte_izquierda, sizeof(float), 4*numero_coeficientes_cepstrales, fichero);

}


/**
 * Función:   lee_parte_estatica                                                      
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:
 *			- fichero: fichero del que se va a leer la información estática del       
 *            difonema.                                                               
 */

void Vector_difonema::lee_parte_estatica(FILE *fichero) {

  fread(this, sizeof(Vector_difonema), 1, fichero);

}


/**
 * Función:   lee_parte_dinamica                                                
 * Clase:     Vector_difonema                                                         
 * Entrada y salida:                                                                  
 *          - cadena: cadena de caracteres en la que se encuentra la información de   
 *            la parte dinámica del objeto.                                           
 * \remarks NOTA:      Se supone que la memoria está correctamente reservada. Si no, casca.    
 */

void Vector_difonema::lee_parte_dinamica(char **cadena) {

  marcas_izquierda = (int *) *cadena;

  marcas_derecha = marcas_izquierda + numero_marcas_izquierda;

  *cadena += sizeof(int)*(numero_marcas_izquierda + numero_marcas_derecha);

  cepstrum_izquierdo_inicio = (float *) *cadena;

  cepstrum_izquierdo_final = cepstrum_izquierdo_inicio + numero_coeficientes_cepstrales;
  cepstrum_derecho_inicio = cepstrum_izquierdo_final + numero_coeficientes_cepstrales;
  cepstrum_derecho_final = cepstrum_derecho_inicio + numero_coeficientes_cepstrales;
  cepstrum_medio_parte_izquierda = cepstrum_derecho_final + numero_coeficientes_cepstrales;
  cepstrum_medio_parte_derecha = cepstrum_medio_parte_izquierda + numero_coeficientes_cepstrales;
  cepstrum_medio_siguiente_semifonema = cepstrum_medio_parte_derecha + numero_coeficientes_cepstrales;
  cepstrum_medio_anterior_semifonema = cepstrum_medio_siguiente_semifonema + numero_coeficientes_cepstrales;

  *cadena += 8*numero_coeficientes_cepstrales*sizeof(float);

}

/**
 * Función:   devuelve_fonemas                                                        
 * Clase:     Vector_difonema                                                         
 *	\remarks Salida:
 *			- fonema_izdo: fonema izquierdo del difonema.                             
 *          - fonema_dcho: fonema derecho del difonema.                               
 */

void Vector_difonema::devuelve_fonemas(char fonema_izdo[], char fonema_dcho[]) {

  strcpy(fonema_izdo, fonema_izquierdo);
  strcpy(fonema_dcho, fonema_derecho);

}

/**
 * Función:   devuelve_validez
 * Clase:     Vector_difonema
 * \remarks Valor devuelto:
 *          - La validez de la unidad.
 */

unsigned char Vector_difonema::devuelve_validez() {

  return validez;

}


/**
 * Función:   devuelve_identificador                                                  
 * Clase:     Vector_difonema                                                         
 */

int Vector_difonema::devuelve_identificador() {

  return identificador;

}

/**
 * Función:   escribe_datos_txt                                                       
 * Clase:     Vector_difonema                                                         
 * \remarks Entrada:	- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la unidad.                                                            
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            fácilmente legible por el usuario.                                      
 */

void Vector_difonema::escribe_datos_txt(FILE *fichero, int frecuencia_muestreo) {

  int contador = 0;
  int *correcaminos;
  float *recorre_coeficientes;

  fprintf(fichero, "Identificador: %d.\n", identificador);
  fprintf(fichero, "%s\t%s.\n", fonema_izquierdo, fonema_derecho);

  fprintf(fichero, "Índice: %d.\n", indice);

  fprintf(fichero, "Frontera inicial (izda): %d.\n", frontera_inicial_izquierda);
  fprintf(fichero, "Frontera final (izda): %d.\n", frontera_final_izquierda);
  fprintf(fichero, "Frontera inicial (dcha): %d.\n", frontera_inicial_derecha);
  fprintf(fichero, "Frontera final (dcha): %d.\n", frontera_final_derecha);

  fprintf(fichero, "Frontera prosódica: %d.\n", frontera_prosodica);

  fprintf(fichero, "%f\t%f.\n", duracion_izquierda, duracion_derecha);
  fprintf(fichero, "%f\t%f\t%f.\n", frecuencia_izquierda, frecuencia_central,
	  frecuencia_derecha);
  fprintf(fichero, "%f\t%f\t%f.\n", potencia_izquierda, potencia_central,
	  potencia_derecha);

  fprintf(fichero, "Número marcas izquierda: %d.\n", numero_marcas_izquierda);

  correcaminos = marcas_izquierda;

  for (; contador < numero_marcas_izquierda; contador++) {
    fprintf(fichero, "%d (%f) ", (int)*correcaminos, (float) *correcaminos / frecuencia_muestreo);
    correcaminos++;
  }

  fprintf(fichero, "\nNúmero marcas derecha: %d.\n", numero_marcas_derecha);

  correcaminos = marcas_derecha;

  for (contador = 0; contador < numero_marcas_derecha; contador++) {
    fprintf(fichero, "%d (%f) ", (int) *correcaminos, (float) *correcaminos / frecuencia_muestreo);
    correcaminos++;
  }

  fprintf(fichero, "\n\nCoeficientes cepstrales:\n");

  fprintf(fichero, "\nCepstrum al inicio del semifonema izquierdo:\n");

  recorre_coeficientes = (float *) cepstrum_izquierdo_inicio;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");

  fprintf(fichero, "\nCepstrum al final del semifonema izquierdo:\n");

  recorre_coeficientes = (float *) cepstrum_izquierdo_final;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");

  fprintf(fichero, "\nCepstrum al inicio del semifonema derecho:\n");

  recorre_coeficientes = (float *) cepstrum_derecho_inicio;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");

  fprintf(fichero, "\nCepstrum al final del semifonema derecho:\n");

  recorre_coeficientes = (float *) cepstrum_derecho_final;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");


  fprintf(fichero, "Vector cepstrum medio del semifonema pasado:\n");

  recorre_coeficientes = cepstrum_medio_anterior_semifonema;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");


  fprintf(fichero, "Vector cepstrum medio de la parte izquierda:\n");

  recorre_coeficientes = cepstrum_medio_parte_izquierda;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");

  fprintf(fichero, "Vector cepstrum medio de la parte derecha:\n");

  recorre_coeficientes = cepstrum_medio_parte_derecha;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n");

  fprintf(fichero, "Vector cepstrum medio del semifonema siguiente:\n");

  recorre_coeficientes = cepstrum_medio_siguiente_semifonema;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(fichero, "%f ", *recorre_coeficientes);
  }

  fprintf(fichero, "\n\n");

}


/**
 * \brief	Función que lee los datos de la cabecera de un fichero binario.
 * \remarks	Lee las variables estáticas de la clase.
 * \param	entrada fichero binario
 */

void Vector_difonema::lee_cabecera(FILE *entrada) {

  fread(&numero_coeficientes_cepstrales, sizeof(int), 1, entrada);

}


/**
 * \brief	Función que escribe los datos en la cabecera de un fichero binario.
 * \remarks	Escribe las variables estáticas de la clase.
 * \param	salida fichero binario
 */

void Vector_difonema::escribe_cabecera(FILE *salida) {

  fwrite(&numero_coeficientes_cepstrales, sizeof(int), 1, salida);

}


/**
 * Función:   constructor de la clase                                                  
 * Clase:     Vector_semifonema_candidato                                              
 * \remarks Objetivo:  Inicializar las variables de tipo cadena o puntero para evitar problemas 
 *            en algún acceso a ellas si están sin inicializar.                        
 */

Vector_semifonema_candidato::Vector_semifonema_candidato() {

  semifonema[0] = '\0';
  contexto_derecho[0] = '\0';
  contexto_izquierdo[0] = '\0';
  contexto_derecho_2[0] = '\0';
  contexto_izquierdo_2[0] = '\0';
  marcas_pitch = NULL;
  numero_marcas = 0;
  palabra = NULL;
  cepstrum_inicial = NULL;

}


/**
 * Función:   calcula_duracion_efectiva                                               
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada:
 *			- frec_inicial: valor de frecuencia deseada al inicio del semifonema.     
 *          - frec_final: valor de frecuencia deseada al final del semifonema.        
 *			- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  	
 * \remarks Valor devuelto:                                                                    
 *          - La duración efectiva del semifonema.                                    
 * \remarks Objetivo:  Calcula la duración real del semifonema una vez hecha la modificación   
 *            de frecuencia fundamental.                                              
 */

float Vector_semifonema_candidato::calcula_duracion_efectiva(float frec_inicial,
                                                             float frec_final,
                                                             int frecuencia_muestreo) {

  int contador;
  int *marcas = marcas_pitch;
  int primera_marca = *marcas_pitch;
  int ultima_marca = marcas[numero_marcas - 1];
  float frec_auxiliares[PITCH_M_MAX];
  int marc_inter[PITCH_M_MAX];
  float denominador = ultima_marca - primera_marca;
  float duracion_auxiliar;

  if (numero_marcas == 1) {
    return ( (float )(ultimo_pitch + anterior_pitch) ) / frecuencia_muestreo;
  }
  else
    for (contador = 0; contador < numero_marcas; contador++, marcas++)
      frec_auxiliares[contador] = (frec_inicial*(ultima_marca - *marcas) -
				   frec_final*(primera_marca - *marcas) )
	/ denominador;

  // Cálculo real de la marcas de pitch intermedias a partir de frec intermedias

  marc_inter[0] = marcas_pitch[0];

  for (contador = 1; contador < numero_marcas; contador++) {
    if (frec_auxiliares[contador]!=0.0) {
      if (frec_auxiliares[contador]<1E-06)
	frec_auxiliares[contador]=1E-06;
      marc_inter[contador] = marc_inter[contador - 1] + (int) (frecuencia_muestreo/frec_auxiliares[contador]);
    } // if (frec_auxiliares[contador]
    else
      marc_inter[contador] = marc_inter[contador - 1] + (int) (frecuencia_muestreo*TSORDOS);
  } // for (contador = 1; ...

  duracion_auxiliar = marc_inter[numero_marcas - 1];

  // Ahora añadimos la duración debida a los segmentos de voz anterior a la primera marca y
  // posterior a la última.

  if (numero_marcas > 1) {
    duracion_auxiliar += marc_inter[1] - marc_inter[0];
    duracion_auxiliar += marc_inter[numero_marcas - 1] - marc_inter[numero_marcas - 2];
  } // if (numero_marcas > 1)
  else
    duracion_auxiliar += TSORDOS;

  return duracion_auxiliar/frecuencia_muestreo;

}


/**
 * Función:   calcula_duracion_efectiva                                               
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada:	- frecuencia_muestreo: frecuencia de muestreo de los datos de sonido.	  	
 * \remarks Objetivo:  Calcula la duración real del semifonema según sus marcas de pitch.      
 */

void Vector_semifonema_candidato::calcula_duracion_efectiva(int frecuencia_muestreo) {

  float duracion_auxiliar = marcas_pitch[numero_marcas - 1];

  // Añadimos la duración debida a la parte inicial de la primera ventana y a la final
  // de la última.

  if (numero_marcas > 1) {
    duracion_auxiliar += marcas_pitch[1] - marcas_pitch[0];
    duracion_auxiliar += marcas_pitch[numero_marcas - 1] -
      marcas_pitch[numero_marcas - 2];    
  } // if (numero_marcas > 1)
  else
    duracion_auxiliar += 2*TSORDOS;

  duracion = duracion_auxiliar/frecuencia_muestreo;

}

/**
 * Función:   adapta_difonema
 * Clase:     Vector_semifonema_candidato
 * \remarks Entrada:
 *			- difonema: vector de tipo difonema con la información que se desea
 *            adaptar.
 *          - izq_der: indica la parte del difonema que nos interesa.
 * \remarks Valor devuelto:
 *          - En ausencia de error devuelve un cero.
 */

int Vector_semifonema_candidato::adapta_difonema(Vector_difonema *difonema, char izq_der) {


  izquierdo_derecho = izq_der;

  palabra = NULL;

  frontera_prosodica = difonema->frontera_prosodica;

  marca_inventada = 0;

  *contexto_derecho_2 = 0;
  *contexto_izquierdo_2 = 0;

  identificador = difonema->identificador;

  indice_fichero = difonema->indice;

  frase = ENUNCIATIVA;      // Estos datos son por poner algo.
  posicion = POSICION_MEDIA;

  if (izq_der == IZQUIERDO) {

    strcpy(semifonema, difonema->fonema_derecho);
    strcpy(contexto_izquierdo, difonema->fonema_izquierdo);
    strcpy(contexto_derecho, "#");   // Antiguamente, "?"

    frontera_inicial = difonema->frontera_inicial_derecha;
    frontera_final = difonema->frontera_final_derecha;

    duracion = difonema->duracion_derecha;

    frecuencia_final = difonema->frecuencia_derecha;
    frecuencia_inicial = difonema->frecuencia_central;

    potencia_inicial = difonema->potencia_central;
    potencia_final = difonema->potencia_derecha;

    cepstrum_inicial = difonema->cepstrum_derecho_inicio;
    cepstrum_mitad = difonema->cepstrum_derecho_final;
    cepstrum_final = difonema->cepstrum_medio_siguiente_semifonema;

    numero_marcas = difonema->numero_marcas_derecha;

    if (numero_marcas == 0) {
      marca_inventada = 1;
      numero_marcas = 1;
      marcas_pitch = difonema->marcas_izquierda + difonema->numero_marcas_izquierda - 1;
      anterior_pitch = 160;
      ultimo_pitch = 160;
    } // if (numero_marcas == 0)
    else {
      marcas_pitch = difonema->marcas_derecha;
      if (numero_marcas > 1) {
	anterior_pitch = marcas_pitch[1] - marcas_pitch[0];
	ultimo_pitch = marcas_pitch[numero_marcas - 1] -
	  marcas_pitch[numero_marcas - 2];
      } // if (numero_marcas > 1)
      else {
	anterior_pitch = 160;
	ultimo_pitch = 160;
      } // else

    }

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    strcpy(semifonema, difonema->fonema_izquierdo);
    strcpy(contexto_derecho, difonema->fonema_derecho);
    strcpy(contexto_izquierdo, "#"); // Antiguamente, "?"

    frontera_inicial = difonema->frontera_inicial_izquierda;
    frontera_final = difonema->frontera_final_izquierda;

    duracion = difonema->duracion_izquierda;

    frecuencia_inicial = difonema->frecuencia_izquierda;
    frecuencia_final = difonema->frecuencia_central;

    potencia_inicial = difonema->potencia_izquierda;
    potencia_final = difonema->potencia_central;

    cepstrum_inicial = difonema->cepstrum_medio_anterior_semifonema;
    cepstrum_mitad = difonema->cepstrum_izquierdo_inicio;
    cepstrum_final = difonema->cepstrum_izquierdo_final;


    numero_marcas = difonema->numero_marcas_izquierda;

    if (numero_marcas == 0) {
      marca_inventada = 1;
      numero_marcas = 1;
      marcas_pitch = difonema->marcas_derecha;
      anterior_pitch = 160;
      ultimo_pitch = 160;
    } // if (numero_marcas == 0)
    else {
      marcas_pitch = difonema->marcas_izquierda;
      if (numero_marcas > 1) {
	anterior_pitch = marcas_pitch[1] - marcas_pitch[0];
	ultimo_pitch = marcas_pitch[numero_marcas - 1] -
	  marcas_pitch[numero_marcas - 2];
      } // if (numero_marcas > 1)
      else {
	anterior_pitch = 160;
	ultimo_pitch = 160;
      } // else
    } // else
  } // else // DERECHO

  if (semifonema[0] == '#')
    acento = ATONA;
  else
    acento = TONICA;

  if (tipo_fon_espectral_enumerado(semifonema, (clase_espectral *) &tipo_espectral))
    return 1;

  if (tipo_de_fonema_duracion(semifonema, (clase_duracion *) &tipo_duracion))
    return 1;

  return 0;

}


/**
 * Función:   crea_semifonema_candidato
 * Clase:     Vector_semifonema_candidato
 * \remarks Entrada:
 *			- candidato: vector_descriptor_candidato que contiene la información que
 *            se desea copiar.                                                        
 *          - izq_der: indica si la unidad es por la izquierda o por la derecha.      
 * \remarks Valor devuelto:                                                                    
 *          - En ausencia de error, devuelve un cero.                                 
 */

int Vector_semifonema_candidato::crea_semifonema_candidato(Vector_descriptor_candidato *candidato, char izq_der) {
                                                           
  int contador;
  int origen;
  float *parametro_cepstrum;

  identificador = candidato->identificador;

  frontera_prosodica = candidato->frontera_prosodica;
    
  marca_inventada = 0;

  sonoridad = (char) sordo_sonoro(candidato->semifonema);
    
  strcpy(semifonema, candidato->semifonema);
  strcpy(contexto_izquierdo, candidato->contexto_izquierdo);
  strcpy(contexto_izquierdo_2, candidato->contexto_izquierdo_2);
  strcpy(contexto_derecho, candidato->contexto_derecho);
  strcpy(contexto_derecho_2, candidato->contexto_derecho_2);

  izquierdo_derecho = izq_der;

  acento = candidato->acento;
  posicion = candidato->posicion;
  frase = candidato->frase;

  frontera_inicial = candidato->frontera_inicial;
  frontera_final = candidato->frontera_final;

  frontera_prosodica = candidato->frontera_prosodica;

  tipo_espectral = candidato->tipo_espectral;
  tipo_duracion = candidato->tipo_duracion;

  if (identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {

    if ( (palabra = (char *) malloc((strlen(candidato->palabra) + 1)*sizeof(char))) == NULL) {
      fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
      return 1;
    } // if ( (palabra = ...

    strcpy(palabra, candidato->palabra);

  } // if (identificador < PRIMER_IDENTIFICADOR_DIFONEMAS)
  else {

    if ( (palabra = (char *) malloc(sizeof(char))) == NULL) {
      fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
      return 1;
    } // if ( (palabra = ...

    *palabra = '\0';
        
  } // else
  indice_fichero = candidato->indice_fichero;

  if (izq_der == IZQUIERDO) 
    duracion = candidato->duracion_1;
  else
    duracion = candidato->duracion_2;

  if ( (cepstrum_inicial = (float *) malloc(3*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
    return 1;
  }

  cepstrum_mitad = cepstrum_inicial + numero_coeficientes_cepstrales;
  cepstrum_final = cepstrum_mitad + numero_coeficientes_cepstrales;

  if (izq_der == IZQUIERDO) {

    anterior_pitch = candidato->anterior_pitch;
        
    frecuencia_inicial = candidato->frecuencia_izquierda;
    if (frecuencia_inicial == 0)
      frecuencia_inicial = 100.0;

    frecuencia_final = candidato->frecuencia_mitad;
    if (frecuencia_final == 0)
      frecuencia_final = 100.0;

    potencia_inicial = candidato->potencia_inicial;
    potencia_final = candidato->potencia;

    parametro_cepstrum = (float *) candidato->cepstrum;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_inicial[contador] = *parametro_cepstrum++;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_mitad[contador] = *parametro_cepstrum++;

    parametro_cepstrum += numero_coeficientes_cepstrales;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_final[contador] = *parametro_cepstrum++;

    indice = candidato->indice_inicial;

    numero_marcas = candidato->numero_marcas_izquierda;

    if (numero_marcas > 1) {
      if ( (marcas_pitch = (int *) malloc(numero_marcas*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
	return 1;
      } // if ( (marcas_pitch = ...
    } // if (numero_marcas > 1)
    else
      if ( (marcas_pitch = (int *) malloc(2*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
	return 1;
      } // if ( (marcas_pitch = ...

    if (numero_marcas != 0) {

      origen = candidato->marcas_izquierda[0];
      indice_primera_marca = candidato->indice_fichero + origen*sizeof(short int);

      if (identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {

	for (contador = 0; contador < numero_marcas; contador++) {
	  marcas_pitch[contador] = candidato->marcas_izquierda[2*contador] - origen;
	} // for (contador = 0; ...

	if (candidato->numero_marcas_derecha)
	  ultimo_pitch = candidato->marcas_izquierda[2*numero_marcas] -
	    candidato->marcas_izquierda[2*numero_marcas - 2];
	else
	  ultimo_pitch = candidato->ultimo_pitch;
      } // if (identificador < PRIMER_IDENTIFICADOR_DIFONEMAS)
      else {

	for (contador = 0; contador < numero_marcas; contador++)
	  marcas_pitch[contador] = candidato->marcas_izquierda[contador] - origen;

	ultimo_pitch = candidato->ultimo_pitch;
                
      }  // else
    } // if (numero_marcas != 0)
    else { // Nos las inventamos. Añadimos una a boleo.
      // Intentamos tomarla a partir de la referencia de
      // la parte derecha.

      marca_inventada = 1;
      if (candidato->numero_marcas_derecha) {
	origen = candidato->marcas_derecha[0];
	indice_primera_marca = candidato->indice_fichero + origen*sizeof(short int);
      } // if (candidato->numero_marcas_derecha)
      else { // Tampoco hay marcas en la parte derecha. Nos inventamos la marca coincidiendo con el inicio
	// del semifonema.
	indice_primera_marca = candidato->indice_fichero + candidato->indice_inicial; // original->indice_fichero;
	//                printf("Añadimos dos marcas de silencio a la unidad %d.\n", identificador);
      } // else

      numero_marcas = 1;

      marcas_pitch[0] = 0;

      ultimo_pitch = candidato->ultimo_pitch;

    } // else

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    ultimo_pitch = candidato->ultimo_pitch;

    frecuencia_inicial = candidato->frecuencia_mitad;
    if (frecuencia_inicial == 0)
      frecuencia_inicial = 100.0;

    frecuencia_final = candidato->frecuencia_derecha;
    if (frecuencia_final == 0)
      frecuencia_final = 100.0;

    potencia_inicial = candidato->potencia;
    potencia_final = candidato->potencia_final;

    indice = candidato->indice_mitad;

    parametro_cepstrum = (float *) candidato->cepstrum;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_inicial[contador] = *parametro_cepstrum++;

    parametro_cepstrum += numero_coeficientes_cepstrales;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_mitad[contador] = *parametro_cepstrum++;
    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++)
      cepstrum_final[contador] = *parametro_cepstrum++;

    numero_marcas = candidato->numero_marcas_derecha;

    if (numero_marcas > 0) {
      if ( (marcas_pitch = (int *) malloc(numero_marcas*sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
	return 1;
      } // if ( (marcas_pitch = ...
    } // if (numero_marcas ...
    else
      if ( (marcas_pitch = (int *) malloc(sizeof(int)))
	   == NULL) {
	fprintf(stderr, "Error en crea_semifonema_candidato, al asignar memoria.\n");
	return 1;
      } // if ( (marcas_pitch = ...

    if (numero_marcas != 0) {

      origen = candidato->marcas_derecha[0];
      indice_primera_marca = candidato->indice_fichero + origen*sizeof(short int);

      if (identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {
	for (contador = 0; contador < numero_marcas; contador++) {
	  marcas_pitch[contador] = candidato->marcas_derecha[2*contador] - origen;
	} // for (contador = 0; ...

	if (candidato->numero_marcas_izquierda)
	  anterior_pitch = candidato->marcas_derecha[0] -
	    candidato->marcas_izquierda[2*(candidato->numero_marcas_izquierda - 1)];
	else
	  anterior_pitch = candidato->anterior_pitch;

      } // if (identificador < ...
      else {
	for (contador = 0; contador < numero_marcas; contador++)
	  marcas_pitch[contador] = candidato->marcas_derecha[contador] - origen;

	anterior_pitch = candidato->anterior_pitch;
                
      } // else



    } // if (numero_marcas != 0)
    else { // Nos las inventamos. Añadimos una a boleo.
      //	Intentamos tomarla de la parte izquierda.

      marca_inventada = 1;
      numero_marcas = 1;

      if (candidato->numero_marcas_izquierda) {
	if (candidato->numero_marcas_izquierda == 1)
	  origen = candidato->marcas_izquierda[0];
	else
	  origen = candidato->marcas_izquierda[candidato->numero_marcas_izquierda - 2];
	indice_primera_marca = candidato->indice_fichero + origen*sizeof(short int);
      } // if (candidato->numero_marcas_izquierda)
      else // Tampoco hay marcas en la parte izquierda. Rellenamos con silencio.
	indice_primera_marca = candidato->indice_fichero + candidato->indice_mitad;

      marcas_pitch[0] = 0;
      anterior_pitch = candidato->anterior_pitch;
    } // else

  } // else (DERECHO)

  return 0;

}


/**
 * Función:   copia_contenido                                                         
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada:
 *			- vector_original                                                         
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 en caso de error.                                         
 * \remarks Objetivo:  copiar las variables miembro de un vector descriptor a otro. No llega   
 *            con una simple igualación, debido a la presencia de punteros. Liberar   
 *            la memoria del vector original provocaría la pérdida de la información  
 *            del segundo.                                                            
 */

int Vector_semifonema_candidato::copia_contenido(Vector_semifonema_candidato vector_original) {

  int contador;

  *this = vector_original; // Copiamos de golpe toda la parte estática.

  // Asignamos memoria para las variables dinámicas:

  if ( (palabra = (char *) malloc((strlen(vector_original.palabra) + 1)*sizeof(char)))
       == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  } // if ( (palabra = ...

  strcpy(palabra, vector_original.palabra);

  if ( (marcas_pitch = (int *) malloc(2*numero_marcas*sizeof(int)))
       == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  } // if ( (marcas_pitch = ...

  for (contador = 0; contador < numero_marcas; contador++)
    marcas_pitch[contador] = vector_original.marcas_pitch[contador];

  if ( (cepstrum_inicial = (float *) malloc(3*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  }

  cepstrum_mitad = cepstrum_inicial + numero_coeficientes_cepstrales;
  cepstrum_final = cepstrum_mitad + numero_coeficientes_cepstrales;

  for (contador = 0; contador < 3*numero_coeficientes_cepstrales; contador++)
    cepstrum_inicial[contador] = vector_original.cepstrum_inicial[contador];

  return 0;

}


/**
 * Función:   copia_contenido                                                         
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada:
 *			- vector_original                                                         
 *          - izq_der: indica qué parte del difonema queremos copiar.                 
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 en caso de error.                                         
 * \remarks Objetivo:  copiar las variables miembro de un vector descriptor a otro. No llega   
 *            con una simple igualación, debido a la presencia de punteros. Liberar   
 *            la memoria del vector original provocaría la pérdida de la información  
 *            del segundo.
 */

int Vector_semifonema_candidato::copia_contenido(Vector_difonema *vector_original, char izq_der) {

  int contador;
  clase_espectral clase_esp;
  clase_duracion clase_dur;

  identificador = vector_original->identificador;
  marca_inventada = 0;

  acento = TONICA; // vector_original->acento;
  frase = ENUNCIATIVA; // vector_original->frase;
  posicion = vector_original->posicion;

  indice_fichero = vector_original->indice;
  indice = 0;

  if ( (palabra = (char *) malloc(1)) == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  }

  palabra[0] = '\0';

  *contexto_derecho_2 = 0;
  *contexto_izquierdo_2 = 0;

  if ( (cepstrum_inicial = (float *) malloc(3*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
    return 1;
  }

  cepstrum_mitad = cepstrum_inicial + numero_coeficientes_cepstrales;
  cepstrum_final = cepstrum_mitad + numero_coeficientes_cepstrales;

  if (izq_der == IZQUIERDO) {

    strcpy(semifonema, vector_original->fonema_izquierdo);
    strcpy(contexto_derecho, vector_original->fonema_derecho);
    contexto_izquierdo[0] = '\0';

    duracion = vector_original->duracion_izquierda;

    frecuencia_inicial = vector_original->frecuencia_central;
    frecuencia_final = vector_original->frecuencia_derecha;

    potencia_final = vector_original->potencia_derecha;
    potencia_inicial = vector_original->potencia_central;

    numero_marcas = vector_original->numero_marcas_derecha;

    if ( (marcas_pitch = (int *) malloc(numero_marcas*sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
      return 1;
    } // if ( (marcas_pitch = ...

    // OJO aquí al coger las marcas de la parte adecuada.

    int *recorre = vector_original->marcas_derecha;

    for (contador = 0; contador < numero_marcas; contador++)
      marcas_pitch[contador] = *recorre++;

    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++) {
      cepstrum_inicial[contador] = vector_original->cepstrum_derecho_inicio[contador];
      cepstrum_mitad[contador] = vector_original->cepstrum_derecho_final[contador];
      cepstrum_final[contador] = vector_original->cepstrum_medio_siguiente_semifonema[contador];
    } // for (contador = 0; ...

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    strcpy(semifonema, vector_original->fonema_derecho);
    strcpy(contexto_izquierdo, vector_original->fonema_izquierdo);
    contexto_derecho[0] = '\0';

    duracion = vector_original->duracion_derecha;

    frecuencia_inicial = vector_original->frecuencia_izquierda;
    frecuencia_final = vector_original->frecuencia_central;

    potencia_final = vector_original->potencia_central;
    potencia_inicial = vector_original->potencia_izquierda;

    numero_marcas = vector_original->numero_marcas_izquierda;

    // OJO aquí al coger las marcas de la parte adecuada (antes lo hacía al revés).

    int *recorre = vector_original->marcas_izquierda;

    if ( (marcas_pitch = (int *) malloc(numero_marcas*sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en copia_contenido, al asignar memoria.\n");
      return 1;
    }

    for (contador = 0; contador < numero_marcas; contador++)
      marcas_pitch[contador] = *recorre++;

    // Comprobar esto. CULIO
    for (contador = 0; contador < numero_coeficientes_cepstrales; contador++) {
      cepstrum_inicial[contador] = vector_original->cepstrum_medio_anterior_semifonema[contador];
      cepstrum_mitad[contador] = vector_original->cepstrum_izquierdo_inicio[contador];
      cepstrum_final[contador] = vector_original->cepstrum_izquierdo_final[contador];
    } // for (contador = 0; ...

  } // DERECHO

  if (tipo_fon_espectral_enumerado(semifonema, &clase_esp))
    return 1;

  tipo_espectral = (char) clase_esp;

  if (tipo_de_fonema_duracion(semifonema, &clase_dur))
    return 1;

  tipo_duracion = (char) clase_dur;

  validez = vector_original->validez;

  return 0;

}


/**
 * \brief Escribe las características del vector semifonema, comparándolas con las del objetivo que se buscaba
 * \param[in] salida fichero de texto en el que se escribe la información relevante a la unidad.                                                            
 * \param[in] objetivo Vector_descriptor_objetivo para el que se ha escogido dicha unidad
 * \param[in] excluye_mp_y_cepstrum Parámetro opcional. Si vale distinto de cero, no se escribe la información relativa a las marcas de pitch y los cepstrum
 * \return Actualmente siempre devuelve un 0.
 */

int Vector_semifonema_candidato::escribe_datos_txt(FILE *salida, const Vector_descriptor_objetivo &objetivo, int excluye_mp_y_cepstrum) {

  const char *nombre_clase;
  float *recorre_coeficientes;

  fprintf(salida, "***%d\n", identificador);

  fprintf(salida, "%s\t=>\t%s\n", semifonema, objetivo.semifonema);
  fprintf(salida, "%s %s\t=>\t%s %s\n", contexto_izquierdo_2, contexto_izquierdo, objetivo.contexto_izquierdo_2, objetivo.contexto_izquierdo);
  fprintf(salida, "%s %s\t=>\t%s %s\n", contexto_derecho, contexto_derecho_2, objetivo.contexto_derecho, objetivo.contexto_derecho_2);
  if (izquierdo_derecho == IZQUIERDO)
    fprintf(salida, "Izquierdo\n");
  else
    fprintf(salida, "Derecho\n");

  if (acento) {
    fprintf(salida, "Sílaba tónica\t=>\t");
    if (objetivo.acento)
      fprintf(salida, "tónica\n");
    else
      fprintf(salida, "átona\n");
  }
  else {
    fprintf(salida, "Sílaba átona\t=>\t");
    if (objetivo.acento)
      fprintf(salida, "tónica\n");
    else
      fprintf(salida, "átona\n");
    
  }
  
  fprintf(salida, "%s\t=>\t%s\n", palabra, objetivo.palabra);

  fprintf(salida, "Frontera inicial: %d\t=> %d\n", frontera_inicial, objetivo.frontera_inicial);
  fprintf(salida, "Frontera final: %d\t=> %d\n", frontera_final, objetivo.frontera_final);
  fprintf(salida, "Frontera prosódica: %d\t=> %d\n", frontera_prosodica, objetivo.frontera_prosodica);


  if ( (nombre_clase = devuelve_cadena_alofono_espectral((clase_espectral) tipo_espectral)) == NULL)
    return 1;
  
  fprintf(salida, "Clase espectral: %s\n", nombre_clase);
  
  if ( (nombre_clase = devuelve_cadena_alofono_duracion((clase_duracion) tipo_duracion)) == NULL)
    return 1;
  
  fprintf(salida, "Clase duración: %s\n", nombre_clase);
  
  fprintf(salida, "Pitch anterior: %d\n", anterior_pitch);
  fprintf(salida, "Siguiente pitch: %d\n", ultimo_pitch);
  
  char *cadena_auxiliar, *cadena_objetivo;

  if (devuelve_tipo_frase_cadena(frase, &cadena_auxiliar))
    return 1;

  if (devuelve_tipo_frase_cadena(objetivo.frase, &cadena_objetivo))
    return 1;

  fprintf(salida, "%s => %s\n", cadena_auxiliar, cadena_objetivo);
  
  if (devuelve_tipo_posicion_cadena(posicion, &cadena_auxiliar))
    return 1;

  if (devuelve_tipo_posicion_cadena(objetivo.posicion, &cadena_objetivo))
    return 1;

  fprintf(salida, "%s => %s\n", cadena_auxiliar, cadena_objetivo);

  //   if (frontera_prosodica == UNIDAD_FIN_FRASE)
  //     fprintf(salida, "Final de frase.\n");
  //   else
  //     if (frontera_prosodica == UNIDAD_RUPTURA) 
  //       fprintf(salida, "Ruptura entonativa o ruptura coma.\n");
  //     else
  //       fprintf(salida, "No final de frase.\n");
    
  if (izquierdo_derecho == IZQUIERDO) {
    fprintf(salida, "%f\t=>\t%f\n", duracion, objetivo.duracion_1);
    fprintf(salida, "%f %f\t=>\t%f %f\n", frecuencia_inicial, frecuencia_final, objetivo.frecuencia_izquierda, objetivo.frecuencia_mitad);
    
  }
  else {
    fprintf(salida, "%f\t=>\t%f\n", duracion, objetivo.duracion_2);
    fprintf(salida, "%f %f\t=>\t%f %f\n", frecuencia_inicial, frecuencia_final, objetivo.frecuencia_mitad, objetivo.frecuencia_derecha);
  }
  
  fprintf(salida, "%f %f\t=>\t%f\n", potencia_inicial, potencia_final, objetivo.potencia);

  fprintf(salida, "%d %d\n\n", indice_fichero, indice);

  if (excluye_mp_y_cepstrum)
    return 0;

  int *correcaminos = marcas_pitch;
  int i;

  fprintf(salida, "Marcas de pitch: %d\n", numero_marcas);
  for (i = 0; i < numero_marcas; i++) {
    fprintf(salida, "%d ", (int) *correcaminos);
    correcaminos++;
  }

  fprintf(salida, "\n\nCoeficientes cepstrales:\n");

  fprintf(salida, "\nCoeficientes cepstrales iniciales:\n");

  recorre_coeficientes = cepstrum_inicial;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(salida, "%f ", *recorre_coeficientes);
  }

  //  fprintf(salida, "\n");

  fprintf(salida, "\nCoeficientes cepstrales a la mitad:\n");

  recorre_coeficientes = cepstrum_mitad;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(salida, "%f ", *recorre_coeficientes);
  }

  fprintf(salida, "\nCoeficientes cepstrales finales:\n");

  recorre_coeficientes = cepstrum_final;

  for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
    fprintf(salida, "%f ", *recorre_coeficientes);
  }
  fprintf(salida, "\n");

  fprintf(salida, "\n\n");

  return 0;

}

/**
 * \brief Escribe las características del vector semifonema, comparándolas con las del objetivo que se buscaba
 * \param[in] salida fichero de texto en el que se escribe la información relevante a la unidad.                                                            
 * \param[in] excluye_mp_y_cepstrum Parámetro opcional. Si vale distinto de cero, no se escribe la información relativa a las marcas de pitch y los cepstrum
 * \return Actualmente siempre devuelve un 0.
 */

int Vector_semifonema_candidato::escribe_datos_txt(FILE *salida, int excluye_mp_y_cepstrum) {

  // Identificador
  fprintf(salida, "%d\t", identificador);

  // Fonemas
  fprintf(salida, "%s\t%s\t%s\t%s\t%s\t", semifonema, contexto_izquierdo, contexto_derecho, contexto_izquierdo_2, contexto_derecho_2);

  // Izquierdo o derecho
  if (izquierdo_derecho == IZQUIERDO)
    fprintf(salida, "I\t");
  else
    fprintf(salida, "D\t");

  // Acento
  if (acento)
    fprintf(salida, "T\t");
  else
    fprintf(salida, "A\t");

  // Frontera Inicial, final y prosódica:
  fprintf(salida, "%d\t%d\t%d\t", frontera_inicial, frontera_final, frontera_prosodica);

  // Frase:
  switch (frase) {
  case FRASE_ENUNCIATIVA:
    fprintf(salida, "EN\t");
    break;
  case FRASE_EXCLAMATIVA:
    fprintf(salida, "EX\t");
    break;
  case FRASE_INTERROGATIVA:
    fprintf(salida, "IN\t");
    break;
  case FRASE_INACABADA:
    fprintf(salida, "SU\t");
    break;
  default:
    fprintf(stderr, "Error en escribe_datos_txt: Tipo de frase no contemplado.\n");
    return 1;
  }

  // F0
  fprintf(salida, "%.2f\t%.2f\t", frecuencia_inicial, frecuencia_final);

  // Duración
  fprintf(salida, "%.4f\t", duracion);

  // Energía
  fprintf(salida, "%.1f\t%.1f\n\n", potencia_inicial, potencia_final);
  
  /*

  const char *nombre_clase;
  float *recorre_coeficientes;

  fprintf(salida, "***%d\n", identificador);


  fprintf(salida, "%s\n", semifonema);
  fprintf(salida, "%s %s\n", contexto_izquierdo_2, contexto_izquierdo);
  fprintf(salida, "%s %s\n", contexto_derecho, contexto_derecho_2);

  if (izquierdo_derecho == IZQUIERDO)
    fprintf(salida, "Izquierdo\n");
  else
    fprintf(salida, "Derecho\n");

  if (acento)
    fprintf(salida, "Sílaba tónica\n");
  else
    fprintf(salida, "Sílaba átona\n");

  fprintf(salida, "%s\n", palabra);

  fprintf(salida, "Frontera inicial: %d\n", frontera_inicial);
  fprintf(salida, "Frontera final: %d\n", frontera_final);
  fprintf(salida, "Frontera prosódica: %d\n", frontera_prosodica);

  if ( (nombre_clase = devuelve_cadena_alofono_espectral((clase_espectral) tipo_espectral)) == NULL)
    return 1;

  fprintf(salida, "Clase espectral: %s\n", nombre_clase);

  if ( (nombre_clase = devuelve_cadena_alofono_duracion((clase_duracion) tipo_duracion)) == NULL)
    return 1;

  fprintf(salida, "Clase duración: %s\n", nombre_clase);

  fprintf(salida, "Pitch anterior: %d\n", anterior_pitch);
  fprintf(salida, "Siguiente pitch: %d\n", ultimo_pitch);

  switch (frase) {
  case FRASE_ENUNCIATIVA:
    fprintf(salida, "Frase enunciativa.\n");
    break;
  case FRASE_EXCLAMATIVA:
    fprintf(salida, "Frase exclamativa.\n");
    break;
  case FRASE_INTERROGATIVA:
    fprintf(salida, "Frase interrogativa.\n");
    break;
  case FRASE_INACABADA:
    fprintf(salida, "Frase inacabada.\n");
    break;
  default:
    fprintf(stderr, "Error en escribe_datos_txt: Tipo de frase no contemplado.\n");
    return 1;
  }
  switch (posicion) {
  case POSICION_INICIAL:
    fprintf(salida, "Posición inicial.\n");
    break;
  case POSICION_MEDIA:
    fprintf(salida, "Posición media.\n");
    break;
  case POSICION_FINAL:
    fprintf(salida, "Posición final.\n");
    break;
  default:
    fprintf(stderr, "Error en escribe_datos_txt: Posición del semifonema errónea.\n");
    return 1;
  }


  //   if (frontera_prosodica == UNIDAD_FIN_FRASE)
  //     fprintf(salida, "Final de frase.\n");
  //   else
  //     if (frontera_prosodica == UNIDAD_RUPTURA) 
  //       fprintf(salida, "Ruptura entonativa o ruptura coma.\n");
  //     else
  //       fprintf(salida, "No final de frase.\n");
    
  fprintf(salida, "%f\n", duracion);
  fprintf(salida, "%f %f\n", frecuencia_inicial, frecuencia_final);
  fprintf(salida, "%f %f\n", potencia_inicial, potencia_final);

  fprintf(salida, "%d %d\n\n", indice_fichero, indice);

  if (excluye_mp_y_cepstrum == 0) {

    int *correcaminos = marcas_pitch;
    int i;

    fprintf(salida, "Marcas de pitch: %d\n", numero_marcas);
    for (i = 0; i < numero_marcas; i++) {
      fprintf(salida, "%d ", (int) *correcaminos);
      correcaminos++;
    }
    
    fprintf(salida, "\n\nCoeficientes cepstrales:\n");
    
    fprintf(salida, "\nCoeficientes cepstrales iniciales:\n");
    
    recorre_coeficientes = cepstrum_inicial;
    
    for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
      fprintf(salida, "%f ", *recorre_coeficientes);
    }
    
    //  fprintf(salida, "\n");
    
    fprintf(salida, "\nCoeficientes cepstrales a la mitad:\n");
    
    recorre_coeficientes = cepstrum_mitad;
    
    for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
      fprintf(salida, "%f ", *recorre_coeficientes);
    }
    
    fprintf(salida, "\nCoeficientes cepstrales finales:\n");
    
    recorre_coeficientes = cepstrum_final;
    
    for (int j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++) {
      fprintf(salida, "%f ", *recorre_coeficientes);
    }
    fprintf(salida, "\n");
    
  }

  fprintf(salida, "\n");

  */

  return 0;

}

/**
 * Función:   escribe_datos_clustering                                                
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero de texto en el que se escribe la información relevante  
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas para realizar  
 *			  el clustering de unidades.											  
 */

int Vector_semifonema_candidato::escribe_datos_clustering(FILE *salida) {

  int j;

  fprintf(salida, "%d\t", identificador);
  fprintf(salida, "%s\t", semifonema);
  fprintf(salida, "%s\t", contexto_izquierdo);
  fprintf(salida, "%s\t", contexto_derecho);

  fprintf(salida, "%d\t", (int) (izquierdo_derecho - '0'));
    
  fprintf(salida, "%d\t", (int) acento);

  fprintf(salida, "%d\t", (int) frontera_inicial);

  fprintf(salida, "%d\t", (int) frontera_final);

  fprintf(salida, "%d\t", (int) frontera_prosodica);

  fprintf(salida, "%d\t", (int) frase);

  fprintf(salida, "%d\t", (int) posicion);

  fprintf(salida, "%f\t", duracion);
  fprintf(salida, "%f\t%f\t", frecuencia_inicial, frecuencia_final);
  fprintf(salida, "%f\t%f\t", potencia_inicial, potencia_final);

  float *recorre_coeficientes = cepstrum_inicial;

  for (j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++)
    fprintf(salida, "%f\t", *recorre_coeficientes);


  recorre_coeficientes = cepstrum_mitad;

  for (j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++)
    fprintf(salida, "%f\t", *recorre_coeficientes);

  recorre_coeficientes = cepstrum_final;

  for (j = 0; j < numero_coeficientes_cepstrales; j++, recorre_coeficientes++)
    fprintf(salida, "%f\t", *recorre_coeficientes);

  fprintf(salida, "\n");
    
  return 0;

}


/**
 * Función:   escribe_datos_bin                                                       
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            que ocupe poco espacio en memoria.                                      
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Vector_semifonema_candidato::escribe_datos_bin(FILE *salida) {

  unsigned char tamano_char;


  // De golpe, la parte estática. (estamos escribiendo el valor de los punteros a las
  // partes dinámicas, pero merece la pena, con respecto a hacer múltiples fwrite
  // para cada variable y tener que actualizar esta función cada vez que se añade
  // una simple variable estática. El mantenimiento es mucho más sencillo.

  fwrite(this, sizeof(Vector_semifonema_candidato), 1, salida);

  // Luego, la dinámica:

  tamano_char = (unsigned char) (strlen(palabra) + 1);

  fwrite(&tamano_char, sizeof(char), 1, salida);

  fwrite(palabra, sizeof(char), tamano_char, salida);

  fwrite(marcas_pitch, sizeof(int), numero_marcas, salida);

  fwrite(cepstrum_inicial, sizeof(float), 3*numero_coeficientes_cepstrales, salida);
    
  return 0;

}


/**
 * Función:   lee_datos_bin                                                           
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada:
 *			- entrada: fichero binario en el que se encuentra la información de los   
 *            vectores semifonema.                                                    
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve un 1 al llegar a fin de fichero, y un 2 si hay error.          
 * \remarks Objetivo:  cargar en memoria la información de los vectores descriptor desde un
 *            fichero binario.
 */

int Vector_semifonema_candidato::lee_datos_bin(FILE *entrada) {

  unsigned char tamano_char;

  // Primero, la parte estática:

  fread(this, sizeof(Vector_semifonema_candidato), 1, entrada);

  if (feof(entrada))
    return 1;

  // Luego, la dinámica:

  fread(&tamano_char, sizeof(char), 1, entrada);

  if ( (palabra = (char *) malloc(tamano_char*sizeof(char)))
       == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  } // if ( (palabra

  fread(palabra, sizeof(char), tamano_char, entrada);

  if ( (marcas_pitch = (int *) malloc(numero_marcas*sizeof(int)))
       == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  } // if ( (marcas_pitch = ...

  fread(marcas_pitch, sizeof(int), numero_marcas, entrada);

  if ( (cepstrum_inicial = (float *) malloc(3*numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  }

  cepstrum_mitad = cepstrum_inicial + numero_coeficientes_cepstrales;
  cepstrum_final = cepstrum_mitad + numero_coeficientes_cepstrales;

  fread(cepstrum_inicial, sizeof(float), 3*numero_coeficientes_cepstrales, entrada);
    
  return 0;

}

/**
 * Función:   libera_memoria                                                          
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Objetivo:  Liberar la memoria reservada para las variables miembro.                
 */

void Vector_semifonema_candidato::libera_memoria() {

  if (palabra) {
    free(palabra);
    palabra = NULL;
  } // if (palabra)

  if (marcas_pitch) {
    free(marcas_pitch);
    marcas_pitch = NULL;
  } // if (marcas_pitch)

  if (cepstrum_inicial) {
    free(cepstrum_inicial);
    cepstrum_inicial = NULL;
  } // if (cepstrum_inicial)

}

/**
 * Función:   libera_memoria_menos_marcas_pitch
 * Clase:     Vector_semifonema_candidato
 * \remarks Objetivo:  Liberar la memoria reservada para las variables miembro.
 */

void Vector_semifonema_candidato::libera_memoria_menos_marcas_pitch() {

  if (palabra) {
    free(palabra);
    palabra = NULL;
  } // if (palabra)

  if (cepstrum_inicial) {
    free(cepstrum_inicial);
    cepstrum_inicial = NULL;
  } // if (cepstrum_inicial)

}

/**
 * Función:   escribe_datos_parte_dinamica                                            
 * Clase:     Vector_semifonema_candidato                                             
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  Escribir de forma rápida el contenido de un vector en un fichero.       
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Vector_semifonema_candidato::escribe_datos_parte_dinamica(FILE *salida) {

  fwrite(palabra, sizeof(char), strlen(palabra) + 1, salida);

  fwrite(marcas_pitch, sizeof(int), numero_marcas, salida);

  fwrite(cepstrum_inicial, sizeof(float), 3*numero_coeficientes_cepstrales, salida);

  return 0;

}


/**
 * Función:   lee_datos_parte_dinamica
 * Clase:     Vector_semifonema_candidato
 * Entrada y salida:
 *          - cadena: cadena de caracteres en la que se encuentra la información de
 *            la parte dinámica del objeto.
 * \remarks NOTA:      Se supone que la memoria está correctamente reservada. Si no, casca.
 */

void Vector_semifonema_candidato::lee_datos_parte_dinamica(char **cadena) {

  char *correcaminos = *cadena;
  int tamano = strlen(correcaminos);

  palabra = correcaminos;

  correcaminos += tamano + 1;

  marcas_pitch = (int *) correcaminos;

  correcaminos += sizeof(int)*numero_marcas;

  cepstrum_inicial = (float *) correcaminos;

  cepstrum_mitad = cepstrum_inicial + numero_coeficientes_cepstrales;

  cepstrum_final = cepstrum_mitad + numero_coeficientes_cepstrales;

  correcaminos += sizeof(float)*3*numero_coeficientes_cepstrales;	  

  *cadena = correcaminos;

}

/**
 * \brief	Función que lee los datos de la cabecera de un fichero binario.
 * \remarks	Lee las variables estáticas de la clase.
 * \param	entrada fichero binario
 */

void Vector_semifonema_candidato::lee_cabecera(FILE *entrada) {

  fread(&numero_coeficientes_cepstrales, sizeof(int), 1, entrada);

}


/**
 * \brief	Función que escribe los datos en la cabecera de un fichero binario.
 * \remarks	Escribe las variables estáticas de la clase.
 * \param	salida fichero binario
 */

void Vector_semifonema_candidato::escribe_cabecera(FILE *salida) {

  fwrite(&numero_coeficientes_cepstrales, sizeof(int), 1, salida);

}
