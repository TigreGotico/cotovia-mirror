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
 

#ifndef _VITERBI_ACENTUAL_HH

#define _VITERBI_ACENTUAL_HH

#include "grupos_acentuales.hpp"

//#define _DEPURA_VITERBI_ACENTUAL

/// Los valores de C_PODA_C_i y C_PODA_UNIDAD se calcularon empleando el script entrena_costes_poda_entonativa.pl, sobre 500 frases y para los corpus de Freire y Rocío.

#define C_PODA_C_i_acentual 2.0 //        // En un 0.01% de las ocasiones se perdía la mejor unidad (para el corpus de Freire. Para el de Rocío, 0.00%)
#define C_PODA_UNIDAD_acentual 4.0 //    // En un 0.01% de las ocasiones se perdía la mejor unidad (para el corpus de Freire. Para el de Rocío, 0.00%)
#define N_PODA_UNIDAD_acentual 200 // 200
#define N_PODA_Ci_acentual 200

#define PESO_COSTE_OBJETIVO_ACENTUAL	0.5F	///<  0.6 Peso que se le da al coste de objetivo.
#define PESO_COSTE_CONCATENACION_ACENTUAL	0.5F	///< 0.4 Peso que se le da al coste de concatenación.

#define _VITERBI_V2					// Modo en el que se escoge más de un camino posible hasta
// cada grupo de la iteración actual.
#define _CAMINO_UNICO // Modo en el que se escoge un único camino (incoherente con lo anterior, pero por compatibilidad hacia atrás)

#define MAXIMA_DIFERENCIA_PICO_FRECUENCIA 20.0 // Máxima diferencia de picos de frecuencia entre
// dos grupos acentuales consecutivos.

#define INCREMENTO_VARIABILIDAD_CAMINOS 2 // Incremento de coste que se hace a un camino que ya está siendo considerado en la iteración actual

#define PODA_C_t_POR_NUMERO_acentual
#define PODA_C_t_POR_VALOR_acentual
#define PODA_C_i_POR_NUMERO_acentual
#define PODA_C_i_POR_VALOR_acentual

#if defined(PODA_C_t_POR_NUMERO_acentual) || defined(PODA_C_t_POR_VALOR_acentual)


#define PODA_C_t_acentual

#endif

#if defined(PODA_C_i_POR_NUMERO_acentual) || defined(PODA_C_i_POR_VALOR_acentual)

#define PODA_C_i_acentual

#endif

#if defined(PODA_C_i_acentual) || defined(PODA_C_t_acentual)

#define PODA_acentual

#endif

// Umbrales que se consideran válidos alrededor del valor generado por el modelo de continuidad entonativa
// tras una pausa (por una coma o similar).
#define UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA	10.0F // 10.0F
#define UMBRAL_CONTINUIDAD_FONICA_TRAS_PAUSA_INTERROGATIVA	20.0F // 20.0F

// Umbral para el coste de continuidad de f0
#define UMBRAL_CONTINUIDAD_F0 10.0F

// Umbral para el máximo coste en la diferencia de f0 en el punto de concatenación
#define UMBRAL_CONTINUIDAD_F0_MAXIMO_COSTE	40.0F

// Umbral bajo el que se considera que dos saltos de f0 son iguales
#define UMBRAL_SALTO_F0 5.0F
// Umbral similar al anterior, pero para el cociente entre saltos de f0
#define UMBRAL_COCIENTE_SALTO_F0 0.65F
#define UMBRAL_SALTO_F0_COSTE_SILABICO 40.0F ///< Umbral sobre el cual se considera que la estructura de la sílaba afecta a la entonación.

// Umbral bajo el que se considera que dos factores de caída son iguales
#define UMBRAL_FACTOR_CAIDA 30.0F
// Umbral similar anterior, pero para el cociente entre factores de caída
#define UMBRAL_COCIENTE_FACTOR_CAIDA 0.5F

// Umbral para la diferencia de duración (ojo, es por fonema)
#define UMBRAL_DIFERENCIA_DURACION_GA 0.04F

#define absoluto(a) (((a) >= 0) ? (a) : -(a))
#define maximo(a, b) ( ((a) > (b)) ? a : b)   
#define minimo(a, b) ( ((a) < (b)) ? a : b)
#define mismo_signo(a, b) ( ( (a) >= 0 ) && ( (b) >= 0 ) || ( (a) < 0 ) && ( (b) < 0) ? 1 : 0)

class Viterbi_acentual {

  Locutor * locutor_actual;

#ifdef _DEPURA_VITERBI_ACENTUAL
  FILE *fichero_depura;
  char nombre_fichero_depura[FILENAME_MAX];
  int iteracion;
  int iteracion_gf;
#endif

  Estructura_C_i_acentual *C_i;
  int tamano_C_i;
  int memoria_reservada_C_i;

  Estructura_C_i_acentual_GF *C_i_GF;

  Estructura_coste_acentual *vector_coste;
  int tamano_vector_coste;

  Estructura_coste_acentual_GF *vector_coste_GF;

  Matriz matriz;
  //	Cache memoria_cache;

  int numero_C_c;

#ifdef PODA_C_t_POR_VALOR_acentual
  float minimo_c_t;
#endif

#ifdef PODA_C_i_POR_VALOR_acentual
  float minimo_c_i;
#endif

#ifdef PODA_C_i_POR_NUMERO_acentual
  char maxima_memoria_reservada;
#endif

#ifdef _BUSQUEDA_EXHAUSTIVA
  float peso_obj_conc;
  float peso_pos_acentual, peso_tip_prop, peso_num_sil;
  float peso_pos_fonico, peso_dur, peso_fin_grupo;

  int primer_identificador, ultimo_identificador;
#endif

