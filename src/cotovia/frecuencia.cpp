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
 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "modos.hpp"
#include "fonemas.hpp"
#include "distancia_espectral.hpp"
#include "frecuencia.hpp"

/**
 * Función:   calcula_frecuencias                                                   
 * \remarks Entrada:
 *			- inicio: marca temporal de inicio del fonema.                          
 *          - fin: marca temporal de fin del fonema.                                
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.				
 *			- frecuencia_minima_locutor: mínimo valor de f0 admitido para ese		
 *			  locutor.																
 *			- frecuencia_maxima_locutor: máximo valor de f0 admitido para ese 		
 *			  locutor.																	
 *	\remarks Salida:
 *			- vector: descriptor de la unidad, en el que se van a almacenar los     
 *            datos relativos a la frecuencia y la duración.                        
 * \remarks Entrada y Salida:                                                                
 *          - *cadena: cadena con las marcas de pitch.                              
 * \remarks Objetivo:  Función principal para la estimación de las frecuencias de un fonema  
 *            Se encarga de llamar a tres funciones secundarias que son las que     
 *            realmente hacen el cálculo. Así ocultamos los detalles de             
 *            implementación al programa llamante.                                  
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error se devuelve un 0. Si hay situación anómala,      
 *            devuelve un 1. En otro caso, devuelve un -1.                          
 * \remarks NOTA:      En el caso de encontrarse alguna situación anómala (descrita en las   
 *            siguientes funciones), se escribe la información relativa a la unidad 
 *            que la produjo en el fichero "salida_anomala".                        
 */

int calcula_frecuencias(marca_struct *cadena, float inicio, float fin,
			int frecuencia_muestreo, float frecuencia_minima_locutor,
			float frecuencia_maxima_locutor,
			Vector_descriptor_candidato *vector) {

  float frecuencia_inicial, frecuencia_final, frecuencia_mitad;
  float punto_medio;
  int caso_anomalo = 0;
  marca_struct *cadena_auxiliar = cadena;

  punto_medio = (fin + inicio) / 2;

  if (calcula_frecuencia_inicial(&cadena_auxiliar, &frecuencia_inicial, inicio, fin))
    caso_anomalo = 1;

  if (crea_cadena_pitch(cadena_auxiliar, inicio, fin, frecuencia_muestreo,
			(float) frecuencia_muestreo / frecuencia_maxima_locutor,
			(float) frecuencia_muestreo / frecuencia_minima_locutor, vector))
    return -1;

  if (calcula_frecuencia_mitad(cadena, &frecuencia_mitad, punto_medio))
    caso_anomalo = 1;

  if (calcula_frecuencia_final(cadena, &frecuencia_final, fin))
    caso_anomalo = 1;

  /*
   *    if ( sordo_sonoro(vector->semifonema) !=  sordo_sonoro(vector->contexto_izquierdo)) {
   *      if (vector->numero_marcas_izquierda > 1) {
   *        frecuencia_inicial = frecuencia_muestreo / (float)(vector->marcas_izquierda[2] - vector->marcas_izquierda[0]);
   *      }
   *    }
   *    else {
   *      frecuencia_inicial = frecuencia_muestreo / (float) vector->anterior_pitch;
   *    }
   *
   *    if ( sordo_sonoro(vector->semifonema) !=  sordo_sonoro(vector->contexto_derecho)) {
   *      if (vector->numero_marcas_derecha > 1){
   *        frecuencia_final = frecuencia_muestreo / (float) (vector->marcas_derecha[2*vector->numero_marcas_derecha - 2] - vector->marcas_derecha[2*vector->numero_marcas_derecha - 4]);
   *      }
   *    }
   *    else {
   *      frecuencia_final = frecuencia_muestreo / (float) vector->ultimo_pitch;
   *    }
   */

  frecuencia_inicial = frecuencia_muestreo / (float) vector->anterior_pitch;
  frecuencia_final = frecuencia_muestreo / (float) vector->ultimo_pitch;

  if (vector->numero_marcas_izquierda  && vector->numero_marcas_derecha) {
    frecuencia_mitad = frecuencia_muestreo / (float) (vector->marcas_derecha[0] - vector->marcas_izquierda[2*vector->numero_marcas_izquierda - 2]);
  }
  // Comprobamos si hay un posible error de etiquetado.

  if ( ((frecuencia_final - frecuencia_mitad) > MAXIMA_DIFERENCIA_HZ) ||
       ((frecuencia_mitad - frecuencia_inicial) > MAXIMA_DIFERENCIA_HZ) )
    caso_anomalo = 1;

  if ( (frecuencia_inicial > frecuencia_maxima_locutor) ||
       (frecuencia_inicial < frecuencia_minima_locutor) ) {
    frecuencia_inicial = FRECUENCIA_POR_DEFECTO;
    caso_anomalo = 1;
  } // if ( (frecuencia_inicial ...

  if ( (frecuencia_mitad > frecuencia_maxima_locutor) ||
       (frecuencia_mitad < frecuencia_minima_locutor) ) {
    frecuencia_mitad = FRECUENCIA_POR_DEFECTO;
    caso_anomalo = 1;
  } // if ( (frecuencia_mitad ...

  if ( (frecuencia_final > frecuencia_maxima_locutor) ||
       (frecuencia_final < frecuencia_minima_locutor) ) {
    frecuencia_final = FRECUENCIA_POR_DEFECTO;
    caso_anomalo = 1;
  } // if ( (frecuencia_final ...

  vector->anhade_frecuencia(frecuencia_mitad, frecuencia_inicial, frecuencia_final);

  if (caso_anomalo)
    return 1;

  return 0;

}


