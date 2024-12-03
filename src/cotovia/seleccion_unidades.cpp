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
#include <float.h>

#include "info_corpus_voz.hpp"
#include "descriptor.hpp"
#include "cache.hpp"
#include "matriz.hpp"
#include "path_list.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "Viterbi.hpp"
#include "info_estructuras.hpp"
#include "Viterbi_acentual.hpp"
#include "seleccion_unidades.hpp"


/**
 * \fn seleccion_unidades
 * \author Fran Campillo
 * \remarks Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus
 * \param[in] cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in] numero_unidades número de elementos de la cadena anterior
 * \param[in] cadena_acentuales cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in] numero_acentuales número de elementos de la cadena anterior
 * \param[in] escalado_frecuencia valor por el que se desea escalar la frecuencia
 * \param[in] numero_total_contornos_entonativos número total de contornos escogidos en la búsqueda entonativa
 * \param[in] numero_rutas_optimas número de contornos que pasan a la búsqueda acústica
 * \param[in] locutor puntero al locutor empleado
 * \param[in] tipo_candidatos forma de escoger los precandidatos para la búsqueda de Viterbi
 * \param[in] opciones: opción de funcionamiento:
 0: únicamente se realiza la selección
 1: se escribe información sobre los contornos escogidos
 2: genera los ficheros del modo _CALCULA_FACTORES
 * \param[in] frase_fonetica cadena con la transcripción fonética de la frase
 * \param[in] frase_entrada frase de entrada (texto)
 * \param[in] nombre_fichero_entrada nombre del fichero de entrada
 * \param[in] ficheros cadena de ficheros del modo _CALCULA_FACTORES
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out] cadena_sintesis cadena con las unidades acústicas seleccionadas
 * \param[out] numero_vectores_sintesis número de elementos de la cadena anterior
 * \return En ausencia de error, devuelve un cero
 */
#ifndef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
int Seleccion_unidades::seleccion_unidades(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  /* NOTA: ESTA FUNCIÓN NO COMPILA DESDE QUE SE HIZO EL CAMBIO DE ARRAYS A VECTORES AL ALMACENAR LOS Vectores_descriptor_objetivo */

  return 0;

  //   static int cuenta_frases = 1;
  //   float coste_camino_escogido = 0;
  //   int cuenta, contador;
  //   unsigned char contador_acentuales;
  //   int indice_coste_minimo;
  //   int numero_semifonemas_grupo_actual;
  //   int numero_grupos_grupo_fonico_actual;
  //   int indice_acentual;
  //   int indice_primer_grupo_acentual_actual;
  //   int indice_primer_fonema_grupo_fonico;
  //   int indice_camino_sintesis;
  //   int numero_semifonemas_real_grupo;
  //   int ultimo_grupo_prosodico;
  //   int numero_unidades_candidatas;
  //   int numero_rutas_optimas_reales;
  //   int numero_unidades_grupo_fonico = 1;
  //   int numero_total_grupos_prosodicos = -1;
  //   unsigned char izquierdo_derecho;
  //   char avanza_puntero_objetivo = 0x1;
  //   float *costes_minimos;

  //   Grupo_acentual_objetivo *puntero_acentual;
  //   Grupo_acentual_candidato *acentuales_cand;
  //   vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_grupo_fonico, puntero_objetivo, apunta_objetivo;
  //   Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  //   estructura_frecuencias *frecuencias;

  //   Grupo_acentual_candidato *contorno_total_escogido,
  //     *apunta_contorno_total, *apunta_contorno_escogido;

  // #ifndef _MODELO_ENTONATIVO_ANTERIOR
  //   estructura_frecuencias *frec;
  // #endif

  //   viterbi.inicializa(locutor);
  // viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  //   if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
  //     numero_total_contornos_entonativos = MEJORES_CONTORNOS;

  //   // Lo mismo para la otra variable

  //   if (numero_rutas_optimas == 0)
  //     numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;

  //   if (numero_total_contornos_entonativos < numero_rutas_optimas) {
  //     fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades. El número total de contornos (N) \
  // debe ser mayor o igual que el de los seleccionados (M).\n");
  //     return 1;
  //   } // if (num_total_contornos < ...


  //   if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
  //     return 1;

  //   numero_grupos_grupo_fonico_actual = 0;
  //   indice_primer_grupo_acentual_actual = 0;
  //   indice_primer_fonema_grupo_fonico = 0;
  //   indice_camino_sintesis = 0;
  //   izquierdo_derecho = IZQUIERDO;

  //   ultimo_grupo_prosodico = 1;
  //   numero_unidades_candidatas = 2*numero_unidades;
  //   puntero_acentual = cadena_acentuales;
  //   inicio_objetivo_grupo_fonico = cadena_unidades.begin();

  //   apunta_objetivo = cadena_unidades.begin();

  //   for (contador = 0; contador < numero_unidades; contador++)
  //     if (apunta_objetivo++->semifonema[0] == '#')
  //       numero_total_grupos_prosodicos++;


  //   // Actualmente, el número de vectores de síntesis es siempre el doble que el de
  //   // unidades:

  //   *numero_vectores_sintesis = numero_unidades_candidatas;

  //   if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
  //     fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //     return 1;
  //   } // if ( (costes_minimos = ...

  //   if ( (acentuales_cand = (Grupo_acentual_candidato *) malloc(numero_acentuales*numero_rutas_optimas*sizeof(Grupo_acentual_candidato)))
  //        == NULL) {
  //     fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //     return 1;
  //   } // if ( (acentuales_cand = ...

  //   if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
  //        == NULL) {
  //     fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //     return 1;
  //   } // if ( (unidades_candidatas = ...

  //   if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
  //        == NULL) {
  //     fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //     return 1;
  //   } // if ( (frecuencias = ...

  //   if ( (opciones != 0) && (opciones != 4) ) {

  //     if ( (contorno_total_escogido = (Grupo_acentual_candidato *) malloc((numero_rutas_optimas + 1)*numero_acentuales*sizeof(Grupo_acentual_candidato)))
  // 	 == NULL) {
  //       fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //       return 1;
  //     } // if ( (contorno_total_escogido = ...

  //     apunta_contorno_escogido = contorno_total_escogido + numero_rutas_optimas*numero_acentuales;

  //   } // if (opciones != 0)

  //   // Reservamos memoria para la cadena de salida:

  //   if ( (*cadena_sintesis = (Vector_sintesis *)
  // 	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
  //        == NULL) {
  //     fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
  //     return 1;
  //   } // if ( (*cadena_sintesis = ...

  //   apunta_salida = *cadena_sintesis;

  //   contador = 0;

  //   while (contador++ < numero_total_grupos_prosodicos) {

  //     if (contador == numero_total_grupos_prosodicos)
  //       ultimo_grupo_prosodico = 0;


  //     // Hay que inicializar numero_unidades_grupo_fonico a 1;

  //     indice_primer_fonema_grupo_fonico += numero_unidades_grupo_fonico - 1;

  //     indice_primer_grupo_acentual_actual += numero_grupos_grupo_fonico_actual;
  //     // Contamos el número de fonemas del grupo fónico actual:

  //     // Calculamos el número de grupos acentuales del grupo fónico actual:

  //     char *inicio_token = strstr(frase_fonetica, "#%pausa");
  //     numero_grupos_grupo_fonico_actual = 0;

  //     for (cuenta = 0; cuenta < contador - 1; cuenta++)
  //       if ( (inicio_token = strstr(inicio_token + 1, "#%pausa")) == NULL) {
  // 	fprintf(stderr, "Error en selecciona_unidades: frase_fonetica no válida.\n");
  // 	return 1;
  //       }
  //     char *final_token = strstr(inicio_token + 1, "#%pausa");

  //     while ( (final_token > inicio_token) && (inicio_token != NULL) )
  //       if ( (inicio_token = strstr(inicio_token + 1, "^")) != NULL)
  // 	numero_grupos_grupo_fonico_actual++;

  //     //        numero_grupos_grupo_fonico_actual =
  //     //                puntero_acentual->grupos_acentuales_grupo_prosodico;

  //     indice_acentual = puntero_acentual - cadena_acentuales;

  //     numero_unidades_grupo_fonico = 1;

  //     apunta_objetivo = inicio_objetivo_grupo_fonico;

  //     while ( (apunta_objetivo->semifonema[0] != '#') ||
  // 	    (apunta_objetivo == inicio_objetivo_grupo_fonico) ) {
  //       numero_unidades_grupo_fonico++;
  //       apunta_objetivo++;
  //     } // while ( (apunta_objetivo->semifonema[0] != '#') ...


  //     if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
  // 							   numero_grupos_grupo_fonico_actual))
  //       return 1;

  //     indice_acentual++;

  //     if ( (numero_acentuales > 1) && (indice_acentual != numero_acentuales) ) {

  //       // Mientras sea grupo intermedio o final:
  //       while ( (puntero_acentual->posicion_grupo_acentual != GRUPO_INICIAL) ||
  // 	      (puntero_acentual->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) ||
  // 	      (puntero_acentual->tipo_pausa_anterior == PAUSA_RUPTURA_COMA) ) { // TAMBIÉN INCLUYE
  // 	// GRUPO_INICIAL_Y_FINAL
  // #ifdef _VITERBI_V2
  // 	if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
  // #else
  // 	  if (viterbi_acentual.siguiente_recursion_Viterbi(puntero_acentual++))
  // #endif
  // 	    return 1;

  // 	if (++indice_acentual == numero_acentuales)
  // 	  break;

  //       } // while (puntero_acentual->posicion_grupo_

  //     } // if ( (numero_acentuales > 1) && (indice_acentual != numero_acentuales) )

  //     // Iniciamos a cero la cadena de costes mínimos:

  //     for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas; cuenta_costes++)
  //       costes_minimos[cuenta_costes] = 0.0;

  //     numero_rutas_optimas_reales = numero_rutas_optimas;

  //     if (viterbi_acentual.devuelve_mejores_caminos(acentuales_cand, indice_primer_grupo_acentual_actual, numero_grupos_grupo_fonico_actual, numero_acentuales, &numero_rutas_optimas_reales, 0))
  //       return 1;

  //     if (numero_rutas_optimas_reales < numero_rutas_optimas)
  //       numero_rutas_optimas = numero_rutas_optimas_reales;

  //     for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas;
  // 	 contador_acentuales++) {

  //       Grupo_acentual_candidato *apunta_grupo =
  // 	acentuales_cand + contador_acentuales*numero_acentuales + indice_primer_grupo_acentual_actual;

  //       if (opciones) {
  // 	apunta_contorno_total =
  // 	  contorno_total_escogido + contador_acentuales*numero_acentuales + indice_primer_grupo_acentual_actual;

  // 	for (cuenta = 0; cuenta < numero_grupos_grupo_fonico_actual; cuenta++)
  // 	  *apunta_contorno_total++ = *(apunta_grupo + cuenta);

  //       } // if (opciones)


  //       // Comprobamos la continuidad entre contornos de grupos acentuales.

  //       comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_grupos_grupo_fonico_actual);

  //       // Ojo aquí, por el # compartido:

  //       estructura_frecuencias *apunta_frecuencia =
  // 	frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_grupo_fonico;

  // #ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
  //       if (crea_vector_frecuencias(inicio_objetivo_grupo_fonico, numero_unidades_grupo_fonico, apunta_grupo, cadena_acentuales + indice_primer_grupo_acentual_actual, escalado_frecuencia, &apunta_frecuencia))
  // 	return 1;
  // #else
  //       if (crea_vector_frecuencias_V2(inicio_objetivo_grupo_fonico, numero_unidades_grupo_fonico, apunta_grupo, cadena_acentuales[indice_primer_grupo_acentual_actual], escalado_frecuencia, &apunta_frecuencia))
  // 	return 1;
  // #endif

  // #ifndef _MODELO_ENTONATIVO_ANTERIOR

  //       // Añadimos las frecuencias a la cadena de objetivos:

  //       frec = frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_grupo_fonico;

  //       for (cuenta = 0; cuenta < numero_unidades_grupo_fonico; cuenta++, frec++)
  // 	inicio_objetivo_grupo_fonico[cuenta].anhade_frecuencia(frec->frecuencia_mitad, frec->frecuencia_inicial, frec->frecuencia_final);
  // #endif

  //       // Selección de unidades de voz:

  //       if (inicio_objetivo_grupo_fonico != cadena_unidades.begin())
  // 	izquierdo_derecho = DERECHO;

  //       puntero_objetivo = inicio_objetivo_grupo_fonico;

  //       // Liberamos la memoria

  //       viterbi.libera_memoria_algoritmo();

  //       if (puntero_objetivo == cadena_unidades.begin())
  // 	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - ultimo_grupo_prosodico;
  //       else
  // 	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - 1 - ultimo_grupo_prosodico;

  //       if (viterbi.inicia_algoritmo_Viterbi(puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho))
  // 	return 1;

  //       if (puntero_objetivo != cadena_unidades.begin())
  // 	puntero_objetivo++;

  //       for (int contador_unidades = 1; contador_unidades < numero_semifonemas_real_grupo;
  // 	   contador_unidades++) {

  // 	if (viterbi.siguiente_recursion_Viterbi(puntero_objetivo,
  // 						(contador_unidades == numero_semifonemas_real_grupo - 1) && !ultimo_grupo_prosodico))
  // 	  return 1;

  // 	if (contador_unidades % 2 == avanza_puntero_objetivo)
  // 	  puntero_objetivo++;
  //       } // for (int contador_unidades = 0

  //       puntero_sintesis =
  // 	unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;

  //       if (viterbi.devuelve_camino_optimo_sintesis(&numero_semifonemas_grupo_actual, &costes_minimos[contador_acentuales], &puntero_sintesis, &numero_rutas_optimas_reales))
  // 	return 1;


  //     } // for (contador_acentuales = 0;

  //     // Escogemos el camino de coste mínimo en el grupo fónico actual:

  //     indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas);
        

  //     if (opciones) {

  //       apunta_contorno_total =
  // 	contorno_total_escogido + indice_coste_minimo*numero_acentuales + indice_primer_grupo_acentual_actual;

  //       for (cuenta = 0; cuenta < numero_grupos_grupo_fonico_actual; cuenta++)
  // 	*apunta_contorno_escogido++ = *apunta_contorno_total++;

  //       coste_camino_escogido += *(costes_minimos + indice_coste_minimo);

  //     } // if (opciones)

  // #ifndef _MODELO_ENTONATIVO_ANTERIOR

  //     // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:

  //     frec = frecuencias + indice_coste_minimo*numero_unidades + indice_primer_fonema_grupo_fonico;

  //     for (; inicio_objetivo_grupo_fonico < puntero_objetivo;
  // 	 inicio_objetivo_grupo_fonico++, frec++)
  //       inicio_objetivo_grupo_fonico->anhade_frecuencia(frec->frecuencia_mitad,
  // 						      frec->frecuencia_inicial,
  // 						      frec->frecuencia_final);
  // #else
  //     inicio_objetivo_grupo_fonico = puntero_objetivo;
  // #endif

  //     // Escribimos las unidades escogidas en la cadena de salida:

  //     puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas + indice_camino_sintesis;

  //     for (cuenta = 0; cuenta < numero_semifonemas_grupo_actual; cuenta++)
  //       *apunta_salida++ = *puntero_sintesis++;

  //     // Actualizamos las variables del bucle:

  //     indice_camino_sintesis += numero_semifonemas_grupo_actual;
  //     //        inicio_objetivo_grupo_fonico = puntero_objetivo;
  //     viterbi_acentual.libera_memoria_algoritmo_acentual();
  //     avanza_puntero_objetivo = 0;

  //   } // while (contador++ < numero_acentuales)

  //   viterbi_acentual.finaliza();
  //   viterbi.libera_memoria_algoritmo();

  // #ifdef _MODO_NORMAL

  //   if (opciones == 1) {

  //     char opcion[3];
  //     char nombre_fichero[FILENAME_MAX];
  //     char *apunta_nombre_fichero;

  //     if (cuenta_frases == 1)
  //       strcpy(opcion, "wt");
  //     else
  //       strcpy(opcion, "at");

  //     strcpy(nombre_fichero, nombre_fichero_entrada);

  //     apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

  //     sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);


  //     if (escribe_cadena_escogida_final(numero_unidades_candidatas,
  // 				      nombre_fichero,
  // 				      cadena_unidades, *cadena_sintesis,
  // 				      locutor, 0, opcion))
  //       return 1;

  //     sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

  //     if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
  // 					 nombre_fichero, cadena_unidades,
  // 					 *cadena_sintesis, locutor, opcion,
  // 					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
  //       return 1;

  //     sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

  //     if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
  // 					nombre_fichero, frase_fonetica,cadena_unidades,
  // 					contorno_total_escogido + numero_rutas_optimas*numero_acentuales, opcion))
  //       return 1;


  //     sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

  //     if (escribe_contorno_escogido_calculo_distancia(numero_acentuales,
  // 						    nombre_fichero, contorno_total_escogido + numero_rutas_optimas*numero_acentuales, frase_entrada,
  // 						    coste_camino_escogido / numero_unidades, opcion))
  //       return 1;


  //     sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

  //     if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis"wt"))
  //       return 1;

  //     for (contador = 0; contador < numero_rutas_optimas; contador++) {
  //       sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

  //       if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas, numero_unidades,
  // 					   frecuencias + contador*numero_unidades, locutor,
  // 					   "wt",
  // 					   nombre_fichero, contador))
  // 	return 1;

  //       sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

  //       if (crea_fichero_grupos_acentuales(contorno_total_escogido +contador*numero_acentuales,
  // 					 numero_acentuales, "wt", nombre_fichero, contador))
  // 	return 1;

  //     } // for (contador = 0; ...

  //     cuenta_frases++;

  //   } // if (opciones == 1)
  //   else if (opciones > 1) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

  //     escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
  // 						   cadena_unidades,
  // 						   locutor,
  // 						   numero_unidades_candidatas,
  // 						   ficheros[0], ficheros[1],
  // 						   ficheros[2], ficheros[3],
  // 						   ficheros[4], ficheros[5],
  // 						   ficheros[6], ficheros[11]);

  //     escribe_factores_funcion_coste_entonativa(cadena_acentuales,
  // 					      contorno_total_escogido + numero_rutas_optimas*numero_acentuales,
  // 					      numero_acentuales, ficheros[7], ficheros[8],
  // 					      ficheros[9]);

  //     if (escribe_concatenacion_grupos_acentuales(contorno_total_escogido + numero_rutas_optimas*numero_acentuales,
  // 						numero_acentuales,
  // 						ficheros[10]))
  //       return 1;

  //   } // else

  // #endif

  //   // Liberamos la memoria de las variables locales:

  //   apunta_contorno_escogido = acentuales_cand;

  //   for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_contorno_escogido++)
  //     apunta_contorno_escogido->libera_memoria();

  //   free(acentuales_cand);
  //   free(unidades_escogidas);
  //   free(frecuencias);
  //   free(costes_minimos);

  //   if (opciones)
  //     free(contorno_total_escogido);

  //   return 0;

}
#endif

/**
 * \fn seleccion_unidades
 * \author Fran Campillo
 * \remarks Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de la anterior en que en este caso se considera la continuidad de f0 tras una pausa en la búsqueda entonativa.
 * \param[in] cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in] numero_unidades número de elementos de la cadena anterior
 * \param[in] cadena_acentuales cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in] numero_acentuales número de elementos de la cadena anterior
 * \param[in] escalado_frecuencia valor por el que se desea escalar la frecuencia
 * \param[in] numero_total_contornos_entonativos número total de contornos escogidos en la búsqueda entonativa
 * \param[in] numero_rutas_optimas número de contornos que pasan a la búsqueda acústica
 * \param[in] locutor puntero al locutor empleado
 * \param[in] tipo_candidatos forma de seleccionar los precandidatos para la búsqueda de Viterbi
 * \param[in]	opciones opción de funcionamiento:
 0: únicamente se realiza la selección.
 1: se escribe información sobre los contornos escogidos.
 2: genera los ficheros del modo _CALCULA_FACTORES.
 * \param[in] frase_fonetica cadena con la transcripción fonética de la frase
 * \param[in] frase_entrada frase de entrada (texto)
 * \param[in] nombre_fichero_entrada nombre del fichero de entrada
 * \param[in] ficheros cadena de ficheros del modo _CALCULA_FACTORES
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out] cadena_sintesis cadena con las unidades acústicas seleccionadas
 * \param[out] numero_vectores_sintesis número de elementos de la cadena anterior
 * \return En ausencia de error, devuelve un cero
 */