  int numero_contornos;	// número de contornos resultantes de la búsqueda entonativa.

  int numero_contornos_escogidos;	// número de contornos que pasan a la búsqueda acústica.
    
  int *indices_escogidos; // cadena empleada para la devolución de los mejores caminos.

  char *contornos_escogidos; // cadena empleada para la devolución de los mejores caminos.

  unsigned char fuerza_fronteras_entonativas; // Si vale cero, deja que el algoritmo de selección decida si se introduce o no una frontera entonativa. En caso contrario, fuerza que se introduzca la frontera objetivo.

  void inicia_poda_C_t_acentual();

  int carga_medias_grupos(FILE *fichero, int locutor);

  void inicia_poda_C_i_acentual();

  int carga_tabla_cache_acentual();

  int crea_delta_u(Grupo_acentual_candidato ***cadena_id, int *tamano, Grupo_acentual_objetivo *objetivo);

  float comprueba_entrada_en_cache_acentual(int identificador1, int identificador2);

  void calcula_coste_minimo_hasta_unidad(Estructura_coste_acentual *C_concatena);

  void calcula_coste_minimo_hasta_unidades(Estructura_coste_acentual *vector_coste, int numero_candidatos);

  int crea_vector_C_t(Estructura_coste_acentual **costes, int *tamano, Grupo_acentual_candidato **delta_u, Grupo_acentual_objetivo *objetivo);

  int crea_vector_C_t_grupos_fonicos(vector<Grupo_acentual_candidato *> &grupos, vector<float> &costes_fonicos, int tamano_grupo_fonico, Estructura_coste_acentual_GF *costes);

  float calcula_C_t(Grupo_acentual_candidato *candidato, Grupo_acentual_objetivo *objetivo);

  inline float calcula_C_c(Grupo_acentual_candidato *unidad1, Grupo_acentual_candidato *unidad2);

  inline float calcula_C_c(Grupo_acentual_candidato *unidad1, Grupo_acentual_candidato *unidad2,
			   Grupo_acentual_objetivo *unidad_objetivo);

  int ordena_y_poda_C_t(Estructura_coste_acentual *actual, int posicion);

  int ordena_y_poda_C_i(Estructura_C_i_acentual *inicio, int posicion);

  int ordena_y_poda_C_i_GF(Estructura_C_i_acentual_GF *inicio, int posicion);

  int crea_vector_C_i(Estructura_coste_acentual *C_concatena, int tamano);

  int crea_vector_C_i_grupos_fonicos(Estructura_coste_acentual_GF *C_concatena, int tamano);

  int crea_vector_C_i_V2(Estructura_coste_acentual *C_concatena, int tamano,
			 Grupo_acentual_objetivo *objetivo);
  int crea_vector_C_i_V2_unico_camino(Estructura_coste_acentual *C_concatena, int tamano, Grupo_acentual_objetivo *objetivo);
  int crea_vector_C_i_V2(Estructura_coste_acentual *C_concatena, int tamano);
  int crea_vector_C_i_V2_unico_camino(Estructura_coste_acentual *C_concatena, int tamano);

public:

  Viterbi_acentual();

  ~Viterbi_acentual();

  void modifica_fuerza_fronteras_entonativas(unsigned char fuerza);

  int inicia_algoritmo_Viterbi_acentual(Grupo_acentual_objetivo *objetivo, int numero_objetivos, char *inicio_nombre_fichero_grupos = NULL);

  int inicia_algoritmo_Viterbi_acentual_grupos_fonicos(vector<Grupo_acentual_candidato * > &grupos, vector<float> &costes, int tamano_grupo_fonico, int numero_grupos_fonicos, char *inicio_nombre_fichero_grupos = NULL);

  int siguiente_recursion_Viterbi(Grupo_acentual_objetivo *objetivo);

  int siguiente_recursion_Viterbi_V2(Grupo_acentual_objetivo *objetivo);

  int siguiente_recursion_Viterbi_grupos_fonicos(vector<Grupo_acentual_candidato *> &grupos, vector<float> &costes, int tamano_grupo_fonico);

  int devuelve_camino_optimo_acentual(unsigned char secuencia_optima, Grupo_acentual_candidato **acentuales, int *numero_unidades, int *numero_real_caminos, float *coste = NULL);

  int devuelve_camino_optimo_acentual_GF(unsigned char secuencia_optima, Grupo_acentual_candidato **acentuales, int *numero_unidades, int *numero_real_caminos, vector<int> &tamano_grupos_fonicos, float *coste = NULL);

  int devuelve_mejores_caminos(Grupo_acentual_candidato *acentuales, int indice_acentuales,
			       int numero_grupos, int numero_grupos_totales, int *numero_real_caminos, char criterio);

  void libera_memoria_algoritmo_acentual();

  void finaliza();
        
  int imprime_C_c();

  int inicializa(int num_total_contornos, int num_contornos_escogidos, Locutor * locutor);

  int devuelve_grupo_acentual(Grupo_acentual_candidato *grupo, int posicion_grupo,
			      int posicion_silaba_acentuada, int tipo_proposicion,
			      int identificador);

#ifdef _DEPURA_VITERBI_ACENTUAL
  void avanza_iterador();
  void reinicia_iterador();
#endif

#ifdef _BUSQUEDA_EXHAUSTIVA

  void establece_identificadores(int p_identificador, int u_identificador);

  void establece_pesos_busqueda(float p_obj_conc, float p_pos_ac, float p_tip_prop,
				float p_num_sil, float p_pos_fon, float p_dur,
				float p_fin_grupo);

  float calcula_C_t_busqueda(Grupo_acentual_candidato *candidato, Grupo_acentual_objetivo *objetivo);

#endif

};
#endif
