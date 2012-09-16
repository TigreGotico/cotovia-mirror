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
#include <float.h>

#include "modos.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"

#ifdef _CALCULA_DISTANCIAS_CEPSTRALES
static Ventanas_cepstrum *vector_ventanas_cepstrum = NULL;
static Vector_distancias vector_distancia[MAXIMO_NUMERO_UNIDADES_GRUPO*MAXIMO_NUMERO_UNIDADES_GRUPO];
static int numero_distancias = 0;
#endif


/**
 * Función:   tipo_fon_espectral_cadena                                             
 * \remarks Entrada:
 *			- fon: representación gráfica de un alófono, según la clasificación del 
 *            modelo espectral.                                                     
 * \remarks Valor devuelto:                                                                  
 *          - El tipo de fonema, en formato de cadena.                              
 */

const char *tipo_fon_espectral_cadena(char *fon) {

  switch (strlen(fon)) {

  case 1:

    switch (fon[0]) {

    case 'a':
      return ("vocabier");
    case 'e':
    case 'o':
    case 'E':
    case 'O':
      return ("vocmed");
    case 'i':
    case 'u':
      return ("voccerr");
    case 'p':
    case 't':
    case 'k':
#ifdef _MODOA_EU
    case 'c':
#endif
      return ("oclusor");
    case 'b':
    case 'd':
    case 'g':
      return ("ocluson");
    case 'B':
    case 'D':
    case 'G':
      return ("aproximante");
    case 'l':
#ifdef _MODOA_EU
    case 'L': //euskera
#endif
    case 'Z':
      return ("lateral");
    case 'f':
    case 's':
    case 'S':
    case 'T':
    case 'x':
      return ("fricsor");
    case 'm':
    case 'n':
    case 'N':
    case 'J':
      return ("nasal");
    case 'r':
      return ("vibrante");
    case '#':
      return ("#");
    default:
      //fprintf(stderr, "Error en tipo_fon_espectral_cadena: Fonema (%s) no considerado en el modelo.\n", fon);
      return NULL;
    } // switch (fon[0])
  case 2:
#ifdef _MODOA_EU
    if ( (fon[0] == 's') && (fon[1] == '`') )
      return ("fricsor");
    if ( (fon[0] == 't') && (fon[1] == 'S' || fon[1] == 's') )
#else
      if ( (fon[0] == 't') && (fon[1] == 'S') )
#endif
	return ("oclusor");
    if ( (fon[0] == 'r') && (fon[1] == 'r') )
      return ("vibrante");
#ifdef _MODOA_EU
    if ( (fon[0] == 'j') && (fon[1] == 'j') )
      return ("lateral");
    if ( (fon[0] == 'g') && (fon[1] == 'j') )
      return ("lateral");
  case 3:
    if (!strcmp(fon,"ts`")){
      return ("oclusor");
    }
#endif
  default:
    //fprintf(stderr, "Error en tipo_fon_espectral_cadena: Fonema (%s) no considerado en el modelo.\n", fon);
    return NULL;
  } // switch (strlen(fon))

}


/**
 * Función:   tipo_fon_espectral_enumerado                                          
 * \remarks Entrada:
 *			- fon: representación gráfica de un alófono, según la clasificación del 
 *            modelo espectral.                                                     
 *	\remarks Salida:
 *			- tipo_espectral: tipo del fonema, según la clasificacion del modelo    
 *            espectral.                                                            
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un cero.                               
 */

int tipo_fon_espectral_enumerado(char *fon, clase_espectral *tipo_espectral) {

  switch (strlen(fon)) {

  case 1:

    switch (fon[0]) {

    case 'a':
      *tipo_espectral = VOC_AB_ESP;
      return 0;
    case 'e':
    case 'o':
    case 'E':
    case 'O':
      *tipo_espectral = VOC_MED_ESP;
    return 0;
    case 'i':
    case 'u':
      *tipo_espectral = VOC_CER_ESP;
    return 0;
#ifdef _CONSIDERA_SEMIVOCALES
    case 'j':
    case 'w':
      *tipo_espectral = SEMI_VOCAL_ESP;
      return 0;
#endif
    case 'p':
    case 't':
    case 'k':
#ifdef _MODOA_EU
    case 'c':
#endif
      *tipo_espectral = OCLU_SOR_ESP;
    return 0;
    case 'b':
    case 'd':
    case 'g':
      *tipo_espectral = OCLU_SON_ESP;
    return 0;
    case 'B':
    case 'D':
    case 'G':
      *tipo_espectral = APROX_SON_ESP;
    return 0;
    case 'l':
#ifdef _MODOA_EU
    case 'L': //euskera
#endif
    case 'Z':
      *tipo_espectral = LATERAL_ESP;
    return 0;
    case 'f':
    case 's':
    case 'S':
    case 'T':
    case 'x':
      *tipo_espectral = FRIC_SOR_ESP;
    return 0;
    case 'm':
    case 'n':
    case 'N':
    case 'J':
      *tipo_espectral = NASAL_ESP;
    return 0;
    case 'r':
      *tipo_espectral = VIBRANTE_ESP;
      return 0;
    case '#':
      *tipo_espectral = SILENCIO_ESP;
      return 0;
    default:
      //fprintf(stderr, "Error en tipo_fon_espectral: Fonema (%s) no considerado en el modelo.\n", fon);
      return 1;
    } // switch (fon[0])
  case 2:
#ifdef _MODOA_EU
    if ( (fon[0] == 's') && (fon[1] == '`') ){
      *tipo_espectral = FRIC_SOR_ESP;
      return 0;
    }
    if ( (fon[0] == 'j') && (fon[1] == 'j') ){
      *tipo_espectral = LATERAL_ESP;
      return 0;
    }
    if ( (fon[0] == 'g') && (fon[1] == 'j') ){
      *tipo_espectral = LATERAL_ESP;
      return 0;
    }
    if ( (fon[0] == 't') && (fon[1] == 'S' || fon[1] == 's')) 
#else
      if ( (fon[0] == 't') && (fon[1] == 'S' )) 
#endif
	{
	  *tipo_espectral = OCLU_SOR_ESP;
	  return 0;
	} // if ( (fon[0] == 't') ...
    if ( (fon[0] == 'r') && (fon[1] == 'r') ) {
      *tipo_espectral = VIBRANTE_ESP;
      return  0;
    } // if ( (fon[0] == 'r') ...
#ifdef _MODOA_EU		
  case 3:
    if (!strcmp(fon,"ts`")){
      *tipo_espectral = OCLU_SOR_ESP;
      return 0;
    }
#endif
  default:
    //fprintf(stderr, "Error en tipo_fon_espectral: Fonema (%s) no considerado en el modelo.\n", fon);
    return 1;
  } // switch (strlen(fon))

}


/**
 * Función:   devuelve_tipo_alofono_espectral                                       
 * \remarks Entrada:
 *			- nombre: nombre del fonema.                                            
 *	\remarks Salida:
 *			- clase: tipo del fonema, según la clasificación empleada para el       
 *            modelo espectral.                                                     
 */

int devuelve_tipo_alofono_espectral(char *nombre, clase_espectral *clase) {

  if (strcmp(nombre, "#") == 0)
    *clase = SILENCIO_ESP;
  else
    if (strcmp(nombre, "vocabier") == 0)
      *clase = VOC_AB_ESP;
    else
      if (strcmp(nombre, "vocmed") == 0)
	*clase = VOC_MED_ESP;
      else
	if (strcmp(nombre, "voccerr") == 0)
	  *clase = VOC_CER_ESP;
	else
	  if (strcmp(nombre, "ocluson") == 0)
	    *clase = OCLU_SON_ESP;
	  else
	    if (strcmp(nombre, "oclusor") == 0)
	      *clase = OCLU_SOR_ESP;
	    else
	      if (strcmp(nombre, "fricsor") == 0)
		*clase = FRIC_SOR_ESP;
	      else
		if (strcmp(nombre, "lateral") == 0)
		  *clase = LATERAL_ESP;
		else
		  if (strcmp(nombre, "nasal") == 0)
		    *clase = NASAL_ESP;
		  else
		    if (strcmp(nombre, "vibrante") == 0)
		      *clase = VIBRANTE_ESP;
		    else
		      if (strcmp(nombre, "aproximante") == 0)
			*clase = APROX_SON_ESP;
#ifdef _CONSIDERA_SEMIVOCALES
		      else
			if (strcmp(nombre, "semivocal") == 0)
			  *clase = SEMI_VOCAL_ESP;
#endif
		      else {
			fprintf(stderr, "Error en devuelve_tipo_alofono_espectral: clase no contemplada (%s).\n", nombre);
			return 1;
		      } // else
  return 0;

}