#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
int Seleccion_unidades::seleccion_unidades(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  static int cuenta_frases = 1;
  float coste_camino_escogido = 0;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  //    int numero_grupos_grupo_fonico_actual;
  int indice_acentual;
  //    int indice_primer_grupo_acentual_actual;
  int indice_primer_fonema_grupo_fonico;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_unidades_grupo_fonico = 1;
  int numero_total_grupos_prosodicos = -1;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo;
  float *costes_minimos, coste_actual;
  short int *cadena_posiciones, *apunta_posiciones;

  Grupo_acentual_objetivo *puntero_acentual;
  Grupo_acentual_candidato *acentuales_cand;
  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_grupo_fonico, puntero_objetivo, apunta_objetivo;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;
    
#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
    numero_total_contornos_entonativos = MEJORES_CONTORNOS;
    
  // Lo mismo para la otra variable
    
  if (numero_rutas_optimas == 0)
    numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;
    
  if (numero_total_contornos_entonativos < numero_rutas_optimas) {
    fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades. El número total de contornos (N) debe ser mayor o igual que el de los seleccionados (M).\n");
    return 1;
  } // if (num_total_contornos < ...
    

  if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
    return 1;
    
  //    numero_grupos_grupo_fonico_actual = 0;
  //    indice_primer_grupo_acentual_actual = 0;
  indice_primer_fonema_grupo_fonico = 0;
  //    indice_camino_sintesis = 0;
  izquierdo_derecho = IZQUIERDO;
    
  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;
  puntero_acentual = cadena_acentuales;
    
  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error an seleccion_unidades, al asignar memoria.\n");
    return 1;
  }
    
  // Contamos el número de grupos fónicos, y salvamos las posiciones originales:
    
  apunta_objetivo = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;
    
  for (; apunta_objetivo != cadena_unidades.end(); apunta_objetivo++) {
    *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
    if (apunta_objetivo->semifonema[0] == '#')
      numero_total_grupos_prosodicos++;
  }
    
  // Ya no sirve lo siguiente porque ahora consideramos rupturas entonativas.
  //	numero_total_grupos_prosodicos = puntero_acentual->numero_grupos_prosodicos;
    
    
  // Actualmente, el número de vectores de síntesis es siempre el doble que el de
  // unidades:
    
  *numero_vectores_sintesis = numero_unidades_candidatas;
    
  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...
    
  if ( (acentuales_cand = (Grupo_acentual_candidato *) malloc(numero_acentuales*numero_rutas_optimas*sizeof(Grupo_acentual_candidato)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (acentuales_cand = ...
    
  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...
    
  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...
    
  // Reservamos memoria para la cadena de salida:
    
  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...
    
  apunta_salida = *cadena_sintesis;

  // Primero, la búsqueda entonativa:
    
  if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							 numero_acentuales))
    return 1;
    
  indice_acentual = 1;
    
  while (indice_acentual < numero_acentuales) {
      
#ifdef _VITERBI_V2
    if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
#else
      if (viterbi_acentual.siguiente_recursion_Viterbi(puntero_acentual++))
#endif
	return 1;
      
    indice_acentual++;
      
  } // while (indice_acentual < numero_acentuales
    
    
  // Y ahora, la acústica:
    
  numero_rutas_optimas_reales = numero_rutas_optimas;
    
  if (viterbi_acentual.devuelve_mejores_caminos(acentuales_cand, 0, numero_acentuales, numero_acentuales, &numero_rutas_optimas_reales, 0))
    return 1;

  if (numero_rutas_optimas_reales < numero_rutas_optimas)
    numero_rutas_optimas = numero_rutas_optimas_reales;
    
  // Inicializamos a cero la cadena de costes mínimos:
    
  for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas; cuenta_costes++)
    costes_minimos[cuenta_costes] = 0.0;
    
  viterbi_acentual.libera_memoria_algoritmo_acentual();
    
  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {
      
    // Escogemos el siguiente contorno posible:
      
    Grupo_acentual_candidato *apunta_grupo = acentuales_cand + contador_acentuales*numero_acentuales;
      
    // Comprobamos la continuidad entre contornos de grupos acentuales.
      
    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);
      
    // Creamos la cadena de valores de f0 objetivo:
      
    estructura_frecuencias *apunta_frecuencia = frecuencias + contador_acentuales*numero_unidades;
      
#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#endif				  
      
    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, apunta_grupo, numero_acentuales))
      return 1;

    // Ahora, hacemos la búsqueda acústica por grupos fónicos:
      
    indice_primer_fonema_grupo_fonico = 0;
    numero_unidades_grupo_fonico = 1;
    ultimo_grupo_prosodico = 1;
    indice_camino_sintesis = 0;
    izquierdo_derecho = IZQUIERDO;
    puntero_objetivo = cadena_unidades.begin();
    avanza_puntero_objetivo = 1;

    for (int cuenta_fonicos = 0; cuenta_fonicos < numero_total_grupos_prosodicos; cuenta_fonicos++) {
	
      inicio_objetivo_grupo_fonico = puntero_objetivo;
	
      if (cuenta_fonicos == numero_total_grupos_prosodicos - 1)
	ultimo_grupo_prosodico = 0;
	
      indice_primer_fonema_grupo_fonico += numero_unidades_grupo_fonico - 1; // Acordarme de actualizarlo fuera del bucle.
	
      numero_unidades_grupo_fonico = 1;
	
      apunta_objetivo = inicio_objetivo_grupo_fonico;
	
      while ( (apunta_objetivo->semifonema[0] != '#') ||
	      (apunta_objetivo == inicio_objetivo_grupo_fonico) ) {
	numero_unidades_grupo_fonico++;
	apunta_objetivo++;
      } // while ( (apunta_objetivo->semifonema[0] != '#') ...
	
#ifndef _MODELO_ENTONATIVO_ANTERIOR
	
      // Añadimos las frecuencias a la cadena de objetivos:
	
      frec = frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_grupo_fonico;
	
      for (cuenta = 0; cuenta < numero_unidades_grupo_fonico; cuenta++, frec++)
	inicio_objetivo_grupo_fonico[cuenta].anhade_frecuencia(frec->frecuencia_mitad, frec->frecuencia_inicial, frec->frecuencia_final);
#endif
	
      // Selección de unidades de voz:
	
      if (inicio_objetivo_grupo_fonico != cadena_unidades.begin())
	izquierdo_derecho = DERECHO;
	
      //            puntero_objetivo = inicio_objetivo_grupo_fonico;
	
      // Liberamos la memoria
	
      viterbi.libera_memoria_algoritmo();
	
      if (puntero_objetivo == cadena_unidades.begin())
	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - ultimo_grupo_prosodico;
      else
	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - 1 - ultimo_grupo_prosodico;
	
      if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho))
	return 1;
	
      if (puntero_objetivo != cadena_unidades.begin())
	puntero_objetivo++;
	
      for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
	  
	if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo,
						(contador == numero_semifonemas_real_grupo - 1) && !ultimo_grupo_prosodico))
	  return 1;
	  
	if (contador % 2 == avanza_puntero_objetivo)
	  puntero_objetivo++;
	  
      } // for (int contador = 0
	
      // Ahora escogemos el mejor camino hasta ese punto:
	
      puntero_sintesis =
	unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;
	
      if (viterbi.devuelve_camino_optimo_sintesis(&numero_semifonemas_grupo_actual, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales))
	return 1;
	
      costes_minimos[contador_acentuales] +=  coste_actual;
      indice_camino_sintesis += numero_semifonemas_grupo_actual;
      avanza_puntero_objetivo = 0;
	
    } // for (int cuenta_fonicos = 0; ...
      
      
  } // for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas...
    
  // Buscamos el mejor camino:
    
  indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas);
    
  coste_camino_escogido = costes_minimos[indice_coste_minimo];
    
#ifndef _MODELO_ENTONATIVO_ANTERIOR
    
  // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:
    
  frec = frecuencias + indice_coste_minimo*numero_unidades;
  inicio_objetivo_grupo_fonico = cadena_unidades.begin();
    
  for (; inicio_objetivo_grupo_fonico < puntero_objetivo;
       inicio_objetivo_grupo_fonico++, frec++)
    inicio_objetivo_grupo_fonico->anhade_frecuencia(frec->frecuencia_mitad,
						    frec->frecuencia_inicial,
						    frec->frecuencia_final);
#endif
    
  if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand + indice_coste_minimo*numero_acentuales, numero_acentuales))
    return 1;
    
  // Escribimos las unidades escogidas en la cadena de salida:
    
  puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;
    
  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, puntero_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  for (cuenta = 0; cuenta < numero_unidades_candidatas; cuenta++)
    *apunta_salida++ = *puntero_sintesis++;
    
  viterbi_acentual.finaliza();
  viterbi.libera_memoria_algoritmo();
    
#ifdef _MODO_NORMAL
    
  if (opciones == 1) {
      
    char opcion[3];
    char nombre_fichero[FILENAME_MAX];
    char *apunta_nombre_fichero;
      
    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");
      
    strcpy(nombre_fichero, nombre_fichero_entrada);
      
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);
      
    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);
      
    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;
      
    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);
      
    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;
      
    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);
      
    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
					nombre_fichero, frase_fonetica,cadena_unidades, acentuales_cand + indice_coste_minimo*numero_acentuales, cadena_acentuales, opcion))
      return 1;
      
      
    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);
      
    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades, nombre_fichero, cadena_unidades, acentuales_cand + indice_coste_minimo*numero_acentuales, cadena_acentuales, opcion))
      return 1;
      
    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);
      
    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales, nombre_fichero, acentuales_cand + indice_coste_minimo*numero_acentuales, frase_entrada, coste_camino_escogido / numero_unidades, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);
      
    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;
      
    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;

    for (contador = 0; contador < numero_rutas_optimas; contador++) {
      sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);
	
      if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand + contador*numero_acentuales, numero_acentuales))
	return 1;


      if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas + contador*numero_unidades_candidatas, numero_unidades, frecuencias + contador*numero_unidades, locutor, "wt", nombre_fichero, contador))
	return 1;
	
      sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);
	
      if (crea_fichero_grupos_acentuales(acentuales_cand + contador*numero_acentuales, numero_acentuales, "wt", nombre_fichero, contador))
	return 1;
	
    } // for (contador = 0; ...
      
    cuenta_frases++;
      
  } // if (opciones == 1)
  else if (opciones > 1) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)
      
    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);
      
    escribe_factores_funcion_coste_entonativa(cadena_acentuales, acentuales_cand + indice_coste_minimo*numero_acentuales, numero_acentuales, ficheros[7], ficheros[8], ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(acentuales_cand + indice_coste_minimo*numero_acentuales, numero_acentuales, ficheros[10]))
      return 1;
      
  } // else
    
#endif
    
  // Liberamos la memoria de las variables locales:
    
  Grupo_acentual_candidato *apunta_contorno_escogido = acentuales_cand;
    
  for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
    
  free(acentuales_cand);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(cadena_posiciones);
    
  return 0;
    
}
	
#endif


/**
 * \fn seleccion_unidades_contorno_total
 * \author Fran Campillo
 * \remarks Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus
 * \param[in] cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in] numero_unidades número de elementos de la cadena anterior
 * \param[in] cadena_acentuales cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in] numero_acentuales número de elementos de la cadena anterior
 * \param[in] escalado_frecuencia valor por el que se desea escalar la frecuencia
 * \param[in] numero_total_contornos_entonativos número de contornos de f0 resultantes de la búsqueda entonativa
 * \param[in] numero_rutas_optimas número de contornos entonativos que pasan a la búsqueda de unidades acústicas
 * \param[in] dir_salida si es NULL, no se escriben los ficheros de resultados. Si no lo es, indica la ruta del directorio de salida
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out]	cadena_sintesis cadena con los vectores seleccionados
 * \param[out]	numero_vectores_sintesis número de elementos de la cadena anterior
 * \return En ausencia de error, devuelve un cero
 */
int Seleccion_unidades::seleccion_unidades_contorno_total(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, char *dir_salida, char * frase_fonetica, unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  int contador, cuenta_frec;
  unsigned  char contador_acentuales;
  int numero_unidades_candidatas;
  char nombre_fichero[FILENAME_MAX];
  int numero_unidades_grupo_fonico;
  int inicio_grupo_fonico;
  unsigned char ultima_unidad_grupo_prosodico;
  float *costes_minimos, coste_actual;

  estructura_frecuencias *frecuencias;
#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  vector<Vector_descriptor_objetivo>::iterator puntero_objetivo;
  Grupo_acentual_objetivo *puntero_acentual;
  Grupo_acentual_candidato *acentuales_cand, *apunta_grupo;
  Vector_sintesis *unidades_escogidas;


  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
    numero_total_contornos_entonativos = MEJORES_CONTORNOS;

  // Lo mismo para la otra variable

  if (numero_rutas_optimas == 0)
    numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;

  if (numero_total_contornos_entonativos < numero_rutas_optimas) {
    fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades. El número total de contornos (N) debe ser mayor o igual que el de los seleccionados (M).\n");
    return 1;
  } // if (num_total_contornos < ...


  if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
    return 1;

  numero_unidades_candidatas = 2*numero_unidades; // Ya que trabajamos con semifonemas.

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_total, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_total, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...

  if ( (acentuales_cand = (Grupo_acentual_candidato *) malloc(numero_acentuales*numero_rutas_optimas*sizeof(Grupo_acentual_candidato)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_total, al asignar memoria.\n");
    return 1;
  } // if ( (acentuales_cand = ...

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_total, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  for (contador = 0; contador < numero_rutas_optimas; contador++)
    costes_minimos[contador] = 0.0;

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_total, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...


  // Selección de contornos entonativos:

  if (numero_acentuales) {

    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(cadena_acentuales, numero_acentuales))
      return 1;

    if (numero_acentuales > 1) {

      contador = 1;
      puntero_acentual = cadena_acentuales + 1;

      while (contador ++ < numero_acentuales) {
#ifdef _VITERBI_V2
	if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
#else
	  if (siguiente_recursion_Viterbi(puntero_acentual++))
#endif
	    return 1;

      } // while (contador++...

    } // if (numero_acentuales >...


  } // if (numero_acentuales...


  // Recuperamos los M mejores caminos:

  if (viterbi_acentual.devuelve_mejores_caminos(acentuales_cand, 0, numero_acentuales, numero_acentuales, &numero_rutas_optimas, 0))
    return 1;

  // Ahora, la selección de unidades acústicas:
    
    
  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas;
       contador_acentuales++) {
      
    apunta_grupo = acentuales_cand + contador_acentuales*numero_acentuales;
      
    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);
      
    estructura_frecuencias *apunta_frecuencia =
      frecuencias + contador_acentuales*numero_unidades;
      
#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades,
				   apunta_grupo,
				   cadena_acentuales, escalado_frecuencia,
				   &apunta_frecuencia))
      return 1;
#endif

#ifndef _MODELO_ENTONATIVO_ANTERIOR

    // Añadimos dichas frecuencias a la cadena de vectores:

    frec = frecuencias + contador_acentuales*numero_unidades;

    for (cuenta_frec = 0; cuenta_frec < numero_unidades; cuenta_frec++, frec++)
      cadena_unidades[cuenta_frec].anhade_frecuencia(frec->frecuencia_mitad,
						     frec->frecuencia_inicial,
						     frec->frecuencia_final);
      
#endif

    puntero_objetivo = cadena_unidades.begin();

    if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_unidades_candidatas, IZQUIERDO))
      return 1;
				    
    contador = 0;

    inicio_grupo_fonico = 0;
    Vector_sintesis *puntero_sintesis;
      
    ultima_unidad_grupo_prosodico = 0;
      
    while (contador++ < numero_unidades_candidatas - 1) {
	
      if (contador == numero_unidades_candidatas - 1)
	ultima_unidad_grupo_prosodico = 1;
	
	
      if ( (puntero_objetivo->semifonema[0] == '#') && ( (contador & 0x1) == 1) &&
	   (puntero_objetivo->contexto_izquierdo[0] !=  '#') &&
	   (contador != numero_unidades_candidatas - 1) ) {
	// Cambiamos de grupo fónico.
	  
	puntero_sintesis = unidades_escogidas + contador_acentuales*numero_unidades_candidatas + inicio_grupo_fonico;
	  
	if (viterbi.devuelve_camino_optimo_sintesis(&numero_unidades_grupo_fonico,
						    &coste_actual, &puntero_sintesis, &numero_rutas_optimas) != 0)
	  return 1;
	  
	costes_minimos[contador_acentuales] += coste_actual;
	  
	inicio_grupo_fonico += numero_unidades_grupo_fonico;
	  
	// Liberamos la memoria de la matriz de caminos óptimos a cada unidad.
	  
	viterbi.libera_memoria_algoritmo();
	  
	if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_unidades_candidatas, DERECHO))
	  return 1;
	  
	puntero_objetivo++;
	  
	continue;
	  
      } // if (puntero_objetivo->semifonema...
	
      if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo, ultima_unidad_grupo_prosodico))
	return 1;
	
      if ( (contador & 0x1) == 1)
	puntero_objetivo++;
	
    } // while (contador++ < numero_unidades_candidatas - 1)...
      
      
    puntero_sintesis = unidades_escogidas + contador_acentuales*numero_unidades_candidatas + inicio_grupo_fonico;
      
    if (viterbi.devuelve_camino_optimo_sintesis(&numero_unidades_grupo_fonico,
						&costes_minimos[contador_acentuales],
						&puntero_sintesis, &numero_rutas_optimas) != 0)
      return 1;
      
      
    // Liberamos la memoria de la matriz de caminos óptimos a cada unidad.
      
    viterbi.libera_memoria_algoritmo();
      
      
  } // for(contador_acentuales = 0...
    
    
  viterbi_acentual.libera_memoria_algoritmo_acentual();
    
  // Ahora tenemos que encontrar la secuencia de menor coste:
    
  int indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas);

#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Añadimos las frecuencias a la cadena de objetivos:

  frec = frecuencias + indice_coste_minimo*numero_unidades;

  for (cuenta_frec = 0; cuenta_frec < numero_unidades; cuenta_frec++, frec++)
    cadena_unidades[cuenta_frec].anhade_frecuencia(frec->frecuencia_mitad,
						   frec->frecuencia_inicial,
						   frec->frecuencia_final);
#endif


  Vector_sintesis *recorre_sintesis = *cadena_sintesis;

  for (cuenta_frec = 0; cuenta_frec < numero_unidades_candidatas; cuenta_frec++)
    *recorre_sintesis++ = *(unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas + cuenta_frec);

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if (dir_salida != NULL) {

    sprintf(nombre_fichero, "%scontorno", dir_salida);

    if (crea_fichero_contorno_frecuencia(cadena_unidades, *cadena_sintesis, numero_unidades, frecuencias + indice_coste_minimo*numero_unidades, locutor, "wt", nombre_fichero, 1))
      return 1;


    sprintf(nombre_fichero, "%sunidades_escogidas_%d.txt", dir_salida, 1);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas, locutor, 0, "wt"))
      return 1;


    sprintf(nombre_fichero, "%sduraciones.txt", dir_salida);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(nombre_fichero, "%scontorno_escogido.txt", dir_salida);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades, nombre_fichero,frase_fonetica, cadena_unidades, acentuales_cand + indice_coste_minimo*numero_acentuales, cadena_acentuales, "wt"))
      return 1;

    for (contador = 0; contador < numero_rutas_optimas; contador++) {

      sprintf(nombre_fichero, "%scontorno_%d.txt", dir_salida, contador);

      if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades, nombre_fichero, frase_fonetica, cadena_unidades, acentuales_cand + contador*numero_acentuales, cadena_acentuales, "wt"))
	return 1;
                
    } // for (contador = 0...


  } // if (dir_salida != NULL)

  viterbi_acentual.finaliza();

  apunta_grupo = acentuales_cand;

  for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_grupo++)
    apunta_grupo->libera_memoria();

  free(frecuencias);
  free(costes_minimos);
  free(unidades_escogidas);
  free(acentuales_cand);

  return 0;

}

