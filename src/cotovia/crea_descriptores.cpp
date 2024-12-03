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
#include <limits.h>

#include "modos.hpp"
#include "tip_var.hpp"
//#include "cotovia.hpp"
#include "prosodia.hpp"

#include "descriptor.hpp"
#include "frecuencia.hpp"
#include "indices.hpp"
#include "estadistica.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "sintagma.hpp"
#include "info_estructuras.hpp"
#include "trat_fon.hpp"
#include "crea_descriptores.hpp"



/**
 * \author	Fran Campillo
 * \remarks	Constructor de la clase Crea_vectores_descrpiptor
 */

Crea_vectores_descriptor::Crea_vectores_descriptor() {

#if defined(_GRUPOS_ACENTUALES) || defined(_MODO_NORMAL)

  grupo_acentual_actual = 0;
  numero_elementos_grupo_acentual = 0;
  numero_silabas_grupo_acentual = 0;
  numero_grupos_acentuales = 0;
  grupos_prosodicos_total = 0;
  grupo_prosodico_actual = -1;

#endif

#ifdef _GRUPOS_ACENTUALES
  cadena_fonemas = NULL;
  numero_fonemas = 0;
#endif
#ifdef _CALCULA_DATOS_MODELO_ENERGIA
  cadena_fonemas = NULL;
  numero_fonemas = 0;
  muestras_tiempo = NULL;
  numero_muestras = 0;
  cadena_energia = NULL;
  frase = NULL;
  original_marcas = NULL;
  numero_marcas = 0;
#endif

#ifdef _SCRIPT_DURACION
  cadena_fonemas = NULL;
  numero_fonemas = 0;
  frase = NULL;
#endif

#ifdef _SCRIPT

  cadena_vectores_cepstrum = NULL;
  numero_vectores_cepstrum = 0;
  muestras_tiempo = NULL;
  numero_muestras = 0;
  cadena_fonemas = NULL;
  numero_fonemas = 0;
  original_marcas = NULL;
  numero_marcas = 0;
  cadena_cepstrum = NULL;
  numero_cepstrum = 0;

#ifdef _PARA_ELISA
  cadena_fonemas_fichero = NULL;
  numero_fonemas = 0;
#endif

#endif

#ifndef _GRUPOS_ACENTUALES

  fragmentos_entre_pausas = 0;
  fragmento_actual = -1;
  numero_elementos = 0;
  frase = NULL;

#endif

}


/**
 * \author	Fran Campillo
 * \remarks	Destructor de la clase Crea_vectores_descrpiptor
 */

Crea_vectores_descriptor::~Crea_vectores_descriptor() {

#ifdef _GRUPOS_ACENTUALES
  if (cadena_fonemas != NULL)
    free(cadena_fonemas);
#endif

#ifdef _CALCULA_DATOS_MODELO_ENERGIA
  if (cadena_fonemas != NULL)
    free(cadena_fonemas);

  if (original_marcas != NULL)
    free(original_marcas);

  if (muestras_tiempo != NULL)
    free(muestras_tiempo);

  if (cadena_energia != NULL)
    free(cadena_energia);

  if (frase)
    free(frase);
#endif

#ifdef _SCRIPT_DURACION
  if (cadena_fonemas != NULL)
    free(cadena_fonemas);

  if (frase)
    free(frase);
#endif

#ifdef _SCRIPT

  if (cadena_vectores_cepstrum != NULL)
    free(cadena_vectores_cepstrum);

  if (muestras_tiempo != NULL)
    free(muestras_tiempo);

  if (cadena_fonemas != NULL)
    free(cadena_fonemas);

  if (original_marcas != NULL)
    free(original_marcas);

  if (cadena_cepstrum != NULL)
    free(cadena_cepstrum);

  if (frase)
    free(frase);
#endif

#ifdef _PARA_ELISA
  if (cadena_fonemas_fichero != NULL)
    free(cadena_fonemas_fichero);
#endif

}

#ifndef _GRUPOS_ACENTUALES

/**
 * Función:   inicializa_calculo_posicion_y_tonicidad
 * \remarks	  Inicia los valores de las variables fragmentos_entre_pausas y      
 *            fragmento_actual. Se emplea al inicio de cada frase.               
 */

void Crea_vectores_descriptor::inicializa_calculo_posicion_y_tonicidad() {

  fragmentos_entre_pausas = 0;
  fragmento_actual = -1;
  numero_elementos = 0;

  if (frase) {
    free(frase);
    frase = NULL;
  }
}

/**
 * Función:   reinicia_variables_posicion_y_tonicidad
 * \remarks   Inicia los valores del resto de las variables estáticas relacionadas
 *            con el cálculo de la posición y la tonicidad. Se diferencia de la
 *            función anterior en que a ésta hay que llamarla al inicio de cada
 *            grupo entre pausas.                                                  
 */

void Crea_vectores_descriptor::reinicia_variables_posicion_y_tonicidad() {

  tonicidad = 0;
  tamano_silaba = 0;

  primer_acento = 0;
  segundo_acento = 0;

  numero_elementos = 0;
  cuenta_silabas = -1;

  if (frase) {
    free(frase);
    frase = NULL; // Actualmente no sería necesario, pero por compatibilidad hacia delante.
  }
}

#endif

#if defined(_SCRIPT_DURACION) || defined(_MODO_NORMAL) || defined(_SCRIPT) || \
  defined(_CALCULA_DATOS_MODELO_ENERGIA)

/**
 * Función:   reinicia_variables_posicion_silabica
 * \remarks   Inicia los valores de las variables relacionadas con la posición
 *            silábica dentro del grupo entre pausas.
 */

void Crea_vectores_descriptor::reinicia_variables_posicion_silabica() {

  posicion_en_silaba = 0;
  tamano_palabra = 0;
  numero_silabas_palabra = 0;
  silabas_totales = 0;
  tonicas_totales = 0;
  tonica_actual = 0;
  numero_fonemas_silaba = 0;
}

#endif

/**
 * Función:   crea_nombre_fichero_sonido
 * \remarks	  Extrae el nombre del fichero en el que se encuentra la unidad,
 *            eliminando la ruta, para que ésta no tenga por qué ser fija.
 * Entrada y Salida
 *          - nombre_salida: nombre del fichero de salida, sin la ruta.
 */

void crea_nombre_fichero_sonido(char nombre_salida[]) {

  char *auxiliar = nombre_salida, *anterior = nombre_salida;

  while ( (auxiliar = strstr(auxiliar, "\\")) != NULL) {
    auxiliar++;
    anterior = auxiliar;
  }

  strcpy(nombre_salida, anterior);
}

/**
 * Función:   construye_nombre_sfs
 * Entrada y salida:                                                               
 *          - cadena: cadena con la ruta al fichero de sonido.                     
 * \remarks Objetivo:  Crear el nombre del fichero .sfs, para generar el script de errores  
 *            de etiquetado.                                                       
 */

void Crea_vectores_descriptor::construye_nombre_sfs(char **cadena) {

  char *auxiliar = *cadena, *anterior = *cadena;

  while ( (auxiliar = strstr(auxiliar, "\\")) != NULL) {
    auxiliar++;
    anterior = auxiliar;
  }

  auxiliar = strstr(anterior, ".");
  if (auxiliar) {
    strcpy(auxiliar, ".sfs");
    *cadena = anterior;
  } // if (auxiliar)
  else {
    *cadena = anterior;
    strcat(*cadena, ".sfs");
  } // else

}

#if defined(_SCRIPT)

/**
 * Función:   crea_vectores_descriptor
 * Versión:   entrena_corpus.
 * \remarks Entrada:
 *			- frase_separada: cadena que contiene la información de cada palabra procesada.
 *            Se emplea para obtener la información de palabra contexto y tipo de frase.
 *          - frase_prosodica: cadena en la que se encuentra la información de cada fonema
 *            por separado.                                                                  
 *          - frase_en_grupos: división en grupos de la frase de entrada.
 *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
 *            tonicidad y de pausas.                                                         
 *          - frase_sintagmada: división en sintagmas de la frase de entrada.
 *          - nombre_fichero_sfs: nombre del fichero sfs, para la posterior corrección  de
 *            errores.                                                                       
 *          - fichero_fonemas: fichero en el que se encuentra la información del etiquetado
 *            temporal de los fonemas.                                                       
 *          - fichero_marcas: fichero en el que se encuentra la información de las marcas de
 *            pitch.                                                                         
 *          - fichero_sonido: fichero de datos de audio "en bruto", empleado para el cálculo
 *            de la energía de cada fonema.
 *          - fichero_cepstrum: fichero con la información de los coeficientes cepstrales
 *            calculados en ventanas centradas en las marcas de pitch del fichero.           
 *          - fichero_cepstrum_bin: fichero con la información de los coeficientes
 *            cepstrales calculados cada 0.008 milisegundos.
 *			- frecuencia_muestreo: frecuencia de muestreo de los ficheros de sonido.		 
 *			- numero_de_elementos_de_frase_separada: número de elementos de la variable
 *			  frase_separada.																 
 *			- frecuencia_minima_locutor: frecuencia fundamental mínima admitida para ese
 *			  locutor.																		 
 *			- frecuencia_maxima_locutor: frecuencia fundamental máxima admitida para ese
 *			  locutor.
 *			- correccion_manual: si es cero, el corpus no fue corregido manualmente.																		 *                      - nombre_fichero_origen_configuracion: nombre del fichero de origen de unidades, tal y como aparece en el fichero de configuración.
 *	\remarks Salida:
 *			- fichero_bin: fichero en el que se escribe la información de los vectores
 *            descriptor.                                                                    
 * \remarks Valor devuelto:                                                                           
 *          - En ausencia de error devuelve un 0. Si hay algún fallo en la comprobación del
 *            fichero de marcas, devuelve un 2. Si es en el fichero de sonido, un 3.         
 * \remarks Objetivo:  Calcula las características de los fonemas que se encuentran en el corpus,
 *            para su posterior uso en el modo de funcionamiento normal del programa, es     
 *            decir, en la síntesis.
 */

int Crea_vectores_descriptor::crea_vectores_descriptor(t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, char * locutores, t_frase_sintagmada *frase_sintagmada, char *fichero_origen, char *nombre_fonemas, char *nombre_txt, char *nombre_txt_morf, char *nombre_fichero_sfs, FILE *fichero_fonemas, FILE *fichero_marcas, FILE *fichero_sonido, FILE *fichero_cepstrum, FILE *fichero_cepstrum_bin, int frecuencia_muestreo, float frecuencia_minima_locutor, float frecuencia_maxima_locutor, int numero_de_elementos_de_frase_separada, FILE *fichero_bin, int correccion_manual, char *nombre_fichero_origen_configuracion) {

  static int numero_semifonema = 0; // Variable que se emplea para asignar un identificador numérico
  // único a cada unidad.
  static int numero_lecturas = 0;
    
  FILE *fichero_inicio_numero; // Fichero en el que se almacena el primer identificador único
  // que asignaremos a la primera unidad del fichero de texto.
  static char primera_vez = 2; // Variable estática que controla la apertura de los ficheros de
  // sonido, marcas y fonemas.

  static int primera_frase = 1; // Variable que indica si se trata de la primera frase del fichero.
    
  static char primer_fonema_frase = 1;
  static int origen;
  static tabla_ficheros indice_de_fichero;

  static int posicion_fichero_cepstrum = 0;

  FILE *fichero_situacion_unidades = NULL; // Fichero en el que se almacena el origen de cada unidad.
#ifdef _ESCRIBE_SITUACIONES_ANOMALAS
  FILE *fichero_anomalo;   // Fichero en el que se almacenan las situaciones anómalas.
#endif
  int resultado_anomalo;

  // Punteros a las diferentes cadenas.
  t_frase_separada *palabra_separada = frase_separada;
  t_prosodia *correcaminos = frase_prosodica;
  char *f_fonetica = frase_fonetica;
  t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;

  t_palabra_proc palabra_contexto= ""; // Variable en la que se almacena la palabra a la que pertenece
  // la unidad actual.
  char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#"; // Alófonos por la derecha e izquierda, tal y como los estima Cotovia.
  char derecha_corregida[OCTETOS_POR_FONEMA + 1], derecha_corregida_2[OCTETOS_POR_FONEMA + 1]; // Alófonos reales a los que se le ha quitado una posible
  // marca de corrección. (como !, ver ficheros de fonemas).
  char izquierda_corregida[OCTETOS_POR_FONEMA + 1], izquierda_corregida_2[OCTETOS_POR_FONEMA + 1];
  char actual_corregido[OCTETOS_POR_FONEMA + 1]; // Lo mismo de arriba.
  int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
  int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.
  Vector_descriptor_candidato nuevo_vector; // Objeto en el que se almacena la información de cada unidad.

  // Variables para determinar el tipo de frase al que pertenece la unidad.
  unsigned char final_tonema_frase_separada; // Índice en el que se termina el tonema en el que nos encontramos.
  unsigned char tipo_de_tonema; // Sólo consideramos cuatro: ENUNCIATIVA, INTERROGATIVA, EXCLAMATIVA E INACABADA

#ifndef _OBVIA_FRECUENCIA
  marca_struct *cadena_marcas;
  //   static marca_struct *cadena_marcas, *original_marcas; // Cadena en la que almacenamos la información de marcas de pitch.
  //   static int numero_marcas; // Número de elementos de la cadena anterior.
#endif

  static estructura_fonemas *fonema_actual;

  static estructura_fonemas *cont_derecha_real, *cont_izquierda_real; // Alófonos, tal y como vienen en el fichero de etiquetas.
  static estructura_fonemas *cont_derecha_real_2, *cont_izquierda_real_2; // El siguiente y el anterior, respecto a los anteriores.

  static float tiempo_final_sonido; // Instante de finalización real en el fichero de sonido.

#ifndef _OBVIA_FRECUENCIA
  float tiempo_ultima_marca; // Posición de la última marca.
#endif

  char incluye_fonema = 1; // Variable binaria empleada para controlar posibles desfases entre la
  // transcripción de Cotovia y la del fichero de fonemas.

  char primera = 1; // Variable binaria que controla la presencia de un silencio al inicio de la transcripción.

  // Variables destinadas al control de aquellas palabras que tienen el campo pal_transformada != NULL
  int numero_palabra = 0; // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
  // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
  char ciclo_transformada = 0;
  char no_final_ciclo = 1;
  t_palabra_proc parte_transformada, parte; // Variables en las que se almacena la palabra transformada completa
  // y la palabra dentro de ella a la que nos referimos.

  unsigned char posicion_frase = 0; // Posición del fonema dentro de la frase (inicial, media o final).
  unsigned char tonicidad = 0; // Variable que indica si una sílaba es tónica o no.
  short int numero_fonemas_silaba;
  short int posicion_en_silaba;
  short int numero_silabas_desde_inicio;
  short int numero_silabas_hasta_final;
  short int numero_tonicas_desde_inicio;
  short int numero_tonicas_hasta_final;
  char nueva_silaba, fin_silaba;
  char frontera_inicial = 0;
  char frontera_final = 0;
  short int numero_silabas_palabra;
  unsigned char indice_silaba_palabra;

  clase_espectral tipo_espectral; // tipo de fonema según la clasificación de clase_espectral.
  clase_duracion tipo_duracion;   // tipo de fonema según la clasificación de clase_duracion.
    
  char fonema_no_pronunciado = 0; // Variable que controla si tenemos que avanzar el puntero a los
  // fonemas transcritos por Cotovía, debido a que el locutor no ha pronunciado dicho fonema.
  char cuenta_hasta_salto_1 = 0; // Variable que trabaja con la anterior para avanzar en el momento adecuado el
  char cuenta_hasta_salto_2 = 0; // Variable que permite que haya varios sonidos consecutivos que no se leen.
  // puntero a los fonemas transcritos por Cotovía, cuando hay que hacer algún cambio.
  char final_frase;
  int fonemas_iguales; // Variable que sirve para comparar si el fonema real actual y el de Cotovía son iguales.
  int resultado;

  // Contador de posición del alófono considerado. Se emplea para almacenar la posición en el
  // fichero de fonemas del lugar en el que hay un error de transcripción.

  static int indice_alofono_actual = 1;

  // Comprobamos que no se trate de una frase con sólo signos de puntuación:

  if (frase_solo_signos_de_puntuacion(frase_separada))
    return 0;

  if (primera_vez == 2) {

    primera_vez = 1;
    primer_fonema_frase = 0;

    // Cargamos el índice al inicio de la frase dentro del fichero de sonido único.
    FILE *fichero_indices;
    //    	int numero_fichero;
    //    	int fre = 1;

    //    	extrae_identificador(fichero_origen, &numero_fichero);

    //    	if ( (strstr(fichero_origen, "500")) || (strstr(fichero_origen, "marga")) )
    //        	fre = 0;

    char nombre_fichero[FILENAME_MAX];

    sprintf(nombre_fichero, "indices_ficheros_de_sonido_%s.bin", locutores);

    if ( (fichero_indices = fopen(nombre_fichero, "rb")) == NULL) {
      fprintf(stderr, "Error en crea_vectores_descriptor, al intentar abrir el fichero de índices de sonido %s.\n", nombre_fichero);
      return 1;
    } // if ( (fichero_indices = ...

    // Pasamos del valor devuelto porque así es compatible con la ejecución de un único fichero (no modo batch).
        
    encuentra_indice_frase_en_fichero(fichero_origen, fichero_indices, &indice_de_fichero);

    //    	fseek(fichero_indices, calcula_indice_en_fichero(numero_fichero,fre) , SEEK_SET);

    //    	fread(&indice_de_fichero, sizeof(tabla_ficheros), 1, fichero_indices);

    fclose(fichero_indices);

    origen = indice_de_fichero.posicion_fichero;

#ifndef _OBVIA_FRECUENCIA
    // Cargamos en memoria el contenido del fichero de marcas de pitch.

    if (crea_cadena_fichero(&original_marcas, fichero_marcas, &numero_marcas, &tiempo_ultima_marca))
      return 1;

    //       	original_marcas = cadena_marcas;

    fclose(fichero_marcas);
    fichero_marcas = NULL;
#endif

 
    // Cargamos en memoria el contenido del fichero de la forma de onda.
    // Si no se desea calcular la frecuencia, la propia función se encarga de devolver
    // sólo el valor de muestras_tiempo, necesario para saber la duración de la última
    // unidad.

    if (carga_fichero_coeficientes_espectrales(&cadena_cepstrum, fichero_cepstrum,  Vector_descriptor_candidato::numero_coeficientes_cepstrales, &numero_cepstrum))
      return 1;

    //       	if (carga_fichero_cepstrum_bin(&cadena_vectores_cepstrum, fichero_cepstrum_bin,
//     if (carga_fichero_cepstrum_txt(&cadena_vectores_cepstrum, fichero_cepstrum_bin,
// 				   &numero_vectores_cepstrum, Vector_descriptor_candidato::numero_coeficientes_cepstrales))
//       return 1;

    if (carga_vector_muestras(frecuencia_muestreo, &muestras_tiempo, &numero_muestras,
			      &tiempo_final_sonido, fichero_sonido))
      return 1;

    // Cargamos en memoria el contenido del fichero de fonemas:

    if (cadena_fonemas == NULL) {

      if (carga_fichero_fonemas(&cadena_fonemas, fichero_fonemas, &numero_fonemas, 1))
	return 1;

      fonema_actual = cadena_fonemas;
      cont_izquierda_real = fonema_actual;
      cont_izquierda_real_2 = fonema_actual;
      cont_derecha_real = fonema_actual + 1;
      cont_derecha_real_2 = cont_derecha_real + 1;
    } // if (fonema_actual == NULL...

#ifndef _OBVIA_FRECUENCIA
    /* Ahora tenemos el contenido del fichero de marcas indexado en cadena_marcas. Hay dos opciones:
       o consideramos que el fichero de marcas es correcto, es decir, que hay marcas para todos los
       fonemas, o bien en cada incremento del índice comprobamos que no nos salimos de la zona que hemos
       reservado para la cadena. Para evitar hacer estas suposiciones, y no tener que hacer la comprobación
       cada vez, insertamos una función intermedia que compara la última marca de pitch con la última marca
       de fonema. Para ello tenemos que leer entero el fichero de fonemas (no creo que sea necesario cargarlo
       en una cadena, como el de marcas, ya que sólo tendríamos que rebobinarlo una vez), lo que no es demasiado
       elegante, pero es bastante cómodo.
    */
    if (comprueba_validez_fichero_marcas(original_marcas, numero_marcas,
                                         (cadena_fonemas + numero_fonemas - 3)->inicio))
      return 2;

#endif

    // Hacemos lo mismo con el fichero de sonido

    if (comprueba_validez_fichero_sonido(numero_muestras, (cadena_fonemas + numero_fonemas - 3)->inicio,
					 frecuencia_muestreo, &tiempo_final_sonido))
      return 3;


#ifndef _OBVIA_FRECUENCIA
    if (tiempo_final_sonido > tiempo_ultima_marca)
      tiempo_final_sonido = tiempo_ultima_marca - FLT_MIN;
#endif

    // Le añadimos el tiempo final al último silencio del fichero.
    /*        if (tiempo_final_sonido > (cadena_fonemas + numero_fonemas - 3)->inicio + 0.5) {
	      (cadena_fonemas + numero_fonemas - 2)->inicio = (cadena_fonemas + numero_fonemas - 3)->inicio + 0.5;
	      (cadena_fonemas + numero_fonemas - 1)->inicio = (cadena_fonemas + numero_fonemas - 2)->inicio;
	      }
	      else {
    */
    (cadena_fonemas + numero_fonemas - 2)->inicio = tiempo_final_sonido;
    (cadena_fonemas + numero_fonemas - 1)->inicio = tiempo_final_sonido;
    //        }

  } // if (primera_pausa == 2)


  // Creamos la ventana con la que iremos calculando la potencia de la señal.

  if (inicia_ventana("hamming", (int) (TAMANO_VENTANA*frecuencia_muestreo + 1)))
    return 1;

  // Estas variables son necesarias para determinar el tipo de frase a la que pertenece
  // cada unidad.

  final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;

  tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);


  // Inicializamos las variables necesarias para el cálculo de la posición en la frase
  // y la tonicidad.

  inicializa_calculo_posicion_y_tonicidad();
  /*
    FILE *fichero_bin;

    if ( (fichero_bin = fopen("descriptores.uni_bin", "ab")) == NULL) {
    fprintf(stderr, "Error en crea_descriptores, al abrir el fichero de salida binario.\n");
    return 1;
    }
  */
  // Si estamos en el modo _script, leemos el primer identificador numérico de un fichero.
  if (numero_semifonema == 0)
    if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "rt")) != NULL) {
      fscanf(fichero_inicio_numero, "%d", &numero_semifonema);
      fclose(fichero_inicio_numero);
    }

  FILE *fichero_cambia_real;

  if ( (fichero_cambia_real = fopen("cambia_transcripcion.txt", "a")) == NULL) {
    fprintf(stderr, "Error en crea_vectores_descriptor, al abrir el fichero cambia_transcripcion.\n");
    fclose(fichero_bin);
    return 1;
  }

  // Construimos el nombre del fichero sfs.
  //   construye_nombre_sfs(&nombre_fichero_sfs);

  // Abrimos el fichero de orígenes de unidades.

  char nombre_fichero_origen[FILENAME_MAX];

  if (strstr(nombre_fichero_origen_configuracion, ".") == NULL)
    sprintf(nombre_fichero_origen, "%s%s.txt", nombre_fichero_origen_configuracion, locutores);
  else {
    char *apunta_nombre;
    strcpy(nombre_fichero_origen, nombre_fichero_origen_configuracion);
    apunta_nombre = strstr(nombre_fichero_origen, ".");
    sprintf(apunta_nombre, "_%s.txt", locutores);
  }
    

  if ( (fichero_situacion_unidades  =  fopen(nombre_fichero_origen, "at")) == NULL) {
    fprintf(stderr, "Error en crea_vectores_descriptor, al abrir el fichero %s.\n", nombre_fichero_origen);
    return 1;
  }



#ifdef _ESCRIBE_SITUACIONES_ANOMALAS
  // Abrimos el fichero de situaciones anómalas:

  if ( (fichero_anomalo = fopen("salida_anomala.txt", "at")) == NULL) {
    fprintf(stderr, "Error en crea_vectores_descriptor, al intentar abrir el fichero de situaciones anómalas.\n");
    return 1;
  }
#endif

  cont_derecha_real = fonema_actual + 1;

  while ( (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '*') ||
	  (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '+') ||
	  (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '-') ) {
    cont_derecha_real++;
    cuenta_hasta_salto_1 = 2;
    fonema_no_pronunciado++;
    indice_alofono_actual++;
    //            numero_lecturas++;
  }

  cont_derecha_real_2 = cont_derecha_real + 1;

  while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	  (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+') ||
	  (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') )  {
    cont_derecha_real_2++;
    fonema_no_pronunciado++;
    cuenta_hasta_salto_1 = 3;
    indice_alofono_actual++;
  }

  cadena_marcas = original_marcas; // Necesario para evitar desincronización en los ficheros
  // del corpus de más de una frase.

  if (posicion_fichero_cepstrum != 0) {
    if (fseek(fichero_cepstrum, SEEK_SET, posicion_fichero_cepstrum)
	== -1) { // OJO , chapuza gorda. Esto no se hace. Menos mal que solo es para
      // la versión _SCRIPT
      char chapuza;
      fseek(fichero_cepstrum, SEEK_SET, 0);
      for (int i = 0; i < posicion_fichero_cepstrum; i++)
	fread(&chapuza, 1, 1, fichero_cepstrum);
    }
  }


  // Entramos en el bucle principal.

  while (correcaminos->alof[0] != '\0') {

    // Reservamos memoria para los coeficientes cepstrum:

    if ( (nuevo_vector.cepstrum = (float *) malloc(4*Vector_descriptor_candidato::numero_coeficientes_cepstrales*sizeof(float))) == NULL) {
      fprintf(stderr, "Error en crea_descriptores, al asignar memoria.\n");
      return 1;
    }

    if ( (correcaminos->alof[0] == '#') && ((correcaminos+1)->alof[0] == '#') ) {
      correcaminos++;
      continue;
    }

    if (calcula_posicion_frase_tonicidad_silabas(f_fonetica, correcaminos->alof, &posicion_frase, &tonicidad, &numero_fonemas_silaba, NULL, &numero_silabas_palabra, &posicion_en_silaba, &numero_silabas_desde_inicio, &numero_silabas_hasta_final,&numero_tonicas_desde_inicio, &numero_tonicas_hasta_final, &nueva_silaba, &fin_silaba, NULL, &frontera_inicial, &frontera_final, &final_frase, &indice_silaba_palabra)) {

      fclose(fichero_bin);
      if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	fprintf(fichero_inicio_numero, "%d", numero_semifonema + 1);
	fclose(fichero_inicio_numero);
      }
      fclose(fichero_situacion_unidades);
      return 1;
    } // if (calcula_posicion_frase_tonicidad_silabas.


    //    fwrite(&indice_de_fichero, sizeof(tabla_ficheros), 1, fichero_situacion_unidades);

    /*
    // Añadimos el nombre del fichero (común para todos los alófonos de la frase).

    if (nuevo_vector.anhade_nombre_fichero(fichero_origen)) {
    fclose(fichero_bin);
    #ifdef _SCRIPT
    if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
    fprintf(fichero_inicio_numero, "%d", numero_semifonema);
    fclose(fichero_inicio_numero);
    }
    #endif
    return 1;
    }
    */

    // Comprobamos que no haya ningún problema de pronunciación del locutor o de etiquetado:

    //      resultado = 1;
    fonemas_iguales = strcmp(correcaminos->alof, fonema_actual->fonema);

    if (fonemas_iguales && (fonema_no_pronunciado == 0) ) {
      if ( (resultado = comprueba_error_etiquetado(correcaminos->alof, fonema_actual->fonema, correccion_manual)) == 1) {
	FILE *fichero_etiquetado;
	if ( (fichero_etiquetado = fopen("errores_etiquetado.txt", "a")) == NULL) {
	  fprintf(stderr, "Error al abrir el fichero de errores de etiquetado.\n");
	  fclose(fichero_bin);
	  if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	    fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	    fclose(fichero_inicio_numero);
	  }
	  fclose(fichero_situacion_unidades);
	  return 1;
	} // if (fichero_etiquetado = ...
	fprintf(fichero_etiquetado, "#\t%s.\n#\tCotovía: %s.\tReal: %s.\n", fichero_origen,
		correcaminos->alof, fonema_actual->fonema);
	fprintf(fichero_etiquetado, "\tgvim -o %s %s %s\n", nombre_fonemas, nombre_txt, nombre_txt_morf); 
	//fprintf(fichero_etiquetado "\tEs -s %f -e %f %s &\n", fonema_actual->inicio - 0.3, fonema_actual->inicio + 0.3, nombre_fichero_sfs);
	fprintf(fichero_etiquetado, "\tEs -isp -gsp -i 5.01 -i 5.02 -s %f -e %f %s &\n", fonema_actual->inicio - 0.3, fonema_actual->inicio + 0.3, nombre_fichero_sfs);
	fclose(fichero_bin);
	if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	  fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	  fclose(fichero_inicio_numero);
	}
	fclose(fichero_situacion_unidades);
	return 1;
      } // if (resultado = comprueba...
      else
	if (resultado == 2)
	  fprintf(fichero_cambia_real, "%s %s %d\n", fichero_origen, correcaminos->alof,
                  indice_alofono_actual);
    } // if ( (fonemas_iguales...


    if (!((fonema_actual->fonema[0] == '#') && (correcaminos->alof[0] != '#'))) {
      if ( (correcaminos->alof[0] == '#') || primer_fonema_frase) {
	primer_fonema_frase = 0;
	if (primera) {
	  // Se trata del primer silencio del fichero (el inicial).
	  primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) {
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Cambié el > por >=.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	      fclose(fichero_bin);
	      if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
		fprintf(fichero_inicio_numero, "%d", numero_semifonema);
		fclose(fichero_inicio_numero);
	      }
	      fclose(fichero_situacion_unidades);
	      return 1;
	    } // if (++elementos_de_frase_separada...
	    palabra_separada++; // Avanzamos palabra_separada, ya que no es una palabra, sino
	    // un signo de puntuación.
	  } // while (palabra_separada...
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...
	  if (palabra_separada->pal_transformada) {
	    // Entramos en un ciclo de palabra transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      fclose(fichero_bin);
	      if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
		fprintf(fichero_inicio_numero, "%d", numero_semifonema);
		fclose(fichero_inicio_numero);
	      }
	      fclose(fichero_situacion_unidades);
	      return 1;
	    } // if (no_final_ciclo...
	    strcpy(palabra_contexto, parte);
	  } // if (palabra_separada->...
	  else
	    // No hay palabra transformada -> La palabra contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  if (correcaminos->alof[0] == '#')
	    anterior_orden++;
	} // if (primera)...
      }  // if (correcaminos->alof[0] == '#') ||primer_fonema_frase...

      // Comprobamos si estamos en un ciclo de palabra transformada.
      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Cambiamos de palabra, pero dentro de un ciclo de palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    anterior_orden = correcaminos->pp;
	  else
	    ciclo_transformada = 0;
	} // if (correcaminos->pp > anterior_orden...
	if (no_final_ciclo) {
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (char)
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	    fclose(fichero_bin);
	    if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	      fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	      fclose(fichero_inicio_numero);
	    }
	    fclose(fichero_situacion_unidades);
	    return 1;
	  } // if (no_final_ciclo = (char)
	  strcpy(palabra_contexto, parte);
	} // if (no_final_ciclo)
      } // if (ciclo_transformada)

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;
	  while (palabra_separada->clase_pal == SIGNO_PUNTUACION) {
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	      fclose(fichero_bin);
	      if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
		fprintf(fichero_inicio_numero, "%d", numero_semifonema);
		fclose(fichero_inicio_numero);
	      }
	      fclose(fichero_situacion_unidades);
	      return 1;
	    } // if (++elementos_de_frase_separada...
	    palabra_separada++;
	  } // while (palabra_separada->clase_pal...
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...

	  anterior_orden = correcaminos->pp;
	} // if (elementos_de_frase_separada++ < numero_...
	else {
	  fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	  fclose(fichero_bin);
	  if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	    fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	    fclose(fichero_inicio_numero);
	  }
	  fclose(fichero_situacion_unidades);
	  return 1;
	} // else

	if (palabra_separada->pal_transformada) {
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      fclose(fichero_bin);
	      if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
		fprintf(fichero_inicio_numero, "%d", numero_semifonema);
		fclose(fichero_inicio_numero);
	      }
	      fclose(fichero_situacion_unidades);
	      return 1;
	    }
	    strcpy(palabra_contexto, parte);
	  } // if ( (no_final_ciclo...
	} //if (palabra_separada->pal_transformada...
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      } // if (correcaminos->pp > anterior_orden...


      // Actualizamos el contexto por la derecha.

      if ( (correcaminos + 1)->alof[0] )
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");
      incluye_fonema = 1;
    }


    if (cuenta_hasta_salto_1)
      cuenta_hasta_salto_1--;
    if (cuenta_hasta_salto_2)
      cuenta_hasta_salto_2--;

    if (fonema_no_pronunciado && (cuenta_hasta_salto_1 == 0) ){
      cuenta_hasta_salto_1 = cuenta_hasta_salto_2;
      cuenta_hasta_salto_2 = 0;
      strcpy(cont_izquierda, correcaminos->alof);
      fonema_no_pronunciado--;
      correcaminos++;
      continue;
    } // if (fonema_no_pronunciado...

    indice_de_fichero.posicion_fichero = numero_semifonema;

#ifdef _elimina_sil_sil

    if ( (cont_derecha[0] != '#') || (cont_izquierda[0] != '#') ||
	 (correcaminos->alof[0] != '#') ) {

#endif
      // Cargamos la información en el objeto nuevo_vector.

      strcpy(derecha_corregida, cont_derecha_real->fonema);
      if (derecha_corregida[strlen(derecha_corregida) - 1] == '!')
	derecha_corregida[strlen(derecha_corregida) -1] = '\0';

      strcpy(derecha_corregida_2, cont_derecha_real_2->fonema);
      if (derecha_corregida_2[strlen(derecha_corregida_2) - 1] == '!')
	derecha_corregida_2[strlen(derecha_corregida_2) -1] = '\0';

      strcpy(actual_corregido, fonema_actual->fonema);
      if (actual_corregido[strlen(actual_corregido) - 1] == '!')
	actual_corregido[strlen(actual_corregido) -1] = '\0';

      strcpy(izquierda_corregida, cont_izquierda_real->fonema);
      if (izquierda_corregida[strlen(izquierda_corregida) - 1] == '!')
	izquierda_corregida[strlen(izquierda_corregida) -1] = '\0';

      strcpy(izquierda_corregida_2, cont_izquierda_real_2->fonema);
      if (izquierda_corregida_2[strlen(izquierda_corregida_2) - 1] == '!')
	izquierda_corregida_2[strlen(izquierda_corregida_2) -1] = '\0';

      if (tipo_fon_espectral_enumerado(actual_corregido, &tipo_espectral))
	return 1;

      nuevo_vector.tipo_espectral = (char) tipo_espectral;

      if (tipo_de_fonema_duracion(actual_corregido, &tipo_duracion))
	return 1;

      nuevo_vector.tipo_duracion = (char) tipo_duracion;
                
      if ( ( ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) || 
	     ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	   //	   ( (frontera_final == FRONTERA_PALABRA) || 
	   //  (frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) &&
	   ( (indice_silaba_palabra == numero_silabas_palabra - 1) ) ) 
	nuevo_vector.frontera_prosodica = UNIDAD_RUPTURA; /// RUPTURA_ENTONATIVA o RUPTURA_COMA.
      else
	if (palabra_separada != frase_separada) {
	  if ( ( ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
		 ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	       //	       ( (frontera_inicial == FRONTERA_PALABRA) ||
	       //	 (frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) &&
	       ( (indice_silaba_palabra == 0)) )
	    nuevo_vector.frontera_prosodica = UNIDAD_RUPTURA;
	  else
	    nuevo_vector.frontera_prosodica = final_frase;
	}
	else
	  nuevo_vector.frontera_prosodica = final_frase;
           
      
      t_palabra_proc nueva_palabra_contexto;

      if (correcaminos->alof[0] == '#')
	strcpy(nueva_palabra_contexto, "#");
      else
	strcpy(nueva_palabra_contexto, palabra_contexto);

      if (nuevo_vector.introduce_datos(numero_semifonema, actual_corregido, derecha_corregida, izquierda_corregida, derecha_corregida_2, izquierda_corregida_2, tonicidad, nueva_palabra_contexto, tipo_de_tonema, posicion_frase, frontera_inicial, frontera_final)) {
	fclose(fichero_bin);
	if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	  fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	  fclose(fichero_inicio_numero);
	}
	fclose(fichero_situacion_unidades);
	return 1;
      } // if (nuevo_vector.introduce_datos.


#ifndef _OBVIA_FRECUENCIA
      if ( (resultado_anomalo = calcula_frecuencias(cadena_marcas, fonema_actual->inicio, cont_derecha_real->inicio, frecuencia_muestreo, frecuencia_minima_locutor, frecuencia_maxima_locutor, &nuevo_vector)) == -1)
	return 1;
#ifdef _ESCRIBE_SITUACIONES_ANOMALAS
      else
	if (resultado_anomalo == 1) { // No hay error, pero hay una situación anómala.
	  fprintf(fichero_anomalo, "#\t%s.\n#\tUnidad: %s. Frecuencia.\n",
		  nombre_fichero_sfs, fonema_actual->fonema);
	  fprintf(fichero_anomalo, "\tEs -s %f -e %f %s\n\n", fonema_actual->inicio - 0.3,
		  fonema_actual->inicio + 0.3, nombre_fichero_sfs);
	}

#endif
#endif
      if (calcula_duracion(fonema_actual->inicio, cont_derecha_real->inicio, frecuencia_muestreo, &nuevo_vector)) {
#ifdef _ESCRIBE_SITUACIONES_ANOMALAS
	// Se ha producido una situación anómala.
	fprintf(fichero_anomalo, "#\t%s.\n#\tUnidad: %s. Duración.\n",
		nombre_fichero_sfs, fonema_actual->fonema);
	fprintf(fichero_anomalo, "\tEs -s %f -e %f %s\n\n", fonema_actual->inicio - 0.3,
		fonema_actual->inicio + 0.3, nombre_fichero_sfs);
#endif
      }

      calcula_indices_unidad(fonema_actual->inicio, cont_derecha_real->inicio, origen,
			     frecuencia_muestreo, &nuevo_vector);

      // Cargamos los coeficientes cepstrales:

      if (devuelve_coeficientes(cadena_cepstrum, fonema_actual->inicio, cont_derecha_real->inicio, original_marcas, Vector_descriptor_candidato::numero_coeficientes_cepstrales, numero_cepstrum, nuevo_vector.cepstrum)) {
	fclose(fichero_bin);
	if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	  fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	  fclose(fichero_inicio_numero);
	}
	fclose(fichero_situacion_unidades);
	return 1;
      }

      // Calculamos las potencias.

      if (calcula_potencias(muestras_tiempo, fonema_actual->inicio, cont_derecha_real->inicio, original_marcas, frecuencia_muestreo, &nuevo_vector)) {
	fclose(fichero_bin);
	if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) != NULL) {
	  fprintf(fichero_inicio_numero, "%d", numero_semifonema);
	  fclose(fichero_inicio_numero);
	}
	fclose(fichero_situacion_unidades);
	return 1;
      }


      if ( (correcaminos != frase_prosodica) || (primera_frase == 1) ) {

	nuevo_vector.escribe_datos_bin(fichero_bin);
	fprintf(fichero_situacion_unidades, "%d\t%s\n", indice_de_fichero.posicion_fichero,
		indice_de_fichero.nombre_fichero);

	numero_semifonema++;

      }

      nuevo_vector.libera_memoria();

