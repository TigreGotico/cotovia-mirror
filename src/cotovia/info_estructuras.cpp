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
#include <time.h>
#include <math.h>
#include <float.h>

#include "modos.hpp"
#include "fonemas.hpp"
#include "info_corpus_voz.hpp"
#include "distancia_espectral.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "energia.hpp"
#include "modelo_duracion.hpp"
#include "matriz.hpp"
#include "path_list.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "Viterbi.hpp"
#include "grupos_acentuales.hpp"
#include "descriptor.hpp"
#include "letras.hpp"
#include "info_estructuras.hpp"
#include "fonemas.hpp"

#if !defined(_MODO_NORMAL) && defined(_CALCULA_DISTANCIAS_CEPSTRALES) && \
  (defined(_SCRIPT) || defined(_GRUPOS_ACENTUALES))
#undef _CALCULA_DISTANCIAS_CEPSTRALES

#endif

static estructura_unidades *tabla_unidades_d[NUMERO_FONEMAS][NUMERO_FONEMAS];
static estructura_unidades *tabla_unidades_i[NUMERO_FONEMAS][NUMERO_FONEMAS];

#ifdef _CALCULA_DISTANCIAS_CEPSTRALES
static Vector_cepstrum centroides_semifonemas[NUMERO_FONEMAS][NUMERO_FONEMAS][2];
static Vector_cepstrum_varianza centroides_semifonemas_varianza[NUMERO_FONEMAS][NUMERO_FONEMAS][2];
#endif

#ifdef _ESCRIBE_FRECUENCIA_GRUPOS_ACENTUALES
static int *ocurrencias = NULL;
#endif


/**
 * \remarks Escribe en el fichero de salida indicado (o por la salida estándar) los fonemas que componen la frase de entrada.
 * \param[in] frase_prosodica Array de tipo t_prosodia que contiene la información deseada.
 * \param[in] fichero_salida parte inicial del nombre del fichero de salida (sin extensión)
 * \param[in] fstdout Si vale 0, la salida se escribe en el fichero de nombre indicado en "fichero_salida". En otro caso, se escribe en la salida estándar.
 * \return En ausencia de error, devuelve un cero.
 */

int escribe_fonemas_frase(t_prosodia *frase_prosodica, char *fichero_salida, int fstdout) {

  char nombre_fichero[FILENAME_MAX];
  FILE *fichero;
  t_prosodia *correcaminos = frase_prosodica;

  if (fstdout == 0) {
    sprintf(nombre_fichero, "%s.fon", fichero_salida);
    if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en escribe_fonemas_frase, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    }
  }
  else
    fichero = stdout;
  
  while (correcaminos->alof[0] != '\0') {
    fprintf(fichero, "%s\n", correcaminos->alof);
    correcaminos++;
  }

  if (fstdout == 0)
    fclose(fichero);
  
  return 0;

}

/**
 * \remarks Filtra los acentos de un nombre de fichero
 * \param nombre_fichero nombre del fichero
 *
 */

void filtra_nombre_fichero(char *nombre_fichero) {

  char problematicas[][2] = {"Á", "á", "É", "é", "Í", "í", "Ó", "ó", "Ú", "ú", "Ñ", "ñ"};
  char cambios[][2] = {"A", "a", "E", "e", "I", "i", "O", "o", "U", "u", "N", "n"};
  char *apunta_vocal;

  for (int contador = 0; contador < 12; contador++) {
    while ( (apunta_vocal = strstr(nombre_fichero, problematicas[contador])) != NULL) 
      *apunta_vocal = cambios[contador][0];
  }


}

/**
 * Función:	incluye_limites_grupo_entonativo
 * \remarks	Función que modifica los fonemas objetivo cuando el grupo acentual
 * acaba en una ruptura entonativa.
 * \param	fonemas_objetivo: cadena con los fonemas objetivo.
 * \param	numero_fonemas: número de elementos de la cadena.
 * \param	cadena_posiciones: cadena con las posiciones originales (respecto a
 * los grupos fónicos) de los fonemas objetivo.
 * \param	grupos_acentuales: cadena de grupos acentuales candidatos.
 * \param	numero_grupos: número de grupos de la cadena.
 * \param       paralelo (opcional) Si no se indica, se introducen los cambios en la variables miembro Vector_descriptor_objetivo::frontera_prosodica. Si se iundica, se introducen en la variable Vector_descriptor_objetivo::fronteras_prosodicas
 * \return	En ausencia de error, devuelve un cero.
 */

int incluye_limites_grupo_entonativo(vector<Vector_descriptor_objetivo> &fonemas_objetivo, int numero_fonemas, short int *cadena_posiciones, Grupo_acentual_candidato *grupos_acentuales, int numero_grupos, char paralelo, int indice_primer_fonema) {

  int cuenta_fonemas = 0;

  vector<Vector_descriptor_objetivo>::iterator fonemas_it = fonemas_objetivo.begin();

  fonemas_it += indice_primer_fonema;
  cadena_posiciones += indice_primer_fonema;

  for (int contador = 0; contador < numero_grupos; contador++, grupos_acentuales++) {

    while ( (fonemas_it->acento == ATONA) && (cuenta_fonemas < numero_fonemas) ) {

      if (paralelo == 0) {
	if ( (grupos_acentuales->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) &&
	     ( (fonemas_it->frontera_inicial == FRONTERA_PALABRA) ||
	       (fonemas_it->frontera_inicial == FRONTERA_GRUPO_ENTONATIVO)) )
	  fonemas_it->frontera_prosodica = UNIDAD_RUPTURA;
	else
	  fonemas_it->frontera_prosodica = *cadena_posiciones;
      }
      else {
#ifndef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS
	if (sordo_sonoro(fonemas_it->semifonema) == SORDO) {
#else
	  if (fonemas_it->semifonema[0] == '#') {
#endif
	    if (fonemas_it->fronteras_prosodicas.size() == 0)
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	}

	else {
	  if ( (grupos_acentuales->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) &&
	       ( (fonemas_it->frontera_inicial == FRONTERA_PALABRA) ||
		 (fonemas_it->frontera_inicial == FRONTERA_GRUPO_ENTONATIVO)) )
	    fonemas_it->fronteras_prosodicas.push_back(UNIDAD_RUPTURA);
	  else
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	} // else (SONORO)
      }

      if (cuenta_fonemas++ > numero_fonemas) {
	fprintf(stderr, "Error en incluye_limites_grupo_entonativo: número de fonemas erróneo.\n");
	return 1;
      }
      fonemas_it++;
      cadena_posiciones++;
      continue;

    } // while (fonemas_it->acento == ATONA)

      if (cuenta_fonemas == numero_fonemas) {
	if (numero_grupos != 1) {
	fprintf(stderr, "Error en incluye_limites_grupo_entonativo: número de fonemas erróneo.\n");
	return 1;
	}
	return 0;
      }

    while (fonemas_it->acento == TONICA) {

      if (paralelo == 0) {
	if ( ( (grupos_acentuales->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) && 
	       ( (fonemas_it->frontera_final == FRONTERA_PALABRA) ||
		 (fonemas_it->frontera_final == FRONTERA_GRUPO_ENTONATIVO)) ) ||
	     ( (grupos_acentuales->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) &&
	       ( (fonemas_it->frontera_inicial == FRONTERA_PALABRA) ||
		 (fonemas_it->frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) ) )
	  fonemas_it->frontera_prosodica = UNIDAD_RUPTURA;
	else
	  fonemas_it->frontera_prosodica = *cadena_posiciones;
      }
      else {
#ifndef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS	
	if (sordo_sonoro(fonemas_it->semifonema) == SORDO) {
#else
	  if (fonemas_it->semifonema[0] == '#') {
#endif
	  if (fonemas_it->fronteras_prosodicas.size() == 0)
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	} // if (sordo_sonoro...
	else {
	  if ( ( (grupos_acentuales->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) &&
		 ( (fonemas_it->frontera_final == FRONTERA_PALABRA) ||
		   (fonemas_it->frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) ) ||
	       ( (grupos_acentuales->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) &&
		 ( (fonemas_it->frontera_inicial == FRONTERA_PALABRA) ||
		   (fonemas_it->frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) ) )
	    fonemas_it->fronteras_prosodicas.push_back(UNIDAD_RUPTURA);
	  else
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	} // else (SONORO)
      } // if (paralelo == 0)

      if (++cuenta_fonemas >= numero_fonemas) {
	fprintf(stderr, "Error en incluye_limites_grupo_entonativo: número de fonemas erróneo.\n");
	return 1;
      }
      fonemas_it++;
      cadena_posiciones++;

      if ( (fonemas_it->frontera_inicial == FRONTERA_PALABRA) ||
	   (fonemas_it->frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) )
	break;
                
      continue;

    }  // while (fonemas_it->acento == TONICA)

    while ( ( (fonemas_it->frontera_inicial != FRONTERA_PALABRA) || (contador == numero_grupos - 1) ) &&
	    (fonemas_it->frontera_inicial != FRONTERA_GRUPO_ENTONATIVO) ) {

      if (fonemas_it->semifonema[0] == '#') {
	if (paralelo == 0)
	  fonemas_it->frontera_prosodica = *cadena_posiciones;
	else
	  if (fonemas_it->fronteras_prosodicas.size() == 0)
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	break;
      }

      if (paralelo == 0) {
	if ( (grupos_acentuales->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) &&
	     (fonemas_it->frontera_final == FRONTERA_PALABRA) )
	  fonemas_it->frontera_prosodica = UNIDAD_RUPTURA;
	else
	  fonemas_it->frontera_prosodica = *cadena_posiciones;
      }
      else {
#ifndef _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS	
	if (sordo_sonoro(fonemas_it->semifonema) == SORDO) {
#else
	  if (fonemas_it->semifonema[0] == '#') {
#endif
	  if (fonemas_it->fronteras_prosodicas.size() == 0)
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	} // if (sordo_sonoro...
	else {
	  if ( (grupos_acentuales->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) &&
	       (fonemas_it->frontera_final == FRONTERA_PALABRA) )
	    fonemas_it->fronteras_prosodicas.push_back(UNIDAD_RUPTURA);
	  else
	    fonemas_it->fronteras_prosodicas.push_back(*cadena_posiciones);
	}
      } // else (SONORO)

      if (++cuenta_fonemas >= numero_fonemas) {
	fprintf(stderr, "Error en incluye_limites_grupo_entonativo: número de fonemas erróneo.\n");
	return 1;
      }
      fonemas_it++;
      cadena_posiciones++;
      continue;

    } // while (fonemas_it->frontera_inicial != FRONTERA_PALABRA...)


  } // for (int contador = 0; ...

  return 0;

}

/**
 * \brief Función que indica sobre qué unidades es preciso realizar una modificación de la f0
 * \param[in] fonemas_objetivo vector de Vector_descriptor_objetivo
 * \param[in] vectores_candidatos array de Vector_semifonema_candidato
 * \param[in] vectores_sintesis array de Vectores_sintesis
 * \param[in] numero_semifonemas número de elementos de los dos arrays anteriores.
 * \return 0
 */

int incluye_modificacion_entonativa(vector<Vector_descriptor_objetivo> &fonemas_objetivo, Vector_semifonema_candidato *vectores_candidatos, Vector_sintesis *vectores_sintesis, int numero_semifonemas) {

  unsigned char izq_der = IZQUIERDO;
  float diferencia_frec_est, diferencia_frec_coart;

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = fonemas_objetivo.begin();

  for (int contador = 0; contador < numero_semifonemas; contador++) {

    if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {

      if (izq_der == IZQUIERDO) {
	
	diferencia_frec_coart = absoluto(recorre_objetivo->frecuencia_izquierda - 
					 vectores_candidatos->frecuencia_inicial);
	diferencia_frec_est = absoluto(recorre_objetivo->frecuencia_mitad -
				       vectores_candidatos->frecuencia_final);
	
	izq_der = DERECHO;
      } 
      else { // DERECHO
	
	diferencia_frec_est = absoluto(recorre_objetivo->frecuencia_mitad -
				       vectores_candidatos->frecuencia_inicial);
	diferencia_frec_coart = absoluto(recorre_objetivo->frecuencia_derecha -
					 vectores_candidatos->frecuencia_final);
	
	izq_der = IZQUIERDO;
	recorre_objetivo++;
      }
      
      if ( (diferencia_frec_est > FREC_UMBRAL) || (diferencia_frec_coart > FREC_UMBRAL) )
	vectores_sintesis->validez = vectores_candidatos->validez | 1;
      else
	vectores_sintesis->validez = vectores_candidatos->validez & 0xFE;

    } // if (sordo_sonoro() == SONORO)
    else {
      if (izq_der == IZQUIERDO)
	izq_der = DERECHO;
      else {
	izq_der = IZQUIERDO;
	recorre_objetivo++;
      }
    } // else

    vectores_candidatos++;
    vectores_sintesis++;

  }
  
  return 0;

}

/**
 * \brief Función que indica sobre qué unidades es preciso realizar una modificación de la f0
 * \remarks Versión simplificada de "incluye_modificacion_entonativa_sintesis", en la que se coge toda la información de la cadena de vectores de síntesis
 * \param[in] fonemas_objetivo vector de Vector_descriptor_objetivo
 * \param[in] vectores_sintesis array de Vectores_sintesis
 * \param[in] numero_semifonemas número de elementos de los dos arrays anteriores.
 * \return 0
 */

int incluye_modificacion_entonativa_sintesis(vector<Vector_descriptor_objetivo> &fonemas_objetivo, Vector_sintesis *vectores_sintesis, int numero_semifonemas, int frecuencia_muestreo) {

  unsigned char izq_der = IZQUIERDO;
  unsigned char validez;
  float diferencia_frec_est, diferencia_frec_coart, diferencia_duracion, diferencia_potencia, duracion_efectiva;
  float porcentaje_duracion;
  float f0_punto_medio;

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = fonemas_objetivo.begin();

  for (int contador = 0; contador < numero_semifonemas; contador++) {

    validez = 0;

#ifndef _SOLO_COSTE_INTELIGIBILIDAD

    duracion_efectiva = vectores_sintesis->duracion;

    if (izq_der == IZQUIERDO) {
      
      if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {

	diferencia_frec_coart = absoluto(recorre_objetivo->frecuencia_izquierda - vectores_sintesis->frecuencia_inicial);
	diferencia_frec_est = absoluto(recorre_objetivo->frecuencia_mitad - vectores_sintesis->frecuencia_final);
	
#ifdef _MODIFICACION_F0_SOLO_ZONA_ESTACIONARIA
	if (diferencia_frec_est > FREC_UMBRAL) {
	  validez = 1;
	  if (sordo_sonoro(recorre_objetivo->contexto_izquierdo) == SORDO)
	    recorre_objetivo->frecuencia_izquierda = vectores_sintesis->frecuencia_inicial; // Dejamos la original de la unidad escogida
	  else {
	    f0_punto_medio = (vectores_sintesis->frecuencia_inicial + (vectores_sintesis - 1)->frecuencia_final) / 2;
	    recorre_objetivo->frecuencia_izquierda = f0_punto_medio;
	    (recorre_objetivo - 1)->frecuencia_derecha = f0_punto_medio;
	  }
	} // if (diferencia_frec_est > FREC_UMBRAL
	else {
	  if (sordo_sonoro(recorre_objetivo->contexto_izquierdo) == SONORO)
	    if (absoluto(vectores_sintesis->frecuencia_inicial - (vectores_sintesis - 1)->frecuencia_final) > FREC_UMBRAL) {
	      validez = 1;
	      f0_punto_medio = (vectores_sintesis->frecuencia_inicial + (vectores_sintesis - 1)->frecuencia_final) / 2;
	      recorre_objetivo->frecuencia_izquierda = f0_punto_medio;
	      (recorre_objetivo - 1)->frecuencia_derecha = f0_punto_medio;
	    } // if (absoluto(vector_sintesis->frecuencia_inicial ...
	}
#else
	if ( (diferencia_frec_coart > FREC_UMBRAL) || (diferencia_frec_est > FREC_UMBRAL) )
	  validez = 1;

#endif
	if (validez == 1)
	  duracion_efectiva = vectores_sintesis->calcula_duracion_efectiva(recorre_objetivo->frecuencia_izquierda, recorre_objetivo->frecuencia_mitad, frecuencia_muestreo);
	else
	  duracion_efectiva = vectores_sintesis->duracion;
      } // if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO
      else 
	duracion_efectiva = vectores_sintesis->duracion;
      
      diferencia_duracion = absoluto(recorre_objetivo->duracion_1 - duracion_efectiva);
      porcentaje_duracion = recorre_objetivo->duracion_1 / duracion_efectiva;

      diferencia_potencia = absoluto(recorre_objetivo->potencia - vectores_sintesis->potencia_final);

    } 
    else { // DERECHO
      
      if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {

	diferencia_frec_est = absoluto(recorre_objetivo->frecuencia_mitad - vectores_sintesis->frecuencia_inicial);
	diferencia_frec_coart = absoluto(recorre_objetivo->frecuencia_derecha - vectores_sintesis->frecuencia_final);
	
#ifdef _MODIFICACION_F0_SOLO_ZONA_ESTACIONARIA
	
	if (diferencia_frec_est > FREC_UMBRAL) {
	  validez = 1;
	  if (sordo_sonoro(recorre_objetivo->contexto_derecho) == SORDO) {
	    if (recorre_objetivo->frontera_prosodica != UNIDAD_FIN_FRASE)
	      recorre_objetivo->frecuencia_derecha = vectores_sintesis->frecuencia_final; // mantenemos la frecuencia de la unidad escogida
	  }
	  else {
	    if (recorre_objetivo->frontera_prosodica != UNIDAD_FIN_FRASE) {
	      f0_punto_medio = (vectores_sintesis->frecuencia_final + (vectores_sintesis + 1)->frecuencia_inicial) / 2;
	      recorre_objetivo->frecuencia_derecha = f0_punto_medio;
	      (recorre_objetivo + 1)->frecuencia_izquierda = f0_punto_medio;
	    }
	  }
	} // if (diferencia_frec_est > FREC_UMBRAL)
	else {
	  if (sordo_sonoro(recorre_objetivo->contexto_derecho) == SONORO) {
	    if (absoluto(vectores_sintesis->frecuencia_final - (vectores_sintesis + 1)->frecuencia_inicial) > FREC_UMBRAL) {
	      validez = 1;
	      f0_punto_medio = (vectores_sintesis->frecuencia_final + (vectores_sintesis + 1)->frecuencia_inicial) / 2;
	      recorre_objetivo->frecuencia_derecha = f0_punto_medio;
	      (recorre_objetivo + 1)->frecuencia_izquierda = f0_punto_medio;
	    }
	  } // if (sordo_sonoro(...
	  else if (recorre_objetivo->frontera_prosodica == UNIDAD_FIN_FRASE) {
	    if (diferencia_frec_coart > FREC_UMBRAL)
	      validez = 1;
	  } // else if (recorre_objetivo->frontera_prosodica == UNIDAD_FIN_FRASE)
	} // else
#else
	if ( (diferencia_frec_est > FREC_UMBRAL) || (diferencia_frec_coart > FREC_UMBRAL) )
	  validez = 1;
       
#endif

	if (validez == 1)
	  duracion_efectiva = vectores_sintesis->calcula_duracion_efectiva(recorre_objetivo->frecuencia_mitad, recorre_objetivo->frecuencia_derecha, frecuencia_muestreo);
	else
	  duracion_efectiva = vectores_sintesis->duracion;
      }
      else
	duracion_efectiva = vectores_sintesis->duracion;
      
      diferencia_duracion = absoluto(recorre_objetivo->duracion_2 - duracion_efectiva);
      porcentaje_duracion = recorre_objetivo->duracion_2 / duracion_efectiva;

      diferencia_potencia = absoluto(recorre_objetivo->potencia - vectores_sintesis->potencia_inicial);

    }

    if (vectores_sintesis->tipo_espectral == SILENCIO_ESP)
      validez |= 2;
    else
      if (diferencia_duracion > DUR_UMBRAL)
	if ( (izq_der == IZQUIERDO) || 
	     (vectores_sintesis->tipo_espectral != OCLU_SOR_ESP) ) {
	  
	  validez |= 2;
	  
	  if (izq_der == IZQUIERDO) {
	    if (porcentaje_duracion >  DUR_DIF_COEF_3)
	      recorre_objetivo->duracion_1 = 1.2*duracion_efectiva;
	    else
	      if (porcentaje_duracion < DUR_DIF_COEF_MENOR_3)
		recorre_objetivo->duracion_1 = duracion_efectiva / 1.2;
	  } // if (izq_der == IZQUIERDO)
	  else {
	    if (porcentaje_duracion > DUR_DIF_COEF_3)
	      recorre_objetivo->duracion_2 = 1.2*duracion_efectiva;
	    else
	      if (porcentaje_duracion < DUR_DIF_COEF_MENOR_3)
		recorre_objetivo->duracion_2 = duracion_efectiva / 1.2;
	  } // else // DERECHO
	  
	    
	}
    
    if (diferencia_potencia > ENERGIA_UMBRAL)
      validez |= 4;

#endif

    vectores_sintesis->validez = validez;

    if (izq_der == IZQUIERDO)
      izq_der = DERECHO;
    else {
      izq_der = IZQUIERDO;
      recorre_objetivo++;
    }

    vectores_sintesis++;

  }
  
  return 0;

}


/**
 * Función:   lee_ruta_fichero				                                  
 * \remarks Entrada:
 *			- fichero_txt: nombre del fichero de texto, con su ruta.		  
 *			- informacion: identificador de la información buscada.			  
 *			- fichero_configuracion: fichero con los datos del proyecto.      
 *	\remarks Salida:	- nombre_salida: nombre del fichero buscado.					  
 * \remarks Valor devuelto:															  
 *			En ausencia de error, devuelve un cero.							  
 */

int lee_ruta_fichero(char *fichero_txt, char *informacion, FILE *fichero_configuracion,
		     char *nombre_salida) {

  char nombre_local[FILENAME_MAX], *apunta_fichero, *nombre_sin_ruta, *elimina_extension = NULL;
  char linea[1000], parametro[100], ruta[500] = "", extension[100];

  fseek(fichero_configuracion, 0, SEEK_SET);

  while (fgets(linea, 1000, fichero_configuracion)) {

    apunta_fichero = (char *) linea;

    while ( (apunta_fichero = strstr(apunta_fichero, ",")) != NULL)
      *apunta_fichero++ = ' ';

    sscanf(linea, "%s,", parametro);

    if (strcmp(parametro, informacion) == 0) {

      sscanf(linea, "%s %s %s", parametro, ruta, extension);

      strcpy(nombre_local, fichero_txt);
      apunta_fichero = (char *) nombre_local;

      while ( (apunta_fichero = strstr(apunta_fichero, "\\")) != NULL)
	*apunta_fichero++= '/';

      apunta_fichero = (char *) nombre_local;
      nombre_sin_ruta = apunta_fichero;

      while ( (apunta_fichero = strstr(apunta_fichero, "/")) != NULL)
	nombre_sin_ruta = ++apunta_fichero;

      apunta_fichero = nombre_sin_ruta;

      while ( (apunta_fichero = strstr(apunta_fichero, ".")) != NULL)
	elimina_extension = apunta_fichero++;

      if (elimina_extension)
	*elimina_extension = '\0';
                
      sprintf(nombre_salida, "%s%s%s", ruta, nombre_sin_ruta, extension);

      return 0;



    } // if (strcmp(parametro, informacion) == 0)

  } // while (fgets(linea,...

  fprintf(stderr, "Error en lee_ruta_fichero: %s no encontrado en el fichero de entrada.\n",
	  informacion);

  return 1;

}


/**
 * Función:   lee_parametro_configuracion	                                  
 * \remarks Entrada:
 *			- informacion: identificador de la información buscada.			  
 *			- fichero_configuracion: fichero con los datos del proyecto.      
 *	\remarks Salida:	- parametro: parámetro buscado en el fichero de entrada.		  
 * \remarks Valor devuelto:															  
 *			En ausencia de error, devuelve un cero.							  
 */

int lee_parametro_configuracion(char *informacion, FILE *fichero_configuracion,
				char *parametro) {

  char linea[1000], auxiliar[100], *apunta_linea;

  fseek(fichero_configuracion, 0, SEEK_SET);

  while (fgets(linea, 1000, fichero_configuracion)) {

    apunta_linea = (char *) linea;

    while ( (apunta_linea = strstr(apunta_linea, ",")) != NULL)
      *apunta_linea++ = ' ';

    sscanf(linea, "%s", auxiliar);

    if (strcmp(auxiliar, informacion) == 0) {
      sscanf(linea, "%s %s", auxiliar, parametro);
      return 0;
    }
  }

  fprintf(stderr, "Error en lee_parametro_configuracion: %s no encontrado en el fichero de entrada.\n",
	  informacion);

  return 1;

}


/**
 * Función:   crea_nombre_fichero			                                  
 * \remarks Entrada:
 *			- indice1: equivalente numérico del 1er alófono según la cadena   
 *            indices_inversos.                                               
 *          - indice2: equivalente numérico del 2º alófono.                   
 *          - izq_der: IZQUIERDO o DERECHO.                                   
 *			- extension: extensión del nombre.								  
 * \remarks Entrada y Salida:                                                          
 *          - nombre: inicio del nombre. A la salida, el nombre completo.     
 * \remarks Valor devuelto:															  
 *			En ausencia de error, devuelve un cero.							  
 */

int crea_nombre_fichero(int indice1, int indice2, char izq_der, char *nombre,
			const char *extension) {

  char *alofono1, *alofono2;

  if ( (alofono1 = devuelve_puntero_a_nombre_fonema(indice1)) == NULL)
    return 1;

  if ( (alofono2 = devuelve_puntero_a_nombre_fonema(indice2)) == NULL)
    return 1;

  strcat(nombre, "_");

  strcat(nombre, alofono1);

  if (fonema_mayusculas(alofono1))
    strcat(nombre, "m_");
  else
    strcat(nombre, "_");

  strcat(nombre, alofono2);

  if (fonema_mayusculas(alofono2))
    strcat(nombre, "m");

  if (izq_der == IZQUIERDO)
    strcat(nombre, "_i");
  else
    strcat(nombre, "_d");

  strcat(nombre, extension);

  return 0;

}


/**
 * Función:   encuentra_indice_frase_en_fichero 	                             
 * \remarks Entrada:
 *			- nombre_fichero: nombre de un fichero del corpus.                   
 *			- fichero: puntero al fichero con los índices del inicio de cada	 
 *			  frase en el fichero de sonido único.								 
 *	\remarks Salida:	- indice_fichero: estructura con la información del nombre de la 	 
 *			  frase y su posición en el fichero único.							 
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error, devuelve un cero.							 
 */

int encuentra_indice_frase_en_fichero(char *nombre_fichero, FILE *fichero, tabla_ficheros *indice_fichero) {

  char delimitador[3] = "/\0";
  char nombre_local[FILENAME_MAX], *apunta_fichero, *anterior;
  tabla_ficheros correcaminos;

  strcpy(nombre_local, nombre_fichero);
  if ( (apunta_fichero = strstr(nombre_local, ".fon")) != NULL)
    *apunta_fichero = '\0';

  if (strstr(nombre_local, "\\") != NULL)
    delimitador[0] = '\\';
  else
    if (strstr(nombre_local, "/"))
      delimitador[0] = '/';

  apunta_fichero = (char *) nombre_local;
  anterior = apunta_fichero;

  while ( (apunta_fichero = strstr(apunta_fichero + 1, delimitador)) != NULL)
    anterior = apunta_fichero + 1;

  while (fread(&correcaminos, sizeof(tabla_ficheros), 1, fichero) == 1) {
    if (strstr(correcaminos.nombre_fichero, anterior) != NULL) {
      *indice_fichero = correcaminos;
      return 0;
    }
  }

  *indice_fichero = correcaminos;

  return 1;

}


/**
 * \author campillo
 * \brief Función: extrae_numero_fichero         		                             
 * \param[in] nombre_fichero nombre de un fichero del corpus.                   
 * \param[out] numero_fichero número de dicho fichero en el corpus.				 
 * \return En ausencia de error se devuelve un cero.                          
 */
int extrae_numero_fichero(char *nombre_fichero, int *numero_fichero) {

  char nombre_auxiliar[FILENAME_MAX];
  char *cadena1, *cadena2 = NULL;

  strcpy(nombre_auxiliar, nombre_fichero);

  cadena1 = strstr(nombre_auxiliar, "_");

  while (cadena1 != NULL) {
    cadena2 = cadena1++;
    cadena1 = strstr(cadena1, "_");
  }

  if (cadena2 == NULL) {
    fprintf(stderr, "Error en extrae_numero_fichero: formato de nombre de entrada (%s) no válido.\n", nombre_fichero);
    return 1;
  }
  else
    cadena1 = cadena2 + 1;

  if ( (cadena2 = strstr(cadena1, ".")) == NULL) {
    fprintf(stderr, "Error en extrae_numero_fichero: formato de nombre de entrada (%s) no válido.\n", nombre_fichero);
    return 1;
  } // if ( (cadena2 = ...


  *cadena2 = '\0';

  *numero_fichero = atoi(cadena1);

  return 0;

}


/**
 * Función:   convierte_fichero_unidades_bin                                     
 * \remarks Entrada:
 *			- fichero_txt: nombre del fichero en el que se indican los ficheros  
 *			  en los que están las unidades.                                     
 *	\remarks Salida:	- origen_unidades: cadena con el número del fichero en el que está   
 *			  cada unidad (indexada por el identificador de la unidad).			 
 *			- numero_unidades: número de elementos de la cadena anterior.		 
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 */

int convierte_fichero_unidades_bin(char *fichero_txt, map<int, int> *origen_unidades,
				   int *numero_unidades) {

  FILE *fichero_ent;
  int identificador;
  int num_unidades = 0;
  int numero_fichero;
  char cadena_fichero[FILENAME_MAX];

  if ( (fichero_ent = fopen(fichero_txt, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", fichero_txt);
    return 1;
  } // if ( (fichero_ent = ...

  origen_unidades->clear();
    

  while (!feof(fichero_ent)) {

    fscanf(fichero_ent, "%d %s\n", &identificador, cadena_fichero);

    if (extrae_numero_fichero(cadena_fichero, &numero_fichero))
      return 1;

    //		origen_unidades[identificador] = numero_fichero;
    origen_unidades->insert(make_pair(identificador, numero_fichero));
        
    num_unidades++;
        
    if (feof(fichero_ent))
      break;

  } // while(¡feof(fichero_ent))

  *numero_unidades = num_unidades;

  fclose(fichero_ent);

  return 0;

}


/**
 * Función:   escribe_cadena_unidades_escogidas                               
 * \remarks Entrada:
 *			- cadena_unidades: cadena de Vectores descriptores objetivo.      
 *          - unidades_escogidas: cadena de Vectores de síntesis.             
 *			- indice_fichero: número del fichero del Corpus al que se 		  
 *			  refieren las unidades.										  
 *          - numero_unidades: número de elementos de cadena_unidades.        
 *          - indice_menor: índice de la unidad del Corpus que se corresponde 
 *            con la primera unidad de la frase. (es decir, la original)      
 *	\remarks Salida:
 *			- fichero_unidades: fichero en el que se va escribiendo la        
 *            información de las unidades escogidas.                          
 * Valor_devuelto:                                                            
 *          - En ausencia de error se devuelve un cero.                       
 * \remarks NOTA:
 *			- Esta función sólo se emplea sobre el Corpus de 800 frases.      
 */

void escribe_cadena_unidades_escogidas(Vector_descriptor_objetivo *cadena_unidades,
                                       Vector_sintesis *unidades_escogidas,
                                       int indice_fichero,
                                       int numero_unidades, int indice_menor,
                                       FILE *fichero_unidades) {

  char izquierdo_derecho = IZQUIERDO;
  Vector_sintesis *apunta_sintesis = unidades_escogidas;
  Vector_descriptor_objetivo *apunta_objetivo = cadena_unidades;

  for (int contador = 0; contador < 2*numero_unidades; contador++, apunta_sintesis++) {

    if (izquierdo_derecho == IZQUIERDO)
      fprintf(fichero_unidades, "%s\t%s\t0\t", apunta_objetivo->contexto_izquierdo,
	      apunta_objetivo->semifonema);
    else
      fprintf(fichero_unidades, "%s\t%s\t1\t", apunta_objetivo->semifonema,
	      apunta_objetivo->contexto_derecho);

    fprintf(fichero_unidades, "%d\t%d\t%d\n", indice_fichero,
	    apunta_sintesis->devuelve_identificador(),
	    indice_menor);

    if (izquierdo_derecho == DERECHO) {
      izquierdo_derecho = IZQUIERDO;
      apunta_objetivo++;
      indice_menor++;
    } // if (izquierdo_derecho == ...
    else
      izquierdo_derecho = DERECHO;

  } // for (int contador = 0; ...

  
}


/**
   \fn actualiza_estadisticas_uso_unidades
   \param[in] unidades_escogidas array de unidades escogidas en la selección acústica
   \param[in] numero_unidades número de elementos del array anterior
   \param map_unidades_escogidas map con la información del número de veces que cada semifonema fue escogido
   \remarks Actualiza el map de unidades escogidas para hacer la síntesis
 */
 void actualiza_estadisticas_uso_unidades(Vector_sintesis *unidades_escogidas, int numero_unidades, map<string, map<int, int> > &map_unidades_escogidas) {
   
   string nombre_fonema;
   map<string, map<int, int> >::iterator it_fonema;
   map<int, int>::iterator it_unidad;

   for (int i = 0; i < numero_unidades; i++) {

     nombre_fonema = string(unidades_escogidas->semifonema);

     if ( (it_fonema = map_unidades_escogidas.find(nombre_fonema)) != map_unidades_escogidas.end()) {
       if ( (it_unidad = (it_fonema->second.find(unidades_escogidas->identificador))) != it_fonema->second.end())
	 it_unidad->second++;
       else 
	 it_fonema->second[unidades_escogidas->identificador] = 1;
     }
     else {
       map_unidades_escogidas[nombre_fonema] = map<int, int>();
       map_unidades_escogidas[nombre_fonema][unidades_escogidas->identificador] = 1;
     }

     unidades_escogidas++;

   }

 }

 /**
  * \fn escribe_map_uso_unidades
  * \param[in] map_unidades_escogidas map con la información del número de veces que cada semifonema fue escogido
  * \param[in] nombre_fichero nombre del fichero en el que se va a escribir la información
  * \remarks Escribe en un fichero la información del número de veces que cada unidad acústica del corpus fue utilizada
  * \return En ausencia de error devuelve un cero
  */
 int escribe_map_uso_unidades(map<string, map<int, int> > &map_unidades_escogidas, char *nombre_fichero) {


   FILE *fichero;

   if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
     fprintf(stderr, "Error al crear el fichero %s.\n", nombre_fichero);
     return 1;
   }

   for (map<string, map<int, int> >::iterator it_fonema = map_unidades_escogidas.begin(); it_fonema != map_unidades_escogidas.end(); ++it_fonema) {
     fprintf(fichero, "Fonema = %s.\n", it_fonema->first.c_str());
     for (map<int, int>::iterator it_unidad = it_fonema->second.begin(); it_unidad != it_fonema->second.end(); ++it_unidad)
       fprintf(fichero, "\t%d => %d\n", it_unidad->first, it_unidad->second);

   }

   fclose(fichero);



 }

/**
 * \brief Función que escribe la diferencia de f0 entre los objetivos buscados y las unidades escogidas
 * \param[in] objetivos vector de Vectores_descriptor_objetivo
 * \param[in] candidatos array de Vectores_sintesis
 * \param[in] numero_candidatos número de elementos del array anterior
 * \param[in] nombre_fichero nombre del fichero en el que se almacenan los resultados
 * \param[in] opciones opciones con las que se creará el fichero
 */

 int escribe_distancia_f0(vector<Vector_descriptor_objetivo> &objetivos, Vector_sintesis *candidatos, int numero_candidatos, char *nombre_fichero, const char *opciones) {

  char izquierdo_derecho = IZQUIERDO;
  float diferencia_frec_inicial, diferencia_frec_final;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = objetivos.begin();
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_distancia_f0, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = fopen(nombre_fichero...

  for (int contador = 0; contador < numero_candidatos; contador++, candidatos++) {
    
    if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {
      
      if (izquierdo_derecho == DERECHO) {

	diferencia_frec_inicial = absoluto(recorre_objetivo->frecuencia_mitad - candidatos->frecuencia_inicial);
	diferencia_frec_final = absoluto(recorre_objetivo->frecuencia_derecha - candidatos->frecuencia_final);

      } // if (izquierdo_derecho == DERECHO)
      else { // IZQUIERDO

	diferencia_frec_inicial = absoluto(recorre_objetivo->frecuencia_izquierda - candidatos->frecuencia_inicial);
	diferencia_frec_final = absoluto(recorre_objetivo->frecuencia_mitad - candidatos->frecuencia_final);

      } // else

      fprintf(fichero, "%f\t%f\t", diferencia_frec_inicial, diferencia_frec_final);
    } // if (sordo_sonoro(...

    if (izquierdo_derecho == DERECHO) {
      recorre_objetivo++;
      izquierdo_derecho = IZQUIERDO;
    }
    else {
      izquierdo_derecho = DERECHO;

    }
  }

  fclose(fichero);

  return 0;

}

/**
 * \brief Función que escribe en un fichero el coste de una secuencia de unidades
 * \param[in] nombre_fichero nombre de un fichero de texto
 * \param[in] coste coste de la secuencia
 * \param[in] indice_contorno índice del contorno escogido
 * \param[in] opciones opciones para la creación del fichero
 * \return En ausencia de error devuelve un cero
 */
int escribe_coste(char *nombre_fichero, float coste, int indice_contorno, const char *opciones) {

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_coste, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  fprintf(fichero, "%f\t%d\n", coste, indice_contorno);

  fclose(fichero);

  return 0;

}

/**
 * \brief Función que escribe en un fichero el coste de una secuencia de unidades
 * \param[in] nombre_fichero nombre de un fichero de texto
 * \param[in] coste coste de la secuencia
 * \param[in] caminos_escogidos vector con los caminos escogidos para cada grupo fónico
 * \param[in] opciones opciones para la creación del fichero
 * \return En ausencia de error devuelve un cero
 */
int escribe_coste(char *nombre_fichero, float coste, vector<int> &caminos_escogidos, const char *opciones) {

  FILE *fichero;
  vector<int>::iterator it = caminos_escogidos.begin();

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_coste, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  fprintf(fichero, "%f", coste);

  for (; it != caminos_escogidos.end(); ++it)
    fprintf(fichero, "\t%d", *it);

  fclose(fichero);

  return 0;

}

/**
 * Función:   devuelve_indices_menor_y_mayor                                  
 * \remarks Entrada:
 *			- numero_frase: número de la frase dentro del Corpus.             
 *          - nombre_fichero: nombre del fichero de origen de las unidades.   
 *	\remarks Salida:
 *			- identificador_menor: identificador de la primera unidad de la   
 *            frase.                                                          
 *          - identificador_mayor: identificador de la última unidad de dicha 
 *            frase.                                                          
 * Valor_devuelto:                                                            
 *          - En ausencia de error se devuelve un cero.                       
 * \remarks NOTA:
 *			- Esta función sólo se emplea sobre el Corpus de 800 frases.      
 */

int devuelve_indices_menor_y_mayor(int numero_frase, char *nombre_fichero,
                                   int *indice_menor, int *indice_mayor) {
  FILE *fichero;
  char nombre_buscado[20], ruta_fichero[200];

  if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
    fprintf(stderr, "Error en devuelve_indices_menor_y_mayor, al intentar abrir el fichero %s.\n",
	    nombre_fichero);
    return 1;
  } // if ( (fichero = fopen...

  sprintf(nombre_buscado, "fre800_%d.", numero_frase);

  while (!feof(fichero)) {
    fscanf(fichero, "%d %s\n", indice_menor, ruta_fichero);
    if (strstr(ruta_fichero, nombre_buscado) != NULL)
      break;
  } // while (!feof(fichero))

  if (feof(fichero)) {
    fprintf(stderr, "Error en devuelve_indices_menor_y_mayor: Índice menor no encontrado.\n");
    fclose(fichero);
    return 1;
  } // if (feof(fichero))

  while (!feof(fichero)) {
    fscanf(fichero, "%d %s\n", indice_mayor, ruta_fichero);
    if (strstr(ruta_fichero, nombre_buscado) == NULL)
      break;
  } // while (!feof(fichero)

  if (feof(fichero) && (numero_frase != 807) ) {
    fprintf(stderr, "Error en devuelve_indices_menor_y_mayor: Fin de fichero no esperado.\n");
    fclose(fichero);
    return 1;
  } // if (feof(fichero) && ...

  if (numero_frase != 807)
    (*indice_mayor)--;

  fclose(fichero);
    
  return 0;

}

/**
 * Función:   extrae_identificador                                            
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero donde se encuentra la unidad.
 *	\remarks Salida:
 *			- identificador: índice del fichero.                              
 */

void extrae_identificador(char *nombre_fichero, int *identificador) {

  char *auxiliar;

  if ( (auxiliar = strstr(nombre_fichero, "_")) == NULL)
    auxiliar = strstr(nombre_fichero, "-");

  if (auxiliar != NULL) {
    auxiliar++;
    *identificador = atoi(auxiliar);
  }
  else 
    *identificador = 1;  // Por poner algo.

}


/**
 * Función:   obten_unidad_de_nombre_fichero                                  
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero en el que se encuentra la    
 *            información. Debe tener el formato <nom_(1)_(2)_(I/D).bin>,     
 *            donde nom es cualquier cadena alfanumérica, (1) es el primer    
 *            fonema, (2) es el segundo, y (I/D) es i o d, según sea una      
 *            unidad con contexto por la izquierda o por la derecha,          
 *            respectivamente.                                                
 *	\remarks Salida:
 *			- primer_fonema: nombre del primer fonema de la unidad.           
 *          - segundo_fonema: nombre del segundo fonema de la unidad.         
 *          - izq_der: contexto por la derecha o por la izquierda.            
 * \remarks Valor devuelto:                                                            
 *          - Si el formato es correcto, devuelve un 0.                       
 */

int obten_unidad_de_nombre_fichero(char *nombre_fichero, char primer_fonema[],
                                   char segundo_fonema[], unsigned char *izq_der) {

  char *aux1, *aux2;

  for (int contador = 1; contador < 3; contador++) {
    primer_fonema[contador] = 0;
    segundo_fonema[contador] = 0;
  }

  if ( (aux1 = strstr(nombre_fichero, "\\")) != NULL) {
    aux1++;
    if ( (aux1 = strstr(aux1, "_")) == NULL) {
      fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
      return 1;
    }
  }
  else
    if ( (aux1 = strstr(nombre_fichero, "_")) == NULL) {
      fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
      return 1;
    }

  if ( (aux2 = strstr(++aux1, "_")) == NULL) {
    fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
    return 1;
  }

  strncpy(primer_fonema, aux1, aux2 - aux1);

  ++aux2;

  if (aux1[1] != '_') {
    if (aux1[1] == 'm') // Ojo esto sólo sirve para el juego de caracteres actual.
      primer_fonema[1] = '\0';
    else
      if ( (aux1[1] != 'S') && (aux1[1] != 'r') ) {
	fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
	return 1;
      }
  }

  if ( (aux1 = strstr(aux2, "_")) == NULL) {
    fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
    return 1;
  }

  strncpy(segundo_fonema, aux2, aux1 - aux2);

  ++aux1;

  if (aux2[1] != '_') {
    if (aux2[1] == 'm') // Ojo esto sólo sirve para el juego de caracteres actual.
      segundo_fonema[1] = '\0';
    else
      if ( (aux2[1] != 'S') && (aux2[1] != 'r') ) {
	fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre del fichero.\n");
	return 1;
      }
  }

  if (*aux1 == 'd')
    *izq_der = DERECHO;
  else
    if (*aux1 == 'i')
      *izq_der = IZQUIERDO;
    else {
      fprintf(stderr, "Error en obten_unidad_de_nombre_fichero. Formato incorrecto del nombre de fichero.\n");
      return 1;
    }

  return 0;

}


/**
 * Función:   carga_unidades_en_memoria                                       
 * \remarks Entrada:
 *			- fichero: fichero binario en el que se listan los archivos de    
 *            unidades, tal y como se obtiene al hacer un dir > [fichero] en  
 *            entorno MS-DOS, eliminándole la información no relativa a dichos
 *            archivos (es decir, las 6 primeras líneas y las 2 últimas).     
 *            Se convierte a formato binario mediante el proyecto             
 *            convierte_ficheros_unidades_bin.bpr.                            
 *          - locutor: objeto de tipo locutor en el que se van a cargar los   
 *			  datos del Corpus de voz.										  	
 */
/*
  int carga_unidades_en_memoria(char * dir_bd, FILE *fichero, Locutor *locutor) {

  estructura_unidades *correcaminos;
  Vector_semifonema_candidato *puntero_vector;
  int contador;
  int indice1, indice2;
  register int numero_unidades;
  FILE *fichero_unico;
  char cadena_lectura[TAMANO_MAXIMO_FICHERO];
  char *cadena_datos;
  register int memoria_puntero_a_unidades = TIPOS_UNIDADES*sizeof(estructura_indices);
  ficheros_unidades *unidades, *recorre;
  int cuenta_unidades;
  indice_ficheros *indice_a_fichero;
  indice_ficheros indices_en_fichero[NUMERO_FONEMAS][NUMERO_FONEMAS][2];
  char nombre_fichero[FILENAME_MAX];

  //  inicializa_tabla_unidades();

  fread(&cuenta_unidades, sizeof(int), 1, fichero);

  if ( (unidades = (ficheros_unidades *) malloc(cuenta_unidades*sizeof(ficheros_unidades))) == NULL) {
  fprintf(stderr, "Error en carga_unidades_en_memoria, al asignar memoria.\n");
  return 1;
  }

  fread(unidades, sizeof(ficheros_unidades), cuenta_unidades, fichero);

  recorre = unidades;

  locutor->numero_unidades_voz = cuenta_unidades;

  sprintf(nombre_fichero, "%s%s\\%s", dir_bd, locutor->directorio, FICHERO_CORPUS_VOZ);

  if ( (fichero_unico = fopen(nombre_fichero,"rb")) == NULL) {
  fprintf(stderr, "Error al intentar abrir el fichero de unidades.\n");
  return 1;
  }

  fread(indices_en_fichero, sizeof(indice_ficheros), NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero_unico);

  while(cuenta_unidades--) {

  if ( ( (indice1 = devuelve_indice_fonema(recorre->primer_fonema)) == -1) ||
  ( (indice2 = devuelve_indice_fonema(recorre->segundo_fonema)) == -1) ) {
  fprintf(stderr, "Error en carga_unidades_en_memoria: Fichero de unidades no válido.\n");
  return 1;
  }

  if (recorre->izq_der == IZQUIERDO) {
  if ( (locutor->tabla_semifonemas_i[indice1][indice2] = (estructura_unidades *)
  malloc(sizeof(estructura_unidades))) == NULL) {
  fprintf(stderr, "Error en carga_unidades_en_memoria, al asignar memoria.\n");
  return 1;
  }
  correcaminos = locutor->tabla_semifonemas_i[indice1][indice2];
  indice_a_fichero = &indices_en_fichero[indice1][indice2][0];
  } // if (recorre->izq_der...
  else {
  if ( (locutor->tabla_semifonemas_d[indice1][indice2] = (estructura_unidades *)
  malloc(sizeof(estructura_unidades))) == NULL) {
  fprintf(stderr, "Error en carga_unidades_en_memoria, al asignar memoria.\n");
  return 1;
  }
  correcaminos = locutor->tabla_semifonemas_d[indice1][indice2];
  indice_a_fichero = &indices_en_fichero[indice1][indice2][1];
  } // else { if (recorre

  fread(cadena_lectura, sizeof(char), indice_a_fichero->tamano, fichero_unico);

  if ( (cadena_datos = (char *) malloc(indice_a_fichero->tamano - memoria_puntero_a_unidades - sizeof(int))) == NULL) {
  fprintf(stderr, "Error en carga_unidades_en_memoria, al asignar memoria.\n");
  return 1;
  }

  memcpy(correcaminos->puntero_a_unidades, cadena_lectura, memoria_puntero_a_unidades);
  memcpy(&correcaminos->numero_unidades, cadena_lectura + memoria_puntero_a_unidades, sizeof(int));

  memcpy(cadena_datos, cadena_lectura + memoria_puntero_a_unidades + sizeof(int),
  indice_a_fichero->tamano - memoria_puntero_a_unidades - sizeof(int));

  numero_unidades = correcaminos->numero_unidades;

  correcaminos->unidades = (Vector_semifonema_candidato *) cadena_datos;

  cadena_datos += numero_unidades*sizeof(Vector_semifonema_candidato);

  puntero_vector = correcaminos->unidades;

  for (contador = 0; contador < numero_unidades; contador++) {
  puntero_vector->lee_datos_parte_dinamica(&cadena_datos);
  puntero_vector++;
  }

  recorre++;
  }

  fclose(fichero_unico);
  free(unidades);

  return 0;

  }
*/

/**
 * Función:   crea_nombre_fichero
 * \remarks Entrada:
 *			- primero: primer fonema.
 *          - segundo: segundo fonema.
 *          - opcion: IZQUIERDO o DERECHO.
 *          - path: path del fichero cuyo nombre se desea crear.
 *          - extension: extensión del fichero.
 *	\remarks Salida:
 *			- nombre: nombre del fichero que va a contener la unidad.
 */

void crea_nombre_fichero(char nombre[], char primero[], char segundo[],
                         char opcion, char path[], const char *extension) {

  strcpy(nombre, path);

  strcat(nombre, primero);

  if (fonema_mayusculas(primero))
    strcat(nombre, "m_");
  else
    strcat(nombre, "_");

  strcat(nombre, segundo);

  if (fonema_mayusculas(segundo))
    strcat(nombre, "m_");
  else
    strcat(nombre, "_");

  opcion == IZQUIERDO ? strcat(nombre, "i") : strcat(nombre, "d");

  strcat(nombre, extension);
}


/**
 * Función:    devuelve_puntero_a_unidades                                     
 * \remarks Entrada:
 *			- primero: primer fonema.                                         
 *           - segundo: segundo fonema.                                        
 *           - izq_der: IZQUIERDO o DERECHO                                    
 *			 - locutor: objeto de tipo locutor, en el que está almacenada la   
 *			   información del Corpus de voz.								   
 * \remarks Valor devuelto:                                                             
 *           - Se devuelve el puntero a la cadena de unidades del tipo         
 *             seleccionado.                                                   
 */

estructura_unidades *devuelve_puntero_a_unidades(const char *primero, const char *segundo, char izq_der, Locutor *locutor) {

  int indice_1, indice_2;

  if ( ( (indice_1 = devuelve_indice_fonema(primero)) == -1) ||
       ( (indice_2 = devuelve_indice_fonema(segundo)) == -1) ) {
    fprintf(stderr, "Error en devuelve_puntero_a_unidades: Fonemas no existentes.\n");
    return NULL;
  }

  if (izq_der == IZQUIERDO)
    return locutor->tabla_semifonemas_i[indice_1][indice_2];
  else
    return locutor->tabla_semifonemas_d[indice_1][indice_2];

}


/**
 * Función:    devuelve_puntero_a_unidades                                     
 * \remarks Entrada:
 *			- primero: primer fonema.                                         
 *           - segundo: segundo fonema.                                        
 *           - izq_der: IZQUIERDO o DERECHO                                    
 * \remarks Valor devuelto:                                                             
 *           - Se devuelve el puntero a la cadena de unidades del tipo         
 *             seleccionado.                                                   
 */

estructura_unidades *devuelve_puntero_a_unidades(const char *primero, const char *segundo, char izq_der) {

  int indice_1, indice_2;

  if ( ( (indice_1 = devuelve_indice_fonema(primero)) == -1) ||
       ( (indice_2 = devuelve_indice_fonema(segundo)) == -1) ) {
    fprintf(stderr, "Error en devuelve_puntero_a_unidades: Fonemas no existentes.\n");
    return NULL;
  }

  if (izq_der == IZQUIERDO)
    return tabla_unidades_i[indice_1][indice_2];
  else
    return tabla_unidades_d[indice_1][indice_2];

}


/**
 * \fn devuelve_puntero_a_unidades_fonemas
 * \param[in] fonema fonema del que se quieren obtener sus realizaciones
 * \param[in] locutor objeto de tipo Locutor con la información de las unidades acústicas disponibles
 * \param[out] vector_unidades Vector en el que se incluyen dichas realizaciones
 * \remarks Devuelve todas las realizaciones de un fonema que hay en la base de datos
 * \return En ausencia de error, devuelve un cero
 */
 
 int devuelve_vector_unidades_fonemas(char fonema[],  Locutor *locutor, vector<Vector_semifonema_candidato *> &vector_unidades) {

   int indice_fonema, indice_unidad;
   char *contexto;

   estructura_unidades *unidades;

   //   set<int> unidades_incluidas;

   //   unidades_incluidas.clear();

   // Primero la parte izquierda:

   for (indice_fonema = 0; indice_fonema < NUMERO_FONEMAS; indice_fonema++) {

     if ( (contexto = devuelve_puntero_a_nombre_fonema(indice_fonema)) == NULL) {
       fprintf(stderr, "Error: el índice %d no se corresponde con ningún fonema válido (ver indices_inversos en fonemas.cpp).\n", indice_fonema);
       return 1;
     }

     if ( (unidades = devuelve_puntero_a_unidades(contexto, fonema, IZQUIERDO, locutor)) != NULL) {

       for (indice_unidad = 0; indice_unidad < unidades->numero_unidades; indice_unidad++) {

	 //	   unidades_incluidas.insert(unidades->unidades[indice_unidad].identificador);
	   vector_unidades.push_back(&unidades->unidades[indice_unidad]);

       } // for (indice_unidad =
       
     } // if ( (unidades = devuelve...

   } // for (indice_fonema = 0...


   // Luego, la derecha.

   for (indice_fonema = 0; indice_fonema < NUMERO_FONEMAS; indice_fonema++) {

     if ( (contexto = devuelve_puntero_a_nombre_fonema(indice_fonema)) == NULL) {
       fprintf(stderr, "Error: el índice %d no se corresponde con ningún fonema válido (ver indices_inversos en fonemas.cpp).\n", indice_fonema);
       return 1;
     }

     if ( (unidades = devuelve_puntero_a_unidades(fonema, contexto, DERECHO, locutor)) != NULL) {

       for (indice_unidad = 0; indice_unidad < unidades->numero_unidades; indice_unidad++) {
	 //	 if (unidades_incluidas.find(unidades->unidades[indice_unidad].identificador) == unidades_incluidas.end()) {
	 //	   unidades_incluidas.insert(unidades->unidades[indice_unidad].identificador);
	   vector_unidades.push_back(&unidades->unidades[indice_unidad]);

	   //	 } // if (unidades_incluidas.find...

       } // for (indice_unidad =
       
     } // if ( (unidades = devuelve...

   } // for (indice_fonema = 0...


   
   return 0;

 }
   

/**
 * Función:    crea_cadena_unidades_candidatas                                 
 * \remarks Entrada:
 *			- unidad: cadena indexada de unidades de un mismo tipo.           
 *           - frase: tipo de frase a la que pertenece la unidad.              
 *           - posicion: posición de la unidad dentro de la frase.             
 *           - acento: tonicidad de la unidad.                                 
 *	\remarks Salida:
 *			- tamano: número de unidades de la cadena.                        
 * \remarks Valor devuelto:                                                             
 *           - Se devuelve un puntero a la cadena de unidades del tipo         
 *             seleccionado. Si hay algún error, se devuelve NULL.             
 */
#ifdef _WIN32
#pragma argsused
#endif
Vector_semifonema_candidato **crea_cadena_unidades_candidatas(estructura_unidades *unidad, unsigned char frase, unsigned char posicion, unsigned char acento, int *tamano, char tipo_candidatos) {
  Vector_semifonema_candidato **puntero_unidades;
  Vector_semifonema_candidato **recorre;
  Vector_semifonema_candidato *recorre_unidades;
  estructura_indices indice_a_tipo;

  int numero_unidades, numero_unidades_final;
  int contador;

#ifdef _DIFONEMAS_VARIOS_CONTORNOS

  *tamano = 0;

  numero_unidades = unidad->numero_unidades;

  if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*sizeof(Vector_semifonema_candidato *))) == NULL) {
    fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
    return NULL;
  }

  recorre = puntero_unidades;
  recorre_unidades = unidad->unidades;

  for (contador = 0; contador < numero_unidades; contador++, recorre_unidades++)
    if (recorre_unidades->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS) {
      *recorre++ = recorre_unidades;
      (*tamano)++;
    }

  if (*tamano != 0)
    return puntero_unidades;
  else
    free(puntero_unidades);

#endif

  if (tipo_candidatos == 0) {

    numero_unidades = unidad->numero_unidades;

    if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*
								     sizeof(Vector_semifonema_candidato *))) == NULL) {
      fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
      return NULL;
    }
    
    recorre = puntero_unidades;
    recorre_unidades = unidad->unidades;

    // Incluimos todas menos las que tienen distinto carácter tónico:

    numero_unidades_final = 0;
    for (contador = 0; contador < numero_unidades; contador++, recorre_unidades++) {
      if (recorre_unidades->acento == acento) {
	*recorre++ = recorre_unidades;
	numero_unidades_final++;
      }

    }

    if (numero_unidades_final == 0) { // Incluimos todas

      recorre_unidades = unidad->unidades;
      for (contador = 0; contador < numero_unidades; contador++, recorre++, recorre_unidades++)	  
	*recorre = recorre_unidades;

      numero_unidades_final = numero_unidades;

    }

    *tamano = numero_unidades_final;
    
    return puntero_unidades;
  }

  // Tipo_candidatos = 1 => Sólo cogemos las unidades del tipo adecuado.

  indice_a_tipo = unidad->puntero_a_unidades[conjunto_unidades (frase, posicion, acento)];
  
  if (frase == FRASE_ENUNCIATIVA) {
    
    if ( (numero_unidades = indice_a_tipo.numero_unidades) < 10) { // Antes, cero.
      
      numero_unidades = unidad->numero_unidades;
      
      if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*
								       sizeof(Vector_semifonema_candidato *))) == NULL) {
	fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
	return NULL;
      }
      
      recorre = puntero_unidades;
      recorre_unidades = unidad->unidades;
      
      for (contador = 0; contador < numero_unidades; contador++, recorre++, recorre_unidades++)
	*recorre = recorre_unidades;
      
      *tamano = numero_unidades;
      
      return puntero_unidades;
      
    } // if ( (numero_unidades = indice_a_tipo.numero_unidades) == 0)
    
    if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*sizeof(Vector_semifonema_candidato *))) == NULL) {
      fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
      return NULL;
    }
    
    recorre = puntero_unidades;
    recorre_unidades = &unidad->unidades[indice_a_tipo.posicion];
    *tamano = numero_unidades;
    
    for (contador = 0; contador < numero_unidades; contador++, recorre++, recorre_unidades++)
      *recorre = recorre_unidades;
    
  } // if (frase == FRASE_ENUNCIATIVA)
  else {
    estructura_indices indice_enunciativo =
      unidad->puntero_a_unidades[conjunto_unidades(FRASE_ENUNCIATIVA, posicion, acento)];
    
    numero_unidades = indice_a_tipo.numero_unidades + indice_enunciativo.numero_unidades;
    
    if (numero_unidades != 0) {
      
      if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*
								       sizeof(Vector_semifonema_candidato *))) == NULL) {
	fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
	return NULL;
      }

      *tamano = numero_unidades;
      recorre = puntero_unidades;

      if (indice_a_tipo.numero_unidades != 0) {

	recorre_unidades = &unidad->unidades[indice_a_tipo.posicion];

	for (contador = 0; contador < indice_a_tipo.numero_unidades;
	     contador++, recorre++, recorre_unidades++)
	  *recorre = recorre_unidades;

      } // if (indice_a_tipo.numero_unidades != 0)

      if (indice_enunciativo.numero_unidades != 0) {

	recorre_unidades = &unidad->unidades[indice_enunciativo.posicion];

	for (contador = 0; contador < indice_enunciativo.numero_unidades;
	     contador++, recorre++, recorre_unidades++)
	  *recorre = recorre_unidades;

      } // if (indice_enunciativo.numero_unidades != 0)
            
    } // if (numero_unidades != 0)
    else {

      numero_unidades = unidad->numero_unidades;

      if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*
								       sizeof(Vector_semifonema_candidato *))) == NULL) {
	fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
	return NULL;
      }

      recorre = puntero_unidades;
      recorre_unidades = unidad->unidades;

      for (contador = 0; contador < numero_unidades; contador++, recorre++, recorre_unidades++)
	*recorre = recorre_unidades;

      *tamano = numero_unidades;

      return puntero_unidades;

    } // else

  } // else

    // No devolvemos directamente el puntero a la cadena original porque puede ser necesario realizar
    // una poda posterior.

  return puntero_unidades;

}

