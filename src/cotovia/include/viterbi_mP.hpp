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
 

#ifndef _VITERBI_mP

#define _VITERBI_mP

#include "viterbi_template.hpp"
#include "minor_phrasing.hpp"

#define C_PODA_mP 45.0       // 10.0 Estos valores son por poner algo. Dependerán de las
#define N_PODA_UNIDAD_mP 50 // 300
#define N_PODA_Ci_mP 75 //200

#define PESO_COSTE_OBJETIVO_mP	0.5F
#define PESO_COSTE_CONCATENACION_mP 0.5F

class Viterbi_mP: public Viterbi_template<Grupo_fonico_frontera, Grupo_fonico_frontera> {

    public:

    //! Basic constructor
    Viterbi_mP() {
    	poda_Ct_numero = false;
        poda_Ct_valor = false;
        poda_Ci_numero = false;
        poda_Ci_valor = false;
		poda_Ct = false;
        poda_Ci = false;
        indices_escogidos = NULL;
        peso_coste_objetivo = 0.5;
	    peso_coste_concatenacion = 0.5;

    }

    //! Constructor
    Viterbi_mP(bool poda_Ct_n, bool poda_Ct_v, bool poda_Ci_n, bool poda_Ci_v) {
    	poda_Ct_numero = poda_Ct_n;
        poda_Ct_valor = poda_Ct_v;
        poda_Ci_numero = poda_Ci_n;
        poda_Ci_valor = poda_Ci_v;
		poda_Ct = poda_Ct_numero || poda_Ct_valor;
        poda_Ci = poda_Ci_numero || poda_Ci_valor;
        indices_escogidos = NULL;
        peso_coste_objetivo = 0.5;
	    peso_coste_concatenacion = 0.5;
    }

	private:

	int crea_delta_u(Grupo_fonico_frontera ***cadena_id, int *tamano, Grupo_fonico_frontera *objetivo);

	float calcula_C_t(Grupo_fonico_frontera *candidato, Grupo_fonico_frontera *objetivo);

	float calcula_C_c(Grupo_fonico_frontera *unidad1, Grupo_fonico_frontera *unidad2);

};

#endif

