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
* Contiene la clase gbm_sustantivos.
*/


#ifndef IF_SUSTANTIVOS
#define IF_SUSTANTIVOS

#include <cotovia2eagles.hpp>
#include <gestor_busquedas_memoria.hpp>
//#include <gbm_derivativos.hpp>


#define NUM_MAX_CATEGORIA 10

typedef struct{

   char *palabra;
   char *categoria[NUM_MAX_CATEGORIA];
   char *lema[NUM_MAX_CATEGORIA];
   
   char *xen;
   char *num;
/*
   char *lema;
*/   
   char xenero;
   char numero;
   

}t_nombre;





/**
*  \author Zaral Arelsiak.
*  \brief Clase heredera de gbm especializada en tratar ficheros con sustantivos y adjetivos.
*  \remark Además del formato básico y depuración que proporciona como hija de gbm,
* gbm_sustantivos proporciona manejo de ficheros de sustantivos (y adjetivos,
* pues siguen el mismo formato). El formato es, para cada línea, 3 parámetros:
* \par
*   -El primero es la palabra (sustantivo o adjetivo).
* \par
*   -El segundo será uno de MASCULINO/AMBIGUO/FEMININO/NO_ASIGNADO.
* \par
*   -El tercero será uno de SINGULAR/PLURAL/AMBIGUO/NO_ASIGNADO.
*/
class gbm_sustantivos:public gbm {


protected:
	t_nombre *nom;
//  gbm_regexp *gbmrgp;
  int categoria;

protected:

        void procesa(char *nombre);
        inline unsigned char traducirXeneroNumero(char *entrada);
        CotoviaEagles *g2c;
public:

        gbm_sustantivos(void);
        gbm_sustantivos(char *nombre,int cat=NOME);
//        gbm_sustantivos::gbm_sustantivos(char *nombre, char *nombreDiminutivos);
        unsigned char devuelveGenero();
        unsigned char devuelveNumero();
        char *devuelveLema();
        char *mGetLemma(int idx=0);
        int busca(char *nombre);
        ~gbm_sustantivos(void);
        int buscar(char *palabra,t_frase_separada *rec=NULL);
};

#endif

