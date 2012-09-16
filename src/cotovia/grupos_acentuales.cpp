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
 

#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "modos.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "crea_descriptores.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "utilidades.hpp"
#include "locutor.hpp"
#include "info_estructuras.hpp"


static estructura_grupos_acentuales **tabla_grupos;

/**
 * Función:   Constructor                                                             
 * Clase:     Grupo_acentual_candidato                                                
 */

Grupo_acentual_candidato::Grupo_acentual_candidato() {

  frecuencia.marcas = NULL;
  //    fichero_origen = NULL;
    
}


/**
 * Función:   devuelve_identificador                                                  
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Valor devuelto:                                                                    
 *          - Devuelve el identificador de la unidad.                                 
 */

int Grupo_acentual_candidato::devuelve_identificador() {

  return identificador;

}

/**
 * \class	Grupo_acentual_candidato
 * \brief	sorts a sequence of candidate contours according to their costs
 * \param	contornos_escogidos	array of candidate contours
 * \param[in]	numero_contornos_actuales	number of candidate contours
 * \param[in]	costes_minimos	array of costs
 * \param	relacion_original	relation between the original array and the resulting one
 */
void Grupo_acentual_candidato::reorganiza_caminos_segun_costes(Grupo_acentual_candidato **contornos_escogidos,
							       int numero_contornos_actuales, float *costes_minimos, int *relacion_original) {

  int contador, cuenta;
  int relacion;
  float coste;
  Grupo_acentual_candidato *correcaminos;

  for (contador = 1; contador < numero_contornos_actuales; contador++) {

    if (costes_minimos[contador] < costes_minimos[contador - 1]) {
      // hay que reordenar

      for (cuenta = contador; cuenta > 0; cuenta--)
	if (costes_minimos[cuenta] < costes_minimos[cuenta - 1]) {

	  coste = costes_minimos[cuenta - 1];
	  costes_minimos[cuenta - 1] = costes_minimos[cuenta];
	  costes_minimos[cuenta] = coste;

	  correcaminos = contornos_escogidos[cuenta - 1];
	  contornos_escogidos[cuenta - 1] = contornos_escogidos[cuenta];
	  contornos_escogidos[cuenta] = correcaminos;

	  relacion = relacion_original[cuenta - 1];
	  relacion_original[cuenta - 1] = relacion_original[cuenta];
	  relacion_original[cuenta] = relacion;

	} // if (costes_minimos[cuenta]

    } // if (costes_minimos[contador] < ...

  } // for (contador = 0; contador < numero_contornos_actuales; ...

}


/**
 * \class	Grupo_acentual_candidato
 * \brief	selects a subset of candidate contours according to some criterium, in order to give more options in the acoustic search
 * \param	contornos	set of candidate contours resulting from the intonation unit selection
 * \param[in]	numero_contornos_totales	number of candidate contours in the preceding array
 * \param[in]	numero_contornos	number of output contours
 * \param[in]	numero_grupos_acentuales	number of accent groups in each contour
 * \param[in]	criterio	criterium for selection:
 0 =>  quadratic
 1 => groups
 other => cost
 * \param[in]	costes_minimos	array of costs
 * \param	relacion_original	relation between the original array and the resulting one
 * \return	0, in absence of errors
 */
int Grupo_acentual_candidato::selecciona_mejores_opciones(Grupo_acentual_candidato **contornos, int numero_contornos_totales, int numero_contornos, int numero_grupos_acentuales, char criterio, float *costes_minimos, int *relacion_original) {


  int contador;
  int grupos_escogidos = 1;
  int indice = 0;
  int indice_intercambia = 1;
  char iguales;
  int relacion;
  float coste;
  float distancia, maximo, distancia_auxiliar;
  char *repetidos;
  Grupo_acentual_candidato *apunta_candidato, *intercambia, *apunta_2;

  // En primer lugar, reordenamos

  reorganiza_caminos_segun_costes(contornos, numero_contornos_totales, costes_minimos, relacion_original);

  // Como puede que haya repetidos, los marcamos:

  if ( (repetidos = (char *) malloc(numero_contornos_totales*sizeof(char))) == NULL) {
    fprintf(stderr, "Error en selecciona_mejores_opciones, al asignar memoria.\n");
    return 1;
  } // if ( (repetidos = ...

  for (contador = 0; contador < numero_contornos_totales; contador++)
    repetidos[contador] = 0;

  for (contador = 0; contador < numero_contornos_totales; contador++) {

    if (repetidos[contador] == 1)
      continue;

    apunta_candidato = contornos[contador];
    for (int cuenta = contador + 1; cuenta < numero_contornos_totales; cuenta++) {
      apunta_candidato = contornos[contador];
      apunta_2 = contornos[cuenta];
      iguales = 1;
      for (int cuenta_2 = 0; cuenta_2 < numero_grupos_acentuales; cuenta_2++) {
	if (apunta_candidato++->identificador != apunta_2++->identificador) {
	  iguales = 0;
	  break;
	}
      }
      if (iguales == 1) {
	repetidos[contador] = 1;
	repetidos[cuenta] = 1;
      }
    } // for (int cuenta = 0; 

  } // for (contador = 0; ...

  // Luego seleccionamos los mejores:

  if ( (criterio == 0) || (criterio == 1) ) {

    while (grupos_escogidos < numero_contornos) {

      // El primer contorno siempre se incluye en la búsqueda (por eso empezamos la
      // búsqueda en contador = 1)

      // Ahora seleccionamos el resto en función de las distancias al último
      // seleccionado:

      maximo = -1.0;

      for (contador = indice_intercambia; contador < numero_contornos_totales; contador++) {

	if (repetidos[contador] == 1)
	  continue;

	// Calculamos la distancia a los grupos ya escogidos:
	
	distancia = 0.0;

	for (int cuenta = 0; cuenta < grupos_escogidos; cuenta++) {
	  apunta_candidato = contornos[cuenta];

	  if (criterio == 0) {
	    if (calcula_distancia_entre_contornos_cuadratica(apunta_candidato, contornos[contador], numero_grupos_acentuales, &distancia_auxiliar))
	      return 1;
	  } // if (criterio == 0)
	  else
	    if (calcula_distancia_entre_contornos_grupos(apunta_candidato, contornos[contador], numero_grupos_acentuales, &distancia_auxiliar))
	      return 1;
	  
	  distancia += distancia_auxiliar;
	  
	}
	
	if (distancia > maximo) {
	  maximo = distancia;
	  indice = contador;
	} // if (distancia < minimo)

      } // for (contador = 0; ...

      // Intercambiamos las posiciones:

      intercambia = contornos[indice_intercambia];
      contornos[indice_intercambia] = contornos[indice];
      contornos[indice] = intercambia;

      coste = costes_minimos[indice_intercambia];
      costes_minimos[indice_intercambia] = costes_minimos[indice];
      costes_minimos[indice] = coste;

      relacion = relacion_original[indice_intercambia];
      relacion_original[indice_intercambia++] = relacion_original[indice];
      relacion_original[indice] = relacion;

      grupos_escogidos++;
            
    } // while ...

  }

  free(repetidos);

  return 0;

}


/**
 * Función:   copia_grupo_acentual
 * Clase:     Grupo_acentual_candidato
 * \remarks Entrada:
 *			- original: grupo que se desea copiar.
 * \remarks Valor devuelto:
 *          - En ausencia de error devuelve un cero.
 * Nota:	No llega con una simple igualación, por las variables dinámicas. Si se
 *			liberase la memoria de uno, nos cargaríamos también el otro.
 */

