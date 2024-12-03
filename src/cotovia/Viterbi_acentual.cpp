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
#include <float.h>

#include "descriptor.hpp"
#include "info_corpus_voz.hpp"
#include "matriz.hpp"
#include "cache.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "info_estructuras.hpp"
#include "Viterbi_acentual.hpp"


Viterbi_acentual::Viterbi_acentual() {
#ifdef _DEPURA_VITERBI_ACENTUAL
  iteracion_gf = 0;
#endif
  fuerza_fronteras_entonativas = 0;

  indices_escogidos = NULL;
  contornos_escogidos = NULL;
  vector_coste = NULL;
  vector_coste_GF = NULL;
  C_i_GF = NULL;
  C_i = NULL;

}

Viterbi_acentual::~Viterbi_acentual() {

  if (indices_escogidos)
    free(indices_escogidos);

  if (contornos_escogidos)
    free(contornos_escogidos);

  if (vector_coste)
    free(vector_coste);

  if (vector_coste_GF)
    free(vector_coste_GF);

  if (C_i_GF)
    free(C_i_GF);

  if (C_i)
    free(C_i);

}

/**
 * \remarks Modifica la variable miembro modifica_fronteras_entonativas
 * \param[in] fuerza Valor que se le quiere asignar a dicha variable
 */
void Viterbi_acentual::modifica_fuerza_fronteras_entonativas(unsigned char fuerza) {
  fuerza_fronteras_entonativas = fuerza;
}

#ifdef _BUSQUEDA_EXHAUSTIVA

/**
 * Función:    establece_identificadores                                                
 * \remarks Entrada:	 - p_identificador: identificador numérico del primer grupo original de la  
 *			   frase que se desea sintetizar.											
 *			 - u_identificador: identificador numérico del último grupo original de la  
 *			   frase que se desea sintetizar.											
 */

void Viterbi_acentual::establece_identificadores(int p_identificador, int u_identificador) {

  primer_identificador = p_identificador;
  ultimo_identificador = u_identificador;

}

/**
 * Función:    establece_pesos_busqueda                                                 
 * \remarks Entrada:
 *			- p_obj_conc: peso que regula la relación entre los pesos de objetivo y	
 *			   concatenación.															
 *			 - p_pos_ac: peso para la posición del grupo acentual en el grupo fónico.	
 *			 - p_tip_prop: peso para el tipo de proposición.							
 *			 - p_num_sil: peso para el número de sílabas.								
 *			 - p_pos_fon: peso para la posición del grupo fónico en la oración.			
 *			 - p_dur: peso para la duración.											
 *			 - p_fin_grupo: peso para el subcoste de fin de grupo.						
 */


void Viterbi_acentual::establece_pesos_busqueda(float p_obj_conc, float p_pos_ac, float p_tip_prop,
						float p_num_sil, float p_pos_fon, float p_dur,
						float p_fin_grupo) {

  peso_obj_conc = p_obj_conc;
  peso_pos_acentual = p_pos_ac;
  peso_tip_prop = p_tip_prop;
  peso_num_sil = p_num_sil;
  peso_pos_fonico = p_pos_fon;
  peso_dur = p_dur;
  peso_fin_grupo = p_fin_grupo;

}


/**
 * Función:   calcula_C_t_busqueda
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.                           
 *          - objetivo: identificador de la unidad objetivo.                             
 * \remarks Objetivo:  Calcula el coste de unidad.                                                
 */

float Viterbi_acentual::calcula_C_t_busqueda(Grupo_acentual_candidato *candidato,
					     Grupo_acentual_objetivo *objetivo) {

  float coste = 0.0;
  float duracion_cand = candidato->final - candidato->inicio;
  float duracion_obj = objetivo->duracion;
  unsigned char posicion_obj = objetivo->posicion_grupo_acentual;
  float pos_obj, pos_cand;
  float pos_prosodica_obj = 0, pos_prosodica_cand = 0;
  char tipo_pausa_obj = objetivo->tipo_pausa;
  char tipo_pausa_cand = candidato->tipo_pausa;

  if ( (candidato->identificador >= primer_identificador) &&
       (candidato->identificador <= ultimo_identificador) )
    return 1000;
         
  // Coste de posición de grupo acentual en grupo prosódico.

  if ( (posicion_obj != 0) && (posicion_obj != objetivo->grupos_acentuales_grupo_prosodico - 1) ) {

    pos_obj = (float) posicion_obj / (objetivo->grupos_acentuales_grupo_prosodico - 2);
    pos_cand = (float) candidato->posicion_grupo_acentual /
      (candidato->grupos_acentuales_grupo_prosodico - 2);
    coste += peso_pos_acentual*absoluto(pos_obj - pos_cand);

  }

  // Coste de tipo de proposición

  if (objetivo->tipo_proposicion2 != candidato->tipo_grupo_prosodico_1)
    coste += peso_tip_prop;

  // Coste de número de sílabas.

  coste += peso_num_sil*absoluto(objetivo->numero_silabas - candidato->numero_silabas); // Antes 0.1

  // Coste de posición de grupo prosódico en frase.

  if (objetivo->posicion_grupo_prosodico)
    pos_prosodica_obj = (float) objetivo->posicion_grupo_prosodico /
      (objetivo->numero_grupos_prosodicos - 1);
  if (candidato->posicion_grupo_prosodico)
    pos_prosodica_cand = (float) candidato->posicion_grupo_prosodico /
      (candidato->numero_grupos_prosodicos - 1);

  coste += peso_pos_fonico*absoluto(pos_prosodica_obj - pos_prosodica_cand);   // Antes 1.5*

  // Coste de duraciones:

  coste += peso_dur*absoluto( (duracion_obj - duracion_cand) / duracion_obj);

  // coste de fin de grupo acentual (refleja el tipo de pausa).

  if (objetivo->posicion_grupo_acentual == objetivo->grupos_acentuales_grupo_prosodico - 1)
    if (tipo_pausa_obj != tipo_pausa_cand)
      coste += peso_fin_grupo*absoluto(tipo_pausa_obj - tipo_pausa_cand) /
	maximo(tipo_pausa_obj, tipo_pausa_cand);

  return peso_obj_conc*coste;

}

#endif


/**
 * Función:   carga_tabla_cache_acentual                                                
 * \remarks Valor devuelto:                                                                      
 *           - Si no existe el fichero de caché, o bien hay algún error al cargarlo en  
 *             memoria, devuelve un 1.                                                  
 * \remarks Objetivo:  Carga el contenido del archivo indicado en el define FICHERO_CACHE en     
 *            memoria.                                                                  
 */
/*
  int Viterbi_acentual::carga_tabla_cache_acentual() {

  return memoria_cache.inicia_cache(FICHERO_CACHE);

  }
*/

/**
 * Función:   inicia_poda_C_t_acentual                                                  
 * \remarks Objetivo:  Inicializa la variable minimo_c_t, necesaria para el cálculo del umbral   
 *            para la poda por valor del array de costes de unidad. Es necesario llamar 
 *            a esta función cada vez que se inicia un nuevo paso del algoritmo de      
 *            Viterbi.                                                                  
 */

#ifdef PODA_C_t_POR_VALOR_acentual

void Viterbi_acentual::inicia_poda_C_t_acentual() {

  minimo_c_t = FLT_MAX - C_PODA_UNIDAD_acentual;

}

#endif

/**
 * Función:   inicia_poda_C_i_acentual                                                  
 * \remarks Objetivo:  Inicializa la variable minimo_c_i, necesaria para el cálculo del umbral   
 *            para la poda por valor del array de caminos de coste mínimo. Es necesario 
 *            llamar a esta función cada vez que se inicia un nuevo paso del algoritmo  
 *            de Viterbi.                                                               
 */

#ifdef PODA_C_i_POR_VALOR_acentual

void Viterbi_acentual::inicia_poda_C_i_acentual() {

  minimo_c_i = FLT_MAX - C_PODA_C_i_acentual;

}

#endif

/**
 * Función:   comprueba_entrada_en_cache_acentual                                        
 * \remarks Entrada:
 *			- identificador1: identificador de la unidad que proporciona un camino de    
 *            coste mínimo en el paso anterior.                                          
 *          - identificador2: identificador de la unidad considerada en el paso actual.  
 * \remarks Objetivo:  Comprueba si el coste de concatenación entre ambas unidades se encuentra   
 *            almacenado en la caché. En caso afirmativo, devuelve su valor. Si no,      
 *            devuelve un número negativo.                                               
 */
/*
  float Viterbi_acentual::comprueba_entrada_en_cache_acentual(int identificador1, int identificador2) {

  float resultado;

  // Comprobamos si está en la caché. Si está, ya devolvemos el resultado.
  if ( (resultado = memoria_cache.comprueba_existencia(identificador1, identificador2))
  >= 0)
  return resultado;

  // De momento, para las pruebas.

  return 50.0; // Así forzamos que haya poda.
  }
*/
/**
 * Función:   crea_delta_u                                                                
 * \remarks Entrada:
 *			- objetivo: vector descriptor de la unidad que se plantea como objetivo.      
 * Salida   - *cadena_id: cadena de identificadores de las unidades preseleccíonadas.     
 *          - tamano: número de elementos de dicha cadena.
 * \remarks Valor devuelto:
 *          - Devuelve 0 en caso de que no se produzca error.
 * \remarks Objetivo:  Almacena en *delta_u las candidatas para representar a la unidad objetivo
 *            que se le pasa como parámetro.
 */

int Viterbi_acentual::crea_delta_u(Grupo_acentual_candidato ***cadena_id, int *tamano, Grupo_acentual_objetivo *objetivo) {

  int indice;
  estructura_grupos_acentuales *unidad_seleccionada = NULL;
  Grupo_acentual_candidato *recorre_unidades;
  Grupo_acentual_candidato **recorre;
  int contador_sustituciones = -1, ronda_sustituciones = 0;
  unsigned char tipo_proposicion = objetivo->tipo_proposicion;
  int posicion; // = objetivo->posicion_grupo_acentual;
  int posicion_silaba_acentuada = objetivo->posicion_silaba_acentuada;

  /*
    if (posicion != 0) {
    if (posicion != objetivo->grupos_acentuales_grupo_prosodico - 1)
    posicion = GRUPO_MEDIO;
    else
    posicion = GRUPO_FINAL;
    } // if (posicion != 0)
    else
    if (objetivo->grupos_acentuales_grupo_prosodico == 1)
    posicion = GRUPO_INICIAL_Y_FINAL;
  */

  unsigned char tipo_pausa_anterior = objetivo->tipo_pausa_anterior;
  unsigned char tipo_pausa = objetivo->tipo_pausa;

  if ( (tipo_pausa_anterior == PAUSA_PUNTO) || (tipo_pausa_anterior == PAUSA_COMA) ||
       (tipo_pausa_anterior == PAUSA_PUNTOS_SUSPENSIVOS) ||
       (tipo_pausa_anterior == PAUSA_RUPTURA_COMA) ||
       (tipo_pausa_anterior == PAUSA_OTRO_SIGNO_PUNTUACION) ) {

    if ( (tipo_pausa == PAUSA_PUNTO) || (tipo_pausa == PAUSA_COMA) ||
	 (tipo_pausa == PAUSA_PUNTOS_SUSPENSIVOS) ||
	 (tipo_pausa == PAUSA_RUPTURA_COMA) ||
	 (tipo_pausa == PAUSA_OTRO_SIGNO_PUNTUACION) )
      posicion = GRUPO_INICIAL_Y_FINAL;
    else
      posicion = GRUPO_INICIAL;
  }
  else {

    if ( (tipo_pausa == PAUSA_PUNTO) || (tipo_pausa == PAUSA_COMA) ||
	 (tipo_pausa == PAUSA_PUNTOS_SUSPENSIVOS) ||
	 (tipo_pausa == PAUSA_RUPTURA_COMA) ||
	 (tipo_pausa == PAUSA_OTRO_SIGNO_PUNTUACION) )
      posicion = GRUPO_FINAL;
    else
      posicion = GRUPO_MEDIO;

  }


  while (unidad_seleccionada == NULL) {

    indice = calcula_indice_grupo(tipo_proposicion, posicion,
				  posicion_silaba_acentuada);

    if ( (unidad_seleccionada = locutor_actual->tabla_grupos[indice]) == NULL) {
      contador_sustituciones = (contador_sustituciones + 1) % 4;
      if (contador_sustituciones == 3) {
	contador_sustituciones = 0;
	ronda_sustituciones = 1;
	tipo_proposicion = 0; // ENUNCIATIVA
	posicion_silaba_acentuada = objetivo->posicion_silaba_acentuada;
      }
      else
	posicion_silaba_acentuada = (posicion_silaba_acentuada + 2) % TIPOS_GRUPO_SEGUN_ACENTO;

      if (ronda_sustituciones != 0)
	posicion = (posicion + 1) % TIPOS_GRUPO_SEGUN_POSICION;
    }

  } // while (unidad_seleccionada

  *tamano = unidad_seleccionada->numero_grupos;

  if ( (*cadena_id = (Grupo_acentual_candidato **) malloc(*tamano*
							  sizeof(Grupo_acentual_candidato *)))
       == NULL) {
    fprintf(stderr, "Error en crea_delta_u, al asignar memoria.");
    return 1;
  }

  recorre = *cadena_id;
  recorre_unidades = unidad_seleccionada->grupos;

  for (int cuenta = 0; cuenta < *tamano; cuenta++, recorre++, recorre_unidades++)
    *recorre = recorre_unidades;

  return 0;

}


/**
 * Función:   calcula_C_t
 * \remarks Entrada:
 *			- candidato: identificador de la unidad candidata.
 *          - objetivo: identificador de la unidad objetivo.
 * \remarks Objetivo:  Calcula el coste de unidad.
 */

