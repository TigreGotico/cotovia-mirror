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
 

/**
* \file
* Contiene la clase gbm_raices_verbales.
*/

#ifndef IF_RAICES_VERBALES
#define IF_RAICES_VERBALES

#include "gestor_busquedas_memoria.hpp"

#define IF_NUM_MODELOS              4  // Numero de modelos por infinitivo
#define IF_NUM_INFINITIVO           4  // Numero de infinitivos por raiz

typedef  struct{                    // **** ESTRUCTURA DA INFORMACION DE VERBOS
  char *inf; // Infinitivo que ten como raiz: raiz[]
  char grupos[IF_NUM_MODELOS];        // Modelos de conxugacion asociados a un verbo
} modelos2;                          // Tamanho max observado co xerador_taboas

typedef  struct{                    // **** ESTRUCTURA DOS VERBOS DA LISTA ****
  char *raiz;
  modelos2 modelo[IF_NUM_INFINITIVO];  // Estructura con informacion de verbos
} t_raices_verbales;               // Tamanho max observado co xerador_taboas

/**
 *  \author Zaral Arelsiak.
 *  \brief Clase heredera de gbm especializada en tratar ficheros con abreviaturas.
 *  \remark Además del formato básico y depuración que proporciona como hija de gbm,
 * gbm_raices_verbales proporciona manejo de ficheros de raíces verbales.
 * El formato es, para cada línea:
 *   * Entre 3 y n parámetros.
 */
class gbm_raices_verbales:public gbm {


protected:
  t_raices_verbales *rv;
  char lema[50];
  void procesa(char *nombre);
public:

        gbm_raices_verbales(char *nombre);
        gbm_raices_verbales(void);
        int buscar(char *nombre,t_frase_separada *frase_separada);
        char *devuelveLema();
};


#endif

