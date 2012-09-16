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
 

#ifndef _VITERBI_CLASSES_HPP

#define _VITERBI_CLASSES_HPP

 /**
 * \class	Coste_iteracion
 * \brief	Basic class used in the Viterbi template
 */
template <class T> class Coste_iteracion {

	public:

	//! Unit of the current iteration
    T *unidad;
	//! Identifier of the previous unit of mimimum cost to the current one
    int id_coste_minimo;
	//! Target cost
    float coste_unidad;
	//! Index to the matrix of optimum paths
    int camino;
	//! Total cost until the current unit
    float coste_c;

};


/**
 * \class	Coste_C_i
 * \brief	Basic class used in the Viterbi template
 */
template <class T> class Coste_Ci {

	public:
    
	//! Unit of the current iteration.
	T *unidad;
	//! Index to the matrix of optimum paths (sequence of optimum units to the current one)
	int camino;
	//! Total cost until the current unit
    float C_i;

};

#endif
