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
#include <limits.h>
#include <math.h>

#include "modos.hpp"
#ifdef _WIN32
#include <windows.h>
#endif
#include "audio.hpp"
#include "tip_var.hpp"
//#include "lib_m_pro.hpp"
#include "fonemas.hpp"
#include "info_corpus_voz.hpp"
#include "descriptor.hpp"
#include "matriz.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
//#include "difonemas.hpp"
#include "procesado_senhal.hpp"

//#define _DEBUG_PROCESADO
//#define _ESCRIBE_WAV

/**
 * \author 
 * \remarks 
 *
 * \param paudio  
 * \param pwav  
 * \param pf8  
 * \param fichero_sal  
 * \param dir_salida  
 * \param fichero_entrada  
 *
 * \return 
 */

void Procesado_senhal::inicializa(bool paudio, bool pwav, bool pf8, int fm, char * fichero_sal,
				  char * dir_salida, char * fichero_entrada, Audio * pdispositivo_audio){

  audio = paudio;
  buffer = NULL; // Buffer para el solapamiento de los segmentos de voz sintética.
  datos = NULL;
  vent = NULL;

  datos_audio = (char *)malloc(fm * BYTES_MUESTRA * TIEMPO_BUF / 1000);

  memoria_reservada = 0;

  num_dat_fwav=0;
  frecuencia_muestreo = fm;

  dispositivo_audio = pdispositivo_audio;
  wav = pwav;
  f8 = pf8;
  fsalida = fichero_sal;
  fwav = NULL;

  dir_sal = dir_salida;
  fentrada = fichero_entrada;

}

/**
 * \author 
 * \remarks 
 *
 * \param paudio  
 * \param pwav  
 * \param pf8  
 * \param fichero_sal  
 * \param dir_salida  
 * \param fichero_entrada  
 *
 * \return 
 */

void Procesado_senhal::reinicia(bool paudio, bool pwav, bool pf8, int fm, char * fichero_sal,
				char * dir_salida, char * fichero_entrada){

  if (datos_audio == NULL){
    datos_audio = (char *)malloc(fm * BYTES_MUESTRA * TIEMPO_BUF / 1000);
  }

  num_dat_fwav=0;
  frecuencia_muestreo = fm;
  audio = paudio;
  wav = pwav;
  f8 = pf8;
  fsalida = fichero_sal;
  fwav = NULL;

  dir_sal = dir_salida;
  fentrada = fichero_entrada;

}


/**
 * Función:   genera_senhal_sintetizada                                        
 * \remarks Entrada:
 *	    - cadena_sintesis: cadena indexada con las unidades escogidas para 
 *            la síntesis de la frase de entrada.                              
 *          - objetivo: cadena indexada con las estimaciones prosódicas de     
 *            Cotovía.                                                         
 *          - numero_unidades: número de unidades de la cadena de síntesis.    
 *          - modif_prosod: Si es distinto de 0, se modifica la prosodia de    
 *            cada unidad para adaptarlo a las estimaciones de Cotovía.        
 *            Si vale 1, se realiza siempre modificación prosódica. Si vale 2,
 * 			  no se modifica la prosodia, pero no se copia directamente la forma
 *			  de onda de unidades consecutivas. En otro caso, sólo se realiza
 *			  cuando es necesario, en función de los umbrales que hay en Viterbi.hpp.
 *			- minpitch: pitch mínimo del locutor, en muestras.				   
 *			- maxpitch: pitch máximo del locutor, en muestras.				   
 *			- fichero_sonido: fichero de sonido                                
 *			- ficheros_segmentacion: si vale 0, no se generan los ficheros con 
 *			  la información de la segmentación fonética y las marcas de 	   
 *			  síntesis de la voz sintética. En otro caso sí que se generan.    	
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error devuelve un cero.                           
 */