#ifdef _elimina_sil_sil
    }
#endif


    if ( (correcaminos + 1)->alof[0] == '\0')
      break;

    if (incluye_fonema) {
      // Actualizamos el contexto por la izquierda.
      strcpy(cont_izquierda, correcaminos->alof);
      correcaminos++;
      incluye_fonema = 0;
    }

    // Avanzamos los contextos reales.

    cont_izquierda_real_2 = cont_izquierda_real;
    cont_izquierda_real = fonema_actual;

    fonema_actual = cont_derecha_real;

    cont_derecha_real = cont_derecha_real_2;

    cont_derecha_real_2++;
    indice_alofono_actual++;

    while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	    (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+') ||
	    (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') ) {
      cont_derecha_real_2++;
      numero_lecturas++;
      if (cuenta_hasta_salto_1 == 0)
	cuenta_hasta_salto_1 = 3;
      else
	cuenta_hasta_salto_2 = (short int) (5 - fonema_no_pronunciado);
      fonema_no_pronunciado++;

    } // while (cont_derecha_real_2...


    //        indice_alofono_actual++;
  } // while(correcaminos->alof[0]...

  primera_frase = 0;
      
  if ( (fichero_inicio_numero = fopen("inicio_numero.txt", "wt")) == NULL) {
    fprintf(stderr, "Error en crea_vectores_descriptor, al abrir el fichero inicio_numero.\n");
    fclose(fichero_bin);
    fclose(fichero_situacion_unidades);
    return 1;
  }
  fprintf(fichero_inicio_numero, "%d", numero_semifonema);
  fclose(fichero_inicio_numero);

  primer_fonema_frase = 1;

  fclose(fichero_situacion_unidades);
  fclose(fichero_cambia_real);
  //     fclose(fichero_bin);
#ifdef _ESCRIBE_SITUACIONES_ANOMALAS
  fclose(fichero_anomalo);
#endif

  libera_memoria_ventana();
    
  return 0;
     
}

#endif


#ifdef _CALCULA_DATOS_MODELO_ENERGIA

/**
 * Función:   calcula_datos_modelo_energia
 * \remarks Entrada:
 *			- frase_separada: cadena que contiene la información de cada palabra procesada.
 *            Se emplea para obtener la información de palabra contexto y tipo de frase.
 *          - frase_prosodica: cadena en la que se encuentra la información de cada fonema
 *            por separado.                                                                  
 *          - frase_en_grupos: división en grupos de la frase de entrada.
 *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
 *            tonicidad y de pausas.                                                         
 *          - frase_sintagmada: división en sintagmas de la frase de entrada.
 *          - nombre_fichero_sfs: nombre del fichero sfs, para la posterior corrección  de
 *            errores.                                                                       
 *          - fichero_fonemas: fichero en el que se encuentra la información del etiquetado
 *            temporal de los fonemas.                                                       
 *          - fichero_sonido: fichero de datos de audio "en bruto", empleado para el cálculo
 *            de la energía de cada fonema.
 *			- frecuencia_muestreo: frecuencia de muestreo del fichero de sonido.			 
 *			- numero_de_elementos_de_frase_separada: número de elementos de la variable
 *			  frase_separada.
 *			- correccion_manual: si vale cero, el corpus no se revisó manualmente.
 *	\remarks Salida:
 *			- cadena_datos_energia: cadena de estructuras en las que se almacena la 		 
 *			  información para el modelo de estimación de la energía.
 *			- numero_fonemas_energia: número de elementos de la cadena anterior.
 * \remarks Valor devuelto:                                                                           
 *          - En ausencia de error devuelve un 0. Si hay algún fallo en la comprobación del
 *            fichero de marcas, devuelve un 2. Si es en el fichero de sonido, un 3.         
 * \remarks Objetivo:  Obtiene la información para el modelo de estimación de la energía basado en
 *			  regresión lineal.
 */


int Crea_vectores_descriptor::calcula_datos_modelo_energia(t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, char *fichero_origen, char *nombre_fichero_sfs, FILE *fichero_fonemas, FILE *fichero_marcas, FILE *fichero_sonido, int frecuencia_muestreo, int numero_de_elementos_de_frase_separada, Parametros_modelo_energia **cadena_datos_energia, int *numero_fonemas_energia, int correccion_manual) {

  static int numero_semifonema = 0; // Variable que se emplea para asignar un identificador numérico
  // único a cada unidad.
  static int numero_lecturas = 0;

  static char primera_vez = 2; // Variable estática que controla la apertura de los ficheros de
  // sonido, marcas y fonemas.

  static char primer_fonema_frase = 1;

  // Punteros a las diferentes cadenas.

  t_frase_separada *palabra_separada = frase_separada;
  t_prosodia *correcaminos = frase_prosodica;
  char *f_fonetica = frase_fonetica;
  t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;

  t_palabra_proc palabra_contexto= ""; // Variable en la que se almacena la palabra a la que pertenece
  // la unidad actual.
  char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#"; // Alófonos por la derecha e izquierda, tal y como los estima Cotovia.
  char derecha_corregida[OCTETOS_POR_FONEMA + 1]; // Alófonos reales a los que se le ha quitado una posible
  // marca de corrección. (como !, ver ficheros de fonemas).
  char izquierda_corregida[OCTETOS_POR_FONEMA + 1];
  char actual_corregido[OCTETOS_POR_FONEMA + 1]; // Lo mismo de arriba.
  int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
  int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.

  char nueva_silaba, fin_silaba;

  // Variables para determinar el tipo de frase al que pertenece la unidad.
  unsigned char final_tonema_frase_separada; // Índice en el que se termina el tonema en el que nos encontramos.
  unsigned char tipo_de_tonema; // Sólo consideramos cuatro: ENUNCIATIVA, INTERROGATIVA, EXCLAMATIVA E INACABADA

  static estructura_fonemas *fonema_actual;

  static estructura_fonemas *cont_derecha_real, *cont_izquierda_real; // Alófonos, tal y como vienen en el fichero de etiquetas.
  static estructura_fonemas *cont_derecha_real_2, *cont_izquierda_real_2; // El siguiente y el anterior, respecto a los anteriores.

  static estructura_energia *apunta_cadena_energia;
  static float energia_media;

  static Parametros_modelo_energia *apunta_parametros;

  static float tiempo_final_sonido; // Instante de finalización real en el fichero de sonido.

  char incluye_fonema = 1; // Variable binaria empleada para controlar posibles desfases entre la
  // transcripción de Cotovia y la del fichero de fonemas.

  char primera = 1; // Variable binaria que controla la presencia de un silencio al inicio de la transcripción.

  // Variables destinadas al control de aquellas palabras que tienen el campo pal_transformada != NULL
  int numero_palabra = 0; // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
  // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
  char ciclo_transformada = 0;
  char no_final_ciclo = 1;
  t_palabra_proc parte_transformada, parte; // Variables en las que se almacena la palabra transformada completa
  // y la palabra dentro de ella a la que nos referimos.

  unsigned char posicion_frase = 0; // Posición del fonema dentro de la frase (inicial, media o final).
  unsigned char tonicidad = 0; // Variable que indica si una sílaba es tónica o no.
  short int numero_fonemas_silaba;
  short int posicion_en_silaba;
  short int numero_silabas_desde_inicio;
  short int numero_silabas_hasta_final;
  short int numero_tonicas_desde_inicio;
  short int numero_tonicas_hasta_final;
  short int silabas_de_palabra, fonemas_de_palabra;
  char frontera_inicial = 0;
  char frontera_final = 0;
  char final_frase = 0;
  unsigned char indice_silaba_palabra;

  clase_espectral tipo_espectral; // tipo de fonema según la clasificación de clase_espectral.

  char fonema_no_pronunciado = 0; // Variable que controla si tenemos que avanzar el puntero a los
  // fonemas transcritos por Cotovía, debido a que el locutor no ha pronunciado dicho fonema.
  char cuenta_hasta_salto_1 = 0; // Variable que trabaja con la anterior para avanzar en el momento adecuado el
  char cuenta_hasta_salto_2 = 0; // Variable que permite que haya varios sonidos consecutivos que no se leen.
  // puntero a los fonemas transcritos por Cotovía, cuando hay que hacer algún cambio.

  int fonemas_iguales; // Variable que sirve para comparar si el fonema real actual y el de Cotovía son iguales.
  int resultado;
  float tiempo_ultima_marca;

  // Contador de posición del alófono considerado. Se emplea para almacenar la posición en el
  // fichero de fonemas del lugar en el que hay un error de transcripción.

  static int indice_alofono_actual = 1;

  // Comprobamos que no se trate de una frase con sólo signos de puntuación:

  if (frase_solo_signos_de_puntuacion(frase_separada))
    return 0;

  if (primera_vez == 2) {

    primera_vez = 1;
    primer_fonema_frase = 0;

    // Cargamos en memoria el contenido del fichero de la forma de onda.
    // Si no se desea calcular la frecuencia, la propia función se encarga de devolver
    // sólo el valor de muestras_tiempo, necesario para saber la duración de la última
    // unidad.

    if (carga_vector_muestras(frecuencia_muestreo, &muestras_tiempo,
			      &numero_muestras, &tiempo_final_sonido, fichero_sonido))
      return 2;

    // Cargamos en memoria el contenido del fichero de marcas de pitch.

    if (crea_cadena_fichero(&original_marcas, fichero_marcas, &numero_marcas, &tiempo_ultima_marca))
      return 1;

    //       	original_marcas = cadena_marcas;

    fclose(fichero_marcas);
    fichero_marcas = NULL;

    // Cargamos en memoria el contenido del fichero de fonemas:

    if (fonema_actual == NULL) {

      if (carga_fichero_fonemas(&cadena_fonemas, fichero_fonemas, &numero_fonemas, 1))
	return 3;

      fonema_actual = cadena_fonemas;
      cont_izquierda_real = fonema_actual;
      cont_izquierda_real_2 = fonema_actual;
      cont_derecha_real = fonema_actual + 1;
      cont_derecha_real_2 = cont_derecha_real + 1;
    } // if (fonema_actual == NULL...


    // Hacemos lo mismo con el fichero de sonido

    if (comprueba_validez_fichero_sonido(numero_muestras, (cadena_fonemas + numero_fonemas - 3)->inicio,
                                         frecuencia_muestreo, &tiempo_final_sonido))
      return 2;

    (cadena_fonemas + numero_fonemas)->inicio = tiempo_final_sonido;
    strcpy((cadena_fonemas + numero_fonemas)->fonema, "#");
        
    if (calcula_energia_media_y_parte_estacionaria(muestras_tiempo, numero_muestras, cadena_fonemas, numero_fonemas, original_marcas, numero_marcas, frecuencia_muestreo, &energia_media, &cadena_energia))
      return 2;

    apunta_cadena_energia = cadena_energia;
        
    if ( (*cadena_datos_energia = (Parametros_modelo_energia *) malloc(numero_fonemas*sizeof(Parametros_modelo_energia))) == NULL) {
      fprintf(stderr, "Error en calcula_datos_modelo_energia, al asignar memoria.\n");
      return 1;
    } // if ( (*cadena_datos_energia = ...

    apunta_parametros = *cadena_datos_energia;
    *numero_fonemas_energia = 0;


  } // if (primera_vez == 2)
  else {
    apunta_cadena_energia--;
    (*numero_fonemas_energia)--;
  }


  // Estas variables son necesarias para determinar el tipo de frase a la que pertenece
  // cada unidad.

  final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;

  tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);

  // Inicializamos las variables necesarias para el cálculo de la posición en la frase
  // y la tonicidad.

  inicializa_calculo_posicion_y_tonicidad();

  FILE *fichero_cambia_real;

  if ( (fichero_cambia_real = fopen("cambia_transcripcion.txt", "a")) == NULL) {
    fprintf(stderr, "Error en calcula_datos_modelo_energia, al abrir el fichero \
         cambia_transcripcion.\n");
    return 1;
  } // if ( (fichero_cambia_real = ...

  // Construimos el nombre del fichero sfs.
  //   	construye_nombre_sfs(&nombre_fichero_sfs);

  cont_derecha_real = fonema_actual + 1;

  while ( (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '*') ||
	  (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '+') ||
	  (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '-') ) {
    cont_derecha_real++;
    cuenta_hasta_salto_1 = 2;
    fonema_no_pronunciado++;
    indice_alofono_actual++;
    //            numero_lecturas++;
  }

  cont_derecha_real_2 = cont_derecha_real + 1;

  while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	  (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+') ||
	  (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') )  {
    cont_derecha_real_2++;
    fonema_no_pronunciado++;
    cuenta_hasta_salto_1 = 3;
    indice_alofono_actual++;
  }

  // Entramos en el bucle principal.

  while (correcaminos->alof[0] != '\0') {

    if ( (correcaminos->alof[0] == '#') && ((correcaminos+1)->alof[0] == '#') ) {
      correcaminos++;
      continue;
    }

    if (calcula_posicion_frase_tonicidad_silabas(f_fonetica, correcaminos->alof, &posicion_frase, &tonicidad, &numero_fonemas_silaba, &fonemas_de_palabra, &silabas_de_palabra, &posicion_en_silaba, &numero_silabas_desde_inicio, &numero_silabas_hasta_final,&numero_tonicas_desde_inicio, &numero_tonicas_hasta_final, &nueva_silaba, &fin_silaba, NULL, &frontera_inicial, &frontera_final, &final_frase, &indice_silaba_palabra))
      return 1;

    // Comprobamos que no haya ningún problema de pronunciación del locutor o de etiquetado:


    fonemas_iguales = strcmp(correcaminos->alof, fonema_actual->fonema);

    if (fonemas_iguales && (fonema_no_pronunciado == 0) ) {
      if ( (resultado = comprueba_error_etiquetado(correcaminos->alof, fonema_actual->fonema, correccion_manual)) == 1) {
	FILE *fichero_etiquetado;
	if ( (fichero_etiquetado = fopen("errores_etiquetado.txt", "a")) == NULL) {
	  fprintf(stderr, "Error al abrir el fichero de errores de etiquetado.\n");
	  return 1;
	} // if (fichero_etiquetado = ...
	fprintf(fichero_etiquetado, "#\t%s.\n#\tCotovía: %s.\tReal: %s.\n", fichero_origen,
		correcaminos->alof, fonema_actual->fonema);
	fprintf(fichero_etiquetado, "\tEs -s %f -e %f %s\n", fonema_actual->inicio - 0.3,
		fonema_actual->inicio + 0.3, nombre_fichero_sfs);
	return 1;
      } // if (resultado = comprueba...
      else
	if (resultado == 2)
	  fprintf(fichero_cambia_real, "%s %s %d\n", fichero_origen, correcaminos->alof,
                  indice_alofono_actual);
    } // if ( (fonemas_iguales...


    if (!((fonema_actual->fonema[0] == '#') && (correcaminos->alof[0] != '#'))) {
      if ( (correcaminos->alof[0] == '#') || primer_fonema_frase) {
	primer_fonema_frase = 0;
	if (primera) {
	  // Se trata del primer silencio del fichero (el inicial).
	  primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) {
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Cambié el > por >=.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_datos_modelo_energia: \
                      datos de entrada incorrectos.\n");
	      return 1;
	    } // if (++elementos_de_frase_separada...
	    palabra_separada++; // Avanzamos palabra_separada, ya que no es una palabra, sino
	    // un signo de puntuación.
	  } // while (palabra_separada...
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...
	  if (palabra_separada->pal_transformada) {
	    // Entramos en un ciclo de palabra transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      return 1;
	    } // if (no_final_ciclo...
	    strcpy(palabra_contexto, parte);
	  } // if (palabra_separada->...
	  else
	    // No hay palabra transformada -> La palabra contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  if (correcaminos->alof[0] == '#')
	    anterior_orden++;
	} // if (primera)...
      }  // if (correcaminos->alof[0] == '#') ||primer_fonema_frase...

      // Comprobamos si estamos en un ciclo de palabra transformada.
      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Cambiamos de palabra, pero dentro de un ciclo de palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    anterior_orden = correcaminos->pp;
	  else
	    ciclo_transformada = 0;
	} // if (correcaminos->pp > anterior_orden...
	if (no_final_ciclo) {
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (char)
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	    return 1;
	  } // if (no_final_ciclo = (char)
	  strcpy(palabra_contexto, parte);
	} // if (no_final_ciclo)
      } // if (ciclo_transformada)

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;
	  while (palabra_separada->clase_pal == SIGNO_PUNTUACION) {
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_datos_modelo_energia: \
                         datos de entrada incorrectos.\n");
	      return 1;
	    } // if (++elementos_de_frase_separada...
	    palabra_separada++;
	  } // while (palabra_separada->clase_pal...
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...

	  anterior_orden = correcaminos->pp;
	} // if (elementos_de_frase_separada++ < numero_...
	else {
	  fprintf(stderr, "Error en calcula_datos_modelo_energia: \
                  datos de entrada incorrectos.\n");
	  return 1;
	} // else

	if (palabra_separada->pal_transformada) {
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      return 1;
	    }
	    strcpy(palabra_contexto, parte);
	  } // if ( (no_final_ciclo...
	} //if (palabra_separada->pal_transformada...
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      } // if (correcaminos->pp > anterior_orden...


      // Actualizamos el contexto por la derecha.

      if ( (correcaminos + 1)->alof[0] )
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");
      incluye_fonema = 1;
    }


    if (cuenta_hasta_salto_1)
      cuenta_hasta_salto_1--;
    if (cuenta_hasta_salto_2)
      cuenta_hasta_salto_2--;

    if (fonema_no_pronunciado && (cuenta_hasta_salto_1 == 0) ){
      cuenta_hasta_salto_1 = cuenta_hasta_salto_2;
      cuenta_hasta_salto_2 = 0;
      strcpy(cont_izquierda, correcaminos->alof);
      fonema_no_pronunciado--;
      correcaminos++;
      continue;
    } // if (fonema_no_pronunciado...


#ifdef _elimina_sil_sil

    if ( (cont_derecha[0] != '#') || (cont_izquierda[0] != '#') ||
	 (correcaminos->alof[0] != '#') ) {

#endif


      // Cargamos la información en el objeto nuevo_vector.

      strcpy(derecha_corregida, cont_derecha_real->fonema);
      if (derecha_corregida[strlen(derecha_corregida) - 1] == '!')
	derecha_corregida[strlen(derecha_corregida) -1] = '\0';

      strcpy(actual_corregido, fonema_actual->fonema);
      if (actual_corregido[strlen(actual_corregido) - 1] == '!')
	actual_corregido[strlen(actual_corregido) -1] = '\0';

      strcpy(izquierda_corregida, cont_izquierda_real->fonema);
      if (izquierda_corregida[strlen(izquierda_corregida) - 1] == '!')
	izquierda_corregida[strlen(izquierda_corregida) -1] = '\0';

      if (tipo_fon_espectral_enumerado(actual_corregido, &tipo_espectral))
	return 1;

      if (strcmp(apunta_cadena_energia->fonema, actual_corregido)) {
	if ( (correcaminos + 1)->alof[0] == '\0')
	  return 0;
	fprintf(stderr, "Error en calcula_datos_modelo_energia: correspondencia \
						entre transcripciones de fonemas perdida.\n");
	return 1;
      } // if (strcmp(...

      strcpy(apunta_parametros->fonema, actual_corregido);

      if (tipo_fon_espectral_enumerado(actual_corregido, &(apunta_parametros->tipo_fonema)))
	return 1;

      if (tipo_fon_espectral_enumerado(izquierda_corregida, &(apunta_parametros->tipo_fonema_anterior)))
	return 1;

      if (tipo_fon_espectral_enumerado(derecha_corregida, &(apunta_parametros->tipo_fonema_siguiente)))
	return 1;

      if (numero_semifonema == 0)
	apunta_parametros->energia_fonema_anterior = 10*log10(MINIMA_ENERGIA_INF);
      else
	apunta_parametros->energia_fonema_anterior = (apunta_parametros - 1)->energia;
                    
      apunta_parametros->silabas_frase = numero_silabas_desde_inicio + numero_silabas_hasta_final;
      apunta_parametros->energia_media_fichero = energia_media;
      apunta_parametros->energia = apunta_cadena_energia->energia;
      apunta_parametros->posicion = posicion_frase;
               
      if (actual_corregido[0] != '#') {
	apunta_parametros->silabas_palabra = silabas_de_palabra;
	apunta_parametros->fonemas_palabra = fonemas_de_palabra;
      } // if (actual_corregido[0] != '#')
      else {
	apunta_parametros->silabas_palabra = 0;
	apunta_parametros->fonemas_palabra = 0;
      } // else

      apunta_parametros->silabas_inicio = numero_silabas_desde_inicio;
      apunta_parametros->silabas_final = numero_silabas_hasta_final;	
      apunta_parametros->acento = tonicidad;
      apunta_parametros->frontera_inicial = frontera_inicial;
      apunta_parametros->frontera_final = frontera_final;

      if ( ( ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) || 
	     ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	   //	   ( (frontera_final == FRONTERA_PALABRA) || 
	   //  (frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) && 
	   (indice_silaba_palabra == silabas_de_palabra - 1) )
	apunta_parametros->frontera_prosodica = UNIDAD_RUPTURA; /// RUPTURA_ENTONATIVA o RUPTURA_COMA.
      else
	if (palabra_separada != frase_separada) {
	  if ( ( ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
		 ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	       //	       ( (frontera_inicial == FRONTERA_PALABRA) ||
	       //	 (frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) &&
	       (indice_silaba_palabra == 0) )
	    apunta_parametros->frontera_prosodica = UNIDAD_RUPTURA;
	  else
	    apunta_parametros->frontera_prosodica = final_frase;
	}
	else
	  apunta_parametros->frontera_prosodica = final_frase;

      apunta_parametros->tipo_proposicion = tipo_de_tonema;

      (*numero_fonemas_energia)++;	
      apunta_parametros++;
               
      apunta_cadena_energia++;
               
      numero_semifonema++;


#ifdef _elimina_sil_sil
    }
#endif


    if ( (correcaminos + 1)->alof[0] == '\0')
      break;

    if (incluye_fonema) {
      // Actualizamos el contexto por la izquierda.
      strcpy(cont_izquierda, correcaminos->alof);
      correcaminos++;
      incluye_fonema = 0;
    }

    // Avanzamos los contextos reales.

    cont_izquierda_real_2 = cont_izquierda_real;
    cont_izquierda_real = fonema_actual;

    fonema_actual = cont_derecha_real;

    cont_derecha_real = cont_derecha_real_2;

    cont_derecha_real_2++;
    indice_alofono_actual++;

    while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	    (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+') ||
	    (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') ) {
      cont_derecha_real_2++;
      numero_lecturas++;
      if (cuenta_hasta_salto_1 == 0)
	cuenta_hasta_salto_1 = 3;
      else
	cuenta_hasta_salto_2 = (short int) (5 - fonema_no_pronunciado);
      fonema_no_pronunciado++;

    } // while (cont_derecha_real_2...


    indice_alofono_actual++;
  } // while(correcaminos->alof[0]...

  apunta_parametros--;
  primer_fonema_frase = 1;

  fclose(fichero_cambia_real);

  libera_memoria_ventana();
     
  return 0;

}

#endif

#ifdef _SCRIPT_DURACION

/**
 * Función:   calcula_parametros_duracion
 * Versión:   info_duracion.
 * \remarks Entrada:
 *			- frase_separada: cadena que contiene la información de cada palabra procesada.
 *            Se emplea para obtener la información de palabra contexto y tipo de frase.
 *          - frase_prosodica: cadena en la que se encuentra la información de cada fonema
 *            por separado.                                                                  
 *          - frase_en_grupos: división en grupos de la frase de entrada.
 *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
 *            tonicidad y de pausas.                                                         
 *          - frase_sintagmada: división en sintagmas de la frase de entrada.
 *          - fichero_origen: nombre del fichero de texto en el que se encuentra la unidad.  
 *          - fichero_fonemas: fichero en el que se encuentra la información del etiquetado
 *            temporal de los fonemas.                                                       
 *			- numero_de_elementos_de_frase_separada: número de elementos de la variable
 *			  frase_separada.
 *			- correccion_manual: si vale 0, el corpus no fue analizado manualmente.									 
 *	\remarks Salida:
 *			- cadena_duracion: cadena de vectores con la información de duración para cada
 *            fonema.
 *          - numero_elementos_duracion: número de fonemas de la cadena anterior.
 * \remarks Valor devuelto:
 *          - En ausencia de error devuelve un 0. Si hay algún fallo en la comprobación del
 *            fichero de marcas, devuelve un 2. Si es en el fichero de sonido, un 3.
 * \remarks Objetivo:  Calcula las características de los fonemas que se encuentran en el corpus,
 *            para su posterior uso en el modo de funcionamiento normal del programa, es
 *            decir, en la síntesis.
 */


int Crea_vectores_descriptor::calcula_parametros_duracion(t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, char *fichero_origen, FILE *fichero_fonemas, int numero_de_elementos_de_frase_separada, Parametros_duracion_entrenamiento **cadena_duracion, int *numero_elementos_duracion, int correccion_manual) {

  static char primera_frase_fichero = 1;
  // Punteros a las diferentes cadenas.
  t_frase_separada *palabra_separada = frase_separada;
  t_prosodia *correcaminos = frase_prosodica;
  char *f_fonetica = frase_fonetica;
  t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;

  t_palabra_proc palabra_contexto; // Variable en la que se almacena la palabra a la que pertenece
  // la unidad actual.
  char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#"; // Alófonos por la derecha e izquierda, tal y como los estima Cotovia.
  int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
  int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.

  // Variables para determinar el tipo de frase al que pertenece la unidad.
  unsigned char final_tonema_frase_separada; // Índice en el que se termina el tonema en el que nos encontramos.
  unsigned char tipo_de_tonema; // Sólo consideramos cuatro: ENUNCIATIVA, INTERROGATIVA y EXCLAMATIVA.

  static estructura_fonemas *cont_derecha_real, *cont_izquierda_real, *fonema_actual = NULL; // Alófonos, tal y como vienen en el fichero de etiquetas.
  static estructura_fonemas *cont_derecha_real_2, *cont_izquierda_real_2; // Alófonos n - 2 y n + 2, pronunciados por el locutor.

  int numero_fonemas;
    
  short int numero_fonemas_silaba; // Número de fonemas contenidos en la sílaba actual.
  short int posicion_en_silaba;    // Posición del fonema dentro de la sílaba.
  short int numero_silabas_desde_inicio; // Número de sílabas hasta la actual dentro de su grupo entre pausas.
  short int numero_silabas_hasta_final;  // Número de sílabas hasta la final, dentro de su grupo entre pausas.
  short int numero_tonicas_desde_inicio; // Número de sílabas tónicas desde el inicio del grupo.
  short int numero_tonicas_hasta_final;  // Número de sílabas tónicas hasta el final del grupo.

  static float tiempo_acumulado = 0;            // Tiempo desde la última pausa hasta el fonema actual.

  char tipo_pausa_anterior, tipo_siguiente_pausa;  // Variables que indican los tipos de pausa que rodean
  // a un grupo fónico.
  char nueva_silaba;                  // Variable que indica el comienzo de una nueva silaba.
  char fin_silaba;
  short int silabas_palabra = 0;      // Contador del número de sílabas de una palabra.
  short int fonemas_palabra = 0;      // Contador del número de fonemas de una palabra.

  //    short int fonemas_en_palabra;
  short int silabas_de_palabra;
        
  char frontera_inicial, frontera_final; // Variables que indican el tipo de frontera antes y después del fonema (según la clasificación de descriptores.hpp)
  char final_frase; 					// Lo mismo para el final de la oración (en este caso se diferencia según el tipo de frontera de grupo entonativo).

  unsigned char indice_silaba_palabra; // Índice de la sílaba en la que se encuentra el fonema (con respecto a la palabra)

  char primera = 1; // Variable binaria que controla la presencia de un silencio al inicio de la transcripción.

  // Variables destinadas al control de aquellas palabras que tienen el campo pal_transformada != NULL
  int numero_palabra = 0; // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
  // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
  char ciclo_transformada = 0;
  char no_final_ciclo = 1;
  t_palabra_proc parte_transformada, parte; // Variables en las que se almacena la palabra transformada completa
  // y la palabra dentro de ella a la que nos referimos.

  unsigned char posicion_frase; // Posición del fonema dentro de la frase (inicial, media o final).
  unsigned char tonicidad; // Variable que indica si una sílaba es tónica o no.

  // Contador de posición del alófono considerado. Se emplea para almacenar la posición en el
  // fichero de fonemas del lugar en el que hay un error de transcripción.

  int indice_alofono_actual = 1;

  char fonema_no_pronunciado = 0; // Variable que controla si tenemos que avanzar el puntero a los
  // fonemas transcritos por Cotovía, debido a que el locutor no ha pronunciado dicho fonema.
  char cuenta_hasta_salto_1 = 0; // Variable que trabaja con la anterior para avanzar en el momento adecuado el
  char cuenta_hasta_salto_2 = 0; // Variable que permite que haya varios sonidos consecutivos que no se leen.
  // puntero a los fonemas transcritos por Cotovía, cuando hay que hacer algún cambio.

  int fonemas_iguales; // Variable que sirve para comparar si el fonema real actual y el de Cotovía son iguales.
  int cuenta_fonemas;

  static Parametros_duracion_entrenamiento *recorre_parametros;
  Parametros_duracion_entrenamiento *anterior_pausa, *anterior_palabra;

  // Comprobamos que no se trate de una frase con sólo signos de puntuación:

  if (frase_solo_signos_de_puntuacion(frase_separada))
    return 0;

  //Para poder seguir la equivalencia entre los datos del cotovía  y del fichero de voz
  // tenemos que eliminar el posible silencio repetido del principio.

  if (fonema_actual == NULL) {
    if (carga_fichero_fonemas(&cadena_fonemas, fichero_fonemas, &numero_fonemas, 1))
      return 1;

    *numero_elementos_duracion = numero_fonemas - 2;

    fonema_actual = cadena_fonemas;
    cont_izquierda_real = fonema_actual;
    cont_izquierda_real_2 = fonema_actual;
    cont_derecha_real = fonema_actual + 1;
    cont_derecha_real_2 = cont_derecha_real + 1;

    if ( (*cadena_duracion = (Parametros_duracion_entrenamiento *) malloc(numero_fonemas*sizeof(Parametros_duracion_entrenamiento)))
	 == NULL) { 
      fprintf(stderr, "Error en calcula_parametros_duracion, al asignar memoria.\n");
      return 1;
    } // if ( (cadena_duracion = (Parametros_duracion_entrenamiento *)...

    recorre_parametros = *cadena_duracion;
  } // if (fonema_actual...

  indice_alofono_actual++;

  while ( (cont_derecha_real->fonema[0] == fonema_actual->fonema[0]) &&
	  (fonema_actual->fonema[0] == '#') ) {
    cont_izquierda_real = fonema_actual;
    fonema_actual = cont_derecha_real;
    cont_derecha_real++;
    indice_alofono_actual++;
  } // while ( (cont_derecha_real...

  // Este cambio es por la falta de concordancia entre la transcripción de Cotovía y lo
  // pronunciado realmente por el locutor (chapucilla).

  if (fonema_actual->fonema[strlen(fonema_actual->fonema) - 1] == '+')
    fonema_actual->fonema[strlen(fonema_actual->fonema) - 1] = '\0';
  if (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '+')
    cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] = '\0';

  while (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '*') {
    cont_derecha_real++;
    if (cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] == '+')
      cont_derecha_real->fonema[strlen(cont_derecha_real->fonema) - 1] = '\0';
    cuenta_hasta_salto_1 = 2;
    fonema_no_pronunciado++;
    (*numero_elementos_duracion)--;
  } // while (cont_derecha_real...

  cont_derecha_real_2 = cont_derecha_real + 1;

  if (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+')
    cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] = '\0';

  while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	  (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') ) {
    cont_derecha_real_2++;
    if (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+')
      cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] = '\0';
    cuenta_hasta_salto_1 = 3;
    fonema_no_pronunciado++;
    (*numero_elementos_duracion)--;
  }


  // Estas variables son necesarias para determinar el tipo de frase a la que pertenece
  // cada unidad.

  final_tonema_frase_separada = (unsigned char)
    (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;

  tipo_de_tonema = (unsigned char)
    halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);


  // Inicializamos las variables necesarias para el cálculo de la posición en la frase
  // y la tonicidad.

  inicializa_calculo_posicion_y_tonicidad();

  int resultado;
  FILE *fichero_cambia_real;

  if ( (fichero_cambia_real = fopen("cambia_transcripcion.txt", "a")) == NULL) {
    fprintf(stderr, "Error en calcula_parametros_duracion, al abrir el fichero cambia_transcripcion.\n");
    return 1;
  }

  anterior_pausa = recorre_parametros;
  anterior_palabra = recorre_parametros;

  tipo_pausa_anterior = PAUSA_PUNTO; // De momento no vamos a distinguir entre "punto" y "punto y aparte".
  // Y probablemente no lo hagamos nunca, ya que necesitaríamos datos de
  // habla continua.

  if (primera_frase_fichero == 1)
    tiempo_acumulado = 0.0;
    
  // El primero es un silencio:

  recorre_parametros->FonemasPalabra = 0;
  recorre_parametros->SilabasPalabra = 0;
        
  // Entramos en el bucle principal.

  while (correcaminos->alof[0] != '\0') {

    if ( (correcaminos != frase_prosodica) &&
	 ( (correcaminos->alof[0] == '#') && ( (correcaminos - 1)->alof[0] == '#')) ) {
      correcaminos++;
      continue;
    }

    if (correcaminos->alof[0] == '#')
      //      recorre_parametros->FinSilaba = 0;

    // Comprobamos que no haya ningún problema de pronunciación del locutor o de etiquetado:

    resultado = 1;
    fonemas_iguales = strcmp(correcaminos->alof, fonema_actual->fonema);

    if (fonemas_iguales && (fonema_no_pronunciado == 0) ) {
      if ( (resultado = comprueba_error_etiquetado(correcaminos->alof, fonema_actual->fonema, correccion_manual)) == 1) {
	FILE *fichero_etiquetado;
	if ( (fichero_etiquetado = fopen("errores_etiquetado.txt", "a")) == NULL) {
	  fprintf(stderr, "Error en calcula_parametros_duracion, al abrir el fichero de errores de etiquetado.\n");
	  return 1;
	}
	fprintf(fichero_etiquetado, "#\t%s.\n#\tCotovía: %s.\tReal: %s.\n", fichero_origen, correcaminos->alof, fonema_actual->fonema);
	fprintf(fichero_etiquetado, "\tEs %s -s %f -e %f\n", fichero_origen, fonema_actual->inicio - 0.3, fonema_actual->inicio + 0.3);
	return 1;
      }
      else
	if (resultado == 2)
	  fprintf(fichero_cambia_real, "%s %s %d\n", fichero_origen, correcaminos->alof, indice_alofono_actual);
    }

    if ((!((fonema_actual->fonema[0] == '#') && (correcaminos->alof[0] != '#')) ) || fonema_no_pronunciado) {
      if (correcaminos->alof[0] == '#') {
	if (primera) {
	  // Se trata del primer silencio del fichero (el inicial).
	  //                primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { //||
	    //                         (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Cambié el > por >=.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_parametros_duracion: datos de entrada incorrectos.\n");
	      return 1;
	    }
	    palabra_separada++; // Avanzamos palabra_separada, ya que no es una palabra, sino
	    // un signo de puntuación.
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }
	  if (palabra_separada->pal_transformada) {
	    // Entramos en un ciclo de palabra transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      return 1;
	    }
	    strcpy(palabra_contexto, parte);
	  }
	  else
	    // No hay palabra transformada -> La palabra contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  anterior_orden++;
	}
      }

      // Comprobamos si estamos en un ciclo de palabra transformada.
      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Cambiamos de palabra, pero dentro de un ciclo de palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    anterior_orden = correcaminos->pp;
	  else
	    ciclo_transformada = 0;
	}
	if (no_final_ciclo) {
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (unsigned char)
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	    return 1;
	  }
	  strcpy(palabra_contexto, parte);
	}
      }

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_parametros_duracion: datos de entrada incorrectos.");
	      return 1;
	    }
	    palabra_separada++;
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }

	  anterior_orden = correcaminos->pp;
	}
	else {
	  fprintf(stderr, "Error en calcula_parametros_duracion: datos de entrada incorrectos.\n");
	  return 1;
	}

	if (palabra_separada->pal_transformada) {
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      return 1;
	    }
	    strcpy(palabra_contexto, parte);
	  }
	}
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      }

      // Calculamos la posición dentro de la frase y su tonicidad.

      if (calcula_posicion_frase_tonicidad_silabas(f_fonetica, correcaminos->alof, &posicion_frase, &tonicidad, &numero_fonemas_silaba, &fonemas_palabra, &silabas_de_palabra, &posicion_en_silaba, &numero_silabas_desde_inicio, &numero_silabas_hasta_final,&numero_tonicas_desde_inicio, &numero_tonicas_hasta_final, &nueva_silaba, &fin_silaba, NULL, &frontera_inicial, &frontera_final, &final_frase, &indice_silaba_palabra)) {
	return 1;
      }

      if ( (fonema_actual->fonema[0] == '#') && (correcaminos != frase_prosodica) ) {
	unsigned char categoria_gramatical = (palabra_separada + 1)->cat_gramatical[0];
	if (categoria_gramatical == COMA)
	  tipo_siguiente_pausa = PAUSA_COMA;
	else if (categoria_gramatical == PUNTOS_SUSPENSIVOS)
	  tipo_siguiente_pausa = PAUSA_PUNTOS_SUSPENSIVOS;
	else
	  tipo_siguiente_pausa = PAUSA_PUNTO;

	/*            else if ( (categoria_gramatical == PUNTO) || (categoria_gramatical == PUNTO_E_COMA) ||
                      (categoria_gramatical == DOUS_PUNTOS) )
                      tipo_siguiente_pausa = PAUSA_PUNTO;
		      else
		      tipo_siguiente_pausa = PAUSA_OTRO_SIGNO_PUNTUACION;
	*/
	for (; anterior_pausa <= recorre_parametros; anterior_pausa++)
	  anterior_pausa->SiguientePausa = tipo_siguiente_pausa;
      } // if (fonema_actual->fonema...


      if ( (fonemas_iguales == 0) || (resultado == 0) ){
	if (primera == 0) {
	  if ( (correcaminos - 1)->alof[0] == '#')
	    tiempo_acumulado = 0.0;
	  else
	    tiempo_acumulado += (fonema_actual->inicio - cont_izquierda_real->inicio);
	} // if (primera == 0)
	recorre_parametros->DuracionDesdePausa = tiempo_acumulado;
      } // if ( (fonemas_iguales...

      /*          if ( (correcaminos->alof[0] == '#') && (recorre_parametros != *cadena_duracion) )
		  (recorre_parametros - 1)->FinSilaba = 1;
      */
      //      recorre_parametros->FinSilaba = fin_silaba;

      if (nueva_silaba == 1) {
	silabas_palabra++;
	//            (recorre_parametros - 1)->FinSilaba = 1;
	//	recorre_parametros->InicioSilaba = 1;
      } // if (nueva_silaba)
      //      else
      //	recorre_parametros->InicioSilaba = 0;

      // if ( (fonema_actual->fonema[0] != '#') &&
      // 	   ( (fonema_no_pronunciado == 0) || (cuenta_hasta_salto_1 != 1) ) )
      // 	fonemas_palabra++;

      // if (frontera_inicial) {
      // 	for (cuenta_fonemas = 0; cuenta_fonemas < fonemas_palabra; anterior_palabra++, cuenta_fonemas++) {
      // 	  anterior_palabra->FonemasPalabra = fonemas_palabra;
      // 	  anterior_palabra->SilabasPalabra = silabas_palabra;
      // 	} // for (; anterior...

      // 	fonemas_palabra = 0;
      // 	silabas_palabra = 0;

      // } // if (frontera_inicial)

      if (cuenta_hasta_salto_1)
	cuenta_hasta_salto_1--;

      if (cuenta_hasta_salto_2)
	cuenta_hasta_salto_2--;

      if  (fonema_no_pronunciado && (cuenta_hasta_salto_1 == 0) ) {

	cuenta_hasta_salto_1 = cuenta_hasta_salto_2;
	cuenta_hasta_salto_2 = 0;
	strcpy(cont_izquierda, correcaminos->alof);
	correcaminos++;
	fonema_no_pronunciado--;
	continue;
      }

      recorre_parametros->Duracion = cont_derecha_real->inicio - fonema_actual->inicio;
      recorre_parametros->AnteriorPausa = tipo_pausa_anterior;
      if (sordo_sonoro(fonema_actual->fonema) == SORDO)
	recorre_parametros->sordo_sonoro = 1;
      else
	recorre_parametros->sordo_sonoro = 0;
      strcpy(recorre_parametros->Alofono, fonema_actual->fonema);
      if (recorre_parametros->Alofono[strlen(recorre_parametros->Alofono) - 1] == '!')
	recorre_parametros->Alofono[strlen(recorre_parametros->Alofono) - 1] = '\0';
      strcpy(recorre_parametros->Fut1, cont_derecha_real->fonema);
      if (recorre_parametros->Fut1[strlen(recorre_parametros->Fut1) - 1] == '!')
	recorre_parametros->Fut1[strlen(recorre_parametros->Fut1) - 1] = '\0';
      strcpy(recorre_parametros->Fut2, cont_derecha_real_2->fonema);
      if (recorre_parametros->Fut2[strlen(recorre_parametros->Fut2) - 1] == '!')
	recorre_parametros->Fut2[strlen(recorre_parametros->Fut2) - 1] = '\0';
      strcpy(recorre_parametros->Pas1, cont_izquierda_real->fonema);
      if (recorre_parametros->Pas1[strlen(recorre_parametros->Pas1) - 1] == '!')
	recorre_parametros->Pas1[strlen(recorre_parametros->Pas1) - 1] = '\0';
      strcpy(recorre_parametros->Pas2, cont_izquierda_real_2->fonema);
      if (recorre_parametros->Pas2[strlen(recorre_parametros->Pas2) - 1] == '!')
	recorre_parametros->Pas2[strlen(recorre_parametros->Pas2) - 1] = '\0';
      recorre_parametros->Tonica = tonicidad;
      recorre_parametros->FonemasPalabra = fonemas_palabra;
      recorre_parametros->FonemasSilaba = numero_fonemas_silaba;
      recorre_parametros->PosEnSilaba = posicion_en_silaba;
      strcpy(recorre_parametros->fichero_origen, fichero_origen);
      recorre_parametros->FronteraInicial = frontera_inicial;
      if (recorre_parametros->FronteraInicial == FRONTERA_GRUPO_ENTONATIVO)
	recorre_parametros->FronteraInicial = FRONTERA_PALABRA;				   
      recorre_parametros->FronteraFinal = frontera_final;
      if (recorre_parametros->FronteraFinal == FRONTERA_GRUPO_ENTONATIVO)
	recorre_parametros->FronteraFinal = FRONTERA_PALABRA;
      recorre_parametros->SilabasFinal = numero_silabas_hasta_final;
      recorre_parametros->SilabasInicio = numero_silabas_desde_inicio;
      recorre_parametros->TonicasInicio = numero_tonicas_desde_inicio;
      recorre_parametros->TonicasFinal = numero_tonicas_hasta_final;
      recorre_parametros->TipoProposicion = tipo_de_tonema;
      strcpy(recorre_parametros->palabra, palabra_contexto);
          

      if ( ( ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) || 
	     ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	   //	   ( (frontera_final == FRONTERA_PALABRA) || 
	   //  (frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) &&
	   (indice_silaba_palabra == silabas_de_palabra - 1) )
	recorre_parametros->FinalFrase = UNIDAD_RUPTURA; /// RUPTURA_ENTONATIVA o RUPTURA_COMA.
      else
	if (palabra_separada != frase_separada) {
	  if ( ( ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
		 ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	       //	       ( (frontera_inicial == FRONTERA_PALABRA) ||
	       //	 (frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) &&
	       (indice_silaba_palabra == 0) )
	    recorre_parametros->FinalFrase = UNIDAD_RUPTURA;
	  else
	    recorre_parametros->FinalFrase = final_frase;
	}
	else
	  recorre_parametros->FinalFrase = final_frase;

      if (fonema_actual->fonema[0] == '#') {
	if (primera_frase_fichero == 1) {
	  anterior_palabra++;
	  //                anterior_pausa++;
	} // if (primera_frase_fichero...
	if (correcaminos != frase_prosodica)
	  tipo_pausa_anterior = tipo_siguiente_pausa;
      }

      if ( (correcaminos->alof[0] != '#') || (primera_frase_fichero == 1) ||
	   (primera == 0) )
	recorre_parametros++;

      primera = 0;

      // Actualizamos el contexto por la derecha.

      if ( (correcaminos + 1)->alof[0] )
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");
    }

    // Actualizamos el contexto por la izquierda.
    strcpy(cont_izquierda, correcaminos->alof);
    correcaminos++;

    // Avanzamos los contextos reales.

    if  ( ( (correcaminos + 1)->alof[0] != '\0') ||
	  ( (correcaminos - 1)->alof[0] != '#') ) {

      cont_izquierda_real_2 = cont_izquierda_real;
      cont_izquierda_real = fonema_actual;
      fonema_actual = cont_derecha_real;
      cont_derecha_real = cont_derecha_real_2;

      cont_derecha_real_2++;

      if (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+')
	cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] = '\0';

      while ( (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '*') ||
	      (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '-') ) {
	cont_derecha_real_2++;
	if (cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] == '+')
	  cont_derecha_real_2->fonema[strlen(cont_derecha_real_2->fonema) - 1] = '\0';
	(*numero_elementos_duracion)--;
	if (cuenta_hasta_salto_1 == 0)
	  cuenta_hasta_salto_1 = 3;
	else {
	  cuenta_hasta_salto_2 = (short int) (5 - fonema_no_pronunciado);
	}
	fonema_no_pronunciado++;
      } // while (cont_derecha_real_2...

      indice_alofono_actual++;
    } // if ( (correcaminos + 1)->alof[0] != 0
  }

  recorre_parametros->FonemasPalabra = 0;
  recorre_parametros->SilabasPalabra = 0;
    
  primera_frase_fichero = 0;

  fclose(fichero_cambia_real);

  return 0;
    
}