int Grupo_acentual_candidato::copia_grupo_acentual(Grupo_acentual_candidato *original) {

  // Copiamos toda la información de golpe:

  *this = *original;

  // Y nos ocupamos de la parte dinámica:

  if ( (frecuencia.marcas = (estructura_marcas_frecuencia *) malloc(frecuencia.numero_marcas*sizeof(estructura_marcas_frecuencia)))
       == NULL) {
    fprintf(stderr, "Error en Grupo_acentual_candidato:copia_grupo_acentual, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencia.marcas = ...

  estructura_marcas_frecuencia *correcaminos = frecuencia.marcas, *recorre_original = original->frecuencia.marcas;

  for (int contador = 0; contador < frecuencia.numero_marcas; contador++, correcaminos++)
    *correcaminos = *recorre_original++;


  return 0;

}


/**
 * Función:   escribe_datos_txt                                                       
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la unidad.                                                            
 * \remarks Objetivo:  escribir en un fichero la información del grupo acentual de forma       
 *            fácilmente legible por el usuario.                                      
 * \remarks Valor devuelto: 																	  
 *			- En ausencia de error, devuelve un cero.								  	 
 */

int Grupo_acentual_candidato::escribe_datos_txt(FILE *fichero) {

  fprintf(fichero, "***%d\n", identificador);
  fprintf(fichero, "%f\t%f\n", inicio, final);
  fprintf(fichero, "%x\t%x\t%x\n", tipo_grupo_prosodico_1&0xff, tipo_grupo_prosodico_2&0xff,
	  tipo_grupo_prosodico_3&0xff);
  fprintf(fichero, "Factor de caída: %f\n", factor_caida); 
  fprintf(fichero, "Salto de f0: %f\n", salto_f0);
  fprintf(fichero, "Posición sílaba acentuada: %c\n", posicion_silaba_acentuada + '0');
  fprintf(fichero, "Posición grupo prosódico: %d\n",  posicion_grupo_prosodico);
  fprintf(fichero, "Grupos acentuales del grupo prosódico: %d\n",
	  grupos_acentuales_grupo_prosodico);
  fprintf(fichero, "Posición grupo acentual: %d\n", posicion_grupo_acentual);
  fprintf(fichero, "Número de sílabas del grupo acentual: %d\n", numero_silabas);
  fprintf(fichero, "Número de grupos prosódicos de la frase: %d.\n", numero_grupos_prosodicos);
  fprintf(fichero, "Tipo de pausa inicial: %d.\n", tipo_pausa_anterior);
  fprintf(fichero, "Tipo de pausa final: %d.\n", tipo_pausa);
#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
  char cadena_categoria[50];

  if (escribe_tipo_categoria_morfosintactica_grupo((int) etiqueta_morfosintactica, cadena_categoria)) {
    fprintf(stderr, "Error en el grupo %d.\n", identificador);
    return 1;
  }

  fprintf(fichero, "Categoría morfosintactica: %s.\n", cadena_categoria);

  if (escribe_tipo_sintagma_grupo((int) sintagma_actual, cadena_categoria)) {
    fprintf(stderr, "Error en el grupo %d.\n", identificador);
    return 1;
  }

  fprintf(fichero, "Sintagma actual: %s.\n", cadena_categoria);

  if (escribe_tipo_sintagma_grupo((int) siguiente_sintagma, cadena_categoria)) {
    fprintf(stderr, "Error en el grupo %d.\n", identificador);
    return 1;
  }

  fprintf(fichero, "Siguiente sintagma: %s.\n", cadena_categoria);

#endif

  fprintf(fichero, "Frecuencia media: %f.\n", frecuencia_media);
  fprintf(fichero, "Índice del máximo: %d.\n", indice_maximo);
  fprintf(fichero, "Número de la frase: %d.\n\n", numero_frase);
  fprintf(fichero, "Número de marcas: %d.\n", frecuencia.numero_marcas);

  fprintf(fichero, "Tiempo inicio marcas: %f.\n", frecuencia.tiempo_inicio);
  fprintf(fichero, "Tiempo final marcas: %f.\n", frecuencia.tiempo_final);
    
  estructura_marcas_frecuencia *correcaminos = frecuencia.marcas;

#ifdef _INCLUYE_SILABAS
  for (int cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++)
    fprintf(fichero, "%f, %f, %s, %c, (%c)\n", correcaminos->tiempo, correcaminos->frecuencia,
	    correcaminos->silaba,
	    correcaminos->tipo_silaba + '0',
	    correcaminos->sonoridad_pobre + '0');
#else
  for (int cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++)
    fprintf(fichero, "%f, %f, %c, (%c)\n", correcaminos->tiempo, correcaminos->frecuencia,
	    correcaminos->tipo_silaba + '0',
	    correcaminos->sonoridad_pobre + '0');

#endif

  fprintf(fichero, "\n\n");

  return 0;
    
}


/**
 * Función:   lee_datos_txt                                                       	  
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto del que se lee la información de la unidad.   
 * \remarks Valor devuelto:																	  
 *			- En ausencia de error devuelve un cero.								  		
 * \remarks Objetivo:  leer de un fichero la información de un grupo acentual candidato.		  
 */

int Grupo_acentual_candidato::lee_datos_txt(FILE *fichero) {

  char c1[30], c2[30], c3[30], c4[30], c5[30], c6[30], c7[30];

  fscanf(fichero, "%d\n", &identificador);
  fscanf(fichero, "%f\t%f\n", &inicio, &final);
  fscanf(fichero, "%d\t%d\t%d\n", &tipo_grupo_prosodico_1, &tipo_grupo_prosodico_2,
	 &tipo_grupo_prosodico_3);
  fscanf(fichero, "%s %s %s %f\n", c1, c2, c3, &factor_caida);
  fscanf(fichero, "%s %s %s %f\n", c1, c2, c3, &salto_f0);
  fscanf(fichero, "%s %s %s %c\n", c1, c2, c3, &posicion_silaba_acentuada);
  posicion_silaba_acentuada -= '0';
  fscanf(fichero, "%s %s %s %d\n", c1, c2, c3, &posicion_grupo_prosodico);
  fscanf(fichero, "%s %s %s %s %s %d\n", c1, c2, c3, c4, c5, &grupos_acentuales_grupo_prosodico);
  fscanf(fichero, "%s %s %s %d\n", c1, c2, c3, &posicion_grupo_acentual);
  fscanf(fichero, "%s %s %s %s %s %s %d\n", c1, c2, c3, c4, c5, c6, &numero_silabas);
  fscanf(fichero, "%s %s %s %s %s %s %s %d.\n", c1, c2, c3, c4, c5, c6, c7, &numero_grupos_prosodicos);
  fscanf(fichero, "%s %s %s %d.\n", c1, c2, c3, &tipo_pausa);
  fscanf(fichero, "%s %s %f.\n", c1, c2, &frecuencia_media);
  fscanf(fichero, "%s %s %s %d.\n", c1, c2, c3, &indice_maximo);
  fscanf(fichero, "%s %s %s %s %d.\n\n", c1, c2, c3, c4, &numero_frase);
  fscanf(fichero, "%s %s %s %d.\n", c1, c2, c3, &frecuencia.numero_marcas);

  fscanf(fichero, "%s %s %s %f.\n", c1, c2, c3, &frecuencia.tiempo_inicio);
  fscanf(fichero, "%s %s %s %f.\n", c1, c2, c3, &frecuencia.tiempo_final);

  if ( (frecuencia.marcas = (estructura_marcas_frecuencia *) malloc(frecuencia.numero_marcas*sizeof(estructura_marcas_frecuencia)))
       == NULL) {
    fprintf(stderr, "Error en Grupo_acentual_candidato:lee_datos.txt, al asignar memoria.\n");
    return 1;
  } // if ( (frecuencia.marcas = ...

  estructura_marcas_frecuencia *correcaminos = frecuencia.marcas;

  for (int cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++) {
    fscanf(fichero, "%f, %f, (%c)\n", &correcaminos->tiempo, &correcaminos->frecuencia,
	   &correcaminos->sonoridad_pobre);
    correcaminos->sonoridad_pobre -= '0';
  } // for (int cuenta = 0; ...

  return 0;

}


/**
 * Función:   escribe_contorno                                                        
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la unidad.                                                            
 * \remarks Objetivo:  escribir en un fichero la información del contorno del grupo.           
 */

void Grupo_acentual_candidato::escribe_contorno(FILE *fichero) {

  estructura_marcas_frecuencia *correcaminos = frecuencia.marcas;

  for (int cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++)
    fprintf(fichero, "%f\t%f\n", correcaminos->tiempo, correcaminos->frecuencia);

  fprintf(fichero, "\n");

}

/**
 * Función:   escribe_datos_txt_tabla                                                 
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la unidad.                                                            
 * \remarks Objetivo:  escribir en un fichero la información del grupo acentual en forma de    
 *            tabla, para generar un fichero que se pueda emplear en programas como   
 *            el excel.                                                               
 */

void Grupo_acentual_candidato::escribe_datos_txt_tabla(FILE *fichero) {

  fprintf(fichero, "%f\t%f\t ", inicio, final);
  fprintf(fichero, "%x\t%x\t%x\t", tipo_grupo_prosodico_1 & 0xff, tipo_grupo_prosodico_2 & 0xff,
	  tipo_grupo_prosodico_3 & 0xff);
  fprintf(fichero, "%c\t", posicion_silaba_acentuada + '0');
  fprintf(fichero, "%d\t", posicion_grupo_prosodico);
  fprintf(fichero, "%d\t", grupos_acentuales_grupo_prosodico);
  fprintf(fichero, "%d\t", posicion_grupo_acentual);
  fprintf(fichero, "%d\t", numero_silabas);
  fprintf(fichero, "%d\t", numero_grupos_prosodicos);
  fprintf(fichero, "%d\t", numero_frase);

  //    fprintf(fichero, "%s\t", fichero_origen);
    
  fprintf(fichero, "%d\t", frecuencia.numero_marcas);

  estructura_marcas_frecuencia *correcaminos;
  int cuenta;

  //    for (int cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++)
  //        fprintf(fichero, "%f, %f, (%c)\n", correcaminos->tiempo, correcaminos->frecuencia,
  //                                           correcaminos->sonoridad_pobre + '0');

  /*    for (cuenta = 0; cuenta < frecuencia.numero_marcas; cuenta++, correcaminos++)
        fprintf(fichero, "%f ", correcaminos->tiempo);
  */
  for (correcaminos = frecuencia.marcas, cuenta = 0; cuenta < frecuencia.numero_marcas;
       cuenta++, correcaminos++)
    if (correcaminos->sonoridad_pobre == 0)
      fprintf(fichero, "%f\t", correcaminos->frecuencia);
    else
      fprintf(fichero, "%f\t", 0.0);

  /*
    for (correcaminos = frecuencia.marcas, cuenta = 0; cuenta < frecuencia.numero_marcas;
    cuenta++, correcaminos++)
    fprintf(fichero, "%c ", correcaminos->sonoridad_pobre + '0');
  */

    
  fprintf(fichero, "\n");

}


/**
 * Función:   anhade_identificador                                                    
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada:
 *			- ident: identificador de la unidad.                                      
 */

void Grupo_acentual_candidato::anhade_identificador(int ident) {

  //    fichero_origen = NULL; // Algo chapuza, pero es sólo para las pruebas.
  identificador = ident;

}


/**
 * Función:   escribe_datos_bin                                                       
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - salida: fichero binario en el que se escribe la información relevante   
 *            a la unidad.                                                            
 * \remarks Valor devuelto:                                                                    
 *          - Actualmente siempre devuelve un 0.                                      
 * \remarks Objetivo:  escribir en un fichero la información de los semifonemas de forma       
 *            que ocupe poco espacio en memoria.                                      
 * \remarks NOTA:      Hay que tener en cuenta los criterios de representación en memoria de   
 *            cada máquina (extremista mayor, menor...) Tengo que comentárselo a Edu. 
 */

int Grupo_acentual_candidato::escribe_datos_bin(FILE *salida) {

  if (fwrite(this, sizeof(Grupo_acentual_candidato), 1, salida) != 1) {
    fprintf(stderr, "Error en Grupo_acentual_candidato::escribe_datos_bin, al \
intentar escribir en el fichero de salida.\n");
    return 1;
  }

  // Ahora escribimos la parte dinámica:

  if (fwrite(frecuencia.marcas, sizeof(estructura_marcas_frecuencia), frecuencia.numero_marcas,
	     salida) != (unsigned int) frecuencia.numero_marcas) {
    fprintf(stderr, "Error en Grupo_acentual_candidato::escribe_datos_bin, al \
intentar escribir en el fichero de salida.\n");
    return 1;
  }

  return 0;

}


/**
 * Función:   lee_datos_bin                                                           
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada y Salida:                                                                  
 *          - entrada: fichero binario del que se lee la información relevante de la  
 *            unidad.                                                                 
 * \remarks Valor devuelto:                                                                    
 *          - En caso de error devuelve un uno, y si se llega a fin de fichero, un 2. 
 */

int Grupo_acentual_candidato::lee_datos_bin(FILE *entrada) {

  fread(this, sizeof(Grupo_acentual_candidato), 1, entrada);

  if (feof(entrada))
    return 2;

  if ( (frecuencia.marcas = (estructura_marcas_frecuencia *) malloc(frecuencia.numero_marcas*
								    sizeof(estructura_marcas_frecuencia))) == NULL) {
    fprintf(stderr, "Error en lee_datos_bin, al asignar memoria.\n");
    return 1;
  }

  // Ahora leemos la parte dinámica:

  fread(frecuencia.marcas, sizeof(estructura_marcas_frecuencia), frecuencia.numero_marcas,
	entrada);

  return 0;

}


/**
 * Función:   anhade_fichero_origen                                                   
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada:
 *			- fichero: nombre del fichero en el que se encuentra la unidad.           
 * \remarks Valor devuelto:                                                                    
 *          - En caso de error, devuelve un 1.                                        
 * \remarks Objetivo:  Introducir en el descriptor el nombre del fichero de texto del que se   
 *            extrajo el grupo acentual.                                              
 */
/*
  int Grupo_acentual_candidato::anhade_fichero_origen(char *fichero) {

  if ( (fichero_origen = (char *) malloc(strlen(fichero) + 1)) == NULL) {
  puts("Error en anhade_fichero_origen, al asignar memoria.");
  return 1;
  }

  strcpy(fichero_origen, fichero);

  return 0;

  }
*/
/**
 * Función:   escribe_parte_estatica                                                  
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada:
 *			- fichero: fichero en el que se va a escribir la información estática del 
 *            grupo_acentual.                                                         
 * \remarks Objetivo:  Escribir en un fichero la parte estática de la unidad. En conjunto con  
 *            la siguiente función miembro permite una lectura posterior rápida y     
 *            eficaz de los datos de un fichero.                                      
 */

void Grupo_acentual_candidato::escribe_parte_estatica(FILE *fichero) {

  fwrite(this, sizeof(Grupo_acentual_candidato), 1, fichero);

}

/**
 * Función:   escribe_parte_dinamica                                                  
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Entrada:
 *			- fichero: fichero en el que se va a escribir la información dinámica del 
 *            Grupo_acentual_candidato                                                
 * \remarks Objetivo:  Escribir en un fichero la parte dinámica de la unidad. En conjunto con  
 *            la anterior función miembro permite una lectura posterior rápida y      
 *            eficaz de los datos de un fichero.                                      
 */

void Grupo_acentual_candidato::escribe_parte_dinamica(FILE *fichero) {

  fwrite(frecuencia.marcas, sizeof(estructura_marcas_frecuencia), frecuencia.numero_marcas,
	 fichero);

}


/**
 * Función:   lee_datos_parte_dinamica                                                
 * Clase:     Grupo_acentual_candidato                                                
 * Entrada y salida:                                                                  
 *          - cadena: cadena de caracteres en la que se encuentra la información de   
 *            la parte dinámica del objeto.                                           
 * \remarks NOTA:      Se supone que la memoria está correctamente reservada. Si no, casca.    
 */

void Grupo_acentual_candidato::lee_parte_dinamica(char **cadena) {

  frecuencia.marcas = (estructura_marcas_frecuencia *) *cadena;

  *cadena += frecuencia.numero_marcas*sizeof(estructura_marcas_frecuencia);

}


/**
 * Función:   libera_memoria                                                          
 * Clase:     Grupo_acentual_candidato                                                
 * \remarks Objetivo:  Liberar la memoria reservada para la cadena de frecuencias estilizadas. 
 */

void Grupo_acentual_candidato::libera_memoria() {

  if (frecuencia.marcas) {
    free(frecuencia.marcas);
    frecuencia.marcas = NULL;
  }
  /*
    if (fichero_origen) {
    free(fichero_origen);
    fichero_origen = NULL;
    }
  */
}


/**
 * Función:   crea_grupo_objetivo                                                     
 * Clase:     Grupo_acentual_objetivo                                                 
 * \remarks Entrada:		                                                                      
 *          - candidato: objeto de tipo Grupo_acentual_objetivo.					  
 * \remarks Objetivo:  Convertir la información, dentro de lo posible, del tipo candidato al   
 *			  objetivo.																  
 */

void Grupo_acentual_objetivo::crea_grupo_objetivo(Grupo_acentual_candidato *candidato) {



  duracion = candidato->final - candidato->inicio;

  // float duracion_hasta_acento; // No disponible

  tipo_proposicion = candidato->tipo_grupo_prosodico_3;

  posicion_silaba_acentuada = candidato->posicion_silaba_acentuada;

  posicion_grupo_prosodico = candidato->posicion_grupo_prosodico;

  // indice_alofono_tonico; // No disponible.

  grupos_acentuales_grupo_prosodico = candidato->grupos_acentuales_grupo_prosodico;

  posicion_grupo_acentual = candidato->posicion_grupo_acentual;

  numero_silabas = candidato->numero_silabas;

  numero_grupos_prosodicos = candidato->numero_grupos_prosodicos;

  // short int numero_alofonos; // No disponible

  tipo_pausa = candidato->tipo_pausa;

  tipo_proposicion2 = candidato->tipo_grupo_prosodico_1;

  // estructura_silaba *silabas; // No disponible.


}


/**
 * Función:   escribe_datos_txt                                                       
 * Clase:     Grupo_acentual_objetivo                                                 
 * \remarks Entrada y Salida:                                                                  
 *          - fichero: fichero de texto en el que se escribe la información relevante 
 *            a la unidad.                                                            
 * \remarks Objetivo:  escribir en un fichero la información del grupo acentual de forma       
 *            fácilmente legible por el usuario.                                      
 */

void Grupo_acentual_objetivo::escribe_datos_txt(FILE *fichero) {

  fprintf(fichero, "%f\n", duracion);
  fprintf(fichero, "%x\n", tipo_proposicion);
  fprintf(fichero, "Posición sílaba acentuada: %c\n", posicion_silaba_acentuada + '0');
  fprintf(fichero, "Posición grupo prosódico: %d\n",  posicion_grupo_prosodico);
  fprintf(fichero, "Grupos acentuales del grupo prosódico: %d\n",
	  grupos_acentuales_grupo_prosodico);
  fprintf(fichero, "Posición grupo acentual: %d\n", posicion_grupo_acentual);
  fprintf(fichero, "Número de sílabas del grupo acentual: %d\n", numero_silabas);
  fprintf(fichero, "Número de fonemas del grupo acentual: %d\n", numero_alofonos);
  fprintf(fichero, "Número de grupos prosódicos de la frase: %d.\n", numero_grupos_prosodicos);

  fprintf(fichero, "\n\n");

}


/**
 * Función:   libera_memoria                                                          
 * Clase:     Grupo_acentual_objetivo                                                 
 */

void Grupo_acentual_objetivo::libera_memoria() {

  if (silabas)
    free(silabas);    
}


/**
 * Función:   carga_fichero_frecuencia_estilizada                                   
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la evolución de las marcas de 
 *            frecuencia estilizada.                                                
 *	\remarks Salida:
 *			- frecuencia_estilizada: estructura con dicha información.              
 * \remarks Objetivo:  Cargar en memoria la información de la frecuencia estilizada.         
 */

int carga_fichero_frecuencia_estilizada(FILE *fichero,
                                        Frecuencia_estilizada *frecuencia_estilizada) {

  char cadena[100] = "";
  int numero_elementos;
  float tiempo;
  estructura_marcas_frecuencia *correcaminos;
  // Eliminamos la cabecera:


  while (strcmp(cadena, "\"PitchTier\""))
    fscanf(fichero, "%s\n", cadena);

  fscanf(fichero, "%f\n", &tiempo);
  fscanf(fichero, "%f\n", &tiempo);

  fscanf(fichero, "%d\n", &numero_elementos);

  if ( (frecuencia_estilizada->marcas = (estructura_marcas_frecuencia *) malloc(
										numero_elementos*sizeof(estructura_marcas_frecuencia)))
       == NULL) {

    fprintf(stderr, "Error en carga_fichero_frecuencia_estilizada, al asignar memoria.\n");
    return 1;
  }

  frecuencia_estilizada->numero_marcas = numero_elementos;

  correcaminos = frecuencia_estilizada->marcas;

  for (; numero_elementos > 0; numero_elementos--, correcaminos++) {
    fscanf(fichero, "%f\n", &correcaminos->tiempo);
    fscanf(fichero, "%f\n", &correcaminos->frecuencia);
  }

  frecuencia_estilizada->tiempo_inicio = frecuencia_estilizada->marcas[0].tiempo;
  frecuencia_estilizada->tiempo_final = (correcaminos - 1)->tiempo;
    
  return 0;

}


/**
 * Función:   interpola                                                             
 * \remarks Entrada:
 *			- marcas_frecuencia: estructura con la información de la evolución de   
 *            la frecuencia estilizada.                                             
 *          - x: punto en el que se quiere calcular la frecuencia.                  
 *	\remarks Salida:
 *			- sonoridad: indica si el valor devuelto se encuentra dentro de los     
 *            límites máximo y mínimo o no. Si es por debajo, devuelve 1; por       
 *            encima, 2; por último, si está entre los límites, se devuelve 0.      
 * \remarks Valor devuelto:                                                                  
 *          - La frecuenca interpolada.                                             
 * \remarks NOTA:      Si se encuentra fuera de los límites, esta versión devuelve el valor  
 *            real más cercano.                                                     
 */

float interpola(Frecuencia_estilizada *marcas_frecuencia, float x, char *sonoridad) {

  if (x >= marcas_frecuencia->tiempo_final) {
    if (sonoridad)
      *sonoridad = 2;
    // De momento
    return marcas_frecuencia->marcas[marcas_frecuencia->numero_marcas - 1].frecuencia;
  }
  if (x <= marcas_frecuencia->tiempo_inicio) {
    if (sonoridad)
      *sonoridad = 1;
    // De momento
    return marcas_frecuencia->marcas[0].frecuencia;
  }

  if (sonoridad)
    *sonoridad = 0;

  // En este caso, tenemos que interpolar

  estructura_marcas_frecuencia *correcaminos = marcas_frecuencia->marcas;

  while (correcaminos->tiempo < x)
    correcaminos++;

  // Interpolamos, a partir de la ecuación de la recta que definen los dos puntos
  // conocidos que rodean a x:
    
  return ( (correcaminos->frecuencia - (correcaminos - 1)->frecuencia) /
	   (correcaminos->tiempo - (correcaminos - 1)->tiempo)*(x - correcaminos->tiempo)
	   + correcaminos->frecuencia);

}



/**
 * Función:   inicia_tabla_grupos                                                   
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un cero.                               
 * \remarks Objetivo:  Inicializar a cero los punteros a las cadenas de clústeres de grupos  
 *            acentuales.                                                           
 */

int inicia_tabla_grupos() {

  if ( (tabla_grupos = (estructura_grupos_acentuales **) malloc(NUMERO_GRUPOS_POSIBLES*
								sizeof(estructura_grupos_acentuales *))) == NULL) {
    fprintf(stderr, "Error en inicia_lista_grupos, al asignar memoria.\n");
    return 1;
  }

  memset(tabla_grupos, 0, NUMERO_GRUPOS_POSIBLES*sizeof(estructura_grupos_acentuales *));

  return 0;

}


/**
 * Función:   anhade_grupo_acentual                                                 
 * \remarks Entrada:
 *			- nuevo_grupo: objeto de la clase Grupo_acentual_candidato cuya         
 *            información se quiere almacenar en clústeres.                         
 *			- fichero: fichero en el que se escriben los grupos descartados.		
 *			- opcion: == 0 => Se escribe toda la información del grupo descartado.  
 *			  En otro caso, sólo el identificador.									
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un cero.                               
 */

int anhade_grupo_acentual(Grupo_acentual_candidato *nuevo_grupo, FILE *fichero, int opcion) {

  int indice;//, indice_maximo;
  float variacion;
  float pendiente;
  //    float frecuencia_media;
  estructura_grupos_acentuales *correcaminos;
  Grupo_acentual_candidato copia;

  unsigned char posicion;
  /*
    unsigned char posicion = nuevo_grupo->posicion_grupo_acentual;

    if (posicion != 0) {
    if (posicion == nuevo_grupo->grupos_acentuales_grupo_prosodico - 1)
    posicion = GRUPO_FINAL;
    else
    posicion = GRUPO_MEDIO;
    } // if (posicion != 0)
    if (nuevo_grupo->grupos_acentuales_grupo_prosodico == 1)
    posicion = GRUPO_INICIAL_Y_FINAL;
  */

  unsigned char tipo_pausa_anterior = nuevo_grupo->tipo_pausa_anterior;
  unsigned char tipo_pausa = nuevo_grupo->tipo_pausa;

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



  copia.copia_grupo_acentual(nuevo_grupo);
  modifica_contorno_grupo_acentual_natural(0, 0, &copia.frecuencia);
  //    frecuencia_media = calcula_frecuencia_media_grupo(&copia.frecuencia);
  //    indice_maximo = calcula_frecuencia_maxima_grupo_absoluto(&copia.frecuencia);
  variacion =  encuentra_maxima_variacion_absoluto_grupo(&copia.frecuencia);
  pendiente = absoluto(nuevo_grupo->frecuencia.marcas[0].frecuencia -
		       nuevo_grupo->frecuencia.marcas[nuevo_grupo->frecuencia.numero_marcas - 1].frecuencia);

  if ( comprueba_error_estimacion_contorno(nuevo_grupo) ||
       (	(variacion < UMBRAL_GRUPO_ACENTUAL) &&
       		(pendiente < UMBRAL_PENDIENTE_GRUPO_ACENTUAL) &&
       		(posicion != GRUPO_FINAL) &&
       		(nuevo_grupo->posicion_grupo_acentual != nuevo_grupo->grupos_acentuales_grupo_prosodico - 2) ) ) {
    if (opcion == 0)
      nuevo_grupo->escribe_datos_txt(fichero);
    else
      fprintf(fichero, "%d\n", nuevo_grupo->identificador);
    copia.libera_memoria();
    return 0; // No es un grupo válido.
  }// if ( (nuevo_grupo->....

  if ( (posicion == GRUPO_FINAL) && (variacion == 0) ) {
    if (opcion == 0)
      nuevo_grupo->escribe_datos_txt(fichero);
    else
      fprintf(fichero, "%d\n", nuevo_grupo->identificador);
    copia.libera_memoria();
    return 0; // No es un grupo válido.
  }

  copia.libera_memoria();
    
  indice = calcula_indice_grupo(nuevo_grupo->tipo_grupo_prosodico_3, posicion,
				nuevo_grupo->posicion_silaba_acentuada);
    
  if (tabla_grupos[indice] == NULL) {

    if ( (tabla_grupos[indice] = (estructura_grupos_acentuales *) malloc(
									 sizeof(estructura_grupos_acentuales))) == NULL) {

      fprintf(stderr, "Error en anhade_grupo_acentual, al asignar memoria.\n");
      return 1;
    }

    correcaminos = tabla_grupos[indice];

    if ( (correcaminos->grupos = (Grupo_acentual_candidato *) malloc(1000*sizeof(Grupo_acentual_candidato)))
	 == NULL) {
      fprintf(stderr, "Error en anhade_grupo_acentual, al asignar memoria.\n");
      return 1;
    }

    correcaminos->numero_grupos = 1;

    correcaminos->grupos[0] = *nuevo_grupo; // Ojo, para hacer esto, es necesario que se vaya
    // asignando diferente memoria a nuevo_grupo en cada
    // llamada. Si no, estaríamos modificando los grupos
    // anteriores.

  }
  else { // Ya existe ese grupo.

    correcaminos = tabla_grupos[indice];

    if ( (correcaminos->numero_grupos % 1000) == 999) { // Añadimos más memoria
      if ( (tabla_grupos[indice]->grupos = (Grupo_acentual_candidato *) realloc(
										tabla_grupos[indice]->grupos, (tabla_grupos[indice]->numero_grupos + 1000)*
										sizeof(Grupo_acentual_candidato)) ) == NULL) {
	fprintf(stderr, "Error en anhade_grupo_acentual, al asignar memoria.\n");
	return 1;
      }
      correcaminos = tabla_grupos[indice];
    }

    correcaminos->grupos[correcaminos->numero_grupos++] = *nuevo_grupo;

  }

  return 0;

}


/**
 * \brief	Función que comprueba un posible fallo en la estimación del contorno.
 * \remarks	Comprueba que no haya una variación muy grande de una sílaba a la
 * siguiente en la zona del contorno que no se corresponde con el acento.
 * \remarks	Los grupos que van después de una ruptura entonativa suelen tener esta
 * variación (gran bajada y luego subida), así que aquí estaremos eliminando muchos
 * grupos de ese tipo si la base no está bien etiquetada. En cualquier caso, puede
 * que sea mejor eliminarlos que utilizarlos donde no se debe.
 * \param	grupo: grupo acentual candidato.
 * \return	Devuelve un 1 si hay un posible fallo. Cero en caso contrario.
 */

int comprueba_error_estimacion_contorno(Grupo_acentual_candidato *grupo) {

  estructura_marcas_frecuencia *marcas = grupo->frecuencia.marcas;

  if (grupo->tipo_pausa_anterior != SIN_PAUSA)
    return 0;
        
  for (int contador = 0; contador < grupo->numero_silabas - grupo->posicion_silaba_acentuada - 1;
       contador++, marcas++)
    if ( (marcas->frecuencia - marcas[1].frecuencia > MAXIMA_VARIACION_CONTORNO_ZONA_NO_ACENTUADA) &&
	 (marcas[2].frecuencia - marcas[1].frecuencia > MAXIMA_VARIACION_CONTORNO_ZONA_NO_ACENTUADA) )
      return 1;

  return 0;
    
}



/**
 * Función:   escribe_grupos_ordenados
 * \remarks Entrada:
 *			- unico_fichero: variable que indica si se desea que la salida sea en   
 *            un único fichero (1), o en uno por cada clúster (0).                  
 *          - locutor: Nombre del locutor.		                                    
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un cero.                               
 * \remarks Objetivo:  Escribir en diferentes ficheros los distintos grupos acentuales       
 *            organizados en clústeres.                                             
 */

int escribe_grupos_ordenados(char unico_fichero, char locutor[]) {

  FILE *fichero = NULL;
  FILE *fichero_txt;

  char nombre_fichero[200];
  char indice[10];
  int cuenta, contador;
  char nombre_acentuales_bin[200];
  char nombre_acentuales_txt[200];

  estructura_grupos_acentuales **correcaminos = tabla_grupos;
  Grupo_acentual_candidato *recorre_acentuales;

  sprintf(nombre_acentuales_bin, "%s%s.bin", NOMBRE_FICHERO_ACENTUALES, locutor);
  sprintf(nombre_acentuales_txt, "%s%s.txt", NOMBRE_FICHERO_ACENTUALES, locutor);

  if ( (fichero_txt = fopen(nombre_acentuales_txt, "wt")) == NULL) {

    fprintf(stderr, "Error en escribe_grupos_ordenados, al intentar abrir el fichero clusters_acentuales.txt.\n");
    return 1;
  }

  fprintf(fichero_txt, "Proposición\tPosición\tSílaba acentuada\tNúmero.\n");

  if (unico_fichero) {
    if ( (fichero = fopen(nombre_acentuales_bin, "wb")) == NULL) {
      fprintf(stderr, "Error en escribe_grupos_ordenados, al crear el fichero clusters_acentuales.bin.\n");
      return 1;
    }
#ifdef _x86_64
    fwrite(tabla_grupos, sizeof(estructura_grupos_acentuales_compatible), NUMERO_GRUPOS_POSIBLES,
	   fichero);
#else
    estructura_grupos_acentuales_compatible tabla_grupos_compatible[NUMERO_GRUPOS_POSIBLES];
    for (contador = 0; contador < NUMERO_GRUPOS_POSIBLES; contador++) {
      tabla_grupos_compatible[contador].dummy = 0;
      tabla_grupos_compatible[contador].puntero = tabla_grupos[contador];
    } // for (contador = 0; ...
    fwrite(tabla_grupos_compatible, sizeof(estructura_grupos_acentuales_compatible), NUMERO_GRUPOS_POSIBLES, fichero);
#endif
  } // if (unico_fichero);

  for (contador = 0; contador < NUMERO_GRUPOS_POSIBLES; contador++, correcaminos++) {

    if (*correcaminos != NULL) {

      fprintf(fichero_txt, "%d\t\t\t%d\t\t\t%d\t\t\t\t%d.\n", contador & 0x3, (contador >> 2) & 0x3,
	      (contador >> 4) & 0x3, (*correcaminos)->numero_grupos);

      if (unico_fichero == 0) {

	strcpy(nombre_fichero, "grupos_ordenados\\fich_");
	sprintf(indice, "%d.grp", contador);
	strcat(nombre_fichero, indice);

	if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
	  fprintf(stderr, "Error en escribe_grupos_ordenados, al intentar crear el fichero %s.\n",
		  nombre_fichero);
	  fclose(fichero_txt);
	  return 1;
	}

	Grupo_acentual_candidato *recorre_acentuales = (*correcaminos)->grupos;
	/*
	  for (int cuenta = 0; cuenta < (*correcaminos)->numero_grupos; recorre_acentuales++, cuenta++)
	  recorre_acentuales->escribe_datos_txt(fichero);
	*/

	// Escribimos el número de grupos

	fwrite(&((*correcaminos)->numero_grupos), sizeof(int), 1, fichero);

	// Luego, la parte estática.

	fwrite((*correcaminos)->grupos, sizeof(Grupo_acentual_candidato), (*correcaminos)->numero_grupos,
	       fichero);

	// Por último, la parte dinámica

	for (int cuenta = 0; cuenta < (*correcaminos)->numero_grupos; cuenta++)
	  recorre_acentuales++->escribe_parte_dinamica(fichero);

	fclose(fichero);
      } // if (unico_fichero == 0)
      else {

	fwrite(*correcaminos, sizeof(estructura_grupos_acentuales), 1, fichero);

	recorre_acentuales = (*correcaminos)->grupos;

	for (cuenta = 0; cuenta < (*correcaminos)->numero_grupos; cuenta++,
	       recorre_acentuales++)
	  recorre_acentuales->escribe_datos_bin(fichero);

      } // else (if (unico_fichero)

    } // if (correcaminos != NULL...

  } // for (int contador...

  if (fichero)
    fclose(fichero);
  fclose(fichero_txt);

  return 0;

}


/**
 * Función:   libera_memoria_tabla_grupos                                           
 * \remarks Objetivo:  Liberar la memoria reservada para la tabla de grupos acentuales.      
 */

void libera_memoria_tabla_grupos() {

  Grupo_acentual_candidato *recorre_acentuales;
  estructura_grupos_acentuales **correcaminos, *libera;

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


}


/**
 * Función:   lee_tabla_de_fichero                                                  
 * \remarks Entrada:
 *			- fichero: fichero en el que se encuentra la información de los grupos  
 *            acentuales candidatos.                                                
 * \remarks Objetivo:  Leer de un fichero único la información de los clústeres de grupos    
 *            acentuales.                                                           
 */

int lee_tabla_de_fichero(FILE *fichero) {

  estructura_grupos_acentuales **correcaminos;
  estructura_grupos_acentuales *acentuales;
  Grupo_acentual_candidato *recorre_acentuales;
  int contador = 0, cuenta;

  if (inicia_tabla_grupos())
    return 1;

  if ( (acentuales = (estructura_grupos_acentuales *) malloc(NUMERO_CLUSTERES*
							     sizeof(estructura_grupos_acentuales))) == NULL) {
    fprintf(stderr, "Error en lee_tabla_de_fichero, al asignar memoria.\n");
    return 1;
  }

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


  return 0;

}


/**
 * Función:   devuelve_puntero_a_tabla                                              
 * \remarks Entrada:
 *			- indice: índice a la posición de la tabla cuyo valor se desea obtener. 
 * \remarks Valor devuelto:                                                                  
 *          - Un puntero a la posición de la tabla indexada por la variable índice. 
 */

estructura_grupos_acentuales *devuelve_puntero_a_tabla(int indice) {

  return tabla_grupos[indice];
  /*
    if (locutor == FREIRE)
    return tabla_grupos_fre[indice];

    return tabla_grupos_pau[indice];
  */

}


/**
 * Función:   devuelve_parametros_estructura_matriz                           
 * \remarks Entrada:
 *			- parametros: carácter con la información anterior codificada.    
 *	\remarks Salida:
 *			- proposicion: tipo de proposición.                               
 *          - posicion: INICIAL, MEDIA o FINAL.                               
 *          - silaba_acentuada: posición de la sílaba acentuada del grupo.    
 */

void devuelve_parametros_estructura_matriz(char parametros, unsigned char *proposicion,
					   unsigned char *posicion, unsigned char *silaba_acentuada) {

  *proposicion = (unsigned char) (parametros & 0x03);
  *posicion = (unsigned char) ((parametros >> 2) & 0x03);
  *silaba_acentuada = (unsigned char) ((parametros >> 4) & 0x03);

}


/**
 * Función:   crea_parametros_estructura_matriz                               
 * \remarks Entrada:
 *			- proposicion: tipo de proposición.                               
 *          - posicion: INICIAL, MEDIA o FINAL.                               
 *          - silaba_acentuada: posición de la sílaba acentuada del grupo.    
 *	\remarks Salida:
 *			- parametros: carácter con la información anterior codificada.    
 */

void crea_parametros_estructura_matriz(char *parametros, unsigned char proposicion,
				       unsigned char posicion, unsigned char silaba_acentuada) {

  char auxiliar = 0;

  auxiliar |= (char) ((silaba_acentuada << 4) | (posicion << 2) | proposicion);

  *parametros = auxiliar;

}


/**
 * Función:   devuelve_tabla                                                        
 *	\remarks Salida:
 *			-  numero_elementos: número de elementos de la tabla acentual.           
 * \remarks Valor devuelto:                                                                  
 *          - Un puntero al inicio de la tabla.                                     
 */

estructura_grupos_acentuales **devuelve_tabla(int *numero_elementos) {

  *numero_elementos = NUMERO_GRUPOS_POSIBLES;

  return tabla_grupos;

}


/**
 * Función:   calcula_frecuencia_media_grupo                                        
 * \remarks Entrada:
 *			- frecuencia: estructura en la que está la información del contorno de  
 *            pitch.                                                                
 * \remarks Valor devuelto:                                                                  
 *          - La frecuencia media del contorno de pitch del grupo acentual.         
 */

float calcula_frecuencia_media_grupo(Frecuencia_estilizada *frecuencia) {

  int cont;
  float media = 0.0;
  int numero_datos = 0;

  estructura_marcas_frecuencia *recorre_marcas = frecuencia->marcas;

  for (cont = 0; cont < frecuencia->numero_marcas; cont++, recorre_marcas++) {

    if (recorre_marcas->sonoridad_pobre == 0) {
      numero_datos++;
      media += recorre_marcas->frecuencia;
    }

  } // for (cont...

  if (numero_datos)
    return  media / numero_datos;

  for (cont = 0, recorre_marcas = frecuencia->marcas; cont < frecuencia->numero_marcas;
       cont++, recorre_marcas++)
    media += recorre_marcas->frecuencia;

  return media / frecuencia->numero_marcas;

}


/**
 * Función:   encuentra_maxima_variacion_absoluto_grupo                             
 * \remarks Entrada:
 *			- frecuencia: estructura en la que está la información del contorno de  
 *            pitch.                                                                
 * \remarks Valor devuelto:                                                                  
 *          - La máxima diferencia (en valor absoluto) entre valores consecutivos   
 *			  del contorno.	Se emplea como un posible indicador de si el contorno   
 *			  se corresponde realmente con un grupo acentual.								
 */

float encuentra_maxima_variacion_absoluto_grupo(Frecuencia_estilizada *frecuencia) {

  int cont;
  float diferencia = INT_MAX, maximo = -INT_MAX;

  estructura_marcas_frecuencia *recorre_marcas = frecuencia->marcas;

  for (cont = 0; cont < frecuencia->numero_marcas - 1; cont++, recorre_marcas++) {

    if (recorre_marcas->sonoridad_pobre == 0) {
      diferencia = absoluto(recorre_marcas->frecuencia - (recorre_marcas + 1)->frecuencia);
      if (diferencia > maximo)
	maximo = diferencia;
    }

  } // for (cont...

  if (diferencia == INT_MAX)
    return 0.0;
  else
    return maximo;
    
}


/**
 * Función:   calcula_frecuencia_maxima_grupo                                       
 * \remarks Entrada:
 *			- frecuencia: estructura en la que está la información del contorno de  
 *            pitch.                                                                
 * \remarks Valor devuelto:                                                                  
 *          - El índice del valor más alto de pitch en la cadena de marcas.         
 */

short int calcula_frecuencia_maxima_grupo(Frecuencia_estilizada *frecuencia) {

  short int indice_maximo = 0;
  float maximo_pitch = -INT_MAX;
  estructura_marcas_frecuencia *recorre_marcas = frecuencia->marcas;

  for (short int contador = 0; contador < frecuencia->numero_marcas; contador++, recorre_marcas++)
    if (recorre_marcas->frecuencia >= maximo_pitch) {
      maximo_pitch = recorre_marcas->frecuencia;
      indice_maximo = contador;
    }

  return indice_maximo;

}


/**
 * Función:   calcula_frecuencia_maxima_grupo_absoluto                              
 * \remarks Entrada:
 *			- frecuencia: estructura en la que está la información del contorno de  
 *            pitch.                                                                
 * \remarks Valor devuelto:                                                                  
 *          - El índice del valor más alto del contorno en valor absoluto.          
 */

short int calcula_frecuencia_maxima_grupo_absoluto(Frecuencia_estilizada *frecuencia) {

  short int indice_maximo = 0;
  float maximo_pitch = -INT_MAX;
  estructura_marcas_frecuencia *recorre_marcas = frecuencia->marcas;

  for (short int contador = 0; contador < frecuencia->numero_marcas; contador++, recorre_marcas++)
    if (absoluto(recorre_marcas->frecuencia) >= maximo_pitch) {
      maximo_pitch = absoluto(recorre_marcas->frecuencia);
      indice_maximo = contador;
    }

  return indice_maximo;

}

/**
 * Función:   calcula_factor_caida                                                  
 * \remarks Entrada:
 *			- grupo: Grupo_acentual_candidato del que queremos calcular el factor   
 *            de caida.                                                             
 * \remarks Valor devuelto:                                                                  
 *          - El factor de caída, considerado como la pendiente entre el punto en   
 *            el que recae el acento y el último.                                   
 * \remarks NOTA:
 *			- Podría ser interesante comprobar la influencia de tomar el punto real 
 *            más alto del contorno, en lugar del punto donde recae el acento.      
 */

float calcula_factor_caida(Grupo_acentual_candidato *grupo) {

  Frecuencia_estilizada frecuencia = grupo->frecuencia;
  short int posicion_acento = (short int) (frecuencia.numero_marcas -
					   grupo->posicion_silaba_acentuada - 2);
  float pitch_acento = frecuencia.marcas[posicion_acento].frecuencia;
  float tiempo_acento = frecuencia.marcas[posicion_acento].tiempo;
  float pitch_final = frecuencia.marcas[frecuencia.numero_marcas - 1].frecuencia;
  float tiempo_final = frecuencia.marcas[frecuencia.numero_marcas - 1].tiempo;

  return ( (pitch_final - pitch_acento) / (tiempo_final - tiempo_acento));

}


/**
 * Función:   calcula_salto_f0
 * \brief	Función que calcula el salto de f0 del grupo acentual
 * \param [in] grupo	Grupo_acentual_candidato del que queremos calcular el salto de f0
 * \param [out]	salto_f0	El salto de f0, calculado como la diferencia entre el valor de f0 en la sílaba acentuada y en la sílaba anterior, calculados sobre la componenente de acento del grupo acentual (la componente de frase se elimina al llamar a modifica_contorno_grupo_acentual_natural).
 * \return	
 */

int calcula_salto_f0(Grupo_acentual_candidato *grupo, float *salto_f0) {

  int indice_silaba_acentuada = grupo->numero_silabas - grupo->posicion_silaba_acentuada;
  Frecuencia_estilizada correcaminos;
	
  if ( (correcaminos.marcas = (estructura_marcas_frecuencia *) malloc(grupo->frecuencia.numero_marcas*sizeof(estructura_marcas_frecuencia)) ) == NULL) {
    fprintf(stderr, "Error en calcula_salto_f0, a asignar memoria.\n");
    return 1;
  }
  correcaminos.numero_marcas = grupo->frecuencia.numero_marcas;
  correcaminos.tiempo_inicio = grupo->frecuencia.tiempo_inicio;
  correcaminos.tiempo_final = grupo->frecuencia.tiempo_final;
	
  for (int i = 0; i < correcaminos.numero_marcas; i++)
    correcaminos.marcas[i] = grupo->frecuencia.marcas[i];
		
  modifica_contorno_grupo_acentual_natural(0, 0, &correcaminos);

  if (indice_silaba_acentuada)
    *salto_f0 = correcaminos.marcas[indice_silaba_acentuada].frecuencia -
      correcaminos.marcas[indice_silaba_acentuada - 1].frecuencia;
  else
    *salto_f0 = correcaminos.marcas[1].frecuencia -
      correcaminos.marcas[0].frecuencia;

  free(correcaminos.marcas);
	
  return 0;

}

/**
 * Función:   calcula_estadisticas_factor_caida                                     
 * \remarks Entrada:
 *			- fichero_salida: fichero en el que se van a escribir las estadísticas  
 *            de los factores de caída.                                             
 * \remarks Objetivo:  Esta función devuelve la media y la varianza de los factores de caída 
 *            de los clústeres de grupos acentuales.                                
 */

void calcula_estadisticas_factor_caida(FILE *fichero_salida) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int indice_pausas;
  int contador, reinicia_cuentas;
  double media[NUMERO_POSIBLES_PAUSAS];
  double varianza[NUMERO_POSIBLES_PAUSAS];
  double maximo[NUMERO_POSIBLES_PAUSAS], minimo[NUMERO_POSIBLES_PAUSAS];
  Grupo_acentual_candidato *recorre_unidades;
  int numero_unidades, numero_unidades_tipo[NUMERO_POSIBLES_PAUSAS];
  estructura_grupos_acentuales *unidad_seleccionada;


  fprintf(fichero_salida, "Proposición\tSílaba Tónica\tPosición\tTipo Pausa\tMedia\tVarianza\tMáximo\tMínimo\tNúmero de grupos\n\n");

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  for (reinicia_cuentas = 0; reinicia_cuentas < NUMERO_POSIBLES_PAUSAS; reinicia_cuentas++) {
	    varianza[reinicia_cuentas] = 0.0;
	    media[reinicia_cuentas] = 0.0;
	    maximo[reinicia_cuentas] = -1*FLT_MAX;
	    minimo[reinicia_cuentas] = FLT_MAX;
	    numero_unidades_tipo[reinicia_cuentas] = 0;
	  } // for (reinicia_cuentas

	  recorre_unidades = unidad_seleccionada->grupos;
	  numero_unidades = unidad_seleccionada->numero_grupos;

	  for (contador = 0; contador < numero_unidades; contador++, recorre_unidades++) {
	    indice_pausas = recorre_unidades->tipo_pausa;
	    numero_unidades_tipo[indice_pausas]++;
	    if (recorre_unidades->factor_caida > maximo[indice_pausas])
	      maximo[indice_pausas] = recorre_unidades->factor_caida;
	    if (recorre_unidades->factor_caida < minimo[indice_pausas])
	      minimo[indice_pausas] = recorre_unidades->factor_caida;

	    media[indice_pausas] += recorre_unidades->factor_caida;

	  } // for (contador = 0;...

	  for (reinicia_cuentas = 0; reinicia_cuentas < NUMERO_POSIBLES_PAUSAS; reinicia_cuentas++) {
	    if (numero_unidades_tipo[reinicia_cuentas]) {
	      media[reinicia_cuentas] /= numero_unidades_tipo[reinicia_cuentas];
	      numero_unidades_tipo[reinicia_cuentas] = 0;
	    } // if (numero_unidades_tipo...
	  } // for (reinicia_cuentas...

	  for (contador = 0, recorre_unidades = unidad_seleccionada->grupos;
	       contador < numero_unidades; contador++, recorre_unidades++) {
	    indice_pausas = recorre_unidades->tipo_pausa;
	    numero_unidades_tipo[indice_pausas]++;
	    varianza[indice_pausas] += (recorre_unidades->factor_caida - media[indice_pausas])*
	      (recorre_unidades->factor_caida - media[indice_pausas]);
	  }

	  for (reinicia_cuentas = 0; reinicia_cuentas < NUMERO_POSIBLES_PAUSAS; reinicia_cuentas++)
	    if (numero_unidades_tipo[reinicia_cuentas] > 1)
	      varianza[reinicia_cuentas] /= numero_unidades_tipo[reinicia_cuentas] - 1;
	    else
	      varianza[reinicia_cuentas] = 0.0;

	  for (reinicia_cuentas = 0; reinicia_cuentas < NUMERO_POSIBLES_PAUSAS; reinicia_cuentas++)
	    if (numero_unidades_tipo[reinicia_cuentas])
	      fprintf(fichero_salida, "%d\t%d\t%d\t%d\t%g\t%g\t%g\t%g\t%d\n", tipo_proposicion,
		      silaba_acentuada, posicion, reinicia_cuentas, media[reinicia_cuentas],
		      varianza[reinicia_cuentas], maximo[reinicia_cuentas], minimo[reinicia_cuentas],
		      numero_unidades_tipo[reinicia_cuentas]);

	} // if ( (unidad_seleccionada...


      } // for (posicion = 0;...


}