int Procesado_senhal::genera_senhal_sintetizada(Vector_sintesis *cadena_sintesis, vector<Vector_descriptor_objetivo> &objetivo, int numero_unidades, unsigned char modif_prosod, int minpitch, int maxpitch, FILE * fichero_sonido, char ficheros_segmentacion) {

  int total_datos;
  int unidades_consecutivas = 0;
  vector<Vector_descriptor_objetivo>::iterator  puntero_objetivo = objetivo.begin();
  Vector_sintesis *puntero_sintesis = cadena_sintesis;
  short int voz[TAM_VECTOR];
  int contador;
  int solape = 0;
  int tam_unit_sint, nmost_buf = 0;
  unsigned int long_datos_audio;
  //int ultimo_pitch_sintesis = INT_MAX;
  int ultimo_pitch_sintesis = 0;
  int anterior_pitch_analisis = INT_MAX;
  //    char datos_audio[TAM_BUF];
  int cuenta;
  char lleno = 0;
  char nombre_fichero[FILENAME_MAX];

  tamano_media_ventana = FACTOR*maxpitch + 1;
  tamano_ventana = 2*tamano_media_ventana;

  if (memoria_reservada < tamano_ventana) {

    memoria_reservada = tamano_ventana;

    if (buffer != NULL)
      free(buffer);

    if ( (buffer = (short int *) malloc(tamano_ventana*sizeof(short int)))
	 == NULL) {
      fprintf(stderr, "Error en genera_senhal_sintetizada, al asignar memoria.\n");
      return 1;
    } // if ( (buffer = ...

    memset(buffer, 0, tamano_ventana*sizeof(short int));

    if (datos != NULL)
      free(datos);

    if ( (datos = (short int *) malloc(tamano_ventana*sizeof(short int)))
	 == NULL) {
      fprintf(stderr, "Error en sintesis_unidad, al asignar memoria.\n");
      return 1;
    } // if ( (datos = ...

    if (vent != NULL)
      free(vent);

    if ( (vent = (float *) malloc(tamano_ventana*sizeof(float)))
	 == NULL) {
      fprintf(stderr, "Error en sintesis_unidad, al asignar memoria.\n");
      return 1;
    } // if ( (vent = ...


  } // if (memoria_reservada < tamano_ventana)

  FILE *fichero = NULL, *fichero_marcas = NULL;
  int cuenta_marcas;
  int posicion_primera_marca_sintesis;
  static int posicion_primera_marca_sintesis_unidad_anterior = 0;
  static int tiempo_anterior = 0, tiempo_anterior_pitch = 0, primera_frase = 1;
  char fichero_etiqueta[FILENAME_MAX], *apunta_nombre, *apunta_nombre_sin_ruta;

  if (ficheros_segmentacion) {

#ifdef _WIN32
    apunta_nombre_sin_ruta = strrchr(fentrada, '\\');
#else
    apunta_nombre_sin_ruta = strrchr(fentrada, '/');
#endif

    if (apunta_nombre_sin_ruta == NULL)
      apunta_nombre_sin_ruta = fentrada;

    sprintf(fichero_etiqueta, "%s%s", dir_sal, apunta_nombre_sin_ruta);

    apunta_nombre = strstr(fichero_etiqueta, ".txt");
    if (apunta_nombre != NULL)
      strcpy(apunta_nombre, ".eti");
    else
      strcat(fichero_etiqueta, ".eti");

    if (tiempo_anterior == 0) {
      if ( (fichero = fopen(fichero_etiqueta, "wt")) == NULL) {
	fprintf(stderr, "Error en genera_senhal_sintetizada, al intentar abrir el fichero %s.\n", fichero_etiqueta);
	return 1;
      } // if ( (fichero = fopen(...
    } // if (tiempo_anterior == 0)
    else  {
      if ( (fichero = fopen(fichero_etiqueta, "at")) == NULL) {
	fprintf(stderr, "Error en genera_senhal_sintetizada, al intentar abrir el fichero %s.\n", fichero_etiqueta);
	return 1;
      } // if ( (fichero = fopen(...
      primera_frase = 0;
    } // else
    sprintf(nombre_fichero, "%s%s", dir_sal, apunta_nombre_sin_ruta);
    apunta_nombre = strstr(nombre_fichero, ".txt");

    if (apunta_nombre != NULL)
      strcpy(apunta_nombre, ".sint");
    else
      strcat(nombre_fichero, ".sint");

    if (tiempo_anterior == 0) {
      if ( (fichero_marcas = fopen(nombre_fichero, "wt")) == NULL) {
	fprintf(stderr, "Error en genera_senhal_sintetizada, al intentar abrir el fichero %s.\n", nombre_fichero);
	return 1;
      } // if ( (fichero = fopen(...
    } // if (tiempo_anterior == 0)
    else
      if ( (fichero_marcas = fopen(nombre_fichero, "at")) == NULL) {
	fprintf(stderr, "Error en genera_senhal_sintetizada, al intentar abrir el fichero %s.\n", nombre_fichero);
	return 1;
      } // if ( (fichero = fopen(...

  } // if (ficheros_segmentacion)

  //#endif


  if (wav) {
    if (fwav==NULL) {
      strcpy(nombre_fichero,fsalida);
      strcat(nombre_fichero,".wav");
      if ((fwav=fopen(nombre_fichero,"wb"))==NULL) {
	fprintf(stderr, "No se puede crear %s.\n",nombre_fichero);
	return 1;
      }
      if (f8) {
	crear_cab_wav(fwav,8000);
      }
      else {
	crear_cab_wav(fwav,frecuencia_muestreo);
      }
    }
  }

  //    memset(buffer, 0, tamano_ventana*sizeof(short int));


  for (contador = 0; contador < numero_unidades; contador += unidades_consecutivas) {

#ifdef _DEBUG_PROCESADO
    fprintf(stderr,"\n%d %s (+%d) (%s): anterior_pitch:%d ultimo_pitch:%d\n",puntero_sintesis->identificador, puntero_sintesis->semifonema, unidades_consecutivas-1, puntero_objetivo->palabra, puntero_sintesis->anterior_pitch, puntero_sintesis->ultimo_pitch);
    fprintf(stderr, "MA= %d", 	puntero_sintesis->marcas_analisis[0]);
    for (int i = 1; i < puntero_sintesis->numero_marcas_analisis; i++) {
      fprintf(stderr, " %d", 	puntero_sintesis->marcas_analisis[i]);
    }
    fprintf(stderr, "\n");
#endif
    switch (modif_prosod) {

    case 0:
    case 2:
      if (puntero_sintesis->semifonema[0] == '#') {
	if (puntero_sintesis->validez & 3) {
	  if (ultimo_pitch_sintesis){
	    if (puntero_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
	      puntero_objetivo->frecuencia_izquierda = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	    }
	    else {
	      puntero_objetivo->frecuencia_mitad = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	    }
	  }
	  if (modifica_prosodia_corpus(*puntero_objetivo, puntero_sintesis, 3))
	    return 1;
	}
	else {
	  if (prosodia_original(puntero_sintesis))
	    return 1;
	}
      }
      else {
	if (prosodia_original(puntero_sintesis))
	  return 1;
      }
      break;
    case 1:
      if (ultimo_pitch_sintesis){
	if (puntero_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
	  puntero_objetivo->frecuencia_izquierda = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	}
	else {
	  puntero_objetivo->frecuencia_mitad = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	}
      }
      if (modifica_prosodia_corpus(*puntero_objetivo, puntero_sintesis, 3))
	return 1;
      break;
    case 4:
      if (prosodia_original(puntero_sintesis))
	return 1;
      break;
    default:
      if (puntero_sintesis->validez & 3) { // Si hay que modificar f0 o duración...
	if (ultimo_pitch_sintesis){
	  if (sordo_sonoro(puntero_objetivo->contexto_izquierdo) == SONORO) {
	    if (puntero_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
	      puntero_objetivo->frecuencia_izquierda = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	    }
	    else {
	      puntero_objetivo->frecuencia_mitad = (float)frecuencia_muestreo / ultimo_pitch_sintesis;
	    }
	  } // if (sordo_sonoro(puntero_objetivo...
	} // if (ultimo_pitch_sintesis)
	if (modifica_prosodia_corpus(*puntero_objetivo, puntero_sintesis, puntero_sintesis->validez))
	  return 1;
      }
      else
	if (prosodia_original(puntero_sintesis))
	  return 1;
    }

    memset(voz, 0, TAM_VECTOR*sizeof(short int));

		if ( ( (puntero_sintesis->validez & 3) || (modif_prosod == 1) || (modif_prosod == 4) ) && ( ( (modif_prosod != 0) && (modif_prosod != 2) ) || (puntero_sintesis->semifonema[0] == '#') ) )
			//    if ( ( puntero_sintesis->validez & 3 || modif_prosod == 1 || modif_prosod == 4 ) && ( modif_prosod != 0 && modif_prosod != 2 ) ) 
		{
      unidades_consecutivas = 1;

      if (sintesis_unidad(puntero_sintesis, minpitch, maxpitch, voz, &total_datos,
			  &ultimo_pitch_sintesis, &anterior_pitch_analisis,
			  &posicion_primera_marca_sintesis, fichero_sonido) != 0){
	return 1;
      }

      if (ficheros_segmentacion) {
	//				if ( (contador > 1) || (primera_frase == 0) ){ // > 1
	//					tiempo_anterior += (posicion_primera_marca_sintesis - posicion_primera_marca_sintesis_unidad_anterior);
	//				}
	//				else {
	//					if (contador == 1){ // == 1
	tiempo_anterior += posicion_primera_marca_sintesis;
	//					}
	//				}
	//if ( (contador == 0) && (tiempo_anterior_pitch == 0) ){
	tiempo_anterior_pitch += posicion_primera_marca_sintesis;
	//}
	//else {
	//tiempo_anterior_pitch += (posicion_primera_marca_sintesis - posicion_primera_marca_sintesis_unidad_anterior);
	//}
#ifdef _DEBUG_PROCESADO
	fprintf(stderr,"tiempo_anterior:%d tiempo_anterior_pitch:%d posicion_primera_marca_sintesis_unidad_anterior:%d\nMarcas:",tiempo_anterior,tiempo_anterior_pitch, posicion_primera_marca_sintesis_unidad_anterior);
#endif

	for (cuenta_marcas = 0; cuenta_marcas < puntero_sintesis->numero_marcas_sintesis;
	     cuenta_marcas++) {

	  fprintf(fichero_marcas, "%g\t", ((double) tiempo_anterior_pitch +
					   (double) puntero_sintesis->marcas_sintesis[cuenta_marcas])
		  / frecuencia_muestreo);
	  fprintf(fichero_marcas, "%s\n", puntero_objetivo->semifonema);
#ifdef _DEBUG_PROCESADO
	  fprintf(stderr," %d",tiempo_anterior_pitch+puntero_sintesis->marcas_sintesis[cuenta_marcas]); 
#endif

	} // for (cuenta_marcas  = 0; ...
#ifdef _DEBUG_PROCESADO
	fprintf(stderr,"\n");
#endif

      } // if (ficheros_segmentacion)

    } // if (puntero_sintesis->validez & 3)
    else {

      Vector_sintesis *proximo_vector = puntero_sintesis + 1;
      cuenta = contador;
      int indice_inicial = puntero_sintesis->indice;
      int tamano_actual;
      char izq_der_anterior = puntero_sintesis->izquierdo_derecho;

      int anterior_identificador = puntero_sintesis->identificador;

      while ( (cuenta < numero_unidades - 1) &&
	      (modif_prosod != 2) && 
	      ( (proximo_vector->semifonema[0] != '#') ||
		( (proximo_vector->semifonema[0] == '#') && ( (proximo_vector->validez & 3) == 0) ) )  &&
	      ( ((proximo_vector->validez & 3) == 0) || (modif_prosod == 0) ) &&
	      ( (anterior_identificador == proximo_vector->identificador - 1) ||
		( (anterior_identificador == proximo_vector->identificador) &&
		  ( (izq_der_anterior == IZQUIERDO) && (proximo_vector->izquierdo_derecho == DERECHO) && (puntero_sintesis->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) ) ) ) ) {

	anterior_identificador = proximo_vector->identificador;
	izq_der_anterior = proximo_vector->izquierdo_derecho;
	tamano_actual = proximo_vector->indice - indice_inicial +
	  //                		(int) (FS*proximo_vector->calcula_duracion_efectiva()) + TAM_VENTANA;
	  (int) (frecuencia_muestreo*proximo_vector->calcula_duracion_efectiva(frecuencia_muestreo)) + tamano_ventana;
	if (tamano_actual > (int) TAM_VECTOR)
	  break;
	cuenta++;
	proximo_vector++;
      } // while

      if (extrae_forma_de_onda_original(puntero_sintesis, proximo_vector - 1,
					minpitch, maxpitch, voz,
					&total_datos, &ultimo_pitch_sintesis,
					&anterior_pitch_analisis, &posicion_primera_marca_sintesis,
					fichero_sonido)){
	return 1;
      }

      if (ficheros_segmentacion) {
	tiempo_anterior_pitch += posicion_primera_marca_sintesis;
	tiempo_anterior += posicion_primera_marca_sintesis;

	//				if ( (contador > 1) || (primera_frase == 0) ) // > 1
	//					tiempo_anterior += (posicion_primera_marca_sintesis -
	//							posicion_primera_marca_sintesis_unidad_anterior);
	//				else
	//					if (contador == 1) // == 1
	//						tiempo_anterior += posicion_primera_marca_sintesis;
	//
	//				if ( (contador == 0) && (tiempo_anterior == 0) ){
	//					tiempo_anterior_pitch += posicion_primera_marca_sintesis;
	//				}
	//				else {
	//					tiempo_anterior_pitch += (posicion_primera_marca_sintesis - posicion_primera_marca_sintesis_unidad_anterior);
	//				}

      } // if (ficheros_segmentacion)

      unidades_consecutivas = cuenta - contador + 1;

    } // else


    tam_unit_sint = total_datos - ultimo_pitch_sintesis;
		
    // Atenuamos el último silencio (para evitar la posible distorsión que se puede haber introducido
    // al modificar la duración.

    if ( (puntero_objetivo->semifonema[0] == '#') && (puntero_objetivo->contexto_derecho[0] == '#') &&
	 (puntero_objetivo->contexto_derecho_2[0] == '#') &&
	 (puntero_sintesis->izquierdo_derecho == DERECHO) ) {
      short int *apunta_voz = (short int *) voz;
      float factor_energia = 0.2;
      for (int cuenta = 0; cuenta < total_datos; cuenta++, apunta_voz++)
	*apunta_voz = (short int) ( (float) *apunta_voz * factor_energia);
    }

    // Lo mismo con el primer silencio
    if ( (puntero_objetivo->semifonema[0] == '#') && (puntero_objetivo->contexto_izquierdo[0] == '#') &&
	 (puntero_objetivo->contexto_izquierdo_2[0] == '#') &&
	 (puntero_sintesis->izquierdo_derecho == IZQUIERDO) ) {
      short int *apunta_voz = (short int *) voz;
      float factor_energia = 0.2;
      for (int cuenta = 0; cuenta < total_datos; cuenta++, apunta_voz++)
	*apunta_voz = (short int) ( (float) *apunta_voz * factor_energia);
    }

    // Modificación de la energía:
    /*
      if ( (puntero_sintesis != cadena_sintesis) && (contador != numero_unidades - 1) ) {

      float factor_energia, factor_energia_db;
      float dif_1, dif_2;
      static float log_diez = log(10);
      short int *apunta_voz;
      int modificable;
      short int valor_maximo;
      int valor_maximo_posible;


      if (criterio_modificacion_energia(puntero_objetivo, contador == numero_unidades - 2, &modificable))
      return 1;

      if (modificable) {

      if ( absoluto((puntero_sintesis - 1)->potencia_final -
      puntero_sintesis->potencia_inicial) > UMBRAL_DIFERENCIA_ENERGIA)  {

      dif_1 = sqrt(exp(log_diez*( (puntero_sintesis - 1)->potencia_final - puntero_sintesis->potencia_inicial) / 10));
      dif_2 = sqrt(exp(log_diez*( puntero_sintesis->potencia_final - (puntero_sintesis + 1)->potencia_inicial) / 10));

      factor_energia = sqrt(dif_1*dif_2);

      factor_energia_db = 10*log(factor_energia);

      puntero_sintesis->potencia_inicial += factor_energia_db;
      puntero_sintesis->potencia_final += factor_energia_db;

      //           	printf("Cambio la energía del sonido %s: Original(%d): %f -> Objetivo: %f.\n",
      //                		puntero_objetivo->semifonema, puntero_sintesis->devuelve_identificador(),
      //                           puntero_sintesis->potencia, puntero_objetivo->potencia);

      apunta_voz = (short int *) voz;
      //        factor_energia = sqrt(exp(log_diez*(puntero_objetivo->potencia - puntero_sintesis->potencia) / 10));
      //					factor_energia = sqrt(pow(10,(puntero_objetivo->potencia - puntero_sintesis->potencia) / 10));

      devuelve_valor_maximo_senhal(voz, total_datos, &valor_maximo);

      valor_maximo_posible = (int) (factor_energia*valor_maximo);

      if (valor_maximo_posible > SHRT_MAX)
      factor_energia = (float) SHRT_MAX / valor_maximo - 0.2;

      for (int cuenta = 0; cuenta < total_datos; cuenta++, apunta_voz++)
      *apunta_voz = (short int) ( (float) *apunta_voz * factor_energia);

      }

      } // if (modificable)

      } // if (puntero_sintesis != cadena_sintesis)
    */
    /*
      if (modif_prosod) {

      float factor_energia;
      static float log_diez = log(10);
      short int *apunta_voz;
      int modificable;
      short int valor_maximo;
      int valor_maximo_posible;

      if (puntero_sintesis->validez & 4) { // Si hay que modificar la energía.

      if (criterio_modificacion_energia(puntero_objetivo, contador == numero_unidades - 2, &modificable))
      return 1;

      if (modificable) {

      //           	printf("Cambio la energía del sonido %s: Original(%d): %f -> Objetivo: %f.\n",
      //                		puntero_objetivo->semifonema, puntero_sintesis->devuelve_identificador(),
      //                           puntero_sintesis->potencia, puntero_objetivo->potencia);

      apunta_voz = (short int *) voz;
      factor_energia = sqrt(exp(log_diez*(puntero_objetivo->potencia - puntero_sintesis->potencia) / 10));
      //					factor_energia = sqrt(pow(10,(puntero_objetivo->potencia - puntero_sintesis->potencia) / 10));

      devuelve_valor_maximo_senhal(voz, total_datos, &valor_maximo);

      valor_maximo_posible = (int) factor_energia*valor_maximo;

      if (valor_maximo_posible > SHRT_MAX)
      factor_energia = (float) SHRT_MAX / valor_maximo;

      for (int cuenta = 0; cuenta < total_datos; cuenta++, apunta_voz++)
      *apunta_voz = (short int) ( (float) *apunta_voz * factor_energia);
      } // if (modificable)

      } // if (puntero_sintesis->validez...
      } // if (modif_prosod)

    */

#ifdef _ESCRIBE_WAV
    {
      FILE *f;
      f = fopen("voz.raw", "wb");
      fwrite(voz, sizeof(short int), total_datos, f);
      fclose(f);
    }
#endif

	
    puntero_sintesis += unidades_consecutivas;


    if (ficheros_segmentacion) {

#ifndef _ESCRIBE_CONTINUIDAD_ESPECTRAL

      fprintf(fichero, "%g\t", (double) tiempo_anterior / frecuencia_muestreo);

      if (unidades_consecutivas == 1)
	fprintf(fichero, "%s ", puntero_objetivo->semifonema);
      else
	for (cuenta_marcas = unidades_consecutivas; cuenta_marcas > 0; cuenta_marcas--)
	  fprintf(fichero, "%s ", (puntero_sintesis - cuenta_marcas)->semifonema);

      fprintf(fichero, "\n");

#else

      double tiempo_auxiliar = tiempo_anterior;
      Vector_sintesis *sintesis_auxiliar = puntero_sintesis - unidades_consecutivas;
      Vector_descriptor_objetivo *objetivo_auxiliar = puntero_objetivo;

      fprintf(fichero, "%g\t%s\n", (double) tiempo_auxiliar / frecuencia_muestreo,
	      objetivo_auxiliar->semifonema);

      for (int j = 1; j < unidades_consecutivas; j++) {

	tiempo_auxiliar += ((sintesis_auxiliar + j)->indice - (sintesis_auxiliar + j - 1)->indice) /
	  sizeof(short int);

	if ( (contador & 0x1) == 1) {
	  if ( (j & 0x1) == 1)
	    objetivo_auxiliar++;
	} // if ( (contador & 0x1) == 1)
	else
	  if ((j & 0x1) == 0)
	    objetivo_auxiliar++;

	fprintf(fichero, "%g\t%s\n", (double) tiempo_auxiliar / frecuencia_muestreo,
		objetivo_auxiliar->semifonema);

      } // for (int j = 0; ...

#endif

      //tam_unit_sint = total_datos - ultimo_pitch_sintesis;
      tiempo_anterior += tam_unit_sint - posicion_primera_marca_sintesis ;
      tiempo_anterior_pitch += tam_unit_sint - posicion_primera_marca_sintesis;

      posicion_primera_marca_sintesis_unidad_anterior = posicion_primera_marca_sintesis;

#ifdef _DEBUG_PROCESADO
      fprintf(stderr,"tiempo_anterior:%d tiempo_anterior_pitch:%d posicion_primera_marca_sintesis_unidad_anterior:%d\n",tiempo_anterior,tiempo_anterior_pitch, posicion_primera_marca_sintesis_unidad_anterior);
#endif

    } // if (ficheros_segmentacion)

#ifdef _ESCRIBE_WAV
    {
      FILE *buffer_antes = fopen("buffer_antes.raw", "wb");

      fwrite(buffer, sizeof(short int), tamano_ventana, buffer_antes);

      fclose(buffer_antes);
    }
#endif

    if (contador & 1)
      puntero_objetivo += (unidades_consecutivas / 2) + (unidades_consecutivas & 1);
      //      puntero_objetivo += (unidades_consecutivas / 2) + (unidades_consecutivas & 1);
    else
      puntero_objetivo += (unidades_consecutivas / 2);
      //      puntero_objetivo += (unidades_consecutivas / 2);

#ifdef _DEBUG_PROCESADO
    fprintf(stderr,"total_datos:%d tam_unit_sint:%d solape:%d posicion_primera_marca_sintesis:%d (%d) ultimo_pitch_sintesis:%d\n", total_datos, tam_unit_sint, solape, posicion_primera_marca_sintesis, solape-posicion_primera_marca_sintesis, ultimo_pitch_sintesis);
#endif
		
    if (tam_unit_sint > 0) {
      if (tam_unit_sint > solape){
	for (cuenta = 0; cuenta < solape; cuenta++) { // Actualizamos la zona de solape del buffer.
	  voz[cuenta] += buffer[cuenta];
	} // for (cuenta = 0;
      }
      else {
#ifdef _DEBUG_PROCESADO
	fprintf(stderr, "tam_unit_sint (%d) <= solape (%d)\n", tam_unit_sint, solape);
#endif
	for (cuenta = 0; cuenta < solape; cuenta++)
	  voz[cuenta] += buffer[cuenta];
	//for (cuenta = 0; cuenta < tamano_ventana; cuenta++)
	//  buffer[cuenta] = voz[tam_unit_sint + cuenta];
      } // else
    } // if (tam_unit_sint > 0)
    else {
#ifdef _DEBUG_PROCESADO
      fprintf(stderr, "tam_unit_sint (%d) <= 0)\n", tam_unit_sint);
#endif
      //            tam_unit_sint = total_datos;
      for (cuenta = 0; cuenta < solape; cuenta++) // Actualizamos la zona de solape del buffer.
	buffer[cuenta] += voz[cuenta];
      continue;
    } // else
    memcpy(buffer, voz+tam_unit_sint, sizeof(short int) *  ultimo_pitch_sintesis); 
    solape = ultimo_pitch_sintesis;
    // Escribimos la unidad en el dispositivo de audio.

#ifdef _ESCRIBE_WAV
    {
      FILE *f2 = fopen("voz_y_buffer.raw", "wb");
      fwrite(voz, sizeof(short int), tam_unit_sint, f2);
      fclose(f2);
    }

    {
      FILE *f3 = fopen("buffer_despues.raw", "wb");
      fwrite(buffer, sizeof(short int), tamano_ventana, f3);
      fclose(f3);
    }
#endif

    //fmendez       tam_unit_sint = total_datos - TAM_MED_VENTANA;

    //fmendez
    if (f8) {
      filtra_diezma(voz,tam_unit_sint);
      tam_unit_sint/=2;
    }

    //        fprintf(fichero_datos, "%d\n", tam_unit_sint);

    if (2*(nmost_buf + tam_unit_sint) > TAM_BUF)
      lleno=1;
    else {
      memcpy(datos_audio + 2*nmost_buf, voz, 2*tam_unit_sint);
      nmost_buf += tam_unit_sint;
    }

    if (lleno || (contador + unidades_consecutivas >= numero_unidades - 1) ) {
      long_datos_audio = 2*nmost_buf;
      if (audio)
	dispositivo_audio->EscribeAudio(datos_audio,long_datos_audio);
      if (wav){
	unsigned int aux;
	/*                  short int *kk= (short int *) datos_audio;
	 * 								 for (int pepe=0; pepe < nmost_buf; pepe++){
	 * 								 	float asdf = 1.8 *  *kk;
	 * 									if (asdf > 32767) {
	 * 											asdf = 32767;
	 * 									}
	 * 									else if (asdf < -32767){
	 * 											asdf = -32767;
	 * 									}
	 * 									*kk++ = (short) asdf;
	 * 								}
	 * 								 
	 */
	fwrite(datos_audio, sizeof(short int), nmost_buf, fwav);
	num_dat_fwav+=long_datos_audio;
	fseek(fwav,40,0);
	fwrite(&num_dat_fwav,sizeof(unsigned int),1,fwav);
	aux=num_dat_fwav+36;
	fseek(fwav,4,0);
	fwrite(&aux,sizeof(unsigned int),1,fwav);
	fseek(fwav,0,SEEK_END);
      }
      nmost_buf=0;
    }



    if (lleno) {
      lleno=0;
      memcpy(datos_audio, voz, 2*tam_unit_sint);
      nmost_buf += tam_unit_sint;
      if (contador == numero_unidades - 1) {
	long_datos_audio = 2*nmost_buf;
	if (audio)
	  dispositivo_audio->EscribeAudio(datos_audio,long_datos_audio);
	if (wav){
	  unsigned int aux;
	  /*                  short int *kk=(short int *) datos_audio;
	   * 								 for (int pepe=0; pepe < nmost_buf; pepe++){
	   * 								 	float asdf = 1.8 *  *kk;
	   * 									if (asdf > 32767) {
	   * 											asdf = 32767;
	   * 									}
	   * 									else if (asdf < -32767){
	   * 											asdf = -32767;
	   * 									}
	   * 									*kk++ = (short) asdf;
	   * 								}
	   */
	  fwrite(datos_audio, sizeof(short int), nmost_buf, fwav);
	  num_dat_fwav+=long_datos_audio;
	  fseek(fwav,40,0);
	  fwrite(&num_dat_fwav,sizeof(unsigned int),1,fwav);
	  aux=num_dat_fwav+36;
	  fseek(fwav,4,0);
	  fwrite(&aux,sizeof(unsigned int),1,fwav);
	  fseek(fwav,0,SEEK_END);
	}
      }

    }

  }

#ifdef _FUERZA_CONTORNOS_ORIGINALES
  if (fwav != NULL) {
    fclose(fwav);
    fwav = NULL;
  } // if (fwav != NULL)
#endif

  if (ficheros_segmentacion) {
    fclose(fichero);
    fclose(fichero_marcas);
  }

  return 0;

}