/**
 * Función:    crea_cadena_unidades_candidatas_tonicidad                       
 * \remarks Entrada:
 *			- unidad: cadena indexada de unidades de un mismo tipo.           
 *           - acento: tonicidad de la unidad.                                 
 *	\remarks Salida:
 *			- tamano: número de unidades de la cadena.                        
 * \remarks Valor devuelto:                                                             
 *           - Se devuelve un puntero a la cadena de unidades del tipo         
 *             seleccionado. Si hay algún error, se devuelve NULL.             
 * \remarks NOTA:	 - Se diferencia de la función anterior en que, en este caso, se   
 *			   devuelven todas aquellas unidades de ese semifonema que tienen  
 *			   el mismo carácter tónico que la buscada.						   
 */
#ifdef _WIN32
#pragma argsused
#endif
Vector_semifonema_candidato **crea_cadena_unidades_candidatas_tonicidad(estructura_unidades *unidad, unsigned char acento, int *tamano) {
  Vector_semifonema_candidato **puntero_unidades;
  Vector_semifonema_candidato **recorre;
  Vector_semifonema_candidato *recorre_unidades;

  int numero_unidades = unidad->numero_unidades;
  int cuenta, contador = 0;

  if ( (puntero_unidades = (Vector_semifonema_candidato **) malloc(numero_unidades*
								   sizeof(Vector_semifonema_candidato *))) == NULL) {
    fprintf(stderr, "Error en crea_cadena_unidades_candidatas, al asignar memoria.\n");
    return NULL;
  }

  recorre_unidades = unidad->unidades;

  recorre = puntero_unidades;

  for (cuenta = 0; cuenta  < numero_unidades; cuenta++, recorre_unidades++)

    if (recorre_unidades->acento == acento) {

      *recorre = recorre_unidades;
      contador++;
      recorre++;
    } // if (recorre_unidades->acento == acento)


  if (contador == 0) {

    recorre_unidades = unidad->unidades;

    *tamano = numero_unidades;

    for (cuenta = 0; cuenta < numero_unidades; cuenta++, recorre++, recorre_unidades++)
      *recorre = recorre_unidades;

  } // if (contador == 0)
  else
    *tamano = contador;


  // No devolvemos directamente el puntero a la cadena original porque puede ser necesario realizar
  // una poda posterior.

  return puntero_unidades;

}


/**
 * Función:   decide_siguiente_tipo_unidad                                        
 * \remarks Entrada:
 *			- inicio_busqueda: variable que indica si es, para una unidad, la     
 *            primera vez que se accede a esta función. Sirve para evitar bucles. 
 * \remarks Entrada y Salida:                                                              
 *          - tipo_frase: tipo de frase de la unidad.                             
 *          - posicion: posición de la unidad en la frase.                        
 *          - acento: acento de la sílaba en la que está la unidad.               
 * \remarks Valor devuelto:                                                                
 *            Devuelve cero mientras haya más posibilidades para esa unidad.      
 */
/*
  int decide_siguiente_tipo_unidad(char inicio_busqueda, unsigned char *tipo_frase,
  unsigned char *posicion, unsigned char *acento) {

  return 0;
  }
*/



//#ifdef _MODO_NORMAL

/**
 * Función:   convierte_a_cadena_vectores                                         
 * \remarks Entrada:
 *			- inicio: cadena indexada de unidades en el formato de la estructura  
 *            matriz.                                                             
 *          - tamano: número de unidades de la cadena anterior.                   
 *	\remarks Salida:
 *			- cadena_salida: cadena indexada de Vectores_sintesis.                
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error se devuelve un cero.                           
 */

int convierte_a_cadena_vectores(estructura_matriz *inicio, int tamano,
                                Vector_sintesis **cadena_salida) {

  Vector_sintesis *correcaminos;
  int contador;

  if (*cadena_salida == NULL) {
    if ( (correcaminos = (Vector_sintesis *) malloc(tamano*sizeof(Vector_sintesis)))
	 == NULL) {
      fprintf(stderr, "Error en convierte_a_cadena_vectores, al asignar memoria.\n");
      return 1;
    }

    *cadena_salida = correcaminos;
  }
  else
    correcaminos = *cadena_salida;

  for (contador = 0; contador < tamano; contador++, inicio++) {

    if (correcaminos->crea_vector_sintesis(inicio->localizacion.vector,
					   (char) inicio->parametro_1, inicio->parametro_2))
      return 1;

    correcaminos++;

  }

  return 0;

}


//#endif


/**
 * Función:   convierte_a_cadena_vectores                                         
 * \remarks Entrada:
 *			- inicio: cadena indexada de unidades en el formato de la estructura  
 *            matriz.                                                             
 *          - tamano: número de unidades de la cadena anterior.                   
 *	\remarks Salida:
 *			- cadena_salida: cadena indexada de Vectores_sintesis.                
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error se devuelve un cero.                           
 * \remarks NOTA:      Versión sobrecargada de la anterior, que devuelve una cadena de     
 *            vectores candidato.                                                 
 */

int convierte_a_cadena_vectores(estructura_matriz *inicio, int tamano,
                                Vector_semifonema_candidato **cadena_salida) {

  Vector_semifonema_candidato *correcaminos;
  int contador;

  if (*cadena_salida == NULL) {
    if ( (correcaminos = (Vector_semifonema_candidato *) malloc(tamano*sizeof(Vector_semifonema_candidato)))
	 == NULL) {

      fprintf(stderr, "Error en convierte_a_cadena_vectores, al asignar memoria.\n");
      return 1;
    }

    *cadena_salida = correcaminos;
  }
  else
    correcaminos = *cadena_salida;

  for (contador = 0; contador < tamano; contador++, inicio++) {

    if (correcaminos->copia_contenido(*inicio->localizacion.vector))
      return 1;

    correcaminos++;

  }

  return 0;

}


/**
 * Función:   convierte_a_cadena_acentuales                                       
 * \remarks Entrada:
 *			- inicio: cadena indexada de unidades en el formato de la estructura  
 *            matriz.                                                             
 *          - tamano: número de unidades de la cadena anterior.                   
 *	\remarks Salida:
 *			- cadena_salida: cadena indexada de Grupos_acentuales_candidatos.     
 * \remarks Valor devuelto:                                                                
 *          - En ausencia de error se devuelve un cero.                           
 */

int convierte_a_cadena_acentuales(estructura_matriz *inicio, int tamano,
				  Grupo_acentual_candidato **cadena_salida) {


  Grupo_acentual_candidato *correcaminos;
  int contador;// , identificador;


  if (*cadena_salida == NULL) {

    if ( (correcaminos = (Grupo_acentual_candidato *) malloc(tamano*sizeof(Grupo_acentual_candidato)))
	 == NULL) {

      fprintf(stderr, "Error en convierte_a_cadena_acentuales, al asignar memoria.\n");
      return 1;
    }

    *cadena_salida = correcaminos;

  } // if (*cadena_salida == NULL)
  else
    correcaminos = *cadena_salida;

  for (contador = 0; contador < tamano; contador++, inicio++, correcaminos++)
    //    memcpy(correcaminos, inicio->localizacion.grupo, sizeof(Grupo_acentual_candidato));
    if (correcaminos->copia_grupo_acentual(inicio->localizacion.grupo))
      return 1;

  return 0;

}


/**
 * \brief Función que convierte un camino óptimo resultante de la ejecución del algoritmo de Viterbi acentual, en una secuencia de grupos acentuales.
 * \remarks Se emplea en el cálculo de contornos por grupos fónicos.
 * \param[in] inicio cadena indexada de unidades en el formato de la estructura matriz.                                                             
 * \param[in] tamano Número de unidades de la cadena anterior.                   * \param[in] tamano_grupos_fonicos Vector con el tamaño de los diferentes grupos fónicos (en grupos acentuales).
 * \param[out] cadena_salida Cadena indexada de Grupos_acentuales_candidatos.    * \param[out] numero_grupos_acentuales Número de grupos acentuales de la cadena anterior.
 * \return En ausencia de error se devuelve un cero.                           
 */

int convierte_a_cadena_acentuales_GF(estructura_matriz *inicio, vector<int> &tamano_grupos_fonicos, int tamano, Grupo_acentual_candidato **cadena_salida, int *numero_grupos_acentuales) {


  Grupo_acentual_candidato *correcaminos;
  int contador;// , identificador;
  int tamano_grupos_acentuales = 0;

  // Calculamos el número total de grupos acentuales:
  vector<int>::iterator it_grupo_fonico = tamano_grupos_fonicos.begin();

  while (it_grupo_fonico != tamano_grupos_fonicos.end()) {
    tamano_grupos_acentuales += *it_grupo_fonico;
    it_grupo_fonico++;
  }

  *numero_grupos_acentuales = tamano_grupos_acentuales;

  if (*cadena_salida == NULL) {

    if ( (correcaminos = (Grupo_acentual_candidato *) malloc(tamano_grupos_acentuales*sizeof(Grupo_acentual_candidato)))
	 == NULL) {

      fprintf(stderr, "Error en convierte_a_cadena_acentuales, al asignar memoria.\n");
      return 1;
    }

    *cadena_salida = correcaminos;

  } // if (*cadena_salida == NULL)
  else
    correcaminos = *cadena_salida;

  for (contador = 0; contador < tamano; contador++, inicio++)
    //    memcpy(correcaminos, inicio->localizacion.grupo, sizeof(Grupo_acentual_candidato));
    for (int cuenta = 0; cuenta < tamano_grupos_fonicos[contador]; cuenta++, correcaminos++) {
      if (correcaminos->copia_grupo_acentual(inicio->localizacion.grupo + cuenta))
	return 1;
      
    }

  return 0;

}


/**
 * Función:    convierte_difonema_a_candidato                                  
 * \remarks Entrada:
 *			- difonemas: cadena indexada de vectores_difonema                 
 */
Vector_semifonema_candidato **convierte_difonema_a_candidato(Vector_difonema *difonemas,
							     int tamano, char izq_der) {

  Vector_semifonema_candidato **salida, **recorre, *recorre_unidades;


  if ( (salida = (Vector_semifonema_candidato **) malloc(tamano
							 *sizeof(Vector_semifonema_candidato *))) == NULL) {
    fprintf(stderr, "Error en convierte_difonema_a_candidato, al asignar memoria.\n");
    return NULL;
  }

  recorre = salida;

  for (int contador = 0; contador < tamano; contador++, recorre++) {

    if ( (recorre_unidades = (Vector_semifonema_candidato *) malloc(sizeof(
									   Vector_semifonema_candidato))) == NULL) {
      fprintf(stderr, "Error en convierte_difonema_a_candidato, al asignar memoria.\n");
      return NULL;
    }

    recorre_unidades->adapta_difonema(&difonemas[contador], izq_der);
    *recorre = recorre_unidades;
  }

  return salida;

}


/**
 * Función:    obten_candidato_de_lista_difonemas                              
 * \remarks Entrada:
 *			- objetivo: vector con las características de la unidad que se    
 *             desea sintetizar.                                               
 *           - izq_der: indica si se trata de un semifonema con contexto por   
 *             la izquierda o por la derecha.                                  
 *			 - locutor: puntero al locutor seleccionado en ese instante.	   	
 *	\remarks Salida:
 *			- candidato: vector escogido.                                     
 *           - tamano: número de elementos.                                    
 * \remarks Valor devuelto:                                                             
 *           - En ausencia de error devuelve un cero.                          
 * \remarks Objetivo:   Libera la memoria empleada para almacenar el corpus de unidades 
 */
/*
  Vector_semifonema_candidato **obten_candidato_de_lista_difonemas(Vector_descriptor_objetivo *objetivo,
  int *tamano, char izq_der, Locutor *locutor) {

  

  }
*/

/**
 * Función:    lee_unidad_y_crea_candidato
 * \remarks Entrada:
 *			- posicion: posición de la unidad en el fichero.
 *	\remarks Salida:
 *			- candidato: vector con la información relativa a la unidad
 *             escogida
 * \remarks Valor devuelto:
 *           - En ausencia de error devuelve un cero.
 * \remarks Objetivo:   Carga la información relativa a la unidad y construye a partir  
 *             de ella un vector descriptor candidato.                         
 */
/*
  int lee_unidad_y_crea_candidato(int posicion, Vector_descriptor_candidato *candidato) {



  }
*/


/**
 * Función:   construye_nombre_unidad                                          
 * \remarks Entrada:
 *			- objetivo: vector del que se desea encontrar la unidad óptima.    
 *          - izq_der: indica qué parte del vector anterior se desea buscar.   
 *	\remarks Salida:
 *			- nombre: nombre de la unidad.                                     
 */

void construye_nombre_unidad(Vector_descriptor_objetivo *objetivo,
			     char izq_der, char nombre[]) {

  char fonema_izq[OCTETOS_POR_FONEMA], fonema_cent[OCTETOS_POR_FONEMA],
    fonema_dcho[OCTETOS_POR_FONEMA];

  objetivo->devuelve_fonemas(fonema_cent, fonema_izq, fonema_dcho);

  if (izq_der == IZQUIERDO) {
    strcpy(nombre, fonema_izq);
    if (nombre[1] == '\0')
      strcat(nombre, ".");
    strcat(nombre, fonema_cent);
  }
  else {
    strcpy(nombre, fonema_cent);
    if (nombre[1] == '\0')
      strcat(nombre, ".");
    strcat(nombre, fonema_dcho);
  }

}

/**
 * Función:   imprime_energia_senhal                                           
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la secuencia cuya energía 
 *            se desea imprimir.                                               
 * \remarks Entrada y Salida:                                                           
 *          - fichero: fichero en el que se va a escribir dicha información.   
 * Valor_devuelto:                                                             
 *          - En ausencia de error devuelve un cero.                           
 */

/*
  KK_FUNCION_COMENTADA

  #ifdef _MODO_NORMAL

  int imprime_energia_senhal(FILE *fichero, int numero_unidades) {

  Vector_semifonema_candidato *lista_unidades, *correcaminos;
  //    FILE *fichero_prueba;

  //    fichero_prueba = fopen("fich_prueba.txt", "wt");

  if ( (lista_unidades = devuelve_camino_optimo_candidato(&numero_unidades)) == NULL)
  return 1;

  correcaminos = lista_unidades;

  for (int contador = 0; contador < numero_unidades; contador++) {
  fprintf(fichero, "%f %f", correcaminos->potencia_inicial, correcaminos->potencia_final);
  //        correcaminos->escribe_datos_txt(fichero_prueba);
  correcaminos++->libera_memoria();
  }

  //    fclose(fichero_prueba);
  free(lista_unidades);

  return 0;

  }

  #endif
*/

/**
 * \brief Función que adapta el contorno de un grupo acentual candidato para
 * que tenga el mismo número de sílabas que el grupo objetivo.
 * \param[in] original contorno del grupo candidato
 * \param[in] objetivo estructura silábica del grupo objetivo
 * \param[in] nuevo_numero número de elementos del nuevo contorno
 * \param[in] silaba_acentuada posición de la sílaba acentuada:
 *			0 => aguda
 *			1 => llana
 *			2 => esdrújula
 * \param[out] nuevo_contorno contorno con el número de puntos adecuado
 * \return En ausencia de error devuelve un cero
 */