/**
 * Función:   calcula_duracion                                                      
 * \remarks Entrada:
 *			- inicio: marca temporal de inicio del fonema.                          
 *          - fin: marca temporal de fin del fonema.                                
 *	\remarks Salida:
 *			- vector: descriptor de la unidad, en el que se van a almacenar los     
 *            datos relativos a la frecuencia y la duración.                        
 * \remarks Objetivo:  Función encargada del cálculo de la duración de una unidad.           
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error devuelve un 0. En caso de situación anómala, 1.  
 * \remarks NOTA:      En el caso de encontrarse alguna situación anómala (descrita en las   
 *            siguientes funciones), se escribe la información relativa a la unidad 
 *            que la produjo en el fichero "salida_anomala".                        
 *            Se calcula la duración del semifonema como la mitad del total.        
 */

int calcula_duracion(float inicio, float fin, int frecuencia_muestreo, Vector_descriptor_candidato *vector) {

  int caso_anomalo = 0;
  float duracion_1, duracion_2;

  // Consideramos la duración del semifonema exactamente como la mitad de la duración del
  // fonema, que es el dato que tenemos. Hay que tomar alguna decisión para el caso del último
  // silencio del fichero (no tenemos marca de fin de fonema).

  //    if ( (duracion_1 = (fin - inicio) / 2) < DURACION_MINIMA) {
  //       caso_anomalo = 1;
  // 	 }
  // 
  //    // ABU: modifica aquí...
  //    duracion_2 = duracion_1;

  if (vector->numero_marcas_izquierda > 1) {
    duracion_1 = (float)(vector->marcas_izquierda[2 * vector->numero_marcas_izquierda - 2] - vector->marcas_izquierda[0]) / frecuencia_muestreo;
  }
  else if (vector->numero_marcas_izquierda == 1 && vector->numero_marcas_derecha) {
    duracion_1 =  (float)(vector->marcas_derecha[0] - vector->marcas_izquierda[0]) / frecuencia_muestreo;
  }
  else {
    duracion_1 = (fin - inicio) / 2;
  }
  if (vector->numero_marcas_derecha > 1) {
    duracion_2 =  (float)(vector->marcas_derecha[2 * vector->numero_marcas_derecha - 2] - vector->marcas_derecha[0]) / frecuencia_muestreo;
  }
  else if (vector->numero_marcas_derecha == 1 && vector->numero_marcas_izquierda) {
    duracion_2 =  (float)(vector->marcas_derecha[0] - vector->marcas_izquierda[0]) / frecuencia_muestreo;
  }
  else {
    duracion_2 = (fin - inicio) / 2;
  } 

  if (duracion_1 < DURACION_MINIMA || duracion_2 < DURACION_MINIMA) {
    caso_anomalo = 1;
  }

  vector->anhade_duracion(duracion_1, duracion_2);
  /*
  // Si se ha producido alguna situación extraña, lo anotamos en el fichero de casos anómalos.
  if (caso_anomalo) {
  FILE *fichero_anomalo;
  if ( (fichero_anomalo = fopen("salida_anomala", "a")) == NULL) {
  puts("Error al intentar abrir el fichero de salidas anómalas.");
  return -1;
  }
  vector->escribe_datos_txt(fichero_anomalo);
  fclose(fichero_anomalo);
  }
  */

  return caso_anomalo;

}


