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
 
#ifndef ALOFONOS_H
#define ALOFONOS_H

// Fran Campillo:

typedef struct {

	int posicion; ///< posición dentro de la ruptura entonativa o ruptura-coma en la frase silabeada y acentuada.
	int tipo; ///< tipo de ruptura: 0 => ruptura entonativa, 1 => ruptura coma.

} Estructura_rupturas;

void insertar_string(char *pos_insertar, const char * insercion);
void detectar_comentario(char *comentario, unsigned char *pos_comentario,char *bufer_de_texto_a_sustituir,char *insertar_en);
char *insertar_comentario(char *comentario, unsigned char pos_comentario,char *frase_fonetica,t_regla *regla,char insertar_en,char *pos_actual_frase_sil);
int crea_cadena_rupturas(char *frase_sil, char **nueva_frase_sil, Estructura_rupturas **rupturas, int *numero_rupturas);
#endif

