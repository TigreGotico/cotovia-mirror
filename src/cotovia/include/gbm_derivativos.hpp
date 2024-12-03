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
* Contiene la clase gbm_regexp.
*/

#ifndef IF_GBM_REGEXP
#define IF_GBM_REGEXP

#include <gestor_busquedas_memoria.hpp>



#define IF_REGEXP           200  // Numero de modelos por desinencia


typedef struct{
   char *regexp;
   char *tp[3];   //tipo de palabra.
   char *td[3];   //tipo de diccionario y tipo de palabra que se exige.
   char info[IF_REGEXP][10];
}t_regexp;                     


/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con desinencias verbales.
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_regexp proporciona manejo de ficheros de desinencias. El formato es, para cada línea:
*   -Entre 3 y n parámetros.
*   -El primer campo es la desinencia.
*   -Los campos pares se corresponden al código del tiempo verbal (1-216)
*   -Los campos impares se corresponden con un modelo de verbo.
*/
class gbm_regexp:public gbm {

private:
        t_regexp *rgp;
        char candidatos[20][20];
        gbm **diccionario;
        map <char,int> escoge;
        map <char,int> categoria;
        map <char,int> genero;
        map <char,int> numero;
        map <char,int> persona;                                

protected:
        void procesa(char *nombre);
public:
        gbm_regexp(void); /// Comentario chorras 1
        gbm_regexp(char *nombre, gbm **dic=NULL); /// Comentario chorras 2
//        int busca(char *nombre, int indice=0);
        int busca(char *patron,char tipo_busqueda);
        int buscar(char *nombre,t_frase_separada *frase_separada);
        char **devuelveCandidatos();
        char *devuelveLema() {return NULL;}

};

#endif