#endif

#if defined(_SCRIPT_DURACION) || defined(_MODO_NORMAL) || defined (_SCRIPT) || \
  defined(_CALCULA_DATOS_MODELO_ENERGIA)

/**
 * Función:   cuenta_fonemas_en_silaba
 * \remarks Entrada:
 *			- silaba: cadena en la que se encuentra la sílaba cuyo número de
 *            fonemas se desea contar.                                             
 * \remarks Objetivo:  Dado que algunos fonemas se representan por medio de dos caracteres, 
 *            no llega con calcular el tamaño de la cadena para saber su número de
 *            alófonos. Para solventar ese problema se emplea esta función.
 * \remarks NOTA:      Esta función sólo produce buenos resultados en el caso de que se
 *            emplee el código Sampa de Cotovía, en el que los únicos fonemas que
 *            se representan con dos caracteres son tS y rr.                       
 */

short int cuenta_fonemas_en_silaba(char silaba[]) {

  short int tamano = (short int) strlen(silaba);

  if (strstr(silaba, "tS"))
    tamano--;
  if (strstr(silaba, "rr"))
    tamano--;
  if (strstr(silaba, "^"))
    tamano--;
#ifdef _MODOA_EU
  if (strstr(silaba, "ts`"))
    tamano-=2;
  else if(strstr(silaba, "ts"))
    tamano--;
  else if (strstr(silaba, "s`"))
    tamano--;
  if (strstr(silaba, "jj"))
    tamano--;
  if (strstr(silaba, "gj"))
    tamano--;
#endif

  return tamano;

}


/**
 * Función:   calcula_posicion_frase_tonicidad_silabas
 * \remarks Entrada:
 *			- f_fonetica: cadena con la información de tonicidad, sílabas, y
 *            tokens de pausas y proposiciones.                                    
 *          - alof: alófono del que queremos hallar su posición en la frase y
 *            tonicidad.                                                           
 *	\remarks Salida:
 *			- posicion: posición del alófono en la frase. Si está antes de la
 *            primera sílaba tónica (incluida), se considera POSICION_INICIAL.
 *            Si está después de la última (incluida), se considera POSICION_FINAL.
 *            En otro caso, se considera POSICION_MEDIA.                           
 *          - tonica: tonicidad del alófono, considerada ésta como algo propio de
 *            la sílaba, y no de un único fonema de ella. (Tónica -> 1).
 *          - fonemas_en_silaba: número de fonemas de la sílaba actual.
 *          - fonemas_en_palabra: número de fonemas de la palabra actual.
 *          - silabas_en_palabra: número de sílabas de la palabra_actual.          *
 *          - posicion_silaba: posición del alófono en la sílaba que lo contiene.
 *          - posicion_inicio: número de sílabas desde el inicio del grupo entre
 *            pausas.                                                              
 *          - posicion_final: número de sílabas hasta el final del grupo entre
 *            pausas.                                                              
 *          - tonicas_inicio: número de sílabas tónicas desde el inicio del grupo.
 *          - tonicas_final: número de sílabas tónicas hasta el final del grupo.
 *          - nueva_silaba: variable que indica cuándo se comienza una nueva
 *            sílaba. (comienzo de sílaba -> 1; otro caso -> 0)
 *          - fin_silaba: equivalente al anterior, pero relativo al final de la    
 *            sílaba.
 *          - silaba: sílaba actual. Su valor sólo es real cuando nueva_silaba == 1
 *          - frontera_inicial: indica la frontera al inicio del fonema (según la clasificación al inicio de descriptor.hpp).
 *          - frontera_final: indica la frontera al final del fonema (según la clasificación al inicio de descriptor.hpp).
 *	    - final_frase: indica si está al final de la oración.		*           - indice_silaba: indice de la sílaba en la que se encuentra el fonema (en relación a las sílabas de la palabra)
		   
 * \remarks Valor devuelto:                                                                 
 *          - En ausencia de error, devuelve un 0.                                 
 * \remarks Objetivo:  Calcular las características de tonicidad y posición en la frase de
 *            cada alófono, así como en la sílaba que lo contiene.
 */

int Crea_vectores_descriptor::calcula_posicion_frase_tonicidad_silabas(char *f_fonetica, char alof[], unsigned char *posicion, unsigned char *tonica, short int *fonemas_en_silaba, short int *fonemas_en_palabra, short int *silabas_en_palabra, short int *posicion_silaba, short int *posicion_inicio, short int *posicion_final, short int *tonicas_inicio, short int *tonicas_final, char *nueva_silaba, char *fin_silaba, char *silaba_actual, char *frontera_inicial, char *frontera_final, char *final_frase, unsigned char *indice_silaba) {

  char silaba[LONX_MAX_PAL_SIL_E_ACENTUADA];
  int tamano_alof = strlen(alof);

  static char *correcaminos = NULL;
  static int cuenta_fonemas_palabra;
  char inicio_palabra = 0;

  if (frontera_inicial)
    *frontera_inicial = SIN_FRONTERA;
  if (frontera_final)
    *frontera_final = SIN_FRONTERA;
  if (fin_silaba)
    *fin_silaba = 0;
  if (nueva_silaba)
    *nueva_silaba = 0;
  if (final_frase)
    *final_frase = UNIDAD_NO_FRONTERA;

  if (alof[0] == '#') {
    *tonica = 0;
    *posicion = POSICION_INICIAL; // Por poner algo.
    *fonemas_en_silaba = 0;
    *posicion_silaba = 0;
    *posicion_inicio = 0;
    *posicion_final = 0;
    *tonicas_inicio = 0;
    *tonicas_final = 0;
    return 0;
  } // if (alof[0] == '#')

  if (numero_elementos < -1) {
    fprintf(stderr, "Error en calcula_posicion_frase_tonicidad_silabas: formato de frase erróneo.\n");
    return 1;
  }

  if (numero_elementos) {
    numero_elementos -= tamano_alof;
  }
  else
    if (--fragmentos_entre_pausas) {
      // Reiniciamos las variables necesarias para el cálculo de la posición en la frase
      // y la tonicidad silábica.
      reinicia_variables_posicion_y_tonicidad();
      reinicia_variables_posicion_silabica();
      if ( (frase = extrae_frase_y_posiciones(f_fonetica)) == NULL)
	return 1;
      else {
	numero_elementos -= tamano_alof;
	correcaminos = frase;
      }
    }

  if (!correcaminos && !tamano_silaba) // La frase de entrada sólo estaba constituida por tokens,
    return 0;          // o bien ya hemos llegado a la última sílaba y los valores de tónica y posicion son constantes.

  if (frontera_inicial)
    if ( (correcaminos == frase) || (*(correcaminos - 1) == ' ') ) {
      char nueva_palabra[255], *recorre_palabra = nueva_palabra;
      inicio_palabra = 1;

      // Ojo, ¿FRONTERA_GRUPO_ENTONATIVO?
      //      *frontera_inicial = FRONTERA_PALABRA;
      tamano_palabra = (short int) strcspn(correcaminos, " #");
      memcpy(nueva_palabra, correcaminos, tamano_palabra);
      nueva_palabra[tamano_palabra] = '\0';

      // Calculamos el número de fonemas de la palabra

      while (*recorre_palabra != '\0') {
	switch (*recorre_palabra) {
	case '-':
#ifdef _MODOA_EU
	case '`':
#endif
	case '^':
	  tamano_palabra--;
	break;
	case 'r':
	  if (*(recorre_palabra + 1) == 'r') {
	    tamano_palabra--;
	    recorre_palabra++;
	  }
	  break;
	case 't':
#ifdef _MODOA_EU
	  if (*(recorre_palabra + 1) == 'S' || *(recorre_palabra + 1) == 's') 
#else
	    if (*(recorre_palabra + 1) == 'S') 
#endif
	      {
		tamano_palabra--;
		recorre_palabra++;
	      }
	  break;
#ifdef _MODOA_EU
	case 'g':
	  if (*(recorre_palabra + 1) == 'j') {
	    tamano_palabra--;
	    recorre_palabra++;
	  }
	  break;
	case 'j':
	  if (*(recorre_palabra + 1) == 'j') {
	    tamano_palabra--;
	    recorre_palabra++;
	  }
	  break;
#endif
	default:
	  break;
	}
	recorre_palabra++;
      } // while (*recorre_palabra...
      if (fonemas_en_palabra)
	*fonemas_en_palabra = tamano_palabra;
      cuenta_fonemas_palabra = tamano_palabra;

      if (silabas_en_palabra) {
	recorre_palabra = (char *) nueva_palabra;
	numero_silabas_palabra = 1;
	while (*recorre_palabra != '\0') {
	  // Contamos el número de sílabas
	  switch(*recorre_palabra) {
	  default:
	    break;
	  case '-':
	    numero_silabas_palabra++;
	  } // switch
	  recorre_palabra++;
	} // while (*recorre_palabra...
	*silabas_en_palabra = numero_silabas_palabra;
      } // if (silabas_en_palabra...
    } // if ( (correcaminos == frase...
    else {
      //      *inicio_palabra = 0;
      if (fonemas_en_palabra)
	*fonemas_en_palabra = tamano_palabra;
      if (silabas_en_palabra)
	*silabas_en_palabra = numero_silabas_palabra;
    } // else

  if (frontera_final)
    if (--cuenta_fonemas_palabra == 0)
      *frontera_final = FRONTERA_PALABRA;

  if (tamano_silaba) { // Continuamos en la misma sílaba.
    posicion_en_silaba += (short int) tamano_alof;
    *posicion_silaba = posicion_en_silaba;
    *posicion_inicio = (short int) cuenta_silabas;
    *posicion_final = (short int) (silabas_totales - cuenta_silabas);
    *tonicas_inicio = tonica_actual;
    *tonicas_final = (short int) (tonicas_totales - tonica_actual);
    tamano_silaba -= (unsigned char) tamano_alof;
    if (tamano_silaba == 0)
      if (fin_silaba) {
	*fin_silaba = 1;
	if (frontera_final)
	  if (*frontera_final != FRONTERA_PALABRA)
	    *frontera_final = FRONTERA_SILABA;
      }
  }
  else { // cambiamos de sílaba.
    //    if (frontera_inicial)
    //  if (*frontera_inicial != FRONTERA_PALABRA)
    //	*frontera_inicial = FRONTERA_SILABA;

    while ( (*correcaminos == '-') || (*correcaminos == '^') )
      correcaminos++;

    if (*correcaminos == '\0') {
      fprintf(stderr, "Error en calcula_posicion_frase_tonicidad_silabas: frase fonética no válida.\n");
      return 1;
    }

    posicion_en_silaba = 0;
    cuenta_silabas++;

    tamano_silaba = (unsigned char) strcspn(correcaminos, " -"); // El límite de la sílaba viene dado por uno de estos delimitadores.
    strncpy(silaba, correcaminos, tamano_silaba);
    silaba[tamano_silaba] = '\0';

    numero_fonemas_silaba = cuenta_fonemas_en_silaba(silaba);

    //      correcaminos += tamano_silaba + 1;

    if (nueva_silaba) {
      if (correcaminos[tamano_silaba] != '-') {
	*nueva_silaba = 2; // última sílaba de la palabra
      }
      else {
	*nueva_silaba = 1;
      }
      if (silaba_actual){
	strcpy(silaba_actual, silaba);
      }
    }

    if (strstr(silaba, "^")) { // Si en la sílaba está el circunflejo, es tónica.
      tonica_actual++;
      tonicidad = 1;
      tamano_silaba -= (unsigned char) (1 + tamano_alof);
    }
    else {
      tonicidad = 0;
      tamano_silaba -= (unsigned char) tamano_alof;
    }

    if (fin_silaba)
      if (tamano_silaba == 0)
	*fin_silaba = 1;
    
    *posicion_silaba = posicion_en_silaba;
    *posicion_inicio = cuenta_silabas;
    *posicion_final = (short int) (silabas_totales - cuenta_silabas);
    *tonicas_inicio = tonica_actual;
    *tonicas_final = (short int) (tonicas_totales - tonica_actual);
    *fonemas_en_silaba = numero_fonemas_silaba;
  }

  correcaminos += tamano_alof;

  while ( (*correcaminos == '-') || (*correcaminos == '^') || (*correcaminos == ' ') )
    correcaminos++;

  *tonica = tonicidad;

  if (frontera_inicial) {

    if (nueva_silaba) {
      if (*nueva_silaba != 0) {
	if (*posicion_inicio == 0) {
	  *frontera_inicial = FRONTERA_GRUPO_ENTONATIVO;
	  indice_silaba_palabra = 0;
	}
	else if (inicio_palabra == 1) {
	  *frontera_inicial = FRONTERA_PALABRA;
	  indice_silaba_palabra = 0;
	}
	else {
	  *frontera_inicial = FRONTERA_SILABA;
	  indice_silaba_palabra++;
	}
      }
    }

  } // if (frontera_inicial)

  if (indice_silaba)
    *indice_silaba = indice_silaba_palabra;

  if (frontera_final) {
    if (fin_silaba) {
      if (*fin_silaba != 0) {
	if (*posicion_final == 1)
	  *frontera_final = FRONTERA_GRUPO_ENTONATIVO;
	else if (*(correcaminos - 1) == ' ')
	  *frontera_final = FRONTERA_PALABRA;
	else
	  *frontera_final = FRONTERA_SILABA;
      }
    }
  } // if (frontera_final)

  // Ahora tenemos que comprobar la posición del alófono en la frase.

  if (cuenta_silabas >= segundo_acento) {
    *posicion = POSICION_FINAL;
    if (fragmentos_entre_pausas == 1)
      *final_frase = UNIDAD_FIN_FRASE;
  }
  else
    if (cuenta_silabas <= primer_acento) {
      *posicion = POSICION_INICIAL;
      if (final_frase)
	if (fragmentos_entre_pausas == 1)
	  *final_frase = UNIDAD_NO_FRONTERA;
    }
    else
      if (cuenta_silabas < segundo_acento) {
	*posicion = POSICION_MEDIA;
	if (final_frase)
	  if (fragmentos_entre_pausas == 1)
	    *final_frase = UNIDAD_NO_FRONTERA;
      }
      else {
	*posicion = POSICION_FINAL;
	if (final_frase)
	  if (fragmentos_entre_pausas == 1)
	    *final_frase = UNIDAD_FIN_FRASE;
      }

  return 0;

}

#endif


#ifdef _MODO_NORMAL

/**
 * Función:   crea_vectores_descriptor
 * Versión:   modo_normal.
 * \param	frase_separada: cadena que contiene la información de cada palabra procesada.
 *            Se emplea para obtener la información de palabra contexto y tipo de frase.
 * \param	frase_prosodica: cadena en la que se encuentra la información de cada fonema
 *            por separado.
 * \param	frase_en_grupos: división en grupos de la frase de entrada.
 * \param	frase_fonetica: cadena en la que se encuentra la información silábica, de
 *            tonicidad y de pausas.
 * \param	frase_sintagmada: división en sintagmas de la frase de entrada.
 * \param	esc_temp: valor del escalado temporal, normalizado.
 * \param	esc_pitch: valor del escalado del pitch, normalizado.
 * \retval	vectores: cadena indexada con la información de las unidades objetivo.
 * \retval	numero_unidades: número de unidades de la cadena anterior.
 * \retval	acentuales: cadena indexada con la información de los grupos acentuales
 *            objetivo.
 * \retval	numero_acentuales: número de unidades de la cadena anterior.
 * \return	En ausencia de error devuelve un 0.
 * \remarks Objetivo:  Calcula las características de los fonemas de frase_prosodica, basándose en las
 *            predicciones de Cotovia. De aquí saldrán las unidades que se considerarán como
 *            objetivo en el momento de realizar la síntesis.
 */

#ifndef _ESTIMA_ENERGIA_RED_NEURONAL
int Crea_vectores_descriptor::crea_vectores_descriptor(char * frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, float esc_temp, float esc_pitch, char difo, int numero_de_elementos_de_frase_separada, struct Modelo_energia * modelo_energia, struct Modelo_duracion * modelo_duracion, vector<Vector_descriptor_objetivo> &vectores, int *numero_unidades,
#ifdef _MINOR_PHRASING_TEMPLATE
						       Frase_frontera *frase_frontera,	
#endif
						       Grupo_acentual_objetivo **acentuales,
						       int *numero_acentuales) {
#else

  int Crea_vectores_descriptor::crea_vectores_descriptor(char * frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_en_grupos *frase_en_grupos, t_frase_sintagmada *frase_sintagmada, float esc_temp, float esc_pitch, char difo, int numero_de_elementos_de_frase_separada, Red_neuronal *red_energia, Media_varianza_energia *medias_varianzas_energia, struct Modelo_duracion * modelo_duracion, vector<Vector_descriptor_objetivo> &vectores,  int *numero_unidades,
#ifdef _MINOR_PHRASING_TEMPLATE
							 Frase_frontera *frase_frontera,	
#endif

							 Grupo_acentual_objetivo **acentuales,
							 int *numero_acentuales) {
#endif

    int error_ejecucion = 0; // Variable que controla si se ha producido o no algún error en la
    // ejecución de la función.
    int memoria_reservada_unidades = 0 ; // Variable que indica el número de posiciones reservadas para
    // la cadena de unidades.
    int memoria_reservada_acentuales; // Variable que indica el número de posiciones reservadas
                                      // para la cadena de grupos acentuales.

    static int numero_frase = 0; // Número de la frase en el fichero que se está sintetizando.
    
    // Punteros a las cadenas de entrada.
    t_frase_separada *palabra_separada = frase_separada;
    char *f_fonetica = frase_fonetica;
    t_prosodia *correcaminos = frase_prosodica;
    t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;
    t_frase_sintagmada *recorre_frase_sintagmada = frase_sintagmada;
    
    t_palabra_proc palabra_contexto; // Palabra a la que pertenece la unidad considerada.

    char fut1[OCTETOS_POR_FONEMA + 1] = "#", pas1[OCTETOS_POR_FONEMA + 1] = "#"; // Fonemas por la derecha y por la izquierda
    char pas2[OCTETOS_POR_FONEMA + 1] = "#", fut2[OCTETOS_POR_FONEMA + 1] = "#";

    short int numero_fonemas_silaba; // Número de fonemas contenidos en la sílaba actual.
    short int posicion_en_silaba;    // Posición del fonema dentro de la sílaba.
    short int numero_silabas_desde_inicio = 0; // Número de sílabas hasta la actual, dentro de su
                                           // grupo entre pausas.
    short int numero_silabas_hasta_final = 0;  // Número de sílabas hasta la final dentro de su
                                           // grupo entre pausas.
    short int numero_tonicas_desde_inicio = 0; // Número de sílabas tónicas desde el inicio del grupo.
    short int numero_tonicas_hasta_final = 0;  // Número de sílabas tónicas hasta el final del grupo.

    unsigned char tipo_siguiente_sintagma;
    
    int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
    int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.

    Vector_descriptor_objetivo nuevo_vector; //Objeto en el que se almacena la información relativa a la unidad.
    Grupo_acentual_objetivo *recorre_acentuales; // puntero a la cadena de grupos acentuales objetivo.

    // Variables para determinar el tipo de frase a la que pertenece el fonema actual.
    unsigned char tipo_de_tonema; // Consideramos tres: ENUNCIATIVA, EXCLAMATIVA e INTERROGATIVA.
    unsigned char final_tonema_frase_separada; // Índice de frase_separada en el que se acaba el tonema actual.

    char primera = 1; // Variable que controla la ejecución de la primera vuelta del bucle.
    char no_primera_acentual = 0; // Variable análoga a la anterior, para los grupos acentuales.
    // Variables que controlan cuándo hay que acceder a las palabras
    int numero_palabra = 0; // // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
    // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
    char ciclo_transformada = 0;
    char no_final_ciclo = 1;

    t_palabra_proc parte_transformada, parte; // Variables en las que se almacenan la palabra transformada completa y
    // la palabra dentro de ella a la que nos referimos.


    short int numero_fonemas_palabra, numero_silabas_de_palabra;
    char fin_de_silaba;

    unsigned char posicion_frase; // Posición del fonema dentro de la frase  (inicial, media o final).
    unsigned char tonicidad; // Tonicidad del fonema, considerada como un rasgo de la sílaba. (0 -> átona).

    char final_frase = 0;	// 1 -> si está al final de la oración. 2 -> si está en las inmediaciones de una ruptura entonativa. 0 en otro caso.

    float duracion1 = 0.0, duracion2 = 0.0; // Variables para almacenar la duración de la unidad.
    float duracion_desde_pausa = 0.0; // Variable para el modelo de duraciones, que cuenta el tiempo transcurrido
                                      // desde la última pausa.
    float media_pitch; // Variable para almacenar la media del pitch.

    float duracion_grupo_acentual = 0; // Variable que recoge la duración estimada para el grupo
                                       // acentual.
    float duracion_silaba = 0; // duración de la sílaba actual.
    char alofono_acento;        // Variable si un alófono lleva o no acento.
    char nuevo_grupo = 0; // Variable que controla el inicio de un grupo acentual.
    short int numero_alofonos_grupo = 0; // Variable que controla el número de alófonos de cada grupo
    // acentual.
    int posicion_vocal_fuerte = -1; // Variable empleada para controlar la posición de la vocal fuerte de cada sílaba.
    int almacena_posicion_vocal_fuerte = 0;

    char silaba[LONX_MAX_PAL_SIL_E_ACENTUADA]; // Variable en la que se almacena la sílaba actual.
    silaba[0]='\0';

    unsigned char posicion_alofono_tonico = 0; // Posición del alófono en el que recae el acento.
    unsigned char indice_silaba_palabra; // Índice de la sílaba en la que se encuentra el fonema (con respecto a la palabra)
    char nueva_silaba; // Variable que indica el comienzo de una nueva sílaba.
    unsigned char indice_silaba = 0; // Número de sílaba en el grupo acentual.
    char frontera_inicial, frontera_final;
    char tipo_de_pausa;
    char tipo_de_pausa_anterior = PAUSA_PUNTO;
    char unidad_no_silencio = 0;
    char tipo_silaba, tipo_silaba_anterior = RESTO_SILABAS;
    
    //    char ruptura_prosodica;

#ifdef _MINOR_PHRASING_TEMPLATE
    POS_frontera pos_frontera;
    Sintagma_frontera sintagma_frontera;
    Grupo_fonico_frontera grupo_fonico_frontera;
#endif

    Parametros_duracion datos_prosodia;
    Parametros_modelo_energia datos_energia;

    datos_energia.energia = 10*log10(MINIMA_ENERGIA_INF);
    datos_energia.energia_media_fichero =  ENERGIA_MEDIA_FRASE;

    *numero_unidades = 0;
    *numero_acentuales = 1;

    // Comprobamos que no se trate de una frase con sólo signos de puntuación:

    if (frase_solo_signos_de_puntuacion(frase_separada))
      return 0;

    // Calculamos el tipo del primer tonema y hasta dónde llega.

    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);


    // Inicializamos las variables necesarias para el cálculo de la posición en la frase
    // y la tonicidad.

    inicializa_calculo_posicion_y_tonicidad();

    // inicializamos a cero el número de unidades.

    *numero_unidades = 0;

    // Asignamos memoria para la cadena de unidades:

    vectores.clear();
    vectores.reserve(500);

    // Lo mismo, para la cadena de grupos acentuales

    if (*acentuales != NULL)
      free(*acentuales);

    memoria_reservada_acentuales = 30;
    if ( (*acentuales = (Grupo_acentual_objetivo *) malloc(memoria_reservada_acentuales*
							   sizeof(Grupo_acentual_objetivo))) == NULL) {
      fprintf(stderr, "Error en crea_vectores_descriptor, al asignar memoria.\n");
      vectores.clear();
      //      free(*vectores);
      return 1;
    }


    reinicia_grupos_acentuales();


    recorre_acentuales = *acentuales;

#ifdef _MINOR_PHRASING_TEMPLATE
    if ( (frase_separada->tonicidad_forzada == 0) &&
    	 (frase_sintagmada->inicio == frase_sintagmada->fin) &&
    	 (!LOCUCION(frase_separada->cat_gramatical[0])) &&
	 ((frase_separada->cat_gramatical[0] == RELA) ||
	  (frase_separada->cat_gramatical[0] == PREP) ||
	  (frase_separada->cat_gramatical[0] == CONTR_PREP_ART_DET) ||
	  (frase_separada->cat_gramatical[0] == PRON_PERS_AT) ||
	  (frase_separada->cat_gramatical[0] == PRON_PERS_AT_REFLEX) ||
	  (frase_separada->cat_gramatical[0] == PRON_PERS_AT_ACUS) ||
	  (frase_separada->cat_gramatical[0] == PRON_PERS_AT_DAT) ||
	  (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_AC) ||
	  (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_DAT_AC) ||
	  (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT) ||
	  (CONXUNCION(frase_separada->cat_gramatical[0]))) )
      {
	pos_frontera.etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(frase_separada);
	pos_frontera.tipo_pausa = 0;
	pos_frontera.salto_f0 = 0;
	pos_frontera.factor_caida = 0;
	sintagma_frontera.elementos.push_back(pos_frontera);
	sintagma_frontera.tipo = decide_sintagma_grupo_acentual(frase_separada->tipo_sintagma);
	sintagma_frontera.acento = 0; // sintagma átono
	grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	sintagma_frontera.elementos.clear();
      }
#endif

    // Entramos en el bucle principal.

    datos_prosodia.AnteriorPausa = PAUSA_PUNTO;
    datos_prosodia.SiguientePausa = encuentra_tipo_siguiente_pausa(palabra_separada);

    while (correcaminos->alof[0] != '\0') {

      //      nuevo_vector = &vectores[*numero_unidades];

      if (palabra_separada - frase_separada > recorre_frase_sintagmada->fin) {
#ifdef _MINOR_PHRASING_TEMPLATE
	if (sintagma_frontera.elementos.size()) {
	  grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	  sintagma_frontera.elementos.clear();
	}
	/*            if ( (frase_separada->tonicidad_forzada == 0) &&
		      (LOCUCION(frase_separada->cat_gramatical[0]) == 0) &&
		      ( (CONXUNCION(frase_separada->cat_gramatical[0])) ||
		      (frase_separada->cat_gramatical[0] == RELA) ) ) {
	*/

	if ( (palabra_separada->tonicidad_forzada == 0) &&
	     (!LOCUCION(palabra_separada->cat_gramatical[0])) &&
	     ((palabra_separada->cat_gramatical[0] == RELA) ||
	      (palabra_separada->cat_gramatical[0] == PRON_PERS_AT) ||
	      (palabra_separada->cat_gramatical[0] == PRON_PERS_AT_REFLEX) ||
	      (palabra_separada->cat_gramatical[0] == PRON_PERS_AT_ACUS) ||
	      (palabra_separada->cat_gramatical[0] == PRON_PERS_AT_DAT) ||
	      (palabra_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_AC) ||
	      (palabra_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_DAT_AC) ||
	      (palabra_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT) ||
	      (CONXUNCION(palabra_separada->cat_gramatical[0]))) ) {
	  pos_frontera.etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(palabra_separada);
	  pos_frontera.tipo_pausa = 0;
	  pos_frontera.salto_f0 = 0;
	  pos_frontera.factor_caida = 0;
	  sintagma_frontera.elementos.push_back(pos_frontera);
	  sintagma_frontera.tipo = decide_sintagma_grupo_acentual(palabra_separada->tipo_sintagma);
	  sintagma_frontera.acento = 0; // sintagma átono
	  grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	  sintagma_frontera.elementos.clear();
	}
#endif
	while (palabra_separada - frase_separada > recorre_frase_sintagmada->fin)
	  recorre_frase_sintagmada++;

      } // if (frase_separada - original_frase_separada > ...

      if (primera == 0) {
        if ( ( (correcaminos - 1)->alof[0] == '#') && ((correcaminos - 1)->alof[1] == '\0' ) )
	  duracion_desde_pausa = 0.0;
        else
	  duracion_desde_pausa += 2*duracion1; // El 2, porque se refiere al fonema completo,

      } // if (primera == 0)

      if (palabra_separada->clase_pal == SIN_CLASIFICAR) {
        if (palabra_solo_signos_puntuacion(palabra_separada->pal))
	  palabra_separada->clase_pal = SIGNO_PUNTUACION;
      }


      if ( (primera == 0) &&
           ( (correcaminos->alof[0] == '#') && ( (correcaminos - 1)->alof[0] == '#')) ) {
	correcaminos++;
	continue;
      }

      if (*numero_unidades > 0) {
	strcpy(pas2, pas1);
	strcpy(pas1, (correcaminos - 1)->alof);
      }

      if ( (correcaminos + 1)->alof[0] != '\0') {
	strcpy(fut1, (correcaminos + 1)->alof);
	if ( (correcaminos + 2)->alof[0] != '\0')
	  strcpy(fut2, (correcaminos + 2)->alof);
	else
	  strcpy(fut2, "#");
      }
      else {
        strcpy(fut1, "#");
        strcpy(fut2, "#");
      }

      nuevo_vector.inicializa();

      // Calculamos la posición del fonema y su tonicidad.

      if (calcula_posicion_frase_tonicidad_silabas(f_fonetica, correcaminos->alof, &posicion_frase, &tonicidad, &numero_fonemas_silaba, &numero_fonemas_palabra, &numero_silabas_de_palabra, &posicion_en_silaba, &numero_silabas_desde_inicio, &numero_silabas_hasta_final, &numero_tonicas_desde_inicio, &numero_tonicas_hasta_final, &nueva_silaba, &fin_de_silaba, silaba, &frontera_inicial, &frontera_final, &final_frase, &indice_silaba_palabra)) {

	error_ejecucion = 1;
	break;
      }

      // Calculamos los parámetros relativos a los grupos acentuales:

      if (correcaminos->alof[0] != '#') {

    	unidad_no_silencio = 1;

        if (fin_grupo_acentual(frase_fonetica, correcaminos->alof, &nuevo_grupo, &alofono_acento, &tipo_de_pausa)) {
	  error_ejecucion = 1;
	  break;
        }

        if (nuevo_grupo) {

	  posicion_alofono_tonico = 0; // NUEVO

	  if (no_primera_acentual) {

	    recorre_acentuales->duracion = duracion_grupo_acentual; // en segundos
	    duracion_grupo_acentual = 0;
	    recorre_acentuales->numero_alofonos = numero_alofonos_grupo;
	    numero_alofonos_grupo = 0;

	    recorre_acentuales->tipo_pausa_anterior = tipo_de_pausa_anterior;

	    if ( ( ( (palabra_separada + 1)->cat_gramatical[0] == GUION) ) &&
		 (tipo_de_pausa != 0) ) {
	      //                         ( (palabra_separada + 1)->cat_gramatical[0] == DOBLES_COMINNAS)  ||
	      //                         ( (palabra_separada + 1)->cat_gramatical[0] == SIMPLES_COMINNAS) ||
	      //                         ( (palabra_separada + 1)->cat_gramatical[0] == GUION_BAIXO) ) {
	      recorre_acentuales->tipo_pausa = PAUSA_OTRO_SIGNO_PUNTUACION;
	    }
	    else
	      recorre_acentuales->tipo_pausa = tipo_de_pausa;

	    tipo_de_pausa_anterior = recorre_acentuales->tipo_pausa;

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
	    recorre_acentuales->etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(palabra_separada);

	    recorre_acentuales->sintagma_actual = decide_sintagma_grupo_acentual(palabra_separada->tipo_sintagma);
	    recorre_acentuales->numero_elementos_sintagma = (unsigned char) cuenta_elementos_tonicos_sintagma(frase_sintagmada, frase_separada, palabra_separada);
                    

	    // Cambio
	    devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, palabra_separada - frase_separada,
						      &tipo_siguiente_sintagma);
	    recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);
	    //

#ifdef _MINOR_PHRASING_TEMPLATE
	    pos_frontera.etiqueta_morfosintactica = recorre_acentuales->etiqueta_morfosintactica;
	    pos_frontera.tipo_pausa = recorre_acentuales->tipo_pausa;
	    //			pos_frontera.salto_f0 = recorre_acentuales->salto_f0;
	    //            pos_frontera.factor_caida = recorre_acentuales->factor_caida;

            sintagma_frontera.elementos.push_back(pos_frontera);
            sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
            sintagma_frontera.acento = 1; // sintagma tónico

	    if (palabra_separada - frase_separada > recorre_frase_sintagmada->fin) {
	      // Cambiamos de sintagma:
	      //                sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	      grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	      sintagma_frontera.elementos.clear();
            }

	    if ( (recorre_acentuales->tipo_pausa != SIN_PAUSA) &&
            	 (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) &&
                 (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_COMA) ) {

	      if (sintagma_frontera.elementos.size()) {
		//					sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
		grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
		sintagma_frontera.elementos.clear();
	      }

	      grupo_fonico_frontera.tipo_grupo = recorre_acentuales->tipo_proposicion;
	      grupo_fonico_frontera.siguiente_sintagma = recorre_acentuales->siguiente_sintagma;
	      //                grupo_fonico_frontera.identificador = identificador_GF++;

	      if (frase_frontera->elementos.size() == 0)
		grupo_fonico_frontera.posicion = GRUPO_FONICO_INICIAL;
	      else
		grupo_fonico_frontera.posicion = GRUPO_FONICO_MEDIO;

	      frase_frontera->elementos.push_back(grupo_fonico_frontera);
	      grupo_fonico_frontera.elementos.clear();
	    }
#endif

	    /*
	      if (recorre_frase_sintagmada->fin == palabra_separada - frase_separada) {

	      devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, palabra_separada - frase_separada,
	      &tipo_siguiente_sintagma);

	      recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);

	      }
	      else {
	      if (recorre_acentuales->sintagma_actual == GA_S_PREPOSICIONAL)
	      recorre_acentuales->siguiente_sintagma = GA_S_NOMINAL;
	      else
	      recorre_acentuales->siguiente_sintagma = recorre_acentuales->sintagma_actual;
	      }
	    */
#endif
	    recorre_acentuales->silabas[indice_silaba++].tiempo_silaba =
	      duracion_silaba;

	    if (++(*numero_acentuales) >= memoria_reservada_acentuales) {
	      memoria_reservada_acentuales += 100;
	      if ( (*acentuales = (Grupo_acentual_objetivo *) realloc(*acentuales, memoria_reservada_acentuales*sizeof(Grupo_acentual_objetivo)))
		   == NULL) {
		fprintf(stderr, "Error en crea_vectores_descriptor, al reasignar memoria.\n");
		error_ejecucion = 1;
		break;
	      }
	      recorre_acentuales = *acentuales + *numero_acentuales - 1;
	    } // if (++(*numero...
	    else
	      recorre_acentuales++;
	  } // if (no_primera_acentual
	  else
	    no_primera_acentual = 1;

	  if ( (recorre_acentuales->silabas = (estructura_silaba *) malloc(
									   numero_silabas_grupo_acentual*sizeof(estructura_silaba)))
	       == NULL) {
	    fprintf(stderr, "Error en crea_descriptores, al asignar memoria.\n");
	    error_ejecucion = 1;
	    break;
	  }
	  indice_silaba = 0;

	  recorre_acentuales->silabas[0].fonemas_por_silaba = numero_fonemas_silaba;
	  recorre_acentuales->silabas[0].tipo_silaba = tipo_silaba_anterior;
	  recorre_acentuales->tipo_proposicion = halla_tipo_de_frase(correcaminos->ti_prop);
	  recorre_acentuales->tipo_proposicion2 = correcaminos->ti_prop;
	  recorre_acentuales->posicion_silaba_acentuada = posicion_acento;
	  recorre_acentuales->posicion_grupo_prosodico = (short int) fragmento_actual;
	  recorre_acentuales->grupos_acentuales_grupo_prosodico = numero_grupos_acentuales;
	  recorre_acentuales->posicion_grupo_acentual = grupo_acentual_actual;
	  recorre_acentuales->numero_silabas = numero_silabas_grupo_acentual;
	  recorre_acentuales->numero_grupos_prosodicos = (short int) grupos_fonicos_total;
	  recorre_acentuales->duracion_hasta_acento = 0;
	  recorre_acentuales->indice_alofono_tonico = 0;
	  recorre_acentuales->numero_frase = numero_frase;
                
	  if (nueva_silaba) {
	    duracion_silaba = 0;
	    if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1) {
	      fprintf(stderr, "Error en crea_descriptores: Sílaba sin vocal fuerte.\n");
	      error_ejecucion = 1;
	      break;
	    }
	    almacena_posicion_vocal_fuerte = posicion_vocal_fuerte;
	  }

	} // if (nuevo_grupo...
	else
	  if (nueva_silaba) {
	    recorre_acentuales->silabas[indice_silaba++].tiempo_silaba =
	      duracion_silaba;
	    duracion_silaba = 0;
	    recorre_acentuales->silabas[indice_silaba].fonemas_por_silaba =
	      numero_fonemas_silaba;

	    if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1) {
	      fprintf(stderr, "Error en crea_descriptores: Sílaba sin vocal fuerte.\n");
	      error_ejecucion = 1;
	      break;
	    }
	    almacena_posicion_vocal_fuerte = posicion_vocal_fuerte;
	  }

      } // if (correcaminos->alof[0] != ...


      if (correcaminos->alof[0] == '#'){
        if (primera) {

	  tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  // Se trata del silencio inicial del fichero.
	  primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                    (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Cambié el > por >=
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	      error_ejecucion = 1;
	      break;
	    }

	    palabra_separada++;
	    if (palabra_separada->clase_pal == SIN_CLASIFICAR) {
	      if (palabra_solo_signos_puntuacion(palabra_separada->pal))
		palabra_separada->clase_pal = SIGNO_PUNTUACION;
	    }
	  } // while (palabra_separada...

	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...

	  if (palabra_separada->pal_transformada && palabra_separada->pal_transformada[0]) {
	    // Entramos en un ciclo de palabra_transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);

	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      error_ejecucion = 1;
	      break;
	    }

	    strcpy(palabra_contexto, parte);
	  } // if (palabra_separada->pal_transformada...
	  else
	    // No hay palabra transformada -> el contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  anterior_orden++;
	} // if (primera...
	else { // Actualizamos los campos de tipo de pausa.
	  datos_prosodia.AnteriorPausa = datos_prosodia.SiguientePausa;
	  datos_prosodia.SiguientePausa = (short int)
	    encuentra_tipo_siguiente_pausa(palabra_separada);
	} // else

      } // if (correcaminos->alof[0] == '#'...

        // Comprobamos si estamos en un ciclo de palabra transformada.

      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Avanzamos a la siguiente palabra contenida en palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    // El ciclo actual no ha terminado. Actualizamos el índice a la palabra a la
	    // que pertenece el alófono actual.
	    anterior_orden = correcaminos->pp;
	  else
	    // Ya hemos acabado con el ciclo de la última palabra transformada.
	    ciclo_transformada = 0;
	} // if (correcaminos->pp...

	if (no_final_ciclo) {
	  // Todavía no ha terminado el ciclo de palabra transformada.
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (char)
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	    error_ejecucion = 1;
	    break;
	  }

	  strcpy(palabra_contexto, parte);
	} // if (no_final_ciclo...

      } // if (ciclo_transformada...

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  // Actualizamos las varibles para el siguiente ciclo de palabra transformada.
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;

	  if (palabra_separada->clase_pal == SIN_CLASIFICAR) {
	    if (palabra_solo_signos_puntuacion(palabra_separada->pal))
	      palabra_separada->clase_pal = SIGNO_PUNTUACION;
	  }

	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    // Avanzamos palabra_separada hasta llegar a una palabra.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	      error_ejecucion = 1;
	      break;
	    }

	    palabra_separada++;

	    if (palabra_separada->clase_pal == SIN_CLASIFICAR) {
	      if (palabra_solo_signos_puntuacion(palabra_separada->pal))
		palabra_separada->clase_pal = SIGNO_PUNTUACION;
	    }
	  } // while (palabra_separada->...

	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  } // if (elementos_de_frase_separada...

	  // Actualizamos el índice a la palabra actual.
	  anterior_orden = correcaminos->pp;

	} // if (elementos_de_frase_separada...
	else {
	  fprintf(stderr, "Error en crea_descriptores: datos de entrada incorrectos.\n");
	  error_ejecucion = 1;
	  break;
	}

	if (palabra_separada->pal_transformada) {
	  // Comenzamos un ciclo de palabra transformada.
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {
	      error_ejecucion = 1;
	      break;
	    }

	    strcpy(palabra_contexto, parte);
	  } // if (no_final_ciclo...
	} // if (palabra_separada...
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      } // if (correcaminos->pp...