/**
 * Función:   libera_memoria_procesado_senhal                                  
 * \remarks Objetivo:  Libera la memoria del buffer de solapamiento, y cierra el 	   
 *			  fichero wav.													   
 */

void Procesado_senhal::libera_memoria_procesado_senhal(){

  memoria_reservada = 0;

  if (buffer != NULL) {
    free(buffer);
    buffer = NULL;
  } // if (buffer != NULL)

  if (datos != NULL) {
    free(datos);
    datos = NULL;
  } // if (datos != NULL)

  if (vent != NULL) {
    free(vent);
    vent = NULL;
  } // if (vent != NULL)

  if (fwav != NULL){
    fclose(fwav);
    fwav = NULL;
  }
  if (datos_audio != NULL){
    free(datos_audio);
    datos_audio = NULL;
  }

}
void Procesado_senhal::finaliza(){
  if (fwav != NULL){
    fclose(fwav);
    fwav = NULL;
  }
  if (datos_audio != NULL){
    free(datos_audio);
    datos_audio = NULL;
  }

}


/**
 * \brief	Función que realiza la modificación de f0 teniendo en cuenta el cambio
 * trama a trama en la señal original.
 * \param[in]	marcas_originales: array con las marcas de pitch originales.
 * \param[in]	numero_marcas: número de elementos del array anterior.
 * \param[in]	f_inicial: f0 al inicio del segmento.
 * \param[in]	f_final: f0 al final del segmento.
 * \param[out]	marcas_modificadas: array con las marcas de pitch modificadas.
 * \param[in]	tamano_maximo: memoria reservada para el array anterior.
 * \return	En ausencia de error, devuelve un cero.
 */

