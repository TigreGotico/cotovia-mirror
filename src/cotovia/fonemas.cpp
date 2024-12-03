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
#include <stdlib.h>
#include <string.h>

#include "modos.hpp"
#include "fonemas.hpp"

char indices_fonemas[NUMERO_INDICES] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0 => 9
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 10 => 19
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 20 => 29
  -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, // 35 -> #
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 40 => 49
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 50 => 59
  -1, -1, -1, -1, -1, -1, 1, -1, 2, 3, //66->B,68->D,69->E
  -1, 4, -1, -1, 5, -1, INDICE_L, -1, 6, 7, //71->G,74->J,78->N,79->O
  -1, -1, -1, 8, 9, -1, -1, -1, -1, -1, //83->S,84->T
  10, -1, -1, -1, -1, -1, -1, 11, 12, INDICE_c, //90->Z,97->a...
  13, 14, 15, 16, -1, 17, 31, 18, 19, 20, // Añadimos las semivocales: j => 106 (índice 33), w => 119 (índice 34)
  21, 22, 23, -1, 24, 25, 26, 27, -1, 32, // 110 => 119 ('w' => 119)
  28, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 120 // 
  -1, -1, -1, -1, INDICE_gj, -1, -1, -1, -1, -1, // 130
  -1, -1, -1, -1, -1, -1, INDICE_jj, -1, -1, -1, // 140
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 150
  -1, -1, -1, INDICE_tS, -1, -1, -1, -1, -1, -1, // 160
  -1, -1, INDICE_sbis, -1, -1, -1, -1, -1, -1, -1, // 170
  -1, -1, -1, -1, -1, -1, INDICE_rr, -1, -1, -1, // 180
  -1, -1, -1, -1, -1, INDICE_ts, -1, -1, -1, -1, // 190
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 200
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 210
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 220
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 230
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 240
  -1, -1, -1, -1, -1, INDICE_tsbis}; // 250


char indices_inversos[][4] = {"#", "B", "D", "E", "G", "J", "N", "O", "S", "T", // 0 => 9
			      "Z", "a", "b", "d", "e", "f", "g", "i", "k", "l", // 10 => 19
			      "m", "n", "o", "p", "r", "s", "t", "u", "x", "tS", // 20 => 29
			      "rr","j", "w", "L","c","ts","s`","gj","jj","ts`"};


/**
 * \brief Function that returns whether a phone is a silence or a voiceless plosive
 * \param[in] phone phoneme
 * \return 1 if the phone is a silence or a voiceless plosive. 0 in other case.
 */
int is_silence_or_voiceless_plosive(char *phone) {

  if ( (phone[0] == '#') ||
       (phone[0] == 't') ||
       (phone[0] == 'k') ||
       (phone[0] == 'p') )
    return 1;
  
  return 0;

}


/**
 * \brief Function that returns whether a phone is an appropriate boundary for the segmentwise unit selection system
 * \param[in] phone phoneme
 * \return 1 if the phone is a silence, a voiceless fricative or a voiceless plosive. 0 in other case.
 */
int is_segment_boundary(char *phone) {

  if ( (phone[0] == '#') ||
       (phone[0] == 't') ||
       (phone[0] == 'k') ||
       (phone[0] == 'p') ||
       (phone[0] == 'f') ||
       (phone[0] == 's') ||
       (phone[0] == 'S') ||
       (phone[0] == 'T') ||
       (phone[0] == 'h') )

    return 1;
  
  return 0;

}

/**
 * \brief Function that returns whether a phone is a vowel
 * \param[in] phone phoneme
 * \return 1 if the phone is a vowel
 */
int is_vowel(char *phone) {

  if ( (phone[0] == 'a') ||
       (phone[0] == 'e') ||
       (phone[0] == 'E') ||
       (phone[0] == 'i') ||
       (phone[0] == 'o') ||
       (phone[0] == 'O') ||
       (phone[0] == 'u') )
    return 1;
  
  return 0;

}

/**
 * Función:   devuelve_puntero_a_nombre_fonema                                
 * \remarks Entrada:
 *			- indice: índice del fonema en la cadena indices_inversos.        
 * \remarks Valor devuelto:                                                            
 *          - Puntero al nombre del fonema, y NULL en caso de error.		  
 */

char *devuelve_puntero_a_nombre_fonema(int indice) {

    if ( (indice >= NUMERO_FONEMAS) || (indice < 0) ) {
        fprintf(stderr, "Error en devuelve_nombre_fonema: Índice (%d) no válido.\n", indice);
        return NULL;
    } // if ( (indice >= NUMERO_FONEMAS) ...

    return (char *) indices_inversos[indice];

}

