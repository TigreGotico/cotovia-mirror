/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ram�n Pi�eiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigaci�n en Humanidades,
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
 

#include "info_corpus_voz.hpp"
#include "path_list.hpp"

/**
 * \brief Asigna memoria al array de caminos para cada iteraci�n
 * \param[in] tamano N�mero de elementos del array
 * \return En ausencia de error, 0
 */
int Path_list::inicia_path_list(int tamano) {

  if ( (path = new (nothrow) Nodo_path_list * [tamano]) == NULL) {
    fprintf(stderr, "Error en inicia_path_list, al asignar memoria.");
    return 1;
  }

  iteracion = -1;

  numero_objetivos = tamano;

  for (int contador = 0; contador < tamano; contador++)
    path[contador] = NULL;

  return 0;

}

/**
 * \brief Avanza a la siguiente iteraci�n del algoritmo, reservando memoria para el n�mero de candidatos que se le indica
 * \param[in] numero_unidades N�mero de candidatos de dicha iteraci�n
 * \return En ausencia de error, 0
 */
int Path_list::siguiente_iteracion(int numero_unidades) {

  if (++iteracion >= numero_objetivos) {
    fprintf(stderr, "Error en Path_list::siguiente_iteracion: iteraci�n (%d) m�s grande que n�mero de objetivos (%d).\n", iteracion, numero_objetivos);
    return 1;
  }

  numero_candidatos = numero_unidades;

  if ( (path[iteracion] = new (nothrow) Nodo_path_list [numero_candidatos]) == NULL) {
    fprintf(stderr, "Error en Path_list::siguiente_iteracion, al asignar memoria.\n");
    return 1;
  }

  return 0;

}


/**
 * \brief A�ade un nuevo nodo en la posici�n indicada, apuntando al nodo adecuado de la iteraci�n anterior
 * \param[in] posicion �ndice de la iteraci�n actual
 * \param[in] unidad Puntero al candidato que se a�ade
 * \param[in] camino Camino de la iteraci�n anterior que proporciona el mejor coste
 * \param[in] indice_contorno Para el c�mputo de contornos en paralelo, indica a qu� contorno se refiere
 * \return En ausencia de error, 0
 */

int Path_list::anhade_nodo(int posicion, Vector_semifonema_candidato *unidad, int camino, char izq_der, int indice_contorno) {

  Nodo_path_list *nuevo_nodo;

  if ( (posicion < 0) || (posicion >= numero_candidatos) ) {
    fprintf(stderr, "Error en Path_list::anhade_nodo: posici�n (%d) fuera de los l�mites permitidos (0 y %d).\n", posicion, numero_candidatos);
    return 1;
  }

  nuevo_nodo = path[iteracion] + posicion;

  nuevo_nodo->nodo.tipo_unidad = 0; // Indica que se trata de un semifonema
  nuevo_nodo->nodo.parametro_1 = izq_der;
  nuevo_nodo->nodo.parametro_2 = unidad->validez;
  nuevo_nodo->nodo.localizacion.vector = unidad;

  if (iteracion != 0)
    nuevo_nodo->enlace[indice_contorno] = path[iteracion - 1] + camino;
  
  return 0;

}

/**
 * \brief Devuelve el conjunto de unidades que se corresponden con ese camino
 * \param[in] indice �ndice del camino que se quiere recuperar
 * \param[out] numero_unidades N�mero de unidades que componen el camino �ptimo
 * \param[out] numero_real caminos N�mero real de candidatos que hay en esa iteraci�n
 * \param[in] indice_contorno �ndice del contorno que se quiere considerar
 * \return Array con el camino �ptimo pedidom, y NULL en caso de error
 */

estructura_matriz *Path_list::devuelve_camino(int indice, int *numero_unidades, int *numero_real_caminos, int indice_contorno) {

  estructura_matriz *camino_optimo, *apunta_camino;
  Nodo_path_list *apunta_nodo;
  map<int, Nodo_path_list *>::iterator it;

  if ( (indice < 0) || (indice > numero_candidatos) ) {
    fprintf(stderr, "Error en Path_list::devuelve_camino: camino pedido (%d) fuera de los l�mites (0 y %d).\n", indice, numero_candidatos);
    return NULL;
  }

  *numero_unidades = numero_objetivos;

  if (numero_real_caminos)
    *numero_real_caminos = numero_candidatos;

  if ( (camino_optimo = (estructura_matriz *) malloc(numero_objetivos*sizeof(estructura_matriz))) == NULL) {
    fprintf(stderr, "Error en Path_list::devuelve_camino, al asignar memoria.\n");
    return NULL;
  } // if ( (camino_optimo = ...

  apunta_camino =  camino_optimo + numero_objetivos - 1;

  apunta_nodo = path[iteracion] + indice;

  for (int contador = 1; contador < numero_objetivos; contador++, apunta_camino--) {
    *apunta_camino = apunta_nodo->nodo;
    //    printf("A�adimos nodo %d, (%d).\n", apunta_camino->localizacion.vector->identificador, apunta_nodo->nodo.localizacion.vector->identificador);
    if ( (it = apunta_nodo->enlace.find(indice_contorno)) != apunta_nodo->enlace.end())
      apunta_nodo = it->second;
    else
      if ( (it = apunta_nodo->enlace.find(CONTORNO_SORDO)) != apunta_nodo->enlace.end())
	apunta_nodo = it->second;
      else {
	fprintf(stderr, "Error en Path_list::devuelve_camino. Contorno (%d) no considerado.\n", indice_contorno);
	return NULL;
      }
    
  }
  
  *apunta_camino = apunta_nodo->nodo;
  //  printf("A�adimos nodo %d, (%d).\n", apunta_camino->localizacion.vector->identificador, apunta_nodo->nodo.localizacion.vector->identificador);

  // for (int cuenta = 0; cuenta < numero_objetivos; cuenta++) {
  //   printf("%d.- ", cuenta);
  //   printf("%d.\n", camino_optimo[cuenta].localizacion.vector->identificador);
  // }

  return camino_optimo;

}

/**
 * \brief Libera la memoria para la siguiente ejecuci�n del algoritmo
 */

void Path_list::libera_memoria() {

  if (path != NULL) {
    for (int contador = 0; contador < numero_objetivos; contador++)
      delete [] path[contador];
    delete [] path;
    path = NULL;
    numero_objetivos = 0;
  }

}