int Procesado_senhal::modifica_f0_no_lineal(int *marcas_originales, int numero_marcas,
					    float f_inicial, float f_final,
					    float *marcas_modificadas,
					    int tamano_maximo) {

  float variacion_objetivo, variacion_original;
  float f0, f0_anterior;

  if (numero_marcas > tamano_maximo) {
    fprintf(stderr, "Error en modifica_f0_no_lineal: número de marcas mayor que el permitido.\n");
    return 1;
  }

  if (numero_marcas == 1) {
    *marcas_modificadas = (f_inicial + f_final) / 2;
    return 0;
  }

  *marcas_modificadas = f_inicial;
  marcas_modificadas[numero_marcas - 1] = f_final;

  variacion_objetivo = absoluto(f_final - f_inicial);
  variacion_original = frecuencia_muestreo*absoluto(1.0 / (marcas_originales[numero_marcas - 1] - marcas_originales[numero_marcas - 2]) -
						    1.0 / (marcas_originales[1] - marcas_originales[0]));

  if (variacion_original == 0) {
    float x;
    float x2 = (float) marcas_originales[numero_marcas - 2]; // Cálculo de los extremos
    float x1 = (float) marcas_originales[0];    // num. muestras para hacer la
    float denominador = x2-x1;              // variación lineal de frecuencia
    marcas_modificadas[0] = f_inicial;
    marcas_modificadas[numero_marcas - 2] = f_final; // Antes [... - 2] = fb;
    for (int i = 1;i < numero_marcas - 2; i++) { // Antes (...; i < numero_marcas_analisis - 2...)
      x = (float) marcas_originales[i];
      marcas_modificadas[i] = (f_inicial*(x2-x) - f_final*(x1-x)) / denominador;
    } // for
				
    return 0;

  }

  f0_anterior = frecuencia_muestreo / (marcas_originales[1] - marcas_originales[0]);

  for (int contador = 1; contador < numero_marcas - 1; contador++) {

    f0 = frecuencia_muestreo / (marcas_originales[contador + 1] - marcas_originales[contador]);

    marcas_modificadas[contador] = marcas_modificadas[contador - 1] + (f0 - f0_anterior) / variacion_original*variacion_objetivo;

    f0_anterior = f0;

  } // for (int contador = 1...

  return 0;

}

/**
 * Función:   modifica_prosodia_corpus
 * \remarks Entrada:
 *			- objetivo: unidad cuya prosodia se quiere aproximar.
 *          - frec_tiempo: variable que indica el tiempo de modificación que
 *            se le ha de realizar a la unidad. 1 -> frecuencia, 2->tiempo,
 *            3->ambas.
 * \remarks Entrada y Salida:
 *          - unidad_sintesis: unidad cuyas marcas de síntesis se quieren
 *            calcular.
 */