/**
 * Función:   calcula_frecuencia_media_grupos_acentuales                            
 *	\remarks Salida:
 *			- frecuencia: valor de la frecuencia media del corpus de grupos         
 *            acentuales.                                                           
 *          - varianza: varianza de dichos valores de pitch.                        
 *          - numero_valores: número de muestras consideradas para el cálculo.      
 * \remarks NOTA:
 *			- Para el cálculo de la frecuencia media no se tienen en cuenta los     
 *            valores con el campo sonoridad_pobre = 1.                             
 */

void calcula_frecuencia_media_grupos_acentuales(double *frecuencia,
                                                double *varianza, int *numero_valores) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int contador, cuenta_marcas;
  double media = 0.0;
  double varianza_local = 0.0;
  Grupo_acentual_candidato *recorre_unidades;
  int numero_unidades = 0;
  estructura_grupos_acentuales *unidad_seleccionada;
  estructura_marcas_frecuencia *marcas_frecuencia;

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice])  != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    marcas_frecuencia = recorre_unidades->frecuencia.marcas;
	    for (cuenta_marcas = 1; cuenta_marcas < recorre_unidades->frecuencia.numero_marcas - 1;
		 cuenta_marcas++, marcas_frecuencia++) {
	      if (marcas_frecuencia->sonoridad_pobre == 0) {
		media = (numero_unidades*media + marcas_frecuencia->frecuencia) / (numero_unidades + 1);
		numero_unidades++;
	      } // if (marcas_frecuencia->sonoridad_pobre)
	    } // for (cuenta_marcas = 1...
	  } // for (contador = 0...
	} // if ( (unidad_seleccionada...
      } // for (posicion = 0;...

  *frecuencia = media;
  *numero_valores = numero_unidades;

  // Y ahora, la varianza.

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    marcas_frecuencia = recorre_unidades->frecuencia.marcas;
	    for (cuenta_marcas = 1; cuenta_marcas < recorre_unidades->frecuencia.numero_marcas - 1;
		 cuenta_marcas++, marcas_frecuencia++) {
	      if (marcas_frecuencia->sonoridad_pobre == 0) {
		varianza_local += (marcas_frecuencia->frecuencia - media)*
		  (marcas_frecuencia->frecuencia - media);
	      } // if (marcas_frecuencia->sonoridad_pobre)
	    } // for (cuenta_marcas = 1...
	  } // for (contador = 0...
	} // if ( (unidad_seleccionada...
      } // for (posicion = 0;...

  *varianza = varianza_local / (numero_unidades - 1);

}

