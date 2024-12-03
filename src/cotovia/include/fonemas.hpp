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
 


#ifndef _FONEMAS_HPP

#define _FONEMAS_HPP

#define NUMERO_INDICES 256
#define DERECHO '1'
#define IZQUIERDO '0'
#define INDICE_tS 29
#define INDICE_rr 30
//éstos son para el euskera

#ifdef _MODOA_EU //euskera
#define NUMERO_FONEMAS 40
#define OCTETOS_POR_FONEMA 4
#define INDICE_L 33
#define INDICE_c 34
#define INDICE_ts 35
#define INDICE_sbis 36
#define INDICE_gj 37
#define INDICE_jj 38
#define INDICE_tsbis 39
#else
#define NUMERO_FONEMAS 33
#define OCTETOS_POR_FONEMA 3
#define INDICE_L -1
#define INDICE_c -1
#define INDICE_ts -1
#define INDICE_sbis -1
#define INDICE_gj -1
#define INDICE_jj -1
#define INDICE_tsbis -1
#endif

#define TAMANO_UNIDAD 3*OCTETOS_POR_FONEMA

// Defines para el tipo de sonido.

#define SORDO 0
#define SONORO 1
  
#define calcula_indice_fonema_doble(primero, segundo) ((4*primero + segundo) % 128) + 128

//! Returns whether a phone is silence or voiceless plosive
int is_silence_or_voiceless_plosive(char *phone);

//! Returns whether a phone is a vowel
int is_vowel(char *phone);

//! Returns whether a phone is an appropriate boundary for the segmentwise unit selection sytem
int is_segment_boundary(char *phone);

int devuelve_valor_de_indice_fonema(int indice);

char *devuelve_puntero_a_nombre_fonema(int indice);

int devuelve_nombre_fonema(int indice, char *nombre_fonema);

int sordo_sonoro(char *alofono);

int devuelve_indice_fonema(const char *unidad);

int fonema_mayusculas(char fonema[]);

void conversion_de_fonemas(char nombre[]);


#endif