float Viterbi_acentual::calcula_C_t(Grupo_acentual_candidato *candidato, Grupo_acentual_objetivo *objetivo) {

  int contador;
  float coste = 0.0;
  float duracion_cand = candidato->final - candidato->inicio;
  float duracion_obj = objetivo->duracion;
  float diferencia_duracion;
  //  float pos_prosodica_obj = 0, pos_prosodica_cand = 0;
  float umbral_duracion;

  //    float factor_caida = candidato->factor_caida;

  //	if ( (candidato->identificador == 102701) || (candidato->identificador == 126302) )
  //    	puts("");

  if (objetivo->posicion_grupo_acentual == objetivo->grupos_acentuales_grupo_prosodico - 1)
    for (contador = 1; contador < candidato->numero_silabas + 2; contador++)
      if (candidato->frecuencia.marcas[contador].sonoridad_pobre != 0)
	coste += 0.5;


  if ( (candidato->tipo_pausa_anterior != SIN_PAUSA) &&
       (candidato->tipo_pausa_anterior != PAUSA_RUPTURA_ENTONATIVA) &&
       (candidato->tipo_pausa_anterior != PAUSA_RUPTURA_COMA) ) {

    if (candidato->numero_silabas >= objetivo->numero_silabas) {
      for (contador = 1; contador < candidato->numero_silabas + 2; contador++)
	if (candidato->frecuencia.marcas[contador].sonoridad_pobre == 1)
	  coste += 1;
    }
    else
      if (candidato->numero_silabas < objetivo->numero_silabas) {
	for (contador = 0; contador < candidato->numero_silabas + 2; contador++)
	  if (candidato->frecuencia.marcas[contador].sonoridad_pobre == 1)
	    coste += 1.5;
      }
  }


  //  if ( (objetivo->posicion_grupo_acentual == 0) || (candidato->tipo_pausa!= SIN_PAUSA) ) {
    // Coste de estructura silábica.
    if (candidato->salto_f0 > UMBRAL_SALTO_F0_COSTE_SILABICO)
      coste += 2*coste_estructura_silabica(objetivo->silabas, objetivo->numero_silabas, candidato->frecuencia.marcas, candidato->numero_silabas, objetivo->posicion_silaba_acentuada);
    // else
    //   coste += 3*coste_estructura_silabica(objetivo->silabas, objetivo->numero_silabas, candidato->frecuencia.marcas, candidato->numero_silabas, objetivo->posicion_silaba_acentuada);

    // FRAN CAMPILLO: quizás habría que reconsiderar este coste en todos los casos (aunque con menor peso)

    //  }

  // Coste de número de sílabas.

  if ( (objetivo->numero_silabas > candidato->numero_silabas) && (candidato->numero_silabas < 2 + objetivo->posicion_silaba_acentuada) )
    coste += 4*absoluto(objetivo->numero_silabas - candidato->numero_silabas); // Para respetar el contorno alrededor del acento.
  else if ( (candidato->numero_silabas > objetivo->numero_silabas) && (objetivo->numero_silabas < 2 + candidato->posicion_silaba_acentuada) )
    coste += 4*absoluto(objetivo->numero_silabas - candidato->numero_silabas); // Para respetar el contorno alrededor del acento.
  else
    coste += 0.5*absoluto(objetivo->numero_silabas - candidato->numero_silabas); // Antes 1.5


#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL

  float coste_morfosintactico = 0;
  float coste_salto_f0 = 0.0, coste_factor_caida = 0.0;
    
#ifdef _MINOR_PHRASING_TEMPLATE

  float diferencia_salto_f0;
  float diferencia_factor_caida;
  float cociente_salto_f0;
  float cociente_factor_caida;

  if ( (objetivo->salto_f0 != 0) || (objetivo->factor_caida != 0) ) {
    // La estructura prosódica está fijada

    diferencia_salto_f0 = absoluto(objetivo->salto_f0 - candidato->salto_f0);
    diferencia_factor_caida = absoluto(objetivo->factor_caida - candidato->factor_caida);

    if (diferencia_salto_f0 > UMBRAL_SALTO_F0) {
      cociente_salto_f0 = minimo(absoluto(objetivo->salto_f0), absoluto(candidato->salto_f0)) / maximo(absoluto(objetivo->salto_f0), absoluto(candidato->salto_f0));
      if (cociente_salto_f0 < UMBRAL_COCIENTE_SALTO_F0) {
	if (absoluto(objetivo->salto_f0) != 0) {
	  coste_salto_f0 = absoluto( (diferencia_salto_f0 - UMBRAL_SALTO_F0) / objetivo->salto_f0);
	}
	else
	  if (absoluto(candidato->salto_f0) != 0) {
	    coste_salto_f0 = absoluto( (diferencia_salto_f0 - UMBRAL_SALTO_F0) / candidato->salto_f0);
	  }
      }

    } // if (diferencia_salto_f0 > UMBRAL_SALTO_F0)

    if (diferencia_factor_caida > UMBRAL_FACTOR_CAIDA) {
      cociente_factor_caida = minimo(absoluto(objetivo->factor_caida), absoluto(candidato->factor_caida)) / maximo(absoluto(objetivo->factor_caida), absoluto(candidato->factor_caida));
      if ( (cociente_factor_caida < UMBRAL_COCIENTE_FACTOR_CAIDA) ||
	   (mismo_signo(objetivo->factor_caida, candidato->factor_caida) == 0) ) {
	if (absoluto(objetivo->factor_caida) != 0)
	  coste_factor_caida = absoluto( (diferencia_factor_caida - UMBRAL_FACTOR_CAIDA) / objetivo->factor_caida);
	else
	  if (absoluto(candidato->factor_caida) != 0)
	    coste_factor_caida = absoluto( (diferencia_factor_caida - UMBRAL_FACTOR_CAIDA) / candidato->factor_caida);
      }
    } // if (diferencia_factor_caida > UMBRAL_FACTOR_CAIDA)

    coste_morfosintactico += 4*coste_salto_f0 + 2*coste_factor_caida;

    if ( (objetivo->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) || (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) )
      if (objetivo->tipo_pausa != candidato->tipo_pausa)
	coste_morfosintactico += 5;

    coste += 0.8*coste_morfosintactico;
        
  } // if ( (objetivo->salto_f0 != 0) ...

  else {

#endif

#ifndef _INFORMACION_MORFOSINTACTICA_AUTOMATICA

    if ( (candidato->tipo_pausa == SIN_PAUSA) ||
    	 (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) ) {

      if ( (objetivo->siguiente_sintagma >= GA_RELATIVO) &&
	   (objetivo->siguiente_sintagma <= GA_CONJ_SUBOR) ) {
	if ( (candidato->siguiente_sintagma < GA_RELATIVO) ||
	     (candidato->siguiente_sintagma > GA_CONJ_SUBOR) )
	  //         	coste_morfosintactico += 2;
	  coste += 2;
      }
      else
	if ( (candidato->siguiente_sintagma >= GA_RELATIVO) &&
	     (candidato->siguiente_sintagma <= GA_CONJ_SUBOR) )
	  coste += 2;

    } // if ( (candidato->tipo_pausa == SIN_PAUSA...

#else

    if ( (candidato->tipo_pausa == SIN_PAUSA) ||
    	 (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) ) {

      if ( (objetivo->siguiente_sintagma >= GA_RELATIVO) &&
	   (objetivo->siguiente_sintagma <= GA_CONJ_SUBOR) ) {
	if ( ( (candidato->siguiente_sintagma < GA_RELATIVO) ||
	       (candidato->siguiente_sintagma > GA_CONJ_SUBOR) )&&
	     (candidato->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) )
	  //         	coste_morfosintactico += 2;
	  coste_morfosintactico += 1;
      }
      else
	if ( (candidato->siguiente_sintagma >= GA_RELATIVO) &&
	     (candidato->siguiente_sintagma <= GA_CONJ_SUBOR) )
	  coste_morfosintactico += 1;

      int clave_obj = crea_clave_hash_inserciones(objetivo->sintagma_actual,
						  objetivo->siguiente_sintagma);

      /*
	float probabilidad_dif;
        int clave_cand = crea_clave_hash_inserciones(candidato->sintagma_actual,
	candidato->siguiente_sintagma);

        if (candidato->etiqueta_morfosintactica != GA_DETERMINANTE)
	probabilidad_dif = absoluto(locutor_actual->probabilidad_insercion_rupturas[clave_cand] -
	locutor_actual->probabilidad_insercion_rupturas[clave_obj]);
        else
	probabilidad_dif = locutor_actual->probabilidad_insercion_rupturas[clave_obj];

        coste_morfosintactico += 3*probabilidad_dif;
      */
      // Ajuste al modelo (debido a que únicamente consideramos sintagmas).
      // Intentamos evitar rupturas entonativas tras un determinante.

      if (fuerza_fronteras_entonativas  == 0) {

	if ( (objetivo->etiqueta_morfosintactica == GA_DETERMINANTE) &&
	     (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) )
	  coste_morfosintactico += 3;
	else
	  if ( ( (objetivo->sintagma_actual == GA_S_NOMINAL) ||
		 (objetivo->sintagma_actual == GA_S_PREPOSICIONAL) ) &&
	       (objetivo->etiqueta_morfosintactica != GA_NOMBRE) &&
	       (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) )
	    coste_morfosintactico += 3;
	
	if (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)
	  coste_morfosintactico += 3*(1 - locutor_actual->probabilidad_insercion_rupturas[clave_obj]); // obj? 5
	else
	  coste_morfosintactico += 2*locutor_actual->probabilidad_insercion_rupturas[clave_obj];//  3
      }
      else {
	if (objetivo->tipo_pausa != candidato->tipo_pausa)
	  coste_morfosintactico += 1000;
      }
    } // if ( (candidato->tipo_pausa == ...

    if (fuerza_fronteras_entonativas == 1) {
	if (objetivo->tipo_pausa_anterior != candidato->tipo_pausa_anterior)
	  coste_morfosintactico += 1000;
    }

#endif

#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA

    int clave = crea_clave_hash_contextos(candidato->etiqueta_morfosintactica, candidato->siguiente_sintagma, objetivo->etiqueta_morfosintactica, objetivo->siguiente_sintagma, candidato->tipo_pausa);

    int clave_2;

    // Primero, el coste relacionado con el salto de f0:

    if (locutor_actual->contexto_gramatical_salto_f0.count(clave))
      coste_salto_f0 = locutor_actual->contexto_gramatical_salto_f0[clave];
    else {
      if (candidato->tipo_pausa == PAUSA_RUPTURA_COMA) {
	clave_2 = crea_clave_hash_contextos(candidato->etiqueta_morfosintactica, candidato->siguiente_sintagma, objetivo->etiqueta_morfosintactica, objetivo->siguiente_sintagma, PAUSA_COMA);
	if (locutor_actual->contexto_gramatical_salto_f0.count(clave_2))
	  coste_salto_f0 = locutor_actual->contexto_gramatical_salto_f0[clave_2];
      } // if (candidato->tipo_pausa == PAUSA_RUPTURA_COMA)
      else {
	if (objetivo->tipo_pausa_anterior == PAUSA_PUNTO) {
	  if (candidato->salto_f0 < 5)
	    coste_salto_f0 = 5;
	} // if (objetivo->tipo_pausa_anterior == PAUSA_PUNTO)
	// else 
	//   if (objetivo->tipo_pausa == PAUSA_PUNTO) 
	//     if (candidato->salto_f0 > 0)
	//       coste_salto_f0 = 5;
      }
      
    }
    //    int tamano = locutor_actual->contexto_gramatical_salto_f0.size();

    // Luego, el coste del factor de caída:

    if (locutor_actual->contexto_gramatical_factor_caida.count(clave))
      coste_factor_caida = locutor_actual->contexto_gramatical_factor_caida[clave];
    else {
      if (candidato->tipo_pausa == PAUSA_RUPTURA_COMA) {
	clave_2 = crea_clave_hash_contextos(candidato->etiqueta_morfosintactica, candidato->siguiente_sintagma, objetivo->etiqueta_morfosintactica, objetivo->siguiente_sintagma, PAUSA_COMA);
	if (locutor_actual->contexto_gramatical_salto_f0.count(clave_2))
	  coste_factor_caida = locutor_actual->contexto_gramatical_salto_f0[clave_2];
      }
    }

    if (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)
      coste_morfosintactico += 4*(2*coste_salto_f0 + coste_factor_caida);
    else
      coste_morfosintactico += 2*(coste_salto_f0 + coste_factor_caida);

    coste += 2*coste_morfosintactico;

#else

    if (candidato->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA) {

      if ( (objetivo->etiqueta_morfosintactica == GA_OTROS) &&
	   ((objetivo + 1)->etiqueta_morfosintactica == GA_NOMBRE) )
	coste_morfosintactico += 3;

      if (objetivo->siguiente_sintagma != candidato->siguiente_sintagma) {

	if (objetivo->siguiente_sintagma == GA_CONJ_COOR_DIS) {
	  if (candidato->siguiente_sintagma != GA_CONJ_COOR_DIS)
	    if ( (candidato->siguiente_sintagma >= GA_RELATIVO) &&
		 (candidato->siguiente_sintagma <= GA_CONJ_SUBOR) )
	      coste_morfosintactico += 1;
	    else
	      coste_morfosintactico += 2;
	}
	else
	  if (candidato->siguiente_sintagma == GA_CONJ_SUBOR) {
	    if (candidato->siguiente_sintagma != GA_CONJ_SUBOR)
	      if ( (candidato->siguiente_sintagma >= GA_RELATIVO) &&
		   (candidato->siguiente_sintagma <= GA_CONJ_SUBOR) )
		coste_morfosintactico += 1;
	      else
		coste_morfosintactico += 2;
	  }
	  else
	    if ( (objetivo->siguiente_sintagma >= GA_RELATIVO) &&
		 (objetivo->siguiente_sintagma <= GA_CONJ_SUBOR) ) {
	      if ( (candidato->siguiente_sintagma >= GA_RELATIVO) &&
		   (candidato->siguiente_sintagma <= GA_CONJ_SUBOR) )
		coste_morfosintactico += 0.5;
	      else
		coste_morfosintactico += 1;
	    }
	    else
	      coste_morfosintactico += 0.5;

      }

    }
    else
      if (candidato->tipo_pausa == PAUSA_COMA) {


	if ( (candidato->tipo_pausa_anterior == PAUSA_RUPTURA_COMA) ||
	     (candidato->tipo_pausa_anterior == PAUSA_COMA) ||
	     (candidato->tipo_pausa_anterior == PAUSA_PUNTO) ||
	     (candidato->tipo_pausa_anterior == PAUSA_PUNTOS_SUSPENSIVOS) ) {


	  if ( (objetivo->etiqueta_morfosintactica == GA_NOMBRE) ||
	       (objetivo->etiqueta_morfosintactica == GA_ADJETIVO) ||
	       (objetivo->etiqueta_morfosintactica == GA_VERBO) ) {

	    if ( (candidato->etiqueta_morfosintactica != GA_NOMBRE) &&
		 (candidato->etiqueta_morfosintactica != GA_ADJETIVO) &&
		 (candidato->etiqueta_morfosintactica != GA_VERBO) )

	      coste_morfosintactico += 2;

	  }
	  else
	    if ( (candidato->etiqueta_morfosintactica == GA_NOMBRE) ||
		 (candidato->etiqueta_morfosintactica == GA_ADJETIVO) ||
		 (candidato->etiqueta_morfosintactica == GA_VERBO) )

	      coste_morfosintactico += 2;

	}


	if ( ( (objetivo->siguiente_sintagma == GA_S_VERBAL) && (candidato->siguiente_sintagma != GA_S_VERBAL) ) ||
	     ( (objetivo->siguiente_sintagma != GA_S_VERBAL) && (candidato->siguiente_sintagma == GA_S_VERBAL) ))
	  coste_morfosintactico += 1.75;
	else {
	  // Estos costes son para Freire. Habría que buscar una forma de personalizar esta
	  // función. Tal vez leer los costes de un fichero generado automáticamente.


	  if ( (objetivo->siguiente_sintagma == GA_CONJ_COOR_DIS) ||
	       (objetivo->siguiente_sintagma == GA_RELATIVO) ||
	       (objetivo->siguiente_sintagma == GA_CONJ_SUBOR) ||
	       (objetivo->siguiente_sintagma == GA_NO_CLASIFICADO) ||
	       (objetivo->siguiente_sintagma == GA_S_ADJETIVAL) ) {

	    if ( (candidato->siguiente_sintagma != GA_CONJ_COOR_DIS) &&
		 (candidato->siguiente_sintagma != GA_RELATIVO) &&
		 (candidato->siguiente_sintagma != GA_CONJ_SUBOR) &&
		 (candidato->siguiente_sintagma != GA_NO_CLASIFICADO) &&
		 (candidato->siguiente_sintagma != GA_S_ADJETIVAL) )
	      coste_morfosintactico += 1;
	  }
	  else {
	    if ( (candidato->siguiente_sintagma == GA_CONJ_COOR_DIS) ||
		 (candidato->siguiente_sintagma == GA_RELATIVO) ||
		 (candidato->siguiente_sintagma == GA_CONJ_SUBOR) ||
		 (candidato->siguiente_sintagma == GA_NO_CLASIFICADO) ||
		 (candidato->siguiente_sintagma == GA_S_ADJETIVAL) )
	      coste_morfosintactico += 1;
	  }
	}

	if (objetivo->sintagma_actual != candidato->sintagma_actual)
	  coste_morfosintactico += 0.25;

      }
      else
	if (candidato->tipo_pausa == PAUSA_RUPTURA_COMA) {

	  //				if (objetivo->sintagma_actual != candidato->sintagma_actual)
	  //                	coste_morfosintactico += 0.75;

	  if ( (candidato->tipo_pausa_anterior == PAUSA_RUPTURA_COMA) ||
	       (candidato->tipo_pausa_anterior == PAUSA_COMA) ||
	       (candidato->tipo_pausa_anterior == PAUSA_PUNTO) ||
	       (candidato->tipo_pausa_anterior == PAUSA_PUNTOS_SUSPENSIVOS) ) {


	    if ( (objetivo->etiqueta_morfosintactica == GA_NOMBRE) ||
		 (objetivo->etiqueta_morfosintactica == GA_ADJETIVO) ||
		 (objetivo->etiqueta_morfosintactica == GA_VERBO) ) {

	      if ( (candidato->etiqueta_morfosintactica != GA_NOMBRE) &&
		   (candidato->etiqueta_morfosintactica != GA_ADJETIVO) &&
		   (candidato->etiqueta_morfosintactica != GA_VERBO) )

		coste_morfosintactico += 2;

	    }
	    else
	      if ( (candidato->etiqueta_morfosintactica == GA_NOMBRE) ||
		   (candidato->etiqueta_morfosintactica == GA_ADJETIVO) ||
		   (candidato->etiqueta_morfosintactica == GA_VERBO) )

		coste_morfosintactico += 2;

	  }

	  if ( ( (objetivo->siguiente_sintagma == GA_RELATIVO) && (candidato->siguiente_sintagma != GA_RELATIVO) ) ||
	       ( (objetivo->siguiente_sintagma != GA_RELATIVO) && (candidato->siguiente_sintagma == GA_RELATIVO) ))
	    coste_morfosintactico += 2;

        }
        else
	  if (candidato->tipo_pausa == SIN_PAUSA) {

	    //                if (objetivo->siguiente_sintagma != candidato->siguiente_sintagma)
	    //                    coste_morfosintactico += 0.2;

	    // Considerar aquí grupos de categorías:

	    if ( (objetivo->etiqueta_morfosintactica == GA_NOMBRE) ||
		 (objetivo->etiqueta_morfosintactica == GA_ADJETIVO) ||
		 (objetivo->etiqueta_morfosintactica == GA_VERBO) ) {

	      if ( (candidato->etiqueta_morfosintactica != GA_NOMBRE) &&
		   (candidato->etiqueta_morfosintactica != GA_ADJETIVO) &&
		   (candidato->etiqueta_morfosintactica != GA_VERBO) )

		coste_morfosintactico += 2;

	    }
	    else
	      if ( (candidato->etiqueta_morfosintactica == GA_NOMBRE) ||
		   (candidato->etiqueta_morfosintactica == GA_ADJETIVO) ||
		   (candidato->etiqueta_morfosintactica == GA_VERBO) )

		coste_morfosintactico += 2;

	  }

    coste += 0.4*coste_morfosintactico;

#endif

#ifdef _MINOR_PHRASING_TEMPLATE
  } // else
#endif

#endif

  // Coste de posición de grupo prosódico en frase.

  if ( ( (objetivo->posicion_grupo_prosodico == 0) && (candidato->posicion_grupo_prosodico != 0) ) ||
       ( (candidato->posicion_grupo_prosodico == 0) && (objetivo->posicion_grupo_prosodico != 0) ) ) {
    if (objetivo->posicion_grupo_acentual == 0)
      coste += 1;
  }

  if ( (objetivo->posicion_grupo_prosodico != 0) || (candidato->posicion_grupo_prosodico != 0) ) {

    if ( ( (objetivo->posicion_grupo_prosodico == objetivo->numero_grupos_prosodicos - 1) && (candidato->posicion_grupo_prosodico != candidato->numero_grupos_prosodicos - 1) ) ||
	 ( (candidato->posicion_grupo_prosodico == candidato->numero_grupos_prosodicos - 1) && (objetivo->posicion_grupo_prosodico != objetivo->numero_grupos_prosodicos - 1) ) ) {

      if ( (objetivo->posicion_grupo_acentual == 0) || (objetivo->posicion_grupo_acentual == objetivo->grupos_acentuales_grupo_prosodico) )
	coste += 0.5;
      
    }

  } // if ( (objetivo->posicion_grupo_prosodico != 0...
  
  // if ( (objetivo->posicion_grupo_prosodico != 0) || (candidato->posicion_grupo_prosodico != 0) )

  //   if ( (objetivo->posicion_grupo_prosodico != objetivo->numero_grupos_prosodicos - 1) ||
  // 	 (candidato->posicion_grupo_prosodico != candidato->numero_grupos_prosodicos - 1) ) {

  //     if (objetivo->posicion_grupo_prosodico)
  // 	pos_prosodica_obj = (float) objetivo->posicion_grupo_prosodico /
  // 	  (objetivo->numero_grupos_prosodicos - 1);
  //     if (candidato->posicion_grupo_prosodico)
  // 	pos_prosodica_cand = (float) candidato->posicion_grupo_prosodico /
  // 	  (candidato->numero_grupos_prosodicos - 1);

  //     coste += 1.5*absoluto(pos_prosodica_obj - pos_prosodica_cand);   // Antes 1.5*

  //   }

  // Coste de posición de la frase en el párrafo:

  // if ( (objetivo->numero_frase == 0) && (candidato->numero_frase != 0) )
  //   coste += 1;

  // Coste de duraciones:

  diferencia_duracion = absoluto(duracion_obj - duracion_cand);

  umbral_duracion = UMBRAL_DIFERENCIA_DURACION_GA*objetivo->numero_alofonos;

  if (diferencia_duracion > umbral_duracion)
    coste += (diferencia_duracion - umbral_duracion) / duracion_obj;

  // Coste de ruptura entonativa (por si se quiere forzar una R-E tras un grupo acentual).

  if (objetivo->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)
    if (candidato->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA)
      coste += 3;

  // Coste de caída anterior a una pausa que no es final de frase.
  /*
    if (objetivo->tipo_pausa == PAUSA_OTRO_SIGNO_PUNTUACION) {
    if (factor_caida <= 0)
    coste += 0.2;
    }
    else if (objetivo->tipo_pausa == PAUSA_COMA)
    if (factor_caida >= -20)
    coste += 0.5;
  */
  if (objetivo->tipo_pausa == PAUSA_PUNTO)
    if (candidato->tipo_pausa != PAUSA_PUNTO)
      coste += 20;

  if (candidato->tipo_pausa == PAUSA_PUNTO)
    if (objetivo->tipo_pausa != PAUSA_PUNTO)
      coste += 20;

  if (objetivo->tipo_pausa == PAUSA_COMA)
    if (candidato->tipo_pausa != PAUSA_COMA)
      coste += 4;

  if (candidato->tipo_pausa == PAUSA_COMA)
    if (objetivo->tipo_pausa != PAUSA_COMA)
      coste += 4;


  if (objetivo->tipo_pausa_anterior == PAUSA_PUNTO)
    if (candidato->tipo_pausa_anterior != PAUSA_PUNTO)
      coste += 4;

  if (candidato->tipo_pausa_anterior == PAUSA_PUNTO)
    if (objetivo->tipo_pausa_anterior != PAUSA_PUNTO)
      coste += 4;

  if (objetivo->tipo_pausa_anterior == PAUSA_COMA)
    if (candidato->tipo_pausa_anterior != PAUSA_COMA)
      coste += 4;

  if (candidato->tipo_pausa_anterior == PAUSA_COMA)
    if (objetivo->tipo_pausa_anterior != PAUSA_COMA)
      coste += 4;

  // Vamos a dejar lo del número de frase de momento, puesto que no tenemos la
  // información de "Punto y aparte" en Cotovía. Sería fácil de conseguir, pero
  // como en el corpus hay muy pocos ficheros que consten de más de una frase,
  // tampoco tiene demasiado sentido.
 
  // if (objetivo->posicion_grupo_acentual == 0)
  //   if (objetivo->posicion_grupo_prosodico == 0)
  //     if (candidato->posicion_grupo_prosodico != 0) // Siempre va a ser inicial, por el clustering.
  // 	coste += 3;
 
  // Coste de factor de caída al final de la frase:
  /*

    if ( (objetivo->posicion_grupo_acentual == objetivo->grupos_acentuales_grupo_prosodico - 1) &&
    (objetivo->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) )
    if (objetivo->tipo_proposicion != FRASE_INACABADA) {
    if (candidato->factor_caida >= -10)
    coste += 0.8;
    } // if (objetivo->tipo_proposicion != FRASE_INACABADA
    else
    if (candidato->factor_caida <= 0)
    coste += 0.5;
  */

  return coste;

}