/**
 * \fn seleccion_unidades_contorno_fichero
 * \author Fran Campillo
 * \remarks Función que realiza la selección de unidades acústicas. Lee de un fichero el contorno de f0 que se desea emplear
 * \param[in] cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in] numero_unidades número de elementos de la cadena anterior
 * \param[in] cadena_acentuales: cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in] numero_acentuales número de elementos de la cadena anterior
 * \param[in] escalado_frecuencia valor por el que se desea escalar la frecuencia.
 * \param[in] dir_salida si es NULL, no se escriben los ficheros de resultados. Si no lo es, indica la ruta del directorio de salida
 * \param[in] nombre_fichero_grupos nombre del fichero en el que se almacenan los grupos acentuales con los que se reconstruye el contorno de frecuencia. En la primera línea del fichero se indica su formato, que puede ser "Información completa", en el que sigue el formato de salida de "transcribe_acentual.exe", con toda la información de cada grupo acentual, o "Grupos base prosódica", en el que se indica el número de grupos, y en cada línea, la posición en el grupo fónico, la sílaba acentuada, el tipo de frase, y el identificador
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out] cadena_sintesis cadena con los vectores seleccionados
 * \param[out] numero_vectores_sintesis número de elementos de la cadena anterior
 * \return En ausencia de error, devuelve un cero
 */

int Seleccion_unidades::seleccion_unidades_contorno_fichero(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, char *dir_salida, char *nombre_fichero_grupos, char * frase_fonetica, Locutor * locutor, char tipo_candidatos, unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  int contador, cuenta_frec;
  int tipo_frase, posicion_grupo, silaba_acentuada, identificador;
  int numero_unidades_candidatas;
  char nombre_fichero[FILENAME_MAX];
  char tipo_fichero[200], formato_fichero;
  int numero_unidades_grupo_fonico;
  int inicio_grupo_fonico;
  int numero_total_contornos = 1, numero_rutas_optimas = 1;
  unsigned char ultima_unidad_grupo_prosodico;
  float costes_minimos, coste_actual;

  estructura_frecuencias *frecuencias;
#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif
  vector<Vector_descriptor_objetivo>::iterator puntero_objetivo;
  Grupo_acentual_candidato *acentuales_cand, *recorre_acentuales;
  Vector_sintesis *unidades_escogidas;
  Viterbi viterbi;

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  if (viterbi_acentual.inicializa(numero_total_contornos, numero_rutas_optimas, locutor))
    return 1;

  FILE *fichero;

  if ( (fichero = fopen(nombre_fichero_grupos, "rt")) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_contorno_fichero, al \
intentar abrir el fichero %s.\n", nombre_fichero_grupos);
    return 1;
  } // if ( (fichero = fopen...

  // Comprobamos el formato del fichero

  fgets(tipo_fichero, 200, fichero);

  if (strcmp(tipo_fichero, "Información completa\n") == 0) {

    formato_fichero = 0;

    if (lee_contorno_fichero(&acentuales_cand, &contador, 0, fichero))
      return 1;

    if (contador != numero_acentuales) {
      fprintf(stderr, "Error en seleccion_unidades_contorno_fichero: fichero \
erróneo. No coincide el número de grupos acentuales con el esperado.\n");
      return 1;

    } // if (contador != numero_acentuales)
  }
  else if (strcmp(tipo_fichero, "Contorno escogido\n") == 0) {

    formato_fichero = 0;

    if (lee_contorno_fichero(&acentuales_cand, &contador, 1, fichero))
      return 1;

    if (contador != numero_acentuales) {
      fprintf(stderr, "Error en seleccion_unidades_contorno_fichero: fichero \
erróneo. No coincide el número de grupos acentuales con el esperado.\n");
      return 1;
    }

  } // else if
  else if (strcmp(tipo_fichero, "Grupos base prosódica\n") == 0) {

    formato_fichero = 1;

    fscanf(fichero, "%d\n", &contador);

    if (contador != numero_acentuales) {
      fprintf(stderr, "Error en seleccion_unidades_contorno_fichero: fichero \
erróneo. No coincide el número de grupos acentuales con el esperado.\n");
      return 1;
    } // if (contador != numero_acentuales)

    if ( (acentuales_cand = (Grupo_acentual_candidato *) malloc(numero_acentuales*sizeof(Grupo_acentual_candidato))) == NULL) {
      fprintf(stderr, "Error en seleccion_unidades_contorno_fichero, al asignar memoria.\n");
      return 1;
    } // if ( (acentuales_cand ...

    for (contador = 0; contador < numero_acentuales; contador++) {

      fscanf(fichero, "%d\t%d\t%d\t%d\n", &posicion_grupo, &silaba_acentuada,
	     &tipo_frase, &identificador);

      if (viterbi_acentual.devuelve_grupo_acentual(acentuales_cand + contador, posicion_grupo, silaba_acentuada, tipo_frase, identificador))
	return 1;

    } // for (contador = 0; contador < ...

  }
  else {
    fprintf(stderr, "Error en seleccion_unidades_contorno_fichero: fichero con el formato erróneo.\n");
    return 1;
  } // else

  fclose(fichero);

  // Comprobamos si hay que hacer algún tipo de modificación de f0, por falta de continuidad.

  comprueba_y_asegura_continuidad_contornos(acentuales_cand, numero_acentuales);

  costes_minimos = 0;
  //    unidades_escogidas = NULL;
  frecuencias = NULL;


  // Ahora, la selección de unidades acústicas:

  numero_unidades_candidatas = 2*numero_unidades;

#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
  if (crea_vector_frecuencias(cadena_unidades, numero_unidades, acentuales_cand, cadena_acentuales, escalado_frecuencia, &frecuencias))
    return 1;
#else
  if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades, acentuales_cand, cadena_acentuales, escalado_frecuencia, &frecuencias))
    return 1;
#endif

  // Añadimos dichas frecuencias a la cadena de vectores:

#ifndef _MODELO_ENTONATIVO_ANTERIOR

  frec = frecuencias;

  for (cuenta_frec = 0; cuenta_frec < numero_unidades; cuenta_frec++, frec++)
    cadena_unidades[cuenta_frec].anhade_frecuencia(frec->frecuencia_mitad,
						   frec->frecuencia_inicial,
						   frec->frecuencia_final);

#endif

  puntero_objetivo = cadena_unidades.begin();
				
  if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_unidades_candidatas, IZQUIERDO))
    return 1;

  contador = 0;

  inicio_grupo_fonico = 0;
  Vector_sintesis *puntero_sintesis;

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(
							numero_unidades_candidatas*sizeof(Vector_sintesis))) == NULL) {
    puts("Error al asignar memoria.");
    return 1;
  } // if ( (unidades_escogidas...
	
  ultima_unidad_grupo_prosodico = 0;
	
  while (contador++ < numero_unidades_candidatas - 1) {
	  
    if (contador == numero_unidades_candidatas - 1)
      ultima_unidad_grupo_prosodico = 1;


    if ( (puntero_objetivo->semifonema[0] == '#') && ( (contador & 0x1) == 1) &&
	 (puntero_objetivo->contexto_izquierdo[0] !=  '#') &&
	 (contador != numero_unidades_candidatas - 1) ) {
      // Cambiamos de grupo fónico.

      puntero_sintesis = (Vector_sintesis *) &unidades_escogidas[inicio_grupo_fonico];

      if (viterbi.devuelve_camino_optimo_sintesis(&numero_unidades_grupo_fonico,
						  &coste_actual, &puntero_sintesis, &numero_rutas_optimas) != 0)
	return 1;

      costes_minimos += coste_actual;

      inicio_grupo_fonico += numero_unidades_grupo_fonico;

      // Liberamos la memoria de la matriz de caminos óptimos a cada unidad.

      viterbi.libera_memoria_algoritmo();

      if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_unidades_candidatas, DERECHO))
	return 1;

      puntero_objetivo++;

      continue;

    } // if (puntero_objetivo->semifonema...

    if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo, ultima_unidad_grupo_prosodico))
      return 1;

    if ( (contador & 0x1) == 1)
      puntero_objetivo++;

  } // while (contador++ < numero_unidades_candidatas - 1)...


  puntero_sintesis = (Vector_sintesis *) &unidades_escogidas[inicio_grupo_fonico];

  if (viterbi.devuelve_camino_optimo_sintesis(&numero_unidades_grupo_fonico,
					      &costes_minimos, &puntero_sintesis, &numero_rutas_optimas) != 0)
    return 1;


  // Liberamos la memoria de la matriz de caminos óptimos a cada unidad.

  viterbi.libera_memoria_algoritmo();

  //    viterbi_acentual.libera_memoria_algoritmo_acentual();

#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Añadimos las frecuencias a la cadena de objetivos:

  frec = frecuencias;

  for (cuenta_frec = 0; cuenta_frec < numero_unidades; cuenta_frec++, frec++)
    cadena_unidades[cuenta_frec].anhade_frecuencia(frec->frecuencia_mitad,
						   frec->frecuencia_inicial,
						   frec->frecuencia_final);
#endif

  *cadena_sintesis = unidades_escogidas;

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  if (dir_salida != NULL) {

    sprintf(nombre_fichero, "%scontorno", dir_salida);

    if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas, numero_unidades, frecuencias, locutor, "wt", nombre_fichero, 1))
      return 1;


    sprintf(nombre_fichero, "%sunidades_escogidas_%d.txt", dir_salida, 1);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, unidades_escogidas, locutor, 0, "wt"))
      return 1;


    sprintf(nombre_fichero, "%sduraciones.txt", dir_salida);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(nombre_fichero, "%scontorno_escogido.txt", dir_salida);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades, nombre_fichero, frase_fonetica, cadena_unidades, acentuales_cand, cadena_acentuales, "wt"))
      return 1;

  } // if (dir_salida != NULL)

  viterbi_acentual.finaliza();

  if (formato_fichero == 0) {

    recorre_acentuales = (Grupo_acentual_candidato *) acentuales_cand;

    for (contador = 0; contador < numero_acentuales; contador++, recorre_acentuales++)
      recorre_acentuales->libera_memoria();

  } // if (formato_fichero == 0)

  free(frecuencias);
  free(acentuales_cand);

  return 0;

}


#ifdef _MODO_NORMAL

#ifdef _MINOR_PHRASING_TEMPLATE 

/**
 * \class	Seleccion_unidades
 * \brief	Intonation unit selection considering different prosodic structures
 * \param[in]	cadena_acentuales	array of target accent groups
 * \param[in]	numero_acentuales	number of elements of the previous array
 * \param[in]	frase_objetivo	target prosodic structure
 * \param[in]	frase_candidata	vector of prosodic structures
 * \param[in]	numero_total_contornos_entonativos	total number of contours resulting from the
 unit search
 * \param[in]	numero_rutas_optimas	initial number of intonation contours for the next
 stage (acoustic unit selection)
 * \param[in]	locutor	pointer to the speaker database
 * \param[out]	contornos_escogidos	array of chosen contours
 * \param[out]	costes_minimos	array of costs of the chosen sequences of units
 * \param[out]	numero_contornos	number of elements in the previous array
 * \param[in]   nombre_fichero_entrada parte inicial del nombre del fichero de salida. Se utilizará para generar los ficheros con la información de los contornos escogidos
 * \param[in]   opciones si es distinto de cero, se escribe la información de los contornos escogidos
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \return	0, in absence of error
 */

int Seleccion_unidades::seleccion_unidades_entonativas(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, Grupo_acentual_candidato ***contornos_escogidos, float **costes_minimos, int *numero_contornos, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras) {

  static int numero_frase = 1;

  int indice_acentual, contador;
  int numero_rutas_optimas_reales;
  int numero_maximo_caminos_considerados, numero_contornos_local;

  int *relacion_original;

  int numero_estructuras_prosodicas = frase_candidata.size();

  int numero_estructuras_prosodicas_validas = 0;

  int numero_contornos_actuales = 0;

  float coste_actual;

  Grupo_acentual_objetivo *puntero_acentual;
  Grupo_acentual_candidato *acentuales_cand;

  if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
    numero_total_contornos_entonativos = MEJORES_CONTORNOS;

  // Lo mismo para la otra variable

  if (numero_rutas_optimas == 0)
    numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;

  if (numero_total_contornos_entonativos < numero_rutas_optimas) {
    fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades_entonativas. El número total de contornos (N) debe ser mayor o igual que el de los seleccionados (M).\n");
    return 1;
  } // if (num_total_contornos_entonativos < ...

  numero_maximo_caminos_considerados = numero_total_contornos_entonativos*(numero_estructuras_prosodicas + 1);

  if ( (*contornos_escogidos = (Grupo_acentual_candidato **)
	malloc(numero_maximo_caminos_considerados*sizeof(Grupo_acentual_candidato *)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*contornos_escogidos = ...

  if ( (*costes_minimos = (float *) malloc(numero_maximo_caminos_considerados*sizeof(float)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*costes_minimos = ...

  if ( (relacion_original = (int *) malloc(numero_maximo_caminos_considerados*sizeof(int)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (relacion_original = ...


  if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
    return 1;

  viterbi_acentual.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  // Primero hacemos la búsqueda para cada estructura prosódica:

  for (int cuenta_estructuras = 0; cuenta_estructuras < numero_estructuras_prosodicas; cuenta_estructuras++) {

    // Comprobamos si es una estructura válida:

    if (Frase_frontera::candidato_valido(frase_objetivo, &frase_candidata[cuenta_estructuras]) == 0)
      continue;

    //        printf("\tEstructura prosódica válida.\n");

    numero_estructuras_prosodicas_validas++;

    if (Frase_frontera::establece_estructura_prosodica(&frase_candidata[cuenta_estructuras], frase_objetivo, cadena_acentuales, numero_acentuales))
      return 1;

#ifdef _DEPURA_VITERBI_ACENTUAL
    viterbi_acentual.avanza_iterador();
#endif

    // Para cada estructura prosódica, una búsqueda de Viterbi:

    puntero_acentual = cadena_acentuales;

#ifdef _DEPURA_VITERBI_ACENTUAL
    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							   numero_acentuales, (char *) "seleccion_entonativa"))
      return 1;
#else
    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							   numero_acentuales))
      return 1;
#endif

    indice_acentual = 1;

    while (indice_acentual++ < numero_acentuales) {

      if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
	return 1;

    } // while (indice_acentual < numero_acentuales

    // Copiamos los mejores caminos:

    numero_contornos_local = numero_total_contornos_entonativos;

    for (contador = 0; contador < numero_contornos_local; contador++) {

      acentuales_cand = NULL;

      if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) contador, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
	return 1;

      if (numero_rutas_optimas_reales < numero_contornos_local)
	numero_contornos_local = numero_rutas_optimas_reales;

      (*costes_minimos)[numero_contornos_actuales] = coste_actual;
      (*contornos_escogidos)[numero_contornos_actuales++] = acentuales_cand;

    }

    viterbi_acentual.libera_memoria_algoritmo_acentual();

    if ( (numero_total_contornos_entonativos == 1) && (numero_rutas_optimas == 1) )
      break;

  } // for (int cuenta_estructuras = 0; ...

  viterbi_acentual.finaliza();

  if ( (numero_total_contornos_entonativos != 1) || (numero_rutas_optimas != 1)  || (numero_contornos_actuales == 0) ) {

    numero_maximo_caminos_considerados = (numero_estructuras_prosodicas_validas + 1)*numero_total_contornos_entonativos;
    
    if (numero_contornos_actuales < numero_maximo_caminos_considerados) {
      
      if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
	return 1;
      
      // Dejamos que decida la estructura prosódica el propio algoritmo entonativo
      
      Frase_frontera::establece_estructura_prosodica_por_defecto(cadena_acentuales, numero_acentuales);
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      viterbi_acentual.avanza_iterador();
#endif
      
      puntero_acentual = cadena_acentuales;
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     numero_acentuales, (char *) "seleccion_entonativa"))
	return 1;
#else
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     numero_acentuales))
	return 1;
#endif
      
      indice_acentual = 1;
      
      while (indice_acentual++ < numero_acentuales) {
	
	if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
	  return 1;
	
      } // while (indice_acentual < numero_acentuales
      
      // Copiamos los mejores caminos:
      
      numero_contornos_local = numero_total_contornos_entonativos;
      
      for (contador = 0; contador < numero_contornos_local; contador++) {
	
	acentuales_cand = NULL;
	
	if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) contador, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
	  return 1;
	
	if (numero_rutas_optimas_reales < numero_contornos_local)
	  numero_contornos_local = numero_rutas_optimas_reales;
	
	(*costes_minimos)[numero_contornos_actuales] = coste_actual;
	(*contornos_escogidos)[numero_contornos_actuales++] = acentuales_cand;
	
	if (contador >= numero_rutas_optimas_reales - 1)
	  break;
	
	if (numero_contornos_actuales >= numero_maximo_caminos_considerados)
	  break;
	
      }
      
      viterbi_acentual.libera_memoria_algoritmo_acentual();
      viterbi_acentual.finaliza();
      
    } // if (numero_estructuras_prosodicas_validas < numero_total_contornos_entonativos)

  } // if ( (numero_total_contornos_entonativos != 1) || (numero_rutas_optimas != 1) )
    
  // Reorganizamos los caminos (con sus costes), y nos quedamos con los que nos dan más variabilidad para
  // la búsqueda acústica:

  for (contador = 0; contador < numero_contornos_actuales; contador++)
    relacion_original[contador] = contador;

  if (numero_contornos_actuales < numero_rutas_optimas)
    numero_rutas_optimas = numero_contornos_actuales;

  Grupo_acentual_candidato::selecciona_mejores_opciones(*contornos_escogidos, numero_contornos_actuales, numero_rutas_optimas, numero_acentuales, 1, *costes_minimos, relacion_original);

 
  *numero_contornos = numero_rutas_optimas;

  if (opciones) {
    char nombre_fichero[FILENAME_MAX];
    sprintf(nombre_fichero, "%s_%d.id_contornos", nombre_fichero_entrada, numero_frase);
    
    if (escribe_identificadores_contornos_fichero(*contornos_escogidos, *numero_contornos, numero_acentuales, nombre_fichero))
      return 1;

    sprintf(nombre_fichero, "%s_%d.total_contornos", nombre_fichero_entrada, numero_frase);

    if (escribe_identificadores_contornos_fichero(*contornos_escogidos, numero_contornos_actuales, numero_acentuales, nombre_fichero))
      return 1;

  }

  // Liberamos la memoria de los contornos no escogidos:

  for (int j = numero_rutas_optimas; j < numero_contornos_actuales; j++)
    free((*contornos_escogidos)[j]);

  //    free(*contornos_escogidos);


  free(relacion_original);
  //    free(costes_minimos);
  numero_frase++;

  return 0;

}


/**
 * \class	Seleccion_unidades
 * \brief	Intonation unit selection considering different prosodic structures
 * \remarks     Returns coherent contours for the whole sentence
 * \param[in]	cadena_acentuales	array of target accent groups
 * \param[in]	numero_acentuales	number of elements of the previous array
 * \param[in]	frase_objetivo	target prosodic structure
 * \param[in]	frase_candidata	vector of prosodic structures
 * \param[in]	numero_total_contornos_entonativos	total number of contours resulting from the
 unit search
 * \param[in]	numero_rutas_optimas	initial number of intonation contours for the next
 stage (acoustic unit selection)
 * \param[in]	locutor	pointer to the speaker database
 * \param[out]	contornos_escogidos	array of chosen contours
 * \param[out]	costes_minimos	array of costs of the chosen sequences of units
 * \param[out]	numero_contornos	number of elements in the previous array
 * \param[in]   nombre_fichero_entrada parte inicial del nombre del fichero de salida. Se utilizará para generar los ficheros con la información de los contornos escogidos
 * \param[in]   opciones si es distinto de cero, se escribe la información de los contornos escogidos
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \return	0, in absence of error
 */