int crea_contorno_objetivo(Frecuencia_estilizada *original,
			   estructura_silaba *objetivo,
			   int nuevo_numero,
			   int silaba_acentuada,
			   Frecuencia_estilizada *nuevo_contorno) {

  int contador;
  int posicion_silaba_acentuada_original, posicion_silaba_acentuada_nuevo;
  int silabas_primera_parte_obj;
  int numero_marcas_original = original->numero_marcas;

  float duracion_primera_parte_objetivo; // duración del tramo modificado. En este
  // caso vamos a escalarlo según el número de sílabas.
  estructura_marcas_frecuencia *datos_primera_parte;
  float *tiempos_primera_parte, *apunta_tiempos;
  float tiempo;
  float y0, y1, x0, x1, pendiente; // variables para el escalado.
  // y = (x - x0)*(y1 - y0)/(x1 - x0) + y0;
  estructura_marcas_frecuencia *p_original, *p_nuevo;

  // Ojo con los grupos de una sola sílaba.

  nuevo_contorno->numero_marcas = nuevo_numero;

  if (nuevo_numero == numero_marcas_original) { // No hay que hacer chapuzas,
    // únicamente copiar los valores.

    p_original = original->marcas;
    datos_primera_parte = nuevo_contorno->marcas;
    for (contador = 0; contador < numero_marcas_original; contador++) {
      datos_primera_parte->tiempo = p_original->tiempo;
      datos_primera_parte++->frecuencia = p_original++->frecuencia;
    }

    nuevo_contorno->tiempo_final = nuevo_contorno->marcas[numero_marcas_original - 1].tiempo;
    nuevo_contorno->tiempo_inicio = nuevo_contorno->marcas[0].tiempo;

    return 0;

  }
  else {

    posicion_silaba_acentuada_original = numero_marcas_original - silaba_acentuada - 2;

    posicion_silaba_acentuada_nuevo = nuevo_numero - silaba_acentuada - 2;

    p_original = original->marcas + posicion_silaba_acentuada_original;
    p_nuevo = nuevo_contorno->marcas + posicion_silaba_acentuada_nuevo;

    for (contador = posicion_silaba_acentuada_nuevo; contador < nuevo_numero; contador++) {
      p_nuevo->tiempo = p_original->tiempo;
      p_nuevo++->frecuencia = p_original++->frecuencia;
    }

    silabas_primera_parte_obj = nuevo_numero - silaba_acentuada - 3; // 2 por los valores inicial y final, y la otra por la
    // sílaba acentuada (hacemos la interpolación únicamente en la zona anterior al acento).

    if (silabas_primera_parte_obj > 0) {

      duracion_primera_parte_objetivo = 0;

      estructura_silaba *apunta_obj = objetivo;

      for (contador = 0; contador < silabas_primera_parte_obj; contador++)
	duracion_primera_parte_objetivo += apunta_obj++->tiempo_silaba;

      // Escalamos temporalmente el contorno original:

      if ( (tiempos_primera_parte = (float *) malloc((silabas_primera_parte_obj + 2)*sizeof(float))) == NULL) {
	fprintf(stderr, "Error en crea_contorno_objetivo, al asignar memoria.\n");
	return 1;
      }

      p_original = original->marcas + posicion_silaba_acentuada_original;
      tiempos_primera_parte[silabas_primera_parte_obj + 1] = p_original->tiempo;

      apunta_tiempos = tiempos_primera_parte + silabas_primera_parte_obj;

      for (contador = silabas_primera_parte_obj - 1; apunta_tiempos > tiempos_primera_parte; contador--, apunta_tiempos--) {
	*apunta_tiempos = *(apunta_tiempos + 1) -
	  (objetivo[contador + 1].tiempo_hasta_nucleo + objetivo[contador].tiempo_silaba -
	   objetivo[contador].tiempo_hasta_nucleo);
      } // for (contador = silabas_primera_parte_obj...

      *tiempos_primera_parte = tiempos_primera_parte[1] - objetivo->tiempo_hasta_nucleo;

      p_original = original->marcas;
      x1 = tiempos_primera_parte[silabas_primera_parte_obj];
      x0 = *tiempos_primera_parte;
      if (posicion_silaba_acentuada_original > 1) {
	y1 = p_original[posicion_silaba_acentuada_original - 1].tiempo;
	y0 = p_original->tiempo;
      }
      else {
	y1 = p_original[posicion_silaba_acentuada_original].tiempo;
	y0 = y1 - (p_original[1].tiempo - p_original->tiempo);
      }


      pendiente = (y1 - y0)/(x1 - x0);

      p_nuevo = nuevo_contorno->marcas;

      for (contador = 0; contador < silabas_primera_parte_obj + 1; contador++) {
	p_nuevo->tiempo = tiempos_primera_parte[contador];
	tiempo = pendiente*(p_nuevo->tiempo - x0) + y0;
	p_nuevo++->frecuencia = interpola(original, tiempo, NULL);
      } // for (contador = 0; ...

      free(tiempos_primera_parte);
            
    } //if (silabas_primera_parte_obj > 0)
    else {

      nuevo_contorno->marcas[0].tiempo = nuevo_contorno->marcas[1].tiempo - objetivo->tiempo_hasta_nucleo;
      nuevo_contorno->marcas[0].frecuencia = original->marcas[0].frecuencia;

    } // else // if (silabas_primera_parte_obj > 0) {

    nuevo_contorno->tiempo_final = nuevo_contorno->marcas[nuevo_numero - 1].tiempo;
    nuevo_contorno->tiempo_inicio = nuevo_contorno->marcas[0].tiempo;

  } // else (nuevo_numero != numero_marcas_original)

  return 0;

}


/**
 * \brief Función que genera un contorno objetivo a partir de un candidato que,
 * posiblemente, tiene un número diferente de sílabas.
 * \remarks Variante de la función crea_vector_frecuencias. En ella se repetían
 * valores de f0, lo cual daba lugar a tramos planos y variaciones bruscas que
 * degradaban la calidad de la entonación sintética.
 * \param[in] cadena_unidades cadena de unidades de tipo objetivo.
 * \param[in] numero_unidades número de elementos de la cadena anterior
 * \param[in] acentual_cand cadena de grupos acentuales candidato de los que se
 * extrae la información del contorno de f0
 * \param[in] acentual_obj cadne de grupos acentuales objetivo, de los que se
 * extrae el número de sílabas de cada grupo acentual
 * \param[in] esc_pitch escalado de pitch
 * \param[out] frecuencias cadena de estructuras en las que se almacenan los
 * valores inicial, medio y final de f0 para cada alófono
 * \param[in] indice_fonema índice del fonema en el vector cadena_unidades. Por defecto es cero.
 * \return en ausencia de error, devuelve un cero
 */

int crea_vector_frecuencias_V2(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_candidato *acentual_cand, Grupo_acentual_objetivo *acentual_obj, float esc_pitch, estructura_frecuencias **frecuencias, int indice_fonema) {
  
  char central[OCTETOS_POR_FONEMA], izquierdo[OCTETOS_POR_FONEMA], derecho[OCTETOS_POR_FONEMA];
  estructura_frecuencias *correcaminos;
  int cuenta_fonemas = 0;
  
  Frecuencia_estilizada marcas_objetivo;
  unsigned char cuenta_fonemas_silaba = 0;
  unsigned char indice_silaba = 0, numero_silabas_obj;
  estructura_silaba *silabas = acentual_obj->silabas;
  estructura_marcas_frecuencia *marcas;
  float escalado, instante_calculo, tiempo_acumulado = 0;
  float y0, y1; // variables para el escalado.
  // y = (x - x0)*(y1 - y0)/(x1 - x0) + y0;
  float duracion;


  if (*frecuencias == NULL)

    if ( (*frecuencias = (estructura_frecuencias *) malloc(numero_unidades*
							   sizeof(estructura_frecuencias))) == NULL) {
      fprintf(stderr, "Error en crea_vector_frecuencias, al asignar memoria.\n");
      return 1;
    }

  correcaminos = *frecuencias;

  if ( (marcas_objetivo.marcas = (estructura_marcas_frecuencia *) malloc( (acentual_obj->numero_silabas + 2)*sizeof(estructura_marcas_frecuencia)))
       == NULL) {
    fprintf(stderr, "Error en crea_vector_frecuencias_V2, al asignar memoria.\n");
    return 1;
  }

  marcas = marcas_objetivo.marcas;
    
  crea_contorno_objetivo(&acentual_cand->frecuencia, acentual_obj->silabas, acentual_obj->numero_silabas + 2, acentual_obj->posicion_silaba_acentuada, &marcas_objetivo);

  vector<Vector_descriptor_objetivo>::iterator it = cadena_unidades.begin() + indice_fonema;

  for (; numero_unidades; numero_unidades--, correcaminos++, it++) {

    it->devuelve_fonemas(central, izquierdo, derecho);

    if (central[0] == '#') {
      correcaminos->frecuencia_inicial = 0;
      correcaminos->frecuencia_mitad = 0;
      correcaminos->frecuencia_final = 0;
      continue;
    }

    if (cuenta_fonemas >= acentual_obj->numero_alofonos) {
      acentual_obj++;
      silabas = acentual_obj->silabas;
      numero_silabas_obj = acentual_obj->numero_silabas;

      acentual_cand++;

      cuenta_fonemas = 0;
      cuenta_fonemas_silaba = 0;
      indice_silaba = 0;

      tiempo_acumulado = 0;

      if (marcas_objetivo.marcas != NULL)
	free(marcas_objetivo.marcas);

      if ( (marcas_objetivo.marcas = (estructura_marcas_frecuencia *) malloc( (numero_silabas_obj + 2)*sizeof(estructura_marcas_frecuencia)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vector_frecuencias_V2, al asignar memoria.\n");
	return 1;
      }

      crea_contorno_objetivo(&acentual_cand->frecuencia, acentual_obj->silabas, acentual_obj->numero_silabas + 2, acentual_obj->posicion_silaba_acentuada, &marcas_objetivo);

      marcas = marcas_objetivo.marcas;
                     
    } // if (cuenta_fonemas >= acentual_obj->numero_alofonos)

    duracion = it->duracion_1*2; // Antes, 0.002

    if (sordo_sonoro(central) == SORDO) {

      correcaminos->frecuencia_inicial = 0;
      correcaminos->frecuencia_mitad = 0;
      correcaminos->frecuencia_final = 0;

      cuenta_fonemas++;

      if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	cuenta_fonemas_silaba = 0;
	silabas++;
	indice_silaba++;
	tiempo_acumulado = 0;
      }
      else
	tiempo_acumulado += duracion;

      continue;
    } // if (sordo_sonoro...

    if (cuenta_fonemas_silaba < silabas->vocal_fuerte) {

      y1 = marcas[indice_silaba + 1].tiempo;
      if (indice_silaba == 0)
	y0 = marcas->tiempo;
      else
	y0 = (y1 + marcas[indice_silaba].tiempo) / 2;

      if (silabas->tiempo_hasta_nucleo)
	escalado =  (y1 - y0) / silabas->tiempo_hasta_nucleo;
      else
	escalado = 1; // Esto tengo que comprobarlo.
    }
    else
      if (cuenta_fonemas_silaba > silabas->vocal_fuerte) {

	y0 = marcas[indice_silaba + 1].tiempo;

	if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	  y1 = marcas[indice_silaba + 2].tiempo;
	else
	  y1 = (y0 + marcas[indice_silaba + 2].tiempo) / 2;

	escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

      }
      else { // hay que interpolar en dos tramos.

	y1 = marcas[indice_silaba + 1].tiempo; // tiempo a la mitad de la vocal fuerte
	// en el contorno original.
	if (indice_silaba == 0)
	  y0 = marcas->tiempo;
	else
	  y0 = (y1 + marcas[indice_silaba].tiempo) / 2;  // Punto medio entre ambas sílabas.

	if (silabas->tiempo_hasta_nucleo)
	  escalado = (y1 - y0) / silabas->tiempo_hasta_nucleo;
	else
	  escalado = 1;

	instante_calculo = tiempo_acumulado*escalado + y0;

	correcaminos->frecuencia_inicial = esc_pitch*
	  interpola(&marcas_objetivo, instante_calculo, NULL);

	correcaminos->frecuencia_mitad = esc_pitch*marcas[indice_silaba + 1].frecuencia;

	y0 = y1;

	if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	  y1 = marcas[indice_silaba + 2].tiempo;
	else
	  y1 = (marcas[indice_silaba + 2].tiempo + y0) / 2;

	escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

	tiempo_acumulado = duracion / 2;

	instante_calculo = tiempo_acumulado*escalado + y0;

	correcaminos->frecuencia_final = esc_pitch*
	  interpola(&marcas_objetivo, instante_calculo, NULL);

	if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	  cuenta_fonemas_silaba = 0;
	  silabas++;
	  indice_silaba++;
	  tiempo_acumulado = 0;
	}

	cuenta_fonemas++;

	continue;

      } // else

    instante_calculo = tiempo_acumulado*escalado + y0;

    correcaminos->frecuencia_inicial = esc_pitch*interpola(&marcas_objetivo, instante_calculo, NULL);

    instante_calculo = (tiempo_acumulado + duracion/2)*escalado + y0;

    correcaminos->frecuencia_mitad = esc_pitch*
      interpola(&marcas_objetivo, instante_calculo, NULL);

    tiempo_acumulado += duracion;

    instante_calculo = tiempo_acumulado*escalado + y0;

    correcaminos->frecuencia_final = esc_pitch*
      interpola(&marcas_objetivo, instante_calculo, NULL);


    if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
      cuenta_fonemas_silaba = 0;
      silabas++;
      indice_silaba++;
      tiempo_acumulado = 0;
    }

    cuenta_fonemas++;

  }

  free(marcas_objetivo.marcas);

  return 0;

}


/**
 * Función:   crea_vector_frecuencias
 * \remarks Entrada:
 *			- cadena_unidades: cadena de unidades de tipo objetivo.
 *          - numero_unidades: número de elementos de la cadena anterior.
 *          - acentual_cand: cadena de grupos acentuales candidato de los que
 *            se extrae la información de la frecuencia.
 *          - acentual_obj: cadena de grupos acentuales objetivo, de los que
 *            se extrae el número de sílabas de cada grupo acentual.
 *          - esc_pitch: escalado de pitch.
 *          - indice_fonema: índice del primer fonema en el vector cadena unidades.
 *	\remarks Salida:
 *			- frecuencias: cadena de estructuras en las que se almacenan los
 *            valores inicial, medio y final de frecuencia para cada alófono.
 * Valor_devuelto:
 *          - En ausencia de error devuelve un cero.
 * \remarks NOTA:      Se diferencia de la función anterior en que ésta devuelve una
 *            cadena de frecuencias adecuada para la representación gráfica.
 *            La anterior asigna valores nulos a los alófonos sordos.
 */

//#ifdef _MODO_NORMAL

int crea_vector_frecuencias(Vector_descriptor_objetivo *cadena_unidades, int numero_unidades, Grupo_acentual_candidato *acentual_cand, Grupo_acentual_objetivo *acentual_obj, float esc_pitch, estructura_frecuencias **frecuencias, int indice_fonema) {

  char central[OCTETOS_POR_FONEMA], izquierdo[OCTETOS_POR_FONEMA], derecho[OCTETOS_POR_FONEMA];
  estructura_frecuencias *correcaminos;
  int cuenta_fonemas = 0;
  int silabas_antes_de_tonica = 0;
  int diferencia_silabas = 0;
  int cuenta_1 = 0;

  char relacion_silabas;
  estructura_marcas_frecuencia *marcas = acentual_cand->frecuencia.marcas;
  unsigned char cuenta_fonemas_silaba = 0;
  unsigned char indice_silaba = 0, numero_silabas_cand, numero_silabas_obj;
  estructura_silaba *silabas = acentual_obj->silabas;
  float escalado, instante_calculo, tiempo_acumulado = 0;
  float y0, y1; // variables para el escalado.
  // y = (x - x0)*(y1 - y0)/(x1 - x0) + y0;
  float duracion;

  cadena_unidades += indice_fonema;

  if (*frecuencias == NULL)

    if ( (*frecuencias = (estructura_frecuencias *) malloc(numero_unidades*
							   sizeof(estructura_frecuencias))) == NULL) {
      fprintf(stderr, "Error en crea_vector_frecuencias, al asignar memoria.\n");
      return 1;
    }

  correcaminos = *frecuencias;



  numero_silabas_cand = acentual_cand->numero_silabas;
  numero_silabas_obj = acentual_obj->numero_silabas;

  if (numero_silabas_cand == numero_silabas_obj)
    relacion_silabas = 0;
  else {
    silabas_antes_de_tonica = numero_silabas_obj - acentual_obj->posicion_silaba_acentuada - 1;
    if (numero_silabas_cand > numero_silabas_obj) {
      diferencia_silabas = numero_silabas_cand - numero_silabas_obj;
      relacion_silabas = 1;
    }
    else {
      diferencia_silabas = numero_silabas_obj - numero_silabas_cand;
      relacion_silabas = -1;
    }
  }

  for (; numero_unidades; numero_unidades--, cadena_unidades++, correcaminos++) {

    cadena_unidades->devuelve_fonemas(central, izquierdo, derecho);

    if (central[0] == '#') {
      correcaminos->frecuencia_inicial = 0;
      correcaminos->frecuencia_mitad = 0;
      correcaminos->frecuencia_final = 0;
      continue;
    }

    if (cuenta_fonemas >= acentual_obj->numero_alofonos) {
      acentual_obj++;
      silabas = acentual_obj->silabas;
      numero_silabas_obj = acentual_obj->numero_silabas;

      cuenta_1 = 0;
      acentual_cand++;
      numero_silabas_cand = acentual_cand->numero_silabas;
      marcas = acentual_cand->frecuencia.marcas;

      cuenta_fonemas = 0;
      cuenta_fonemas_silaba = 0;
      indice_silaba = 0;

      tiempo_acumulado = 0;

      if (numero_silabas_cand == numero_silabas_obj)
	relacion_silabas = 0;
      else {
	silabas_antes_de_tonica = numero_silabas_obj -
	  acentual_obj->posicion_silaba_acentuada - 1;
	if (numero_silabas_cand > numero_silabas_obj) {
	  diferencia_silabas = numero_silabas_cand - numero_silabas_obj;
	  relacion_silabas = 1;
	}
	else {
	  diferencia_silabas = numero_silabas_obj - numero_silabas_cand;
	  relacion_silabas = -1;
	}
      }
    }

    duracion = cadena_unidades->duracion_1*2; // Antes, 0.002

    if (sordo_sonoro(central) == SORDO) {

      correcaminos->frecuencia_inicial = 0;
      correcaminos->frecuencia_mitad = 0;
      correcaminos->frecuencia_final = 0;

      cuenta_fonemas++;

      if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	cuenta_fonemas_silaba = 0;
	silabas++;
	indice_silaba++;
	tiempo_acumulado = 0;
      }
      else
	tiempo_acumulado += duracion;

      continue;
    } // if (sordo_sonoro...

    switch (relacion_silabas) {

    case 0: {

      if (cuenta_fonemas_silaba < silabas->vocal_fuerte) {

	y1 = marcas[indice_silaba + 1].tiempo;
	if (indice_silaba == 0)
	  y0 = marcas->tiempo;
	else
	  y0 = (y1 + marcas[indice_silaba].tiempo) / 2;

	if (silabas->tiempo_hasta_nucleo)
	  escalado =  (y1 - y0) / silabas->tiempo_hasta_nucleo;
	else
	  escalado = 1; // Esto tengo que comprobarlo.
      }
      else
	if (cuenta_fonemas_silaba > silabas->vocal_fuerte) {

	  y0 = marcas[indice_silaba + 1].tiempo;

	  if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	    y1 = marcas[indice_silaba + 2].tiempo;
	  else
	    y1 = (y0 + marcas[indice_silaba + 2].tiempo) / 2;

	  escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);
	}
	else { // hay que interpolar en dos tramos.
	  y1 = marcas[indice_silaba + 1].tiempo; // tiempo a la mitad de la vocal fuerte
	  // en el contorno original.
	  if (indice_silaba == 0)
	    y0 = marcas->tiempo;
	  else
	    y0 = (y1 + marcas[indice_silaba].tiempo) / 2;  // Punto medio entre ambas sílabas.

	  if (silabas->tiempo_hasta_nucleo)
	    escalado = (y1 - y0) / silabas->tiempo_hasta_nucleo;
	  else
	    escalado = 1;

	  instante_calculo = tiempo_acumulado*escalado + y0;

	  correcaminos->frecuencia_inicial = esc_pitch*
	    interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	  correcaminos->frecuencia_mitad = esc_pitch*marcas[indice_silaba + 1].frecuencia;

	  y0 = y1;

	  if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	    y1 = marcas[indice_silaba + 2].tiempo;
	  else
	    y1 = (marcas[indice_silaba + 2].tiempo + y0) / 2;

	  escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

	  tiempo_acumulado = duracion / 2;

	  instante_calculo = tiempo_acumulado*escalado + y0;

	  correcaminos->frecuencia_final = esc_pitch*
	    interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	  if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	    cuenta_fonemas_silaba = 0;
	    silabas++;
	    indice_silaba++;
	    tiempo_acumulado = 0;
	  }

	  cuenta_fonemas++;

	  break;

	}

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_inicial = esc_pitch*interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      instante_calculo = (tiempo_acumulado + duracion/2)*escalado + y0;

      correcaminos->frecuencia_mitad = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      tiempo_acumulado += duracion;

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_final = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);


      if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	cuenta_fonemas_silaba = 0;
	silabas++;
	indice_silaba++;
	tiempo_acumulado = 0;
      }

      cuenta_fonemas++;

      break;

    } // case 0

    case 1: {

      if ( (indice_silaba < silabas_antes_de_tonica) &&
	   (indice_silaba < numero_silabas_obj - acentual_obj->posicion_silaba_acentuada - 1) ){

	correcaminos->frecuencia_inicial = esc_pitch*(marcas[cuenta_1].frecuencia +
						      indice_silaba*(marcas[numero_silabas_cand - acentual_cand->posicion_silaba_acentuada].frecuencia -
								     marcas[cuenta_1].frecuencia) /
						      silabas_antes_de_tonica);
	correcaminos->frecuencia_mitad = correcaminos->frecuencia_inicial;
	correcaminos->frecuencia_final = correcaminos->frecuencia_mitad;

	if (cuenta_1 == 0)
	  cuenta_1 = 1;
                
	if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	  cuenta_fonemas_silaba = 0;
	  silabas++;
	  indice_silaba++;
	  tiempo_acumulado = 0;
	}

	cuenta_fonemas++;
	break;
      }
      else
	if (cuenta_fonemas_silaba < silabas->vocal_fuerte) {
	  y1 = marcas[indice_silaba + diferencia_silabas + 1].tiempo;

	  if (indice_silaba == 0)
	    y0 = marcas->tiempo;
	  else
	    y0 = (y1 + marcas[indice_silaba + diferencia_silabas].tiempo) / 2;

	  if (silabas->tiempo_hasta_nucleo)
	    escalado =  (y1 - y0) / silabas->tiempo_hasta_nucleo;
	  else
	    escalado = 1; // Esto tengo que comprobarlo.
	}
	else // if (cuenta_fonemas_silaba < silabas_vocal_fuerte...
	  if (cuenta_fonemas_silaba > silabas->vocal_fuerte) {

	    if (indice_silaba == 0)
	      y0 = marcas->tiempo;
	    else
	      y0 = marcas[indice_silaba + diferencia_silabas + 1].tiempo;

	    if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	      y1 = marcas[indice_silaba + diferencia_silabas + 2].tiempo;
	    else
	      y1 = (y0 + marcas[indice_silaba + diferencia_silabas + 2].tiempo) / 2;

	    escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);
	  } // if (cuenta_fonemas_silaba > silabas->vocal_fuerte...
	  else { // hay que interpolar en dos tramos.
	    y1 = marcas[indice_silaba + diferencia_silabas + 1].tiempo;
	    if (indice_silaba == 0)
	      y0 = marcas[diferencia_silabas].tiempo;
	    else
	      y0 = (y1 + marcas[indice_silaba + diferencia_silabas].tiempo) / 2;

	    if (silabas->tiempo_hasta_nucleo)
	      escalado = (y1 - y0) / silabas->tiempo_hasta_nucleo;
	    else
	      escalado = 1;

	    instante_calculo = tiempo_acumulado*escalado + y0;

	    correcaminos->frecuencia_inicial = esc_pitch*
	      interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	    correcaminos->frecuencia_mitad = esc_pitch*
	      marcas[indice_silaba + diferencia_silabas + 1].frecuencia;

	    y0 = y1;

	    if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	      y1 = marcas[indice_silaba + diferencia_silabas + 2].tiempo;
	    else
	      y1 = (marcas[indice_silaba + diferencia_silabas + 2].tiempo + y0) / 2;

	    escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

	    tiempo_acumulado = duracion / 2;

	    instante_calculo = tiempo_acumulado*escalado + y0;

	    correcaminos->frecuencia_final = esc_pitch*
	      interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	    if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	      cuenta_fonemas_silaba = 0;
	      silabas++;
	      indice_silaba++;
	      tiempo_acumulado = 0;
	    }

	    cuenta_fonemas++;

	    break;

	  }

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_inicial = esc_pitch*interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      instante_calculo = (tiempo_acumulado + duracion/2)*escalado + y0;

      correcaminos->frecuencia_mitad = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      tiempo_acumulado += duracion;

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_final = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);


      if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	cuenta_fonemas_silaba = 0;
	silabas++;
	indice_silaba++;
	tiempo_acumulado = 0;
      }

      cuenta_fonemas++;

      break;

    } // case 1:

    default: {

      if (indice_silaba < diferencia_silabas) {
	correcaminos->frecuencia_inicial = esc_pitch*marcas[0].frecuencia;
	correcaminos->frecuencia_mitad = correcaminos->frecuencia_inicial;
	correcaminos->frecuencia_final = correcaminos->frecuencia_mitad;

	if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	  cuenta_fonemas_silaba = 0;
	  silabas++;
	  indice_silaba++;
	  tiempo_acumulado = 0;
	}

	cuenta_fonemas++;

	break;

      } // if (indice_silaba < diferencia_silabas)
      else
	if (cuenta_fonemas_silaba < silabas->vocal_fuerte) {
	  y1 = marcas[indice_silaba - diferencia_silabas + 1].tiempo;
	  y0 = (y1 + marcas[indice_silaba - diferencia_silabas].tiempo) / 2;

	  if (silabas->tiempo_hasta_nucleo)
	    escalado =  (y1 - y0) / silabas->tiempo_hasta_nucleo;
	  else
	    escalado = 1; // Esto tengo que comprobarlo.
	}
	else
	  if (cuenta_fonemas_silaba > silabas->vocal_fuerte) {
	    y0 = marcas[indice_silaba - diferencia_silabas + 1].tiempo;

	    if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	      y1 = marcas[indice_silaba - diferencia_silabas + 2].tiempo;
	    else
	      y1 = (y0 + marcas[indice_silaba - diferencia_silabas + 2].tiempo) / 2;
	    escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

	  }
	  else { // hay que interpolar en dos tramos.
	    y1 = marcas[indice_silaba - diferencia_silabas + 1].tiempo;
	    y0 = (y1 + marcas[indice_silaba - diferencia_silabas].tiempo) / 2;

	    if (silabas->tiempo_hasta_nucleo)
	      escalado = (y1 - y0) / silabas->tiempo_hasta_nucleo;
	    else
	      escalado = 1;

	    instante_calculo = tiempo_acumulado*escalado + y0;

	    correcaminos->frecuencia_inicial = esc_pitch*
	      interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	    correcaminos->frecuencia_mitad = esc_pitch*
	      marcas[indice_silaba - diferencia_silabas + 1].frecuencia;

	    y0 = y1;

	    if (cuenta_fonemas == acentual_obj->numero_alofonos - 1)
	      y1 = marcas[indice_silaba - diferencia_silabas + 2].tiempo;
	    else
	      y1 = (marcas[indice_silaba - diferencia_silabas + 2].tiempo + y0) / 2;

	    escalado = (y1 - y0) / (silabas->tiempo_silaba - silabas->tiempo_hasta_nucleo);

	    tiempo_acumulado = duracion / 2;

	    instante_calculo = tiempo_acumulado*escalado + y0;

	    correcaminos->frecuencia_final = esc_pitch*
	      interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

	    if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	      cuenta_fonemas_silaba = 0;
	      silabas++;
	      indice_silaba++;
	      tiempo_acumulado = 0;
	    }

	    cuenta_fonemas++;

	    break;

	  }

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_inicial = esc_pitch*interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      instante_calculo = (tiempo_acumulado + duracion/2)*escalado + y0;

      correcaminos->frecuencia_mitad = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);

      tiempo_acumulado += duracion;

      instante_calculo = tiempo_acumulado*escalado + y0;

      correcaminos->frecuencia_final = esc_pitch*
	interpola(&acentual_cand->frecuencia, instante_calculo, NULL);


      if (++cuenta_fonemas_silaba >= silabas->fonemas_por_silaba) {
	cuenta_fonemas_silaba = 0;
	silabas++;
	indice_silaba++;
	tiempo_acumulado = 0;
      }

      cuenta_fonemas++;

    } // default:

    }

  } // for (; numero_unidades...

  return 0;

}

/**
 * \brief Función que añade una secuencia de frecuencias objetivo (correspondientes a un mismo contorno) a los vectores descriptores objetivo
 * \param cadena_unidades cadena de vectores descriptores objetivo
 * \param[in] numero_unidades 
 * \param[in] frecuencias cadena de estructuras de tipo estructura_frecuencias, con los valores objetivo de f0
 * \param[in] indice_primer_fonema índice del primer fonema del vector
 * \return Actualmente siempre devuelve 0
 */
int anhade_frecuencias_objetivo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, estructura_frecuencias *frecuencias, int indice_primer_fonema) {

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_unidades.begin();

  recorre_objetivo += indice_primer_fonema;

  for (int contador = 0; contador < numero_unidades; contador++, recorre_objetivo++, frecuencias++) {
    if ((sordo_sonoro(recorre_objetivo->semifonema) == SONORO) || (recorre_objetivo->frecuencias_inicial.size() == 0) ) {
      recorre_objetivo->frecuencias_inicial.push_back(frecuencias->frecuencia_inicial);
      recorre_objetivo->frecuencias_mitad.push_back(frecuencias->frecuencia_mitad);
      recorre_objetivo->frecuencias_final.push_back(frecuencias->frecuencia_final);
    }
  }

  return 0;

}

//#endif

/**
 * Función:   crea_fichero_contorno_frecuencia                                 
 * \remarks Entrada:
 *			- cadena_unidades: cadena de unidades de tipo objetivo.            
 *			- cadena_escogidas: cadena de unidades escogidas.				   
 *          - numero_unidades: número de elementos de la cadena anterior.      
 *          - frecuencias: cadena de estructuras en las que se almacenan los   
 *            valores inicial, medio y final de frecuencia para cada alófono.  
 *			- locutor_actual: puntero al locutor empleado.					   
 *			- opcion: opción de escritura sobre el fichero de salida.		   
 * Valor_devuelto:                                                             
 *          - En ausencia de error devuelve un cero.                           
 * \remarks NOTA:	Esta función no devuelve el contorno exacto de frecuencia, ya que  
 *			los instantes temporales considerados se calculan a partir de los  
 *			objetivos, y no de la duración real de las unidades, lo cual 	   
 *			podría incluir alguna modificación prosódica.					   		
 */

int crea_fichero_contorno_frecuencia(vector<Vector_descriptor_objetivo> &cadena_unidades,
				     Vector_sintesis *cadena_escogidas,
                                     int numero_unidades,
                                     estructura_frecuencias *frecuencias,
                                     Locutor *locutor_actual,
                                     const char *opcion,
                                     char *nombre_fichero, int indice) {

  char central[OCTETOS_POR_FONEMA], izquierdo[OCTETOS_POR_FONEMA], derecho[OCTETOS_POR_FONEMA];
  char nombre_fichero_total[FILENAME_MAX];
  float tiempo_acumulado = 0;
  float duracion;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_unidades.begin();
  Vector_sintesis *recorre_sintesis = cadena_escogidas;
  Vector_semifonema_candidato *semifonema_escogido;
  void *unidad_escogida;
  FILE *fichero_contorno, *fichero_fonemas;

  sprintf(nombre_fichero_total, "%s%d.ctn", nombre_fichero, indice);

  if ( (fichero_contorno = fopen(nombre_fichero_total, opcion)) == NULL) {
    fprintf(stderr, "Error en crea_contorno_frecuencia, al intentar crear el fichero %s.\n",
	    nombre_fichero_total);
    return 1;
  } // if ( (fichero_contorno = ...

  sprintf(nombre_fichero_total, "%s%d.fon", nombre_fichero, indice);

  if ( (fichero_fonemas = fopen(nombre_fichero_total, opcion)) == NULL) {
    fprintf(stderr, "Error en crea_contorno_frecuencia, al intentar crear el fichero %s.\n",
	    nombre_fichero_total);
    return 1;
  } // if ( (fichero_fonemas

  for (int contador = 0; contador < 2*numero_unidades; contador++, recorre_sintesis++) {

    if ( (contador % 2 == 0) && (contador != 0) ) {
      frecuencias++;
      recorre_objetivo++;
    } // if (contador & 1)

    recorre_objetivo->devuelve_fonemas(central, izquierdo, derecho);

    duracion = recorre_objetivo->duracion_1;

    if ( (contador % 2) == 0)
      fprintf(fichero_fonemas, "%f %s\n", tiempo_acumulado, central);
    //        	fprintf(fichero_fonemas, "%s\n", central);

    if ( (central[0] == '#') || (sordo_sonoro(central) == SORDO) ) {

      if ( (central[0] == '#') && (contador) )  {
	fprintf(fichero_contorno, "%f -1.0 -1.0\n", tiempo_acumulado + 0.001);
      } // if (central[0] == '#')

      tiempo_acumulado += duracion;
      continue;
    }

#ifdef _SELECCION_ANTERIOR
    if (convierte_vector_sintesis_a_candidato(recorre_sintesis, *recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;
#else
    if (convierte_vector_sintesis_a_candidato_tonicidad(recorre_sintesis, recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;

#endif

    semifonema_escogido = (Vector_semifonema_candidato *) unidad_escogida;

    if ( (contador % 2) == 0) {

      fprintf(fichero_contorno, "%f %f %f\n", tiempo_acumulado, frecuencias->frecuencia_inicial,
	      semifonema_escogido->frecuencia_inicial);

      fprintf(fichero_contorno, "%f %f %f\n", tiempo_acumulado + duracion / 2, frecuencias->frecuencia_mitad,
	      semifonema_escogido->frecuencia_final);

    } // if ( (contador % 2) == 0)
    else {

      fprintf(fichero_contorno, "%f %f %f\n", tiempo_acumulado + duracion - 0.005, frecuencias->frecuencia_final,
	      semifonema_escogido->frecuencia_final);

    } // else

    tiempo_acumulado += duracion;

  } // for (; numero_unidades;...

  fclose(fichero_contorno);
  fclose(fichero_fonemas);

  return 0;

}

/**
 * Función:   crea_fichero_grupos_acentuales                                   
 * \remarks Entrada:
 *			- candidatos: cadena de vectores de tipo Grupo_acentual_candidato. 
 *          - numero_unidades: número de elementos de la cadena anterior.      
 *			- opcion: opción de escritura sobre el fichero de salida.		   
 * Valor_devuelto:                                                             
 *          - En ausencia de error devuelve un cero.                           
 */

int crea_fichero_grupos_acentuales(Grupo_acentual_candidato *candidatos,
				   int numero_acentuales,
				   const char *opcion,
                                   char *nombre_fichero, int indice) {

  char nombre_fichero_total[FILENAME_MAX];
  int posicion;
  FILE *fichero_contorno;

  sprintf(nombre_fichero_total, "%s%d.grup", nombre_fichero, indice);

  if ( (fichero_contorno = fopen(nombre_fichero_total, opcion)) == NULL) {
    fprintf(stderr, "Error en crea_fichero_grupos_acentuales, al intentar \
crear el fichero %s.\n", nombre_fichero_total);
    return 1;
  } // if ( (fichero_contorno = ...

  fprintf(fichero_contorno, "%d\n", numero_acentuales);

  for (; numero_acentuales; numero_acentuales--, candidatos++) {

    if (candidatos->grupos_acentuales_grupo_prosodico == 1)
      posicion = GRUPO_INICIAL_Y_FINAL;
    else
      if (candidatos->posicion_grupo_acentual == 0)
	posicion = GRUPO_INICIAL;
      else
	if (candidatos->posicion_grupo_acentual ==
	    candidatos->grupos_acentuales_grupo_prosodico - 1)
	  posicion = GRUPO_FINAL;
	else
	  posicion = GRUPO_MEDIO;

    fprintf(fichero_contorno, "%d\t%d\t%d\t%d\n", posicion,
	    candidatos->posicion_silaba_acentuada,
	    candidatos->tipo_grupo_prosodico_3,
	    candidatos->identificador);

  } // for (; numero_acentuales;...

  fclose(fichero_contorno);

  return 0;

}


/**
 * Función:   encuentra_indice_coste_minimo                                   
 * \remarks Entrada:
 *			- costes_minimos: cadena de costes mínimos.                       
 *          - numero: número de elementos de la cadena anterior.              
 * \remarks Valor devuelto:                                                            
 *          - El índice de la cadena con un coste menor.                      
 */

int encuentra_indice_coste_minimo(float *costes_minimos, int numero) {

  int indice = 0;
  float coste = FLT_MAX, *puntero_costes = costes_minimos;

  for (int contador = 0; contador < numero; contador++, puntero_costes++)
    if (*puntero_costes < coste) {

      coste = *puntero_costes;
      indice = contador;
    }

  return indice;

}


/**
 * Función:   encuentra_indice_coste_minimo                                   
 * \remarks Entrada:
 *	    - costes_minimos: cadena de costes mínimos.                       
 *          - numero: número de elementos de la cadena anterior.              
 *          - mascara_contornos: array de caracteres que indica qué caminos siguen en activo (para el cálculo de contornos en paralelo)
 * \remarks Valor devuelto:                                                            
 *          - El índice de la cadena con un coste menor.                      
 */

int encuentra_indice_coste_minimo(float *costes_minimos, int numero, char *mascara_contornos) {

  int indice = 0;
  float coste = FLT_MAX, *puntero_costes = costes_minimos;

  for (int contador = 0; contador < numero; contador++, puntero_costes++) {
    
    if (mascara_contornos[contador] == 0)
      continue;

    if (*puntero_costes < coste) {
      coste = *puntero_costes;
      indice = contador;
    }

  }

  return indice;

}


/**
 * Función:   comprueba_continuidad_frecuencia                                
 * \remarks Entrada:
 *			- numero_candidatos: número de elementos de la cadena de vectores 
 *            candidatos.                                                     
 * \remarks Entrada y Salida:                                                          
 *          - cadena_candidatos: cadena con los vectores seleccionados para   
 *            la síntesis de la frase.                                        
 * \remarks Objetivo:                                                                  
 *          - En la fase de selección se comprueba la distancia prosódica de  
 *            cada unidad al objetivo, y sólo se realizan modificaciones si   
 *            dicha diferencia está por encima de un umbral. Sin embargo, es  
 *            posible que haya un salto demasiado grande al concatenar dos    
 *            unidades que se encontrasen a priori dentro del umbral, y por   
 *            ello es necesario una comprobación final.                       
 */

void comprueba_continuidad_frecuencia(Vector_semifonema_candidato *cadena_candidatos,
				      int numero_candidatos) {

  char validez1, validez2;
  Vector_semifonema_candidato *vector1 = cadena_candidatos, *vector2 = cadena_candidatos + 1;

  for (int contador = 0; contador < numero_candidatos - 1; contador++, vector1++) {

    validez1 = vector1->validez;
    validez2 = vector2->validez;

    if (absoluto(vector1->frecuencia_final - vector2->frecuencia_final) > FREC_DIF_UMBRAL) {
      validez1 |= 1;
      validez2 |= 1;
    } // if (absoluto(..

    vector1->validez = validez1;
    vector2->validez = validez2;
        
    vector2 = vector1;

  } // for (int contador...

}


/**
 * \author	Fran Campillo
 * \brief	Función que define las reglas de sustitución de semifonemas en el caso de
 * que en el corpus no se encuentre el deseado.
 * \param[in]	fonema1 primer fonema
 * \param[in]	fonema2	segundo fonema
 * \param[in]	izq_der	IZQUIERDO o DERECHO
 * \param[in]	locutor_actual	puntero al locutor empleado
 * \return	El puntero a las unidades candidatas.
 */

estructura_unidades *reglas_sustitucion_semifonemas(char fonema1[], char fonema2[], char izq_der, Locutor *locutor_actual) {


  int contador, cambio = 0;
  int numero_casos = 26;
  const char originales[][3] = {"e", "E", "o", "O", "g", 
				"G", "d", "D", "b", "B", 
				"n", "N", "x", "r", "rr", 
				"m", "n", "S", "p", "t", 
				"k", "b", "d", "g", "j", "w"};
  const char cambios[][3] = {"E", "e", "O", "o", "G", 
			     "g", "D", "d", "B", "b", 
			     "N", "n", "S", "rr", "r", 
			     "n", "m", "s", "b", "d", 
			     "g", "p", "t", "k", "i", "u"};
  // Se podría hacer de otras formas más elegantes, pero así tenemos en cuenta que algunos fonemas
  // se pueden representar con dos letras (rr y tS).

  char sustitucion[OCTETOS_POR_FONEMA];
  estructura_unidades *unidad_seleccionada = NULL;

  if (izq_der == DERECHO) {

    for (contador = 0; contador < numero_casos; contador++)
      if (strcmp(fonema2, originales[contador]) == 0) {
	cambio = 1;
	strcpy(sustitucion, cambios[contador]);
	break;
      }

    if (cambio == 1)
      unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, sustitucion, DERECHO, locutor_actual);
    if (unidad_seleccionada == NULL) {

      cambio = 0;

      for (contador = 0; contador < numero_casos; contador++)
	if (strcmp(fonema1, originales[contador]) == 0){
	  cambio = 1;
	  strcpy(sustitucion, cambios[contador]);
	  break;
	}
      if (cambio == 1)
	unidad_seleccionada = devuelve_puntero_a_unidades(sustitucion, fonema2, DERECHO, locutor_actual);

      if (unidad_seleccionada == NULL) {
	printf("Sustitución chunga del semifonema %s.%s(d).\n", fonema1, fonema2);
	if ( (unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, "p", DERECHO, locutor_actual))
	     == NULL) // Probamos con "t".
	  if ( (unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, "t", DERECHO, locutor_actual))
	       == NULL) // Probamos con "k".
	    if ( (unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, "k", DERECHO, locutor_actual))
		 == NULL) // ¡Situación límite...! Lo intentamos con el contexto silencio.
	      if ( (unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, "#", DERECHO, locutor_actual))
		   == NULL) { // Introducimos un silencio.
		//                              puts("Introducimos un silencio.");
		unidad_seleccionada = devuelve_puntero_a_unidades("#", "#", DERECHO, locutor_actual);
	      }
      } // if (unidad_seleccionada == NULL)


    } // if (unidad_seleccionada == NULL)...

  } // if (izq_der == DERECHO)
  else {
        
    for (contador = 0; contador < numero_casos; contador++)
      if (strcmp(fonema1, originales[contador]) == 0) {
	cambio = 1;
	strcpy(sustitucion, cambios[contador]);
	break;
      }

    if (cambio == 1)
      unidad_seleccionada = devuelve_puntero_a_unidades(sustitucion, fonema2, IZQUIERDO, locutor_actual);

    if (unidad_seleccionada == NULL) {

      cambio = 0;
      for (contador = 0; contador < numero_casos; contador++)
	if (strcmp(fonema2, originales[contador]) == 0){
	  cambio = 1;
	  strcpy(sustitucion, cambios[contador]);
	  break;
	}
      if (cambio == 1)
	unidad_seleccionada = devuelve_puntero_a_unidades(fonema1, sustitucion, IZQUIERDO, locutor_actual);


      if (unidad_seleccionada == NULL) {
	printf("Sustitución chunga del semifonema %s.%s(i).\n", fonema1, fonema2);
	if ( (unidad_seleccionada = devuelve_puntero_a_unidades("#", fonema2, IZQUIERDO, locutor_actual))
	     == NULL) { // Introducimos un silencio.
	  //                  puts("Introducimos un silencio.");
	  unidad_seleccionada = devuelve_puntero_a_unidades("#", "#", IZQUIERDO, locutor_actual);
	}
      }
    } // if (unidad_seleccionada == NULL)

  }

  return unidad_seleccionada;

}