#ifdef _elimina_sil_sil

      if (  (fut1[0] != '#') || (pas1[0] != '#') ||
	    (correcaminos->alof[0] != '#') ) {

#endif
	// Introducimos los datos en el objeto de tipo vector descriptor.

	if (nuevo_vector.introduce_datos(correcaminos->alof, fut1, pas1, fut2, pas2, tonicidad, palabra_contexto, tipo_de_tonema, posicion_frase, frontera_inicial, frontera_final, nueva_silaba) ) {
	  error_ejecucion = 1;
	  (*numero_unidades)++;
	  //                nuevo_vector.libera_memoria();
	  break;
	} // if (nuevo_vector....

	
	if ( ( ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) || 
	       ( (palabra_separada + 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
	     //	     ( (frontera_final == FRONTERA_PALABRA) || 
	     //   (frontera_final == FRONTERA_GRUPO_ENTONATIVO) ) &&
	     ( (indice_silaba_palabra == numero_silabas_de_palabra - 1 ) ) )
	  nuevo_vector.frontera_prosodica = UNIDAD_RUPTURA; /// RUPTURA_ENTONATIVA o RUPTURA_COMA.
	else
	  if (palabra_separada != frase_separada) {
	    if ( ( ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
		   ( (palabra_separada - 1)->cat_gramatical[0] == RUPTURA_COMA) ) &&
		 //		 ( (frontera_inicial == FRONTERA_PALABRA) ||
		 //   (frontera_inicial == FRONTERA_GRUPO_ENTONATIVO) ) &&
		 ( (indice_silaba_palabra == 0) ) )
	      nuevo_vector.frontera_prosodica = UNIDAD_RUPTURA;
	    else
	      nuevo_vector.frontera_prosodica = final_frase;
	  }
	  else
	    nuevo_vector.frontera_prosodica = final_frase;

	// Añadimos la información relativa a la prosodia:

	strcpy(datos_prosodia.Alofono, correcaminos->alof);
	strcpy(datos_prosodia.Fut1, fut1);
	strcpy(datos_prosodia.Fut2, fut2);
	strcpy(datos_prosodia.Pas1, pas1);
	strcpy(datos_prosodia.Pas2, pas2);
	datos_prosodia.FinalFrase = final_frase;
	datos_prosodia.Tonica = tonicidad;
	datos_prosodia.FonemasPalabra = numero_fonemas_palabra;
	datos_prosodia.SilabasPalabra = numero_silabas_palabra;
	datos_prosodia.TipoProposicion = tipo_de_tonema;
	//	datos_prosodia.InicioSilaba = nueva_silaba;
	datos_prosodia.TonicasFinal = numero_tonicas_hasta_final;
	datos_prosodia.TonicasInicio = numero_tonicas_desde_inicio;
	datos_prosodia.SilabasInicio = numero_silabas_desde_inicio;
	datos_prosodia.SilabasFinal = numero_silabas_hasta_final;
	datos_prosodia.PosEnSilaba = posicion_en_silaba;
	datos_prosodia.FonemasSilaba = numero_fonemas_silaba;
	datos_prosodia.FronteraInicial = frontera_inicial;
	if (datos_prosodia.FronteraInicial == FRONTERA_GRUPO_ENTONATIVO)
	  datos_prosodia.FronteraInicial = FRONTERA_PALABRA;
      	datos_prosodia.FronteraFinal = frontera_final;
	if (datos_prosodia.FronteraFinal == FRONTERA_GRUPO_ENTONATIVO)
	  datos_prosodia.FronteraFinal = FRONTERA_PALABRA;
	//datos_prosodia.FinSilaba = fin_de_silaba;
	datos_prosodia.DuracionDesdePausa = duracion_desde_pausa;

	datos_energia.energia_fonema_anterior = datos_energia.energia;
	//			datos_energia.tipo_fonema_anterior = datos_energia.tipo_fonema;

	if (tipo_fon_espectral_enumerado(correcaminos->alof, &(datos_energia.tipo_fonema)))
	  return 1;

	if (tipo_fon_espectral_enumerado(pas1, &(datos_energia.tipo_fonema_anterior)))
	  return 1;

	if (tipo_fon_espectral_enumerado(fut1, &(datos_energia.tipo_fonema_siguiente)))
	  return 1;

	strcpy(datos_energia.fonema, correcaminos->alof);

	if (correcaminos->alof[0] == '#') {
	  datos_energia.fonemas_palabra = 0;
	  datos_energia.silabas_palabra = 0;
	  datos_energia.acento = 0;
	  datos_energia.frontera_inicial = 0;
	  datos_energia.frontera_final = 0;
	} // if (correcaminos->alof[0] == '#')
	else {
	  datos_energia.fonemas_palabra = numero_fonemas_palabra;
	  datos_energia.silabas_palabra = numero_silabas_palabra;
	  datos_energia.acento = tonicidad;
	  datos_energia.frontera_inicial = frontera_inicial;
	  datos_energia.frontera_final = frontera_final;
	} // else

	datos_energia.final_frase = final_frase;
	datos_energia.silabas_inicio = numero_silabas_desde_inicio;
	datos_energia.silabas_final = numero_silabas_hasta_final;
	datos_energia.silabas_frase = numero_silabas_desde_inicio +
	  numero_silabas_hasta_final;

	datos_energia.tipo_proposicion = tipo_de_tonema;

	datos_energia.posicion = posicion_frase;

	// Estimación de la energía:

#ifdef _ESTIMA_ENERGIA_RED_NEURONAL
	if (estima_energia_red_neuronal(&red_energia[datos_energia.tipo_fonema], &medias_varianzas_energia[datos_energia.tipo_fonema], &datos_energia)) {
	  (*numero_unidades)++;
	  error_ejecucion = 1;
	  break;
	} // if (estima_energia...
#else
	if (estima_energia(&datos_energia, modelo_energia)) {
	  (*numero_unidades)++;
	  error_ejecucion = 1;
	  break;
	} // if (estima_energia...
#endif
	nuevo_vector.potencia = datos_energia.energia;

	// Estimación de la duración:

	if ( (correcaminos->alof[0] != '#') && (difo != 1) ) {
	  if (estima_duracion_ms(&datos_prosodia, modelo_duracion)) {
	    //				if ( (datos_prosodia.Duracion = estima_duracion_ms_arbol_regresion(&datos_prosodia)) < 0) {
	    (*numero_unidades)++;
	    error_ejecucion = 1;
	    break;
	  } // if ( (duracion1 = estima...

	  //              if (palabra_separada->clase_pal != ABREVIATURA)
	  duracion1 = esc_temp*datos_prosodia.Duracion / 1000;
	  //              else
	  //              	  duracion1 = esc_temp*datos_prosodia.Duracion / 1000 ;
	}
	else
	  duracion1 = (float) esc_temp*correcaminos->dur1 / 1000;

	if ( (palabra_separada->clase_pal == PALABRA_TODO_MAIUSCULAS) ||
	     (palabra_separada->clase_pal == ABREVIATURA) ||
	     (palabra_separada->clase_pal == DELETREO) )
	  duracion1 *= 1.5;
                 
	if (correcaminos->alof[0] != '#') {

	  if (alofono_acento == 0) {
	    recorre_acentuales->indice_alofono_tonico = posicion_alofono_tonico;
	    recorre_acentuales->duracion_hasta_acento =
	      duracion_grupo_acentual + duracion1 / 2;
	  }
	  else
	    posicion_alofono_tonico++;


	  duracion_grupo_acentual += duracion1;

	  if (posicion_vocal_fuerte == 0) {
	    recorre_acentuales->silabas[indice_silaba].vocal_fuerte = posicion_en_silaba;
	    recorre_acentuales->silabas[indice_silaba].tiempo_hasta_nucleo =
	      duracion_silaba + duracion1 / 2;
	    if (devuelve_tipo_estructura_silaba(silaba, almacena_posicion_vocal_fuerte, &tipo_silaba))
	      return 1;
	    recorre_acentuales->silabas[indice_silaba].tipo_silaba = tipo_silaba;
	    tipo_silaba_anterior = tipo_silaba;
	  } // if (posicion_vocal_fuerte == 0)

	  posicion_vocal_fuerte -= strlen(correcaminos->alof);

	  duracion_silaba += duracion1;
	  numero_alofonos_grupo++;
	}

	duracion1 /= 2;
	duracion2 = duracion1;

	if (correcaminos->dur2 == 0) { // Si el fonema sólo tiene una parte.

	  if ( (correcaminos->pitch1 > 0) && (correcaminos->pitch2 > 0) ) {
	    media_pitch = (correcaminos->pitch1 + correcaminos->pitch2)/2.0;
	    correcaminos->pitch3 = correcaminos->pitch2;
	    correcaminos->pitch2 = (int) media_pitch;
	  }
	  else
	    media_pitch = 0;
	} // if (correcaminos->dur2 ==...
	else
	  media_pitch = correcaminos->pitch2;

	nuevo_vector.anhade_frecuencia(esc_pitch*media_pitch, esc_pitch*correcaminos->pitch1, esc_pitch*correcaminos->pitch3);

	nuevo_vector.anhade_duracion(duracion1, duracion2);

	strcpy(nuevo_vector.silaba_hts, silaba); // HTS
    nuevo_vector.palsep = palabra_separada; 
   

	vectores.push_back(nuevo_vector);
	++*numero_unidades;

#ifdef _elimina_sil_sil
      }
#endif


      correcaminos++;

    } // while (correcaminos->alof...

    if (error_ejecucion) {
      int contador;

      for (vector<Vector_descriptor_objetivo>::iterator it = vectores.begin();
	   it != vectores.end(); it++)
	it->libera_memoria();
      vectores.clear();

      Grupo_acentual_objetivo *borra_acentuales = *acentuales;
        
      if (*acentuales) {
	for (contador = 0; contador < *numero_acentuales; contador++, borra_acentuales++)
	  borra_acentuales->libera_memoria();
	free(*acentuales);
	*acentuales = NULL;
      }
#ifdef _MINOR_PHRASING
      frase_frontera->elementos.clear();
#endif
    }
    else {

      if (unidad_no_silencio == 0) { // Si la frase son sólo pausas.

	if ( (recorre_acentuales->silabas = (estructura_silaba *) malloc(
									 sizeof(estructura_silaba)))
	     == NULL) {
	  fprintf(stderr, "Error en crea_descriptores, al asignar memoria.\n");
	  return 1;
	}

	recorre_acentuales->duracion = 1.0; // Por poner algo
	recorre_acentuales->numero_alofonos = numero_alofonos_grupo;
	recorre_acentuales->silabas[indice_silaba].tiempo_silaba = 1.0;
	recorre_acentuales->tipo_proposicion = 0;
	recorre_acentuales->posicion_grupo_acentual = 0;
	recorre_acentuales->grupos_acentuales_grupo_prosodico = 1;
	recorre_acentuales->posicion_silaba_acentuada = 1;
	recorre_acentuales->numero_frase = numero_frase;
      } // if (unidad_no_silencio == 0)
      else {
	recorre_acentuales->duracion = duracion_grupo_acentual; // en segundos
	recorre_acentuales->numero_alofonos = numero_alofonos_grupo;
	recorre_acentuales->silabas[indice_silaba].tiempo_silaba = duracion_silaba;
      } // else

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
      recorre_acentuales->etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(palabra_separada);

      recorre_acentuales->sintagma_actual = decide_sintagma_grupo_acentual(palabra_separada->tipo_sintagma);
      recorre_acentuales->numero_elementos_sintagma = (unsigned char) cuenta_elementos_tonicos_sintagma(frase_sintagmada, frase_separada, palabra_separada);

      // Cambio
      devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, palabra_separada - frase_separada,
						&tipo_siguiente_sintagma);

      recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);
      //

      /*
        if (recorre_frase_sintagmada->fin == palabra_separada - frase_separada) {

	devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, palabra_separada - frase_separada,
	&tipo_siguiente_sintagma);

	recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);
        }
        else {
	if (recorre_acentuales->sintagma_actual == GA_S_PREPOSICIONAL)
	recorre_acentuales->siguiente_sintagma = GA_S_NOMINAL;
	else
	recorre_acentuales->siguiente_sintagma = recorre_acentuales->sintagma_actual;
        }
      */

#endif

      recorre_acentuales->tipo_pausa_anterior = tipo_de_pausa_anterior;

      if (frase_extraida[strlen(frase_extraida) - 1] == ',')
	// Para aquellas frases muy largas que se parten por una coma. No tengo claro que vaya a funcionar
	// bien, ya que al entrar aquí se considera una frase nueva.
	recorre_acentuales->tipo_pausa = PAUSA_COMA;
      else
	if (strstr(frase_extraida, "·") == NULL)
	  recorre_acentuales->tipo_pausa = PAUSA_PUNTO;
	else
	  recorre_acentuales->tipo_pausa = PAUSA_PUNTOS_SUSPENSIVOS;

      // Modificamos la duración del último silencio. Probablemente, cuando el modelo de duraciones
      // esté bien, no será necesario.

      vectores[*numero_unidades - 1].anhade_duracion(duracion1*0.05,
						     duracion2*0.95);

#ifdef _MINOR_PHRASING_TEMPLATE

      pos_frontera.etiqueta_morfosintactica = recorre_acentuales->etiqueta_morfosintactica;
      pos_frontera.tipo_pausa = recorre_acentuales->tipo_pausa;
      //			pos_frontera.salto_f0 = recorre_acentuales->salto_f0;
      //            pos_frontera.factor_caida = recorre_acentuales->factor_caida;

      sintagma_frontera.elementos.push_back(pos_frontera);
      sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
      sintagma_frontera.acento = 1; // sintagma tónico

      if ( (recorre_acentuales->tipo_pausa != SIN_PAUSA) &&
	   (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) &&
	   (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_COMA) ) {

	if (sintagma_frontera.elementos.size()) {
	  //					sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	  grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	  sintagma_frontera.elementos.clear();
	}

	grupo_fonico_frontera.tipo_grupo = recorre_acentuales->tipo_proposicion;
	grupo_fonico_frontera.siguiente_sintagma = recorre_acentuales->siguiente_sintagma;
	//                grupo_fonico_frontera.identificador = identificador_GF++;

	if (frase_frontera->elementos.size() == 0)
	  grupo_fonico_frontera.posicion = GRUPO_FONICO_INICIAL_Y_FINAL;
	else
	  grupo_fonico_frontera.posicion = GRUPO_FONICO_FINAL;

	frase_frontera->elementos.push_back(grupo_fonico_frontera);
	grupo_fonico_frontera.elementos.clear();
      }

#endif

    }

    if (frase) {
      free(frase);
      frase = NULL;
    }

    numero_frase++;

    return error_ejecucion;

  }


#endif


#ifdef _ESTADISTICA

  /**
   * Función:   calcula_estadística_1_0                                            * \remarks Entrada:
   *			- frase_prosodica: cadena en la que se encuentra la información de cada fonema
   *            por separado.
   *	\remarks Salida:
   *          - fonema_est: objeto en el que se almacena la información de las ocurrencias de fonemas.
   *	      - semifonema_izquierdo_est: objeto en el que se almacena la información de las ocurrencias de semifonemas con contexto por la izquierda.
   *          - semifonema_derecho_est: análogo al anterior, pero con el contexto por la derecha.
   *          - trifonema_est: igual, pero con información de trifonemas.
   * \remarks Valor devuelto:
   *          - En ausencia de error, devuelve un 0.
   * \remarks Objetivo:  Calcula las estadísticas de ocurrencias de semifonemas y trifonemas, que serán útiles posteriormente a la hora de diseñar un corpus.
   * \remarks NOTA:      Esta versión no incluye la información de contexto, tonicidad y tipo de proposición.
   */

  int Crea_vectores_descriptor::calcula_estadistica_1_0(t_prosodia *frase_prosodica, Estadistica *fonema_est, Estadistica *semifonema_izquierdo_est, Estadistica *semifonema_derecho_est, Estadistica *trifonema_est) {


    t_prosodia *correcaminos = frase_prosodica;
    char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#";

    char trifonema[TAMANO_UNIDAD];
    char sem_izdo[TAMANO_UNIDAD];
    char sem_dcho[TAMANO_UNIDAD];

    while (correcaminos->alof[0] != '\0') {

      if ((correcaminos + 1)->alof[0])
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");

#ifdef _elimina_sil_sil

      if ( (cont_derecha[0] !=  '#') || (correcaminos->alof[0] != '#') ||
	   (cont_izquierda[0] != '#') ) {

#endif
	strcpy(sem_izdo, cont_izquierda);
	strcat(sem_izdo, ".");
	strcat(sem_izdo, correcaminos->alof);
	strcpy(sem_dcho, correcaminos->alof);
	strcat(sem_dcho, ".");
	strcat(sem_dcho, cont_derecha);
	strcpy(trifonema, cont_izquierda);
	strcat(trifonema, ".");
	strcat(trifonema, correcaminos->alof);
	strcat(trifonema, ".");
	strcat(trifonema, cont_derecha);

	if (fonema_est->anhade_dato(correcaminos->alof) ||
	    semifonema_izquierdo_est->anhade_dato(sem_izdo) ||
	    semifonema_derecho_est->anhade_dato(sem_dcho)   ||
	    trifonema_est->anhade_dato(trifonema) )
	  return 1;
                      

#ifdef _elimina_sil_sil

      }

#endif
      strcpy(cont_izquierda, correcaminos->alof);
      correcaminos++;
    }
    return 0;  
  }


  /**
   * Función:   calcula_estadistica_2_0
   * \remarks Entrada:
   *			- frase_separada: cadena que contiene la información de cada palabra procesada.
   *            Se emplea para obtener la información de palabra contexto y tipo de frase.
   *          - frase_prosodica: cadena en la que se encuentra la información de cada fonema
   *            por separado.                                                                  
   *          - frase_en_grupos: división en grupos de la frase de entrada.
   *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
   *            tonicidad y de pausas.                                                         
   *          - frase_sintagmada: división en sintagmas de la frase de entrada.
   *			- numero_de_elementos_de_frase_separada: número de elementos de la variable
   *			  frase_separada.
   *	\remarks Salida:
   *			- semifonema_izquierdo_est: objeto en el que se almacena la información de las
   *            ocurrencias de semifonemas con contexto por la izquierda.                      
   *          - semifonema_derecho_est: análogo al anterior, pero con el contexto por la
   *            derecha.                                                                       
   *          - trifonema_est: igual, pero con información de trifonemas.
   * \remarks Valor devuelto:                                                                           
   *          - En ausencia de error devuelve un 0.                                            
   * \remarks Objetivo:  Calcula el número de ocurrencias de semifonemas con contexto por la derecha,
   *            por la izquierda, y trifonemas.                                                
   * \remarks NOTA:      En esta versión ya se incluye la información de tonicidad, posición en la
   *            frase y tipo de proposición.
   */

  int Crea_vectores_descriptor::calcula_estadistica_2_0(t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
							char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
							t_frase_sintagmada *frase_sintagmada, int numero_de_elementos_de_frase_separada, 
							Estadistica *semifonema_izquierdo_est,
							Estadistica *semifonema_derecho_est, Estadistica *trifonema_est) {

    // Cadenas empleadas para el identificador de cada unidad.

    char trifonema[TAMANO_UNIDAD];
    char sem_izdo[TAMANO_UNIDAD];
    char sem_dcho[TAMANO_UNIDAD];

    // Punteros a las cadenas de entrada.
    t_frase_separada *palabra_separada = frase_separada;
    char *f_fonetica = frase_fonetica;
    t_prosodia *correcaminos = frase_prosodica;
    t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;

    t_palabra_proc palabra_contexto; // Palabra a la que pertenece la unidad considerada.
    char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#"; // Fonemas por la derecha y por la izquierda.

    int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
    int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.
    Vector_descriptor_objetivo nuevo_vector; //Objeto en el que se almacena la información relativa a la unidad.

    // Variables para determinar el tipo de frase a la que pertenece el fonema actual.
    unsigned char tipo_de_tonema; // Consideramos tres: ENUNCIATIVA, EXCLAMATIVA e INTERROGATIVA.
    unsigned char final_tonema_frase_separada; // Índice de frase_separada en el que se acaba el tonema actual.

    char primera = 1; // Variable que controla la ejecución de la primera vuelta del bucle.

    // Variables que controlan cuándo hay que acceder a las palabras
    int numero_palabra = 0; // // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
    // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
    char ciclo_transformada = 0;
    char no_final_ciclo = 1;

    t_palabra_proc parte_transformada, parte; // Variables en las que se almacenan la palabra transformada completa y
    // la palabra dentro de ella a la que nos referimos.


    unsigned char posicion_frase; // Posición del fonema dentro de la frase  (inicial, media o final).
    unsigned char tonicidad; // Tonicidad del fonema, considerada como un rasgo de la sílaba. (0 -> átona).

    // Comprobamos que no se trate de una frase con sólo signos de puntuación:

    if (frase_solo_signos_de_puntuacion(frase_separada))
      return 0;

    // Calculamos el tipo del primer tonema y hasta dónde llega.

    final_tonema_frase_separada = (unsigned char)
      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
    tipo_de_tonema = (unsigned char)
      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);


    // Inicializamos las variables necesarias para el cálculo de la posición en la frase
    // y la tonicidad.

    inicializa_calculo_posicion_y_tonicidad();

    // Entramos en el bucle principal.

    while (correcaminos->alof[0] != '\0') {


      // Calculamos la posición del fonema y su tonicidad.

      if (calcula_posicion_frase_y_tonicidad(f_fonetica, correcaminos->alof, &posicion_frase,
					     &tonicidad))
	return 1;

      if (correcaminos->alof[0] == '#') {
	if (primera) {
	  // Se trata del silencio inicial del fichero.
	  primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Aquí le cambié el >= por >.
	    if (++elementos_de_frase_separada > numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_estadistica_2_0: datos de entrada incorrectos.\n");
	      return 1;
	    }
	    palabra_separada++;
	    if (palabra_separada->clase_pal == SIN_CLASIFICAR) {
	      if (palabra_solo_signos_puntuacion(palabra_separada->pal))
		palabra_separada->clase_pal = SIGNO_PUNTUACION;
	    }
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }

	  if (palabra_separada->pal_transformada) {
	    // Entramos en un ciclo de palabra_transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	      return 1;
	    strcpy(palabra_contexto, parte);
	  }
	  else
	    // No hay palabra transformada -> el contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  anterior_orden++;
	}
      }

      // Comprobamos si estamos en un ciclo de palabra transformada.
      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Avanzamos a la siguiente palabra contenida en palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    // El ciclo actual no ha terminado. Actualizamos el índice a la palabra a la
	    // que pertenece el alófono actual.
	    anterior_orden = correcaminos->pp;
	  else
	    // Ya hemos acabado con el ciclo de la última palabra transformada.
	    ciclo_transformada = 0;
	}
	if (no_final_ciclo) {
	  // Todavía no ha terminado el ciclo de palabra transformada.
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (unsigned char) 
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	    return 1;
	  strcpy(palabra_contexto, parte);
	}
      }

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  // Actualizamos las varibles para el siguiente ciclo de palabra transformada.
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;

	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    // Avanzamos palabra_separada hasta llegar a una palabra.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en calcula_estadistica_2_0: datos de entrada incorrectos.\n");
	      return 1;
	    }
	    palabra_separada++;
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }

	  // Actualizamos el índice a la palabra actual.
	  anterior_orden = correcaminos->pp;

	}
	else {
	  fprintf(stderr, "Error en calcula_estadistica_2_0: datos de entrada incorrectos.\n");
	  return 1;
	}
	if (palabra_separada->pal_transformada) {
	  // Comenzamos un ciclo de palabra transformada.
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	      return 1;
	    strcpy(palabra_contexto, parte);
	  }
	}
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      }

      // Actualizamos el contexto por la derecha.

      if ( (correcaminos + 1)->alof[0] )
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");

#ifdef _elimina_sil_sil

      if ( (cont_derecha[0] != '#') || (cont_izquierda[0] != '#') ||
	   (correcaminos->alof[0] != '#') ) {

#endif
	// Actualizamos los objetos de estadísticas.
	strcpy(sem_izdo, cont_izquierda);
	strcat(sem_izdo, ".");
	strcat(sem_izdo, correcaminos->alof);
	strcpy(sem_dcho, correcaminos->alof);
	strcat(sem_dcho, ".");
	strcat(sem_dcho, cont_derecha);
	strcpy(trifonema, cont_izquierda);
	strcat(trifonema, ".");
	strcat(trifonema, correcaminos->alof);
	strcat(trifonema, ".");
	strcat(trifonema, cont_derecha);

	if (semifonema_izquierdo_est->anhade_dato(sem_izdo, tonicidad, posicion_frase,
						  tipo_de_tonema) ||
	    semifonema_derecho_est->anhade_dato(sem_dcho, tonicidad, posicion_frase,
						tipo_de_tonema) ||
	    trifonema_est->anhade_dato(trifonema, tonicidad, posicion_frase,
				       tipo_de_tonema) )

	  return 1;

#ifdef _elimina_sil_sil
      }
#endif

      // Actualizamos el contexto por la izquierda.

      strcpy(cont_izquierda, correcaminos->alof);
      correcaminos++;

    }

    return 0;
      
  }


#endif