int Seleccion_unidades::seleccion_unidades_entonativas_por_grupos_fonicos_total(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, Grupo_acentual_candidato ***contornos_escogidos, float **costes_minimos, int *numero_contornos, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras) {

  static int numero_frase = 1;

  int indice_acentual, contador;
  int numero_rutas_optimas_reales;
  int numero_maximo_caminos_considerados, numero_contornos_local;

  int numero_grupos_fonicos = 0;
  int tamano_grupo = 0;

  int *relacion_original;

  int numero_estructuras_prosodicas = frase_candidata.size();

  int numero_estructuras_prosodicas_validas = 0;

  int numero_contornos_actuales = 0;
  int numero_contornos_actuales_estructura = 0;

  float coste_actual;

  float *costes_minimos_libre;
  Grupo_acentual_candidato **contornos_escogidos_libre;

  Grupo_acentual_objetivo *puntero_acentual = cadena_acentuales;
  Grupo_acentual_objetivo *inicio_grupo_fonico = cadena_acentuales;

  Grupo_acentual_candidato *acentuales_cand;

  vector<int> tamano_grupo_fonico;
  // Contamos el número de grupos fónicos:
  for (indice_acentual = 0; indice_acentual < numero_acentuales; indice_acentual++, puntero_acentual++) {
    tamano_grupo++;

    // Ante la duda de qué hacer con las rupturas coma, internamente las vamos a transformar en rupturas entonativas
    if (puntero_acentual->tipo_pausa == PAUSA_RUPTURA_COMA)
      puntero_acentual->tipo_pausa = PAUSA_RUPTURA_ENTONATIVA;
      
    if ( (puntero_acentual->tipo_pausa != SIN_PAUSA) &&
	 (puntero_acentual->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) ) {
      numero_grupos_fonicos++;
      tamano_grupo_fonico.push_back(tamano_grupo);
      tamano_grupo = 0;
    }

  }


  if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
    numero_total_contornos_entonativos = MEJORES_CONTORNOS;

  // Lo mismo para la otra variable

  if (numero_rutas_optimas == 0)
    numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;

  if (numero_total_contornos_entonativos < numero_rutas_optimas) {
    fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades_entonativas. El número total de contornos (N) debe ser mayor o igual que el de los seleccionados (M).\n");
    return 1;
  } // if (num_total_contornos_entonativos < ...

  numero_maximo_caminos_considerados = numero_total_contornos_entonativos + numero_estructuras_prosodicas;

  vector<vector<Grupo_acentual_candidato *> > vector_grupos; //(numero_grupos_fonicos, vector<Grupo_acentual_candidato *> (numero_maximo_caminos_considerados));
  vector<vector<float> > costes_grupos; //(numero_grupos_fonicos, vector<float>(numero_maximo_caminos_considerados));

  vector_grupos.reserve(numero_grupos_fonicos);
  costes_grupos.reserve(numero_grupos_fonicos);

  for (contador = 0; contador < numero_grupos_fonicos; contador++) {
    vector_grupos.push_back(vector<Grupo_acentual_candidato *> () );
    costes_grupos.push_back(vector<float> ());
  }

  if ( (*contornos_escogidos = (Grupo_acentual_candidato **)
	malloc(numero_maximo_caminos_considerados*sizeof(Grupo_acentual_candidato *)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*contornos_escogidos = ...

  if ( (contornos_escogidos_libre = (Grupo_acentual_candidato **)
	malloc(numero_maximo_caminos_considerados*sizeof(Grupo_acentual_candidato *)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*contornos_escogidos_libre = ...

  if ( (*costes_minimos = (float *) malloc(numero_maximo_caminos_considerados*sizeof(float)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*costes_minimos = ...

  if ( (costes_minimos_libre = (float *) malloc(numero_maximo_caminos_considerados*sizeof(float)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos_libre = ...

  if ( (relacion_original = (int *) malloc(numero_maximo_caminos_considerados*sizeof(int)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (relacion_original = ...


  if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
    return 1;

  viterbi_acentual.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  // Primero hacemos la búsqueda para cada estructura prosódica:
  
  for (int cuenta_estructuras = 0; cuenta_estructuras < numero_estructuras_prosodicas; cuenta_estructuras++) {
    
    // Comprobamos si es una estructura válida:
    
    if (Frase_frontera::candidato_valido(frase_objetivo, &frase_candidata[cuenta_estructuras]) == 0)
      continue;
    
    numero_estructuras_prosodicas_validas++;
    
    if (Frase_frontera::establece_estructura_prosodica(&frase_candidata[cuenta_estructuras], frase_objetivo, cadena_acentuales, numero_acentuales))
      return 1;
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    viterbi_acentual.avanza_iterador();
#endif
    
    // Para cada estructura prosódica, una búsqueda de Viterbi:
    
    puntero_acentual = cadena_acentuales;
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							   numero_acentuales,
							   (char *) "sel_entonativa_estructura"))
      return 1;
#else
    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							   numero_acentuales))
      return 1;
#endif
    indice_acentual = 1;
    
    while (indice_acentual++ < numero_acentuales) {
      
      if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
	return 1;
      
    } // while (indice_acentual < numero_acentuales
    
      // Copiamos los mejores caminos:
    
    numero_contornos_local = numero_total_contornos_entonativos;
    
    // Nos quedamos únicamente con el mejor camino:

    acentuales_cand = NULL;
    
    if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) 0, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
      return 1;
    
    (*costes_minimos)[numero_contornos_actuales_estructura] = coste_actual;
    (*contornos_escogidos)[numero_contornos_actuales_estructura++] = acentuales_cand;
    
    viterbi_acentual.libera_memoria_algoritmo_acentual();
    
  } // for (int cuenta_estructuras = 0; ...
  
  viterbi_acentual.finaliza();
  
  if (numero_contornos_actuales_estructura < numero_maximo_caminos_considerados) {
    
    // Dejamos que decida la estructura prosódica el propio algoritmo entonativo
      
    Frase_frontera::establece_estructura_prosodica_por_defecto(cadena_acentuales, numero_acentuales);
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    viterbi_acentual.reinicia_iterador();
#endif

    for (int cuenta_grupo_fonico = 0; cuenta_grupo_fonico < numero_grupos_fonicos; cuenta_grupo_fonico++) {
      
      numero_contornos_actuales = 0;

      if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
	return 1;
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      viterbi_acentual.avanza_iterador(); // Hay que arreglar esto...
#endif
      
      puntero_acentual = inicio_grupo_fonico;
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     tamano_grupo_fonico[cuenta_grupo_fonico], (char *) "sel_entonativa_grupo_fonico"))
	return 1;
#else
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     tamano_grupo_fonico[cuenta_grupo_fonico]))
	return 1;
#endif
      
      indice_acentual = 1;
      
      while (indice_acentual++ < tamano_grupo_fonico[cuenta_grupo_fonico]) {
	
	if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
	  return 1;
	
      } // while (indice_acentual < numero_acentuales
      
      inicio_grupo_fonico = puntero_acentual;

      // Copiamos los mejores caminos:
      
      numero_contornos_local = numero_total_contornos_entonativos;
      
      for (contador = 0; contador < numero_contornos_local; contador++) {
	
	acentuales_cand = NULL;

	if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) contador, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
	  return 1;
	
	if (numero_rutas_optimas_reales < numero_contornos_local)
	  numero_contornos_local = numero_rutas_optimas_reales;
	
	
	contornos_escogidos_libre[numero_contornos_actuales] = acentuales_cand;
	costes_minimos_libre[numero_contornos_actuales++] = coste_actual;

	if (contador >= numero_rutas_optimas_reales - 1)
	  break;
	
	if (numero_contornos_actuales + numero_contornos_actuales_estructura >= numero_maximo_caminos_considerados)
	  break;
	
      }

      numero_contornos_actuales = numero_contornos_local;

      viterbi_acentual.libera_memoria_algoritmo_acentual();
      viterbi_acentual.finaliza();

      for (contador = 0; contador < numero_contornos_actuales; contador++)
	relacion_original[contador] = contador;
  
      if (numero_contornos_local > numero_rutas_optimas)
	numero_contornos_local = numero_rutas_optimas;

      Grupo_acentual_candidato::selecciona_mejores_opciones(contornos_escogidos_libre, numero_contornos_actuales, numero_contornos_local, tamano_grupo_fonico[cuenta_grupo_fonico], 1, costes_minimos_libre, relacion_original);

      // Copiamos los escogidos al vector de grupos:
      
      for (contador = 0; contador < numero_contornos_local; contador++) {
	vector_grupos[cuenta_grupo_fonico].push_back(contornos_escogidos_libre[contador]);
	costes_grupos[cuenta_grupo_fonico].push_back(costes_minimos_libre[contador]);
      }

    } // for (int cuenta_grupo_fonico = 0;...

#ifdef _DEPURA_VITERBI_ACENTUAL
    
    FILE *fichero;
    
    if ( (fichero = fopen("seleccion_grupos_fonicos.txt", "wt")) == NULL) {
      fprintf(stderr, "Error en seleccion_unidades_entonativas_por_grupos_fonicos, al crear el fichero seleccion_grupos_fonicos.txt");
      return 1;
    }

    vector<vector<Grupo_acentual_candidato * > >::iterator it_grupos = vector_grupos.begin();
    vector<Grupo_acentual_candidato *>::iterator it_acentual;
    vector<vector<float> >::iterator it_vector_costes = costes_grupos.begin();
    vector<float>::iterator it_coste;
    int indice;

    for (contador = 0; contador < numero_grupos_fonicos; contador++, it_grupos++, it_vector_costes++) {

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

#endif

    // Ahora tenemos que considerar las transiciones entre grupos fónicos:

    viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor);
    
    numero_contornos_actuales = 0;

#ifdef _DEPURA_VITERBI_ACENTUAL
    viterbi_acentual.reinicia_iterador();
    viterbi_acentual.avanza_iterador();

    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual_grupos_fonicos(vector_grupos[0], costes_grupos[0], tamano_grupo_fonico[0], numero_grupos_fonicos, (char *) "seleccion_post_entonativa"))
      return 1;
#else
    if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual_grupos_fonicos(vector_grupos[0], costes_grupos[0], tamano_grupo_fonico[0], numero_grupos_fonicos))
      return 1;
#endif

    indice_acentual = 1;
    
    while (indice_acentual < numero_grupos_fonicos) {
      
      if (viterbi_acentual.siguiente_recursion_Viterbi_grupos_fonicos(vector_grupos[indice_acentual], costes_grupos[indice_acentual], tamano_grupo_fonico[indice_acentual]))
	return 1;
      
      indice_acentual++;

    } // while (indice_acentual < numero_acentuales
      

    // Recuperamos los mejores caminos:
    numero_contornos_local = numero_total_contornos_entonativos;

    for (contador = 0; contador < numero_contornos_local; contador++) {
      
      acentuales_cand = NULL;

      if (viterbi_acentual.devuelve_camino_optimo_acentual_GF((unsigned char) contador, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, tamano_grupo_fonico, &coste_actual))
	return 1;
      
      if (numero_rutas_optimas_reales < numero_contornos_local)
	numero_contornos_local = numero_rutas_optimas_reales;

      costes_minimos_libre[numero_contornos_actuales] = coste_actual;
      contornos_escogidos_libre[numero_contornos_actuales++] = acentuales_cand;

    } // for (contador = 0; contador 

    // Liberamos la memoria del vector de vectores de caminos:

    vector<vector<Grupo_acentual_candidato *> >::iterator it_gf = vector_grupos.begin();
    vector<Grupo_acentual_candidato *>:: iterator if_ga;

    while (it_gf != vector_grupos.end() ) {

      if_ga = it_gf->begin();
      while (if_ga != it_gf->end() ) {
	(*if_ga)->libera_memoria();
	free(*if_ga++);
      }
      ++it_gf;

    } // while (it_gf != ...

    // Reorganizamos los caminos (con sus costes), y nos quedamos con los que nos dan más variabilidad para la búsqueda acústica:
  
    for (contador = 0; contador < numero_contornos_actuales; contador++)
      relacion_original[contador] = contador;
  
    if (numero_contornos_actuales < numero_rutas_optimas)
      numero_rutas_optimas = numero_contornos_actuales;
  
    Grupo_acentual_candidato::selecciona_mejores_opciones(contornos_escogidos_libre, numero_contornos_actuales, numero_rutas_optimas, numero_acentuales, 1, costes_minimos_libre, relacion_original);
  
    numero_contornos_actuales = numero_rutas_optimas;
  
  } // if (numero_estructuras_prosodicas_validas < numero_total_contornos_entonativos)

  // Combinamos los contornos de ambas búsquedas:

  for (contador = numero_contornos_actuales_estructura; contador < numero_contornos_actuales_estructura + numero_contornos_actuales; contador++) {
    (*contornos_escogidos)[contador] = contornos_escogidos_libre[contador - numero_contornos_actuales_estructura];
    (*costes_minimos)[contador] = costes_minimos_libre[contador - numero_contornos_actuales_estructura];
  }

  // Y volvemos a reordenarlos:

  for (contador = 0; contador < numero_contornos_actuales + numero_contornos_actuales_estructura; contador++)
    relacion_original[contador] = contador;

  Grupo_acentual_candidato::selecciona_mejores_opciones(*contornos_escogidos, numero_contornos_actuales + numero_contornos_actuales_estructura, numero_rutas_optimas, numero_acentuales, 1, *costes_minimos, relacion_original);

  *numero_contornos = numero_rutas_optimas;
  
  if (opciones) {
    char nombre_fichero[FILENAME_MAX];
    sprintf(nombre_fichero, "%s_%d.id_contornos", nombre_fichero_entrada, numero_frase);
    
    if (escribe_identificadores_contornos_fichero(*contornos_escogidos, *numero_contornos, numero_acentuales, nombre_fichero))
      return 1;
    
    sprintf(nombre_fichero, "%s_%d.total_contornos", nombre_fichero_entrada, numero_frase);

    if (escribe_identificadores_contornos_fichero(*contornos_escogidos, numero_contornos_actuales, numero_acentuales, nombre_fichero))
      return 1;

  }

  // Liberamos la memoria de los contornos no escogidos:

  for (int j = numero_rutas_optimas; j < numero_contornos_actuales; j++)
    free((*contornos_escogidos)[j]);

  //    free(*contornos_escogidos);


  free(relacion_original);
  free(contornos_escogidos_libre);
  free(costes_minimos_libre);

  numero_frase++;

  return 0;

}


/**
 * \class	Seleccion_unidades
 * \brief	Intonation unit selection considering different prosodic structures
 * \remarks     Returns coherent contours for the whole sentence
 * \param[in]	cadena_acentuales	array of target accent groups
 * \param[in]	numero_acentuales	number of elements of the previous array
 * \param[in]	frase_objetivo	target prosodic structure
 * \param[in]	frase_candidata	vector of prosodic structures
 * \param[in]	numero_total_contornos_entonativos	total number of contours resulting from the
 unit search
 * \param[in]	numero_rutas_optimas	initial number of intonation contours for the next
 stage (acoustic unit selection)
 * \param[in]	locutor	pointer to the speaker database
 * \param[out]	vector_grupos	vector of vectors with the multiple possibilities for each phonic group
 * \param[out]	vector_costes	vector of vectors with the costs of each possibility
 * \param[out]  tamano_grupo_fonico número de grupos acentuales de cada grupo fónico
 * \param[in]   nombre_fichero_entrada parte inicial del nombre del fichero de salida. Se utilizará para generar los ficheros con la información de los contornos escogidos
 * \param[in]   opciones si es distinto de cero, se escribe la información de los contornos escogidos
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \return	0, in absence of error
 */

int Seleccion_unidades::seleccion_unidades_entonativas_por_grupos_fonicos(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, vector<vector<Grupo_acentual_candidato *> > & vector_grupos, vector<vector<float> > &vector_costes, vector<int> &tamano_grupo_fonico, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras) {

  static int numero_frase = 1;

  int indice_acentual, contador;
  int numero_rutas_optimas_reales;
  int numero_maximo_caminos_considerados, numero_contornos_local;

  int numero_grupos_fonicos = 0;
  int tamano_grupo = 0;

  int *relacion_original;

  int numero_estructuras_prosodicas = frase_candidata.size();

  int numero_estructuras_prosodicas_validas = 0;

  int numero_contornos_actuales = 0;
  int numero_contornos_actuales_estructura = 0;

  float coste_actual;

  float *costes_minimos_libre;
  Grupo_acentual_candidato **contornos_escogidos_libre;

  Grupo_acentual_objetivo *puntero_acentual = cadena_acentuales;
  Grupo_acentual_objetivo *inicio_grupo_fonico = cadena_acentuales;

  Grupo_acentual_candidato *acentuales_cand;

  // Contamos el número de grupos fónicos:
  for (indice_acentual = 0; indice_acentual < numero_acentuales; indice_acentual++, puntero_acentual++) {
    tamano_grupo++;

    // Ante la duda de qué hacer con las rupturas coma, internamente las vamos a transformar en rupturas entonativas
    if (puntero_acentual->tipo_pausa == PAUSA_RUPTURA_COMA)
      puntero_acentual->tipo_pausa = PAUSA_RUPTURA_ENTONATIVA;

    if ( (puntero_acentual->tipo_pausa != SIN_PAUSA) &&
	 (puntero_acentual->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) ) {
      numero_grupos_fonicos++;
      tamano_grupo_fonico.push_back(tamano_grupo);
      tamano_grupo = 0;
    }

  }

  numero_maximo_caminos_considerados = numero_total_contornos_entonativos + numero_estructuras_prosodicas;

  if ( (contornos_escogidos_libre = (Grupo_acentual_candidato **)
	malloc(numero_maximo_caminos_considerados*sizeof(Grupo_acentual_candidato *)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*contornos_escogidos_libre = ...

  if ( (costes_minimos_libre = (float *) malloc(numero_maximo_caminos_considerados*sizeof(float)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (*costes_minimos = ...


  if (numero_total_contornos_entonativos == 0) // Si es cero, cogemos el valor por defecto.
    numero_total_contornos_entonativos = MEJORES_CONTORNOS;

  // Lo mismo para la otra variable

  if (numero_rutas_optimas == 0)
    numero_rutas_optimas = NUMERO_RUTAS_OPTIMAS;

  if (numero_total_contornos_entonativos < numero_rutas_optimas) {
    fprintf(stderr, "Error en Seleccion_unidades::seleccion_unidades_entonativas. El número total de contornos (N) debe ser mayor o igual que el de los seleccionados (M).\n");
    return 1;
  } // if (num_total_contornos_entonativos < ...


  vector_grupos.reserve(numero_grupos_fonicos);
  vector_costes.reserve(numero_grupos_fonicos);

  for (contador = 0; contador < numero_grupos_fonicos; contador++) {
    vector_grupos.push_back(vector<Grupo_acentual_candidato *> () );
    vector_costes.push_back(vector<float> ());
  }

  if ( (relacion_original = (int *) malloc(numero_maximo_caminos_considerados*sizeof(int)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_entonativas, al asignar memoria.\n");
    return 1;
  } // if ( (relacion_original = ...


  if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
    return 1;

  viterbi_acentual.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  // Primero hacemos la búsqueda para cada estructura prosódica:
  
  //   for (int cuenta_estructuras = 0; cuenta_estructuras < numero_estructuras_prosodicas; cuenta_estructuras++) {
    
  //     // Comprobamos si es una estructura válida:
    
  //     if (Frase_frontera::candidato_valido(frase_objetivo, &frase_candidata[cuenta_estructuras]) == 0)
  //       continue;
    
  //     numero_estructuras_prosodicas_validas++;
    
  //     if (Frase_frontera::establece_estructura_prosodica(&frase_candidata[cuenta_estructuras], frase_objetivo, cadena_acentuales, numero_acentuales))
  //       return 1;
    
  // #ifdef _DEPURA_VITERBI_ACENTUAL
  //     viterbi_acentual.avanza_iterador();
  // #endif
    
  //     // Para cada estructura prosódica, una búsqueda de Viterbi:
    
  //     puntero_acentual = cadena_acentuales;
    
  // #ifdef _DEPURA_VITERBI_ACENTUAL
  //     if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
  // 							   numero_acentuales,
  // 							   (char *) "sel_entonativa_estructura"))
  //       return 1;
  // #else
  //     if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
  // 							   numero_acentuales))
  //       return 1;
  // #endif
  //     indice_acentual = 1;
    
  //     while (indice_acentual++ < numero_acentuales) {
      
  //       if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
  // 	return 1;
      
  //     } // while (indice_acentual < numero_acentuales
    
  //       // Copiamos los mejores caminos:
    
  //     numero_contornos_local = numero_total_contornos_entonativos;
    
  //     // Nos quedamos únicamente con el mejor camino:

  //     acentuales_cand = NULL;
    
  //     if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) 0, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
  //       return 1;
    
  //     (*costes_minimos)[numero_contornos_actuales_estructura] = coste_actual;
  //     (*contornos_escogidos)[numero_contornos_actuales_estructura++] = acentuales_cand;
    
  //     viterbi_acentual.libera_memoria_algoritmo_acentual();
    
  //   } // for (int cuenta_estructuras = 0; ...
  
  //   viterbi_acentual.finaliza();
  
  if (numero_contornos_actuales_estructura < numero_maximo_caminos_considerados) {
    
    // Dejamos que decida la estructura prosódica el propio algoritmo entonativo
      
    Frase_frontera::establece_estructura_prosodica_por_defecto(cadena_acentuales, numero_acentuales);
    
#ifdef _DEPURA_VITERBI_ACENTUAL
    viterbi_acentual.reinicia_iterador();
#endif

    for (int cuenta_grupo_fonico = 0; cuenta_grupo_fonico < numero_grupos_fonicos; cuenta_grupo_fonico++) {
      
      numero_contornos_actuales = 0;

      if (viterbi_acentual.inicializa(numero_total_contornos_entonativos, numero_rutas_optimas, locutor))
	return 1;
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      viterbi_acentual.avanza_iterador(); // Hay que arreglar esto...
#endif
      
      puntero_acentual = inicio_grupo_fonico;
      
#ifdef _DEPURA_VITERBI_ACENTUAL
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     tamano_grupo_fonico[cuenta_grupo_fonico], (char *) "sel_entonativa_grupo_fonico"))
	return 1;
#else
      if (viterbi_acentual.inicia_algoritmo_Viterbi_acentual(puntero_acentual++,
							     tamano_grupo_fonico[cuenta_grupo_fonico]))
	return 1;
#endif
      
      indice_acentual = 1;
      
      while (indice_acentual++ < tamano_grupo_fonico[cuenta_grupo_fonico]) {
	
	if (viterbi_acentual.siguiente_recursion_Viterbi_V2(puntero_acentual++))
	  return 1;
	
      } // while (indice_acentual < numero_acentuales
      
      inicio_grupo_fonico = puntero_acentual;

      // Copiamos los mejores caminos:
      
      numero_contornos_local = numero_total_contornos_entonativos;
      
      for (contador = 0; contador < numero_contornos_local; contador++) {
	
	acentuales_cand = NULL;

	if (viterbi_acentual.devuelve_camino_optimo_acentual((unsigned char) contador, &acentuales_cand, &numero_acentuales, &numero_rutas_optimas_reales, &coste_actual))
	  return 1;
	
	if (numero_rutas_optimas_reales < numero_contornos_local)
	  numero_contornos_local = numero_rutas_optimas_reales;
	
	
	contornos_escogidos_libre[numero_contornos_actuales] = acentuales_cand;
	costes_minimos_libre[numero_contornos_actuales++] = coste_actual;

	if (contador >= numero_rutas_optimas_reales - 1)
	  break;
	
	if (numero_contornos_actuales + numero_contornos_actuales_estructura >= numero_maximo_caminos_considerados)
	  break;
	
      }

      numero_contornos_actuales = numero_contornos_local;

      viterbi_acentual.libera_memoria_algoritmo_acentual();
      viterbi_acentual.finaliza();

      for (contador = 0; contador < numero_contornos_actuales; contador++)
	relacion_original[contador] = contador;
  
      if (numero_contornos_local > numero_rutas_optimas)
	numero_contornos_local = numero_rutas_optimas;

      Grupo_acentual_candidato::selecciona_mejores_opciones(contornos_escogidos_libre, numero_contornos_actuales, numero_contornos_local, tamano_grupo_fonico[cuenta_grupo_fonico], 1, costes_minimos_libre, relacion_original);

      // Copiamos los escogidos al vector de grupos:
      
      for (contador = 0; contador < numero_contornos_local; contador++) {
	vector_grupos[cuenta_grupo_fonico].push_back(contornos_escogidos_libre[contador]);
	vector_costes[cuenta_grupo_fonico].push_back(costes_minimos_libre[contador]);
      }

      // Liberamos la memoria de los no escogidos:
      for (; contador < numero_contornos_actuales; contador++) {
	Grupo_acentual_candidato *apunta_grupo = contornos_escogidos_libre[contador];
	for (int cuenta = 0; cuenta < tamano_grupo_fonico[cuenta_grupo_fonico]; cuenta++, apunta_grupo++)
	  apunta_grupo->libera_memoria();
	free(contornos_escogidos_libre[contador]);
      }
    } // for (int cuenta_grupo_fonico = 0;...

  } // if (numero_estructuras_prosodicas_validas < numero_total_contornos_entonativos)

  // Combinamos los contornos de ambas búsquedas:

  // for (contador = numero_contornos_actuales_estructura; contador < numero_contornos_actuales_estructura + numero_contornos_actuales; contador++) {
  //   (*contornos_escogidos)[contador] = contornos_escogidos_libre[contador - numero_contornos_actuales_estructura];
  //   (*costes_minimos)[contador] = costes_minimos_libre[contador - numero_contornos_actuales_estructura];
  // }

  // Y volvemos a reordenarlos:

  // for (contador = 0; contador < numero_contornos_actuales + numero_contornos_actuales_estructura; contador++)
  //   relacion_original[contador] = contador;

  // Grupo_acentual_candidato::selecciona_mejores_opciones(*contornos_escogidos, numero_contornos_actuales + numero_contornos_actuales_estructura, numero_rutas_optimas, numero_acentuales, 1, *costes_minimos, relacion_original);

  if (opciones) {
    char nombre_fichero[FILENAME_MAX];
    sprintf(nombre_fichero, "%s_%d.id_contornos", nombre_fichero_entrada, numero_frase);
    
    if (escribe_identificadores_contornos_fichero(vector_grupos, vector_costes, tamano_grupo_fonico,  nombre_fichero))
      return 1;

  }

  free(relacion_original);
  free(contornos_escogidos_libre);
  free(costes_minimos_libre);

  numero_frase++;

  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye
 * la selección de contornos entonativos basada en Corpus. Se diferencia de la anterior
 * en que en este caso se considera la continuidad de f0 tras una pausa en la
 * búsqueda entonativa.
 * \param	cadena_unidades: cadena con los vectores descriptores objetivo de las
 * unidades acústicas deseadas.
 * \param	numero_unidades: número de elementos de la cadena anterior.
 * \param	cadena_acentuales: cadena con los vectores que describen los grupos
 acentuales objetivo.
 * \param	acentuales_cand: cadena con los contornos de grupos acentuales candidatos
 * \param	costes_caminos_acentuales: cadena con los costes de cada camino acentual 
 * \param	numero_acentuales: número de elementos de la cadena anterior.
 * \param	escalado_frecuencia: valor por el que se desea escalar la frecuencia.
 * \param	numero_rutas_optimas: número de contornos que pasan a la búsqueda acústica.
 * \param	locutor: puntero al locutor empleado.
 * \param       tipo_candidatos: forma de seleccionar las unidades candidatas para la búsqueda de Viterbi.
 * \param	opciones: opción de funcionamiento:
 0: únicamente se realiza la selección.
 1: se escribe información sobre los contornos escogidos.
 2: genera los ficheros del modo _CALCULA_FACTORES.
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida.
 * \param	frase_fonetica: cadena con la transcripción fonética de la frase.
 * \param	frase_entrada: frase de entrada.
 * \param	nombre_fichero_entrada: nombre del fichero de entrada.
 * \param	ficheros: cadena de ficheros del modo _CALCULA_FACTORES.
 * \param fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \retval	cadena_sintesis: cadena con los vectores seleccionados.
 * \retval	numero_vectores_sintesis: número de elementos de la cadena anterior.
 * \return	En ausencia de error, devuelve un cero.
 */
int Seleccion_unidades::seleccion_unidades_acusticas(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  static int cuenta_frases = 1;
  float coste_camino_escogido = 0;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  int indice_primer_fonema_grupo_fonico;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_unidades_grupo_fonico = 1;
  int numero_total_grupos_prosodicos = -1;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo;
  float *costes_minimos, coste_actual;
  short int *cadena_posiciones, *apunta_posiciones;

  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_grupo_fonico, puntero_objetivo, apunta_objetivo;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  //    numero_grupos_grupo_fonico_actual = 0;
  //    indice_primer_grupo_acentual_actual = 0;
  indice_primer_fonema_grupo_fonico = 0;
  //    indice_camino_sintesis = 0;
  izquierdo_derecho = IZQUIERDO;

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error an seleccion_unidades, al asignar memoria.\n");
    return 1;
  }

  // Contamos el número de grupos fónicos, y salvamos las posiciones originales:

  apunta_objetivo = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;


  for (; apunta_objetivo != cadena_unidades.end(); apunta_objetivo++) {
    //    for (contador = 0; contador < numero_unidades; contador++) {
    *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
    //    if (apunta_objetivo->semifonema[0] == '#')
    //  numero_total_grupos_prosodicos++;
  }

  // Culio
  numero_total_grupos_prosodicos = 1;

  // Ya no sirve lo siguiente porque ahora consideramos rupturas entonativas.
  //	numero_total_grupos_prosodicos = puntero_acentual->numero_grupos_prosodicos;


  // Actualmente, el número de vectores de síntesis es siempre el doble que el de
  // unidades:

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  apunta_salida = *cadena_sintesis;

  // Inicializamos a cero la cadena de costes mínimos:

  for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas; cuenta_costes++)
    costes_minimos[cuenta_costes] = 0.0;

  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {

    // Escogemos el siguiente contorno posible:

    Grupo_acentual_candidato *apunta_grupo = acentuales_cand[contador_acentuales];

    // Comprobamos la continuidad entre contornos de grupos acentuales.

    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);

    // Creamos la cadena de valores de f0 objetivo:

    estructura_frecuencias *apunta_frecuencia = frecuencias + contador_acentuales*numero_unidades;

#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#endif

    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, apunta_grupo, numero_acentuales))
      return 1;
	
				    
    // Ahora, hacemos la búsqueda acústica por grupos fónicos:

    indice_primer_fonema_grupo_fonico = 0;
    numero_unidades_grupo_fonico = 1;
    ultimo_grupo_prosodico = 1;
    indice_camino_sintesis = 0;
    izquierdo_derecho = IZQUIERDO;
    puntero_objetivo = cadena_unidades.begin();
    avanza_puntero_objetivo = 1;

#ifdef _DEPURA_VITERBI
    viterbi.avanza_contorno();
#endif

    for (int cuenta_fonicos = 0; cuenta_fonicos < numero_total_grupos_prosodicos; cuenta_fonicos++) {

      inicio_objetivo_grupo_fonico = puntero_objetivo;

      if (cuenta_fonicos == numero_total_grupos_prosodicos - 1)
	ultimo_grupo_prosodico = 0;

      indice_primer_fonema_grupo_fonico += numero_unidades_grupo_fonico - 1; // Acordarme de actualizarlo fuera del bucle.

      numero_unidades_grupo_fonico = numero_unidades;

#ifndef _MODELO_ENTONATIVO_ANTERIOR

      // Añadimos las frecuencias a la cadena de objetivos:

      frec = frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_grupo_fonico;

      for (cuenta = 0; cuenta < numero_unidades_grupo_fonico; cuenta++, frec++)
	inicio_objetivo_grupo_fonico[cuenta].anhade_frecuencia(frec->frecuencia_mitad, frec->frecuencia_inicial, frec->frecuencia_final);
#endif

      // Selección de unidades de voz:

      if (inicio_objetivo_grupo_fonico != cadena_unidades.begin())
	izquierdo_derecho = DERECHO;

      //            puntero_objetivo = inicio_objetivo_grupo_fonico;

      // Liberamos la memoria

      viterbi.libera_memoria_algoritmo();

      if (puntero_objetivo == cadena_unidades.begin())
	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - ultimo_grupo_prosodico;
      else
	numero_semifonemas_real_grupo = 2*numero_unidades_grupo_fonico - 1 - ultimo_grupo_prosodico;

      if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho))
	return 1;
	    
      if (puntero_objetivo != cadena_unidades.begin())
	puntero_objetivo++;
	    
      for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
	      
	if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo,
						(contador == numero_semifonemas_real_grupo - 1) && !ultimo_grupo_prosodico))
	  return 1;

	if (contador % 2 == avanza_puntero_objetivo)
	  puntero_objetivo++;

      } // for (int contador = 0

      // Ahora escogemos el mejor camino hasta ese punto:

      puntero_sintesis =
	unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;

      if (viterbi.devuelve_camino_optimo_sintesis(&numero_semifonemas_grupo_actual,
						  &coste_actual,
						  &puntero_sintesis, &numero_rutas_optimas_reales))
	return 1;

      costes_minimos[contador_acentuales] +=  coste_actual;
      indice_camino_sintesis += numero_semifonemas_grupo_actual;
      avanza_puntero_objetivo = 0;

    } // for (int cuenta_fonicos = 0; ...


  } // for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas...

  // Buscamos el mejor camino:

  indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas);

  coste_camino_escogido = costes_minimos[indice_coste_minimo];
    
#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:

  frec = frecuencias + indice_coste_minimo*numero_unidades;
  inicio_objetivo_grupo_fonico = cadena_unidades.begin();
    
  for (; inicio_objetivo_grupo_fonico != puntero_objetivo;
       inicio_objetivo_grupo_fonico++, frec++)
    inicio_objetivo_grupo_fonico->anhade_frecuencia(frec->frecuencia_mitad,
						    frec->frecuencia_inicial,
						    frec->frecuencia_final);
#endif

  if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[indice_coste_minimo], numero_acentuales))
    return 1;

  // Escribimos las unidades escogidas en la cadena de salida:

  puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, puntero_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  for (cuenta = 0; cuenta < numero_unidades_candidatas; cuenta++)
    *apunta_salida++ = *puntero_sintesis++;

  viterbi.libera_memoria_algoritmo();

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_camino_escogido, indice_coste_minimo, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
					nombre_fichero, frase_fonetica,cadena_unidades, acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades,
					    nombre_fichero, cadena_unidades,
					    acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales,
						    nombre_fichero, acentuales_cand[indice_coste_minimo], frase_entrada,
						    coste_camino_escogido / numero_unidades, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;


    // for (contador = 0; contador < numero_rutas_optimas; contador++) {
    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[contador], numero_acentuales))
    // 	return 1;


    //   if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas + contador*numero_unidades_candidatas, numero_unidades, frecuencias + contador*numero_unidades, locutor, "wt", nombre_fichero, contador))
    // 	return 1;

    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (crea_fichero_grupos_acentuales(acentuales_cand[contador],
    // 					 numero_acentuales, "wt", nombre_fichero, contador))
    // 	return 1;

    // } // for (contador = 0; ...

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    //        sprintf(apunta_nombre_fichero, "_%d.costes", cuenta_frases);
    strcpy(apunta_nombre_fichero, ".costes");

    if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades,
					       costes_caminos_acentuales[indice_coste_minimo], costes_minimos[indice_coste_minimo], nombre_fichero))
      return 1;

        
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales,
					      acentuales_cand[indice_coste_minimo],
					      numero_acentuales, ficheros[7], ficheros[8],
					      ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(acentuales_cand[indice_coste_minimo],
						numero_acentuales,
						ficheros[10]))

      return 1;
    
    cuenta_frases++;

  } // else


  // Liberamos la memoria de las variables locales:
  /*

    Grupo_acentual_candidato *apunta_contorno_escogido = acentuales_cand;

    for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
  */
  //    free(acentuales_cand);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(cadena_posiciones);

  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de la anterior en que en este caso se realiza la selección acústica dividiendo la secuencia de fonemas en segmentos separados por silencios u oclusivas sordas.
 * \param	cadena_unidades: cadena con los vectores descriptores objetivo de las
 * unidades acústicas deseadas.
 * \param	numero_unidades: número de elementos de la cadena anterior.
 * \param	cadena_acentuales: cadena con los vectores que describen los grupos
 acentuales objetivo.
 * \param	acentuales_cand: cadena con los contornos de grupos acentuales candidatos
 * \param	costes_caminos_acentuales: cadena con los costes de cada camino acentual 
 * \param	numero_acentuales: número de elementos de la cadena anterior.
 * \param	escalado_frecuencia: valor por el que se desea escalar la frecuencia.
 * \param	numero_rutas_optimas: número de contornos que pasan a la búsqueda acústica.
 * \param	locutor: puntero al locutor empleado.
 * \param       tipo_candidatos: forma de seleccionar los candidatos para la búsqueda de Viterbi.
 * \param	opciones: opción de funcionamiento:
 0: únicamente se realiza la selección.
 1: se escribe información sobre los contornos escogidos.
 2: genera los ficheros del modo _CALCULA_FACTORES.
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida.
 * \param	frase_fonetica: cadena con la transcripción fonética de la frase.
 * \param	frase_entrada: frase de entrada.
 * \param	nombre_fichero_entrada: nombre del fichero de entrada.
 * \param	ficheros: cadena de ficheros del modo _CALCULA_FACTORES.
 * \param fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \retval	cadena_sintesis: cadena con los vectores seleccionados.
 * \retval	numero_vectores_sintesis: número de elementos de la cadena anterior.
 * \return	En ausencia de error, devuelve un cero.
 */