/**
 * Función:   calcula_C_c
 * \remarks Entrada:
 *			- unidad1: unidad del paso anterior.                                         
 *          - unidad2: unidad del paso actual.                                           
 * \remarks Objetivo:  Calcula el coste de concatenación entre dos unidades.                      
 */

inline float Viterbi_acentual::calcula_C_c(Grupo_acentual_candidato *unidad1, Grupo_acentual_candidato *unidad2) {

  float coste = 0;
  float diferencia;
  float frec_dcha1 = unidad1->frecuencia.marcas[unidad1->frecuencia.numero_marcas - 1].frecuencia;
  //    float frec_media1 = unidad1->frecuencia_media;
  float frec_izda2 = unidad2->frecuencia.marcas[0].frecuencia;
  //    float frec_media2 = unidad2->frecuencia_media;

  //    numero_C_c++;

  //	if ( (unidad1->identificador == 734001) &&
  //    	 (unidad2->identificador == 744002) )
  //         puts("");

  if (unidad2->identificador == unidad1->identificador + 1)
    return 0.0; // Las unidades son consecutivas. Su coste de concatenación es nulo.

  if (fuerza_fronteras_entonativas) {
      if (unidad1->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)
	if (unidad2->identificador != unidad2->identificador + 1)
	  coste = 100;
  }

  diferencia = absoluto(frec_izda2 - frec_dcha1);

  if (diferencia > MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS) {
    diferencia -= MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS;
    if (diferencia < UMBRAL_CONTINUIDAD_F0)
      coste += 10*diferencia / maximo(frec_izda2, frec_dcha1);
    else
      if (diferencia < UMBRAL_CONTINUIDAD_F0_MAXIMO_COSTE)
	coste += 15*diferencia / maximo(frec_izda2, frec_dcha1);
      else
	coste += 30*diferencia / maximo(frec_izda2, frec_dcha1);
  }

  if (unidad1->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)
    if (unidad2->tipo_pausa_anterior != PAUSA_RUPTURA_ENTONATIVA)
      coste += 8;

  if (unidad2->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA)
    if (unidad1->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA)
      coste += 8;

  return coste;

}

/**
 * Función:   calcula_C_c                                                                
 * \remarks Entrada:
 *			- unidad1: unidad del paso anterior.                                         
 *          - unidad2: unidad del paso actual.                                           
 *			- unidad_objetivo: unidad objetivo del paso actual.							 
 * \remarks Objetivo:  Calcula el coste de concatenación entre dos unidades.                      
 * \remarks NOTA:	- Esta función sólo se llama en el caso del primer grupo acentual de todos   
 *			  los grupos fónicos que no sean el primero, para considerar la continuidad  
 *			  de f0 tras una pausa que no suponga fin de oración.						 
 * \remarks NOTA:	- Se va a repetir el mismo cálculo de la distancia para todas las unidades   
 *			  del primer paso de cada grupo fónico, excepto el primero. Lo dejo así por  
 *			  generalidad. Si cambiásemos el modelo y dependiese de otros factores de    
 *			  funcíón. Eso sí, si necesitásemos optimizar el código en velocidad, habría 
 *			  que cambiarlo, aunque esta parte no se va a repetir tantas veces como para 
 *			  que suponga una gran diferencia.											 
 */
//#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA

inline float Viterbi_acentual::calcula_C_c(Grupo_acentual_candidato *unidad1, Grupo_acentual_candidato *unidad2, Grupo_acentual_objetivo *unidad_objetivo) {

  float coste;
  float frec_dcha1 = unidad1->frecuencia.marcas[unidad1->frecuencia.numero_marcas - 1].frecuencia;
  float frec_izda2 = unidad2->frecuencia.marcas[0].frecuencia;
  float salto_f0 = absoluto(frec_izda2 - frec_dcha1);

#ifdef _CONTINUIDAD_FONICA_TRAS_PAUSA_CON_RED_NEURONAL
  
  double ideal;
  double datos_red[NUMERO_ENTRADAS_RED_CONTINUIDAD_FONICA], *apunta_datos;
  Media_varianza *apunta_media_varianza;

  //    numero_C_c++;
  apunta_datos = (double *) datos_red;
  apunta_media_varianza = locutor_actual->estadisticos_continuidad_fonica;

  *apunta_datos++ = ((double) unidad_objetivo->posicion_grupo_prosodico - apunta_media_varianza->media) / apunta_media_varianza->desviacion;
  apunta_media_varianza++;
  *apunta_datos++ = ((double) unidad_objetivo->grupos_acentuales_grupo_prosodico - apunta_media_varianza->media) / apunta_media_varianza->desviacion;
  apunta_media_varianza++;
  // El tamaño de pausa de 0.100 es el que se le asigna a las pausas relacionadas con una coma (pausas.cpp, pon_pausas_segun_tipo()).
  *apunta_datos++ = (0.100 - apunta_media_varianza->media) / apunta_media_varianza->desviacion;
  apunta_media_varianza++;
  *apunta_datos = (frec_dcha1 - apunta_media_varianza->media) / apunta_media_varianza->desviacion;

  locutor_actual->red_continuidad_fonica.simula_red((double *) datos_red, &ideal);

  //    printf("%f\n", distancia);
    
  // Tengo que buscar una solución mejor que ésta, en la que se consideren diferencias negativas y positivas.
  
  if (unidad_objetivo->tipo_proposicion == FRASE_INTERROGATIVA) {
    if (absoluto(frec_izda2 - ideal) > UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA_INTERROGATIVA)
      coste = 5*absoluto((frec_izda2 - ideal)) / maximo(frec_izda2, ideal);
    else
      coste = 0.0;
  }
  else
    if (absoluto(frec_izda2 - ideal) > UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA)
      coste = 10*absoluto((frec_izda2 - ideal)) / maximo(frec_izda2, ideal);
    else
      coste = 0.0;
  

#endif

  if (unidad1->tipo_grupo_prosodico_3 == FRASE_INTERROGATIVA) {
    if (salto_f0 > UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA_INTERROGATIVA)
      coste = 5*(salto_f0 - UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA_INTERROGATIVA) / maximo(frec_izda2, frec_dcha1);
    else
      coste = 0.0;
  }
  else
    if (salto_f0 > UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA)
      coste = 10*(salto_f0 - UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA) / maximo(frec_izda2, frec_dcha1);
    else
      coste = 0.0;

  return coste;

}
//#endif


