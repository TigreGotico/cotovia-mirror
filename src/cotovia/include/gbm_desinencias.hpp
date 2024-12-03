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
* \author giglesia
* Contiene la clase gbm_desinencias.
*/

#ifndef IF_DESINENCIAS
#define IF_DESINENCIAS

#include "gestor_busquedas_memoria.hpp"



#define IF_NUM_MODEL_DES           200  // Numero de modelos por desinencia


typedef struct{                     // **** ESTRUCTURA DAS DESINENCIAS DA LISTA
   char *desinencia;                // Desinencia contida na estructura
//   char *inf[IF_NUM_MODEL_DES];
   char info[IF_NUM_MODEL_DES];        // Informacion das desinencias
}t_desinencia;                     // Tamanho max observado co xerador_taboas


/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con desinencias verbales.
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_desinencias proporciona manejo de ficheros de desinencias. El formato es, para cada línea:
*   -Entre 3 y n parámetros.
*   -El primer campo es la desinencia.
*   -Los campos pares se corresponden al código del tiempo verbal (1-216)
*   -Los campos impares se corresponden con un modelo de verbo.
*/
class gbm_desinencias:public gbm {


protected:

		t_desinencia *des;
		void procesa(char *nombre);
public:
        gbm_desinencias(void); /// Comentario chorras 1
        gbm_desinencias(char *nombre); /// Comentario chorras 2
        int buscar(char *nombre,t_frase_separada *frase_separada) {return -1;};
        char *devuelveLema() {return NULL;}         
};

#endif