/**
 * Función:   crea_fichero_frecuencia_media_grupos_acentuales                       
 * \remarks Entrada:
 *			- fichero: fichero en el que se van a almacenar los datos de frecuencia 
 *            media.                                                                
 */

void crea_fichero_frecuencia_media_grupos_acentuales(FILE *fichero) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int contador;
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++)
	    fprintf(fichero, "%g\n", recorre_unidades->frecuencia_media);
	} // if ( (unidad_seleccionada...
      } // for (posicion = 0;...

}


/**
 * Función:   crea_fichero_frecuencia_media_grupos_acentuales_por_grupos            
 * \remarks Entrada:
 *			- fichero: fichero en el que se van a almacenar los datos de frecuencia 
 *            media.                                                                
 *			- opciones: "wt" o "wb", según se quiera escribir en formato binario o  
 *			  de texto.																
 * \remarks NOTA:
 *			- Se diferencia de la función anterior en que en este caso se calcula   
 *			  frecuencia media por grupo.											
 * \remarks Valor devuelto:																	
 *			- En ausencia de error, devuelve un cero.								
 */

int crea_fichero_frecuencia_media_grupos_acentuales_por_grupos(FILE *fichero,
							       char opciones[]) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int contador;
  float varianza, diferencia;
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;
  Media_varianza medias[NUMERO_GRUPOS_POSIBLES];

  for (contador = 0; contador < NUMERO_GRUPOS_POSIBLES; contador++) {
    medias[contador].media = 0.0;
    medias[contador].desviacion = 1.0;
  } // for (contador = 0; ...

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++)
	    medias[indice].media += recorre_unidades->frecuencia_media;
	  medias[indice].media /= unidad_seleccionada->numero_grupos;

	  varianza = 0.0;
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    diferencia = medias[indice].media - recorre_unidades->frecuencia_media;
	    varianza += diferencia*diferencia;
	  } // for (contador = 0; ...
	  medias[indice].desviacion = sqrt(varianza / unidad_seleccionada->numero_grupos);
	  if (medias[indice].desviacion == 0.0)
	    medias[indice].desviacion = 1.0;
	} // if ( (unidad_seleccionada...
      } // for (posicion = 0;...

  if (strcmp(opciones, "wb") == 0)
    fwrite(medias, sizeof(Media_varianza), NUMERO_GRUPOS_POSIBLES, fichero);
  else
    if (strcmp(opciones, "wt") == 0) {

      fprintf(fichero, "Proposición\tSílaba\tPosición\tMedia\tDesviación\n");

      for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
	for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
	  for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {
	    indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);
	    if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {
	      fprintf(fichero, "%d\t%d\t%d\t%f\t%f\n", tipo_proposicion, silaba_acentuada,
		      posicion, medias[indice].media, medias[indice].desviacion);
	    } // if ( (unidad_seleccionada...
	  } // for (posicion = 0;...

    } // if (strcmp(opciones, "wt") == 0)
    else {
      fprintf(stderr, "Error en crea_fichero_frecuencia_media_por_grupos: opciones \
            de entrada (%s) no válidas.\n", opciones);
      return 1;
    } // else

  return 0;

}