/**
 * Función:   calcula_coste_minimo_hasta_unidad.                                         
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_acentual *C_ii: estructura en la que se almacena el coste 
 *            mínimo, el identificador de la propia unidad, el de aquella del paso       
 *            anterior que proporciona el coste mínimo, el propio coste y al puntero al  
 *            camino óptimo.                                                             
 * \remarks Objetivo:  Calcula mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).                           
 */

void Viterbi_acentual::calcula_coste_minimo_hasta_unidad(Estructura_coste_acentual *C_ii) {

  Grupo_acentual_candidato *unidad_actual =  C_ii->unidad;
  register float minimo = FLT_MAX;
  register float coste_actual;
  register int camino = 0;
  register Estructura_C_i_acentual *correcaminos = C_i;

  // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la
  // iteración anterior (i.e., los elementos de la cadena de estructuras C_i).

  for (register int contador = 0; contador < tamano_C_i; contador++) {
    // Calculamos el coste total hasta la unidad considerada en el paso actual.
#ifdef _BUSQUEDA_EXHAUSTIVA
    coste_actual = (1 - peso_obj_conc)*calcula_C_c(correcaminos->unidad, unidad_actual) +
      correcaminos->C_i;
#else
    coste_actual = calcula_C_c(correcaminos->unidad, unidad_actual) + correcaminos->C_i;
#endif
    // Si el coste el menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.
    if (coste_actual < minimo) {
      C_ii->id_coste_minimo = correcaminos->unidad->identificador;
      C_ii->coste_c = coste_actual;
      minimo = coste_actual;
      camino = contador;
    }
    correcaminos++;
  }

  // Apuntamos qué camino proporciona el coste mínimo y su valor.
  C_ii->camino = camino;
  C_ii->coste_c += C_ii->coste_unidad;
}


/**
 * Función:   calcula_coste_minimo_hasta_unidades.                                       
 * \remarks Entrada:
 *			- numero_candidatos: número de estructuras de vector_coste.                  
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_acentual *vector_coste: estructura en la que se almacena  
 *            el coste mínimo, el identificador de la propia unidad, el de aquella del   
 *            paso anterior que proporciona el coste mínimo, el propio coste y al        
 *            puntero al camino óptimo.                                                  
 * \remarks Objetivo:  Calcula mín( Cc(u, u-1) + Ci(u-1) ) + Ct(u, t)).                           
 */

void Viterbi_acentual::calcula_coste_minimo_hasta_unidades(Estructura_coste_acentual *vector_coste, int numero_candidatos) {

  Grupo_acentual_candidato *unidad_actual;
  Estructura_coste_acentual *recorre_costes = vector_coste;
  register float minimo;
  register float coste_actual;
  register int camino;
  register Estructura_C_i_acentual *correcaminos;
  register int contador;

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En calcula_coste_minimo_hasta_unidades (%d): (%d %d %d)\n", iteracion, vector_coste->unidad->tipo_grupo_prosodico_3, vector_coste->unidad->posicion_grupo_prosodico, vector_coste->unidad->posicion_silaba_acentuada);
#endif

  for (int cuenta = 0; cuenta < numero_candidatos; cuenta++, recorre_costes++) {

    // Se repite el bucle para cada unidad que haya proporcionado un camino de coste mínimo en la
    // iteración anterior (i.e., los elementos de la cadena de estructuras C_i).

    unidad_actual = recorre_costes->unidad;
    camino = 0;
    correcaminos = C_i;
    minimo = FLT_MAX;

#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "------> %d\n", unidad_actual->identificador);
#endif

    for (contador = 0; contador < tamano_C_i; contador++) {
      // Calculamos el coste total hasta la unidad considerada en el paso actual.
#ifdef _BUSQUEDA_EXHAUSTIVA
      coste_actual = (1 - peso_obj_conc)*calcula_C_c(correcaminos->unidad, unidad_actual) +
	correcaminos->C_i;
#else
      coste_actual = calcula_C_c(correcaminos->unidad, unidad_actual) + correcaminos->C_i;
#endif
      // Si el coste el menor que el mínimo, apuntamos el elemento de C_i que lo proporciona.
      if (coste_actual < minimo) {
	recorre_costes->id_coste_minimo = correcaminos->unidad->identificador;
	recorre_costes->coste_c = coste_actual;
	minimo = coste_actual;
	camino = contador;
      }
      correcaminos++;
    }

    // Apuntamos qué camino proporciona el coste mínimo y su valor.
    recorre_costes->camino = camino;
    recorre_costes->coste_c += recorre_costes->coste_unidad;

#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "---------> %d, (%f)\n", recorre_costes->id_coste_minimo, recorre_costes->coste_c);
#endif
    
  } // for (int cuenta = 0; ...

}


/**
 * Función:   crea_vector_C_t                                                             
 * \remarks Entrada:
 *			- delta_u: array con las unidades candidatas para una unidad objetivo.        
 *          - objetivo: identificador de la unidad considerada como objetivo.             
 *	\remarks Salida:
 *			- Estructura_coste_acentual **costes: array con los costes de unidad.         
 * \remarks Entrada y Salida:                                                                      
 *          - *tamano: inicialmente contiene el número de unidades almacenadas en delta_u.
 *            A la salida, almacena el tamaño de *costes, ya que puede haber poda.        
 * \remarks Objetivo:  Crea el array de estructuras de coste de unidad, incluyendo la posibilidad  
 *            de realizar poda, tanto por número como por valor.                          
 */

int Viterbi_acentual::crea_vector_C_t(Estructura_coste_acentual **costes, int *tamano, Grupo_acentual_candidato **delta_u, Grupo_acentual_objetivo *objetivo) {

  Estructura_coste_acentual *correcaminos;
  Grupo_acentual_candidato **puntero;
  int tamano_actual = 0;
  int contador;

#ifdef PODA_C_t_acentual
  int poda;
#endif

  correcaminos = *costes;
  puntero = delta_u;

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_t (%d):\n", iteracion);
#endif

  // Repetimos el bucle para cada elemento de la cadena de unidades candidatas.

  for (contador = 0; contador < *tamano; contador++) {

    // Calculamos el coste de unidad respecto al objetivo.
#ifdef _BUSQUEDA_EXHAUSTIVA
    correcaminos->coste_unidad = calcula_C_t_busqueda(*puntero, objetivo);
#else
    correcaminos->coste_unidad = calcula_C_t(*puntero, objetivo);
#endif
    // Anotamos el identificador de la unidad.
    correcaminos->unidad = *puntero;
    // Incrementamos la variable tamano_actual.
    tamano_actual++;


#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "------> %d (%f)\n", correcaminos->unidad->identificador, correcaminos->coste_unidad);
#endif

#ifdef PODA_C_t_acentual
    // Comprobamos si la unidad puede formar parte del camino óptimo.
    if ( (poda = ordena_y_poda_C_t(*costes, tamano_actual)) != 0) {
      tamano_actual -= poda; // En este caso, no puede.
      correcaminos = *costes + tamano_actual;
    }
    else
#endif
      correcaminos++;

    puntero++;

  } // for (int contador = 0; ...

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---------> Vector de costes: (tamano = %d)\n", tamano_actual);
  correcaminos = *costes;
  for (contador = 0; contador < tamano_actual; contador++, correcaminos++) {
    fprintf(fichero_depura, "------------> (%d %d %f)\n", contador, correcaminos->unidad->identificador, correcaminos->coste_unidad);
  }
#endif

  /*
  // Si ha habido poda, las unidades sobrantes están al final, así que llega con
  // liberar esa memoria.

  if (*tamano != tamano_actual) {
  *tamano = tamano_actual;
  if ( (*costes = (Estructura_coste_acentual *) realloc(*costes,
  tamano_actual*sizeof(Estructura_coste_acentual))) == NULL) {
  fprintf(stderr, "Error en crea_vector_C_t, al reasignar memoria.");
  return 1;
  }
  }
  */
  // Si ha habido poda, avisamos al programa llamante cambiando el valor de *tamano.
  // No liberamos la memoria, porque sería una pérdida de tiempo (sólo liberaríamos unas
  // posiciones). Ya se libera al final del ciclo por completo.

  *tamano = tamano_actual;

  return 0;
}



/**
 * \brief Función que crea el vector de costes de objetivo para la búsqueda de Viterbi por grupos fónicos
 * \param[in] grupos Vector de vectores con los grupos acentuales de la búsqueda anterior.
 * \param[in] costes Vector de costes, con los costes de la búsqueda anterior.
 * \param[in] tamano_grupo_fonico Número de grupos acentuales de cada grupo fónico
 * \param[out] **costes array con los costes de unidad.         
 * \return En ausencia de error, devuelve un cero.
 */

int Viterbi_acentual::crea_vector_C_t_grupos_fonicos(vector<Grupo_acentual_candidato *> &grupos, vector<float> &costes_fonicos, int tamano_grupo_fonico, Estructura_coste_acentual_GF *costes) {

  Estructura_coste_acentual_GF *correcaminos;
//   Grupo_acentual_candidato **puntero;
  int tamano_actual = costes_fonicos.size();
  int contador;

  correcaminos = costes;
//   puntero = delta_u;

 #ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_t_grupos_fonicos (%d):\n", iteracion);
#endif

  // Repetimos el bucle para cada elemento de la cadena de unidades candidatas.

  for (contador = 0; contador < tamano_actual; contador++) {

    correcaminos->coste_unidad = costes_fonicos[contador];
    correcaminos->numero_unidades = tamano_grupo_fonico;
    correcaminos->secuencia_unidades = grupos[contador];

#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "------> %d-%d (%f)\n", correcaminos->secuencia_unidades->identificador, (correcaminos->secuencia_unidades + correcaminos->numero_unidades - 1)->identificador, correcaminos->coste_unidad);
#endif

    correcaminos++;

   } // for (int contador = 0; ...

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---------> Vector de costes: (tamano = %d)\n", tamano_actual);
  correcaminos = costes;
  for (contador = 0; contador < tamano_actual; contador++, correcaminos++) {
    fprintf(fichero_depura, "------------> (%d %d-%d %f)\n", contador, correcaminos->secuencia_unidades->identificador, (correcaminos->secuencia_unidades + correcaminos->numero_unidades - 1)->identificador, correcaminos->coste_unidad);
  }
#endif

  return 0;

}


#ifdef PODA_C_i_acentual

/**
 * Función:   ordena_y_poda_C_i                                                          
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_acentual *inicio: array ordenado según el coste de unidad 
 *            de sus elementos.                                                          
 * \remarks Valor devuelto:                                                                       
 *           - Si no hay poda se devuelve un 0. Un 1 en caso contrario.                  
 * \remarks Objetivo:  ordena y realiza la poda del array por número.                             
 * \remarks NOTA:      De momento no se consideran como no podables aquellos elementos que tienen 
 *            C_c nulo en el siguiente paso.                                             
 */

int Viterbi_acentual::ordena_y_poda_C_i(Estructura_C_i_acentual *inicio, int posicion) {

  char poda_numero = 0, poda_valor = 0;
  //  int identificador, 
  int camino;
  //  float incremento_coste = 1.05;//1.05;
  Estructura_C_i_acentual *correcaminos = inicio;
  
  // La posición del nuevo elemento es la del tamaño - 1.

  posicion -= 1;

  register float coste = correcaminos[posicion].C_i;

  //  identificador = correcaminos[posicion].unidad->identificador;
  camino = correcaminos[posicion].camino;

#ifdef PODA_C_i_POR_VALOR_acentual

  register float umbral = minimo_c_i + C_PODA_C_i_acentual;

  if (coste > umbral)
    // no puede formar parte de la secuencia óptima. Lo eliminamos.
    return 1;

  // Comprobamos si el valor es menor que el mínimo actual.
  if (coste < minimo_c_i) {
    umbral = coste + C_PODA_C_i_acentual;
    minimo_c_i = coste;
  }

#endif

  int contador = 0;

  // Ordenamos el array.
  // Esto lo tengo que cambiar por una búsqueda binaria.

  while ( (correcaminos->C_i <= coste) && (contador < posicion) ) {
    //    if (correcaminos->unidad->identificador == identificador)
    //  coste *= incremento_coste;
    // if (correcaminos->camino == camino)
    //   coste *= incremento_coste;
    contador++;
    correcaminos++;
  }

  inicio[posicion].C_i = coste;
  
  if (contador != posicion) {
    Estructura_C_i_acentual nuevo = inicio[posicion];

    correcaminos = inicio + posicion;

    for (int cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef PODA_C_i_POR_VALOR_acentual
      if (correcaminos->C_i > umbral)
	poda_valor = (short int) (posicion - cuenta);
#endif
      *correcaminos = *(correcaminos - 1);
      correcaminos--;
    }

    *correcaminos = nuevo;

  }

#ifdef PODA_C_i_POR_NUMERO_acentual
  if (posicion >= N_PODA_Ci_acentual) // El >= viene del posicion -= 1.
    //       poda_numero = posicion - N_PODA_UNIDAD_acentual;
    poda_numero = 1;
#endif

  // Devolvemos el valor que produce una poda mayor.

  if (poda_valor > poda_numero)
    return poda_valor;

  return poda_numero;

}


/**
 * Función:   ordena_y_poda_C_i_GF                                                          
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_acentual *inicio: array ordenado según el coste de unidad 
 *            de sus elementos.                                                          
 * \remarks Valor devuelto:                                                                       
 *           - Si no hay poda se devuelve un 0. Un 1 en caso contrario.                  
 * \remarks Objetivo:  ordena y realiza la poda del array por número.                             
 * \remarks NOTA:      De momento no se consideran como no podables aquellos elementos que tienen 
 *            C_c nulo en el siguiente paso.                                             
 */