/**
 * Función:   crea_cadena_pitch                                                     
 * \remarks Entrada:
 *			- cadena: vector con las marcas de pitch, apuntando ya el inicio del    
 *            fonema.                                                               
 *          - inicial: instante inicial de la unidad.                               
 *          - final: instante de finalización de la unidad.                         
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.				
 *			- periodo_minimo_locutor: mínimo período de pitch del locutor.			
 *			- periodo_maximo_locutor: máximo período de pitch del locutor.			
 *	\remarks Salida:
 *			- vector: vector descriptor en el que se almacenan las marcas de pitch. 
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error (al asignar memoria), se devuelve un 0.          
 * Función:
 *			- Añadir las marcas de pitch pertenecientes a la unidad.                
 * \remarks NOTA:      En este momento hay problemas debido a que algunos semifonemas sólo   
 *            contienen una marca de pitch. Para solucionarlo, añadiremos la marca  
 *            siguiente al límite del fonema, aunque se encuentre fuera. Para el    
 *            semifonema por la izquierda, llega con tomar la 1ª marca de la derecha
 *            ya que están consecutivas en memoria.                                 
 * \remarks NOTA2:     Actualmente ya no se añade una marca más, dado que en ocasiones       
 *            provocaba errores, (Por ejemplo, se podía repetir la oclusión en una  
 *            oclusiva.                                                             
 */

// PREGUNTAR A EDU QUÉ HAGO CON LAS MARCAS QUE SE ENCUENTRAN EN LA FRONTERA.

int crea_cadena_pitch(marca_struct *cadena, float inicial, float final,
		      int frecuencia_muestreo, float periodo_minimo_locutor,
		      float periodo_maximo_locutor,
		      Vector_descriptor_candidato *vector) {

  // Aquí resultaría mejor emplear estructuras de tipo marca_struct en lugar de float *,
  // (podríamos hacer directamente un memcpy(), pero la compatibilidad con la programación
  // anterior nos lo impide.

  int *puntero_inicio, *correcaminos;
  float limite;
  short int marcas_derecha = 0, marcas_izquierda = 0;
  short int tamano_actual = 0, tamano_reservado = 1000;

  if ( (puntero_inicio = (int *) malloc(2*tamano_reservado*sizeof(int))) == NULL) {
    fprintf(stderr, "Error en crea_cadena_pitch, al asignar memoria.\n");
    return 1;
  }

  if (inicial < 0.02) {
    //vector->anterior_pitch = (int) (0.01*frecuencia_muestreo);
    vector->anterior_pitch = (int) (cadena->etiqueta *frecuencia_muestreo + 0.5);
  }
  else {
    //fmendez por redondeos vector->anterior_pitch = (int) ((cadena->etiqueta - (cadena - 1)->etiqueta)*frecuencia_muestreo);
    vector->anterior_pitch = (int) (cadena->etiqueta *frecuencia_muestreo + 0.5) - (int)((cadena - 1)->etiqueta * frecuencia_muestreo + 0.5);
  }
  correcaminos = puntero_inicio;

  limite = (inicial + final) / 2;

  // Evidentemente, esto sólo sirve para dos sonidos por unidad.

  for (int i = 0; i < 2; i++) {

    while ( (cadena->etiqueta < limite) && (cadena->etiqueta != -1.0) ) {
      //        *correcaminos++ = (int) (FS*cadena->etiqueta + 1);
      *correcaminos++ = (int) (frecuencia_muestreo*cadena->etiqueta + 0.5);
      if (cadena->sonoridad[0] == 'S')
	*correcaminos = 1;
      else
	*correcaminos = 0;

      if (++tamano_actual >= tamano_reservado) {
	tamano_reservado += (short int) 1000;
	if ( (puntero_inicio = (int *) realloc(puntero_inicio, 2*tamano_reservado*sizeof(int)))
	     == NULL) {
	  fprintf(stderr, "Error en crea_cadena_pitch, al asignar memoria.\n");
	  return 1;
	}
	correcaminos = puntero_inicio + 2*tamano_actual;

      }
      else
	correcaminos++;

      cadena++;
    }

    if (i == 0) {
      limite = final;
      marcas_izquierda = tamano_actual;
    }
    else {
      marcas_derecha = (short int) (tamano_actual - marcas_izquierda);
      /*
	if (marcas_derecha == 1) { // Añadimos una marca más.
	marcas_derecha = 2;
	*correcaminos++ = (int) (FS*cadena->etiqueta + 1);
	if (cadena->sonoridad[0] == 'S')
	*correcaminos = 1;
	else
	*correcaminos = 0;
	tamano_actual++;
	}
      */
    }
  }

  // Liberamos la memoria ocupada:

  if (tamano_actual) {
    if ( (puntero_inicio = (int *) realloc(puntero_inicio, 2*tamano_actual*sizeof(int)))
	 == NULL) {
      fprintf(stderr, "Error en crea_cadena_pitch, al asignar memoria.\n");
      return 1;
    }

    vector->anhade_marcas_pitch(marcas_izquierda, puntero_inicio, marcas_derecha,
				puntero_inicio + 2*marcas_izquierda);
  }
  else {
    vector->anhade_marcas_pitch(0, NULL, 0, NULL);
    free(puntero_inicio);
  }

  if (cadena->etiqueta == -1)
    vector->ultimo_pitch = (int)((cadena - 1)->etiqueta * frecuencia_muestreo + 0.5) - (int) ((cadena - 2)->etiqueta * frecuencia_muestreo + 0.5);
  else
    vector->ultimo_pitch = (int) (cadena->etiqueta * frecuencia_muestreo + 0.5) - (int) ((cadena - 1)->etiqueta * frecuencia_muestreo + 0.5);

  if (vector->ultimo_pitch > periodo_maximo_locutor)
    vector->ultimo_pitch = (int) periodo_maximo_locutor;
  if (vector->ultimo_pitch < periodo_minimo_locutor)
    vector->ultimo_pitch = (int) periodo_minimo_locutor;

  if (vector->anterior_pitch > periodo_maximo_locutor)
    vector->anterior_pitch = (int) periodo_maximo_locutor;
  if (vector->anterior_pitch < periodo_minimo_locutor)
    vector->anterior_pitch = (int) periodo_minimo_locutor;

  return 0;

}