/*
 *	\author	Fran Campillo
 *	\remarks	Función que recupera el vector del que se obtuvo el vector de
 * síntesis que se le pasa como parámetro.
 *	\param	vector_sintesis: vector de tipo Vector_sintesis en el que está la
 * información.
 *	\param	objetivo: Vector_descriptor_objetivo al que se refiere el vector
 * de síntesis.
 *	\param	locutor: puntero al locutor empleado en ese momento.
 *	\retval	semifonema: puntero al vector de salida.
 *	\return	En ausencia de error, devuelve un cero.
 */

int convierte_vector_sintesis_a_candidato(Vector_sintesis *vector_sintesis,
					  Vector_descriptor_objetivo &objetivo,
                                          Locutor *locutor,
                                          void **semifonema) {

  int identificador, numero_unidades, contador = 0;
  char izquierdo[OCTETOS_POR_FONEMA], central[OCTETOS_POR_FONEMA],
    derecho[OCTETOS_POR_FONEMA];
  unsigned char frase, posicion, acento, izq_der;
  estructura_unidades *unidad_seleccionada;

  Vector_semifonema_candidato *recorre_candidato;

  izq_der = vector_sintesis->izquierdo_derecho;

  identificador = vector_sintesis->devuelve_identificador();

  objetivo.devuelve_fonemas(central, izquierdo, derecho);
  objetivo.devuelve_frase_posicion_acento(&frase, &posicion, &acento);

  izq_der == IZQUIERDO ? unidad_seleccionada = devuelve_puntero_a_unidades(izquierdo, central, IZQUIERDO, locutor):
    unidad_seleccionada = devuelve_puntero_a_unidades(central, derecho, DERECHO, locutor);

  if (unidad_seleccionada != NULL) {

    recorre_candidato = unidad_seleccionada->unidades;
    numero_unidades = unidad_seleccionada->numero_unidades;

    while ( (recorre_candidato->identificador != identificador) &&
	    (++contador < numero_unidades) )
      recorre_candidato++;

    if ( (contador == numero_unidades) && (recorre_candidato->identificador != identificador) ) {
      fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato: Unidad (%d) no encontrada.\n",
	      identificador);
      return 1;
    } // if (contador == numero_unidades)..

    *semifonema = (void *) recorre_candidato;

  } // if (unidad_seleccionada != NULL...
  else {

    if (unidad_seleccionada == NULL)
      izq_der == IZQUIERDO ? unidad_seleccionada = reglas_sustitucion_semifonemas(izquierdo, central, IZQUIERDO, locutor):
	unidad_seleccionada = reglas_sustitucion_semifonemas(central, derecho, DERECHO, locutor);

    if (unidad_seleccionada == NULL) {
      if (izq_der == IZQUIERDO)
	fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato: no hay candidatos para %s.%s.\n", izquierdo, central);
      else
	fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato: no hay candidatos para %s.%s.\n", central, derecho);
      return 1;
    }

    recorre_candidato = unidad_seleccionada->unidades;
    numero_unidades = unidad_seleccionada->numero_unidades;

    while ( (recorre_candidato->identificador != identificador) &&
	    (++contador < numero_unidades) )
      recorre_candidato++;

    if ( (contador == numero_unidades) && (recorre_candidato->identificador != identificador) ) {
      fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato. Unidad (%d) no encontrada.\n",
	      identificador);
      return 1;
    } // if (contador == numero_unidades)

    *semifonema = (void *) recorre_candidato;

  } // else

  return 0;

}


/*
 *	\author	Fran Campillo
 *	\remarks	Función que recupera el vector del que se obtuvo el vector de
 * síntesis que se le pasa como parámetro.
 *	\param	vector_sintesis: vector de tipo Vector_sintesis en el que está la
 * información.
 *	\param	objetivo: Vector_descriptor_objetivo al que se refiere el vector
 * de síntesis.
 *	\param	locutor: puntero al locutor empleado en ese momento.
 *	\retval	semifonema: puntero al vector de salida.
 *	\return	En ausencia de error, devuelve un cero.
 */

int convierte_vector_sintesis_a_candidato_tonicidad(Vector_sintesis *vector_sintesis,
						    Vector_descriptor_objetivo *objetivo,
						    Locutor *locutor,
						    void **semifonema) {

  int identificador, numero_unidades, contador = 0;
  char izquierdo[OCTETOS_POR_FONEMA], central[OCTETOS_POR_FONEMA],
    derecho[OCTETOS_POR_FONEMA];
  unsigned char frase, posicion, acento, izq_der;
  estructura_unidades *unidad_seleccionada;
  Vector_semifonema_candidato *recorre_candidato;

  izq_der = vector_sintesis->izquierdo_derecho;

  identificador = vector_sintesis->devuelve_identificador();

  objetivo->devuelve_fonemas(central, izquierdo, derecho);
  objetivo->devuelve_frase_posicion_acento(&frase, &posicion, &acento);

  izq_der == IZQUIERDO ? unidad_seleccionada = devuelve_puntero_a_unidades(izquierdo, central, IZQUIERDO, locutor):
    unidad_seleccionada = devuelve_puntero_a_unidades(central, derecho, DERECHO, locutor);

  if (unidad_seleccionada != NULL) {

    recorre_candidato = unidad_seleccionada->unidades;
    numero_unidades = unidad_seleccionada->numero_unidades;

    while ( (recorre_candidato->identificador != identificador) &&
	    (++contador < numero_unidades) )
      recorre_candidato++;

    if ( (contador == numero_unidades) && (recorre_candidato->identificador != identificador) ) {
      fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato: Unidad (%d) no encontrada.\n",
	      identificador);
      return 1;
    } // if (contador == numero_unidades)..

    *semifonema = (void *) recorre_candidato;

  } // if (unidad_seleccionada != NULL...
  else {
    if (izq_der == DERECHO) { // Primero buscamos entre las oclusivas.

      if ( (unidad_seleccionada = devuelve_puntero_a_unidades(central, "p", DERECHO, locutor))
	   == NULL) // Probamos con "t".
	if ( (unidad_seleccionada = devuelve_puntero_a_unidades(central, "t", DERECHO, locutor))
	     == NULL) // Probamos con "k".
	  if ( (unidad_seleccionada = devuelve_puntero_a_unidades(central, "k", DERECHO, locutor))
	       == NULL) // ¡Situación límite...! Lo intentamos con el contexto silencio.
	    if ( (unidad_seleccionada = devuelve_puntero_a_unidades(central, "#", DERECHO, locutor))
		 == NULL) { // Introducimos un silencio.
	      //                                  puts("Introducimos un silencio.");
	      unidad_seleccionada = devuelve_puntero_a_unidades("#", "#", DERECHO, locutor);
	    }

    } // if (izq_der == DERECHO)
    else // En este caso, buscamos con un contexto silencio por la izquierda.

      if ( (unidad_seleccionada = devuelve_puntero_a_unidades("#", central, IZQUIERDO, locutor))
	   == NULL) { // Introducimos un silencio.
	//                  puts("Introducimos un silencio.");
	unidad_seleccionada = devuelve_puntero_a_unidades("#", "#", IZQUIERDO, locutor);
      }

    recorre_candidato = unidad_seleccionada->unidades;
    numero_unidades = unidad_seleccionada->numero_unidades;

    while ( (recorre_candidato->identificador != identificador) &&
	    (++contador < numero_unidades) )
      recorre_candidato++;

    if ( (contador == numero_unidades) && (recorre_candidato->identificador != identificador) ) {
      fprintf(stderr, "Error en convierte_vector_sintesis_a_candidato. Unidad (%d) no encontrada.\n",
	      identificador);
      return 1;
    } // if (contador == numero_unidades)

    *semifonema = (void *) recorre_candidato;

  } // else

  return 0;

}

//#ifdef _MODO_NORMAL

/**
 * Función:   escribe_cadena_escogida_final                                    
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.                 
 *			- informacion: si vale 0, se devuelve toda la información de la    
 *			  unidad. En caso contrario, sólo el identificador.				   
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

int escribe_cadena_escogida_final(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, Locutor * locutor_actual, char informacion, const char *opciones) {

  FILE *fichero;
  void *unidad_escogida;
  Vector_sintesis *recorre_sintesis = cadena_sintesis;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_semifonema_candidato *recorre_candidato;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_cadena_escogida, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (int contador = 0; contador < numero_unidades; recorre_sintesis++) {


#ifdef _SELECCION_ANTERIOR
    if (convierte_vector_sintesis_a_candidato(recorre_sintesis, *recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;
#else
    if (convierte_vector_sintesis_a_candidato_tonicidad(recorre_sintesis, recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;

#endif

    recorre_candidato = (Vector_semifonema_candidato *) unidad_escogida;

    if (informacion == 0) {

      //            fprintf(fichero, "%s\t(%s)\n", recorre_objetivo->semifonema,
      //                                           recorre_objetivo->palabra);
      recorre_candidato->escribe_datos_txt(fichero, *recorre_objetivo);

    }
    else
      fprintf(fichero, "%d\n", recorre_candidato->identificador);

    contador++;

    if ((contador % 2) == 0)
      recorre_objetivo++;

  } // for (int contador...

  fclose(fichero);

  return 0;

}

/**
 * Función:   escribe_costes_coarticulacion
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.    
 *          - locutor: locutor empleado.
 *          - mascara_contornos: si es distinto de NULL, indica que se ha hecho el cálculo de contornos en paralelo.
 *          - indice_coste_minimo: mejor contorno.
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.
 * \remarks Esta función sólo dará resultados coherentes con la fase de selección de unidades si se emplea Viterbi::calcula_C_t_peso_coarticulacion_texto como coste de objetivo.                     
 */