int Viterbi_acentual::ordena_y_poda_C_i_GF(Estructura_C_i_acentual_GF *inicio, int posicion) {

  char poda_numero = 0, poda_valor = 0;
  //  int identificador, 
  int camino;
  Estructura_C_i_acentual_GF *correcaminos = inicio;
  
  // La posición del nuevo elemento es la del tamaño - 1.

  posicion -= 1;

  register float coste = correcaminos[posicion].C_i;

  //  identificador = correcaminos[posicion].unidad->identificador;
  camino = correcaminos[posicion].camino;

#ifdef PODA_C_i_POR_VALOR_acentual

  register float umbral = minimo_c_i + C_PODA_C_i_acentual;

  if (coste > umbral)
    // no puede formar parte de la secuencia óptima. Lo eliminamos.
    return 1;

  // Comprobamos si el valor es menor que el mínimo actual.
  if (coste < minimo_c_i) {
    umbral = coste + C_PODA_C_i_acentual;
    minimo_c_i = coste;
  }

#endif

  int contador = 0;

  // Ordenamos el array.
  // Esto lo tengo que cambiar por una búsqueda binaria.

  while ( (correcaminos->C_i <= coste) && (contador < posicion) ) {
    contador++;
    correcaminos++;
  }

  inicio[posicion].C_i = coste;

  if (contador != posicion) {
    Estructura_C_i_acentual_GF nuevo = inicio[posicion];

    correcaminos = inicio + posicion;

    for (int cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef PODA_C_i_POR_VALOR_acentual
      if (correcaminos->C_i > umbral)
	poda_valor = (short int) (posicion - cuenta);
#endif
      *correcaminos = *(correcaminos - 1);
      correcaminos--;
    }

    *correcaminos = nuevo;

  }

#ifdef PODA_C_i_POR_NUMERO_acentual
  if (posicion >= N_PODA_Ci_acentual) // El >= viene del posicion -= 1.
    //       poda_numero = posicion - N_PODA_UNIDAD_acentual;
    poda_numero = 1;
#endif

  // Devolvemos el valor que produce una poda mayor.

  if (poda_valor > poda_numero)
    return poda_valor;

  return poda_numero;

}

#endif

#ifdef PODA_C_t_acentual

/**
 * Función:   ordena_y_poda_C_t                                                          
 * \remarks Entrada:
 *			- posicion: índice de la estructura que se va a añadir.                      
 * \remarks Entrada y Salida:                                                                     
 *          - Estructura_coste_acentual *inicio: array ordenado según el coste de unidad 
 *            de sus elementos.                                                          
 * \remarks Valor devuelto:                                                                       
 *           - Se devuelve el número de elementos podados.                               
 * \remarks Objetivo:  ordena y realiza la poda del array, ya sea por número o por valor.         
 * \remarks NOTA:      De momento no se consideran como no podables aquellos elementos que tienen 
 *            C_c nulo en el siguiente paso.                                             
 */

int Viterbi_acentual::ordena_y_poda_C_t(Estructura_coste_acentual *inicio, int posicion) {

  char poda_numero = 0, poda_valor = 0;
  Estructura_coste_acentual *correcaminos = inicio;

  // La posición en la que está situado el nuevo elemento es la del tamaño menos 1.
  posicion -= 1;

  register float coste = correcaminos[posicion].coste_unidad;

#ifdef PODA_C_t_POR_VALOR_acentual

  register float umbral = minimo_c_t + C_PODA_UNIDAD_acentual;

  if (coste > umbral)
    // no puede formar parte de la secuencia óptima. Lo eliminamos.
    return 1;

  // Si el coste es menor que el mínimo, actualizamos su valor.
       
  if (coste < minimo_c_t) {
    umbral = coste + C_PODA_UNIDAD_acentual;
    minimo_c_t = coste;
  }

#endif
  int contador = 0;

  // Ordenamos el array.
  // Esto lo tengo que cambiar por una búsqueda binaria.

  while ( (correcaminos->coste_unidad <= coste) && (contador < posicion) ) {
    contador++;
    correcaminos++;
  }

  // if (posicion != 0) {
  //   if (correcaminos->coste_unidad  == coste) {
  //     int aleatorio = rand() % 10;
  //     if (aleatorio < 5) {
  // 	contador--;
  // 	correcaminos--;
  //     }
  //   }
  // }

  if (contador != posicion) {
    Estructura_coste_acentual nuevo = inicio[posicion];

    correcaminos = inicio + posicion;

    for (int cuenta = posicion; cuenta > contador; cuenta--) {

#ifdef PODA_C_t_POR_VALOR_acentual
      if (correcaminos->coste_unidad > umbral)
	poda_valor = (short int) (posicion - cuenta);
#endif
      *correcaminos = *(correcaminos - 1); // YA QUE SON POSICIONES CONSECUTIVAS EN MEMORIA,
      correcaminos--;  // DEBERÍA INTENTAR CAMBIARLO POR UN SIMPLE MEMMOVE.
    }

    *correcaminos = nuevo;

  }

#ifdef PODA_C_t_POR_NUMERO_acentual
  if (posicion >= N_PODA_UNIDAD_acentual) // El >= viene del posicion -= 1.
    poda_numero = (char) 1;
#endif

  // Devolvemos el valor que produce una poda mayor.
    
  if (poda_valor > poda_numero)
    return poda_valor;
  return poda_numero;

}

#endif


/**
 * Función:   crea_vector_C_i                                                             
 * \remarks Entrada:
 *			- Estructura_coste_acentual *C_concatena: array, ya podado, con los costes de 
 *            caminos más cortos a cada unidad.                                           
 *          - tamano: contiene la longitud de C_concatena.
 * \remarks Valor devuelto:
 *           - Devuelve 0 si no se produce ningún error.
 * \remarks Objetivo:  crea la cadena de caminos mínimos hasta el paso actual del algoritmo.
 * \remarks Esta función actuamente está obsoleta. Para el mismo cometido hay que usar crea_vector_C_i_camino_unico
 */

int Viterbi_acentual::crea_vector_C_i(Estructura_coste_acentual *C_concatena, int tamano) {

  int contador;
  int tamano_actual = 0;
  Estructura_coste_acentual *correcaminos = C_concatena;

#ifdef PODA_C_i_acentual
  int poda;
#endif

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_i (%d)\n", iteracion);
#endif

#ifndef PODA_C_i_POR_NUMERO_acentual
  // Liberamos la memoria que estaba reservada para la variable C_i.

  if ( (tamano > tamano_C_i) || (tamano > memoria_reservada_C_i) ) {
    if (C_i)
      free(C_i);

    // Le asignamos memoria de nuevo.
    if ( (C_i = (Estructura_C_i_acentual *) malloc(tamano*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }

    memoria_reservada_C_i = tamano;
  }

#else

  if ( (tamano > tamano_C_i) && (maxima_memoria_reservada == 0) ) {

    // Liberamos la memoria que estaba reservada anterirmente.
    if (C_i)
      free(C_i);

    // Le asignamos memoria de nuevo.
    if ( (C_i = (Estructura_C_i_acentual *) malloc((N_PODA_Ci_acentual + 1)*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }

    maxima_memoria_reservada = 1;

  }


#endif


  Estructura_C_i_acentual *recorre_C_i = C_i;

  // Vamos rellenando los campos de cada estructura del array.

  for (contador = 0; contador < tamano; contador++) {
    // Añadimos la propia unidad.
    recorre_C_i->unidad = correcaminos->unidad;
    // El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
    recorre_C_i->camino = correcaminos->camino;
    // El coste mínimo para llegar hasta dicha unidad.
    recorre_C_i->C_i = correcaminos->coste_c;
    tamano_actual++;

#ifdef PODA_C_i_acentual
    // Comprobamos si hay que realizar algún tipo de poda.
    if ( ( poda = ordena_y_poda_C_i(C_i, tamano_actual)) != 0) {
      tamano_actual -= poda;
      recorre_C_i = C_i + tamano_actual;
    }
    else
#endif
      recorre_C_i++;
    correcaminos++;
  }

  // Actualizamos el número de elementos de la cadena C_i.

  tamano_C_i = tamano_actual;

   
  // Actualizamos el número de filas (por si ha habido poda):

  if (tamano != tamano_actual)
    matriz.actualiza_numero_de_filas(tamano_actual);

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "------> tamano = %d (índice, identificador, coste, camino)\n", tamano_actual);
#endif

  // Actualizamos la matriz

  recorre_C_i = C_i;

  for (contador = 0; contador < tamano_actual; contador++) {
    if (matriz.introduce_elemento(contador, recorre_C_i->unidad, recorre_C_i->camino))
      return 1;
#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "---------> %d (%d %f %d)\n", contador, recorre_C_i->unidad->identificador, recorre_C_i->C_i, recorre_C_i->camino);
#endif

    recorre_C_i++;
  }

  // Actualizamos el número de filas (por si ha habido poda):
  //   if (tamano != tamano_actual)
  //	   	matriz.actualiza_numero_de_filas(tamano_actual);

  /*
    if (matriz.introduce_conjunto_elementos(C_i, tamano_C_i))
    return 1;
  */
  return 0;

}


/**
 * Función:   crea_vector_C_i_V2
 * \remarks Entrada:
 *			- Estructura_coste_acentual *C_concatena: array, ya podado, con los costes de 
 *            caminos más cortos a cada unidad.                                           
 *          - tamano: contiene la longitud de C_concatena.                                
 *			- objetivo: grupo acentual buscado.											  
 * \remarks Valor devuelto:                                                                        
 *           - Devuelve 0 si no se produce ningún error.                                  
 * \remarks Objetivo:  crea la cadena de caminos mínimos hasta el paso actual del algoritmo.       
 * \remarks NOTA:	 - Se diferencia de la función anterior en que en esta se considera más de un 
 *			   camino hasta cada unidad de la iteración actual.							  
 */
#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
int Viterbi_acentual::crea_vector_C_i_V2(Estructura_coste_acentual *C_concatena, int tamano, Grupo_acentual_objetivo *objetivo) {
#else
  int Viterbi_acentual::crea_vector_C_i_V2(Estructura_coste_acentual *C_concatena, int tamano) {
#endif
    
    int contador;
    int tamano_actual = 0;
    float coste_concatenacion;
    Estructura_coste_acentual *correcaminos;
    Estructura_C_i_acentual *nuevo_C_i;


#ifdef PODA_C_i_acentual
    int poda;
#else
    int tamano_total = tamano*tamano_C_i;
#endif

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_i (%d)\n", iteracion);
#endif

#ifndef PODA_C_i_POR_NUMERO_acentual

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual *) malloc(tamano_total*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }


#else

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual *) malloc((N_PODA_Ci_acentual + 1)*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }

#endif


    Estructura_C_i_acentual *recorre_C_i, *recorre_nuevo_C_i;


    correcaminos = C_concatena;
    recorre_nuevo_C_i = nuevo_C_i;

    for (contador = 0; contador < tamano; contador++, correcaminos++) {

      recorre_C_i = C_i;

      for (int cuenta_C_i = 0; cuenta_C_i < tamano_C_i; cuenta_C_i++, recorre_C_i++) {

	// Añadimos la unidad
	recorre_nuevo_C_i->unidad = correcaminos->unidad;
	// El camino (la fila en la matriz de caminos óptimos) que proporciona ese coste
	recorre_nuevo_C_i->camino = cuenta_C_i;
	// El coste para llegar a dicha unidad por ese camino
#ifndef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
	coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad);
	recorre_nuevo_C_i->C_i = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion + recorre_C_i->C_i;
#else
	if ( (objetivo->posicion_grupo_acentual == 0) && (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_ENTONATIVA) && (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_COMA) ) {
	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad, objetivo);
	}
	else {
	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad);
	}
	recorre_nuevo_C_i->C_i = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion + recorre_C_i->C_i;
	
#endif

	recorre_nuevo_C_i->id_coste_minimo = recorre_C_i->unidad->identificador;
	recorre_nuevo_C_i->coste_unidad = correcaminos->coste_unidad;
	recorre_nuevo_C_i->coste_concatenacion = coste_concatenacion;
	recorre_nuevo_C_i->coste_anterior = recorre_C_i->C_i;

	tamano_actual++;

#ifdef PODA_C_i_acentual
      if ( (poda = ordena_y_poda_C_i(nuevo_C_i, tamano_actual)) != 0) {
	tamano_actual -= poda;
	recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
      }
      else
	recorre_nuevo_C_i++;
#else
      recorre_nuevo_C_i++;
#endif

      } // for (int cuenta_C_i = 0; ...
      
  } // for (contador = 0;

//     recorre_nuevo_C_i = nuevo_C_i;
//     recorre_C_i = C_i;

//     // Vamos rellenando los campos de cada estructura del array.

//     for (int cuenta_C_i = 0; cuenta_C_i < tamano_C_i; cuenta_C_i++, recorre_C_i++) {

//       correcaminos = C_concatena;

//       for (contador = 0; contador < tamano ; contador++) {
// 	// Añadimos la propia unidad.
// 	recorre_nuevo_C_i->unidad = correcaminos->unidad;
// 	// El camino (fila de la matriz de caminos óptimos) que proporciona coste mínimo.
// 	recorre_nuevo_C_i->camino = cuenta_C_i;
// 	// El coste mínimo para llegar hasta dicha unidad.
// #ifndef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
// 	recorre_nuevo_C_i->C_i = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad +
// 	  PESO_COSTE_CONCATENACION_ACENTUAL*calcula_C_c(recorre_C_i->unidad, correcaminos->unidad) +
// 	  recorre_C_i->C_i;
// #else


// 	if ((objetivo->posicion_grupo_acentual == 0) && (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_ENTONATIVA) &&
// 	    (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_COMA) ) {
// 	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad, objetivo);
// 	  recorre_nuevo_C_i->C_i = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion +
// 	    recorre_C_i->C_i;
// 	}
// 	else {
// 	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad);
// 	  recorre_nuevo_C_i->C_i = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion +
// 	    recorre_C_i->C_i;
// 	}

// #endif

// 	tamano_actual++;

// #ifdef PODA_C_i_acentual
// 	// Comprobamos si hay que realizar algún tipo de poda.
// 	if ( ( poda = ordena_y_poda_C_i(nuevo_C_i, tamano_actual)) != 0) {
// 	  tamano_actual -= poda;
// 	  recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
// 	}
// 	else
// #endif
// 	  recorre_nuevo_C_i++;
// 	correcaminos++;
//       } // for (contador = 0; ...
      
//     } // for (cuenta_C_i...


    // Reservamos memoria para la matriz de caminos óptimos.

    if (matriz.reinicia_ciclo(tamano_actual))
      return 1;

    // Actualizamos el número de elementos de la cadena C_i.

    tamano_C_i = tamano_actual;

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "------> tamano = %d (índice, identificador, coste [unidad, concatenacion, coste_anterior], camino [id_anterior])\n", tamano_actual);
#endif

    // Actualizamos la matriz

    recorre_C_i = nuevo_C_i;

    for (contador = 0; contador < tamano_actual; contador++) {
      if (matriz.introduce_elemento(contador, recorre_C_i->unidad, recorre_C_i->camino))
	return 1;
#ifdef _DEPURA_VITERBI_ACENTUAL
      fprintf(fichero_depura, "---------> %d (%d %f [%f %f %f] %d [%d])\n", contador, recorre_C_i->unidad->identificador, recorre_C_i->C_i, recorre_C_i->coste_unidad, recorre_C_i->coste_concatenacion, recorre_C_i->coste_anterior, recorre_C_i->camino, recorre_C_i->id_coste_minimo);
#endif
      recorre_C_i++;
    } // for (contador = 0;

    free(C_i);
    C_i = nuevo_C_i;
    
    return 0;

  }

/**
 * Función:   crea_vector_C_i_V2_unico_camino
 * \remarks Entrada:
 *			- Estructura_coste_acentual *C_concatena: array, ya podado, con los costes de 
 *            caminos más cortos a cada unidad.                                           
 *          - tamano: contiene la longitud de C_concatena.                                
 *			- objetivo: grupo acentual buscado.											  
 * \remarks Valor devuelto:                                                                        
 *           - Devuelve 0 si no se produce ningún error.                                  
 * \remarks Objetivo:  crea la cadena de caminos mínimos hasta el paso actual del algoritmo.       
 * \remarks NOTA:	 - Variante de la función anterior que considera un único camino hasta la unidad actual.
 */
