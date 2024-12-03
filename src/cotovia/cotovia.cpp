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

#ifdef _WIN32
#include <windows.h>
#endif
#include "modos.hpp"
#include "tip_var.hpp"
#include "errores.hpp"
#include "minor_phrasing.hpp"
#include "leer_frase.hpp"
#include "sep_pal.hpp"
#include "clas_pal.hpp"
#include "preproc.hpp"
#include "sil_acen.hpp"
#include "transcripcion.hpp"
#include "alofonos.hpp"
#include "crea_descriptores.hpp"
#include "estadistica.hpp"
#include "frecuencia.hpp"
#include "audio.hpp"
#include "procesado_senhal.hpp"
#include "modelo_duracion.hpp"
#include "morfolo.hpp"
#include "categorias_analisis_morfosintactico.hpp"
#include "perfhash.hpp"
#include "modelo_lenguaje.hpp"
#include "matriz.hpp"
#include "path_list.hpp"
#include "Viterbi_categorias.hpp"
#include "analisis_morfosintactico.hpp"
#include "sintagma.hpp"
#include "pausas.hpp"
#include "trat_fon.hpp"
#include "prosodia.hpp"
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
#include "locutor.hpp"
#include "Viterbi_acentual.hpp"
#include "Viterbi.hpp"
#include "info_estructuras.hpp"
#include "seleccion_unidades.hpp"
#include "gbm_locuciones.hpp"
#include "configuracion.hpp"
#include "rupturas_entonativas.hpp"
#include "modulo_minor_phrasing.hpp"
#include "cotovia.hpp"


//declaración de las variables estáticas de la clase
map<string, Lengua> Cotovia::lenguas;
map<unsigned char, char *> Cotovia::lang;
Locutores_ya_cargados *Cotovia::locutores_ya_cargados = NULL;
Locutor ** Cotovia::locutor = NULL;
int Cotovia::locutores_cargados = 0;


Cotovia::Cotovia() {
  
  locutor_actual = NULL;
  acentuales_obj = NULL;

}

Cotovia::~Cotovia() {

  if (acentuales_obj) {
    Grupo_acentual_objetivo *apunta_obj = acentuales_obj;
    for (int i = 0; i < numero_acentuales; i++, apunta_obj++)
      apunta_obj->libera_memoria();

    free(acentuales_obj);

  }

  for (vector<Vector_descriptor_objetivo>::iterator it = cadena_unidades.begin(); it != cadena_unidades.end(); it++) {
    it->libera_memoria();
  }

}

/**
 * \author	Fran Campillo
 * \remarks	Función encargada de devolver la posición en la cadena de locutores
 * cargados del locutor cuyo nombre se indica como parámetro.
 * \param	nombre_locutor: nombre del locutor.
 * \retval	posicion: índice dentro de la cadena.
 * \return	En ausencia de error devuelve un cero.
 */

