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
 

#ifndef _MODULO_mP

#define _MODULO_mP

#include "energia.hpp"
#include "red_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"

//! Original number of candidate prosodic structures (sequences of Grupo_fonico_frontera objects) considered
#define NUMERO_FRASES_CANDIDATAS_mP	3

int selecciona_minor_phrasing(Locutor *locutor, Frase_frontera &frase_objetivo, vector<Frase_frontera> *frases_candidatas, int *numero_frases_candidatas);

#endif
 