int Procesado_senhal::modifica_prosodia_corpus(Vector_descriptor_objetivo &objetivo, Vector_sintesis *unidad_sintesis, unsigned char frec_tiempo) {

  float T0, T1, fa, fb, eps, x1, x2, y1, y2, x, denominador;
  float frec_auxiliares[PITCH_M_MAX];
  int marc_inter[PITCH_M_MAX];
  int *marc_sintesis, *marc_correspondientes, *marc_analisis;
  short int numero_marcas_analisis, a, b;
  int i;
  bool cambio_sonoridad;

  unidad_sintesis->devuelve_marcas_analisis(&marc_analisis, &numero_marcas_analisis);

  if ( (marc_sintesis = (int *) malloc(PITCH_M_MAX*sizeof(int))) == NULL) {
    fprintf(stderr, "Error en modifica_prosodia_corpus, al asignar memoria.\n");
    return 1;
  }

  if ( (marc_correspondientes = (int *) malloc(PITCH_M_MAX*sizeof(long  int)))
       == NULL) {
    fprintf(stderr, "Error en modifica_prosodia_corpus, al asignar memoria.\n");
    return 1;
  }

  if ( (frec_tiempo < 1) || (frec_tiempo > 7) ) {
    fprintf(stderr, "Valor de entrada no válido en modifica_prosodia_corpus.\n");
    return 1;
  }
  //frec_tiempo=1;
  if (frec_tiempo == 0) {

    for (i = 0; i < numero_marcas_analisis; i++) {
      marc_sintesis[i] = marc_analisis[i];
      marc_correspondientes[i] = i;
    }

    if (numero_marcas_analisis == 1)
      (unidad_sintesis->anterior_pitch > unidad_sintesis->ultimo_pitch) ?
	unidad_sintesis->pitch_sintesis = unidad_sintesis->ultimo_pitch:
	unidad_sintesis->pitch_sintesis = unidad_sintesis->anterior_pitch;

    unidad_sintesis->incluye_marcas_sintesis_y_correspondientes(marc_sintesis,
								marc_correspondientes, numero_marcas_analisis);
    return 0;

  }

  // Calculamos los valores objetivo.

  if (unidad_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
    fa = (int) (frecuencia_muestreo/objetivo.frecuencia_izquierda + 0.5);
    fb = (int) (frecuencia_muestreo/objetivo.frecuencia_mitad + 0.5);
    T1 = objetivo.duracion_1 * frecuencia_muestreo;
    //T1 = marc_analisis[numero_marcas_analisis - 1];
#ifdef _DEBUG_PROCESADO
    //fa*=1.5;fb*=1.5;
    //fa*=0.67;fb*=0.67;
    fprintf(stderr,"fa=%fHz(%f) fb=%fHz(%f) T1=%f(%f)\n", objetivo.frecuencia_izquierda, fa, objetivo.frecuencia_mitad, fb, objetivo.duracion_1, T1);
#endif
  }
  else {
    fa = (int) (frecuencia_muestreo/objetivo.frecuencia_mitad + 0.5);
    fb = (int) (frecuencia_muestreo/objetivo.frecuencia_derecha + 0.5);
    T1 = objetivo.duracion_2 * frecuencia_muestreo;
    //T1 = marc_analisis[numero_marcas_analisis - 1];
#ifdef _DEBUG_PROCESADO
    //fa*=1.5;fb*=1.5;
    //fa*=0.67;fb*=0.67;
    fprintf(stderr,"fa=%fHz(%f) fb=%fHz(%f) T1=%f(%f)\n", objetivo.frecuencia_mitad, fa, objetivo.frecuencia_derecha, fb, objetivo.duracion_2, T1);
#endif
  }
  //    if ( (frec_tiempo != 2) &&
  //         (marc_analisis[1] != 0) ) { // Tenemos que modificar la frecuencia fundamental.
  if ( (frec_tiempo != 2) && (sordo_sonoro(objetivo.semifonema) == SONORO) ) {
    if ( (fa == 0.0) && (fb == 0) ) // Si fa=fb=0, todas las f0's interm=0
      for (i = 0;i < numero_marcas_analisis; i++)
	frec_auxiliares[i] = 0.0;
    else  {
      if (numero_marcas_analisis == 1){
	frec_auxiliares[0] = fa; // si solo hay una marca, fa=f0 // FRAN CAMPILLO: Por lo que hace más adelante, da igual.
	// FRAN CAMPILLO: No estoy muy seguro de esta modificación. Tal y como se está haciendo ahora, la f0 al final no es
	// la deseada. Si se estuviese modificando siempre la f0 (como se hacía antes), ese valor se conseguiría en la
	// siguiente unidad, y la variación sería lineal globalmente. Sin embargo, ahora, ya no se modifica siempre la f0,
	// y me da la impresión de que en ocasiones se pierde la periodicidad. Lo cambio para que en la misma unidad se
	// garanticen esos valores de f0 al principio y al final.
	cambio_sonoridad = (sordo_sonoro(objetivo.semifonema) !=  sordo_sonoro(objetivo.contexto_derecho));
	//if (cambio_sonoridad && unidad_sintesis->izquierdo_o_derecho() == DERECHO ){
	//	frec_auxiliares[numero_marcas_analisis - 1] = fb / unidad_sintesis->anterior_pitch; 
	//}
	//else {
	frec_auxiliares[0] = fb / unidad_sintesis->ultimo_pitch; 
	//}
      }
      else {
        /*
	 *if (numero_marcas_analisis == 2) {
	 *  frec_auxiliares[0] = (fa + fb) / 2;
	 *  frec_auxiliares[0] = fa / marc_analisis[1];
	 *  if ((unidad_sintesis+1)->sonoridad){
	 *    frec_auxiliares[numero_marcas_analisis - 1] = fb / unidad_sintesis->ultimo_pitch; 
	 *  }
	 *  else {
	 *    frec_auxiliares[numero_marcas_analisis - 1] = (marc_analisis[numero_marcas_analisis - 1] - marc_analisis[numero_marcas_analisis - 2]) / fb; 
	 *  }
	 *}
	 *else { // Pero si hay mas de una, variacion lineal de f0's
         */
	//					if (modifica_f0_no_lineal(marc_analisis, numero_marcas_analisis, fa, fb, frec_auxiliares, PITCH_M_MAX))
	//					return 1;
				
	cambio_sonoridad = (sordo_sonoro(objetivo.semifonema) !=  sordo_sonoro(objetivo.contexto_izquierdo));
					
	//	if (cambio_sonoridad && unidad_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
	//		y1 = fa / marc_analisis[1];
	//		x1 = (float) marc_analisis[0]; 
	//	}
	//	else {
	y1 = fa / unidad_sintesis->anterior_pitch;
	x1 = (float) -unidad_sintesis->anterior_pitch;
	//	}

	cambio_sonoridad = (sordo_sonoro(objetivo.semifonema) !=  sordo_sonoro(objetivo.contexto_derecho));
					
	//if (cambio_sonoridad && unidad_sintesis->izquierdo_o_derecho() == DERECHO ){
	//y2 = fb / (marc_analisis[numero_marcas_analisis - 1] - marc_analisis[numero_marcas_analisis - 2]);
	//}
	//else {
	y2 = fb / unidad_sintesis->ultimo_pitch; 
	//}

	/*
	 *if (unidad_sintesis->izquierdo_o_derecho() == IZQUIERDO) {
	 *  y1 = fa / marc_analisis[1];
	 *  x1 = (float) marc_analisis[0]; 
	 *}
	 *else {
	 *  y1 = fa / unidad_sintesis->anterior_pitch;
	 *  x1 = (float) -unidad_sintesis->anterior_pitch;
	 *}
	 *
	 *if ((unidad_sintesis+1)->sonoridad){
	 *  frec_auxiliares[numero_marcas_analisis - 1] = fb / unidad_sintesis->ultimo_pitch; 
	 *  y2 = fb / unidad_sintesis->ultimo_pitch; 
	 *}
	 *else {
	 *  frec_auxiliares[numero_marcas_analisis - 1] = fb / (marc_analisis[numero_marcas_analisis - 1] - marc_analisis[numero_marcas_analisis - 2]);
	 *  y2 = fb / (marc_analisis[numero_marcas_analisis - 1] - marc_analisis[numero_marcas_analisis - 2]);
	 *}
	 */
					
	x2 = (float) marc_analisis[numero_marcas_analisis - 1]; // Cálculo de los extremos
	denominador = x2-x1;              // variación lineal de frecuencia
	/*
	 *for (i = 1;i < numero_marcas_analisis - 1; i++) { // Antes (...; i < numero_marcas_analisis - 2...)
	 *  x = (float) marc_analisis[i];
	 *  frec_auxiliares[i] = (y1*(x2-x) - y2 *(x1-x)) / denominador;
	 *} // for
	 */
	for (i = 0;i <= numero_marcas_analisis - 1; i++) { // Antes (...; i < numero_marcas_analisis - 2...)
	  x = (float) marc_analisis[i];
	  frec_auxiliares[i] = (y1*(x2-x) - y2 *(x1-x)) / denominador;
	} // for
      } // else
      //}
    } // else

    // Cálculo real de la marcas de pitch intermedias a partir de frec intermedias
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "DF= %f", frec_auxiliares[0]);
    for (int ii = 1; ii < numero_marcas_analisis; ii++) {
      fprintf(stderr, " %f", 	frec_auxiliares[ii]);
    }
    fprintf(stderr, "\n");
#endif
 
    marc_inter[0] = marc_analisis[0];

    for (i = 1; i < numero_marcas_analisis; i++) {
      //marc_inter[i] = marc_inter[i - 1] + (int) frec_auxiliares[i - 1];
      marc_inter[i] = marc_inter[i - 1] + (int) (frec_auxiliares[i-1] * (float) (marc_analisis[i] -marc_analisis[i - 1]) + 0.5);

    } // for (i = 1; i < numero_marcas_analisis
    //if (cambio_sonoridad && unidad_sintesis->izquierdo_o_derecho() == DERECHO ){
    //	unidad_sintesis->ultimo_pitch = (int) (fb + 0.5);
    //}
    //else {
    unidad_sintesis->ultimo_pitch = (int) (frec_auxiliares[numero_marcas_analisis - 1] * unidad_sintesis->ultimo_pitch + 0.5);
    //}
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "MI= %d", marc_inter[0]);
    for (int ii = 1; ii < numero_marcas_analisis; ii++) {
      fprintf(stderr, " %d", 	marc_inter[ii]);
    }
    fprintf(stderr, " -> ultimo_pitch:%d\n", unidad_sintesis->ultimo_pitch);
    //delete [] df;
#endif
  } // if (frec_tiempo...
  else // No había que modificar la frecuencia. Tenemos que actualizar las marcas intermedias.
    for (i = 0; i <  numero_marcas_analisis; i++)
      marc_inter[i] = marc_analisis[i];

  if (frec_tiempo & 0x2) {

    // Ahora tenemos que modificar la duración de la señal.

    marc_sintesis[0] = marc_inter[0];
    marc_correspondientes[0] = 0;

    // No sé por qué en el original le añaden una marca intermedia de más. En principio vamos a
    // pasar de ella.

    if (numero_marcas_analisis > 1)
      marc_inter[numero_marcas_analisis] = 2*marc_inter[numero_marcas_analisis - 1]
	- marc_inter[numero_marcas_analisis - 2];
    else
      marc_inter[numero_marcas_analisis] = marc_inter[numero_marcas_analisis - 1] + unidad_sintesis->ultimo_pitch;

    // Duración actual de la señal:

    //     T0 = (float) marc_inter[numero_marcas_analisis];
    if (numero_marcas_analisis > 1)
      T0 = (float) marc_inter[numero_marcas_analisis - 1]; // (En marcas)
    else
      T0 = (float) unidad_sintesis->anterior_pitch;


    // Pasamos la duración objetivo a muestras.

    //    T1 *= 0.001*FS;
    //        T1 *= FS;
    //T1 *= frecuencia_muestreo;
    eps = T1/T0; // Factor de compresión de marcas.

    a = b = 1;

    while (a < numero_marcas_analisis) {
      marc_sintesis[b] = marc_sintesis[b - 1] + marc_inter[a] - marc_inter[a - 1];
      if (marc_sintesis[b] <= (int) ((float)marc_inter[a]*eps + 0.5)) {
	marc_correspondientes[b] = a;
	b++;
      } // if (marc_sintesis[b] < marc_inter[a].eps)
      else
	a++;
    } // while (a < numero_marcas_analisis)

    // Ajuste final para ver si hay que incluir una ultima marca

    if (marc_correspondientes[b-1] < a - 1) { //incluir siempre la anterior al límite
      marc_sintesis[b] = marc_sintesis[b-1] + marc_inter[a - 1] - marc_inter[a - 2];
      marc_correspondientes[b] = a - 1;
      b++;
    } // if (marc_correspondientes...

    marc_sintesis[b] = marc_sintesis[b - 1] + marc_inter[a] - marc_inter[a-1];
    marc_correspondientes[b] = a;
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "MS(MC)= %d(%d)", marc_sintesis[0],	marc_correspondientes[0]);
    for (int ii = 1; ii < b; ii++) {
      fprintf(stderr, " %d(%d)", 	marc_sintesis[ii], marc_correspondientes[ii]);
    }
    fprintf(stderr, " -> ultimo_pitch:%d\n", unidad_sintesis->ultimo_pitch);
#endif

    unidad_sintesis->incluye_marcas_sintesis_y_correspondientes(marc_sintesis, marc_correspondientes,
								b);

  } // if (frec_tiempo % 0x2)
  else {
    for (int i = 0; i < numero_marcas_analisis; i++) {
      marc_sintesis[i] = marc_inter[i];
      marc_correspondientes[i] = i;
    } // for (i = 0; ...

    unidad_sintesis->incluye_marcas_sintesis_y_correspondientes(marc_sintesis, marc_correspondientes,
								numero_marcas_analisis);
  } // else
	
	
  /*
   *cambio_sonoridad = (sordo_sonoro(objetivo.semifonema) !=  sordo_sonoro(objetivo.contexto_derecho));	
   *if (cambio_sonoridad && unidad_sintesis->izquierdo_o_derecho() == DERECHO && unidad_sintesis->numero_marcas_sintesis > 1){
   *  //unidad_sintesis->ultimo_pitch = (int) (fb + 0.5);
   *  unidad_sintesis->ultimo_pitch = marc_sintesis[	unidad_sintesis->numero_marcas_sintesis - 1] - marc_analisis[unidad_sintesis->numero_marcas_sintesis - 2];
   *}
   */
  if (unidad_sintesis->numero_marcas_sintesis == 1){
    if (unidad_sintesis->sonoridad == SONORO){
      //unidad_sintesis->pitch_sintesis = (int) (0.5 + 2*frecuencia_muestreo / (fa + fb));
      unidad_sintesis->pitch_sintesis = unidad_sintesis->ultimo_pitch;
    }
    else {
      //unidad_sintesis->pitch_sintesis = (int) (frecuencia_muestreo*TSORDOS);
      unidad_sintesis->pitch_sintesis = unidad_sintesis->ultimo_pitch;
    }
  }
  return 0;
}