/**
 * Función:   devuelve_cadena_alofono_espectral                                     
 * \remarks Entrada:
 *			- clase: clase del fonema, según la clasificación empleada para el      
 *            modelo espectral.                                                     
 * \remarks Valor devuelto:                                                                  
 *          - Una cadena de caracteres que representa el nombre de la clase, y NULL 
 *            en caso de error.                                                     
 */

const char *devuelve_cadena_alofono_espectral(clase_espectral clase) {

  switch (clase) {

  case SILENCIO_ESP:
    return "#";
  case VOC_AB_ESP:
    return "vocabier";
  case VOC_MED_ESP:
    return "vocmed";
  case VOC_CER_ESP:
    return "voccerr";
  case OCLU_SON_ESP:
    return "ocluson";
  case OCLU_SOR_ESP:
    return "oclusor";
  case FRIC_SOR_ESP:
    return "fricsor";
  case LATERAL_ESP:
    return "lateral";
  case NASAL_ESP:
    return "nasal";
  case VIBRANTE_ESP:
    return "vibrante";
  case APROX_SON_ESP:
    return "aproximante";
  case SEMI_VOCAL_ESP:
    return "semivocal";
  default:
    fprintf(stderr, "Error en devuelve_cadena_alofono_espectral: Tipo (%d) no contemplado.\n",
	    (int) clase);
    return NULL;
  } // switch

}

/**
 * Función:   carga_fichero_coeficientes_espectrales                                      
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de la fase o la	  
 *			  envolvente del espectro.													  
 *			- numero_elementos_vector: número de elementos de cada vector.				  
 *	\remarks Salida:
 *			- *cadena_coeficientes: array en el que se almacena el contenido de ese       
 *			  fichero.  	  															  
 *          - numero_vectores: número de vectores del fichero.                            
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */

