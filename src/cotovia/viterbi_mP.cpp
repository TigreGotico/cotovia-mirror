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

#include "viterbi_mP.hpp"
#include "matriz_template.hpp"

/**
 * \class	Viterbi_mP
 * \brief	target cost of the Viterbi_mP class
 * \param[in]	candidato	candidate Grupo_fonico_frontera object
 * \param[in]	objetivo	target object
 * \return	the target cost
 */
float Viterbi_mP::calcula_C_t(Grupo_fonico_frontera *candidato, Grupo_fonico_frontera *objetivo) {

    unsigned int contador;
    unsigned int cuenta_pos;

    Sintagma_frontera *apunta_candidato, *apunta_objetivo;

    float coste = 0.0;

    if (candidato->elementos.size() != objetivo->elementos.size())
    	return 100.0;

    if (candidato->posicion != objetivo->posicion)
    	coste += 10;

    for (contador = 0; contador < candidato->elementos.size(); contador++) {

    	apunta_candidato = &candidato->elementos[contador];
        apunta_objetivo = &objetivo->elementos[contador];

    	if (apunta_candidato->acento != apunta_objetivo->acento) {
			coste += 50;
        }
        else
        	if (apunta_candidato->acento == 0)
            	continue;
            else
		    	if (apunta_candidato->tipo != apunta_objetivo->tipo) {
		        	coste += 30;
		            continue;
		        }
		        else
					if (apunta_candidato->elementos.size() !=
		    	    	apunta_objetivo->elementos.size() )
		        	    coste += 5;
		    	    else
		            	for (cuenta_pos = 0; cuenta_pos < apunta_candidato->elementos.size(); cuenta_pos++)
			            	if (apunta_candidato->elementos[cuenta_pos].etiqueta_morfosintactica !=
			                	apunta_objetivo->elementos[cuenta_pos].etiqueta_morfosintactica)
			                    coste += 2;

    } // for (contador = 0; ...

	return coste;

}


/**
 * \class	Viterbi_mP
 * \brief	concatenation cost of the Viterbi_mP class
 * \param[in]	unidad1	Grupo_fonico_frontera object preceding the pause
 * \param[in]	unidad2	Grupo_fonico_frontera object after the pause
 * \return	the concatenation cost
 */
float Viterbi_mP::calcula_C_c(Grupo_fonico_frontera *unidad1, Grupo_fonico_frontera *unidad2) {

//	if ( (unidad1->identificador == 109000) && (unidad2->identificador == 109001) )
//    	puts("");

	unsigned char pos_1;
    float coste_salto_f0 = 0.0, coste_factor_caida = 0.0, coste;

    Sintagma_frontera *sintagma = &unidad1->elementos[unidad1->elementos.size() - 1];

    pos_1 = sintagma->elementos[sintagma->elementos.size() - 1].etiqueta_morfosintactica;

#ifdef _INFORMACION_MORFOSINTACTICA_AUTOMATICA
	int clave = crea_clave_hash_contextos(pos_1, unidad1->siguiente_sintagma,
    					pos_1, unidad2->elementos[0].tipo, PAUSA_COMA);

	// Primero, el coste relacionado con el salto de f0:

    if (locutor_actual->contexto_gramatical_salto_f0.count(clave))
    	coste_salto_f0 = locutor_actual->contexto_gramatical_salto_f0[clave];

    if (locutor_actual->contexto_gramatical_salto_f0.count(clave))
    	coste_factor_caida = locutor_actual->contexto_gramatical_factor_caida[clave];

 	coste = coste_salto_f0 + coste_factor_caida;
#endif

    return coste;
    
/*
	if (unidad1->siguiente_sintagma != unidad2->elementos[0].tipo)
    	return 10.0;
    else
		return 1.0;
*/

}


/**
 * \class	Viterbi_mP
 * \brief	selects a suitable set of candidate units for the target unit
 * \param[out]	cadena_id	array of pointers to the candidate units
 * \param[out]	tamano	number of candidate units
 * \param[in]	objetivo	target unit
 * \return	0, in absence of errors
 */
int Viterbi_mP::crea_delta_u(Grupo_fonico_frontera ***cadena_id, int *tamano, Grupo_fonico_frontera *objetivo) {

	int indice;
    Grupo_fonico_frontera **recorre;
    char contador_sustituciones = -1;
    unsigned char tipo_grupo = objetivo->tipo_grupo;
	char posicion = objetivo->posicion;

    indice = calcula_indice_GF(tipo_grupo, posicion);

    while (locutor_actual->tabla_GF[indice].elementos.size() == 0) {

        if (contador_sustituciones < 3) {
        	contador_sustituciones = (char) ((contador_sustituciones + 1) % 4);
            tipo_grupo = contador_sustituciones;
        }
        else
        	posicion = (char) ((posicion + 1) % TIPOS_GRUPO_FONICO_SEGUN_POSICION);

        indice = calcula_indice_GF(tipo_grupo, posicion);

    } // while (grupo_seleccionado.elementos.size() == 0)

    *tamano = locutor_actual->tabla_GF[indice].elementos.size();

    if ( (*cadena_id = (Grupo_fonico_frontera **) malloc(*tamano*
                sizeof(Grupo_fonico_frontera *)))
        == NULL) {
        fprintf(stderr, "Error en crea_delta_u, al asignar memoria.");
        return 1;
    }

    recorre = *cadena_id;

    for (int cuenta = 0; cuenta < *tamano; cuenta++, recorre++)
        *recorre = &locutor_actual->tabla_GF[indice].elementos[cuenta];

	return 0;

}

