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
 


/** \file indices.hpp
 *
 * Se trata de un módulo muy sencillo que se limita a encontrar la posición de 
 * las muestras que delimitan el inicio, la mitad y el final de un fonema en un 
 * fichero de sonido con formato de datos en bruto. Es necesario para poder 
 * recoger los segmentos de voz seleccionados para la síntesis en la fase de la 
 * concatenación.
 * 
 */

#ifndef _INDICES

#include "descriptor.hpp"

#define _INDICES

/// Obtiene los índices de las muestras inicial, final central de la unidad 
/// descrita en el vector de entrada, y delimitada temporalmente por las otras
/// variables de entrada.

void calcula_indices_unidad(float inicio, float final, int origen,
							              int frecuencia_muestreo,
                            Vector_descriptor_candidato *nuevo_vector);

#endif