int carga_fichero_coeficientes_espectrales(float **cadena_coeficientes, FILE *fichero,
					   int numero_elementos_vector,
					   int *numero_vectores) {

  int tamano = 500, tamano_actual = 0;
  int contador;
  float *apunta_vector;
  float *apunta_parametro;


  // Asignamos memoria a la cadena de salida.

  if ( (*cadena_coeficientes = (float *) malloc(tamano*sizeof(float)))
       == NULL) {
    fprintf(stderr, "Error en carga_fichero_coeficientes_cepstrales, al asignar memoria.\n");
    return 1;
  }

  apunta_vector = *cadena_coeficientes;

  while (feof(fichero) == 0) {

    apunta_parametro = (float *) apunta_vector;

    for (contador = 0; contador < numero_elementos_vector;
	 contador++, apunta_parametro++) {
      if (feof(fichero))
	break;
      fscanf(fichero, "%f ", apunta_parametro);
    } // for (contador = 0; ...

    tamano_actual += numero_elementos_vector;

    if (tamano_actual > tamano - numero_elementos_vector) {
      tamano += 1000;
      if ( (*cadena_coeficientes = (float *) realloc(*cadena_coeficientes,
						     tamano*sizeof(float))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_coeficientes_cepstrales, \
al asignar memoria.\n");
	return 1;
      }
      apunta_vector = *cadena_coeficientes + tamano_actual;
    }
    else
      apunta_vector += numero_elementos_vector;

  } // while (feof(fichero) == 0)

  *numero_vectores = tamano_actual / numero_elementos_vector;

  return 0;

}

/**
 * Función:   carga_fichero_lsf						                                      
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de los coeficientes  
 *			  lsf.																		  
 *			- numero_elementos_vector: número de elementos de cada vector.				  
 *			- lee_tiempo: si vale cero, considera que los vectores se corresponden con    
 *			  ventanas centradas en múltiplos de 8 ms. En caso contrario, lee el tiempo   
 *			  del fichero.																  		
 *	\remarks Salida:
 *			- *cadena_coeficientes: array en el que se almacena el contenido de ese       
 *			  fichero.  	  															  
 *          - numero_vectores: número de vectores del fichero.                            
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */

int carga_fichero_lsf(Vector_lsf_tiempo **cadena_lsf, FILE *fichero,
		      int numero_elementos_vector, int *numero_vectores, char lee_tiempo) {

  int tamano = 500, tamano_actual = 0;
  int contador;
  float tiempo = 0.008;
  Vector_lsf_tiempo *apunta_vector;
  float *apunta_parametro;


  // Asignamos memoria a la cadena de salida.

  if ( (*cadena_lsf = (Vector_lsf_tiempo *) malloc(tamano*sizeof(Vector_lsf_tiempo)))
       == NULL) {
    fprintf(stderr, "Error en carga_fichero_lsf, al asignar memoria.\n");
    return 1;
  }

  apunta_vector = *cadena_lsf;

  while (feof(fichero) == 0) {

    if (lee_tiempo == 0) {
      apunta_vector->tiempo = tiempo;
      tiempo += 0.008;
    } // if (lee_tiempo == 0)
    else
      fscanf(fichero, "%f ", &apunta_vector->tiempo);

    apunta_parametro = (float *) apunta_vector->vector;

    for (contador = 0; contador < numero_elementos_vector;
	 contador++, apunta_parametro++) {
      if (feof(fichero))
	break;
      fscanf(fichero, "%f ", apunta_parametro);
    } // for (contador = 0; ...

    tamano_actual += 1;

    if (tamano_actual > tamano - 1) {
      tamano += 1000;
      if ( (*cadena_lsf = (Vector_lsf_tiempo *) realloc(*cadena_lsf,
							tamano*sizeof(Vector_lsf_tiempo))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_lsf, al asignar memoria.\n");
	return 1;
      }
      apunta_vector = *cadena_lsf + tamano_actual;
    }
    else
      apunta_vector += 1;

  } // while (feof(fichero) == 0)

  *numero_vectores = tamano_actual;

  return 0;

}

/**
 * Función:   escribe_fichero_cepstrum_bin                                                  
 * \remarks Entrada:
 *			- cadena_cepstrum: vector con la información de los vectores cepstrales y el  
 *			  tiempo.																	  
 *			- numero_vectores: número de vectores de la cadena anterior.
 *			- numero_coeficientes_cepstrales: número de coeficientes cepstrales
 *	\remarks Salida:
 *			- fichero: fichero de texto en el que se va a escribir la información de los
 *			  vectores cepstrales.
 * \remarks Valor devuelto:
 *          - En ausencia de error devuelve un 0.
 */

int escribe_fichero_cepstrum_bin(Vector_cepstrum_tiempo *cadena_cepstrum, int numero_vectores, int numero_coeficientes_cepstrales, FILE *fichero) {

  float *apunta_coeficiente;

  if (cadena_cepstrum == NULL) {
    fprintf(stderr, "Error en escribe_fichero_cepstrum_bin: cadena de entrada nula.\n");
    return 1;
  } // if (cadena_cepstrum == NULL)

  for (int contador = 0; contador < numero_vectores; contador++, cadena_cepstrum++) {

    fprintf(fichero, "%f\t", cadena_cepstrum->tiempo);
    apunta_coeficiente = (float *) cadena_cepstrum->vector;

    for (int cuenta = 0; cuenta < numero_coeficientes_cepstrales - 1;
	 cuenta++, apunta_coeficiente++)
      fprintf(fichero, "%f\t", *apunta_coeficiente);

    fprintf(fichero, "%f\n", *apunta_coeficiente);
        
  } // for (int contador = 0; ...


  return 0;

}


/**
 * Función:   carga_fichero_cepstrum_bin                                                  
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de los cepstrum.     
 *	\remarks Salida:
 *			- *cadena_cepstrum: array en el que se almacena el contenido de ese fichero.  
 *          - numero_vectores: número de vectores del fichero.
 *			- numero_coeficientes_cepstrum: número de coeficiences cepstrales.                      
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */

int carga_fichero_cepstrum_bin(Vector_cepstrum_tiempo **cadena_cepstrum, FILE *fichero, int *numero_vectores, int numero_coeficientes_cepstrum) {

  int tamano = 1000, tamano_actual = 0;
  char cabecera[TAMANO_CABECERA_HTK_MFCC];
  Vector_cepstrum_tiempo *apunta_vector;

  // Asignamos memoria a la cadena de salida.

  if ( (*cadena_cepstrum = (Vector_cepstrum_tiempo *) malloc(tamano*sizeof(Vector_cepstrum_tiempo)))
       == NULL) {
    fprintf(stderr, "Error en carga_fichero_cepstrum_bin, al asignar memoria.\n");
    return 1;
  }

  // Leemos la cabecera:

  fread(cabecera, sizeof(char), TAMANO_CABECERA_HTK_MFCC, fichero);

  apunta_vector = *cadena_cepstrum;

  while (feof(fichero) == 0) {

    fread(&apunta_vector->tiempo, sizeof(float), 1, fichero);

    if (feof(fichero))
      break;

    fread(apunta_vector->vector, sizeof(float), numero_coeficientes_cepstrum, fichero);

    tamano_actual++;

    if (tamano_actual >= tamano) {
      tamano += 1000;
      if ( (*cadena_cepstrum = (Vector_cepstrum_tiempo *) realloc(*cadena_cepstrum, tamano*sizeof(Vector_cepstrum_tiempo))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_cepstrum_bin, al asignar memoria.\n");
	return 1;
      }
      apunta_vector = *cadena_cepstrum + tamano_actual;
    }
    else
      apunta_vector++;

  } // while (feof(fichero) == 0)

  *numero_vectores = tamano_actual;

  return 0;

}


/**
 * Función:   carga_fichero_cepstrum_txt
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de los cepstrum.     
 *	\remarks Salida:
 *			- *cadena_cepstrum: array en el que se almacena el contenido de ese fichero.  
 *          - numero_vectores: número de vectores del fichero.
 *			- numero_coeficientes_cepstrum: número de coeficientes cepstrales.                            
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks NOTA:	  Se trata de una versión de carga_fichero_cepstrum_bin que carga los datos   
 *			  de un fichero de texto.													  
 */

int carga_fichero_cepstrum_txt(Vector_cepstrum_tiempo **cadena_cepstrum, FILE *fichero, int *numero_vectores, int numero_coeficientes_cepstrum) {

  int tamano = 1000, tamano_actual = 0;
  int elimina;
  char linea_cabecera[1000] = "", *apunta_linea;
  float *apunta_coeficiente;
  Vector_cepstrum_tiempo *apunta_vector;

  // Eliminamos la cabecera:

  while (strcmp(linea_cabecera, "EST_Header_End\n"))
    fgets(linea_cabecera, 1000, fichero);

  // Asignamos memoria a la cadena de salida.

  if ( (*cadena_cepstrum = (Vector_cepstrum_tiempo *) malloc(tamano*sizeof(Vector_cepstrum_tiempo))) == NULL) {
    fprintf(stderr, "Error en carga_fichero_cepstrum_bin, al asignar memoria.\n");
    return 1;
  }

  apunta_vector = *cadena_cepstrum;

  //    while (feof(fichero) == 0) {
  while (fgets(linea_cabecera, 1000, fichero) != NULL) {

    sscanf(linea_cabecera, "%f %d", &apunta_vector->tiempo, &elimina);

    apunta_linea = strstr(linea_cabecera, "\t");
    apunta_linea = strstr(apunta_linea + 1, "\t") + 1;

    //        fscanf(fichero, "%f", &apunta_vector->tiempo);

    //		fscanf(fichero, "%d", &elimina);

    //        if (feof(fichero))
    //            break;

    apunta_coeficiente = (float *) apunta_vector->vector;

    for (int contador = 0; contador < numero_coeficientes_cepstrum;
	 contador++) {
      sscanf(apunta_linea, "%f", apunta_coeficiente++);
      apunta_linea = strstr(apunta_linea, " ") + 1;
    }

    tamano_actual++;

    if (tamano_actual >= tamano) {
      tamano += 1000;
      if ( (*cadena_cepstrum = (Vector_cepstrum_tiempo *) realloc(*cadena_cepstrum, tamano*sizeof(Vector_cepstrum_tiempo))) == NULL) {
	fprintf(stderr, "Error en carga_fichero_cepstrum_bin, al asignar memoria.\n");
	return 1;
      }
      apunta_vector = *cadena_cepstrum + tamano_actual;
    }
    else
      apunta_vector++;

  } // while (feof(fichero) == 0)

  *numero_vectores = tamano_actual;

  return 0;

}


/**
 * Función:   devuelve_coeficientes
 * \remarks Entrada:
 *			- cadena_coeficientes: cadena con los vectores de coeficientes cepstrales.
 *          - tiempo_inicial: instante de inicio del fonema actual.
 *          - tiempo_final: instante final del fonema  actual.
 *          - cadena_marcas: cadena con la información de las marcas de pitch.
 *			- numero_elementos_vector: número de coeficientes de cada vector.
 *          - numero_vectores: número de vectores cepstrales (y de marcas de pitch).  	  
 *	\remarks Salida:
 *			- *coeficientes: vector con los coeficientes  al inicio, a la mitad y final   
 *            del fonema actual.                                                    	  
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un cero.                                      
 */

int devuelve_coeficientes(float *cadena_coeficientes, float tiempo_inicial,
			  float tiempo_final, marca_struct *cadena_marcas,
                          int numero_elementos_vector,
                          int numero_vectores, float *coeficientes) {

  float *apunta_coeficientes_fichero;
  int contador, cuenta_marcas = 1;
  float punto_medio = (tiempo_inicial + tiempo_final) / 2;
  float *apunta_coeficientes = coeficientes;
  marca_struct *recorre_marcas = cadena_marcas;
  float *recorre_coeficientes = cadena_coeficientes;

  // En primer lugar, el vector del inicio:

  while ( (recorre_marcas->etiqueta < tiempo_inicial) && (cuenta_marcas <= numero_vectores) ) {
    recorre_marcas++;
    recorre_coeficientes += numero_elementos_vector;
    cuenta_marcas++;
  } // while ( (recorre_marcas->etiqueta...

  if (cuenta_marcas > numero_vectores) {
    fprintf(stderr, "Error en devuelve_coeficientes. Fichero de marcas incorrecto.\n");
    return 1;
  } // if (cuenta_marcas == ...

  apunta_coeficientes_fichero = recorre_coeficientes;
  for (contador = 0; contador < numero_elementos_vector; contador++, apunta_coeficientes++)
    *apunta_coeficientes = *apunta_coeficientes_fichero++;

  // Ahora, el vector anterior al punto medio:

  while ( (recorre_marcas->etiqueta < punto_medio) && (cuenta_marcas <= numero_vectores) ) {
    recorre_marcas++;
    recorre_coeficientes += numero_elementos_vector;
    cuenta_marcas++;
  } // while ( (recorre_marcas->etiqueta...

  if (cuenta_marcas > numero_vectores) {
    fprintf(stderr, "Error en devuelve_coeficientes. Fichero de marcas incorrecto.\n");
    return 1;
  } // if (cuenta_marcas == ...

  if (recorre_marcas != cadena_marcas) {
    recorre_marcas--;
    recorre_coeficientes -= numero_elementos_vector;
    cuenta_marcas--;
  } // if (recorre_marcas != cadena_marcas)

  apunta_coeficientes_fichero = recorre_coeficientes;
  for (contador = 0; contador < numero_elementos_vector; contador++, apunta_coeficientes++)
    *apunta_coeficientes = *apunta_coeficientes_fichero++;

  // El vector posterior al punto medio:

  if (++cuenta_marcas > numero_vectores) {
    fprintf(stderr, "Error en devuelve_coeficientes. Fichero de marcas incorrecto.\n");
    return 1;
  } // if (++cuenta_marcas > numero_vectores)

  recorre_marcas++;
  recorre_coeficientes += numero_elementos_vector;

  apunta_coeficientes_fichero = recorre_coeficientes;
  for (contador = 0; contador < numero_elementos_vector; contador++, apunta_coeficientes++)
    *apunta_coeficientes = *apunta_coeficientes_fichero++;

  // Ahora, el vector del final:

  while ( (recorre_marcas->etiqueta < tiempo_final) && (cuenta_marcas < numero_vectores) ) {
    recorre_marcas++;
    recorre_coeficientes += numero_elementos_vector;
    cuenta_marcas++;
  } // while ( (recorre_marcas->etiqueta...

  if ( (cuenta_marcas < numero_vectores) && (recorre_coeficientes != cadena_coeficientes) )
    recorre_coeficientes -= numero_elementos_vector;

  apunta_coeficientes_fichero = recorre_coeficientes;
  for (contador = 0; contador < numero_elementos_vector; contador++, apunta_coeficientes++)
    *apunta_coeficientes = *apunta_coeficientes_fichero++;

  return 0;

}


/**
 * Función:   calcula_distancia_euclidea                                                  
 * \remarks Entrada:
 *			- vector1, vector2: vectores de coeficientes cepstrales.
 * 			- numero_coeficientes_cepstrales: número de elementos de los vectores.                      
 * \remarks Valor devuelto:
 *          - La distancia euclídea.
 */

float calcula_distancia_euclidea(Vector_cepstrum vector1, Vector_cepstrum vector2, int numero_coeficientes_cepstrales) {

  float *cepstrum1 = (float *) vector1;
  float *cepstrum2 = (float *) vector2;
  float diferencia;
  float acumulado = 0.0;

  for (int i = 0; i < numero_coeficientes_cepstrales; i++) {
    diferencia = *cepstrum1++ - *cepstrum2++;
    acumulado += diferencia*diferencia;
  } // for (int i = 0; i < ...

  return (float) sqrt(acumulado);

}

/**
 * Función:   calcula_distancia_euclidea                                                  
 * \remarks Entrada:
 *			- vector1, vector2: vectores de coeficientes cepstrales.                      
 *			- numero_coeficientes: número de componentes de cada vector cepstrum.		  
 * \remarks Valor devuelto:                                                                        
 *          - La distancia euclídea.                                                      
 */

float calcula_distancia_euclidea_general(float *vector1, float *vector2,
					 int numero_coeficientes) {

  float diferencia;
  float acumulado = 0.0;

  volatile float *local_vector1 = vector1;
  volatile float *local_vector2 = vector2;

  for (int i = 0; i < numero_coeficientes; i++) {
    diferencia = *local_vector1++ - *local_vector2++;   
    // diferencia = *vector1++ - *vector2++;
    acumulado += diferencia*diferencia;
  } // for (int i = 0; i < numero_coeficientes

  return (float) sqrt(acumulado);

}

#ifdef _CALCULA_DISTANCIAS_CEPSTRALES

/**
 * Función:   carga_vectores_cepstrum_unidad                                              
 * \remarks Entrada:
 *			- unidad: vector descriptor candidato del que se quieren cargar los vectores  
 *            de coeficientes cepstrales.                                                 
 *          - fichero: fichero en el que se encuentran los coeficientes cepstrales.       
 *			- frecuencia_muestreo: frecuencia de muestreo de los archivos de sonido.	  	
 *	\remarks Salida:
 *			- *cadena_cepstrum: vector con los coeficientes cepstrales de toda la unidad. 
 *          - numero_vectores: número de vectores de coeficientes de la unidad.           
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error. devuelve un cero.                                     
 * \remarks NOTA:
 *			- El fichero debe encontrarse en el formato de salida htk_mfcc del festival,  
 *            es decir, binario con cada vector de coeficientes (float) precedido por el  
 *            instante temporal. Se supone que el número de coeficientes de cada vector   
 *            es 12, y el espaciado entre ventanas, 8 milisegundos.                       
 */

int carga_vectores_cepstrum_unidad(Vector_semifonema_candidato *unidad, FILE *fichero,
                                   Vector_cepstrum_tiempo *cadena_cepstrum, int *numero_vectores,
                                   int frecuencia_muestreo) {

  float inicio, final;
  //    int indice;
  int posicion_lectura;
  Vector_cepstrum_tiempo *correcaminos = cadena_cepstrum;

  // Calculamos los límites temporales de la unidad:

  //    inicio = ( (float) unidad->indice / sizeof(short int)) / FS;

  inicio = ( (float) unidad->indice / sizeof(short int)) / frecuencia_muestreo;

  final = inicio + unidad->duracion;

  // Leemos el vector de coeficientes:

  posicion_lectura = (int) (inicio / DESPLAZAMIENTO_VENTANAS_CEPSTRUM);
  posicion_lectura *= (Vector_semifonema_candidato::numero_coeficientes_cepstrales + 1)*sizeof(float); // El +1 se debe a la
  // etiqueta temporal.
  posicion_lectura += TAMANO_CABECERA_HTK_MFCC;

  fseek(fichero, posicion_lectura, SEEK_SET);

  if ( ftell(fichero) != posicion_lectura) {
    fprintf(stderr, "Error en carga_vectores_cepstrum_unidad, al leer el vector de coeficientes cepstrales.\n");
    return 1;
  } // if (ftell(fichero) != posicion_lectura)

  *numero_vectores = 0;

  while (!feof(fichero)) {
    fread(&(correcaminos->tiempo), sizeof(float), 1, fichero);
    fread(correcaminos->vector, sizeof(float), Vector_semifonema_candidato::numero_coeficientes_cepstrales, fichero);

    if (correcaminos->tiempo > final)
      break;

    correcaminos++;
    if (++(*numero_vectores) >= MAXIMO_NUMERO_VECTORES_CEPSTRUM) {
      fprintf(stderr, "Error en carga_vectores_cepstrum_unidad: \
            unidad demasiado larga.\n");
      return 1;
    }// if (++(*numero_vectores...
  } // while (!feof(fichero))


  if ( (feof(fichero)) && (unidad->semifonema[0] != '#') ) {
    fprintf(stderr, "Error en carga_vectores_cepstrum_unidad: \
        fin de fichero.\n");
    return 1;
  } // if ( (feof(fichero)) && ...

    // Si no tiene ninguna marca, debido a que es un sonido demasiado corto, nos quedamos
    // con la última:

  if (*numero_vectores == 0)
    *numero_vectores = 1;

  return 0;

}


/**
 * Función:   carga_vectores_cepstrum_unidad_txt                                             
 * \remarks Entrada:
 *			- unidad: vector descriptor candidato del que se quieren cargar los vectores
 *            de coeficientes cepstrales.
 *          - fichero: fichero en el que se encuentran los coeficientes cepstrales.
 *			- frecuencia_muestreo: frecuencia de muestreo de los archivos de sonido.
 *	\remarks Salida:
 *			- *cadena_cepstrum: vector con los coeficientes cepstrales de toda la unidad.
 *          - numero_vectores: número de vectores de coeficientes de la unidad.
 * \remarks Valor devuelto:
 *          - En ausencia de error. devuelve un cero.
 * \remarks NOTA:
 *			- El fichero debe encontrarse en el formato de salida track del EST.
 *            Se supone que el número de coeficientes de cada vector   
 *            es 12, y el espaciado entre ventanas, 8 milisegundos.
 */

int carga_vectores_cepstrum_unidad_txt(Vector_semifonema_candidato *unidad, FILE *fichero,
                                       Vector_cepstrum_tiempo *cadena_cepstrum, int *numero_vectores,
                                       int frecuencia_muestreo) {

  float inicio, final;
  int elimina;
  char linea_cabecera[1000] = "", *apunta_linea;
  float *apunta_coeficiente;
  Vector_cepstrum_tiempo *correcaminos = cadena_cepstrum;

  // Eliminamos la cabecera:

  while (strcmp(linea_cabecera, "EST_Header_End\n"))
    fgets(linea_cabecera, 1000, fichero);

  if (feof(fichero)) {
    fprintf(stderr, "Error en carga_vectores_cepstrum_unidad_txt, formato de \
fichero cepstrum no válido.\n");
    return 1;
  }

  // Calculamos los límites temporales de la unidad:

  inicio = ( (float) unidad->indice / sizeof(short int)) / frecuencia_muestreo;

  final = inicio + unidad->duracion;

  *numero_vectores = 0;

  // Leemos hasta llegar al inicio de la unidad:

  while ( (apunta_linea = fgets(linea_cabecera, 1000, fichero)) != NULL) {

    sscanf(linea_cabecera, "%f %d", &correcaminos->tiempo, &elimina);

    if (correcaminos->tiempo >= inicio)
      break;

  };

  if (apunta_linea == NULL) {
    fprintf(stderr, "Error en carga_vectores_cepstrum_unidad_txt, formato de \
fichero cepstrum no válido.\n");
    return 1;
  }

  // Leemos la unidad

  while (correcaminos->tiempo <= final) {

    apunta_linea = strstr(linea_cabecera, "\t");
    apunta_linea = strstr(apunta_linea + 1, "\t") + 1;

    apunta_coeficiente = (float *) correcaminos->vector;

    for (int contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales;
	 contador++) {
      sscanf(apunta_linea, "%f", apunta_coeficiente++);
      apunta_linea = strstr(apunta_linea, " ") + 1;
    }

    correcaminos++;
    if (++(*numero_vectores) >= MAXIMO_NUMERO_VECTORES_CEPSTRUM) {
      fprintf(stderr, "Error en carga_vectores_cepstrum_unidad_txt: unidad demasiado larga.\n");
      return 1;
    }// if (++(*numero_vectores...

    if (fgets(linea_cabecera, 1000, fichero) == NULL) {

      if (unidad->semifonema[0] != '#') {
	fprintf(stderr, "Error en carga_vectores_cepstrum_unidad_txt: fin de fichero.\n");
	return 1;
      }
      else
	break;
    }

    sscanf(linea_cabecera, "%f %d", &correcaminos->tiempo, &elimina);

  }


  // Si no tiene ninguna marca, debido a que es un sonido demasiado corto, nos quedamos
  // con la última:

  if (*numero_vectores == 0) {
    *numero_vectores = 1;

    apunta_linea = strstr(linea_cabecera, "\t");
    apunta_linea = strstr(apunta_linea + 1, "\t") + 1;

    apunta_coeficiente = (float *) correcaminos->vector;

    for (int contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales;
	 contador++) {
      sscanf(apunta_linea, "%f", apunta_coeficiente++);
      apunta_linea = strstr(apunta_linea, " ") + 1;
    }

  }
    
  return 0;

}


/**
 * Función:   calcula_distancia_cepstral_media                                            
 * \remarks Entrada:
 *			- cepstrum_objetivo: cadena de vectores de coeficientes cepstrales objetivo.  
 *          - ventanas_objetivo: número de vectores de la cadena anterior.                
 *          - cepstrum_candidato: cadena de vectores de coeficientes cepstrales           
 *            candidato.                                                                  
 *          - ventanas_candidato: número de vectores de la cadena anterior.
 *			- numero_coeficientes_cepstrales: número de elementos de cada vector.               
 *	\remarks Salida:
 *			- distancia: distancia cepstral media.                                        
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error. devuelve un cero.                                     
 */

int calcula_distancia_cepstral_media(Vector_cepstrum_tiempo *cepstrum_objetivo, int ventanas_objetivo, Vector_cepstrum_tiempo *cepstrum_candidato, int ventanas_candidato, int numero_coeficientes_cepstrales, float *distancia) {

  float minimo, parcial, pos_objetivo;
  int divide_obj, divide_cand;
  float distancia_local = 0.0;
  Vector_cepstrum_tiempo *apunta_objetivo = cepstrum_objetivo;
  Vector_cepstrum_tiempo *apunta_candidato = cepstrum_candidato;

  if ( (ventanas_objetivo == 0) || (ventanas_candidato == 0) ) {
    fprintf(stderr, "Error en calcula_distancia_cepstral_media: Número de ventanas nulo.\n");
    return 1;
  } // if ( (ventanas_objetivo...

  if (ventanas_candidato != 1)
    divide_cand = ventanas_candidato - 1;
  else
    divide_cand = ventanas_candidato;

  if (ventanas_objetivo != 1)
    divide_obj = ventanas_objetivo - 1;
  else
    divide_obj = ventanas_objetivo;

  for (int contador = 0; contador < ventanas_objetivo; contador++, apunta_objetivo++) {

    pos_objetivo = (float) contador / divide_obj;
    minimo = FLT_MAX;

    for (int cuenta = 0; cuenta <  ventanas_candidato; cuenta++) {
      if ( (parcial = absoluto(((float) cuenta / divide_cand) - pos_objetivo)) < minimo) {
	apunta_candidato = cepstrum_candidato + cuenta;
	minimo = parcial;
      } // if ( (parcial = absoluto(...
    } // for (int cuenta = 0; cuenta < ventanas_candidato...


    distancia_local += calcula_distancia_euclidea(apunta_objetivo->vector, apunta_candidato->vector,
						  numero_coeficientes_cepstrales);

  } // for (int contador = 0; ...

  *distancia = distancia_local / ventanas_objetivo;

  //    fclose(fichero);

  return 0;

}




/**
 * Función:   devuelve_ventanas_cepstrum_unidad                                           
 * \remarks Entrada:
 *			- semifonema: vector descriptor candidato cuyos vectores de coeficietntes     
 *            cepstrales se quieren cargar.                                               
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)           
 *            de los ficheros con los vectores cepstrales precalculados.                  
 *          - numero_fichero: número del fichero en el Corpus800 al que pertenece el      
 *            semifonema. Es un char * porque viene rellenado con ceros por la izquierda, por el
 *            formato del nombre del fichero.
 *	        - frecuencia_muestreo: frecuencia de muestreo del fichero de sonido.
 *          - formato: 0 => htk_mfcc, != 0 => EST_Track.
 *	\remarks Salida:
 *	    - cadena_cepstrum: cadena de vectores de coeficientes cepstrales.             
 *          - numero_ventanas: número de vectores de la cadena anterior.                  
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error, devuelve un 0.                                        
 */

int devuelve_ventanas_cepstrum_unidad(Vector_semifonema_candidato *semifonema, char path_cepstrum[],
                                      char *numero_fichero,
                                      Vector_cepstrum_tiempo **cadena_cepstrum,
                                      int *numero_ventanas,
                                      int frecuencia_muestreo,
                                      unsigned int formato) {


  char nombre_fichero[FILENAME_MAX];
  FILE *fichero_cepstrum;
  Vector_cepstrum_tiempo cepstrum_auxiliar[MAXIMO_NUMERO_VECTORES_CEPSTRUM];
  Ventanas_cepstrum *apunta_cepstrum = vector_ventanas_cepstrum;

  while (apunta_cepstrum != NULL) {
    if (apunta_cepstrum->identificador > semifonema->identificador)
      break;

    if (apunta_cepstrum->identificador == semifonema->identificador) {
      *numero_ventanas = apunta_cepstrum->numero_ventanas;
      *cadena_cepstrum = (Vector_cepstrum_tiempo *) apunta_cepstrum->ventana;
      return 0;
    } // if (apunta_cepstrum->identificador...

    apunta_cepstrum = apunta_cepstrum->siguiente;
  } // while (apunta_cepstrum != NULL)

    // No estaba en la caché, así que tenemos que leerlo del fichero.

  sprintf(nombre_fichero, "%s%s.mfc", path_cepstrum, numero_fichero);

  if ( (fichero_cepstrum = fopen(nombre_fichero, "rb")) == NULL) {
    fprintf(stderr, "Error en devuelve_ventanas_cepstrum_unidad, al intentar abrir el fichero %s (unidad %d).\n", nombre_fichero, semifonema->identificador);
    return 1;
  } // if ( (fichero_cepstrum = fopen(...


  if (formato == 0) {
    if (carga_vectores_cepstrum_unidad(semifonema, fichero_cepstrum, cepstrum_auxiliar, numero_ventanas, frecuencia_muestreo)) {
      fprintf(stderr, "Error en el fichero %s.\n", nombre_fichero);
      return 1;
    }
  }
  else {
    if (carga_vectores_cepstrum_unidad_txt(semifonema, fichero_cepstrum, cepstrum_auxiliar, numero_ventanas, frecuencia_muestreo)) {
      fprintf(stderr, "Error en el fichero %s.\n", nombre_fichero);
      return 1;
    }
  }

  fclose(fichero_cepstrum);

  // Ahora añadimos a la caché el nuevo nodo:

  Ventanas_cepstrum *nuevo_nodo;

  if ( (nuevo_nodo = (Ventanas_cepstrum *) malloc(sizeof(Ventanas_cepstrum)))
       == NULL) {
    fprintf(stderr, "Error en devuelve_ventanas_cepstrum_unidad, al asignar memoria.\n");
    return 1;
  } // if ( (nuevo_nodo = ...

  nuevo_nodo->identificador = semifonema->identificador;
  nuevo_nodo->numero_ventanas = *numero_ventanas;
  memcpy(nuevo_nodo->ventana, cepstrum_auxiliar,
	 nuevo_nodo->numero_ventanas*sizeof(Vector_cepstrum_tiempo));
  nuevo_nodo->siguiente = NULL;

  *cadena_cepstrum = (Vector_cepstrum_tiempo *) nuevo_nodo->ventana;

  if (vector_ventanas_cepstrum == NULL)
    vector_ventanas_cepstrum = nuevo_nodo;
  else {
    Ventanas_cepstrum *anterior_cepstrum = vector_ventanas_cepstrum;
    apunta_cepstrum = vector_ventanas_cepstrum;

    while (apunta_cepstrum->siguiente != NULL) {
      if (apunta_cepstrum->identificador > nuevo_nodo->identificador) {
	if (apunta_cepstrum == vector_ventanas_cepstrum) {
	  nuevo_nodo->siguiente = vector_ventanas_cepstrum;
	  vector_ventanas_cepstrum = nuevo_nodo;
	} // if (apunta_cepstrum == vector_ventanas_cepstrum)
	else {
	  anterior_cepstrum->siguiente = nuevo_nodo;
	  nuevo_nodo->siguiente = apunta_cepstrum;
	} // else
	return 0;
      } // if (apunta_cepstrum->identificador > nuevo_nodo->identificador)

      anterior_cepstrum = apunta_cepstrum;
      apunta_cepstrum = apunta_cepstrum->siguiente;

    } // while (apunta_cepstrum->siguiente...

    // Tenemos que añadirlo al final:

    apunta_cepstrum->siguiente = nuevo_nodo;

  } // else

  return 0;

}


/**
 * Función:   anhade_coeficientes_cepstrum_difonema                                           
 * \remarks Entrada:
 *			- semifonema: vector descriptor candidato cuyos vectores de coeficietntes     
 *            cepstrales se quieren cargar.                                               
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error, devuelve un 0.                                        
 * \remarks NOTA:      Esta función sólo es una adaptación de devuelve_ventanas_cepstrum_unidad,   
 *			  que se emplea con el locutor Freire y su base de difonemas.				  
 */

int anhade_coeficientes_cepstrum_difonema(Vector_semifonema_candidato *semifonema) {


  Ventanas_cepstrum *apunta_cepstrum = vector_ventanas_cepstrum;

  while (apunta_cepstrum != NULL) {
    if (apunta_cepstrum->identificador > semifonema->identificador)
      break;

    if (apunta_cepstrum->identificador == semifonema->identificador) {
      // Ya estaba en la caché.
      return 0;
    } // if (apunta_cepstrum->identificador == ...

    apunta_cepstrum = apunta_cepstrum->siguiente;
  } // while (apunta_cepstrum != NULL)

    // No estaba en la caché. Como en este caso la unidad proviene de un difonema,
    // damos por bueno su valor medio. (Evidentemente es sólo una simplificación,
    // puesto que ésto sólo se usará con Freire y cuando haya que recurrir a la
    // base de difonemas.

    // Añadimos a la caché el nuevo nodo:

  Ventanas_cepstrum *nuevo_nodo;

  if ( (nuevo_nodo = (Ventanas_cepstrum *) malloc(sizeof(Ventanas_cepstrum)))
       == NULL) {
    fprintf(stderr, "Error en anhade_coeficientes_cepstrum_difonema, al asignar memoria.\n");
    return 1;
  } // if ( (nuevo_nodo = ...

  nuevo_nodo->identificador = semifonema->identificador;
  nuevo_nodo->numero_ventanas = 1;
  memcpy(nuevo_nodo->ventana, semifonema->cepstrum_mitad, sizeof(Vector_cepstrum_tiempo));
  nuevo_nodo->siguiente = NULL;

  if (vector_ventanas_cepstrum == NULL)
    vector_ventanas_cepstrum = nuevo_nodo;
  else {
    Ventanas_cepstrum *anterior_cepstrum = vector_ventanas_cepstrum;
    apunta_cepstrum = vector_ventanas_cepstrum;

    while (apunta_cepstrum->siguiente != NULL) {
      if (apunta_cepstrum->identificador > nuevo_nodo->identificador) {
	if (apunta_cepstrum == vector_ventanas_cepstrum) {
	  nuevo_nodo->siguiente = vector_ventanas_cepstrum;
	  vector_ventanas_cepstrum = nuevo_nodo;
	} // if (apunta_cepstrum == vector_ventanas_cepstrum)
	else {
	  anterior_cepstrum->siguiente = nuevo_nodo;
	  nuevo_nodo->siguiente = apunta_cepstrum;
	} // else
	return 0;
      } // if (apunta_cepstrum->identificador > nuevo_nodo->identificador)

      anterior_cepstrum = apunta_cepstrum;
      apunta_cepstrum = apunta_cepstrum->siguiente;

    } // while (apunta_cepstrum->siguiente...

    // Tenemos que añadirlo al final:

    apunta_cepstrum->siguiente = nuevo_nodo;

  } // else

  return 0;

}

/**
 * Función:   anhade_vector_medio_a_cache                                                 
 * \remarks Entrada:
 *			- semifonema: vector descriptor candidato cuyos vectores de coeficietntes     
 *            cepstrales se quieren cargar.                                               
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)           
 *            de los ficheros con los vectores cepstrales precalculados.                  
 *          - numero_fichero: número del fichero en el Corpus800 al que pertenece el      
 *            semifonema. Es un char * porque está completado con ceros a la izquierda,
 *            por el formato del nombre de los ficheros.
 *			- frecuencia_muestreo: frecuencia de muestreo del fichero de sonido.
 * \remarks Valor devuelto:
 *          - En ausencia de error, devuelve un 0.
 */

int anhade_vector_medio_a_cache(Vector_semifonema_candidato *semifonema, char path_cepstrum[],
                                char *numero_fichero, int frecuencia_muestreo) {

  int numero_ventanas;
  char nombre_fichero[FILENAME_MAX];
  FILE *fichero_cepstrum;
  Vector_cepstrum vector_cepstrum_medio;
  Vector_cepstrum_tiempo cepstrum_auxiliar[MAXIMO_NUMERO_VECTORES_CEPSTRUM];

  sprintf(nombre_fichero, "%s%s.mfc", path_cepstrum, numero_fichero);

  if ( (fichero_cepstrum = fopen(nombre_fichero, "rb")) == NULL) {
    fprintf(stderr, "Error en anhade_vector_medio_a_cache, al intentar abrir el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero_cepstrum = fopen(...


  if (carga_vectores_cepstrum_unidad(semifonema, fichero_cepstrum, cepstrum_auxiliar, &numero_ventanas, frecuencia_muestreo))
    return 1;

  fclose(fichero_cepstrum);

  if (calcula_vector_cepstrum_medio(cepstrum_auxiliar, numero_ventanas, vector_cepstrum_medio))
    return 1;


  // Ahora lo añadimos a la caché:

  Ventanas_cepstrum *nuevo_nodo;
  Ventanas_cepstrum *apunta_cepstrum; // = vector_ventanas_cepstrum;
    
  if ( (nuevo_nodo = (Ventanas_cepstrum *) malloc(sizeof(Ventanas_cepstrum)))
       == NULL) {
    fprintf(stderr, "Error en anhade_vector_medio_a_cache, al asignar memoria.\n");
    return 1;
  } // if ( (nuevo_nodo = ...

  nuevo_nodo->identificador = semifonema->identificador;
  nuevo_nodo->numero_ventanas = numero_ventanas;
  memcpy(nuevo_nodo->ventana, cepstrum_auxiliar,
	 nuevo_nodo->numero_ventanas*sizeof(Vector_cepstrum_tiempo));
  memcpy(nuevo_nodo->cepstrum_medio, vector_cepstrum_medio, sizeof(Vector_cepstrum));
    
  nuevo_nodo->siguiente = NULL;

  if (vector_ventanas_cepstrum == NULL)
    vector_ventanas_cepstrum = nuevo_nodo;
  else {
    Ventanas_cepstrum *anterior_cepstrum = vector_ventanas_cepstrum;
    apunta_cepstrum = vector_ventanas_cepstrum;

    while (apunta_cepstrum->siguiente != NULL) {
      if (apunta_cepstrum->identificador > nuevo_nodo->identificador) {
	if (apunta_cepstrum == vector_ventanas_cepstrum) {
	  nuevo_nodo->siguiente = vector_ventanas_cepstrum;
	  vector_ventanas_cepstrum = nuevo_nodo;
	} // if (apunta_cepstrum == vector_ventanas_cepstrum)
	else {
	  anterior_cepstrum->siguiente = nuevo_nodo;
	  nuevo_nodo->siguiente = apunta_cepstrum;
	} // else
	return 0;
      } // if (apunta_cepstrum->identificador > nuevo_nodo->identificador)

      anterior_cepstrum = apunta_cepstrum;
      apunta_cepstrum = apunta_cepstrum->siguiente;

    } // while (apunta_cepstrum->siguiente...

    // Tenemos que añadirlo al final:

    apunta_cepstrum->siguiente = nuevo_nodo;

  } // else


  return 0;

}


/**
 * Función:   borra_cache_distancias                                                      
 * \remarks Objetivo:  Liberar la memoria reservada para la memoria caché.                         
 */

void borra_cache_distancias() {

  Ventanas_cepstrum *apunta_ventana = vector_ventanas_cepstrum;
  Ventanas_cepstrum *siguiente;

  while (apunta_ventana != NULL) {
    siguiente = apunta_ventana->siguiente;
    free(apunta_ventana);
    apunta_ventana = siguiente;
  } // while (apunta_ventana != NULL)

  vector_ventanas_cepstrum = NULL;

}


/**
 * Función:   calcula_vector_cepstrum_medio                                               
 * \remarks Entrada:
 *			- cadena_vectores: cadena de vectores de coeficientes cepstrales.             
 *          - numero_vectores: número de elementos de la cadena anterior.                 
 *	\remarks Salida:
 *			- vector_cepstrum_medio: vector medio de coeficientes cepstrales.             
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Calcula el vector de coeficientes cepstrales medio de la cadena de vectores 
 *            que se le pasa como parametro.                                              
 */

int calcula_vector_cepstrum_medio(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores,
                                  Vector_cepstrum vector_cepstrum_medio) {
  int contador, cuenta_vectores;
  float *apunta_cepstrum = (float *) cadena_vectores->vector;

  if (numero_vectores < 1) {
    fprintf(stderr, "Error en calcula_vector_cepstrum_medio. Valor no válido (%d) del parámetro \"numero_vectores\".",
	    numero_vectores);
    return 1;
  } // if (numero_vectores ...

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    vector_cepstrum_medio[contador] = *apunta_cepstrum++;

  for (cuenta_vectores = 1; cuenta_vectores < numero_vectores; cuenta_vectores++) {
    apunta_cepstrum = cadena_vectores[cuenta_vectores].vector;
    for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
      vector_cepstrum_medio[contador] += *apunta_cepstrum++;
  } // for (cuenta_vectores ...

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    vector_cepstrum_medio[contador] /= numero_vectores;

  return 0;

}


/**
 * Función:   calcula_vector_cepstrum_medio                                               
 * \remarks Entrada:
 *			- cadena_vectores: cadena de vectores de coeficientes cepstrales.             
 *          - numero_vectores: número de elementos de la cadena anterior.                   
 *          - inicio: instante de inicio del segmento de voz.                             
 *          - final: instante de finalización del segmento de voz.                        
 *	\remarks Salida:
 *			- vector_cepstrum_medio: vector medio de coeficientes cepstrales.             
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Calcula el vector de coeficientes cepstrales medio de la cadena de vectores 
 *            que se le pasa como parametro, entre los instantes de tiempo considerados.  
 */

int calcula_vector_cepstrum_medio(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores, 
                                  float inicio, float final,  
                                  Vector_cepstrum vector_cepstrum_medio) {

  int contador, numero_elementos, cuenta_vectores = 0;
  float *apunta_cepstrum;
  Vector_cepstrum_tiempo *apunta_vector_cepstrum = cadena_vectores;

  if (numero_vectores < 1) {
    fprintf(stderr, "Error en calcula_vector_cepstrum_medio. Valor no válido (%d) del parámetro \"numero_vectores\".", numero_vectores);
    return 1;
  } // if (numero_vectores ...

  while (apunta_vector_cepstrum->tiempo <= inicio) {
    apunta_vector_cepstrum++;
    if (++cuenta_vectores >= numero_vectores) {
      fprintf(stderr, "Error en calcula_cepstrum_medio: Sobrepasado el número de elementos de la cadena \
                  de entrada.\n");
      return 1;
    } // if (++cuenta_vectores ...
  } // while (apunta_vector_cepstrum ...

  apunta_cepstrum = apunta_vector_cepstrum->vector;

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    vector_cepstrum_medio[contador] = *apunta_cepstrum++;

  numero_elementos = 1;

  while ((++apunta_vector_cepstrum)->tiempo <= final) {
    if (++cuenta_vectores > numero_vectores) {
      fprintf(stderr, "Error en calcula_cepstrum_medio: Sobrepasado el número de elementos de la cadena de entrada.\n");
      return 1;
    } // if (++cuenta_vectores ...

    apunta_cepstrum = apunta_vector_cepstrum->vector;
    for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
      vector_cepstrum_medio[contador] += *apunta_cepstrum++;
    numero_elementos++;
  } // while (apunta_vector_cepstrum->tiempo ...

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    vector_cepstrum_medio[contador] /= numero_elementos;

  return 0;

}


/**
 * Función:   obten_vector_cepstrum_anterior_a                                            
 * \remarks Entrada:
 *			- cadena_vectores: cadena de vectores de coeficientes cepstrales.             
 *          - numero_vectores: número de elementos de la cadena anterior.                   
 *			- tiempo: instante de tiempo.												  
 *			- anterior_posterior: = 0: vector anterior a tiempo.						  
 *								  != 0: vector posterior a tiempo.						  
 *	\remarks Salida:
 *			- vector_cepstrum_medio: vector de coeficientes cepstrales.             	  
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Devuelve el vector de coeficientes cepstrales correspondiente al instante	  
 *			  anterior a tiempo indicado.												  
 */

int obten_vector_cepstrum_proximo_a(Vector_cepstrum_tiempo *cadena_vectores, int numero_vectores,
				    float tiempo, char anterior_posterior,
				    Vector_cepstrum vector_cepstrum) {

  int contador = 0;
  float *apunta_cepstrum;
  Vector_cepstrum_tiempo *apunta_vector_cepstrum = cadena_vectores;

  if (numero_vectores < 1) {
    fprintf(stderr, "Error en calcula_vector_cepstrum_medio. Valor no válido (%d) del parámetro \"numero_vectores\".",
	    numero_vectores);
    return 1;
  } // if (numero_vectores ...

  while (apunta_vector_cepstrum->tiempo <= tiempo) {
    apunta_vector_cepstrum++;
    if (++contador >= numero_vectores) {
      fprintf(stderr, "Error en calcula_cepstrum_medio: Sobrepasado el número de elementos de la cadena de entrada.\n");
      return 1;
    } // if (++contador ...
  } // while (apunta_vector_cepstrum ...

  if (anterior_posterior == 0)
    if (apunta_vector_cepstrum != cadena_vectores)
      apunta_vector_cepstrum--;

  apunta_cepstrum = apunta_vector_cepstrum->vector;

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    vector_cepstrum[contador] = *apunta_cepstrum++;

  return 0;

}

/**
 * Función:   calcula_vector_lsf_medio                                               	  
 * \remarks Entrada:
 *			- cadena_vectores: cadena de vectores de coeficientes lsf.             		  
 *          - numero_vectores: número de elementos de la cadena anterior.                 
 *	\remarks Salida:
 *			- vector_lsf_medio: vector medio de coeficientes cepstrales.             	  
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Calcula el vector de coeficientes lsf medio de la cadena de vectores que se 
 *            le pasa como parametro.                                              		  
 */

int calcula_vector_lsf_medio(Vector_lsf_tiempo *cadena_vectores, int numero_vectores,
                             Vector_lsf vector_lsf_medio) {
                             
  int contador, cuenta_vectores;
  float *apunta_lsf = (float *) cadena_vectores->vector;

  if (numero_vectores < 1) {
    fprintf(stderr, "Error en calcula_vector_lsf_medio. Valor no válido (%d) del parámetro \"numero_vectores\".",
	    numero_vectores);
    return 1;
  } // if (numero_vectores ...

  for (contador = 0; contador < NUMERO_COEFICIENTES_LSF; contador++)
    vector_lsf_medio[contador] = *apunta_lsf++;

  for (cuenta_vectores = 1; cuenta_vectores < numero_vectores; cuenta_vectores++) {
    apunta_lsf = cadena_vectores[cuenta_vectores].vector;
    for (contador = 0; contador < NUMERO_COEFICIENTES_LSF; contador++)
      vector_lsf_medio[contador] += *apunta_lsf++;
  } // for (cuenta_vectores ...

  for (contador = 0; contador < NUMERO_COEFICIENTES_LSF; contador++)
    vector_lsf_medio[contador] /= numero_vectores;

  return 0;

}


/**
 * Función:   calcula_centroide                                                           
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Calcula el centroide de los vectores de coeficientes cepstrales de los      
 *            semifonemas que están almacenados en la caché.                              
 */

int calcula_centroide(Vector_cepstrum centroide) {

  int contador;
  int cuenta;
  int numero_unidades = 0;
  float *parcial_cepstrum;
  Vector_cepstrum_tiempo *apunta_vector;
  Ventanas_cepstrum *apunta_cepstrum = vector_ventanas_cepstrum;

  if (vector_ventanas_cepstrum == NULL) {
    fprintf(stderr, "Error en calcula_centroide. Caché vacía.\n");
    return 1;
  } // if (vector_ventanas_cepstrum == NULL)

    // Primero calculamos los vectores cepstrales medios de cada semifonema:

  while (apunta_cepstrum != NULL) {
    numero_unidades++;
    apunta_vector = apunta_cepstrum->ventana;
    parcial_cepstrum = (float *) apunta_cepstrum->cepstrum_medio;
    for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
      parcial_cepstrum[contador] = apunta_vector->vector[contador];
    apunta_vector++;
    for (contador = 1; contador < apunta_cepstrum->numero_ventanas;
	 contador++, apunta_vector++)
      for (cuenta = 0; cuenta < Vector_semifonema_candidato::numero_coeficientes_cepstrales; cuenta++)
	parcial_cepstrum[cuenta] += apunta_vector->vector[cuenta];
    for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
      parcial_cepstrum[contador] /= apunta_cepstrum->numero_ventanas;

    apunta_cepstrum = apunta_cepstrum->siguiente;

  } // while (apunta_cepstrum != NULL)

    // Ahora calculamos el centroide a partir de los vectores cepstrales medios:

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    centroide[contador] = 0.0;

  apunta_cepstrum = vector_ventanas_cepstrum;

  while (apunta_cepstrum != NULL) {
    parcial_cepstrum = apunta_cepstrum->cepstrum_medio;

    for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
      centroide[contador] += parcial_cepstrum[contador];

    apunta_cepstrum = apunta_cepstrum->siguiente;

  } // while (apunta_cepstrum != NULL)

  for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++)
    centroide[contador] /= numero_unidades;

  return 0;

}


/**
 * Función:   calcula_varianza                                                            
 * \remarks Entrada:
 *			- centroide: centroide de los vectores de coeficientes cepstrales del grupo   
 *            considerado.                                                                
 *	\remarks Salida:
 *			- varianza: varianza del grupo.
 *			- numero_coeficientes_cepstrales: número de elementos del centroide.                                            
 * \remarks Valor devuelto:                                                                        
 *           - En ausencia de error devuelve un 0.                                        
 * \remarks Objetivo:   Calcula la varianza de las distancias espectrales de las unidades que      
 *             están almacenadas en la caché.                                             
 */

int calcula_varianza(Vector_cepstrum centroide, int numero_coeficientes_cepstrales, float *varianza) {

  int numero_unidades = 0;
  float distancia;
  float varianza_local = 0.0;
  Ventanas_cepstrum *apunta_cepstrum = vector_ventanas_cepstrum;

  if (vector_ventanas_cepstrum == NULL) {
    fprintf(stderr, "Error en calcula_varianza. Caché vacía.\n");
    return 1;
  } // if (vector_ventanas_cepstrum == NULL)

    // Primero calculamos los vectores cepstrales medios de cada semifonema:

  while (apunta_cepstrum != NULL) {
    numero_unidades++;
    distancia = calcula_distancia_euclidea(centroide, apunta_cepstrum->cepstrum_medio,
					   numero_coeficientes_cepstrales);
    varianza_local += distancia*distancia;
    apunta_cepstrum = apunta_cepstrum->siguiente;
  } // while (apunta_cepstrum != NULL)

  *varianza = varianza_local / numero_unidades;

  return 0;

}


/**
 * Función:   encuentra_unidad_proxima_a_centroide                                        
 * \remarks Entrada:
 *			- centroide: centroide de los vectores de coeficientes cepstrales.
 *			- numero_coeficientes_cepstrales: número de elementos del centroide.            
 *	\remarks Salida:
 *			- identificador: identificador del semifonema que se encuentra más próximo al 
 *            centroide.                                                                  
 *          - cepstrum_objetivo: su conjunto de vectores de coeficientes cepstrales.      
 *          - numero_ventanas_objetivo: número de vectores.                               
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 * \remarks Objetivo:  Devuelve la unidad de la caché que se encuentra más próxima al centroide de 
 *            vectores de coeficientes cepstrales.                                        
 */

int encuentra_unidad_proxima_a_centroide(Vector_cepstrum centroide, int numero_coeficientes_cepstrales, int *identificador, Vector_cepstrum_tiempo **cepstrum_objetivo, int *numero_ventanas_objetivo) {

  float distancia_actual;
  float distancia_minima = FLT_MAX;
  Ventanas_cepstrum *apunta_ventanas = vector_ventanas_cepstrum;

  if (vector_ventanas_cepstrum == NULL) {
    fprintf(stderr, "Error en encuentra_unidad_proxima_a_centroide: Caché vacía.\n");
    return 1;
  } // if (vector_ventanas_cepstrum == NULL)

  while (apunta_ventanas != NULL) {
    if ( (distancia_actual = calcula_distancia_euclidea(centroide, apunta_ventanas->cepstrum_medio, numero_coeficientes_cepstrales))
	 < distancia_minima) {
      distancia_minima = distancia_actual;
      *cepstrum_objetivo = apunta_ventanas->ventana;
      *identificador = apunta_ventanas->identificador;
      *numero_ventanas_objetivo = apunta_ventanas->numero_ventanas;
    } // if ( (distancia_actual = calcula...

    apunta_ventanas = apunta_ventanas->siguiente;
        
  } // while (apunta_ventanas != NULL)

  return 0;

}


/**
 * Función:   anhade_vector_distancia                                                     
 * \remarks Entrada:
 *			- nuevo_vector: nuevo nodo que se desea añadir al array ordenado de           
 *            distancias espectrales.                                                     
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */

int anhade_vector_distancia(Vector_distancias nuevo_vector) {

  int contador, cuenta;
  Vector_distancias *apunta_distancia = (Vector_distancias *) vector_distancia;
  /*
    if (numero_distancias >= MAXIMO_NUMERO_UNIDADES_GRUPO*MAXIMO_NUMERO_UNIDADES_GRUPO) {
    puts("Error en anhade_vector_distancia: Vector de distancias completo.");
    return 1;
    }
  */

  for (contador = 0; contador < numero_distancias; contador++, apunta_distancia++)
    if (apunta_distancia->distancia >= nuevo_vector.distancia)
      break;

  if (contador == numero_distancias)
    *apunta_distancia = nuevo_vector;
  else {
    apunta_distancia = vector_distancia + numero_distancias;
    for (cuenta = numero_distancias; cuenta > contador; cuenta--, apunta_distancia--)
      *apunta_distancia = *(apunta_distancia - 1);
    *apunta_distancia = nuevo_vector;
  } // else

  numero_distancias++;
    
  return 0;
    
}


/**
 * Función:   anhade_vector_distancia                                                     
 * \remarks Entrada:
 *			- nuevo_vector: nuevo nodo que se desea añadir al array ordenado de           
 *            distancias espectrales.                                                     
 *          - tamano_maximo: número más alto de unidades que se pueden almacenar al mismo 
 *            tiempo.                                                                     
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error devuelve un 0.                                         
 */

int anhade_vector_distancia(Vector_distancias nuevo_vector, int tamano_maximo) {

  int contador, cuenta;
  int limite;
  Vector_distancias *apunta_distancia = (Vector_distancias *) vector_distancia;
  /*
    if (numero_distancias >= MAXIMO_NUMERO_UNIDADES_GRUPO*MAXIMO_NUMERO_UNIDADES_GRUPO) {
    puts("Error en anhade_vector_distancia: Vector de distancias completo.");
    return 1;
    }
  */
  if (numero_distancias >= tamano_maximo) {
    if ((apunta_distancia + tamano_maximo - 1)->distancia < nuevo_vector.distancia)
      return 0;
    limite = tamano_maximo;
  } // if (numero_distancias >= tamano_maximo
  else
    limite = numero_distancias++;


  for (contador = 0; contador < limite; contador++, apunta_distancia++)
    if (apunta_distancia->distancia >= nuevo_vector.distancia)
      break;

  if ( (apunta_distancia->distancia == nuevo_vector.distancia) &&
       (numero_distancias != 1) ) {// pasamos de ese valor.
    if (limite != tamano_maximo)
      numero_distancias--;
    return 0;
  } // if ( (apunta_distancia->distancia

  if (contador == limite)
    *apunta_distancia = nuevo_vector;
  else {
    apunta_distancia = vector_distancia + limite;
    for (cuenta = limite; cuenta > contador; cuenta--, apunta_distancia--)
      *apunta_distancia = *(apunta_distancia - 1);
    *apunta_distancia = nuevo_vector;
  } // else

  return 0;
    
}


/**
 * Función:   reinicia_vector_distancia                                                   
 */

void reinicia_vector_distancia() {

  numero_distancias = 0;

}


/**
 * Función:   devuelve_vector_distancia                                                   
 *	\remarks Salida:
 *			- apunta_distancia: vector que apunta a la cadena ordenada de distancias      
 *            espectrales.                                                                
 *          - distancias: número de elementos de la cadena anterior.                      
 * \remarks Valor devuelto:                                                                        
 *          - En ausencia de error, devuelve un 0.                                              
 */

int devuelve_vector_distancia(Vector_distancias **apunta_distancia, int *distancias) {

  if (numero_distancias == 0) {
    fprintf(stderr, "Error en devuelve_vector_distancia. Array vacío.\n");
    return 1;
  } // if (numero_distancias == 0)

  *apunta_distancia = (Vector_distancias *) vector_distancia;
  *distancias = numero_distancias;

  return 0;

}

#endif 
