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
 

#include <stdio.h>

#include "tip_var.hpp"
#include "sintagma.hpp"
#include "rupturas_entonativas.hpp"

/**
 * \author	Fran Campillo.
 * \remarks	Función básica que se encarga de introducir rupturas entonativas en
 * el discurso. Únicamente se marcan los lugares en los que debe haber una
 * ruptura. Posteriormente, según los análisis sintáctico y morfosintáctico, el
 * módulo de selección de contornos entonativos se encargará de escoger los grupos
 * más adecuados.
 * \remarks	Esta primera versión es muy sencilla, únicamente introduce rupturas
 * antes de las conjunciones (siempre y cuando no haya una pausa en esa misma
 * posición). Esta aproximación es excesivamente simple para el problema, puesto que
 * puede haber frases muy largas sin conjunciones. Es necesario desarrollar un
 * modelo de inserción de rupturas entonativas para esos casos.
 * \param	frase_sintagmada: cadena con la información de los sintagmas.
 * \param	frase_separada: cadena con la información de las palabras de la frase.
 * \param	frase_grupos: cadena con la información de la frase organizada en grupos.
 * Es al final de cada uno de estos grupos donde se considera la posibilidad de
 * introducir la ruptura entonativa. De todas formas, cuando se introduzca el nuevo
 * modelo, habrá que cambiar esto, puesto que podrá haber rupturas dentro de cada
 * grupo.
 */

void introduce_rupturas_entonativas(t_frase_sintagmada *frase_sintagmada, t_frase_separada *frase_separada,
		t_frase_en_grupos *frase_grupos) {

	int contador_grupos = 0;
	t_frase_sintagmada *apunta_frase_sintagmada;

	while ( (contador_grupos < LONX_MAX_FRASE_EN_GRUPOS) &&
			(	(frase_grupos->inicio != 0) ||
				(frase_grupos->fin != 0) ||
				(contador_grupos == 0) ) 	) {

		apunta_frase_sintagmada = frase_sintagmada + frase_grupos->fin;

		switch ((apunta_frase_sintagmada + 1)->tipo_sintagma) {

			case NEXO_SUBORDINANTE:
			case CONX_SUBOR:
			case CONX_COOR_COPU:
			case CONX_COOR_DISX:
			case CONX_SUBOR_CONDI:
			case CONX_SUBOR_CAUS:
			case CONX_SUBOR_CONCES:
			case CONX_SUBOR_CONSE:
			case CONX_SUBOR_TEMP:
			case CONX_SUBOR_LOCA:
			case CONX_SUBOR_COMPAR:
			case CONX_SUBOR_MODAL:
			case CONX_COOR_ADVERS:
			case LOC_CONX_COOR_COPU:
			case LOC_CONX_COOR_ADVERS:
			case LOC_CONX_SUBOR_CAUS:
			case LOC_CONX_SUBOR_CONCES:
			case LOC_CONX_SUBOR_CONSE:
			case LOC_CONX_SUBOR_CONDI:
			case LOC_CONX_SUBOR_LOCAL:
			case LOC_CONX_SUBOR_TEMP:
			case LOC_CONX_SUBOR_MODA:

				if (frase_separada[apunta_frase_sintagmada->fin].pausa == 0)
					frase_grupos->ruptura_entonativa = 1;
				break;

			default:
				break;

		}

		contador_grupos++;
		frase_grupos++;

	}

}