/**
 * Función:   sintesis_unidad                                                 
 * \remarks Entrada:
 *			- unidad_sintesis: variable con la información relativa a la      
 *            unidad que se desea sintetizar.                                 
 *			- minpitch: pitch mínimo del locutor, en muestras.				  
 *			- maxpitch: pitch máximo del locutor, en muestras.				  
 *	\remarks Salida:
 *			- senhal: señal de voz sintetizada.                               
 *          - total_datos: número de muestras del fragmento sintetizado.      
 * \remarks Entrada y Salida:														  
 *			- ultimo_pitch_sintesis: valor del período de pitch de síntesis   
 *			  de la última trama de la unidad anterior.	A la salida, dicho    
 *			  valor para la unidad actual.									  
 *			- anterior_pitch_analisis: valor del período de pitch de la 	  
 *			  última trama de la unidad anterior. A la salida, dicho valor 	  
 *			  para la unidad actual.										  
 *	\remarks Salida:	- posicion_primera_marca_sintesis: posición, en muestras, de la   
 *			  primera marca de síntesis del segmento de voz.				  
 * \remarks Valor devuelto:                                                            
 *          - En ausencia de error devuelve un 0.                             
 * \remarks NOTA:
 *			- Versión de la función anterior, necesaria para proveer la 	  
 *			  información con la que se genera el fichero de marcas de pitch  
 *			  y de inicio de los semifonemas.								  
 */

int Procesado_senhal::sintesis_unidad(Vector_sintesis *unidad_sintesis,
				      int minpitch, int maxpitch,
				      short int *senhal,
				      int *total_datos, int *ultimo_pitch_sintesis,
				      int *anterior_pitch_analisis,
				      int *posicion_primera_marca_sintesis,FILE * fichero_sonido) {


  int *marc_analisis, *marc_sintesis, *marc_corr;
  short int num_marcas_analisis, num_marcas_sintesis;
  int posic, correspondiente, Pi, Pd, Pi_a, Pd_a, Pi_s, Pd_s;
  int posicion_lectura, posicion_escritura;
  int marc_analisis_anterior = -1;
  int tam_ventana_analisis;
  int posicion_origen;


  if (datos == NULL) {

    if ( (datos = (short int *) malloc(tamano_ventana*sizeof(short int)))
	 == NULL) {
      fprintf(stderr, "Error en sintesis_unidad, al asignar memoria.\n");
      return 1;
    } // if ( (datos = ...

    if ( (vent = (float *) malloc(tamano_ventana*sizeof(float)))
	 == NULL) {
      fprintf(stderr, "Error en sintesis_unidad, al asignar memoria.\n");
      return 1;
    } // if ( (vent = ...

  } // if (datos == NULL)

  int j;
  short int *pd, *px;
  float *pv;


  unidad_sintesis->devuelve_marcas(&marc_analisis, &marc_corr, &marc_sintesis, &num_marcas_analisis,
				   &num_marcas_sintesis);
  unidad_sintesis->devuelve_indice(&posicion_origen);

  //    posic = tamano_media_ventana;

  /* if (*ultimo_pitch_sintesis != INT_MAX)*/
  //posic = tamano_media_ventana - *anterior_pitch_analisis + *ultimo_pitch_sintesis;
  //else
  //posic = tamano_media_ventana;


  /*
  //    if (unidad_sintesis->sonoridad == SONORO) {

  if (*anterior_pitch_analisis != INT_MAX) {
  if (num_marcas_sintesis != 1)
  posic = tamano_media_ventana - *anterior_pitch_analisis + marc_sintesis[1];
  else {
  unidad_sintesis->anterior_pitch > unidad_sintesis->ultimo_pitch ?
  P = unidad_sintesis->ultimo_pitch:
  P = unidad_sintesis->anterior_pitch;
  posic = tamano_media_ventana - *anterior_pitch_analisis + P; // Pruebas
  }
  }
  else
  posic = tamano_media_ventana;
  //	}
  //    else
  //    	posic = tamano_media_ventana;
  */

  *posicion_primera_marca_sintesis = posic;

  for (int contador = 0; contador < num_marcas_sintesis; contador++) {
    correspondiente = *marc_corr++;

    if (correspondiente == 0)  { // Si se corresponde con la primera marca de análisis...
      Pi_a = unidad_sintesis->anterior_pitch;
      if (num_marcas_analisis > 1) {
	Pd_a = marc_analisis[1] - marc_analisis[0];
      }
      else {
	Pd_a = unidad_sintesis->ultimo_pitch;
      }
      if (contador == 0) {
	if (*ultimo_pitch_sintesis != 0){
	  Pi_s = *ultimo_pitch_sintesis;
	}
	else {
	  Pi_s = unidad_sintesis->anterior_pitch;
	}
	//Pi_s = minimo(*ultimo_pitch_sintesis, *anterior_pitch_analisis);
      }
      else {
	Pi_s = marc_sintesis[contador] - marc_sintesis[contador - 1];
      }
      if (num_marcas_sintesis > 1) {
	Pd_s = marc_sintesis[contador + 1 ] - marc_sintesis[contador];
      }
      else {
	Pd_s = unidad_sintesis->pitch_sintesis;
      }
    } // if (correspondiente == 0)
    else if (correspondiente == num_marcas_analisis - 1) { // Si se corresponde con la última...
      Pd_a = unidad_sintesis->ultimo_pitch;
      if (num_marcas_analisis > 1) {
	Pi_a = marc_analisis[correspondiente] - marc_analisis[correspondiente - 1];
      }
      else {
	Pi_a = unidad_sintesis->anterior_pitch;
      }
      Pi_s = marc_sintesis[contador] - marc_sintesis[contador - 1];
      if (contador == (num_marcas_sintesis -1)) { //última de síntesis
	Pd_s =  unidad_sintesis->ultimo_pitch; //Pd_s =  marc_sintesis[contador] - marc_sintesis[contador - 1];
      }
      else {
	Pd_s =  marc_sintesis[contador + 1] - marc_sintesis[contador];
      }
    }
    else { // En el resto de los casos...
      if (num_marcas_analisis > 1) {
	Pd_a = marc_analisis[correspondiente + 1] - marc_analisis[correspondiente];
	Pi_a = marc_analisis[correspondiente] - marc_analisis[correspondiente - 1];
      }
      else {
	fprintf(stderr, "hola\n");
      }
      Pi_s = marc_sintesis[contador] - marc_sintesis[contador - 1];
      Pd_s =  marc_sintesis[contador + 1] - marc_sintesis[contador];
    }

		
    Pi = minimo(Pi_a, Pi_s);//Pi=Pi_a;
    Pd = minimo(Pd_a, Pd_s);//Pd=Pd_a;

    
    if (Pi < minpitch) { // No se permite que el pitch se salga de los valores máximo y
#ifdef _DEBUG
			fprintf(stderr, "Pi (%d) < minpitch (%d)\n",Pi,minpitch);
#endif
      Pi = minpitch; // mínimo fijados.
    }
     
    if (Pi > maxpitch) {
#ifdef _DEBUG
			fprintf(stderr, "Pi (%d) > maxpitch (%d)\n",Pi,maxpitch);
#endif
      Pi = maxpitch;
    }
    if (Pd < minpitch) { // No se permite que el pitch se salga de los valores máximo y
#ifdef _DEBUG
			fprintf(stderr, "Pd (%d) < minpitch (%d)\n",Pd,minpitch);
#endif
      Pd = minpitch; // mínimo fijados.
    }
    if (Pd > maxpitch) {
#ifdef _DEBUG
			fprintf(stderr, "Pd (%d) > maxpitch (%d)\n",Pd,maxpitch);
#endif
      Pd = maxpitch;
    }

    if (contador== 0){
      if (*ultimo_pitch_sintesis != 0){
	posic = *ultimo_pitch_sintesis - Pi;
	*posicion_primera_marca_sintesis = *ultimo_pitch_sintesis;
      }
      else {
	posic = 0;
	*posicion_primera_marca_sintesis = Pi_s;
      }
    }
    //P = minimo(Pi, Pd);

    // Inicializamos el vector de ventana:

    memset(datos, 0, tamano_ventana*sizeof(short int));

    posicion_lectura = posicion_origen + (marc_analisis[correspondiente] - Pi*FACTOR)*sizeof(short int);
    // Pos read de *.bin

    // Comprobamos que no sea una posición negativa:

    if (posicion_lectura < 0)
      posicion_lectura = 0;

    // Forzamos a posición par:

    if (posicion_lectura % 2)
      posicion_lectura += 1;

    //posicion_escritura = posic + *posicion_primera_marca_sintesis + marc_sintesis[contador] - Pi*FACTOR;
    posicion_escritura = *posicion_primera_marca_sintesis + marc_sintesis[contador] - Pi*FACTOR;

    //while ( (posicion_escritura =
    //posic + marc_sintesis[contador] - Pi*FACTOR) < 0) {
    //Pi = (int) ( (posic + marc_sintesis[contador]) / FACTOR);
    //posicion_lectura = posicion_origen + (marc_analisis[correspondiente] - Pi*FACTOR)*sizeof(short int);
    //// Forzamos a posición par:

    //if (posicion_lectura % 2)
    //posicion_lectura += 1;
    //} // while

    //tam_ventana_analisis = 2*P*FACTOR+1; // Cálculo del tamaño de la ventana de análisis
    //ventana_hanning(vent, tam_ventana_analisis, 0); // Cálculo de la ventana hanning
    tam_ventana_analisis = (Pi + Pd)* FACTOR ; // Cálculo del tamaño de la ventana de análisis
    ventana_rectangu_hanning(vent, tam_ventana_analisis, Pi, Pd); // Cálculo de la ventana hanning

    /*
     *for (j = tam_ventana_analisis - 1; j < tamano_ventana; j++)
     *  vent[j] = 0.0; // Inic. ventana datos
     */

    fseek(fichero_sonido, posicion_lectura, SEEK_SET); // Nos situamos en la posición adecuada

    // Leemos el fragmento de sonido.
    fread(datos, sizeof(short int), tam_ventana_analisis, fichero_sonido);

#ifdef _ESCRIBE_WAV
    FILE *f;
    f = fopen("tal_cual.raw", "wb");
    fwrite(datos, sizeof(short int), tam_ventana_analisis, f);
    fclose(f);

#endif

    if (posicion_escritura < 0) {
      fprintf(stderr, "Error en sintesis_unidad: posicion_escritura menor que cero!!!!.\n");
      return 1;
    }

    pd = datos; px = &(senhal[posicion_escritura]);
    pv = vent; // pw = &(ventana[posicion_escritura]);

    if ( (marc_analisis_anterior == correspondiente) &&
	 (unidad_sintesis->sonoridad == SORDO) ) { //Si sorda y se repite la correspondencia

      pd = &(datos[tam_ventana_analisis]);
      pv = &(vent[tam_ventana_analisis]);

      while (pd > datos) {
	*px++ += (short int) (ALPHA * (*(--pv)) * (*(--pd)));
	//--pv;
      }
      // printf("I(%d) ",i); printf("Inversion Sorda\n");
      marc_analisis_anterior = correspondiente - 1;
    }
    else  { // Si no, calculo de xo normal y ventana de analisis normal
      for (j = 0; j < tam_ventana_analisis; j++)
	*px++ += (short int) (ALPHA * (*pv++) * (*pd++));
      marc_analisis_anterior = correspondiente;
    }

  }
#ifdef _ESCRIBE_WAV
  {
    FILE *f;
    f = fopen("trama_enventanada.raw", "wb");
    fwrite(&senhal[posicion_escritura], sizeof(short int), tam_ventana_analisis, f);
    fclose(f);
  }
#endif

#ifdef _ESCRIBE_WAV
  {
    FILE *f;
    f = fopen("enventanada.raw", "wb");
    fwrite(senhal, sizeof(short int), 25600, f);
    fclose(f);
  }
#endif



  /*
   *  if (num_marcas_sintesis > 1){
   *    *ultimo_pitch_sintesis = marc_sintesis[num_marcas_sintesis - 1] - marc_sintesis[num_marcas_sintesis - 2];
   *  }
   *  else {
   *    *ultimo_pitch_sintesis = unidad_sintesis->pitch_sintesis;
   *  }
   *
   *  *anterior_pitch_analisis = Pd;
   */
  *ultimo_pitch_sintesis = Pd_s; 
	
  // Cálculo del numero total de muestras en el vector de síntesis
  *total_datos = posicion_escritura + tam_ventana_analisis ;
  /*
   *if (num_marcas_sintesis > 1)
   *  *total_datos = (int) (posic + marc_sintesis[num_marcas_sintesis - 1] + Pd);
   *else
   *  *total_datos = (int) (posic + marc_sintesis[0] + Pd);
   */

  return 0;
}