int Seleccion_unidades::seleccion_unidades_acusticas_segmentos(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {


  static int cuenta_frases = 1;
  float coste_camino_escogido = 0;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  int indice_primer_fonema_segmento;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_unidades_segmento = 1;
  int numero_total_segmentos = -1;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo;
  float *costes_minimos, coste_actual;
  short int *cadena_posiciones, *apunta_posiciones;

  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_segmento, puntero_objetivo, apunta_objetivo;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  //    numero_grupos_grupo_fonico_actual = 0;
  //    indice_primer_grupo_acentual_actual = 0;
  indice_primer_fonema_segmento = 0;
  //    indice_camino_sintesis = 0;
  izquierdo_derecho = IZQUIERDO;

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error an seleccion_unidades, al asignar memoria.\n");
    return 1;
  }

  // Contamos el número de segmentos, y salvamos las posiciones originales:

  apunta_objetivo = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;

  for (; apunta_objetivo != cadena_unidades.end(); apunta_objetivo++) {
    //    for (contador = 0; contador < numero_unidades; contador++) {
    *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
    if (is_segment_boundary(apunta_objetivo->semifonema))
      numero_total_segmentos++;
  }

  // Actualmente, el número de vectores de síntesis es siempre el doble que el de
  // unidades:

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  apunta_salida = *cadena_sintesis;

  // Inicializamos a cero la cadena de costes mínimos:

  for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas; cuenta_costes++)
    costes_minimos[cuenta_costes] = 0.0;

  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {

    // Escogemos el siguiente contorno posible:

    Grupo_acentual_candidato *apunta_grupo = acentuales_cand[contador_acentuales];

    // Comprobamos la continuidad entre contornos de grupos acentuales.

    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);

    // Creamos la cadena de valores de f0 objetivo:

    estructura_frecuencias *apunta_frecuencia = frecuencias + contador_acentuales*numero_unidades;