#ifdef _GENERA_CORPUS

  /**
   * Función:   genera_corpus
   * \remarks Entrada:
   *			- frase_extraida: frase original.                                                
   *          - frase_separada: cadena que contiene la información de cada palabra procesada.
   *            Se emplea para obtener la información de palabra contexto y tipo de frase.
   *          - frase_prosodica: cadena en la que se encuentra la información de cada fonema
   *            por separado.                                                                  
   *          - frase_en_grupos: división en grupos de la frase de entrada.
   *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
   *            tonicidad y de pausas.                                                         
   *          - frase_sintagmada: división en sintagmas de la frase de entrada.
   *	\remarks Salida:
   *			- semifonema_izquierdo_est: objeto en el que se almacena la información de las
   *            ocurrencias de semifonemas con contexto por la izquierda.                      
   *          - semifonema_derecho_est: análogo al anterior, pero con el contexto por la
   *            derecha.                                                                       
   *          - trifonema_est: igual, pero con información de trifonemas.
   *          - fichero_corpus: fichero en el que se van añadiendo las frases seleccionadas.
   *          - fichero_rechazo: fichero en el que se incluyen aquellas frases que no tenían
   *            ninguna unidad entre las buscadas.                                             
   * \remarks Entrada y Salida:                                                                         
   *          - lista_unidades: objeto Estadistica en el que se recogen aquellas unidades que  
   *            se desea encontrar.                                                            
   * \remarks Valor devuelto:                                                                           
   *          - En ausencia de error devuelve un 0.                                            
   * \remarks Objetivo:  Realiza las mismas funciones que calcula_estadistica_2_0, pero además
   *            comprueba que en la frase de entrada haya alguna de las unidades que se le     
   *            en la cadena de entrada lista_unidades. Si sí que las hay, añade al fichero
   *            fichero_corpus la frase, y actualiza el número de ocurrencias, y si no, la
   *            añade al fichero fichero_rechazo, ya deja las estadísticas como estaban.
   */

  int Crea_vectores_descriptor::genera_corpus(char *frase_extraida, t_frase_separada *frase_separada, t_prosodia *frase_prosodica,
					      char *frase_fonetica, t_frase_en_grupos *frase_en_grupos,
					      t_frase_sintagmada *frase_sintagmada, int numero_de_elementos_de_frase_separada,
					      Estadistica *semifonema_izquierdo_est,
					      Estadistica *semifonema_derecho_est, Estadistica *trifonema_est,
					      Estadistica *lista_unidades, FILE *fichero_corpus, FILE *fichero_rechazo) {

    // Cadenas empleadas para el identificador de cada unidad.

    char trifonema[TAMANO_UNIDAD];
    char sem_izdo[TAMANO_UNIDAD];
    char sem_dcho[TAMANO_UNIDAD];

    // Punteros a las cadenas de entrada.
    t_frase_separada *palabra_separada = frase_separada;
    char *f_fonetica = frase_fonetica;
    t_prosodia *correcaminos = frase_prosodica;
    t_frase_en_grupos *recorre_frase_en_grupos = frase_en_grupos;

    t_palabra_proc palabra_contexto; // Palabra a la que pertenece la unidad considerada.
    char cont_derecha[OCTETOS_POR_FONEMA + 1] = "F", cont_izquierda[OCTETOS_POR_FONEMA + 1] = "#"; // Fonemas por la derecha y  por la izquierda.

    int anterior_orden = correcaminos->pp; // Posición de la palabra dentro de la frase.
    int elementos_de_frase_separada = 0; // Índice de frase_separada en el que nos encontramos.
    Vector_descriptor_objetivo nuevo_vector; //Objeto en el que se almacena la información relativa a la unidad.

    // Variables para determinar el tipo de frase a la que pertenece el fonema actual.
    unsigned char tipo_de_tonema; // Consideramos tres: ENUNCIATIVA, EXCLAMATIVA e INTERROGATIVA.
    unsigned char final_tonema_frase_separada; // Índice de frase_separada en el que se acaba el tonema actual.

    char primera = 1; // Variable que controla la ejecución de la primera vuelta del bucle.

    // Variables que controlan cuándo hay que acceder a las palabras
    int numero_palabra = 0; // // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
    // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
    char ciclo_transformada = 0;
    char no_final_ciclo = 1;

    t_palabra_proc parte_transformada, parte; // Variables en las que se almacenan la palabra transformada completa y
    // la palabra dentro de ella a la que nos referimos.


    unsigned char posicion_frase; // Posición del fonema dentro de la frase  (inicial, media o final).
    unsigned char tonicidad; // Tonicidad del fonema, considerada como un rasgo de la sílaba. (0 -> átona).

    // Objetos de tipo estadistica, para la información de la frase actual.

    Estadistica nuevo_derecho, nuevo_izquierdo, nuevo_trifonema;

    // Comprobamos que no se trate de una frase con sólo signos de puntuación:

    if (frase_solo_signos_de_puntuacion(frase_separada))
      return 0;

    // Calculamos el tipo del primer tonema y hasta dónde llega.

    final_tonema_frase_separada = (unsigned char) (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
    tipo_de_tonema = (unsigned char) halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);


    // Inicializamos las variables necesarias para el cálculo de la posición en la frase
    // y la tonicidad.

    inicializa_calculo_posicion_y_tonicidad();

    // Entramos en el bucle principal.

    while (correcaminos->alof[0] != '\0') {


      // Calculamos la posición del fonema y su tonicidad.

      if (calcula_posicion_frase_y_tonicidad(f_fonetica, correcaminos->alof, &posicion_frase,
					     &tonicidad))
	return 1;

      if (correcaminos->alof[0] == '#') {
	if (primera) {
	  // Se trata del silencio inicial del fichero.
	  primera = 0;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) { // Aquí le cambié el >= por >.
	    if (++elementos_de_frase_separada > numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en genera_corpus: datos de entrada incorrectos.\n");
	      return 1;
	    }
	    palabra_separada++;
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }

	  if (palabra_separada->pal_transformada) {
	    // Entramos en un ciclo de palabra_transformada.
	    ciclo_transformada = 1;
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	      return 1;
	    strcpy(palabra_contexto, parte);
	  }
	  else
	    // No hay palabra transformada -> el contexto es directamente pal.
	    strcpy(palabra_contexto, palabra_separada->pal);
	  anterior_orden++;
	}
      }

      // Comprobamos si estamos en un ciclo de palabra transformada.
      if (ciclo_transformada) {
	if (correcaminos->pp > anterior_orden) {
	  // Avanzamos a la siguiente palabra contenida en palabra_transformada.
	  numero_palabra++;
	  if (no_final_ciclo)
	    // El ciclo actual no ha terminado. Actualizamos el índice a la palabra a la
	    // que pertenece el alófono actual.
	    anterior_orden = correcaminos->pp;
	  else
	    // Ya hemos acabado con el ciclo de la última palabra transformada.
	    ciclo_transformada = 0;
	}
	if (no_final_ciclo) {
	  // Todavía no ha terminado el ciclo de palabra transformada.
	  strcpy(parte_transformada, palabra_separada->pal_transformada);
	  if ( (no_final_ciclo = (unsigned char) 
		inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	    return 1;
	  strcpy(palabra_contexto, parte);
	}
      }

      if (correcaminos->pp > anterior_orden) { // cambiamos de palabra
	if (elementos_de_frase_separada++ < numero_de_elementos_de_frase_separada) {
	  // Actualizamos las varibles para el siguiente ciclo de palabra transformada.
	  no_final_ciclo = 1;
	  numero_palabra =0;
	  palabra_separada++;
	  while ( (palabra_separada->clase_pal == SIGNO_PUNTUACION) ) { // ||
	    //                        (palabra_separada->clase_pal == SIN_CLASIFICAR) ) {
	    // Avanzamos palabra_separada hasta llegar a una palabra.
	    if (++elementos_de_frase_separada >= numero_de_elementos_de_frase_separada) {
	      fprintf(stderr, "Error en genera_corpus: datos de entrada incorrectos.\n");
	      return 1;
	    }
	    palabra_separada++;
	  }
	  if (elementos_de_frase_separada >= final_tonema_frase_separada) {
	    // Cambiamos de tonema.
	    recorre_frase_en_grupos++;
	    final_tonema_frase_separada = (unsigned char)
	      (frase_sintagmada + recorre_frase_en_grupos->fin)->fin;
	    tipo_de_tonema = (unsigned char)
	      halla_tipo_de_frase(recorre_frase_en_grupos->tipo_tonema);
	  }

	  // Actualizamos el índice a la palabra actual.
	  anterior_orden = correcaminos->pp;

	}
	else {
	  fprintf(stderr, "Error en genera_corpus: datos de entrada incorrectos.\n");
	  return 1;
	}
	if (palabra_separada->pal_transformada) {
	  // Comenzamos un ciclo de palabra transformada.
	  ciclo_transformada = 1;
	  if (no_final_ciclo) {
	    strcpy(parte_transformada, palabra_separada->pal_transformada);
	    if ( (no_final_ciclo = (unsigned char)
		  inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0)
	      return 1;
	    strcpy(palabra_contexto, parte);
	  }
	}
	else
	  strcpy(palabra_contexto, palabra_separada->pal);

      }

      // Actualizamos el contexto por la derecha.

      if ( (correcaminos + 1)->alof[0] )
	strcpy(cont_derecha, (correcaminos + 1)->alof);
      else
	strcpy(cont_derecha, "#");

#ifdef _elimina_sil_sil

      if ( (cont_derecha[0] != '#') || (cont_izquierda[0] != '#') ||
	   (correcaminos->alof[0] != '#') ) {

#endif
	// Actualizamos los objetos de estadísticas.
	strcpy(sem_izdo, cont_izquierda);
	strcat(sem_izdo, ".");
	strcat(sem_izdo, correcaminos->alof);
	strcpy(sem_dcho, correcaminos->alof);
	strcat(sem_dcho, ".");
	strcat(sem_dcho, cont_derecha);
	strcpy(trifonema, cont_izquierda);
	strcat(trifonema, ".");
	strcat(trifonema, correcaminos->alof);
	strcat(trifonema, ".");
	strcat(trifonema, cont_derecha);

	if (nuevo_izquierdo.anhade_dato(sem_izdo, tonicidad, posicion_frase,
					tipo_de_tonema) ||
	    nuevo_derecho.anhade_dato(sem_dcho, tonicidad, posicion_frase,
				      tipo_de_tonema) ||
	    nuevo_trifonema.anhade_dato(trifonema, tonicidad, posicion_frase,
					tipo_de_tonema) )

	  return 1;

#ifdef _elimina_sil_sil
      }
#endif

      // Actualizamos el contexto por la izquierda.

      strcpy(cont_izquierda, correcaminos->alof);
      correcaminos++;

    }

    // Comprobamos si hay alguna de las unidades buscadas en la frase.

    unidad_extendida *lista = NULL;
    int tamano_lista_extendida = 0;


         
    if (nuevo_derecho.existe_lista(lista_unidades, &lista, &tamano_lista_extendida))
      return 1;

    if (tamano_lista_extendida) { // En este caso sí que hay alguna de las unidades buscadas.

      // Añadimos las estadísticas.

      if (semifonema_izquierdo_est->suma_estadisticas(nuevo_izquierdo) ||
	  semifonema_derecho_est->suma_estadisticas(nuevo_derecho)     ||
	  trifonema_est->suma_estadisticas(nuevo_trifonema) )
	return 1;

      // Copiamos la frase en el fichero.
      fprintf(fichero_corpus, "Unidades encontradas:\n");
      unidad_extendida *recorre_lista = lista;
      for (int contador = 0; contador < tamano_lista_extendida; contador++) {
	fprintf(fichero_corpus, "%s %d %d %d.\n", recorre_lista->unidad, recorre_lista->acento,
		recorre_lista->proposicion, recorre_lista->contexto);
	recorre_lista++;
      }
      fprintf(fichero_corpus, "%s\n", frase_extraida);

      // Liberamos la memoria de lista:

      free(lista);

    }
    else {
      // La frase no es válida. La escribimos en el fichero de frases rechazadas.
      if (frase_extraida[strlen(frase_extraida) - 1] == '·')
	strcpy(&frase_extraida[strlen(frase_extraida) - 2], "...");
      fprintf(fichero_rechazo, "%s\n", frase_extraida);

    }

    // Liberamos la memoria reservada.

    nuevo_derecho.libera_memoria();
    nuevo_izquierdo.libera_memoria();
    nuevo_trifonema.libera_memoria();
      
    return 0;

  }

#endif


  /**
   * Función:   inicia_ciclo_transformada
   * \remarks Entrada:
   *			- palabra_transformada: palabra extendida, tal y como nos la da Cotovia. Por   
   *            ejemplo, si Cotovia lee 225, aquí vendría "douscentos vintecinco".
   *          - numero_palabra: posición de la palabra que queremos extraer de la variable
   *            anterior.                                                                    
   *	\remarks Salida:
   *			- salida: palabra contenida en palabra_transformada, en la posición indicada
   *            por numero_palabra.                                                          
   * \remarks Valor devuelto:                                                                         
   *          - Si es la última palabra contenida en pal_transformada, se devuelve un 0. En
   *            caso contrario, se devuelve un 1. Si se produce algún error, devuelve -1.
   * \remarks Objetivo:  Dado que cada elemento de frase_separada se puede desdoblar en una o más
   *            palabras (como en el ejemplo de arriba), en ocasiones hay que sacar la       
   *            palabra contexto a la que pertenece un fonema del campo palabra_transformada.
   */

  int inicia_ciclo_transformada(char *palabra_transformada, char *salida, int numero_palabra) {

    int longitud, final = 0;
    char *auxiliar, *anterior = NULL;

    if (palabra_transformada[0] == '\0') {
      fprintf(stderr, "Error en inicia_ciclo_transformada: palabra nula.\n");
      return -1;
    }

    // Eliminamos posibles espacios al principio y al final de la palabra (Cosas de Cotovia).

    while (palabra_transformada[0] == ' ')
      palabra_transformada++;

    longitud = strlen(palabra_transformada);

    while (palabra_transformada[longitud - 1] == ' ') {
      longitud--;
      palabra_transformada[longitud] = '\0';
    }

    auxiliar = palabra_transformada;

    // Si numero_palabra == 0 (lo más común), llega con copiar la primera palabra contenida en
    // pal_transformada.

    if (numero_palabra <= 0) {
      longitud = strcspn(palabra_transformada, " ");
      strncpy(salida, palabra_transformada, longitud);
      salida[longitud] = '\0';
      // Con este return le indicamos si hay más palabras contenidas en pal_transformada.
      if (strcmp(salida, palabra_transformada))
	return 1;
      return 0;
    }

    // Avanzamos hasta la palabra contenida en pal_transformada que nos interesa.

    for (int contador = 0; contador <= numero_palabra; contador++) {
      if ( (auxiliar = strstr(auxiliar, " ") ) == NULL) {
	final = 1;
	break;
      }
      longitud = auxiliar - palabra_transformada;
      strncpy(salida, palabra_transformada, longitud);
      salida[longitud] = '\0';
      palabra_transformada += longitud + 1;
      while ( (*auxiliar == ' ') && (*auxiliar != '\0') )
	auxiliar++;
      anterior = auxiliar;
      // El problema está aquí: hay varios espacios entre 400 y mil....
    }

    // Si es la última palabra, lo indicamos devolviendo un 0.

    if (final) {
      strcpy(salida, anterior);
      return 0;
    }

    return 1;
    
  }

  /**
   * Función:   halla_tipo_de_frase
   * \remarks Entrada:
   *			- codigo: tipo de tonema al que pertenece la palabra, según lo
   *            especificado en el archivo tip_var.h. (en "Defines de  categorias   
   *            de como son as pausas e as entonacions).                            
   * \remarks Valor devuelto:                                                                
   *          - Devuelve uno de los cuatro tipos de frases considerados hasta el    
   *            momento: FRASE_ENUNCIATIVA, FRASE_EXCLAMATIVA, FRASE_INTERROGATIVA  
   *            y FRASE_INACABADA.                                                  
   * \remarks Objetivo:  Según el código de entrada, se determina el tipo de frase.
   */

  unsigned char halla_tipo_de_frase(unsigned char codigo) {

    if ( ((codigo >= INICIO_INTERROGACION_TOTAL) && (codigo <= INTERROGACION_TOTAL)) ||
	 ((codigo >= INICIO_INTERROGACION_PARCIAL) && (codigo <= INTERROGACION_PARCIAL)) )
      return FRASE_INTERROGATIVA;

    if ( (codigo >= INICIO_EXCLAMACION) && (codigo <= EXCLAMACION) )
      return FRASE_EXCLAMATIVA;

    if (codigo == DE_SUSPENSE)
      return FRASE_INACABADA;

    return FRASE_ENUNCIATIVA;
  }


  /**
   * Función:   palabra_solo_signos_puntuacion
   * \remarks Entrada:
   *			- palabra: palabra de la que se quiere comprobar si sólo está
   *            formada por signos de puntuación.
   * \remarks Valor devuelto:                                                            
   *          - Si sólo está compuesta por signos de puntuación, devuelve un 1.
   * \remarks NOTA:
   *			- Esta función no sería necesaria si Cotovía hiciese un análisis
   *            bueno de la categoría de las palabras. En otras palabras, es
   *            una chapuza.                                                    
   */

  int palabra_solo_signos_puntuacion(char *palabra) {

    int tamano = strlen(palabra);
    char *puntero = palabra;

    for (int contador = 0; contador < tamano; contador++, puntero++)
      switch (*puntero) {
      case '\\':
      case '.':
      case ':':
      case ';':
      case '/':
      case '-':
      case '_':
	continue;
      default:
	return 0;
      }

    return 1;

  }

#ifndef _GRUPOS_ACENTUALES

  /**
   * Función:   calcula_posicion_frase_y_tonicidad
   * \remarks Entrada:
   *			- f_fonetica: cadena con la información de tonicidad, sílabas, y
   *            tokens de pausas y proposiciones.                                    
   *          - alof: alófono del que queremos hallar su posición en la frase y
   *            tonicidad.                                                           
   *	\remarks Salida:
   *			- posicion: posición del alófono en la frase. Si está antes de la
   *            primera sílaba tónica (incluida), se considera POSICION_INICIAL.
   *            Si está después de la última (incluida), se considera POSICION_FINAL.
   *            En otro caso, se considera POSICION_MEDIA.                           
   *          - tonica: tonicidad del alófono, considerada ésta como algo propio de
   *            la sílaba, y no de un único fonema de ella. (Tónica -> 1).
   * \remarks Valor devuelto:                                                                 
   *          - En ausencia de error, devuelve un 0.                                 
   * \remarks Objetivo:  Calcular las características de tonicidad y posición en la frase de
   *            cada alófono.
   */

  int Crea_vectores_descriptor::calcula_posicion_frase_y_tonicidad(char *f_fonetica, char alof[],
								   unsigned char *posicion, unsigned char *tonica) {


    char palabra[LONX_MAX_PAL_SIL_E_ACENTUADA];
    static char *correcaminos = NULL;

    if (alof[0] == '#') {
      *tonica = 0;
      *posicion = POSICION_INICIAL; // Por poner algo.
      return 0;
    }

    if (numero_elementos < -1) {
      getchar();
      puts("Hola");
    }

    if (numero_elementos)
      numero_elementos -= strlen(alof);
    else
      if (--fragmentos_entre_pausas) {
	// Reiniciamos las variables necesarias para el cálculo de la posición en la frase
	// y la tonicidad silábica.
	reinicia_variables_posicion_y_tonicidad();
	if ( (frase = extrae_frase(f_fonetica)) == NULL)
	  return 1;
	else {
	  numero_elementos -= strlen(alof);
	  correcaminos = frase;
	}
      }

    if (correcaminos == NULL) // La frase de entrada sólo estaba constituida por tokens,
      return 0;          // o bien ya hemos llegado a la última sílaba y los valores de tónica y posicion son constantes.

    if (tamano_silaba) { // Continuamos en la misma sílaba.
      //      if (alof[0] != '#')
      tamano_silaba -= (unsigned char) strlen(alof);
    }
    else { // cambiamos de sílaba.
      cuenta_silabas++;
      while (correcaminos[0] == ' ')
	correcaminos++; // Nos ponemos al inicio de la palabra.

      tamano_silaba = (unsigned char) strcspn(correcaminos, " -"); // El límite de la sílaba viene dado por uno de estos delimitadores.
      strncpy(palabra, correcaminos, tamano_silaba);
      palabra[tamano_silaba] = '\0';

      correcaminos += tamano_silaba + 1;

      if (strstr(palabra, "^")) { // Si en la sílaba está el circunflejo, es tónica.
	tonicidad = 1;
	//           if (alof[0] != '#')
	tamano_silaba -= (unsigned char) (1 + strlen(alof));
	//             else
	//                tamano_silaba -= 1;
      }
      else {
	tonicidad = 0;
	//             if (alof[0] != '#')
	tamano_silaba -=  (unsigned char) strlen(alof);
      }
    }

    *tonica = tonicidad;

    // Ahora tenemos que comprobar la posición del alófono en la frase.

    if (cuenta_silabas <= primer_acento)
      *posicion = POSICION_INICIAL;
    else
      if (cuenta_silabas < segundo_acento)
	*posicion = POSICION_MEDIA;
      else
	*posicion = POSICION_FINAL;


    return 0;

  }


  /**
   * Función:   extrae_frase
   * \remarks Entrada:
   *			- frase_total: cadena con la frase completa.                         
   * \remarks Valor devuelto:                                                               
   *          - En ausencia de error devuelve el fragmento de cadena de posición
   *            fragmento_actual, eliminándole los tokens de pausa y proposición.
   * \remarks Objetivo:  Extraer las porciones de frase entre pausas, dejando sólo la infor-
   *            mación de tonicidad y silabificación.
   */

  char *Crea_vectores_descriptor::extrae_frase(char *frase_total) {

    char *inicio_token, *final_token, *origen_auxiliar, *auxiliar, *original = frase_total;
    char *salida;
    int elimina, tamano;
    int contador;
    tamano = strlen(frase_total) + 1;

    fragmento_actual++;

    // Asignamos espacio en memoria para la variable de salida.

    if ( (auxiliar = (char *) malloc(tamano)) == NULL) {
      fprintf(stderr, "Error en extrae_frase, al asignar memoria.\n");
      return NULL;
    }

    origen_auxiliar = auxiliar;

    // Avanzamos hasta el fragmento que nos ocupa.
    if (fragmento_actual) {
      for (contador = 0; contador < fragmento_actual; contador++)
	frase_total = strstr(frase_total + 2, "%pausa");
    }

    strcpy(auxiliar, frase_total);

    // Buscamos el primer token de pausa.

    if ( (inicio_token = strstr(auxiliar, "%pausa")) == NULL) {
      fprintf(stderr, "Error en extrae_frase: formato de frase fonética no válido.\n");
      return NULL;
    }

    auxiliar = strstr(inicio_token + 1, "#") + 2; // Eliminamos el # y el ' '.
    /*
      auxiliar = strstr(inicio_token + 1, "%");  // Esto se podría hacer en un paso si nos pudiésemos
      auxiliar = strstr(auxiliar, " ") + 1; // fiar de la transcripción de Cotovia.
    */ // Ahora ya debería funcionar.

    // Buscamos el segundo token.

    if ( (final_token = strstr(auxiliar + 1, "%pausa")) == NULL) {
      fprintf(stderr, "Error en extrae_frase: formato de frase fonética no válido.\n");
      return NULL;
    }


    inicio_token = final_token + 1;

    // Acotamos la cadena.
    tamano = final_token - auxiliar; //+ 1; // Así dejamos un espacio en blanco al final, que permite el
    // correcto funcionamiento al final de la frase, cuando hay 2 tokens
    // de pausa seguidos.

    if ( (salida = (char *) malloc(tamano)) == NULL) {
      fprintf(stderr, "Error en extrae_frase: error al asignar memoria.\n");
      return NULL;
    }

    salida = (char *) memmove(salida, auxiliar, tamano);

    salida[tamano - 1] = '\0';

    // Calculamos el número de grupos entre pausas que hay en la frase.

    if (!fragmento_actual) {
      fragmentos_entre_pausas = 1;

      while ( (inicio_token = strstr(inicio_token, "%pausa")) != NULL) {
	inicio_token++;
	if (inicio_token - final_token > 16) // Para evitar dos tokens seguidos.
	  fragmentos_entre_pausas++;
	final_token = inicio_token;
      }
    }


    // Ahora eliminamos los token del tipo %prop %

    while ( (inicio_token = strstr(salida, "%prop")) != NULL) {
      inicio_token++;
      if ( (final_token = strstr(inicio_token, "%")) == NULL) {
	fprintf(stderr, "Error en extrae_frase: formato de frase fonética incorrecto.\n");
	free(salida);
	return NULL;
      }
      elimina = final_token - inicio_token + 3; // Así eliminamos también el espacio en blanco.

      auxiliar = inicio_token - 1;
      // Aquí no es necesaria la asignación. Tengo que comprobarlo.
      //           auxiliar = (char *)
      memmove(auxiliar, auxiliar + elimina, tamano - elimina + salida - inicio_token);
      salida[tamano - elimina - 1] = '\0';
    }

    // Comprobamos que no se trate de una cadena formada sólo por tokens

    //     printf("Salida = %s.\n", salida);

    if ( (salida[0] == '\0') || ( (salida[1] == '\0') &&
				  ( (salida[0] == '#') || (salida[0] == ' '))  ) ) {
      fragmentos_entre_pausas--;
      free(origen_auxiliar);
      free(salida);
      return extrae_frase(original);
    }

    // Contamos el número de elementos significativos (es decir, todos menos ' ', '-' y '^').
    // Esto nos permitirá saber cuándo hemos procesado por completo un grupo de palabras entre pausas.

    inicio_token = salida;
    final_token = salida + strlen(salida);

    while (inicio_token < final_token)
      if ( (tamano = strcspn(inicio_token, "^ -")) != 0) {
	numero_elementos += tamano;
	inicio_token += tamano + 1;
      }
      else
	inicio_token++;

    if (numero_elementos < 0) {
      getchar();
      puts("Hola");
    }
    // Calculamos la posición silábica en la cadena de los límites de
    // posición en la frase.

    if ( (inicio_token = strstr(salida, "^")) == NULL)
      primer_acento = INT_MAX;
    else {
      // Contamos el número de sílabas hasta llegar a la tónica.

      primer_acento = -1;
      final_token = salida;
      while (final_token < inicio_token)
	if ( (tamano = strcspn(final_token, "- ")) == 0)
	  final_token++;
	else {
	  final_token +=tamano + 1;
	  primer_acento++;
	}
      // Ahora calculamos la posición de la última sílaba tónica.

      final_token = salida + strcspn(salida, "\0") - 1;

      while (final_token[0] != '^')
	final_token--;

      inicio_token = salida;
      segundo_acento = -1;

      while (inicio_token < final_token)
	if ( (tamano = strcspn(inicio_token, " -")) == 0)
	  inicio_token++;
	else {
	  inicio_token += tamano + 1;
	  segundo_acento++;
	}
    }

    free(origen_auxiliar);
    return salida;

  }

#endif


#if defined(_SCRIPT_DURACION) || defined(_MODO_NORMAL) || defined(_SCRIPT) || \
  defined(_CALCULA_DATOS_MODELO_ENERGIA) || defined(_PARA_ELISA) ||	\
  defined(_GRUPOS_ACENTUALES)

  /**
   * Función:   extrae_frase_y_posiciones
   * \remarks Entrada:
   *			- frase_total: cadena con la frase completa.                         
   * \remarks Valor devuelto:                                                               
   *          - En ausencia de error devuelve el fragmento de cadena de posición
   *            fragmento_actual, eliminándole los tokens de pausa y proposición.
   * \remarks Objetivo:  Extraer las porciones de frase entre pausas, dejando sólo la infor-
   *            mación de tonicidad y división en sílabas. Además, calcula el
   *            número de sílabas totales y tónicas del grupo.
   */

  char *Crea_vectores_descriptor::extrae_frase_y_posiciones(char *frase_total) {

    char *inicio_token, *final_token, *auxiliar;
    char *salida;
    int tamano;
    int contador;

    fragmento_actual++;

    // Avanzamos hasta el fragmento que nos ocupa.
    if (fragmento_actual) {
      for (contador = 0; contador < fragmento_actual; contador++) {
	frase_total = strstr(frase_total + 2, "%pausa");
	//			inicio_token = strstr(frase_total + 2, "%pausa");
	//           	final_token = strstr(frase_total + 2, "%ruptu");
	//            if (final_token == NULL)
	//            	frase_total = inicio_token;
	//            else
	//            	if (inicio_token == NULL)
	//                	frase_total = final_token;
	//                else
	//                	(inicio_token < final_token) ? frase_total = inicio_token:
	//                    							   frase_total = final_token;
      }
    }
    else {
      // Calculamos el número de grupos entre pausas que hay en la frase.

      fragmentos_entre_pausas = 0;

      inicio_token = frase_total + 2; // Para eliminar el primer token de pausa.

      while ( (inicio_token = strstr(inicio_token, "%pausa")) != NULL) {
	inicio_token++;
	fragmentos_entre_pausas++;
      }

      // Añadimos las rupturas entonativas:

      //        inicio_token = frase_total + 2;

      //        while ( (inicio_token = strstr(inicio_token, "%rupt")) != NULL) {
      //        	inicio_token++;
      //            fragmentos_entre_pausas++;
      //        }

    }

    // Buscamos el primer token de pausa.

    auxiliar = strstr(frase_total + 1, "#") + 2;

    // Buscamos el segundo token.

    final_token = strstr(auxiliar + 1, "%pausa");
    //    inicio_token = strstr(auxiliar + 1, "%rupt");

    //    if ( (final_token == NULL) && (inicio_token == NULL) ) {
    if (final_token == NULL) {
      fprintf(stderr, "Error en extrae_frase: formato de frase fonética no válido.\n");
      return NULL;
    }

    // Acotamos la cadena.
    // Así dejamos un espacio en blanco al final, que permite el
    // correcto funcionamiento al final de la frase, cuando hay 2 tokens
    // de pausa seguidos.

    //    if (final_token == NULL)
    //     	tamano = inicio_token - auxiliar;
    //    else
    //    	if (inicio_token == NULL)
    tamano = final_token - auxiliar;
    //        else
    //        	(inicio_token < final_token) ? 	tamano = inicio_token - auxiliar:
    //            								tamano = final_token - auxiliar;

    if ( (salida = (char *) malloc(tamano)) == NULL) {
      fprintf(stderr, "Error en extrae_frase: error al asignar memoria.\n");
      return NULL;
    }

    salida = (char *) memmove(salida, auxiliar, tamano);

    salida[tamano - 1] = '\0';

    // Eliminamos las posibles marcas de ruptura entonativa:

    while ( (inicio_token = strstr(salida, "#%rupt")) != NULL) {

      final_token = strstr(inicio_token + 1, "#");

      for (contador = inicio_token - salida; contador < (int) strlen(salida); contador++)
	salida[contador] = salida[final_token - inicio_token + contador + 2];

    } // while ( (inicio_token = ...

    // Contamos el número de elementos significativos (es decir, todos menos ' ', '-' y '^').
    // Esto nos permitirá saber cuándo hemos procesado por completo un grupo de palabras entre pausas.

    inicio_token = salida;
    final_token = salida + strlen(salida);

    while (inicio_token < final_token)
      if ( (tamano = strcspn(inicio_token, "^ -")) != 0) {
	numero_elementos += tamano;
	inicio_token += tamano + 1;
      }
      else
	inicio_token++;

    if (numero_elementos < 0) {
      fprintf(stderr, "Error en extrae_frase_y_posiciones: formato de frase no válido.\n");
      return NULL;
    }

    // Calculamos el número de sílabas:

    inicio_token = salida;
    silabas_totales = 0;

    while ( (inicio_token[0] == ' ') && (inicio_token[0] != '\0') )
      inicio_token++;

    while ( (tamano = strcspn(inicio_token, " -")) != 0) {
      inicio_token += tamano + 1;
      silabas_totales++;
    }

    // Contamos el número de tónicas.

    inicio_token = salida;
    tonicas_totales = 0;

    while ( (inicio_token = strstr(inicio_token, "^")) != NULL) {
      tonicas_totales++;
      inicio_token++;
    }

    // Calculamos la posición silábica en la cadena de los límites de
    // posición en la frase.

    if ( (inicio_token = strstr(salida, "^")) == NULL)
      primer_acento = INT_MAX;
    else {
      // Contamos el número de sílabas hasta llegar a la tónica.

      primer_acento = -1;
      final_token = salida;
      while (final_token < inicio_token)
	if ( (tamano = strcspn(final_token, "- ")) == 0)
	  final_token++;
	else {
	  final_token +=tamano + 1;
	  primer_acento++;
	}
      // Ahora calculamos la posición de la última sílaba tónica.

      final_token = salida + strcspn(salida, "\0") - 1;

      while (final_token[0] != '^')
	final_token--;

      inicio_token = salida;
      segundo_acento = -1;

      while (inicio_token < final_token)
	if ( (tamano = strcspn(inicio_token, " -")) == 0)
	  inicio_token++;
	else {
	  inicio_token += tamano + 1;
	  segundo_acento++;
	}
    }

    return salida;

  }


  /**
   * Función:   comprueba_error_etiquetado
   * \remarks Entrada:
   *			- alof_cotovia: alófono transcrito por Cotovía.
   *			- correccion_manual: si es cero, el corpus no fue corregido manualmente,
   *			  por lo que nos fiamos del timbre de Cotovía.
   * Entrada y salida:
   *          - alof_real: alófono con la transcripción obtenida del fichero del
   *            corpus. A la salida tiene el alófono que adoptamos como válido.
   * \remarks Valor devuelto:
   *          - Se devuelve un 1 en el caso de que sea un error de etiquetado, y
   *            0 en caso contrario.
   * \remarks Objetivo:  Comprobar si la diferencia se debe simplemente a un error en la
   *            detección del timbre del fonema, o bien son fonemas totalmente
   *            diferentes.
   */

  int Crea_vectores_descriptor::comprueba_error_etiquetado(char *alof_cotovia, char *alof_real,
							   int correccion_manual) {


    if (alof_real[strlen(alof_real) - 1] == '!') { // Es una corrección manual (Luis Juncal)
      alof_real[strlen(alof_real) - 1] = '\0';
      return 0;
    } // if (alof_real...

    // Primero comprobamos si es un error en el timbre de 'o' o 'e'.


    if ( ( (alof_cotovia[0] == 'o') && (alof_real[0] == 'O') ) ||
         ( (alof_cotovia[0] == 'O') && (alof_real[0] == 'o') ) ||
         ( (alof_cotovia[0] == 'e') && (alof_real[0] == 'E') ) ||
         ( (alof_cotovia[0] == 'E') && (alof_real[0] == 'e') ) )

      if (correccion_manual == 1)
	// En este caso tomamos como valor válido el real.
	return 0;
      else {
	alof_real[0] = alof_cotovia[0];
	return 2;
      }



    // En los siguientes casos, suponemos que la transcripción adecuada es la de Cotovía.

    if ( (alof_cotovia[0] == 'b') && (alof_real[0] == 'B') ) {
      alof_real[0] = 'b';
      return 2;
    }

    if ( (alof_cotovia[0] == 'B') && (alof_real[0] == 'b') ) {
      alof_real[0] = 'B';
      return 2;
    }

    if ( (alof_cotovia[0] == 'D') && (alof_real[0] == 'd') ) {
      alof_real[0] = 'D';
      return 2;
    }

    if ( (alof_cotovia[0] == 'd') && (alof_real[0] == 'D') ) {
      alof_real[0] = 'd';
      return 2;
    }

    if ( (alof_cotovia[0] == 'G') && (alof_real[0] == 'g') ) {
      alof_real[0] = 'G';
      return 2;
    }

    if ( (alof_cotovia[0] == 'g') && (alof_real[0] == 'G') ) {
      alof_real[0] = 'g';
      return 2;
    }
    
    /////martaaaaa
    /*
     *if ( (alof_cotovia[0] == 'Z') && (alof_real[0] == 'L') ) {
     *   alof_real[0] = 'Z';
     *   return 2;
     *}
     *if ( (alof_cotovia[0] == 'Z') && !strcmp(alof_real, "jj") ) {
     *   alof_real[0] = 'Z';
     *   return 2;
     *}
     */
    /*
      if ( (alof_cotovia[0] == 'r') && (alof_real[0] == 'R') ) {
      alof_real[0] = 'r';
      return 2;
      }
      if ( (alof_cotovia[0] == 's') && (alof_real[0] == 'z') ) {
      alof_real[0] = 's';
      return 2;
      }
      if ( (alof_cotovia[0] == 'n') &&  (alof_real[0] == 'N') ) {
      alof_real[0] = 'n';
      return 2;
      }
      if ( (alof_cotovia[0] == 'N') && (alof_real[0] == 'n') ) {
      alof_real[0] = 'N';
      return 2;
      }
    */
    /////martaaaaa
    //karolina
#ifdef _MODOA_EU
    if ( (alof_cotovia[0] == 'n') && (alof_real[0] == 'm') ) {
      alof_real[0] = 'n';
      return 2;
    }
#endif
    //karolina


    // En cualquier otro caso, consideramos que es un error importante. (Por ejemplo,
    // de pronunciación del locutor).

    return 1;

  }

#endif

  /**
   * Función:   frase_solo_signos_de_puntuacion
   * \remarks Entrada:
   *			- frase: cadena en la que se recoge la información de cada
   *            palabra por separado.                                        
   * \remarks Valor devuelto:                                                         
   *          - Si la cadena consta de sólo signos de puntuación, devuelve
   *            un 1.                                                        
   * \remarks Objetivo:  Comprobar si la frase de entrada consta únicamente de signos
   *            de puntuación.
   */

  int frase_solo_signos_de_puntuacion(t_frase_separada *frase) {

    t_frase_separada *correcaminos = frase;

    while (*correcaminos->pal) {
      //    for (int contador = 0; contador < numero_de_elementos_de_frase_separada; contador++) {
      if ( (correcaminos->clase_pal != SIGNO_PUNTUACION) &&
	   (correcaminos->clase_pal != SIN_CLASIFICAR) )
	return 0;
      correcaminos++;
    }

    return 1;

  }

  /**
   * Función:   cuenta_numero_palabras
   * \remarks Entrada:
   *			- frase: cadena en la que se recoge la información de cada
   *            palabra por separado.                                        
   * \remarks Valor devuelto:                                                         
   *          - Si la cadena consta de sólo signos de puntuación, devuelve
   *            un 1.                                                        
   * \remarks Objetivo:  Comprobar si la frase de entrada consta únicamente de signos
   *            de puntuación.
   */

  int cuenta_numero_palabras(t_frase_separada *frase) {

    int numero_palabras = 0;
    t_frase_separada *correcaminos = frase;

    while (*correcaminos->pal) {
      //    for (int contador = 0; contador < numero_de_elementos_de_frase_separada; contador++) {
      if ( (correcaminos->clase_pal != SIGNO_PUNTUACION) &&
	   (correcaminos->clase_pal != SIN_CLASIFICAR) &&
	   (correcaminos->clase_pal != CARACTER_ESPECIAL) )
	numero_palabras++;
      correcaminos++;
    }

    return numero_palabras;

  }


#ifdef _SECUENCIAS_FONETICAS

  /**
   * Función:   lee_estructuras_foneticas
   * \remarks Entrada:
   *			- fichero: fichero de texto en el que se encuentran las estructuras       
   *            fonéticas que se desea buscar, según el formato siguiente, por líneas:
   *            * Número de estructuras en la zona estacionaria \t Número de
   *			  estructuras en la zona de transición
   *            * Número de ocurrencias por frase, es decir, número de fonemas y
   *            transiciones entre fonemas buscados que se desea que, como mínimo,
   *            aparezcan en cada frase seleccionada.                                   
   *            * Número mínimo de palabras en la frase.
   *            * Número máximo de palabras en la frase.
   *            * Por cada estructura (en la misma línea, separado por tabuladores):
   *               - Clase del primer fonema.                                           
   *               - Clase del segundo (si la unión es por la parte de transición).
   *	\remarks Salida:
   *			- *fonemas_estacionario: cadena indexada con la información de los
   *            fonemas buscados.                                                       
   *          - numero_fonemas_estacionario: número de estructuras de la cadena
   *            anterior.                                                               
   *          - fonemas_transicion: cadena indexada con la información de las
   *            transiciones entre fonemas buscados.                                    
   *          - numero_fonemas_estacionario: número de estructuras de la cadena
   *            anterior.                                                               
   *          - minimo_numero_palabras: mínimo número de palabras de una frase para ser
   *            escogida.                                                               
   *          - maximo_numero_palabras: máximo número de palabras de una frase para ser
   *            escogida.                                                               
   *          - porcentaje_minimo_ocurrencias: mínimo porcentaje de ocurrencias de las
   *            estructuras anteriores para que la frase pueda ser escogida.            
   * \remarks Valor devuelto:                                                                    
   *          - En ausencia de error se devuelve un cero.                               
   * \remarks Objetivo:  Cargar en memoria el contenido del fichero que contiene las estructuras 
   *            fonéticas que se desean encontrar.
   * \remarks NOTA:	- Las estructuras deben ir en primer lugar las de la zona estacionaria,   
   *			  seguidas de las de la zona de transición.
   */

  int Crea_vectores_descriptor::lee_estructuras_foneticas(Estructura_fonema_estacionario **fonemas_estacionario,
							  int *numero_fonemas_estacionario,
							  Estructura_fonema_transicion **fonemas_transicion,
							  int *numero_fonemas_transicion,
							  int *minimo_numero_palabras,
							  int *maximo_numero_palabras,
							  int *porcentaje_minimo_ocurrencias, FILE *fichero) {

    int contador;
    char clase_fonema_1[100], clase_fonema_2[100];
    Estructura_fonema_estacionario *correcaminos_estacionario;
    Estructura_fonema_transicion *correcaminos_transicion;


    fscanf(fichero, "%d\t%d\n", numero_fonemas_estacionario, numero_fonemas_transicion);

    fscanf(fichero, "%d\n%d\n%d\n", porcentaje_minimo_ocurrencias, minimo_numero_palabras,
	   maximo_numero_palabras);

    if ( (*fonemas_estacionario = (Estructura_fonema_estacionario *) malloc(
									    *numero_fonemas_estacionario*sizeof(Estructura_fonema_estacionario))) == NULL) {
      fprintf(stderr, "Error en lee_estructuras_foneticas, al asignar memoria.\n");
      return 1;
    }
    correcaminos_estacionario = *fonemas_estacionario;

    if ( (*fonemas_transicion = (Estructura_fonema_transicion *) malloc(
									*numero_fonemas_transicion*sizeof(Estructura_fonema_transicion))) == NULL) {
      fprintf(stderr, "Error en lee_estructuras_foneticas, al asignar memoria.\n");
      return 1;
    }
    correcaminos_transicion = *fonemas_transicion;

    for (contador = 0; contador < *numero_fonemas_estacionario; contador++) {

      fscanf(fichero, "%s\n", clase_fonema_1);

      if (devuelve_tipo_alofono_espectral(clase_fonema_1, &correcaminos_estacionario->fonema))
	return 1;

      correcaminos_estacionario++;

    } // for (contador = 0; ... // estacionario

    for (contador = 0; contador < *numero_fonemas_transicion; contador++) {

      fscanf(fichero, "%s\t%s\n", clase_fonema_1, clase_fonema_2);

      if (devuelve_tipo_alofono_espectral(clase_fonema_1, &correcaminos_transicion->primer_fonema))
	return 1;
      if (devuelve_tipo_alofono_espectral(clase_fonema_2, &correcaminos_transicion->segundo_fonema))
	return 1;

      correcaminos_transicion++;

    } // for (contador = 0; ... // transicion

    return 0;

  }

  /**
   * Función:   seleccion_frases_secuencias_foneticas
   * \remarks Entrada:
   *			- frase_extraida: frase original.                                                
   *          - frase_prosodica: cadena en la que se encuentra la información de los fonemas
   *			  de frase_extraida.											                 
   *			- frase_separada: array con la información de cada palabra.
   *			- fonema_estacionario: cadena con los fonemas buscados.							 
   *			- numero_fonemas_estacionario: número de elementos de la cadena anterior.
   *			- fonema_transicion: cadena con las transiciones de buscadas de fonemas.		 
   *			- numero_fonemas_transicion: número de elementos de la cadena anterior.
   *			- minimo_numero_palabras: número mínimo de palabras para que la frase sea válida.
   *			- maximo_numero_palabras: número máximo de palabras para que la frase sea válida.
   *			- porcentaje_fonemas_buscados: porcentaje de transiciones entre semifonemas que  
   *			  debe de haber en una frase, como mínimo, para que ésta sea válida.
   *	\remarks Salida:
   *			- fichero_corpus: fichero en el que se van añadiendo las frases seleccionadas.
   *          - fichero_rechazo: fichero en el que se incluyen aquellas frases que no tenían
   *            ninguna unidad entre las buscadas.                                             
   * \remarks Entrada y Salida:                                                                         
   *          - *corpus_prosodico: cadena de estructuras que se desea que formen el corpus     
   *            prosódico. A la salida se eliminan aquellas ya encontradas.
   *          - numero_estructuras: número de elementos de la cadena anterior. Se modifica a
   *            salida, si es necesario.                                                       
   * \remarks Valor devuelto:                                                                           
   *          - En ausencia de error devuelve un 0.                                            
   * \remarks Objetivo:  Determina si una frase debe pertenecer o no al corpus prosódico, en función
   *            del número de grupos entre pausas y de las palabras tónicas contenidas.
   */

  int Crea_vectores_descriptor::seleccion_frases_secuencias_foneticas(char *frase_extraida, t_prosodia *frase_prosodica,
								      t_frase_separada *frase_separada,
								      Estructura_fonema_estacionario *fonema_estacionario, int numero_fonemas_estacionario,
								      Estructura_fonema_transicion *fonema_transicion, int numero_fonemas_transicion,
								      int minimo_numero_palabras, int maximo_numero_palabras,
								      float porcentaje_fonemas_buscados,
								      FILE *fichero_corpus, FILE *fichero_rechazo) {


    int numero_palabras;
    int cuenta, encontrados = 0, cuenta_total = 0;
    clase_espectral clase_primer_fonema, clase_segundo_fonema;
    Estructura_fonema_estacionario *recorre_estacionario;
    Estructura_fonema_transicion *recorre_transicion;
    t_prosodia *correcaminos = frase_prosodica;

    numero_palabras = cuenta_numero_palabras(frase_separada);

    if ( (numero_palabras < minimo_numero_palabras ) ||
    	 (numero_palabras > maximo_numero_palabras) ) { // La frase no es válida
      fprintf(fichero_rechazo, "%s\n", frase_extraida);
      return 0;
    } // if ( (numero_palabras ...

    while (correcaminos->alof[0] != '\0') {

      cuenta_total++;

      // Comprobamos si el fonema está entre los buscados:

      recorre_estacionario = fonema_estacionario;

      if (tipo_fon_espectral_enumerado(correcaminos->alof, &clase_primer_fonema))
	return 1;

      for (cuenta = 0; cuenta < numero_fonemas_estacionario; cuenta++) {
	if (clase_primer_fonema == (clase_espectral) recorre_estacionario->fonema) {
	  encontrados++;
	  break;
	}
	else
	  recorre_estacionario++;
      } // for (cuenta = 0; ...

      if (cuenta_total > 1) {

	clase_segundo_fonema = clase_primer_fonema;

	if (tipo_fon_espectral_enumerado((correcaminos - 1)->alof, &clase_primer_fonema))
	  return 1;

	recorre_transicion = fonema_transicion;

	for (cuenta = 0; cuenta < numero_fonemas_transicion; cuenta++)
	  if ( (clase_primer_fonema == recorre_transicion->primer_fonema) &&
	       (clase_segundo_fonema == recorre_transicion->segundo_fonema) ) {
	    encontrados++;
	    break;
	  }
	  else
	    recorre_transicion++;
      } // if (cuenta_fonemas > 1)

      correcaminos++;

    } // while (correcaminos->alof[0] != '\0')


    cuenta_total *= 2; // Dado que en realidad estamos tratando con semifonemas.

    if ( (float) encontrados / cuenta_total >= porcentaje_fonemas_buscados) {
      // Se trata de una frase válida.
      fprintf(fichero_corpus, "%s\n", frase_extraida);
    } // if ( (float)
    else
      fprintf(fichero_rechazo, "%s\n", frase_extraida);

    return 0;

  }


  /**
   * Función:   libera_estructuras_foneticas
   * \remarks Entrada:
   *			- fonema_estacionario: cadena con los fonemas buscados.				  
   *			 - fonema_transicion: cadena con las transiciones buscadas de fonemas.	  	 	
   */

  void Crea_vectores_descriptor::libera_estructuras_foneticas(Estructura_fonema_estacionario *fonema_estacionario,
							      Estructura_fonema_transicion *fonema_transicion) {
    free(fonema_estacionario);
    free(fonema_transicion);
    
  }

#endif

#ifdef _CORPUS_PROSODICO

  /**
   * Función:   genera_corpus_prosodico
   * \remarks Entrada:
   *			- frase_extraida: frase original.                                                
   *          - frase_fonetica: cadena en la que se encuentra la información silábica, de
   *            tonicidad y de pausas.                                                         
   *			- frase_separada: array con la información de cada palabra.
   *	\remarks Salida:
   *			- fichero_corpus: fichero en el que se van añadiendo las frases seleccionadas.
   *          - fichero_rechazo: fichero en el que se incluyen aquellas frases que no tenían
   *            ninguna unidad entre las buscadas.                                             
   * \remarks Entrada y Salida:                                                                         
   *          - *corpus_prosodico: cadena de estructuras que se desea que formen el corpus     
   *            prosódico. A la salida se eliminan aquellas ya encontradas.
   *          - numero_estructuras: número de elementos de la cadena anterior. Se modifica a
   *            salida, si es necesario.                                                      
   * \remarks Valor devuelto:                                                                           
   *          - En ausencia de error devuelve un 0.                                            
   * \remarks Objetivo:  Determina si una frase debe pertenecer o no al corpus prosódico, en función
   *            del número de grupos entre pausas y de las palabras tónicas contenidas.
   */

  int Crea_vectores_descriptor::genera_corpus_prosodico(char *frase_extraida, char *frase_fonetica,
							t_frase_separada *frase_separada,
							Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
							FILE *fichero_corpus, FILE *fichero_rechazo) {

    static int identificador_frase = 0; // Variable precisa para asignar un identificador único a
    // todos los campos de una frase válida para el corpus.
    short int *cadena_tonicas;
    int numero_grupos;
    int cuenta, contador;
    char encontrado = 0;
    Estructura_prosodica *correcaminos = *corpus_prosodico;
    unsigned char tipo_frase;
    char cadena_frase[3];

    if (frase_solo_signos_de_puntuacion(frase_separada))
      return 0;

    if (frase_extraida[0]) {
      if (cuenta_grupos_y_tonicas(frase_fonetica, &numero_grupos, &cadena_tonicas))
	return 1;

      // Comprobamos el tipo de frase:
      if (strstr(frase_extraida, "?"))
	tipo_frase = FRASE_INTERROGATIVA;
      else
	if (strstr(frase_extraida, "!"))
	  tipo_frase = FRASE_EXCLAMATIVA;
	else
	  if (!strcmp(&frase_extraida[strlen(frase_extraida) - 1], "·"))
	    tipo_frase = FRASE_INACABADA;
	  else
	    tipo_frase = FRASE_ENUNCIATIVA;

      // Ahora comprobamos si es alguna de las buscadas:

      for (contador = 0; contador < *numero_estructuras; contador++) {
	if ( (numero_grupos == correcaminos->numero_grupos)
	     && (correcaminos->tipo_frase == tipo_frase) ) {
	  encontrado = 1;
	  for (cuenta = 0; cuenta < numero_grupos; cuenta++)
	    if ( (cadena_tonicas[cuenta] > (correcaminos->numero_tonicas)[cuenta].maximo) ||
		 (cadena_tonicas[cuenta] < (correcaminos->numero_tonicas)[cuenta].minimo) ) {
	      encontrado = 0;
	      break;
	    }
	  if (encontrado)
	    break;
	}
	correcaminos++;
      }

      if (encontrado) {

	switch (correcaminos->tipo_frase) {
	case FRASE_EXCLAMATIVA:
	  strcpy(cadena_frase, "E!");
	  break;
	case FRASE_INTERROGATIVA:
	  strcpy(cadena_frase, "I?");
	  break;
	case FRASE_INACABADA:
	  strcpy(cadena_frase, "I.");
	  strcpy(&frase_extraida[(strlen(frase_extraida) - 2)], "...");
	  break;
	default:
	  strcpy(cadena_frase, "EN");
	}

	if (--(correcaminos->ocurrencias) <= 0) { // Tenemos que eliminar el elemento.
	  if (elimina_estructura_prosodia(corpus_prosodico, contador, numero_estructuras))
	    return 1;
	}
	fprintf(fichero_corpus, "**** %d Número de grupos: %d.\n", identificador_frase,
		numero_grupos);
	fprintf(fichero_corpus, "$$$$ %d Tónicas: ", identificador_frase);
	for (cuenta = 0; cuenta < numero_grupos; cuenta++)
	  fprintf(fichero_corpus, "%d ", cadena_tonicas[cuenta]);
	fprintf(fichero_corpus, "\n%%%%%%%% %d Frase fonética: %s.\n", identificador_frase,
		frase_fonetica);

	fprintf(fichero_corpus, "////\t(%s) %d %s\n\n", cadena_frase, identificador_frase++,
		frase_extraida);
	fprintf(fichero_corpus, "%s\n", frase_extraida);
      } // if (encontrado)
      else {
	if (frase_extraida[strlen(frase_extraida) - 1] == '·')
	  strcpy(&frase_extraida[strlen(frase_extraida) - 2], "...");

	fprintf(fichero_rechazo, "%s\n", frase_extraida);

	// Contamos el número de sílabas.
	/*
	  inicializa_calculo_posicion_y_tonicidad();
	  char *salida;
	  int total = 0, numero_silabas = 0;
	  for (contador = 0; contador < numero_grupos; contador++) {
	  salida = extrae_frase_y_posiciones(frase_fonetica);
	  numero_silabas += silabas_totales;
	  free(salida);
	  }

	  for (cuenta = 0; cuenta < numero_grupos; cuenta++)
	  total += cadena_tonicas[cuenta];
	  fprintf(fichero_rechazo, "%d\t%d\t%d\n", numero_grupos, total, numero_silabas);
	*/

      } // else
      /*
	fprintf(fichero_corpus, "Frase: %s\n", frase_extraida);


        fprintf(fichero_corpus, "Número de grupos: %d.\n", numero_grupos);
       	for (int contador = 0; contador < numero_grupos; contador++)
	fprintf(fichero_corpus, "Grupo %d\t Tónicas: %d.\n", contador, cadena_tonicas[contador]);
        fprintf(fichero_corpus, "\n");
      */
      free(cadena_tonicas);
    }


    return 0;

  }

  /**
   * Función:   escribe_corpus_prosodico
   * \remarks Entrada:
   *			- inicial: puntero a la cadena indexada de estructuras prosódicas buscadas.
   *          - numero_estructuras: número de elementos de la cadena anterior.
   *	\remarks Salida:
   *			- fichero: fichero de tipo texto en el que se escriben las estructuras que no se 
   *            han encontrado.                                                                
   * Objtivo:   Escribe en un fichero el número y el tipo de frases que no se han encontrado.
   */

  void Crea_vectores_descriptor::escribe_corpus_prosodico(Estructura_prosodica *inicial, int numero_estructuras,
							  FILE *fichero) {

    Estructura_prosodica *correcaminos = inicial;
    Estructura_umbrales *recorre;
    char tipo_de_frase[3];
    int contador;

    fprintf(fichero, "%d\n\n", numero_estructuras);

    for (; numero_estructuras; numero_estructuras--) {
      switch (correcaminos->tipo_frase) {
      case FRASE_ENUNCIATIVA:
        strcpy(tipo_de_frase, "EN");
        break;
      case FRASE_EXCLAMATIVA:
        strcpy(tipo_de_frase, "E!");
        break;
      case FRASE_INTERROGATIVA:
        strcpy(tipo_de_frase, "I?");
        break;
      default:
        strcpy(tipo_de_frase, "I.");
      }

      fprintf(fichero, "%d %s\n", correcaminos->ocurrencias, tipo_de_frase);
      fprintf(fichero, "%d\n", correcaminos->numero_grupos);

      recorre = correcaminos->numero_tonicas;
      for (contador = 0; contador < correcaminos->numero_grupos; contador++) {
        fprintf(fichero, "%d %d", recorre->minimo, recorre->maximo);
        if (contador != correcaminos->numero_grupos - 1)
          fprintf(fichero, ", ");
      }
      fprintf(fichero, "\n\n");
      correcaminos++;
    }
  }

  /**
   * Función:   cuenta_grupos_y_tonicas
   * \remarks Entrada:
   *			- frase_fonetica: cadena en la que se encuentra la información silábica, de
   *            tonicidad, y de pausas.                                                        
   *	\remarks Salida:
   *			- numero_grupos: número de grupos entre pausas.
   *          - *tonicas: cadena en la que irá indexado el número de palabras tónicas de cada
   *            grupo.                                                                         
   * \remarks Valor devuelto:                                                                           
   *          - En ausencia de error devuelve un 0.                                            
   * \remarks Objetivo:  Calcula el número de grupos entre pausas de una frase, además del número de
   *            palabras  tónicas de cada uno de ellos.
   */

  int Crea_vectores_descriptor::cuenta_grupos_y_tonicas(char *frase_fonetica, int *numero_grupos, short int **tonicas) {

    register char *inicio_token, *final_token, *inicio_grupo;
    register int fragmentos_entre_pausas = 0;
    register short int numero_tonicas; // = -1;
    register int tamano_reservado = 100;


    if ( (*tonicas = (short int *) malloc(tamano_reservado*sizeof(short int))) == NULL) {
      fprintf(stderr, "Error en cuenta_grupos_y_tonicas, al asignar memoria.\n");
      return 1;
    }

    inicio_token = frase_fonetica + 2; // Adelantamos el primer token de pausa.
    final_token = inicio_token;
    inicio_grupo = inicio_token;
    while ( (inicio_token = strstr(inicio_token, "%pausa")) != NULL) {

      inicio_token++;
      numero_tonicas = -1;
      if (inicio_token - final_token > 14) { // Para evitar dos tokens seguidos
	final_token = inicio_token;
	while (inicio_grupo++ < inicio_token) { // Contamos el número de tildes.
	  numero_tonicas++;
	  if ((inicio_grupo = strstr(inicio_grupo, "^")) == NULL)
	    break;
	}
	if (++fragmentos_entre_pausas >= tamano_reservado) {
	  tamano_reservado += 100;
	  if ( (*tonicas = (short int *) realloc(*tonicas, tamano_reservado*sizeof(short int))
		) == NULL) {
	    fprintf(stderr, "Error en cuenta_grupos_y_tonicas, al asignar memoria.\n");
	    return 1;
	  }
	}

	(*tonicas)[fragmentos_entre_pausas - 1] = numero_tonicas;
	//             numero_tonicas = -1;
      }
      inicio_grupo = inicio_token;
    }



    if ( (*tonicas = (short int *) realloc(*tonicas, fragmentos_entre_pausas*sizeof(short int)))
         == NULL) {
      fprintf(stderr, "Error en cuenta_grupos_y_tonicas, al reasignar memoria.\n");
      return 1;
    }

    *numero_grupos = fragmentos_entre_pausas;

    return 0;

  }



  /**
   * Función:   lee_corpus_prosodico
   * \remarks Entrada:
   *			- fichero: fichero de texto en el que se encuentran las estructuras       
   *            prosódicas que se desea buscar, según el formato siguiente, por líneas:
   *            * Número de estructuras en total.
   *            * Línea en blanco.
   *            * Por cada estructura:                                                  
   *               - Número de ocurrencias de la estructura que se desea encontrar, y
   *                 tipo de frase [\n]                                                 
   *               - Número de grupos entre pausas [\n]
   *               - Umbrales de palabras tónicas en cada grupo, por orden y según el
   *                 formato MIN0 MAX0, MIN1 MAX 1, MIN2 MAX2,... [\n]                  
   *               - Línea en blanco.
   *	\remarks Salida:
   *			- *corpus_prosodico: cadena indexada con la información de las
   *            estructuras que quedan por encontrarse. A la salida se eliminan las ya  
   *            las ya encontradas.                                                     
   *          - numero_estructuras: número de estructuras de la cadena indexada.
   * \remarks Valor devuelto:                                                                    
   *          - En ausencia de error se devuelve un cero.                               
   * \remarks Objetivo:  Cargar en memoria el contenido del fichero que contiene las estructuras 
   *            prosódicas que se desea que formen parte del corpus.
   */

  int Crea_vectores_descriptor::lee_corpus_prosodico(Estructura_prosodica **corpus_prosodico, int *numero_estructuras,
						     FILE *fichero) {

    Estructura_prosodica *correcaminos;
    int cuenta;
    char tipo_frase[3];
    Estructura_umbrales *recorre_tonicas;

    fscanf(fichero, "%d\n", numero_estructuras);

    if ( (*corpus_prosodico = (Estructura_prosodica *) malloc(
							      *numero_estructuras*sizeof(Estructura_prosodica))) == NULL) {
      fprintf(stderr, "Error en lee_corpus_prosodico, al asignar memoria.\n");
      return 1;
    }
    correcaminos = *corpus_prosodico;

    for (int contador = 0; contador < *numero_estructuras; contador++) {
      fscanf(fichero, "%d %s\n", &correcaminos->ocurrencias, tipo_frase);
      fscanf(fichero, "%d\n", &correcaminos->numero_grupos);

      if (!strcmp(tipo_frase, "E!"))
	correcaminos->tipo_frase = FRASE_EXCLAMATIVA;
      else
	if (!strcmp(tipo_frase, "I?"))
	  correcaminos->tipo_frase = FRASE_INTERROGATIVA;
	else
	  if (!strcmp(tipo_frase, "I."))
	    correcaminos->tipo_frase = FRASE_INACABADA;
	  else
	    correcaminos->tipo_frase = FRASE_ENUNCIATIVA;


      if ( (correcaminos->numero_tonicas = (Estructura_umbrales *)
	    malloc(correcaminos->numero_grupos*sizeof(Estructura_umbrales))) == NULL) {
	fprintf(stderr, "Error en lee_corpus_prosodico, al asignar memoria.\n");
	return 1;
      } // Tengo que acordarme de liberar correctamente la memoria del campo numero_tonicas

      recorre_tonicas = correcaminos->numero_tonicas;

      for (cuenta = 0; cuenta < correcaminos->numero_grupos; cuenta++) {
	fscanf(fichero, "%d %d,", &(recorre_tonicas->minimo), &(recorre_tonicas->maximo));
	recorre_tonicas++;
      }
      //        fscanf(fichero, "\n");
      correcaminos++;
    }

    return 0;

  }


  /**
   * Función:   libera_corpus_prosodico
   * \remarks Entrada:
   *			- corpus_prosodico: cadena indexada con la información de las
   *            estructuras que quedan por encontrarse.                                 
   *          - numero_estructuras: número de estructuras de la cadena indexada.
   */

  void Crea_vectores_descriptor::libera_corpus_prosodico(Estructura_prosodica *corpus_prosodico,
							 int numero_estructuras) {

    Estructura_prosodica *correcaminos;

    correcaminos = corpus_prosodico;

    for (int contador = 0; contador < numero_estructuras; contador++) {
      free(correcaminos->numero_tonicas);
      correcaminos++;
    }

    if (corpus_prosodico) {
      free(corpus_prosodico);
      corpus_prosodico = NULL;
    }
  }


  /**
   * Función:   elimina_estructura_prosodia
   * \remarks Entrada:
   *			- posicion: índice del elemento que se va a eliminar.
   * \remarks Entrada y Salida:                                                                  
   *          - *inicial: puntero al inicio de la cadena.                               
   *          - tamano: número de elementos de la cadena.
   * \remarks Valor devuelto:                                                                    
   *          - En ausencia de error se devuelve un cero.                               
   * \remarks Objetivo:  Eliminar aquellas estructuras de las que ya se ha encontrado el número
   *            deseado de ocurrencias.                                                 
   */

  int Crea_vectores_descriptor::elimina_estructura_prosodia(Estructura_prosodica **inicial,
							    int posicion, int *tamano) {

    Estructura_prosodica *correcaminos = *inicial + posicion;

    // Liberamos la memoria del campo numero_tonicas:

    free(correcaminos->numero_tonicas);

    for (int contador = posicion; contador < *tamano - 1; contador++) {
      *correcaminos = *(correcaminos + 1);
      correcaminos++;
    }

    (*tamano)--;

    if (*tamano) {
      if ( (*inicial = (Estructura_prosodica *) realloc(*inicial, *tamano*sizeof(Estructura_prosodica)))
	   == NULL) {
	fprintf(stderr, "Error en elimina_estructura_corpus, al reasignar memoria.\n");
	return 1;
      }
    }
    else {
      free(*inicial);
      *inicial = NULL;
    }

    return 0;

  }

#endif

#ifdef _PARA_ELISA

  /**
   * Función:    consigue_numero_frase
   * \remarks Entrada:
   *			- nombre_fichero: nombre del fichero, con el formato                  
   *             "<num_frase>"-"<identificador>"-"<repetición>".
   * \remarks Valor devuelto:                                                                 
   *           - Se devuelve el número de la frase. Si el formato es incorrecto, se
   *             devuelve un número negativo.
   */

  int consigue_numero_frase(char *nombre_fichero) {

    char auxiliar[10], *correcaminos;
    int numero;

    if ( (numero = strcspn(nombre_fichero, "-")) == 0)
      return -1;

    strncpy(auxiliar, nombre_fichero, numero);

    auxiliar[numero] = '\0';

    correcaminos = (char *) auxiliar + numero - 1;

    while ( (correcaminos[0] != '\\') && (correcaminos > auxiliar) )
      correcaminos--;

    if (correcaminos != auxiliar)
      numero = atoi(correcaminos + 1);
    else
      numero = atoi(auxiliar);

    if (numero)
      return numero;

    fprintf(stderr, "Error en consigue_numero_frase: formato incorrecto.\n");
    
    return -1;

  }


  /**
   * Función:    extraer_frase
   * \remarks Entrada:
   *			- numero_frase: número de la frase.
   *	\remarks Salida:
   *			- frase_extraida: frase que se corresponde con el número anterior.
   */

  void Crea_vectores_descriptor::extraer_frase(int numero_frase, char *frase_extraida) {

    strcpy(frase_extraida, frases_elisa[numero_frase - 1]);

  }

  /**
   * Función:    carga_fichero_frases_elisa
   * \remarks Entrada:
   *			- fichero: puntero al fichero en el que se encuentran las frases.     
   */

  void Crea_vectores_descriptor::carga_fichero_frases_elisa(FILE *fichero) {

    int contador = 0;
    int cuenta;
    int numero_frase;

    while (!feof(fichero)) {

      fscanf(fichero, "%d.", &numero_frase);

      for (cuenta = contador; cuenta < numero_frase - 1; cuenta++)
	frases_elisa[cuenta][0] = '\0';

      contador = numero_frase;

      fgets(frases_elisa[numero_frase - 1], 1000, fichero);

      if (contador == NUMERO_FRASES_ELISA)
	break;

    }

  }


  /**
   * Función:    crea_nombre_fichero_silabas
   * \remarks Entrada:
   *			- fichero_original: nombre del fichero de fonemas original.           
   *	\remarks Salida:
   *			- nombre_salida: nombre de salida, cambiando la extensión de .sal a
   *             .sil.                                                               
   * \remarks Valor devuelto:                                                                 
   *           - Si el formato de entrada del fichero no es correcto, devuelve -1.   
   */

  int crea_nombre_fichero_silabas(char *fichero_original, char *nombre_salida) {

    char *auxiliar;

    strcpy(nombre_salida, fichero_original);

    if ( (auxiliar = strstr(nombre_salida, ".sal")) == NULL) {
      fprintf(stderr, "Error en crea_nombre_fichero_silabas: formato incorrecto.\n");
      return -1;
    } // if ( (auxiliar = strstr...

    strcpy(auxiliar, ".sil");

    return 0;

  }


  /**
   * Función:    crea_nombre_fichero_duraciones
   * \remarks Entrada:
   *			- fichero_original: nombre del fichero de fonemas original.           
   *	\remarks Salida:
   *			- nombre_salida: nombre de salida, cambiando la extensión de .sal a
   *             .dur.                                                               
   * \remarks Valor devuelto:                                                                 
   *           - Si el formato de entrada del fichero no es correcto, devuelve -1.   
   */

  int crea_nombre_fichero_duraciones(char *fichero_original, char *nombre_salida) {

    char *auxiliar;

    strcpy(nombre_salida, fichero_original);

    if ( (auxiliar = strstr(nombre_salida, ".sal")) == NULL) {
      fprintf(stderr, "Error en crea_nombre_fichero_duraciones: formato incorrecto.\n");
      return -1;
    } // if ( (auxiliar = strstr...

    strcpy(auxiliar, ".dur");

    return 0;

  }


  /**
   * Función:    calcula_duraciones_silabicas
   * \remarks Entrada:
   *			- frase_prosodica: cadena de estructuras con la información prosodica
   *             de las unidades.                                                    
   *           - frase_fonetica: cadena con la transcripción fonética del texto.
   *           - fichero_fonemas: fichero con la información de duraciones de
   *             fonemas.                                                            
   *           - nombre_fichero_sfs: nombre del fichero sfs. Sólo para depurar.
   *			 - nombre_fichero_fonemas: nombre del fichero de fonemas, para depurar.
   *			 - correccion_manual: si vale 0, el corpus no se revisó manualmente.
   *	\remarks Salida:
   *			- fichero_silabas: fichero en el que se va escribiendo la información
   *             con la duración temporal silábica.
   *           - cadena_silabas: cadena indexada con la duración de las sílabas.
   *           - cadena_fonemas: cadena indexada con la duración de los fonemas.
   * \remarks Entrada y Salida:                                                               
   *           - numero_silabas: número de sílabas de cadena_silabas.
   *           - numero_fonemas: número de fonemas de cadena_fonemas.
   * \remarks Valor devuelto:                                                                 
   *             En ausencia de error devuelve un cero.                              
   * \remarks Objetivo:   Convierte la información de duraciones de fonemas a duraciones
   *             silábicas.
   */

  int Crea_vectores_descriptor::calcula_duraciones_silabicas(t_prosodia *frase_prosodica, char *frase_fonetica,
							     FILE *fichero_fonemas, FILE *fichero_silabas,
							     estructura_duracion *cadena_silabas, int *numero_silabas,
							     estructura_duracion *cadena_fonemas, int *numero_fonemas,
							     char nombre_fichero_sfs[], char nombre_fichero_fonemas[],
							     int correccion_manual) {


    static int numero_lecturas = 0;
    int resultado;
    static int indice_alofono_actual = 0;
    estructura_fonemas *fonema_real;

    t_prosodia *correcaminos = frase_prosodica;
    char *punt_frase = frase_fonetica;
    int tamano_silaba = 0;
    char silaba_actual[250] = "";
    char primera = 1;

    float inicio_fonema_anterior = 0, inicio_silaba_anterior = 0;

    estructura_duracion *corre_silabas = cadena_silabas + *numero_silabas;
    estructura_duracion *corre_fonemas = cadena_fonemas + *numero_fonemas;

    FILE *fichero;

    if (numero_lecturas) {

      fonema_real = cadena_fonemas_fichero;
      for (int cuenta = 1; cuenta < numero_lecturas; cuenta++)
	fonema_real++;

    } // if (numero_lecturas)
    else {
      if (carga_fichero_fonemas(&cadena_fonemas_fichero, fichero_fonemas, numero_fonemas, 1))
	return 1;
      fonema_real = cadena_fonemas_fichero;
    } // else

    if (numero_lecturas == 0)
      numero_lecturas++;

    if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
      fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
    if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
      fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
    if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
      fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
    if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!')
      fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';


    while (correcaminos->alof[0] != '\0') {

      indice_alofono_actual++;
        
      if (!primera)
	if ( (correcaminos->alof[0] == '#') && ((correcaminos - 1)->alof[0] == '#') ) {
	  correcaminos++;
	  continue;
	}

      if ( ( (strlen(correcaminos->alof) != strlen(fonema_real->fonema)) ||
	     (correcaminos->alof[0] != fonema_real->fonema[0]) ) &&
	   (fonema_real->fonema[strlen(fonema_real->fonema) - 1] != '!')	) {

	if (correcaminos->alof[0] == '#') {


	  if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
	    fprintf(stderr, "Error en calcula_duraciones_silabicas, \
                        al intentar abrir el fichero de problemas.\n");
	    getchar();
	    return 1;
	  }

	  fprintf(fichero, "%s\t%s\t%s\n", correcaminos->alof, fonema_real->fonema,
		  nombre_fichero_fonemas);
	  fprintf(fichero, "%s\n", frase_fonetica);

	  fclose(fichero);
	  indice_alofono_actual--;
	  correcaminos++;

	  continue;
	}

	if (fonema_real->fonema[0] == '#') {

	  fprintf(fichero_silabas, "\t%f %s\n", inicio_fonema_anterior, fonema_real->fonema);

	  fflush(fichero_silabas);

	  strcpy(corre_fonemas->unidad, fonema_real->fonema);

	  fonema_real++;
	  numero_lecturas++;

	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!') {
	    strcpy(silaba_actual, fonema_real->fonema);
	    silaba_actual[strlen(silaba_actual) - 1] = '\0';
	  }
	  else
	    strcpy(silaba_actual, fonema_real->fonema);

	  corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	  corre_fonemas++;

	  (*numero_fonemas)++;

	  inicio_silaba_anterior = fonema_real->inicio;
	  inicio_fonema_anterior = fonema_real->inicio;

	  continue;

	}

	if ( (resultado = comprueba_error_etiquetado(correcaminos->alof, fonema_real->fonema, correccion_manual)) == 1) {

	  if ( (fichero = fopen("errores_etiquetado.txt", "a")) == NULL) {
	    fprintf(stderr, "Error al abrir el fichero de errores de etiquetado.\n");
	    return 1;
	  } // if (fichero_etiquetado = ...

	  fprintf(fichero, "#\t%s.\n#\tCotovía: %s.\tReal: %s.\n", nombre_fichero_sfs,
		  correcaminos->alof, fonema_real->fonema);
	  fprintf(fichero, "\tEs -s %f -e %f %s\n", fonema_real->inicio - 0.3,
		  fonema_real->inicio + 0.3, nombre_fichero_sfs);
	  return 1;
	} // if (resultado = comprueba...
	else
	  if (resultado == 2) {
	    if ( (fichero = fopen("cambia_transcripcion.txt", "at")) == NULL) {
	      fprintf(stderr, "Error al intentar abrir el fichero cambia_transcripcion.txt\n");
	      return 1;
	    } // if ( (fichero = ...

	    fprintf(fichero, "%s %s %d\n", nombre_fichero_fonemas, correcaminos->alof,
		    indice_alofono_actual);
	    fclose(fichero);
	    tamano_silaba -= strlen(fonema_real->fonema);

	    if (tamano_silaba == 0) {
	      strcpy(corre_fonemas->unidad, fonema_real->fonema);

	      fonema_real++;
	      numero_lecturas++;

	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	      corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	      corre_fonemas++;
	      (*numero_fonemas)++;
	      corre_silabas->duracion = fonema_real->inicio - inicio_silaba_anterior;

	      fprintf(fichero_silabas, "\t%f %s\n", inicio_silaba_anterior, silaba_actual);

	      fflush(fichero_silabas);

	      inicio_fonema_anterior = fonema_real->inicio;
	      inicio_silaba_anterior = fonema_real->inicio;
	      strcpy(corre_silabas->unidad, silaba_actual);
	      corre_silabas++;
	      (*numero_silabas)++;

	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!') {
		strcpy(silaba_actual, fonema_real->fonema);
		silaba_actual[strlen(silaba_actual) - 1] = '\0';
	      }
	      else
		strcpy(silaba_actual, fonema_real->fonema);

	      extrae_tamano_siguiente_silaba(&punt_frase, &tamano_silaba);
	    } // if (tamano_silaba == 0)
	    else {
	      strcpy(corre_fonemas->unidad, fonema_real->fonema);

	      fonema_real++;
	      numero_lecturas++;

	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
		fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	      strcat(silaba_actual, fonema_real->fonema);

	      if (silaba_actual[strlen(silaba_actual) - 1] == '!')
		silaba_actual[strlen(silaba_actual) - 1] = '\0';

	      corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	      corre_fonemas++;
	      (*numero_fonemas)++;

	      inicio_fonema_anterior = fonema_real->inicio;
	    } // else

	    correcaminos++;

	    continue;
	  } // if (resultado == 2)

      } // if ( (strlen(correcaminos...


      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!')
	fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

      if (primera) {

	fonema_real++;

	numero_lecturas++;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	inicio_silaba_anterior = fonema_real->inicio;
	inicio_fonema_anterior = fonema_real->inicio;
	correcaminos++;

	extrae_tamano_siguiente_silaba(&punt_frase, &tamano_silaba);

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!') {
	  strcpy(silaba_actual, fonema_real->fonema);
	  silaba_actual[strlen(silaba_actual) - 1] = '\0';
	}
	else
	  strcpy(silaba_actual, fonema_real->fonema);

	primera = 0;

	continue;

      }

      if (fonema_real->fonema[0] == '#') {

	fprintf(fichero_silabas, "\t%f %s\n", inicio_fonema_anterior, fonema_real->fonema);

	fflush(fichero_silabas);

	strcpy(corre_fonemas->unidad, fonema_real->fonema);

	fonema_real++;

	numero_lecturas++;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	corre_fonemas++;
	(*numero_fonemas)++;

	inicio_silaba_anterior = fonema_real->inicio;
	inicio_fonema_anterior = fonema_real->inicio;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!') {
	  strcpy(silaba_actual, fonema_real->fonema);
	  silaba_actual[strlen(silaba_actual) - 1] = '\0';
	}
	else
	  strcpy(silaba_actual, fonema_real->fonema);

	correcaminos++;

	continue;
      }


      tamano_silaba -= strlen(correcaminos->alof);

      if (tamano_silaba == 0) {

	strcpy(corre_fonemas->unidad, fonema_real->fonema);

	fonema_real++;
	numero_lecturas++;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	corre_fonemas++;
	(*numero_fonemas)++;
	corre_silabas->duracion = fonema_real->inicio - inicio_silaba_anterior;

	fprintf(fichero_silabas, "\t%f %s\n", inicio_silaba_anterior, silaba_actual);

	fflush(fichero_silabas);

	inicio_fonema_anterior = fonema_real->inicio;
	inicio_silaba_anterior = fonema_real->inicio;
	strcpy(corre_silabas->unidad, silaba_actual);
	corre_silabas++;
	(*numero_silabas)++;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!') {
	  strcpy(silaba_actual, fonema_real->fonema);
	  silaba_actual[strlen(silaba_actual) - 1] = '\0';
	}
	else
	  strcpy(silaba_actual, fonema_real->fonema);

	extrae_tamano_siguiente_silaba(&punt_frase, &tamano_silaba);

      }
      else {
	strcpy(corre_fonemas->unidad, fonema_real->fonema);

	fonema_real++;
	numero_lecturas++;

	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	  fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	strcat(silaba_actual, fonema_real->fonema);

	if (silaba_actual[strlen(silaba_actual) - 1] == '!')
	  silaba_actual[strlen(silaba_actual) - 1] = '\0';

	corre_fonemas->duracion = fonema_real->inicio - inicio_fonema_anterior;
	corre_fonemas++;
	(*numero_fonemas)++;

	inicio_fonema_anterior = fonema_real->inicio;
      }

      correcaminos++;
    }

    numero_lecturas--;

    indice_alofono_actual--;
    
    return 0;

  }


  /**
   * Función:    extrae_tamano_siguiente_silaba
   *	\remarks Salida:
   *			- tamano_silaba: número de caracteres (no fonemas) de la siquiente
   *             sílaba. (hay fonemas que se representan por medio de 2 caracteres).
   * \remarks Entrada y Salida:                                                               
   *           - frase: frase fonética, con tokens del tipo %prop  %, %pausa %, que
   *             debemos eliminar.                                                   
   */

  void Crea_vectores_descriptor::extrae_tamano_siguiente_silaba(char **frase, int *tamano_silaba) {

    char *punt_frase = *frase;
    int avanza;
    char silaba[100];
    char *auxiliar = punt_frase;

    do {

      avanza = strcspn(punt_frase, "% -");

      auxiliar += avanza;

      if (auxiliar[0] == '\0') {
	*tamano_silaba = 0;
	return ;
      }

      switch (auxiliar[0]) {

      case '%':
	auxiliar++;
	auxiliar = strstr(auxiliar, "%") + 1;
	if (auxiliar[0] == '#')
	  auxiliar++;
	if (auxiliar[0] == '\0') {
	  *tamano_silaba = 0;
	  return;
	}
	punt_frase = auxiliar;
	continue;
      default:
	if ( (punt_frase[0] == '#') || (punt_frase[0] == '%') ||
	     (punt_frase[0] == ' ') || (punt_frase[0] == '-') ) {
	  auxiliar++;
	  punt_frase = auxiliar;
	  continue;
	}
	break;
      }

      break;
    } while (1);

    avanza = strcspn(punt_frase, " -");

    strncpy(silaba, punt_frase, avanza);

    silaba[avanza] = '\0';

    if (strstr(silaba, "^"))
      *tamano_silaba = avanza - 1;
    else
      *tamano_silaba = avanza;

    *frase = punt_frase + avanza;

  }

