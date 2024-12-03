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
* Contiene la clase gbm_locuciones.
*/

#ifndef IF_LOCUCIONES
#define IF_LOCUCIONES

#include "gestor_busquedas_memoria.hpp"
#include "sintagma.hpp"

#define N_MAX_LOC 5
#define N_MAX_PREP 5
#define N_MAX_PALABRAS 5

typedef struct {
   char *palabras;
   //char *preps[N_MAX_PREP];
   //char *t_c[N_MAX_PREP][N_MAX_LOC];
   //unsigned char tipo_conxuncion[N_MAX_PREP][N_MAX_LOC];
   //unsigned char tipo_conxuncion;
   unsigned char tipo_conxuncion[N_MAX_LOC];
   unsigned char num_pal_conxuncion;
   char *tonicidad;
} t_locucion;

/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con locuciones.
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_locuciones proporciona manejo de ficheros de locuciones. El formato es, para cada línea:
*   * Entre 3 y n parámetros:
*   * La locución
*   * Código de tonicidad
*   * Parámetro 3 en adelante, tipo de locución (LOC_PREP_MODO, ...)
*/
class gbm_locuciones:public gbm {

protected:

        t_locucion *resultadosPrimero,*resultadosUltimo,*resultadosMedio;
        char patr[4*LONX_MAX_PALABRA_PROCESADA];
        char frase[N_MAX_PALABRAS][N_MAX_PALABRAS*LONX_MAX_PALABRA_PROCESADA];

        void procesa(char *nombre);
        int buscarLocEspeciales(t_frase_separada *frase_separada,t_locucion *conxuncion, char *posibleLoc);
        void *buscarConxunciones2(char *patron);
        int buscarConxunciones(char* patron);
        t_locucion *refinaBusquedaLoc(char * patron);
public:
        gbm_locuciones(void);
        gbm_locuciones(char *nombre);
        int busca(char *patron);
        int busca(t_frase_separada *frase_separada);
        int buscar(char *nombre,t_frase_separada *frase_separada) {return -1;}
        char *devuelveLema() {return NULL;}        
};



#endif