#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades, apunta_grupo, cadena_acentuales, escalado_frecuencia, &apunta_frecuencia))
      return 1;
#endif

    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, apunta_grupo, numero_acentuales))
      return 1;
	
				    
    // Ahora, hacemos la búsqueda acústica por segmentos

    indice_primer_fonema_segmento = 0;
    numero_unidades_segmento = 1;
    ultimo_grupo_prosodico = 1;
    indice_camino_sintesis = 0;
    izquierdo_derecho = IZQUIERDO;
    puntero_objetivo = cadena_unidades.begin();
    avanza_puntero_objetivo = 1;

#ifdef _DEPURA_VITERBI
    viterbi.avanza_contorno();
#endif

    inicio_objetivo_segmento = puntero_objetivo;

    for (int cuenta_segmentos = 0; cuenta_segmentos < numero_total_segmentos; cuenta_segmentos++) {

      if (cuenta_segmentos == numero_total_segmentos - 1)
	ultimo_grupo_prosodico = 0;

      indice_primer_fonema_segmento += numero_unidades_segmento - 1; // Acordarme de actualizarlo fuera del bucle.

      numero_unidades_segmento = 1;

      apunta_objetivo = inicio_objetivo_segmento;

      while ( (is_segment_boundary(apunta_objetivo->semifonema) != 1) ||
	      (apunta_objetivo == inicio_objetivo_segmento) ) {
	numero_unidades_segmento++;
	apunta_objetivo++;
      } // while ( (apunta_objetivo->semifonema[0] != '#') ...

#ifndef _MODELO_ENTONATIVO_ANTERIOR

      // Añadimos las frecuencias a la cadena de objetivos:

      frec = frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_segmento;

      for (cuenta = 0; cuenta < numero_unidades_segmento; cuenta++, frec++)
	inicio_objetivo_segmento[cuenta].anhade_frecuencia(frec->frecuencia_mitad, frec->frecuencia_inicial, frec->frecuencia_final);
#endif

      // Selección de unidades de voz:

      if (inicio_objetivo_segmento != cadena_unidades.begin())
	izquierdo_derecho = DERECHO;

      inicio_objetivo_segmento = apunta_objetivo;

      //            puntero_objetivo = inicio_objetivo_grupo_fonico;

      // Liberamos la memoria

      viterbi.libera_memoria_algoritmo();

      if (puntero_objetivo == cadena_unidades.begin())
	numero_semifonemas_real_grupo = 2*numero_unidades_segmento - ultimo_grupo_prosodico;
      else
	numero_semifonemas_real_grupo = 2*numero_unidades_segmento - 1 - ultimo_grupo_prosodico;

      if (numero_unidades_segmento == 1)
	numero_semifonemas_real_grupo++;

      if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho))
	return 1;
	    
      if (puntero_objetivo != cadena_unidades.begin())
	puntero_objetivo++;
	    
      for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
	      
	if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo,
						contador == numero_semifonemas_real_grupo - 1))
	  return 1;

	if (contador % 2 == avanza_puntero_objetivo)
	  puntero_objetivo++;

      } // for (int contador = 0

      // Ahora escogemos el mejor camino hasta ese punto:

      puntero_sintesis =
	unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;

      if (viterbi.devuelve_camino_optimo_sintesis(&numero_semifonemas_grupo_actual, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales))
	return 1;

      costes_minimos[contador_acentuales] +=  coste_actual;
      indice_camino_sintesis += numero_semifonemas_grupo_actual;
      avanza_puntero_objetivo = 0;

    } // for (int cuenta_segmentos = 0; ...


  } // for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas...

  // Buscamos el mejor camino:

  indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas);

  coste_camino_escogido = costes_minimos[indice_coste_minimo];
    
#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:

  frec = frecuencias + indice_coste_minimo*numero_unidades;
  inicio_objetivo_segmento = cadena_unidades.begin();
    
  for (; inicio_objetivo_segmento != puntero_objetivo;
       inicio_objetivo_segmento++, frec++)
    inicio_objetivo_segmento->anhade_frecuencia(frec->frecuencia_mitad,
						frec->frecuencia_inicial,
						frec->frecuencia_final);
#endif

  if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[indice_coste_minimo], numero_acentuales))
    return 1;

  // Escribimos las unidades escogidas en la cadena de salida:

  puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, puntero_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  for (cuenta = 0; cuenta < numero_unidades_candidatas; cuenta++)
    *apunta_salida++ = *puntero_sintesis++;

  viterbi.libera_memoria_algoritmo();

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero,
				      cadena_unidades, *cadena_sintesis,
				      locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_camino_escogido, indice_coste_minimo, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
					nombre_fichero, frase_fonetica,cadena_unidades, acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades,
					    nombre_fichero, cadena_unidades,
					    acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales,
						    nombre_fichero, acentuales_cand[indice_coste_minimo], frase_entrada,
						    coste_camino_escogido / numero_unidades, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;


    // for (contador = 0; contador < numero_rutas_optimas; contador++) {
    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[contador], numero_acentuales))
    // 	return 1;


    //   if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas + contador*numero_unidades_candidatas, numero_unidades, frecuencias + contador*numero_unidades, locutor, "wt", nombre_fichero, contador))
    // 	return 1;

    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (crea_fichero_grupos_acentuales(acentuales_cand[contador],
    // 					 numero_acentuales, "wt", nombre_fichero, contador))
    // 	return 1;

    // } // for (contador = 0; ...

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    //        sprintf(apunta_nombre_fichero, "_%d.costes", cuenta_frases);
    strcpy(apunta_nombre_fichero, ".costes");

    if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades,
					       costes_caminos_acentuales[indice_coste_minimo], costes_minimos[indice_coste_minimo], nombre_fichero))
      return 1;

        
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales,
					      acentuales_cand[indice_coste_minimo],
					      numero_acentuales, ficheros[7], ficheros[8],
					      ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(acentuales_cand[indice_coste_minimo],
						numero_acentuales,
						ficheros[10]))

      return 1;
    
    cuenta_frases++;

  } // else


  // Liberamos la memoria de las variables locales:
  /*

    Grupo_acentual_candidato *apunta_contorno_escogido = acentuales_cand;

    for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
  */
  //    free(acentuales_cand);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(cadena_posiciones);

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de seleccion_unidades_acusticas_segmentos en que en este caso se consideran los diferentes contornos por grupo fónico, y no globales para toda la frase
 * \param[in]	cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in]	numero_unidades número de elementos de la cadena anterior
 * \param[in]	cadena_acentuales: cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in]	vector_grupos vector de vectores de grupos acentuales candidato para cada grupo fónico
 * \param[in]	vector_costes vector de vectores de costes de cada camino acentual 
 * \param[in]   tamano_grupo_fonico vector con las longitudes, en grupos acentuales, de cada grupo fónico
 * \param[in]	numero_acentuales número de grupos acentuales de la frase
 * \param[in]	escalado_frecuencia valor por el que se desea escalar la frecuencia.
 * \param[in]   numero_rutas_optimas número máximo de contornos considerados por grupo fónico.
 * \param[in]	locutor puntero al locutor empleado.
 * \param[in]       tipo_candidatos forma de seleccionar los candidatos para la búsqueda de Viterbi.
 * \param[in]	opciones opción de funcionamiento:
 0: únicamente se realiza la selección
 1: se escribe información sobre los contornos escogidos
 2: genera los ficheros del modo _CALCULA_FACTORES
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida
 * \param[in]	frase_fonetica cadena con la transcripción fonética de la frase
 * \param[in]	frase_entrada frase de entrada
 * \param[in]	nombre_fichero_entrada  nombre del fichero de entrada
 * \param[in]	ficheros  cadena de ficheros del modo _CALCULA_FACTORES
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out]	cadena_sintesis  cadena con los vectores seleccionados
 * \param[out]	numero_vectores_sintesis  número de elementos de la cadena anterior
 * \return	En ausencia de error, devuelve un cero
 */
int Seleccion_unidades::seleccion_unidades_acusticas_segmentos_por_grupos_fonicos(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_caminos_acentuales, vector<int> &tamano_grupo_fonico, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {


  static int cuenta_frases = 1;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  int indice_primer_fonema_segmento;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_rutas_optimas_grupo_fonico;
  int numero_unidades_segmento = 1;
  int numero_total_segmentos;
  int numero_fonemas_grupo_fonico = 0;
  int numero_grupos_fonicos;
  int indice_primer_fonema_grupo_fonico = 0;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo = 0;
  float *costes_minimos, coste_actual, coste_total = 0;
  short int *cadena_posiciones, *apunta_posiciones;

  Grupo_acentual_candidato *contorno_escogido, *apunta_contorno_escogido, *auxiliar_grupo;
  Grupo_acentual_objetivo *apunta_grupo_objetivo = cadena_acentuales;

  vector<int> caminos_escogidos;

  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_segmento, puntero_objetivo, apunta_objetivo, inicio_objetivo_grupo_fonico;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;

  caminos_escogidos.clear();

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  numero_unidades_candidatas = 2*numero_unidades;
  numero_grupos_fonicos = vector_grupos.size();

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  // Actualmente, el número de vectores de síntesis es siempre el doble que el de unidades:

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...


  if ( (contorno_escogido = (Grupo_acentual_candidato *) malloc(numero_acentuales*sizeof(Grupo_acentual_candidato))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  }

  apunta_contorno_escogido = contorno_escogido;

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*numero_rutas_optimas*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  apunta_salida = *cadena_sintesis;

  indice_primer_fonema_segmento = 0;
  izquierdo_derecho = IZQUIERDO;

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos, al asignar memoria.\n");
    return 1;
  }

  inicio_objetivo_grupo_fonico = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;


  for (int cuenta_grupo_fonico = 0; cuenta_grupo_fonico < numero_grupos_fonicos; cuenta_grupo_fonico++) {

    apunta_objetivo = inicio_objetivo_grupo_fonico;

    // Contamos el número de segmentos, y salvamos las posiciones originales:
    
    numero_total_segmentos = 0;
    numero_fonemas_grupo_fonico = 1;

    for (; apunta_objetivo != cadena_unidades.end();) {
      //    for (contador = 0; contador < numero_unidades; contador++) {
      *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
      numero_fonemas_grupo_fonico++;
      if (is_segment_boundary(apunta_objetivo->semifonema))
	numero_total_segmentos++;
      ++apunta_objetivo;
      if (apunta_objetivo->semifonema[0] == '#') {
	*apunta_posiciones = apunta_objetivo->frontera_prosodica;
	break;
      }
    }

    // Inicializamos a cero la cadena de costes mínimos:
    
    for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas; cuenta_costes++)
      costes_minimos[cuenta_costes] = 0.0;
    
    numero_rutas_optimas_grupo_fonico = vector_grupos[cuenta_grupo_fonico].size();
  
    for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas_grupo_fonico; contador_acentuales++) {

      // Escogemos el siguiente contorno posible:

      Grupo_acentual_candidato *apunta_grupo = vector_grupos[cuenta_grupo_fonico][contador_acentuales];

      // Comprobamos la continuidad entre contornos de grupos acentuales.

      comprueba_y_asegura_continuidad_contornos(apunta_grupo, tamano_grupo_fonico[cuenta_grupo_fonico]);

      // Creamos la cadena de valores de f0 objetivo:
    
      estructura_frecuencias *apunta_frecuencia = frecuencias + contador_acentuales*numero_unidades;
    
#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
      if (crea_vector_frecuencias(cadena_unidades, numero_fonemas_grupo_fonico, apunta_grupo, apunta_grupo_objetivo, escalado_frecuencia, &apunta_frecuencia, indice_primer_fonema_grupo_fonico))
	return 1;
#else
      if (crea_vector_frecuencias_V2(cadena_unidades, numero_fonemas_grupo_fonico, apunta_grupo, apunta_grupo_objetivo, escalado_frecuencia, &apunta_frecuencia, indice_primer_fonema_grupo_fonico))
	return 1;
#endif

      if (incluye_limites_grupo_entonativo(cadena_unidades, numero_fonemas_grupo_fonico, cadena_posiciones, apunta_grupo, tamano_grupo_fonico[cuenta_grupo_fonico], 0, indice_primer_fonema_grupo_fonico))
	return 1;
    
				    
      // Ahora, hacemos la búsqueda acústica por segmentos
    
      indice_primer_fonema_segmento = 0;
      numero_unidades_segmento = 1;
      ultimo_grupo_prosodico = 1;
      indice_camino_sintesis = 0;
      izquierdo_derecho = IZQUIERDO;
      //      puntero_objetivo = cadena_unidades.begin();
      if (cuenta_grupo_fonico == 0)
	avanza_puntero_objetivo = 1;
    
#ifdef _DEPURA_VITERBI
      viterbi.avanza_contorno();
#endif
    
      inicio_objetivo_segmento = inicio_objetivo_grupo_fonico;

      apunta_objetivo = inicio_objetivo_segmento;

      for (int cuenta_segmentos = 0; cuenta_segmentos < numero_total_segmentos; cuenta_segmentos++) {
      
	if ((cuenta_segmentos == numero_total_segmentos - 1) && (cuenta_grupo_fonico == numero_grupos_fonicos - 1) )
	  ultimo_grupo_prosodico = 0;
      
	indice_primer_fonema_segmento += numero_unidades_segmento - 1; // Acordarme de actualizarlo fuera del bucle.
      
	numero_unidades_segmento = 1;
      
	while ( (is_segment_boundary(apunta_objetivo->semifonema) != 1) ||
		(apunta_objetivo == inicio_objetivo_segmento) ) {
	  numero_unidades_segmento++;
	  apunta_objetivo++;
	} // while ( (apunta_objetivo->semifonema[0] != '#') ...
      
#ifndef _MODELO_ENTONATIVO_ANTERIOR
      
	// Añadimos las frecuencias a la cadena de objetivos:
      
	frec = frecuencias + contador_acentuales*numero_unidades + indice_primer_fonema_segmento;
      
	for (cuenta = 0; cuenta < numero_unidades_segmento; cuenta++, frec++)
	  inicio_objetivo_segmento[cuenta].anhade_frecuencia(frec->frecuencia_mitad, frec->frecuencia_inicial, frec->frecuencia_final);
#endif
      
	// Selección de unidades de voz:
      
	if (inicio_objetivo_segmento != cadena_unidades.begin())
	  izquierdo_derecho = DERECHO;
      
	puntero_objetivo = inicio_objetivo_segmento;
      
	// Liberamos la memoria
      
	viterbi.libera_memoria_algoritmo();
      
	if (puntero_objetivo == cadena_unidades.begin())
	  numero_semifonemas_real_grupo = 2*numero_unidades_segmento - ultimo_grupo_prosodico;
	else
	  numero_semifonemas_real_grupo = 2*numero_unidades_segmento - 1 - ultimo_grupo_prosodico;
      
	if (numero_unidades_segmento == 1)
	  numero_semifonemas_real_grupo++;
      
	if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho))
	  return 1;
      
	if ( (cuenta_grupo_fonico != 0) || (cuenta_segmentos != 0) )
	  puntero_objetivo++;
      
	for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
	
	  if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo,
						  contador == numero_semifonemas_real_grupo - 1))
	    return 1;
	
	  if (contador % 2 == avanza_puntero_objetivo)
	    puntero_objetivo++;
	
	} // for (int contador = 0
      
	// Ahora escogemos el mejor camino hasta ese punto:
      
	puntero_sintesis =
	  unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;
      
	if (viterbi.devuelve_camino_optimo_sintesis(&numero_semifonemas_grupo_actual, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales))
	  return 1;

	costes_minimos[contador_acentuales] +=  coste_actual;
	indice_camino_sintesis += numero_semifonemas_grupo_actual;
	avanza_puntero_objetivo = 0;
	inicio_objetivo_segmento = puntero_objetivo;

      } // for (int cuenta_segmentos = 0; ...
    
    } // for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas...

    // Buscamos el mejor camino:
    
    indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas_grupo_fonico);
    
    caminos_escogidos.push_back(indice_coste_minimo);

    coste_total += costes_minimos[indice_coste_minimo];
    auxiliar_grupo = vector_grupos[cuenta_grupo_fonico][indice_coste_minimo];

    for (int cuenta_grupos = 0; cuenta_grupos < tamano_grupo_fonico[cuenta_grupo_fonico]; cuenta_grupos++, apunta_contorno_escogido++)
      apunta_contorno_escogido->copia_grupo_acentual(auxiliar_grupo++);

#ifndef _MODELO_ENTONATIVO_ANTERIOR
    
    // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:
    
    frec = frecuencias + indice_coste_minimo*numero_unidades;
    
    for (; inicio_objetivo_grupo_fonico != puntero_objetivo;
	 inicio_objetivo_grupo_fonico++, frec++)
      inicio_objetivo_grupo_fonico->anhade_frecuencia(frec->frecuencia_mitad,
						  frec->frecuencia_inicial,
						  frec->frecuencia_final);
#endif
    
    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_fonemas_grupo_fonico, cadena_posiciones, vector_grupos[cuenta_grupo_fonico][indice_coste_minimo], tamano_grupo_fonico[cuenta_grupo_fonico], 0, indice_primer_fonema_grupo_fonico))
      return 1;
    
    // Escribimos las unidades escogidas en la cadena de salida:

    puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;
    
    for (cuenta = 0; cuenta < indice_camino_sintesis; cuenta++)
      *apunta_salida++ = *puntero_sintesis++;

    apunta_grupo_objetivo += tamano_grupo_fonico[cuenta_grupo_fonico];

    indice_primer_fonema_grupo_fonico += numero_fonemas_grupo_fonico - 1;

  } // for (int cuenta_grupo_fonico = 0; ...

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  viterbi.libera_memoria_algoritmo();

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_total, caminos_escogidos, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades, nombre_fichero, frase_fonetica,cadena_unidades, contorno_escogido, cadena_acentuales, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades, nombre_fichero, cadena_unidades, contorno_escogido, cadena_acentuales, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales, nombre_fichero, contorno_escogido, frase_entrada, coste_total / numero_unidades, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);


    // strcpy(apunta_nombre_fichero, ".costes");

    // if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades, costes_caminos_acentuales[indice_coste_minimo], coste_total, nombre_fichero))
    //   return 1;

        
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales, contorno_escogido, numero_acentuales, ficheros[7], ficheros[8], ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(contorno_escogido, numero_acentuales, ficheros[10]))

      return 1;
    
    cuenta_frases++;

  } // else


  // Liberamos la memoria de las variables locales:
  

  apunta_contorno_escogido = contorno_escogido;

  for (contador = 0; contador < numero_acentuales; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
    
  free(contorno_escogido);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(cadena_posiciones);

  return 0;

}



/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de la anterior en que en este caso se calculan los caminos relacionados con cada contorno de una sola pasada, en lugar de hacerlo de forma secuencial
 * \param	cadena_unidades: cadena con los vectores descriptores objetivo de las unidades acústicas deseadas.
 * \param	numero_unidades: número de elementos de la cadena anterior.
 * \param	cadena_acentuales: cadena con los vectores que describen los grupos
 acentuales objetivo.
 * \param	acentuales_cand: cadena con los contornos de grupos acentuales candidatos
 * \param	costes_caminos_acentuales: cadena con los costes de cada camino acentual 
 * \param	numero_acentuales: número de elementos de la cadena anterior.
 * \param	escalado_frecuencia: valor por el que se desea escalar la frecuencia.
 * \param	numero_rutas_optimas: número de contornos que pasan a la búsqueda acústica.
 * \param	locutor: puntero al locutor empleado.
 * \param       tipo_candidatos: forma de seleccionar las unidades candidatas para la búsqueda de Viterbi.
 * \param	opciones: opción de funcionamiento:
 0: únicamente se realiza la selección.
 1: se escribe información sobre los contornos escogidos.
 2: genera los ficheros del modo _CALCULA_FACTORES.
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida.
 * \param	frase_fonetica: cadena con la transcripción fonética de la frase.
 * \param	frase_entrada: frase de entrada.
 * \param	nombre_fichero_entrada: nombre del fichero de entrada.
 * \param	ficheros: cadena de ficheros del modo _CALCULA_FACTORES.
 * \param fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \retval	cadena_sintesis: cadena con los vectores seleccionados.
 * \retval	numero_vectores_sintesis: número de elementos de la cadena anterior.
 * \return	En ausencia de error, devuelve un cero.
 */