/**
 * Función:   prosodia_original                                               
 * \remarks Entrada y Salida:                                                          
 *          - vector: vector con las marcas de la unidad de síntesis.         
 * \remarks Valor devuelto:                                                            
 *          - En ausencia de error, devuelve un cero.                         
 * \remarks Objetivo:  Generar una señal de voz respetando la prosodia de las unidades 
 *            originales.                                                     
 */

int Procesado_senhal::prosodia_original(Vector_sintesis *vector) {

  int *marc_sintesis, *marc_correspondientes, *marc_analisis;
  short int numero_marcas_analisis;
  int contador;


  vector->devuelve_marcas_analisis(&marc_analisis, &numero_marcas_analisis);

  if ( (marc_sintesis = (int *) malloc(numero_marcas_analisis*sizeof(int))) == NULL) {
    fprintf(stderr, "Error en prosodia_original, al asignar memoria.\n");
    return 1;
  }

  if ( (marc_correspondientes = (int *) malloc(numero_marcas_analisis*sizeof(long  int)))
       == NULL) {
    fprintf(stderr, "Error en prosodia_original, al asignar memoria.\n");
    return 1;
  }

  for (contador = 0; contador < numero_marcas_analisis; contador++) {
    marc_sintesis[contador] = marc_analisis[contador];
    marc_correspondientes[contador] = contador;
  }

  vector->pitch_sintesis = vector->ultimo_pitch;
    
  vector->incluye_marcas_sintesis_y_correspondientes(marc_sintesis, marc_correspondientes,
						     numero_marcas_analisis);
  return 0;

}


/**
 * Función:   ventana_hanning                                                 
 * \remarks Entrada:
 *			- lonx: tamaño de la ventana.                                     
 *          - pos: posición a partir de la que se enmpieza a crear la ventana.
 *	\remarks Salida:
 *			- vent: ventana de tipo hanning.                                  
 */

void ventana_hanning(float *vent, int lonx, int pos){

  float *pvent; //fact_norm, energia=0.0;
  register int i;

  double dosPipartido;
  pvent=vent+pos;

  lonx--;
  dosPipartido=2*PI/lonx;
  *pvent=0;

  for (i=1;i<=lonx;++i)
    *(pvent+i) = 0.5*(1-cos(dosPipartido*i));
}


/**
 * Función:   ventana_rectangu_hanning                                        
 * \remarks Entrada:
 *			- tamano: tamaño total de la ventana.                             
 *			- pitch_inicial: período de pitch del inicio, a partir del cual	  
 *			  se calcula la caída inicial de la ventana.					  
 *			- pitch_final: período de pitch del final, a partir del cual se   
 *			  calcula la caída final de la ventana.							  
 *	\remarks Salida:
 *			- vent: ventana con diferente caída al inicio y al final, y con   
 *			  unos en la parte intermedia.									  
 */

void ventana_rectangu_hanning(float *vent, int tamano, int pitch_inicial, int pitch_final) {

  int numero_unos;
  float *pvent; //fact_norm, energia=0.0;
  int i;

  double dosPipartido;

  numero_unos = 1 + tamano - pitch_inicial - pitch_final;

  dosPipartido = PI / pitch_inicial; // No es 2*PI, ya que pitch inicial es la mitad de la
  // ventana.

  pvent = (float *) vent;
  *pvent++=0.0;

  for (i = 1;i < pitch_inicial; i++){
    *pvent++ = 0.5*(1 - cos(dosPipartido*i));
  }

  for (i = 0; i < numero_unos; i++){
    *pvent++ = 1.0;
  }

  dosPipartido = PI / pitch_final;

  for (i = pitch_final - 1; i > 0; i--){
    *pvent++ = 0.5*(1 - cos(dosPipartido*i));
  }
  //*pvent = 0.0;

}


/**
 * Función:   extrae_forma_de_onda_original	                                  
 * \remarks Entrada:
 *			- primer_vector: variable con la información relativa a la        
 *            primera unidad de la secuencia consecutiva que se va a copiar   
 *			  directamente.													  
 *			- ultimo_vector: variable con la información de la última unidad  
 *			  de la secuencia consecutiva que se va a copiar directamente.	  
 *			- minpitch: mínimo pitch del locutor, en muestras.				  
 *			- maxpitch: máximo pitch del locutor, en muestras.				  		
 *	\remarks Salida:
 *			- senhal: señal de voz sintetizada.                               
 *          - total_datos: número de muestras del fragmento sintetizado.      
 * \remarks Entrada y Salida:														  
 *			- ultimo_pitch_sintesis: valor del período de pitch de síntesis   
 *			  de la última trama de la unidad anterior.	A la salida, dicho    
 *			  valor para la unidad actual.									  	
 *			- anterior_pitch_analisis: valor del período de pitch de la 	  
 *		      última trama de la unidad anterior. A la salida, dicho valor 	  
 *		      para la unidad actual.										  
 *	\remarks Salida:	- posicion_primera_marca_sintesis: posición de la primera marca   
 *			  de síntesis del segmento de voz actual.						  
 * \remarks Valor devuelto:                                                            
 *          - En ausencia de error devuelve un 0.                             
 * \remarks NOTA:
 *			- Variante de la función sintesis_unidad, que se emplea cuando no 
 *			  es preciso modificar prosódicamente una unidad o conjunto de 	  
 *			  unidades consecutivas en el Corpus, lo cual permite copiar su   
 *			  forma de onda directamente en lugar de reconstruirla a partir   
 *			  de las marcas de pitch. Esta versión se emplea para generar los 
 *			  ficheros de segmentación y marcas de síntesis.		  		  
 */