#endif

#ifdef _GRUPOS_ACENTUALES

  /**
   * Función:    calcula_grupos_acentuales
   * \author	Fran Campillo.
   * \param	frase_extraida: frase, en formato texto.
   * \param	frase_prosodica: cadena de estructuras con la información prosodica
   *             de las unidades.
   * \param	frase_fonetica: cadena con la transcripción fonética del texto.
   * \param	frase_separada: variable en la que se encuentra la información de
   *			   las palabras que forman el texto de entrada.
   * \param	numero_elementos_frase_separada: número de elementos de la cadena
   *			   anterior.
   * \param	frase_sintagmada: variable con la información sintagmática.
   * \param	fichero_fonemas: fichero con la información de duraciones de
   *             fonemas.
   * \param	fichero_silabas: fichero con la división en sílabas del texto de
   *             entrada.
   * \param	frecuencia_estilizada: estructura con las marcas de evolución de la
   *             frecuencia.
   * \param	nombre_fichero: nombre del fichero en el que está la información
   * fonética.
   * \param	correccion_manual: si es cero, el corpus no fue corregido manualmente.
   * \retval	acentuales: cadena de estructuras acentuales que forman la frase
   *             actual.
   * \retval	numero_acentuales: número de elementos de la cadena actual.
   * \return	En ausencia de error devuelve un cero.                              
   * \remarks Extrae las características de los grupos acentuales de la frase del
   *             Corpus considerada.                                                 
   */