int Seleccion_unidades::seleccion_unidades_acusticas_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras,  Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {

  static int cuenta_frases = 1;
  float coste_camino_escogido = 0;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int indice_camino_sintesis;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_total_grupos_prosodicos = -1;
  unsigned char izquierdo_derecho = IZQUIERDO;
  float *costes_minimos;
  float coste_actual;
  short int *cadena_posiciones, *apunta_posiciones;
  char *mascara_contornos = NULL;

  vector<Vector_descriptor_objetivo>::iterator puntero_objetivo;
  //  Vector_semifonema_candidato *cadena_candidatos = NULL;
  estructura_frecuencias *frecuencias = NULL;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

#ifdef _DEPURA_VITERBI
  viterbi.avanza_contorno();
#endif

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error an seleccion_unidades, al asignar memoria.\n");
    return 1;
  }

  // Salvamos las posiciones originales:

  puntero_objetivo = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;

  for (; puntero_objetivo != cadena_unidades.end(); puntero_objetivo++) {
    //      for (contador = 0; contador < numero_unidades; contador++) {
    *apunta_posiciones++ = puntero_objetivo->frontera_prosodica;
  }

  // Actualmente, el número de vectores de síntesis es siempre el doble que el de unidades:

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_escogidas = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  apunta_salida = *cadena_sintesis;

  if ( (mascara_contornos = (char *) malloc(numero_rutas_optimas*sizeof(char))) == NULL) {
    fprintf(stderr, "Error an selecciona_contornos_acusticos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (mascara_contornos...

  // Añadimos a los vectores objetivo la cadena de f0 y fronteras posibles según los contornos escogidos anteriormente:

  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {

    mascara_contornos[contador_acentuales] = 1;

    // Escogemos el siguiente contorno posible:

    Grupo_acentual_candidato *apunta_grupo = acentuales_cand[contador_acentuales];

    // Comprobamos la continuidad entre contornos de grupos acentuales.

    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);

    // Creamos la cadena de valores de f0 objetivo:

#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades,
				apunta_grupo,
				cadena_acentuales,
				escalado_frecuencia,
				&frecuencias))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades,
				   apunta_grupo,
				   cadena_acentuales,
				   escalado_frecuencia,
				   &frecuencias))
      return 1;
#endif

    // Añadimos dichas frecuencias a la cadena de vectores objetivo:
		
    if (anhade_frecuencias_objetivo(cadena_unidades, numero_unidades, frecuencias))
      return 1;
      
    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, apunta_grupo, numero_acentuales, 1))
      return 1;
      
  } // for (contador_acentuales = 0;...
    
  izquierdo_derecho = IZQUIERDO;
  puntero_objetivo = cadena_unidades.begin();

  // Ahora hacemos la búsqueda para toda la frase:

  izquierdo_derecho = IZQUIERDO;
  
  viterbi.libera_memoria_algoritmo();

  if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_unidades_candidatas, izquierdo_derecho, mascara_contornos, numero_rutas_optimas))
    return 1;
      
  for (contador = 1; contador < numero_unidades_candidatas; contador++) {
    //    fprintf(stderr, "Recursión = %d (%s).\n", contador, puntero_objetivo->semifonema);
    if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo, (contador == numero_unidades_candidatas - 1), mascara_contornos, numero_rutas_optimas))
      return 1;
    
    if (contador % 2 == 1)
      puntero_objetivo++;
    
  } // for (int contador = 0
  
    // Ahora escogemos el mejor camino global:

  for (contador = 0; contador < numero_rutas_optimas; contador++) {
    if (mascara_contornos[contador] != 0) {
      puntero_sintesis = unidades_escogidas + contador*numero_unidades_candidatas;
      if (viterbi.devuelve_camino_optimo_sintesis_paralelo(&numero_unidades_candidatas, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales, contador, mascara_contornos))
	return 1;
      costes_minimos[contador] = coste_actual;
    }
    else
      costes_minimos[contador] = FLT_MAX;
  }

  indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas, mascara_contornos);
      
  puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;

#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:

  puntero_objetivo = cadena_unidades.begin();
    
  for (; puntero_objetivo != cadena_unidades.end(); puntero_objetivo++) {
    if (sordo_sonoro(puntero_objetivo->semifonema) == SONORO)
      puntero_objetivo->anhade_frecuencia(puntero_objetivo->frecuencias_mitad[indice_coste_minimo], puntero_objetivo->frecuencias_inicial[indice_coste_minimo], puntero_objetivo->frecuencias_final[indice_coste_minimo]);
    else
      puntero_objetivo->anhade_frecuencia(puntero_objetivo->frecuencias_mitad[0], puntero_objetivo->frecuencias_inicial[0], puntero_objetivo->frecuencias_final[0]);      
  }
#endif

  if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[indice_coste_minimo], numero_acentuales))
    return 1;

  // Poner aquí lo de la validez de las unidades escogidas según ese contorno :)

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, puntero_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  // Escribimos las unidades escogidas en la cadena de salida:

  for (contador = 0; contador < numero_unidades_candidatas; contador++)
    *apunta_salida++ = *puntero_sintesis++;



  viterbi.libera_memoria_algoritmo();

  if (mascara_contornos != NULL)
    free(mascara_contornos);

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_actual, indice_coste_minimo, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    // sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    // if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
    // 					 nombre_fichero, cadena_unidades,
    // 					 *cadena_sintesis, locutor, opcion,
    // 					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
    //   return 1;

    sprintf(apunta_nombre_fichero, "_%d.contornos", cuenta_frases);

    if (escribe_contornos_fichero(cadena_unidades, nombre_fichero, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
					nombre_fichero, frase_fonetica,
					cadena_unidades,
					acentuales_cand[indice_coste_minimo], 
					cadena_acentuales, 
					opcion))
      return 1;


    // sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    // if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades,
    // 					    nombre_fichero, cadena_unidades,
    // 					    acentuales_cand[indice_coste_minimo], opcion))
    //   return 1;

    // sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    // if (escribe_contorno_escogido_calculo_distancia(numero_acentuales,
    // 						    nombre_fichero, acentuales_cand[indice_coste_minimo], frase_entrada,
    // 						    coste_camino_escogido / numero_unidades, opcion))
    //   return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, mascara_contornos, indice_coste_minimo, opcion))
      return 1;

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    //        sprintf(apunta_nombre_fichero, "_%d.costes", cuenta_frases);
    strcpy(apunta_nombre_fichero, ".costes");

    if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades, costes_caminos_acentuales[indice_coste_minimo], coste_actual, nombre_fichero))
      return 1;
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales,
					      acentuales_cand[indice_coste_minimo],
					      numero_acentuales, ficheros[7], ficheros[8],
					      ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(acentuales_cand[indice_coste_minimo],
						numero_acentuales,
						ficheros[10]))

      return 1;

    cuenta_frases++;

  } // else

  free(unidades_escogidas);
  free(costes_minimos);
  free(frecuencias);
  free(cadena_posiciones);

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de la anterior en que en este caso se realiza la selección acústica dividiendo la secuencia de fonemas en segmentos separados por silencios u oclusivas sordas.
 * \param	cadena_unidades: cadena con los vectores descriptores objetivo de las
 * unidades acústicas deseadas.
 * \param	numero_unidades: número de elementos de la cadena anterior.
 * \param	cadena_acentuales: cadena con los vectores que describen los grupos
 acentuales objetivo.
 * \param	acentuales_cand: cadena con los contornos de grupos acentuales candidatos
 * \param	costes_caminos_acentuales: cadena con los costes de cada camino acentual 
 * \param	numero_acentuales: número de elementos de la cadena anterior.
 * \param	escalado_frecuencia: valor por el que se desea escalar la frecuencia.
 * \param	numero_rutas_optimas: número de contornos que pasan a la búsqueda acústica.
 * \param	locutor: puntero al locutor empleado.
 * \param       tipo_candidatos: forma de seleccionar los candidatos para la búsqueda de Viterbi.
 * \param	opciones: opción de funcionamiento:
 0: únicamente se realiza la selección.
 1: se escribe información sobre los contornos escogidos.
 2: genera los ficheros del modo _CALCULA_FACTORES.
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida.
 * \param	frase_fonetica: cadena con la transcripción fonética de la frase.
 * \param	frase_entrada: frase de entrada.
 * \param	nombre_fichero_entrada: nombre del fichero de entrada.
 * \param	ficheros: cadena de ficheros del modo _CALCULA_FACTORES.
 * \param fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \retval	cadena_sintesis: cadena con los vectores seleccionados.
 * \retval	numero_vectores_sintesis: número de elementos de la cadena anterior.
 * \return	En ausencia de error, devuelve un cero.
 */
int Seleccion_unidades::seleccion_unidades_acusticas_segmentos_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {


  static int cuenta_frases = 1;
  float coste_camino_escogido = 0;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  int indice_primer_fonema_segmento;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_unidades_segmento = 1;
  int numero_total_segmentos = -1;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo;
  float *costes_minimos, coste_actual;
  float *costes_acumulados;
  short int *cadena_posiciones, *apunta_posiciones;
  char *mascara_contornos = NULL;

  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_segmento, puntero_objetivo, apunta_objetivo;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  //    numero_grupos_grupo_fonico_actual = 0;
  //    indice_primer_grupo_acentual_actual = 0;
  indice_primer_fonema_segmento = 0;
  //    indice_camino_sintesis = 0;
  izquierdo_derecho = IZQUIERDO;

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error an seleccion_unidades, al asignar memoria.\n");
    return 1;
  }

  // Contamos el número de segmentos, y salvamos las posiciones originales:

  apunta_objetivo = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;

  for (; apunta_objetivo != cadena_unidades.end(); apunta_objetivo++) {
    //    for (contador = 0; contador < numero_unidades; contador++) {
    *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
    if (is_segment_boundary(apunta_objetivo->semifonema))
      numero_total_segmentos++;
  }

  // Actualmente, el número de vectores de síntesis es siempre el doble que el de
  // unidades:

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  if ( (costes_acumulados = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en selección_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (costes_acumulados = 

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_escogidas = ...

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  apunta_salida = *cadena_sintesis;

  if ( (mascara_contornos = (char *) malloc(numero_rutas_optimas*sizeof(char))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_paralelo, al asignar memoria.\n");
    return 1;
  }

  // Añadimos a los vectores objetivo la cadena de f0 y fronteras posibles según los contornos escogidos anteriormente:

  for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {

    mascara_contornos[contador_acentuales] = 1;

    // Inicializamos a cero la cadena de costes mínimos:
    costes_minimos[contador_acentuales] = 0.0;

    // Escogemos el siguiente contorno posible:

    Grupo_acentual_candidato *apunta_grupo = acentuales_cand[contador_acentuales];

    // Comprobamos la continuidad entre contornos de grupos acentuales.

    comprueba_y_asegura_continuidad_contornos(apunta_grupo, numero_acentuales);

    // Creamos la cadena de valores de f0 objetivo:

#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
    if (crea_vector_frecuencias(cadena_unidades, numero_unidades,
				apunta_grupo,
				cadena_acentuales,
				escalado_frecuencia,
				&frecuencias))
      return 1;
#else
    if (crea_vector_frecuencias_V2(cadena_unidades, numero_unidades,
				   apunta_grupo,
				   cadena_acentuales,
				   escalado_frecuencia,
				   &frecuencias))
      return 1;
#endif

    // Añadimos dichas frecuencias a la cadena de vectores objetivo:
		
    if (anhade_frecuencias_objetivo(cadena_unidades, numero_unidades, frecuencias))
      return 1;
      
    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, apunta_grupo, numero_acentuales, 1))
      return 1;
      
    costes_acumulados[contador_acentuales] = 0;

  } // for (contador_acentuales = 0;...
    
  // Ahora, hacemos la búsqueda acústica por segmentos
  
  indice_primer_fonema_segmento = 0;
  numero_unidades_segmento = 1;
  ultimo_grupo_prosodico = 1;
  indice_camino_sintesis = 0;
  izquierdo_derecho = IZQUIERDO;
  puntero_objetivo = cadena_unidades.begin();
  avanza_puntero_objetivo = 1;

#ifdef _DEPURA_VITERBI
  viterbi.avanza_contorno();
#endif

  inicio_objetivo_segmento = puntero_objetivo;

  for (int cuenta_segmentos = 0; cuenta_segmentos < numero_total_segmentos; cuenta_segmentos++) {
    
    if (cuenta_segmentos == numero_total_segmentos - 1)
      ultimo_grupo_prosodico = 0;
    
    indice_primer_fonema_segmento += numero_unidades_segmento - 1; // Acordarme de actualizarlo fuera del bucle.
    
    numero_unidades_segmento = 1;
    
    apunta_objetivo = inicio_objetivo_segmento;

    while ( (is_segment_boundary(apunta_objetivo->semifonema) != 1) ||
	    (apunta_objetivo == inicio_objetivo_segmento) ) {
      numero_unidades_segmento++;
      apunta_objetivo++;
    } // while ( (apunta_objetivo->semifonema[0] != '#') ...
    
    
    if (inicio_objetivo_segmento != cadena_unidades.begin())
      izquierdo_derecho = DERECHO;

    inicio_objetivo_segmento = apunta_objetivo;

    // Selección de unidades de voz:
    
    //            puntero_objetivo = inicio_objetivo_grupo_fonico;
    
    // Liberamos la memoria
    
    viterbi.libera_memoria_algoritmo();
    
    if (puntero_objetivo == cadena_unidades.begin())
      numero_semifonemas_real_grupo = 2*numero_unidades_segmento - ultimo_grupo_prosodico;
    else
      numero_semifonemas_real_grupo = 2*numero_unidades_segmento - 1 - ultimo_grupo_prosodico;

    if (numero_unidades_segmento == 1)
      numero_semifonemas_real_grupo++;
    
    if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho, mascara_contornos, numero_rutas_optimas))
      return 1;
    
    if (puntero_objetivo != cadena_unidades.begin())
      puntero_objetivo++;
    
    for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
      
      if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo,
					      (contador == numero_semifonemas_real_grupo - 1), mascara_contornos, numero_rutas_optimas, costes_acumulados))
	return 1;
      
      if (contador % 2 == avanza_puntero_objetivo)
	puntero_objetivo++;
      
    } // for (int contador = 0
    
      // Ahora escogemos el mejor camino hasta ese punto, para cada contorno
    
    for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas; contador_acentuales++) {

      if (mascara_contornos[contador_acentuales] == 0)
	continue;

      puntero_sintesis =
	unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;
      
      if (viterbi.devuelve_camino_optimo_sintesis_paralelo(&numero_semifonemas_grupo_actual, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales, contador_acentuales, mascara_contornos))
	return 1;

      if (mascara_contornos[contador_acentuales] == 0)
	continue;

      costes_minimos[contador_acentuales] +=  coste_actual;
      costes_acumulados[contador_acentuales] += coste_actual;

    } // for (contador_acentuales = 0; contador_acentuales < ...

    if (cuenta_segmentos == numero_total_segmentos - 1)
      puntero_objetivo--;

    if (viterbi.poda_contornos_paralelo(mascara_contornos, costes_acumulados, numero_rutas_optimas, *puntero_objetivo))
      return 1;

    indice_camino_sintesis += numero_semifonemas_grupo_actual;
    avanza_puntero_objetivo = 0;
    
  } // for (int cuenta_segmentos = 0; ...
  
  // Buscamos el mejor camino:

  indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas, mascara_contornos);

  coste_camino_escogido = costes_minimos[indice_coste_minimo];
    
#ifndef _MODELO_ENTONATIVO_ANTERIOR

  // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:

  inicio_objetivo_segmento = cadena_unidades.begin();
    
  for (; inicio_objetivo_segmento != cadena_unidades.end();
       inicio_objetivo_segmento++)
    if (sordo_sonoro(inicio_objetivo_segmento->semifonema) == SONORO) 
      inicio_objetivo_segmento->anhade_frecuencia(inicio_objetivo_segmento->frecuencias_mitad[indice_coste_minimo], inicio_objetivo_segmento->frecuencias_inicial[indice_coste_minimo], inicio_objetivo_segmento->frecuencias_final[indice_coste_minimo]);
    else 
      inicio_objetivo_segmento->anhade_frecuencia(inicio_objetivo_segmento->frecuencias_mitad[0], inicio_objetivo_segmento->frecuencias_inicial[0], inicio_objetivo_segmento->frecuencias_final[0]);
#endif

  if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[indice_coste_minimo], numero_acentuales))
    return 1;

  // Escribimos las unidades escogidas en la cadena de salida:

  puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;

  // Poner aquí lo de la validez de las unidades escogidas según ese contorno :)
  
  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, puntero_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  for (cuenta = 0; cuenta < numero_unidades_candidatas; cuenta++)
    *apunta_salida++ = *puntero_sintesis++;

  viterbi.libera_memoria_algoritmo();

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_camino_escogido, indice_coste_minimo, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades,
					nombre_fichero, frase_fonetica,cadena_unidades, acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;


    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades,
					    nombre_fichero, cadena_unidades,
					    acentuales_cand[indice_coste_minimo], cadena_acentuales, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales, nombre_fichero, acentuales_cand[indice_coste_minimo], frase_entrada, coste_camino_escogido / numero_unidades, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;


    // for (contador = 0; contador < numero_rutas_optimas; contador++) {
    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (incluye_limites_grupo_entonativo(cadena_unidades, numero_unidades, cadena_posiciones, acentuales_cand[contador], numero_acentuales))
    // 	return 1;


    //   if (crea_fichero_contorno_frecuencia(cadena_unidades, unidades_escogidas + contador*numero_unidades_candidatas, numero_unidades, frecuencias + contador*numero_unidades, locutor, "wt", nombre_fichero, contador))
    // 	return 1;

    //   sprintf(apunta_nombre_fichero, "_%d_", cuenta_frases);

    //   if (crea_fichero_grupos_acentuales(acentuales_cand[contador],
    // 					 numero_acentuales, "wt", nombre_fichero, contador))
    // 	return 1;

    // } // for (contador = 0; ...

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    //        sprintf(apunta_nombre_fichero, "_%d.costes", cuenta_frases);
    strcpy(apunta_nombre_fichero, ".costes");

    if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades,
					       costes_caminos_acentuales[indice_coste_minimo], costes_minimos[indice_coste_minimo], nombre_fichero))
      return 1;

        
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales,
					      acentuales_cand[indice_coste_minimo],
					      numero_acentuales, ficheros[7], ficheros[8],
					      ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(acentuales_cand[indice_coste_minimo],
						numero_acentuales,
						ficheros[10]))

      return 1;
    
    cuenta_frases++;

  } // else


  // Liberamos la memoria de las variables locales:
  /*

    Grupo_acentual_candidato *apunta_contorno_escogido = acentuales_cand;

    for (contador = 0; contador < numero_acentuales*numero_rutas_optimas; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
  */
  //    free(acentuales_cand);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(costes_acumulados);
  free(cadena_posiciones);
  free(mascara_contornos);

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función que realiza la selección de unidades acústicas. También incluye la selección de contornos entonativos basada en Corpus. Se diferencia de seleccion_unidades_acusticas_segmentos_por_grupos_fonicos en que en este caso se consideran los diferentes contornos por grupo fónico, y no globales para toda la frase, y el cómputo se realiza para todos los contornos en paralelo
 * \param[in]	cadena_unidades cadena con los vectores descriptores objetivo de las unidades acústicas deseadas
 * \param[in]	numero_unidades número de elementos de la cadena anterior
 * \param[in]	cadena_acentuales: cadena con los vectores que describen los grupos acentuales objetivo
 * \param[in]	vector_grupos vector de vectores de grupos acentuales candidato para cada grupo fónico
 * \param[in]	vector_costes vector de vectores de costes de cada camino acentual 
 * \param[in]   tamano_grupo_fonico vector con las longitudes, en grupos acentuales, de cada grupo fónico
 * \param[in]	numero_acentuales número de grupos acentuales de la frase
 * \param[in]	escalado_frecuencia valor por el que se desea escalar la frecuencia.
 * \param[in]   numero_rutas_optimas número máximo de contornos considerados por grupo fónico.
 * \param[in]	locutor puntero al locutor empleado.
 * \param[in]       tipo_candidatos forma de seleccionar los candidatos para la búsqueda de Viterbi.
 * \param[in]	opciones opción de funcionamiento:
 0: únicamente se realiza la selección
 1: se escribe información sobre los contornos escogidos
 2: genera los ficheros del modo _CALCULA_FACTORES
 3: genera el fichero con el número de unidades consecutivas y el coste de la secuencia escogida
 * \param[in]	frase_fonetica cadena con la transcripción fonética de la frase
 * \param[in]	frase_entrada frase de entrada
 * \param[in]	nombre_fichero_entrada  nombre del fichero de entrada
 * \param[in]	ficheros  cadena de ficheros del modo _CALCULA_FACTORES
 * \param[in] fuerza_fronteras Indica si las fronteras entonativas deben ir fijadas por la frase de entrada o no
 * \param[out]	cadena_sintesis  cadena con los vectores seleccionados
 * \param[out]	numero_vectores_sintesis  número de elementos de la cadena anterior
 * \return	En ausencia de error, devuelve un cero
 */