/**
 * Función:   escribe_datos_grupos_acentuales                                       
 * \remarks Entrada:
 *			- fichero: fichero en el que se van a almacenar los datos de los grupos 
 *            acentuales.                                                           
 */

void escribe_datos_grupos_acentuales(FILE *fichero) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int contador;
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;

  fprintf(fichero, "Ident\tDur\tFrecMedia\tFrecMax\tFacCaida\tPausa\tAcen\tPosGa\tPosGent\tTipoGrup\tNumSil\tGacGen\tNumGen\n");
    

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    fprintf(fichero, "%d\t", recorre_unidades->identificador);
	    fprintf(fichero, "%g\t", recorre_unidades->final - recorre_unidades->inicio);
	    fprintf(fichero, "%g\t", recorre_unidades->frecuencia_media);
	    fprintf(fichero, "%g\t",
		    recorre_unidades->frecuencia.marcas[recorre_unidades->indice_maximo].frecuencia);
	    fprintf(fichero, "%g\t", recorre_unidades->factor_caida);
	    fprintf(fichero, "%g\t", recorre_unidades->salto_f0);
	    fprintf(fichero, "%c\t", recorre_unidades->tipo_pausa + '0');
	    fprintf(fichero, "%c\t", recorre_unidades->posicion_silaba_acentuada + '0');
	    fprintf(fichero, "%d\t", recorre_unidades->posicion_grupo_acentual);
	    fprintf(fichero, "%c\t", recorre_unidades->posicion_grupo_prosodico + '0');
	    fprintf(fichero, "%c\t", recorre_unidades->tipo_grupo_prosodico_3 + '0');
	    fprintf(fichero, "%d\t", recorre_unidades->numero_silabas & 0xff);
	    fprintf(fichero, "%d\t", recorre_unidades->grupos_acentuales_grupo_prosodico & 0xff);
	    fprintf(fichero, "%d\n", recorre_unidades->numero_grupos_prosodicos & 0xff);
	  } // for (contador = 0;...
	} // if ( (unidad_seleccionada...

      } // for (posicion = 0...

}


/**
 * Función:   escribe_frecuencia_inicial_o_final_grupos_entonativos
 * \author	Fran Campillo.
 * \remarks	Función que escribe la f0 inicial o final del grupo entonativo según
 * los parámetros que se indican.
 * \param	nombre_locutor: nombre del locutor.
 * \param	indice: distancia al principio o al fin de la oración (en número de grupos
 * entonativos).
 * \param	opcion_f0: 0 => f0 al inicio. En otro caso => f0 al final.
 * \param	opcion_pausa: clasificación de grupos_acentuales.hpp.
 * \param	opcion_intervalo: 0 => la distancia es la indicada por el índice. En otro
 * caso, tiene que ser mayor.
 * \return	En ausencia de error, devuelve un cero.
 */

int escribe_frecuencia_inicial_o_final_grupos_entonativos(char *nombre_locutor, int indice, char opcion_f0, char opcion_pausa, char opcion_intervalo) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice_grupos;
  int contador;
  char nombre_fichero[FILENAME_MAX];
  const char *cadena_pausas[] = {"NP", "C", "P", "PS", "RE", "RC", "O"};
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;
  FILE *fichero;

  if ( (opcion_pausa < 0) || (opcion_pausa > PAUSA_OTRO_SIGNO_PUNTUACION) ) {
    fprintf(stderr, "Error en escribe_frecuencia_inicial_o_final_grupos: tipo de pausa escogido (%d)\
 no válido.\n", opcion_pausa);
    return 1;
  }

  if (opcion_f0 == 0)
    sprintf(nombre_fichero, "%s_f0_inicial_%s_%d.txt", nombre_locutor, cadena_pausas[opcion_pausa], indice);
  else
    sprintf(nombre_fichero, "%s_f0_final_%s_%d.txt", nombre_locutor, cadena_pausas[opcion_pausa], indice);

  if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
    fprintf(stderr, "Error en escribe_frecuencia_inicial_o_final_grupos_entonativos, al intentar crear el \
fichero %s.\n", nombre_fichero);
    return 1;
  }
    
  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	if (posicion == GRUPO_INICIAL_Y_FINAL)
	  continue;
                    
	//                if (tipo_proposicion == FRASE_INTERROGATIVA)
	//                	continue;
                    
	indice_grupos = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice_grupos]) != NULL) {
	  recorre_unidades = unidad_seleccionada->grupos;
	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {

	    if (opcion_f0 == 0) {

	      if (recorre_unidades->tipo_pausa_anterior != opcion_pausa)
		continue;

	      if (opcion_intervalo == 0) {
		if (recorre_unidades->posicion_grupo_prosodico != indice)
		  continue;
	      }
	      else {
		if (recorre_unidades->posicion_grupo_prosodico < indice)
		  continue;
	      }

	      fprintf(fichero, "%g\n", recorre_unidades->frecuencia.marcas[0].frecuencia);

	    }
	    else {

	      if (recorre_unidades->tipo_pausa != opcion_pausa)
		continue;

	      if (opcion_intervalo == 0) {
		if (recorre_unidades->numero_grupos_prosodicos - recorre_unidades->posicion_grupo_prosodico - 1 != indice)
		  continue;
	      }
	      else {
		if (recorre_unidades->numero_grupos_prosodicos - recorre_unidades->posicion_grupo_prosodico - 1 < indice)
		  continue;
	      }

	      fprintf(fichero, "%g\n", recorre_unidades->frecuencia.marcas[recorre_unidades->numero_silabas + 1].frecuencia);

	    }

	  } // for (contador = 0;...

	} // if ( (unidad_seleccionada...

      } // for (posicion = 0...

  fclose(fichero);
    
  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función encargada de calcular la distancia entre dos contornos de frecuencia,
 * a partir de la distancia cuadrática en los puntos medios de la vocal fuerte de cada sílaba.
 * \param	contorno_1: primer contorno de frecuencia.
 * \param	contorno_2: segundo contorno de frecuencia.
 * \param	numero_grupos_acentuales: número de grupos de los contornos anteriores.
 * \retval  distancia: distancia entre los contornos.
 * \return	En ausencia de error, devuelve un cero.
 */

