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
* \remark Contiene la clase gbm_abreviaturas.
*/

 

#ifndef IF_ABREVIATURAS
#define IF_ABREVIATURAS

#include "gestor_busquedas_memoria.hpp"



typedef struct {
        char *abreviatura;
        char *extension[2];
}t_abreviatura;

typedef struct {

        char *palabras;
	t_abreviatura *abreviaturas;   //Aquí cargamos las abreviaturas o lo que sea.
	int numero;    //Aqui cargaremos  la cantidad encontrada.

} t_tabla_abreviatura;

/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con abreviaturas.
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_abreviaturas proporciona manejo de ficheros de abreviaturas. El formato es, para cada línea:
*   * Entre 2 y 3 parámetros, debido a que una abreviatura puede tener dos extensiones (plural).
*/
class gbm_abreviaturas:public gbm {


private:

        t_abreviatura *abr;

protected:

        void procesa(char *nombre);


public:
        gbm_abreviaturas(void);
        gbm_abreviaturas(char *nombre);
        char *devuelveExtension();
        char *devuelveExtension(char numero);
        int buscar(char *nombre,t_frase_separada *frase_separada) {return -1;};
        char *devuelveLema() {return NULL;}


};


#endif
 

