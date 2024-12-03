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
 

#include "minor_phrasing.hpp"
#include "modulo_minor_phrasing.hpp"
#include "viterbi_template.hpp"
#include "viterbi_mP.hpp"
#include "locutor.hpp"


/**
 * \brief	module in charge of deciding an appropriate prosodic structure for the sentence.
 * \remarks	it's implemented as a Viterbi search of candidate Grupo_fonico_frontera objects,
 * as taken from an annotated prosodic corpus
 * \param[in]	locutor	pointer to the current speaker database
 * \param[in]	frase_objetivo	sequence of target Grupo_fonico_frontera objects
 * \param[out]	frases_candidatas	vector of candidate prosodic structures
 * \param	numero_frases_candidatas	number of elements of frases_candidatas. It can be
modified in the absence of enough candidates
 * \return	0, in absence of errors
 */
int selecciona_minor_phrasing(Locutor *locutor, Frase_frontera &frase_objetivo,
							  vector<Frase_frontera> *frases_candidatas, int *numero_frases_candidatas) {


	unsigned int numero_GF = frase_objetivo.elementos.size();
    float coste;
	Grupo_fonico_frontera grupo_fonico_actual = frase_objetivo.elementos[0];
	Grupo_fonico_frontera *grupos_candidatos, *apunta_grupos;
    Frase_frontera frase;

//    return 0;
    
	// Búsqueda de Viterbi.
    Viterbi_mP viterbi(true, true, true, true);
	viterbi.introduce_valores_poda(C_PODA_mP, N_PODA_UNIDAD_mP, N_PODA_Ci_mP);
    viterbi.introduce_pesos_funciones_coste(PESO_COSTE_OBJETIVO_mP, PESO_COSTE_CONCATENACION_mP);

    viterbi.inicializa(locutor);

    if (viterbi.inicia_algoritmo_Viterbi(&grupo_fonico_actual, numero_GF))
			return 1;

    for (unsigned int contador = 1; contador < numero_GF; contador++) {

    	grupo_fonico_actual = frase_objetivo.elementos[contador];

        if (viterbi.siguiente_recursion_Viterbi(&grupo_fonico_actual))
        	return 1;

	} // for (int contador = 1; contador < frase_objetivo...

    viterbi.finaliza();

    int numero_unidades;

    for (int i = 0; i < *numero_frases_candidatas; i++) {

    	if ( (grupos_candidatos = (Grupo_fonico_frontera *)
        				malloc(numero_GF*sizeof(Grupo_fonico_frontera))) == NULL){
        	fprintf(stderr, "Error en selecciona_minor_phrasing, al asignar memoria.\n");
            return 1;
        }

    	if (viterbi.devuelve_camino_optimo((unsigned char) i, &grupos_candidatos,
    					&numero_unidades, numero_frases_candidatas, &coste))
			return 1;

        apunta_grupos = grupos_candidatos;
        frase.elementos.clear();
        for (unsigned int j = 0; j < numero_GF; j++) {
			frase.elementos.push_back(*apunta_grupos++);
        }

        frases_candidatas->push_back(frase);

        free(grupos_candidatos);

    } // for (int i = 0; ...

    viterbi.libera_memoria_algoritmo();
	viterbi.finaliza();
    
	return 0;

}

