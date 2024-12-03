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
#include <map>
#include "modos.hpp"

#include "fonemas.hpp"
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


/**
 * \author	Fran Campillo
 * \remark	Constructor de la clase Locutor.
 */
#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA
Locutor::Locutor() : contexto_gramatical_salto_f0(), contexto_gramatical_factor_caida(), probabilidad_insercion_rupturas() {
#else
  Locutor::Locutor() {
#endif

    directorio[0] = '\0';

#if defined(_UTILIZA_CENTROIDES) ||					\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

    centroides_semifonemas_varianza = NULL;

#endif

    fichero_sonido = NULL;

    tabla_grupos = NULL;

    frecuencia_muestreo = 16000; // Por defecto, 16 KHz.

    for (int filas = 0; filas < NUMERO_FONEMAS; filas++)
      for (int columnas = 0; columnas < NUMERO_FONEMAS; columnas++) {
	tabla_semifonemas_d[filas][columnas] = NULL;
	tabla_semifonemas_i[filas][columnas] = NULL;
      }

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_contextual[contador].inicia_red();

#endif

#ifdef _ESTIMA_ENERGIA_RED_NEURONAL

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
   
      red_energia[contador].inicia_red();


#else

    modelo_energia.coeficientes = NULL;

#endif

    //	corpus_difonemas = NULL;

    for (int i = 0; i < NUMERO_ELEMENTOS_CLASE_DURACION; i++)
      modelo_duracion[i].coef = NULL;
    

  }

  /**
     \brief Destructor de la clase Locutor
   */

  Locutor::~Locutor() {

    libera_memoria_modelos_duracion();
    libera_memoria_modelo_energia();
    libera_memoria_corpus_voz_y_ficheros_seleccion();
    libera_memoria_corpus_prosodico();

  }

  /**
   * \author	Fran Campillo
   * \remark	Función que inicializa las variables dinámicas de la red.
   */

  void Locutor::inicia_locutor() {

    directorio[0] = '\0';

#if defined(_UTILIZA_CENTROIDES) ||					\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

    centroides_semifonemas_varianza = NULL;

#endif

    fichero_sonido = NULL;

    tabla_grupos = NULL;

    frecuencia_muestreo = 16000; // Por defecto, 16 KHz.
    
#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_contextual[contador].inicia_red();

#endif

#ifdef _ESTIMA_ENERGIA_RED_NEURONAL
    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_energia[contador].inicia_red();

#endif

    //	corpus_difonemas = NULL;

  }

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de almacenar el nombre del directorio en el
   * que se encuentra la información de los diferentes modelos.
   * \param 	nombre_directorio: nombre del directorio en el que se encuentra
   * la información del locutor.
   * \param	nombre_locutor: nombre del locutor.
   */

  void Locutor::introduce_directorio(char *nombre_directorio, char *nombre_locutor) {

    strcpy(directorio, nombre_directorio);
    strcpy(nombre, nombre_locutor);

  }

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de almacenar el nombre del directorio en el
   * que se encuentra la información de los diferentes modelos.
   * \param 	nombre_directorio: nombre del directorio en el que se encuentra
   * la información del locutor.
   */

  void Locutor::introduce_directorio(char *nombre_directorio) {

    strcpy(directorio, nombre_directorio);

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los datos de los
   *			modelos de duración.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_modelos_duracion() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif
    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    // Silencio:

    strcpy(apunta_nombre, FICHERO_DURACION_SILENCIO);

    if (modelo_duracion[SILENCIO_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Vocales:

    strcpy(apunta_nombre, FICHERO_DURACION_VOCAL);

    if(modelo_duracion[VOC_DUR].lee_modelo(nombre_fichero))
      return 1;

#ifdef _CONSIDERA_SEMIVOCALES
    // Semivocales:

    strcpy(apunta_nombre, FICHERO_DURACION_SEMIVOCAL);

    if(modelo_duracion[SEMI_VOCAL_DUR].lee_modelo(nombre_fichero))
      return 1;

#endif

    // Fricativas sordas:

    strcpy(apunta_nombre, FICHERO_DURACION_FRIC_SOR);

    if (modelo_duracion[FRIC_SOR_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Oclusivas sordas:

    strcpy(apunta_nombre, FICHERO_DURACION_OCLU_SOR);

    if (modelo_duracion[OCLU_SOR_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Oclusivas sonoras:

    strcpy(apunta_nombre, FICHERO_DURACION_OCLU_SON);

    if (modelo_duracion[OCLU_SON_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Vibrantes:

    strcpy(apunta_nombre, FICHERO_DURACION_VIBRANTE);

    if (modelo_duracion[VIBRANTE_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Laterales:

    strcpy(apunta_nombre, FICHERO_DURACION_LATERAL);

    if (modelo_duracion[LATERAL_DUR].lee_modelo(nombre_fichero))
      return 1;

    // Nasales:

    strcpy(apunta_nombre, FICHERO_DURACION_NASAL);

    if (modelo_duracion[NASAL_DUR].lee_modelo(nombre_fichero))
      return 1;

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria del objeto destinada a
   * 			guardar los modelos de duración.
   */

  void Locutor::libera_memoria_modelos_duracion() {

    for (int i = 0; i < NUMERO_ELEMENTOS_CLASE_DURACION; i++)
      if (modelo_duracion[i].coef) {
	free(modelo_duracion[i].coef);
	modelo_duracion[i].coef = NULL;
      }
    
  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los datos de los
   *  modelos de energía.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_modelo_energia() {

    char nombre_fichero[FILENAME_MAX];

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\%s", directorio, FICHERO_MODELO_ENERGIA);
#else
    sprintf(nombre_fichero, "%s/%s", directorio, FICHERO_MODELO_ENERGIA);
#endif

    if (modelo_energia.lee_modelo(nombre_fichero))
      return 1;

#else

    char *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    // Silencio:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_SILENCIO);

    if (red_energia[SILENCIO_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_SILENCIO);
  
    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[SILENCIO_ESP].numero_entradas,
				  estadisticos_energia[SILENCIO_ESP]))
      return 1;


    // Vocales abiertas

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_VOCAL_ABIERTA);

    if(red_energia[VOC_AB_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_VOCAL_ABIERTA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[VOC_AB_ESP].numero_entradas,
				  estadisticos_energia[VOC_AB_ESP]))
      return 1;

    // Vocales medias

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_VOCAL_MEDIA);

    if (red_energia[VOC_MED_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_VOCAL_MEDIA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[VOC_MED_ESP].numero_entradas,
				  estadisticos_energia[VOC_MED_ESP]))
      return 1;

    // Vocales cerradas

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_VOCAL_CERRADA);
    
    if (red_energia[VOC_CER_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_VOCAL_CERRADA);


    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[VOC_CER_ESP].numero_entradas,
				  estadisticos_energia[VOC_CER_ESP]))
      return 1;


#ifdef _CONSIDERA_SEMIVOCALES
    // Semivocales

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_SEMI_VOCAL);
    
    if (red_energia[SEMI_VOCAL_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_SEMI_VOCAL);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[SEMI_VOCAL_ESP].numero_entradas, estadisticos_energia[SEMI_VOCAL_ESP]))
      return 1;
#endif

    // Oclusivas sordas:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_OCLU_SOR);

    if (red_energia[OCLU_SOR_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_OCLU_SOR);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[OCLU_SOR_ESP].numero_entradas,
				  estadisticos_energia[OCLU_SOR_ESP]))
      return 1;
  

    // Oclusivas sonoras:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_OCLU_SON);

    if (red_energia[OCLU_SON_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_OCLU_SON);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[OCLU_SON_ESP].numero_entradas,
				  estadisticos_energia[OCLU_SON_ESP]))
      return 1;

    // Aproximantes:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_APROXIMANTE);

    if (red_energia[APROX_SON_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_APROXIMANTE);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[APROX_SON_ESP].numero_entradas,
				  estadisticos_energia[APROX_SON_ESP]))
      return 1;

    // Fricativas sordas:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_FRIC_SOR);

    if (red_energia[FRIC_SOR_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_FRIC_SOR);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[FRIC_SOR_ESP].numero_entradas,
				  estadisticos_energia[FRIC_SOR_ESP]))
      return 1;


    // Nasales:
  
    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_NASAL);
  
    if (red_energia[NASAL_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;
  
    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_NASAL);
  
    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[NASAL_ESP].numero_entradas,
				  estadisticos_energia[NASAL_ESP]))
      return 1;
  

    // Vibrantes:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_VIBRANTE);
  
    if (red_energia[VIBRANTE_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;
  
    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_VIBRANTE);
  
    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[VIBRANTE_ESP].numero_entradas,
				  estadisticos_energia[VIBRANTE_ESP]))
      return 1;

    // Laterales:

    strcpy(apunta_nombre, FICHERO_RN_ENERGIA_LATERAL);

    if (red_energia[LATERAL_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RN_MEDIA_VARIANZA_LATERAL);

    if (lee_medias_y_desviaciones(nombre_fichero, red_energia[LATERAL_ESP].numero_entradas,
				  estadisticos_energia[LATERAL_ESP]))
      return 1;

 
#endif

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria del objeto destinada a
   * 			guardar el modelo de energía.
   */

  void Locutor::libera_memoria_modelo_energia() {

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL

    modelo_energia.libera_memoria();

#else

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_energia[contador].libera_memoria_red();

#endif

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los datos de los
   *			grupos acentuales del Corpus prosódico.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_corpus_prosodico() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;
    estructura_grupos_acentuales **correcaminos;
    estructura_grupos_acentuales *acentuales;
    Grupo_acentual_candidato *recorre_acentuales;
    int contador = 0, cuenta;
    FILE *fichero;

    // Asignamos memoria para la tabla.

    if ( (tabla_grupos = (estructura_grupos_acentuales **) malloc(NUMERO_GRUPOS_POSIBLES*
								  sizeof(estructura_grupos_acentuales *))) == NULL) {
      fprintf(stderr, "Error en inicia_lista_grupos, al asignar memoria.\n");
      return 1;
    }

    memset(tabla_grupos, 0, NUMERO_GRUPOS_POSIBLES*sizeof(estructura_grupos_acentuales *));


    if ( (acentuales = (estructura_grupos_acentuales *) malloc(NUMERO_CLUSTERES*
							       sizeof(estructura_grupos_acentuales))) == NULL) {
      fprintf(stderr, "Error en lee_tabla_de_fichero, al asignar memoria.\n");
      return 1;
    }

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    strcpy(apunta_nombre, FICHERO_ACENTUALES);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_prosodico, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...

#ifdef _x86_64
    fread(tabla_grupos, sizeof(estructura_grupos_acentuales_compatible), NUMERO_GRUPOS_POSIBLES, fichero);
#else
    estructura_grupos_acentuales_compatible tabla_grupos_compatible[NUMERO_GRUPOS_POSIBLES];
    fread(tabla_grupos_compatible, sizeof(estructura_grupos_acentuales_compatible), NUMERO_GRUPOS_POSIBLES, fichero);
    for (cuenta = 0; cuenta < NUMERO_GRUPOS_POSIBLES; cuenta++)
      if (tabla_grupos_compatible[cuenta].puntero != NULL)
	tabla_grupos[cuenta] = tabla_grupos_compatible[cuenta].puntero;
      else
	if (tabla_grupos_compatible[cuenta].dummy != 0)
	  tabla_grupos[cuenta] = (estructura_grupos_acentuales *) &tabla_grupos_compatible;
	else
	  tabla_grupos[cuenta] = NULL;
#endif
    correcaminos = tabla_grupos;

    while (contador++ < NUMERO_GRUPOS_POSIBLES) {

      if (*correcaminos == NULL) {
	correcaminos++;
	continue;
      }

      (*correcaminos) = acentuales++;

      fread(*correcaminos, sizeof(estructura_grupos_acentuales), 1, fichero);

      if ( ((*correcaminos)->grupos = (Grupo_acentual_candidato *) malloc(
									  (*correcaminos)->numero_grupos*sizeof(Grupo_acentual_candidato)))
	   == NULL) {

	fprintf(stderr, "Error en lee_tabla_de_fichero, al asignar memoria.\n");
	return 1;
      } // if ( ((*correcaminos...

      recorre_acentuales = (*correcaminos)->grupos;

      for (cuenta = 0; cuenta < (*correcaminos)->numero_grupos; cuenta++, recorre_acentuales++)
	if (recorre_acentuales->lee_datos_bin(fichero))
	  return 1;

      correcaminos++;

    } // while(!feof(...

    fclose(fichero);

#ifdef _MINOR_PHRASING_TEMPLATE

    strcpy(apunta_nombre, FICHERO_GF);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_prosodico, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...

    if (Grupo_fonico_frontera::lee_fichero_unico(fichero, tabla_GF))
      return 1;

    fclose(fichero);

#endif

    /*
    // Por último, leemos los datos de las medias de los grupos acentuales:

    strcpy(apunta_nombre, FICHERO_MEDIAS_GRUPOS_ACENTUALES);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
    fprintf(stderr, "Error en Locutor::lee_corpus_prosodico, al intentar \
    abrir el fichero %s.\n", nombre_fichero);
    return 1;
    } // if ( (fichero = ...

    fread(medias_grupos, sizeof(Media_varianza), NUMERO_GRUPOS_POSIBLES, fichero);

    fclose(fichero);
    */
    return 0;

  }

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria del objeto destinada a
   * 			guardar el corpus prosódico.
   */

  void Locutor::libera_memoria_corpus_prosodico() {

    Grupo_acentual_candidato *recorre_acentuales;
    estructura_grupos_acentuales **correcaminos, *libera;

    if (tabla_grupos == NULL)
      return;

    correcaminos = tabla_grupos;

    libera = NULL;

    for (int contador = 0; contador < NUMERO_GRUPOS_POSIBLES; contador++, correcaminos++) {

      if (*correcaminos != NULL) {

	recorre_acentuales = (*correcaminos)->grupos;

	for (int cuenta = 0; cuenta < (*correcaminos)->numero_grupos;
	     cuenta++, recorre_acentuales++)
	  recorre_acentuales->libera_memoria();

	if (libera == NULL)
	  libera = *correcaminos;

	free((*correcaminos)->grupos);

	tabla_grupos[contador] = NULL;

      } // if (correcaminos != NULL

    } // for (int contador = 0...

    free(libera);

    if (*tabla_grupos)
      free(*tabla_grupos);

    free(tabla_grupos);

    tabla_grupos = NULL;

  }


  /**
   * \author	Fran Campillo
   * \remarks	Función encargada de cargar en la memoria del objeto los datos de los
   * semifonemas del Corpus de voz, además de los pesos del modelo de regresión lineal
   * o las redes neuronales, según proceda.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_corpus_voz_y_ficheros_seleccion() {

    estructura_unidades *correcaminos;
    Vector_semifonema_candidato *puntero_vector;
    int contador;
    int indice1, indice2;
    register int numero_unidades;
    FILE *fichero_unico;
    char * cadena_lectura; //char cadena_lectura[TAMANO_MAXIMO_FICHERO];
    char *cadena_datos;
    register int memoria_puntero_a_unidades = TIPOS_UNIDADES*sizeof(estructura_indices);
    ficheros_unidades *unidades, *recorre;
    int cuenta_unidades;
    indice_ficheros *indice_a_fichero;
    indice_ficheros indices_en_fichero[NUMERO_FONEMAS][NUMERO_FONEMAS][2];
    char nombre_fichero[FILENAME_MAX], *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    // Leemos la frecuencia de muestreo:

    if (lee_frecuencia_muestreo())
      return 1;

    strcpy(apunta_nombre, FICHERO_UNIDADES);

    // Cargamos el Corpus de semifonemas.

    if ( (fichero_unico = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al intentar abrir \
				el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_unico = ...

    fread(&cuenta_unidades, sizeof(int), 1, fichero_unico);

    numero_unidades_voz = cuenta_unidades;

    if ( (unidades = (ficheros_unidades *) malloc(cuenta_unidades*sizeof(ficheros_unidades))) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
      return 1;
    } // if ( (unidades = ...

    if ((cadena_lectura = (char *) malloc(TAMANO_MAXIMO_FICHERO * sizeof(char))) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
      return 1;
    } // if ( (cadena_lectura = ...

    fread(unidades, sizeof(ficheros_unidades), cuenta_unidades, fichero_unico);

    fclose(fichero_unico);

    recorre = unidades;

    strcpy(apunta_nombre, FICHERO_CORPUS_VOZ);

    if ( (fichero_unico = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al intentar abrir el \
				fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_unico = fopen(...

    Vector_semifonema_candidato::lee_cabecera(fichero_unico);

    fread(indices_en_fichero, sizeof(indice_ficheros), NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero_unico);

    memset(tabla_semifonemas_i, 0, NUMERO_FONEMAS*NUMERO_FONEMAS*sizeof(estructura_unidades *));
    memset(tabla_semifonemas_d, 0, NUMERO_FONEMAS*NUMERO_FONEMAS*sizeof(estructura_unidades *));

    while(cuenta_unidades--) {

      if ( ( (indice1 = devuelve_indice_fonema(recorre->primer_fonema)) == -1) ||
	   ( (indice2 = devuelve_indice_fonema(recorre->segundo_fonema)) == -1) ) {
	fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion: Fichero de unidades no válido.\n");
	return 1;
      } // if ( (indice1 = ...

      if (recorre->izq_der == IZQUIERDO) {
	if ( (tabla_semifonemas_i[indice1][indice2] = (estructura_unidades *)
	      malloc(sizeof(estructura_unidades))) == NULL) {
	  fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
	  return 1;
	} // if ( (tabla_semifonemas_i...
	correcaminos = tabla_semifonemas_i[indice1][indice2];
	indice_a_fichero = &indices_en_fichero[indice1][indice2][0];
      } // if (recorre->izq_der...
      else {
	if ( (tabla_semifonemas_d[indice1][indice2] = (estructura_unidades *)
	      malloc(sizeof(estructura_unidades))) == NULL) {
	  fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
	  return 1;
	}
	correcaminos = tabla_semifonemas_d[indice1][indice2];
	indice_a_fichero = &indices_en_fichero[indice1][indice2][1];
      } // else { if (recorre

      fread(cadena_lectura, sizeof(char), indice_a_fichero->tamano, fichero_unico);

      if ( (cadena_datos = (char *) malloc(indice_a_fichero->tamano - memoria_puntero_a_unidades - sizeof(int))) == NULL) {
	fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
	return 1;
      } // if ( (cadena_datos...

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
      } // for (contador = 0; ...

      recorre++;
    } // while (cuenta_unidades--)

    fclose(fichero_unico);
    free(unidades);
    free(cadena_lectura);

    // Ahora, el resto de las variables que se emplean en el algoritmo de Viterbi.

    // Leemos los índices de continuidad espectral:

    if (lee_indices_continuidad_espectral())
      return 1;

#if (defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)) && !defined(_UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES)
    // Cargamos las redes neuronales y los estadísticos de la entrada.

    if (lee_redes_neuronales_funcion_objetivo())
      return 1;

    // También los ficheros de centroides:

    if (lee_centroides_semifonemas())
      return 1;

#endif

// #ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
//     if (lee_red_neuronal_continuidad_fonica())
//       return 1;

// #endif

#if defined(_UTILIZA_CENTROIDES) || defined(UTILIZA_REGRESION) ||	\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION)

    // Cargamos los pesos del modelo de regresión lineal y los centroides:

    // Los pesos del modelo:

    if (lee_pesos_funcion_objetivo())
      return 1;

    // Los ficheros de centroides:

    if (lee_centroides_semifonemas())
      return 1;

#endif

    return 0;

  }

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria destinada a guardar los
   * semifonemas del Corpus de voz, además de las variables del algoritmo de
   * selección de unidades.
   */

  void Locutor::libera_memoria_corpus_voz_y_ficheros_seleccion() {

    // Primero liberamos el Corpus de voz:

    libera_memoria_corpus_voz();

    // Luego, el resto de las variables.

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_contextual[contador].libera_memoria_red();

#endif
       
#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA_RED_NEURONAL
     red_continuidad_fonica.libera_memoria_red();
 #endif

#if defined(_UTILIZA_CENTROIDES) || defined(UTILIZA_REGRESION) ||	\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

    libera_memoria_centroides_semifonemas();

#endif

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los datos de los
   * semifonemas del Corpus de voz.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_corpus_voz() {

    estructura_unidades *correcaminos;
    Vector_semifonema_candidato *puntero_vector;
    int contador;
    int indice1, indice2;
    register int numero_unidades;
    FILE *fichero_unico;
    char *cadena_lectura; //[TAMANO_MAXIMO_FICHERO];
    char *cadena_datos;
    register int memoria_puntero_a_unidades = TIPOS_UNIDADES*sizeof(estructura_indices);
    ficheros_unidades *unidades, *recorre;
    int cuenta_unidades;
    indice_ficheros *indice_a_fichero;
    indice_ficheros indices_en_fichero[NUMERO_FONEMAS][NUMERO_FONEMAS][2];
    char nombre_fichero[FILENAME_MAX], *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    if ((cadena_lectura = (char *) malloc(TAMANO_MAXIMO_FICHERO * sizeof(char))) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz_y_ficheros_seleccion, al asignar memoria.\n");
      return 1;
    } // if ( (cadena_lectura = ...

    // Leemos la frecuencia de muestreo:

    if (lee_frecuencia_muestreo())
      return 1;

    strcpy(apunta_nombre, FICHERO_UNIDADES);

    // Cargamos el Corpus de semifonemas.

    if ( (fichero_unico = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_unico = ...

    fread(&cuenta_unidades, sizeof(int), 1, fichero_unico);

    numero_unidades_voz = cuenta_unidades;

    if ( (unidades = (ficheros_unidades *) malloc(cuenta_unidades*sizeof(ficheros_unidades))) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz, al asignar memoria.\n");
      return 1;
    } // if ( (unidades = ...

    fread(unidades, sizeof(ficheros_unidades), cuenta_unidades, fichero_unico);

    fclose(fichero_unico);

    recorre = unidades;

    strcpy(apunta_nombre, FICHERO_CORPUS_VOZ);

    if ( (fichero_unico = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_corpus_voz, al intentar abrir el \
        fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_unico = fopen(...

    Vector_semifonema_candidato::lee_cabecera(fichero_unico);
  	
    fread(indices_en_fichero, sizeof(indice_ficheros), NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero_unico);

    memset(tabla_semifonemas_i, 0, NUMERO_FONEMAS*NUMERO_FONEMAS*sizeof(estructura_unidades *));
    memset(tabla_semifonemas_d, 0, NUMERO_FONEMAS*NUMERO_FONEMAS*sizeof(estructura_unidades *));

    while(cuenta_unidades--) {

      if ( ( (indice1 = devuelve_indice_fonema(recorre->primer_fonema)) == -1) ||
	   ( (indice2 = devuelve_indice_fonema(recorre->segundo_fonema)) == -1) ) {
	fprintf(stderr, "Error en Locutor::lee_corpus_voz: Fichero de unidades no válido.\n");
	return 1;
      } // if ( (indice1 = ...

      if (recorre->izq_der == IZQUIERDO) {
	if ( (tabla_semifonemas_i[indice1][indice2] = (estructura_unidades *)
	      malloc(sizeof(estructura_unidades))) == NULL) {
	  fprintf(stderr, "Error en Locutor::lee_corpus_voz, al asignar memoria.\n");
	  return 1;
	} // if ( (tabla_semifonemas_i...
	correcaminos = tabla_semifonemas_i[indice1][indice2];
	indice_a_fichero = &indices_en_fichero[indice1][indice2][0];
      } // if (recorre->izq_der...
      else {
	if ( (tabla_semifonemas_d[indice1][indice2] = (estructura_unidades *)
	      malloc(sizeof(estructura_unidades))) == NULL) {
	  fprintf(stderr, "Error en Locutor::lee_corpus_voz, al asignar memoria.\n");
	  return 1;
	}
	correcaminos = tabla_semifonemas_d[indice1][indice2];
	indice_a_fichero = &indices_en_fichero[indice1][indice2][1];
      } // else { if (recorre

      fread(cadena_lectura, sizeof(char), indice_a_fichero->tamano, fichero_unico);

      if ( (cadena_datos = (char *) malloc(indice_a_fichero->tamano - memoria_puntero_a_unidades - sizeof(int))) == NULL) {
	fprintf(stderr, "Error en Locutor::lee_corpus_voz, al asignar memoria.\n");
	return 1;
      } // if ( (cadena_datos...

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
      } // for (contador = 0; ...

      recorre++;
    } // while (cuenta_unidades--)

    fclose(fichero_unico);
    free(cadena_lectura);
    free(unidades);

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria del objeto destinada a
   * 			guardar el corpus de semifonemas.
   */

  void Locutor::libera_memoria_corpus_voz() {

    estructura_unidades *correcaminos;

    for (int filas = 0; filas < NUMERO_FONEMAS; filas++)
      for(int columnas = 0; columnas < NUMERO_FONEMAS; columnas++) {
	
	correcaminos = tabla_semifonemas_d[filas][columnas];
	
	if (correcaminos != NULL) {
	  free(correcaminos->unidades);
	  free(correcaminos);
	  tabla_semifonemas_d[filas][columnas] = NULL;
	} // if (correcaminos != NULL)
	
	correcaminos = tabla_semifonemas_i[filas][columnas];
	
	if (correcaminos != NULL) {
	  free(correcaminos->unidades);
	  free(correcaminos);
	  tabla_semifonemas_i[filas][columnas] = NULL;
	} // if (correcaminos != NULL)
	
      } // for (int columnas = 0; ...
  
  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los datos de la
   * base de difonemas. Sólo se emplea con el locutor Freire, por compatibilidad
   * hacia atrás.
   * \return	En ausencia de error, devuelve un cero.
   */
  /*
    int Locutor::lee_base_difonemas() {

    if ( (corpus_difonemas = (Corpus_difonemas *) malloc(sizeof(Corpus_difonemas))) == NULL) {
    fprintf(stderr, "Error en Locutor::lee_base_difonemas, al asignar memoria.\n");
    return 1;
    } // if ( (corpus_difonemas = ...

    if ( (corpus_difonemas->indice_semifonemas = carga_lista_unidades(directorio, &corpus_difonemas->numero_semifonemas)) == NULL)
    return 1;

    if (carga_informacion_difonemas(directorio, &(corpus_difonemas->difonemas)))
    return 1;

    return 0;

    }
  */
  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria reservada para la base de
   * difonemas. Sólo se emplea con el locutor Freire, por compatibilidad hacia
   * atrás.
   */
  /*
    void Locutor::libera_memoria_base_difonemas() {

    if (corpus_difonemas != NULL) {

    free(corpus_difonemas->indice_semifonemas);

    if (corpus_difonemas->difonemas == NULL)
    return;

    // Primero liberamos la memoria de los difonemas.

    estructura_difonemas *correcaminos = corpus_difonemas->difonemas;

    correcaminos->unidades->libera_memoria(); // En Cotovía se está reservando de golpe
    // la memoria dinámica necesaria para todos los difonemas.

    for (int cuenta = 0; cuenta < corpus_difonemas->numero_semifonemas; cuenta++, correcaminos++)
    free(correcaminos->unidades);

    free(corpus_difonemas->difonemas);

    free(corpus_difonemas);

    corpus_difonemas = NULL;

    } // if (corpus_difonemas != NULL)

    }
  */

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto la frecuencia
   * de muestreo de los segmentos de voz. También lee las frecuencias máxima y
   * mínima admitidas como correctas para el locutor.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_frecuencia_muestreo() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;
    float min_f0, max_f0;
    FILE *fichero;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    strcpy(apunta_nombre, FICHERO_FRECUENCIA_MUESTREO);

    if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
      fprintf(stderr, "Error en lee_frecuencia_muestreo, al intentar abrir el \
        fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = fopen

    fscanf(fichero, "%d\n%f\n%f\n%d", &frecuencia_muestreo, &max_f0, &min_f0,
	   &primera_unidad_corpus500);

    min_pitch = (int) (frecuencia_muestreo/max_f0); // en muestras.
    max_pitch = (int) (frecuencia_muestreo/min_f0); // en muestras.
    fclose(fichero);

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los índices de
   * continuidad espectral en las uniones entre semifonemas.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_indices_continuidad_espectral() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;
    FILE *fichero;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    strcpy(apunta_nombre, FICHERO_PESOS_CONCATENACION_COART);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_indices_continuidad_espectral, al \
intentar abrir el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = fopen(...

    fread(indices_continuidad_coart, sizeof(float),
	  NUMERO_ELEMENTOS_CLASE_ESPECTRAL*NUMERO_ELEMENTOS_CLASE_ESPECTRAL, fichero);

    fclose(fichero);

    strcpy(apunta_nombre, FICHERO_PESOS_CONCATENACION_EST);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_indices_continuidad_espectral, al \
intentar abrir el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = fopen(...

    fread(indices_continuidad_est, sizeof(float), NUMERO_ELEMENTOS_CLASE_ESPECTRAL,
	  fichero);

    fclose(fichero);

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los centroides de
   * los semifonemas del Corpus.
   * \return	En ausencia de error, devuelve un cero.
   */

#if defined(_UTILIZA_CENTROIDES) ||					\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

  int Locutor::lee_centroides_semifonemas() {

    char nombre_fichero[FILENAME_MAX];
    FILE *fichero;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\%s", directorio, FICHERO_CENTROIDES);
#else
    sprintf(nombre_fichero, "%s/%s", directorio, FICHERO_CENTROIDES);
#endif

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en lee_centroides_semifonemas, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...

    if ( (centroides_semifonemas_varianza = (Vector_cepstrum_varianza *) malloc(NUMERO_FONEMAS*NUMERO_FONEMAS*2*sizeof(Vector_cepstrum_varianza)))
	 == NULL) {
      fprintf(stderr, "Error en Locutor::lee_centroides_semifonemas, al asignar memoria.\n");
      return 1;
    } // if ( (tabla_centroides = ...


    fread(centroides_semifonemas_varianza, sizeof(Vector_cepstrum_varianza),
	  NUMERO_FONEMAS*NUMERO_FONEMAS*2, fichero);

    fclose(fichero);

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de liberar la memoria del objeto destinada a
   * 			guardar los modelos de duración.
   */

  void Locutor::libera_memoria_centroides_semifonemas() {

#if defined(_UTILIZA_CENTROIDES) ||					\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION) || defined(_UTILIZA_RED_NEURONAL) ||	\
  defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION) ||		\
  defined(_CALCULA_DISTANCIAS_CEPSTRALES)

    if (centroides_semifonemas_varianza) {
      free(centroides_semifonemas_varianza);
      centroides_semifonemas_varianza = NULL;
    } // if (centroides_semifonemas_varianza)

#endif

  }

#endif



  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto los pesos de la
   * función de coste de objetivo, entrenados por medio de regresión lineal.
   * \return	En ausencia de error, devuelve un cero.
   */

#if defined(_UTILIZA_CENTROIDES) || defined(UTILIZA_REGRESION) ||	\
  defined(_UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS) ||			\
  defined(_UTILIZA_REGRESION)

  int Locutor::lee_pesos_funcion_objetivo() {

    char nombre_fichero[FILENAME_MAX];
    FILE *fichero;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\%s", directorio, FICHERO_PESOS_FUNCION_OBJETIVO);
#else
    sprintf(nombre_fichero, "%s/%s", directorio, FICHERO_PESOS_FUNCION_OBJETIVO);
#endif

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_pesos_funcion_objetivo, al intentar \
        abrir el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...

    // El orden se especifica en el proyecto "convierte_fichero_pesos"
    // char orden_grupo[][20] = {"silencio", "vocal", "oclu.son", "oclu.sor", "vibrante",
    //                           "nasal", "lateral", "fric.sor"};

    fread(pesos_regresion, sizeof(Vector_pesos_regresion),
	  NUMERO_ELEMENTOS_CLASE_ESPECTRAL, fichero);

    fclose(fichero);

    return 0;

  }

#endif


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto las redes neuronales
   * que aproximan la parte contextual de la función de coste de objetivo.
   * \return	En ausencia de error, devuelve un cero.
   */

#if defined(_UTILIZA_RED_NEURONAL) || defined(_UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION)

  int Locutor::lee_redes_neuronales_funcion_objetivo() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    // Primero cargamos las redes neuronales:

    // Las inicializamos:

    for (int contador = 0; contador < NUMERO_ELEMENTOS_CLASE_ESPECTRAL; contador++)
      red_contextual[contador].inicia_red();

    strcpy(apunta_nombre, FICHERO_RED_VOCAL_ABIERTA);

    if (red_contextual[VOC_AB_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_VOCAL_MEDIA);

    if (red_contextual[VOC_MED_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_VOCAL_CERRADA);

    if (red_contextual[VOC_CER_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_APROX_SON);

    if (red_contextual[APROX_SON_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_NASAL);

    if (red_contextual[NASAL_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_LATERAL);

    if (red_contextual[LATERAL_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_OCLU_SON);

    if (red_contextual[OCLU_SON_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_OCLU_SOR);

    if (red_contextual[OCLU_SOR_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_FRIC_SOR);

    if (red_contextual[FRIC_SOR_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_SILENCIO);

    if (red_contextual[SILENCIO_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    strcpy(apunta_nombre, FICHERO_RED_VIBRANTE);

    if (red_contextual[VIBRANTE_ESP].lee_fichero_pesos(nombre_fichero))
      return 1;

    // Ahora, los ficheros de medias y desviaciones típicas:

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_VOCAL_ABIERTA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[VOC_AB_ESP].numero_entradas,
				  estadisticos_contextual[VOC_AB_ESP]))
      return 1;


    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_VOCAL_MEDIA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[VOC_MED_ESP].numero_entradas,
				  estadisticos_contextual[VOC_MED_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_VOCAL_CERRADA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[VOC_CER_ESP].numero_entradas,
				  estadisticos_contextual[VOC_CER_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_NASAL);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[NASAL_ESP].numero_entradas,
				  estadisticos_contextual[NASAL_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_LATERAL);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[LATERAL_ESP].numero_entradas,
				  estadisticos_contextual[LATERAL_ESP]))
      return 1;


    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_APROX_SON);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[APROX_SON_ESP].numero_entradas,
				  estadisticos_contextual[APROX_SON_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_OCLU_SON);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[OCLU_SON_ESP].numero_entradas,
				  estadisticos_contextual[OCLU_SON_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_OCLU_SOR);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[OCLU_SOR_ESP].numero_entradas,
				  estadisticos_contextual[OCLU_SOR_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_FRIC_SOR);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[FRIC_SOR_ESP].numero_entradas,
				  estadisticos_contextual[FRIC_SOR_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_SILENCIO);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[SILENCIO_ESP].numero_entradas,
				  estadisticos_contextual[SILENCIO_ESP]))
      return 1;

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_VIBRANTE);

    if (lee_medias_y_desviaciones(nombre_fichero, red_contextual[VIBRANTE_ESP].numero_entradas,
				  estadisticos_contextual[VIBRANTE_ESP]))
      return 1;

    return 0;

  }

#endif


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cargar en la memoria del objeto la red neuronal
   * que modela la continuidad de f0 tras una pausa debida a una coma.
   * \return	En ausencia de error, devuelve un cero.
   */

#ifdef _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA_RED_NEURONAL

  int Locutor::lee_red_neuronal_continuidad_fonica() {


    char nombre_fichero[FILENAME_MAX], *apunta_nombre;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    red_continuidad_fonica.inicia_red();

    strcpy(apunta_nombre, FICHERO_CONTINUIDAD_FONICA);

    if (red_continuidad_fonica.lee_fichero_pesos(nombre_fichero))
      return 1;

    // Ahora, los ficheros de medias y desviaciones típicas:

    strcpy(apunta_nombre, FICHERO_MEDIA_VARIANZA_PARAMETROS_CONTINUIDAD_FONICA);

    if (lee_medias_y_desviaciones(nombre_fichero, red_continuidad_fonica.numero_entradas,
				  estadisticos_continuidad_fonica))
      return 1;

    return 0;

  }

#endif

  /**
   * \author	Fran Campillo
   * \remark	Función encargada de abrir el fichero de sonido en el que se encuentra
   * la información de la forma de onda.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::abre_fichero_sonido() {

    char nombre_fichero[FILENAME_MAX];

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\%s", directorio, FICHERO_SONIDO);
#else
    sprintf(nombre_fichero, "%s/%s", directorio, FICHERO_SONIDO);
#endif

    if ( (fichero_sonido = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en Locutor::abre_fichero_sonido, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_sonido = ...

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark	Función encargada de cerrar el fichero de sonido en el que se encuentra
   * la información de la forma de onda.
   */

  void Locutor::cierra_fichero_sonido() {

    if (fichero_sonido) {
      fclose(fichero_sonido);
      fichero_sonido = NULL;
    } // if (fichero_sonido)

  }

  /**
   * \author	Fran Campillo
   * \remarks	Función encargada de escribir en el fichero que se le pasa como
   * parámetro el contenido de corpus de voz.
   * \remarks	La versión actual ordena según el identificador la base de voz.
   * \param	fichero_corpus: puntero al fichero en el que se van a escribir todas
   * las unidades del corpus.
   * \param	fichero_catalogo: puntero al fichero en el que se escribe el número
   * de instancias de cada semifonema.
   * \param	opcion: si vale 0, se escribe sólo el identificador de las unidades.
   * En otro caso, se escribe la información completa de cada unidad.
   * \return 	En ausencia de error, se devuelve un cero.
   */

  int Locutor::escribe_contenido_corpus_voz(FILE *fichero_corpus, FILE *fichero_catalogo, char opcion) {

    int indice1, indice2, unidades, cuenta;
    char izquierdo_derecho;
    int memoria_reservada = 50000;
    estructura_unidades *grupo_unidades;
    Vector_semifonema_candidato *correcaminos;
    Vector_semifonema_candidato **semifonemas_ordenados, **recorre;

    if ( (tabla_semifonemas_i == NULL) || (tabla_semifonemas_d == NULL) ) {
      fprintf(stderr, "Error en Locutor::escribe_contenido_corpus_voz. Corpus \
no inicializado.\n");
      return 1;
    } // if ( (tabla_semifonemas_i == NULL) || ...

    // Ordenamos la base de voz.

    if (fichero_corpus) {
      if ( (semifonemas_ordenados = (Vector_semifonema_candidato **) malloc(memoria_reservada*sizeof(Vector_semifonema_candidato *)))
	   == NULL) {
	fprintf(stderr, "Error en escribe_contenido_corpus_voz, al asignar memoria.\n");
	return 1;
      }

      recorre = semifonemas_ordenados;

      for (indice1 = 0; indice1 < memoria_reservada; indice1++, recorre++)
	*recorre = NULL;

    }

    for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
      for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
	for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	  if (izquierdo_derecho == '0')
	    grupo_unidades = tabla_semifonemas_i[indice1][indice2];
	  else
	    grupo_unidades = tabla_semifonemas_d[indice1][indice2];

	  if (grupo_unidades == NULL)
	    continue;

	  correcaminos = grupo_unidades->unidades;
	  
	  if (izquierdo_derecho == '0')
	    fprintf(fichero_catalogo, "%s.%s.i\t%d\n", correcaminos->contexto_izquierdo, correcaminos->semifonema, grupo_unidades->numero_unidades);
	  else
	    fprintf(fichero_catalogo, "%s.%s.d\t%d\n", correcaminos->semifonema, correcaminos->contexto_derecho, grupo_unidades->numero_unidades);

	  if (fichero_corpus == NULL) 
	    continue;

	  for (unidades = 0; unidades < grupo_unidades->numero_unidades;
	       unidades++, correcaminos++) {

	    if (2*correcaminos->identificador  >= memoria_reservada) {

	      cuenta = memoria_reservada;
	      memoria_reservada = 2*correcaminos->identificador + 10000;

	      if ( (semifonemas_ordenados = (Vector_semifonema_candidato **) realloc(semifonemas_ordenados,
										     memoria_reservada*sizeof(Vector_semifonema_candidato *)))
		   == NULL) {
		fprintf(stderr, "Error en escribe_contenido_corpus_voz, al reasignar memoria.\n");
		return 1;
	      }

	      recorre = semifonemas_ordenados + cuenta;

	      for (; cuenta < memoria_reservada; cuenta++, recorre++)
		*recorre = NULL;

	    }

	    recorre = semifonemas_ordenados + 2*correcaminos->identificador + correcaminos->izquierdo_derecho - '0';

	    if (*recorre != NULL) {
	      fprintf(stderr, "Error en escribe_contenido_corpus_voz: unidad duplicada.\n");
	      return 1;
	    }

	    *recorre = correcaminos;

	  }


	} // for (izquierdo_derecho = '0'; ...


    if (fichero_corpus) {

      // Escribimos la lista ordenada:

      recorre = semifonemas_ordenados;
    
      for (indice1 = 0; indice1 < memoria_reservada; indice1++, recorre++) {
      
	if (*recorre == NULL)
	  continue;
      
	if (opcion == 0)
	  fprintf(fichero_corpus, "%d\n", (*recorre)->identificador);
	else
	  (*recorre)->escribe_datos_txt(fichero_corpus);
      
      }

      free(semifonemas_ordenados);

    }
    
    return 0;

  }

  /**
   * \author	Fran Campillo
   * \remarks	Función encargada de escribir en el fichero que se le pasa como
   * parámetro el contenido de corpus de voz.
   * \remarks	Similar a la función anterior, pero organiza la base en fonemas
   * \param	fichero_corpus: puntero al fichero en el que se van a escribir todas
   * las unidades del corpus.
   * \param	fichero_catalogo: puntero al fichero en el que se escribe el número
   * de instancias de cada semifonema.
   * \param	opcion: si vale 0, se escribe sólo el identificador de las unidades.
   * En otro caso, se escribe la información completa de cada unidad.
   * \return 	En ausencia de error, se devuelve un cero.
   */

  int Locutor::escribe_contenido_corpus_voz_fonemas(FILE *fichero_corpus, FILE *fichero_catalogo, char opcion) {

    int indice1, indice2, unidades, cuenta;
    char izquierdo_derecho;
    int memoria_reservada = 50000;
    estructura_unidades *grupo_unidades;
    Vector_semifonema_candidato *correcaminos;
    Vector_semifonema_candidato **semifonemas_ordenados, **recorre;

    Vector_descriptor_candidato fonema;

    if ( (tabla_semifonemas_i == NULL) || (tabla_semifonemas_d == NULL) ) {
      fprintf(stderr, "Error en Locutor::escribe_contenido_corpus_voz. Corpus \
no inicializado.\n");
      return 1;
    } // if ( (tabla_semifonemas_i == NULL) || ...

    // Ordenamos la base de voz.

    if (fichero_corpus) {
      if ( (semifonemas_ordenados = (Vector_semifonema_candidato **) malloc(memoria_reservada*sizeof(Vector_semifonema_candidato *)))
	   == NULL) {
	fprintf(stderr, "Error en escribe_contenido_corpus_voz, al asignar memoria.\n");
	return 1;
      }

      recorre = semifonemas_ordenados;

      for (indice1 = 0; indice1 < memoria_reservada; indice1++, recorre++)
	*recorre = NULL;

    }

    for (indice1 = 0; indice1 < NUMERO_FONEMAS; indice1++)
      for (indice2 = 0; indice2 < NUMERO_FONEMAS; indice2++)
	for (izquierdo_derecho = '0'; izquierdo_derecho < '2'; izquierdo_derecho++) {

	  if (izquierdo_derecho == '0')
	    grupo_unidades = tabla_semifonemas_i[indice1][indice2];
	  else
	    grupo_unidades = tabla_semifonemas_d[indice1][indice2];

	  if (grupo_unidades == NULL)
	    continue;

	  correcaminos = grupo_unidades->unidades;
	  
	  if (izquierdo_derecho == '0')
	    fprintf(fichero_catalogo, "%s.%s.i\t%d\n", correcaminos->contexto_izquierdo, correcaminos->semifonema, grupo_unidades->numero_unidades);
	  else
	    fprintf(fichero_catalogo, "%s.%s.d\t%d\n", correcaminos->semifonema, correcaminos->contexto_derecho, grupo_unidades->numero_unidades);

	  if (fichero_corpus == NULL) 
	    continue;

	  for (unidades = 0; unidades < grupo_unidades->numero_unidades;
	       unidades++, correcaminos++) {

	    if (2*correcaminos->identificador  >= memoria_reservada) {

	      cuenta = memoria_reservada;
	      memoria_reservada = 2*correcaminos->identificador + 10000;

	      if ( (semifonemas_ordenados = (Vector_semifonema_candidato **) realloc(semifonemas_ordenados,
										     memoria_reservada*sizeof(Vector_semifonema_candidato *)))
		   == NULL) {
		fprintf(stderr, "Error en escribe_contenido_corpus_voz, al reasignar memoria.\n");
		return 1;
	      }

	      recorre = semifonemas_ordenados + cuenta;

	      for (; cuenta < memoria_reservada; cuenta++, recorre++)
		*recorre = NULL;

	    }

	    recorre = semifonemas_ordenados + 2*correcaminos->identificador + correcaminos->izquierdo_derecho - '0';

	    if (*recorre != NULL) {
	      fprintf(stderr, "Error en escribe_contenido_corpus_voz: unidad duplicada.\n");
	      return 1;
	    }

	    *recorre = correcaminos;

	  }


	} // for (izquierdo_derecho = '0'; ...


    if (fichero_corpus) {

      // Escribimos la lista ordenada:

      recorre = semifonemas_ordenados;
    
      for (indice1 = 0; indice1 < memoria_reservada; indice1 += 2, recorre += 2) {
      
	if (*recorre == NULL)
	  continue;
      
	if ((*recorre)->identificador < PRIMER_IDENTIFICADOR_DIFONEMAS) {

	  if ((*recorre)->identificador != (*(recorre + 1))->identificador) {
	    fprintf(stderr, "Falta una parte del fonema %d.\n", (*recorre)->identificador);
	    return 1;
	  }

	} // if ((*recorre)->identificador...
	else { // La unidad viene de la base de datos de difonemas
	  continue; // Pasamos de ellas para la base de fonemas
	} 

	if (opcion == 0)
	  fprintf(fichero_corpus, "%d\n", (*recorre)->identificador);
	else {
	  fonema.crea_fonema(*recorre, *(recorre + 1));
	  fonema.escribe_datos_txt_simplificado(fichero_corpus);
	}
      
      }

      free(semifonemas_ordenados);

    }
    
    return 0;

  }



  /**
   * \author	Fran Campillo
   * \remark	Función encargada de escribir en el fichero que se le pasa como
   * parámetro el contenido de corpus prosódico.
   * \param	fichero: puntero al fichero en el que se va a escribir la
   * información.
   * \param	opcion: si vale 0, se escribe sólo el identificador de las unidades.
   * En otro caso, se escribe la información completa de cada unidad.
   * \return 	En ausencia de error, se devuelve un cero.
   */

  int Locutor::escribe_contenido_corpus_prosodico(FILE *fichero, char opcion) {

    int posicion, silaba_acentuada, tipo_proposicion;
    int indice, cuenta;
    int memoria_reservada = 50000;
    estructura_grupos_acentuales *unidad_seleccionada;
    Grupo_acentual_candidato *correcaminos;
    Grupo_acentual_candidato **grupos_ordenados, **recorre;

    if (tabla_grupos == NULL) { 
      fprintf(stderr, "Error en Locutor::escribe_contenido_corpus_prosodico. Corpus no inicializado.\n");
      return 1;
    } // if ( (tabla_grupos == NULL)

    // Ordenamos la base prosódica.

    if ( (grupos_ordenados = (Grupo_acentual_candidato **) malloc(memoria_reservada*sizeof(Grupo_acentual_candidato *)))
	 == NULL) {
      fprintf(stderr, "Error en escribe_contenido_corpus_prosodico, al asignar memoria.\n");
      return 1;
    }

    recorre = grupos_ordenados;

    memset(grupos_ordenados, NULL, memoria_reservada*sizeof(Grupo_acentual_candidato *));

    // for (indice = 0; indice < memoria_reservada; indice++, recorre++)
    //   *recorre = NULL;


    for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
      for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
	for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	  //	  printf("Prop = %d, Pos = %d, Sil = %d", tipo_proposicion, posicion, silaba_acentuada);
 
	  indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	  if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	    correcaminos = unidad_seleccionada->grupos;

	    for (int unidades = 0; unidades < unidad_seleccionada->numero_grupos;
		 unidades++, correcaminos++) {

	      if (correcaminos->identificador >= memoria_reservada) {

		cuenta = memoria_reservada;
		memoria_reservada = correcaminos->identificador + 10000;

		if ( (grupos_ordenados = (Grupo_acentual_candidato **) realloc(grupos_ordenados,
									       memoria_reservada*sizeof(Grupo_acentual_candidato *)))
		     == NULL) {
		  fprintf(stderr, "Error en escribe_contenido_corpus_prosodico, al reasignar memoria.\n");
		  return 1;
		}

		memset(grupos_ordenados + cuenta, NULL, (memoria_reservada - cuenta)*sizeof(Grupo_acentual_candidato *));
		//		recorre = grupos_ordenados + cuenta;

		// for (; cuenta < memoria_reservada; cuenta++, recorre++)
		//   *recorre = NULL;

	      }

	      recorre = grupos_ordenados + correcaminos->identificador;

	      if (*recorre != NULL) {
		fprintf(stderr, "Error en escribe_contenido_corpus_prosodico: unidad duplicada.\n");
		return 1;
	      }

	      *recorre = correcaminos;

	    }


	  } // if ( (unidad_seleccionada...


	} // for (posicion = 0;...


    // Escribimos la lista ordenada:

    recorre = grupos_ordenados;

    for (indice = 0; indice < memoria_reservada; indice++, recorre++) {

      if (*recorre == NULL)
	continue;

      if (opcion == 0)
	fprintf(fichero, "%d\n", (*recorre)->identificador);
      else
	(*recorre)->escribe_datos_txt(fichero);

    }


    free(grupos_ordenados);
    
    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark  Función chorra que sólo sirve para comprobar que se carga
   *	correctamente en memoria toda la información de las unidades.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::escribe_contenido_de_tablas() {

    estructura_unidades *correcaminos;
    int indice1, indice2;
    FILE *fichero_salida, *fichero;
    ficheros_unidades *unidades, *recorre;
    int contador;
    Vector_semifonema_candidato *puntero_vector;
    int cuenta_unidades;
    char comienzo_path[100];
    char nombre_fichero[FILENAME_MAX];


    if ( (unidades = (ficheros_unidades *) malloc(numero_unidades_voz*sizeof(ficheros_unidades))) == NULL) {
      fprintf(stderr, "Error en escribe_contenido_de_tablas, al asignar memoria.\n");
      return 1;
    }

    sprintf(nombre_fichero, "%s\\%s", directorio, FICHERO_UNIDADES);

    if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error escribe_contenido_de_tablas, al intentar abrir %s\n.", nombre_fichero);
      return 1;
    }

    cuenta_unidades = numero_unidades_voz;

    fread(unidades, sizeof(ficheros_unidades), cuenta_unidades, fichero);

    fclose(fichero);

    recorre = unidades;

    while (cuenta_unidades--) {

      crea_nombre_fichero(nombre_fichero, recorre->primer_fonema, recorre->segundo_fonema,
			  recorre->izq_der, comienzo_path, ".bin");

      if ( (fichero_salida = fopen(nombre_fichero, "wb")) == NULL) {
	fprintf(stderr, "Error escribe_contenido_de_tablas, al intentar crear el fichero %s.\n", nombre_fichero);
	return 1;
      }

      if ( ( (indice1 = devuelve_indice_fonema(recorre->primer_fonema)) == -1) ||
	   ( (indice2 = devuelve_indice_fonema(recorre->segundo_fonema)) == -1) ) {
	fprintf(stderr, "Error en escribe_contenido_de_tablas: Fichero de unidades no válido.\n");
	return 1;
      }

      recorre->izq_der == IZQUIERDO ? correcaminos = tabla_semifonemas_i[indice1][indice2]:
	correcaminos = tabla_semifonemas_d[indice1][indice2];

      fwrite(correcaminos->puntero_a_unidades, sizeof(estructura_indices), TIPOS_UNIDADES, fichero_salida);
      fwrite(&correcaminos->numero_unidades, sizeof(int), 1, fichero_salida);
      fwrite(correcaminos->unidades, sizeof(Vector_semifonema_candidato), correcaminos->numero_unidades,
	     fichero_salida);

      puntero_vector = correcaminos->unidades;

      for (contador = 0; contador < correcaminos->numero_unidades; contador++) {
	if (puntero_vector->escribe_datos_parte_dinamica(fichero_salida))
	  return 1;
	puntero_vector++;
      }

      fclose(fichero_salida);

      recorre++;
    }

    return 0;

  }


  /**
   * \author	Fran Campillo
   * \remark  Función que escribe en un fichero la información de las unidades
   * indicadas en los parámetros de entrada, para realizar un clustering externo.
   * \param	unidad_1: Parte del semifonema (central o contexto, según el valor
   * de izquierdo_derecho).
   * \param	unidad_2: Parte del semifonema.
   * \param	izquierdo_derecho: IZQUIERDO o DERECHO.
   * \return	En ausencia de error, devuelve un cero.
   */

  int Locutor::escribe_informacion_clustering(char unidad_1[], char unidad_2[],
					      char izquierdo_derecho,
					      FILE *fichero_salida) {

    int indice1, indice2;
    estructura_unidades *grupo_unidades;
    Vector_semifonema_candidato *correcaminos;


    if ( (indice1 = devuelve_indice_fonema(unidad_1)) < 0)
      return 1;

    if ( (indice2 = devuelve_indice_fonema(unidad_2)) < 0)
      return 1;

    if (izquierdo_derecho == '0')
      grupo_unidades = tabla_semifonemas_i[indice1][indice2];
    else
      grupo_unidades = tabla_semifonemas_d[indice1][indice2];

    if (grupo_unidades == NULL) {
      fprintf(stderr, "Error en escribe_informacion_clustering: unidad (%s.%s) no \
        disponible en el Corpus.\n", unidad_1, unidad_2);
      return 1;
    } // if (grupo_unidades == NULL)

    correcaminos = grupo_unidades->unidades;

    for (int contador = 0; contador < grupo_unidades->numero_unidades; contador++, correcaminos++)
      correcaminos->escribe_datos_clustering(fichero_salida);

    return 0;

  }


#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL

#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA

  /**
   * \brief Función que carga el fichero con los contextos gramaticales.
   * \author Fran Campillo.
   * \remarks El fichero con los contextos se crea con el script en Perl
   * crea_coste_morf_sin.pl
   * \return En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_fichero_contextos_gramaticales() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;
    char linea[1000];
    char acentual_cand[100], acentual_obj[100], acentual_siguiente_cand[100],
      acentual_siguiente_obj[100], pausa[100];
    int numero_elementos, contador;
    unsigned char val_cand, val_siguiente_cand, val_obj, val_siguiente_obj,
      val_pausa;
    int clave;
    float coste;

    FILE *fichero;


#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    strcpy(apunta_nombre, FICHERO_CONTEXTOS_GRAMATICALES);

    if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_contextos_gramaticales, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...


    // Primero leemos los contextos relativos al salto de f0:

    fgets(linea, 1000, fichero);

    sscanf(linea, "%s\t%d", acentual_cand, &numero_elementos);

    for (contador = 0; contador < numero_elementos; contador++) {

      fgets(linea, 1000, fichero);

      sscanf(linea, "%s\t%s\t%s\t%s\t%s\t%f", acentual_cand,
	     acentual_siguiente_cand, acentual_obj, acentual_siguiente_obj,
	     pausa, &coste);

      if (lee_tipo_categoria_morfosintactica_grupo(acentual_cand, &val_cand))
	return 1;
      if (lee_tipo_sintagma_grupo(acentual_siguiente_cand,
				  &val_siguiente_cand))
	return 1;
      if (lee_tipo_categoria_morfosintactica_grupo(acentual_obj, &val_obj))
	return 1;
      if (lee_tipo_sintagma_grupo(acentual_siguiente_obj,
				  &val_siguiente_obj))
	return 1;
      if (lee_tipo_pausa_grupo_acentual(pausa, &val_pausa))
	return 1;

      clave = crea_clave_hash_contextos(val_cand, val_siguiente_cand,
					val_obj, val_siguiente_obj,
					val_pausa);

      contexto_gramatical_salto_f0[clave] = coste;

    }

    // Después, los contextos relativos al factor de caída:

    //  contexto_gramatical_factor_caida.clear();

    fgets(linea, 1000, fichero);

    sscanf(linea, "%s\t%d", acentual_cand, &numero_elementos);

    for (contador = 0; contador < numero_elementos; contador++) {

      fgets(linea, 1000, fichero);

      sscanf(linea, "%s\t%s\t%s\t%s\t%s\t%f", acentual_cand,
	     acentual_siguiente_cand, acentual_obj, acentual_siguiente_obj,
	     pausa, &coste);

      if (lee_tipo_categoria_morfosintactica_grupo(acentual_cand, &val_cand))
	return 1;
      if (lee_tipo_sintagma_grupo(acentual_siguiente_cand,
				  &val_siguiente_cand))
	return 1;
      if (lee_tipo_categoria_morfosintactica_grupo(acentual_obj, &val_obj))
	return 1;
      if (lee_tipo_sintagma_grupo(acentual_siguiente_obj,
				  &val_siguiente_obj))
	return 1;
      if (lee_tipo_pausa_grupo_acentual(pausa, &val_pausa))
	return 1;

      clave = crea_clave_hash_contextos(val_cand, val_siguiente_cand,
					val_obj, val_siguiente_obj,
					val_pausa);

      contexto_gramatical_factor_caida[clave] = coste;

    }



    fclose(fichero);

    return 0;

  }


  /**
   * \brief Función que carga el fichero con las probabilidades de inserción
   *  de rupturas entonativas entre dos sintagmas.
   * \author Fran Campillo.
   * \remarks El fichero con los contextos se crea con el script en Perl
   * crea_datos_insercion_rupturas.pl
   * \return En ausencia de error, devuelve un cero.
   */

  int Locutor::lee_fichero_insercion_rupturas() {

    char nombre_fichero[FILENAME_MAX], *apunta_nombre;
    char linea[1000];
    char acentual_cand[100], acentual_siguiente_cand[100];
    int numero_elementos, contador;
    unsigned char val_cand, val_siguiente_cand;
    int clave;
    float coste;

    FILE *fichero;

#ifdef _WIN32
    sprintf(nombre_fichero, "%s\\", directorio);
#else
    sprintf(nombre_fichero, "%s/", directorio);
#endif

    apunta_nombre = (char *) nombre_fichero + strlen(nombre_fichero);

    strcpy(apunta_nombre, FICHERO_INSERCION_R_E);

    if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
      fprintf(stderr, "Error en Locutor::lee_fichero_insercion_rupturas, al intentar abrir \
        el fichero %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero = ...



    fgets(linea, 1000, fichero);

    sscanf(linea, "%d", &numero_elementos);

    for (contador = 0; contador < numero_elementos; contador++) {

      fgets(linea, 1000, fichero);

      sscanf(linea, "%s\t%s\t%f", acentual_cand,
	     acentual_siguiente_cand, &coste);

      if (lee_tipo_sintagma_grupo(acentual_cand, &val_cand))
	return 1;
      if (lee_tipo_sintagma_grupo(acentual_siguiente_cand,
				  &val_siguiente_cand))
	return 1;

      clave = crea_clave_hash_inserciones(val_cand, val_siguiente_cand);

      probabilidad_insercion_rupturas[clave] = coste;

    }

    fclose(fichero);

    return 0;

  }

#endif

#endif