int calcula_distancia_entre_contornos_cuadratica(Grupo_acentual_candidato *contorno_1,
						 Grupo_acentual_candidato *contorno_2,
						 int numero_grupos_acentuales,
						 float *distancia) {

  int numero_silabas_1, numero_silabas_2;
  int divide_1, divide_2;
  float diferencia, minimo, parcial, pos_objetivo;
  float distancia_local = 0.0, distancia_local_grupo;
  estructura_marcas_frecuencia *marcas_1, *marcas_2;

  for (int contador = 0; contador < numero_grupos_acentuales;
       contador++, contorno_1++, contorno_2++) {

    marcas_1 = contorno_1->frecuencia.marcas + 1; // La primera se corresponde con el
    // valor de inicio del grupo, no con la primera sílaba.

    if ( (numero_silabas_1 = contorno_1->numero_silabas) == 0) {
      fprintf(stderr, "Error en calcula_distancia_entre_contornos: número de sílabas \
            nulo en el grupo %d.\n", contorno_1->identificador);
      return 1;
    } // if ( (numero_silabas_1 ...

    if (numero_silabas_1 > 1)
      divide_1 = numero_silabas_1 - 1;
    else
      divide_1 = 1;

    if ( (numero_silabas_2 = contorno_2->numero_silabas) == 0) {
      fprintf(stderr, "Error en calcula_distancia_entre_contornos: número de sílabas \
            nulo en el grupo %d.\n", contorno_2->identificador);
      return 1;
    } // if ( (numero_silabas_1 ...

    if (numero_silabas_2 > 1)
      divide_2 = numero_silabas_2 - 1;
    else
      divide_2 = 1;

    distancia_local_grupo = 0.0;
        
    for (int cuenta_silabas = 0; cuenta_silabas < numero_silabas_1;
	 cuenta_silabas++, marcas_1++) {

      // Dado que puede que no tengan el mismo número de sílabas, es preciso alinear:

      minimo = FLT_MAX;
      pos_objetivo = (float) cuenta_silabas / divide_1;

      for (int cuenta_silabas_2 = 0; cuenta_silabas_2 < numero_silabas_2;
	   cuenta_silabas_2++) {
	if ( (parcial = absoluto(((float) cuenta_silabas_2 / divide_2) - pos_objetivo)) < minimo) {

	  marcas_2 = contorno_2->frecuencia.marcas + cuenta_silabas_2 + 1;
	  minimo = parcial;
	} // if ( (parcial = absoluto(...

      } // for (int cuenta_silabas_2 ...

      diferencia = marcas_1->frecuencia - marcas_2->frecuencia;

      distancia_local_grupo += diferencia*diferencia;

    } // for (int cuenta_silabas = 0; ...

    distancia_local += distancia_local_grupo / numero_silabas_1;

  } // for (int contador = 0; contador < numero_grupos_acentuales...

  *distancia = distancia_local;

  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función encargada de calcular la distancia entre dos contornos de frecuencia,
 * a partir de la distancia cuadrática en los puntos medios de la vocal fuerte de cada sílaba.
 * \param	contorno_1: primer contorno de frecuencia.
 * \param	contorno_2: segundo contorno de frecuencia.
 * \param	numero_grupos_acentuales: número de grupos de los contornos anteriores.
 * \retval  distancia: distancia rms entre los contornos.
 * \retval	distancia_media: distancia media entre los contornos.
 * \return	En ausencia de error, devuelve un cero.
 */

int calcula_distancia_entre_contornos_cuadratica(Grupo_acentual_candidato *contorno_1,
						 Grupo_acentual_candidato *contorno_2,
						 int numero_grupos_acentuales,
						 float *distancia,
						 float *distancia_media) {

  int numero_silabas_1, numero_silabas_2;
  int divide_1, divide_2;
  float diferencia, minimo, parcial, pos_objetivo;
  float distancia_local = 0.0, distancia_local_grupo;
  float distancia_local_media = 0.0, distancia_local_media_grupo;
  estructura_marcas_frecuencia *marcas_1, *marcas_2 = NULL;

  for (int contador = 0; contador < numero_grupos_acentuales;
       contador++, contorno_1++, contorno_2++) {

    marcas_1 = contorno_1->frecuencia.marcas + 1; // La primera se corresponde con el
    // valor de inicio del grupo, no con la primera sílaba.

    if ( (numero_silabas_1 = contorno_1->numero_silabas) == 0) {
      fprintf(stderr, "Error en calcula_distancia_entre_contornos: número de sílabas \
            nulo en el grupo %d.\n", contorno_1->identificador);
      return 1;
    } // if ( (numero_silabas_1 ...

    if (numero_silabas_1 > 1)
      divide_1 = numero_silabas_1 - 1;
    else
      divide_1 = 1;

    if ( (numero_silabas_2 = contorno_2->numero_silabas) == 0) {
      fprintf(stderr, "Error en calcula_distancia_entre_contornos: número de sílabas \
            nulo en el grupo %d.\n", contorno_2->identificador);
      return 1;
    } // if ( (numero_silabas_1 ...

    if (numero_silabas_2 > 1)
      divide_2 = numero_silabas_2 - 1;
    else
      divide_2 = 1;

    distancia_local_grupo = 0.0;
    distancia_local_media_grupo = 0.0;

    for (int cuenta_silabas = 0; cuenta_silabas < numero_silabas_1;
	 cuenta_silabas++, marcas_1++) {

      // Dado que puede que no tengan el mismo número de sílabas, es preciso alinear:

      minimo = FLT_MAX;
      pos_objetivo = (float) cuenta_silabas / divide_1;

      for (int cuenta_silabas_2 = 0; cuenta_silabas_2 < numero_silabas_2;
	   cuenta_silabas_2++) {
	if ( (parcial = absoluto(((float) cuenta_silabas_2 / divide_2) - pos_objetivo)) < minimo) {

	  marcas_2 = contorno_2->frecuencia.marcas + cuenta_silabas_2 + 1;
	  minimo = parcial;
	} // if ( (parcial = absoluto(...

      } // for (int cuenta_silabas_2 ...

      diferencia = marcas_1->frecuencia - marcas_2->frecuencia;

      distancia_local_media_grupo += diferencia;

      distancia_local_grupo += diferencia*diferencia;

    } // for (int cuenta_silabas = 0; ...

    distancia_local += distancia_local_grupo / numero_silabas_1;

    distancia_local_media += distancia_local_media_grupo / numero_silabas_1;
        
  } // for (int contador = 0; contador < numero_grupos_acentuales...

  *distancia = distancia_local;
  *distancia_media = distancia_local_media;

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función encargada de calcular la distancia entre dos contornos de frecuencia,
 * sumando 1 por cada grupo que difiera.
 * \param	contorno_1: primer contorno de frecuencia.
 * \param	contorno_2: segundo contorno de frecuencia.
 * \param	numero_grupos_acentuales: número de grupos de los contornos anteriores.
 * \retval  distancia: distancia entre los contornos.
 * \return	En ausencia de error, devuelve un cero.
 */

int calcula_distancia_entre_contornos_grupos(Grupo_acentual_candidato *contorno_1,
					     Grupo_acentual_candidato *contorno_2,
					     int numero_grupos_acentuales,
					     float *distancia) {

  int distancia_local = 0;

  for (int contador = 0; contador < numero_grupos_acentuales;
       contador++, contorno_1++, contorno_2++)

    if (contorno_1->identificador != contorno_2->identificador)
      distancia_local++;

  *distancia = (float) distancia_local;
    
  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función encargada de asignar a la variable estática del módulo
 * tabla_grupos un valor.
 */

void asigna_tabla_grupos(estructura_grupos_acentuales **nueva_tabla) {

  tabla_grupos = nueva_tabla;

}



/**
 * \author	Fran Campillo
 * \remarks	Función encargada de escribir en un fichero la información del corpus
 * prosódico cargado en memoria (Número total de sílabas, sílabas por palabra,
 * sílabas por grupo acentual, sílabas por grupo fónico, grupos acentuales,
 * grupos acentuales por grupo fónico, grupos acentuales iniciales, grupos
 * acentuales medios, grupos acentuales finales, grupos acentuales agudos, llanos y
 * esdrújulos, grupos acentuales en frases enunciativas, exclamativas, interrogativas e
 * inacabadas.
 * \param	fichero: puntero al fichero en el que se va a escribir la información.
 */

void escribe_informacion_corpus_prosodico(FILE *fichero) {

  int posicion, silaba_acentuada, tipo_proposicion;
  int indice;
  int contador;
  int numero_silabas = 0, numero_grupos_acentuales = 0;
  int numero_grupos_posicion[TIPOS_GRUPO_SEGUN_POSICION];
  int numero_grupos_acento[TIPOS_GRUPO_SEGUN_ACENTO];
  int numero_grupos_frase[NUMERO_TIPOS_PROPOSICIONES];

  double numero_grupos_entonativos = 0.0;

  for (contador = 0; contador < TIPOS_GRUPO_SEGUN_POSICION; contador++)
    numero_grupos_posicion[contador] = 0;
  for (contador = 0; contador < TIPOS_GRUPO_SEGUN_ACENTO; contador++)
    numero_grupos_acento[contador] = 0;
  for (contador = 0; contador < NUMERO_TIPOS_PROPOSICIONES; contador++)
    numero_grupos_frase[contador] = 0;
        
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;

  for (tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  recorre_unidades = unidad_seleccionada->grupos;

	  for (contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {

	    //                        if (recorre_unidades->identificador < 4099)
	    //                        	continue;

	    numero_grupos_acentuales += 1;
	    numero_grupos_frase[tipo_proposicion] += 1;
	    numero_grupos_acento[silaba_acentuada] += 1;
	    numero_grupos_posicion[posicion] += 1;

	    numero_silabas += recorre_unidades->numero_silabas;
	    numero_grupos_entonativos += 1.0 / recorre_unidades->grupos_acentuales_grupo_prosodico;

	  } // for (contador = 0;...
	} // if ( (unidad_seleccionada...

      } // for (posicion = 0...

  fprintf(fichero, "Número total de sílabas: %d.\n", numero_silabas);
  fprintf(fichero, "Número total de grupos acentuales: %d.\n", numero_grupos_acentuales);
  fprintf(fichero, "Numero total de grupos entonativos: %d.\n", (int) numero_grupos_entonativos);
  fprintf(fichero, "Número medio de sílabas por grupo acentual: %f.\n",
	  (float) numero_silabas / numero_grupos_acentuales);
  fprintf(fichero, "Número medio de sílabas por grupo fónico: %f.\n",
	  (float) numero_silabas / numero_grupos_entonativos);
  fprintf(fichero, "Número medio de grupos acentuales por grupo fónico: %f\n",
	  (float) numero_grupos_acentuales / numero_grupos_entonativos);

  fprintf(fichero, "Distribución de grupos acentuales según tipo de frase:");
  for (contador = 0; contador < NUMERO_TIPOS_PROPOSICIONES; contador++)
    fprintf(fichero, "\t%d", numero_grupos_frase[contador]);

  fprintf(fichero, "\nDistribución de grupos acentuales según el acento:");
  for (contador = 0; contador < TIPOS_GRUPO_SEGUN_ACENTO; contador++)
    fprintf(fichero, "\t%d", numero_grupos_acento[contador]);

  fprintf(fichero, "\nDistribución de grupos acentuales según la posición:");
  for (contador = 0; contador < TIPOS_GRUPO_SEGUN_POSICION; contador++)
    fprintf(fichero, "\t%d", numero_grupos_posicion[contador]);

}


/**
 * \brief Escribe las frecuencias de todos los grupos acentuales del corpus prosódico
 * \param[in] fichero Puntero al fichero en el que se desea escribir dicha información
 */

void escribe_frecuencias_corpus_prosodico(FILE *fichero) {

  int indice;
  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;
  estructura_marcas_frecuencia *apunta_contorno;

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {
	
	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);
	
	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  recorre_unidades = unidad_seleccionada->grupos;

	  for (int contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    
	    apunta_contorno = recorre_unidades->frecuencia.marcas;
	    
	    for (int cuenta_frecuencias = 0; cuenta_frecuencias < recorre_unidades->frecuencia.numero_marcas; cuenta_frecuencias++, apunta_contorno++) {
	      fprintf(fichero, "%f\n", apunta_contorno->frecuencia);
	    }
	    
	  } // for (contador = 0;...
	} // if ( (unidad_seleccionada...

      } // for (posicion = 0...

}

/**
 * \brief Adapta los valores de frecuencia de la base de datos prosódica a la media y rango dinámico indicados
 * \param[in] antigua_media Media de la base de datos original
 * \param[in] nueva_media Media deseada
 * \param[in] antiguo rango Rango dinámico de la base de datos original
 * \param[in] nuevo_rango Rango dinámico deseado
 */

void adapta_valor_medio_y_rango_dinamico(float antigua_media, float nueva_media, float antiguo_rango, float nuevo_rango) {

  int indice;

  float cociente = nueva_media / antigua_media;

  Grupo_acentual_candidato *recorre_unidades;
  estructura_grupos_acentuales *unidad_seleccionada;
  estructura_marcas_frecuencia *apunta_contorno;

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {
	
	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);
	
	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  recorre_unidades = unidad_seleccionada->grupos;

	  for (int contador = 0; contador < unidad_seleccionada->numero_grupos;
	       contador++, recorre_unidades++) {
	    
	    apunta_contorno = recorre_unidades->frecuencia.marcas;
	    
	    for (int cuenta_frecuencias = 0; cuenta_frecuencias < recorre_unidades->frecuencia.numero_marcas; cuenta_frecuencias++, apunta_contorno++) {
	      apunta_contorno->frecuencia = (apunta_contorno->frecuencia - antigua_media)*cociente + nueva_media;
	    }

	    recorre_unidades->factor_caida = calcula_factor_caida(recorre_unidades);
	    calcula_salto_f0(recorre_unidades, &recorre_unidades->salto_f0);
	    recorre_unidades->frecuencia_media = calcula_frecuencia_media_grupo(&recorre_unidades->frecuencia);

	  } // for (contador = 0;...
	} // if ( (unidad_seleccionada...

      } // for (posicion = 0...

}


/**
 * Función:   lee_contorno_fichero                                                    
 * \remarks Entrada:																			  
 *			- opcion: si vale 0, se supone que el formato del fichero es el generado  
 *			por transcribe_acentual.exe. En otro caso, se supone que es formato		  
 *			generado por la función escribe_contorno_escogido_final, en el módulo	  
 *			info_estructuras.cpp.													  
 *			- fichero: puntero al fichero en el que se encuentra la información.	  
 *	\remarks Salida:																			  
 *			- grupos: cadena indexada con los grupos acentuales.					  
 *			- numero_acentuales: número de elementos de la cadena anterior.			  
 * \remarks Valor devuelto:																	  
 *			- En ausencia de error se devuelve un cero.								  
 * \remarks Objetivo:  Leer de un fichero la información de un conjunto de grupos acentuales,  
 *			  en el formato de salida de transcribe_acentual.exe.					  
 */

