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

#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "frecuencia.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "info_estructuras.hpp"
#include "modos.hpp"


// cadena que almacena las muestras de una ventana empleada para el cálculo de la energía.
static float *ventana = NULL;
static int tamano_ventana = 0;

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

static int contraste[15][14]={{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 6,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 7,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 8,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 9,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,14}};

#endif

/**
 * Función:   inicia_ventana                                                            
 * \remarks Entrada:
 *			- tipo: tipo de ventana que se desea emplear. Actualmente sólo puede ser de 
 *            tipo hanning o hamming.                                                   
 *          - tamano: número de muestras de dicha ventana.                              
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error (si el tipo de ventana es el adecuado, y no hay      
 *            problemas de memoria), se devuelve un 0.                                  
 * \remarks Objetivo:  Crear una ventana del tipo y tamaño especificados.                        
 */

int inicia_ventana(const char *tipo, int tamano) {

  tamano_ventana = tamano;

  // Asignamos memoria a la ventana.

  if ( (ventana =  (float *) malloc(tamano*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en inicia_ventana, al asignar memoria.\n");
    return 1;
  }

  // Comprobamos el tipo de ventana que se desea emplear y la creamos.

  if (!strcmp(tipo, "hamming")) {
    hamming(ventana, tamano);
    return 0;
  }
  else
    if (!strcmp(tipo, "hanning")) {
      hanning(ventana, tamano);
      return 0;
    }

  fprintf(stderr, "Error en inicia_ventana: tipo de ventana (%s) no reconocido.\n",
	  tipo);

  return 1;
    
}


/**
 * Función:   libera_memoria_ventana                                                    
 */

void libera_memoria_ventana() {

  if (ventana != NULL) {
    free(ventana);
    ventana = NULL;
  }

}


/**
 * Función:   calcula_energía                                                           
 * \remarks Entrada:
 *			- vector: cadena cuya energía queremos calcular.                            
 *          - tamano: número de muestras (de la ventana y el vector.                    
 *	\remarks Salida:
 *			- salida: energía de la señal de entrada.                                   
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error (si la ventana ha sido inicializada), devuelve 0.    
 * \remarks Objetivo:  calcula la energía de la señal de entrada, previo enventanado.            
 */

int calcula_energia(short int *vector, int tamano, float *salida) {

  float acumulado = 0;
  short int *correcaminos = vector;
  float auxiliar;
  float *t_ventana = ventana;

  // Si la ventana está inicializada, calculamos la energía del fragmento de voz enventanado.

  if (ventana) {
    for (int contador = 0; contador < tamano; contador++) {
      auxiliar = (float) (*correcaminos++) * (*t_ventana++);
      acumulado += auxiliar*auxiliar;
    }

    *salida = (float) acumulado / tamano;

    return 0;
  }

  fprintf(stderr, "Error en calcula_energia: ventana no inicializada.\n");

  return 1;

}


/**
 * Función:   calcula_potencias                                                         
 * \remarks Entrada:
 *			- inicio: inicio temporal del fonema.                                       
 *          - final: final del fonema.                                                  
 *          - vector_muestras: cadena en la que se encuentran las muestras de sonido.   
 *          - cadena_marcas: cadena con las marcas de pitch.
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.					
 *	\remarks Salida:
 *			- vector: descriptor del fonema.                                            
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error se devuelve un 0.                                    
 * \remarks Objetivo:  Función principal del cálculo de energía. Se encarga de llamar a las tres 
 *            funciones secundarias, encapsulando así los detalles de implemtación.     
 */

int calcula_potencias(short int *vector_muestras, float inicio, float fin,
		      marca_struct *cadena_marcas,
		      int frecuencia_muestreo,
                      Vector_descriptor_candidato *vector) {

  int desplazamiento;
  float potencia_inicial, potencia_mitad, potencia_final;
  
  // Buscamos la primera marca de pitch del fonema:
  for (; cadena_marcas->etiqueta < inicio; cadena_marcas++);
  
  // Avanzamos el vector hasta el inicio_del fonema:

  desplazamiento = (int) (cadena_marcas->etiqueta*frecuencia_muestreo - tamano_ventana / 2);

  if (desplazamiento < 0)
    desplazamiento = 0;
    
  if (calcula_energia(vector_muestras + desplazamiento, tamano_ventana, &potencia_inicial))
    return 1;

  // Buscamos la marca hacia la mitad del fonema:
  for (; cadena_marcas->etiqueta < (inicio + fin) / 2; cadena_marcas++);
  
  desplazamiento = (int) (cadena_marcas->etiqueta*frecuencia_muestreo - tamano_ventana / 2);

  if (calcula_energia(vector_muestras + desplazamiento, tamano_ventana, &potencia_mitad))
    return 1;

  // La cadena del final:

  for (; cadena_marcas->etiqueta < fin; cadena_marcas++);

  desplazamiento = (int) (cadena_marcas->etiqueta*frecuencia_muestreo - tamano_ventana / 2);

  if (calcula_energia(vector_muestras + desplazamiento, tamano_ventana, &potencia_final))
    return 1;

  // Añadimos los datos al vector descriptor.

  vector->anhade_potencia(potencia_mitad, potencia_inicial, potencia_final);

  return 0;
    
}

/**
 * Función:   calcula_potencia_inicial                                                  
 * \remarks Entrada:
 *			- *vector: cadena en la que se encuentran las muestras de sonido.           
 *	\remarks Salida:
 *			- potencia_final: valor calculado.                                          
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error se devuelve un 0.                                    
 * \remarks Objetivo:  Función encargada del cálculo de la energía al inicio del fonema.         
 */

int calcula_potencia_inicial(short int *vector, float *potencia_inicial) {

  return calcula_energia(vector, tamano_ventana, potencia_inicial);

}


/**
 * Función:   calcula_potencia_mitad                                                    
 * \remarks Entrada:
 *			- inicio: inicio temporal del fonema.                                       
 *          - final: final del fonema.                                                  
 *          - vector: cadena con las muestras de sonido.                                
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.					
 *	\remarks Salida:
 *			- potencia_media: potencia en la mitad del fonema.                          
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error se devuelve un 0.                                    
 * \remarks Objetivo:  Función encargada del cálculo de la energía a la mitad del fonema.        
 * \remarks NOTA:      El vector de muestras de sonido debe estar situado al inicio del fonema.  
 */

int calcula_potencia_mitad(short int *vector, float *potencia_media, float inicio, float fin,
			   int frecuencia_muestreo) {

  short int *correcaminos = vector;
  int desplazamiento = (int) ( (fin - inicio)*frecuencia_muestreo / 2) - (tamano_ventana / 2);

  // Centramos la ventana en la mitad del fonema.

  if (desplazamiento > 0)
    correcaminos += desplazamiento;
        
  return calcula_energia(correcaminos, tamano_ventana, potencia_media);

}


/**
 * Función:   calcula_potencia_final                                                    
 * \remarks Entrada:
 *			- inicio: inicio temporal del fonema.                                       
 *          - final: final del fonema.                                                  
 *          - vector: cadena con las muestras de sonido.                                
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.					
 *	\remarks Salida:
 *			- potencia_final: potencia al final del fonema.                             
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error se devuelve un 0.                                    
 * \remarks Objetivo:  Función encargada del cálculo de la energía al final del fonema.          
 * \remarks NOTA:      El vector de muestras de sonido debe estar situado al inicio del fonema.  
 */

int calcula_potencia_final(short int *vector, float *potencia_final, float inicio, float fin,
			   int frecuencia_muestreo) {

  short int *correcaminos = vector;
  int resultado, desplazamiento;

  // Llevamos el vector al final del fonema, menos la longitud de la ventana.

  if ( (desplazamiento = (int) (((fin - inicio)*frecuencia_muestreo - tamano_ventana)) ) > 0)
    correcaminos += desplazamiento;

  resultado = calcula_energia(correcaminos, tamano_ventana, potencia_final);

  if (*potencia_final == 0)
    *potencia_final = MINIMA_ENERGIA_INF;

  return resultado;

}


/**
 * Función:   calcula_potencia                                                          
 * \remarks Entrada:
 *			- vector: cadena con las muestras de sonido.                                
 *	\remarks Salida:
 *			- potencia: potencia del fragmento de voz.                                  
 * \remarks Valor devuelto:                                                                      
 *          - En ausencia de error se devuelve un 0.                                    
 * \remarks NOTA:      Función similar a las anteriores, pero más genérica. Simplemente calcula  
 *            la potencia del fragmento de voz al que apunta la cadena vector.          
 */

int calcula_potencia(short int *vector, float *potencia) {

  int resultado = calcula_energia(vector, tamano_ventana, potencia);

  if (*potencia == 0)
    *potencia = MINIMA_ENERGIA_INF;

  return resultado;

}


/************************************************************************
   hamming:

   This function returns a hamming window of N points starting an finishing
   with a tail of "zeros" zeros. The energy is normalized to be 1.

***************************************************************************/

void hamming(float *window, int N) {

  float energy = 0.0, normal,*twindow;
  float dosPi = 2*PI;
  int i;
  twindow=(float *)window;

  for(i = 0; i < N; ++i){
    *twindow = 0.54 - 0.46*cos(dosPi*i/(N-1));
    energy = energy + (*twindow)*(*twindow);
    ++twindow;
  }

  normal = sqrt(energy);
  twindow = (float *) window;

  for(i = 0; i < N; ++i){
    *twindow = (*twindow) / normal;
    ++twindow;
  }
}

/*************************************************************************
  hanning:

  This function returns a Hanning window. The call variables are the same
  that in the hamming function.

***************************************************************************/

void hanning(float *window, int N) {

  /* Función que devuelve un puntero a una ventana
     hanning de N puntos.  */

  float *temp, energy, normal, *twindow;
  int i;

  twindow = (float *) window;
  temp = twindow;

  energy = 0.0;

  for(i = 0; i < N; ++i){
    *twindow = 0.5*(1-cos(2*PI*i/(N-1)));
    energy = energy + (*twindow)*(*twindow);
    ++twindow;
  }

  normal = sqrt(energy);
  twindow = temp;

  for(i = 0; i < N; ++i){
    *twindow = (*twindow) / normal;
    ++twindow;
  }
}

/**
 * Función:   carga_vector_muestras                                              
 * \remarks Entrada:
 *			- frecuencia_muestreo: frecuencia de muestreo del fichero de sonido. 
 *			- fichero: fichero en el que se encuentra la informacíón sobre la    
 *            evolución temporal de la forma de onda.                            
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.			 
 *	\remarks Salida:
 *			- vector: cadena en la que se van a almacenar dichas muestras.       
 *          - tamano: número de elementos de dicha cadena.                       
 *			- tiempo_final_sonido: duración temporal del fichero.				 
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un 0.                             
 * \remarks Objetivo:  Cargar en memoria el contenido del fichero de forma de onda, en    
 *            formato .RAW, para agilizar posteriormente los cálculos.           
 * \remarks NOTA:	- Cambio. Si está definido _FICHEROS_WAV (modos.hpp), se supone que  
 *			  el formato es wav. En caso contrario, datos en bruto.				 	
 */

int carga_vector_muestras(int frecuencia_muestreo, short int **vector,
			  int *tamano,  float *tiempo_final_sonido, FILE *fichero) {

  short int *apunta_muestras;
  int tamano_ventana = (int) (frecuencia_muestreo*TAMANO_VENTANA);

#ifdef _FICHEROS_WAV
  tCabeceraWav cabecera_wav;
#endif

  fseek(fichero, 0, SEEK_END);
  *tamano = ftell(fichero);

  rewind(fichero);

  *tiempo_final_sonido = (float) *tamano/(sizeof(short int)*frecuencia_muestreo);

#ifdef _FICHEROS_WAV
  *tamano -= sizeof(tCabeceraWav);
#endif

  // Reservamos más memoria de la debida, y añadimos ceros al final.
  // Así arreglamos casos de ficheros con una pausa final demasiado corta.

  //    if ( (*vector = (short int *) realloc(*vector, *tamano + TAMANO_VENTANA*sizeof(short int))) == NULL) {
  if ( (*vector = (short int *) realloc(*vector, *tamano + tamano_ventana*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error en carga_vector_muestras, al asignar memoria.\n");
    return 1;
  }

  *tamano /= sizeof(short int);

#ifdef _FICHEROS_WAV
  fread(&cabecera_wav, sizeof(tCabeceraWav), 1, fichero);
#endif
  fread(*vector, sizeof(short int), *tamano, fichero);
#ifdef _FICHEROS_WAV
  if (cabecera_wav.nChannels == 2) {
    short int * lee = *vector;
    short int * escribe = *vector;
    *tamano /= 2;
    for(int i = 0; i < *tamano; i++){
      *escribe++ = *lee;
      lee += 2;
    }
  }
#endif

  apunta_muestras = *vector + *tamano;

  //    for (int contador = 0; contador < TAMANO_VENTANA; contador++)
  for (int contador = 0; contador < tamano_ventana; contador++)
    *apunta_muestras++ = 0;

  // Rellanamos con 0 hasta el tamaño de la ventana.

  return 0;
}


/**
 * Función:   comprueba_validez_fichero_sonido                                             
 * \remarks Entrada:
 *			- tamano: número de muestras de sonido del fichero RAW.                        
 *          - tiempo_final_fonemas: última marca temporal del fichero de fonemas.          
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.					   
 *	\remarks Salida:
 *			- tiempo_final_sonido: duración total del fichero de sonido.                   
 * \remarks Valor devuelto:                                                                         
 *          - Se devuelve un 0 si la cadena de marcas de pitch es correcta.                
 * \remarks Objetivo: Nos evita tener que comprobar si hay muestras para todos los fonemas del      
 *           fichero. Simplemente comparamos la posición de la última muestra con la       
 *           última marca de fonema, y si es mayor la primera, está garantizado que no nos 
 *           vamos a salir de la zona de memoria reservada para la cadena de muestras.     
 */

int comprueba_validez_fichero_sonido(int tamano, float tiempo_final_fonemas,
				     int frecuencia_muestreo, float *tiempo_final_sonido) {

  // Comprobamos que tenemos valores de onda hasta más allá de la última etiqueta. (El 0.03 no es
  // más que un margen de seguridad. Se cumple en todos los ficheros que miré).

  *tiempo_final_sonido = (float) tamano / frecuencia_muestreo;

  if (tiempo_final_fonemas >= *tiempo_final_sonido ) {
    fprintf(stderr, "Error en comprueba_validez_fichero_sonido: el fichero de sonido no es correcto.\n");
    return 1;
  }

  return 0;

}


/**
 * Función:   calcula_energia_media_y_parte_estacionaria                                   
 * \remarks Entrada:
 *			- cadena_muestras: cadena con las muestras del fichero de sonido.              
 *			- numero_muestras: número de elementos de la cadena anterior.				   
 *			- cadena_fonemas: cadena con las etiquetas de los fonemas y sus instantes de   
 *			  inicio.																	   
 *  		- numero fonemas: número de elementos de la cadena anterior.				   
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.					   
 *	\remarks Salida:
 *			- energia_media: energía media del fichero, en unidades logarítmicas, 		   
 *			  calculada a partir de la energía en la zona estacionaria de cada fonema, 	   
 *			  obviando los silencios.													   
 *			- cadena_energia: cadena con la energía en la parte estacionaria de los 	   
 *			  fonemas.																	   
 * \remarks Valor devuelto:                                                                         
 *          - En ausencia de error, se devuelve un cero.								   
 * \remarks Objetivo:																			   
 *			- Calcular la energía media del fichero, así como la energía en las partes 	   
 *			  estacionarias de los fonemas que forman la frase, para el modelo de 		   
 *			  estimación de la energía.													   
 * \remarks NOTA:																				   
 *			- Se supone que la longitud real de la cadena de fonemas es 1 más que la que   
 *			  se indica en la variable numero_fonemas. En la última posición viene el 	   
 *			  instante final del último silencio.										   
 */

int calcula_energia_media_y_parte_estacionaria(short int *cadena_muestras, int numero_muestras, estructura_fonemas *cadena_fonemas, int numero_fonemas, marca_struct *cadena_marcas, int numero_marcas, int frecuencia_muestreo, float *energia_media, estructura_energia **cadena_energia) {

  int inicio_ventana_muestras, fonemas_no_silencio = 0;
  int tamano_ventana = (int) (frecuencia_muestreo*TAMANO_VENTANA);
  estructura_energia *apunta_energia;
  estructura_fonemas *apunta_fonema;
  float energia = 0.0, inicio_siguiente_fonema, inicio_ventana;
  char marca;

  if (ventana == NULL)
    //    	if (inicia_ventana("hamming", TAMANO_VENTANA))
    if (inicia_ventana("hamming", tamano_ventana))
      return 1;

  if ( (*cadena_energia = (estructura_energia *) malloc(numero_fonemas*sizeof(estructura_energia))) == NULL) {
    fprintf(stderr, "Error en calcula_energia_media_y_parte_estacionaria, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_energia = ...

  apunta_energia = *cadena_energia;

  for (int contador = 0; contador < numero_fonemas; contador++, cadena_fonemas++) {

    marca = cadena_fonemas->fonema[strlen(cadena_fonemas->fonema) - 1];

    if ( (marca == '*') || (marca == '-') || (marca == '+') ) // Fonema no existente.
      continue;

    apunta_fonema = cadena_fonemas + 1;

    do {
      marca = apunta_fonema->fonema[strlen(apunta_fonema->fonema) - 1];
      inicio_siguiente_fonema = apunta_fonema++->inicio;
    } while ( (marca == '*') || (marca == '+') || (marca == '-') );
        
    inicio_ventana = (cadena_fonemas->inicio + inicio_siguiente_fonema) / 2;

    // Buscamos la marca correspondiente a ese instante:
    for (; cadena_marcas->etiqueta < inicio_ventana; cadena_marcas++);

    //        inicio_ventana_muestras = (int) (inicio_ventana*frecuencia_muestreo - TAMANO_VENTANA/2);
    inicio_ventana_muestras = (int) (cadena_marcas->etiqueta*frecuencia_muestreo - tamano_ventana/2);

    //    	if (inicio_ventana_muestras + TAMANO_VENTANA > numero_muestras) {
    if (inicio_ventana_muestras + tamano_ventana > numero_muestras) {
      if (contador != numero_fonemas - 1) {
	fprintf(stderr, "Error en calcula_energia_media_y_parte_estacionaria: fichero de \
	            sonido incorrecto.\n");
	return 1;
      } // if (contador != numero_fonemas - 1)
      apunta_energia->energia = (apunta_energia - 1)->energia;
      strcpy(apunta_energia->fonema, cadena_fonemas->fonema);
      break;
    } // if (inicio_ventana_muestras + tamano_ventana ...

    calcula_energia(cadena_muestras + inicio_ventana_muestras,
		    //        				TAMANO_VENTANA, &(apunta_energia->energia));
		    tamano_ventana, &(apunta_energia->energia));

    strcpy(apunta_energia->fonema, cadena_fonemas->fonema);

    if (apunta_energia->fonema[strlen(apunta_energia->fonema) - 1] == '!')
      apunta_energia->fonema[strlen(apunta_energia->fonema) - 1] = '\0';

    if (cadena_fonemas->fonema[0] != '#') { // Si no es un silencio,
      energia += apunta_energia->energia;
      fonemas_no_silencio++;
    } // if (cadena_fonemas->fonema[0] != '#')

    if (apunta_energia->energia < MINIMA_ENERGIA_INF)
      apunta_energia->energia = MINIMA_ENERGIA_INF;
            
    apunta_energia->energia = 10*log10(apunta_energia->energia);
    apunta_energia++;
  } // for (int contador = 0; ...

  if (fonemas_no_silencio)
    *energia_media = 10*log10(energia / fonemas_no_silencio);
  else {
    fprintf(stderr, "Error en calcula_energia_media_y_parte_estacionaria: el fichero \
        de entrada sólo contiene silencios.\n");
    return 1;
  } // else

  return 0;

}


/**
 * Función:   escribe_datos_modelo_energia				                                   
 * \remarks Entrada:
 *			- datos: estructura en la que se encuentran los datos con los que se pretende  
 *			  entrenar el modelo de estimación de la energía basado en regresión lineal.   
 *			- fichero: fichero en el que se escribe la información.						   
 *			- opcion_salida: Si vale 0: nombre de las clases.							   
 *							 En otro caso: valor númerico de la clase.					   
 * \remarks Valor devuelto:																		   
 *			- En ausencia de error devuelve un cero.									   
 * \remarks Objetivo:																			   
 *			- Escribir la información para entrenar el modelo de estimación de la energía. 
 */

int Energia::escribe_datos_modelo_energia(Parametros_modelo_energia *datos, FILE *fichero, int opcion_salida) {

  const char *apunta_nombre_clase;

  fprintf(fichero, "%f\t", datos->energia_fonema_anterior);
  fprintf(fichero, "%f\t", datos->energia_media_fichero);

    //    fprintf(fichero, "%s\t", datos->fonema);
  
  if ( (apunta_nombre_clase = devuelve_cadena_alofono_espectral(datos->tipo_fonema))
       == NULL)
    return 1;
  
  fprintf(fichero, "%s\t", apunta_nombre_clase);
  
  if ( (apunta_nombre_clase = devuelve_cadena_alofono_espectral(datos->tipo_fonema_anterior))
       == NULL)
    return 1;
  
  fprintf(fichero, "%s\t", apunta_nombre_clase);
  
  if ( (apunta_nombre_clase = devuelve_cadena_alofono_espectral(datos->tipo_fonema_siguiente))
       == NULL)
    return 1;
  
  fprintf(fichero, "%s\t", apunta_nombre_clase);

  if (opcion_salida == 0) {

    fprintf(fichero, "%d\t", datos->fonemas_palabra);

    fprintf(fichero, "%d\t", datos->silabas_palabra);
    
    fprintf(fichero, "%d\t", datos->silabas_inicio);
    
    fprintf(fichero, "%d\t", datos->silabas_final);

    fprintf(fichero, "%d\t", datos->silabas_frase);

    fprintf(fichero, "%c\t", datos->frontera_inicial + '0');

    fprintf(fichero, "%c\t", datos->frontera_final + '0');

    //    fprintf(fichero, "%c\t", datos->frontera_prosodica);

    fprintf(fichero, "%d\t", datos->final_frase);
    
    fprintf(fichero, "%c\t", datos->acento + '0');
    
    fprintf(fichero, "%c\t", datos->tipo_proposicion + '0');
    
    fprintf(fichero, "%c\t", datos->posicion + '0');

  }
  else {

    fprintf(fichero, "%c\t", datos->acento + '0');
    fprintf(fichero, "%d\t", datos->final_frase);
    fprintf(fichero, "%d\t", datos->frontera_prosodica);
    
  }

  fprintf(fichero, "%f\n", datos->energia);

  return 0;

}


/**
 * Función:   escribe_parametros_modelo_energia			                                   
 * \remarks Entrada:
 *			- fichero: fichero en el que se escriben los nombres de los parámetros del 	   
 *			  modelo de estimación de la energía.										   
 *			- opcion_salida: 0, si se desea escribir la cabecera del modelo basado en 	   
 *			  regresión lineal. 1, en el caso de redes neuronales.						   
 * \remarks Objetivo:																			   
 *			- Escribir la cabecera del fichero de datos de entrenamiento del modelo.	   
 */

void escribe_parametros_modelo_energia(FILE *fichero, int opcion_salida) {

  if (opcion_salida == 0)
    fprintf(fichero, "EnergiaAnt\tEnergiaMedia\tClaseFonema\tClaseAnterior\t\
ClaseSiguiente\tFonemasPalabra\tSilabasPalabra\tSilabasInicio\tSilabasFinal\t\
SilabasFrase\tFronteraInicial\tFronteraFinal\tFinalFrase\tAcento\tFrase\tPosicion\tEnergia\n");
  else
    /*
      fprintf(fichero, "EnergiaAnt\tEnergiaMedia\tClaseFonema\tClaseAnterior\t\
      ClaseSiguiente\tFonemasPalabra\tSilabasPalabra\tSilabasInicio\tSilabasFinal\t\
      SilabasFrase\tFronteraInicial\tFronteraFinal\tFinalFrase\tAcento\tFrase\tPosicion\tEnergia\n");
    */
    fprintf(fichero, "EnergiaAnt\tEnergiaMedia\tClaseFonema\t\
ClaseAnterior\tClaseSiguiente\tAcento\tFinalFrase\tFronteraProsodica\tEnergia\n");

}


/**
 * Función:   lee_modelo 								                                   
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero de texto en el que se encuentra la 	   
 *			  información del modelo de estimación de la energía basado en regresión 	   
 *		      lineal. 																	   
 * \remarks Valor devuelto:																		   
 *			- En ausencia de error, devuelve un cero.									   
 */

int Modelo_energia::lee_modelo(char *nombre_fichero) {

  int contador;
  char info_inutil[100];
  float *apunta_coeficientes;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
    fprintf(stderr, "Error en Modelo_energia::lee_modelo, al intentar abrir el \
        fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  fscanf(fichero, "%s %d\n", info_inutil, &maximo_silabas_inicio);
  fscanf(fichero, "%s %d\n", info_inutil, &maximo_silabas_final);

  fscanf(fichero, "%d\n", &numero_coeficientes);

  if ( (coeficientes = (float *) malloc(numero_coeficientes*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en Modelo_energia:lee_modelo, al asignar memoria.\n");
    return 1;
  } // if ( (coeficientes = ...

  apunta_coeficientes = coeficientes;

  for (contador = 0; contador < numero_coeficientes; contador++)
    fscanf(fichero, "%f ", apunta_coeficientes++);

  fscanf(fichero, "%d\n", &numero_variables);

  for (contador = 0; contador < numero_variables; contador++)
    fscanf(fichero, "%s\n", variables[contador]);

  fclose(fichero);
    
  return 0;

}

/**
 * Función:   libera_memoria							                                   
 * \remarks Objetivo:  liberar la memoria reservada para los campos dinámicos de la estructura.	   
 */

void Modelo_energia::libera_memoria() {

  if (coeficientes) {
    free(coeficientes);
    coeficientes = NULL;
  }

}


/**
 * Función:   estima_energia	                                                    
 * Entrada y Salida                                                                 
 *          - datos: estructura con la información de los predictores y en la que   
 *            se devuelve la energía estimada, en unidades logarítmicas.			
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, se devuelve un 0.                               
 */

#ifdef _MODO_NORMAL

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

int estima_energia(Parametros_modelo_energia *datos, struct Modelo_energia *modelo_temp) {

  //struct Modelo_energia *modelo_temp;
  int *apunta_contrastes;

  // Niveles asignados a Enunciativa, Exclamativa, Interrogativa, Inacabada
  int niv_tipoprop[NUMERO_TIPOS_PROPOSICIONES]={0,1,2,3};

  //	En distancia_espectral.hpp:
  //    typedef enum {SIL, VOCAB, VOCMED, VOCCERR, OCLUSOR, OCLUSON, APROXSON,
  //              FRICSOR, NAS, VIBR, LATER} clase_espectral;

  // En el modelo de energía, están ordenadas por orden alfabético:
  // #, Aproximante, fricativa, Lateral, Nasal, Oclusiva sonora, Oclusiva sorda,
  // Vibrante, Vocal abierta, Vocal cerrada, Vocal media.

  int equivalencia_clases[] = {0, 8, 10, 9, 6, 5, 1, 2, 4, 7, 3};

  float datos_x[TAMANO_VARIABLE_DATOS_ENERGIA];
  float *temp_x, energia_local = 0.0;
  int i,k,j,fila=0;

  //modelo_temp = &locutor_actual->modelo_energia;


  if (modelo_temp == NULL) {
    fprintf(stderr, "Error en estima_energia: modelo no cargado.\n");
    return 1;
  } // if (modelo_temp == NULL)


  temp_x=(float *) datos_x;
  *temp_x++=1; // El primer elemento de datos_x es siempre 1.

  if (datos->silabas_inicio > modelo_temp->maximo_silabas_inicio)
    datos->silabas_inicio = (short int) modelo_temp->maximo_silabas_inicio;

  if (datos->silabas_final > modelo_temp->maximo_silabas_final)
    datos->silabas_final = (short int) modelo_temp->maximo_silabas_final;

  // De momento no consideramos el predictor "Fonema".

  // El siguiente bucle comienza en i=1 para saltar (Intercept)
  for(i = 1;i < modelo_temp->numero_variables; ++i) {

    if (strcmp(modelo_temp->variables[i], "EnergiaAnt") == 0) {
      *temp_x++ = datos->energia_fonema_anterior;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "EnergiaAnt")

    if (strcmp(modelo_temp->variables[i], "EnergiaMedia") == 0) {
      *temp_x++ = datos->energia_media_fichero;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "EnergiaMedia") == 0)

    if (strcmp(modelo_temp->variables[i], "ClaseFonema") == 0) {
      apunta_contrastes = (int *) contraste[equivalencia_clases[datos->tipo_fonema]];
      for(j = 0;j < NUMERO_ELEMENTOS_CLASE_ESPECTRAL - 1; ++j)
	*temp_x++ = *apunta_contrastes++;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "ClaseFonema") == 0)

    if (strcmp(modelo_temp->variables[i], "ClaseSiguiente") == 0) {
      apunta_contrastes = (int *) contraste[equivalencia_clases[datos->tipo_fonema_siguiente]];
      for(j = 0;j < NUMERO_ELEMENTOS_CLASE_ESPECTRAL - 1; ++j)
	*temp_x++ = *apunta_contrastes++;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "ClaseSiguiente") == 0)

    if (strcmp(modelo_temp->variables[i], "ClaseAnterior") == 0) {
      apunta_contrastes = (int *) contraste[equivalencia_clases[datos->tipo_fonema_anterior]];
      for(j = 0;j < NUMERO_ELEMENTOS_CLASE_ESPECTRAL - 1; ++j)
	*temp_x++ = *apunta_contrastes++;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "ClaseAnterior") == 0)

    if (strcmp(modelo_temp->variables[i], "FonemasPalabra") == 0) {
      *temp_x++ = datos->fonemas_palabra;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "FonemasPalabra") == 0)

    if (strcmp(modelo_temp->variables[i], "SilabasPalabra") == 0) {
      *temp_x++ = datos->silabas_palabra;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "SilabasPalabra") == 0)

    if (strcmp(modelo_temp->variables[i], "SilabasInicio") == 0) {
      *temp_x++ = datos->silabas_inicio;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "SilabasInicio") == 0)

    if (strcmp(modelo_temp->variables[i], "SilabasFinal") == 0) {
      *temp_x++ = datos->silabas_final;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "SilabasFinal") == 0)

    if (strcmp(modelo_temp->variables[i], "SilabasFrase") == 0) {
      *temp_x++ = datos->silabas_frase;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "SilabasFrase") == 0)

    
    if (strcmp(modelo_temp->variables[i], "FronteraInicial") == 0) {
      if (datos->frontera_inicial == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "FronteraInicial") == 0)

    if (strcmp(modelo_temp->variables[i], "FronteraFinal") == 0) {
      if (datos->frontera_final == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "FronteraFinal") == 0)

    if (strcmp(modelo_temp->variables[i], "FronteraProsodica") == 0) {
      if (datos->frontera_prosodica == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "FronteraProsodica") == 0)

    if (strcmp(modelo_temp->variables[i], "Acento") == 0) {
      if (datos->acento == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
      continue;
    } // if (strcmp(modelo_temp->variables[i], "Acento") == 0)

    if (strcmp(modelo_temp->variables[i],"Frase") == 0) {
      for(j = 0;j < NUMERO_TIPOS_PROPOSICIONES; ++j){
	if (datos->tipo_proposicion == niv_tipoprop[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUMERO_TIPOS_PROPOSICIONES - 1; ++k)
	*temp_x++ = contraste[fila][k];
    } //  if (strcmp(modelo_temp->variables[i],"Frase") == 0) {

    fprintf(stderr, "Error en estima_energia: Variable no contemplada (%s).\n",
	    modelo_temp->variables[i]);
    return 1;

  } //end for

  temp_x = (float *) datos_x;

  float *correcaminos = modelo_temp->coeficientes;

  //Se realiza la multiplicación del vector  datos_x por los coef. del modelo.

  for(i = 1;i <= modelo_temp->numero_coeficientes; ++i)
    energia_local += (*temp_x++)*(*correcaminos++);

  //Como se modela log(Dur) se aplica la transformación inversa.

  datos->energia = energia_local;

  return 0;

}

#endif

#endif

/**
 * Función:   estima_energia_red_neuronal                                           
 * \remarks Entrada:	- red: red neuronal que modela la energía.								
 *			- estadisticos: medias y varianzas de los parámetros de entrada.		
 * Entrada y Salida                                                                 
 *          - datos: estructura con la información de los predictores y en la que   
 *            se devuelve la energía estimada, en unidades logarítmicas.			
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, se devuelve un 0.                               
 */

#ifdef _MODO_NORMAL

#ifdef _ESTIMA_ENERGIA_RED_NEURONAL

int estima_energia_red_neuronal(Red_neuronal *red, Media_varianza_energia *estadisticos, Parametros_modelo_energia *datos) {

  double datos_local[NUMERO_PREDICTORES_MODELO_ENERGIA], *apunta_datos;  
  double energia;
  Media_varianza *apunta_estadisticos;

  apunta_estadisticos = (Media_varianza *) estadisticos;
  apunta_datos = (double *) datos_local;

  *apunta_datos++ =  (datos->energia_fonema_anterior - apunta_estadisticos->media) /
    apunta_estadisticos->desviacion;
  apunta_estadisticos++;

  *apunta_datos++ = (datos->energia_media_fichero - apunta_estadisticos->media) /
    apunta_estadisticos->desviacion;
  apunta_estadisticos++;

  // *apunta_datos++ = (datos->tipo_fonema - apunta_estadisticos->media) /
  //   apunta_estadisticos->desviacion;
  // apunta_estadisticos++;

  *apunta_datos++ = (datos->tipo_fonema_anterior - apunta_estadisticos->media) /
    apunta_estadisticos->desviacion;
  apunta_estadisticos++;

  *apunta_datos++ = (datos->tipo_fonema_siguiente - apunta_estadisticos->media) /
    apunta_estadisticos->desviacion;
  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->fonemas_palabra - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;
  
  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->silabas_palabra - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++;

  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->silabas_inicio - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;
  
  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->silabas_final - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;

  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->silabas_frase - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;

  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->frontera_inicial - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;

  apunta_estadisticos++;
  
  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->frontera_final - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;
  /*
  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->frontera_prosodica - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;
    */

  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->final_frase - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;

  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos++ = (datos->acento - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos++ = 0;

  apunta_estadisticos++;

  *apunta_datos++ = (datos->tipo_proposicion - apunta_estadisticos->media) /
    apunta_estadisticos->desviacion;
  apunta_estadisticos++;

  if (apunta_estadisticos->desviacion)
    *apunta_datos = (datos->posicion - apunta_estadisticos->media) /
      apunta_estadisticos->desviacion;
  else
    *apunta_datos = 0;

  //    estadisticos++;

  red->simula_red(datos_local, &energia);

  datos->energia = energia;

  return 0;

}

#endif

#endif


/**
 * Función:   criterio_modificacion_energia                                     
 * \remarks Entrada:
 *			- objetivo: vector descriptor de la unidad objetivo.				
 *			- sonido_final: entero que indica si un sonido es el anterior al    
 *			  silencio de final de frase.										
 *	\remarks Salida:	- modificable: Si el fonema se encuentra en el grupo de los que se  
 *			  deben modificar en energía, vale 1, y 0 en otro caso.				
 * \remarks Valor devuelto:																
 *			- En ausencia de error, devuelve un cero.							
 */

int criterio_modificacion_energia(Vector_descriptor_objetivo &objetivo, int sonido_final, int *modificable) {

  clase_espectral clase;
  /*
   *modificable = 0;

   return 0;
  */
  // Cambio:

  if (tipo_fon_espectral_enumerado(objetivo.semifonema, &clase))
    return 1;

  if ( (clase == SILENCIO_ESP) || (clase == OCLU_SOR_ESP) || (clase == FRIC_SOR_ESP) ) {
    *modificable = 0;
    return 0;
  } // if ( (clase == SILENCIO_ESP ...

  if (sonido_final)
    if (objetivo.contexto_derecho[0] == '#') {
      *modificable = 0;
      return 0;
    } // if (objetivo.contexto_derecho[0] == ...

  *modificable = 1;

  return 0;

}