int Seleccion_unidades::seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_caminos_acentuales, vector<int> &tamano_grupo_fonico, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis) {


  static int cuenta_frases = 1;
  int cuenta, contador;
  unsigned char contador_acentuales;
  int indice_coste_minimo;
  int numero_semifonemas_grupo_actual;
  int indice_primer_fonema_segmento;
  int indice_camino_sintesis;
  int numero_semifonemas_real_grupo;
  int ultimo_grupo_prosodico;
  int numero_unidades_candidatas;
  int numero_rutas_optimas_reales;
  int numero_rutas_optimas_grupo_fonico;
  int numero_unidades_segmento = 1;
  int numero_total_segmentos;
  int numero_fonemas_grupo_fonico = 0;
  int numero_grupos_fonicos;
  int indice_primer_fonema_grupo_fonico = 0;
  unsigned char izquierdo_derecho;
  char avanza_puntero_objetivo = 0;
  float *costes_minimos, coste_actual, coste_total = 0;
  float *costes_acumulados;
  short int *cadena_posiciones, *apunta_posiciones;
  char *mascara_contornos = NULL;

  Grupo_acentual_candidato *contorno_escogido, *apunta_contorno_escogido, *auxiliar_grupo;
  Grupo_acentual_objetivo *apunta_grupo_objetivo = cadena_acentuales;

  vector<int> caminos_escogidos;

  vector<Vector_descriptor_objetivo>::iterator inicio_objetivo_segmento, puntero_objetivo, apunta_objetivo, inicio_objetivo_grupo_fonico;
  Vector_sintesis *unidades_escogidas, *puntero_sintesis, *apunta_salida;
  estructura_frecuencias *frecuencias;

  caminos_escogidos.clear();

#ifndef _MODELO_ENTONATIVO_ANTERIOR
  estructura_frecuencias *frec;
#endif

  if ( (mascara_contornos = (char *) malloc(numero_rutas_optimas*sizeof(char))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  }

  numero_unidades_candidatas = 2*numero_unidades;
  numero_grupos_fonicos = vector_grupos.size();

  viterbi.inicializa(locutor, tipo_candidatos);
  viterbi.modifica_fuerza_fronteras_entonativas(fuerza_fronteras);

  // Actualmente, el número de vectores de síntesis es siempre el doble que el de unidades:

  *numero_vectores_sintesis = numero_unidades_candidatas;

  if ( (costes_minimos = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (costes_minimos = ...

  if ( (unidades_escogidas = (Vector_sintesis *) malloc(numero_unidades_candidatas*numero_rutas_optimas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (unidades_candidatas = ...


  if ( (contorno_escogido = (Grupo_acentual_candidato *) malloc(numero_acentuales*sizeof(Grupo_acentual_candidato))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  }

  apunta_contorno_escogido = contorno_escogido;

  if ( (frecuencias = (estructura_frecuencias *) malloc(numero_unidades*sizeof(estructura_frecuencias)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencias = ...

  // Reservamos memoria para la cadena de salida:

  if ( (*cadena_sintesis = (Vector_sintesis *)
	malloc(numero_unidades_candidatas*sizeof(Vector_sintesis)))
       == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (*cadena_sintesis = ...

  if ( (costes_acumulados = (float *) malloc(numero_rutas_optimas*sizeof(float))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  } // if ( (costes_acumulados = ...
  
  apunta_salida = *cadena_sintesis;

  indice_primer_fonema_segmento = 0;
  izquierdo_derecho = IZQUIERDO;

  ultimo_grupo_prosodico = 1;
  numero_unidades_candidatas = 2*numero_unidades;

  if ( (cadena_posiciones = (short int *) malloc(numero_unidades*sizeof(short int))) == NULL) {
    fprintf(stderr, "Error en seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo, al asignar memoria.\n");
    return 1;
  }

  inicio_objetivo_grupo_fonico = cadena_unidades.begin();
  apunta_posiciones = cadena_posiciones;


  for (int cuenta_grupo_fonico = 0; cuenta_grupo_fonico < numero_grupos_fonicos; cuenta_grupo_fonico++) {

    apunta_objetivo = inicio_objetivo_grupo_fonico;

    // Contamos el número de segmentos, y salvamos las posiciones originales:
    
    numero_total_segmentos = 0;
    numero_fonemas_grupo_fonico = 1;

    for (; apunta_objetivo != cadena_unidades.end();) {
      //    for (contador = 0; contador < numero_unidades; contador++) {
      *apunta_posiciones++ = apunta_objetivo->frontera_prosodica;
      numero_fonemas_grupo_fonico++;
      if (is_segment_boundary(apunta_objetivo->semifonema))
	numero_total_segmentos++;
      ++apunta_objetivo;
      if (apunta_objetivo->semifonema[0] == '#') {
	*apunta_posiciones = apunta_objetivo->frontera_prosodica;
	break;
      }
    }

    numero_rutas_optimas_grupo_fonico = vector_grupos[cuenta_grupo_fonico].size();
    // Inicializamos a cero la cadena de costes mínimos y la máscara de contornos
    for (int cuenta_costes = 0; cuenta_costes < numero_rutas_optimas_grupo_fonico; cuenta_costes++) {
      costes_minimos[cuenta_costes] = 0.0;
      mascara_contornos[cuenta_costes] = 1;
    }
    
    for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas_grupo_fonico; contador_acentuales++) {

      // Escogemos el siguiente contorno posible:

      Grupo_acentual_candidato *apunta_grupo = vector_grupos[cuenta_grupo_fonico][contador_acentuales];

      // Comprobamos la continuidad entre contornos de grupos acentuales.

      comprueba_y_asegura_continuidad_contornos(apunta_grupo, tamano_grupo_fonico[cuenta_grupo_fonico]);

      // Creamos la cadena de valores de f0 objetivo:
    
#ifdef _METODO_ANTIGUO_INTERPOLADO_CONTORNOS
      if (crea_vector_frecuencias(cadena_unidades, numero_fonemas_grupo_fonico, apunta_grupo, apunta_grupo_objetivo, escalado_frecuencia, &frecuencias, indice_primer_fonema_grupo_fonico))
	return 1;
#else
      if (crea_vector_frecuencias_V2(cadena_unidades, numero_fonemas_grupo_fonico, apunta_grupo, apunta_grupo_objetivo, escalado_frecuencia, &frecuencias, indice_primer_fonema_grupo_fonico))
	return 1;
#endif

      if (anhade_frecuencias_objetivo(cadena_unidades, numero_fonemas_grupo_fonico, frecuencias, indice_primer_fonema_grupo_fonico))
	return 1;

      if (incluye_limites_grupo_entonativo(cadena_unidades, numero_fonemas_grupo_fonico, cadena_posiciones, apunta_grupo, tamano_grupo_fonico[cuenta_grupo_fonico], 1, indice_primer_fonema_grupo_fonico))
	return 1;

      costes_acumulados[contador_acentuales] = 0;

    } // for (contador_acentuales = 0; ...

      // Ahora, hacemos la búsqueda acústica por segmentos
    
    indice_primer_fonema_segmento = 0;
    numero_unidades_segmento = 1;
    ultimo_grupo_prosodico = 1;
    indice_camino_sintesis = 0;
    izquierdo_derecho = IZQUIERDO;
    //      puntero_objetivo = cadena_unidades.begin();
    if (cuenta_grupo_fonico == 0)
      avanza_puntero_objetivo = 1;
    
#ifdef _DEPURA_VITERBI
    viterbi.avanza_contorno();
#endif
    
    inicio_objetivo_segmento = inicio_objetivo_grupo_fonico;
    
    apunta_objetivo = inicio_objetivo_segmento;
    
    for (int cuenta_segmentos = 0; cuenta_segmentos < numero_total_segmentos; cuenta_segmentos++) {
      
      if ((cuenta_segmentos == numero_total_segmentos - 1) && (cuenta_grupo_fonico == numero_grupos_fonicos - 1) )
	ultimo_grupo_prosodico = 0;
      
      indice_primer_fonema_segmento += numero_unidades_segmento - 1; // Acordarme de actualizarlo fuera del bucle.
      
      numero_unidades_segmento = 1;
      
      while ( (is_segment_boundary(apunta_objetivo->semifonema) != 1) ||
	      (apunta_objetivo == inicio_objetivo_segmento) ) {
	numero_unidades_segmento++;
	apunta_objetivo++;
      } // while ( (apunta_objetivo->semifonema[0] != '#') ...
      
      // Selección de unidades de voz:
      
      if (inicio_objetivo_segmento != cadena_unidades.begin())
	izquierdo_derecho = DERECHO;
      
      puntero_objetivo = inicio_objetivo_segmento;
      
      // Liberamos la memoria
      
      viterbi.libera_memoria_algoritmo();
      
      if (puntero_objetivo == cadena_unidades.begin())
	numero_semifonemas_real_grupo = 2*numero_unidades_segmento - ultimo_grupo_prosodico;
      else
	numero_semifonemas_real_grupo = 2*numero_unidades_segmento - 1 - ultimo_grupo_prosodico;
      
      if (numero_unidades_segmento == 1)
	numero_semifonemas_real_grupo++;
      
      if (viterbi.inicia_algoritmo_Viterbi(*puntero_objetivo, numero_semifonemas_real_grupo, izquierdo_derecho, mascara_contornos, numero_rutas_optimas_grupo_fonico))
	return 1;
      
      if ( (cuenta_grupo_fonico != 0) || (cuenta_segmentos != 0) )
	puntero_objetivo++;
      
      for (contador = 1; contador < numero_semifonemas_real_grupo; contador++) {
	
	if (viterbi.siguiente_recursion_Viterbi(*puntero_objetivo, contador == numero_semifonemas_real_grupo - 1, mascara_contornos, numero_rutas_optimas_grupo_fonico, costes_acumulados))
	  return 1;
	
	if (contador % 2 == avanza_puntero_objetivo)
	  puntero_objetivo++;
	
      } // for (int contador = 0
      
	// Ahora escogemos el mejor camino hasta ese punto:
   
      for (contador_acentuales = 0; contador_acentuales < numero_rutas_optimas_grupo_fonico; contador_acentuales++) {
	
	if (mascara_contornos[contador_acentuales] == 0)
	  continue;

	puntero_sintesis =
	  unidades_escogidas + contador_acentuales*numero_unidades_candidatas + indice_camino_sintesis;
	
	if (viterbi.devuelve_camino_optimo_sintesis_paralelo(&numero_semifonemas_grupo_actual, &coste_actual, &puntero_sintesis, &numero_rutas_optimas_reales, contador_acentuales, mascara_contornos))
	return 1;
      
	if (mascara_contornos[contador_acentuales] == 0)
	  continue;
	
	costes_minimos[contador_acentuales] +=  coste_actual;
	costes_acumulados[contador_acentuales] += coste_actual;

      } // for (contador_acentuales = 0; ...

      indice_camino_sintesis += numero_semifonemas_grupo_actual;
      avanza_puntero_objetivo = 0;
      inicio_objetivo_segmento = puntero_objetivo;
    
      if ( (cuenta_segmentos == numero_total_segmentos - 1) && (cuenta_grupo_fonico == numero_grupos_fonicos - 1) )
	puntero_objetivo--;

      if (viterbi.poda_contornos_paralelo(mascara_contornos, costes_acumulados, numero_rutas_optimas_grupo_fonico, *puntero_objetivo))
	return 1;
      
    } // for (int cuenta_segmentos = 0; ...
    
    // Buscamos el mejor camino:
    
    indice_coste_minimo = encuentra_indice_coste_minimo(costes_minimos, numero_rutas_optimas_grupo_fonico, mascara_contornos);

    caminos_escogidos.push_back(indice_coste_minimo);

    coste_total += costes_minimos[indice_coste_minimo];
    auxiliar_grupo = vector_grupos[cuenta_grupo_fonico][indice_coste_minimo];

    for (int cuenta_grupos = 0; cuenta_grupos < tamano_grupo_fonico[cuenta_grupo_fonico]; cuenta_grupos++, apunta_contorno_escogido++)
      apunta_contorno_escogido->copia_grupo_acentual(auxiliar_grupo++);

#ifndef _MODELO_ENTONATIVO_ANTERIOR
    
    // Escribimos las frecuencias escogidas en la cadena de vectores objetivo:
    
    for (; inicio_objetivo_grupo_fonico != puntero_objetivo;
	 inicio_objetivo_grupo_fonico++, frec++)
      if (sordo_sonoro(inicio_objetivo_grupo_fonico->semifonema) == SONORO)
	inicio_objetivo_grupo_fonico->anhade_frecuencia(inicio_objetivo_grupo_fonico->frecuencias_mitad[indice_coste_minimo], inicio_objetivo_grupo_fonico->frecuencias_inicial[indice_coste_minimo], inicio_objetivo_grupo_fonico->frecuencias_final[indice_coste_minimo]);
#endif
    
    if (incluye_limites_grupo_entonativo(cadena_unidades, numero_fonemas_grupo_fonico, cadena_posiciones, vector_grupos[cuenta_grupo_fonico][indice_coste_minimo], tamano_grupo_fonico[cuenta_grupo_fonico], 0, indice_primer_fonema_grupo_fonico))
      return 1;
    
    // Escribimos las unidades escogidas en la cadena de salida:

    puntero_sintesis = unidades_escogidas + indice_coste_minimo*numero_unidades_candidatas;
    
    for (cuenta = 0; cuenta < indice_camino_sintesis; cuenta++)
      *apunta_salida++ = *puntero_sintesis++;

    apunta_grupo_objetivo += tamano_grupo_fonico[cuenta_grupo_fonico];

    indice_primer_fonema_grupo_fonico += numero_fonemas_grupo_fonico - 1;

  } // for (int cuenta_grupo_fonico = 0; ...

  if (incluye_modificacion_entonativa_sintesis(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, locutor->frecuencia_muestreo))
    return 1;

  viterbi.libera_memoria_algoritmo();

  char opcion[3];
  char nombre_fichero[FILENAME_MAX];
  char *apunta_nombre_fichero;

  if (opciones == 1) {

    if (cuenta_frases == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);
        
    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);

    sprintf(apunta_nombre_fichero, "_%d.uni", cuenta_frases);

    if (escribe_cadena_escogida_final(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, 0, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0", cuenta_frases);

    if (escribe_distancia_f0(cadena_unidades, *cadena_sintesis, numero_unidades_candidatas, nombre_fichero, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coste", cuenta_frases);

    if (escribe_coste(nombre_fichero, coste_total, caminos_escogidos, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dis_mod", cuenta_frases);

    if (escribe_discontinuidad_espectral(numero_unidades_candidatas,
					 nombre_fichero, cadena_unidades,
					 *cadena_sintesis, locutor, opcion,
					 Vector_semifonema_candidato::numero_coeficientes_cepstrales, 0))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont_esc", cuenta_frases);

    if (escribe_contorno_escogido_final(numero_acentuales, numero_unidades, nombre_fichero, frase_fonetica,cadena_unidades, contorno_escogido, cadena_acentuales, opcion))
      return 1;
    
    sprintf(apunta_nombre_fichero, "_%d.rup", cuenta_frases);

    if (escribe_frase_con_rupturas_y_pausas(numero_acentuales, numero_unidades, nombre_fichero, cadena_unidades, contorno_escogido, cadena_acentuales, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.cont", cuenta_frases);

    if (escribe_contorno_escogido_calculo_distancia(numero_acentuales, nombre_fichero, contorno_escogido, frase_entrada, coste_total / numero_unidades, opcion))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.f0_obj", cuenta_frases);
      
    if (escribe_f0_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.dur_obj", cuenta_frases);

    if (escribe_duraciones_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.pot_obj", cuenta_frases);

    if (escribe_potencias_objetivo(numero_unidades, nombre_fichero, cadena_unidades, *cadena_sintesis, "wt"))
      return 1;

    sprintf(apunta_nombre_fichero, "_%d.coart", cuenta_frases);
      
    if (escribe_costes_coarticulacion(numero_unidades_candidatas, nombre_fichero, cadena_unidades, *cadena_sintesis, locutor, NULL, indice_coste_minimo, opcion))
      return 1;

    cuenta_frases++;

  } // if (opciones == 1)
  else if (opciones == 3) {

    if (cuenta_frases++ == 1)
      strcpy(opcion, "wt");
    else
      strcpy(opcion, "at");

    strcpy(nombre_fichero, nombre_fichero_entrada);

    apunta_nombre_fichero = nombre_fichero + strlen(nombre_fichero);


    // strcpy(apunta_nombre_fichero, ".costes");

    // if (escribe_factores_calidad_voz_sintetica(*cadena_sintesis, cadena_unidades, locutor, numero_unidades, costes_caminos_acentuales[indice_coste_minimo], coste_total, nombre_fichero))
    //   return 1;

        
  }
  else if ( (opciones != 0) && (opciones != 4) ) { // aquí incluímos el resto de las opciones (no sólo opciones == 2)

    escribe_factores_f0_duracion_y_tamano_continuo(*cadena_sintesis,
						   cadena_unidades,
						   locutor,
						   numero_unidades_candidatas,
						   ficheros[0], ficheros[1],
						   ficheros[2], ficheros[3],
						   ficheros[4], ficheros[5],
						   ficheros[6], ficheros[11]);

    escribe_factores_funcion_coste_entonativa(cadena_acentuales, contorno_escogido, numero_acentuales, ficheros[7], ficheros[8], ficheros[9]);

    if (escribe_concatenacion_grupos_acentuales(contorno_escogido, numero_acentuales, ficheros[10]))

      return 1;
    
    cuenta_frases++;

  } // else


  // Liberamos la memoria de las variables locales:
  

  apunta_contorno_escogido = contorno_escogido;

  for (contador = 0; contador < numero_acentuales; contador++, apunta_contorno_escogido++)
    apunta_contorno_escogido->libera_memoria();
    
  free(contorno_escogido);
  free(unidades_escogidas);
  free(frecuencias);
  free(costes_minimos);
  free(cadena_posiciones);
  free(mascara_contornos);
  free(costes_acumulados);

  return 0;

}

#endif

#endif