/**
 * Función:   calcula_frecuencia_inicial                                            
 * \remarks Entrada:
 *			- tiempo_inicio: marca temporal de inicio del fonema.                   
 *          - tiempo_final: marca temporal de final del fonema.                     
 *	\remarks Salida:
 *			- frecuencia_inicial: estimación de la frecuencia al inicio del         
 *            semifonema.                                                           
 * \remarks Entrada y Salida:                                                                
 *          - *cadena: cadena con las marcas de pitch.                              
 * \remarks Valor devuelto:                                                                  
 *          - Si hay una situación anómala se devuelve un 1. En esta función se     
 *            considera como anómalo el caso en el que haya menos de dos marcas de  
 *            pitch en el fonema, o bien si, habiendo 2, el resultado obtenido es   
 *            mayor que FRECUENCIA_MAXIMA_LOCUTOR.                                  
 * \remarks Objetivo: Cálculo de la frecuencia al inicio del fonema.                         
 */

int calcula_frecuencia_inicial(marca_struct **cadena, float *frecuencia_inicial,
			       float tiempo_inicio, float tiempo_final) {

  register float marca1, marca2; //, marca3;
  register marca_struct *correcaminos = *cadena, *original = *cadena;

  while ( (correcaminos->etiqueta < tiempo_inicio) && (correcaminos->etiqueta != -1.0) )
    correcaminos++;

  if (correcaminos->etiqueta == -1.0) {
    fprintf(stderr, "Error en calcula_frecuencia_inicial: fichero de marcas incorrecto.\n");
    return 1;
  } // if (correcaminos->etiqueta == -1.0)

  // Avanzamos al inicio del fonema la propia cadena. (Sólo es útil para el caso de que
  // haya dos silencios consecutivos al inicio del fichero)

  *cadena = correcaminos;

  // Al avanzar así la cadena estamos perdiendo la memoria anterior, pero no es demasiado
  // importante, dado que esto sólo se va a emplear para entrenar el corpus, y no para el
  // funcionamiento normal.

  marca1 = (correcaminos++)->etiqueta;

  if (correcaminos->etiqueta == -1.0) { // Sólo hay una marca en el fonema.
    if (correcaminos != original) {
      *frecuencia_inicial = 1.0 / (marca1 - (correcaminos - 2)->etiqueta);
    }
    else {
      *frecuencia_inicial = FRECUENCIA_POR_DEFECTO;
      // Situación anómala -> la escribimos en el fichero de casos raros.
      return 1;
    } // else
  } // if (correcaminos->etiqueta == -1.0)

  marca2 = correcaminos->etiqueta;

  if ( (marca1 > tiempo_final) || (marca2 > tiempo_final) ) {
    *frecuencia_inicial = marca2 - marca1;
    // Situación anómala -> la escribimos en el fichero de casos raros.
    return 1;
  }

  *frecuencia_inicial = 1.0 / (marca2 - marca1);

  /*
    correcaminos++;
    if (correcaminos->etiqueta == -1.0) { // Sólo hay dos marcas:
    *frecuencia_inicial = 1.0 / (marca2 - marca1);
    } // if (correcaminos->etiqueta == -1.0)
    else {
    if ( (marca3 = correcaminos->etiqueta) > tiempo_final)
    *frecuencia_inicial = 1.0 / (marca2 - marca1);
    else
    *frecuencia_inicial = ( 1.0/(marca3 - marca2) + 1.0/(marca2 - marca1) ) / 2;
    } // else
  */
  return 0;

}