/**
 * Función:   devuelve_nombre_fonema                                          
 * \remarks Entrada:
 *			- indice: índice del fonema en la cadena indices_inversos.        
 *	\remarks Salida:
 *			- nombre_fonema: representación gráfica del alófono.              
 * \remarks Valor devuelto:                                                            
 *          - En ausencia de error devuelve un cero.                          
 */

int devuelve_nombre_fonema(int indice, char *nombre_fonema) {

    if ( (indice >= NUMERO_FONEMAS) || (indice < 0) ) {
        fprintf(stderr, "Error en devuelve_nombre_fonema: Índice (%d) no válido.\n", indice);
        return 1;
    } // if ( (indice >= NUMERO_FONEMAS) ...

    strcpy(nombre_fonema, (char *) indices_inversos[indice]);

    return 0;

}

/**
 * Función:   sordo_sonoro                                                    
 * \remarks Entrada:
 *			- alofono: alofono del que se quiere conocer la sonoridad.        
 * \remarks Valor devuelto:                                                            
 *          - Si el alófono se considera sordo, se devuelve un 0. En caso     
 *            contrario, un 1.                                                
 * \remarks NOTA:	  Se supone que el alófono es correcto dentro del código SAMPA.	  
 */

int sordo_sonoro(char *alofono) {

	switch (alofono[0]) {

		case '#':
		case 'p':
		case 't':
		case 'k':
#ifdef _MODOA_EU
		case 'c':
#endif
		case 'f':
		case 's':
		case 'S':
		case 'T':
		case 'x':
			return SORDO;
		default:
			return SONORO;
	}

}

/** * Función:   devuelve_valor_de_indice_fonema                                 
 * \remarks Entrada:
 *			- indice: índice del fonema en la tabla indices_fonemas.		  
 * Valor_devuelto:                                                            
 *          - Valor de la posición determinada por indice en la tabla.		  
 */

int devuelve_valor_de_indice_fonema(int indice) {

	if ( (indice >= NUMERO_INDICES) || (indice < 0) )
    	return -1;

    return indices_fonemas[indice];

}

/**
 * Función:   devuelve_indice_fonema                                          
 * \remarks Entrada:
 *			- unidad: nombre de la unidad cuyo índice en la tabla se quiere   
 *            conocer.                                                        
 * Valor_devuelto:                                                            
 *          - Índice de la unidad en la tabla indices_fonemas. Si no existe,  
 *            se devuelve un -1.                                              
 */

int devuelve_indice_fonema(const char *unidad) {

	if (unidad[1] == '\0'){
		return indices_fonemas[unidad[0]];
	}
#ifdef _MODOA_EU
	else if (unidad[0] == 't' && unidad[1] == 's' && unidad[2] == '`'){
		return INDICE_tsbis;
	}
#endif
	else if (unidad[2] != '\0'){
		return -1;
	}
	return indices_fonemas[calcula_indice_fonema_doble (unidad[0], unidad[1])];

}

/**
 * Función:   fonema_mayusculas                                                
 * \remarks Objetivo:  Se comprueba si la cadena de entrada se encuentra entre los      
 *            fonemas que se representan con mayúsculas. (Es necesario para    
 *            escribir correctamente el nombre de fichero en ms-dos, donde no  
 *            se diferencian las mayúsculas de las minúsculas.                 
 * \remarks Valor devuelto:                                                             
 *          - Se devuelve un 0 si no es una de las mayúsculas del inventario   
 *            de fonemas.                                                      
 */

int fonema_mayusculas(char fonema[]) {

    switch (fonema[0]) {

       case 'E':
       case 'O':
       case 'A':
       case 'X':
       case 'S':
       case 'G':
       case 'B':
       case 'D':
       case 'N':
       case 'T':
       case 'Z':
       case 'J':
       case 'K':
            return 1;
       default: ;
    }

    return 0;
    
}

/**
 * Función:   conversion_de_fonemas                                            
 * \remarks Entrada y Salida:                                                           
 *         - nombre: nombre del fonema cuya representación se desea cambiar.   
 * \remarks NOTA:     Esta función es necesaria como consecuencia de las diferentes     
 *           representaciones entre Cotovía y la base de datos de logátomos.   
 */

void conversion_de_fonemas(char nombre[]) {

    if (nombre[0] == 'x')
        nombre[0] = 'h';

    if (nombre[0]=='Z') {
        nombre[0]= 'j';
        nombre[1] = 'j';
    }

}
