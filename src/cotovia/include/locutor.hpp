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
 


#ifndef _LOCUTOR_HPP

#define _LOCUTOR_HPP

#define FICHERO_PESOS_FUNCION_OBJETIVO "pesos_funcion_objetivo.bin"
#define TAMANO_MAXIMO_CADENA_LOCUTORES 100L

class Locutor {

public:

  char nombre[FILENAME_MAX];		// Nombre del locutor.
  char directorio[FILENAME_MAX];
  // Directorio en el que se encuentran los ficheros de los
  // modelos del locutor.

#if defined(_UTILIZA_CENTROIDES) ||					\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

  Vector_cepstrum_varianza *centroides_semifonemas_varianza;
  // Vector con los centroides de los vectores mel-cepstrum medios de
  // los semifonemas del Corpus, con su varianza.

#endif

  float indices_continuidad_est[NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  float indices_continuidad_coart[NUMERO_ELEMENTOS_CLASE_ESPECTRAL][NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  // índices de continuidad espectral en las partes estacionaria y de
  // coarticulación.

#if defined(_UTILIZA_CENTROIDES) ||			\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||	\
  defined(_UTILIZA_REGRESION)

  Vector_pesos_regresion pesos_regresion[NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  // Pesos entrenados según el modelo contextual
  // basado en regresión lineal.

#endif

#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA_RED_NEURONAL
   Red_neuronal red_continuidad_fonica;
   // Red neuronal que calcula el salto de f0 tras una pausa.
   Media_varianza estadisticos_continuidad_fonica[NUMERO_ENTRADAS_RED_CONTINUIDAD_FONICA];
   // Medias y desviaciones para normalizar la entrada de la red.
#endif

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

  Red_neuronal red_contextual[NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  // Redes neuronales del modelo contextual basado en
  // redes.

  Media_varianza estadisticos_contextual[NUMERO_ELEMENTOS_CLASE_ESPECTRAL][NUMERO_ENTRADAS_RED_ESPECTRAL];
  // Medias y desviaciones para normalizar las entradas de
  // dichas redes.

#endif

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

  Modelo_energia modelo_energia;
  // Modelo de estimación de la energía basado en regresión lineal.
#else

  Red_neuronal red_energia[NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  // Redes neuronales para la estimación de la energía.

  Media_varianza_energia estadisticos_energia[NUMERO_ELEMENTOS_CLASE_ESPECTRAL];
  // Medias y desviaciones para normalizar las entradas de
  // dichas redes.
#endif

  //    Media_varianza medias_grupos[NUMERO_GRUPOS_POSIBLES];
  // Medias y varianzas de los grupos acentuales organizados según
  // la posición en el grupo entonativo, la sílaba acentuada y el
  // tipo de frase.

  Modelo_duracion modelo_duracion[NUMERO_ELEMENTOS_CLASE_DURACION];
  // Modelos de duración de las diferentes clases de fonemas.

  estructura_unidades *tabla_semifonemas_d[NUMERO_FONEMAS][NUMERO_FONEMAS];
  estructura_unidades *tabla_semifonemas_i[NUMERO_FONEMAS][NUMERO_FONEMAS];
  // Tablas con los semifonemas del Corpus.

  FILE *fichero_sonido;   // Puntero al fichero en el que está la forma de onda.

  int numero_unidades_voz;
  // Número total de semifonemas del Corpus de voz.

  estructura_grupos_acentuales **tabla_grupos;
  // Tabla con los grupos acentuales del Corpus prosódico.

#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA
  map <int, float> contexto_gramatical_salto_f0;
  map <int, float> contexto_gramatical_factor_caida;
  map <int, float> probabilidad_insercion_rupturas;
#endif

#ifdef _MINOR_PHRASING_TEMPLATE
  estructura_grupos_fonicos tabla_GF[NUMERO_GF_POSIBLES];
#endif

  int frecuencia_muestreo;	// Frecuencia de muestreo del fichero de sonido.

  int min_pitch;			// Mínimo período, en muestras, admisible para el locutor.

  int max_pitch;			// Máximo período, en muestras, admisible para el locutor.

  int primera_unidad_corpus500;	// Identificador numérico de la primera unidad del Corpus500
  // (Si existe. Si no, se inicializa a INT_MAX).
    

  Locutor();

  ~Locutor();

  void inicia_locutor();

  void introduce_directorio(char *nombre_directorio, char *nombre_locutor);

  void introduce_directorio(char *nombre_directorio);

  int lee_modelos_duracion();

  void libera_memoria_modelos_duracion();

  int lee_modelo_energia();

  void libera_memoria_modelo_energia();

  int lee_corpus_prosodico();

  void libera_memoria_corpus_prosodico();

  int lee_corpus_voz_y_ficheros_seleccion();

  void libera_memoria_corpus_voz_y_ficheros_seleccion();

  int lee_corpus_voz();

  int escribe_contenido_corpus_voz(FILE *fichero_corpus, FILE *fichero_catalogo, char opcion);

  int escribe_contenido_corpus_voz_fonemas(FILE *fichero_corpus, FILE *fichero_catalogo, char opcion);
    
  int escribe_contenido_corpus_prosodico(FILE *fichero, char opcion);
    
  void libera_memoria_corpus_voz();

  int lee_indices_continuidad_espectral();

  int lee_centroides_semifonemas();

  void libera_memoria_centroides_semifonemas();

  int lee_pesos_funcion_objetivo();

  int lee_redes_neuronales_funcion_objetivo();

  int lee_red_neuronal_continuidad_fonica();

  int lee_frecuencia_muestreo();
    
  int lee_fichero_contextos_gramaticales();

  int lee_fichero_insercion_rupturas();

  int abre_fichero_sonido();

  void cierra_fichero_sonido();

  int escribe_contenido_de_tablas();

  int escribe_informacion_clustering(char unidad_1[], char unidad_2[], char izquierdo_derecho,
				     FILE *fichero_salida);

};

//extern Locutor *locutor;
//extern int locutores_cargados;


#endif