int escribe_costes_coarticulacion(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, Locutor * locutor_actual, char *mascara_contornos, int indice_coste_minimo, const char *opciones) {

  FILE *fichero;
  void *unidad_escogida;
  int frontera_prosodica_objetivo;
  float coste_inteligibilidad, peso_coarticulacion;
  Vector_sintesis *recorre_sintesis = cadena_sintesis;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_semifonema_candidato *recorre_candidato;
  
  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_cadena_escogida, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (int contador = 0; contador < numero_unidades; recorre_sintesis++) {


#ifdef _SELECCION_ANTERIOR
    if (convierte_vector_sintesis_a_candidato(recorre_sintesis, *recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;
#else
    if (convierte_vector_sintesis_a_candidato_tonicidad(recorre_sintesis, recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;

#endif

    recorre_candidato = (Vector_semifonema_candidato *) unidad_escogida;

    peso_coarticulacion = Viterbi::decide_peso_coarticulacion(*recorre_objetivo, recorre_candidato);

    coste_inteligibilidad += Viterbi::decide_peso_fronteras(*recorre_objetivo, recorre_candidato);

    coste_inteligibilidad += Viterbi::decide_peso_posicion_y_tipo_frase(*recorre_objetivo, recorre_candidato);

    if (recorre_candidato->acento != recorre_objetivo->acento)
      coste_inteligibilidad += 100;
   
    if (mascara_contornos == NULL)
      frontera_prosodica_objetivo = recorre_objetivo->frontera_prosodica;
    else
      frontera_prosodica_objetivo = recorre_objetivo->fronteras_prosodicas[indice_coste_minimo];

    // Coste de posición prosódica:
    coste_inteligibilidad += Viterbi::decide_peso_frontera_prosodica(frontera_prosodica_objetivo, recorre_candidato);

    fprintf(fichero, "%s\t(%s %s %s %s %s)\t(%s %s %s %s %s)\t(%c)\t%f\n", recorre_objetivo->semifonema, recorre_objetivo->contexto_izquierdo_2, recorre_objetivo->contexto_izquierdo, recorre_objetivo->semifonema, recorre_objetivo->contexto_derecho, recorre_objetivo->contexto_derecho_2, recorre_candidato->contexto_izquierdo_2, recorre_candidato->contexto_izquierdo, recorre_candidato->semifonema, recorre_candidato->contexto_derecho, recorre_candidato->contexto_derecho_2, recorre_candidato->izquierdo_derecho, coste_inteligibilidad);

    contador++;

    if ((contador % 2) == 0)
      recorre_objetivo++;

  } // for (int contador...

  fclose(fichero);

  return 0;

}

/**
 * \fn devuelve_tipo_frase_cadena
 * \brief Devuelve una cadena que representa el tipo de frase
 * \param[in] tipo_frase tipo de frase (FRASE_ENUNCIATIVA, FRASE_EXCLAMATIVA...)
 * \param[out] cadena_tipo_frase cadena que representa el tipo de frase
 * \return Si el tipo de frase existe, devuelve un cero
 */
 int devuelve_tipo_frase_cadena(int tipo_frase, char **cadena_tipo_frase) {

   switch (tipo_frase) {
   case FRASE_ENUNCIATIVA:
     *cadena_tipo_frase = (char *) "Frase enunciativa";
     break;
   case FRASE_EXCLAMATIVA:
     *cadena_tipo_frase = (char *) "Frase exclamativa";
     break;
   case FRASE_INTERROGATIVA:
     *cadena_tipo_frase = (char *) "Frase interrogativa";
     break;
   case FRASE_INACABADA:
     *cadena_tipo_frase = (char *) "Frase inacabada";
     break;
   default:
     fprintf(stderr, "Tipo de frase (%d) no contemplado.\n", tipo_frase);
     return 1;
   }

   return 0;

 }


/**
 * \fn devuelve_tipo_posicion_cadena
 * \brief Devuelve una cadena que representa la posición en la frase
 * \param[in] tipo_posicion POSICION_INICIAL, POSICION_FINAL...
 * \param[out] cadena_tipo_posicion cadena que representa la posición en la frase
 * \return Si la posición existe, devuelve un cero
 */
 int devuelve_tipo_posicion_cadena(int tipo_posicion, char **cadena_tipo_posicion) {

   switch (tipo_posicion) {
   case POSICION_INICIAL:
     *cadena_tipo_posicion = (char *) "Posición inicial";
     break;
   case POSICION_MEDIA:
     *cadena_tipo_posicion = (char *) "Posición media";
     break;
   case POSICION_FINAL:
     *cadena_tipo_posicion = (char *) "Posición final";
     break;
   default:
     fprintf(stderr, "Posición (%d) no contemplada.\n", tipo_posicion);
     return 1;
   }

   return 0;

 }

/**
 * \fn devuelve_tipo_frontera_prosodica_cadena
 * \brief Devuelve una cadena que representa el tipo de frontera prosódica
 * \param[in] tipo_frontera_prosodica UNIDAD_FIN_FRASE, UNIDAD_RUPTURA...
 * \param[out] cadena_tipo_frontera cadena que representa el tipo de frontera prosódica
 * \return Si la posición existe, devuelve un cero
 */
 int devuelve_tipo_frontera_prosodica_cadena(int tipo_frontera_prosodica, char **cadena_tipo_frontera) {

   switch (tipo_frontera_prosodica) {
   case UNIDAD_NO_FRONTERA:
     *cadena_tipo_frontera = (char *) "No final de frase";
     break;
   case UNIDAD_FIN_FRASE:
     *cadena_tipo_frontera = (char *) "Final de frase";
     break;
   case UNIDAD_RUPTURA:
     *cadena_tipo_frontera = (char *) "Ruptura entonativa o ruptura coma";
     break;
   default:
     fprintf(stderr, "Tipo de frontera (%d) no contemplado.\n", tipo_frontera_prosodica);
     return 1;
   }

   return 0;

 }

/**
 * Función:   escribe_discontinuidad_cepstrum                                  
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.                 
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 *			- numero_coeficientes: número de coeficientes de los que consta	   
 *			  cada vector.													   
 *			- cepstrum_fase: 0 -> vectores cepstrales.						   
 *							 otro valor -> fase.							   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */
#pragma argsused
int escribe_discontinuidad_espectral(int numero_unidades, char *nombre_fichero,
				     vector<Vector_descriptor_objetivo> &cadena_objetivo,
				     Vector_sintesis *cadena_sintesis,
				     Locutor * locutor_actual,
				     const char *opciones, int numero_coeficientes,
                                     char cepstrum_fase) {

  FILE *fichero;
  void *unidad_escogida;
  float distancia;
  Vector_sintesis *recorre_sintesis = cadena_sintesis;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_semifonema_candidato *recorre_candidato, *recorre_anterior_candidato = NULL;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_cadena_escogida, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (int contador = 0; contador < numero_unidades; recorre_sintesis++) {

#ifdef _SELECCION_ANTERIOR
    if (convierte_vector_sintesis_a_candidato(recorre_sintesis, *recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;
#else
    if (convierte_vector_sintesis_a_candidato_tonicidad(recorre_sintesis, recorre_objetivo, locutor_actual, &unidad_escogida) )
      return 1;

#endif

    recorre_candidato = (Vector_semifonema_candidato *) unidad_escogida;

    if (contador > 0) {

      if (recorre_candidato->izquierdo_derecho == IZQUIERDO) 
	distancia = calcula_distancia_euclidea_general((float *) recorre_anterior_candidato->cepstrum_final, (float *) recorre_candidato->cepstrum_inicial, numero_coeficientes);
      else {
	distancia = calcula_distancia_euclidea_general((float *) recorre_anterior_candidato->cepstrum_mitad, (float *) recorre_candidato->cepstrum_mitad, numero_coeficientes);
      }

    } // if ( (contador > 0) ...

    recorre_anterior_candidato = recorre_candidato;

    if (contador++ > 0)
      fprintf(fichero, "%f\n", distancia);
    else
      fprintf(fichero, "0\n");

    if ((contador % 2) == 0)
      recorre_objetivo++;

  } // for (int contador...

  fclose(fichero);

  return 0;

}


//#endif


/**
 * Función:   escribe_f0_objetivo                                      
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

int escribe_f0_objetivo(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, const char *opciones) {

  int contador;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_sintesis *parte_izquierda = cadena_sintesis;
  Vector_sintesis *parte_derecha = cadena_sintesis + 1;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_duraciones_objetivo, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (contador = 0; contador < numero_unidades; contador++, recorre_objetivo++, parte_izquierda += 2, parte_derecha += 2) {

    fprintf(fichero, "%s\t(%s)\t(%g, %g, %g, %g) => (%g, %g, %g, %g)\n", recorre_objetivo->semifonema, recorre_objetivo->palabra, recorre_objetivo->frecuencia_izquierda, recorre_objetivo->frecuencia_mitad, recorre_objetivo->frecuencia_mitad, recorre_objetivo->frecuencia_derecha, parte_izquierda->frecuencia_inicial, parte_izquierda->frecuencia_final, parte_derecha->frecuencia_inicial, parte_derecha->frecuencia_final);

  } // for (contador = 0...

  fclose(fichero);
    
  return 0;

}


/**
 * Función:   escribe_duraciones_objetivo                                      
 * \remarks Entrada:
 *			- numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

int escribe_duraciones_objetivo(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, const char *opciones) {

  int contador;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_sintesis *parte_izquierda = cadena_sintesis;
  Vector_sintesis *parte_derecha = cadena_sintesis + 1;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_duraciones_objetivo, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (contador = 0; contador < numero_unidades; contador++, recorre_objetivo++, parte_izquierda += 2, parte_derecha += 2) {

    if (recorre_objetivo->semifonema[0] != '#')
      fprintf(fichero, "%s\t(%s)\t(%g, %g) => (%g, %g)\n", recorre_objetivo->semifonema, recorre_objetivo->palabra, recorre_objetivo->duracion_1, recorre_objetivo->duracion_2, parte_izquierda->duracion, parte_derecha->duracion);
    else
      fprintf(fichero, "%s\t(%s)\t(%g, %g) => (%g, %g)\n", recorre_objetivo->semifonema, recorre_objetivo->palabra, recorre_objetivo->duracion_1, recorre_objetivo->duracion_2, parte_izquierda->duracion, parte_derecha->duracion);

  } // for (contador = 0...

  fclose(fichero);
    
  return 0;

}

/**
 * Función:   escribe_potencias_objetivo                                      
 * \remarks Entrada:
 *	    - numero_unidades: número de unidades de la cadena.                
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - cadena_sintesis: cadena de vectores de síntesis.
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

int escribe_potencias_objetivo(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, const char *opciones) {

  int contador;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();
  Vector_sintesis *parte_izquierda = cadena_sintesis;
  Vector_sintesis *parte_derecha = cadena_sintesis + 1;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_duraciones_objetivo, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  for (contador = 0; contador < numero_unidades; contador++, recorre_objetivo++, parte_izquierda += 2, parte_derecha += 2) {

    fprintf(fichero, "%s\t(%s)\t(%g, %g) => (%g, %g)\n", recorre_objetivo->semifonema, recorre_objetivo->palabra, recorre_objetivo->potencia, recorre_objetivo->potencia, parte_izquierda->potencia_final, parte_derecha->potencia_inicial);

  } // for (contador = 0...

  fclose(fichero);
    
  return 0;

}

/**
 * \brief Función que escribe en un fichero los contornos (uno por línea) que se consideran para la síntesis.
 * \param[in] objetivos Vector con los descriptores objetivo.
 * \param[in] nombre_fichero Nombre del fichero en el que se almacenan los contornos.
 * \param[in] opciones Opciones para la creación del fichero.
 * \return En ausencia de error, devuelve un cero.
 */
int escribe_contornos_fichero(vector<Vector_descriptor_objetivo> &objetivos, char *nombre_fichero, const char *opciones) {

  int numero_contornos = 1;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = objetivos.begin();
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_contornos_fichero, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  // Primero calculamos el número de contornos:

  while (recorre_objetivo != objetivos.end()) {

    if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {
      numero_contornos = recorre_objetivo->frecuencias_inicial.size();
      break;
    }
   
    ++recorre_objetivo;

  }

  for (int contador = 0; contador < numero_contornos; contador++) {
    for (recorre_objetivo = objetivos.begin(); recorre_objetivo != objetivos.end(); ++recorre_objetivo) {
      if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {
	fprintf(fichero, "%f\t%f\t", recorre_objetivo->frecuencias_inicial[contador], recorre_objetivo->frecuencias_mitad[contador]);
	if ( (recorre_objetivo + 2) == objetivos.end())
	  fprintf(fichero, "%f\t", recorre_objetivo->frecuencias_final[contador]);
      }
      else
	fprintf(fichero, "0\t0\t");
    }
    fprintf(fichero, "\n");
  }

  fclose(fichero);

  return 0;

}

/**
 * \brief Función que crea un fichero con la lista de identificadores de grupos acentuales de cada contorno escogido para la selección acústica
 * \param[in] cadena_acentuales array de punteros a cada contorno
 * \param[in] numero_contornos número de contornos
 * \param[in] numero_acentuales número de grupos acentuales de cada contorno
 * \param[in] nombre_fichero nombre del fichero de salida
 * \return En ausencia de error devuelve un cero
 */
int escribe_identificadores_contornos_fichero(Grupo_acentual_candidato **cadena_acentuales, int numero_contornos, int numero_acentuales, char *nombre_fichero) {

  Grupo_acentual_candidato *apunta_contorno;
  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_identificadores_contornos_fichero, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  for (int cuenta_contornos = 0; cuenta_contornos < numero_contornos; cuenta_contornos++) {
    
    apunta_contorno = cadena_acentuales[cuenta_contornos];
    
    for (int cuenta_gf = 0; cuenta_gf < numero_acentuales; cuenta_gf++, apunta_contorno++)
      fprintf(fichero, "%d\t", apunta_contorno->identificador);

    fprintf(fichero, "\n");
  } // for (int cuenta_contornos = 0...

  fclose(fichero);

  return 0;

}

int escribe_identificadores_contornos_fichero(vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_grupos, vector<int> &tamano_grupo_fonico, char *nombre_fichero) {

  int indice;

  vector<vector<Grupo_acentual_candidato * > >::iterator it_grupos = vector_grupos.begin();
  vector<Grupo_acentual_candidato *>::iterator it_acentual;
  vector<vector<float> >::iterator it_vector_costes = costes_grupos.begin();
  vector<float>::iterator it_coste;
  Grupo_acentual_candidato *acentuales_cand;

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_identificadores_contornos_fichero, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  }

  for (int contador = 0; contador < vector_grupos.size(); contador++, it_grupos++, it_vector_costes++) {
    
      indice = 0;
      fprintf(fichero, "Grupo fónico %d.\n", contador);
      it_acentual = it_grupos->begin();
      it_coste = it_vector_costes->begin();

      while (it_acentual != it_grupos->end()) {
	
	acentuales_cand = *it_acentual;
	fprintf(fichero, "%d.-\tf_i = %3.2f.\t", indice, acentuales_cand->frecuencia.marcas[0].frecuencia);
	for (int cuenta = 0; cuenta < tamano_grupo_fonico[contador]; cuenta++, acentuales_cand++) {
	  fprintf(fichero, "%d\t", acentuales_cand->identificador);
	}

	acentuales_cand--;
	fprintf(fichero, ", f_f = %3.2f.\tC = %f\n", acentuales_cand->frecuencia.marcas[acentuales_cand->frecuencia.numero_marcas - 1].frecuencia, *it_coste);
	indice++;
	it_coste++;
	it_acentual++;
      } // while (it_acentual != ...
      
    } // for (contador = 0; ...

    fclose(fichero);
    
    return 0;

}


/**
 * \remarks Función que escribe la secuencia de grupos acentuales candidato escogidos para una frase.
 * \param[in] numero_acentuales: número de grupos acentuales
 * \param[in] numero_unidades número de unidades acústicas
 * \param[in] nombre_fichero nombre del fichero en el que se desea introducir la información
 * \param[in] frase_fonetica frase con la transcripción fonética de la entrada
 * \param[in] cadena_objetivo cadena de vectores descriptor objetivo
 * \param[in] cadena_acentuales cadena de grupos acentuales candidatos
 * \param[in] acentuales_objetivo cadena de grupos acentuales objetivo
 * \param[in] opciones: opciones con las que se va a llamar a fopen para abrirel fichero
 * \return En ausencia de error devuelve un cero
 */

int escribe_contorno_escogido_final(int numero_acentuales, int numero_unidades, char *nombre_fichero, char * frase_fonetica, vector<Vector_descriptor_objetivo> &cadena_objetivo, Grupo_acentual_candidato *cadena_acentuales, Grupo_acentual_objetivo *acentuales_objetivo, const char *opciones) {

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_entorno_escogido_final, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }

  fprintf(fichero, "Contorno escogido\n");
    
  fprintf(fichero, "%s\n\n", frase_fonetica);

  if (numero_unidades == 1) {
    fclose(fichero);
    return 0;
  } // if (numero_unidades == 1)

  for (int contador = 0; contador < numero_acentuales; contador++, cadena_acentuales++, acentuales_objetivo++) {

    for (int cuenta = 0; cuenta < acentuales_objetivo->numero_alofonos;) {
      if (recorre_objetivo->semifonema[0] != '#') {
	if ( (recorre_objetivo->frontera_final == FRONTERA_PALABRA) ||
	     (recorre_objetivo->frontera_final == FRONTERA_GRUPO_ENTONATIVO) )
	  fprintf(fichero, "%s ", recorre_objetivo->semifonema);
	else
	  fprintf(fichero, "%s", recorre_objetivo->semifonema);
	cuenta++;
      }
      recorre_objetivo++;
    }
   
    fprintf(fichero, "\n");

    cadena_acentuales->escribe_datos_txt(fichero);


  }  // for (int contador = 0...

  fclose(fichero);

  return 0;

}

/**
 * Función:   escribe_secuencia_grupos_acentuales
 * \remarks Entrada:
 *	    - numero_acentuales: número de unidades de la cadena.              
 *	    - numero_unidades: número de unidades de voz objetivo.			   
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *			- frase_fonetica: frase con la transcripción fonética de la entrada.														   		
 *          - cadena_objetivo: cadena de vectores descriptor objetivo.         
 *          - primera: indica si es la primera frase del fichero o no.
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

//#ifdef _MODO_NORMAL

int escribe_secuencia_grupos_acentuales(int numero_acentuales, int numero_unidades, char *nombre_fichero, char * frase_fonetica, vector<Vector_descriptor_objetivo> &cadena_objetivo, int primera) {

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();

  FILE *fichero;

  if (primera == 1) {
    if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en escribe_secuencia_grupos_acentuales, al intentar crear el fichero %s\n", nombre_fichero);
      return 1;
    }
  } // if (primera == 1)
  else {
    if ( (fichero = fopen(nombre_fichero, "at")) == NULL) {
      fprintf(stderr, "Error en escribe_secuencia_grupos_acentuales, al intentar abrir el fichero %s para añadir nueva información.\n", nombre_fichero, "at");
      return 1;
    }
  } // else


  if (primera == 1)
    fprintf(fichero, "%s\n\n", frase_fonetica);

  if (numero_unidades == 1) {
    fclose(fichero);
    return 0;
  } // if (numero_unidades == 1)

  for (int contador = 0; contador < numero_acentuales; contador++) {
    while (recorre_objetivo->acento != 1) {
      if (recorre_objetivo->semifonema[0] != '#') {
	if ( (recorre_objetivo->frontera_final == FRONTERA_PALABRA) ||
	     (recorre_objetivo->frontera_final == FRONTERA_GRUPO_ENTONATIVO) )
	  fprintf(fichero, "%s ", recorre_objetivo->semifonema);
	else
	  fprintf(fichero, "%s", recorre_objetivo->semifonema);
      }
      recorre_objetivo++;
      if (recorre_objetivo->semifonema[0] == '#')
	break;
    } // while (recorre_objetivo...
    while ( (recorre_objetivo->frontera_final != FRONTERA_PALABRA) &&
	    (recorre_objetivo->frontera_final != FRONTERA_GRUPO_ENTONATIVO) &&
	    (recorre_objetivo->semifonema[0] != '#') ) {
      fprintf(fichero, "%s", recorre_objetivo->semifonema);
      recorre_objetivo++;
    } // while ( (recorre_objetivo...

    fprintf(fichero, "%s / ", recorre_objetivo++->semifonema);

  }  // for (int contador = 0...

  fprintf(fichero, "\n");

  fclose(fichero);

  return 0;

}

//#endif

/**
 * \brief Función que escribe la transcripción fonética de la frase de entrada, añadiéndole la información de las nuevas pausas y rupturas entonativas introducidas por los respectivos modelos
 * \param[in] numero_acentuales número de unidades de la cadena
 * \param[in] numero_unidades número de unidades de voz objetivo
 * \param[in] nombre_fichero nombre del fichero en el que se desea introducir la información
 * \param[in] cadena_objetivo cadena de vectores descriptor objetivo
 * \param[in] cadena_acentuales cadena de grupos acentuales candidato
 * \param[in] acentuales_objetivo cadena de grupos acentuales objetivo
 * \param[in] opciones: opciones con las que se va a llamar a fopen para abrir el fichero.
 * \return En ausencia de error se devuelve un cero.
 */

//#ifdef _MODO_NORMAL

int escribe_frase_con_rupturas_y_pausas(int numero_acentuales, int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Grupo_acentual_candidato *cadena_acentuales, Grupo_acentual_objetivo *acentuales_objetivo, const char *opciones) {

  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = cadena_objetivo.begin();

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_entorno_escogido_final, al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }
  if (numero_unidades == 1) {
    fclose(fichero);
    return 0;
  } // if (numero_unidades == 1)

  for (int contador = 0; contador < numero_acentuales; contador++, cadena_acentuales++, acentuales_objetivo++) {

    for (int cuenta = 0; cuenta < acentuales_objetivo->numero_alofonos; ) {
      
      if (recorre_objetivo->semifonema[0] != '#') {
	
	if ( (recorre_objetivo->frontera_final == FRONTERA_PALABRA) ||
	     (recorre_objetivo->frontera_final == FRONTERA_GRUPO_ENTONATIVO ) )
	  fprintf(fichero, "%s ", recorre_objetivo->semifonema);
	else
	  fprintf(fichero, "%s", recorre_objetivo->semifonema);

	cuenta++;
      }
      recorre_objetivo++;

    } // for (int cuenta = 0; cuenta < ...

    // while (recorre_objetivo->acento != 1) {
    //   if (recorre_objetivo->semifonema[0] != '#') {
    // 	if ( (recorre_objetivo->frontera_final == FRONTERA_PALABRA) ||
    // 	     (recorre_objetivo->frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) 
    // 	  fprintf(fichero, "%s ", recorre_objetivo->semifonema);
    // 	else
    // 	  fprintf(fichero, "%s", recorre_objetivo->semifonema);
    //   }
    //   recorre_objetivo++;
    //   if (recorre_objetivo->semifonema[0] == '#')
    // 	break;
    // } // while (recorre_objetivo...
    // while ( (recorre_objetivo->frontera_final != FRONTERA_PALABRA) &&
    // 	    (recorre_objetivo->frontera_final != FRONTERA_GRUPO_ENTONATIVO) &&
    // 	    (recorre_objetivo->semifonema[0] != '#') ) {
    //   fprintf(fichero, "%s", recorre_objetivo->semifonema);
    //   recorre_objetivo++;
    // } // while ( (recorre_objetivo...

    // fprintf(fichero, "%s ", recorre_objetivo++->semifonema);

    switch (cadena_acentuales->tipo_pausa) {
    case PAUSA_COMA:
    case PAUSA_RUPTURA_COMA:
      fprintf(fichero, ", ");
      break;
    case PAUSA_PUNTOS_SUSPENSIVOS:
      fprintf(fichero, "... ");
      break;
    case PAUSA_RUPTURA_ENTONATIVA:
      fprintf(fichero, "%s ", SIMBOLO_RUPTURA_ENTONATIVA);
      break;
    case PAUSA_PUNTO:
      fprintf(fichero, ".\n");
      break;
    case PAUSA_OTRO_SIGNO_PUNTUACION:
      fprintf(fichero, "<> ");
      break;
    default:
      break;
    }


  }  // for (int contador = 0...

  fclose(fichero);

  return 0;

}

/**
 * Función:   escribe_contorno_escogido_calculo_distancia                      
 * \remarks Entrada:
 *			- numero_acentuales: número de unidades de la cadena.              
 *			- numero_unidades: número de unidades de voz objetivo.			   
 *          - nombre_fichero: nombre del fichero en el que se desea introducir 
 *            la información.                                                  
 *          - cadena_acentuales: cadena de grupos acentuales candidatos.       
 *			- frase_entrada: frase de entrada. 								   
 *			- coste_camino: coste relacionado con la secuencia de unidades	   
 *			  acústicas seleccionadas.										   	
 *          - opciones: opciones con las que se va a llamar a fopen para abrir 
 *            el fichero.                                                      
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

//#ifdef _MODO_NORMAL

int escribe_contorno_escogido_calculo_distancia(int numero_acentuales,
						char *nombre_fichero,
						Grupo_acentual_candidato *cadena_acentuales,
						char *frase_entrada, float coste_camino, 
						const char *opciones) {

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
    fprintf(stderr, "Error en escribe_entorno_escogido_calculo_distancia, \
al intentar abrir el fichero %s con las opciones %s.\n",
            nombre_fichero, opciones);
    return 1;
  }                                          

  fprintf(fichero, "%s\n", frase_entrada);
  fprintf(fichero, "FIN-DE-FRASE\n");
  fprintf(fichero, "%f\n", coste_camino);
    
  fprintf(fichero, "%d\n", numero_acentuales);

  for (int contador = 0; contador < numero_acentuales; contador++, cadena_acentuales++) {

    fprintf(fichero, "%d\n", cadena_acentuales->numero_silabas + 2);
        
    cadena_acentuales->escribe_contorno(fichero);

  }  // for (int contador = 0...

  fclose(fichero);

  return 0;

}

/**
 * Función:   escribe_indices_modificados                                      
 * \remarks Entrada:
 *			- unidades: vector de unidades escogidas.                          
 *          - numero_unidades: número de elementos de la cadena anterior.      
 *          - indice_frase: índice de la frase del fichero actual.             
 *	\remarks Salida:
 *			- fichero: fichero en el que se escribe la información de las      
 *            unidades modificadas.                                            
 */

void escribe_indices_modificados(Vector_sintesis *unidades, int numero_unidades,
                                 int indice_frase, FILE *fichero) {

  char validez;
  int modificados_duracion = 0;
  int modificados_frecuencia = 0;
  int ambas_modificaciones = 0;

  for (int contador = 0; contador < numero_unidades; unidades++, contador++) {

    validez = unidades->devuelve_validez();

    if (validez == 3)
      ambas_modificaciones++;
    else
      if (validez == 2)
	modificados_duracion++;
      else
	if (validez == 1)
	  modificados_frecuencia++;

  }

  fprintf(fichero, "%d\t%d\t%d\t%d\t%d\n", indice_frase, numero_unidades, modificados_duracion,
	  modificados_frecuencia, ambas_modificaciones);
  fflush(fichero);
}


/**
 * Función:   escribe_factores_modificacion                                    
 * \remarks Entrada:
 *			- unidades: vector de unidades de síntesis escogidas.              
 *          - vectores: vectores descriptores candidatos.                      
 *          - objetivos: vectores descriptores objetivo.                       
 *          - numero_unidades: número de elementos de la cadena anterior.      
 *          - indice_frase: índice de la frase del fichero actual.             
 *	\remarks Salida:
 *			- fichero: fichero en el que se escribe la información de las      
 *            unidades modificadas.                                            
 */

void escribe_factores_modificacion(Vector_sintesis *unidades, Vector_semifonema_candidato *vectores,
                                   Vector_descriptor_objetivo *objetivos,
                                   int numero_unidades,
                                   int indice_frase, FILE *fichero) {

  char validez;
  float factor_duracion, factor_frecuencia;
  fprintf(fichero, "%d\t%d\t", indice_frase, numero_unidades);

  for (int contador = 0; contador < numero_unidades; vectores++, unidades++, contador++) {

    validez = unidades->devuelve_validez();

    if (sordo_sonoro(objetivos->semifonema) == SONORO) {
      if (validez & 0x1) {// se modifica la frecuencia.
	if (vectores->izquierdo_derecho == DERECHO)
	  factor_frecuencia = vectores->frecuencia_inicial / objetivos->frecuencia_mitad;
	else // IZQUIERDO
	  factor_frecuencia = vectores->frecuencia_final / objetivos->frecuencia_mitad;
      } // if (validez & 0x1)
      else
	factor_frecuencia = 1.0;
    } // if (sordo_sonoro(...
    else
      factor_frecuencia = 1;

    if ( (validez & 0x2) && (objetivos->semifonema[0] != '#') ) // se modifica la duración.
      factor_duracion = vectores->duracion / objetivos->duracion_1;
    else
      factor_duracion = 1.0;

    fprintf(fichero, "%s\t%f\t%f\t", objetivos->semifonema, factor_duracion, factor_frecuencia);

    if (contador & 0x1)
      objetivos++;

  } // for (int contador = 0; ...

  fprintf(fichero, "\n");

  fflush(fichero);
}


/**
 * Función:   escribe_factores_funcion_coste_entonativa		                   
 * \remarks Entrada:
 *			- cadena_acentuales: cadena de grupos acentuales objetivo.         
 *          - contorno_escogido: cadena de grupos acentuales escogidos.		   
 *          - numero_acentuales: número de elementos de las cadenas anteriores.
 *	\remarks Salida:
 *			- fichero_ent_dur: fichero en el que se almacena la información de 
 *			  la relación entre el número de sílabas y la duración de los 	   
 *	  	  	  grupos buscados y los escogidos.								   
 *			- fichero_pos_acentual: fichero en el que se almacena la 		   
 *			  información de la relación entre la posición del grupo acentual  
 *			  buscado y el escogido, en el grupo fónico al que pertenece cada  
 *			  uno. Sólo se aplica a los grupos intermedios.					   
 *			- fichero_pos_fonico: fichero análogo al anterior, pero referido a 
 *			  la posición del grupo fónico en la frase.						   
 */

void escribe_factores_funcion_coste_entonativa(Grupo_acentual_objetivo *cadena_acentuales,
					       Grupo_acentual_candidato *contorno_escogido,
                                               int numero_acentuales, FILE *fichero_ent_dur,
                                               FILE *fichero_pos_acentual,
                                               FILE *fichero_pos_fonico) {


  int posicion_obj;
  float pos_obj, pos_cand, duracion_cand, duracion_obj;

  for (; numero_acentuales > 0; numero_acentuales--, cadena_acentuales++, contorno_escogido++) {

    // Número de sílabas y duración:

    duracion_cand = 1000*(contorno_escogido->final - contorno_escogido->inicio);
    duracion_obj = 1000*cadena_acentuales->duracion;

    fprintf(fichero_ent_dur, "%d\t%f\n",
	    cadena_acentuales->numero_silabas - contorno_escogido->numero_silabas,
	    duracion_obj - duracion_cand);


    // Posición de grupo acentual en grupo fónico:

    posicion_obj = cadena_acentuales->posicion_grupo_acentual;

    if ( (posicion_obj != 0) &&
	 (posicion_obj != cadena_acentuales->grupos_acentuales_grupo_prosodico - 1) ) {

      pos_obj = (float) posicion_obj /
	(cadena_acentuales->grupos_acentuales_grupo_prosodico - 2);
      pos_cand = (float) contorno_escogido->posicion_grupo_acentual /
	(contorno_escogido->grupos_acentuales_grupo_prosodico - 2);

      fprintf(fichero_pos_acentual, "%f\n", pos_obj - pos_cand);

    } // if ( (posicion_obj != 0) && ...

    // Posición de grupo fónico en frase:

    if (cadena_acentuales->posicion_grupo_prosodico)
      pos_obj = (float) cadena_acentuales->posicion_grupo_prosodico /
	(cadena_acentuales->numero_grupos_prosodicos - 1);
    else
      pos_obj = 0.0;

    if (contorno_escogido->posicion_grupo_prosodico)
      pos_cand = (float) contorno_escogido->posicion_grupo_prosodico /
	(contorno_escogido->numero_grupos_prosodicos - 1);
    else
      pos_cand = 0.0;

    fprintf(fichero_pos_fonico, "%f\n", pos_obj - pos_cand);

  } // for (; numero_acentuales > 0; ...

}


/**
 * Función:   escribe_factores_f0_duracion_y_tamano_continuo
 * \remarks Entrada:
 *			- vectores: vectores descriptores candidatos.
 *          - objetivos: vectores descriptores objetivo.
 *          - numero_unidades: número de elementos de la cadena anterior.
 *	\remarks Salida:
 *			- fichero_f0: fichero en el que se escribe la información de las
 *            parejas frecuencia objetivo - frecuencia escogida.
 *          - fichero_dur: fichero en el que se escribe la información de las
 *            parejas duración objetivo - duración escogida.
 *          - fichero_rafaga: fichero en el que se escriben las ráfagas de
 *            unidades escogidas consecutivas en la base de datos.
 *          - fichero_concatena: fichero en el que se van a escribir los
 *            puntos en los que se realiza la concatenación entre las unidades
 *			- fichero_fonema_coart: fichero en el que se almacenan los fonemas
 *			  entre los que se realiza una concatenación por la parte de la
 *			  coarticulación.
 *			- fichero_contextual: fichero en el que se almacena el coste
 *			  contextual, el prosódico y su suma ponderada (coste de objetivo)
 *			- fichero_func_concatena: fichero en el que se almacena el valor
 *			  de los costes de concatenación.
 *			- fichero_diferencias_prosodicas: fichero con las diferencias en
 *			  los valores deseados y encontrados de prosodia (f0, duración y
 *			  energía.
 * \remarks Valor devuelto:
 *			- En ausencia de error devuelve un cero.
 */
#pragma argused
#ifdef _MODO_NORMAL
int escribe_factores_f0_duracion_y_tamano_continuo(Vector_sintesis *vectores,
                                                   vector<Vector_descriptor_objetivo> &objetivos,
						   Locutor * locutor_actual,
                                                   int numero_unidades,
                                                   FILE *fichero_f0,
                                                   FILE *fichero_dur, FILE *fichero_rafaga,
                                                   FILE *fichero_concatena,
                                                   FILE *fichero_fonema_coart,
                                                   FILE *fichero_contextual,
                                                   FILE *fichero_func_concatena,
                                                   FILE *fichero_diferencias_prosodicas) {

  const char *fonema_ant, *fonema;
  unsigned char izq_der = IZQUIERDO;
  int cuenta_seguidas = 1;
  int descriptor_anterior = -1;
  char semifonema_anterior[OCTETOS_POR_FONEMA] = "#", semifonema[OCTETOS_POR_FONEMA];
  float frecuencia_inicial, frecuencia_final, duracion;
  float coste_contextual, coste_prosodico, coste_concatenacion;
  float coste_f0, coste_duracion, coste_energia;
  float diferencia_f0_est, diferencia_f0_tran, diferencia_duracion, diferencia_energia;
  void *vector_auxiliar;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = objetivos.begin();
  Vector_semifonema_candidato *vector_semifonema, *anterior_vector_semifonema = NULL,
    vector_auxiliar_semifonema;

  for (int contador = 0; contador < numero_unidades; vectores++, contador++) {

#ifdef _SELECCION_ANTERIOR

    if (convierte_vector_sintesis_a_candidato(vectores, *recorre_objetivo, locutor_actual, &vector_auxiliar))
      return 1;
    
#else

    if (convierte_vector_sintesis_a_candidato_tonicidad(vectores, *recorre_objetivo, locutor_actual, &vector_auxiliar))
      return 1;

#endif

    vector_semifonema = (Vector_semifonema_candidato *) vector_auxiliar;

    frecuencia_inicial = vector_semifonema->frecuencia_inicial;
    frecuencia_final = vector_semifonema->frecuencia_final;
    duracion = vector_semifonema->duracion;
    strcpy(semifonema, vector_semifonema->semifonema);

    if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {

      if (izq_der == IZQUIERDO) { // Primero zona de transición y luego estacionaria.
	fprintf(fichero_f0, "%f\t", recorre_objetivo->frecuencia_izquierda - frecuencia_inicial);
	fprintf(fichero_f0, "%f\n", recorre_objetivo->frecuencia_mitad - frecuencia_final);
      } // if (izq_der == IZQUIERDO)
      else { // DERECHO
	fprintf(fichero_f0, "%f\t", recorre_objetivo->frecuencia_derecha - frecuencia_final);
	fprintf(fichero_f0, "%f\n", recorre_objetivo->frecuencia_mitad - frecuencia_inicial);
      } // else

    } // if (sordo_sonoro

    if (recorre_objetivo->semifonema[0] != '#') // Obviamos los silencios.
      fprintf(fichero_dur, "%f\n", 1000*(recorre_objetivo->duracion_1 - duracion));

    if (descriptor_anterior != -1) {
      if (izq_der == DERECHO) {
	if (vectores->identificador == descriptor_anterior)
	  cuenta_seguidas++;
	else {
	  descriptor_anterior = vectores->identificador;
	  //                    fprintf(fichero_rafaga, "%d\t", cuenta_seguidas);
	  fprintf(fichero_rafaga, "%d\n", cuenta_seguidas);
	  cuenta_seguidas = 1;
	  fprintf(fichero_concatena, "EST\n");
	} // else
      } // if (izq_der == DERECHO
      else {
	if (vectores->identificador == descriptor_anterior + 1)
	  cuenta_seguidas++;
	else {
	  //                    fprintf(fichero_rafaga, "%d\t", cuenta_seguidas);
	  fprintf(fichero_rafaga, "%d\n", cuenta_seguidas);
	  cuenta_seguidas = 1;
	  fprintf(fichero_concatena, "COART\n");
	  if ( ( (fonema_ant = tipo_fon_espectral_cadena(semifonema_anterior)) == NULL) ||
	       ( (fonema = tipo_fon_espectral_cadena(semifonema)) == NULL) )
	    return 1;
	  fprintf(fichero_fonema_coart, "%s\t%s\t%s\t%d\t%s\t%d\n", fonema_ant, fonema,
		  semifonema_anterior, descriptor_anterior,
		  semifonema, vectores->identificador);
	} // else
	descriptor_anterior = vectores->identificador;
      } // else
    } // if (descriptor_anterior != -1;)
    else
      descriptor_anterior = vectores->identificador;

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)
    calcula_costes_objetivo_red_neuronal(vector_semifonema, *recorre_objetivo, izq_der, locutor_actual, &coste_prosodico, &coste_contextual, &coste_f0, &coste_duracion, &coste_energia, &diferencia_f0_est, &diferencia_f0_tran, &diferencia_duracion, &diferencia_energia); 
#elif defined(_UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES)
    fprintf(stderr, "Función no implementada para el modo _UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES.\n");
    return 1;
#else
    calcula_costes_objetivo_regresion_lineal(vector_semifonema, *recorre_objetivo, izq_der, locutor_actual, &coste_prosodico, &coste_contextual, &coste_f0, &coste_duracion, &coste_energia, &diferencia_f0_est, &diferencia_f0_tran, &diferencia_duracion, &diferencia_energia);
#endif
    fprintf(fichero_contextual, "%f\t%f\t%f\t%f\t%f\t%f\n", coste_contextual, coste_prosodico, coste_prosodico*COEFICIENTE_PROSODIA + coste_contextual*COEFICIENTE_INTELIGIBILIDAD, coste_f0, coste_duracion, coste_energia);

    fprintf(fichero_diferencias_prosodicas, "%f\t%f\t%f\t%f\n", diferencia_f0_est, diferencia_f0_tran, diferencia_duracion, diferencia_energia);
    if (contador > 0) {
      coste_concatenacion = calcula_C_c(anterior_vector_semifonema, vector_semifonema, izq_der, locutor_actual);

      fprintf(fichero_func_concatena, "%f\n", coste_concatenacion);
    } // if (contador > 0)

    anterior_vector_semifonema = vector_semifonema;

    if (contador & 0x1)
      recorre_objetivo++;

    if (izq_der == IZQUIERDO)
      izq_der = DERECHO;
    else
      izq_der = IZQUIERDO;

    strcpy(semifonema_anterior, semifonema);


  } // for (int contador = 0...

  fprintf(fichero_rafaga, "%d\n", cuenta_seguidas);

  return 0;

}

/**
 * Función:   escribe_factores_calidad_voz_sintetica
 * \remarks Entrada:
 *			- vectores: vectores descriptores candidatos.
 *          - objetivos: vectores descriptores objetivo.
 *          - numero_unidades: número de elementos de la cadena anterior.
 *			- locutor_actual: puntero al locutor empleado.
 * 			- coste_entonativo: coste de la búsqueda entonativa.
 *			- coste_acustico: coste de la búsqueda acústica.
 *			- nombre_fichero: nombre del fichero de salida.
 * \return	En ausencia de error devuelve un cero.
 */
int escribe_factores_calidad_voz_sintetica(Vector_sintesis *vectores, vector<Vector_descriptor_objetivo> &objetivos, Locutor *locutor_actual, int numero_unidades, float coste_entonativo, float coste_acustico, char *nombre_fichero) {

  unsigned char izq_der = IZQUIERDO;
  int cuenta_seguidas = 1;
  int descriptor_anterior = -1;
  int numero_unidades_sonoras = 0;
  float frecuencia_inicial, frecuencia_final, duracion, valor_auxiliar;
  double diferencia_f0 = 0.0, diferencia_duracion = 0.0;

  void *vector_auxiliar;
  vector<Vector_descriptor_objetivo>::iterator recorre_objetivo = objetivos.begin();
  Vector_semifonema_candidato *vector_semifonema;

  FILE *fichero_salida;

  if ( (fichero_salida = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_factores_calidad_voz_sintetica, al crear el fichero %s.\n", nombre_fichero);
    return 1;
  }

  for (int contador = 0; contador < numero_unidades; vectores++, contador++) {

    if (convierte_vector_sintesis_a_candidato(vectores, *recorre_objetivo, locutor_actual, &vector_auxiliar))
      return 1;

    vector_semifonema = (Vector_semifonema_candidato *) vector_auxiliar;

    frecuencia_inicial = vector_semifonema->frecuencia_inicial;
    frecuencia_final = vector_semifonema->frecuencia_final;
    duracion = vector_semifonema->duracion;

    if (sordo_sonoro(recorre_objetivo->semifonema) == SONORO) {

      numero_unidades_sonoras++;

      if (izq_der == IZQUIERDO) { // Primero zona de transición y luego estacionaria.
	valor_auxiliar = absoluto(recorre_objetivo->frecuencia_izquierda - frecuencia_inicial);
	if (valor_auxiliar > FREC_UMBRAL)
	  diferencia_f0 += valor_auxiliar;
	valor_auxiliar = absoluto(recorre_objetivo->frecuencia_mitad - frecuencia_final);
	if (valor_auxiliar > FREC_UMBRAL)
	  diferencia_f0 += valor_auxiliar;
      } // if (izq_der == IZQUIERDO)
      else { // DERECHO
	valor_auxiliar = absoluto(recorre_objetivo->frecuencia_derecha - frecuencia_final);
	if (valor_auxiliar > FREC_UMBRAL)
	  diferencia_f0 += valor_auxiliar;
	valor_auxiliar = absoluto(recorre_objetivo->frecuencia_mitad - frecuencia_inicial);
	if (valor_auxiliar > FREC_UMBRAL)
	  diferencia_f0 += valor_auxiliar;
      } // else

    } // if (sordo_sonoro

    if (recorre_objetivo->semifonema[0] != '#') { // Obviamos los silencios.
      valor_auxiliar = absoluto(recorre_objetivo->duracion_1 - duracion);
      if (valor_auxiliar > DUR_UMBRAL)
	diferencia_duracion += valor_auxiliar;
    }

    if (descriptor_anterior != -1) {
      if (izq_der == DERECHO) {
	if (vectores->identificador == descriptor_anterior)
	  cuenta_seguidas++;
      } // if (izq_der == DERECHO
      else {
	if (vectores->identificador == descriptor_anterior + 1)
	  descriptor_anterior = vectores->identificador;
      } // else
    } // if (descriptor_anterior != -1;)

    descriptor_anterior = vectores->identificador;

    if (contador & 0x1)
      recorre_objetivo++;

    if (izq_der == IZQUIERDO)
      izq_der = DERECHO;
    else
      izq_der = IZQUIERDO;

  } // for (int contador = 0...

  fprintf(fichero_salida, "%f\t%f\t%f\t%f\t%d\n", coste_entonativo, coste_acustico, diferencia_f0 / numero_unidades_sonoras,
	  diferencia_duracion / numero_unidades, cuenta_seguidas);
    
  fclose(fichero_salida);

  return 0;

}

#endif

float calcula_C_c(Vector_semifonema_candidato *unidad1, Vector_semifonema_candidato *unidad2, unsigned char izq_der, Locutor *locutor) {

  float frec_union2, frec_union1;
  float pot_union1, pot_union2;
  //    char fonema;

  int cont1, cont2;

  float coste, coste_cepstral;

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (unidad1->identificador >= locutor->primera_unidad_corpus500) ||
       ( (unidad1->identificador >= menor_identificador) &&
	 (unidad1->identificador <= mayor_identificador))  )
    return 1000;
  if ( (unidad2->identificador >= locutor->primera_unidad_corpus500) ||
       ( (unidad2->identificador >= menor_identificador) &&
	 (unidad2->identificador <= mayor_identificador)) )
    return 1000;
#endif

  cont1 = unidad1->identificador;
  cont2 = unidad2->identificador;

  if ( (cont1 == cont2) || (cont2 == cont1 + 1) ) {
    return 0.0; // Las unidades son consecutivas. Su coste de concatenación es nulo.
  }

  if (unidad1->izquierdo_derecho == DERECHO) // Parte menos estable
    coste = 5.0;
  //		coste = 1000;
  else
    coste = 0.0;

  if ( (unidad1->marca_inventada == 1) || (unidad2->marca_inventada == 1) )
    coste += 10;

  // Si la unión se realiza por la parte de la coarticulación, damos más importancia
  // al peso:


  frec_union2 = unidad2->frecuencia_inicial;
  frec_union1 = unidad1->frecuencia_final;

  coste += absoluto((frec_union2 - frec_union1)) / maximo(frec_union2, frec_union1);

  float *cepstrum1;
  float *cepstrum2;

  //        float diferencia;

  if (izq_der == IZQUIERDO) {
    cepstrum1 = (float *) unidad1->cepstrum_mitad;
    cepstrum2 = (float *) unidad2->cepstrum_mitad;

    coste_cepstral = calcula_distancia_euclidea(cepstrum1, cepstrum2, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    coste += 4*locutor->indices_continuidad_coart[(unsigned char) unidad1->tipo_espectral][(unsigned char) unidad2->tipo_espectral]*
      coste_cepstral / Vector_semifonema_candidato::numero_coeficientes_cepstrales;
  } // if (izq_der == IZQUIERDO)
  else {
    cepstrum1 = (float *) unidad1->cepstrum_final;
    cepstrum2 = (float *) unidad2->cepstrum_inicial;

    coste_cepstral = calcula_distancia_euclidea(cepstrum1, cepstrum2, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

    coste += 4*locutor->indices_continuidad_est[(unsigned char) unidad1->tipo_espectral]*
      coste_cepstral / Vector_semifonema_candidato::numero_coeficientes_cepstrales;

  }

  pot_union2 = unidad2->potencia_inicial;
  pot_union1 = unidad1->potencia_final;


  if (pot_union2 != pot_union1) // Antes 1.8
    coste += absoluto((pot_union2 - pot_union1)) /
      maximo(absoluto(pot_union2), absoluto(pot_union1));

  return coste;
  // De momento vamos a pasar de la caché.

}

#ifdef _UTILIZA_REGRESION

/**
 * Función:   calcula_costes_objetivo_regresion_lineal
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.
 *          - objetivo: identificador de la unidad objetivo.
 *          - izq_der: IZQUIERDO o DERECHO.
 *			- locutor: puntero al locutor que se está empleando.
 *	\remarks Salida:	- coste_prosodico: parte de la función de coste de objetivo referida a la
 *			  prosodia.
 *			- coste_contextual: parte de la función de coste de objetivo referida al
 *			  contexto.
 *			- coste_f0: coste de frecuencia fundamental.
 *			- coste_duracion: coste de duración.
 *			- coste_energia: coste de energia.
 *			- diferencia_f0_est: diferencia de f0 en la zona estacionaria.
 *			- diferencia_f0_tran: diferencia de f0 en la zona de transición.
 *			- diferencia_duracion: diferencia de duración.
 *			- diferencia_energia: diferencia de energía.
 * \remarks Objetivo:  Calcula los costes contextual y prosódico.
 */

void calcula_costes_objetivo_regresion_lineal(Vector_semifonema_candidato *candidato,
					      Vector_descriptor_objetivo *objetivo,
					      char izq_der, Locutor *locutor,
					      float *coste_prosodico,
					      float *coste_contextual,float *coste_f0,
					      float *coste_duracion, float *coste_energia,
					      float *diferencia_f0_est, float *diferencia_f0_tran,
					      float *diferencia_duracion,
					      float *diferencia_energia) {

  // Revisar esta función... Al cambiar los vectores cepstrum ya no se corresponde con el original

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  float *pesos;
  float coste_inteligibilidad, coste_prosodia = 0.0;

  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur, diferencia_potencia;

  float duracion_obj;
  float potencia_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;

  float duracion_cand, potencia_cand;

  //    char *apunta_candidato;
  //    char *apunta_objetivo;

  //    pesos = devuelve_vector_pesos((clase_espectral) candidato->tipo_espectral);

  pesos = locutor->pesos_regresion[candidato->tipo_espectral];

  coste_inteligibilidad = *pesos++;

  indice_central_obj = devuelve_indice_fonema(candidato->semifonema);
  indice_fut1_obj = devuelve_indice_fonema(objetivo->contexto_derecho);
  indice_pas1_obj = devuelve_indice_fonema(objetivo->contexto_izquierdo);


  duracion_cand = candidato->duracion;
  potencia_obj = objetivo->potencia;

  if ( (frec_cent_obj = objetivo->frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo->duracion_1;

    potencia_cand = candidato->potencia_final;

    if ( (frec_izda_obj = objetivo->frecuencia_izquierda) == 0)
      frec_izda_obj = 100;


    // Anterior fonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial);

    // Semifonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad);

    // Siguiente fonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_final);


    *diferencia_f0_tran = frec_izda_obj - candidato->frecuencia_inicial;

    diferencia_frec_coart = absoluto((frec_izda_obj - candidato->frecuencia_inicial));

    // Frecuencia en la zona de coarticulación:

    coste_inteligibilidad += *pesos++*(diferencia_frec_coart/frec_izda_obj);

    *diferencia_f0_est = frec_cent_obj - candidato->frecuencia_final;

    diferencia_frec_est = absoluto((frec_cent_obj - candidato->frecuencia_final));

    // Frecuencia en la zona estacionaria:

    coste_inteligibilidad += *pesos++*(diferencia_frec_est/frec_cent_obj);


  } // if (izq_der == IZQUIERDO)

  else { // DERECHO

    duracion_obj = objetivo->duracion_2;

    potencia_cand = candidato->potencia_inicial;

    if ( (frec_dcha_obj = objetivo->frecuencia_derecha) == 0)
      frec_dcha_obj = 100;


    // Anterior fonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial);

    // Semifonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad);

    // Siguiente fonema:

    coste_inteligibilidad += *pesos++ *
      calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final);

    *diferencia_f0_tran = frec_dcha_obj - candidato->frecuencia_inicial;

    diferencia_frec_coart = absoluto(frec_dcha_obj - candidato->frecuencia_final);

    // Frecuencia en la zona de coarticulación:

    coste_inteligibilidad += *pesos++*(diferencia_frec_coart/frec_dcha_obj);

    *diferencia_f0_est = frec_cent_obj - candidato->frecuencia_inicial;

    diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_inicial);

    // Frecuencia en la zona estacionaria.

    coste_inteligibilidad += *pesos++*(diferencia_frec_est/frec_cent_obj);


  } // else (DERECHO)

    // Coste de inicio y final de palabra:

  // Esto ya no va a funcionar...
  if (objetivo->frontera_inicial != candidato->frontera_inicial)
    coste_inteligibilidad += *pesos++;
  else
    coste_inteligibilidad -= *pesos++;

  if (objetivo->frontera_final != candidato->frontera_final)
    coste_inteligibilidad += *pesos++;
  else
    coste_inteligibilidad -= *pesos++;

  if (candidato->frase != objetivo->frase)
    coste_inteligibilidad += *pesos++;
  else
    coste_inteligibilidad -= *pesos++;

  if (candidato->posicion != objetivo->posicion)
    coste_inteligibilidad += *pesos++;
  else
    coste_inteligibilidad -= *pesos++;

  if (candidato->acento != objetivo->acento)
    coste_inteligibilidad += *pesos++;
  else
    coste_inteligibilidad -= *pesos++;


  *diferencia_duracion = duracion_obj - duracion_cand;

  diferencia_dur = absoluto(duracion_obj - duracion_cand);

  coste_inteligibilidad += *pesos++*(diferencia_dur/duracion_obj);

  if (izq_der == IZQUIERDO) {
    coste_inteligibilidad += *pesos++*locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].varianza;
    coste_inteligibilidad -= *pesos;
  } // if (izq_der == IZQUIERDO)
  else { // DERECHO
    coste_inteligibilidad += *pesos++*locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].varianza;
    coste_inteligibilidad += *pesos;
  } // else // DERECHO

    // Ahora consideramos el coste debido a la prosodia:

  if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia = MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_frec_coart > FREC_UMBRAL)
      coste_prosodia = (diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            izq_der == IZQUIERDO ? coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_izda_obj:
  //                                   coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_dcha_obj;

  if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_frec_est > FREC_UMBRAL)
      coste_prosodia += (diferencia_frec_est - FREC_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_cent_obj;

  *coste_f0 = coste_prosodia;

  if (objetivo->semifonema[0] != '#')
    if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
      coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
    else
      if (diferencia_dur > DUR_UMBRAL)
	coste_prosodia += (diferencia_dur - DUR_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL);

  *coste_duracion = coste_prosodia - *coste_f0;

  *diferencia_energia = potencia_obj - potencia_cand;

  diferencia_potencia = absoluto(potencia_cand - potencia_obj);

 if ( (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION) ||
      (maximo(potencia_cand, potencia_obj) / minimo(potencia_cand, potencia_obj) > 2))
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_potencia > ENERGIA_UMBRAL)
      coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);

  *coste_energia = coste_prosodia - *coste_f0 - *coste_duracion;
  *coste_prosodico = coste_prosodia;
  *coste_contextual = coste_inteligibilidad;

}

#endif


#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

/**
 * Función:   calcula_costes_objetivo_red_neuronal
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.
 *          - objetivo: identificador de la unidad objetivo.
 *          - izq_der: IZQUIERDO o DERECHO.                                              
 *			- locutor: puntero al locutor que se está empleando.						 
 *	\remarks Salida:	- coste_prosodico: parte de la función de coste de objetivo referida a la    
 *			  prosodia.																	 
 *			- coste_contextual: parte de la función de coste de objetivo referida al	 
 *			  contexto.																	 
 *			- coste_f0: coste de frecuencia fundamental.								 
 *			- coste_duracion: coste de duración.										 
 *			- coste_energia: coste de energia.											 
 *			- diferencia_f0_est: diferencia de f0 en la zona estacionaria.				 
 *			- diferencia_f0_tran: diferencia de f0 en la zona de transición.			 
 *			- diferencia_duracion: diferencia de duración.								 
 *			- diferencia_energia: diferencia de energía.								 	
 * \remarks Objetivo:  Calcula los costes contextual y prosódico.								 
 */