int lee_contorno_fichero(Grupo_acentual_candidato **grupos, int *numero_acentuales, int opcion, FILE *fichero) {

  int numero_actual = 0, memoria_reservada = 100;
  char cadena_auxiliar[2000]; // No me gustan demasiado estas cosas, pero bueno.
  Grupo_acentual_candidato *correcaminos;

  if ( (*grupos = (Grupo_acentual_candidato *) malloc(memoria_reservada*sizeof(Grupo_acentual_candidato)))
       == NULL) {
    fprintf(stderr, "Error en lee_contorno_fichero, al asignar memoria.\n");
    return 1;
  } // if ( (grupos  = ...


  if (opcion) { // eliminamos la frase fonética.
    fgets(cadena_auxiliar, 2000, fichero);
    fgets(cadena_auxiliar, 2000, fichero);
  }

  correcaminos = (Grupo_acentual_candidato *) *grupos;

  while (!feof(fichero)) {

    if (opcion) // eliminamos el grupo acentual.
      fgets(cadena_auxiliar, 2000, fichero);
            
    if (correcaminos->lee_datos_txt(fichero))
      return 1;

    if (++numero_actual >= memoria_reservada) {
      memoria_reservada += 100;

      if ( (*grupos = (Grupo_acentual_candidato *) realloc(grupos, memoria_reservada*sizeof(Grupo_acentual_candidato)))
	   == NULL) {
	fprintf(stderr, "Error en lee_contorno_fichero, al reasignar memoria.\n");
	return 1;
      } // if ( (grupos = ...

      correcaminos = (Grupo_acentual_candidato *) *grupos + numero_actual;

    } // if (++numero_actual >= memoria_reservada)
    else
      correcaminos++;

  } // while (!feof(fichero))

  if ( (*grupos = (Grupo_acentual_candidato *) realloc(*grupos, numero_actual*sizeof(Grupo_acentual_candidato)))
       == NULL) {
    fprintf(stderr, "Error en lee_contorno_fichero, al reasignar memoria.\n");
    return 1;
  } // if ( (grupos = ...

  *numero_acentuales = numero_actual;

  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función encargada de modificar los contornos de f0 de los grupos acentuales para
 * que no haya discontinuidades.
 * \param	acentuales: cadena de grupos acentuales, sobre la que se harán las modificaciones.
 * \param	numero_acentuales: número de elementos de la cadena anterior.
 */

void comprueba_y_asegura_continuidad_contornos(Grupo_acentual_candidato *acentuales, int numero_acentuales) {

  int contador;
  float f0_siguiente_grupo, f0_anterior_grupo;
  float f0_inicial, f0_final;
  char modifica = 0;
  Grupo_acentual_candidato *correcaminos = acentuales + 1;

  for (contador = 1; contador < numero_acentuales; contador++, correcaminos++) {

    f0_inicial = correcaminos->frecuencia.marcas->frecuencia;
    f0_final = (correcaminos->frecuencia.marcas + correcaminos->frecuencia.numero_marcas - 1)->frecuencia;
    f0_anterior_grupo = ((correcaminos - 1)->frecuencia.marcas + (correcaminos - 1)->frecuencia.numero_marcas - 1)->frecuencia;

    if ( (contador == numero_acentuales - 1) || (correcaminos->tipo_pausa == PAUSA_PUNTO) ||
	 (correcaminos->tipo_pausa == PAUSA_RUPTURA_COMA) || (correcaminos->tipo_pausa == PAUSA_COMA) ||
	 (correcaminos->tipo_pausa == PAUSA_PUNTOS_SUSPENSIVOS) ||
	 (correcaminos->tipo_pausa == PAUSA_OTRO_SIGNO_PUNTUACION) )
      // Para el último grupo tomamos como objetivo su última frecuencia.
      f0_siguiente_grupo = f0_final;
    else
      // Si no es el último, el primer valor del siquiente grupo.
      f0_siguiente_grupo = (correcaminos + 1)->frecuencia.marcas->frecuencia;

    //        if ( (absoluto(f0_inicial - f0_anterior_grupo) > MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS) ||
    //        	 (absoluto(f0_final - f0_siguiente_grupo) > MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS) )


    if ( (modifica || (absoluto(f0_inicial - f0_anterior_grupo) > MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS)) &&
	 ( ( (correcaminos - 1)->tipo_pausa == SIN_PAUSA) || ( (correcaminos - 1)->tipo_pausa == PAUSA_RUPTURA_ENTONATIVA)) ) {

      modifica_contorno_grupo_acentual(f0_anterior_grupo, f0_siguiente_grupo, &correcaminos->frecuencia);
      modifica = 0;
    }
    else
      if (absoluto(f0_final - f0_siguiente_grupo) > MAXIMA_DIFERENCIA_CONTINUIDAD_GRUPOS)
	modifica = 1;


  } // for (contador = 1;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que modifica el contorno de frecuencia de un grupo acentual para
 * asegurar la continuidad con los dos grupos que lo rodean.
 * \param	inicio: valor de f0 deseado al principio.
 * \param	fin: valor de f0 deseado al final.
 * \retval	contorno modificado.
 */

void modifica_contorno_grupo_acentual(float frecuencia_inicio, float frecuencia_fin,
				      Frecuencia_estilizada *contorno) {

  float diferencia_inicio, diferencia_fin, m, b; // y = m*x + b (Ecuación de la recta)
  estructura_marcas_frecuencia *marcas = contorno->marcas;


  //    diferencia_inicio = frecuencia_inicio - marcas->frecuencia;
  diferencia_inicio = log(frecuencia_inicio / marcas->frecuencia);
  //    diferencia_fin = frecuencia_fin - (marcas + contorno->numero_marcas - 1)->frecuencia;
  diferencia_fin = log(frecuencia_fin / (marcas + contorno->numero_marcas - 1)->frecuencia);

  m = (diferencia_fin - diferencia_inicio) / ((marcas + contorno->numero_marcas - 1)->tiempo - marcas->tiempo);
  b = diferencia_inicio - m*marcas->tiempo;

  for (int contador = 0; contador < contorno->numero_marcas; contador++, marcas++)
    //    	marcas->frecuencia += m*marcas->tiempo + b;
    marcas->frecuencia = exp(log(marcas->frecuencia) + m*marcas->tiempo + b);
}

/**
 * \author	Fran Campillo
 * \remarks	Función que modifica el contorno de frecuencia de un grupo acentual para
 * asegurar la continuidad con los dos grupos que lo rodean.
 * \param	inicio: valor de f0 deseado al principio.
 * \param	fin: valor de f0 deseado al final.
 * \retval	contorno modificado.
 */

void modifica_contorno_grupo_acentual_natural(float frecuencia_inicio, float frecuencia_fin,
					      Frecuencia_estilizada *contorno) {

  float diferencia_inicio, diferencia_fin, m, b; // y = m*x + b (Ecuación de la recta)
  estructura_marcas_frecuencia *marcas = contorno->marcas;

  diferencia_inicio = frecuencia_inicio - marcas->frecuencia;
  diferencia_fin = frecuencia_fin - (marcas + contorno->numero_marcas - 1)->frecuencia;

  m = (diferencia_fin - diferencia_inicio) / ((marcas + contorno->numero_marcas - 1)->tiempo - marcas->tiempo);
  b = diferencia_inicio - m*marcas->tiempo;

  for (int contador = 0; contador < contorno->numero_marcas; contador++, marcas++)
    marcas->frecuencia += m*marcas->tiempo + b;
}


/**
 * \author	Fran Campillo
 * \remarks	Función que calcula el subcoste relacionado con la estructura silábica (por los fonemas que la componen) para el coste de objetivo acentual. En algún momento habrá que considerar el tipo de sílaba anterior y posterior al grupo objetivo (en el candidato ya se tiene esa información). Tal vez merezca la pena hacer pruebas penalizando más las diferencias en la zona del acento.
 * \param[in]	silabas_objetivo cadena con la estructura silábica deseada.
 * \param[in]	numero_silabas_objetivo número de elementos de la cadena anterior.
 * \param[in]	silabas_candidato cadena con la estructura silábica candidata.
 * \param[in]	numero_silabas_candidato número de elementos de la cadena anterior.
 * \param[in]	posicion_silaba_acentuada posición de la sílaba acentuada (0 -> Aguda, 1 -> Llana, 2 -> esdrújula)
 * \return	Coste relacionado con el intercambio de una estructura silábica por otra.
 */

float coste_estructura_silabica(estructura_silaba *silabas_objetivo, int numero_silabas_objetivo, estructura_marcas_frecuencia *silabas_candidato, int numero_silabas_candidato, int posicion_silaba_acentuada) {

  //	int contador;
  float coste = 0.0;

  // En esta nueva versión solamente se considera la sílaba del acento:

  silabas_objetivo += numero_silabas_objetivo - posicion_silaba_acentuada - 1;
  silabas_candidato += numero_silabas_candidato - posicion_silaba_acentuada;
  
  if (silabas_objetivo->tipo_silaba != silabas_candidato->tipo_silaba)
    coste += 1;
     
  /*  silabas_candidato += numero_silabas_candidato - posicion_silaba_acentuada;
      silabas_objetivo += numero_silabas_objetivo - posicion_silaba_acentuada - 1;
      if (silabas_objetivo++->tipo_silaba != silabas_candidato++->tipo_silaba)
      coste += 0.5;
      if (posicion_silaba_acentuada > 0)
      if (silabas_objetivo->tipo_silaba != silabas_candidato->tipo_silaba)
      coste += 0.5;

      // Hasta aquí se considera sólo la zona que rodea al acento:

      silabas_candidato++;

      if (numero_silabas_objetivo == numero_silabas_candidato) {
      for (contador = 0; contador < numero_silabas_objetivo - posicion_silaba_acentuada - 1;
      contador++, silabas_objetivo++, silabas_candidato++) {
      if (silabas_objetivo->tipo_silaba != silabas_candidato->tipo_silaba)
      coste += 0.1;
      } // for
      for (; contador < numero_silabas_objetivo; contador++, silabas_objetivo++, silabas_candidato++)
      if (silabas_objetivo->tipo_silaba != silabas_candidato->tipo_silaba)
      coste += 0.5; // Por poner algo. Antes 1.5
      } // if (numero_silabas_objetivo == numero_silabas_candidato)
      else {

      // Respetamos la zona del acento:

      estructura_marcas_frecuencia *recorre_candidato = silabas_candidato + numero_silabas_candidato - posicion_silaba_acentuada - 1;
      estructura_silaba *recorre_objetivo = silabas_objetivo + numero_silabas_objetivo - posicion_silaba_acentuada - 1;

      for (contador = 0; contador <= posicion_silaba_acentuada; contador++, recorre_objetivo++, recorre_candidato++)
      if (recorre_objetivo->tipo_silaba != recorre_candidato->tipo_silaba)
      coste += 0.5; // Antes 1.5

      // Ahora, la chapucilla (debería seguir el mismo esquema de interpolación por sílabas de crea_vector_frecuencias,
      // en info_estructuras.cpp)

      int indice_candidato;
      int silabas_candidato_antes_acento = numero_silabas_candidato - posicion_silaba_acentuada - 1;
      int silabas_objetivo_antes_acento = numero_silabas_objetivo - posicion_silaba_acentuada - 1;

      if (silabas_objetivo_antes_acento)

      if (silabas_candidato_antes_acento)

      for (contador = 0; contador < numero_silabas_objetivo - posicion_silaba_acentuada; contador++) {
      indice_candidato = devuelve_alineado_indices(contador, silabas_objetivo_antes_acento, silabas_candidato_antes_acento);

      if (silabas_objetivo[contador].tipo_silaba != silabas_candidato[indice_candidato].tipo_silaba)
      coste += 0.1;

      } // for (contador = 0; contador < ...

      else

      for (contador = 0; contador < numero_silabas_objetivo - posicion_silaba_acentuada; contador++)
      if (silabas_objetivo[contador].tipo_silaba != silabas_candidato->tipo_silaba)
      coste += 0.5; // Antes 0.8

      } // else (numero_silabas_objetivo != numero_silabas_candidato)
  */

  return coste;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que devuelve, dentro de dos cadenas de índices, el índice más
 * próximo de la segunda al indicado en el primera, según sus posiciones relativas.
 * \param	indice_1: índice de la primera cadena.
 * \param	total_1: número de índices de la primera cadena.
 * \param	total_2: número de índices de la segunda cadena.
 * \return	Índice de la segunda cadena más próximo a indice_1 de la primera.
 */

int devuelve_alineado_indices(int indice_1, int total_1, int total_2) {

  int mejor_indice = 0;
  float posicion_relativa_1, posicion_relativa_2;
  float minimo = FLT_MAX, posible_minimo;

  posicion_relativa_1 = (float) indice_1 / total_1;

  for (int contador = 0; contador < total_2; contador++) {

    posicion_relativa_2 = (float) contador / total_2;

    if ( (posible_minimo = absoluto(posicion_relativa_1 - posicion_relativa_2)) < minimo) {
      minimo = posible_minimo;
      mejor_indice = contador;
    } // if (absoluto(posicion_relativa_1 ...

  } // for (contador = 0; ...

  return mejor_indice;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que calcula la continuidad de frecuencia fundamental entre grupos
 * acentuales de la base separados por una pausa.
 * \param	identificador: identificador del grupo acentual.
 * \retval	grupo_acentual: puntero al grupo acentual buscado. NULL si no se encuentra.
 * \return	En ausencia de error devuelve un cero.
 */

int calcula_continuidad_f0_grupos_fonicos(FILE *fichero_distancias, FILE *fichero_no_encontrados) {

  int indice, contador;
  //    float distancia;
  float duracion_pausa;
  float frecuencia_anterior, frecuencia_posterior;
  Grupo_acentual_candidato *grupo_anterior, *correcaminos;
  estructura_grupos_acentuales *unidad_seleccionada;

  if (tabla_grupos == NULL) {
    fprintf(stderr, "Error en encuentra_grupo_acentual_corpus: tabla_grupos no inicializada.\n");
    return 1;
  } // if (tabla_grupos == NULL)

    // Escribimos la cabecera:

  //    fprintf(fichero_distancias, "TipoFrase\tTipoFrase2_Act\tNum_GF\tPos_GF_2\tGA_1\tGA_2\tAcento_1\tAcento_2\tFrecuencia_1\tDistancia\n");
  fprintf(fichero_distancias, "TipoFrase1\tNum_GF\tPos_GF_2\tGA_1\tGA_2\tAcento_1\tAcento_2\tDuracion\tFrecuencia_1\tFrecuencia_2\n");
  //    fprintf(fichero_distancias, "TipoFrase\tNum_GF\tPos_GF_2\tGA_1\tGA_2\tFrecuencia_1\tDistancia\n");

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	// Comprobamos que sea un grupo inicial de grupo fónico.

	if ( (posicion != GRUPO_INICIAL) && (posicion != GRUPO_INICIAL_Y_FINAL) )
	  continue;

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  for (contador = 0, correcaminos = unidad_seleccionada->grupos;
	       contador < unidad_seleccionada->numero_grupos; contador++, correcaminos++) {

	    // Comprobamos que no sea el primero de la frase:

	    if (correcaminos->posicion_grupo_prosodico == 0)
	      continue;

	    // Comprobamos que no se trate de ruptura prosódica:

	    if ( (correcaminos->tipo_pausa_anterior == PAUSA_RUPTURA_ENTONATIVA) ||
		 (correcaminos->tipo_pausa_anterior == PAUSA_RUPTURA_COMA) )
	      continue;
                            
	    if (encuentra_grupo_acentual_corpus(correcaminos->identificador - 1, &grupo_anterior))
	      return 1;

	    if (grupo_anterior == NULL)
	      fprintf(fichero_no_encontrados, "%d\n", correcaminos->identificador - 1);
	    else {
	      frecuencia_anterior = grupo_anterior->frecuencia.marcas[grupo_anterior->frecuencia.numero_marcas - 1].frecuencia;
	      frecuencia_posterior = correcaminos->frecuencia.marcas[0].frecuencia;
	      //                        	distancia = frecuencia_posterior - frecuencia_anterior;
	      duracion_pausa = correcaminos->frecuencia.marcas[0].tiempo -
		grupo_anterior->frecuencia.marcas[grupo_anterior->frecuencia.numero_marcas - 1].tiempo;
	      fprintf(fichero_distancias, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t%f\n",
		      //                            						grupo_anterior->identificador,
		      //                                                    correcaminos->identificador,
		      grupo_anterior->tipo_grupo_prosodico_3,
		      //                                                    correcaminos->tipo_grupo_prosodico_3,
		      //                                                    correcaminos->tipo_grupo_prosodico_2,
		      grupo_anterior->numero_grupos_prosodicos,
		      correcaminos->posicion_grupo_prosodico,
		      grupo_anterior->grupos_acentuales_grupo_prosodico,
		      correcaminos->grupos_acentuales_grupo_prosodico,
		      grupo_anterior->posicion_silaba_acentuada,
		      correcaminos->posicion_silaba_acentuada,
		      duracion_pausa,
		      frecuencia_anterior, frecuencia_posterior);
	      //                                                    distancia);

	    } // else // grupo encontrado.

	  } // for (int contador = 0, ...

	} // if (unidad_seleccionada = tabla_grupos...

      } // for (int posicion = ...


  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que devuelve un puntero al grupo acentual que se busca, según su
 * identificador. No se aprovecha la información del tipo de frase porque podría cambiar
 * de un grupo al siguiente.
 * \param	identificador: identificador del grupo acentual.
 * \retval	grupo_acentual: puntero al grupo acentual buscado. NULL si no se encuentra.
 * \return	En ausencia de error devuelve un cero.
 */

int encuentra_grupo_acentual_corpus(int identificador, Grupo_acentual_candidato **grupo_acentual) {

  int indice, contador;
  Grupo_acentual_candidato *correcaminos;
  estructura_grupos_acentuales *unidad_seleccionada;

  if (tabla_grupos == NULL) {
    fprintf(stderr, "Error en encuentra_grupo_acentual_corpus: tabla_grupos no inicializada.\n");
    return 1;
  } // if (tabla_grupos == NULL)

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  for (contador = 0, correcaminos = unidad_seleccionada->grupos;
	       contador < unidad_seleccionada->numero_grupos; contador++, correcaminos++) {
	    if (correcaminos->identificador == identificador) {
	      *grupo_acentual = correcaminos;
	      return 0;
	    } // if (correcaminos->identificador == ...

	  } // for (int contador = 0, ...

	} // if (unidad_seleccionada = tabla_grupos...

      } // for (int posicion = ...

  *grupo_acentual = NULL;

  return 0;

}


#ifdef _INCLUYE_SILABAS

/**
 * \author	Fran Campillo
 * \remarks	Función que consigue los parámetros para entrenar el salto de f0 entre una sílaba
 * y la siguiente, considerando únicamente la sílaba acentuada y la anterior.
 * \param	fichero: fichero en el que se escriben los resultados
 * \return	En ausencia de error devuelve un cero.
 */

int devuelve_parametros_salto_f0_silabico(FILE *fichero) {

  int indice, contador, indice_silaba_acentuada, banda;
  float duracion, salto_f0;
  char onset_ant, coda_ant, onset_acen, coda_acen;
  char onset[10], coda[10];
  clase_espectral tipo_vocal_acentuada;
  Grupo_acentual_candidato *correcaminos;
  estructura_grupos_acentuales *unidad_seleccionada;

  if (tabla_grupos == NULL) {
    fprintf(stderr, "Error en devuelve_parametros_salto_f0_silabico: base de datos no inicializada.\n");
    return 1;
  } // if (tabla_grupos == NULL)

    // Escribimos la cabecera:

  //	fprintf(fichero, "Onset_Ant\tCoda_Ant\tOnset_Acen\tCoda_Acen\tVocal_Acen\tPos_GA\tDur\tSalto_f0\n");

  fprintf(fichero, "Coda_Ant\tOnset_Acen\tVocal_Acen\tDur\tSalto_f0\n");

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {

	// Comprobamos que no sea fin de grupo fónico:

	if ( (posicion == GRUPO_FINAL) || (posicion == GRUPO_INICIAL_Y_FINAL) )
	  //				if (posicion != GRUPO_MEDIO)
	  continue;

	if (tipo_proposicion != FRASE_ENUNCIATIVA)
	  continue;

	if (silaba_acentuada != 1)
	  continue;

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  for (contador = 0, correcaminos = unidad_seleccionada->grupos;
	       contador < unidad_seleccionada->numero_grupos; contador++, correcaminos++) {

	    if (silaba_acentuada >= correcaminos->numero_silabas - 1)
	      continue;

	    // eliminamos la componente de frase (haciendo la aproximación de línea recta).

	    //	    modifica_contorno_grupo_acentual_natural(0, 0, &correcaminos->frecuencia);

	    indice_silaba_acentuada = correcaminos->numero_silabas - silaba_acentuada;

	    salto_f0 = correcaminos->salto_f0;

	    if (strcmp(correcaminos->frecuencia.marcas[correcaminos->numero_silabas + 1].silaba, "ou") == 0)
	      continue;

	    if (salto_f0 > 35)
	      continue;

	    if (salto_f0 < 5)
	      continue;

	    duracion = correcaminos->frecuencia.marcas[indice_silaba_acentuada].tiempo -
	      correcaminos->frecuencia.marcas[indice_silaba_acentuada - 1].tiempo;

	    if (devuelve_onset(correcaminos->frecuencia.marcas[indice_silaba_acentuada].silaba, onset))
	      return 1;

	    if (devuelve_caracteristicas_parte_silaba(onset, &onset_acen))
	      return 1;

	    if (devuelve_coda(correcaminos->frecuencia.marcas[indice_silaba_acentuada].silaba, coda))
	      return 1;

	    if (devuelve_caracteristicas_parte_silaba(coda, &coda_acen))
	      return 1;

	    if (devuelve_tipo_vocal_fuerte(correcaminos->frecuencia.marcas[indice_silaba_acentuada].silaba, &tipo_vocal_acentuada))
	      return 1;

	    if (devuelve_onset(correcaminos->frecuencia.marcas[indice_silaba_acentuada - 1].silaba, onset))
	      return 1;

	    if (devuelve_caracteristicas_parte_silaba(onset, &onset_ant))
	      return 1;

	    if (devuelve_coda(correcaminos->frecuencia.marcas[indice_silaba_acentuada - 1].silaba, coda))
	      return 1;

	    if (devuelve_caracteristicas_parte_silaba(coda, &coda_ant))
	      return 1;

	    banda = (int) salto_f0 / 8;

	    fprintf(fichero, "%d\t%d\t%d\t%f\t%d\n", coda_ant, onset_acen,
		    (int) tipo_vocal_acentuada, duracion, banda);


	  } // for (int contador = 0, ...

	} // if (unidad_seleccionada = tabla_grupos...

      } // for (int posicion = ...


  return 0;

}

