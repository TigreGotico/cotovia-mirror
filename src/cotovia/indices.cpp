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
 

#include "energia.hpp"
#include "indices.hpp"

/**
 * Función:   calcula_indices_unidad                                                
 * \remarks Entrada:
 *			- inicio: instante temporal de comienzo de la unidad.                   
 *          - fin: instante final de la unidad.                                     
 *          - origen: índice al comienzo de la frase dentro del fichero de sonido.  
 *			- frecuencia_muestreo: frecuencia de muestreo del sonido.				
 *	\remarks Salida:
 *			- nuevo_vector: vector descriptor al que se le añade la información de  
 *            índices al inicio, mitad y final de la unidad.                        
 * \remarks Objetivo:  Obtiene los índices de las muestras inicial, final y mitad de la      
 *            unidad contenida en el vector y delimitada temporalmente por inicio y 
 *            fin. Estos valores se usarán posteriormente en la fase de síntesis de 
 *            la voz para ir copiando los fragmentos de voz de los ficheros del     
 *            corpus a la cadena final.                                             
 */

void calcula_indices_unidad(float inicio, float final, int origen,
							int frecuencia_muestreo,
                            Vector_descriptor_candidato *nuevo_vector) {

   int indice_inicial, indice_mitad; //, indice_final;

   indice_inicial = (int) (inicio * frecuencia_muestreo + 0.5) * sizeof(short int);
   indice_mitad = (int) ((inicio + final) / 2 * frecuencia_muestreo + 0.5) * sizeof(short int);

   nuevo_vector->anhade_indices(origen, indice_inicial, indice_mitad);

}