/**
 * Función:   calcula_frecuencia_mitad                                              
 * \remarks Entrada:
 *			- tiempo_medio: marca temporal a la mitad del fonema.                   
 *          - cadena: cadena con las marcas de pitch. ( Ver Nota.)                  
 *	\remarks Salida:
 *			- frecuencia_mitad: estimación de la frecuencia a la mitad del          
 *            semifonema.                                                           
 * \remarks Valor devuelto:                                                                  
 *          - Dado que calculamos la frecuencia a partir de sólo dos marcas de      
 *            consideramos como caso anómalo que el valor obtenido sea mayor que la 
 *            máxima frecuencia admisible para el locutor. (Si hubiese menos de dos 
 *            marcas ya avisaría la función anterior).                              
 *            Devuelve un 0 en el caso normal.
 * \remarks Objetivo:  Cálculo de la frecuencia a la mitad del fonema.                       
 * \remarks NOTA:    La cadena de marcas de pitch debe estar situada temporalmente al inicio 
 *          del fonema.                                                             
 */

int calcula_frecuencia_mitad(marca_struct *cadena, float *frecuencia_mitad, float tiempo_medio) {

  float primera = 0, segunda = 0;
  register marca_struct *correcaminos = cadena;

  // Podríamos  hacer la comprobación de si es sordo o sonoro ya al principio, pero lo hacemos en
  // el punto medio para evitar inconsistencias.

  while ( (segunda < tiempo_medio) && ( (correcaminos + 1)->etiqueta != -1.0) ) {
    primera = segunda;
    segunda = (correcaminos++)->etiqueta;
  }

  if (segunda)                   
    *frecuencia_mitad = 1.0/(segunda - primera);
  else
    if (correcaminos != cadena)
      *frecuencia_mitad = 1.0 / (primera - (correcaminos - 1)->etiqueta);
    else {
      *frecuencia_mitad = FRECUENCIA_POR_DEFECTO;
      return 1;
    } // else

  return 0;

}


/**
 * Función:   calcula_frecuencia_final                                              
 * \remarks Entrada:
 *			- tiempo_final: marca temporal de final del fonema.                     
 *	\remarks Salida:
 *			- frecuencia_final: estimación de la frecuencia al inicio del           
 *            semifonema.                                                           
 * \remarks Entrada y Salida:                                                                
 *          - *cadena: cadena con las marcas de pitch.                              
 * \remarks Valor devuelto:                                                                  
 *          - Si hay una situación anómala se devuelve un 1. En esta función se     
 *          considera como anómalo el caso en el que haya 2 ó 3 marcas de pitch.    
 * \remarks Objetivo:  Cálculo de la frecuencia al final del fonema.                         
 */

int calcula_frecuencia_final(marca_struct *cadena, float *frecuencia_final, float tiempo_final) {

  int numero_marcas = 0;
  register marca_struct *correcaminos = cadena;
  int final = 0;

  // Contamos el número de marcas hasta llegar al final.

  while ( (correcaminos->etiqueta < tiempo_final) && (correcaminos->etiqueta != -1.0) ){
    numero_marcas++;
    correcaminos++;
  }

  switch (numero_marcas) {

  case 0:
  default:
    if (correcaminos != cadena) {
      if (correcaminos > cadena + 1)
	*frecuencia_final = 1.0 / ( (correcaminos - 1)->etiqueta - (correcaminos - 2)->etiqueta);
      else
	*frecuencia_final = 1.0 / (correcaminos->etiqueta - (correcaminos - 1)->etiqueta);
    } // if (correcaminos != cadena)
    else {
      *frecuencia_final = FRECUENCIA_POR_DEFECTO;
      final = 1;
    } // else
    break;
    /*
      case 2:
      // Se trata de un caso anómalo.
      *frecuencia_final = 1.0 / ( (correcaminos - 1)->etiqueta - (correcaminos - 2)->etiqueta );
      final = 1;
      break;
      case 3:
      // Otro caso anómalo.
      *frecuencia_final = (1.0 / ( (correcaminos - 1)->etiqueta - (correcaminos - 2)->etiqueta )  +
      1.0 /( (correcaminos - 2)->etiqueta - (correcaminos - 3)->etiqueta) ) / 2;
      final = 1;
      break;
      default:
      *frecuencia_final = (1.0 / ( (correcaminos - 1)->etiqueta - (correcaminos - 2)->etiqueta ) +
      1.0 /( (correcaminos - 2)->etiqueta - (correcaminos - 3)->etiqueta) ) / 2;
    */
  }

  return final;

}