void calcula_costes_objetivo_red_neuronal(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der, Locutor *locutor, float *coste_prosodico, float *coste_contextual, float *coste_f0, float *coste_duracion, float *coste_energia, float *diferencia_f0_est, float *diferencia_f0_tran, float *diferencia_duracion, float *diferencia_energia) {

  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  float coste_prosodia = 0.0;
  double coste_inteligibilidad;
  float diferencia_frec_est, diferencia_frec_coart, diferencia_dur, diferencia_potencia;

  float duracion_obj;
  float frec_izda_obj;
  float frec_cent_obj;
  float frec_dcha_obj;
  float potencia_obj;

  float duracion_cand, potencia_cand;

  double datos_red[NUMERO_ENTRADAS_RED_ESPECTRAL], *apunta_datos_red;
  Red_neuronal *apunta_red;
  Media_varianza *apunta_media_var;

  apunta_datos_red = (double *) datos_red;

  apunta_red = &locutor->red_contextual[candidato->tipo_espectral];
  apunta_media_var = locutor->estadisticos_contextual[candidato->tipo_espectral];


  indice_central_obj = devuelve_indice_fonema(candidato->semifonema);
  indice_fut1_obj = devuelve_indice_fonema(objetivo.contexto_derecho);
  indice_pas1_obj = devuelve_indice_fonema(objetivo.contexto_izquierdo);


  duracion_cand = candidato->duracion;
  potencia_obj = objetivo.potencia;

  if ( (frec_cent_obj = objetivo.frecuencia_mitad) == 0)
    frec_cent_obj = 100;

  if (izq_der == IZQUIERDO) {

    duracion_obj = objetivo.duracion_1;

    potencia_cand = candidato->potencia_final;

    if ( (frec_izda_obj = objetivo.frecuencia_izquierda) == 0)
      frec_izda_obj = 100;

    *diferencia_f0_tran = frec_izda_obj - candidato->frecuencia_inicial;
        
    diferencia_frec_coart = absoluto((frec_izda_obj - candidato->frecuencia_inicial));

    // Frecuencia en la zona de coarticulación:

    *apunta_datos_red++ = (frec_izda_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;


    *diferencia_f0_est = frec_cent_obj - candidato->frecuencia_final;
        
    diferencia_frec_est = absoluto((frec_cent_obj - candidato->frecuencia_final));

    // Frecuencia en la zona estacionaria:

    *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Inicio de palabra:

    *apunta_datos_red++ = (objetivo.frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Fin de palabra:

    *apunta_datos_red++ = (objetivo.frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Frase:

    *apunta_datos_red++ = (objetivo.frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Posición

    *apunta_datos_red++ = (objetivo.posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Acento:

    *apunta_datos_red++ = (objetivo.acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Duración:

    *diferencia_duracion = duracion_obj - duracion_cand;

    diferencia_dur = absoluto(duracion_obj - duracion_cand);

    *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Izquierdo:

    *apunta_datos_red++ = (0.0 - apunta_media_var->media) / apunta_media_var->desviacion;

    apunta_media_var++;

    // Anterior fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

   // Semifonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Siguiente fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Varianza:

    *apunta_datos_red = (locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].varianza - apunta_media_var->media) / apunta_media_var->desviacion;

  } // if (izq_der == IZQUIERDO)
  else { // DERECHO

    duracion_obj = objetivo.duracion_2;
    potencia_cand = candidato->potencia_inicial;

    if ( (frec_dcha_obj = objetivo.frecuencia_derecha) == 0)
      frec_dcha_obj = 100;

    *diferencia_f0_tran = frec_dcha_obj - candidato->frecuencia_final;
        
    diferencia_frec_coart = absoluto(frec_dcha_obj - candidato->frecuencia_final);

    // Frecuencia en la zona de coarticulación:

    *apunta_datos_red++ = (frec_dcha_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *diferencia_f0_est = frec_cent_obj - candidato->frecuencia_inicial;
        
    diferencia_frec_est = absoluto(frec_cent_obj - candidato->frecuencia_inicial);

    // Frecuencia en la zona estacionaria:

    *apunta_datos_red++ = (frec_cent_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frecuencia_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Inicio de palabra:

    *apunta_datos_red++ = (objetivo.frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_inicial - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Fin de palabra:

    *apunta_datos_red++ = (objetivo.frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frontera_final - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Frase:

    *apunta_datos_red++ = (objetivo.frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->frase - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Posición

    *apunta_datos_red++ = (objetivo.posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->posicion - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Acento:

    *apunta_datos_red++ = (objetivo.acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (candidato->acento - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Duración:

    *diferencia_duracion = duracion_obj - duracion_cand;
       
    diferencia_dur = absoluto(duracion_obj - duracion_cand);

    *apunta_datos_red++ = (duracion_obj - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    *apunta_datos_red++ = (duracion_cand - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Derecho:

    *apunta_datos_red++ = (1.0 - apunta_media_var->media) / apunta_media_var->desviacion;

    apunta_media_var++;

    // Anterior fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector, candidato->cepstrum_inicial, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Semifonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector, candidato->cepstrum_mitad, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Siguiente fonema:

    *apunta_datos_red++ =
      (calcula_distancia_euclidea(locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector, candidato->cepstrum_final, Vector_semifonema_candidato::numero_coeficientes_cepstrales) - apunta_media_var->media) /
      apunta_media_var->desviacion;

    apunta_media_var++;

    // Varianza:

    *apunta_datos_red = (locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].varianza
			 - apunta_media_var->media) / apunta_media_var->desviacion;

  } // else (DERECHO)

  apunta_red->simula_red(datos_red, &coste_inteligibilidad);

  // Ahora consideramos el coste debido a la prosodia:

  if (diferencia_frec_coart > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia = MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_frec_coart > FREC_UMBRAL)
      coste_prosodia = (diferencia_frec_coart - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            izq_der == IZQUIERDO ? coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_izda_obj:
  //                                   coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_dcha_obj;

  if (diferencia_frec_est > UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION) // Introducimos la penalización más alta.
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_frec_est > FREC_UMBRAL)
      coste_prosodia += (diferencia_frec_est - FREC_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION - FREC_UMBRAL);
  //            coste += PENDIENTE_FRECUENCIA*diferencia_frec_coart/frec_cent_obj;

  *coste_f0 = coste_prosodia;

  if (diferencia_dur > UMBRAL_DURACION_MAXIMA_PENALIZACION) // Penalización más alta
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_dur > DUR_UMBRAL)
      coste_prosodia += (diferencia_dur - DUR_UMBRAL)*MAXIMO_COSTE_SUBFUNCION/(UMBRAL_DURACION_MAXIMA_PENALIZACION - DUR_UMBRAL);

  *coste_duracion = coste_prosodia - *coste_f0;
    
  *diferencia_energia = potencia_obj - potencia_cand;
  diferencia_potencia = absoluto(potencia_cand - potencia_obj);

  if ( (diferencia_potencia > UMBRAL_ENERGIA_MAXIMA_PENALIZACION) ||
       (maximo(potencia_cand, potencia_obj) / minimo(potencia_cand, potencia_obj) > 2))
    coste_prosodia += MAXIMO_COSTE_SUBFUNCION;
  else
    if (diferencia_potencia > ENERGIA_UMBRAL)
      coste_prosodia += (diferencia_potencia - ENERGIA_UMBRAL)*MAXIMO_VALOR_ZONA_LINEAL/(UMBRAL_ENERGIA_MAXIMA_PENALIZACION - ENERGIA_UMBRAL);

  *coste_energia = coste_prosodia - *coste_f0 - *coste_duracion;
  *coste_prosodico = coste_prosodia;
  *coste_contextual = coste_inteligibilidad;

}

#endif

//#ifdef _MODO_NORMAL

/**
 * Función:   crea_cadena_grupos_acentuales_originales                         
 * \remarks Entrada:
 *			- numero_fichero: número de la frase del Corpus.                   
 *          - acentuales_obj: cadena de grupos acentuales objetivo.            
 *          - numero_acentuales: número de grupos acentuales objetivo.         
 *	\remarks Salida:
 *			- acentuales: cadena de grupos acentuales candidato de la frase    
 *            original del Corpus.                                             
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de errores, se devuelve un 0. Un 1 en caso contrario.
 */

int crea_cadena_grupos_acentuales_originales(char * locutores, int numero_fichero, Grupo_acentual_objetivo *acentuales_obj,
                                             Grupo_acentual_candidato **acentuales_cand,
                                             int numero_acentuales) {

  FILE *fichero;
  char nombre_actual[FILENAME_MAX], nombre_fichero[FILENAME_MAX];
  int indice, tamano;
  int descriptor_original;
  estructura_grupos_acentuales *unidad_seleccionada ;
  Grupo_acentual_candidato *apunta_candidato, *recorre_candidatos;
  Grupo_acentual_objetivo *apunta_objetivo = acentuales_obj;
  unsigned char tipo_proposicion;
  int posicion;
  int posicion_silaba_acentuada;
  int cuenta, contador;

  sprintf(nombre_fichero, "%s%s.txt", FICHERO_ORIGEN_GRUPOS_ACENTUALES, locutores);

  if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
    fprintf(stderr, "Error en encuentra_indice_primer_grupo_acentual, al intentar abrir el fichero %s.\n",
	    nombre_fichero);
    return 1;
  } // if ( (fichero = ...

  sprintf(nombre_fichero, "_%d.", numero_fichero);

  while (!feof(fichero)) {
    fscanf(fichero, "%d %s\n", &descriptor_original, nombre_actual);

    if (strstr(nombre_actual, nombre_fichero))
      break;

  } // while (!feof(fichero...

  if (feof(fichero)) {
    puts("Error en crea_cadena_grupos_acentuales_originales: Número de frase no válido.");
    return 1;
  } // if (feof(fichero)

  fclose(fichero);

  if ( (*acentuales_cand = (Grupo_acentual_candidato *) malloc(numero_acentuales*sizeof(Grupo_acentual_candidato)))
       == NULL) {
    fprintf(stderr, "Error en crea_cadena_grupos_acentuales_originales, al asignar memoria.");
    return 1;
  } // if ( (*acentuales_cand = ...

  apunta_candidato = *acentuales_cand;

  for (contador = 0; contador < numero_acentuales; contador++, apunta_candidato++,
	 apunta_objetivo++, descriptor_original++) {

    tipo_proposicion = apunta_objetivo->tipo_proposicion;
    posicion_silaba_acentuada = apunta_objetivo->posicion_silaba_acentuada;
    posicion = apunta_objetivo->posicion_grupo_acentual;

    if (posicion != 0) {
      if (posicion != apunta_objetivo->grupos_acentuales_grupo_prosodico - 1)
	posicion = 1;
      else
	posicion = 2;
    }


    indice = calcula_indice_grupo(tipo_proposicion, posicion,
				  posicion_silaba_acentuada);

    if ( (unidad_seleccionada = devuelve_puntero_a_tabla(indice)) == NULL) {
      puts("Error en crea_cadena_grupos_acentuales_originales: unidad no existente.");
      free(*acentuales_cand);
      return 1;
    } // if ( (unidad_seleccionada...


    tamano = unidad_seleccionada->numero_grupos;

    recorre_candidatos = unidad_seleccionada->grupos;

    for (cuenta = 0; cuenta < tamano; cuenta++, recorre_candidatos++)
      if (recorre_candidatos->identificador == descriptor_original)
	break;

    if (cuenta == tamano) {
      puts("Error en crea_cadena_grupos_acentuales_originales: Descriptor no existente.");
      free(*acentuales_cand);
      return 1;
    } // if (cuenta == tamano)

    memcpy(apunta_candidato, recorre_candidatos, sizeof(Grupo_acentual_candidato));

  } // for (contador = 0; contador...

  return 0;

}

//#endif

/**
 * Función:   escribe_etiquetas_inicio_fonemas                                 
 * \remarks Entrada:
 *			- vector_sintesis: cadena de vectores de síntesis.                 
 *          - vector_candidato: cadena de vectores objetivo.                   
 *          - numero_unidades: número de vectores de síntesis.                 
 *          - nombre_fichero: nombre del fichero en el que se va a almacenar   
 *            la información de inicio de cada fonema de la voz sintética.     
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de errores, se devuelve un 0. Un 1 en caso contrario.
 */

int escribe_etiquetas_inicio_fonemas(Vector_sintesis *vector_sintesis,
                                     Vector_descriptor_objetivo *vector_objetivo,
                                     int numero_unidades,
                                     char nombre_fichero[]) {
  int contador;
  FILE *fichero;
  float duracion = 0.0, duracion_anterior = 0.0;
  Vector_descriptor_objetivo *apunta_objetivo = vector_objetivo;
  Vector_sintesis *apunta_sintesis = vector_sintesis;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_etiquetas_inicio_fonemas, al intentar abrir el fichero %s.\n",
	    nombre_fichero);
    return 1;
  } // if ( (fichero = fopen...

  for (contador = 0; contador < numero_unidades; contador++, apunta_sintesis++) {

    duracion += apunta_sintesis->devuelve_duracion();

    if (contador & 0x1) {
      fprintf(fichero, "%f\t%s\n", duracion_anterior, apunta_objetivo->semifonema);
      duracion_anterior = duracion;
      apunta_objetivo++;
    } // if (contador & 0x1)

  } // for (contador = 0...

  fclose(fichero);

  return 0;

}


/**
 * Función:   escribe_etiquetas_inicio_semifonemas                             
 * \remarks Entrada:
 *			- vector_sintesis: cadena de vectores de síntesis.                 
 *          - vector_candidato: cadena de vectores objetivo.                   
 *          - numero_unidades: número de vectores de síntesis.                 
 *          - nombre_fichero: nombre fdel fichero en el que se va a almacenar  
 *            la información de inicio de cada fonema de la voz sintética.     
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de errores, se devuelve un 0. Un 1 en caso contrario.
 */

int escribe_etiquetas_inicio_semifonemas(Vector_sintesis *vector_sintesis,
					 Vector_descriptor_objetivo *vector_objetivo,
					 int numero_unidades,
					 char nombre_fichero[]) {
  int contador;
  FILE *fichero;
  float duracion = 0.0, duracion_anterior = 0.0;
  Vector_descriptor_objetivo *apunta_objetivo = vector_objetivo;
  Vector_sintesis *apunta_sintesis = vector_sintesis;

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_etiquetas_inicio_fonemas, al intentar abrir el fichero %s.\n",
	    nombre_fichero);
    return 1;
  } // if ( (fichero = fopen...

  for (contador = 0; contador < numero_unidades; contador++, apunta_sintesis++) {

    duracion += apunta_sintesis->devuelve_duracion();

    if (contador & 0x1) {
      fprintf(fichero, "%f\t.\n", duracion_anterior);
      apunta_objetivo++;
    } // if (contador & 0x1)
    else
      fprintf(fichero, "%f\t%s\n", duracion_anterior, apunta_objetivo->semifonema);

    duracion_anterior = duracion;
        
  } // for (contador = 0...

  fclose(fichero);

  return 0;

}


/**
 * Función:   escribe_concatenacion_grupos_acentuales                            
 * \remarks Entrada:
 *			- acentuales: cadena con los grupos acentuales escogidos para la     
 *            curva de frecuencia fundamental.                                   
 *          - numero_grupos: número de elementos de la cadena anterior.          
 *          - fichero: puntero al fichero, ya abierto, en el que se va a 		 
 *			  escribir la información.											 
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Escribir en un fichero los valores de frecuencia fundamental en    
 *            los puntos de concatenación entre grupos, sin considerar aquellos  
 *            casos en los que están separados por pausas.                       
 */

int escribe_concatenacion_grupos_acentuales(Grupo_acentual_candidato *acentuales, int numero_grupos,
                                            FILE *fichero) {

  int indice1;
  Grupo_acentual_candidato *apunta_candidato = acentuales;

  if (numero_grupos == 1)
    return 0;

  for (int contador = 1; contador < numero_grupos; contador++, apunta_candidato++)
    if (apunta_candidato->tipo_pausa == SIN_PAUSA) {
      indice1 = apunta_candidato->frecuencia.numero_marcas;
      fprintf(fichero, "%f\n", apunta_candidato->frecuencia.marcas[indice1 - 1].frecuencia - 
	      (apunta_candidato + 1)->frecuencia.marcas[0].frecuencia);
    } // if (apunta_candidato->tipo_pausa...


  return 0;

}

#ifdef _ESCRIBE_FRECUENCIA_GRUPOS_ACENTUALES

/**
 * Función:   apunta_grupos_acentuales_escogidos                               
 * \remarks Entrada:
 *			- numero_acentuales: número de unidades de la cadena.              
 *          - cadena_acentuales: cadena de grupos acentuales candidatos.       
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

int apunta_grupos_acentuales_escogidos(int numero_acentuales, char * locutores,
                                       Grupo_acentual_candidato *cadena_acentuales) {

  int contador;

  if (ocurrencias == NULL) {
    int *apunta_ocurrencias;
    if (strstr(locutores, "Freire")) {
      if ( (ocurrencias = (int *) malloc((INDICE_MAXIMO_FREIRE + 1)*sizeof(int)))
	   == NULL) {
	puts("Error en apunta_grupos_acentuales_escogidos, al asignar memoria.");
	return 1;
      } // if ( (ocurrencias = ...

      apunta_ocurrencias = ocurrencias;
      for (contador = 0; contador < INDICE_MAXIMO_FREIRE; contador++)
	*apunta_ocurrencias++ = 0;
    } // if (strstr(locutores, "Freire")
    else
      if (strstr(locutores, "Paulino")) {
	if ( (ocurrencias = (int *) malloc((INDICE_MAXIMO_PAULINO + 1)*sizeof(int)))
	     == NULL) {
	  puts("Error en apunta_grupos_acentuales_escogidos, al asignar memoria.");
	  return 1;
	} // if ( (ocurrencias = ...
	apunta_ocurrencias = ocurrencias;
	for (contador = 0; contador < INDICE_MAXIMO_PAULINO; contador++)
	  *apunta_ocurrencias++ = 0;
      } // if (strstr(locutores, "Paulino"))
      else {
	fprintf(stderr, "Error en apunta_grupos_acentuales_escogidos: locutor (%s) no \
                contemplado.\n", locutores);
	return 1;
      } // else
  } // if (ocurrencias == NULL)

  for (contador = 0; contador < numero_acentuales; contador++, cadena_acentuales++)
    ocurrencias[cadena_acentuales->identificador]++;

  return 0;

}


/**
 * Función:   escribe_grupos_acentuales_escogidos                              
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero de entrada.                   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 */

//#ifdef _MODO_NORMAL

int escribe_grupos_acentuales_escogidos(char *nombre_fichero) {

  char *apunta_nombre_fichero, nuevo_fichero[FILENAME_MAX];
  char tipo_frase[][4] = {"_EN", "_EX", "_IN", "_SU"};
  char pos_frase[][20] = {"_INICIAL.ocu", "_MEDIA.ocu", "_FINAL.ocu"};
  estructura_grupos_acentuales *unidad_seleccionada;
  Grupo_acentual_candidato *recorre_unidades;
  int indice, contador;

  FILE *fichero;

  if (ocurrencias == NULL) {
    puts("Llamada incorrecta en escribe_grupos_acentuales escogidos. Ocurrencias = NULL. \
              Debe llamar antes a apunta_grupos_acentuales_escogidos.");
    return 1;
  } // if (ocurrencias == NULL)
  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES;
       tipo_proposicion++) {

    strcpy(nuevo_fichero, nombre_fichero);
    apunta_nombre_fichero = strstr(nuevo_fichero, ".");

    if (apunta_nombre_fichero == NULL)
      strcat(nuevo_fichero, tipo_frase[tipo_proposicion]);
    else
      strcpy(apunta_nombre_fichero, tipo_frase[tipo_proposicion]);

    apunta_nombre_fichero = nuevo_fichero + strlen(nuevo_fichero);

    for (int posicion = 0; posicion < NUMERO_POSICIONES_FRASE; posicion++) {

      strcpy(apunta_nombre_fichero, pos_frase[posicion]);

      if ( (fichero = fopen(nuevo_fichero, "wt")) == NULL) {
	fprintf(stderr, "Error en escribe_grupos_acentuales_escogidos, al intentar abrir el fichero %s.\n",
		nuevo_fichero);
	return 1;
      } // if ( (fichero = fopen...

      for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO;
	   silaba_acentuada++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion,
				      silaba_acentuada);

	if ( (unidad_seleccionada = devuelve_puntero_a_tabla(indice)) == NULL)
	  continue;


	recorre_unidades = unidad_seleccionada->grupos;

	for (contador = 0; contador < unidad_seleccionada->numero_grupos; contador++,
	       recorre_unidades++)
	  fprintf(fichero, "%d\t%d\n", recorre_unidades->identificador,
		  ocurrencias[recorre_unidades->identificador]);
      } // for (int silaba_acentuada = 0...

      fclose(fichero);

    } // for (int posicion = 0...

  } // for (int tipo_proposicion = 0...

  free(ocurrencias);

  ocurrencias = NULL;

  return 0;

}

//#endif

#endif

#ifdef _UTILIZA_CENTROIDES

/**
 * Función:   calcula_centroides_espectrales_por_clases							
 * \remarks Entrada:	- locutor: puntero al objeto Locutor en el que se encuentra la		
 *			  información sobre los centroides espectrales de las unidades de   
 *			  voz.																
 *	\remarks Salida:	- centroides: centroides espectrales calculados según las clases    
 *			  del modelo espectral.												
 * \remarks NOTA:	  El índice del vector de centroides se calcula según la fórmula    
 *			  2*indice_semifonema_central + 1 (si es por la parte derecha).		
 */

void calcula_centroides_espectrales_por_clases(Locutor *locutor, Vector_cepstrum centroides[]) {

  int indice1, indice2, coeficiente;
  int numero_elementos[2*NUMERO_ELEMENTOS_CLASE_ESPECTRAL], *elementos;
  char izquierdo_derecho, existe_unidad;
  float *apunta_coeficiente, *apunta_vector, *apunta_centroide;
  clase_espectral indice_clase;

  for (indice1 = 0; indice1 < 2*NUMERO_ELEMENTOS_CLASE_ESPECTRAL; indice1++) {
    numero_elementos[indice1] = 0;
    for (apunta_coeficiente = (float *) centroides[indice1], indice2 = 0;
	 indice2 < Vector_semifonema_candidato::numero_coeficientes_cepstrales; indice2++, apunta_coeficiente++)
      *apunta_coeficiente = 0.0;
  } // for (indice1 = 0; ...


  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++) {
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      for (izquierdo_derecho = 0; izquierdo_derecho < 2; izquierdo_derecho++) {

	existe_unidad = 0;

	apunta_vector = locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice1, indice2, izquierdo_derecho)].vector;

	for (coeficiente = 0; coeficiente < Vector_semifonema_candidato::numero_coeficientes_cepstrales; coeficiente++)
	  if (*apunta_vector++ != 0) {
	    existe_unidad = 1;
	    break;
	  } // if (*apunta_vector++ != 0)

	if (existe_unidad == 0)
	  continue;

	apunta_vector = locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice1, indice2, izquierdo_derecho)].vector;

	if (izquierdo_derecho == 0) { // El semifonema es por la izquierda.
	  tipo_fon_espectral_enumerado(devuelve_puntero_a_nombre_fonema(indice2), &indice_clase);
	  apunta_centroide = centroides[2*indice_clase];
	  numero_elementos[2*indice_clase]++;
	} // if (izquierdo_derecho == 0)
	else { // semifonema por la derecha.
	  tipo_fon_espectral_enumerado(devuelve_puntero_a_nombre_fonema(indice1), &indice_clase);
	  apunta_centroide = centroides[2*indice_clase + 1];
	  numero_elementos[2*indice_clase + 1]++;
	} // else



	for (coeficiente = 0; coeficiente < Vector_semifonema_candidato::numero_coeficientes_cepstrales; coeficiente++)
	  *apunta_centroide++ += *apunta_vector++;

      } // for (izquierdo_derecho
    } // for (indice2 = 0;
  } // for (indice1 = 0;

  elementos = (int *) numero_elementos;

  for (indice1 = 0; indice1 < 2*NUMERO_ELEMENTOS_CLASE_ESPECTRAL; indice1++, elementos++) {

    if (*elementos) {
      apunta_vector = (float *) (centroides + indice1);
      for (indice2 = 0; indice2 < Vector_semifonema_candidato::numero_coeficientes_cepstrales; indice2++)
	*apunta_vector++ /= *elementos;
    } // if (*elementos)

  } // for (indice1 = 0;



}


/**
 * Función:   escribe_fichero_centroides_clase									
 * \remarks Entrada:	- centroides: centroides espectrales calculados según las clases    
 *			  del modelo espectral.												
 *			- fichero: puntero al fichero en el que se escriben los centroides. 
 * \remarks NOTA:	  El índice del vector de centroides se calcula según la fórmula    
 *			  2*indice_semifonema_central + 1 (si es por la parte derecha).		
 */

void escribe_fichero_centroides_clase(Vector_cepstrum centroides[], FILE *fichero) {

  char izquierdo_derecho;
  float *apunta_coeficiente;


  for (int contador = 0; contador < 2*NUMERO_ELEMENTOS_CLASE_ESPECTRAL;
       contador++) {

    izquierdo_derecho = (char) (contador & 1);
    apunta_coeficiente = (float *) (centroides + contador);

    if (izquierdo_derecho == 0)
      fprintf(fichero, "%s\tIzdo\t",
	      devuelve_cadena_alofono_espectral((clase_espectral) (contador / 2)));
    else
      fprintf(fichero, "%s\tDcho\t",
	      devuelve_cadena_alofono_espectral((clase_espectral) (contador / 2)));

    for (int cuenta = 0; cuenta < Vector_semifonema_candidato::numero_coeficientes_cepstrales;
	 cuenta++, apunta_coeficiente++)
      fprintf(fichero, "%f ", *apunta_coeficiente);

    fprintf(fichero, "\n");

  } // for (int contador = 0; ...

}


/**
 * Función:   organiza_base_de_datos_segun_centroides_clases					 
 * \remarks Entrada:	- inicio_nombre_fichero: parte inicial del nombre de los ficheros de 
 *			  salida.															 
 *			- nombre_fichero_errores: nombre del fichero en el que se escriben   
 *			  los semifonemas que se encuentran más próximos al centroide de una 
 *			  clase que no es la suya.											 
 * 			- locutor: puntero al objeto locutor empleado.						 
 * \remarks Objetivo:  organiza las unidades del corpus en clusteres, según el centroide  
 * 			  más próximo a su vector cepstrum medio.						     
 * \remarks NOTA:	  El índice del vector de centroides se calcula según la fórmula     
 *			  2*indice_semifonema_central + 1 (si es por la parte derecha).		 
 */

int organiza_base_de_datos_segun_centroides_clases(char *inicio_nombre_fichero,
						   char *nombre_fichero_errores, Locutor *locutor) {

  int indice1, indice2, indice_minimo;
  float distancia_minima, distancia_auxiliar;
  char izquierdo_derecho;
  char nombre_fichero[FILENAME_MAX];
  const char *nombre_clase;
  Vector_cepstrum centroides[2*NUMERO_ELEMENTOS_CLASE_ESPECTRAL], *centroide_clase;
  estructura_unidades *grupo_unidades;

  Vector_semifonema_candidato *correcaminos;

  FILE *fichero_clases[NUMERO_ELEMENTOS_CLASE_ESPECTRAL], *fichero_errores,
    *fichero_centroides;

  calcula_centroides_espectrales_por_clases(locutor, centroides);

  if ( (fichero_centroides = fopen("fichero_centroides_clases.txt", "wt")) == NULL) {
    fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides, al intentar \
crear el fichero de centroides por clases.\n");
    return 1;
  } // if ( (fichero_centroides = ...

  escribe_fichero_centroides_clase(centroides, fichero_centroides);

  fclose(fichero_centroides);

  if ( (fichero_errores = fopen(nombre_fichero_errores, "wt")) == NULL) {
    fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides, al intentar \
crear el fichero %s.\n", nombre_fichero_errores);
    return 1;
  } // if ( (fichero_errores = ...

  for (indice1 = 0; indice1 < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; indice1++) {

    if ( (nombre_clase = devuelve_cadena_alofono_espectral((clase_espectral) indice1))
	 == NULL)
      return 1;

    sprintf(nombre_fichero, "%s_%s.txt", inicio_nombre_fichero, nombre_clase);

    if ( (fichero_clases[indice1] = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides, al intentar crear \
el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_clases...

  } // for (indice1 = 0; ...

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++) {
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == IZQUIERDO)
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	for (int cuenta = 0; cuenta < grupo_unidades->numero_unidades;
	     cuenta++, correcaminos++) {

	  distancia_minima = FLT_MAX;

	  for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL;
	       contador++) {

	    centroide_clase = (Vector_cepstrum *) centroides[2*contador + izquierdo_derecho - '0'];

	    distancia_auxiliar = calcula_distancia_euclidea_general((float *) correcaminos->cepstrum_mitad, (float *) centroide_clase, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

	    if (distancia_auxiliar < distancia_minima) {
	      indice_minimo = contador;
	      distancia_minima = distancia_auxiliar;
	    } // if (distancia_auxiliar < ...

	  } // for (int contador = 0; ...


	  if (indice_minimo != (int) correcaminos->tipo_espectral) {
	    if (izquierdo_derecho == IZQUIERDO)
	      fprintf(fichero_errores, "%s.%s\tIzdo\t%d\t%s\t%s\n",
		      correcaminos->contexto_izquierdo, correcaminos->semifonema,
		      correcaminos->identificador, tipo_fon_espectral_cadena(correcaminos->semifonema),
		      devuelve_cadena_alofono_espectral((clase_espectral) indice_minimo));
	    else
	      fprintf(fichero_errores, "%s.%s\tDcho\t%d\t%s\t%s\n",
		      correcaminos->semifonema, correcaminos->contexto_derecho,
		      correcaminos->identificador, tipo_fon_espectral_cadena(correcaminos->semifonema),
		      devuelve_cadena_alofono_espectral((clase_espectral) indice_minimo));
	  } // if (indice_minimo != (int) correcaminos->tipo_espectral)

	  if (izquierdo_derecho == IZQUIERDO)
	    fprintf(fichero_clases[indice_minimo], "%s.%s\tIzdo\t%d\n",
		    correcaminos->contexto_izquierdo, correcaminos->semifonema,
		    correcaminos->identificador);
	  else
	    fprintf(fichero_clases[indice_minimo], "%s.%s\tIzdo\t%d\n",
		    correcaminos->semifonema, correcaminos->contexto_derecho,
		    correcaminos->identificador);

	} // for (int cuenta = 0; cuenta < grupo_unidades

      } // for (izquierdo_derecho = '0'...
    } // for (indice2 = 0;
  } // for (indice1 = 0;

  for (indice1 = 0; indice1 < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; indice1++)
    fclose(fichero_clases[indice1]);

  fclose(fichero_errores);

  return 0;

}

/**
 * Función:   organiza_base_de_datos_segun_centroides_semifonemas  				 
 * \remarks Entrada:
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *			- inicio_nombre_fichero: parte inicial del nombre de los ficheros de
 *			  salida.															 
 *			- nombre_fichero_errores: nombre del fichero en el que se escriben   
 *			  los semifonemas que se encuentran más próximos al centroide de una 
 *			  clase que no es la suya.											 
 *			- nombre_fichero_origen_unidades: nombre del fichero en el que se 	 
 *			  indica el fichero en el que se encuentra cada unidad de la base.	 
 *			- nombre_fichero_unidades_anuladas: nombre del fichero en el que se  
 *			  almacenan las unidades no válidas.								 
 *			- inicio_nombre_ficheros_salida: ruta e inicio del nombre de los 	 
 *			  ficheros que se desean anular.									 
 *			- umbral_eliminacion: diferencia entre la distancia al centroide	 
 *			  propio y al más cercano, a partir de la cual se considera que la   
 *			  unidad no es válida.												 	 	
 * 			- locutor: puntero al objeto locutor empleado.						 
 * \remarks Objetivo:  organiza las unidades del corpus en clusteres, según el centroide  
 * 			  más próximo a su vector cepstrum medio.						     
 * \remarks NOTA:	  El índice del vector de centroides se calcula según la fórmula     
 *			  2*indice_semifonema_central + 1 (si es por la parte derecha).		 
 */

int organiza_base_de_datos_segun_centroides_semifonemas(vector<string> &corpus, char *inicio_nombre_fichero,
							char *nombre_fichero_errores,
                                                        char *nombre_fichero_origen_unidades,
                                                        char *nombre_fichero_unidades_anuladas,
                                                        char *inicio_nombre_ficheros_salida,
                                                        float umbral_eliminacion,
                                                        Locutor *locutor) {

  map<int, int> origen_unidades;
  int numero_unidades;
  int indice1, indice2, indice3, indice4;
  int indice_minimo1, indice_minimo2;
  int numero_ficheros_abiertos = 0;
  int instancias_eliminadas;
  int elimina;
  float distancia_minima, distancia_auxiliar, distancia_correspondiente;
  char izquierdo_derecho;
  char nombre_fichero[FILENAME_MAX];
  Vector_cepstrum *centroide_semifonema;
  estructura_unidades *grupo_unidades, *grupo_unidades2;
  char cadena_identificador[10];

  Vector_semifonema_candidato *correcaminos;

  FILE *fichero_clases[NUMERO_FONEMAS][NUMERO_FONEMAS][2], *fichero_errores = NULL,
    *apunta_fichero, *fichero_unidades_anuladas = NULL;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      fichero_clases[indice1][indice2][0] = NULL;
      fichero_clases[indice1][indice2][1] = NULL;
    } // for (indice2 = 0;

  if ( (fichero_errores = fopen(nombre_fichero_errores, "wt")) == NULL) {
    fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides, al intentar \
crear el fichero %s.\n", nombre_fichero_errores);
    return 1;

  } // if ( (fichero_errores = ...

  if (nombre_fichero_unidades_anuladas == NULL) {

#ifndef _WIN32
    sprintf(nombre_fichero, "rm %s* -rf", inicio_nombre_fichero);
#else
    sprintf(nombre_fichero, "del %s*", inicio_nombre_fichero);
#endif

    system(nombre_fichero);


  } // if (nombre_fichero_unidades_anuladas == NULL)
  else {

    if ( (fichero_unidades_anuladas = fopen(nombre_fichero_unidades_anuladas, "wt"))
	 == NULL) {
      fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides_semifonemas, al \
intentar crear el fichero %s.\n", nombre_fichero_unidades_anuladas);
      return 1;
    } // if ( (fichero_unidades_anuladas = ...

    if (convierte_fichero_unidades_bin(nombre_fichero_origen_unidades, &origen_unidades,
				       &numero_unidades))
      return 1;
  } // else

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == IZQUIERDO)
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	instancias_eliminadas = 0;
                
	for (int cuenta_unidades = 0; cuenta_unidades < grupo_unidades->numero_unidades;
	     cuenta_unidades++, correcaminos++) {

	  distancia_minima = FLT_MAX;

	  for (indice3 = 0; indice3 < NUMERO_FONEMAS; indice3++)
	    for (indice4 = 0; indice4 < NUMERO_FONEMAS; indice4++) {

	      if (izquierdo_derecho == IZQUIERDO)
		grupo_unidades2 = locutor->tabla_semifonemas_i[indice3][indice4];
	      else
		grupo_unidades2 = locutor->tabla_semifonemas_d[indice3][indice4];

	      if (grupo_unidades2 == NULL)
		continue;

	      if ( (grupo_unidades2->numero_unidades < 30) &&
		   ( (indice1 != indice3) || (indice2 != indice4)) )
		continue;

	      if (izquierdo_derecho == IZQUIERDO)
		centroide_semifonema = (Vector_cepstrum *)
		  locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice3, indice4, 0)].vector;
	      else
		centroide_semifonema = (Vector_cepstrum *)
		  locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice3, indice4, 1)].vector;

	      distancia_auxiliar = calcula_distancia_euclidea_general((float *) correcaminos->cepstrum_mitad, (float *) centroide_semifonema, Vector_semifonema_candidato::numero_coeficientes_cepstrales);

	      if (distancia_auxiliar < distancia_minima) {
		indice_minimo1 = indice3;
		indice_minimo2 = indice4;
		distancia_minima = distancia_auxiliar;
	      } // if (distancia_auxiliar < ...

	      if ( (indice1 == indice3) && (indice2 == indice4) )
		distancia_correspondiente = distancia_auxiliar;

	    } // for (indice4 = 0; ...

	  if (origen_unidades.empty()) {

	    if (grupo_unidades->numero_unidades < 30) {
	      indice_minimo1 = indice1;
	      indice_minimo2 = indice2;
	    } // if (grupo_unidades->numero_unidades < 30)

	    if ( (indice_minimo1 != indice1) || (indice_minimo2 != indice2) ) {

	      if (izquierdo_derecho == IZQUIERDO)
		fprintf(fichero_errores, "%s.%s\tIzdo\t%d\t%s.%s.Izdo\t%f\t%f\n",
			correcaminos->contexto_izquierdo, correcaminos->semifonema,
			correcaminos->identificador, devuelve_puntero_a_nombre_fonema(indice_minimo1),
			devuelve_puntero_a_nombre_fonema(indice_minimo2), distancia_minima,
			distancia_correspondiente);
	      else
		fprintf(fichero_errores, "%s.%s\tDcho\t%d\t%s.%s.Dcho\t%f\t%f\n",
			correcaminos->semifonema, correcaminos->contexto_derecho,
			correcaminos->identificador, devuelve_puntero_a_nombre_fonema(indice_minimo1),
			devuelve_puntero_a_nombre_fonema(indice_minimo2), distancia_minima,
			distancia_correspondiente);

	    } // if ( (indice_minimo1 != indice1) ...

	    if ( (apunta_fichero = fichero_clases[indice_minimo1][indice_minimo2][izquierdo_derecho - '0']) ==
		 NULL) {

	      if (numero_ficheros_abiertos == 40) {
		for (int filas = 0; filas < NUMERO_FONEMAS; filas++)
		  for (int columnas = 0; columnas < NUMERO_FONEMAS; columnas++)
		    for (unsigned char zona = 0; zona < 2; zona++)
		      if (fichero_clases[filas][columnas][zona]) {
			fclose(fichero_clases[filas][columnas][zona]);
			fichero_clases[filas][columnas][zona] = NULL;
		      } // if (fichero_clases[filas][columnas][zona]...
		numero_ficheros_abiertos = 0;
	      } // if (numero_ficheros_abiertos == ...


	      strcpy(nombre_fichero, inicio_nombre_fichero);

	      crea_nombre_fichero(indice_minimo1, indice_minimo2, izquierdo_derecho,
				  nombre_fichero, ".txt");

	      if ( (fichero_clases[indice_minimo1][indice_minimo2][izquierdo_derecho - '0'] =
		    fopen(nombre_fichero, "at")) == NULL) {
		fprintf(stderr, "Error en organiza_base_de_datos_segun_centroides_semifonemas,\
 al intentar crear el fichero %s.\n", nombre_fichero);
		return 1;
	      } // if ( (fichero_clases[indice1][indice2]...

	      apunta_fichero = fichero_clases[indice_minimo1][indice_minimo2][izquierdo_derecho - '0'];
	      numero_ficheros_abiertos++;

	    } // if ( (apunta_fichero = ...

	    if (izquierdo_derecho == IZQUIERDO)
	      fprintf(apunta_fichero, "%s.%s\tIzdo\t%d\n",
		      correcaminos->contexto_izquierdo, correcaminos->semifonema,
		      correcaminos->identificador);
	    else
	      fprintf(apunta_fichero, "%s.%s\tDcho\t%d\n",
		      correcaminos->semifonema, correcaminos->contexto_derecho,
		      correcaminos->identificador);

	  } // if (origen_unidades.empty())
	  else {

	    if ( //( (correcaminos->semifonema[0] == '#') && (correcaminos->contexto_izquierdo[0] == '#') && (correcaminos->izquierdo_derecho == IZQUIERDO) && (correcaminos->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) ) ||
		 //( (correcaminos->semifonema[0] == '#') && (correcaminos->contexto_derecho[0] == '#') && (correcaminos->izquierdo_derecho == DERECHO) && (correcaminos->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) ) ||
		 ( (distancia_correspondiente - distancia_minima > umbral_eliminacion) && (grupo_unidades->numero_unidades > 30) &&
		   (correcaminos->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) ) ) {


	      elimina = 1;

	      if (grupo_unidades->numero_unidades - instancias_eliminadas > 30) {

		// Comprobamos el número de unidades por el otro lado
		if (izquierdo_derecho == IZQUIERDO) {
		  if ( (indice3 = devuelve_indice_fonema(correcaminos->semifonema)) == -1)
		    return 1;
		  if ( (indice4 = devuelve_indice_fonema(correcaminos->contexto_derecho)) == -1)
		    return 1;
		  grupo_unidades2 = locutor->tabla_semifonemas_d[indice3][indice4];
		  
		  if (grupo_unidades2->numero_unidades < 30)
		    elimina = 0;
		}
		else {
		  if ( (indice3 = devuelve_indice_fonema(correcaminos->contexto_izquierdo)) == -1)
		    return 1;
		  if ( (indice4 = devuelve_indice_fonema(correcaminos->semifonema)) == -1)
		    return 1;
		  grupo_unidades2 = locutor->tabla_semifonemas_i[indice3][indice4];

		  if (grupo_unidades2->numero_unidades < 30)
		    elimina = 0;
		}

		if (elimina == 1) {
		  
		  instancias_eliminadas++;
                  
		  if (correcaminos->identificador < locutor->primera_unidad_corpus500) {
		    adapta_identificador(origen_unidades[correcaminos->identificador], cadena_identificador);
		    sprintf(nombre_fichero, "%s%s_%s.uni_bin", inicio_nombre_ficheros_salida, corpus[0].c_str(),
			    cadena_identificador);
		  }
		  else {
		    adapta_identificador(origen_unidades[correcaminos->identificador], cadena_identificador);
		    sprintf(nombre_fichero, "%s%s_%s.uni_bin", inicio_nombre_ficheros_salida, corpus[1].c_str(),
			    cadena_identificador);
		  }
		  
		  fprintf(fichero_unidades_anuladas, "%s\t%d\t%s\t%s\t%s\n", nombre_fichero, correcaminos->identificador, correcaminos->semifonema, correcaminos->contexto_izquierdo, correcaminos->contexto_derecho);
		} // if (elimina == 1)

	      } // if (grupo_unidades->numero_unidades > 30)

	    } // if (distancia_correspondiente - distancia_minima ...

	  } // else // if (origen_unidades.empty())

	} // for (int cuenta = 0; cuenta < grupo_unidades

      } // for (izquierdo_derecho = '0'...

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      if (fichero_clases[indice1][indice2][0])
	fclose(fichero_clases[indice1][indice2][0]);
      if (fichero_clases[indice1][indice2][1])
	fclose(fichero_clases[indice1][indice2][1]);
    } // for (indice2 = 0;

  if (fichero_errores)
    fclose(fichero_errores);

  if (fichero_unidades_anuladas)
    fclose(fichero_unidades_anuladas);

  return 0;

}




#endif

/**
 * Función:   adapta_identificador                                               
 * \remarks Entrada:
 *			- identificador: identificador numérico del fichero.				 
 *	\remarks Salida:
 *			- cadena_identificador: cadena con el identificador modificado.		 
 * \remarks Valor devuelto:																 
 *			- En ausencia de error, devuelve un cero.							 
 * \remarks Objetivo:  Adapta el identificador al formato de cuatro caracteres, añadiendo 
 *			  ceros al principio.												 
 */

int adapta_identificador(int identificador, char *cadena_identificador) {

  int tamano;
  char cadena[100];

  sprintf(cadena, "%d", identificador);

  if ( (tamano = strlen(cadena)) > 5) {
    fprintf(stderr, "Error en adapta_identificador: el número no puede ser mayor de cinco dígitos.\n");
    return 1;
  }

  *cadena_identificador = '\0';

  for (int contador = tamano; contador < 5; contador++)
    strcat(cadena_identificador, "0");

  strcat(cadena_identificador, cadena);

  return 0;
    
}


#ifdef _CALCULA_DISTANCIAS_CEPSTRALES

/**
 * Función:   calcula_distancias                                                 
 * \remarks Entrada:
 *			- tipo_alofono: tipo de alófono del que se desean calcular las       
 *            distancias cepstrales entre las unidades de la base, según la      
 *            clasificación existente en el módulo modelo_duracion.hpp, pero     
 *            agrupando todas las vocales.                                       
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.      
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.         
 *          - locutor: objeto de tipo locutor en el que ha cargado el Corpus de  
 *			  de voz.									                         
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Calcula las distancias cepstrales medias entre las unidades de un  
 *			  mismo nombre, pero sólo para la clase de fonemas especificada.	 
 */