#endif

#ifdef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL

/**
 * \author	Fran Campillo
 * \remarks	Función que escribe en un fichero la categoría morfosintáctica de la
 * palabra acentuada del grupo acentual y el salto de f0 de la sílaba pretónica
 * a la tónica, o bien el salto de f0 según el siguiente singtagma tras una
 * ruptura entonativa.
 * \param	fichero: fichero en el que se escriben los resultados
 * \param	categoria: 0 => categoría morfosintáctica de la palabra acentuada.
 *					1 => siguiente sintagma.
 *					2 => sintagma actual.
 *					3 => categoría, sintagma actual y sintagma siguiente.
 * \param	opcion: 0 => salto de f0.
 *                  1 => factor de caída.
 * \param	tipo_pausa: tipo de pausa que se busca (según la clasificación de
 * grupos_acentuales.hpp)
 * \return	En ausencia de error devuelve un cero.
 */

int devuelve_parametros_salto_f0_silabico_y_categoria_morfosintactica(FILE *fichero, char categoria,
								      char opcion, char tipo_pausa) {

  int indice, contador;//, indice_silaba_acentuada;
  float salto_f0;
  const char *categorias[] =
    {"GA_NOMBRE", "GA_ADJETIVO", "GA_VERBO", "GA_PRONOMBRE", "GA_ADVERBIO", "GA_INTERROGATIVO_EXCLAMATIVO", //"GA_EXCLAMATIVO",
     "GA_DETERMINANTE", "GA_OTROS"}; // Ojo, esta clasificación está en grupos_acentuales.hpp.

  const char *sintagmas[NUMERO_SINTAGMAS_MODELO_ENTONATIVO] =
    {"GA_S_NOMINAL", "GA_S_ADJETIVAL", "GA_S_VERBAL", "GA_S_PREPOSICIONAL",
     "GA_S_ADVERBIAL", "GA_COMA", "GA_RUPTURA_ENTONATIVA", "GA_RUPTURA_COMA", "GA_PUNTO", "GA_RELATIVO",
     "GA_CONJ_COOR_ADVERS", "GA_CONJ_COOR_COPU", "GA_CONJ_COOR_DIS", "GA_CONJ_SUBOR",
     "GA_APERTURA_PAR", "GA_CIERRE_PAR", "GA_APERTURA_INT_EXCLA", "GA_CIERRE_INT_EXCLA",
     "GA_NO_CLASIFICADO", "GA_DOBLES_COMILLAS", "GA_OTROS_SINTAGMAS"};

  Grupo_acentual_candidato *correcaminos;
  estructura_grupos_acentuales *unidad_seleccionada;

  if (tabla_grupos == NULL) {
    fprintf(stderr, "Error en devuelve_parametros_salto_f0_silabico: base de datos no inicializada.\n");
    return 1;
  } // if (tabla_grupos == NULL)

    // Escribimos la cabecera:

  //	fprintf(fichero, "Identificador\tTipoFrase\tPosicion\tAcento\tCategoria\tSalto\n");

  if (categoria == 3)
    fprintf(fichero, "Identificador\tCategoria\tSintagma\tSintagma_sig\tSalto\n");
  else
    fprintf(fichero, "Identificador\tCategoria\tSalto\n");

  for (int tipo_proposicion = 0; tipo_proposicion < NUMERO_TIPOS_PROPOSICIONES; tipo_proposicion++)
    for (int silaba_acentuada = 0; silaba_acentuada < TIPOS_GRUPO_SEGUN_ACENTO; silaba_acentuada++)
      for (int posicion = 0; posicion < TIPOS_GRUPO_SEGUN_POSICION; posicion++) {



	//                 if ( (categoria == 0) || (categoria == 3) ){
	// //                	if (tipo_pausa == SIN_PAUSA) {
	// //                    	if ( (posicion == GRUPO_FINAL) || (posicion == GRUPO_INICIAL_Y_FINAL) )
	// //                    		continue;
	// //                    }
	// //                    else
	// //						if ( (posicion != GRUPO_FINAL) && (posicion != GRUPO_INICIAL_Y_FINAL) )
	// //                        	continue;

	// 					if (tipo_proposicion != FRASE_ENUNCIATIVA)
	//             	    	continue;

	//                 else {
	// //                	if (tipo_pausa == SIN_PAUSA) {
	// //                    	if ( (posicion == GRUPO_FINAL) || (posicion == GRUPO_INICIAL_Y_FINAL) )
	// //                    		continue;
	// //                    }
	// //                    else
	// //						if ( (posicion != GRUPO_FINAL) && (posicion != GRUPO_INICIAL_Y_FINAL) )
	// //                        	continue;

	// //                    if (tipo_proposicion == FRASE_INTERROGATIVA)
	// //                    	continue;
	//                 }

	//                if (silaba_acentuada != 1)
	//                	continue;

	indice = calcula_indice_grupo(tipo_proposicion, posicion, silaba_acentuada);

	if ( (unidad_seleccionada = tabla_grupos[indice]) != NULL) {

	  for (contador = 0, correcaminos = unidad_seleccionada->grupos;
	       contador < unidad_seleccionada->numero_grupos; contador++, correcaminos++) {

	    if (correcaminos->tipo_pausa != tipo_pausa)
	      continue;

	    if (opcion == 0) {

	      if (silaba_acentuada >= correcaminos->numero_silabas - 1)
		continue;

	      // eliminamos la componente de frase (haciendo la aproximación de línea recta)

	      //	      modifica_contorno_grupo_acentual_natural(0, 0, &correcaminos->frecuencia);

	      //	      indice_silaba_acentuada = correcaminos->numero_silabas - silaba_acentuada;

	      salto_f0 = correcaminos->salto_f0;

	      if (categoria == 0)
		//                        fprintf(fichero, "%d\t%d\t%d\t%d\t%s\t%f\n", correcaminos->identificador, tipo_proposicion, posicion, silaba_acentuada,
		fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			categorias[correcaminos->etiqueta_morfosintactica], salto_f0);
	      else
		if (categoria == 1)
		  fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			  sintagmas[correcaminos->siguiente_sintagma], salto_f0);
		else
		  if (categoria == 2)
		    fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			    sintagmas[correcaminos->sintagma_actual], salto_f0);
		  else
		    fprintf(fichero, "%d\t%s\t%s\t%s\t%f\n", correcaminos->identificador,
			    categorias[correcaminos->etiqueta_morfosintactica],
			    sintagmas[correcaminos->sintagma_actual],
			    sintagmas[correcaminos->siguiente_sintagma], salto_f0);

	    }
	    else {

	      if (correcaminos->posicion_silaba_acentuada == GRUPO_AGUDO)
		continue;
                                
	      if (categoria == 0)
		//                        fprintf(fichero, "%d\t%d\t%d\t%d\t%s\t%f\n", correcaminos->identificador, tipo_proposicion, posicion, silaba_acentuada,
		fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			categorias[correcaminos->etiqueta_morfosintactica], correcaminos->factor_caida);
	      else
		if (categoria == 1)
		  fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			  sintagmas[correcaminos->siguiente_sintagma], correcaminos->factor_caida);
		else
		  if (categoria == 2)
		    fprintf(fichero, "%d\t%s\t%f\n", correcaminos->identificador,
			    sintagmas[correcaminos->sintagma_actual], correcaminos->factor_caida);
		  else
		    fprintf(fichero, "%d\t%s\t%s\t%s\t%f\n", correcaminos->identificador,
			    categorias[correcaminos->etiqueta_morfosintactica],
			    sintagmas[correcaminos->sintagma_actual],
			    sintagmas[correcaminos->siguiente_sintagma],
			    correcaminos->factor_caida);

	    }

	  } // for (int contador = 0, ...

	} // if (unidad_seleccionada = tabla_grupos...

      } // for (int posicion = ...


  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que escribe el tipo de categoría morfosintáctica de la
 * palabra acentuada del grupo acentual.
 * \param	categoria: número de la categoría, según la clasificación que hay al
 * inicio de grupos_acentuales.hpp
 * \retval	cadena_categoria: cadena legible por el usuario que representa esa
 * categoría.
 * \return	En ausencia de error devuelve un cero.
 */

int escribe_tipo_categoria_morfosintactica_grupo(int categoria, char cadena_categoria[]) {

  const char *categorias[CLASES_GRUPOS_ACENTUALES_SEGUN_CATEGORIA_MORFOSINTACTICA] =
    {"GA_NOMBRE", "GA_ADJETIVO", "GA_VERBO", "GA_PRONOMBRE", "GA_ADVERBIO", "GA_INTERROGATIVO_EXCLAMATIVO", "GA_DETERMINANTE", "GA_OTROS"};

  if ( (categoria >= CLASES_GRUPOS_ACENTUALES_SEGUN_CATEGORIA_MORFOSINTACTICA) ||
       (categoria < 0) ) {
    fprintf(stderr, "Error en escribe_tipo_categoria_morfosintactica_grupo: categoría no \
considerada.\n");
    return 1;
  }

  strcpy(cadena_categoria, categorias[categoria]);
    
  return 0;

}

/**
 * \author	Fran Campillo
 * \remarks	Función que lee el tipo de categoría morfosintáctica de la
 * palabra acentuada del grupo acentual.
 * \param	categoria: cadena con la categoría morfosintactica.
 * \retval	valor_categoria: valor de dicha categoría, según la 
 * clasificación de grupos_acentuales.hpp.
 * categoría.
 * \return	En ausencia de error devuelve un cero.
 */
 
int lee_tipo_categoria_morfosintactica_grupo(char *categoria, unsigned char *valor_categoria) {

  const char *categorias[CLASES_GRUPOS_ACENTUALES_SEGUN_CATEGORIA_MORFOSINTACTICA] =
    {"GA_NOMBRE", "GA_ADJETIVO", "GA_VERBO", "GA_PRONOMBRE", "GA_ADVERBIO",
     "GA_INTERROGATIVO_EXCLAMATIVO", "GA_DETERMINANTE", "GA_OTROS"};

  for (unsigned char cat = 0;
       cat < CLASES_GRUPOS_ACENTUALES_SEGUN_CATEGORIA_MORFOSINTACTICA; cat++)
    if (strcmp(categoria, categorias[cat]) == 0) {
      *valor_categoria = cat;
      return 0;
    }
   
  fprintf(stderr, "Error en lee_tipo_categoria_morfosintactica_grupo, categoria (%s) no considerada.\n", categoria);

  return 1;
   
}


/**
 * \author	Fran Campillo
 * \remarks	Función que escribe el tipo de sintagma según la clasificación
 * empleada en el modelo entonativo.
 * \param	sintagma: número de la categoría, según la clasificación que hay al
 * inicio de grupos_acentuales.hpp
 * \retval	cadena_sintagma: cadena legible por el usuario que representa ese
 * sintagma.
 * \return	En ausencia de error devuelve un cero.
 */

int escribe_tipo_sintagma_grupo(int sintagma, char cadena_sintagma[]) {

  const char *sintagmas[NUMERO_SINTAGMAS_MODELO_ENTONATIVO] =
    {"GA_S_NOMINAL", "GA_S_ADJETIVAL", "GA_S_VERBAL", "GA_S_PREPOSICIONAL",
     "GA_S_ADVERBIAL", "GA_COMA", "GA_RUPTURA_ENTONATIVA", "GA_RUPTURA_COMA", "GA_PUNTO", "GA_RELATIVO",
     "GA_CONJ_COOR_ADVERS", "GA_CONJ_COOR_COPU", "GA_CONJ_COOR_DIS", "GA_CONJ_SUBOR",
     "GA_APERTURA_PAR", "GA_CIERRE_PAR", "GA_APERTURA_INT_EXCLA", "GA_CIERRE_INT_EXCLA",
     "GA_NO_CLASIFICADO", "GA_DOBLES_COMILLAS", "GA_OTROS_SINTAGMAS"};


  if ( (sintagma >= NUMERO_SINTAGMAS_MODELO_ENTONATIVO) ||
       (sintagma < 0) ) {
    fprintf(stderr, "Error en escribe_tipo_sintagma_grupo: categoría no \
considerada.\n");
    return 1;
  }

  strcpy(cadena_sintagma, sintagmas[sintagma]);

  return 0;

}


/**
 * \author	Fran Campillo
 * \remarks	Función que lee el tipo de sintagma según la clasificación
 * empleada en el modelo entonativo.
 * \param	sintagma: categoría sintáctica.
 * \retval	valor_sintagma: valor de dicha categoría según la 
 * clasificación de grupos_acentuales.hpp
 * \return	En ausencia de error devuelve un cero.
 */

int lee_tipo_sintagma_grupo(char *sintagma, unsigned char *valor_sintagma) {

  const char *sintagmas[NUMERO_SINTAGMAS_MODELO_ENTONATIVO] =
    {"GA_S_NOMINAL", "GA_S_ADJETIVAL", "GA_S_VERBAL", "GA_S_PREPOSICIONAL",
     "GA_S_ADVERBIAL", "GA_COMA", "GA_RUPTURA_ENTONATIVA", "GA_RUPTURA_COMA", 
     "GA_PUNTO", "GA_RELATIVO", "GA_CONJ_COOR_ADVERS", "GA_CONJ_COOR_COPU", 
     "GA_CONJ_COOR_DIS", "GA_CONJ_SUBOR", "GA_APERTURA_PAR", "GA_CIERRE_PAR", 
     "GA_APERTURA_INT_EXCLA", "GA_CIERRE_INT_EXCLA",
     "GA_NO_CLASIFICADO", "GA_DOBLES_COMILLAS", "GA_OTROS_SINTAGMAS"};

  for (unsigned char val = 0; val < NUMERO_SINTAGMAS_MODELO_ENTONATIVO;
       val++)
    if (strcmp(sintagma, sintagmas[val]) == 0) {
      *valor_sintagma = val;
      return 0;
    }

  fprintf(stderr, "Error en lee_tipo_sintagma_grupo, sintagma (%s) no considerado.\n", sintagma);

  return 1;

}


/**
 * \author Fran Campillo
 * \brief Función que convierte una cadena de texto que representa un tipo 
 * de pausa al final de un grupo acentual (ver grupos_acentuales.hpp) en 
 * su equivalente numérico.
 * \param pausa: nombre de la pausa en formato legible por el usuario.
 * \retval val_pausa: valor numérico de dicha pausa.
 * \return En ausencia de error, devuelve un cero.
 */ 

int lee_tipo_pausa_grupo_acentual(char *pausa, unsigned char *val_pausa) {

  const char *pausas[NUMERO_POSIBLES_PAUSAS] = {"NP", "C", "P", "PPS", "R-E",
						"R-C", "PO"};

  for (unsigned char val = 0; val < NUMERO_POSIBLES_PAUSAS; val++)
    if (strcmp(pausa, pausas[val]) == 0) {
      *val_pausa = val;
      return 0;
    }

  fprintf(stderr, "Error en lee_tipo_pausa_grupo_acentual, pausa (%s) no considerada.\n", pausa);
  
  return 1;

}

#endif
