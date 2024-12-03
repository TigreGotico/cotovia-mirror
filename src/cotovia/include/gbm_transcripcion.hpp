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
* \author fmendez
* \remark Contiene la clase gbm_transcripcion
*/

#ifndef GBM_TRANSCRIPCION_HPP
#define GBM_TRANSCRIPCION_HPP

#include "gestor_busquedas_memoria.hpp"

typedef struct {
	char * palabra;
	char * transcripciones;
	int n_transcripciones;
} t_transcripcion;

typedef struct {
	char * palabras;
	t_transcripcion * transcripciones; ///Aquí cargamos las transcripciones.
	int numero;    					///Aquí cargaremos la cantidad encontrada.
} t_tabla_transcripciones;

/**
*  \author fmendez
*  \brief Clase heredera de gbm especializada en tratar ficheros con transcripsiones.
*  \remarks Además del formato básico y depuración que proporciona como hija de gbm,
*   gbm_transcripciones proporciona manejo de ficheros de transcripciones fonéticas.
*/
class gbm_transcripcion:public gbm {
	private:
        t_transcripcion * tra;
	protected:
        void procesa(char * nombre);
	public:
        gbm_transcripcion(void);
        gbm_transcripcion(char * nombre);
        char *devuelveTranscripcion();
        char *devuelveLema() {return NULL;}
        virtual int buscar(char *nombre,t_frase_separada *frase_separada) {return -1;}; 
};

extern gbm_transcripcion *if_transcripcion;
#endif