int Procesado_senhal::extrae_forma_de_onda_original(Vector_sintesis *primer_vector, Vector_sintesis *ultimo_vector,
						    int minpitch, int maxpitch,
						    short int *senhal, int *total_datos,
						    int *ultimo_pitch_sintesis,
						    int *anterior_pitch_analisis,
						    int *posicion_primera_marca_sintesis, FILE * fichero_sonido) {

  //*ultimo_pitch_sintesis = Pd;
  //*ultimo_pitch_sintesis = Pd;
  //*ultimo_pitch_sintesis = Pd;
  int posic, P_inicial, P_final, numero_datos;
  int posicion_lectura, posicion_escritura;
  int posicion_origen_1, posicion_origen_2;
  int contador;
  short int num_marcas_analisis;
  int *marc_analisis;
  short int datos[TAM_VECTOR];

  float vent[TAM_VECTOR];
  short int *pd, *px;
  float *pv;


  ultimo_vector->devuelve_indice(&posicion_origen_2);

  primer_vector->devuelve_indice(&posicion_origen_1);

  marc_analisis = primer_vector->marcas_analisis;

  //    posic = TAM_MED_VENTANA;
  //	posic = tamano_media_ventana;

  //if (*ultimo_pitch_sintesis != INT_MAX)
  //posic = tamano_media_ventana - *anterior_pitch_analisis + *ultimo_pitch_sintesis;
  //else
  //posic = tamano_media_ventana;

  /*
  //	if (primer_vector->sonoridad == SONORO) {
  if (*anterior_pitch_analisis != INT_MAX)
  if (primer_vector->numero_marcas_analisis > 1)
  posic = tamano_media_ventana - *anterior_pitch_analisis + marc_analisis[1];
  else
  primer_vector->anterior_pitch > primer_vector->ultimo_pitch ?
  posic = tamano_media_ventana - *anterior_pitch_analisis + primer_vector->ultimo_pitch:
  posic = tamano_media_ventana - *anterior_pitch_analisis + primer_vector->anterior_pitch;
  else
  posic = tamano_media_ventana;
  //    }
  //    else
  //    	posic = tamano_media_ventana;
  */

  //	*posicion_primera_marca_sintesis = posic;

  //	if (primer_vector->numero_marcas_analisis > 1)
  //		P_inicial = marc_analisis[1] - marc_analisis[0];
  //	else {
  // pruebas:
  //fmendezP_inicial = primer_vector->anterior_pitch;
  if (*ultimo_pitch_sintesis != 0){
    P_inicial = *ultimo_pitch_sintesis;
  }
  else {
    P_inicial = primer_vector->anterior_pitch;
  }

  if (P_inicial < minpitch){ // No se permite que el pitch se salga de los valores máximo y
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "Cambio P_inicial: %d -> %d\n", P_inicial, minpitch); 
#endif
    P_inicial = minpitch; // mínimo fijados.
  }
  if (P_inicial > maxpitch){
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "Cambio P_inicial: %d -> %d\n", P_inicial, maxpitch); 
#endif
    P_inicial = maxpitch;
  }


  if (*ultimo_pitch_sintesis > P_inicial){
    posicion_escritura = *ultimo_pitch_sintesis - P_inicial;
  }
  else {
    posicion_escritura = 0;
  }

  *posicion_primera_marca_sintesis = P_inicial + posicion_escritura;


  posicion_lectura = posicion_origen_1 + (marc_analisis[0] - P_inicial*FACTOR)*sizeof(short int);

  if (posicion_lectura % 2)
    posicion_lectura += 1;


  //while ( (posicion_escritura =
  //posic + marc_analisis[0] - P_inicial*FACTOR) < 0) {
  //P_inicial = (int) ( (posic + marc_analisis[0]) / FACTOR);
  //posicion_lectura = posicion_origen_1 + (marc_analisis[0] - P_inicial*FACTOR)*sizeof(short int);
  //// Forzamos a posición par:

  //if (posicion_lectura % 2)
  //posicion_lectura += 1;
  //} // while

  marc_analisis = ultimo_vector->marcas_analisis;
  num_marcas_analisis = ultimo_vector->numero_marcas_analisis;

  //	if (num_marcas_analisis > 1)
  //		P_final = marc_analisis[num_marcas_analisis - 1] - marc_analisis[num_marcas_analisis - 2];
  //	else
  P_final = ultimo_vector->ultimo_pitch;
  //P_final = (int) (TSORDOS*frecuencia_muestreo);


  if (P_final < minpitch){ // No se permite que el pitch se salga de los valores máximo y
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "Cambio P_final: %d -> %d\n", P_final, minpitch); 
#endif
    P_final = minpitch; // mínimo fijados.
  }
  if (P_final > maxpitch){
#ifdef _DEBUG_PROCESADO
    fprintf(stderr, "Cambio P_final: %d -> %d\n", P_final, maxpitch); 
#endif
    P_final = maxpitch;
  }


  *anterior_pitch_analisis = P_final;

  *ultimo_pitch_sintesis = P_final;

  numero_datos = P_inicial + (posicion_origen_2 - posicion_origen_1) / sizeof(short int) +
    marc_analisis[num_marcas_analisis - 1] + P_final;

  // Creamos la ventana mezcla de rectangular y hanning en la caída.

  ventana_rectangu_hanning(vent, numero_datos, P_inicial, P_final);

  // Comprobamos que no sea una posición negativa:

  if (posicion_lectura < 0)
    posicion_lectura = 0;

  // Leemos la forma de onda del fichero

  fseek(fichero_sonido, posicion_lectura, SEEK_SET);

  fread(datos, sizeof(short int), numero_datos, fichero_sonido);
#ifdef _DEBUG_PROCESADO
  //	fprintf(stderr, "Lectura: %d+%d\n", posicion_lectura, numero_datos*sizeof(short int)); 
#endif

#ifdef _ESCRIBE_WAV
  {
    FILE *f;
    f = fopen("voz_igual.raw", "wb");
    fwrite(datos, sizeof(short int), numero_datos, f);
    fclose(f);
  }
#endif

  pd = datos; // Puntero a los datos.
  for(px = senhal, contador = 0; contador < posicion_escritura; contador++){
    *px++ = 0;
  }
  pv = vent; // Puntero a la ventana.
	
  // Finalmente, enventanamos los datos:

  for (contador = 0; contador < numero_datos; contador++)
    *px++ = (short int) (*pd++ * *pv++);

  // Número total de muestras:

  *total_datos = posicion_escritura + numero_datos;

  return 0;

}


/*
 * \author	Fran Campillo
 * \remarks	Función que calcula, en valor absoluto, el máximo valor de la
 * cadena de entrada. Se emplea para evitar que una modificación de energía
 * con un factor demasiado elevado acabe saturando la señal (o incluso, convirtiendo
 * valores positivos en negativos).
 * \param	datos: cadena de enteros cortos con la información del fragmento de voz.
 * \param 	longitud: número de elementos de la cadena anterior.
 * \retval	valor_maximo: valor más altom en valor absoluto, de la cadena.
 */

void devuelve_valor_maximo_senhal(short int *datos, int longitud, short int *valor_maximo) {

  short int *apunta_dato = (short int *) datos;
  short int maximo = 0;

  for (int contador = 0; contador < longitud; contador++, apunta_dato++)
    if (*apunta_dato > absoluto(maximo))
      maximo = *apunta_dato;

  *valor_maximo = maximo;
}


/**
 * \author 
 * \remark Rutina que crea unha cabeceira .wav no arquivo ffwav  
 *
 * \param ffwav  
 * \param frecuencia  
 *
 * \return 
 */
void crear_cab_wav(FILE *ffwav, int frecuencia){ 
	char aux_out[100];
	unsigned int aux_out2;
	unsigned short int aux_out3;
	// Xeranse ficheiros wav con frec. de muetreo de 8Khz ou 16 Khz segundo o mande
	// o parametro wav8 (1 e 0 respectivamente) e 2 bytes/mostra

	strcpy(aux_out,"RIFF"); 
	fwrite(aux_out,sizeof(unsigned char),4,ffwav);
	aux_out2=0+36; 
	fwrite(&aux_out2,sizeof(unsigned int),1,ffwav);
	strcpy(aux_out,"WAVEfmt "); 
	fwrite(aux_out,sizeof(unsigned char),8,ffwav);
	aux_out2=16; 
	fwrite(&aux_out2,sizeof(unsigned int),1,ffwav);
	aux_out3=1;  
	fwrite(&aux_out3,sizeof(unsigned short),1,ffwav);
	fwrite(&aux_out3,sizeof(unsigned short),1,ffwav);
	//   if (wav8) aux_out2=8000UL;
	//   else aux_out2=16000UL;
	aux_out2=frecuencia;
	fwrite(&aux_out2,sizeof(unsigned int),1,ffwav); // fs Hz/sg
	//   if (wav8) aux_out2=16000UL;
	//   else aux_out2=32000UL;
	aux_out2=frecuencia* 2;
	fwrite(&aux_out2,sizeof(unsigned int),1,ffwav);   // bytes/sg
	aux_out3=2;  
	fwrite(&aux_out3,sizeof(unsigned short),1,ffwav); // bytes/sample
	aux_out3=16; 
	fwrite(&aux_out3,sizeof(unsigned short),1,ffwav);
	strcpy(aux_out,"data"); 
	fwrite(aux_out,sizeof(unsigned char),4,ffwav);
	aux_out2=0; 
	fwrite(&aux_out2,sizeof(unsigned int),1,ffwav);
}   // Fin de creacion da cabeceira dun arquivo .WAV



/**
 * \author 
 * \remark 
 *
 * \param entrada  
 * \param tam  
 *
 * \return 
 */
int filtra_diezma(short int * entrada, int tam){
	short int filtrado[TAM_VECTOR];
	float a[7]={1,-1.1079,1.4484,-0.9077,0.4211,-0.0947,0};
	float b[7]={0.0237,0.1186,0.2372,0.2372,0.1186,0.0237,0};
	static float y1[7]={0,0,0,0,0,0,0},x1[7]={0,0,0,0,0,0,0};
	int k;

	// Ecuacion en diferencias que implementa un filtro Chebychev de 5 coeficientes
	// E un filtro IIR, primeiro realizase para o tramo de voz.
	for(k=0;k<tam;k++) {
		filtrado[k]=(short int)(b[0]*(float)entrada[k]+b[1]*x1[0]+b[2]*x1[1]+b[3]*x1[2]+
				b[4]*x1[3]+b[5]*x1[4]+b[6]*x1[5]-a[1]*y1[0]-a[2]*y1[1]-a[3]*y1[2]
				-a[4]*y1[3]-a[5]*y1[4]-a[6]*y1[5]);
		x1[6]=x1[5];
		x1[5]=x1[4];
		x1[4]=x1[3];
		x1[3]=x1[2];
		x1[2]=x1[1];
		x1[1]=x1[0];
		x1[0]=(float)entrada[k];
		y1[6]=y1[5];
		y1[5]=y1[4];
		y1[4]=y1[3];
		y1[3]=y1[2];
		y1[2]=y1[1];
		y1[1]=y1[0];
		y1[0]=(float)filtrado[k];
	}
	for(k=0;k< tam/2;k++) {
		entrada[k]=filtrado[2*k];// Diezmado do sinal unha vez filtrada
	}
	return 0;
}