#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
int Viterbi_acentual::crea_vector_C_i_V2_unico_camino(Estructura_coste_acentual *C_concatena, int tamano, Grupo_acentual_objetivo *objetivo) {
#else
  int Viterbi_acentual::crea_vector_C_i_V2_unico_camino(Estructura_coste_acentual *C_concatena, int tamano) {
#endif
    
    int contador;
    int tamano_actual = 0;
    int indice_mejor_camino;
    float coste_concatenacion;
    float minimo_coste, coste_actual, mejor_coste_concatenacion, anterior_mejor_coste;
    Estructura_coste_acentual *correcaminos;
    Estructura_C_i_acentual *nuevo_C_i;
    Estructura_C_i_acentual *mejor_camino;

    map<int, float> caminos_anteriores;
    map<int, float>::iterator it_caminos;

    caminos_anteriores.clear();

#ifdef PODA_C_i_acentual
    int poda;
#else
    int tamano_total = tamano*tamano_C_i;
#endif

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_i (%d)\n", iteracion);
#endif

#ifndef PODA_C_i_POR_NUMERO_acentual

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual *) malloc(tamano_total*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }


#else

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual *) malloc((N_PODA_Ci_acentual + 1)*sizeof(Estructura_C_i_acentual))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }

#endif


    Estructura_C_i_acentual *recorre_C_i, *recorre_nuevo_C_i;


    correcaminos = C_concatena;
    recorre_nuevo_C_i = nuevo_C_i;

    
    for (contador = 0; contador < tamano; contador++, correcaminos++) {

      recorre_C_i = C_i;
      minimo_coste = FLT_MAX;

      for (int cuenta_C_i = 0; cuenta_C_i < tamano_C_i; cuenta_C_i++, recorre_C_i++) {


#ifndef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
	coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad);
	coste_actual = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion + recorre_C_i->C_i;
#else
	if ( (objetivo->posicion_grupo_acentual == 0) && (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_ENTONATIVA) && (objetivo->tipo_pausa_anterior != PAUSA_RUPTURA_COMA) ) {
	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad, objetivo);
	}
	else {
	  coste_concatenacion = calcula_C_c(recorre_C_i->unidad, correcaminos->unidad);
	}
	coste_actual = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion + recorre_C_i->C_i;

#endif

	if (coste_actual < minimo_coste) {
	  if ( (it_caminos = caminos_anteriores.find(cuenta_C_i)) != caminos_anteriores.end()) {
	    // Ese camino ya fue escogido
	    if (it_caminos->second < coste_actual) {
	      coste_actual += INCREMENTO_VARIABILIDAD_CAMINOS;
	    }
	  }

	  minimo_coste = coste_actual;
	  indice_mejor_camino = cuenta_C_i;
	  mejor_coste_concatenacion = coste_concatenacion;
	  mejor_camino = recorre_C_i;
	}
      } // if (coste_actual < minimo_coste)
      
      // Actualizamos el map:

      if ( (it_caminos = caminos_anteriores.find(indice_mejor_camino)) != caminos_anteriores.end() ) {
	if (it_caminos->second > minimo_coste)
	  it_caminos->second = minimo_coste;
      }
      else
	caminos_anteriores[indice_mejor_camino] = minimo_coste;

      // Añadimos la unidad
      recorre_nuevo_C_i->unidad = correcaminos->unidad;
      // El camino (la fila en la matriz de caminos óptimos) que proporciona ese coste
      recorre_nuevo_C_i->camino = indice_mejor_camino;
      // El coste para llegar a dicha unidad por ese camino
      recorre_nuevo_C_i->id_coste_minimo = mejor_camino->unidad->identificador;
      recorre_nuevo_C_i->coste_unidad = correcaminos->coste_unidad;
      recorre_nuevo_C_i->coste_concatenacion = mejor_coste_concatenacion;
      recorre_nuevo_C_i->coste_anterior = mejor_camino->C_i;
      recorre_nuevo_C_i->C_i = minimo_coste;

      tamano_actual++;

#ifdef PODA_C_i_acentual
      if ( (poda = ordena_y_poda_C_i(nuevo_C_i, tamano_actual)) != 0) {
	tamano_actual -= poda;
	recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
      }
      else
	recorre_nuevo_C_i++;
#else
      recorre_nuevo_C_i++;
#endif

    } // for (contador = 0;
    
    // Reservamos memoria para la matriz de caminos óptimos.

    if (matriz.reinicia_ciclo(tamano_actual))
      return 1;

    // Actualizamos el número de elementos de la cadena C_i.

    tamano_C_i = tamano_actual;

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "------> tamano = %d (índice, identificador, coste [unidad, concatenacion, coste_anterior], camino [id_anterior])\n", tamano_actual);
#endif

    // Actualizamos la matriz

    recorre_C_i = nuevo_C_i;

    for (contador = 0; contador < tamano_actual; contador++) {
      if (matriz.introduce_elemento(contador, recorre_C_i->unidad, recorre_C_i->camino))
	return 1;
#ifdef _DEPURA_VITERBI_ACENTUAL
      fprintf(fichero_depura, "---------> %d (%d %f [%f %f %f] %d [%d])\n", contador, recorre_C_i->unidad->identificador, recorre_C_i->C_i, recorre_C_i->coste_unidad, recorre_C_i->coste_concatenacion, recorre_C_i->coste_anterior, recorre_C_i->camino, recorre_C_i->id_coste_minimo);
#endif
      recorre_C_i++;
    } // for (contador = 0;

    free(C_i);
    C_i = nuevo_C_i;
    
    return 0;

  }


/**
 * \brief Calcula el vector C_i para el algoritmo de Viterbi acentual por grupos fónicos.
 * \param[in] C_concatena array, ya podado, con los costes de caminos más cortos a cada unidad.                                           
 * \param[in] tamano contiene la longitud de C_concatena.                       
 */

int Viterbi_acentual::crea_vector_C_i_grupos_fonicos(Estructura_coste_acentual_GF *C_concatena, int tamano) {
    
    int contador;
    int tamano_actual = 0;
    int indice_mejor_camino;
    int numero_unidades_grupo_fonico;
    float coste_concatenacion;
    float minimo_coste, coste_actual;
    Estructura_coste_acentual_GF *correcaminos;
    Estructura_C_i_acentual_GF *nuevo_C_i;
    Estructura_C_i_acentual_GF *mejor_camino;

    // map<int, float> caminos_anteriores;
    // map<int, float>::iterator it_caminos;

    //    caminos_anteriores.clear();

#ifdef PODA_C_i_acentual
    int poda;
#else
    int tamano_total = tamano*tamano_C_i;
#endif

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "---> En crea_vector_C_i_grupos_fonicos (%d)\n", iteracion);
#endif

  //#ifndef PODA_C_i_POR_NUMERO_acentual

#ifndef PODA_C_i_POR_NUMERO_acentual

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual_GF *) malloc(tamano_total*sizeof(Estructura_C_i_acentual_GF))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }

#else

    // Le asignamos memoria de nuevo.
    if ( (nuevo_C_i = (Estructura_C_i_acentual_GF *) malloc((N_PODA_Ci_acentual + 1)*sizeof(Estructura_C_i_acentual_GF))) == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }


#endif

    Estructura_C_i_acentual_GF *recorre_C_i, *recorre_nuevo_C_i;

    correcaminos = C_concatena;
    recorre_nuevo_C_i = nuevo_C_i;

    numero_unidades_grupo_fonico = correcaminos->numero_unidades;

    for (contador = 0; contador < tamano; contador++, correcaminos++) {

      recorre_C_i = C_i_GF;
      //      minimo_coste = FLT_MAX;

      for (int cuenta_C_i = 0; cuenta_C_i < tamano_C_i; cuenta_C_i++, recorre_C_i++) {

	coste_concatenacion = calcula_C_c(recorre_C_i->secuencia_unidades + recorre_C_i->numero_unidades - 1, correcaminos->secuencia_unidades, NULL);
	coste_actual = PESO_COSTE_OBJETIVO_ACENTUAL*correcaminos->coste_unidad + PESO_COSTE_CONCATENACION_ACENTUAL*coste_concatenacion + recorre_C_i->C_i;
	
      // 	if (coste_actual < minimo_coste) {
      // 	  // if ( (it_caminos = caminos_anteriores.find(cuenta_C_i)) != caminos_anteriores.end()) {
      // 	  //   // Ese camino ya fue escogido
      // 	  //   if (it_caminos->second < coste_actual) {
      // 	  //     coste_actual += INCREMENTO_VARIABILIDAD_CAMINOS;
      // 	  //   }
      // 	  // }
	  
      // 	  minimo_coste = coste_actual;
      // 	  indice_mejor_camino = cuenta_C_i;
      // 	  mejor_coste_concatenacion = coste_concatenacion;
      // 	  mejor_camino = recorre_C_i;
      // 	} // if (coste_actual < minimo_coste)
      // } 
      
      // Añadimos la unidad
	recorre_nuevo_C_i->secuencia_unidades = correcaminos->secuencia_unidades;
	recorre_nuevo_C_i->numero_unidades = numero_unidades_grupo_fonico;
	// El camino (la fila en la matriz de caminos óptimos) que proporciona ese coste
	recorre_nuevo_C_i->camino = cuenta_C_i;
	// El coste para llegar a dicha unidad por ese camino
	recorre_nuevo_C_i->id_coste_minimo = (recorre_C_i->secuencia_unidades + recorre_C_i->numero_unidades - 1)->identificador;
	recorre_nuevo_C_i->coste_unidad = correcaminos->coste_unidad;
	recorre_nuevo_C_i->coste_concatenacion = coste_concatenacion;
	recorre_nuevo_C_i->coste_anterior = recorre_C_i->C_i;
	recorre_nuevo_C_i->C_i = coste_actual;
	
	tamano_actual++;
	
#ifdef PODA_C_i_acentual
	if ( (poda = ordena_y_poda_C_i_GF(nuevo_C_i, tamano_actual)) != 0) {
	  tamano_actual -= poda;
	  recorre_nuevo_C_i = nuevo_C_i + tamano_actual;
	}
	else
	  recorre_nuevo_C_i++;
#else
	recorre_nuevo_C_i++;
#endif

      } // for (int cuenta_C_i

    } // for (contador = 0;
    
    // Reservamos memoria para la matriz de caminos óptimos.

    if (matriz.reinicia_ciclo(tamano_actual))
      return 1;

    // Actualizamos el número de elementos de la cadena C_i.

    tamano_C_i = tamano_actual;

#ifdef _DEPURA_VITERBI_ACENTUAL
  fprintf(fichero_depura, "------> tamano = %d (índice, identificador, coste [unidad, concatenacion, coste_anterior], camino [id_anterior])\n", tamano_actual);