#ifdef _MINOR_PHRASING_TEMPLATE

  int Crea_vectores_descriptor::calcula_grupos_acentuales(char *frase_extraida, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_separada *frase_separada, int numero_elementos_frase_separada, t_frase_sintagmada *frase_sintagmada, Grupo_acentual_candidato **acentuales, int *numero_acentuales, Frase_frontera &frase_frontera, Frecuencia_estilizada *frecuencia_estilizada, FILE *fichero_fonemas, FILE *fichero_silabas, char *nombre_fichero, int correccion_manual) {
#else
    int Crea_vectores_descriptor::calcula_grupos_acentuales(char *frase_extraida, t_prosodia *frase_prosodica, char *frase_fonetica, t_frase_separada *frase_separada, int numero_elementos_frase_separada, t_frase_sintagmada *frase_sintagmada, Grupo_acentual_candidato **acentuales, int *numero_acentuales, Frecuencia_estilizada *frecuencia_estilizada, FILE *fichero_fonemas, FILE *fichero_silabas, char *nombre_fichero, int correccion_manual) {
#endif

      static int num_frase = 0;
      static int numero_lecturas = 0;
      static int numero_silabas = 0;
      static int identificador = 0;
      static int identificador_GF = 0;

      FILE *fichero_inicio;

      float tiempo_silencio = 0.0, tiempo_silaba;

      unsigned char tipo_siguiente_sintagma;
      static estructura_fonemas *fonema_real;
      //    static estructura_fonemas *cadena_fonemas = NULL;
      //    int numero_fonemas;

      char silaba[20], silaba_anterior[20] = "", silaba_antes_de_pausa[20] = "";
      int caracteres_silaba;
      //    int cuenta_alofonos = 0;
      int posicion_vocal_fuerte, posicion_vocal_fuerte_anterior,
    	almacena_posicion_vocal_fuerte;

      Grupo_acentual_candidato *recorre_acentuales;
      int memoria_reservada = 500;
      char nuevo_grupo;
      t_prosodia *correcaminos = frase_prosodica;
      t_frase_separada *anterior_frase_separada = frase_separada, *original_frase_separada = frase_separada;
      t_frase_sintagmada *recorre_frase_sintagmada = frase_sintagmada;
      unsigned char inicio_grupo_acentual = 1;
      FILE *fichero;
      char no_primera = 0;
      char alofono_acento;
      estructura_marcas_frecuencia *recorre_marcas = NULL;
      char calcula_frecuencia_silaba = 0;
      float instante_inicial = 0, instante_calculo;
      char tipo_de_pausa;
      char tipo_de_pausa_anterior = PAUSA_PUNTO;
      char tipo_silaba = 0, anterior_tipo_silaba = RESTO_SILABAS;
      char nueva_silaba;
      int anterior_orden = correcaminos->pp;
      int elementos_de_frase_separada = 0;

      int indice_grupo_fonico = -1;
 
      // Variables destinadas al control de aquellas palabras que tienen el campo pal_transformada != NULL
      int numero_palabra = 0; // Posición de la palabra dentro de pal_transformada. Por ejemplo: Pal = 1050 -> Pal_transformada = Mil cincuenta.
      // Variables binarias que controlan el inicio y el fin de un ciclo de palabra transformada.
      char ciclo_transformada = 0;
      char no_final_ciclo = 1;
      t_palabra_proc parte_transformada, parte; // Variables en las que se almacena la palabra transformada completa
      // y la palabra dentro de ella a la que nos referimos.
    
      Prosodia objeto_prosodia;

#ifdef _MINOR_PHRASING_TEMPLATE
      POS_frontera pos_frontera;
      Sintagma_frontera sintagma_frontera;
      Grupo_fonico_frontera grupo_fonico_frontera;
      //    Frase_frontera frase_frontera;
#endif

      // Abrimos el fichero de inicio de los identificadores;

      if (identificador == 0)
        if ( (fichero_inicio = fopen("inicio_numero_GA.txt", "rt")) != NULL) {
	  fscanf(fichero_inicio, "%d", &identificador);
	  fclose(fichero_inicio);
        }

      if (identificador_GF == 0)
        if ( (fichero_inicio = fopen("inicio_numero_GF.txt", "rt")) != NULL) {
	  fscanf(fichero_inicio, "%d", &identificador_GF);
	  fclose(fichero_inicio);
        }

      if ( (*acentuales = (Grupo_acentual_candidato *) malloc(memoria_reservada*
							      sizeof(Grupo_acentual_candidato))) == NULL) {

        fprintf(stderr, "Error en calcula_grupos_acentuales, al asignar memoria.\n");
        return 1;
      }

      recorre_acentuales = *acentuales;

      *numero_acentuales = 0;

      reinicia_grupos_acentuales();

      if (cadena_fonemas == NULL) {

        if (carga_fichero_fonemas(&cadena_fonemas, fichero_fonemas, &numero_fonemas, 1))
	  return 1;

        fonema_real = cadena_fonemas;
      } // if (cadena_fonemas == NULL)
      else {
        fonema_real = cadena_fonemas;
        for (int cuenta = 0; cuenta < numero_lecturas; cuenta++)
	  fonema_real++;

      } // else

      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
        fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
        fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!')
        fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

      if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
        fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';


      // Leemos la siguiente sílaba:

      if (numero_silabas != 0) {
    	fseek(fichero_silabas, 0, SEEK_SET);
	for (int contador = 0; contador < numero_silabas; contador++)
	  fscanf(fichero_silabas, "%f %s\n", &tiempo_silaba, silaba);
      } // if (numero_silabas != 0)

      do
    	fscanf(fichero_silabas, "%f %s\n", &tiempo_silaba, silaba);
      while (silaba[0] == '#');

      caracteres_silaba = strlen(silaba);

      strcpy(silaba_anterior, silaba);

      if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1) {
        fprintf(stderr, "Error en calcula_grupos_acentuales. Sílaba: %s.\n", silaba);
        return 1;
      }

      posicion_vocal_fuerte_anterior = posicion_vocal_fuerte;
      almacena_posicion_vocal_fuerte = posicion_vocal_fuerte_anterior;

#ifdef _MINOR_PHRASING_TEMPLATE
      if ( (frase_separada->tonicidad_forzada == 0) &&
	   (!LOCUCION(frase_separada->cat_gramatical[0])) && 
	   ((frase_separada->cat_gramatical[0] == RELA) ||
	    (frase_separada->cat_gramatical[0] == PRON_PERS_AT) ||
	    (frase_separada->cat_gramatical[0] == PRON_PERS_AT_REFLEX) ||
	    (frase_separada->cat_gramatical[0] == PRON_PERS_AT_ACUS) ||
	    (frase_separada->cat_gramatical[0] == PRON_PERS_AT_DAT) ||
	    (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_AC) ||
	    (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_DAT_AC) ||
	    (frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT) ||
	    (CONXUNCION(frase_separada->cat_gramatical[0]))) ) {
	pos_frontera.etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(frase_separada);
	pos_frontera.tipo_pausa = 0;
	pos_frontera.salto_f0 = 0;
	pos_frontera.factor_caida = 0;
	sintagma_frontera.elementos.push_back(pos_frontera);
	sintagma_frontera.tipo = decide_sintagma_grupo_acentual(frase_separada->tipo_sintagma);
	sintagma_frontera.acento = 0; // sintagma átono
	grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	sintagma_frontera.elementos.clear();
      }
#endif

      while (correcaminos->alof[0] != '\0') {

	if (strcmp(correcaminos->alof, "#") == 0)
	  indice_grupo_fonico++;

    	if (frase_separada - original_frase_separada > recorre_frase_sintagmada->fin) {
#ifdef _MINOR_PHRASING_TEMPLATE
	  if (sintagma_frontera.elementos.size()) {
	    grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	    sintagma_frontera.elementos.clear();
	  }
	  /*            if ( (frase_separada->tonicidad_forzada == 0) &&
			(LOCUCION(frase_separada->cat_gramatical[0]) == 0) &&
			( (CONXUNCION(frase_separada->cat_gramatical[0])) ||
			(frase_separada->cat_gramatical[0] == RELA) ) ) {
	  */

	  if ( (frase_separada->tonicidad_forzada == 0) &&
	       (!LOCUCION(frase_separada->cat_gramatical[0])) &&
	       ((frase_separada->cat_gramatical[0] == RELA) ||
		(frase_separada->cat_gramatical[0] == PRON_PERS_AT) ||
		(frase_separada->cat_gramatical[0] == PRON_PERS_AT_REFLEX) ||
		(frase_separada->cat_gramatical[0] == PRON_PERS_AT_ACUS) ||
		(frase_separada->cat_gramatical[0] == PRON_PERS_AT_DAT) ||
		(frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_AC) ||
		(frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT_DAT_DAT_AC) ||
		(frase_separada->cat_gramatical[0] == CONTR_PRON_PERS_AT) ||

		(CONXUNCION(frase_separada->cat_gramatical[0]))) ) {
	    pos_frontera.etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(frase_separada);
	    pos_frontera.tipo_pausa = 0;
	    pos_frontera.salto_f0 = 0;
	    pos_frontera.factor_caida = 0;
	    sintagma_frontera.elementos.push_back(pos_frontera);
	    sintagma_frontera.tipo = decide_sintagma_grupo_acentual(frase_separada->tipo_sintagma);
	    sintagma_frontera.acento = 0; // sintagma átono
	    grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	    sintagma_frontera.elementos.clear();
	  }
#endif
	  while (frase_separada - original_frase_separada > recorre_frase_sintagmada->fin)
	    recorre_frase_sintagmada++;
        }

        if (correcaminos == frase_prosodica)
	  anterior_orden++;
            
	while ( (frase_separada->clase_pal == SIGNO_PUNTUACION) ) {
	  if (++elementos_de_frase_separada >= numero_elementos_frase_separada) {
	    fprintf(stderr, "Error en calcula_grupos_acentuales: datos de entrada incorrectos.\n");
	    if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
	      fprintf(stderr, "Error en calcula_grupos_acentuales, \
al intentar abrir el fichero de problemas.\n");
	      return 1;
	    }
	  } // if (++elementos_de_frase_separada...
	  frase_separada++; // Avanzamos palabra_separada, ya que no es una palabra, sino
	  // un signo de puntuación.
        } // while (frase_separada...

        if (correcaminos->pp > anterior_orden) { // Cambiamos de palabra

	  // Comprobamos si estamos en un ciclo de palabra transformada.

	  if (ciclo_transformada) {
	    // Cambiamos de palabra, pero dentro de un ciclo de palabra_transformada.
	    numero_palabra++;
	    anterior_orden = correcaminos->pp;

	    if ( (no_final_ciclo = (char)
		  inicia_ciclo_transformada(parte_transformada, parte,  numero_palabra)) < 0) {

	      fprintf(stderr, "Error en calcula_grupos_acentuales: datos de entrada incorrectos.\n");
	      if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		fprintf(stderr, "Error en calcula_grupos_acentuales, al intentar abrir el fichero \
de problemas.\n");
		return 1;
	      }
	    }

	    if (no_final_ciclo == 0) {
	      ciclo_transformada = 0;
	      numero_palabra = 0;
	    } // if (no_final_ciclo == 0)

	  } // if (ciclo_transformada)
	  else {

	    if (++elementos_de_frase_separada < numero_elementos_frase_separada) {
	      anterior_frase_separada = frase_separada;
	      frase_separada++;

	      anterior_orden = correcaminos->pp;
	      while ( (frase_separada->clase_pal == SIGNO_PUNTUACION) ) {
		if (++elementos_de_frase_separada >= numero_elementos_frase_separada) {
		  fprintf(stderr, "Error en calcula_grupos_acentuales: datos de entrada incorrectos.\n");
		  if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		    fprintf(stderr, "Error en calcula_grupos_acentuales, \
al intentar abrir el fichero de problemas.\n");
		    return 1;
		  }
		} // if (++elementos_de_frase_separada...

		frase_separada++; // Avanzamos palabra_separada, ya que no es una palabra, sino
		// un signo de puntuación.
	      } // while (frase_separada...

	    } // if (++elementos_de_frase_separada < ...
	    else {
	      fprintf(stderr, "Error en calcula_grupos_acentuales: datos de entrada incorrectos.\n");
	      if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		fprintf(stderr, "Error en calcula_grupos_acentuales, \
al intentar abrir el fichero de problemas.\n");
		return 1;
	      }
	    } // else

	    if (frase_separada->pal_transformada) {

	      if (anterior_frase_separada->tonicidad_forzada != 1)
		anterior_frase_separada = frase_separada;
                        
	      // Entramos en un ciclo de palabra transformada.
	      ciclo_transformada = 1;
	      strcpy(parte_transformada, frase_separada->pal_transformada);
	      if ( (no_final_ciclo = (char)
		    inicia_ciclo_transformada(parte_transformada, parte, numero_palabra)) < 0) {

		fprintf(stderr, "Error en calcula_grupos_acentuales: datos de entrada incorrectos.\n");
		if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		  fprintf(stderr, "Error en calcula_grupos_acentuales, \
al intentar abrir el fichero de problemas.\n");
		  return 1;
		}

	      } // if (no_final_ciclo...
	      if (no_final_ciclo == 0)
		ciclo_transformada = 0;
	    } // if (palabra_separada->...

	  } // else

        } // if (correcaminos->pp > anterior_orden)

	////////

        nueva_silaba = 0;

        if (correcaminos != frase_prosodica)
	  if ( (correcaminos->alof[0] == '#') &&
	       ( (correcaminos - 1)->alof[0] == '#') ) {
	    correcaminos++;
	    continue;
	  }

        if (no_primera && (correcaminos->alof[0] == '#') &&
	    ((correcaminos - 1)->alof[0] == '#') ) {
	  //           correcaminos++;
	  recorre_acentuales->final = tiempo_silencio;
	  (*numero_acentuales)++;
	  if (calcula_frecuencia_silaba) {
	    instante_calculo = (tiempo_silencio + instante_inicial) / 2;
	    recorre_marcas->tiempo = instante_calculo;
	    recorre_marcas->frecuencia = interpola(frecuencia_estilizada, instante_calculo,
                                                   &recorre_marcas->sonoridad_pobre);
	    recorre_marcas++;
	    recorre_marcas->tiempo = tiempo_silencio;
	    recorre_marcas->frecuencia = interpola(frecuencia_estilizada, tiempo_silencio,
						   &recorre_marcas->sonoridad_pobre);
	    recorre_acentuales->frecuencia.tiempo_inicio = recorre_acentuales->frecuencia.marcas[0].tiempo;

	  }
	  else {
	    recorre_marcas->tiempo = tiempo_silencio;
	    recorre_marcas->frecuencia = interpola(frecuencia_estilizada, tiempo_silencio,
						   &recorre_marcas->sonoridad_pobre);
	  }

	  recorre_acentuales->frecuencia.tiempo_final = tiempo_silencio;

	  recorre_acentuales->indice_maximo =
	    calcula_frecuencia_maxima_grupo(&recorre_acentuales->frecuencia);

	  recorre_acentuales->frecuencia_media =
	    calcula_frecuencia_media_grupo(&recorre_acentuales->frecuencia);

	  recorre_acentuales->factor_caida = calcula_factor_caida(recorre_acentuales);
	  if (calcula_salto_f0(recorre_acentuales, &recorre_acentuales->salto_f0))
	    return 1;

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
	  recorre_acentuales->etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(anterior_frase_separada);

	  recorre_acentuales->sintagma_actual = decide_sintagma_grupo_acentual(anterior_frase_separada->tipo_sintagma);

	  // Cambio
	  devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, anterior_frase_separada - original_frase_separada,
						    &tipo_siguiente_sintagma);

	  recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);
	  //

#ifdef _MINOR_PHRASING_TEMPLATE
	  pos_frontera.etiqueta_morfosintactica = recorre_acentuales->etiqueta_morfosintactica;
	  pos_frontera.tipo_pausa = recorre_acentuales->tipo_pausa;
	  pos_frontera.salto_f0 = recorre_acentuales->salto_f0;
	  pos_frontera.factor_caida = recorre_acentuales->factor_caida;

	  sintagma_frontera.elementos.push_back(pos_frontera);
	  sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	  sintagma_frontera.acento = 1; // sintagma tónico

	  if (frase_separada - original_frase_separada > recorre_frase_sintagmada->fin) {
	    // Cambiamos de sintagma:
	    //                sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	    grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	    sintagma_frontera.elementos.clear();
	  }

	  if ( (recorre_acentuales->tipo_pausa != SIN_PAUSA) &&
	       (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) &&
	       (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_COMA) ) {

	    if (sintagma_frontera.elementos.size()) {
	      //					sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	      grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	      sintagma_frontera.elementos.clear();
	    }

	    grupo_fonico_frontera.tipo_grupo = recorre_acentuales->tipo_grupo_prosodico_3;
	    grupo_fonico_frontera.siguiente_sintagma = recorre_acentuales->siguiente_sintagma;
	    grupo_fonico_frontera.identificador = identificador_GF++;
                
	    if (frase_frontera.elementos.size() == 0)
	      grupo_fonico_frontera.posicion = GRUPO_FONICO_INICIAL;
	    else
	      grupo_fonico_frontera.posicion = GRUPO_FONICO_MEDIO;

	    frase_frontera.elementos.push_back(grupo_fonico_frontera);
	    grupo_fonico_frontera.elementos.clear();
	  }
#endif

	  /*
            if (recorre_frase_sintagmada->fin == frase_separada - original_frase_separada) {

	    devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, anterior_frase_separada - original_frase_separada,
	    &tipo_siguiente_sintagma);

	    recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);

            }
            else {
	    if (recorre_acentuales->sintagma_actual == GA_S_PREPOSICIONAL)
	    recorre_acentuales->siguiente_sintagma = GA_S_NOMINAL;
	    else
	    recorre_acentuales->siguiente_sintagma = recorre_acentuales->sintagma_actual;
            }
	  */

#endif

	  if (strstr(frase_extraida, "·") == NULL)
	    recorre_acentuales->tipo_pausa = PAUSA_PUNTO;
	  else
	    recorre_acentuales->tipo_pausa = PAUSA_PUNTOS_SUSPENSIVOS;

	  numero_lecturas--;
	  num_frase++;

	  if ( (fichero_inicio = fopen("inicio_numero_GA.txt", "wt")) == NULL) {
	    fprintf(stderr, "Error en calcula_grupos_acentuales, al intentar abrir \
                el fichero de identificadores.\n");
	    return 1;
	  }

	  fprintf(fichero_inicio, "%d\n", identificador);
	  fclose(fichero_inicio);

	  if ( (fichero_inicio = fopen("inicio_numero_GF.txt", "wt")) == NULL) {
	    fprintf(stderr, "Error en calcula_grupos_acentuales, al intentar abrir \
                el fichero de identificadores de rupturas.\n");
	    return 1;
	  }

	  fprintf(fichero_inicio, "%d\n", identificador_GF);
	  fclose(fichero_inicio);


	  return 0;
        }

        if ( strcmp(correcaminos->alof, fonema_real->fonema) &&
	     ( (correcaminos->alof[0] != '#') || (fonema_real->fonema[0] != '#') ) ) { // Problemas con la transcripción.

	  if ( (comprueba_error_etiquetado(correcaminos->alof, fonema_real->fonema, correccion_manual) != 0) &&
	       (fonema_real->fonema[strlen(fonema_real->fonema - 1)] != '!') ) {

	    if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
	      fprintf(stderr, "Error en calcula_grupos_acentuales, \
                    al intentar abrir el fichero de problemas.\n");
	      return 1;
	    }

	    puts("Problemas!!!");
	    //                getchar();
	    fprintf(fichero, "Cotovía: %s\tReal: %s\t%s\n", correcaminos->alof, fonema_real->fonema, nombre_fichero);
	    fclose(fichero);

	    return 1;
	  }

        } // if (strcmp(correcaminos->alof...

        if (correcaminos->alof[0] != '#') {

	  if (posicion_vocal_fuerte == 0) {
	    instante_inicial = fonema_real->inicio;;
	    calcula_frecuencia_silaba = 1;
	    posicion_vocal_fuerte = -1;
	  }
	  else
	    posicion_vocal_fuerte -= strlen(fonema_real->fonema);

	  if (fin_grupo_acentual(frase_fonetica, correcaminos->alof, &nuevo_grupo, &alofono_acento,
				 &tipo_de_pausa))
	    return 1;


	  if (nuevo_grupo && no_primera) {
	    if ((correcaminos - 1)->alof[0] == '#') {
	      recorre_acentuales->final = tiempo_silencio;
	      recorre_acentuales->frecuencia.tiempo_final = tiempo_silencio;
	      recorre_marcas->tiempo = tiempo_silencio;
	      recorre_marcas->frecuencia = interpola(frecuencia_estilizada, tiempo_silencio,
						     &recorre_marcas->sonoridad_pobre);
	      if (devuelve_tipo_estructura_silaba(silaba, posicion_vocal_fuerte_anterior, &tipo_silaba))
		return 1;
#ifdef _INCLUYE_SILABAS
	      strcpy(recorre_marcas->silaba, silaba);
#endif
	      recorre_marcas->tipo_silaba = tipo_silaba;
	    }
	    else {
	      recorre_acentuales->final = fonema_real->inicio;
	      recorre_acentuales->frecuencia.tiempo_final = fonema_real->inicio;
	      recorre_marcas->tiempo = fonema_real->inicio;
	      recorre_marcas->frecuencia = interpola(frecuencia_estilizada, fonema_real->inicio,
						     &recorre_marcas->sonoridad_pobre);
	      if (devuelve_tipo_estructura_silaba(silaba, posicion_vocal_fuerte_anterior, &tipo_silaba))
		return 1;
#ifdef _INCLUYE_SILABAS
	      strcpy(recorre_marcas->silaba, silaba);
#endif
	      recorre_marcas->tipo_silaba = tipo_silaba;
	    }

	    recorre_acentuales->tipo_pausa_anterior = tipo_de_pausa_anterior;

	    recorre_acentuales->tipo_pausa = tipo_de_pausa;
	    tipo_de_pausa_anterior = tipo_de_pausa;

	    recorre_acentuales->indice_maximo =
	      calcula_frecuencia_maxima_grupo(&recorre_acentuales->frecuencia);

	    recorre_acentuales->frecuencia_media =
	      calcula_frecuencia_media_grupo(&recorre_acentuales->frecuencia);

	    recorre_acentuales->factor_caida = calcula_factor_caida(recorre_acentuales);
	    if (calcula_salto_f0(recorre_acentuales, &recorre_acentuales->salto_f0))
	      return 1;
		
#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
            recorre_acentuales->etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(anterior_frase_separada);

            recorre_acentuales->sintagma_actual = decide_sintagma_grupo_acentual(anterior_frase_separada->tipo_sintagma);

	    // Cambio

            devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, anterior_frase_separada - original_frase_separada,
						      &tipo_siguiente_sintagma);

            recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);
	    //

#ifdef _MINOR_PHRASING_TEMPLATE
	    pos_frontera.etiqueta_morfosintactica = recorre_acentuales->etiqueta_morfosintactica;
	    pos_frontera.tipo_pausa = recorre_acentuales->tipo_pausa;
	    pos_frontera.salto_f0 = recorre_acentuales->salto_f0;
            pos_frontera.factor_caida = recorre_acentuales->factor_caida;

            sintagma_frontera.elementos.push_back(pos_frontera);
            sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
            sintagma_frontera.acento = 1; // sintagma tónico
            
	    if (frase_separada - original_frase_separada > recorre_frase_sintagmada->fin) {
	      // Cambiamos de sintagma:
	      //                sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
	      //                sintagma_frontera.acento = 1; // sintagma tónico
	      grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
	      sintagma_frontera.elementos.clear();
            }

	    if ( (recorre_acentuales->tipo_pausa != SIN_PAUSA) &&
            	 (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_ENTONATIVA) ) { // &&
	      //                 (recorre_acentuales->tipo_pausa != PAUSA_RUPTURA_COMA) ) {

	      if (sintagma_frontera.elementos.size()) {
		//					sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
		grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
		sintagma_frontera.elementos.clear();
	      }

	      grupo_fonico_frontera.tipo_grupo = recorre_acentuales->tipo_grupo_prosodico_3;
	      grupo_fonico_frontera.siguiente_sintagma = recorre_acentuales->siguiente_sintagma;
	      grupo_fonico_frontera.identificador = identificador_GF++;
                
	      if (frase_frontera.elementos.size() == 0)
		grupo_fonico_frontera.posicion = GRUPO_FONICO_INICIAL;
	      else
		grupo_fonico_frontera.posicion = GRUPO_FONICO_MEDIO;

	      frase_frontera.elementos.push_back(grupo_fonico_frontera);
	      grupo_fonico_frontera.elementos.clear();
	    }
#endif

	    /*
	      if (recorre_frase_sintagmada->fin == anterior_frase_separada - original_frase_separada) {

	      devuelve_siguiente_sintagma_no_puntuacion(frase_sintagmada, anterior_frase_separada - original_frase_separada,
	      &tipo_siguiente_sintagma);

	      recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(tipo_siguiente_sintagma);

	      }
	      else {
	      if (recorre_acentuales->sintagma_actual == GA_S_PREPOSICIONAL)
	      recorre_acentuales->siguiente_sintagma = GA_S_NOMINAL;
	      else
	      recorre_acentuales->siguiente_sintagma = recorre_acentuales->sintagma_actual;
	      }
	    */

#endif

	    if (++(*numero_acentuales) >= memoria_reservada) {
	      memoria_reservada += 500;
	      if ( (*acentuales = (Grupo_acentual_candidato *)
		    realloc(*acentuales, memoria_reservada*sizeof(Grupo_acentual_candidato))) == NULL) {
		fprintf(stderr, "Error en calcula_grupos_acentuales, al asignar memoria.\n");
		return 1;
	      }
	      recorre_acentuales = *acentuales + *numero_acentuales - 1;
	    } else
	      recorre_acentuales++;

	    inicio_grupo_acentual = 1;
	  }

	  no_primera = 1;

	  if (inicio_grupo_acentual == 1) {
	    recorre_acentuales->anhade_identificador(identificador++);
	    recorre_acentuales->inicio = fonema_real->inicio;
	    recorre_acentuales->tipo_grupo_prosodico_1 = correcaminos->ti_prop;
	    recorre_acentuales->tipo_grupo_prosodico_2 = objeto_prosodia.adaptar_grupo_prosodico(correcaminos->ti_prop);
	    recorre_acentuales->tipo_grupo_prosodico_3 = halla_tipo_de_frase(correcaminos->ti_prop);
	    if (recorre_acentuales > *acentuales)
	      if ((recorre_acentuales - 1)->tipo_grupo_prosodico_3 == FRASE_INTERROGATIVA)
		recorre_acentuales->tipo_grupo_prosodico_3 = FRASE_INTERROGATIVA;

	    recorre_acentuales->posicion_silaba_acentuada = posicion_acento;
	    recorre_acentuales->posicion_grupo_prosodico = (short int) indice_grupo_fonico;
	    recorre_acentuales->grupos_acentuales_grupo_prosodico = numero_grupos_acentuales;
	    recorre_acentuales->posicion_grupo_acentual = grupo_acentual_actual;
	    recorre_acentuales->numero_silabas = numero_silabas_grupo_acentual;
	    recorre_acentuales->numero_grupos_prosodicos = (short int) grupos_fonicos_total;
	    recorre_acentuales->numero_frase = num_frase;
	    recorre_acentuales->frecuencia.numero_marcas = numero_silabas_grupo_acentual + 2;
	    inicio_grupo_acentual = 0;

	    if ( (recorre_acentuales->frecuencia.marcas = (estructura_marcas_frecuencia *) malloc(
												  recorre_acentuales->frecuencia.numero_marcas*sizeof(estructura_marcas_frecuencia)))
		 == NULL) {
	      fprintf(stderr, "Error en calcula_grupos_acentuales, al asignar memoria.\n");
	      return 1;
	    }
	    recorre_acentuales->frecuencia.marcas[0].tipo_silaba = anterior_tipo_silaba;
#ifdef _INCLUYE_SILABAS
	    recorre_acentuales->frecuencia.marcas[0].silaba[0] = '\0';
#endif
	    recorre_acentuales->frecuencia.marcas[0].tiempo = fonema_real->inicio;
	    recorre_acentuales->frecuencia.marcas[0].frecuencia =
	      interpola(frecuencia_estilizada, fonema_real->inicio,
			&recorre_acentuales->frecuencia.marcas[0].sonoridad_pobre);
	    recorre_marcas = recorre_acentuales->frecuencia.marcas + 1;

	  } // if (inicio_grupo_acentual == ...

	  if ( (caracteres_silaba -= strlen(fonema_real->fonema)) == 0) { // Leemos la siguiente sílaba
	    do {
	      if (silaba[0] == '#')
		strcpy(silaba_antes_de_pausa, silaba_anterior);
	      strcpy(silaba_anterior, silaba);
	      fscanf(fichero_silabas, "%f %s\n", &tiempo_silaba, silaba);
	      numero_silabas++;
	      nueva_silaba = 1;
	    } while (!feof(fichero_silabas) && (silaba[0] == '#') );

	    caracteres_silaba = strlen(silaba);
	    if (silaba[0] != '#') {
	      if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1) {
		fprintf(stderr, "Error en calcula_grupos_acentuales. Sílaba: %s.\n", silaba);
		return 1;
	      }
	      almacena_posicion_vocal_fuerte = posicion_vocal_fuerte_anterior;
	      posicion_vocal_fuerte_anterior = posicion_vocal_fuerte;
	    } // if (silaba[0] != '#')
	  } // if ( (caracteres_silaba -= ...

        } // if (correcaminos->alof[0] != '#'...

        
        tiempo_silencio = fonema_real->inicio;
        fonema_real++;

        numero_lecturas++;

        if (calcula_frecuencia_silaba) {
	  instante_calculo = (fonema_real->inicio + instante_inicial) / 2;
	  recorre_marcas->tiempo = instante_calculo;
	  anterior_tipo_silaba = tipo_silaba;
	  if (nueva_silaba == 1) {
	    if (silaba_anterior[0] != '#') {
	      if (devuelve_tipo_estructura_silaba(silaba_anterior, almacena_posicion_vocal_fuerte, &tipo_silaba)) {
		if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		  fprintf(stderr, "Error en calcula_grupos_acentuales: sílaba (%s) no válida.\n", silaba_anterior);
		  return 1;
		}
		puts("Problemas!!!");

		return 1;
	      }
	    }
	    else {
	      if (devuelve_tipo_estructura_silaba(silaba_antes_de_pausa, almacena_posicion_vocal_fuerte, &tipo_silaba)) {
		if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
		  fprintf(stderr, "Error en calcula_grupos_acentuales: sílaba (%s) no válida.\n", silaba_anterior);
		  return 1;
		}
		puts("Problemas!!!");

		return 1;
	      }

	    }

#ifdef _INCLUYE_SILABAS
	    if (silaba_anterior[0] != '#')
	      strcpy(recorre_marcas->silaba, silaba_anterior);
	    else
	      strcpy(recorre_marcas->silaba, silaba_antes_de_pausa);
#endif
	  }
	  else {
	    if (devuelve_tipo_estructura_silaba(silaba, posicion_vocal_fuerte_anterior, &tipo_silaba))
	      return 1;
#ifdef _INCLUYE_SILABAS
	    strcpy(recorre_marcas->silaba, silaba);
#endif
	  }
	  recorre_marcas->tipo_silaba = tipo_silaba;
	  //            strcpy(silaba_anterior, silaba);
	  nueva_silaba = 0;
	  almacena_posicion_vocal_fuerte = posicion_vocal_fuerte_anterior;
	  recorre_marcas->frecuencia = interpola(frecuencia_estilizada, instante_calculo,
						 &recorre_marcas->sonoridad_pobre);
	  recorre_acentuales->frecuencia.tiempo_inicio = recorre_acentuales->frecuencia.marcas[0].tiempo;
	  recorre_acentuales->frecuencia.tiempo_final =
	    recorre_acentuales->frecuencia.marcas[numero_silabas_grupo_acentual + 1].tiempo;
	  recorre_marcas++;
	  /*
            recorre_acentuales->indice_maximo =
	    calcula_frecuencia_maxima_grupo(&recorre_acentuales->frecuencia);

            recorre_acentuales->frecuencia_media =
	    calcula_frecuencia_media_grupo(&recorre_acentuales->frecuencia);

	    //            recorre_acentuales->factor_caida = calcula_factor_caida(recorre_acentuales);
	    */
	  calcula_frecuencia_silaba = 0;

        }


	if ( (correcaminos + 1)->alof[0] != '\0') {
	  // Esto es por el fichero pau800_86, al que justo le falta el silencio del final
	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '+')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '*')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	  /*
	    if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '!')
            fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';
	  */
	  if (fonema_real->fonema[strlen(fonema_real->fonema) - 1] == '-')
	    fonema_real->fonema[strlen(fonema_real->fonema) - 1] = '\0';

	} // if ( (correcaminos + 1)->alof[0] != '\0')

        correcaminos++;

      } // while (correcaminos->alof[0]...

      if (frase_extraida[strlen(frase_extraida) - 1] == ',')
    	recorre_acentuales->tipo_pausa = PAUSA_COMA;
      else
    	if (frase_extraida[strlen(frase_extraida) - 1] == ':')
	  recorre_acentuales->tipo_pausa = PAUSA_OTRO_SIGNO_PUNTUACION;
        else
	  if (strstr(frase_extraida, "·") == NULL)
	    recorre_acentuales->tipo_pausa = PAUSA_PUNTO;
	  else
	    recorre_acentuales->tipo_pausa = PAUSA_PUNTOS_SUSPENSIVOS;

      recorre_acentuales->tipo_pausa_anterior = tipo_de_pausa_anterior;

      fonema_real--;

      if  (fonema_real->fonema[0] != '#') {
        if ( (fichero = fopen("problemas.txt", "at")) == NULL) {
	  fprintf(stderr, "Error en calcula_grupos_acentuales, \
            al intentar abrir el fichero de problemas.\n");
	  return 1;
        }

        puts("Problemas!!!");
        getchar();
        fprintf(fichero, "Cotovía: %s\tReal: %s\n", correcaminos->alof, fonema_real->fonema);
        fclose(fichero);
      }

      recorre_acentuales->final = fonema_real->inicio;
      recorre_acentuales->frecuencia.tiempo_final = fonema_real->inicio;
      recorre_marcas = &recorre_acentuales->frecuencia.marcas[recorre_acentuales->frecuencia.numero_marcas - 1];

#ifdef _INCLUYE_SILABAS
      recorre_marcas->silaba[0] = '\0';
#endif
      recorre_marcas->tipo_silaba = 0;
      recorre_marcas->tiempo = fonema_real->inicio;
      recorre_marcas->frecuencia = interpola(frecuencia_estilizada, fonema_real->inicio,
					     &recorre_marcas->sonoridad_pobre);
      recorre_acentuales->indice_maximo =
        calcula_frecuencia_maxima_grupo(&recorre_acentuales->frecuencia);

      recorre_acentuales->frecuencia_media =
        calcula_frecuencia_media_grupo(&recorre_acentuales->frecuencia);

      recorre_acentuales->factor_caida = calcula_factor_caida(recorre_acentuales);
      if (calcula_salto_f0(recorre_acentuales, &recorre_acentuales->salto_f0))
	return 1;

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
      recorre_acentuales->etiqueta_morfosintactica = decide_categoria_morfosintactica_grupo_acentual(frase_separada);
      recorre_acentuales->sintagma_actual = decide_sintagma_grupo_acentual(frase_separada->tipo_sintagma);
      while ((frase_separada + 1)->pal[0] != 0)
    	frase_separada++;
      recorre_acentuales->siguiente_sintagma = decide_sintagma_grupo_acentual(frase_separada->tipo_sintagma);
#endif

      (*numero_acentuales)++;

      numero_lecturas--;

      num_frase++;

#ifdef _MINOR_PHRASING_TEMPLATE
      pos_frontera.etiqueta_morfosintactica = recorre_acentuales->etiqueta_morfosintactica;
      pos_frontera.tipo_pausa = recorre_acentuales->tipo_pausa;
      pos_frontera.salto_f0 = recorre_acentuales->salto_f0;
      pos_frontera.factor_caida = recorre_acentuales->factor_caida;
      sintagma_frontera.elementos.push_back(pos_frontera);
      sintagma_frontera.tipo = recorre_acentuales->sintagma_actual;
      sintagma_frontera.acento = 1; // sintagma tónico
      grupo_fonico_frontera.elementos.push_back(sintagma_frontera);
      grupo_fonico_frontera.tipo_grupo = recorre_acentuales->tipo_grupo_prosodico_3;
      grupo_fonico_frontera.siguiente_sintagma = recorre_acentuales->siguiente_sintagma;
      grupo_fonico_frontera.identificador = identificador_GF++;
            
      if (frase_frontera.elementos.size() == 0)
	grupo_fonico_frontera.posicion = GRUPO_FONICO_INICIAL_Y_FINAL;
      else
	grupo_fonico_frontera.posicion = GRUPO_FONICO_FINAL;
                
      frase_frontera.elementos.push_back(grupo_fonico_frontera);