/**
 * Función:   comprueba_validez_fichero_marcas                                            
 * \remarks Entrada:
 *			- cadena: array en el que se encuentra la información de marcas de pitch.     
 *          - tamano: número de elementos de dicho array.                                 
 *          - tiempo_final: última marca temporal del fichero de fonemas.                 
 * \remarks Valor devuelto:                                                                        
 *          - Se devuelve un 0 si la cadena de marcas de pitch es correcta.               
 * \remarks Objetivo: Nos evita tener que comprobar si hay marcas de pitch para todos los fonemas  
 *           del fichero. Simplemente comparamos la última marca de pitch con la última   
 *           marca de fonema, y si es mayor la primera, está garantizado que no nos vamos 
 *           a salir de la zona de memoria reservada para *cadena.                        
 */

int comprueba_validez_fichero_marcas(marca_struct *cadena, int tamano, float tiempo_final) {

  register marca_struct *correcaminos;
  //  float etiqueta;
  //  char fonema[3];
  //    float duracion_silencio;

  // Avanzamos hasta el último nodo de marcas de pitch.

  correcaminos = cadena + tamano - 1;

  if (correcaminos->etiqueta <= tiempo_final)
    return 1;

  /*
    if ( (duracion_silencio = (correcaminos->etiqueta - etiqueta)) <= 0)
    return 1;

    // Si la duración es mayor que la que adoptamos por defecto, nos quedamos con la duración
    // por defecto.

    if (duracion_silencio > DURACION_POR_DEFECTO)
    *tiempo_final = etiqueta + DURACION_POR_DEFECTO;
    else
    // En caso contrario, nos quedamos con el tiempo que esté disponible.
    *tiempo_final = correcaminos->etiqueta - MINFLOAT;

    // Rebobinamos el fichero de fonemas para las sucesivas lecturas.
    */
  //    rewind(fichero);

  return 0;
}


/**
 * Función:   crea_cadena_fichero                                                         
 * \remarks Entrada:
 *			- fichero_marcas: fichero en el que se encuentra la información de marcas de  
 *            pitch.                                                                      
 *	\remarks Salida:
 *			- *cadena: array en el que se almacena el contenido de ese fichero.           
 *          - tamano_final: número de elementos del array (etiqueta y marca de sonoridad.)
 *          - tiempo_ultima_marca: última marca.                                          
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Cargar en memoria el contenido del fichero de marcas de pitch, para que no  
 *            sea necesario estar avanzando y rebobinando el propio fichero. Se supone    
 *            que esta función sólo se va a emplear con los ficheros pequeños que forman  
 *            parte del corpus, por lo que no debería haber problemas de memoria.         
 */

int crea_cadena_fichero(marca_struct **cadena, FILE *fichero_marcas, int *tamano_final,
			float *tiempo_ultima_marca) {

  int tamano = 100;
  int tamano_actual = 0;
  register marca_struct *correcaminos;
  float anterior = -1;

  // Asignamos memoria a la cadena de salida.

  if ( (*cadena = (marca_struct *) malloc(tamano*sizeof(marca_struct))) == NULL) {
    fprintf(stderr, "Error en crea_cadena_fichero, al asignar memoria.\n");
    return 1;
  }

  // Vamos cargando en memoria el contenido del fichero.

  correcaminos = *cadena;

  while (!feof(fichero_marcas)) {
    if (fscanf(fichero_marcas, "%f %s\n", &correcaminos->etiqueta,
	       correcaminos->sonoridad) == -1)
      break;

    if (correcaminos->etiqueta == anterior) {
      fprintf(stderr, "Problemas en crea_cadena_fichero: etiqueta repetida (%f).\n", anterior);
      getchar();
      return 1;
    }

    anterior = correcaminos->etiqueta;

    if (++tamano_actual >= tamano - 1)  { // El -1 es para garantizar que hay espacio
      // para la marca final.
      tamano += 100;
      if ( (*cadena = (marca_struct *) realloc(*cadena, tamano*sizeof(marca_struct))) == NULL) {
	fprintf(stderr, "Error en crea_cadena_fichero, al asignar memoria.\n");
	return 1;
      }
      correcaminos = *cadena + tamano_actual;
    }
    else
      correcaminos++;
  }

  // Añadimos un carácter de final de cadena:

  correcaminos->etiqueta = -1;

  // Liberamos la memoria que habíamos reservado en exceso.

  if ( (*cadena = (marca_struct *) realloc(*cadena, (tamano_actual + 1)*sizeof(marca_struct))) == NULL) {
    fprintf(stderr, "Error en crea_cadena_fichero, al asignar memoria.\n");
    return 1;
  }

  if (tiempo_ultima_marca)
    *tiempo_ultima_marca = (*cadena + tamano_actual - 1)->etiqueta;
  *tamano_final = tamano_actual;  // Aquí antes le restaba 1. No recuerdo por qué.

  return 0;

}