#endif

    // Actualizamos la matriz

    recorre_C_i = nuevo_C_i;

    for (contador = 0; contador < tamano_actual; contador++) {
      if (matriz.introduce_elemento(contador, recorre_C_i->secuencia_unidades, recorre_C_i->camino))
	return 1;
#ifdef _DEPURA_VITERBI_ACENTUAL
      fprintf(fichero_depura, "---------> %d (%d-%d %f [%f %f %f] %d [%d])\n", contador, recorre_C_i->secuencia_unidades->identificador, (recorre_C_i->secuencia_unidades + recorre_C_i->numero_unidades - 1)->identificador, recorre_C_i->C_i, recorre_C_i->coste_unidad, recorre_C_i->coste_concatenacion, recorre_C_i->coste_anterior, recorre_C_i->camino, recorre_C_i->id_coste_minimo);
#endif
      recorre_C_i++;
    } // for (contador = 0;

    free(C_i);
    C_i_GF = nuevo_C_i;
    
    return 0;

  }

  /**
   * \brief Función que ejecuta la primera iteración del algoritmo de Viterbi para la selección de contornos entonativos
   * \param[in] objetivo primer grupo acentual objetivo.
   * \param[in] numero_objetivos número de grupos acentuales de la oración.
   * \param[in] inicio_nombre_fichero_depura Inicio del nombre del fichero de depuración
   * \return Devuelve 0 en ausencia de error.                                   
   */

 int Viterbi_acentual::inicia_algoritmo_Viterbi_acentual(Grupo_acentual_objetivo *objetivo, int numero_objetivos, char *inicio_nombre_fichero_depura) {

    Grupo_acentual_candidato **delta_u;
    int numero_candidatos;
    int contador;
    Estructura_coste_acentual *correcaminos;

#ifdef _DEPURA_VITERBI_ACENTUAL
    
    char nombre_fichero[FILENAME_MAX];
    iteracion = 1;

    sprintf(nombre_fichero_depura, "%s_%d", inicio_nombre_fichero_depura, iteracion_gf);
    sprintf(nombre_fichero, "%s_%d.txt", nombre_fichero_depura, iteracion);

    if ( (fichero_depura = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en inicia_algoritmo_Viterbi_acentual, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_depura = ...

    fprintf(fichero_depura, "Inicio del algoritmo de Viterbi (%d): (%d %d %d)\n", iteracion, objetivo->tipo_proposicion, objetivo->posicion_grupo_acentual, objetivo->posicion_silaba_acentuada);

#endif

#ifdef PODA_C_t_POR_VALOR_acentual

    inicia_poda_C_t_acentual();

#endif

#ifdef PODA_C_i_POR_VALOR_acentual

    inicia_poda_C_i_acentual();

#endif


    for (contador = 0; contador < numero_contornos; contador++)
      indices_escogidos[contador] = -1;

    // Creamos la cadena de unidades candidatas.

    if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
      return 1;

    // Calculamos el coste a cada unidad. En este caso, no se considera el coste de concatenación.

    if ( (vector_coste = (Estructura_coste_acentual *) malloc(numero_candidatos*sizeof(Estructura_coste_acentual)))
	 == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }


    if (crea_vector_C_t(&vector_coste, &numero_candidatos, delta_u, objetivo))
      return 1;

    tamano_vector_coste = numero_candidatos;

    // Iniciamos la matriz que se emplea para memorizar los caminos de coste mínimo a cada unidad.

    if (matriz.inicializa_matriz(numero_candidatos, numero_objetivos))
      return 1;

    correcaminos = vector_coste;

    // Creamos la cadena de costes.

    for(contador = 0; contador < numero_candidatos; contador++) {
      correcaminos->coste_c = correcaminos->coste_unidad;
      correcaminos->camino = contador;
      correcaminos++;
    }

    // Creamos el vector C_i

    if (crea_vector_C_i(vector_coste, numero_candidatos))
      return 1;

    // Liberamos la memoria reservada.

    free(delta_u);

#ifdef _DEPURA_VITERBI_ACENTUAL
    fclose(fichero_depura);
#endif

    return 0;

  }


  /**
   * Función:   siguiente_recursion_Viterbi                                              
   * \remarks Entrada:
   *			- objetivo: vector descriptor de la unidad que hay que sintetizar.         
   * \remarks Valor devuelto:                                                                     
   *          - Devuelve 0 en ausencia de errores.                                       
   * \remarks Objetivo:  Ejecuta una iteración del algoritmo de Viterbi, actualizando los caminos 
   *            óptimos para cada unidad candidata.                                      
   */

  int Viterbi_acentual::siguiente_recursion_Viterbi(Grupo_acentual_objetivo *objetivo) {

    Grupo_acentual_candidato **delta_u;
    int numero_candidatos;

#ifdef _DEPURA_VITERBI_ACENTUAL
    char nombre_fichero[FILENAME_MAX];

    iteracion++;

    sprintf(nombre_fichero, "%s_%d.txt", nombre_fichero_depura, iteracion);

    if ( (fichero_depura = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en siguiente_recursion_Viterbi, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_depura = ...

    fprintf(fichero_depura, "Siguiente recursión, %d: (%d %d &d)\n", iteracion, objetivo->tipo_proposicion, objetivo->posicion_grupo_acentual, objetivo->posicion_silaba_acentuada);

#endif

#ifdef PODA_C_t_POR_VALOR_acentual

    inicia_poda_C_t_acentual();

#endif

#ifdef PODA_C_i_POR_VALOR_acentual

    inicia_poda_C_i_acentual();

#endif

    // Avanzamos la columna de la matriz de caminos óptimos sobre la que estamos trabajando.

    matriz.avanza_columna();

    // Creamos la cadena de unidades candidatas.

    if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
      return 1;

    // Calculamos el coste de distorsión de unidad de cada candidata.

    if (tamano_vector_coste < numero_candidatos) {

      if (vector_coste)
	free(vector_coste);

      if ( (vector_coste = (Estructura_coste_acentual *) malloc(numero_candidatos*sizeof(Estructura_coste_acentual)))
	   == NULL) {
	fprintf(stderr, "Error al asignar memoria.");
	return 1;
      }

      tamano_vector_coste = numero_candidatos;
    }


    if (crea_vector_C_t(&vector_coste, &numero_candidatos, delta_u, objetivo))
      return 1;

    //    tamano_vector_coste = numero_candidatos;

    // Reservamos espacio en la matriz de memoria de caminos óptimos.

    if (matriz.reinicia_ciclo(numero_candidatos))
      return 1;

    // Actualizamos el vector de costes mínimos.

    calcula_coste_minimo_hasta_unidades(vector_coste, numero_candidatos);

    if (crea_vector_C_i(vector_coste, numero_candidatos))
      return 1;

    // Liberamos la memoria reservada.

    free(delta_u);

#ifdef _DEPURA_VITERBI_ACENTUAL
    fclose(fichero_depura);
#endif

    return 0;

  }


  /**
   * Función:   siguiente_recursion_Viterbi_V2                                           
   * \remarks Entrada:
   *			- objetivo: vector descriptor de la unidad que hay que sintetizar.         
   * \remarks Valor devuelto:                                                                     
   *          - Devuelve 0 en ausencia de errores.                                       
   * \remarks Objetivo:  Ejecuta una iteración del algoritmo de Viterbi, actualizando los caminos 
   *            óptimos para cada unidad candidata.                                      
   * \remarks NOTA:	- Se diferencia de la función anterior en que en este caso se considera    
   *			  más de un camino hasta cada unidad de la iteración actual.			   		
   */

  int Viterbi_acentual::siguiente_recursion_Viterbi_V2(Grupo_acentual_objetivo *objetivo) {

    Grupo_acentual_candidato **delta_u;
    int numero_candidatos;

#ifdef _DEPURA_VITERBI_ACENTUAL
    char nombre_fichero[FILENAME_MAX];

    iteracion++;

    sprintf(nombre_fichero, "%s_%d.txt", nombre_fichero_depura, iteracion);

    if ( (fichero_depura = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en siguiente_recursion_Viterbi, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_depura = ...

    fprintf(fichero_depura, "Siguiente recursión, %d: (%d %d &d)\n", iteracion, objetivo->tipo_proposicion, objetivo->posicion_grupo_acentual, objetivo->posicion_silaba_acentuada);

#endif

#ifdef PODA_C_t_POR_VALOR_acentual

    inicia_poda_C_t_acentual();

#endif

#ifdef PODA_C_i_POR_VALOR_acentual

    inicia_poda_C_i_acentual();

#endif

    // Avanzamos la columna de la matriz de caminos óptimos sobre la que estamos trabajando.

    matriz.avanza_columna();

    // Creamos la cadena de unidades candidatas.

    if (crea_delta_u(&delta_u, &numero_candidatos, objetivo))
      return 1;

    // Calculamos el coste de distorsión de unidad de cada candidata.

    if (tamano_vector_coste < numero_candidatos) {

      if (vector_coste)
	free(vector_coste);

      if ( (vector_coste = (Estructura_coste_acentual *) malloc(numero_candidatos*sizeof(Estructura_coste_acentual)))
	   == NULL) {
	fprintf(stderr, "Error al asignar memoria.");
	return 1;
      }

      tamano_vector_coste = numero_candidatos;
    }


    if (crea_vector_C_t(&vector_coste, &numero_candidatos, delta_u, objetivo))
      return 1;

    /*
    // Actualizamos el vector de costes mínimos.

    calcula_coste_minimo_hasta_unidades(vector_coste, numero_candidatos);

    calcula_mejores_caminos_hasta_unidades(vector_coste, numero_candidatos
    // Reservamos espacio en la matriz de memoria de caminos óptimos.
    */

#ifdef _CAMINO_UNICO
#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
    if (crea_vector_C_i_V2_unico_camino(vector_coste, numero_candidatos, objetivo))
      return 1;
#else
    if (crea_vector_C_i_V2_unico_camino(vector_coste, numero_candidatos))
      return 1;
#endif
#elif defined(_CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA)
    if (crea_vector_C_i_V2(vector_coste, numero_candidatos, objetivo))
      return 1;
#else
    if (crea_vector_C_i_V2(vector_coste, numero_candidatos))
      return 1;
#endif
    // Liberamos la memoria reservada.

    free(delta_u);

#ifdef _DEPURA_VITERBI_ACENTUAL
    fclose(fichero_depura);
#endif

    return 0;

  }


  /**
   * \brief Implementa un algoritmo de Viterbi utilizando como unidad el grupo fónico.
   * \remarks Coge como entrada el resultado de un Viterbi previo, en el que se han buscado las mejores secuencias de grupos acentuales para cada grupo fónico.
   * \param[in] grupos Vector de vectores de grupos acentuales.
   * \param[in] costes Vector de vectores con los costes de cada camino.
   * \param[in] tamano_grupo_fonico Número de grupos acentuales del grupo fónico.
   * \param[in] numero_grupos_fonicos Número de grupos fónicos de la frase
   * \param[in] inicio_nombre_fichero_depura Inicio del nombre del fichero de depuración.
   * \return Devuelve 0 en ausencia de error.                                     */
  
  int Viterbi_acentual::inicia_algoritmo_Viterbi_acentual_grupos_fonicos(vector<Grupo_acentual_candidato *> &grupos, vector<float> &costes, int tamano_grupo_fonico, int numero_grupos_fonicos, char *inicio_nombre_fichero_depura) {

    int contador;
    int tamano_actual = 0;
    Estructura_coste_acentual_GF *correcaminos;

#ifdef PODA_C_i_acentual
    int poda;
#endif

#ifdef _DEPURA_VITERBI_ACENTUAL
    
    char nombre_fichero[FILENAME_MAX];
    iteracion = 1;

    sprintf(nombre_fichero_depura, "%s_%d", inicio_nombre_fichero_depura, iteracion_gf);
    sprintf(nombre_fichero, "%s_%d.txt", nombre_fichero_depura, iteracion);

    if ( (fichero_depura = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en inicia_algoritmo_Viterbi_acentual_grupos_fonicos, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_depura = ...

    fprintf(fichero_depura, "Inicio del algoritmo de Viterbi por grupos fónicos\n");

#endif

#ifdef PODA_C_t_POR_VALOR_acentual

    inicia_poda_C_t_acentual();

#endif

#ifdef PODA_C_i_POR_VALOR_acentual

    inicia_poda_C_i_acentual();

#endif

    //    if ( (vector_coste = (Estructura_coste_acentual *) malloc(nu
    for (contador = 0; contador < numero_contornos; contador++)
      indices_escogidos[contador] = -1;
    
    // En este caso, el coste de unidad ya está en el parámetro de entrada "costes".

    tamano_vector_coste = costes.size();

    if ( (vector_coste_GF = (Estructura_coste_acentual_GF *) malloc(tamano_vector_coste*sizeof(Estructura_coste_acentual_GF))) == NULL) {
      fprintf(stderr, "Error en inicia_algoritmo_Viterbi_acentual_grupos_fonicos, al asignar memoria.\n");
      return 1;
    }

    if (crea_vector_C_t_grupos_fonicos(grupos, costes, tamano_grupo_fonico, vector_coste_GF))
      return 1;

    // Iniciamos la matriz que se emplea para memorizar los caminos de coste mínimo a cada unidad.

    if (matriz.inicializa_matriz(tamano_vector_coste, numero_grupos_fonicos))
      return 1;
    
    // Creamos el vector C_i, "a mano"

    tamano_C_i = tamano_vector_coste;

    if (C_i_GF)
      free(C_i_GF);

    if ( (C_i_GF = (Estructura_C_i_acentual_GF *) malloc(tamano_C_i*sizeof(Estructura_C_i_acentual_GF))) == NULL) {
      fprintf(stderr, "Error en inicia_algoritmo_Viterbi_acentual_grupos_fonicos, al asignar memoria.\n");
      return 1;
    }

    Estructura_C_i_acentual_GF *recorre_C_i = C_i_GF;
    correcaminos = vector_coste_GF;
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "---> En crea_vector_C_i (%d)\n", iteracion);
#endif
    
    for (contador = 0; contador < tamano_C_i; contador++, correcaminos++) {

      recorre_C_i->secuencia_unidades =  correcaminos->secuencia_unidades;
      recorre_C_i->numero_unidades = tamano_grupo_fonico;
      recorre_C_i->camino = contador;
      recorre_C_i->C_i = correcaminos->coste_unidad;

      tamano_actual++;

#ifdef PODA_C_i_acentual
      if ( (poda = ordena_y_poda_C_i_GF(C_i_GF, tamano_actual)) != 0) {
	tamano_actual -= poda;
	recorre_C_i = C_i_GF + tamano_actual;
      }
      else
	recorre_C_i++;
#else
      recorre_C_i++;
#endif
      
  }

  tamano_C_i = tamano_actual;
  
    // Metemos los mejores caminos en la matriz de caminos óptimos:
  
  recorre_C_i = C_i_GF;

  for (contador = 0; contador < tamano_C_i; contador++, recorre_C_i++) {

    if (matriz.introduce_elemento(contador, recorre_C_i->secuencia_unidades, recorre_C_i->camino))
      return 1;
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    fprintf(fichero_depura, "---------> %d (%d-%d %f %d)\n", contador, recorre_C_i->secuencia_unidades->identificador, (recorre_C_i->secuencia_unidades + recorre_C_i->numero_unidades - 1)->identificador, recorre_C_i->C_i, recorre_C_i->camino);
#endif

  }
  
#ifdef _DEPURA_VITERBI_ACENTUAL
  fclose(fichero_depura);
#endif
  
  return 0;
  
  }


  /**
   * \brief Implementa la siguiente iteración del algoritmo de Viterbi en la búsqueda por grupos fónicos
   * \param[in] grupos Vector de vectores de grupos acentuales (mejores caminos para cada grupo fónico por separado).
   * \param[in] costes Vector de vectores de costes, para cada uno de los caminos de cada grupo fónico.
   * \param[in] indice Índice de la iteración actual.
   * \param[in] tamano_grupo_fonico Número de grupos acentuales del grupo fónico actual
   * \return En ausencia de error, devuelve un cero.
   */

  int Viterbi_acentual::siguiente_recursion_Viterbi_grupos_fonicos(vector<Grupo_acentual_candidato *> &grupos, vector<float> &costes, int tamano_grupo_fonico) {

    Grupo_acentual_candidato **delta_u;
    int numero_candidatos;

#ifdef _DEPURA_VITERBI_ACENTUAL
    char nombre_fichero[FILENAME_MAX];

    iteracion++;

    sprintf(nombre_fichero, "%s_%d.txt", nombre_fichero_depura, iteracion);

    if ( (fichero_depura = fopen(nombre_fichero, "wt")) == NULL) {
      fprintf(stderr, "Error en siguiente_recursion_Viterbi, al crear el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_depura = ...

    fprintf(fichero_depura, "Siguiente recursión, grupos fónicos:\n");

#endif

#ifdef PODA_C_t_POR_VALOR_acentual

    inicia_poda_C_t_acentual();

#endif

#ifdef PODA_C_i_POR_VALOR_acentual

    inicia_poda_C_i_acentual();

#endif

    // Avanzamos la columna de la matriz de caminos óptimos sobre la que estamos trabajando.

    matriz.avanza_columna();

    tamano_vector_coste = costes.size();

    // Creamos la cadena de unidades candidatas.

    if (vector_coste_GF)
      free(vector_coste_GF);

    if ( (vector_coste_GF = (Estructura_coste_acentual_GF *) malloc(tamano_vector_coste*sizeof(Estructura_coste_acentual_GF)))
	 == NULL) {
      fprintf(stderr, "Error al asignar memoria.");
      return 1;
    }


    if (crea_vector_C_t_grupos_fonicos(grupos, costes, tamano_grupo_fonico, vector_coste_GF))
      return 1;

    // Creamos el vector C_i

    if (crea_vector_C_i_grupos_fonicos(vector_coste_GF, tamano_vector_coste))
      return 1;

#ifdef _DEPURA_VITERBI_ACENTUAL
    fclose(fichero_depura);
#endif

    return 0;

  }


  /**
   * Función:   devuelve_camino_optimo_acentual                                          
   * \param	secuencia_optima: indica cuál es el orden de la mejor secuencia que se
   *            desea obtener, siendo 0 la mejor.
   * \retval	numero_unidades: número de unidades que componen el camino óptimo.
   * \retval	acentuales: cadena con los grupos acentuales.
   * \retval	numero_real_caminos: número de caminos óptimos que se consideran (para el caso de
   * que haya menos caminos que los que se buscan en el algoritmo de selección).
   * \retvar	coste	coste de la secuencia
   * \return  - En ausencia de error devuelve un cero.
   * \remarks Objetivo:  Devolver la secuencia de unidades que proporciona un camino de coste
   *            mínimo en la síntesis de la frase objetivo.
   * \remarks NOTA:      Después de la ejecución del Viterbi, siempre hay que comenzar llamando a
   *            esta función con secuencia_optima = 0, y a partir de ahí, 1, 2 ...
   */

  int Viterbi_acentual::devuelve_camino_optimo_acentual(unsigned char secuencia_optima, Grupo_acentual_candidato **acentuales, int *numero_unidades, int *numero_real_caminos, float *coste) {

    int indice_minimo;

    estructura_matriz *camino_optimo;

    if (secuencia_optima >= numero_contornos) {
      fprintf(stderr, "Error en devuelve_camino_optimo_acentual: secuencia_optima no válida (%d, %d).\n", secuencia_optima, numero_contornos);
      return 1;
    }

#ifndef PODA_C_i_acentual

    char elimina;
    int cuenta;
    float minimo = FLT_MAX;

    Estructura_C_i_acentual *correcaminos = C_i;

    indice_minimo = 0;

    // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.

    for (int contador = 0; contador < tamano_C_i; contador++, correcaminos++) {

      if (correcaminos->C_i < minimo) {

	cuenta = 0;
	elimina = 0;

	while (cuenta < secuencia_optima)
	  if (contador == indices_escogidos[cuenta++]) {
	    elimina = 1;
	    break;
	  }

	if (elimina)
	  continue;

	minimo = correcaminos->C_i;
	indice_minimo = contador;
      } // if (correcaminos->C_i...

    } // for (int contador ...
#else
    indice_minimo = secuencia_optima;
#endif

    indices_escogidos[secuencia_optima] = indice_minimo;

    if (coste)
      *coste = C_i[indice_minimo].C_i;

    if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
      return 1;

    if (indice_minimo >= *numero_real_caminos) {
      fprintf(stderr, "Error en devuelve_camino_optimo_acentual: secuencia_optima no válida (%d, %d).\n", indice_minimo, *numero_real_caminos);
      return 1;
    }

    if (convierte_a_cadena_acentuales(camino_optimo, *numero_unidades, acentuales))
      return 1;

    return 0;

  }

  /**
   * \brief Función que devuelve la secuencia óptima de grupos acentuales resultante de la ejecución del algoritmo de Viterbi.
   * \remarks Se emplea en el modo de cálculo de contornos por grupos fónicos.
   * \param[in]	secuencia_optima Indica cuál es el orden de la mejor secuencia que se desea obtener, siendo 0 la mejor.
   * \tamano_grupos_fonicos Vector con el tamaño de los diferentes grupos fónicos (en grupos acentuales).
   * \param[out] numero_unidades Número de unidades que componen el camino óptimo.
   * \param[out] acentuales Cadena con los grupos acentuales.
   * \param[out] numero_real_caminos Nnúmero de caminos óptimos que se consideran (para el caso de que haya menos caminos que los que se buscan en el algoritmo de selección).
   * \param[out] coste Coste de la secuencia
   * \return  En ausencia de error devuelve un cero.
   * \remarks NOTA: Después de la ejecución del Viterbi, siempre hay que comenzar llamando a esta función con secuencia_optima = 0, y a partir de ahí, 1, 2 ...
   */

  int Viterbi_acentual::devuelve_camino_optimo_acentual_GF(unsigned char secuencia_optima, Grupo_acentual_candidato **acentuales, int *numero_unidades, int *numero_real_caminos, vector<int> &tamano_grupos_fonicos, float *coste) {

    int indice_minimo;
    int numero_grupos_acentuales;

    estructura_matriz *camino_optimo;

    if (secuencia_optima >= numero_contornos) {
      fprintf(stderr, "Error en devuelve_camino_optimo_acentual: secuencia_optima no válida (%d, %d).\n", secuencia_optima, numero_contornos);
      return 1;
    }

#ifndef PODA_C_i_acentual

    char elimina;
    int cuenta;
    float minimo = FLT_MAX;

    Estructura_C_i_acentual_GF *correcaminos = C_i_GF;

    indice_minimo = 0;

    // Calculamos el camino óptimo en esa iteración como aquél cuyo coste es mínimo en C_i.

    for (int contador = 0; contador < tamano_C_i; contador++, correcaminos++) {

      if (correcaminos->C_i < minimo) {

	cuenta = 0;
	elimina = 0;

	while (cuenta < secuencia_optima)
	  if (contador == indices_escogidos[cuenta++]) {
	    elimina = 1;
	    break;
	  }

	if (elimina)
	  continue;

	minimo = correcaminos->C_i;
	indice_minimo = contador;
      } // if (correcaminos->C_i...

    } // for (int contador ...
#else
    indice_minimo = secuencia_optima;
#endif

    indices_escogidos[secuencia_optima] = indice_minimo;

    if (coste)
      *coste = C_i_GF[indice_minimo].C_i;

    if ( (camino_optimo = matriz.saca_fila(indice_minimo, numero_unidades, numero_real_caminos)) == NULL)
      return 1;

    if (indice_minimo >= *numero_real_caminos) {
      fprintf(stderr, "Error en devuelve_camino_optimo_acentual: secuencia_optima no válida (%d, %d).\n", indice_minimo, *numero_real_caminos);
      return 1;
    }

    if (convierte_a_cadena_acentuales_GF(camino_optimo, tamano_grupos_fonicos, *numero_unidades, acentuales, &numero_grupos_acentuales))
      return 1;

    *numero_unidades = numero_grupos_acentuales;

    return 0;

  }


  /**
   * Función:   libera_memoria_algoritmo                                                 
   * \remarks Objetivo:  Permite que el usuario libere la memoria empleada en los diferentes      
   *            pasos del algoritmo.                                                     
   */

  void Viterbi_acentual::libera_memoria_algoritmo_acentual() {

    matriz.libera_memoria();

    if (C_i != NULL) {
      free(C_i);
      C_i = NULL;
      tamano_C_i = 0;
      memoria_reservada_C_i = 0;
    }

    if (C_i_GF != NULL) {
      free(C_i_GF);
      C_i_GF = NULL;
      tamano_C_i = 0;
      memoria_reservada_C_i = 0;
    }

    if (vector_coste != NULL) {
      free(vector_coste);
      vector_coste = NULL;
      tamano_vector_coste = 0;
    }

    if (vector_coste_GF != NULL) {
      free(vector_coste_GF);
      vector_coste_GF = NULL;
      tamano_vector_coste = 0;
    }


#ifdef PODA_C_i_POR_NUMERO_acentual
    maxima_memoria_reservada = 0;
#endif

  }

  /**
   * Función:   finaliza				                                                   
   * \remarks Objetivo:  Libera la memoria asignada a algunas variables del objeto.			   	
   */

  void Viterbi_acentual::finaliza() {

    if (indices_escogidos) {
      free(indices_escogidos);
      indices_escogidos = NULL;
    }

    if (contornos_escogidos) {
      free(contornos_escogidos);
      contornos_escogidos = NULL;
    }

  }


  int Viterbi_acentual::imprime_C_c() {
    return numero_C_c;
  }


  /**
   * \author	Fran Campillo
   * \remarks	Función encargada de escoger una serie de contornos entre los mejores
   * resultantes de la ejecución del algoritmo de Viterbi. Ojo, asigna memoria para
   * los nuevos caminos, por lo que posteriormente hay que liberarla.
   * \retval	acentuales: conjunto de contornos de frecuencia fundamental.
   * \param	indice_acentuales: índice, dentro de cada cadena de la variable anterior,
   * a partir del cual se introducen los nuevos grupos.
   * \param	numero_grupos: número de grupos acentuales del grupo fónico.
   * \param	numero_grupos_totales: número total de grupos de la oración.
   * \retval	numero_real_caminos: número de caminos óptimos real que se devuelven (para
   * el caso en el que en la matriz haya menos de los que se piden)
   * \param	criterio: 0 -> cuadratica.
   *					  1 -> grupos.
   *					  otro -> coste prosódico.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Viterbi_acentual::devuelve_mejores_caminos(Grupo_acentual_candidato *acentuales, int indice_acentuales, int numero_grupos, int numero_grupos_totales, int *numero_real_caminos, char criterio) {

    int contador, numero_unidades, grupos_escogidos = 1;
    float maximo, distancia;
    int indice = 0;
    Grupo_acentual_candidato *contornos, *apunta_salida, *apunta_candidato;

    *numero_real_caminos = numero_contornos;

    if ( (contornos = (Grupo_acentual_candidato *) malloc(numero_grupos*numero_contornos*sizeof(Grupo_acentual_candidato)))
	 == NULL) {
      fprintf(stderr, "Error en devuelve_mejores_caminos, al asignar memoria.\n");
      return 1;
    } // if ( (contornos = ...

    // Inicializamos la cadena de contornos ya escogidos:

    for (contador = 0; contador < numero_contornos; contador++)
      contornos_escogidos[contador] = 0;

    // Extraemos los mejores contornos:

    for (contador = 0; contador < *numero_real_caminos; contador++)  {
      Grupo_acentual_candidato *grupo_auxiliar = contornos + contador*numero_grupos;
      if (devuelve_camino_optimo_acentual((unsigned char) contador, &grupo_auxiliar, &numero_unidades, numero_real_caminos))
	return 1;
    }

    numero_contornos = *numero_real_caminos;
    if (numero_contornos_escogidos > numero_contornos)
      numero_contornos_escogidos = numero_contornos;
        
    // Nos quedamos siempre con el primero, por ser el de mejor coste:

    apunta_salida = acentuales + indice_acentuales;
    apunta_candidato = contornos;

    for (contador = 0; contador < numero_unidades; contador++)
      apunta_salida++->copia_grupo_acentual(apunta_candidato++);
    //    	*apunta_salida++ = *apunta_candidato++;


    contornos_escogidos[0] = 1;

    if ( (criterio != 0) && (criterio != 1) ) {

      for (int cuenta = 1; cuenta  < numero_contornos_escogidos; cuenta++) {

	// Apuntamos en la salida el contorno escogido:

	apunta_salida = acentuales + cuenta*numero_grupos_totales + indice_acentuales;
	apunta_candidato = contornos + cuenta*numero_grupos;

	for (contador = 0; contador < numero_unidades; contador++)
	  apunta_salida++->copia_grupo_acentual(apunta_candidato++);
	//                *apunta_salida++ = *apunta_candidato++;

      } // for (int cuenta = 1; ...

    } // if ( (criterio != 0) && (criterior != 1) )
    else

      while (grupos_escogidos < numero_contornos_escogidos) {

        // Ahora seleccionamos el resto en función de las distancias al último
        // seleccionado:

	distancia = 0.0;
	apunta_candidato = contornos + indice*numero_grupos;
	maximo = -1.0;

	for (contador = 1; contador < numero_contornos; contador++) {

	  if (contornos_escogidos[contador] == 1) // Si ya fue escogido...
	    continue;

	  if (criterio == 0) {
	    if (calcula_distancia_entre_contornos_cuadratica(apunta_candidato, contornos + contador*numero_grupos,
							     numero_grupos, &distancia))
	      return 1;
	  } // if (criterio == 0)
	  else
	    if (calcula_distancia_entre_contornos_grupos(apunta_candidato, contornos + contador*numero_grupos,
							 numero_grupos, &distancia))
	      return 1;


	  if (distancia > maximo) {
	    maximo = distancia;
	    indice = contador;
	  } // if (distancia < minimo)

	} // for (contador = 0; ...

	contornos_escogidos[indice] = 1;

	// Apuntamos en la salida el contorno escogido:

	apunta_salida = (acentuales + grupos_escogidos++*numero_grupos_totales) + indice_acentuales;
	apunta_candidato = contornos + indice*numero_grupos;

	for (contador = 0; contador < numero_unidades; contador++)
	  apunta_salida++->copia_grupo_acentual(apunta_candidato++);
	//                *apunta_salida++ = *apunta_candidato++;

      } // while ...

    free(contornos);

    return 0;

  }

  /**
   * \author
   * \remarks
   * \param num_total_contornos: número de contornos resultantes de la búsqueda entonativa.
   * \param num_contornos_escogidos: número de contornos de los anteriores que pasan a la
   * búsqueda acústica.
   * \param loc:	puntero al locutor empleado.
   *
   * \return
   */
  int Viterbi_acentual::inicializa(int num_total_contornos, int num_contornos_escogidos, Locutor * locutor){

    locutor_actual = locutor;

    numero_contornos = num_total_contornos;
    numero_contornos_escogidos = num_contornos_escogidos;
    
    if (indices_escogidos)
      free(indices_escogidos);

    if ( (indices_escogidos = (int *) malloc(numero_contornos*sizeof(int))) == NULL) {
      fprintf(stderr, "Error en Viterbi_acentual::inicializa, al asignar memoria.\n");
      return 1;
    } // if ( (indices_escogidos = ...


    if (contornos_escogidos)
      free(contornos_escogidos);

    if ( (contornos_escogidos = (char *) malloc(numero_contornos*sizeof(char))) == NULL) {
      fprintf(stderr, "Error en Viterbi_acentual::inicializa, al asignar memoria.\n");
      return 1;
    } // if ( (contornos_escogidos = ...

    C_i = NULL;
    C_i_GF = NULL;
    tamano_C_i = 0;
    memoria_reservada_C_i = 0;

    vector_coste = NULL;
    tamano_vector_coste = 0;

    vector_coste_GF = NULL;

    numero_C_c = 0;

#ifdef PODA_C_i_POR_NUMERO_acentual
    maxima_memoria_reservada = 0;
#endif

    return 0;
  }

  /**
   * Función:   devuelve_grupo_acentual                                                     
   * \remarks Entrada:
   *			- posicion_grupo: posición del grupo en la frase (GRUPO_INICIAL, GRUPO_MEDIO, 
   *			  GRUPO_FINAL o GRUPO_INICIAL_Y_FINAL										  
   *			- posicion_silaba_acentuada: GRUPO_AGUDO, GRUPO_LLANO o GRUPO_ESDRUJULO		  
   *			- tipo_proposicion: FRASE_ENUNCIATIVA, FRASE_EXCLAMATIVA, FRASE_INTERROGATIVA 
   *			  o FRASE_INACABADA.														  
   *			- descriptor: identificador numérico del grupo.								  
   * Salida   - grupo: grupo de la base entonativa con esas características.				  
   * \remarks Valor devuelto:                                                                        
   *          - Devuelve 0 en caso de que no se produzca error.                             
   */

  int Viterbi_acentual::devuelve_grupo_acentual(Grupo_acentual_candidato *grupo, int posicion_grupo,
						int posicion_silaba_acentuada, int tipo_proposicion,
						int identificador) {

    int indice;
    estructura_grupos_acentuales *unidad_seleccionada;
    Grupo_acentual_candidato *recorre_unidades;

    indice = calcula_indice_grupo(tipo_proposicion, posicion_grupo,
				  posicion_silaba_acentuada);

    if ( (unidad_seleccionada = locutor_actual->tabla_grupos[indice]) == NULL) {

      fprintf(stderr, "Error en devuelve_grupo_acentual. Unidad no encontrada.\n");
      return 1;
    } // if ( (unidad_seleccionada

    recorre_unidades = unidad_seleccionada->grupos;

    for (int cuenta = 0; cuenta < unidad_seleccionada->numero_grupos;
	 cuenta++, recorre_unidades++)
      if (recorre_unidades->identificador == identificador) {
	*grupo = *recorre_unidades;
	return 0;
      } // if (recorre_unidades->identificador ...

    fprintf(stderr, "Error en devuelve_grupo_acentual. Unidad (%d) no encontrada.\n", identificador);

    return 1;

  }


#ifdef _DEPURA_VITERBI_ACENTUAL
  void Viterbi_acentual::avanza_iterador() {
    iteracion_gf++;
  }
  
  void Viterbi_acentual::reinicia_iterador() {
    iteracion_gf = 0;
  }

#endif