int calcula_distancias(clase_espectral tipo_alofono, map<int, int> &cadena_origen_unidades,
                       vector<string> &corpus, char path_cepstrum[], FILE *fichero_sal, Locutor *locutor) {

  int indice1, indice2;
  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  int maximo_indice;
  char izquierdo_derecho;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  float distancia;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = 0; izquierdo_derecho < 2; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	if (correcaminos->tipo_espectral != tipo_alofono)
	  continue;

	borra_cache_distancias();


	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	    continue;
                        
	  if (correcaminos->identificador >= maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());


	  if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
						numero_fichero, &cepstrum_objetivo, &numero_ventanas_objetivo, locutor->frecuencia_muestreo, 1))
	    return 1;

	  recorre = grupo_unidades->unidades;

	  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	       cuenta_vectores2++, recorre++) {

	    if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	      continue;
                            
	    if (recorre->identificador >= maximo_indice)
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	    else
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	    if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	      return 1;
	    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
						  numero_fichero,
						  &cepstrum_candidato, 
						  &numero_ventanas_candidato,
						  locutor->frecuencia_muestreo,
						  1))
	      return 1;

	    if (calcula_distancia_cepstral_media(cepstrum_objetivo, 
						 numero_ventanas_objetivo,
						 cepstrum_candidato, 
						 numero_ventanas_candidato,
						 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
						 &distancia))
	      return 1;

	    /*                        fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", correcaminos->identificador, correcaminos->contexto_izquierdo_2,
				      correcaminos->contexto_izquierdo, correcaminos->semifonema, correcaminos->contexto_derecho,
				      correcaminos->contexto_derecho_2);
				      fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", recorre->identificador, recorre->contexto_izquierdo_2,
				      recorre->contexto_izquierdo, recorre->semifonema, recorre->contexto_derecho,
				      recorre->contexto_derecho_2);
	    */

	    fprintf(fichero_sal, "%f\n", distancia);
	    //                        fprintf(fichero_sal, "%d\t%d\t\%f\n", correcaminos->identificador, recorre->identificador,
	    //                                                              distancia);

	  } // for (cuenta_vectores2 = 0; ...

	} // for (cuenta_vectores1 = 0; ...

      } // for (izquierdo_derecho = 0; ...

  return 0;

}


/**
 * Función:   calcula_distancias_y_subfunciones                                  
 * \remarks Entrada:
 *			- tipo_alofono: tipo de alófono del que se desean calcular las       
 *            distancias cepstrales entre las unidades de la base, según la      
 *            clasificación existente en el módulo distancia_espectral.hpp.		 
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.
 *          - locutor: objeto de tipo Locutor en el que está almacenada la
 *			  información del Corpus de voz y de los centroides.
 *			- unidades_por_banda: número máximo de unidades por banda que se
 *			  recogen.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Escribe, para los semifonemas del tipo indicado, los valores de 	 
 *			  las subfunciones del coste de objetivo al considerar cada unidad   
 *			  como objetivo y el resto como candidatas.							 
 * \remarks NOTA:
 *			- Las distancias relacionadas con los vectores mel-cepstrum se       
 *            calculan entre los centroides de los semifonemas candidato y       
 *            objetivo.                                                          
 */

int calcula_distancias_y_subfunciones(clase_espectral tipo_alofono, map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[], FILE *fichero_sal, Locutor *locutor, int unidades_por_banda) {

  int indice1, indice2;
  int trifonema, indice_fut2, indice_pas2;
  int cuenta_vectores1, cuenta_vectores2, contador;
  int tamano_maximo[6];
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char izquierdo_derecho;
  char solo_un_cero = '0';
  float distancia;
  int indice_tamano;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  //    int numero_distancias;
  //   Vector_distancias *apunta_distancia, *original_distancia, nuevo_vector;
  Vector_cepstrum *centroide_objetivo, *centroide_candidato;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;
  Viterbi objeto_viterbi;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	if (correcaminos->tipo_espectral != tipo_alofono)
	  continue;

	borra_cache_distancias();

	for (contador = 0; contador < 6; contador++)
	  tamano_maximo[contador] = unidades_por_banda;

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	    continue;

	  if (correcaminos->identificador >= maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	  if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (devuelve_ventanas_cepstrum_unidad(correcaminos, 
						path_cepstrum_local,
						numero_fichero,
						&cepstrum_objetivo, 
						&numero_ventanas_objetivo,
						locutor->frecuencia_muestreo, 
						1))
	    return 1;


	  recorre = grupo_unidades->unidades;

	  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	       cuenta_vectores2++, recorre++) {

	    if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	      continue;

	    if (recorre->identificador >= maximo_indice)
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	    else
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	    if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	      return 1;

	    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
						  numero_fichero,
						  &cepstrum_candidato, 
						  &numero_ventanas_candidato,
						  locutor->frecuencia_muestreo, 
						  1))
	      return 1;

	    if (calcula_distancia_cepstral_media(cepstrum_objetivo, 
						 numero_ventanas_objetivo,
						 cepstrum_candidato, 
						 numero_ventanas_candidato,
						 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
						 &distancia))
	      return 1;


	    if ( (distancia == 0.0) && (solo_un_cero == '1') )
	      continue;
	    else
	      if (distancia == 0.0)
		solo_un_cero = '1';

	    indice_tamano = (int) distancia;

	    if (indice_tamano > 5)
	      indice_tamano = 5;

	    if (tamano_maximo[indice_tamano] == 0)
	      continue;

	    (tamano_maximo[indice_tamano])--;

	    /*
	      nuevo_vector.objetivo = correcaminos;
	      nuevo_vector.candidato = recorre;
	      nuevo_vector.distancia = distancia;
	    */

	    if (objeto_viterbi.escribe_subcostes_C_t(recorre, correcaminos,
						     izquierdo_derecho, 
						     fichero_sal))
	      return 1;

	    if (izquierdo_derecho == IZQUIERDO)
	      fprintf(fichero_sal, "0\t");
	    else
	      fprintf(fichero_sal, "1\t");

	    // Añadimos la distancia euclídea

	    if (izquierdo_derecho == IZQUIERDO) {

	      if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_derecho))
		   == -1) {
		puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
		return 1;
	      } // if ( (trifonema = ...

	      centroide_objetivo =
		(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice2, trifonema, 1)].vector;

	      if ( (trifonema = devuelve_indice_fonema(recorre->contexto_derecho)) 
		   == -1) {
		puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
		return 1;
	      } // if ( (trifonema = ...

	      centroide_candidato =
		(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice2, trifonema, 1)].vector;

	    } // if (izquierdo_derecho == IZQUIERDO)
	    else { // DERECHO

	      if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_izquierdo))
		   == -1) {
		puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
		return 1;
	      } // if ( (trifonema = ...

	      centroide_objetivo =
		(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice1, 0)].vector;

	      if ( (trifonema = devuelve_indice_fonema(recorre->contexto_izquierdo))
		   == -1) {
		puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
		return 1;
	      } // if ( (trifonema = ...

	      centroide_candidato =
		(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice1, 0)].vector;

	    } // else // DERECHO

	    fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo, *centroide_candidato,
								    Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	    if ( (indice_fut2 = devuelve_indice_fonema(correcaminos->contexto_derecho_2))
		 //                        if ( (indice_fut2 = devuelve_indice_fonema(correcaminos->semifonema))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (indice_fut2

	    if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_derecho))
		 //                        if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_derecho))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (trifonema

	    centroide_objetivo =
	      (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice_fut2, 1)].vector;
	    //                            (Vector_cepstrum *) centroides_semifonemas_varianza[indice_fut2][trifonema][0].vector;

	    if ( (indice_fut2 = devuelve_indice_fonema(recorre->contexto_derecho_2))
		 //                        if ( (indice_fut2 = devuelve_indice_fonema(recorre->semifonema))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (indice_fut2

	    if ( (trifonema = devuelve_indice_fonema(recorre->contexto_derecho))
		 //                        if ( (trifonema = devuelve_indice_fonema(recorre->contexto_derecho))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (trifonema

	    centroide_candidato =
	      (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice_fut2, 1)].vector;
	    //                            (Vector_cepstrum *) centroides_semifonemas_varianza[indice_fut2][trifonema][0].vector;


	    fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo,
								    *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	    if ( (indice_pas2 = devuelve_indice_fonema(correcaminos->contexto_izquierdo_2))
		 //                        if ( (indice_pas2 = devuelve_indice_fonema(correcaminos->semifonema))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (indice_fut2

	    if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_izquierdo))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (trifonema

	    centroide_objetivo =
	      (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2, trifonema, 0)].vector;
	    //                            (Vector_cepstrum *) centroides_semifonemas_varianza[trifonema][indice_pas2][1].vector;

	    if ( (indice_pas2 = devuelve_indice_fonema(recorre->contexto_izquierdo_2))
		 //                        if ( (indice_pas2 = devuelve_indice_fonema(recorre->semifonema))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (indice_fut2

	    if ( (trifonema = devuelve_indice_fonema(recorre->contexto_izquierdo))
		 == -1) {
	      puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	      return 1;
	    } // if ( (trifonema

	    centroide_candidato =
	      (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2, trifonema, 0)].vector;
	    //                            (Vector_cepstrum *) centroides_semifonemas_varianza[trifonema][indice_pas2][1].vector;

	    fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo,
								    *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	    fprintf(fichero_sal, "%f\t", locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice1, indice2, izquierdo_derecho - '0')].varianza);


	    fprintf(fichero_sal, "%f\n", distancia);

	    /*                        if (anhade_vector_distancia(nuevo_vector, tamano_maximo))
				      return 1;
	    */
	  } // for (cuenta_vectores2 = 0; ...

	} // for (cuenta_vectores1 = 0; ...

	// Ahora tenemos que escribir en el fichero los valores de las subfunciones de
	// coste para la regresión lineal.
	// El número de valores que tomamos de cada grupo dependerá de su propio tamaño.
	/*
	  if (devuelve_vector_distancia(&original_distancia, &numero_distancias))
	  continue;

	  apunta_distancia = original_distancia;

	  for (contador = 0; contador < numero_distancias; contador++, apunta_distancia++) {
	  escribe_subcostes_C_t(apunta_distancia->candidato, apunta_distancia->objetivo,
	  izquierdo_derecho, fichero_sal);
	  fprintf(fichero_sal, "%f\n", apunta_distancia->distancia);
	  } // for (contador = 0;
	*/
      } // for (izquierdo_derecho = 0; ...

  return 0;

}


/**
 * Función:   calcula_distancias_y_subfunciones_acentual                         
 * \remarks Entrada:
 *			- posicion_grupo: GRUPO_INICIAL, GRUPO_MEDIO, GRUPO_FINAL o			 
 *			  GRUPO_INICIAL_Y_FINAL.											 
 *          - locutor: objeto de tipo Locutor en el que está almacenada la 		 
 *			  información del Corpus prosódico.									 
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las distancias   
 *            entre grupos, con los correspondientes valores de las subfunciones 
 *			  de coste.															 
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks NOTA:
 *			- Las distancias entre los contornos de los grupos acentuales se 	 
 *			  se calculan por medio de la raíz cuadrada de su distancia 		 
 *			  cuadrática media.												     
 */

int calcula_distancias_y_subfunciones_acentual(int posicion_grupo, Locutor *locutor,
					       FILE *fichero_sal) {


  int tipo_proposicion, silaba_acentuada;
  int indice;
  int posicion_obj;
  char tipo_pausa_obj, tipo_pausa_cand;
  float pos_obj, pos_cand;
  float duracion_obj, duracion_cand;
  float factor_caida;
  float distancia;
  Grupo_acentual_candidato *recorre_unidades_obj, *recorre_unidades_cand;
  estructura_grupos_acentuales *unidad_seleccionada;

  fprintf(fichero_sal, "PosAc\tTipProp\tSil\tPosFon\tDur\tFinAc\tCaidaNoFin\tCaidaFin\tDistancia\n");

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++) {

      indice = calcula_indice_grupo(tipo_proposicion, posicion_grupo, silaba_acentuada);

      if ( (unidad_seleccionada = locutor->tabla_grupos[indice]) != NULL) {

	recorre_unidades_obj = unidad_seleccionada->grupos;

	for (int contador = 0; contador < unidad_seleccionada->numero_grupos;
	     contador++, recorre_unidades_obj++) {

	  recorre_unidades_cand = recorre_unidades_obj;

	  posicion_obj = recorre_unidades_obj->posicion_grupo_acentual;

	  tipo_pausa_obj = recorre_unidades_obj->tipo_pausa;

	  duracion_obj = recorre_unidades_obj->final - recorre_unidades_obj->inicio;

	  for (int cuenta = 0; cuenta < unidad_seleccionada->numero_grupos - contador;
	       recorre_unidades_cand++, cuenta++) {

	    if (calcula_distancia_entre_contornos_cuadratica(recorre_unidades_obj,
							     recorre_unidades_cand,
							     1, &distancia))
	      return 1;

	    distancia = sqrt(distancia);

	    // Coste de posición de grupo acentual en grupo prosódico.

	    if ( (posicion_obj != 0) &&
		 (posicion_obj != recorre_unidades_obj->grupos_acentuales_grupo_prosodico - 1) ) {

	      pos_obj = (float) posicion_obj /
		(recorre_unidades_obj->grupos_acentuales_grupo_prosodico - 2);
	      pos_cand = (float) recorre_unidades_cand->posicion_grupo_acentual /
		(recorre_unidades_cand->grupos_acentuales_grupo_prosodico - 2);
	      fprintf(fichero_sal, "%f\t", absoluto(pos_obj - pos_cand));

	    }
	    else
	      fprintf(fichero_sal, "0\t");

	    // Coste de tipo de proposición

	    if (recorre_unidades_obj->tipo_grupo_prosodico_1 !=
		recorre_unidades_cand->tipo_grupo_prosodico_1)
	      fprintf(fichero_sal, "1\t");
	    else
	      fprintf(fichero_sal, "0\t");

	    // Coste de número de sílabas.
                            
	    fprintf(fichero_sal, "%d\t", absoluto(recorre_unidades_obj->numero_silabas -
						  recorre_unidades_cand->numero_silabas));

	    // Coste de posición de grupo prosódico en frase.

	    if (recorre_unidades_obj->posicion_grupo_prosodico)
	      pos_obj = (float) recorre_unidades_obj->posicion_grupo_prosodico /
		(recorre_unidades_obj->numero_grupos_prosodicos - 1);
	    else
	      pos_obj = 0.0;

	    if (recorre_unidades_cand->posicion_grupo_prosodico)
	      pos_cand = (float) recorre_unidades_cand->posicion_grupo_prosodico /
		(recorre_unidades_cand->numero_grupos_prosodicos - 1);
	    else
	      pos_cand = 0.0;


	    fprintf(fichero_sal, "%f\t", absoluto(pos_obj - pos_cand));

	    // Coste de duraciones:

	    duracion_cand = recorre_unidades_cand->final - recorre_unidades_obj->inicio;

                                                    
	    fprintf(fichero_sal, "%f\t",
		    absoluto(duracion_obj - duracion_cand) /
		    duracion_obj);


	    // coste de fin de grupo acentual (refleja el tipo de pausa).

	    tipo_pausa_cand = recorre_unidades_cand->tipo_pausa;

	    if (recorre_unidades_obj->posicion_grupo_acentual ==
		recorre_unidades_obj->grupos_acentuales_grupo_prosodico - 1) {
	      if (tipo_pausa_cand != tipo_pausa_obj)
		fprintf(fichero_sal, "%f\t", absoluto(tipo_pausa_obj - tipo_pausa_cand) /
			maximo(tipo_pausa_obj, tipo_pausa_cand));
	      else
		fprintf(fichero_sal, "0\t");
	    } // if (recorre_unidades_obj->posicion_grupo_acentual == ...
	    else
	      fprintf(fichero_sal, "0\t");

	    // Coste de caída anterior a una pausa que no es final de frase.

	    factor_caida =recorre_unidades_cand->factor_caida;

	    if (tipo_pausa_obj == PAUSA_OTRO_SIGNO_PUNTUACION) {
	      if (factor_caida <= 0)
		fprintf(fichero_sal, "1\t");
	      else
		fprintf(fichero_sal, "0\t");
	    } // if (tipo_pausa_obj == PAUSA_OTRO...
	    else
	      if (tipo_pausa_obj == PAUSA_COMA) {
		if (factor_caida <= 0)
		  fprintf(fichero_sal, "1\t");
		else
		  fprintf(fichero_sal, "0\t");
	      } // if (tipo_pausa_obj == PAUSA_OTRO...
	      else
		fprintf(fichero_sal, "0\t");


	    // Coste de factor de caída al final de la frase:

	    if (recorre_unidades_obj->posicion_grupo_acentual ==
		recorre_unidades_obj->grupos_acentuales_grupo_prosodico - 1) {
	      if (recorre_unidades_obj->tipo_grupo_prosodico_3 != FRASE_INACABADA) {
		if (factor_caida >= -10)
		  fprintf(fichero_sal, "1\t");
		else
		  fprintf(fichero_sal, "0\t");
	      } // if (recorre_unidades_obj->tipo_proposicion != FRASE_INACABADA
	      else
		if (factor_caida <= 0)
		  fprintf(fichero_sal, "1\t");
		else
		  fprintf(fichero_sal, "0\t");
	    } // if (recorre_unidades_obj->posicion_grupo_acentual
	    else
	      fprintf(fichero_sal, "0\t");

	    fprintf(fichero_sal, "%f\n", distancia);

	  } // for (int cuenta = 0; cuenta < ...

	} // for (int contador = 0; ...

      } // if ( (unidad_seleccionada = ...

    } //         for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++) {

  return 0;

}


/**
 * Función:   calcula_distancias_y_subfunciones_vector_medio                     
 * \remarks Entrada:
 *			- tipo_alofono: tipo de alófono del que se desean calcular las       
 *            distancias cepstrales entre las unidades de la base, según la      
 *            clasificación existente en el módulo modelo_duracion.hpp, pero     
 *            agrupando todas las vocales.                                       
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.
 *          - locutor: objeto de tipo locutor en el que están cargados los datos
 *			  del Corpus de voz y de los centroides.
 *			- unidades_por_banda: número máximo de unidades por banda que se 	 
 *			  recogen.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Escribe, para los semifonemas del tipo indicado, los valores de 	 
 *			  las subfunciones del coste de objetivo al considerar cada unidad   
 *			  como objetivo y el resto como candidatas.							 
 * \remarks NOTA:
 *			- Es similar a la función calcula_distancias_y_subfunciones, pero 	 
 *			  calcula las distancias espectrales entre el centroide del objetivo 
 *			  y el vector cepstrum medio de la unidad candidata.                 
 */

int calcula_distancias_y_subfunciones_vector_medio(clase_espectral tipo_alofono,
                                                   map<int, int> &cadena_origen_unidades,
                                                   vector<string> &corpus, 
                                                   char path_cepstrum[], FILE *fichero_sal,
                                                   Locutor *locutor,
                                                   int unidades_por_banda) {

  int indice1, indice2;
  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int cuenta_vectores1, cuenta_vectores2, contador;
  int tamano_maximo[6];
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char izquierdo_derecho;
  char solo_un_cero = '0';
  char path_cepstrum_local[FILENAME_MAX];
  float distancia;
  int indice_tamano;
  int maximo_indice;
  char numero_fichero[10];
  //    int numero_distancias;
  //   Vector_distancias *apunta_distancia, *original_distancia, nuevo_vector;
  Vector_cepstrum *centroide_objetivo, *centroide_candidato;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;
  Viterbi objeto_viterbi;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {
	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	//                if ( (alofono == VOC_ABIER) || (alofono == VOC_CERR) || (alofono == VOC_MED) )
	//                    alofono = VOC;
	//                else
	//				if (alofono == LIQUIDA)
	//                	alofono = LATERAL;

	if (correcaminos->tipo_espectral != tipo_alofono)
	  continue;

	borra_cache_distancias();

	for (contador = 0; contador < 6; contador++)
	  tamano_maximo[contador] = unidades_por_banda;

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	    continue;

	  if (correcaminos->identificador >= maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	  if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
						numero_fichero,
						&cepstrum_objetivo, &numero_ventanas_objetivo,
						locutor->frecuencia_muestreo, 1))
	    return 1;


	  recorre = grupo_unidades->unidades;

	  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	       cuenta_vectores2++, recorre++) {

	    if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	      continue;

	    if (recorre->identificador >= maximo_indice)
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	    else
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	    if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	      return 1;

	    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
						  numero_fichero,
						  &cepstrum_candidato, &numero_ventanas_candidato,
						  locutor->frecuencia_muestreo, 1))
	      return 1;

	    if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
						 cepstrum_candidato, numero_ventanas_candidato,
						 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
						 &distancia))
	      return 1;


	    if ( (distancia == 0.0) && (solo_un_cero == '1') )
	      continue;
	    else
	      if (distancia == 0.0)
		solo_un_cero = '1';

	    indice_tamano = (int) distancia;

	    if (indice_tamano > 5)
	      indice_tamano = 5;
	    /*
	    // Cambiar esto aquí: Sólo es para probarlo en la zona cercana, como
	    // en Black:

	    if (indice_tamano != 0)
	    continue;

	    // Hasta aquí.
	    */
	    if (tamano_maximo[indice_tamano] == 0)
	      continue;

	    (tamano_maximo[indice_tamano])--;

	    if (objeto_viterbi.escribe_subcostes_C_t(recorre, correcaminos,
						     izquierdo_derecho, fichero_sal))
	      return 1;
	    if (izquierdo_derecho == IZQUIERDO)
	      fprintf(fichero_sal, "0\t");
	    else
	      fprintf(fichero_sal, "1\t");

	    // Añadimos la distancia euclídea

	    indice_pas1_obj = devuelve_indice_fonema(correcaminos->contexto_izquierdo);
	    indice_central_obj = devuelve_indice_fonema(correcaminos->semifonema);
	    indice_fut1_obj = devuelve_indice_fonema(correcaminos->contexto_derecho);

	    if (izquierdo_derecho == IZQUIERDO) {

	      // Anterior fonema:

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_inicial;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Semifonema

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_mitad;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Siguiente

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_final;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));
	    } // if (izquierdo_derecho
	    else { // DERECHO
	      
	      // Anterior

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_inicial;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Semifonema

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 1)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_mitad;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Siguiente

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_final;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	    } // else // DERECHO


	    fprintf(fichero_sal, "%f\t", locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice1, indice2, izquierdo_derecho - '0')].varianza);

	    fprintf(fichero_sal, "%f\n", distancia);

	  } // for (cuenta_vectores2 = 0; ...

	} // for (cuenta_vectores1 = 0; ...

      } // for (izquierdo_derecho = 0; ...

  return 0;

}


/**
 * Función:   calcula_distancias_y_subfunciones_vector_medio_valores             
 * \remarks Entrada:
 *			- tipo_alofono: tipo de alófono del que se desean calcular las       
 *            distancias cepstrales entre las unidades de la base, según la      
 *            clasificación existente en el módulo modelo_duracion.hpp, pero     
 *            agrupando todas las vocales.                                       
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)  
 *            de los ficheros con los vectores cepstrales precalculados.         
 *          - locutor: objeto de tipo Locutor en el que está almacenada la		 
 *			  información del Corpus de voz y de los centroides.                 
 *			- unidades_por_banda: número máximo de unidades por banda que se 	 
 *			  recogen.															 
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Escribe, para los semifonemas del tipo indicado, los valores de 	 
 *			  los factores de la función de coste de objetivo al considerar cada 
 *			  unidad como objetivo y el resto como candidatas.					 
 */

int calcula_distancias_y_subfunciones_vector_medio_valores(clase_espectral tipo_alofono,
                                                           map<int, int> &cadena_origen_unidades,
                                                           vector<string> &corpus,
                                                           char path_cepstrum[], FILE *fichero_sal,
                                                           Locutor *locutor, int unidades_por_banda) {

  int indice1, indice2;
  int indice_central_obj, indice_pas1_obj, indice_fut1_obj;
  int cuenta_vectores1, cuenta_vectores2, contador;
  int tamano_maximo[6];
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char izquierdo_derecho;
  char solo_un_cero = '0';
  float distancia;
  int indice_tamano;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  //    int numero_distancias;
  //   Vector_distancias *apunta_distancia, *original_distancia, nuevo_vector;
  Vector_cepstrum *centroide_objetivo, *centroide_candidato;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	//                if ( (alofono == VOC_ABIER) || (alofono == VOC_CERR) || (alofono == VOC_MED) )
	//                    alofono = VOC;
	//                else
	//				if (alofono == LIQUIDA)
	//                	alofono = LATERAL;

	if (correcaminos->tipo_espectral != tipo_alofono)
	  continue;

	borra_cache_distancias();

	for (contador = 0; contador < 6; contador++)
	  tamano_maximo[contador] = unidades_por_banda;

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	    continue;

	  if (correcaminos->identificador >= maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	  if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
						numero_fichero,
						&cepstrum_objetivo, &numero_ventanas_objetivo,
						locutor->frecuencia_muestreo, 1))
	    return 1;


	  recorre = grupo_unidades->unidades;

	  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	       cuenta_vectores2++, recorre++) {

	    if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	      continue;

	    if (recorre->identificador >= maximo_indice)
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	    else
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	    if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	      return 1;

	    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
						  numero_fichero,
						  &cepstrum_candidato, 
						  &numero_ventanas_candidato,
						  locutor->frecuencia_muestreo, 
						  1))
	      return 1;

	    if (calcula_distancia_cepstral_media(cepstrum_objetivo, 
						 numero_ventanas_objetivo,
						 cepstrum_candidato, 
						 numero_ventanas_candidato,
						 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
						 &distancia))
	      return 1;


	    if ( (distancia == 0.0) && (solo_un_cero == '1') )
	      continue;
	    else
	      if (distancia == 0.0)
		solo_un_cero = '1';

	    indice_tamano = (int) distancia;

	    if (indice_tamano > 5)
	      indice_tamano = 5;

	    if (tamano_maximo[indice_tamano] == 0)
	      continue;

	    (tamano_maximo[indice_tamano])--;
	    /*
	      if (escribe_subcostes_C_t(recorre, correcaminos,
	      izquierdo_derecho, fichero_sal))
	      return 1;
	    */
	    if (izquierdo_derecho == IZQUIERDO) {
	      fprintf(fichero_sal, "%lf\t%lf\t", 
		      correcaminos->frecuencia_inicial,
		      recorre->frecuencia_inicial);
	      fprintf(fichero_sal, "%lf\t%lf\t", correcaminos->frecuencia_final,
		      recorre->frecuencia_final);
	    } // if (izquierdo_derecho == IZQUIERDO)
	    else {
	      fprintf(fichero_sal, "%lf\t%lf\t", correcaminos->frecuencia_final,
		      recorre->frecuencia_final);
	      fprintf(fichero_sal, "%lf\t%lf\t", correcaminos->frecuencia_inicial,
		      recorre->frecuencia_inicial);
	    } // else

	    fprintf(fichero_sal, "%c\t%c\t", correcaminos->frontera_inicial + '0',
		    recorre->frontera_inicial + '0');

	    fprintf(fichero_sal, "%c\t%c\t", correcaminos->frontera_final + '0',
		    recorre->frontera_final + '0');

	    fprintf(fichero_sal, "%c\t%c\t", correcaminos->frase + '0',
		    recorre->frase + '0');

	    fprintf(fichero_sal, "%c\t%c\t", correcaminos->posicion + '0',
		    recorre->posicion + '0');

	    fprintf(fichero_sal, "%c\t%c\t", correcaminos->acento + '0',
		    recorre->acento + '0');

	    fprintf(fichero_sal, "%lf\t%lf\t", correcaminos->duracion,
		    recorre->duracion);
	    if (izquierdo_derecho == IZQUIERDO)
	      fprintf(fichero_sal, "0\t");
	    else
	      fprintf(fichero_sal, "1\t");

	    // Añadimos la distancia euclídea

	    indice_pas1_obj = devuelve_indice_fonema(correcaminos->contexto_izquierdo);
	    indice_central_obj = devuelve_indice_fonema(correcaminos->semifonema);
	    indice_fut1_obj = devuelve_indice_fonema(correcaminos->contexto_derecho);

	    if (izquierdo_derecho == IZQUIERDO) {

	      // Anterior

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_inicial;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Semifonema

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_mitad;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Siguiente

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_final;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));
	    } // if (izquierdo_derecho
	    else { // DERECHO

	      // Anterior

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas1_obj, indice_central_obj, 1)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_inicial;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Semifonema

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_mitad;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	      // Siguiente

	      centroide_objetivo = (Vector_cepstrum *)
		locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_central_obj, indice_fut1_obj, 0)].vector;

	      centroide_candidato = (Vector_cepstrum *) recorre->cepstrum_final;

	      fprintf(fichero_sal, "%f\t",
		      calcula_distancia_euclidea(*centroide_objetivo,
						 *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

	    } // else // DERECHO

	    fprintf(fichero_sal, "%f\t", locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice1, indice2, izquierdo_derecho - '0')].varianza);

	    fprintf(fichero_sal, "%f\n", distancia);

	  } // for (cuenta_vectores2 = 0; ...

	} // for (cuenta_vectores1 = 0; ...

      } // for (izquierdo_derecho = 0; ...

  return 0;

}



/**
 * Función:   calcula_distancias_y_subfunciones                                  
 * \remarks Entrada:
 *			- alofono1: Parte del semifonema (central o contexto, según el valor 
 *            de izquierdo_derecho).                                             
 *          - alofono2: Parte del semifonema.                                    
 *          - izquierdo_derecho: IZQUIERDO o DERECHO.                            
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)  
 *            de los ficheros con los vectores cepstrales precalculados.         
 *          - locutor: objeto de tipo Locutor en el que están almacenados los	 
 *			  datos del Corpus de voz y de los centroides.						 
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Escribe, para los semifonemas del nombre indicado, los valores de  
 *			  las subfunciones del coste de objetivo al considerar cada unidad   
 *			  como objetivo y el resto como candidatas.							 
 * \remarks NOTA:
 *			- Las distancias relacionadas con los vectores mel-cepstrum se       
 *            calculan entre los centroides de los semifonemas candidato y       
 *            objetivo.
 */

int calcula_distancias_y_subfunciones(char alofono1[], char alofono2[], char izquierdo_derecho,
                                      map<int, int> &cadena_origen_unidades, vector<string> &corpus,
                                      char path_cepstrum[], FILE *fichero_sal, Locutor *locutor) {

  int indice1, indice2;
  int trifonema, indice_fut2, indice_pas2;
  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char solo_un_cero = '0';
  int maximo_indice;
  float distancia;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  Vector_cepstrum *centroide_objetivo, *centroide_candidato;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;
  Viterbi objeto_viterbi;

  maximo_indice = locutor->primera_unidad_corpus500;

  izquierdo_derecho == IZQUIERDO ? grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
										IZQUIERDO, locutor):
    grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
						 DERECHO, locutor);
  if (grupo_unidades == NULL) {
    fprintf(stderr, "La unidad %s.%s no se encuentra en el Corpus.\n", alofono1, alofono2);
    return 1;
  } // if (grupo_unidades == NULL)

  correcaminos = grupo_unidades->unidades;

  indice1 = devuelve_indice_fonema(alofono1);
  indice2 = devuelve_indice_fonema(alofono2);

  for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
       cuenta_vectores1++, correcaminos++) {


    if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
      continue;

    if (correcaminos->identificador >= maximo_indice)
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
    else
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

    if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
      return 1;

    if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
					  numero_fichero,
					  &cepstrum_objetivo, &numero_ventanas_objetivo,
					  locutor->frecuencia_muestreo, 1))
      return 1;

    recorre = grupo_unidades->unidades;

    for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	 cuenta_vectores2++, recorre++) {

      if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	continue;

      if (recorre->identificador >= maximo_indice)
	sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
      else
	sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

      if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	return 1;

      if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
					    numero_fichero,
					    &cepstrum_candidato, &numero_ventanas_candidato,
					    locutor->frecuencia_muestreo, 1))
	return 1;

      if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
					   cepstrum_candidato, numero_ventanas_candidato,
					   Vector_semifonema_candidato::numero_coeficientes_cepstrales,
					   &distancia))
	return 1;


      if ( (distancia == 0.0) && (solo_un_cero == '1') )
	continue;
      else
	if (distancia == 0.0)
	  solo_un_cero = '1';

      if (objeto_viterbi.escribe_subcostes_C_t(recorre, correcaminos,
					       izquierdo_derecho, fichero_sal))
	return 1;

      if (izquierdo_derecho == IZQUIERDO)
	fprintf(fichero_sal, "0\t");
      else
	fprintf(fichero_sal, "1\t");

      // Añadimos la distancia euclídea

      if (izquierdo_derecho == IZQUIERDO) {

	if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_derecho))
	     == -1) {
	  puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	  return 1;
	} // if ( (trifonema = ...

	centroide_objetivo =
	  (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice2, trifonema, 1)].vector;

	if ( (trifonema = devuelve_indice_fonema(recorre->contexto_derecho))
	     == -1) {
	  puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	  return 1;
	} // if ( (trifonema = ...

	centroide_candidato =
	  (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice2, trifonema, 1)].vector;

      } // if (izquierdo_derecho == IZQUIERDO)
      else {

	if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_izquierdo))
	     == -1) {
	  puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	  return 1;
	} // if ( (trifonema = ...

	centroide_objetivo =
	  (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice1, 0)].vector;

	if ( (trifonema = devuelve_indice_fonema(recorre->contexto_izquierdo))
	     == -1) {
	  puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	  return 1;
	} // if ( (trifonema = ...

	centroide_candidato =
	  (Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice1, 0)].vector;

      } // else

      fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo, *centroide_candidato,
							      Vector_semifonema_candidato::numero_coeficientes_cepstrales));

      if ( (indice_fut2 = devuelve_indice_fonema(correcaminos->contexto_derecho_2))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (indice_fut2

      if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_derecho))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (trifonema

      centroide_objetivo =
	(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice_fut2, 1)].vector;

      if ( (indice_fut2 = devuelve_indice_fonema(recorre->contexto_derecho_2))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (indice_fut2

      if ( (trifonema = devuelve_indice_fonema(recorre->contexto_derecho))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (trifonema

      centroide_candidato =
	(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(trifonema, indice_fut2, 1)].vector;

      fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo,
							      *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

      if ( (indice_pas2 = devuelve_indice_fonema(correcaminos->contexto_izquierdo_2))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (indice_fut2

      if ( (trifonema = devuelve_indice_fonema(correcaminos->contexto_izquierdo))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (trifonema

      centroide_objetivo =
	(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2, trifonema, 0)].vector;

      if ( (indice_pas2 = devuelve_indice_fonema(recorre->contexto_izquierdo_2))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (indice_fut2

      if ( (trifonema = devuelve_indice_fonema(recorre->contexto_izquierdo))
	   == -1) {
	puts("Error en calcula_distancias_y_subfunciones: Fonema erróneo.");
	return 1;
      } // if ( (trifonema

      centroide_candidato =
	(Vector_cepstrum *) locutor->centroides_semifonemas_varianza[calcula_indice_tabla_centroides(indice_pas2, trifonema, 0)].vector;

      fprintf(fichero_sal, "%f\t", calcula_distancia_euclidea(*centroide_objetivo,
							      *centroide_candidato, Vector_semifonema_candidato::numero_coeficientes_cepstrales));

      fprintf(fichero_sal, "%f\n", distancia);

    } // for (cuenta_vectores2 = 0; ...

  } // for (cuenta_vectores1 = 0; ...


  return 0;

}



/**
 * Función:   calcula_distancias_centroide                                       
 * \remarks Entrada:
 *			- alofono1: Parte del semifonema (central o contexto, según el valor 
 *            de izquierdo_derecho).                                             
 *          - alofono2: Parte del semifonema.                                    
 *          - izquierdo_derecho: Izquierdo->0, Derecho->1.                       
 *            Si es izquierdo, alofono2 es la parte central y alofono1 es el     
 *            contexto.                                                          
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.      
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.
 *          - locutor: objeto de tipo Locutor en el que está la información del
 *			  Corpus de voz.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las
 *            distancias cepstrales.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Calcula las distancias de todas las unidades de nombre alofono1-
 *			  alofono2 (izquierdo_derecho) a su correspondiente centroide.
 */

int calcula_distancias_centroide(char alofono1[], char alofono2[], char izquierdo_derecho,
                                 map<int, int> &cadena_origen_unidades, vector<string> &corpus,
                                 char path_cepstrum[],
                                 FILE *fichero_sal, Locutor *locutor) {


  int identificador;
  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  float distancia;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  Vector_cepstrum centroide;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;

  maximo_indice = locutor->primera_unidad_corpus500;

  izquierdo_derecho == IZQUIERDO ? grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
										IZQUIERDO, locutor):
    grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
						 DERECHO, locutor);
  if (grupo_unidades == NULL) {
    fprintf(stderr, "La unidad %s.%s no se encuentra en el Corpus.\n", alofono1, alofono2);
    return 1;
  } // if (grupo_unidades == NULL)

  correcaminos = grupo_unidades->unidades;

  // En primer lugar cargamos los vectores de coeficientes cepstrales:

  for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
       cuenta_vectores1++, correcaminos++) {

    if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
      continue;

    if (correcaminos->identificador >= maximo_indice)
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
    else
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());


    if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
      return 1;

    if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
					  numero_fichero,
					  &cepstrum_objetivo, &numero_ventanas_objetivo,
					  locutor->frecuencia_muestreo, 1))
      return 1;

  } // for (cuenta_vectores1 = 0...

    // Ahora calculamos el centroide de los vectores de coeficientes cepstrales, ya cargados
    // en la caché.

  if (calcula_centroide(centroide))
    return 1;

  // Buscamos la unidad más próxima al centroide:

  if (encuentra_unidad_proxima_a_centroide(centroide, Vector_semifonema_candidato::numero_coeficientes_cepstrales, &identificador, &cepstrum_objetivo, &numero_ventanas_objetivo))
    return 1;


  // Ahora calculamos las distancias a dicha unidad:


  recorre = grupo_unidades->unidades;

  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
       cuenta_vectores2++, recorre++) {

    if ( (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS) ||
	 (recorre->identificador == identificador) )
      continue;

    if (recorre->identificador >= maximo_indice)
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
    else
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

    if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
      return 1;

    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
					  numero_fichero,
					  &cepstrum_candidato, &numero_ventanas_candidato,
					  locutor->frecuencia_muestreo, 1))
      return 1;

    if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
					 cepstrum_candidato, numero_ventanas_candidato,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
					 &distancia))
      return 1;


    /*                        fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", correcaminos->identificador, correcaminos->contexto_izquierdo_2,
			      correcaminos->contexto_izquierdo, correcaminos->semifonema, correcaminos->contexto_derecho,
			      correcaminos->contexto_derecho_2);
			      fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", recorre->identificador, recorre->contexto_izquierdo_2,
			      recorre->contexto_izquierdo, recorre->semifonema, recorre->contexto_derecho,
			      recorre->contexto_derecho_2);
    */

    fprintf(fichero_sal, "%f\n", distancia);
    //                        fprintf(fichero_sal, "%d\t%d\t\%f\n", correcaminos->identificador, recorre->identificador,
    //                                                              distancia);

  } // for (cuenta_vectores2 = 0; ...


  return 0;

}