#endif

      if ( (fichero_inicio = fopen("inicio_numero_GA.txt", "wt")) == NULL) {
        fprintf(stderr, "Error en calcula_grupos_acentuales, al intentar abrir el fichero de identificadores.\n");
        return 1;
      }

      fprintf(fichero_inicio, "%d\n", identificador);
      fclose(fichero_inicio);

      if ( (fichero_inicio = fopen("inicio_numero_GF.txt", "wt")) == NULL) {
        fprintf(stderr, "Error en calcula_grupos_acentuales, al intentar abrir el fichero de identificadores de rupturas.\n");
        return 1;
      }

      fprintf(fichero_inicio, "%d\n", identificador_GF);
      fclose(fichero_inicio);

      return 0;

    }

#endif

#if defined(_GRUPOS_ACENTUALES) || defined(_MODO_NORMAL)

    /**
     * Función:   extrae_grupo_acentual
     * \remarks Entrada:
     *			- frase_total: cadena con la frase completa.                         
     *	\remarks Salida:
     *			- alofono_acento: posición del alófono en el que recae el acento.
     *			- tipo_pausa: tipo de pausa en la que empieza el grupo.				 
     * \remarks Valor devuelto:                                                               
     *          - En ausencia de error se devuelve un cero.                          
     * \remarks Objetivo:  Extraer las porciones de frase entre pausas, dejando sólo la infor-
     *            mación relativa a los grupos acentuales.
     */

    int Crea_vectores_descriptor::extrae_grupo_acentual(char *frase_total,
							unsigned char *alofono_acento,
							char *tipo_pausa) {

      char *inicio_token = NULL, *final_token = NULL, *origen_auxiliar;
      char *salida;
      int tamano;
      int contador;

      tamano = strlen(frase_total) + 1;

      if ( (salida = (char *) malloc(tamano)) == NULL) {
        fprintf(stderr, "Error en extrae_grupo_acentual, al asignar memoria.\n");
        return 1;
      }

      origen_auxiliar = salida;


      // Avanzamos hasta el fragmento que nos ocupa.

      if (grupo_prosodico_actual) {
        for (contador = 0; contador < grupo_prosodico_actual; contador++) {
	  //		    inicio_token = frase_total;
	  //   		final_token = frase_total;
	  inicio_token = strstr(frase_total + 2, "%pausa");
	  final_token = strstr(frase_total + 2, "%rupt");
	  if (final_token == NULL) {
	    frase_total = inicio_token;
	  }
	  else
	    if (inicio_token == NULL)
	      frase_total = final_token;
	    else
	      (inicio_token < final_token) ? frase_total = inicio_token:
		frase_total = final_token;
        } // for (contador = 0; ...

        // Comprobamos el tipo de pausa: (es la pausa al inicio del grupo entonativo,
        // así que si no nos referimos al primer grupo acentual, habrá que cambiarla
        // más adelante.

        if (final_token == NULL)
	  *tipo_pausa = PAUSA_COMA;
        else
	  if (inicio_token == NULL) {
	    if (strncmp(final_token, "%ruptura-coma", 13) == 0)
	      *tipo_pausa = PAUSA_RUPTURA_COMA;
	    else
	      *tipo_pausa = PAUSA_RUPTURA_ENTONATIVA;
	  }
	  else
	    if (inicio_token < final_token)
	      *tipo_pausa = PAUSA_COMA;
	    else
	      if (strncmp(final_token, "%ruptura-coma", 13) == 0)
		*tipo_pausa = PAUSA_RUPTURA_COMA;
	      else
		*tipo_pausa = PAUSA_RUPTURA_ENTONATIVA;

      } // if (grupo_prosodico_actual)
      else { // estamos en el primer grupo

    	*tipo_pausa = PAUSA_PUNTO;

      }


      strcpy(salida, frase_total);

      // Buscamos el segundo token de pausa (O de ruptura, claro está)

      inicio_token = strstr(salida + 2, "#%pausa");
      final_token = strstr(salida + 2, "#%rupt");
      if ( (inicio_token == NULL) && (final_token == NULL) ) {
        fprintf(stderr, "Error en extrae_grupo_acentual: formato de frase fonética no válido.\n");
        free(origen_auxiliar);
        return 1;
      }

      if (final_token == NULL)
	salida[inicio_token - salida]= 0;
      else
    	if (inicio_token == NULL)
	  salida[final_token - salida]= 0;
        else
	  (inicio_token < final_token) ? salida[inicio_token - salida]= 0:
	    salida[final_token - salida]= 0;

      // Eliminamos posibles espacios al final:

      tamano = strlen(salida);

      while (salida[tamano - 1] == ' ')
        tamano--;

      salida[tamano] = 0;

      // Ahora eliminamos el primer token de pausa:

      salida = strstr(salida + 1, "#") + 1;

      // Eliminamos posibles espacios al principio:

      while (salida[0] == ' ')
        salida++;

      // Contamos el número de grupos acentuales del grupo prosódico:

      calcula_numero_grupos_acentuales(salida);

      // Y ahora, hasta el grupo acentual:

      if (grupo_acentual_actual) {

    	*tipo_pausa = SIN_PAUSA;
        
        for (contador = 0; contador < grupo_acentual_actual; contador++)
	  salida = strstr(salida + 1, "^");

        // Avanzamos hasta la siguiente palabra:

        if ( (salida = strstr(salida, " ")) == NULL) {
	  fprintf(stderr, "Error en extrae_grupo_acentual: formato de frase fonética no válido.\n");
	  free(origen_auxiliar);
	  return 1;
        }

        while (salida[0] == ' ')
	  salida++;
      }

      // Recortamos hasta el siguiente grupo acentual:

      if (grupo_acentual_actual != numero_grupos_acentuales - 1) {

        if ( (inicio_token = strstr(salida, "^")) != NULL) {// No es el último grupo acentual
	  if ( (final_token = strstr(inicio_token, " ")) != NULL)
	    salida[final_token - salida] = 0;
	  else
	    salida[inicio_token - salida] = 0;
        }
      }

      // Calculamos la posición silábica del último acento.

      posicion_acento = calcula_posicion_acento(salida, alofono_acento);

      // Contamos el número de elementos significativos (es decir, todos menos ' ', '-' y '^').

      numero_elementos_grupo_acentual = 0;
      inicio_token = salida;
      final_token = salida + strlen(salida);

      while (inicio_token < final_token)
        if ( (tamano = strcspn(inicio_token, "^ -")) != 0) {
	  numero_elementos_grupo_acentual += (short int) tamano;
	  inicio_token += tamano + 1;
        }
        else
	  inicio_token++;

      // Calculamos el número de sílabas:

      inicio_token = final_token = salida;
      numero_silabas_grupo_acentual = 1;

      // Contamos el número de guiones:

      while ( (inicio_token = strstr(inicio_token + 1, "-")) != NULL)
        numero_silabas_grupo_acentual++;

      // Y ahora, el número de espacios:

      while ( (final_token = strstr(final_token + 1, " ")) != NULL)
        numero_silabas_grupo_acentual++;


      free(origen_auxiliar);
    
      return 0;

    }


    /**
     * Función:   calcula_numero_grupos_prosodicos
     * \remarks Entrada:
     *			- frase_total: cadena con la frase completa.                         
     * \remarks Objetivo:  Calcula el número de grupos entre pausas existentes en la frase
     *            se le pasa como parámetro.
     * \remarks NOTA:      Devuelve el valor en la variable global del módulo
     *            grupos_prosodicos_total.                                           
     * NOTA 2:	  A partir de ahora también se consideran las rupturas prosódicas no
     *			  coincidentes con pausas.											 	
     */

    void Crea_vectores_descriptor::calcula_numero_grupos_prosodicos(char *frase_total) {

      char *inicio_token, *final_token;

      inicio_token = final_token = frase_total;

      grupos_prosodicos_total = 0;

      // Primero las pausas:

      while ( (inicio_token = strstr(inicio_token, "%pausa")) != NULL) {
        inicio_token++;
        if (inicio_token - final_token > 14) // Para evitar dos tokens seguidos.
	  grupos_prosodicos_total++;
        final_token = inicio_token;
      }

      grupos_fonicos_total = grupos_prosodicos_total;

      // Ahora las rupturas no relacionadas con pausas:

      inicio_token = final_token = frase_total;

      while ( (inicio_token = strstr(inicio_token, "%ruptu")) != NULL) {
        inicio_token++;
        if (inicio_token - final_token > 14) // Para evitar dos tokens seguidos.
	  grupos_prosodicos_total++;
        final_token = inicio_token;
      }

    }


    /**
     * Función:   calcula_numero_grupos_acentuales
     * \remarks Entrada:
     *			- grupo_prosodico: cadena con un grupo entre pausas.                 
     * \remarks Objetivo:  Calcular el número de grupos acentuales contenidos en el grupo
     *            prosódico.
     * \remarks NOTA:      El valor se devuelve en la variable global del módulo
     *            numero_grupos_acentuales.                                          
     */

    void Crea_vectores_descriptor::calcula_numero_grupos_acentuales(char *grupo_prosodico) {

      char *inicio_token, *final_token;

      inicio_token = final_token = grupo_prosodico;

      numero_grupos_acentuales = 0;

      while ( (inicio_token = strstr(inicio_token, "^")) != NULL) {
        final_token = inicio_token++;
        numero_grupos_acentuales++;
      }

#ifndef _CONSIDERA_GRUPOS_HUERFANOS
      // Ahora comprobamos que la última palabra sea tónica.

      if (strstr(final_token, " ") != NULL)
        numero_grupos_acentuales++;
#endif

      if (numero_grupos_acentuales == 0)
        numero_grupos_acentuales = 1;

    }


    /**
     * Función:   fin_grupo_acentual
     * \remarks Entrada:
     *			- frase: cadena con la frase completa.                               
     *          - Alofono: alófono actual.
     *	\remarks Salida:
     *			- nuevo_grupo: variable que indica si se ha llegado al fin de un     
     *            grupo acentual (1) o no (0).                                       
     *          - alofono_acento: vale 0 si sobre el alófono actual recae el acento,
     *            y 1 en caso contrario.                                             
     *          - tipo_de_pausa: tipo de pausa en la que termina el grupo acentual.  
     * \remarks Valor devuelto:                                                               
     *          - En ausencia de error se devuelve un cero.                          
     * \remarks Objetivo:  Avisar al programa llamante del final grupo acentual, además de
     *            calcular el siguiente, obteniendo los valores precisos para el     
     *            objeto de tipo Grupo_acentual_candidato que lo caracteriza.        
     */

    int Crea_vectores_descriptor::fin_grupo_acentual(char *frase, char *alofono, char *nuevo_grupo, char *alofono_acento, char *tipo_de_pausa) {

      static unsigned char alofonos_hasta_tonica = 0;

      *tipo_de_pausa = SIN_PAUSA;

      if (numero_elementos_grupo_acentual) {
        numero_elementos_grupo_acentual -= (short int) strlen(alofono);
        alofonos_hasta_tonica -= (unsigned char) strlen(alofono);
        if (alofonos_hasta_tonica)
	  *alofono_acento = 1;
        else
	  *alofono_acento = 0;

        *nuevo_grupo = 0;

        return 0;
      }

      alofonos_hasta_tonica = 0;

      if (grupos_prosodicos_total == -1) {
        calcula_numero_grupos_prosodicos(frase);
        grupo_acentual_actual = 0;
        grupo_prosodico_actual = 0;
      }
      else
        if (++grupo_acentual_actual >= numero_grupos_acentuales) {
	  grupo_acentual_actual = 0;
	  grupo_prosodico_actual++;
	  //            *tipo_de_pausa = PAUSA_COMA;
        }


      if (extrae_grupo_acentual(frase, &alofonos_hasta_tonica, tipo_de_pausa))
        return 1;

      numero_elementos_grupo_acentual -= (short int) strlen(alofono);
      alofonos_hasta_tonica -= (unsigned char) strlen(alofono);
      if (alofonos_hasta_tonica == 0)
        *alofono_acento = 0;
      else
        *alofono_acento = 1;

      *nuevo_grupo = 1;

      return 0;
    
    }


    /**
     * Función:   calcula_posicion_acento
     * \remarks Entrada:
     *			- grupo_acentual: conjunto de palabras terminadas en una tónica.
     *	\remarks Salida:
     *			- alofono_acento: posición del alófono sobre el que cae el acento.
     * \remarks Valor devuelto:                                                               
     *          - La posición del acento, según los siguientes valores:                             * 3: grupo huérfano (sin acento léxico).
     *              * 0: palabra aguda.                                              
     *              * 1: palabra grave.                                              
     *              * 2: resto de los casos.                                         
     * \remarks Objetivo:  devuelve, respecto al final, la posición de la sílaba sobre la que
     *            recae el acento.                                                       
     */

    char Crea_vectores_descriptor::calcula_posicion_acento(char *grupo_acentual,
							   unsigned char *alofono_acento) {

      unsigned char pos_alofono;
      unsigned char longitud;
      char *pos_acento, *inicio_token, *final_token, auxiliar[400];

      posicion_acento = GRUPO_AGUDO;

      if ( (pos_acento = strstr(grupo_acentual, "^")) == NULL) {
	// Buscamos la última sílaba:
	final_token = grupo_acentual;
	inicio_token = grupo_acentual;
	while ( (inicio_token = strstr(inicio_token, "-")) != NULL) {
	  inicio_token++;
	  final_token = inicio_token;
	}
	*alofono_acento = encuentra_vocal_fuerte(final_token);
#ifndef _CONSIDERA_GRUPOS_HUERFANOS
	return GRUPO_HUERFANO; // Indicamos que es un grupo huérfano.
#else
	return GRUPO_LLANO; //AGUDO;
#endif
        // if ( (pos_alofono = (unsigned char) strlen(grupo_acentual)) < 2)
	//   *alofono_acento = 0;
        // else
	//   *alofono_acento = (unsigned char) (pos_alofono - 2);

        // return 1; // Suponemos que el acento recae en la penúltima sílaba.
      }

      longitud = (unsigned char) (pos_acento - grupo_acentual);

      memcpy(auxiliar, grupo_acentual, longitud);

      auxiliar[longitud - 1] = '\0';

      inicio_token = auxiliar;

      while ( (inicio_token = strstr(inicio_token, "-")) != NULL) {
        inicio_token++;
        longitud--;
      }

      inicio_token = auxiliar;

      while ( (inicio_token = strstr(inicio_token, " ")) != NULL) {
        inicio_token++;
        longitud--;
      }

      *alofono_acento = longitud;

      // Ahora contamos el número de sílabas que hay después.

      inicio_token = pos_acento;
      while ( (pos_acento = strstr(pos_acento, "-")) != NULL) {
        posicion_acento++;
        pos_acento++;
      }

      // Por si el grupo acaba en palabra átona:

      while ( (inicio_token = strstr(inicio_token, " ")) != NULL) {
	inicio_token++;
	posicion_acento++;
      }

      if (posicion_acento > 2)
        return GRUPO_ESDRUJULO;

      return posicion_acento;

    }


    /**
     * Función:   reinicia_grupos_acentuales
     * \remarks Objetivo:  reiniciar las variables del módulo para su correcto funcionamiento.
     */

    void Crea_vectores_descriptor::reinicia_grupos_acentuales() {

      numero_elementos_grupo_acentual = 0;
      numero_silabas_grupo_acentual = 0;
      numero_grupos_acentuales = 0;
      grupos_prosodicos_total = -1;
    }


    /**
     * Función:   encuentra_tipo_siguiente_pausa
     * \remarks Entrada:
     *			- recorre_frase_separada: puntero a la cadena de estructuras de tipo 
     *           t_frase_separada, en la que se encuentra la información de cada
     *           elemento de la frase.                                               
     * \remarks Valor devuelto:                                                               
     *           - El tipo de pausa, escogido entre los siguientes:                  
     *              * PAUSA_PUNTO.                                                   
     *              * PAUSA_COMA.                                                    
     *              * PAUSA_PUNTOS_SUSPENSIVOS.                                      
     * \remarks NOTA:    Quizás sea necesario añadir en algún momento otros tipos de pausa,
     *          pero los datos de duración del Cospus que tenemos actualmente no nos
     *          lo permiten.                                                         
     */

    char encuentra_tipo_siguiente_pausa(t_frase_separada *recorre_frase_separada) {

      unsigned char clase_pal;

      if (recorre_frase_separada->pal[0] == '\0')
        return PAUSA_PUNTO;

      recorre_frase_separada++;

      if (recorre_frase_separada->pal[0] == '\0')
        return PAUSA_PUNTO;

      clase_pal = recorre_frase_separada->cat_gramatical[0];

      while ( (clase_pal < PUNTO) || (clase_pal > GUION_BAIXO) ) { 
        recorre_frase_separada++;
        if (recorre_frase_separada->pal[0] == '\0')
	  return PAUSA_PUNTO;
        clase_pal = recorre_frase_separada->cat_gramatical[0];
      } // while (clase_pal

      switch (clase_pal) {

      case COMA:
        return PAUSA_COMA;
      case  PUNTO:
      case  DOUS_PUNTOS:
      case  PUNTO_E_COMA:
        return PAUSA_PUNTO;
      case PUNTOS_SUSPENSIVOS:
        return PAUSA_PUNTOS_SUSPENSIVOS;
      default:
        if ( (recorre_frase_separada + 1)->pal[0] == '\0')
	  return PAUSA_PUNTO;
        else
	  return PAUSA_COMA;
      } // switch

    }




    /**
     * Función:   crea_nombre_fichero_grupo
     * \remarks Entrada:
     *			- nombre_entrada: nombre del fichero de texto de entrada.              
     *	\remarks Salida:
     *			- nombre_salida: nombre del fichero de salida, sin la ruta.            
     * \remarks Objetivo:  Extraer el nombre del fichero en el que se encuentra la unidad,      
     *            eliminando la ruta, para que ésta no tenga por qué ser fija.
     */

    void Crea_vectores_descriptor::crea_nombre_fichero_grupo(char *nombre_entrada, char *nombre_salida) {

      char delimitador[2];
      char *auxiliar = nombre_entrada, *anterior = nombre_entrada;

      if (strstr(nombre_entrada, "/"))
    	strcpy(delimitador, "/");
      else
    	strcpy(delimitador, "\\");


#ifdef _WIN32
      while ( (auxiliar = strstr(auxiliar + 1, delimitador)) != NULL)
        anterior = auxiliar;
#else
      while ( (auxiliar = strstr(auxiliar + 1, "/")) != NULL)
        anterior = auxiliar;
#endif

      strcat(nombre_salida, anterior);

      auxiliar = strstr(nombre_salida, ".txt");

      strcpy(auxiliar, ".grp");

    }

    /**
     * \author	Fran Campillo
     * \remarks	Función que devuelve el tipo de sílaba según los fonemas que la componen. Esta función
     * sólo es una primera versión experimental, deberíamos hacer pruebas con ella. De momento sólo se
     * comprueba si hay alguna oclusiva en la coda de la sílaba.
     * \param	silaba: cadena de fonemas de la sílaba.
     * \param	posicion_vocal_fuerte: índice de la vocal fuerte. Ojo, en fonemas. (es una
     * gracia, pero hay dos fonemas (tS y rr) que se representan con dos caracteres).
     * \retval	tipo_silaba: el tipo de sílaba.
     * \return	En ausencia de error devuelve un cero.
     */

    int Crea_vectores_descriptor::devuelve_tipo_estructura_silaba(char *silaba, int posicion_vocal_fuerte, char *tipo_silaba) {

      int contador, tamano_silaba;
      char fonema[OCTETOS_POR_FONEMA];
      char tipo_silaba_local = 0;
      char *apunta_fonema = silaba;
      //      clase_espectral clase_fonema;

      tamano_silaba = strlen(silaba);

      for (contador = 0; contador < tamano_silaba; contador++) {

#ifdef _MODOA_EU
        if  ( (strncmp(apunta_fonema, "ts`", 3) == 0)){
	  strncpy(fonema, apunta_fonema, 3);
          fonema[3] = '\0';
          apunta_fonema += 3;
          contador+=2;
        } 
	else { 
#endif
	  if ( (strncmp(apunta_fonema, "tS", 2) == 0) 
#ifdef _MODOA_EU
	       || (strncmp(apunta_fonema, "ts", 2) == 0)
	       || (strncmp(apunta_fonema, "s`", 2) == 0)
	       || (strncmp(apunta_fonema, "gj", 2) == 0)
	       || (strncmp(apunta_fonema, "jj", 2) == 0)
#endif
	       || (strncmp(apunta_fonema, "rr", 2) == 0)) {
	    strncpy(fonema, apunta_fonema, 2);
	    fonema[2] = '\0';
	    apunta_fonema += 2;
	    contador++;
	  } // if ( (strncmp...
	  else {
	    fonema[0] = *apunta_fonema;
	    fonema[1] = '\0';
	    apunta_fonema++;
	  }
#ifdef _MODOA_EU
	}
#endif
        if (fonema[0] == '^') { // Si es un acento...
	  continue; // No es necesario hacer nada más porque en ese caso ya
        } //if (fonema[0] == '^') // estaremos en la coda.

        // if (tipo_fon_espectral_enumerado(fonema, &clase_fonema))
	//   return 1;

	if (sordo_sonoro(fonema) == SORDO) {
	  //        if (clase_fonema == OCLU_SOR_ESP) {
	  if (contador <= posicion_vocal_fuerte)
	    tipo_silaba_local = 1;
	  else {
	    tipo_silaba_local |= 2;
	    break;
	  } // else
        }

      } // for (contador = 0;...

      if (tipo_silaba_local == 0)
    	*tipo_silaba = RESTO_SILABAS;
      else
    	if (tipo_silaba_local == 1)
	  //	  *tipo_silaba = OCLUSIVAS_SORDAS_CODA;
	  *tipo_silaba = SORDAS_CODA;
        else
	  if (tipo_silaba_local == 2)
	    //	    *tipo_silaba = OCLUSIVAS_SORDAS_ONSET;
	    *tipo_silaba = SORDAS_ONSET;
	  else
	    //	    *tipo_silaba = OCLUSIVAS_SORDAS_CODA_Y_ONSET;
	    *tipo_silaba = SORDAS_CODA_Y_ONSET;

      return 0;

    }


#endif

    /**
     * \author	Fran Campillo
     * \remarks	Función que calcula las características de una parte de la sílaba que se le envía como
     * parámetro (el onset o la coda, por ejemplo).
     * \param	parte_silaba: parte de la sílaba.
     * \retval	caracteristica: clasificación de esa parte de la sílaba.
     * \return	En ausencia de error devuelve un cero.
     */

    int devuelve_caracteristicas_parte_silaba(char *parte_silaba, char *caracteristica) {

      char fonema[OCTETOS_POR_FONEMA] = "\0\0";
      char *apunta_fonema = parte_silaba;
      //    char fonema_sordo = 0;
      char oclusiva_sorda = 0;
      char fonema_sonoro = 0;
      clase_espectral clase_fonema;

      if (*parte_silaba == '\0') {
	//    	*caracteristica = VACIO;
	*caracteristica = PARTE_SONORA;
	return 0;
      }

      for (; *apunta_fonema != '\0'; apunta_fonema++) {

    	if ( (strcmp(apunta_fonema, "tS") == 0) ||
	     (strcmp(apunta_fonema, "rr") == 0) ){
	  strncpy(fonema, apunta_fonema, 2);
	  apunta_fonema++;
        }
        else {
	  fonema[0] = *apunta_fonema;
	  fonema[1] = '\0';
        }

        if (tipo_fon_espectral_enumerado(fonema, &clase_fonema))
	  return 1;

        if (clase_fonema == OCLU_SOR_ESP)
	  oclusiva_sorda = 1;
        else
	  if (sordo_sonoro(fonema) == SONORO)
	    fonema_sonoro = 1;
	//        	else
	//        		fonema_sordo = 1;


      } // for

      if (oclusiva_sorda)
    	*caracteristica = PARTE_OCLUSIVA;
      else
    	if (fonema_sonoro)
	  *caracteristica = PARTE_SONORA;
        else
	  *caracteristica = PARTE_SORDA;
            
      /*
	if (oclusiva_sorda)
    	*caracteristica = PARTE_OCLUSIVA;
	else
    	if (fonema_sordo)
	*caracteristica = PARTE_SORDA;
        else
	*caracteristica = PARTE_SONORA;


	if (fonema_sonoro == 1) {
        if (oclusiva_sorda == 1)
	*caracteristica = PARTE_OCLUSIVA_Y_SONORA;
	else
	if (fonema_sordo == 1)
	*caracteristica = PARTE_SONORA_Y_SORDA;
	else
	*caracteristica = PARTE_SONORA;
	} // if (fonema_sonoro == 1)
	else {
     	if (oclusiva_sorda == 1) {
	if (fonema_sordo == 1)
	*caracteristica = PARTE_OCLUSIVA_Y_SORDA;
	else
	*caracteristica = PARTE_OCLUSIVA;
        }
        else
	if (fonema_sordo == 1)
	*caracteristica = PARTE_SORDA;
	else {
	fprintf(stderr, "Error en devuelve_caracteristicas_parte_silaba: sílaba (%s) no contemplada.\n",
	parte_silaba);
	return 1;
	} // else
	} // else  (fonema_sonoro == 0)
      */

      return 0;

    }


    /**
     * \author	Fran Campillo
     * \remarks	Función que devuelve el onset de la sílaba que se le pasa como parámetro.
     * \param	silaba: sílaba.
     * \retval	onset: onset de la sílaba.
     * \return	En ausencia de error devuelve un cero.
     */

    int devuelve_onset(char *silaba, char *onset) {

      int posicion_vocal_fuerte;
      char anterior_fonema[2] = "\0";
      clase_espectral tipo_fonema;

      if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1)
    	return 1;

      if (posicion_vocal_fuerte == 0) {
    	*onset = '\0';
        return 0;
      }

      anterior_fonema[0] = silaba[posicion_vocal_fuerte - 1];

      if (tipo_fon_espectral_enumerado(anterior_fonema, &tipo_fonema))
    	return 1;

      if ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) ) {
    	strncpy(onset, silaba, posicion_vocal_fuerte - 1);
        onset[posicion_vocal_fuerte - 1] = '\0';
      }
      else {
    	strncpy(onset, silaba, posicion_vocal_fuerte);
        onset[posicion_vocal_fuerte] = '\0';
      }

      return 0;

    }

    /**
     * \author	Fran Campillo
     * \remarks	Función que devuelve la coda de la sílaba que se le pasa como parámetro.
     * \param	silaba: sílaba.
     * \retval	coda: coda de la sílaba.
     * \return	En ausencia de error devuelve un cero.
     */

    int devuelve_coda(char *silaba, char *coda) {

      int posicion_vocal_fuerte;
      char siguiente_fonema[2] = "\0";
      clase_espectral tipo_fonema;

      if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1)
    	return 1;

      if (posicion_vocal_fuerte == (int) strlen(silaba) - 1) {
    	*coda = '\0';
        return 0;
      }

      siguiente_fonema[0] = silaba[posicion_vocal_fuerte + 1];

      if (tipo_fon_espectral_enumerado(siguiente_fonema, &tipo_fonema))
    	return 1;

      if ( (tipo_fonema == VOC_AB_ESP) || (tipo_fonema == VOC_MED_ESP) || (tipo_fonema == VOC_CER_ESP) ) {
    	strcpy(coda, silaba + posicion_vocal_fuerte + 2);
      }
      else {
    	strcpy(coda, silaba + posicion_vocal_fuerte + 1);
      }

      return 0;

    }

    /**
     * \author	Fran Campillo
     * \remarks	Función que devuelve el tipo de vocal fuerte de la sílaba.
     * \param	silaba: sílaba.
     * \retval	tipo_vocal: coda de la sílaba.
     * \return	En ausencia de error devuelve un cero.
     */

    int devuelve_tipo_vocal_fuerte(char *silaba, clase_espectral *tipo_vocal) {

      int posicion_vocal_fuerte;
      char vocal[2] = "\0";

      if ( (posicion_vocal_fuerte = encuentra_vocal_fuerte(silaba)) == -1)
    	return 1;

      vocal[0] = silaba[posicion_vocal_fuerte];

      return tipo_fon_espectral_enumerado(vocal, tipo_vocal);

    }


    /**
     * Función:   tipo_vocal
     * \remarks Entrada:
     *			- vocal: carácter perteneciente a una sílaba.
     * \remarks Valor devuelto:                                                               
     *          - Si es una vocal fuerte: 1.                                         
     *          - Si es débil: 2.
     *          - Otro caso: 0.                                                      
     */

    inline int tipo_vocal(char vocal) {

      if ( (vocal == 'a') || (vocal == 'e') || (vocal == 'E') || (vocal == 'o') ||
	   (vocal == 'O') )
        return 1;

      if ( (vocal == 'i') || (vocal == 'u') )
        return 2;

      return 0;

    }


    /**
     * Función:   encuentra_vocal_fuerte
     * \remarks Entrada:
     *			- silaba: cadena con los caracteres (alófonos), que forman la sílaba.
     * \remarks Valor devuelto:                                                               
     *          - La posición de la vocal fuerte, y -1 en caso de error.
     */

    int encuentra_vocal_fuerte(char *silaba) {

      int contador = 0;
      char *caracter = silaba;
      int resultado1, resultado2;

      int tamano = strlen(silaba);

      if (strstr(silaba, "^"))
        tamano--;

      for (; contador < tamano; contador++, caracter++) {
        if (*caracter == '^') {
	  contador--;
	  continue;
        }
        switch (tipo_vocal(*caracter)) {

        case 0:
	  continue;
        case 1:
	  return contador;
        case 2:
	  if (contador == tamano - 1) // es la única vocal de la sílaba.
	    return contador;
	  if (contador == tamano - 2) { // Tenemos que ver cuál es el siguiente alófono.
	    switch (tipo_vocal(*(caracter + 1))) {
	    case 0: // Si la siguiente es consonante...
	    case 2: // o si es otra vocal débil
	      return contador; // Nos quedamos con la actual.
	    case 1: // Si la siguiente es fuerte...
	      return contador + 1;
	    } // switch (tipo_vocal(caracter + 1...
	  }
	  if (contador < tamano - 2) {
	    resultado1 = tipo_vocal(*(caracter + 1));
	    resultado2 = tipo_vocal(*(caracter + 2));
	    if (resultado1 == 1) // Si la siguiente es una vocal fuerte...
	      return contador + 1;
	    if (resultado1 == 0) // Si la siguiente es una consonante...
	      return contador;
	    // es una vocal débil, tenemos que ver si hay triptongo
	    if (resultado2 == 2) // triptongo
	      return contador + 1; // Suponemos que la fuerte es la del medio.
	    if (resultado2 == 0) // Voc.débil + Voc.débil + consonante
	      return contador;
	    return contador + 2; // No sé si este caso es imposible.
	  }

        } // switch (tipo_vocal...

      } // for (;...

      return -1;

    }


    /**
     * Función:   decide_categoria_morfosintactica_grupo_acentual
     * \remarks Entrada:
     *			- frase_separada: elemento de la cadena con la información sobre
     *			  las palabras de la frase.										   
     * \remarks Valor devuelto:                                                             
     *          - La categoría que le corresponde dentro de la clasificación de
     *			  los grupos acentuales.			    	                       
     * \remarks Objetivo:  Realizar la conversión entre las categorías generales y las de
     *			  los grupos acentuales.										   
     * \remarks NOTA:	  Ver función Trat_fon::tonica para comprobar las categorías que
     *			  consideran tónicas.
     */

    char decide_categoria_morfosintactica_grupo_acentual(t_frase_separada *frase_separada) {

      unsigned char categoria = frase_separada->cat_gramatical[0];

      if ( (adverbio(categoria) || categoria == XERUNDIO) || ( (categoria >= LOC_PREP) && (categoria <= LOC_PREP_CONDI) ))
    	return GA_ADVERBIO;


      switch (categoria) {

      case NOME:
      case NOME_PROPIO:
      case INFINITIVO:
	return GA_NOMBRE;
      case ADXECTIVO:
      case PARTICIPIO:
      case NUME_CARDI_DET: // Pongo aquí estos dos casos.
      case NUME_ORDI_DET:
	return GA_ADJETIVO;
      case VERBO:
      case PERIFRASE:
	return GA_VERBO;
      case INTER:
      case EXCLA:
	return GA_INTERROGATIVO_EXCLAMATIVO;
      case INDEF_PRON:
      case CONTR_PREP_INDEF_PRON:
      case CONTR_DEMO_INDEF_PRON:
      case CONTR_PREP_DEMO_INDEF_PRON:
      case NUME_PRON:
      case NUME_CARDI_PRON:
      case NUME_ORDI_PRON:
      case NUME_PARTI_PRON:
      case NUME_MULTI:
      case NUME_COLECT:
      case PRON_PERS_TON:
      case CONTR_PREP_PRON_PERS_TON:
      case POSE_PRON:
      case DEMO_PRON:
      case CONTR_PREP_DEMO_PRON:
	return GA_PRONOMBRE;
      case INDEF_DET:
      case POSE_DET:
      case DEMO_DET:
      case CONTR_PREP_ART_INDET:
      case CONTR_PREP_ART_DET:
      case CONTR_PREP_INDEF_DET:
      case CONTR_PREP_DEMO_DET:
      case CONTR_PREP_DEMO_INDEF_DET:
      case CONTR_INDEF_ART_DET:
      case CONTR_DEMO_INDEF_DET:
      case ART_INDET: // En el corpus hay algún caso ("un", quizás sea indef_det)
	return GA_DETERMINANTE;
      default:
	return GA_OTROS;
      } // switch

    }

    /**
     * Función:   decide_categoria_sintagmatica_grupo_acentual
     * \remarks Entrada:
     *			- tipo_sintagma: tipo de sintagma, según la clasificación de tip_var.hpp.
     * \remarks Valor devuelto:
     *          - La categoría que le corresponde dentro de la clasificación de
     *			  los grupos acentuales.
     * \remarks Objetivo:  Realizar la conversión entre los sintagmas generales y los de
     *			  los grupos acentuales.
     */

    char decide_sintagma_grupo_acentual(unsigned char tipo_sintagma) {

      if ( ((tipo_sintagma >= CONX_SUBOR) && (tipo_sintagma <= CONX_SUBOR_CONTI)) ||
	   ((tipo_sintagma >= LOC_CONX_SUBOR_CAUS) && (tipo_sintagma <= LOC_CONX_SUBOR_CORREL)) )
	return GA_CONJ_SUBOR;

      switch (tipo_sintagma) {

      case SINTAGMA_NOMINAL:
	return GA_S_NOMINAL;
      case SINTAGMA_ADXECTIVAL:
	return GA_S_ADJETIVAL;
      case SINTAGMA_VERBAL:
	return GA_S_VERBAL;
      case SINTAGMA_ADVERBIAL:
	return GA_S_ADVERBIAL;
      case SINTAGMA_PREPOSICIONAL:
	return GA_S_PREPOSICIONAL;
      case NEXO_SUBORDINANTE:
	return GA_RELATIVO;
      case CONX_COOR_ADVERS:
      case LOC_CONX_COOR_ADVERS:
	return GA_CONJ_COOR_ADVERS;
      case CONX_COOR_COPU:
      case LOC_CONX_COOR_COPU:
	return GA_CONJ_COOR_COPU;
      case CONX_COOR_DISX:
	return GA_CONJ_COOR_DIS;
      case RUPTURA_ENTONATIVA:
	return GA_RUPTURA_ENTONATIVA;
      case RUPTURA_COMA:
	return GA_RUPTURA_COMA;
      case PUNTO:
      case PUNTO_E_COMA:
      case DOUS_PUNTOS:
      case PUNTOS_SUSPENSIVOS:
	return GA_PUNTO;
      case COMA:
	return GA_COMA;
      case APERTURA_PARENTESE:
	return GA_APERTURA_PAR;
      case PECHE_PARENTESE:
	return GA_CIERRE_PAR;
      case APERTURA_INTERROGACION:
      case APERTURA_EXCLAMACION:
	return GA_APERTURA_INT_EXCLA;
      case PECHE_INTERROGACION:
      case PECHE_EXCLAMACION:
	return GA_PUNTO;
	//        	return GA_CIERRE_INT_EXCLA;
      case SINTAGMA_NON_CLASIFICADO:
	return GA_NO_CLASIFICADO;
      case DOBLES_COMINNAS:
	return GA_DOBLES_COMILLAS;
      default:
	return GA_OTROS_SINTAGMAS;
      } // switch

    }

    /**
     * \author	Fran Campillo.
     * \remarks	Función que devuelve el siguiente sintagma, sin considerar aquellos que se corresponden
     * con signos de puntuación (a excepción del correspondiente con el punto final).
     * \param	sintagma_actual: puntero a frase_sintagmada.
     * \param	indice_palabra_actual: índice de la palabra actual dentro de la cadena frase_separada.
     * \retval	tipo_sintagma: tipo de sintagma que sigue al actual.
     */

    void devuelve_siguiente_sintagma_no_puntuacion(t_frase_sintagmada *sintagma_actual, int indice_palabra_actual,
						   unsigned char *tipo_sintagma) {

      while ( (sintagma_actual->inicio <= indice_palabra_actual) && (sintagma_actual->tipo_sintagma != 0) )
	sintagma_actual++;

      // Nuevo
      if ( (sintagma_actual - 1)->fin > indice_palabra_actual) {
    	// Devolvemos el actual
	*tipo_sintagma = (sintagma_actual - 1)->tipo_sintagma;
        if (*tipo_sintagma == SINTAGMA_PREPOSICIONAL) {
	  *tipo_sintagma = SINTAGMA_NOMINAL;
	  return;
        }
        else
	  if ( (*tipo_sintagma < CONX_SUBOR) ||
	       (*tipo_sintagma > LOC_CONX_SUBOR_CORREL) )
	    return;
      }
      // \Nuevo

      while (sintagma_actual->tipo_sintagma != 0)
    	if ( (sintagma_actual->tipo_sintagma < SIGNO) || (sintagma_actual->tipo_sintagma > RUPTURA_COMA) ) {
	  *tipo_sintagma = sintagma_actual->tipo_sintagma;
	  return;
        }
        else
	  sintagma_actual++;

      *tipo_sintagma = (sintagma_actual - 1)->tipo_sintagma;

    }

    int cuenta_elementos_tonicos_sintagma(t_frase_sintagmada *frase_sintagmada, t_frase_separada *frase_separada_inicio,
					  t_frase_separada *frase_separada) {

      int contador;
      int elementos_tonicos = 0;
      int indice_palabra_actual = frase_separada - frase_separada_inicio;
      t_frase_separada *apunta_frase;
      char *apunta_palabra_sil;
      while ( (frase_sintagmada->inicio <= indice_palabra_actual) && (frase_sintagmada->tipo_sintagma != 0) )
	frase_sintagmada++;

      frase_sintagmada--;

      for (contador = frase_sintagmada->inicio; contador <= frase_sintagmada->fin; contador++) {
        apunta_frase = frase_separada_inicio + contador;
	//    	if (apunta_frase->tonicidad_forzada == 1) {
	apunta_palabra_sil = apunta_frase->pal_sil_e_acentuada;
	while ( (apunta_palabra_sil = strstr(apunta_palabra_sil, "^")) != NULL) {
	  elementos_tonicos++;
	  apunta_palabra_sil++;
	}
	//        }
      }

      return elementos_tonicos;

    }