/**
 * Función:   carga_fichero_fonemas                                                       
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de inicio de los     
 *            fonemas.                                                                    
 *          - opcion: Si es 0, no se realiza ninguna modificación sobre los fonemas.      
 *                    Si es 1, sólo se aceptan los fonemas válidos según el código Sampa. 
 *                    Si es 2, también se eliminan las marcas de fonemas modificados.     
 *	\remarks Salida:
 *			- *cadena_fonemas: array en el que se almacena el contenido de ese fichero.   
 *          - numero_fonemas: número de fonemas del fichero.                              
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */


int carga_fichero_fonemas(estructura_fonemas **cadena_fonemas, FILE *fichero,
			  int *numero_fonemas, int opcion) {
  int tamano = 250;
  int tamano_actual = 0;
  register estructura_fonemas *correcaminos;

  // Asignamos memoria a la cadena de salida.
  if ( (*cadena_fonemas = (estructura_fonemas *) calloc(tamano,sizeof(estructura_fonemas)))
       == NULL) {
    fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
    return 1;
  }

  // Vamos cargando en memoria el contenido del fichero.

  correcaminos = *cadena_fonemas;

  while (!feof(fichero)) {
    fscanf(fichero, "%f %s\n", &correcaminos->inicio,
	   correcaminos->fonema);

    // Eliminamos las marcas de ruptura entonativa:
    if (strcmp(correcaminos->fonema, "#R") == 0)
      continue;

    if (opcion != 0) {

#ifndef _CONSIDERA_SEMIVOCALES

#ifdef _MODOA_EU
      if (correcaminos->fonema[0] == 'j' && correcaminos->fonema[1] != 'j')
#else
	if (correcaminos->fonema[0] == 'j' )
#endif
	  correcaminos->fonema[0] = 'i';

      if (correcaminos->fonema[0] == 'w')
	correcaminos->fonema[0] = 'u';

#endif
    } // if (opcion == 1)

    if (opcion == 2) {
      if (correcaminos->fonema[strlen(correcaminos->fonema) - 1] == '!')
	correcaminos->fonema[strlen(correcaminos->fonema) - 1] = '\0';
      if (strstr(correcaminos->fonema, "+") || strstr(correcaminos->fonema, "-") ||
	  strstr(correcaminos->fonema, "*") )
	continue;
    } //if (opcion == 2)

    if (++tamano_actual >= tamano)  {
      tamano += 1000;
      if ( (*cadena_fonemas = (estructura_fonemas *) realloc(*cadena_fonemas,
							     tamano*sizeof(estructura_fonemas))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
	return 1;
      }
      correcaminos = *cadena_fonemas + tamano_actual;
    }
    else
      correcaminos++;
  }

  if (tamano_actual >= tamano - 1) {
    tamano += 2;
    if ( (*cadena_fonemas = (estructura_fonemas *) realloc(*cadena_fonemas,
							   tamano*sizeof(estructura_fonemas))) == NULL) {
      fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
      return 1;
    }
#if !defined(_CALCULA_DISTANCIAS_CEPSTRALES) || defined(_SCRIPT)	\
  || defined(_GRUPOS_ACENTUALES)

    correcaminos = *cadena_fonemas + tamano_actual;
#endif
  }

  *numero_fonemas = tamano_actual;

  // Añadimos dos silencios al final (útil para la versión INFO_DURACIÓN, y no
  // influye en el resto.

#if !defined(_CALCULA_DISTANCIAS_CEPSTRALES) || defined(_SCRIPT)	\
  || defined(_GRUPOS_ACENTUALES)

  if ( (correcaminos - 1)->fonema[0] != '#') {
    strcpy(correcaminos->fonema, "#");
    correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
    correcaminos++;

    strcpy(correcaminos->fonema, "#");
    correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
    correcaminos++;

  }
  else {

    if ( ( ( (correcaminos - 1)->fonema[0] == '#') &&
	   ( (correcaminos - 2)->fonema[0] != '#')) ) {
      strcpy(correcaminos->fonema, "#");
      correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
      correcaminos++;
      (*numero_fonemas)++;
    }
  } // else

  strcpy(correcaminos->fonema, "#");
  correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
  (*numero_fonemas)++;

#endif

  // Liberamos la memoria que habíamos reservado en exceso.
  /*
    if ( (*cadena_fonemas = (estructura_fonemas *) realloc(*cadena_fonemas,
    tamano_actual*sizeof(estructura_fonemas))) == NULL) {
    puts("Error al asignar memoria.");
    return 1;
    }
  */ // Pasamos de liberar esta memoria, ya que en los modos en los que se usa es preferible la
  // velocidad al ahorro de memoria.

  return 0;

}

/**
 * Función:   carga_fichero_fonemas_sintetico                                             
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de inicio de los     
 *            fonemas.                                                                    
 *	\remarks Salida:
 *			- *cadena_fonemas: array en el que se almacena el contenido de ese fichero.   
 *          - numero_fonemas: número de fonemas del fichero.                              
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */


int carga_fichero_fonemas_sintetico(estructura_fonemas **cadena_fonemas, FILE *fichero,
				    int *numero_fonemas) {
  int tamano = 250;
  int tamano_actual = 0;
  register estructura_fonemas *correcaminos;

  // Asignamos memoria a la cadena de salida.
  if ( (*cadena_fonemas = (estructura_fonemas *) calloc(tamano,sizeof(estructura_fonemas)))
       == NULL) {
    fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
    return 1;
  }

  // Vamos cargando en memoria el contenido del fichero.

  correcaminos = *cadena_fonemas;

  while (!feof(fichero)) {
    fscanf(fichero, "%f %s\n", &correcaminos->inicio,
	   correcaminos->fonema);

    if (++tamano_actual >= tamano)  {
      tamano += 1000;
      if ( (*cadena_fonemas = (estructura_fonemas *) realloc(*cadena_fonemas,
							     tamano*sizeof(estructura_fonemas))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
	return 1;
      }
      correcaminos = *cadena_fonemas + tamano_actual;
    }
    else
      correcaminos++;
  }

  if (tamano_actual >= tamano + 2) {
    tamano += 2;
    if ( (*cadena_fonemas = (estructura_fonemas *) realloc(*cadena_fonemas,
							   tamano*sizeof(estructura_fonemas))) == NULL) {
      fprintf(stderr, "Error en carga_fichero_fonemas, al asignar memoria.\n");
      return 1;
    }
  }

  *numero_fonemas = tamano_actual;

  // Añadimos dos silencios al final (útil para la versión INFO_DURACIÓN, y no
  // influye en el resto.

#if !defined(_CALCULA_DISTANCIAS_CEPSTRALES) || defined(_SCRIPT)	\
  || defined(_GRUPOS_ACENTUALES)

  if ( (correcaminos - 1)->fonema[0] != '#') {
    strcpy(correcaminos->fonema, "#");
    correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
    correcaminos++;

    strcpy(correcaminos->fonema, "#");
    correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
    correcaminos++;

  }
  else {

    if ( ( ( (correcaminos - 1)->fonema[0] == '#') &&
	   ( (correcaminos - 2)->fonema[0] != '#')) ) {
      strcpy(correcaminos->fonema, "#");
      correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
      correcaminos++;
      (*numero_fonemas)++;
    }
  } // else

  strcpy(correcaminos->fonema, "#");
  correcaminos->inicio = (correcaminos - 1)->inicio + 0.5; // En realidad, no influye.
  (*numero_fonemas)++;

#endif

  return 0;

}


/**
 * Función:   calcula_frecuencia                                                    
 * \remarks Entrada:
 *			- *cadena: cadena con las marcas de pitch.                              
 *          - num_marcas: número de marcas a partir del cual se estima la           
 *            frecuencia. Puede ser 2 ó 3.                                          
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.				
 *	\remarks Salida:
 *			- frecuencia: valor estimado del pitch.                                 
 * \remarks Objetivo:  Estimar el valor del pitch a partir de las primeras muestras de la    
 *            cadena que se le pasa.                                                
 * \remarks NOTA:      Se supone que las marcas de pitch vienen en muestras, no en unidades  
 *            de tiempo.                                                            
 */

void calcula_frecuencia(int *cadena, int num_marcas, int frecuencia_muestreo,
			float *frecuencia) {

  if (num_marcas == 3)
    //        *frecuencia = FS*( 1.0/(cadena[4] - cadena[2]) + 1.0/(cadena[2] - cadena[0]) ) / 2;
    *frecuencia = frecuencia_muestreo*( 1.0/(cadena[2] - cadena[1]) + 1.0/(cadena[1] - cadena[0]) ) / 2;
  else
    //        *frecuencia = FS/(cadena[2] - cadena[0]);
    *frecuencia = frecuencia_muestreo/(cadena[1] - cadena[0]);

}