/**
 * Función:   calcula_distancias
 * \remarks Entrada:
 *			- alofono1: Parte del semifonema (central o contexto, según el valor
 *            de izquierdo_derecho).
 *          - alofono2: Parte del semifonema.
 *          - izquierdo_derecho: Izquierdo->0, Derecho->1.
 *            Si es izquierdo, alofono2 es la parte central y alofono1 es el
 *            contexto.
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.
 *          - locutor: objeto de la clase Locutor, en el que se encuentra la
 *			  información del Corpus de voz.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las
 *            distancias cepstrales.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Calcula las distancias cepstrales medias entre los semifonemas del
 *			  mismo nombre.
 */

int calcula_distancias(char alofono1[], char alofono2[], char izquierdo_derecho,
                       map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
                       FILE *fichero_sal, Locutor *locutor) {

  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  float distancia;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;


  izquierdo_derecho == IZQUIERDO ? grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
										IZQUIERDO, locutor):
    grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
						 DERECHO, locutor);
  if (grupo_unidades == NULL) {
    fprintf(stderr, "La unidad %s.%s no se encuentra en el Corpus.\n", alofono1, alofono2);
    return 1;
  } // if (grupo_unidades == NULL)


  correcaminos = grupo_unidades->unidades;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
       cuenta_vectores1++, correcaminos++) {

    if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
      continue;

    if (correcaminos->identificador >= maximo_indice)
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
    else
      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum,corpus[0].c_str());

    if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
      return 1;

    if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
					  numero_fichero,
					  &cepstrum_objetivo, &numero_ventanas_objetivo,
					  locutor->frecuencia_muestreo, 1))
      return 1;

    recorre = grupo_unidades->unidades;

    for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	 cuenta_vectores2++, recorre++) {

      if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	continue;

      if (recorre->identificador >= maximo_indice)
	sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
      else
	sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());


      if (adapta_identificador(cadena_origen_unidades[recorre->identificador], numero_fichero))
	return 1;

      if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
					    numero_fichero,
					    &cepstrum_candidato, &numero_ventanas_candidato,
					    locutor->frecuencia_muestreo, 1))
	return 1;

      if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
					   cepstrum_candidato, numero_ventanas_candidato,
					   Vector_semifonema_candidato::numero_coeficientes_cepstrales,
					   &distancia))
	return 1;

      /*                        fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", correcaminos->identificador, correcaminos->contexto_izquierdo_2,
				correcaminos->contexto_izquierdo, correcaminos->semifonema, correcaminos->contexto_derecho,
				correcaminos->contexto_derecho_2);
				fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t", recorre->identificador, recorre->contexto_izquierdo_2,
				recorre->contexto_izquierdo, recorre->semifonema, recorre->contexto_derecho,
				recorre->contexto_derecho_2);
      */

      fprintf(fichero_sal, "%f\n", distancia);
      //                        fprintf(fichero_sal, "%d\t%d\t\%f\n", correcaminos->identificador, recorre->identificador,
      //                                                              distancia);

    } // for (cuenta_vectores2 = 0; ...

  } // for (cuenta_vectores1 = 0; ...

  return 0;

}


/**
 * Función:   calcula_distancias
 * \remarks Entrada:
 *			- alofono1: Parte del semifonema (central o contexto, según el valor
 *            de izquierdo_derecho).
 *          - alofono2: Parte del semifonema.
 *          - izquierdo_derecho: Izquierdo->0, Derecho->1.
 *            Si es izquierdo, alofono2 es la parte central y alofono1 es el
 *            contexto.
 *			- semifonema: semifonema respecto al que se van a calcular las
 *			  distancias cepstrales.
 *          - locutor: objeto de la clase Locutor, en el que se encuentra la
 *			  información del Corpus de voz.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las
 *            distancias cepstrales.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Calcula las distancias cepstrales medias entre los semifonemas del
 *			  mismo nombre.
 */

int calcula_distancias(char alofono1[], char alofono2[], char izquierdo_derecho,
		       Vector_semifonema_candidato *semifonema,	
                       FILE *fichero_sal, Locutor *locutor) {

  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  float distancia;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;
  Vector_semifonema_candidato *correcaminos, *recorre;

  izquierdo_derecho == IZQUIERDO ? grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
										IZQUIERDO, locutor):
    grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
						 DERECHO, locutor);
  if (grupo_unidades == NULL) {
    fprintf(stderr, "La unidad %s.%s no se encuentra en el Corpus.\n", alofono1, alofono2);
    return 1;
  } // if (grupo_unidades == NULL)


  correcaminos = grupo_unidades->unidades;

  for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
       cuenta_vectores1++, correcaminos++) {

    distancia = Viterbi::calcula_coste_coarticulacion(semifonema, correcaminos);

    fprintf(fichero_sal, "%d\t%s\t%s\t%s\t%s\t%s\t%f\n", correcaminos->identificador, correcaminos->contexto_izquierdo_2,
	    correcaminos->contexto_izquierdo,
	    correcaminos->semifonema, correcaminos->contexto_derecho, correcaminos->contexto_derecho_2, distancia);

  } // for (cuenta_vectores1 = 0; ...

  return 0;

}

/**
 * Función:   calcula_centroides                                                 
 * \remarks Entrada:
 *			- cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del
 *            fichero) de los ficheros del corpus con los vectores
 *			  cepstrales precalculados.
 *          - incluye_varianza: si es distinto de 0, se calcula también la
 *            varianza de las distancias espectrales del grupo considerado.
 *          - locutor: objeto de tipo Locutor en el que está la información del
 *			  Corpus de voz.
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las
 *            distancias cepstrales.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Calcula los centroides de todas las unidades del Corpus, a partir
 *			  de las realizaciones de los semifonemas del mismo nombre.
 */

int calcula_centroides(map<int, int> &cadena_origen_unidades, vector<string> &corpus,
		       char path_cepstrum[],
		       FILE *fichero_sal,
                       char incluye_varianza, Locutor *locutor) {

  int indice1, indice2;
  int cuenta_vectores1, contador;
  int numero_ventanas_objetivo;
  float *apunta_cepstrum;
  float varianza;
  char izquierdo_derecho;
  //    char hay_unidades;
  int maximo_indice;
  char numero_fichero[10];
  char path_cepstrum_local[FILENAME_MAX];
  FILE *fichero_centroides;
  estructura_unidades *grupo_unidades;
  Vector_cepstrum centroide;
  Vector_cepstrum_tiempo *cepstrum_objetivo;
  Vector_semifonema_candidato *correcaminos;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++) {
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	borra_cache_distancias();

	//                hay_unidades = 0;

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	  if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (correcaminos->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {
	    if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
						  numero_fichero, &cepstrum_objetivo, &numero_ventanas_objetivo,
						  locutor->frecuencia_muestreo, 1))
	      return 1;
	  }
	  else
	    if (anhade_coeficientes_cepstrum_difonema(correcaminos))
	      return 1;

	} // for (cuenta_vectores1 = 0...


	// Ahora calculamos el centroide de los vectores de coeficientes cepstrales,
	// ya cargados en la caché.

	if (calcula_centroide(centroide))
	  return 1;


	// Una vez calculado el centroide, podemos calcular la varianza (si la piden)

	if (incluye_varianza != 0) {
	  if (calcula_varianza(centroide, Vector_semifonema_candidato::numero_coeficientes_cepstrales, &varianza))
	    return 1;
	} // if (incluye_varianza != 0)


	fprintf(fichero_sal, "%s\t%s\t", devuelve_puntero_a_nombre_fonema(indice1), devuelve_puntero_a_nombre_fonema(indice2));

	if (izquierdo_derecho == IZQUIERDO)
	  fprintf(fichero_sal, "IZQ\t");
	else
	  fprintf(fichero_sal, "DER\t");

	if (incluye_varianza == 0)
	  apunta_cepstrum = (float *)
	    centroides_semifonemas[indice1][indice2][izquierdo_derecho - '0'];
	else {
	  apunta_cepstrum = (float *)
	    centroides_semifonemas_varianza[indice1][indice2][izquierdo_derecho - '0'].vector;
	  centroides_semifonemas_varianza[indice1][indice2][izquierdo_derecho -'0'].varianza = varianza;

	  fprintf(fichero_sal, "%f\t", varianza);

	} // else
	for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++) {
	  fprintf(fichero_sal, "%f ", centroide[contador]);
	  *apunta_cepstrum++ = centroide[contador];
	} // for (contador = 0; contador < ...

	fprintf(fichero_sal, "\n");


      } // for (izquierdo_derecho == '0')

    } // for (indice2 = 0; ...

  } // for (indice1 = 0; ...

    // Ahora escribimos el contenido de centroides_semifonemas en el fichero

  if (incluye_varianza == 0) {

    char nombre_fichero_salida[FILENAME_MAX];

    sprintf(nombre_fichero_salida, "fichero_centroides_%s.bin", locutor->nombre);

    if ( (fichero_centroides = fopen(nombre_fichero_salida, "wb")) == NULL) {
      fprintf(stderr, "Error al intentar crear el fichero %s.\n", nombre_fichero_salida);
      return 1;
    } // if ( (fichero_centroides...


    fwrite(centroides_semifonemas, sizeof(Vector_cepstrum), NUMERO_FONEMAS*NUMERO_FONEMAS*2,
	   fichero_centroides);

    fclose(fichero_centroides);

    return 0;

  } // if (incluye_varianza == 0)

  if ( (fichero_centroides = fopen(FICHERO_CENTROIDES, "wb")) == NULL) {
    fprintf(stderr, "Error al intentar crear el fichero %s.\n", FICHERO_CENTROIDES);
    return 1;
  } // if ( (fichero_centroides =

  fwrite(centroides_semifonemas_varianza, sizeof(Vector_cepstrum_varianza), NUMERO_FONEMAS*NUMERO_FONEMAS*2,
	 fichero_centroides);

  fclose(fichero_centroides);
    
  return 0;

}


/**
 * Función:   calcula_centroides_generico
 * \remarks Entrada:
 *			- cadena_origen_unidades: cadena en la que se encuentra indexado el
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus.
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del
 *            fichero) de los ficheros cepstrum.
 *          - nombre_locutor: nombre del locutor.
 *          - incluye_varianza: si es distinto de 0, se calcula también la       
 *            varianza de las distancias espectrales del grupo considerado.      
 *          - locutor: objeto de tipo Locutor en el que está la información del  
 *			  Corpus de voz.													 
 *	\remarks Salida:
 *			- fichero_sal: puntero al fichero en el que se van a escribir las    
 *            distancias cepstrales.                                             
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 * \remarks Objetivo:  Calcula los centroides de todas las unidades del Corpus, a partir  
 *			  de las realizaciones de los semifonemas del mismo nombre.			 	
 */

int calcula_centroides_generico(map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
				char nombre_locutor[], FILE *fichero_sal, char incluye_varianza, Locutor *locutor) {

  int indice1, indice2;
  int cuenta_vectores1, contador;
  int numero_ventanas_objetivo;
  float *apunta_cepstrum;
  float varianza;
  char izquierdo_derecho;
  char path_cepstrum_real[FILENAME_MAX];
  int maximo_indice;
  char numero_fichero[10];
  FILE *fichero_centroides;
  estructura_unidades *grupo_unidades;
  Vector_cepstrum centroide;
  Vector_cepstrum_tiempo *cepstrum_objetivo;
  Vector_semifonema_candidato *correcaminos;

  maximo_indice = locutor->primera_unidad_corpus500;

  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++) {
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++) {
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	borra_cache_distancias();

	//                hay_unidades = 0;

	if (izquierdo_derecho == '0')
	  printf("%s %s Izquierdo\n", correcaminos->contexto_izquierdo, correcaminos->semifonema);
	else
	  printf("%s %s Derecho\n", correcaminos->semifonema, correcaminos->contexto_derecho);

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {
	    if (adapta_identificador(cadena_origen_unidades[correcaminos->identificador], numero_fichero))
	      return 1;
                      
	    if (correcaminos->identificador < maximo_indice)
	      sprintf(path_cepstrum_real, "%s/%s%s_", path_cepstrum, nombre_locutor, corpus[0].c_str());
	    else
	      sprintf(path_cepstrum_real, "%s/%s%s_", path_cepstrum, nombre_locutor, corpus[1].c_str());

	    if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_real,
						  numero_fichero,
						  &cepstrum_objetivo, &numero_ventanas_objetivo,
						  locutor->frecuencia_muestreo, 1))
	      return 1;

	  }
	  else
	    if (anhade_coeficientes_cepstrum_difonema(correcaminos))
	      return 1;

	} // for (cuenta_vectores1 = 0...

	// Ahora calculamos el centroide de los vectores de coeficientes cepstrales,
	// ya cargados en la caché.

	if (calcula_centroide(centroide))
	  return 1;

	// Una vez calculado el centroide, podemos calcular la varianza (si la piden)

	if (incluye_varianza != 0) {
	  if (calcula_varianza(centroide, Vector_semifonema_candidato::numero_coeficientes_cepstrales, &varianza))
	    return 1;
	} // if (incluye_varianza != 0)

	//                if (hay_unidades) {

	fprintf(fichero_sal, "%s\t%s\t", devuelve_puntero_a_nombre_fonema(indice1), devuelve_puntero_a_nombre_fonema(indice2));

	if (izquierdo_derecho == IZQUIERDO)
	  fprintf(fichero_sal, "IZQ\t");
	else
	  fprintf(fichero_sal, "DER\t");

	if (incluye_varianza == 0)
	  apunta_cepstrum = (float *)
	    centroides_semifonemas[indice1][indice2][izquierdo_derecho - '0'];
	else {
	  apunta_cepstrum = (float *)
	    centroides_semifonemas_varianza[indice1][indice2][izquierdo_derecho - '0'].vector;
	  centroides_semifonemas_varianza[indice1][indice2][izquierdo_derecho -'0'].varianza = varianza;

	  fprintf(fichero_sal, "%f\t", varianza);

	} // else
	for (contador = 0; contador < Vector_semifonema_candidato::numero_coeficientes_cepstrales; contador++) {
	  fprintf(fichero_sal, "%f ", centroide[contador]);
	  *apunta_cepstrum++ = centroide[contador];
	} // for (contador = 0; contador < ...

	fprintf(fichero_sal, "\n");

	//        		} // hay_unidades


      } // for (izquierdo_derecho == '0')
    } // for (indice2 = 0; ...

  } // for (indice1 = 0; ...

    // Ahora escribimos el contenido de centroides_semifonemas en el fichero

  if (incluye_varianza == 0) {

    char nombre_fichero_salida[FILENAME_MAX];

    sprintf(nombre_fichero_salida, "fichero_centroides_%s.bin", locutor->nombre);

    if ( (fichero_centroides = fopen(nombre_fichero_salida, "wb")) == NULL) {
      fprintf(stderr, "Error al intentar crear el fichero %s.\n", nombre_fichero_salida);
      return 1;
    } // if ( (fichero_centroides...


    fwrite(centroides_semifonemas, sizeof(Vector_cepstrum), NUMERO_FONEMAS*NUMERO_FONEMAS*2,
	   fichero_centroides);

    fclose(fichero_centroides);

    return 0;

  } // if (incluye_varianza == 0)

  if ( (fichero_centroides = fopen(FICHERO_CENTROIDES, "wb")) == NULL) {
    fprintf(stderr, "Error al intentar crear el fichero %s.\n", FICHERO_CENTROIDES);
    return 1;
  } // if ( (fichero_centroides =

  fwrite(centroides_semifonemas_varianza, sizeof(Vector_cepstrum_varianza), NUMERO_FONEMAS*NUMERO_FONEMAS*2,
	 fichero_centroides);

  fclose(fichero_centroides);
    
  return 0;

}


/**
 * Función:   carga_centroides_semifonemas                                      
 * \remarks Entrada:
 *			- fichero: puntero al fichero en el que se encuentra la información 
 *            de los centroides de los semifonemas.                             
 *          - incluye_varianza: Si es distinto de 0, se incluye la varianza.    
 */

void carga_centroides_semifonemas(FILE *fichero, int incluye_varianza) {

  if (incluye_varianza == 0)
    fread(centroides_semifonemas, sizeof(Vector_cepstrum), NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero);
  else
    fread(centroides_semifonemas_varianza, sizeof(Vector_cepstrum_varianza),
	  NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero);

}


/**
 * Función:   calcula_distancia_entre_unidades                                   
 * \remarks Entrada:
 *			- alofono1: Parte del semifonema (central o contexto, según el valor 
 *            de izquierdo_derecho).                                             
 *          - alofono2: Parte del semifonema.                                    
 *          - izquierdo_derecho: IZQUIERDO o DERECHO.                            
 *          - cadena_origen_unidades: cadena en la que se encuentra indexado el  
 *            número del fichero del Corpus del que se extrajo cada unidad.      
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)  
 *            de los ficheros con los vectores cepstrales precalculados.         
 *          - identificador_escogido: identificador numérico de la unidad        
 *            escogida al haber eliminado la original.                           
 *          - identificador_original: identificador numérico de la unidad        
 *            original.                                                          
 *			- locutor: objeto de tipo Locutor en el que está la información de 	 
 *			  base de datos de unidades.										 
 *	\remarks Salida:
 *			- distancia: distancia espectral entre ambas unidades.               
 *          - unidad_valida: vale 1 si la unidad especificada por identificador_ 
 *            original es del tipo alofono1-alofono2-izquierdo_derecho. Se       
 *            emplea para no computar aquellos casos en los que se ha perdido el 
 *            sincronismo como consecuencia de que el locutor no pronunción      
 *            algún sonido al grabar el Corpus.                                  
 *          - fichero_errores: fichero en el que se van escribiendo los errores  
 *            de sincronismo.                                                    
 * \remarks Valor devuelto:                                                               
 *          - En ausencia de error se devuelve un cero.                          
 */

int calcula_distancia_entre_unidades(char alofono1[], char alofono2[], char izquierdo_derecho,
                                     map<int, int> &cadena_origen_unidades, char path_cepstrum[],
                                     int identificador_escogido, int identificador_original,
                                     Locutor *locutor,
                                     float *distancia, char *unidad_valida, FILE *fichero_errores) {

  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char encontrado = 0;
  char numero_fichero[10];
  estructura_unidades *grupo_unidades;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;


  Vector_semifonema_candidato *correcaminos, *apunta_escogido = NULL, *apunta_original = NULL;

  izquierdo_derecho == IZQUIERDO ? grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
										IZQUIERDO, locutor):
    grupo_unidades = devuelve_puntero_a_unidades(alofono1, alofono2,
						 DERECHO, locutor);
  if (grupo_unidades == NULL) {
    fprintf(stderr, "La unidad %s.%s no se encuentra en el Corpus.\n", alofono1, alofono2);
    return 1;
  } // if (grupo_unidades == NULL)

  correcaminos = grupo_unidades->unidades;

  for (int contador = 0; contador < grupo_unidades->numero_unidades; contador++, correcaminos++) {

    if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
      continue;
            
    if (correcaminos->identificador == identificador_escogido) {
      apunta_escogido = correcaminos;
      encontrado++;
    } // if (correcaminos->identificador ...
    if (correcaminos->identificador == identificador_original) {
      apunta_original = correcaminos;
      encontrado++;
    } // if (correcaminos->identificador ...
    if (encontrado == 2)
      break;
  } // for (int contador = 0; ...

  if (encontrado != 2) {
    if (apunta_original == NULL) {
      *unidad_valida = 0;
      fprintf(fichero_errores, "%d\t%s %s\n", identificador_original, alofono1, alofono2);
      return 0;
    } // if (apunta_original == NULL
    if (apunta_escogido == NULL) {
      fprintf(stderr, "Error: %d\t%s %s\n", identificador_original, alofono1, alofono2);
      return 1;
    } // if (apunta_escogido == NULL)
  } // if (encontrado != 2)

  *unidad_valida = 1;

  if (adapta_identificador(cadena_origen_unidades[identificador_original], numero_fichero))
    return 1;

  if (devuelve_ventanas_cepstrum_unidad(apunta_original, path_cepstrum,
                                        numero_fichero,
                                        &cepstrum_objetivo, &numero_ventanas_objetivo,
                                        locutor->frecuencia_muestreo, 1))
    return 1;

  if (adapta_identificador(cadena_origen_unidades[identificador_escogido], numero_fichero))
    return 1;

  if (devuelve_ventanas_cepstrum_unidad(apunta_escogido, path_cepstrum,
                                        numero_fichero,
                                        &cepstrum_candidato, &numero_ventanas_candidato,
                                        locutor->frecuencia_muestreo, 1))
    return 1;

  if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
				       cepstrum_candidato, numero_ventanas_candidato,
				       Vector_semifonema_candidato::numero_coeficientes_cepstrales,
				       distancia))
    return 1;

  borra_cache_distancias();

  return 0;

}


/**
 * Función:   calcula_histogramas_semifonemas
 * \remarks Entrada:
 *			- origen_unidades: cadena en la que se encuentra indexado el
 *            número del fichero del Corpus del que se extrajo cada unidad.
 *			- corpus: vector<string> con la extensión relativa a cada parte del corpus. 
 *          - path_cepstrum: ruta (incluyendo el inicio del nombre del fichero)
 *            de los ficheros con los vectores cepstrales precalculados.
 *          - nombre_fichero: inicio del nombre de los ficheros de salida.
 *          - locutor: objeto de tipo Locutor en el que se almacena el Corpus de
 *			  voz.
 *			- opcion_salida: Si vale 0, sólo se escriben las distancias. En otro
 *			  caso, se escriben los identificadores de las unidades comparadas y
 *			  y las distancias.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Escribe en un fichero las distancias cepstrales medias entre todas
 *			  las unidades del mismo nombre.
 */

int calcula_histogramas_semifonemas(map<int, int> &origen_unidades, vector<string> &corpus, char path_cepstrum[],
                                    char *nombre_fichero, Locutor *locutor,
                                    char opcion_salida) {

  int indice1, indice2;
  int cuenta_vectores1, cuenta_vectores2;
  int numero_ventanas_objetivo, numero_ventanas_candidato;
  char izquierdo_derecho;
  char path_cepstrum_local[FILENAME_MAX];
  char nombre[FILENAME_MAX];
  char solo_un_cero, fichero_abierto;
  float distancia;
  int maximo_indice;
  char numero_fichero[10];
  FILE *fichero;
  Vector_cepstrum_tiempo *cepstrum_objetivo, *cepstrum_candidato;
  estructura_unidades *grupo_unidades;

  Vector_semifonema_candidato *correcaminos, *recorre;

  maximo_indice = locutor->primera_unidad_corpus500;
    
  for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
    for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
      for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	if (izquierdo_derecho == '0')
	  grupo_unidades = locutor->tabla_semifonemas_i[indice1][indice2];
	else
	  grupo_unidades = locutor->tabla_semifonemas_d[indice1][indice2];

	if (grupo_unidades == NULL)
	  continue;

	correcaminos = grupo_unidades->unidades;

	borra_cache_distancias();

	//                reinicia_vector_distancia();

	solo_un_cero = 0;
	fichero_abierto = 0;

	for (cuenta_vectores1 = 0; cuenta_vectores1 < grupo_unidades->numero_unidades;
	     cuenta_vectores1++, correcaminos++) {

	  if (correcaminos->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	    continue;
                        
	  if (correcaminos->identificador < maximo_indice)
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());
	  else
	    sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());

	  if (adapta_identificador(origen_unidades[correcaminos->identificador], numero_fichero))
	    return 1;

	  if (devuelve_ventanas_cepstrum_unidad(correcaminos, path_cepstrum_local,
						numero_fichero,
						&cepstrum_objetivo, &numero_ventanas_objetivo,
						locutor->frecuencia_muestreo, 1))
	    return 1;


	  recorre = grupo_unidades->unidades;

	  for (cuenta_vectores2 = 0; cuenta_vectores2 < grupo_unidades->numero_unidades;
	       cuenta_vectores2++, recorre++) {


	    if (recorre->identificador >= PRIMER_IDENTIFICADOR_DIFONEMAS)
	      continue;
                            
	    if (recorre->identificador >= maximo_indice)
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[1].c_str());
	    else
	      sprintf(path_cepstrum_local, "%s%s_", path_cepstrum, corpus[0].c_str());

	    if (adapta_identificador(origen_unidades[recorre->identificador], numero_fichero))
	      return 1;

	    if (devuelve_ventanas_cepstrum_unidad(recorre, path_cepstrum_local,
						  numero_fichero,
						  &cepstrum_candidato, &numero_ventanas_candidato,
						  locutor->frecuencia_muestreo, 1))
	      return 1;

	    if (calcula_distancia_cepstral_media(cepstrum_objetivo, numero_ventanas_objetivo,
						 cepstrum_candidato, numero_ventanas_candidato,
						 Vector_semifonema_candidato::numero_coeficientes_cepstrales,
						 &distancia))
	      return 1;


	    if (distancia == 0)
	      if (solo_un_cero == 1)
		continue;
	      else
		solo_un_cero = 1;

	    if (fichero_abierto == 0) {

	      strcpy(nombre, nombre_fichero);

	      crea_nombre_fichero(indice1, indice2, izquierdo_derecho, nombre, ".hist");

	      if ( (fichero = fopen(nombre, "wt")) == NULL) {
		fprintf(stderr, "Error en calcula_histogramas_semifonemas, al intentar abrir el \
                                        fichero %s.\n", nombre_fichero);
		return 1;
	      } // if ( (fichero = fopen(nombre, ...

	      fichero_abierto = 1;

	    } // if (fichero_abierto == 0)

	    if (opcion_salida == 0)
	      fprintf(fichero, "%f\n", distancia);
	    else
	      fprintf(fichero, "%d\t%d\t%f\n", correcaminos->identificador,
		      recorre->identificador,
		      distancia);


	  } // for (cuenta_vectores2 = 0; ...

	} // for (cuenta_vectores1 = 0; ...

	// Ahora tenemos que escribir en el fichero los valores de las subfunciones de
	// coste para la regresión lineal.
	// El número de valores que tomamos de cada grupo dependerá de su propio tamaño.

	/*                if (devuelve_vector_distancia(&original_distancia, &numero_distancias))
			  continue;

			  if (numero_distancias == 0)
			  continue;
	*/
	if (fichero)
	  fclose(fichero);
	fichero = NULL;
      } // for (izquierdo_derecho = 0; ...    borra_cache_distancias();

  return 0;

}


#endif

/**
 * \brief	Función que encuentra el semifonema de la base que se acerca más al contexto
 * fonético de la unidad objetivo.
 * \param[in]	objetivo	vector con el contexto fonético deseado.
 * \param[in]	locutor	objeto de tipo Locutor con la información del corpus de voz.
 * \param[in]	izq_der	izquierdo o derecho.
 * \retval	candidato	candidato de la base que se encuentra más próximo al contexto
 * fonético deseado.
 * \return	Si se encuentra alguna unidad, devuelve un cero.
 */

int encuentra_candidato_con_coarticulacion_adecuada(Vector_descriptor_objetivo &objetivo, Locutor *locutor, unsigned char izq_der, Vector_semifonema_candidato **candidato) {

  char central[3*OCTETOS_POR_FONEMA], derecho[2*OCTETOS_POR_FONEMA],
    izquierdo[OCTETOS_POR_FONEMA];
  estructura_unidades *unidad_seleccionada;
  Vector_semifonema_candidato *apunta_candidato, *mejor_candidato;

  objetivo.devuelve_fonemas(central, izquierdo, derecho);

  izq_der == IZQUIERDO ? unidad_seleccionada = devuelve_puntero_a_unidades(izquierdo, central,
									   IZQUIERDO, locutor):
    unidad_seleccionada = devuelve_puntero_a_unidades(central,derecho,
						      DERECHO, locutor);


  if (unidad_seleccionada == NULL) {
    *candidato = NULL;
    return 1;
  }

  mejor_candidato = unidad_seleccionada->unidades;
  apunta_candidato = unidad_seleccionada->unidades + 1;

  for (int contador = 1; contador <  unidad_seleccionada->numero_unidades; 
       contador++, apunta_candidato++) {
    
    if (izq_der == IZQUIERDO) {

      if (is_silence_or_voiceless_plosive(apunta_candidato->contexto_derecho) ==
	  is_silence_or_voiceless_plosive(objetivo.contexto_derecho) ) {
	    
	if (is_silence_or_voiceless_plosive(mejor_candidato->contexto_derecho) !=
	    is_silence_or_voiceless_plosive(objetivo.contexto_derecho) )
	  mejor_candidato = apunta_candidato;
	else
	  
	  if (strcmp(apunta_candidato->contexto_derecho, objetivo.contexto_derecho) == 0) {
	    
	    if (strcmp(mejor_candidato->contexto_derecho, objetivo.contexto_derecho))
	      mejor_candidato = apunta_candidato;
	    else
	      
	      if (apunta_candidato->frontera_final == objetivo.frontera_final) {
		
		if (mejor_candidato->frontera_final != objetivo.frontera_final)
		  mejor_candidato = apunta_candidato;
		else
		  if (apunta_candidato->frontera_inicial == objetivo.frontera_inicial) {
		    
		    if (mejor_candidato->frontera_inicial != objetivo.frontera_inicial)
		      mejor_candidato = apunta_candidato;
		    else
		      if (is_silence_or_voiceless_plosive(apunta_candidato->contexto_derecho_2) ==
			  is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) ) {

			if (is_silence_or_voiceless_plosive(mejor_candidato->contexto_derecho_2) != 
			    is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) )
			  mejor_candidato = apunta_candidato;
			else
			  if (strcmp(apunta_candidato->contexto_izquierdo_2, 
				     objetivo.contexto_izquierdo_2) == 0) {
			    if (strcmp(mejor_candidato->contexto_izquierdo_2, 
				       objetivo.contexto_izquierdo_2))
			      mejor_candidato = apunta_candidato;
			    else {
			      if (strcmp(apunta_candidato->contexto_derecho_2, 
					 objetivo.contexto_derecho_2) == 0) {
				if (strcmp(mejor_candidato->contexto_derecho_2, 
					   objetivo.contexto_derecho_2))
				  mejor_candidato = apunta_candidato;
				else
				  if ( (apunta_candidato->acento == objetivo.acento) &&
				       (mejor_candidato->acento != objetivo.acento) )
				    mejor_candidato = apunta_candidato;
			      }
			      else {
				if (strcmp(apunta_candidato->contexto_derecho_2, 
					   objetivo.contexto_derecho_2))
				  if ( (apunta_candidato->acento == objetivo.acento) &&
				       (mejor_candidato->acento != objetivo.acento) )
				    mejor_candidato = apunta_candidato;
			      }
			    }
			  }
			  else {
			    if (strcmp(apunta_candidato->contexto_izquierdo_2, 
				       objetivo.contexto_izquierdo_2))
			      if ( (apunta_candidato->acento == objetivo.acento) &&
				   (mejor_candidato->acento != objetivo.acento) )
				mejor_candidato = apunta_candidato;
			  }
		      }
		  }
	      }
	  }
      }	  
    }
    else { // DERECHO
      
      if (is_silence_or_voiceless_plosive(apunta_candidato->contexto_izquierdo) == 
	  is_silence_or_voiceless_plosive(objetivo.contexto_izquierdo) ) {

	if (is_silence_or_voiceless_plosive(mejor_candidato->contexto_izquierdo) != 
	    is_silence_or_voiceless_plosive(objetivo.contexto_izquierdo) )
	  mejor_candidato = apunta_candidato;
	else
	  if (strcmp(apunta_candidato->contexto_izquierdo, objetivo.contexto_izquierdo) == 0) {
	
	    if (strcmp(mejor_candidato->contexto_izquierdo, objetivo.contexto_izquierdo))
	      mejor_candidato = apunta_candidato;
	    else
	      if (apunta_candidato->frontera_final == objetivo.frontera_final) {
		if (mejor_candidato->frontera_final != objetivo.frontera_final)
		  mejor_candidato = apunta_candidato;
		else
		  if (apunta_candidato->frontera_inicial == objetivo.frontera_inicial) {
		    if (mejor_candidato->frontera_inicial != objetivo.frontera_inicial)
		      mejor_candidato = apunta_candidato;
		    else
		      if (is_silence_or_voiceless_plosive(apunta_candidato->contexto_derecho_2) ==
			  is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) ) {
			if (is_silence_or_voiceless_plosive(mejor_candidato->contexto_derecho_2) !=
			    is_silence_or_voiceless_plosive(objetivo.contexto_derecho_2) )
			  mejor_candidato = apunta_candidato;
			else
			  if (strcmp(apunta_candidato->contexto_izquierdo_2, 
				     objetivo.contexto_izquierdo_2) == 0) {
			    if (strcmp(mejor_candidato->contexto_izquierdo_2, 
				       objetivo.contexto_izquierdo_2))
			      mejor_candidato = apunta_candidato;
			    else {
			      if (strcmp(apunta_candidato->contexto_derecho_2, 
					 objetivo.contexto_derecho_2) == 0) {
				if (strcmp(mejor_candidato->contexto_derecho_2, 
					   objetivo.contexto_derecho_2))
				  mejor_candidato = apunta_candidato;
				else
				  if ( (apunta_candidato->acento == objetivo.acento) &&
				       (mejor_candidato->acento != objetivo.acento) )
				    mejor_candidato = apunta_candidato;
			      }
			      else {
				if (strcmp(apunta_candidato->contexto_derecho_2, 
					   objetivo.contexto_derecho_2))
				  if ( (apunta_candidato->acento == objetivo.acento) &&
				       (mejor_candidato->acento != objetivo.acento) )
				    mejor_candidato = apunta_candidato;
			      }
			    }
			  }
			  else {
			    if (strcmp(apunta_candidato->contexto_izquierdo_2, 
				       objetivo.contexto_izquierdo_2))
			      if ( (apunta_candidato->acento == objetivo.acento) &&
				   (mejor_candidato->acento != objetivo.acento) )
				mejor_candidato = apunta_candidato;
			  }
		      }
		  }
	      }
	  }
      }
    }
  }
  // Comprobamos que se respeten al menos los fonemas anterior y posterior y las fronteras:

  if (strcmp(mejor_candidato->contexto_izquierdo, objetivo.contexto_izquierdo) ||
      strcmp(mejor_candidato->contexto_derecho, objetivo.contexto_derecho) ||
      (mejor_candidato->frontera_final != objetivo.frontera_final) ||
      (mejor_candidato->frontera_inicial != objetivo.frontera_inicial) ) {
    *candidato = NULL;
    return 1;
  }
        
  *candidato = mejor_candidato;
    
  return 0;

}


/**
 * \brief	Función que carga las unidades con las que se desea sintetizar una frase
 * \param[in]	nombre_fichero	Nombre del fichero con las unidades.
 * \param[out]	vectores_sintesis  Cadena de vectores de tipo Vector_sintesis.
 * \param[out]	numero_vectores_sintesis Número de elementos de la cadena anterior
 * \param[out]	vectores_objetivo Cadena de vectores de tipo Vector_descriptor_objetivo
 * \param[out]	numero_vectores_objetivo	Número de elementos de la cadena anterior
 * \return	0 en ausencia de error.
 */
int crea_vectores_sintesis_desde_fichero(char *nombre_fichero, Vector_sintesis **vectores_sintesis, int *numero_vectores_sintesis, vector<Vector_descriptor_objetivo> &vectores_objetivo, int *numero_vectores_objetivo) {

  int memoria_reservada_vectores_sintesis = 1000;
  int numero_real_sintesis = 0;
  int numero_real_objetivo = 0;
  Vector_descriptor_objetivo vector_objetivo;
  Vector_descriptor_candidato vector;
  Vector_semifonema_candidato vector_semifonema;

  FILE *fichero_entrada;

  if ( (fichero_entrada = fopen(nombre_fichero, "rb")) == NULL) {
    fprintf(stderr, "Error en crea_vectores_sintesis_desde_fichero, al intentar abrir el fichero %s.\n", nombre_fichero);
    return 1;
  } // if ( ( fichero_entrada = ...

  *numero_vectores_sintesis = 0;

  if ( (*vectores_sintesis = (Vector_sintesis *) malloc(memoria_reservada_vectores_sintesis*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en crea_vectores_sintesis_desde_fichero, al asignar memoria.\n");
    return 1;
  }

  vectores_objetivo.clear();
  vectores_objetivo.reserve(100);

  Vector_descriptor_candidato::lee_cabecera(fichero_entrada);
  Vector_semifonema_candidato::numero_coeficientes_cepstrales = Vector_descriptor_candidato::numero_coeficientes_cepstrales;

  while (vector.lee_datos_bin(fichero_entrada) == 0) {

    // Primero la parte izquierda:

    if (vector_semifonema.crea_semifonema_candidato(&vector, IZQUIERDO))
      return 1;

    vector_semifonema.libera_memoria_menos_marcas_pitch();

    if (++numero_real_sintesis >= memoria_reservada_vectores_sintesis) {

      memoria_reservada_vectores_sintesis += 100;

      if ( (*vectores_sintesis = (Vector_sintesis *) realloc(*vectores_sintesis,
							     memoria_reservada_vectores_sintesis*sizeof(Vector_sintesis)))
	   == NULL) {
	fprintf(stderr, "Error en crea_vectores_sintesis_desde_fichero, al reasignar memoria.\n");
	return 1;
      }

    } // if (++numero_real_sintesis >= memoria_reservada_vectores_sintesis)

    if ((*vectores_sintesis)[numero_real_sintesis - 1].crea_vector_sintesis(&vector_semifonema, IZQUIERDO, 0))
      return 1;

    // Luego la derecha:

    if (vector_semifonema.crea_semifonema_candidato(&vector, DERECHO))
      return 1;

    if ((*vectores_sintesis)[numero_real_sintesis++].crea_vector_sintesis(&vector_semifonema, DERECHO, 0))
      return 1;

    vector_semifonema.libera_memoria_menos_marcas_pitch();

    // Por último, creamos el vector objetivo:

    vector_objetivo.crea_vector_objetivo(&vector);
    vectores_objetivo.push_back(vector_objetivo);
    numero_real_objetivo++;

    // Liberamos la memoria que no nos interesa:
    vector.libera_memoria();

  } // while (vector.lee_datos_bin...)

  fclose(fichero_entrada);

  *numero_vectores_sintesis = numero_real_sintesis;

  *numero_vectores_objetivo = numero_real_objetivo;

  return 0;

}
