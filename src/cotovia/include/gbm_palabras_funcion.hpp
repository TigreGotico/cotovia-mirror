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
* Contiene la clase gbm_palabras_funcion.
*/

#ifndef IF_PALABRAS_FUNCION
#define IF_PALABRAS_FUNCION

#include "gestor_busquedas_memoria.hpp"


typedef struct {
	char *palabra;
        char *xen;
        char *num;
        char *otros[N_CAT*2];
	char xenero;
	char numero;
        char *lema;
	unsigned char cat[N_CAT];
	unsigned char cat_desc[N_CAT];
}t_palabra_funcion;

/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con palabras función (conjunciones, pronombres, etcétera).
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_palabras_funcion proporciona manejo de ficheros de palabras función. El formato es, para cada línea:
*   * Entre 3 y n parámetros.
*/
class gbm_palabras_funcion:public gbm {

private:

        t_palabra_funcion *plf;


protected:

        void procesa(char *nombre);
public:
        gbm_palabras_funcion(void);
        gbm_palabras_funcion(char *nombre);
        unsigned char devuelveGenero();
        unsigned char devuelveNumero();
        char *devuelveLema();
        unsigned char *devuelveCategoriasAsignadas();
        unsigned char *devuelveCategoriasDescartadas();
//        int busca(char *nombre,t_frase_separada *frase_separada) {return -1;}
        int buscar(char *nombre,t_frase_separada *frase_separada) {return -1;};
};


#endif