int Cotovia::devuelve_posicion_locutor(char *nombre_locutor, int *posicion) {

  int contador = 0;
  char *apunta_nombres;
  Locutores_ya_cargados *apunta_locutores = locutores_ya_cargados;
  char nombre_local[TAMANO_MAXIMO_CADENA_LOCUTORES];

  if (locutores_ya_cargados == NULL) {
    fprintf(stderr, "Error en devuelve_posicion_locutor: ningún locutor cargado.\n");
    return 1;
  } // if (locutores_ya_cargados == NULL)

  strcpy(nombre_local, locutores_empleados);

  apunta_nombres = strtok(nombre_local, " ");

  while (apunta_nombres != NULL) {

    if (strcmp(apunta_nombres, nombre_locutor) == 0)
      break;

    //        apunta_nombres++;
    apunta_nombres = strtok(NULL, " ");

  } // while ...

  if (apunta_nombres == NULL) {
    fprintf(stderr, "Error en devuelve_posicion_locutor: locutor no cargado.\n");
    return 1;
  } // if (apunta_nombres == NULL)

  for (; contador < locutores_cargados; contador++, apunta_locutores++) {

    if (strcmp(apunta_nombres, apunta_locutores->nombre_locutor) == 0)
      break;

  } // for (; contador ...

  if (contador == locutores_cargados) {

    fprintf(stderr, "Error en devuelve_posicion_locutor: locutor no cargado.\n");
    return 1;

  } // if (contador == locutores_cargados)

  *posicion = contador;

  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función encargada de añadir un locutor a la cadena de locutores
 * cargados. Si el locutor ya está cargado en memoria, se incrementa su
 * contador de usuarios. En caso contrario, se añade a la cadena.
 * \param	nombre_locutor: nombre del locutor.
 * \retval	carga_locutor: true, si hay que cargar en memoria el locutor. false
 * en caso contrario.
 * \return	En ausencia de error devuelve un cero.
 */

int Cotovia::anhade_locutor(char *nombre_locutor, bool *carga_locutor) {

  int contador = 0;
  Locutores_ya_cargados *correcaminos;

  if (locutores_ya_cargados == NULL) {
    *carga_locutor = true;
    if ( (locutores_ya_cargados = (Locutores_ya_cargados *)
	  malloc(sizeof(Locutores_ya_cargados))) == NULL) {
      fprintf(stderr, "Error en anhade_locutor, al asignar memoria.\n");
      return 1;
    }// if (locutores_ya_cargados = (Locutores_ya_cargados *) ...

    locutores_cargados++;
    strcpy(locutores_ya_cargados->nombre_locutor, nombre_locutor);
    locutores_ya_cargados->numero_usuarios = 1;
  } // if (locutores_ya_cargados == NULL)
  else {

    correcaminos = locutores_ya_cargados;

    do {
      if (strcmp(correcaminos->nombre_locutor, nombre_locutor) == 0) {
	correcaminos->numero_usuarios++;
	*carga_locutor = false;
	return 0;
      } // if (strcmp...
      correcaminos++;
    } while (++contador < locutores_cargados);

    // Es un nuevo locutor: se añade a la cadena:

    *carga_locutor = true;
    locutores_cargados++;

    if ( (locutores_ya_cargados = (Locutores_ya_cargados *) realloc(locutores_ya_cargados,
								    locutores_cargados*sizeof(Locutores_ya_cargados))) == NULL) {
      fprintf(stderr, "Error en anhade_locutor, al reasignar memoria.\n");
      return 1;
    } // if ( (locutores_ya_cargados = ...

    correcaminos = locutores_ya_cargados + locutores_cargados - 1;
    strcpy(correcaminos->nombre_locutor, nombre_locutor);
    correcaminos->numero_usuarios = 1;
  } // else

  return 0;
}


/**
 * \author	Fran Campillo
 * \remarks	Función encargada de liberar un usuario de la cadena de locutores
 * cargados. Si era el único usuario, avisa para que se libere la memoria
 * reservada para dicho locutor.
 * \param	nombre_locutor: nombre del locutor.
 * \retval	libera_memoria: true, si se puede liberar la memoria asignada a
 * dicho locutor. false, en caso contrario.
 * \retval	posicion: índice dentro de la cadena de locutores cargados.
 * \return	En ausencia de error devuelve un cero.
 */

int Cotovia::libera_locutor(char *nombre_locutor, bool *libera_memoria, int *posicion) {

  int contador = 0;

  Locutores_ya_cargados *recorre, *correcaminos = locutores_ya_cargados;

  // Buscamos el locutor:

  do {

    if (strcmp(correcaminos->nombre_locutor, nombre_locutor) == 0) {

      if (--correcaminos->numero_usuarios == 0) {
	*libera_memoria = 1;
	*posicion = correcaminos - locutores_ya_cargados;
	if (--locutores_cargados == 0) {
	  free(locutores_ya_cargados);
	  locutores_ya_cargados = NULL;
	} // if (--locutores_cargados == 0)...
	else {
	  recorre = correcaminos;
	  for (int cuenta = contador; cuenta < locutores_cargados;
	       cuenta++, recorre++)
	    *recorre = *(recorre + 1);

	  if ( (locutores_ya_cargados = (Locutores_ya_cargados *)
		realloc(locutores_ya_cargados,
			locutores_cargados*sizeof(Locutores_ya_cargados)))
	       == NULL) {
	    fprintf(stderr, "Error en libera_locutor, al reasignar memoria.\n");
	    return 1;
	  } // if ( (locutores_ya_cargados = ...

	} // else

	return 0;

      } // if (--correcaminos->numero_usuarios == 0)
      else {
	*libera_memoria = 0;
	return 0;
      } // else
    } // if (strcmp...

    correcaminos++;

  } while (++contador < locutores_cargados);

  fprintf(stderr, "Error en libera_memoria: locutor (%s) no cargado.\n", nombre_locutor);

  return 1;

}


#if defined(_ESTADISTICA) ||					\
  defined(_GENERA_CORPUS) || defined(_CORPUS_PROSODICO) ||	\
  defined(_LEE_ENTONACION_DE_FICHERO)  ||			\
  defined(_SECUENCIAS_FONETICAS)

int Cotovia::cotovia(int argc, char **argv, char * entrada, t_opciones opc) 

#elif defined(_CALCULA_INDICES)

  int Cotovia::cotovia(char *entrada, char *salida, t_opciones opc, float duracion, float frecuencia) 

#elif defined(_CALCULA_PENALIZACIONES_OPTIMAS)

  int Cotovia::cotovia(char *entrada, t_opciones opc, float pen_4_gramas, float pen_3_gramas,
		       float pen_2_gramas) 

#elif defined(_CALCULA_FACTORES)

  int Cotovia::cotovia(char *entrada, char *salida_f0, char *salida_dur, char *salida_rafaga,
		       char *salida_concatena, char *salida_fonema_coart, char *salida_contextual,
		       char *salida_func_concatena, char *salida_ent_dur,
		       char *salida_pos_acentual, char *salida_pos_fonico,
		       char *salida_conc_acentual, char *salida_diferencias_prosodicas,
		       t_opciones opc) 

#else
 
  int Cotovia::cotovia(char * entrada, t_opciones opc)
 

#endif
{ 

  opciones=opc;
  bool ultima_frase = false;

  f_analisis = NULL;

  int indice_frase = 1;

#ifdef _MODO_NORMAL

  char nombre_fichero_salida[FILENAME_MAX];
  FILE *fichero_errores;
  //Vector_descriptor_objetivo *cadena_unidades = NULL; // fmendez ahora es una variable de clase
  cadena_unidades.clear();
  //int numero_unidades; // fmendez ahora es una variable de clase
  Vector_sintesis *unidades_escogidas = NULL;
  int numero_unidades_sintesis;
  vector<Frase_frontera> frase_frontera_cand;
  float *costes_caminos_acentuales = NULL;
  int numero_frases_candidatas;
  //   int cuenta_contorno= 1;
  //Grupo_acentual_objetivo *acentuales_obj = NULL;  // fmendez ahora es una variable de clase
  //int numero_acentuales; // fmendez ahora es una variable de clase
  acentuales_obj = NULL;
  bool un_fichero = false;

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES
  FILE *fichero_unidades;
  int indice_menor, indice_mayor;

#endif

#ifdef _FUERZA_CONTORNOS_ORIGINALES
  int cuenta_frase = 1;
#endif

#endif // ifdef _MODO_NORMAL

#ifdef _CALCULA_INDICES
  FILE *fichero_salida;

  if ( (fichero_salida = fopen(salida, "wt")) == NULL) {
    printf("Error al intentar abrir el fichero %s.\n", salida);
    return 1;
  } // if ( (fichero_salida = ...

  //    fprintf(fichero_salida, "Frase\tNumUnidades\tDur\tFrec\tDurFrec\n");
  especifica_umbrales(duracion / 1000, frecuencia);

#endif

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

  analisis_morfosintactico.modelo_lenguaje.establece_penalizaciones_n_gramas(pen_4_gramas, pen_3_gramas, pen_2_gramas);

#endif

#ifdef _CALCULA_FACTORES
  FILE *ficheros[12];//_f0, *fichero_dur, *fichero_rafaga, *fichero_concatena,
  //    		*fichero_fonema_coart, *fichero_contextual, *fichero_func_concatena,
  //			*fichero_ent_dur, *fichero_pos_acentual, *fichero_pos_fonico,
  //			*fichero_conc_acentual, *fichero_diferencias_prosodicas;
  //    int indice_frase = 0;

  opciones.info_unidades = 2;

  if ( (ficheros[0] = fopen(salida_f0, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_f0);
    return 1;
  } // if ( (fichero_f0

  if ( (ficheros[1] = fopen(salida_dur, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_dur);
    return 1;
  } // if ( (fichero_f0

  if ( (ficheros[2] = fopen(salida_rafaga, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_rafaga);
    return 1;
  } // if ( (fichero_f0

  if ( (ficheros[3] = fopen(salida_concatena, "wt")) == NULL) {
   fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_concatena);
    return 1;
  } // if ( (fichero_concatena

  if ( (ficheros[4] = fopen(salida_fonema_coart, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_fonema_coart);
    return 1;
  } // if ( (fichero_fonema_coart

  if ( (ficheros[5] = fopen(salida_contextual, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_contextual);
    return 1;
  } // if ( (fichero_contextual

  if ( (ficheros[6] = fopen(salida_func_concatena, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_func_concatena);
    return 1;
  } // if ( (fichero_contextual

  if ( (ficheros[7] = fopen(salida_ent_dur, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_ent_dur);
    return 1;
  }

  if ( (ficheros[8] = fopen(salida_pos_acentual, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_pos_acentual);
    return 1;
  }

  if ( (ficheros[9] = fopen(salida_pos_fonico, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_pos_fonico);
    return 1;
  }

  if ( (ficheros[10] = fopen(salida_conc_acentual, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_conc_acentual);
    return 1;
  } // if ( (ficheros[10] = ...

  if ( (ficheros[11] = fopen(salida_diferencias_prosodicas, "wt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", salida_diferencias_prosodicas);
    return 1;
  } // if ( (ficheros[11] = ...

#endif

#ifdef _GRUPOS_ACENTUALES
  char nombre_fichero_salida[FILENAME_MAX];
  char nombre_fichero_GF[FILENAME_MAX];
  char nombre_fichero_origen_configuracion[FILENAME_MAX];
  char auxiliar_correccion[200];
  int correccion_manual;
  FILE *fichero_errores;
  Grupo_acentual_candidato *acentuales = NULL;
  int numero_acentuales = 0;
  Frase_frontera frase_frontera;
  char nombre_fonemas[FILENAME_MAX], nombre_silabas[FILENAME_MAX], nombre_contornos[FILENAME_MAX], nombre_rupturas[FILENAME_MAX];

  FILE *fichero_fonemas = NULL;
  FILE *fichero_silabas = NULL;
  FILE *fichero_origen = NULL;
  FILE *fichero_origen_GF = NULL;
  FILE *fichero_grupos = NULL;
  FILE *fichero_rupturas = NULL;
    
  char nombre_salida[200];

  Frecuencia_estilizada frecuencia_estilizada;

  Configuracion configuracion;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_parametro((char *) "correccion_manual", auxiliar_correccion))
    return 1;

  (strcmp(auxiliar_correccion, "0")) ? correccion_manual = 1 :
    correccion_manual = 0;

  if (configuracion.devuelve_parametro((char *) "locutor", opciones.locutores))
    return 1;

  if (configuracion.devuelve_parametro((char *) "fichero_temporal_grupos", nombre_salida))
    return 1;

  if (configuracion.devuelve_parametro((char *) "fichero_origen_acentuales", nombre_fichero_origen_configuracion))
    return 1;


  if ( (fichero_grupos = fopen(nombre_salida, "wb")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de salida %s.", nombre_salida);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "morfologico", opciones.fentrada, opciones.fich_morfosintactico))
    return 1;

  if (configuracion.devuelve_direccion((char *) "contornos", opciones.fentrada, nombre_contornos))
    return 1;

  if (configuracion.devuelve_direccion((char *) "minor_phrasing", opciones.fentrada, nombre_rupturas))
    return 1;

  if ( (fichero_rupturas = fopen(nombre_rupturas, "wb")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de rupturas %s.\n", nombre_rupturas);
    return 1;
  }

  if ( (fichero_fonemas = fopen(nombre_contornos, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de frecuencia estilizada %s.\n", nombre_contornos);
    return 1;
  }

  if (carga_fichero_frecuencia_estilizada(fichero_fonemas, &frecuencia_estilizada))
    return 1;

  fclose(fichero_fonemas);

  if (configuracion.devuelve_direccion((char *) "silabas", opciones.fentrada, nombre_silabas))
    return 1;

  if ( (fichero_silabas = fopen(nombre_silabas, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de sílabas %s.\n", nombre_silabas);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "fonemas", opciones.fentrada, nombre_fonemas))
    return 1;

  if ( (fichero_fonemas = fopen(nombre_fonemas, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero %s.\n", nombre_fonemas);
    return 1;
  }


#endif  // ifdef _GRUPOS_ACENTUALES

#if defined(_ESTADISTICA) || defined(_GENERA_CORPUS)
  FILE *fichero_fonema_est, *fichero_semi_izquierdo, *fichero_semi_derecho, *fichero_trifonema;
  Estadistica fonema_est;
  Estadistica semifonema_izquierdo_est;
  Estadistica semifonema_derecho_est;
  Estadistica trifonema_est;
#endif // if defined(_ESTADISTICA) || defined...

#ifdef _GENERA_CORPUS
  FILE *fichero_corpus, *fichero_rechazo;
  FILE *fichero_objetivo;
  Estadistica lista_unidades;
#endif // ifdef _GENERA_CORPUS



#if defined(_ESTADISTICA) || defined(_GENERA_CORPUS)

  if ( (fichero_fonema_est = fopen("fonema.bin", "rb")) != NULL) {
    fonema_est.lee_tabla_de_fichero_bin(fichero_fonema_est);
    fclose(fichero_fonema_est);
  }

  if ( (fichero_semi_izquierdo = fopen("semifon_izdo.bin", "rb")) != NULL) {
    semifonema_izquierdo_est.lee_tabla_de_fichero_bin(fichero_semi_izquierdo);
    fclose(fichero_semi_izquierdo);
  }

  if ( (fichero_semi_derecho = fopen("semifon_dcho.bin", "rb")) != NULL) {
    semifonema_derecho_est.lee_tabla_de_fichero_bin(fichero_semi_derecho);
    fclose(fichero_semi_derecho);
  }

  if ( (fichero_trifonema = fopen("trifonema.bin", "rb")) != NULL) {
    trifonema_est.lee_tabla_de_fichero_bin(fichero_trifonema);
    fclose(fichero_trifonema);
  }

#ifdef _GENERA_CORPUS

  if ( (fichero_objetivo = fopen("enun_tonica.txt", "r")) == NULL) {
    puts("Error al abrir el fichero.");
    return 1;;
  }

  lista_unidades.lee_tabla_de_fichero_corpus(fichero_objetivo);

  fclose(fichero_objetivo);

  if ( (fichero_objetivo = fopen("enun_notonica.txt", "r")) == NULL) {
    puts("Error al abrir el fichero.");
    return 1;
  }

  lista_unidades.lee_tabla_de_fichero_corpus(fichero_objetivo);

  fclose(fichero_objetivo);

  if ( (fichero_objetivo = fopen("enun_notonica_cont.txt", "r")) == NULL) {
    puts("Error al abrir el fichero.");
    return 1;
  }

  lista_unidades.lee_tabla_de_fichero_corpus(fichero_objetivo);

  fclose(fichero_objetivo);

  if ( ((fichero_corpus = fopen("fichero_corpus.txt", "w")) == NULL) ||
       ((fichero_rechazo = fopen("fichero_rechazo.txt", "w")) == NULL) ) {
    puts("Error al crear los ficheros del corpus.");
    return 1;
  }

#endif

#endif

#ifdef _CORPUS_PROSODICO
  FILE *fichero_corpus, *fichero_rechazo;
  Estructura_prosodica *corpus_prosodico;
  int numero_estructuras;
  unsigned char corpus_incompleto = 1;
#endif // ifdef _CORPUS_PROSODICO

#ifdef _SECUENCIAS_FONETICAS
  FILE *fichero_corpus, *fichero_rechazo;
  Estructura_fonema_estacionario *fonemas_estacionario;
  int numero_fonemas_estacionario;
  Estructura_fonema_transicion *fonemas_transicion;
  int numero_fonemas_transicion;
  int minimo_numero_palabras;
  int maximo_numero_palabras;
  int porcentaje_secuencias;
#endif // ifdef _SECUENCIAS_FONETICAS

#ifdef _CALCULA_DATOS_MODELO_ENERGIA

  char nombre_fichero_salida[FILENAME_MAX];
  char nombre_sonidos[FILENAME_MAX], nombre_fonemas[FILENAME_MAX], nombre_marcas[FILENAME_MAX], nombre_sfs[FILENAME_MAX];
  char cadena_frecuencia[100];
  char auxiliar_correccion[200];
  int correccion_manual;
  Parametros_modelo_energia *cadena_datos_energia;
  int numero_elementos_energia;
  int frecuencia_muestreo;
  FILE *fichero_fonemas, *fichero_marcas, *fichero_sonido, *fichero_sal;
  Energia objeto_energia;
  Configuracion configuracion;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_parametro("correccion_manual", auxiliar_correccion))
    return 1;

  (strcmp(auxiliar_correccion, "0")) ? correccion_manual = 1 :
    correccion_manual = 0;

  if (configuracion.devuelve_parametro("locutor", opciones.locutores))
    return 1;

  if (configuracion.devuelve_direccion("morfologico", opciones.fentrada, opciones.fich_morfosintactico))
    return 1;

  if (configuracion.devuelve_direccion("sfs", opciones.fentrada, nombre_sfs))
    return 1;

  if (configuracion.devuelve_direccion("fonemas", opciones.fentrada, nombre_fonemas))
    return 1;

  if ( (fichero_fonemas = fopen(nombre_fonemas, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de fonemas %s.\n", nombre_fonemas);
    return 1;
  } // if ( (fichero_fonemas = ...

  if (configuracion.devuelve_direccion("marcas_pitch", opciones.fentrada, nombre_marcas))
    return 1;

  if ( (fichero_marcas = fopen(nombre_marcas, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de marcas de pitch %s.\n", nombre_fonemas);
    return 1;
  } // if ( (fichero_marcas = ...

  if (configuracion.devuelve_direccion("sonido", opciones.fentrada, nombre_sonidos))
    return 1;

  if ( (fichero_sonido = fopen(nombre_sonidos, "rb")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de sonido %s.\n", nombre_sonidos);
    return 1;
  } // if ( (fichero_sonido = ...

  if (configuracion.devuelve_parametro("frecuencia_muestreo", cadena_frecuencia))
    return 1;

  frecuencia_muestreo = atoi(cadena_frecuencia);

  sprintf(nombre_fichero_salida, "datos_energia_%s.txt", opciones.locutores);

  if ( (fichero_sal = fopen(nombre_fichero_salida, "rt")) == NULL) {
    // No existe, escribimos la cabecera:
    if ( (fichero_sal = fopen(nombre_fichero_salida, "wt")) == NULL) {
      fprintf(stderr, "Error al intentar abrir el fichero de salida de datos de energía.\n");
      return 1;
    } // if ( (fichero_sal = ...
#ifdef _ESTIMA_ENERGIA_RED_NEURONAL
    escribe_parametros_modelo_energia(fichero_sal, 0);
#else
    escribe_parametros_modelo_energia(fichero_sal, 0);
#endif
  } // if ( (fichero_sal = ...
  else { // Lo abrimos para añadir datos.
    fclose(fichero_sal);   
    if ( (fichero_sal = fopen(nombre_fichero_salida, "at")) == NULL) {
      fprintf(stderr, "Error al intentar abrir el fichero de salida de datos de energía.\n");
      return 1;
    } // if ( (fichero_sal = ...
  }
#endif // ifdef _CALCULA_DATOS_MODELO_ENERGIA



#ifdef _SCRIPT
  FILE *fichero_fonetico;
  char nombre_fichero_fonetico[FILENAME_MAX];

  FILE *fichero_fonemas = NULL, *fichero_marcas = NULL, *fichero_sonido = NULL, *fichero_sal = NULL,
    *fichero_cepstrum = NULL, *fichero_cepstrum_bin = NULL;
  char nombre_fonemas[FILENAME_MAX], nombre_marcas[FILENAME_MAX], nombre_sonido[FILENAME_MAX],
    nombre_cepstrum[FILENAME_MAX], nombre_cepstrum_bin[FILENAME_MAX], nombre_sfs[FILENAME_MAX],
    nombre_salida[FILENAME_MAX], nombre_txt_morf[FILENAME_MAX];
  char nombre_fichero_origen_configuracion[FILENAME_MAX];

  char cadena_frecuencia[100], cadena_f0_min[100], cadena_f0_max[100];

  float frecuencia_maxima_locutor, frecuencia_minima_locutor;
  int frecuencia_muestreo;

  char auxiliar_correccion[200];
  int correccion_manual;

  Configuracion configuracion;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_parametro((char *) "correccion_manual", auxiliar_correccion))
    return 1;

  (strcmp(auxiliar_correccion, "0")) ? correccion_manual = 1 :
    correccion_manual = 0;

  if (configuracion.devuelve_parametro((char *) "coeficientes_cepstrales", auxiliar_correccion))
    return 1;

  Vector_descriptor_candidato::numero_coeficientes_cepstrales = atoi(auxiliar_correccion);

  Vector_semifonema_candidato::numero_coeficientes_cepstrales = Vector_descriptor_candidato::numero_coeficientes_cepstrales;


  if (configuracion.devuelve_direccion((char *) "morfologico", opciones.fentrada, opciones.fich_morfosintactico))
    return 1;

  if (configuracion.devuelve_parametro((char *) "locutor", opciones.locutores))
    return 1;

  if (configuracion.devuelve_parametro((char *) "fichero_origen_unidades", nombre_fichero_origen_configuracion))
    return 1;

  if (configuracion.devuelve_direccion((char *) "transcripcion", opciones.fentrada, nombre_fichero_fonetico))
    return 1;

  if ( (fichero_fonetico = fopen(nombre_fichero_fonetico, "wt")) == NULL) {
    fprintf(stderr, "Error en Cotovía, intentar crear el fichero de transcripción fonética %s\n",nombre_fichero_fonetico );
    return 1;
  }

  if (configuracion.devuelve_parametro((char *) "fichero_temporal_voz", nombre_salida))
    return 1;

  if ( (fichero_sal = fopen(nombre_salida, "wb")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de descriptores.\n");
    return 1;
  }

  Vector_descriptor_candidato::escribe_cabecera(fichero_sal);

  if (configuracion.devuelve_direccion((char *) "sfs", opciones.fentrada, nombre_sfs))
    return 1;

  if (configuracion.devuelve_direccion((char *) "marcas_pitch", opciones.fentrada, nombre_marcas))
    return 1;

  if ( (fichero_marcas = fopen(nombre_marcas, "r")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de marcas %s.\n", nombre_marcas);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "fonemas", opciones.fentrada, nombre_fonemas))
    return 1;

  if ( (fichero_fonemas = fopen(nombre_fonemas, "r")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de fonemas %s.\n", nombre_fonemas);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "txt_morf", opciones.fentrada, nombre_txt_morf))
    return 1;

  if (configuracion.devuelve_direccion((char *) "sonido", opciones.fentrada, nombre_sonido))
    return 1;

  if ( (fichero_sonido = fopen(nombre_sonido, "rb")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de sonidos %s.\n", nombre_sonido);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "cepstrum_pm", opciones.fentrada, nombre_cepstrum))
    return 1;

  if ( (fichero_cepstrum = fopen(nombre_cepstrum, "rb")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de coeficientes cepstrales %s.\n", nombre_cepstrum);
    return 1;
  }

  if (configuracion.devuelve_direccion((char *) "cepstrum", opciones.fentrada, nombre_cepstrum_bin))
    return 1;

  if ( (fichero_cepstrum_bin = fopen(nombre_cepstrum_bin, "rb")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de coeficientes cepstrales bin %s.\n", nombre_cepstrum_bin);
    return 1;
  }

  if (configuracion.devuelve_parametro((char *) "frecuencia_muestreo", cadena_frecuencia))
    return 1;

  frecuencia_muestreo = atoi(cadena_frecuencia);

  Vector_descriptor_candidato::frecuencia_muestreo = frecuencia_muestreo;

  if (configuracion.devuelve_parametro((char *) "f0_minima", cadena_f0_min))
    return 1;

  frecuencia_minima_locutor = atoi(cadena_f0_min);

  if (configuracion.devuelve_parametro((char *) "f0_maxima", cadena_f0_max))
    return 1;

  frecuencia_maxima_locutor = atoi(cadena_f0_max);

#endif

#ifdef _SCRIPT_DURACION
  Parametros_duracion_entrenamiento *cadena_duracion = NULL;
  int numero_elementos_duracion = 0;
  int correccion_manual;
  char auxiliar_correccion[200];
  char nombre_fonemas[FILENAME_MAX];

  FILE *fichero_fonemas = NULL, *fichero_sal = NULL;

  Configuracion configuracion;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_parametro((char *) "correccion_manual", auxiliar_correccion))
    return 1;

  (strcmp(auxiliar_correccion, "0")) ? correccion_manual = 1 :
    correccion_manual = 0;

  if (configuracion.devuelve_parametro((char *) "locutor", opciones.locutores))
    return 1;


  if (configuracion.devuelve_direccion((char *) "morfologico", opciones.fentrada, opciones.fich_morfosintactico))
    return 1;

  if (configuracion.devuelve_direccion((char *) "fonemas", opciones.fentrada, nombre_fonemas))
    return 1;

  if ( (fichero_fonemas = fopen(nombre_fonemas, "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de fonemas %s.\n", nombre_fonemas);
    return 1;
  }

#endif


#ifdef _CORPUS_PROSODICO
  if ( (fichero_rechazo = fopen("corpus_prosodico_buscado.txt", "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero del corpus prosodico.");
    return 1;
  }

  if (crea_descriptores.lee_corpus_prosodico(&corpus_prosodico, &numero_estructuras, fichero_rechazo))
    return 1;

  fclose(fichero_rechazo);

  if ( (fichero_corpus = fopen("corpus.txt", "wt")) == NULL) {
    fprintf(stderr, "Error al intentar crear el fichero de frases aceptadas.");
    return 1;
  }

  if ( (fichero_rechazo = fopen("prosodicas_rechazadas.txt", "wt")) == NULL) {
    fprintf(stderr, "Error al intentar crear el fichero de frases rechazadas.");
    return 1;
  }
#endif

#ifdef _SECUENCIAS_FONETICAS

  if ( (fichero_rechazo = fopen("secuencias_foneticas_buscadas.txt", "rt")) == NULL) {
    fprintf(stderr, "Error al intentar abrir el fichero de secuencias fonéticas buscadas.\n");
    return 1;
  } // if ( (fichero_rechazo = ...

  if (crea_descriptores.lee_estructuras_foneticas(&fonemas_estacionario, &numero_fonemas_estacionario, &fonemas_transicion, &numero_fonemas_transicion, &minimo_numero_palabras, &maximo_numero_palabras, &porcentaje_secuencias, fichero_rechazo))
    return 1;

  fclose(fichero_rechazo);

  if ( (fichero_corpus = fopen("frases_secuencias_seleccionadas.txt", "wt")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de salida frases_secuencias_seleccionadas.txt.\n");
    return 1;
  } // if ( (fichero_corpus = ...

  if ( (fichero_rechazo = fopen("frases_rechazadas.txt", "wt")) == NULL) {
    fprintf(stderr, "Error al intentar crear el fichero de salida frases_rechazadas.txt.\n");
    return 1;
  } // if ( (fichero_rechazo = ...

#endif // ifdef _SECUENCIAS_FONETICAS

#ifdef _PARA_ELISA

  char nombre_silabas[FILENAME_MAX], nombre_fonemas[FILENAME_MAX],
    nombre_sfs[FILENAME_MAX];
  char auxiliar_correccion[200];
  int correccion_manual;
  estructura_duracion duracion_silabas[500];
  estructura_duracion duracion_fonemas[1000];
  int numero_fonemas;
  int numero_silabas;

  FILE *fichero_silabas, *fichero_fonemas;

  Configuracion configuracion;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_parametro("correccion_manual", auxiliar_correccion))
    return 1;

  (strcmp(auxiliar_correccion, "0")) ? correccion_manual = 1 :
    correccion_manual = 0;

  if (configuracion.devuelve_parametro("locutor", opciones.locutores))
    return 1;

  if (configuracion.carga_fichero(opciones.fich_conf))
    return 1;

  if (configuracion.devuelve_direccion("morfologico", opciones.fentrada, opciones.fich_morfosintactico))
    return 1;

  if (configuracion.devuelve_direccion("sfs", opciones.fentrada, nombre_sfs))
    return 1;

  if (configuracion.devuelve_direccion("fonemas", opciones.fentrada, nombre_fonemas))
    return 1;

  if ( (fichero_fonemas = fopen(nombre_fonemas, "rt")) == NULL) {
    printf("Error al intentar abrir el fichero %s.\n", nombre_fonemas);
    return 1;
  }

  if (configuracion.devuelve_direccion("silabas", opciones.fentrada, nombre_silabas))
    return 1;

  if ( (fichero_silabas = fopen(nombre_silabas, "wt")) == NULL) {
    printf("Error al intentar crear el fichero %s.\n", nombre_silabas);
    return 1;
  }

#endif

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES
  if ( (fichero_unidades = fopen("fichero_unidades_distancias.txt", "wt")) == NULL) {
    puts("Error al intentar abrir el fichero \"fichero_unidades_distancias.txt\".");
    return 1;
  } // if ( (fichero_unidades = ...
#endif


  if (opciones.fich_morfosintactico[0] != '\0')
    if ( (f_analisis = fopen(opciones.fich_morfosintactico, "r")) == NULL) {
      fprintf(stderr, "Error en Cotovia::cotovia, al intentar abrir el fichero %s.", opciones.fich_morfosintactico);
      return 1;
    }


  if (OK!=Inicio(entrada)) {
    return ERROR_GENERICO;
  }

  do {

#ifdef _CORPUS_PROSODICO
    if (corpus_incompleto == 0)
      break;
#endif
    Reset();

#ifdef _PARA_ELISA

    numero_silabas = 0;
    numero_fonemas = 0;

#endif

#ifdef _MODO_NORMAL
    if ( (opciones.fich_unidades[0]) && (un_fichero) )
      break;

    if (opciones.fich_unidades[0] == '\0') {
#endif
      //if (leer_frase.leer_frase(entrada, frase, ultima_frase))
      if (carga_frase(entrada, frase, ultima_frase))
	{
	  return -1;
	}

#if defined(_CORPUS_PROSODICO) || defined(_SECUENCIAS_FONETICAS)
      printf("%s\n", frase);
#endif


      if (*frase == 0)
	break;

      if (opciones.audio || opciones.wav || opciones.lin == 1) {
	if (opciones.fich_uso_unidades[0] != '\0')
	  fprintf(stderr, "%d\n", indice_frase);
	fputs(frase,stderr);
	putc('\n',stderr);
      }

#ifdef _CORPUS_PROSODICO
      if (corpus_incompleto) {
#endif

	if (procesado_linguistico()){
	  fprintf(stderr, "Error en el procesado lingüístico\n");
	  return 1;
	}

	if ( (!opciones.lin || opciones.lin == 2 || opciones.lin > 4) && opciones.lin != 8 && opciones.tra < 4 && !opciones.audio && !opciones.wav && !opciones.fon && !opciones.hts)
	  continue;

	//fmendez comienzo generacion_prosodia

#ifdef _PARA_ELISA
	int quehago = generacion_prosodia(fichero_fonemas, fichero_silabas, duracion_silabas, &numero_silabas,
					  duracion_fonemas, &numero_fonemas, nombre_sfs, nombre_fonemas, correccion_manual);
#elif defined(_ESTADISTICA)
	int quehago = generacion_prosodia(&fonema_est, &semifonema_izquierdo_est, &semifonema_derecho_est, &trifonema_est, fichero_fonema_est, fichero_semi_izquierdo, fichero_semi_derecho, fichero_trifonema);
#elif defined(_CALCULA_DATOS_MODELO_ENERGIA)
	int quehago = generacion_prosodia(nombre_sonidos, nombre_fonemas, nombre_marcas, nombre_sfs, fichero_fonemas, fichero_marcas, fichero_sonido, frecuencia_muestreo, &cadena_datos_energia, &numero_elementos_energia, correccion_manual);
#elif defined(_CORPUS_PROSODICO)
	int quehago = generacion_prosodia(&corpus_prosodico, &numero_estructuras, 
					  &corpus_incompleto, fichero_corpus,
					  fichero_rechazo);

#else
	int quehago = generacion_prosodia();
#endif

	//fprintf(stderr, "quehago=%d\n", quehago);
	if (quehago == 1) {
	  return 1;
	}
	//else if (quehago == 2) 
	else if (quehago) 
	  {
	    continue;
	  }

	if (opciones.hts && !opciones.wav && !opciones.audio) {
		libera_memoria_prosodia();
		continue;
	}

#ifdef _MODO_NORMAL

	if (opciones.difo == 0) {

	  //fmendez fin generacion_prosodia

	  if (numero_acentuales)  // Si la frase consta de algún sonido, realizamos la búsqueda.

	    numero_frases_candidatas = NUMERO_FRASES_CANDIDATAS_mP;

	  if (selecciona_minor_phrasing(locutor_actual, frase_frontera_obj, &frase_frontera_cand, &numero_frases_candidatas)) {
	    sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	    if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	      fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	    fprintf(fichero_errores, "%s\n", frase);
	    fclose(fichero_errores);
	    return 1;
	  } // if (selecciona_minor_phrasing

#if defined(_CALCULA_FACTORES)

	  if (seleccion_unidades.seleccion_unidades(cadena_unidades, numero_unidades, acentuales_obj,
						    numero_acentuales,
						    (float) opciones.ep / 100, opciones.N, opciones.M,
						    locutor_actual,
						    opciones.tipo_candidatos,
						    opciones.info_unidades, f_fonetica, frase,
						    opciones.fsalida, ficheros,
						    opciones.fuerza_fronteras,
						    &unidades_escogidas,
						    &numero_unidades_sintesis)) {

	    sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	    if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	      fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	    fprintf(fichero_errores, "%s\n", frase);
	    fclose(fichero_errores);
	    return 1;
	  } // if (seleccion_unidades...

#elif defined(_LEE_ENTONACION_DE_FICHERO)

	  if (seleccion_unidades.seleccion_unidades_contorno_fichero(cadena_unidades, numero_unidades, acentuales_obj, numero_acentuales, (float) opciones.ep / 100, opciones.dir_sal, argv[argc], f_fonetica, locutor_actual, opciones.tipo_candidatos, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {

	    sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	    if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	      fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	    fprintf(fichero_errores, "%s\n", frase);
	    fclose(fichero_errores);
	    return 1;
	  } // if (seleccion_unidades_contorno_fichero

#else

#ifdef _MINOR_PHRASING_TEMPLATE

	  Grupo_acentual_candidato **unidades_entonativas_escogidas;
	  int numero_unidades_entonativas_escogidas;
	  vector<vector<Grupo_acentual_candidato *> > vector_grupos;
	  vector<vector<float> > vector_costes;
	  vector<int> tamano_grupo_fonico;

	  if (opciones.tipo_seleccion_entonativa == 0) {

	    if (seleccion_unidades.seleccion_unidades_entonativas(acentuales_obj, numero_acentuales, frase_frontera_obj, frase_frontera_cand, opciones.N, opciones.M, locutor_actual, &unidades_entonativas_escogidas, &costes_caminos_acentuales, &numero_unidades_entonativas_escogidas, opciones.fsalida, opciones.info_unidades, opciones.fuerza_fronteras)) {
	      
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
		return 1;
	      } // if ( (fichero_errores = ...
	      
	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	    
	    frase_frontera_obj.elementos.clear();
	    frase_frontera_cand.clear();
	  } // if (opciones.tipo_seleccion_entonativa == 0)
	  else if (opciones.tipo_seleccion_entonativa == 1) {

	    if (seleccion_unidades.seleccion_unidades_entonativas_por_grupos_fonicos_total(acentuales_obj, numero_acentuales, frase_frontera_obj, frase_frontera_cand, opciones.N, opciones.M, locutor_actual, &unidades_entonativas_escogidas, &costes_caminos_acentuales, &numero_unidades_entonativas_escogidas, opciones.fsalida, opciones.info_unidades, opciones.fuerza_fronteras)) {
	      
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
		return 1;
	      } // if ( (fichero_errores = ...
	      
	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	    
	    frase_frontera_obj.elementos.clear();
	    frase_frontera_cand.clear();
	    

	  } // else // if (opciones.tipo_seleccion_entonativa....
	  else {
	    if (seleccion_unidades.seleccion_unidades_entonativas_por_grupos_fonicos(acentuales_obj, numero_acentuales, frase_frontera_obj, frase_frontera_cand, opciones.N, opciones.M, locutor_actual, vector_grupos, vector_costes, tamano_grupo_fonico, opciones.fsalida, opciones.info_unidades, opciones.fuerza_fronteras)) {
	      
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
		return 1;
	      } // if ( (fichero_errores = ...
	      
	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	    
	    frase_frontera_obj.elementos.clear();
	    frase_frontera_cand.clear();
	    

	  } // else // if (opciones.tipo_seleccion_entonativa....

	  if (opciones.tipo_seleccion_unidades == 1) {
	    if (seleccion_unidades.seleccion_unidades_acusticas(cadena_unidades, numero_unidades, acentuales_obj, unidades_entonativas_escogidas, costes_caminos_acentuales, numero_acentuales, (float) opciones.ep / 100, numero_unidades_entonativas_escogidas, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {

	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	  } // if (opciones.tipo_seleccion_unidades == 1)
	  else if (opciones.tipo_seleccion_unidades == 2) {
 	    if (seleccion_unidades.seleccion_unidades_acusticas_segmentos(cadena_unidades, numero_unidades, acentuales_obj, unidades_entonativas_escogidas, costes_caminos_acentuales, numero_acentuales, (float) opciones.ep / 100, numero_unidades_entonativas_escogidas, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {

	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	  } // if (opciones.tipo_seleccion_unidades == 2)
	  else if (opciones.tipo_seleccion_unidades == 4) { // culio
 	    if (seleccion_unidades.seleccion_unidades_acusticas_segmentos_por_grupos_fonicos(cadena_unidades, numero_unidades, acentuales_obj, vector_grupos, vector_costes, tamano_grupo_fonico, numero_acentuales, (float) opciones.ep / 100, opciones.M, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	  } // if (opciones.tipo_seleccion_unidades == 4)
	  
	  else if (opciones.tipo_seleccion_unidades == 5) {
 	    if (seleccion_unidades.seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo(cadena_unidades, numero_unidades, acentuales_obj, vector_grupos, vector_costes, tamano_grupo_fonico, numero_acentuales, (float) opciones.ep / 100, opciones.M, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	  } // if (opciones.tipo_seleccion_unidades == 5)
	  else if (opciones.tipo_seleccion_unidades == 3) {
 	    if (seleccion_unidades.seleccion_unidades_acusticas_segmentos_paralelo(cadena_unidades, numero_unidades, acentuales_obj, unidades_entonativas_escogidas, costes_caminos_acentuales, numero_acentuales, (float) opciones.ep / 100, numero_unidades_entonativas_escogidas, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {

	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...

	  } // if (opciones.tipo_seleccion_unidades == 3
	  else {
	    if (seleccion_unidades.seleccion_unidades_acusticas_paralelo(cadena_unidades, numero_unidades, acentuales_obj, unidades_entonativas_escogidas, costes_caminos_acentuales, numero_acentuales, (float) opciones.ep / 100, numero_unidades_entonativas_escogidas, locutor_actual, opciones.tipo_candidatos, opciones.info_unidades, f_fonetica, frase, opciones.fsalida, 0, opciones.fuerza_fronteras, &unidades_escogidas, &numero_unidades_sintesis)) {
	      
	      sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	      if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	      fprintf(fichero_errores, "%s\n", frase);
	      fclose(fichero_errores);
	      return 1;
	    } // if (seleccion_unidades...
	  }
	  // Liberamos la memoria de los grupos acentuales:
	
	  if ( (opciones.tipo_seleccion_entonativa == 0) || (opciones.tipo_seleccion_entonativa == 1) ) {

	    for (int contador = 0; contador < numero_unidades_entonativas_escogidas; contador++) {
	      Grupo_acentual_candidato *apunta_cand = unidades_entonativas_escogidas[contador];
	      for (int cuenta = 0; cuenta < numero_acentuales; cuenta++, apunta_cand++)
		apunta_cand->libera_memoria();
	      free(unidades_entonativas_escogidas[contador]);
	    }
	    free(unidades_entonativas_escogidas);
	  } // if ( (opciones.tipo_seleccion_entonativa == 0) ||
	  else {
	    vector<vector<Grupo_acentual_candidato *> >::iterator it_gf = vector_grupos.begin();
	    vector<Grupo_acentual_candidato *>::iterator it_ga;
	    int i_gf = 0;
	    while (it_gf != vector_grupos.end()) {
	      it_ga = it_gf->begin();
	      while (it_ga != it_gf->end()) {
		int i_ag = 0;
		Grupo_acentual_candidato *libera_ag = *it_ga;
		Grupo_acentual_candidato *primero = libera_ag;
		while (i_ag < tamano_grupo_fonico[i_gf]) {
		  libera_ag->libera_memoria();
		  libera_ag++;
		  i_ag++;
		//		(*it_ga)->libera_memoria();
		//	++it_ga;
		}
		free(primero);
		++it_ga;
	      }
	      ++it_gf;
	      i_gf++;
	    }

	  } // else / if ( (opciones.tipo_seleccion_entonativa == 0) || ...
	  
	  if (costes_caminos_acentuales) {
	    free(costes_caminos_acentuales);
	  costes_caminos_acentuales = NULL;
	  }

	
      
#else
	  if (seleccion_unidades.seleccion_unidades(cadena_unidades, numero_unidades, acentuales_obj,
						    numero_acentuales,
						    (float) opciones.ep / 100, opciones.N, opciones.M,
						    locutor_actual,
						    opciones.info_unidades, f_fonetica, frase,
						    opciones.fsalida, 0,
						    opciones.fuerza_fronteras,
						    &unidades_escogidas,
						    &numero_unidades_sintesis)) {

	    sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	    if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	      fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	      return 1;
	    } // if ( (fichero_errores = ...

	    fprintf(fichero_errores, "%s\n", frase);
	    fclose(fichero_errores);
	    return 1;
	  } // if (seleccion_unidades...

#endif

#endif

	  if (opciones.fich_uso_unidades[0] != '\0') {

	    actualiza_estadisticas_uso_unidades(unidades_escogidas, numero_unidades_sintesis, map_unidades_escogidas);

	    if (indice_frase % 1000 == 0)
	      if (escribe_map_uso_unidades(map_unidades_escogidas, opciones.fich_uso_unidades))
		return 1;
	  } // if (opciones.fich_uso_unidades[0] != '\0')

	} // if (opciones.difo == 0)

#ifdef _MODO_NORMAL
      } // if (opciones.fich_unidades[0] == '\0')
      else {

	if (crea_vectores_sintesis_desde_fichero(opciones.fich_unidades, &unidades_escogidas, &numero_unidades_sintesis, cadena_unidades, &numero_unidades) ) {

	  sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	  if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	    puts("Error al intentar abrir el fichero de frases que provocan errores.");
	    return 1;
	  } // if ( (fichero_errores...

	  fprintf(fichero_errores, "%s\n", frase);
	  fclose(fichero_errores);
	  return 1;

	}

	un_fichero = true;

      } // else // if (opciones.fich_unidades[0] == '\0')

#endif
      if (opciones.difo == 0) {
#if !defined(_SINTETIZA_SIN_UNIDADES_ORIGINALES) && !defined(_CALCULA_FRECUENCIA_COSTES_CONCATENACION)

	if (opciones.fich_uso_unidades[0] == '\0') {

	  if (procesado_senhal.genera_senhal_sintetizada(unidades_escogidas, cadena_unidades, numero_unidades_sintesis, opciones.modificacion_prosodica, locutor_actual->min_pitch, locutor_actual->max_pitch, locutor_actual->fichero_sonido, opciones.ficheros_segmentacion)) {
	    sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	    if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	      puts("Error al intentar abrir el fichero de frases que provocan errores.");
	      return 1;
	  } // if ( (fichero_errores...
	    
	    fprintf(fichero_errores, "%s\n", frase);
	    fclose(fichero_errores);
	    return 1;
	  } // if (genera_senhal_sintetizada...
	}
#endif
	// Liberamos la memoria:

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

	escribe_cadena_unidades_escogidas(cadena_unidades, unidades_escogidas, indice_frase, numero_unidades, indice_menor, fichero_unidades);
#endif
	///fmendez inicio libera_memoria_prosodia();

	libera_memoria_prosodia();

	///fmendez fin libera_memoria_prosodia();

	int contador;
	Vector_sintesis *vector_sintesis = unidades_escogidas;

	if (opciones.fich_unidades[0]) {
	  for (contador = 0; contador < numero_unidades_sintesis; contador++, vector_sintesis++) {
	    vector_sintesis->libera_memoria();
	    free(vector_sintesis->marcas_analisis);
	  }
	}
	else
	  for (contador = 0; contador < numero_unidades_sintesis; contador++, vector_sintesis++)
	    vector_sintesis->libera_memoria();

	free(unidades_escogidas);
	unidades_escogidas = NULL;
	numero_unidades_sintesis = 0;

      }
      else { 
	//if (opciones.difo) {

	if (*frase) {
	  //sintesis_difonemas.sintetiza(f_prosodica,(float) opciones.ep/100,(float) opciones.et/100,0,1,0);
		fprintf(stderr,"Sintesis difonemas obsoleta\n");
	}

      } // else (modo difonemas)

#endif

#ifdef _SCRIPT
      // Está adaptado para ficheros de una sola frase, tal y como son los que tenemos
      // en el corpus.

      int resultado;

      fprintf(fichero_fonetico, "%s\n", f_fonetica);

      if ( (resultado = crea_descriptores.crea_vectores_descriptor(f_separada, f_prosodica, f_fonetica, f_en_grupos, opciones.locutores, f_sintagmada, nombre_sonido, nombre_fonemas, opciones.fentrada, nombre_txt_morf, nombre_sfs, fichero_fonemas, fichero_marcas, fichero_sonido, fichero_cepstrum, fichero_cepstrum_bin, frecuencia_muestreo, frecuencia_minima_locutor, frecuencia_maxima_locutor, numero_de_elementos_de_frase_separada, fichero_sal, correccion_manual, nombre_fichero_origen_configuracion)) != 0) {

	FILE *fichero_comprueba;

	if ( (fichero_comprueba = fopen("fallos_comprobacion.txt", "a")) == NULL) {
	  puts("Fallo al abrir el fichero de comprobación.");
	  return 1;;
	}

	switch (resultado) {
	case 1:
	case 2:
	  fprintf(fichero_comprueba, "%s.\n", nombre_marcas);
	  break;
	case 3:
	  fprintf(fichero_comprueba, "%s.\n", nombre_sonido);
	  break;
	default:
	  break;
	}
	fclose(fichero_comprueba);
	return 1;
      }

#endif

#ifdef _SCRIPT_DURACION

      if (crea_descriptores.calcula_parametros_duracion(f_separada, f_prosodica, f_fonetica, f_en_grupos, f_sintagmada, opciones.fentrada, fichero_fonemas, numero_de_elementos_de_frase_separada, &cadena_duracion, &numero_elementos_duracion, correccion_manual)
	  != 0) {
	fclose(fichero_sal);
	return 1;
      }

#endif


      /*
	#ifdef _ESTADISTICA
	if (calcula_estadistica_1_0(f_prosodica, &semifonema_izquierdo_est,
	&semifonema_derecho_est, &trifonema_est)){
	return 1;
	}


	if (calcula_estadistica_2_0(f_separada, frase_prosodica, frase_fonetica,frase_en_grupos,
	frase_sintagmada, &semifonema_izquierdo_est, &semifonema_derecho_est,
	&trifonema_est)) {
	return 1;
	}

	#endif
      */

#ifdef _GRUPOS_ACENTUALES

      if (frase_solo_signos_de_puntuacion(f_separada))
	break;

      if (crea_descriptores.calcula_grupos_acentuales(frase, f_prosodica, f_fonetica, f_separada, numero_de_elementos_de_frase_separada, f_sintagmada, &acentuales, &numero_acentuales, frase_frontera, &frecuencia_estilizada, fichero_fonemas, fichero_silabas, nombre_fonemas, correccion_manual)) {
	sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	  puts("Error al intentar abrir el fichero de frases que provocan errores.");
	  return 1;
	} // if ( (fichero_errores = fopen...

	fprintf(fichero_errores, "%s\n", frase);
	fclose(fichero_errores);

	fclose(fichero_fonemas);
	fclose(fichero_silabas);
	fflush(fichero_grupos);
	fclose(fichero_grupos);
	fflush(fichero_rupturas);
	fclose(fichero_rupturas);

	return 1;

      }

#ifdef _MINOR_PHRASING_TEMPLATE
      frase_frontera.escribe_datos_txt(fichero_rupturas);
      fprintf(fichero_rupturas, "\n");
      sprintf(nombre_fichero_GF, "origen_GF_%s.txt", opciones.locutores);

      if ( (fichero_origen_GF = fopen(nombre_fichero_GF, "at")) == NULL) {
	fprintf(stderr, "Error al intentar abrir el fichero %s.\n", nombre_fichero_GF);
	return 1;
      }

      char cadena_categoria[50];

      for (unsigned int i = 0; i < frase_frontera.elementos.size(); i++) {
	fprintf(fichero_origen_GF, "%d\t%s\t", frase_frontera.elementos[i].identificador, opciones.fentrada);
	Grupo_fonico_frontera *apunta_GF = &frase_frontera.elementos[i];
	for (unsigned int j = 0; j < apunta_GF->elementos.size(); j++) {
	  if (escribe_tipo_sintagma_grupo((int) apunta_GF->elementos[j].tipo, cadena_categoria)) {
	    fprintf(stderr, "Error, sintagma (%d) no contemplado.\n", apunta_GF->elementos[j].tipo);
	    return 1;
	  }
	  fprintf(fichero_origen_GF, "%s\t", cadena_categoria);
	}
	fprintf(fichero_origen_GF, "\n");
      }

      frase_frontera.elementos.clear();

#endif
      //        fprintf(fichero_grupos, "%s\n", frase_fonetica);

      if (strstr(nombre_fichero_origen_configuracion, ".") == NULL)
	sprintf(nombre_salida, "%s_%s.txt", nombre_fichero_origen_configuracion, opciones.locutores);
      else {
	char *apunta_nombre;
	strcpy(nombre_salida, nombre_fichero_origen_configuracion);
	apunta_nombre = strstr(nombre_salida, ".");
	sprintf(apunta_nombre, "_%s.txt", opciones.locutores);
      }
#ifdef _WIN32

      while (fichero_origen == NULL) {

	int cuenta = 0;
	fichero_origen = fopen(nombre_salida, "at");

	if (fichero_origen != NULL)
	  break;

	fprintf(stderr, "%s ocupado. Reintentamos: %d.\n", nombre_salida, cuenta);

	Sleep(500);

	if (++cuenta > 5)
	  break;

      }

      if (fichero_origen == NULL) {
	fprintf(stderr, "Error al intentar abrir el fichero %s.\n", nombre_salida);
	return 1;
      } // if ( (fichero_origen = ...

#else

      if ( (fichero_origen = fopen(nombre_salida, "at")) == NULL) {
	fprintf(stderr, "Error al intentar abrir el fichero %s.\n", nombre_salida);
	return 1;
      }
#endif

      int posicion_grupo;
      Grupo_acentual_candidato *correcaminos_acentual = acentuales;

      for (int cuenta = 0; cuenta < numero_acentuales; cuenta++, correcaminos_acentual++) {

	if (correcaminos_acentual->grupos_acentuales_grupo_prosodico == 1)
	  posicion_grupo = GRUPO_INICIAL_Y_FINAL;
	else
	  if (correcaminos_acentual->posicion_grupo_acentual == 0)
	    posicion_grupo = GRUPO_INICIAL;
	  else
	    if (correcaminos_acentual->posicion_grupo_acentual ==
		correcaminos_acentual->grupos_acentuales_grupo_prosodico - 1)
	      posicion_grupo = GRUPO_FINAL;
	    else
	      posicion_grupo = GRUPO_MEDIO;

	fprintf(fichero_origen, "%d\t%s\t", correcaminos_acentual->identificador, opciones.fentrada);
	fprintf(fichero_origen, "%d\t%d\t%d\n", correcaminos_acentual->tipo_grupo_prosodico_3,
		posicion_grupo, correcaminos_acentual->posicion_silaba_acentuada);
	fflush(fichero_origen);
	//            acentuales[cuenta].anhade_fichero_origen(argv[2]);
	//            acentuales[cuenta].escribe_datos_txt_tabla(fichero_grupos);
	//            acentuales[cuenta].escribe_datos_txt(fichero_grupos);
	correcaminos_acentual->escribe_datos_bin(fichero_grupos);
	fflush(fichero_grupos);
	correcaminos_acentual->libera_memoria();
      }
      free(acentuales);
      acentuales = NULL;


#endif

#ifdef _GENERA_CORPUS

      if (crea_descriptores.genera_corpus(frase, f_separada, f_prosodica, f_fonetica, f_en_grupos, f_sintagmada, numero_de_elementos_de_frase_separada, &semifonema_izquierdo_est, &semifonema_derecho_est, &trifonema_est, &lista_unidades, fichero_corpus, fichero_rechazo)) {

	return 1;
      }

      if (!lista_unidades.devuelve_tamano()) {
	puts("Corpus completo.");
	getchar();
	break;
      }
#endif


#ifdef _CORPUS_PROSODICO
    } else {
      if (frase[strlen(frase) - 1] ==  '·')
	strcpy(&frase[strlen(frase) - 2], "...");
      fprintf(fichero_rechazo, "%s\n", frase);
    }
#endif

    indice_frase++;

  } while(!ultima_frase);

  if (f_analisis)
    fclose(f_analisis);

#ifdef _ESCRIBE_FRECUENCIA_GRUPOS_ACENTUALES
  if (escribe_grupos_acentuales_escogidos(entrada))
    return 1;
#endif

  Fin();

  if (opciones.fich_uso_unidades[0] != '\0') {
    if (escribe_map_uso_unidades(map_unidades_escogidas, opciones.fich_uso_unidades))
      return 1;
  }

#ifdef _PARA_ELISA

  fclose(fichero_fonemas);
  fclose(fichero_silabas);

#endif

#ifdef _CALCULA_DATOS_MODELO_ENERGIA

  // Escribimos los datos en el fichero de salida:

  for (int contador = 0; contador < numero_elementos_energia; contador++){
#ifdef _ESTIMA_ENERGIA_RED_NEURONAL
    if (objeto_energia.escribe_datos_modelo_energia(cadena_datos_energia + contador, fichero_sal, 0))
      return 1;
#else
    if (objeto_energia.escribe_datos_modelo_energia(cadena_datos_energia + contador, fichero_sal, 0))
      return 1;
#endif
  }

  free(cadena_datos_energia);

  fclose(fichero_fonemas);

  fclose(fichero_sonido);

  fclose(fichero_sal);

#endif

#ifdef _SCRIPT_DURACION

  char nombre_fichero_duracion[FILENAME_MAX];

  sprintf(nombre_fichero_duracion, "unidades_%s.txt", opciones.locutores);

  if ( (fichero_sal = fopen(nombre_fichero_duracion, "at")) == NULL) {
    fprintf(stderr, "Error al abrir el fichero de unidades.\n");
    return 1;
  } // if ( (fichero_sal = ...

  for (int cuenta = 0; cuenta < numero_elementos_duracion; cuenta++)
    escribe_parametros_duracion(&cadena_duracion[cuenta], fichero_sal);

  free(cadena_duracion);

  if (fichero_sal != NULL)
    fclose(fichero_sal);
  if (fichero_fonemas != NULL)
    fclose(fichero_fonemas);
#endif


#ifdef _GRUPOS_ACENTUALES
  free(frecuencia_estilizada.marcas);
  if (fichero_origen)
    fclose(fichero_origen);
  if (fichero_origen_GF)
    fclose(fichero_origen_GF);
  if (fichero_fonemas)
    fclose(fichero_fonemas);
  if (fichero_silabas)
    fclose(fichero_silabas);
  if (fichero_grupos)
    fclose(fichero_grupos);
#ifdef _MINOR_PHRASING_TEMPLATE
  if (fichero_rupturas)
    fclose(fichero_rupturas);
#endif

#endif

#ifdef _CALCULA_INDICES
  fclose(fichero_salida);
#endif

#ifdef _CALCULA_FACTORES
  fclose(ficheros[0]);
  fclose(ficheros[1]);
  fclose(ficheros[2]);
  fclose(ficheros[3]);
  fclose(ficheros[4]);
  fclose(ficheros[5]);
  fclose(ficheros[6]);
  fclose(ficheros[7]);
  fclose(ficheros[8]);
  fclose(ficheros[9]);
  fclose(ficheros[10]);
  fclose(ficheros[11]);
#endif

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES
  if (fichero_unidades)
    fclose(fichero_unidades);
#endif

  // FRAN_CAMPILLO
#ifdef _SCRIPT

  if (fichero_fonetico)
    fclose(fichero_fonetico);
  if (fichero_sal)
    fclose(fichero_sal);
  if (fichero_fonemas)
    fclose(fichero_fonemas);
  //   fclose(fichero_marcas);
  if (fichero_sonido)
    fclose(fichero_sonido);
  if (fichero_cepstrum)
    fclose(fichero_cepstrum);
  if (fichero_cepstrum_bin)
    fclose(fichero_cepstrum_bin);

#endif

#if defined(_ESTADISTICA) || defined(_GENERA_CORPUS)

  if ( (fichero_fonema_est = fopen("fonema.bin", "wb")) == NULL)
    return 1;
  if ( (fichero_semi_izquierdo = fopen("semifon_izdo.bin", "wb")) == NULL)
    return 1;
  if ( (fichero_semi_derecho = fopen("semifon_dcho.bin", "wb")) == NULL)
    return 1;
  if ( (fichero_trifonema = fopen("trifonema.bin", "wb")) == NULL)
    return 1;

  fonema_est.escribe_datos_en_fichero(fichero_fonema_est);
  semifonema_izquierdo_est.escribe_datos_en_fichero(fichero_semi_izquierdo);
  semifonema_derecho_est.escribe_datos_en_fichero(fichero_semi_derecho);
  trifonema_est.escribe_datos_en_fichero(fichero_trifonema);

  fclose(fichero_fonema_est);
  fclose(fichero_semi_izquierdo);
  fclose(fichero_semi_derecho);
  fclose(fichero_trifonema);

  if ( (fichero_fonema_est = fopen("Estad_fonema.txt", "wt")) == NULL)
    return 1;

  if ( (fichero_semi_izquierdo = fopen("Estad_izquierdo.txt", "wt")) == NULL)
    return 1;
  if ( (fichero_semi_derecho = fopen("Estad_derecho.txt", "wt")) == NULL)
    return 1;
  if ( (fichero_trifonema = fopen("Estad_trifon.txt", "wb")) == NULL)
    return 1;

  fonema_est.escribe_estadistica(fichero_fonema_est);
  semifonema_izquierdo_est.escribe_estadistica(fichero_semi_izquierdo);
  semifonema_derecho_est.escribe_estadistica(fichero_semi_derecho);
  trifonema_est.escribe_estadistica(fichero_trifonema);

  fclose(fichero_fonema_est);
  fclose(fichero_semi_izquierdo);
  fclose(fichero_semi_derecho);
  fclose(fichero_trifonema);

  fonema_est.libera_memoria();
  semifonema_izquierdo_est.libera_memoria();
  semifonema_derecho_est.libera_memoria();
  trifonema_est.libera_memoria();

#endif

#ifdef _CORPUS_PROSODICO
  if (corpus_incompleto) {
    puts("Corpus no completado.");
    fclose(fichero_corpus);
    if ( (fichero_corpus = fopen("corpus_no_encontrado.txt", "wt")) == NULL) {
      puts("Error al intentar crear el fichero de corpus no encontrado.");
      return 1;
    }
    crea_descriptores.escribe_corpus_prosodico(corpus_prosodico, numero_estructuras, fichero_corpus);
    fclose(fichero_corpus);
    getchar();
  }
  fclose(fichero_rechazo);
  // Liberamos las estructuras del corpus prosódico:
  crea_descriptores.libera_corpus_prosodico(corpus_prosodico, numero_estructuras);

#endif // ifdef _CORPUS_PROSODICO

#ifdef _SECUENCIAS_FONETICAS

  fclose(fichero_corpus);
  fclose(fichero_rechazo);

  crea_descriptores.libera_estructuras_foneticas(fonemas_estacionario, fonemas_transicion);

#endif // ifdef _SECUENCIAS_FONETICAS

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
  if (imprime_frecuencia_costes_concatenacion_bin("costes_concatenacion.bin"))
    return 1;
  borra_cache_frecuencia_costes();

#endif


  return OK;
}



/**
 * \remarks Xa que para cada frase hai que utilizar as mesmas variables globais 
 * hai que resetealas cada vez que empezamos a procesar unha nova frase.                                                     *
 * 
 * \author fmendez
 *
 */
 
void Cotovia::Reset(void){

  register int i,j;
  register Token *token = f_tokenizada;
  register t_frase_separada *rec=f_separada;
  register t_frase_sintagmada *rec1=f_sintagmada;
  register t_frase_en_grupos *rec2=f_en_grupos;
  register t_prosodia *rec3=f_prosodica;
  register t_infinitivo *inf;
  unsigned char *c1,*c2;
  int kk=sizeof(t_frase_separada);
  int kk2=sizeof(t_infinitivo);

  *frase=0;
  *f_procesada=0;
  *f_silabas=0;
  *f_fonetica=0;

  for (i = 0; i < numero_de_tokens; i++){
    *token->token = 0;
    token->pal_transformada = NULL;
  }
  numero_de_tokens = 0;

  for(i=0;i<numero_de_elementos_de_frase_separada;i++){
    *rec->pal=0;
    *rec->lema=0;
    rec->pal_transformada=NULL;
    *rec->pal_sil_e_acentuada=0;
    rec->clase_pal=0;
    if (!opciones.audio && !opciones.wav && !opciones.hts && !opciones.lin &&
	(opciones.pre || (opciones.tra>0 && opciones.tra<4))) {
      rec= (t_frase_separada *) ((char  *)rec + kk);
      continue;
    }

    c1=rec->cat_gramatical;
    c2=rec->cat_gramatical_descartada;
    for(j=0;j<N_CAT;j++,c1++,c2++){
      *c1=0;
      *c2=0;
    }
    inf=rec->inf_verbal;
    c1=rec->cat_verbal;
    for(j=0;j<TEMPOS_POSIBLES;j++,c1++){
      *c1=0;
      *inf->infinitivo=0;
      *inf->raiz=0;
      *inf->desinencia=0;
      *inf->enclitico.enclitico=0;
      inf->enclitico.grupo=0;
      *inf->artigo=0;
      *inf->reconstruccion=0;
      inf->modelo=0;
      inf= (t_infinitivo *) ((char  *)inf + kk2);
    }
    *rec->raiz=0;
    *rec->sufijo=0;
    rec->xenero=0;
    rec->numero=0;
    rec->tipo_sintagma=0;
    rec->n_silabas=0;
    rec->tonicidad_forzada=0;
    rec= (t_frase_separada *) ((char  *)rec + kk);

  }

  //**palabras_procesadas=0;
  //*palabras_procesadas[1]=0;

#if defined(_MODO_NORMAL) && !defined(_CORPUS_PROSODICO) && !defined(_SECUENCIAS_FONETICAS)
  if (!(opciones.audio || opciones.wav || opciones.hts || opciones.lin==1 || opciones.tra==4 ||
	opciones.lin==3 || opciones.lin==4 || opciones.lin==8)) {
    numero_de_elementos_de_frase_separada=0;
    return;
  }
#endif


  for(i=0;i<numero_de_elementos_de_frase_separada;i++,rec1++){
    rec1->tipo_sintagma=0;
    rec1->funcion=0;
    rec1->inicio=0;
    rec1->fin=0;
    rec1->pausa=0;
    rec1->n_silabas=0;
  }
  for(i=0;i<numero_de_elementos_de_frase_separada;i++,rec2++){
    rec2->inicio=0;
    rec2->fin=0;
    rec2->tipo_proposicion=0;
    rec2->tipo_tonema=0;
    *rec2->tipo_inicio=0;
    rec2->tipo_inicio[1]=0;
    *rec2->tipo_fin=0;
    rec2->tipo_fin[1]=0;
    rec2->enlazado=0;
    rec2->ruptura_entonativa = 0;
  }
  numero_de_elementos_de_frase_separada=0;
  while ( *rec3->alof ) {

    *rec3->alof=0;
    rec3->dur1=0;
    rec3->dur2=0;
    rec3->pitch1=0;
    rec3->pitch2=0;
    rec3->pitch3=0;
    rec3->tonica=0;
    rec3->ti_prop=0;
    rec3->pos_grupo_prosodico=0;
    rec3->pos_silabica=0;
    rec3->pos_segmento=0;
    rec3->pos_tonica=0;
    rec3->pga=0;
    rec3->pp=0;
    rec3->ps=0;
    rec3->sil_fin=0;
    rec3->lonx_sil=0;
    rec3->estr_sil=0;
    if (rec3 < f_prosodica + LONX_MAX_FRASE_SIL_E_ACENTUADA - 1){
      rec3++;
    }
    else {
      break;
    }
  }

}

/**
 * \author fmendez
 * \param entrada
 * \return distinto de 0 si hay error
 */
int Cotovia::Inicio(char * entrada){
  char auxiliar[FILENAME_MAX], * kk;


  if (opciones.entrada_por_fichero)
    _copia_cadena(opciones.fentrada,entrada);

  if (opciones.fsalida[0])
    strcpy(auxiliar,opciones.fsalida);
  else if (opciones.fstdin || !opciones.entrada_por_fichero)
    strcpy(auxiliar,"default");
  else
    strcpy(auxiliar,opciones.fentrada);

  kk=auxiliar+strlen(auxiliar);
#ifndef _WIN32
  while ((kk>auxiliar)&&(*kk!='/'))
#else
    while ((kk>auxiliar)&&(*kk!='\\'))
#endif
      kk--;

  if (kk!=auxiliar)
    kk++;

  _copia_cadena(auxiliar,kk);

  kk=auxiliar+strlen(auxiliar);
  while ((kk>auxiliar)&&(*kk!='.'))
    kk--;
  if (kk>auxiliar)
    *kk=0;
  strcpy(opciones.fsalida,opciones.dir_sal);
  strcat(opciones.fsalida,auxiliar);

  if (opciones.fstdout)
    flin=stdout;

  opciones.idioma = lenguas[opciones.lenguajes].tipo;

  leer_frase.inicializa(&tipo_frase, &opciones);
  preprocesador.inicializa(&opciones);
  transcripcion.inicializa(&opciones);
  //inicializar_reglas_transcripcion(opciones.idioma);
  prosodia.inicializa(&opciones);

  if (diccionarios->inicia_diccionario(opciones, lenguas[opciones.lenguajes].indice))
    return 1;

  if (!(opciones.audio || opciones.wav || opciones.lin || opciones.hts || opciones.tra==4 || opciones.fon))
    return OK;

  analisis_morfosintactico.selecciona_lenguaje(opciones.lenguajes);

#ifdef _ESTADISTICA
  return OK;
#endif

#ifdef _MODO_NORMAL
  if (opciones.audio || opciones.wav || opciones.hts) {

    if (opciones.difo) {
			fprintf(stderr,"Sintesis difonemas obsoleta\n");
/*
 *
 *      char * apunta_nombre, nombre_local[TAMANO_MAXIMO_CADENA_LOCUTORES];
 *      strcpy(nombre_local, opciones.locutores);
 *      apunta_nombre = strtok(nombre_local, " ");
 *
 *      if (!strcmp("Freire",apunta_nombre)){
 *  opciones.bd=0;
 *      }
 *      else if (!strcmp("Sonia",apunta_nombre)){
 *  opciones.bd=1;
 *      }
 *      else {
 *  fprintf(stderr,"Erro: Non existe locutor difonemas \"%s\".\n",apunta_nombre);
 *  return 1;
 *      }
 *      sintesis_difonemas.inicializa(&opciones, &audio, 16000);
 */
    }
    else {
      char *apunta_nombre, nombre_local[TAMANO_MAXIMO_CADENA_LOCUTORES];

      strcpy(nombre_local, opciones.locutores);

      apunta_nombre = strtok(nombre_local, " ");

      if (selecciona_locutor(apunta_nombre, locutor_actual))
	return 1;

      procesado_senhal.reinicia(opciones.audio, opciones.wav, opciones.f8,
				locutor_actual->frecuencia_muestreo,opciones.fsalida,
				opciones.dir_sal, opciones.fentrada);
    }
  }

#endif

  return OK;
}

/**
 * \author fmendez
 * \remarks Carga en memoria voces, diccionarios, modelos, ...
 * \param opc Estructura con las opciones de configuración de cotovia.
 * \return Distinto de cero en caso de error.
 */
int Cotovia::Carga(t_opciones &opc){

  numero_de_elementos_de_frase_separada=0;
  numero_de_tokens = 0;
  flin = NULL;
  opciones = opc;

  variantes_cargado = false;
  analisis_morfosintactico_cargado = false;
  locutor_cargado = false;
  audio_cargado = false;

  map_unidades_escogidas.clear();

  if ((frase = (char *) malloc(LONX_MAX_CARACTERES_FRASE *
			       sizeof(char))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en frase\n");
  }


  if ((f_tokenizada = (Token *) malloc(LONX_MAX_FRASE_SEPARADA *
				       sizeof(Token))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_tokenizada\n");
  }

  if ((f_separada = (t_frase_separada *) malloc(LONX_MAX_FRASE_SEPARADA *
						sizeof(t_frase_separada))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_separada\n");
  }

  if ((f_procesada = (char *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA *
				     sizeof(char))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_procesada\n");
  }
  if ((f_silabas = (char *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA *
				   sizeof(char))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_silabas\n");
  }
  if ((f_fonetica = (char *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA *
				    sizeof(char))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_fonetica\n");
  }
  if ((f_sintagmada = (t_frase_sintagmada *) malloc(NUM_ELEMENTOS_FRASE_SINTAGMADA *
						    sizeof(t_frase_sintagmada))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_sintagmada\n");
  }
  if ((f_en_grupos = (t_frase_en_grupos *) malloc(LONX_MAX_FRASE_EN_GRUPOS *
						  sizeof(t_frase_en_grupos))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_en_grupos\n");
  }
  if ((f_prosodica = (t_prosodia *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA *
					   sizeof(t_prosodia))) == NULL) {
    fprintf(stderr, "Error al asignar memoria en f_prosodica\n");
  }


  *frase = 0;
  *f_procesada = 0;
  *f_silabas = 0;
  *f_fonetica = 0;

  memset(f_tokenizada, 0 , LONX_MAX_FRASE_SEPARADA * sizeof(Token));
  memset(f_separada, 0 , LONX_MAX_FRASE_SEPARADA * sizeof(t_frase_separada));
  memset(f_sintagmada, 0 , NUM_ELEMENTOS_FRASE_SINTAGMADA * sizeof(t_frase_sintagmada));
  memset(f_en_grupos, 0 , LONX_MAX_FRASE_EN_GRUPOS * sizeof(t_frase_en_grupos));
  memset(f_prosodica, 0 , LONX_MAX_FRASE_SIL_E_ACENTUADA * sizeof(t_prosodia));

  //memset(f_prosodica, 0 , LONX_MAX_FRASE_SIL_E_ACENTUADA * sizeof(f_prosodica));



  //leer_frase.inicializa(&tipo_frase,fent);

#ifdef _DLL

  if (opciones.audio == 0 && opciones.wav == 0 && opciones.pre == 0 &&
      opciones.tra == 0 && opciones.lin == 0)
    opciones.audio=1;

  if (*opciones.dir_bd==0) {
    char *barra;
#ifdef _WIN32
    if ((barra=strrchr(opciones.dir_bd,'\\')) == NULL)
      strcpy(opciones.dir_bd,"..\\data\\");
    else
      strcpy(barra,"\\..\\data\\");
#else
    if ((barra=strrchr(opciones.dir_bd,'/')) == NULL)
      strcpy(opciones.dir_bd,"../data/");
    else
      strcpy(barra,"/../data/");
#endif
  }
#endif //_DLL


#ifndef _WIN32
  if (opciones.dir_sal[strlen(opciones.dir_sal)-1]!='/')
    strcat(opciones.dir_sal,"/");
  if (opciones.dir_bd[strlen(opciones.dir_bd)-1]!='/')
    strcat(opciones.dir_bd,"/");
#else
  if (opciones.dir_sal[strlen(opciones.dir_sal)-1]!='\\')
    strcat(opciones.dir_sal,"\\");
  if (opciones.dir_bd[strlen(opciones.dir_bd)-1]!='\\')
    strcat(opciones.dir_bd,"\\");
#endif




  char * l;
  char lang_local[TAMANO_MAXIMO_CADENA_LOCUTORES];
  char cadena[FILENAME_MAX];
  FILE * fichero_idioma;

  strcpy(lang_local, opciones.lenguajes);
  numero_idiomas = 0;
  l = strtok(lang_local, " ");

	while (l != NULL){
		if (!lenguas.count(l)) {
			lenguas[string(l)].indice = numero_idiomas;
			lenguas[string(l)].numero_usuarios++;
			lang[numero_idiomas] = l;
			strcpy(opciones.lenguajes, l);

			sprintf(cadena, "%slang/%s/idioma.txt", opciones.dir_bd, opciones.lenguajes);
			if ((fichero_idioma = fopen(cadena, "r"))==NULL){
				fprintf(stderr,"Non se encontra o arquivo de idioma: %s\n", cadena);
				return ERROR_FICHERO_DE_ENTRADA;
			}			
			fgets(cadena, FILENAME_MAX, fichero_idioma);
			fclose(fichero_idioma);

			if (!strcmp(cadena, "galego")) {
				lenguas[string(l)].tipo = GALEGO;
			}
			else if (!strcmp(cadena, "español")) {
				lenguas[string(l)].tipo = CASTELLANO;
			}
			else {
				lenguas[string(l)].tipo = GALEGO;
			}

			if (opciones.alternativas){
				sprintf(cadena, "%slang/%s/variantes.txt", opciones.dir_bd, opciones.lenguajes);

				if (transcripcion.lee_variantes(cadena, opciones.alternativas-1)) {
					return 1;
				}
				variantes_cargado = true;
			}
			if (diccionarios->interfaz(opciones, numero_idiomas)){
				return 1;
			}
		}
		numero_idiomas++;
		l = strtok(NULL, " ");
	}
  strcpy(opciones.lenguajes, lang[0]);//por defecto usa el primer lenguaje

  opc = opciones;

  char ruta[FILENAME_MAX];

#if defined(_GENERA_CORPUS) ||			\
  defined(_PARA_ELISA)
  // FRAN_CAMPILLO: He quitado los modos _SCRIPT, _SCRIPT_DURACION y _GRUPOS_ACENTUALES
  // porque en ellos se lee el análisis morfosintáctico de un fichero.
  //	char ruta[FILENAME_MAX];
  //sprintf(ruta, "%slang/%s/",opciones.dir_bd, lang[opciones.idioma]);

  //	if (analisis_morfosintactico.carga(ruta, lang[0]))
  //		return 1;
  //	analisis_morfosintactico_cargado = true;
#endif

  *locutores_empleados = 0;

#ifndef _ESTADISTICA
  if (!(opciones.audio || opciones.wav || opciones.lin || opciones.hts || opciones.tra==4 || opciones.fon))
    return OK;
#endif

  for(int ind = 0; ind < numero_idiomas; ind++){
    sprintf(ruta, "%slang/%s/",opciones.dir_bd, lang[ind]);
    if (analisis_morfosintactico.carga(ruta, lang[ind])) {
      return 1;
    }
  }
  analisis_morfosintactico_cargado = true;


  if (!(opciones.audio || opciones.wav || opciones.hts)) {
    return OK;
  } // if (!(opciones.audio || ...


#ifdef _MODO_NORMAL
  if (opciones.difo == 0){

    // Contamos el número de locutores

    //		char nombres_locutores[FILENAME_MAX];
    char *apunta_nombres;
    char nombres_locutores_local[TAMANO_MAXIMO_CADENA_LOCUTORES];
    char directorio_locutor[FILENAME_MAX];
    Locutor *apunta_locutor;
    bool carga_locutor;

    strcpy(locutores_empleados, opciones.locutores);

    strcpy(nombres_locutores_local, locutores_empleados);

    if (strlen(opciones.locutores) > TAMANO_MAXIMO_CADENA_LOCUTORES) {
      fprintf(stderr, "Error en Carga. Cadena de locutores no válida.\n");
      return 1;
    } // if (strlen(opciones.locutores...

    apunta_nombres = strtok(nombres_locutores_local, " ");

    while (apunta_nombres != NULL) {

      if (anhade_locutor(apunta_nombres, &carga_locutor))
	return 1;

      if  (carga_locutor == true) {

	// Reservamos la memoria:

	if ( (locutor = (Locutor **) realloc(locutor, locutores_cargados*sizeof(Locutor *)))
	     == NULL) {
	  fprintf(stderr, "Error en carga, al reservar memoria para la cadena de \
							Locutores.\n");
	  return 1;
	}// if ( (locutor = ...

	// Bien, esta mezcla entre new y realloc es especialmente indecorosa desde el punto
	// de vista del estilo. Supongo que deberíamos utilizar la clase vector para la
	// variable locutor. La razón para utilizar new es que malloc no llama al constructor
	// (lo cual es especialmente grave cuando alguna variable miembro es un objeto que
	// necesita inicializarse). Y el realloc se utiliza sobre la variable *locutor*, que es
	// un array de punteros. Es decir, que queda feo, pero no se están utilizando ambas
	// funciones sobre la misma zona de memoria. Eso sí, hay que cambiarlo en algún momento.
	try {
	  apunta_locutor = new Locutor;
	}

	catch (bad_alloc&) {
	  if ( (apunta_locutor = (Locutor *) malloc(sizeof(Locutor))) == NULL) {
	    fprintf(stderr, "Error en carga, al reservar memoria para el locutor.\n");
	    return 1;
	  } // if ( (apunta_locutor = ...
	}

	*(locutor + locutores_cargados - 1) = apunta_locutor;

	// Añadimos el directorio en el que están los datos del locutor:

	sprintf(directorio_locutor, "%s/%s", opciones.dir_bd, apunta_nombres);

	apunta_locutor->introduce_directorio(directorio_locutor, apunta_nombres);

	// El Corpus de voz.

	if (apunta_locutor->lee_corpus_voz_y_ficheros_seleccion())
	  return 1;

	/*
	// La base de difonemas, si es Freire.

	if (strstr(apunta_locutor->directorio, "Freire") != NULL)
	if (apunta_locutor->lee_base_difonemas())
	return 1;
	*/
	// El Corpus prosódico:

	if (apunta_locutor->lee_corpus_prosodico())
	  return 1;


#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA
	if (apunta_locutor->lee_fichero_contextos_gramaticales())
	  return 1;
	if (apunta_locutor->lee_fichero_insercion_rupturas())
	  return 1;
#endif

	// Modelo de duración:

	if (apunta_locutor->lee_modelos_duracion())
	  return 1;

	// Modelo de energía:

	if (apunta_locutor->lee_modelo_energia())
	  return 1;

	// El fichero de sonido:

	if (apunta_locutor->abre_fichero_sonido())
	  return 1;


      } // if (carga_locutor == true)

      //           apunta_nombres++;

      apunta_nombres = strtok(NULL, " ");


    } // while (apunta_nombres != NULL)

    locutor_cargado = true;

    // Seleccionamos, por defecto, el primer locutor:

    strcpy(nombres_locutores_local, locutores_empleados);

    if (selecciona_locutor(strtok(nombres_locutores_local, " "), locutor_actual))
      return 1;

    procesado_senhal.inicializa(opciones.audio, opciones.wav, opciones.f8,
				locutor_actual->frecuencia_muestreo,opciones.fsalida,
				opciones.dir_sal, opciones.fentrada,&audio);

  } // if (opciones.difo == 0)

#endif

#if !defined(_SCRIPT) && !defined(_PARA_ELISA) &&			\
  !defined(_SCRIPT_DURACION) && !defined(_CALCULA_DATOS_MODELO_ENERGIA) && \
  !defined(_GRUPOS_ACENTUALES)


  if (opciones.audio){
      audio.AbreDispositivoAudio(locutor_actual->frecuencia_muestreo);
    /*
     *if (opciones.difo) {
     *  audio.AbreDispositivoAudio(16000);
     *}
     *else {
     *  audio.AbreDispositivoAudio(locutor_actual->frecuencia_muestreo);
     *}
     */
    audio_cargado = true;
  }


#endif

  return OK;
}


/**
 * \author fmendez
 * \remarks Pechanse os arquivos, libera memoria
 */
void Cotovia::Fin (void){

  if (!opciones.fstdout) {
    if (flin!=NULL)
      fclose(flin);
    flin=NULL;
  }


  preprocesador.finaliza();
  transcripcion.finaliza();
  prosodia.finaliza();
  leer_frase.finaliza();


#ifdef _MODO_NORMAL
  if (opciones.audio || opciones.wav ||opciones.hts){
    /*
     *if (opciones.difo == 1){
     *  sintesis_difonemas.libera_memoria();
     *}
     *else {
     *  procesado_senhal.libera_memoria_procesado_senhal();
     *  //procesado_senhal.finaliza();
     *}
     */
      procesado_senhal.libera_memoria_procesado_senhal();
  }
#endif

}

/**
 * \author fmendez
 * \remarks Cierra dispositivo de audio, ficheros y
 * descarga diccionarios, modelos y voces...
 */
int Cotovia::Descarga(void){

#ifdef _MODO_NORMAL

  //#ifndef _DLL
  if (locutor_cargado == true) {
    //#endif

    bool libera_memoria;

    char *apunta_nombre, nombre_local[TAMANO_MAXIMO_CADENA_LOCUTORES];
    Locutor *apunta_locutor;
    int posicion;

    strcpy(nombre_local, locutores_empleados);

    apunta_nombre = strtok(nombre_local , " ");

    while (apunta_nombre != NULL) {

      if (libera_locutor(apunta_nombre, &libera_memoria, &posicion))
	return 1;

      if (libera_memoria == true) {

	apunta_locutor = *(locutor + posicion);
	apunta_locutor->libera_memoria_corpus_prosodico();
	apunta_locutor->libera_memoria_corpus_voz_y_ficheros_seleccion();
	apunta_locutor->libera_memoria_modelos_duracion();
	apunta_locutor->libera_memoria_modelo_energia();
	apunta_locutor->cierra_fichero_sonido();
	//                if (strstr(apunta_locutor->directorio, "Freire"))
	//                    apunta_locutor->libera_memoria_base_difonemas();
	//				libera_recursos_unidades();

	//                free(apunta_locutor);
	delete apunta_locutor;

	for (int contador = posicion; contador < locutores_cargados; contador++){
	  *(locutor + contador) = *(locutor + contador + 1);
	}

      } // if (libera_memoria == true)


      //			apunta_nombre++;
      apunta_nombre = strtok(NULL, " ");

    } // while (apunta_nombre != NULL)

    if (opciones.audio || opciones.wav)
      procesado_senhal.libera_memoria_procesado_senhal();

    if (locutores_ya_cargados == NULL) {
      free(locutor);
      locutor = NULL;
    } // if (locutores_ya_cargados == NULL)
    else {
      if ( (locutor = (Locutor **) realloc(locutor,
					   locutores_cargados*sizeof(Locutor *))) == NULL) {
	fprintf(stderr, "Error en Descarga, al reasignar memoria.\n");
	return 1;
      } // if ( (locutor = ...
    } // else

    /*
      libera_memoria_modelo_duracion();
      #ifdef _ESTIMA_ENERGIA_RED_NEURONAL
      libera_memoria_red_energia();
      #else
      libera_memoria_modelo_energia();
      #endif
      libera_memoria_tabla_grupos();

      #ifdef _UTILIZA_RED_NEURONAL
      libera_memoria_Viterbi_red_neuronal();
      #endif
    */

    //#ifndef _DLL
  }
  //#endif
  /*
    if (analisis_morfosintactico_cargado == true) {
    analisis_morfosintactico.descarga();
    }
  */
  if (variantes_cargado == true){
    transcripcion.libera_variantes();
  }

#endif

  //#ifdef _PARA_ELISA
  if (analisis_morfosintactico_cargado == true) {
    analisis_morfosintactico.descarga();
  }
  //#endif

#if !defined(_SCRIPT) && !defined(_PARA_ELISA) &&			\
  !defined(_SCRIPT_DURACION) && !defined(_CALCULA_DATOS_MODELO_ENERGIA) && \
  !defined(_GRUPOS_ACENTUALES)
  if (audio_cargado == true) {
    audio.CierraDispositivoAudio();
  }
#endif


#ifdef _ESCRIBE_FICHERO_PAUSAS
  cierra_ficheros_pausas();
#endif

  //diccionariosborraListas();

  free(frase);
  free(f_tokenizada);
  free(f_separada);
  free(f_procesada);
  free(f_silabas);
  free(f_fonetica);
  free(f_sintagmada);
  free(f_en_grupos);
  free(f_prosodica);

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función encargada de seleccionar el locutor con el que se va a
 * pronunciar la frase actual.
 * \param nombre_locutor nombre del locutor, que ha de estar previamente cargado.
 * \param locutor_actual
 * \return En ausencia de error, devuelve un cero.
 */
int Cotovia::selecciona_locutor(char *nombre_locutor, Locutor * &locutor_actual) {

  int posicion;

  if (devuelve_posicion_locutor(nombre_locutor, &posicion)) {
    fprintf(stderr, "Error en selecciona_locutor: base de datos (%s) \
				no cargada.\n", nombre_locutor);
    return 1;
  } // if ( (base_datos < 0) ...

  locutor_actual = *(locutor + posicion);

  return 0;

}


/**
 * \author fmendez
 * \brief 
 */
void Cotovia::construye_frase_fonetica(void){
  trat_fon.tratamento_fonetico_previo(f_separada,f_sintagmada,f_en_grupos,f_silabas, opciones.idioma);
  //if (opciones.entrada_con_timbre) trat_fon.arregla_timbre(copia_frase_separada);
  //saca_tipo_prop(f_silabas);
  transcripcion.transformar_a_alofonos(f_silabas,f_fonetica, opciones.idioma);
}

/**
 * \author fmendez
 * \brief
 * \return 0 si no hay error
 */

int Cotovia::procesado_linguistico(void) {

  /*  Extraemo-la frase do texto deixandoa en frase_extraida.                   */
  //fprintf(stderr,"%s\n",frase);
  tokenizador.tokenizar(frase, f_tokenizada, numero_de_tokens, opciones.idioma);
  /* Rellenamos o campo pal_procesada de cada estructura de frase_separada cun
     punteiro a zona de memoria onde se encontra a palabra extendida e decir por
     exemplo pasar un numero a letra. A saida temos xa preparada a variable frase
     procesada.
     Rellenamos o campo pal de cada estructura de frase_separada cunha palabra
     sen procesar. 
                                                             */


  clasificar_palabras(f_tokenizada,strlen(frase));

  //fprintf(stdout,"f_tokenizada->clase_pal despois da chamada a clasificar_palabras: %d\n", f_tokenizada->clase_pal);



/*
 *  clasificar_palabras(f_tokenizada);
 *
 *  if ((f_tokenizada->clase_pal==NUM_ROMANO) && (strlen(frase)==2)){
 *
 *        f_tokenizada->clase_pal=SIN_CLASIFICAR;
 *
 *  }
 */



//      if (opciones.entrada_con_timbre)
  //       memcpy(copia_frase_separada,f_separada,sizeof(f_separada));
  /* Rellenamos o campo clase_pal de cada estructura de frase separada cun
     codigo que identifica a forma da palabra(numero cardinal, ordinal, data,
     palabra simple, con maiusculas etc.                                        */

  preprocesador.preprocesa(f_tokenizada, f_procesada, f_separada, &numero_de_elementos_de_frase_separada);

  if (!(opciones.audio || opciones.wav || opciones.hts || opciones.lin || opciones.tra || opciones.fon)) {
    return 0;
  }
  /* Rellenamos o campo pal_procesada de cada estructura de frase_separada cun
     punteiro a zona de memoria onde se encontra a palabra extendida e decir por
     exemplo pasar un numero a letra. A saida temos xa preparada a variable fra-
     se procesada.                                                              */

  silabificar_e_acentuar(f_separada, opciones.idioma);
  if (opciones.tra && opciones.tra<4 ){
    transcripcion.transcribe(f_separada, f_silabas, f_fonetica, opciones.idioma);
    if(!opciones.hts) transcripcion.vuelca_transcripcion(f_fonetica, f_separada);
    if (!(opciones.audio || opciones.wav || opciones.lin || opciones.hts)) {
      return 0;
    }
    *f_fonetica = 0;
    *f_silabas = 0;
  }


  /* De frase separada recollemos cada palabra, silabificamola ,acentuamola e
     deixamola na variable frase_sil_e_acentuada,e tamen no campo adecuado de
     frase_separada.                                                            */

   analisis_morfologico.analise_morfoloxica(f_separada, opciones);

  if (f_analisis != NULL) {

    if (analisis_morfosintactico.analisis_morfosintactico(f_separada,
							  opciones.fich_morfosintactico,f_analisis)) {

      FILE *fichero_comprueba;

      if ( (fichero_comprueba = fopen("fallos_comprobacion.txt", "a")) == NULL) {
	fprintf(stderr, "Fallo al abrir el fichero de comprobación.\n");
	return 1;
      }

      fprintf(fichero_comprueba, "Error al procesar el fichero %s.\n", opciones.fich_morfosintactico);
      fclose(fichero_comprueba);

      return 1;
    } // if (analisis_morfosintactico....

  } // if (f_analisis != NULL)
  else {
    //analisis_morfologico.analise_morfoloxica(f_separada, opciones);
    if (analisis_morfosintactico.analisis_morfosintactico(f_separada,numero_de_elementos_de_frase_separada, 
							  opciones.dir_sal, frase, opciones.idioma, opciones.lin)){
      return 1;
    }
  } // else // if (f_analisis != NULL)

#if !defined(_CORPUS_PROSODICO) && !defined(_SECUENCIAS_FONETICAS)
  if ( (opciones.lin == 2 || opciones.lin > 4) && opciones.lin != 8 && !opciones.hts)   {
    analisis_morfosintactico.sacar_analisis_morfosintactico(frase, f_separada,
							    f_procesada, f_silabas, f_fonetica, f_sintagmada, f_en_grupos,
							    f_prosodica, flin, opciones.fsalida, opciones.lin,opciones.separa_lin,opciones.idioma);
  }
#endif
  return 0;
}


/**
 * \author fmendez
 * \brief
 * \return 0 si no hay error, 1 si hay error, 2 si hay que saltar a la siguiente frase (continue)
 */
#ifdef _PARA_ELISA
int Cotovia::generacion_prosodia(FILE * fichero_fonemas, FILE * fichero_silabas, estructura_duracion *duracion_silabas, int *numero_silabas, estructura_duracion *duracion_fonemas, int *numero_fonemas, char * nombre_sfs, char * nombre_fonemas, int correccion_manual)
#elif defined(_ESTADISTICA)
  int Cotovia::generacion_prosodia(Estadistica *fonema_est, Estadistica *semifonema_izquierdo_est, Estadistica *semifonema_derecho_est, Estadistica *trifonema_est, FILE *fichero_fonema_est, FILE *fichero_semi_izquierdo, FILE *fichero_semi_derecho, FILE *fichero_trifonema)

#elif defined(_CALCULA_DATOS_MODELO_ENERGIA)
  int Cotovia::generacion_prosodia(char * nombre_sonidos,char * nombre_fonemas, char *nombre_marcas, char * nombre_sfs, FILE * fichero_fonemas, FILE *fichero_marcas, FILE * fichero_sonido, int frecuencia_muestreo, Parametros_modelo_energia ** cadena_datos_energia, int * numero_elementos_energia, int correccion_manual)
#elif defined(_CORPUS_PROSODICO)
  int Cotovia::generacion_prosodia(Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
				   unsigned char *corpus_incompleto,
				   FILE *fichero_corpus, FILE *fichero_rechazo)
#else
  int Cotovia::generacion_prosodia(void)
#endif
{

  char nombre_fichero_salida[FILENAME_MAX];
  FILE * fichero_errores;

  static int primera_frase = 1;

#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
  if (opciones.fich_morfosintactico[0] == '\0') {
    sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
    crea_frase_pausas(f_separada, f_sintagmada);
  }
  else {
    sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
    poner_pausas(f_sintagmada, f_separada);
  }

#endif

#ifdef _SOLO_PAUSAS_ORTOGRAFICAS
  sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
  poner_pausas(f_sintagmada,f_separada);
#endif

#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
  //		introduce_rupturas_entonativas(f_sintagmada, f_separada, f_en_grupos);
#endif

  //#ifdef _MODO_NORMAL
  //poner_pausas(frase_sintagmada);
  //#endif



  trat_fon.atono_ou_tonico_aberto_ou_pechado_e_w_x(f_separada, opciones.idioma);

  construye_frase_fonetica();


#ifndef _CORPUS_PROSODICO
  if ( (opciones.lin==3 || opciones.lin==4 || opciones.lin == 8) && opciones.tra <4 && !(opciones.audio || opciones.wav || opciones.hts)){
    analisis_morfosintactico.sacar_analisis_morfosintactico(frase, f_separada,
							    f_procesada, f_silabas, f_fonetica, f_sintagmada, f_en_grupos,
							    f_prosodica, flin, opciones.fsalida, opciones.lin,opciones.separa_lin,opciones.idioma);
    return 2;
  }
#endif
  if (opciones.tra==4){ //tra
    if (!opciones.hts) transcripcion.vuelca_transcripcion(f_fonetica,f_separada);
    if (!(opciones.audio || opciones.wav || opciones.lin || opciones.hts))
      return 2;
  }

  prosodia.xerar_prosodia(f_fonetica,f_prosodica);


  if (opciones.fon) {
    if (escribe_fonemas_frase(f_prosodica, opciones.fsalida, opciones.fstdout))
      return 1;
    return 2;
  }

#ifndef _SECUENCIAS_FONETICAS
  if (opciones.lin && !opciones.hts) {
    analisis_morfosintactico.sacar_analisis_morfosintactico(frase,
							    f_separada, f_procesada, f_silabas, f_fonetica, f_sintagmada,
							    f_en_grupos, f_prosodica, flin, opciones.fsalida, opciones.lin,opciones.separa_lin,opciones.idioma);
  }
#endif
  saca_tipo_prop(f_fonetica);
  //      fprintf(fichero_sal, "%s\n", frase_fonetica);

  //      if (decide_categorias(f_separada, numero_de_elementos_de_frase_separada))
  //        return ERROR_GENERICO;
#ifdef _CALCULA_DATOS_MODELO_ENERGIA

  int resultado;

  if ( (resultado = crea_descriptores.calcula_datos_modelo_energia(f_separada, f_prosodica, f_fonetica, f_en_grupos, f_sintagmada, nombre_sonidos, nombre_sfs, fichero_fonemas, fichero_marcas, fichero_sonido, frecuencia_muestreo, numero_de_elementos_de_frase_separada, cadena_datos_energia, numero_elementos_energia, correccion_manual)) != 0) {

    FILE *fichero_comprueba;

    if ( (fichero_comprueba = fopen("fallos_comprobacion.txt", "a")) == NULL) {
      fputs("Fallo al abrir el fichero de comprobación.", stderr);
      return 1;;
    }

    switch (resultado) {
    case 1:
      fprintf(fichero_comprueba, "%s.\n", opciones.fentrada);
      break;
    case 2:
      fprintf(fichero_comprueba, "%s.\n", nombre_sonidos);
      break;
    case 3:
      fprintf(fichero_comprueba, "%s.\n", nombre_fonemas);
      break;
    default:
      break;
    }
    fclose(fichero_comprueba);
    return 1;
  }

#endif


#ifdef _PARA_ELISA

  if (crea_descriptores.calcula_duraciones_silabicas(f_prosodica, f_fonetica,
						     fichero_fonemas, fichero_silabas,
						     duracion_silabas, numero_silabas,
						     duracion_fonemas, numero_fonemas,
						     nombre_sfs, nombre_fonemas, correccion_manual) ){
    return 1;
  }

  //        fin_ciclo = 1;

  //    estructura_duracion *recorre_duracion = duracion_fonemas;
  //    float duracion_total = 0.0;
  //    float silabas_por_segundo;
#endif

  // FRAN_CAMPILLO

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  if ( (indice_frase == 51) || (indice_frase == 325) ) {
    indice_frase++;
    return 2;
  } // if (indice_frase == 51)


  if (devuelve_indices_menor_y_mayor(indice_frase, "origen_unidades_Freire.txt", &indice_menor, indice_mayor)){
    return 1;
  }

  printf("Frase = %d.\tÍndice menor = %d.\t Índice mayor: %d.\n", indice_frase,
	 indice_menor, indice_mayor);


  establece_valores_identificadores(indice_menor, indice_mayor);

#endif

#ifdef _CORPUS_PROSODICO

  if (crea_descriptores.genera_corpus_prosodico(frase, f_fonetica, f_separada, corpus_prosodico,
						numero_estructuras, fichero_corpus, fichero_rechazo)) {
    getchar();
    return 1;
  }

  if (*numero_estructuras == 0) {
    puts("Corpus completo.");
    fclose(fichero_corpus);
    getchar();
    *corpus_incompleto = 0;
  }
#endif // ifdef _CORPUS_PROSODICO

#ifdef _SECUENCIAS_FONETICAS

  if (crea_descriptores.seleccion_frases_secuencias_foneticas(frase, f_prosodica, f_separada,
							      fonemas_estacionario, numero_fonemas_estacionario, fonemas_transicion,
							      numero_fonemas_transicion, minimo_numero_palabras,
							      maximo_numero_palabras, (float) porcentaje_secuencias / 100,
							      fichero_corpus, fichero_rechazo)) {
    return 1;
  }

#endif // ifdef _SECUENCIAS_FONETICAS

  // FRAN_CAMPILLO


#ifdef _MODO_NORMAL
  if (!(opciones.audio || opciones.wav || opciones.hts))
    return 2;

  if (opciones.difo == 0) {


    if (frase_solo_signos_de_puntuacion(f_separada))
      return 2;


#ifdef _ESTADISTICA
    Crea_vectores_descriptor crea_descriptores;

    if (crea_descriptores.calcula_estadistica_1_0(f_prosodica, fonema_est, semifonema_izquierdo_est,
						  semifonema_derecho_est, trifonema_est)){
      return 1;
    }

    return 2;

    /*
      if (calcula_estadistica_2_0(f_separada, frase_prosodica, frase_fonetica,frase_en_grupos,
      frase_sintagmada, numero_de_elementos_de_frase_separada,
      &semifonema_izquierdo_est, &semifonema_derecho_est,
      &trifonema_est)) {
      return 1;
      }
    */
#endif

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL
    if (crea_descriptores.crea_vectores_descriptor(frase,f_separada, f_prosodica, f_fonetica,f_en_grupos, f_sintagmada, (float) opciones.et/100, (float) opciones.ep/100, opciones.difo, numero_de_elementos_de_frase_separada, &locutor_actual->modelo_energia, locutor_actual->modelo_duracion, cadena_unidades, &numero_unidades, 
#ifdef _MINOR_PHRASING_TEMPLATE
						   &frase_frontera_obj,
#endif
						   &acentuales_obj, 
						   &numero_acentuales))
#else
      if (crea_descriptores.crea_vectores_descriptor(frase,f_separada, f_prosodica, f_fonetica,f_en_grupos, f_sintagmada, (float) opciones.et/100, (float) opciones.ep/100, opciones.difo, numero_de_elementos_de_frase_separada, locutor_actual->red_energia, locutor_actual->estadisticos_energia, locutor_actual->modelo_duracion, cadena_unidades, &numero_unidades,
#ifdef _MINOR_PHRASING_TEMPLATE
						     &frase_frontera_obj,
#endif
						     &acentuales_obj, &numero_acentuales))

#endif
	{
	  sprintf(nombre_fichero_salida, "%sFrases_errores.txt", opciones.dir_sal);
	  if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
	    fprintf(stderr, "Error al intentar abrir el fichero de frases que provocan errores.\n");
	    return 1;
	  } // if ( (fichero_errores = fopen...

	  fprintf(fichero_errores, "%s\n", frase);
	  fclose(fichero_errores);
	  return 3;
	} // if (crea_vectores_descriptor...

	if (opciones.hts) {
		print_hts_info();
	}

  }

  if (opciones.info_unidades == 4) {
    // Generamos el fichero con la división en grupos acentuales
    char fichero_salida[FILENAME_MAX];
    
    sprintf(fichero_salida, "%s.gac", opciones.fsalida);

    if (escribe_secuencia_grupos_acentuales(numero_acentuales, numero_unidades,
					    fichero_salida, f_fonetica,
					    cadena_unidades,
					    primera_frase == 1))
      return 1;
    
    
  } // if (opciones.info_unidades == 4)

#ifdef _MINOR_PHRASING_TEMPLATE
  /*
    for (unsigned int c = 0; c < frase_frontera_obj.elementos.size(); c++) {
    printf("Frase %d:\n", c);
    frase_frontera_obj.elementos[c].escribe_datos_txt(stdout);
    printf("\n\n");
    }
  */
#endif


#endif // ifdef _MODO_NORMAL

  primera_frase++;

  return 0;
}


/** 
 * \brief Libera memoria reservada en generacion_prosodia (crea_descriptores)
 * \author fmendez
 */
void Cotovia::libera_memoria_prosodia(void){

  int contador;
  //  Vector_descriptor_objetivo * vector_auxiliar = cadena_unidades;

  // Esto quedaría más elegante con el destructor...
  for (vector<Vector_descriptor_objetivo>::iterator it = cadena_unidades.begin();
       it != cadena_unidades.end(); it++) {
    //  for (contador = 0; contador < numero_unidades; contador++, vector_auxiliar++){
    it->libera_memoria();
    //vector_auxiliar->libera_memoria();
  }

  cadena_unidades.clear();

  //  free(cadena_unidades);
  //cadena_unidades = NULL;
  numero_unidades = 0;

  Grupo_acentual_objetivo * auxiliar_acentual = acentuales_obj;

  for (contador = 0; contador < numero_acentuales; contador++, auxiliar_acentual++){
    auxiliar_acentual->libera_memoria();
  }

  free(acentuales_obj);
  acentuales_obj = NULL;
  numero_acentuales = 0;

}

/**
 * \brief Vuelca en el fichero de salida los datos de los fonemas necesarios para HTS
 * \author fmendez
 *
 */

void Cotovia::print_hts_info(void){
 	char cadena_categoria[50];


	
	char aux[FILENAME_MAX];
	
	if (flin==NULL) {
		strcpy(aux,opciones.fsalida);
		strcat(aux,".hts");
		if ((flin=fopen(aux,"w"))==NULL) {
			fprintf(stderr,"\nO arquivo de informacion linguistica \"%s\" non se pode crear",aux);
			return;
		}
	}	
	
	
	transcripcion.divide_frase_fonetica(f_fonetica,f_separada);
	fprintf(flin,"phoneme syll stress word class interr\n");
	fprintf(flin,"sil\n");
//	for( int k = 1 ; k < numero_unidades-1 ; k++ ){
    for (vector<Vector_descriptor_objetivo>::iterator it = cadena_unidades.begin();
       it != cadena_unidades.end(); it++) {

		if (*it->semifonema == '#' ) {
			fprintf(flin,"pau\n");
		}
		else {
			fprintf(flin,"%s", it->semifonema);
			if (it->frontera_inicial) {
				fprintf(flin," %s %d", it->silaba_hts, it->acento);
			}

			if (it->frontera_inicial > 3) {
				fprintf(flin," %s ", it->palsep->tr_fon);
				//fprintf(flin,"%s\t%s\n", it->palsep->pal, it->palsep->tr_fon); //chimarme estp
				escribe_tipo_categoria_morfosintactica_grupo((int) decide_categoria_morfosintactica_grupo_acentual(it->palsep), cadena_categoria);
				fprintf(flin,"%s", cadena_categoria);


			 switch (it->frase){
					case 0:
						fprintf(flin," enu");
						break;
					case 1:
						fprintf(flin," exc");
						break;
					case 2:
						fprintf(flin," int");
						break;
					case 3:
						fprintf(flin," sus");
						break;
					default:
						fprintf(flin," enu");
						break;
				}

			}	
			fprintf(flin,"\n");


		}

	}

	fprintf(flin,"sil\n");

}

