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
 
/** \file  analisis_morfosintactico.cpp
 *
 * Información detallada del propósito general de este módulo en
 * analisis_morfosintactico.hpp.
 *
 */
                    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <map>

#include "modos.hpp"
#include "tip_var.hpp"
#include "perfhash.hpp"
#include "utilidades.hpp"
#include "sintagma.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp"
#include "pausas.hpp"
#include "cotovia2eagles.hpp"

#include "crea_descriptores.hpp"

#include "categorias_analisis_morfosintactico.hpp"
#include "modelo_lenguaje.hpp"
#include "matriz.hpp"
#include "Viterbi_categorias.hpp"
#include "analisis_morfosintactico.hpp"


Modelo_lenguaje ** Analisis_morfosintactico::modelos_lenguaje = NULL;
int Analisis_morfosintactico::numero_modelos_lenguaje = 0;



pair<string, const char *> pares[] =
{
	pair<string, const char *>("ao","a+o"),
	pair<string, const char *>("aos","a+os"),
	pair<string, const char *>("aqueloutra","aquela+outra"),
	pair<string, const char *>("aqueloutras","aquelas+outras"),
	pair<string, const char *>("aqueloutro","aquel+outro"),
	pair<string, const char *>("aqueloutros","aqueles+outros"),
	pair<string, const char *>("chella","che+lle+a"),
	pair<string, const char *>("chellas","che+lle+as"),
	pair<string, const char *>("chelle","che+lle"),
	pair<string, const char *>("chellela","che+lles+a"),
	pair<string, const char *>("chellelas","che+lles+as"),
	pair<string, const char *>("chellelo","che+lles+o"),
	pair<string, const char *>("chellelos","che+lles+os"),
	pair<string, const char *>("chelleme","che+lle+me"),
	pair<string, const char *>("chellenola","che+lle+nos+a"),
	pair<string, const char *>("chellenolas","che+lle+nos+as"),
	pair<string, const char *>("chellenolo","che+lle+nos+o"),
	pair<string, const char *>("chellenolos","che+lle+nos+os"),
	pair<string, const char *>("chellenos","che+lle+nos"),
	pair<string, const char *>("chelles","che+lles"),
	pair<string, const char *>("chellevola","che+lle+vos+a"),
	pair<string, const char *>("chellevolas","che+lle+vos+as"),
	pair<string, const char *>("chellevolo","che+lle+vos+o"),
	pair<string, const char *>("chellevolos","che+lle+vos+os"),
	pair<string, const char *>("chellevos","che+lle+vos"),
	pair<string, const char *>("chello","che+lle+o"),
	pair<string, const char *>("chellos","che+lle+os"),
	pair<string, const char *>("chema","che+me+a"),
	pair<string, const char *>("chemas","che+me+as"),
	pair<string, const char *>("cheme","che+me"),
	pair<string, const char *>("chemelle","che+me+lle"),
	pair<string, const char *>("chemelles","che+me+lles"),
	pair<string, const char *>("chemevos","che+me+vos"),
	pair<string, const char *>("chemo","che+me+o"),
	pair<string, const char *>("chemos","che+me+os"),
	pair<string, const char *>("chenola","che+nos+a"),
	pair<string, const char *>("chenolas","che+nos+as"),
	pair<string, const char *>("chenolo","che+nos+o"),
	pair<string, const char *>("chenolos","che+nos+os"),
	pair<string, const char *>("chenos","che+nos"),
	pair<string, const char *>("chevola","che+vos+a"),
	pair<string, const char *>("chevolas","che+vos+as"),
	pair<string, const char *>("chevolo","che+vos+o"),
	pair<string, const char *>("chevolos","che+vos+os"),
	pair<string, const char *>("chevos","che+vos"),
	pair<string, const char *>("cho","che+o"),
	pair<string, const char *>("chos","che+os"),
	pair<string, const char *>("co","con+o"),
	pair<string, const char *>("coa","con+a"),
	pair<string, const char *>("coas","con+as"),
	pair<string, const char *>("cos","con+os"),
	pair<string, const char *>("cun","con+un"),
	pair<string, const char *>("cunha","con+unha"),
	pair<string, const char *>("cunhas","con+unhas"),
	pair<string, const char *>("cuns","con+uns"),
	pair<string, const char *>("cá","que+a"),
	pair<string, const char *>("cás","que+as"),
	pair<string, const char *>("có","que+o"),
	pair<string, const char *>("cós","que+os"),
	pair<string, const char *>("da","de+a"),
	pair<string, const char *>("dalgunha","de+algunha"),
	pair<string, const char *>("dalgunhas","de+algunhas"),
	pair<string, const char *>("dalgún","de+algún"),
	pair<string, const char *>("dalgúns","de+algúns"),
	pair<string, const char *>("daquel","de+aquel"),
	pair<string, const char *>("daquela","de+aquela"),
	pair<string, const char *>("daquelas","de+aquelas"),
	pair<string, const char *>("daqueles","de+aqueles"),
	pair<string, const char *>("daquelo","de+aquelo"),
	pair<string, const char *>("daqueloutra","de+aquela+outra"),
	pair<string, const char *>("daqueloutras","de+aquelas+outras"),
	pair<string, const char *>("daqueloutro","de+aquel+outro"),
	pair<string, const char *>("daqueloutros","de+aqueles+outros"),
	pair<string, const char *>("daquilo","de+aquilo"),
	pair<string, const char *>("das","de+as"),
	pair<string, const char *>("del","de+el"),
	pair<string, const char *>("dela","de+ela"),
	pair<string, const char *>("delas","de+elas"),
	pair<string, const char *>("deles","de+eles"),
	pair<string, const char *>("desa","de+esa"),
	pair<string, const char *>("desas","de+esas"),
	pair<string, const char *>("dese","de+ese"),
	pair<string, const char *>("deses","de+eses"),
	pair<string, const char *>("deso","de+eso"),
	pair<string, const char *>("desoutra","de+esa+outra"),
	pair<string, const char *>("desoutras","de+esas+outras"),
	pair<string, const char *>("desoutro","de+ese+outro"),
	pair<string, const char *>("desoutros","de+eses+outros"),
	pair<string, const char *>("desta","de+esta"),
	pair<string, const char *>("destas","de+estas"),
	pair<string, const char *>("deste","de+este"),
	pair<string, const char *>("destes","de+estes"),
	pair<string, const char *>("desto","de+esto"),
	pair<string, const char *>("destoutra","de+esta+outra"),
	pair<string, const char *>("destoutras","de+estas+outras"),
	pair<string, const char *>("destoutro","de+este+outro"),
	pair<string, const char *>("destoutros","de+estes+outros"),
	pair<string, const char *>("diso","de+iso"),
	pair<string, const char *>("disto","de+isto"),
	pair<string, const char *>("do","de+o"),
	pair<string, const char *>("dos","de+os"),
	pair<string, const char *>("doutra","de+outra"),
	pair<string, const char *>("doutras","de+outras"),
	pair<string, const char *>("doutro","de+outro"),
	pair<string, const char *>("doutros","de+outros"),
	pair<string, const char *>("dun","de+un"),
	pair<string, const char *>("dunha","de+unha"),
	pair<string, const char *>("dunhas","de+unhas"),
	pair<string, const char *>("duns","de+uns"),
	pair<string, const char *>("entrámbalas","entre+ambas+as"),
	pair<string, const char *>("entrámbolos","entre+ambos+os"),
	pair<string, const char *>("esoutra","esa+outra"),
	pair<string, const char *>("esoutras","esas+outras"),
	pair<string, const char *>("esoutro","ese+outro"),
	pair<string, const char *>("esoutros","eses+outros"),
	pair<string, const char *>("estoutra","esta+outra"),
	pair<string, const char *>("estoutras","estas+outras"),
	pair<string, const char *>("estoutro","este+outro"),
	pair<string, const char *>("estoutros","estes+outros"),
	pair<string, const char *>("lla","lle+a"),
	pair<string, const char *>("llas","lle+as"),
	pair<string, const char *>("llela","lles+a"),
	pair<string, const char *>("llelas","lles+as"),
	pair<string, const char *>("llelo","lles+o"),
	pair<string, const char *>("llelos","lles+os"),
	pair<string, const char *>("llema","lle+me+a"),
	pair<string, const char *>("llemas","lle+me+as"),
	pair<string, const char *>("lleme","lle+me"),
	pair<string, const char *>("llemo","lle+me+o"),
	pair<string, const char *>("llemos","lle+me+os"),
	pair<string, const char *>("llenola","lle+nos+a"),
	pair<string, const char *>("llenolas","lle+nos+as"),
	pair<string, const char *>("llenolo","lle+nos+o"),
	pair<string, const char *>("llenolos","lle+nos+os"),
	pair<string, const char *>("llenos","lle+nos"),
	pair<string, const char *>("llevola","lle+vos+a"),
	pair<string, const char *>("llevolas","lle+vos+as"),
	pair<string, const char *>("llevolo","lle+vos+o"),
	pair<string, const char *>("llevolos","lle+vos+os"),
	pair<string, const char *>("llevos","lle+vos"),
	pair<string, const char *>("llo","lle+o"),
	pair<string, const char *>("llos","lle+os"),
	pair<string, const char *>("ma","me+a"),
	pair<string, const char *>("maila","mais+a"),
	pair<string, const char *>("mailas","mais+as"),
	pair<string, const char *>("mailo","mais+o"),
	pair<string, const char *>("mailos","mais+os"),
	pair<string, const char *>("mas","me+as"),
	pair<string, const char *>("mo","me+o"),
	pair<string, const char *>("mos","me+os"),
	pair<string, const char *>("na","en+a"),
	pair<string, const char *>("nalgunha","en+algunha"),
	pair<string, const char *>("nalgunhas","en+algunhas"),
	pair<string, const char *>("nalgún","en+algún"),
	pair<string, const char *>("nalgúns","en+algúns"),
	pair<string, const char *>("naquel","en+aquel"),
	pair<string, const char *>("naquela","en+aquela"),
	pair<string, const char *>("naquelas","en+aquelas"),
	pair<string, const char *>("naqueles","en+aqueles"),
	pair<string, const char *>("naquelo","en+aquelo"),
	pair<string, const char *>("naqueloutra","en+aquela+outra"),
	pair<string, const char *>("naqueloutras","en+aquelas+outras"),
	pair<string, const char *>("naqueloutro","en+aquel+outro"),
	pair<string, const char *>("naqueloutros","en+aqueles+outros"),
	pair<string, const char *>("naquilo","en+aquilo"),
	pair<string, const char *>("nas","en+as"),
	pair<string, const char *>("nel","en+el"),
	pair<string, const char *>("nela","en+ela"),
	pair<string, const char *>("nelas","en+elas"),
	pair<string, const char *>("neles","en+eles"),
	pair<string, const char *>("nesa","en+esa"),
	pair<string, const char *>("nesas","en+esas"),
	pair<string, const char *>("nese","en+ese"),
	pair<string, const char *>("neses","en+eses"),
	pair<string, const char *>("neso","en+eso"),
	pair<string, const char *>("nesoutra","en+esa+outra"),
	pair<string, const char *>("nesoutras","en+esas+outras"),
	pair<string, const char *>("nesoutro","en+ese+outro"),
	pair<string, const char *>("nesoutros","en+eses+outros"),
	pair<string, const char *>("nesta","en+esta"),
	pair<string, const char *>("nestas","en+estas"),
	pair<string, const char *>("neste","en+este"),
	pair<string, const char *>("nestes","en+estes"),
	pair<string, const char *>("nesto","en+esto"),
	pair<string, const char *>("nestoutra","en+esta+outra"),
	pair<string, const char *>("nestoutras","en+estas+outras"),
	pair<string, const char *>("nestoutro","en+este+outro"),
	pair<string, const char *>("nestoutros","en+estes+outros"),
	pair<string, const char *>("niso","en+iso"),
	pair<string, const char *>("nisto","en+isto"),
	pair<string, const char *>("no","en+o"),
	pair<string, const char *>("nola","nos+a"),
	pair<string, const char *>("nolas","nos+as"),
	pair<string, const char *>("nolo","nos+o"),
	pair<string, const char *>("nolos","nos+os"),
	pair<string, const char *>("nos","en+os"),
	pair<string, const char *>("noutra","en+outra"),
	pair<string, const char *>("noutras","en+outras"),
	pair<string, const char *>("noutro","en+outro"),
	pair<string, const char *>("noutros","en+outros"),
	pair<string, const char *>("nun","en+un"),
	pair<string, const char *>("nunha","en+unha"),
	pair<string, const char *>("nunhas","en+unhas"),
	pair<string, const char *>("nuns","en+uns"),
	pair<string, const char *>("pola","por+a"),
	pair<string, const char *>("polas","por+as"),
	pair<string, const char *>("polo","por+o"),
	pair<string, const char *>("polos","por+os"),
	pair<string, const char *>("seche","se+che"),
	pair<string, const char *>("sechelle","se+che+lle>"),
	pair<string, const char *>("sechelles","se+che+lles"),
	pair<string, const char *>("secheme","se+che+me"),
	pair<string, const char *>("sechemos","se+che+mos"),
	pair<string, const char *>("sechevos","se+che+vos"),
	pair<string, const char *>("selle","se+lle"),
	pair<string, const char *>("selles","se+lles"),
	pair<string, const char *>("seme","se+me"),
	pair<string, const char *>("senos","se+nos"),
	pair<string, const char *>("trala","tras+a"),
	pair<string, const char *>("tralas","tras+as"),
	pair<string, const char *>("tralo","tras+o"),
	pair<string, const char *>("tralos","tras+os"),
	pair<string, const char *>("tódalas","todas+as"),
	pair<string, const char *>("tódolos","todos+os"),
	pair<string, const char *>("vola","vos+a"),
	pair<string, const char *>("volas","vos+as"),
	pair<string, const char *>("volo","vos+o"),
	pair<string, const char *>("volos","vos+os"),
	pair<string, const char *>("á","a+a"),
	pair<string, const char *>("ámbalas","ambas+as"),
	pair<string, const char *>("ámbolos","ambos+os"),
	pair<string, const char *>("ás","a+as"),
	pair<string, const char *>("ó","a+o"),
	pair<string, const char *>("ós","a+os"),
	pair<string, const char *>("al","a+el"),
};

map<string, const char *> contr(pares, pares + sizeof(pares) / sizeof(pares[0]));

//! Tipo enumerado con la traducción del conjunto de categorías morfosintácticas general al genérico (empleado en las clases de ambigüedad)
Categorias_genericas traduce_cat_modelo_a_generica[MAX_NUM_CATEGORIAS]= {
	NULA,// NULO
	ADV_GEN,// ADV
	ADX_GEN,// ADX_FEM_PLURAL
	ADX_GEN,// ADX_FEM_SINGULAR
	ADX_GEN,// ADX_MASC_PLURAL
	ADX_GEN,// ADX_MASC_SINGULAR
	APERTURA_EXCLA_GEN,// APERTURA_EXCLA
	APERTURA_INTERR_GEN,// APERTURA_INTERR
	ART_GEN, // ART_FEM_PLURAL,
	ART_GEN, // ART_FEM_SINGULAR
	ART_GEN, // ART_MASC_SINGULAR
	ART_GEN, // ART_MASC_PLURAL
	COMA_GEN,// COM
	CONX_COORD_GEN,// CONX_COORD
	CONX_SUBORD_GEN,// CONX_SUBORD
	DET_GEN,// DET_FEM_PLURAL
	DET_GEN,// DET_FEM_SINGULAR
	DET_GEN,// DET_MASC_PLURAL
	DET_GEN,// DET_MASC_SINGULAR
	DET_POSESIVO_GEN,// DET_POSE_FEM_PLURAL
	DET_POSESIVO_GEN,// DET_POSE_FEM_SINGULAR
	DET_POSESIVO_GEN,// DET_POSE_MASC_PLURAL
	DET_POSESIVO_GEN,// DET_POSE_MASC_SINGULAR
	EXCLAM_GEN,// EXCLAM
	INTERR_GEN,// INTERR
	INTERXE_GEN,// INTERXE
	NOM_GEN,// NOME_FEM_PLURAL
	NOM_GEN,// NOME_FEM_SINGULAR
	NOM_GEN,// NOME_MASC_PLURAL
	NOM_GEN,// NOME_MASC_SINGULAR
	PECHE_EXCLA_GEN,// PECHE_EXCLA
	PECHE_INTERR_GEN,// PECHE_INTERR
	PREPO_GEN,// PREPO
	PRON_GEN,// PRON_FEM_PLURAL
	PRON_GEN,// PRON_FEM_SINGULAR
	PRON_GEN,// PRON_MASC_PLURAL
	PRON_GEN,// PRON_MASC_SINGULAR
	PRON_POSESIVO_GEN,// PRON_POSE_FEM_PLURAL
	PRON_POSESIVO_GEN,// PRON_POSE_FEM_SINGULAR
	PRON_POSESIVO_GEN,// PRON_POSE_MASC_PLURAL
	PRON_POSESIVO_GEN,// PRON_POSE_MASC_SINGULAR
	PRON_PROC_GEN,// PRON_PROC_FEM_PLURAL
	PRON_PROC_GEN,// PRON_PROC_FEM_SINGULAR
	PRON_PROC_GEN,// PRON_PROC_MASC_PLURAL
	PRON_PROC_GEN,// PRON_PROC_MASC_SINGULAR
	PUNTO_GEN,// PUNT
	RELAT_GEN,// RELAT
	VERB_GEN,// VERBO_PLURAL
	VERB_GEN,// VERBO_SINGULAR
	XERUNDIO_GEN// XERUND
};

//! Conjunto de categorías genéricas (empleadas en las clases de ambigüedad), empleado para escribir por pantalla cada categoría.
static char categorias_genericas[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS][40] = {
	"NULA", // 0
	"ADV_GEN", // 1
	"ADX_GEN", // 2
	"ADX_SUSTANTIVADO_GEN", //3
	"APERTURA_EXCLA_GEN", //4
	"APERTURA_INTERR_GEN", // 5
	"ART_GEN", // 6
	"COMA_GEN", // 7
	"CONX_COORD_GEN", // 8
	"CONX_SUBORD_GEN", // 9
	"DET_GEN", //  10
	"DET_POSESIVO_GEN", // 11
	"EXCLAM_GEN", // 12
	"INTERR_GEN", // 13
	"INTERXE_GEN", // 14
	"NOM_GEN", //15
	"PECHE_EXCLA_GEN", // 16
	"PECHE_INTERR_GEN", // 17
	"PREPO_GEN", // 18
	"PRON_GEN", // 19
	"PRON_POSESIVO_GEN", // 20
	"PRON_PROC_GEN", // 21
	"PUNTO_GEN", // 22
	"RELAT_GEN", // 23
	"VERB_GEN", //  24
	"VERB_SUSTANTIVADO_GEN", //25
	"XERUNDIO_GEN" // 26
};

//! Array con la información de la importancia relativa de los modelos contextual y léxico, para cada categoría genérica
float factores[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS]= {
	0,
	0.625f, //ADV_GEN
	0.615f, //ADX_GEN
	0.625f, //ADX_SUSTANTIVADO_GEN
	0.625f, //APERTURA_EXCLA_GEN
	0.625f, //APERTURA_INTERR_GEN
	0.625f, //ART_GEN
	0.625f, //COMA_GEN
	0.625f, //CONX_COORD_GEN
	0.625f, //CONX_SUBORD_GEN
	0.650f, //DET_GEN
	0.625f, //DET_POSESIVO_GEN
	0.625f, //EXCLAM_GEN
	0.625f, //INTERR_GEN
	0.625f, //INTERXE_GEN
	0.535f, //NOM_GEN
	0.625f, //PECHE_EXCLA_GEN
	0.625f, //PECHE_INTERR_GEN
	0.640f, //PREPO_GEN
	0.625f, //PRON_GEN
	0.625f, //PRON_POSESIVO_GEN
	0.625f, //PRON_PROC_GEN
	0.625f, //PUNTO_GEN
	0.645f, //RELAT_GEN
	0.580f, //VERB_GEN
	0.625f, //VERB_SUSTANTIVADO_GEN
	0.625f //XERUNDIO_GEN
};






/**
 * \fn adjetivo_sustantivable
 * \param frase_separada inicio de la cadena de tipo t_frase_separada en
 *  la que se almacena la información de las palabras de la frase
 * \param palabra_actual puntero al elemento de frase_separada que se
 *  refiere a la palabra actual
 * \param numero_elementos número de elementos de la cadena frase_separada
 * \return Devuelve 1 si el adjetivo es sustantivable, según el contexto, y 
 * 0 en caso contrario
 */

int adjetivo_sustantivable(t_frase_separada *frase_separada, t_frase_separada *palabra_actual, int numero_elementos) {

    bool no_sustantivable = false;
    
	unsigned char *apunta_categoria, genero, numero;
	/*
		 while (*apunta_categoria != 0)
		 if (*apunta_categoria++ == NOME)
		 return 1;
		 */

	if (palabra_actual == frase_separada)
		return 0;

	if (palabra_actual->clase_pal == PALABRA_NORMAL_EMPEZA_MAY)
		return 1;

	if (palabra_actual != frase_separada) {

		apunta_categoria = (palabra_actual - 1)->cat_gramatical;

		while (*apunta_categoria) {
			if ( (*apunta_categoria == NOME) ||
					(*apunta_categoria == ADXECTIVO) ) {
				no_sustantivable = true;
                break;
            }
			apunta_categoria++;
		} // while (apunta_categoria)

        if (no_sustantivable == true) {
			apunta_categoria = (palabra_actual - 1)->cat_gramatical;

            while (*apunta_categoria) {
                if ( (*apunta_categoria == ART_DET) ||
                     (*apunta_categoria == ART_INDET) ||
                     (*apunta_categoria == DEMO_DET) ||
                     (*apunta_categoria == DEMO) ||
                     (*apunta_categoria == POSE_DET) ||
                     (*apunta_categoria == POSE) ) {
                    no_sustantivable = false;
                    break;
                }
                apunta_categoria++;
            } // while (apunta_categoria)

            if (no_sustantivable == true)
            	return 0;
        }

		if ( (		((palabra_actual - 1)->cat_gramatical[0] == VERBO) ||
					((palabra_actual - 1)->cat_gramatical[0] == INFINITIVO) ||
//                    ((palabra_actual - 1)->cat_gramatical[0] == PREP) ||
					((palabra_actual - 1)->cat_gramatical[0] == PERIFRASE) ) &&

					( ((palabra_actual - 1)->cat_gramatical[1] == 0) ||
                      ((palabra_actual - 1)->cat_gramatical[1] == VERBO) ||
                      ((palabra_actual - 1)->cat_gramatical[1] == INFINITIVO) ) &&

					(strstr((palabra_actual - 1)->pal, "-") == NULL))
			return 0;

		if (palabra_actual - frase_separada < numero_elementos)
			if ( ((palabra_actual + 1)->cat_gramatical[0] == NOME) &&
					((palabra_actual + 1)->cat_gramatical[1] == 0))
				return 0;

		// Conjunciones:

		if ( ((palabra_actual - 1)->cat_gramatical[0] >= CONX_COOR) &&
				((palabra_actual - 1)->cat_gramatical[0] <= LOC_CONX_SUBOR_CORREL) )
			return 0;


/* 		if ( ((palabra_actual - 1)->cat_gramatical[0] == ADVE) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_LUG) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_TEMP) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_CANT) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_MODO) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_AFIRM) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_NEGA) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_DUBI) &&
  				((palabra_actual - 1)->cat_gramatical[1] == 0))
  			return 0;*/
 
		if (ADVERBIO((palabra_actual - 1)->cat_gramatical[0]) && 
				((palabra_actual - 1)->cat_gramatical[1] == 0)) {
			return 0;
		}


		if ( ((palabra_actual - 1)->cat_gramatical[0] == COMA) ||
				((palabra_actual - 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
				((palabra_actual - 1)->cat_gramatical[0] == RUPTURA_COMA) )
			return 0;

	} // if (palabra_actual != frase_separada)

	if (palabra_actual - frase_separada < numero_elementos) {

		apunta_categoria = (palabra_actual + 1)->cat_gramatical;
		genero = (palabra_actual + 1)->xenero;
		numero = (palabra_actual + 1)->numero;

		while (*apunta_categoria) {
			if (*apunta_categoria++ == NOME)
				if ( ( (palabra_actual->xenero == genero) ||
							(palabra_actual->xenero == NO_ASIGNADO) ||
							(palabra_actual->xenero == AMBIGUO) ||
							(genero == NO_ASIGNADO) ||
							(genero == AMBIGUO) ) &&
						( (palabra_actual->numero == numero) ||
							(palabra_actual->numero == NO_ASIGNADO) ||
							(palabra_actual->numero == AMBIGUO) ||
							(numero == NO_ASIGNADO) ||
							(numero == AMBIGUO) ) )
					return 0;
		} // while (*apunta_categoria)

	} // if (palabra_actual - frase_separada < numero_elementos)

	if (palabra_actual > frase_separada + 1) {

/* 		if ( ((palabra_actual - 1)->cat_gramatical[0] == ADVE) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_LUG) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_TEMP) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_CANT) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_MODO) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_AFIRM) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_NEGA) ||
  				((palabra_actual - 1)->cat_gramatical[0] == ADVE_DUBI) &&
  				((palabra_actual - 1)->cat_gramatical[1] == 0)) {*/
 
		if (ADVERBIO((palabra_actual - 1)->cat_gramatical[0]) && 
				((palabra_actual - 1)->cat_gramatical[1] == 0)) {

			if ( ((palabra_actual - 2)->cat_gramatical[0] == VERBO) &&
					((palabra_actual - 2)->cat_gramatical[1] == 0) )
				return 0;

			apunta_categoria = (palabra_actual - 2)->cat_gramatical;

			while (*apunta_categoria != 0)
				if (*apunta_categoria++ == NOME)
					return 0;

/* 			if ( ((palabra_actual - 2)->cat_gramatical[0] == ADVE) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_LUG) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_TEMP) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_CANT) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_MODO) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_AFIRM) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_NEGA) ||
  					((palabra_actual - 2)->cat_gramatical[0] == ADVE_DUBI) &&
  					((palabra_actual - 2)->cat_gramatical[1] == 0))
  
 				return 0;*/
  
 
			if (ADVERBIO((palabra_actual - 2)->cat_gramatical[0]) && 
					((palabra_actual - 2)->cat_gramatical[1] == 0)) {
				return 0;
			}


			if ((palabra_actual - 2)->clase_pal == SIGNO_PUNTUACION)
				if (palabra_actual > frase_separada + 2) {
					apunta_categoria = (palabra_actual - 3)->cat_gramatical;
					while (*apunta_categoria)
						if (*apunta_categoria++ == NOME)
							return 0;

				} // if (palabra_actual > frase_separada + 2)
				else
					return 0;
		} // if ( ((palabra_actual - 1)->cat_gramatical[0] == ADVE) ||

		if ( ((palabra_actual - 1)->cat_gramatical[0] == ADXECTIVO) &&
				((palabra_actual - 1)->cat_gramatical[1] == 0))
			if ( ((palabra_actual - 2)->cat_gramatical[0] == NOME) &&
					((palabra_actual - 2)->cat_gramatical[1] == 0) )
				return 0;

	} // if (palabra_actual > frase_separada + 1)


	return 1;

}


/**
 * \fn infinitivo_sustantivable			                               
 * \param frase_separada inicio de la cadena de tipo t_frase_separada en la que se almacena la información de las palabras de la frase
 * \param palabra_actual puntero al elemento de frase_separada que se refiere a la palabra actual			   
 * \return devuelve 1 si el infinitivo es sustantivable, según el contexto, y 0 en caso contrario.								 */

int infinitivo_sustantivable(t_frase_separada *frase_separada, t_frase_separada *palabra_actual) {

	unsigned char genero, numero;


	if (strstr(palabra_actual->pal, "-"))
    	return 0;

    if (palabra_actual > frase_separada) {

    	if ( ((palabra_actual - 1)->cat_gramatical[0] == PREP) ||
			  ((palabra_actual - 1)->cat_gramatical[0] == VERBO) ||
			  	( ((palabra_actual - 1)->cat_gramatical[0] >= ADVE) &&
				  ((palabra_actual - 1)->cat_gramatical[0] <= ADVE_ESPECIFICADOR) ) ||
              ((palabra_actual - 1)->cat_gramatical[0] == NOME) )
	      	return 0;

		genero = (palabra_actual - 1)->xenero;
        numero = (palabra_actual - 1)->numero;
        
    	if ( ((palabra_actual - 1)->cat_gramatical[0] == ART_DET) )
			if ( ( (genero == palabra_actual->xenero) ||
            	   (genero == NO_ASIGNADO) ||
                   (genero ==  AMBIGUO) ||
                   (palabra_actual->xenero == NO_ASIGNADO) ||
                   (palabra_actual->xenero == AMBIGUO) ) &&

				 ( (numero == palabra_actual->numero) ||
                   (numero == NO_ASIGNADO) ||
                   (numero == AMBIGUO) ||
                   (palabra_actual->numero == NO_ASIGNADO) ||
                   (palabra_actual->numero == AMBIGUO) ) )
            	return 1;
            else
            	return 0;

    } // if (palabra_actual > frase_separada)

	if (palabra_actual > frase_separada + 1)

    	if ( ((palabra_actual - 1)->cat_gramatical[0] == ADXECTIVO) &&
        	 ((palabra_actual - 2)->cat_gramatical[0] == VERBO) )
             return 0;


	return 1;

}


/**
 * \fn devuelve_categorias_genericas_cadena                             
 * \param indice índice de la cadena categorías genéricas
 * \return la cadena de caracteres que representa la clase genérica cuyo índice se pasa como parámetro. En presencia de error, devuelve NULL
 */
char *devuelve_categorias_genericas_cadena(int indice) {

	if (indice >= MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS) {
		fprintf(stderr, "Error en devuelve_categorias_genericas_cadena: índice fuera \
        de rango.\n");
        return NULL;
    }


	return categorias_genericas[indice];

}



/**
 * \fn encuentra_categoria_probabilidad_maxima                          
 * \param probabilidades vector de probabilidades, indexado por el tipo categoría morfológica
 * \return La categoría de mayor probabilidad
 * \remarks Encontra la categoría que es más probable para una palabra según el contexto.                                               
 */
/*
 * fmendez no se usa
 *Categorias_modelo encuentra_categoria_probabilidad_maxima(float *probabilidades) {
 *
 *    unsigned char indice = 0;
 *    float probabilidad = 0;
 *    float *correcaminos = probabilidades;
 *
 *    for (unsigned char contador = 0; contador < MAX_NUM_CATEGORIAS; contador++, correcaminos++)
 *        if (*correcaminos > probabilidad) {
 *            probabilidad = *correcaminos;
 *            indice = contador;
 *        }
 *
 *    return (Categorias_modelo) indice;
 *
 *}
 *
 */

/**
 * \fn calcula_probabilidad_numero_numerales                            
 * \param[in] recorre_frase_separada elemento de frase_separada del que se quiere calcular la probabilidad de que sea SINGULAR o PLURAL
 * \param[out] prob_singular probabilidad de que el numeral sea SINGULAR
 * \param[out] prob_plural probabilidad de que el numeral sea PLURAL
 * \param[out] prob_det probabilidad de que el numeral sea determinante
 * \param[out] prob_pron probabilidad de que el numeral sea pronombre
 */
void calcula_probabilidad_numero_numerales(t_frase_separada *recorre_frase_separada, float *prob_singular, float *prob_plural, float *prob_det, float *prob_pron) {

    int numero;

    if ( (recorre_frase_separada->numero != AMBIGUO) &&
         (recorre_frase_separada->numero != NO_ASIGNADO) ){
        *prob_singular = 0.0f; // Da igual, puesto que en el bucle principal sólo se considerará
        *prob_plural = 0.0f;   // el adecuado.
        *prob_det = 0.0f;
        *prob_pron = 0.0f;
        return;
    } // if (recorre_frase_separada->numero...

    switch (recorre_frase_separada->clase_pal) {

    case CARDINAL:
    case NUMERO_CON_PUNTO:
        if ( (*recorre_frase_separada->pal == '1') &&
             (*(recorre_frase_separada->pal + 1) == '\0') ) {
			*prob_singular = (float) log10(0.9);
            *prob_plural = (float) log10(0.1);
        } // if ( (*recorre_frase_separada->pal == 1...
        else {
            numero = atoi(recorre_frase_separada->pal);

            if ( (numero > 1900) && (numero < 2050) ) {
                *prob_singular = (float) log10(0.5);
                *prob_plural = (float) log10(0.5);
            }
            else {
                *prob_singular = (float) log10(0.1);
                *prob_plural = (float) log10(0.9);
            }
        } // else
        *prob_pron = 0.0f;
        *prob_det = 0.0f;
        break;
    case NUM_ROMANO:
        *prob_singular = (float) log10(0.9);
        *prob_plural = (float) log10(0.1);
        *prob_det = (float) log10(0.99999);
        *prob_pron = (float) log10(0.00001);
        break;
    default: // de momento.
        *prob_singular = (float) log10(0.1);
        *prob_plural = (float) log10(0.9);
        *prob_pron = 0.0f;
        *prob_det = 0.0f;
    } // switch


}


/**
 * \fn comprueba_contracciones                                          
 * \param recorre_frase_separada elemento de frase_separada del que se quiere comprobar si tiene la categoría de contracción
 * \param opcion si es 0, se busca entre las categorías posibles, y si es 1, se busca entre las descartadas
 * \return si es una contracción, devuelve: 1-> Si tiene un determinante. 2-> Si tiene un artículo. 3-> Si tiene un pronombre. 4-> Si puede ser determinante  o pronombre
 */
int comprueba_contracciones(t_frase_separada *recorre_frase_separada, int opcion) {

    unsigned char *categoria;

    if (opcion == 0)
        categoria = recorre_frase_separada->cat_gramatical;
    else
        categoria = recorre_frase_separada->cat_gramatical_descartada;

    do {
        switch (*categoria) {

        case CONTR_PREP_DEMO_INDEF_DET:
        case CONTR_PREP_INDEF_DET:
        case CONTR_PREP_DEMO_DET:
        	return 1;
        case CONTR_PREP_ART_DET:
        case CONTR_PREP_ART_INDET:
            return 2;
        case CONTR_PREP_DEMO_PRON:
        case CONTR_PREP_DEMO_INDEF_PRON:
        case CONTR_PREP_INDEF_PRON:
            return 3;
        case CONTR_PREP_DEMO:
        case CONTR_PREP_INDEF:
        case CONTR_PREP_DEMO_INDEF:
            return 4;
        case '\0':
            return 0;
        default:
            categoria++;
        } // switch

    } while (1);

}


/**
 * \fn elimina_categorias_no_contraccion                                
 * \param recorre_frase_separada elemento de frase_separada del que se quiere eliminar las categorías que no sean contracciones
 */
void elimina_categorias_no_contraccion(t_frase_separada *recorre_frase_separada) {

    unsigned char *categoria = recorre_frase_separada->cat_gramatical;

    do {
        switch (*categoria) {

        case CONTR_PREP_DEMO_INDEF_DET:
        case CONTR_PREP_INDEF_DET:
        case CONTR_PREP_DEMO_DET:
        case CONTR_PREP_ART_DET:
        case CONTR_PREP_ART_INDET:
        case CONTR_PREP_DEMO_PRON:
        case CONTR_PREP_DEMO_INDEF_PRON:
        case CONTR_PREP_INDEF_PRON:
        case CONTR_PREP_DEMO:
        case CONTR_PREP_INDEF:
        case CONTR_PREP_DEMO_INDEF:
            categoria++;
            break;
        case '\0':
            return;
        default:
            elimina_categoria(*categoria, recorre_frase_separada);
        } // switch

    } while (1);

}


/**
 * \fn comprueba_locuciones                                             
 * \param[in] recorre_frase_separada elemento de frase_separada del que se quiere comprobar si tiene la categoría de locución
 * \param[out] locucion si es una locución, se devuelve un 1. 0 en otro caso
 * \remarks si puede ser una locución, ya se le asigna definitivamente
 */
void comprueba_locuciones(t_frase_separada *recorre_frase_separada, char *locucion) {

    unsigned char *categoria = recorre_frase_separada->cat_gramatical;

    do {
        switch (*categoria) {
//ZA 
        case LOC_ADXE:
        case LOC_SUST:
//ZA theend        	
				case LOC_ADVE:
        case LOC_ADVE_LUG:
        case LOC_ADVE_TEMP:
        case LOC_ADVE_CANTI:
        case LOC_ADVE_MODO:
        case LOC_ADVE_AFIR:
        case LOC_ADVE_NEGA:
        case LOC_ADVE_DUBI:
				case LOC_PREP:
        case LOC_PREP_LUG:
        case LOC_PREP_TEMP:
        case LOC_PREP_CANT:
        case LOC_PREP_MODO:
        case LOC_PREP_CAUS:
        case LOC_PREP_CONDI:
				case LOC_CONX:
        case LOC_CONX_COOR_COPU:
        case LOC_CONX_COOR_ADVERS:
        case LOC_CONX_SUBOR_CAUS:
        case LOC_CONX_SUBOR_CONCES:
        case LOC_CONX_SUBOR_CONSE:
        case LOC_CONX_SUBOR_COMPAR:
        case LOC_CONX_SUBOR_CONDI:
        case LOC_CONX_SUBOR_LOCAL:
        case LOC_CONX_SUBOR_TEMP:
        case LOC_CONX_SUBOR_MODA:
        case LOC_CONX_SUBOR_CONTRAP:
        case LOC_CONX_SUBOR_FINAL:
        case LOC_CONX_SUBOR_PROPOR:
        case LOC_CONX_SUBOR_CORREL:
            asigna_categoria(*categoria, recorre_frase_separada);
            *locucion = 1;
            return;
        case '\0':
            *locucion = 0;
            return;
        default:
            categoria++;

        } // switch

    } while (1);

}


/**
 * \fn calcula_probabilidades_homografas                                
 * \param[in] palabra elemento de frase_separada en el que se encuentra la información gramatical de la palabra en cuestión
 * \param[out] probabilidad_adjetivo probabilidad que se le asigna a poder ser adjetivo
 * \param[out] probabilidad_infinitivo ídem, para infinitivo
 * \param[out] probabilidad_nombre ídem, para nombre
 * \param[out] probabilidad_participio ídem, para participio
 * \param[out] probabilidad_verbo ídem, para verbo
 */
void calcula_probabilidades_homografas(t_frase_separada *palabra,
                                       float *probabilidad_adjetivo,
                                       float *probabilidad_infinitivo,
                                       float *probabilidad_nombre,
                                       float *probabilidad_participio,
                                       float *probabilidad_verbo) {

    unsigned char *categoria = palabra->cat_gramatical;
    int posible_adjetivo = 0,  posible_infinitivo = 0, posible_nombre = 0;
    int posible_participio = 0, posible_verbo= 0;

    while (*categoria != '\0') {
        switch (*categoria) {
            case NOME:
            case NOME_PROPIO:
                posible_nombre = 1;
                break;
            case ADXECTIVO:
                posible_adjetivo = 1;
                break;
            case PARTICIPIO:
                posible_participio = 1;
                break;
            case INFINITIVO:
                posible_infinitivo = 1;
                break;
            case VERBO:
                posible_verbo = 1;
                break;
            default:
                break;
        } // switch
        categoria++;
    } // while (*categoria)

    if (posible_participio) {

        unsigned char categoria_verbal = (unsigned char) ((palabra->cat_verbal[0] - 183) % 4);
        if (categoria_verbal <= 1) {
            if (posible_nombre && posible_adjetivo) {
				*probabilidad_participio = (float) log10(0.001);
                *probabilidad_adjetivo = (float) log10(0.5);
                *probabilidad_nombre = (float) log10(0.499);
            } // if (posible_nombre && posible_adjetivo
            else
                if (posible_adjetivo) {
					*probabilidad_participio = (float) log10(0.0001);
                    *probabilidad_adjetivo = (float) log10(0.999);
                    *probabilidad_nombre = (float) log10(0.0009);
                } // if (posible_adjetivo)
                else
                    if (posible_nombre) {
                        *probabilidad_participio = (float) log10(0.001);
                        *probabilidad_adjetivo = (float) log10(0.4);
                        *probabilidad_nombre = (float) log10(0.599);
                    } // if (posible_nombre)
                    else {
                        *probabilidad_participio = (float) log10(0.001);
                        *probabilidad_adjetivo = (float) log10(0.9);
                        *probabilidad_nombre = (float) log10(0.099);
                    } // else...
        } // if (categoria_verbal <= 1)
        else {
            if (posible_nombre && posible_adjetivo) {
                *probabilidad_participio = (float) log10(0.01);
                *probabilidad_adjetivo = (float) log10(0.495);
                *probabilidad_nombre = (float) log10(0.495);
            } // if (posible_nombre && posible_adjetivo
            else
                if (posible_adjetivo) {
                    *probabilidad_participio = (float) log10(0.0001);
                    *probabilidad_adjetivo = (float) log10(0.8);
                    *probabilidad_nombre = (float) log10(0.19);
                } // if (posible_adjetivo)
                else
                    if (posible_nombre) {
                        *probabilidad_participio = (float) log10(0.01);
                        *probabilidad_adjetivo = (float) log10(0.19);
                        *probabilidad_nombre = (float) log10(0.8);
                    } // if (posible_nombre)
                    else {
                        *probabilidad_participio = (float) log10(0.01);
                        *probabilidad_adjetivo = (float) log10(0.9);
                        *probabilidad_nombre = (float) log10(0.09);
                    }

        } // else

    } // if (posible_participio)
    else {
        if (posible_infinitivo) {
            if (posible_nombre) {
                *probabilidad_nombre = (float) log10(0.4);
                *probabilidad_infinitivo = (float) log10(0.6);
            } // if (posible_nombre)
            else {
                *probabilidad_nombre = (float) log10(0.005);
                *probabilidad_infinitivo = (float) log10(0.995);
            } // else
        } // if (posible_infinitivo)
        else
            if (posible_verbo) {
                if (posible_nombre && posible_adjetivo) {
                    *probabilidad_verbo = (float) log10(0.333);
                    *probabilidad_adjetivo = (float) log10(0.333);
                    *probabilidad_nombre = (float) log10(0.333);
                } // if (posible_nombre && posible_adjetivo
                else
                    if (posible_adjetivo) {
                        *probabilidad_verbo = (float) log10(0.45);
                        *probabilidad_nombre = (float) log10(0.01);
                        *probabilidad_adjetivo = (float) log10(0.45);
                    } // if (posible_adjetivo)
                    else
                        if (posible_nombre) {
                            *probabilidad_verbo = (float) log10(0.3);
                            *probabilidad_nombre = (float) log10(0.7);
                        } // if (posible_nombre)
                        else {
                            *probabilidad_verbo = 0.0f; // log10(1);
                        } // else...
            } // if (posible_verbo)
            else
                if (posible_adjetivo) {
                    if (posible_nombre) {
                        if (palabra->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA) {
                            *probabilidad_adjetivo = (float) log10(0.2);
                            *probabilidad_nombre = (float) log10(0.8);
                        } // if (palabra->clase_pal == ..
                        else {
                            *probabilidad_nombre = (float) log10(0.5);
                            *probabilidad_adjetivo = *probabilidad_nombre;
                        } // else
                    } // if (posible_nombre)
                    else {
                        *probabilidad_adjetivo = (float) log10(0.9);
                        *probabilidad_nombre = (float) log10(0.1);
                    } // else
                } // if (posible_adjetivo)
                else
                *probabilidad_nombre = 0.0f; // log10(1)
        } // else

}

/**
 * \fn decide_categorias                                                
 * \param[in] frase_separada array en el que se encuentra la información de todas las categorías que se pueden relacionar con las palabras de la frase de entrada
 * \param[in] numero_elementos número de elementos del array anterior
 * \param[in] idioma GALEGO o CASTELLANO
 * \return En ausencia de error se devuelve un cero
 * \remarks Determina la secuencia de categorías más probable para la frase de entrada.                                                      
 */
int Analisis_morfosintactico::decide_categorias(t_frase_separada *frase_separada, int numero_elementos,
					  int idioma) {

    estructura_categorias *lista_categorias;
    Categorias_modelo *categorias;


	// Comprobamos si están cargadas las variables que almacenan los n-gramas:

    if (modelo_lenguaje->pentagramas == NULL) {
		fprintf(stderr, "Error en decide_categorias: modelo de lenguaje no cargado.\n");
        return 1;
    } // if (pentagramas ...

/*
    //de momento en castellano pasamos...
    if (idioma!=GALEGO)
      return 0;
  */

    // Reservamos el doble de la memoria necesaria por el caso de los enclíticos.

    if ( (categorias = (Categorias_modelo *) malloc(2*numero_elementos*sizeof(Categorias_modelo)))
          == NULL) {
        fprintf(stderr, "Error en decide_categorias, al asignar memoria.\n");
        return 1;
    }


    if ( (lista_categorias = (estructura_categorias *) malloc(
                2*numero_elementos*sizeof(estructura_categorias))) == NULL) {
        fprintf(stderr, "Error en decide_categorias, al asignar memoria.\n");
        free(categorias);
        return 1;
    }

    memset(lista_categorias, 0, 2*numero_elementos*sizeof(estructura_categorias));

    // En primer lugar, convertimos la lista de categorías que nos da Cotovía en otra
    // lista diferente en la que se encuentren las categorías consideradas en el modelo
    // de análisis morfosintáctico.

#ifdef _MODELO_POS_COMPLETO
    if (convierte_a_categorias_modelo_lenguaje_completo(frase_separada, &numero_elementos, lista_categorias)) {
        free(lista_categorias);
        free(categorias);
        return 1;
    }
#else

    if (convierte_a_categorias_modelo_lenguaje(frase_separada, &numero_elementos, lista_categorias)) {
        free(lista_categorias);
        free(categorias);
        return 1;
    }

#endif

    if (aplica_modelo_lenguaje_v3(lista_categorias, numero_elementos, categorias)) {
        free(lista_categorias);
        free(categorias);
        return 1;
    }

#ifdef _MODELO_POS_COMPLETO

    if (convierte_modelo_lenguaje_a_categorias_completo(frase_separada, numero_elementos, categorias)) {
        free(lista_categorias);
        free(categorias);
        return 1;
    }

#else

    if (convierte_modelo_lenguaje_a_categorias(frase_separada, numero_elementos, categorias, saca_prob)) {
        free(lista_categorias);
        free(categorias);
        return 1;
    }

#endif

    free(lista_categorias);
    free(categorias);

    return 0;

}

/*
 * fmendez comentario chulo
 */

/**
 * \fn calcula_probabilidades_clases_ambiguedad                         
 * \param[in] mascara array en el que se encuentra la información de todas las categorías genericas (clase de ambiguedad) la palabra.
 * \param categorias: lista de todas las categorías y su probabilidad que se le pueden atribuir a la palabra
 * \remarks Asigna a cada posible categoría una probabilidad obtenida utilizando clases de ambiguedad, o, en caso de que la clase de ambiguedad no exista entre las obtenidas a partir del corpus de entrenamiento, la probabilidad de cada categoría (unigrama)
 */
void Analisis_morfosintactico::calcula_probabilidades_clases_ambiguedad(char *mascara, estructura_categorias *categorias){

  float probabilidades[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS];
  unsigned char clase_ambiguedad[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS];
  unsigned char *correcaminos;
  //unsigned char clave[LONG_CLAVE_CLASE_AMBIGUEDAD];
  unsigned char clave[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS];
  unsigned char long_clase=0;
  char no_existe_la_clase=false;
  
  if (mascara[ADX_SUSTANTIVADO_GEN]!='0' && mascara[NOM_GEN]!='0')
     mascara[ADX_SUSTANTIVADO_GEN]='0';
  if (mascara[VERB_SUSTANTIVADO_GEN]!='0' && mascara[NOM_GEN]!='0')
     mascara[VERB_SUSTANTIVADO_GEN]='0';

  memset(clase_ambiguedad,0,MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS);
  for(int i=0;i<MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS;i++)
    if (*(mascara+i)!='0')
      *(clase_ambiguedad+long_clase++)=(unsigned char)i;

  //if (long_clase==1)
  //	return;

	//if (long_clase < LONG_CLAVE_CLASE_AMBIGUEDAD) {
	if ( long_clase < modelo_lenguaje->clases_ambiguedad->dim ) {
		//memset(clave,0,LONG_CLAVE_CLASE_AMBIGUEDAD);
		memset(clave, 0, modelo_lenguaje->clases_ambiguedad->dim);
		strcpy((char *)clave,(char *)clase_ambiguedad);
    correcaminos=clase_ambiguedad;
    while(*correcaminos){
			//*(clave+LONG_CLAVE_CLASE_AMBIGUEDAD-1)=*correcaminos;
			*(clave + modelo_lenguaje->clases_ambiguedad->dim - 1)=*correcaminos;
			if ((probabilidades[*correcaminos]=modelo_lenguaje->calcula_prob_clase_ambiguedad(clave)) == 1) {
        no_existe_la_clase=true;
        break;
      }
      correcaminos++;
    }
  }
  else
    no_existe_la_clase=true;

  correcaminos=categorias->categoria;
  if (no_existe_la_clase)
    while (*correcaminos){
      //if (long_clase>1)
        categorias->probabilidad[*correcaminos]=modelo_lenguaje->calcula_prob_1_grama(*correcaminos);
      //else
        //categorias->probabilidad[*correcaminos]=0;
      correcaminos++;
    }
  else
    while (*correcaminos){

#ifdef _MODELO_POS_COMPLETO
    	// Para el modelo completo.
		categorias->probabilidad[*correcaminos] = probabilidades[*correcaminos];
        correcaminos++;
        // \fin Para el modelo completo.
#else
       if (mascara[ADX_SUSTANTIVADO_GEN] !='0' &&
                        traduce_cat_modelo_a_generica[*correcaminos]==NOM_GEN) {
        categorias->probabilidad[*correcaminos]=probabilidades[ADX_SUSTANTIVADO_GEN];
      }
      else if (mascara[VERB_SUSTANTIVADO_GEN] !='0' &&
                        traduce_cat_modelo_a_generica[*correcaminos]==NOM_GEN) {
        categorias->probabilidad[*correcaminos]=probabilidades[VERB_SUSTANTIVADO_GEN];
      }
      else
        categorias->probabilidad[*correcaminos]=probabilidades[traduce_cat_modelo_a_generica[*correcaminos]];
      correcaminos++;
#endif
    }

}


/**
 * \fn saca_clase_ambiguedad
 */

void saca_clase_ambiguedad(int tam, unsigned char condicion, unsigned char * mascara, unsigned char * destino){

  unsigned char j=0;

  if (mascara[ADX_SUSTANTIVADO_GEN]!=condicion && mascara[NOM_GEN]!=condicion)
     mascara[ADX_SUSTANTIVADO_GEN]=condicion;
  if (mascara[VERB_SUSTANTIVADO_GEN]!=condicion && mascara[NOM_GEN]!=condicion)
     mascara[VERB_SUSTANTIVADO_GEN]=condicion;

  for(unsigned char i=0;i<tam;i++){
    if (mascara[i] != condicion){
      destino[j]=i;
      j++;
    }
  }
  destino[j]=0;
}


/**
 * \fn convierte_a_categorias_modelo_lenguaje
 * \param[in] frase_separada array en el que se encuentra la información de todas las categorías que se pueden relacionar con las palabras de la frase de entrada
 * \param numero_elementos número de elementos del array anterior. Se puede modificar su valor, ante la presencia de elementos como comillas simples, dobles, o guiones, que no se tienen en cuenta en el modelo del lenguaje
 * \param lista_categorias lista de todas las categorías que se le pueden atribuir a cada palabra de frase_separada
 * \return En ausencia de error se devuelve un cero
 * \remarks Crea una cadena indexada de estructuras con las categorías posibles para cada palabra. Además, esta función sirve para separar las categorías contempladas en el modelo del lenguaje de las de Cotovía, más específicas
 */
int Analisis_morfosintactico::convierte_a_categorias_modelo_lenguaje(t_frase_separada *frase_separada, int *numero_elementos, estructura_categorias *lista_categorias) {

	t_frase_separada *recorre_frase_separada = frase_separada;
	estructura_categorias *recorre_categorias = lista_categorias;
	int cuenta_categorias;
	int *numero_categorias;
	int contador, cuenta;
	char signo_puntuacion = 0;
	unsigned char *punt_categoria_original;
	unsigned char categoria_original = frase_separada->cat_gramatical[0];
	unsigned char *nueva_categoria;
	unsigned char genero, numero, persona;
	//    unsigned char categoria_verbal,
	unsigned char bandera_verbal, *apunta_categoria_verbal;
	char *apunta_enclitico;
	char locucion = 0;
	char contraccion;
	int tipo_contraccion;
	char mascara_categorias[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS];
	float probabilidad_nombre = 0.0f, probabilidad_adjetivo = 0.0f, probabilidad_participio = 0.0f,
				probabilidad_infinitivo = 0.0f, probabilidad_verbo = 0.0f;
	float probabilidad_singular = 0.0f, probabilidad_plural = 0.0f;
	float probabilidad_det = 0.0f, probabilidad_pron = 0.0f;
	int numero_elementos_original = *numero_elementos;

	for (cuenta = 0; cuenta < MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS; cuenta++)
		mascara_categorias[cuenta] = '0';

	for (contador = 0; contador < numero_elementos_original; contador++) {

		contraccion = '0';

		if (recorre_frase_separada->cat_gramatical[0] != PERIFRASE) {
			comprueba_locuciones(recorre_frase_separada, &locucion);
			calcula_probabilidades_homografas(recorre_frase_separada,
					&probabilidad_adjetivo,
					&probabilidad_infinitivo,
					&probabilidad_nombre,
					&probabilidad_participio,
					&probabilidad_verbo);
		} // if (recorre_frase_separada->cat_gramatical[0]...

		punt_categoria_original = recorre_frase_separada->cat_gramatical;
		nueva_categoria = (unsigned char *) recorre_categorias->categoria;
		numero_categorias = (int *) &recorre_categorias->numero_categorias;
		cuenta_categorias = 0;

#ifdef _CORPUS_PROSODICO
		if (categoria_original == 0) {
			asigna_categoria(NOME, recorre_frase_separada);
			categoria_original = NOME;
		}
#endif

		while ( (cuenta_categorias++ < N_CAT) && (categoria_original != 0) &&
				(signo_puntuacion == 0) && (contraccion != '2') ) {

			signo_puntuacion = 0;

			genero = recorre_frase_separada->xenero;
			numero = recorre_frase_separada->numero;

			switch (categoria_original) {

				case ART_DET:
				case ART_INDET:
				case CONTR_INDEF_ART_DET:
					if (mascara_categorias[ART_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_MASC_PLURAL;
								recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_MASC_SINGULAR;
								recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_FEM_PLURAL;
								recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_FEM_SINGULAR;
								recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[ART_GEN] = '1';
					}

					break;

				case CONTR_PREP_ART_DET:
				case CONTR_PREP_ART_INDET:

					elimina_categorias_no_contraccion(recorre_frase_separada);
					if (contraccion == '0') {
						contraccion = '1';
						*nueva_categoria++ = PREPO;
						recorre_categorias->probabilidad[PREPO] = 0.0f;
						recorre_categorias++->numero_categorias = 1;
						*nueva_categoria = '\0';
						nueva_categoria = (unsigned char *) recorre_categorias->categoria;
						//                            apunta_probabilidad = (float *) recorre_categorias->probabilidad;
						//                            apunta_informacion = recorre_categorias->informacion;
						numero_categorias = (int *) &recorre_categorias->numero_categorias;
						(*numero_elementos)++;
						//                    recorre_frase_separada--;
					} // if (contraccion == '0')

					if (mascara_categorias[ART_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_MASC_PLURAL;
								recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_MASC_SINGULAR;
								recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_FEM_PLURAL;
								recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_FEM_SINGULAR;
								recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[ART_GEN] = '1';
					}

					break;

				case CONTR_PREP_DEMO_INDEF_DET:
				case CONTR_PREP_INDEF_DET:
				case CONTR_PREP_DEMO_DET:
					elimina_categorias_no_contraccion(recorre_frase_separada);
					if (contraccion == '0') {
						contraccion = '1';
						*nueva_categoria++ = PREPO;
						recorre_categorias->probabilidad[PREPO] = 0.0f;
						recorre_categorias++->numero_categorias = 1;
						*nueva_categoria = '\0';
						nueva_categoria = (unsigned char *) recorre_categorias->categoria;
						//                            apunta_probabilidad = (float *) recorre_categorias->probabilidad;
						//                            apunta_informacion = recorre_categorias->informacion;
						numero_categorias = (int *) &recorre_categorias->numero_categorias;
						(*numero_elementos)++;
						//                    recorre_frase_separada--;
					} // if (contraccion == '0')

					if (mascara_categorias[DET_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[DET_GEN] = '1';
					}

					break;

				case CONTR_PREP_DEMO_PRON:
				case CONTR_PREP_DEMO_INDEF_PRON:
				case CONTR_PREP_INDEF_PRON:
					elimina_categorias_no_contraccion(recorre_frase_separada);
					if (contraccion == '0') {
						contraccion = '1';
						*nueva_categoria++ = PREPO;
						recorre_categorias->probabilidad[PREPO] = 0.0f;
						recorre_categorias++->numero_categorias = 1;
						*nueva_categoria = '\0';
						nueva_categoria = (unsigned char *) recorre_categorias->categoria;
						//                            apunta_probabilidad = (float *) recorre_categorias->probabilidad;
						//                            apunta_informacion = recorre_categorias->informacion;
						numero_categorias = (int *) &recorre_categorias->numero_categorias;
						(*numero_elementos)++;
						//                    recorre_frase_separada--;
					} // if (contraccion == '0')
					if (mascara_categorias[PRON_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case CONTR_PREP_DEMO:
				case CONTR_PREP_INDEF:
				case CONTR_PREP_DEMO_INDEF:
					elimina_categorias_no_contraccion(recorre_frase_separada);
					if (contraccion == '0') {
						contraccion = '1';
						*nueva_categoria++ = PREPO;
						recorre_categorias->probabilidad[PREPO] = 0.0f;
						recorre_categorias++->numero_categorias = 1;
						*nueva_categoria = '\0';
						nueva_categoria = (unsigned char *) recorre_categorias->categoria;
						//                            apunta_probabilidad = (float *) recorre_categorias->probabilidad;
						//                            apunta_informacion = recorre_categorias->informacion;
						numero_categorias = (int *) &recorre_categorias->numero_categorias;
						(*numero_elementos)++;
						//                    recorre_frase_separada--;
					} // if (contraccion == '0')

					if (mascara_categorias[DET_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[DET_GEN] = '1';
					}
					if (mascara_categorias[PRON_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case DEMO_DET:
				case INDEF_DET:
				case CONTR_DEMO_INDEF_DET:

					if (mascara_categorias[DET_GEN] == '0') {
						if (contraccion == '1')
							contraccion = '2';

						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[DET_GEN] = '1';

					}
					break;

				case NUME_DET:
				case NUME_CARDI_DET:
				case NUME_ORDI_DET:
				case NUME_PARTI_DET:
					if (mascara_categorias[DET_GEN] == '0') {
						calcula_probabilidad_numero_numerales(recorre_frase_separada,
								&probabilidad_singular, &probabilidad_plural,
								&probabilidad_det, &probabilidad_pron);

						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = probabilidad_plural + probabilidad_det;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR]= probabilidad_singular + probabilidad_det;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL]= probabilidad_plural + probabilidad_det;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR]= probabilidad_singular + probabilidad_det;
							}
						}
						mascara_categorias[DET_GEN] = '1';
					}
					break;

				case CONTR_CONX_ART_DET:

					*nueva_categoria++ = CONX_SUBORD;
					recorre_categorias->probabilidad[CONX_SUBORD] = 0.0f;
					recorre_categorias++->numero_categorias = 1;
					*nueva_categoria = '\0';
					nueva_categoria = (unsigned char *) recorre_categorias->categoria;
					numero_categorias = (int *) &recorre_categorias->numero_categorias;
					(*numero_elementos)++;

					if (mascara_categorias[ART_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_MASC_PLURAL;
								recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_MASC_SINGULAR;
								recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_FEM_PLURAL;
								recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_FEM_SINGULAR;
								recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[ART_GEN] = '1';
					}

					break;

				case CONTR_CONX_COOR_COP_ART_DET:

					*nueva_categoria++ = CONX_COORD;
					recorre_categorias->probabilidad[CONX_COORD] = 0.0f;
					recorre_categorias++->numero_categorias = 1;
					*nueva_categoria = '\0';
					nueva_categoria = (unsigned char *) recorre_categorias->categoria;
					numero_categorias = (int *) &recorre_categorias->numero_categorias;
					(*numero_elementos)++;

					if (mascara_categorias[ART_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_MASC_PLURAL;
								recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_MASC_SINGULAR;
								recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ART_FEM_PLURAL;
								recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ART_FEM_SINGULAR;
								recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
							}
						}
						mascara_categorias[ART_GEN] = '1';
					}

					break;

				case POSE_DISTR:
				case POSE_DET:
					if (mascara_categorias[DET_POSESIVO_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_POSE_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_POSE_MASC_PLURAL]= 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_POSE_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_POSE_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_POSE_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_POSE_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_POSE_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_POSE_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[DET_POSESIVO_GEN] = '1';
					}
					break;
				case POSE_PRON:
					if (mascara_categorias[PRON_POSESIVO_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_POSE_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_POSE_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_POSE_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_POSE_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_POSE_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_POSE_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_POSE_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_POSE_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_POSESIVO_GEN] = '1';
					}
					break;
				case POSE: // Esta la añado porque no sé qué dato nos da Cotovía.
					if (mascara_categorias[DET_POSESIVO_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_POSE_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_POSE_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_POSE_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_POSE_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_POSE_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_POSE_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_POSE_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_POSE_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[DET_POSESIVO_GEN] = '1';
					}
					if (mascara_categorias[PRON_POSESIVO_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_POSE_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_POSE_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_POSE_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_POSE_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_POSE_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_POSE_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_POSE_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_POSE_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_POSESIVO_GEN] = '1';
					}
					break;

				case NUME:
				case NUME_CARDI:
				case NUME_ORDI:
				case NUME_PARTI:

					if (mascara_categorias[DET_GEN] == '0') {
						calcula_probabilidad_numero_numerales(recorre_frase_separada,
								&probabilidad_singular, &probabilidad_plural,
								&probabilidad_det, &probabilidad_pron);

						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = probabilidad_plural + probabilidad_det;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR]= probabilidad_singular + probabilidad_det;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL]= probabilidad_plural + probabilidad_det;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR]= probabilidad_singular + probabilidad_det;
							}
						}
						mascara_categorias[DET_GEN] = '1';
					}

					if (mascara_categorias[PRON_GEN] == '0') {
						calcula_probabilidad_numero_numerales(recorre_frase_separada,
								&probabilidad_singular, &probabilidad_plural,
								&probabilidad_det, &probabilidad_pron);
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = probabilidad_plural + probabilidad_pron;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = probabilidad_singular + probabilidad_pron;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = probabilidad_plural + probabilidad_pron;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = probabilidad_singular + probabilidad_pron;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case DEMO:
				case CONTR_DEMO_INDEF:
				case INDEF:

					if (mascara_categorias[DET_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_MASC_PLURAL;
								recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_MASC_SINGULAR;
								recorre_categorias->probabilidad[DET_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = DET_FEM_PLURAL;
								recorre_categorias->probabilidad[DET_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = DET_FEM_SINGULAR;
								recorre_categorias->probabilidad[DET_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[DET_GEN] = '1';
					}
					if (mascara_categorias[PRON_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case NUME_PRON:
				case NUME_CARDI_PRON:
				case NUME_ORDI_PRON:
				case NUME_PARTI_PRON:

					if (mascara_categorias[PRON_GEN] == '0') {
						calcula_probabilidad_numero_numerales(recorre_frase_separada,
								&probabilidad_singular, &probabilidad_plural,
								&probabilidad_det, &probabilidad_pron);

						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = probabilidad_plural + probabilidad_pron;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = probabilidad_singular + probabilidad_pron;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = probabilidad_plural + probabilidad_pron;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = probabilidad_singular + probabilidad_pron;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case PRON_PERS_TON:
				case INDEF_PRON:
				case DEMO_PRON:
					//                case CONTR_PREP_DEMO_PRON:
					//                case CONTR_PREP_DEMO_INDEF_PRON:
				case CONTR_DEMO_INDEF_PRON:
				case CONTR_PREP_PRON_PERS_TON:
					//                case CONTR_PREP_INDEF_PRON:

					if (mascara_categorias[PRON_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_GEN] = '1';
					}
					break;

				case PRON_PERS_AT:
				case PRON_PERS_AT_REFLEX:
				case CONTR_PRON_PERS_AT:
				case PRON_PERS_AT_ACUS:
				case PRON_PERS_AT_DAT:
				case CONTR_PRON_PERS_AT_DAT_AC:
				case CONTR_PRON_PERS_AT_DAT_DAT_AC:
					if (mascara_categorias[PRON_PROC_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_PROC_MASC_PLURAL;
								recorre_categorias->probabilidad[PRON_PROC_MASC_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_PROC_MASC_SINGULAR;
								recorre_categorias->probabilidad[PRON_PROC_MASC_SINGULAR] = 0.0f;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = PRON_PROC_FEM_PLURAL;
								recorre_categorias->probabilidad[PRON_PROC_FEM_PLURAL] = 0.0f;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = PRON_PROC_FEM_SINGULAR;
								recorre_categorias->probabilidad[PRON_PROC_FEM_SINGULAR] = 0.0f;
							}
						}
						mascara_categorias[PRON_PROC_GEN] = '1';
					}
					break;

				case PARTICIPIO:
					/*
						 if (mascara_categorias[VERB_GEN] == '0') {
						 categoria_verbal = (unsigned char)
						 ((recorre_frase_separada->cat_verbal[0] - 183) % 4);
						 if (categoria_verbal <= 1) {
					 *nueva_categoria++ = VERBO_SINGULAR;
					 recorre_categorias->probabilidad[VERBO_SINGULAR] = probabilidad_participio;
					 }
					 mascara_categorias[VERB_GEN] = '1';
					 }
					 */

				case ADXECTIVO:

					if (mascara_categorias[ADX_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ADX_MASC_PLURAL;
								recorre_categorias->probabilidad[ADX_MASC_PLURAL] = probabilidad_adjetivo;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ADX_MASC_SINGULAR;
								recorre_categorias->probabilidad[ADX_MASC_SINGULAR] = probabilidad_adjetivo;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = ADX_FEM_PLURAL;
								recorre_categorias->probabilidad[ADX_FEM_PLURAL] = probabilidad_adjetivo;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = ADX_FEM_SINGULAR;
								recorre_categorias->probabilidad[ADX_FEM_SINGULAR] = probabilidad_adjetivo;
							}
						}

						mascara_categorias[ADX_GEN] = '1';
					}

					if (adjetivo_sustantivable(frase_separada, recorre_frase_separada, numero_elementos_original)) {

						if (mascara_categorias[ADX_SUSTANTIVADO_GEN] == '0' &&
								mascara_categorias[NOM_GEN] == '0') {
							if (genero != FEMININO) {
								if (numero != SINGULAR) {
									*nueva_categoria++ = NOME_MASC_PLURAL;
									recorre_categorias->probabilidad[NOME_MASC_PLURAL] = probabilidad_nombre;
								}
								if (numero != PLURAL) {
									*nueva_categoria++ = NOME_MASC_SINGULAR;
									recorre_categorias->probabilidad[NOME_MASC_SINGULAR] = probabilidad_nombre;
								}
							}
							if (genero != MASCULINO) {
								if (numero != SINGULAR) {
									*nueva_categoria++ = NOME_FEM_PLURAL;
									recorre_categorias->probabilidad[NOME_FEM_PLURAL] = probabilidad_nombre;
								}
								if (numero != PLURAL) {
									*nueva_categoria++ = NOME_FEM_SINGULAR;
									recorre_categorias->probabilidad[NOME_FEM_SINGULAR] = probabilidad_nombre;
								}
							}
							mascara_categorias[ADX_SUSTANTIVADO_GEN] = '1';
						}
					} // if (adjetivo_sustantivable(...

					break;

				case NUME_MULTI:
				case NUME_COLECT:
				case NOME_PROPIO:
				case NOME:
#ifdef _CORPUS_PROSODICO
				default:
#endif
					if (mascara_categorias[NOM_GEN] == '0' &&
							mascara_categorias[ADX_SUSTANTIVADO_GEN] == '0' &&
							mascara_categorias[VERB_SUSTANTIVADO_GEN] == '0') {
						if (genero != FEMININO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = NOME_MASC_PLURAL;
								recorre_categorias->probabilidad[NOME_MASC_PLURAL] = probabilidad_nombre;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = NOME_MASC_SINGULAR;
								recorre_categorias->probabilidad[NOME_MASC_SINGULAR] = probabilidad_nombre;
							}
						}
						if (genero != MASCULINO) {
							if (numero != SINGULAR) {
								*nueva_categoria++ = NOME_FEM_PLURAL;
								recorre_categorias->probabilidad[NOME_FEM_PLURAL] = probabilidad_nombre;
							}
							if (numero != PLURAL) {
								*nueva_categoria++ = NOME_FEM_SINGULAR;
								recorre_categorias->probabilidad[NOME_FEM_SINGULAR] = probabilidad_nombre;
							}
						}

					}
					mascara_categorias[NOM_GEN] = '1';
					mascara_categorias[ADX_SUSTANTIVADO_GEN] = '0';
					mascara_categorias[VERB_SUSTANTIVADO_GEN] = '0';
					break;

				case INFINITIVO:

					persona = (char) ((recorre_frase_separada->cat_verbal[0] - 1) % 6);

					if (infinitivo_sustantivable(frase_separada, recorre_frase_separada))
						if (recorre_categorias != lista_categorias)
							if ( ( (recorre_categorias - 1)->categoria[0] != (unsigned char) PREPO) &&
									( (recorre_categorias - 1)->categoria[0] != (unsigned char) VERBO_SINGULAR) &&
									( (recorre_categorias - 1)->categoria[0] != (unsigned char) VERBO_PLURAL) &&
									( (recorre_categorias - 1)->categoria[0] != (unsigned char) CONX_COORD) &&
									( (recorre_categorias - 1)->categoria[0] != (unsigned char) CONX_SUBOR) ) {

								if ( (mascara_categorias[VERB_SUSTANTIVADO_GEN] == '0') && (persona < 3) ) {
									if (persona == 1) {
										*nueva_categoria++ = NOME_MASC_PLURAL;
										recorre_categorias->probabilidad[NOME_MASC_PLURAL] = probabilidad_nombre;
									}
									else {
										*nueva_categoria++ = NOME_MASC_SINGULAR;
										recorre_categorias->probabilidad[NOME_MASC_SINGULAR] = probabilidad_nombre;
									}

									mascara_categorias[VERB_SUSTANTIVADO_GEN] = '1';
								}
							} // if ( (recorre_categorias - 1)->categoria != PREPO)


					if (mascara_categorias[VERB_GEN] == '0') {
						if (persona < 3) {
							*nueva_categoria++ = VERBO_SINGULAR;
							recorre_categorias->probabilidad[VERBO_SINGULAR] = probabilidad_infinitivo;
						}
						else {
							*nueva_categoria++ = VERBO_PLURAL;
							recorre_categorias->probabilidad[VERBO_PLURAL] = probabilidad_infinitivo;
						}
						mascara_categorias[VERB_GEN] = '1';
					}
					break;


				case PERIFRASE:

					persona = (char) ((recorre_frase_separada->cat_verbal[0] - 1) % 6);

					if (mascara_categorias[VERB_GEN] == '0') {
						mascara_categorias[VERB_GEN] = '1';
						if (persona < 3) {
							*nueva_categoria++ = VERBO_SINGULAR;
							recorre_categorias->probabilidad[VERBO_SINGULAR] = 0.0f;
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while ((++recorre_frase_separada)->cat_gramatical[0] == PERIFRASE) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							}
						}
						else {
							*nueva_categoria++ = VERBO_PLURAL;
							recorre_categorias->probabilidad[VERBO_PLURAL] = 0.0f;
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while ((++recorre_frase_separada)->cat_gramatical[0] == PERIFRASE) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							}
						}
						recorre_frase_separada--;

						apunta_enclitico = recorre_frase_separada->inf_verbal->artigo;

						if (*apunta_enclitico) {

							recorre_categorias++->numero_categorias = 1;
							*nueva_categoria = '\0';
							nueva_categoria = (unsigned char *) recorre_categorias->categoria;
							//                            apunta_probabilidad = (float *) recorre_categorias->probabilidad;
							//                            apunta_informacion = recorre_categorias->informacion;
							numero_categorias = (int *) &recorre_categorias->numero_categorias;
							if (*apunta_enclitico == 'a') {
								if (*(apunta_enclitico + 1)) {
									*nueva_categoria++ = DET_FEM_PLURAL;
									recorre_categorias->probabilidad[DET_FEM_PLURAL] = 0.0f;
								}
								else {
									*nueva_categoria++ = DET_FEM_SINGULAR;
									recorre_categorias->probabilidad[DET_FEM_SINGULAR] = 0.0f;
								}
							} // if (*apunta_enclitico == 'a')
							else {
								if (*(apunta_enclitico + 1)) {
									*nueva_categoria++ = DET_MASC_PLURAL;
									recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
								}
								else {
									*nueva_categoria++ = DET_MASC_SINGULAR;
									recorre_categorias->probabilidad[DET_MASC_SINGULAR] = 0.0f;
								}
							} // else...
							(*numero_elementos)++;
						} // if (*apunta_enclitico)

					}
					break;

				case VERBO:

					apunta_enclitico = recorre_frase_separada->inf_verbal->artigo;

					apunta_categoria_verbal = recorre_frase_separada->cat_verbal;

					if (mascara_categorias[VERB_GEN] == '0') {
						bandera_verbal = 0x3;
						do {

							persona = (char) ((*apunta_categoria_verbal - 1) % 6);
							//                        persona = (char) ((recorre_frase_separada->cat_verbal[0] - 1) % 6);

							if ((persona < 3) && (bandera_verbal & 0x1) ){
								*nueva_categoria++ = VERBO_SINGULAR;
								recorre_categorias->probabilidad[VERBO_SINGULAR] = probabilidad_verbo;
								bandera_verbal -= (unsigned char) 1;
							}
							else
								if ( (persona >= 3) &&(bandera_verbal & 0x2) ) {
									*nueva_categoria++ = VERBO_PLURAL;
									recorre_categorias->probabilidad[VERBO_PLURAL] = probabilidad_verbo;
									bandera_verbal -= (unsigned char) 2;
								}

						} while (*(++apunta_categoria_verbal) != 0);

						mascara_categorias[VERB_GEN] = '1';

						if (*apunta_enclitico) {

							if (bandera_verbal == 0)
								recorre_categorias++->numero_categorias = 2;
							else
								recorre_categorias++->numero_categorias = 1;
							*nueva_categoria = '\0';
							nueva_categoria = (unsigned char *) recorre_categorias->categoria;
							//                                apunta_probabilidad = (float *) recorre_categorias->probabilidad;
							//                            apunta_informacion = recorre_categorias->informacion;
							numero_categorias = (int *) &recorre_categorias->numero_categorias;
							if (*apunta_enclitico == 'a') {
								if (*(apunta_enclitico + 1)) {
									*nueva_categoria++ = DET_FEM_PLURAL;
									recorre_categorias->probabilidad[DET_FEM_PLURAL] = 0.0f;
								}
								else {
									*nueva_categoria++ = DET_FEM_SINGULAR;
									recorre_categorias->probabilidad[DET_FEM_SINGULAR] = 0.0f;
								}
							} // if (*apunta_enclitico == 'a')
							else {
								if (*(apunta_enclitico + 1)) {
									*nueva_categoria++ = DET_MASC_PLURAL;
									recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
								}
								else {
									*nueva_categoria++ = DET_MASC_SINGULAR;
									recorre_categorias->probabilidad[DET_MASC_SINGULAR] = 0.0f;
								}
							} // else...
							(*numero_elementos)++;
						} // if (*apunta_enclitico)

					} // if (marcara_categorias[VERBB]

					break;

				case XERUNDIO:
					if (mascara_categorias[XERUNDIO_GEN] == '0') {
						*nueva_categoria++ = XERUND;
						recorre_categorias->probabilidad[XERUND] = 0.0f;
						mascara_categorias[XERUNDIO_GEN] = '1';
					}
					break;

				case ADVE:
				case ADVE_LUG:
				case ADVE_TEMP:
				case ADVE_CANT:
				case ADVE_MODO:
				case ADVE_AFIRM:
				case ADVE_NEGA:
				case ADVE_DUBI:
				case ADVE_ORD:
				case ADVE_MOD:
				case ADVE_COMP:
				case ADVE_CORREL:
				case ADVE_DISTR:
				case ADVE_ESPECIFICADOR:                	
				case LOC_ADVE_LUG:
				case LOC_ADVE_TEMP:
				case LOC_ADVE_CANTI:
				case LOC_ADVE_MODO:
				case LOC_ADVE_AFIR:
				case LOC_ADVE_NEGA:
				case LOC_ADVE_DUBI:
					//ZA
				case LOC_ADVE:                	
					if (mascara_categorias[ADV_GEN] == '0') {
						*nueva_categoria++ = ADV;
						recorre_categorias->probabilidad[ADV] = 0.0f;
						mascara_categorias[ADV_GEN] = '1';
						if (locucion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while (esta_palabra_ten_a_categoria_de(categoria_original, ++recorre_frase_separada)) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							} // while
							recorre_frase_separada--;
						} // if (locucion == 1
					}
					break;

				case PREP:
				case LOC_PREP:
				case LOC_PREP_LUG:
				case LOC_PREP_TEMP:
				case LOC_PREP_CANT:
				case LOC_PREP_MODO:
				case LOC_PREP_CAUS:
				case LOC_PREP_CONDI:
					if (mascara_categorias[PREPO_GEN] == '0') {
						*nueva_categoria++ = PREPO;
						recorre_categorias->probabilidad[PREPO] = 0.0f;
						mascara_categorias[PREPO_GEN] = '1';
						if (locucion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while (esta_palabra_ten_a_categoria_de(categoria_original, ++recorre_frase_separada)) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							} // while
							recorre_frase_separada--;
						} // if (locucion == 1

					}
					break;

				case CONX_COOR:
				case CONX_COOR_COPU:
				case CONX_COOR_DISX:
				case CONX_COOR_ADVERS:
				case CONX_COOR_DISTRIB:
				case LOC_CONX_COOR_COPU:
				case LOC_CONX_COOR_ADVERS:
					if (mascara_categorias[CONX_COORD_GEN] == '0') {
						*nueva_categoria++ = CONX_COORD;
						recorre_categorias->probabilidad[CONX_COORD] = 0.0f;
						mascara_categorias[CONX_COORD_GEN] = '1';
						if (locucion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while (esta_palabra_ten_a_categoria_de(categoria_original, ++recorre_frase_separada)) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							} // while
							recorre_frase_separada--;
						} // if (locucion == 1

					}
					break;

				case CONX_SUBOR:
				case CONX_SUBOR_PROPOR:
				case CONX_SUBOR_FINAL:
				case CONX_SUBOR_CONTRAP:
				case CONX_SUBOR_CAUS:
				case CONX_SUBOR_CONCES:
				case CONX_SUBOR_CONSE:
				case CONX_SUBOR_CONDI:
				case CONX_SUBOR_COMPAR:
				case CONX_SUBOR_LOCA:
				case CONX_SUBOR_TEMP:
				case CONX_SUBOR_MODAL:
				case CONX_SUBOR_COMPLETIVA:
				case CONX_SUBOR_CONTI:
				case LOC_CONX_SUBOR_CAUS:
				case LOC_CONX_SUBOR_CONCES:
				case LOC_CONX_SUBOR_CONSE:
				case LOC_CONX_SUBOR_COMPAR:
				case LOC_CONX_SUBOR_CONDI:
				case LOC_CONX_SUBOR_LOCAL:
				case LOC_CONX_SUBOR_TEMP:
				case LOC_CONX_SUBOR_MODA:
				case LOC_CONX_SUBOR_CONTRAP:
				case LOC_CONX_SUBOR_FINAL:
				case LOC_CONX_SUBOR_PROPOR:
				case LOC_CONX_SUBOR_CORREL:
					if (mascara_categorias[CONX_SUBORD_GEN] == '0') {
						*nueva_categoria++ = CONX_SUBORD;
						recorre_categorias->probabilidad[CONX_SUBORD] = 0.0f;
						mascara_categorias[CONX_SUBORD_GEN] = '1';
						if (locucion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
							saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
							//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
							while (esta_palabra_ten_a_categoria_de(categoria_original, ++recorre_frase_separada)) {
#ifdef _SACA_CLASES_AMBIGUEDAD
								saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
								//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
								numero_elementos_original--;
								(*numero_elementos)--;
							} // while
							recorre_frase_separada--;
						} // if (locucion == 1

					}
					break;

				case RELA:
					if (mascara_categorias[RELAT_GEN] == '0') {
						*nueva_categoria++ = RELAT;
						recorre_categorias->probabilidad[RELAT] = 0.0f;
						mascara_categorias[RELAT_GEN] = '1';
					}
					break;

				case INTER:
					if (mascara_categorias[INTERR_GEN] == '0') {
						*nueva_categoria++ = INTERR;
						recorre_categorias->probabilidad[INTERR] = 0.0f;
						mascara_categorias[INTERR_GEN] = '1';
					}
					break;

				case EXCLA:
					if (mascara_categorias[EXCLAM_GEN] == '0') {
						*nueva_categoria++ = EXCLAM;
						recorre_categorias->probabilidad[EXCLAM] = 0.0f;
						mascara_categorias[EXCLAM_GEN] = '1';
					}
					break;

				case PUNTO:
				case PUNTO_E_COMA:
				case DOUS_PUNTOS:
				case PUNTOS_SUSPENSIVOS:
					if (mascara_categorias[PUNTO_GEN] == '0') {
						*nueva_categoria++ = PUNT;
						recorre_categorias->probabilidad[PUNT] = 0.0f;
						mascara_categorias[PUNTO_GEN] = '1';
					}
					break;
				case RUPTURA_ENTONATIVA:
                	(*numero_elementos)--;
                    break;
				case RUPTURA_COMA:
				case COMA:
					if (mascara_categorias[COMA_GEN] == '0') { // Aquí no debería hacer falta, pero por si
						*nueva_categoria++ = COM;       // acaso...
						recorre_categorias->probabilidad[COM] = 0.0f;
						mascara_categorias[COMA_GEN] = '1';
					}
					break;
				case INTERX:
					if (mascara_categorias[INTERXE_GEN] == '0') { // Aquí, igual que en COMA.
						*nueva_categoria++ = INTERXE;
						recorre_categorias->probabilidad[INTERXE] = 0.0f;
						mascara_categorias[INTERXE_GEN] = '1';
					}
					break;
				case APERTURA_INTERROGACION:
					if (mascara_categorias[APERTURA_INTERR_GEN] == '0') {
						*nueva_categoria++ = APERTURA_INTERR;
						recorre_categorias->probabilidad[APERTURA_INTERR] = 0.0f;
						mascara_categorias[APERTURA_INTERR_GEN] = '1';
					}
					break;
				case APERTURA_EXCLAMACION:
					if (mascara_categorias[APERTURA_EXCLA_GEN] == '0') {
						*nueva_categoria++ = APERTURA_EXCLA;
						recorre_categorias->probabilidad[APERTURA_EXCLA] = 0.0f;
						mascara_categorias[APERTURA_EXCLA_GEN] = '1';
					}
					break;
				case PECHE_INTERROGACION:
					if (mascara_categorias[PECHE_INTERR_GEN] == '0') {
						*nueva_categoria++ = PECHE_INTERR;
						recorre_categorias->probabilidad[PECHE_INTERR] = 0.0f;
						mascara_categorias[PECHE_INTERR_GEN] = '1';
					}
					break;
				case PECHE_EXCLAMACION:
					if (mascara_categorias[PECHE_EXCLA_GEN] == '0') {
						*nueva_categoria++ = PECHE_EXCLA;
						recorre_categorias->probabilidad[PECHE_EXCLA] = 0.0f;
						mascara_categorias[PECHE_EXCLA_GEN] = '1';
					}
					break;
				case PECHE_PARENTESE:
				case APERTURA_PARENTESE:
				case APERTURA_CORCHETE:
				case PECHE_CORCHETE:
				case GUION:
				case DOBLES_COMINNAS:
				case SIMPLES_COMINNAS:
				case GUION_BAIXO:
				case SIGNO:
					signo_puntuacion = 1;
					break; // Simplemente, pasamos de ellos, aunque habrá que tenerlos en cuenta
					// al hacer la correspondencia inversa.
#ifndef _CORPUS_PROSODICO
				default:
					char yipicallei[500];
					escribe_categoria_gramatical(categoria_original,yipicallei);
					fprintf(stderr, "Error en convierte_a_categorias_modelo_lenguaje. Categoría no contemplada.\n");
					printf("%s\n",yipicallei);
					return 1;
#endif
			} // Debe de ser del switch...

	        if (recorre_frase_separada->cat_gramatical[0] == RUPTURA_ENTONATIVA)
    	    	break;

			if (signo_puntuacion == 0)
				categoria_original = *(++punt_categoria_original);

		} // Creo que es del While...

        if (recorre_frase_separada->cat_gramatical[0] == RUPTURA_ENTONATIVA) {
        	recorre_frase_separada++;
			categoria_original = recorre_frase_separada->cat_gramatical[0];
        	continue;
        }
            
		if (signo_puntuacion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
			recorre_frase_separada->clase_ambiguedad[0]=0;
#endif
			signo_puntuacion = 0;
			(*numero_elementos)--;
			recorre_frase_separada++;
			categoria_original = recorre_frase_separada->cat_gramatical[0];
#ifdef _CORPUS_PROSODICO
			if (categoria_original == 0) {
				asigna_categoria(NOME, recorre_frase_separada);
				categoria_original = NOME;
			}
#endif
			continue;
		} // if (signo_puntuacion == 1).

		if ( (*numero_categorias = nueva_categoria - recorre_categorias->categoria) == 0) {
			fprintf(stderr, "Error en convierte_a_categorias_modelo_lenguaje: Palabra sin ninguna categoría asignada.\n");
			return 1;
		}

		//        printf("%d\n", *numero_categorias);

#ifdef _SACA_CLASES_AMBIGUEDAD
		if (*recorre_frase_separada->inf_verbal->artigo ){
			recorre_frase_separada->clase_ambiguedad[0]=(unsigned char)VERB_GEN;
			recorre_frase_separada->clase_ambiguedad[1]=0;
			//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)(recorre_categorias-1)->categoria);
		}
		else if (locucion == 0)
			saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
		//strcpy((char *)recorre_frase_separada->clase_ambiguedad,(char *)recorre_categorias->categoria);
#endif
		recorre_frase_separada++;
		//fmendez
		calcula_probabilidades_clases_ambiguedad(mascara_categorias,recorre_categorias);
		//fmendez
		recorre_categorias++;
		categoria_original = recorre_frase_separada->cat_gramatical[0];
#ifdef _CORPUS_PROSODICO
		if (categoria_original == 0) {
			asigna_categoria(NOME, recorre_frase_separada);
			categoria_original = NOME;
		}
#endif
		*nueva_categoria = 0; // Indicamos el fin de las categorías posibles con un 0.
		//        apunta_informacion->categoria = 0;
		//fmendez
		for (cuenta = 0; cuenta < MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS; cuenta++)
			mascara_categorias[cuenta] = '0';
		//fmendez
		if (locucion == 1)
			if ( (tipo_contraccion = comprueba_contracciones(recorre_frase_separada - 1, 0)) != 0) {
				// Añadimos el determinante y/o el pronombre.
				genero = (recorre_frase_separada - 1)->xenero;
				numero = (recorre_frase_separada - 1)->numero;
				recorre_categorias->numero_categorias = 1;
				nueva_categoria = (unsigned char *) recorre_categorias->categoria;
				//               numero_categorias = (int *) &recorre_categorias->numero_categorias;
				if (tipo_contraccion == 1) { // Tenemos que añadir un determinante
					//fmendez
					mascara_categorias[DET_GEN] = '1';
					//fmendez
					if (genero != FEMININO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = DET_MASC_PLURAL;
							recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = DET_MASC_SINGULAR;
							recorre_categorias->probabilidad[DET_MASC_SINGULAR]= 0.0f;
						}
					}
					if (genero != MASCULINO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = DET_FEM_PLURAL;
							recorre_categorias->probabilidad[DET_FEM_PLURAL]= 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = DET_FEM_SINGULAR;
							recorre_categorias->probabilidad[DET_FEM_SINGULAR]= 0.0f;
						}
					}
				} // if (contraccion & 0x01)

				if (tipo_contraccion == 2) { // Tenemos que añadir un artículo
					//fmendez
					mascara_categorias[ART_GEN] = '1';
					//fmendez
					if (genero != FEMININO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = ART_MASC_PLURAL;
							recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = ART_MASC_SINGULAR;
							recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
						}
					}
					if (genero != MASCULINO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = ART_FEM_PLURAL;
							recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = ART_FEM_SINGULAR;
							recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
						}
					}
				} // if (contraccion == 2)


				if (tipo_contraccion == 3) { // Tenemos que añadir un pronombre
					//fmendez
					mascara_categorias[ART_GEN] = '1';
					//fmendez
					if (genero != FEMININO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = ART_MASC_PLURAL;
							recorre_categorias->probabilidad[ART_MASC_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = ART_MASC_SINGULAR;
							recorre_categorias->probabilidad[ART_MASC_SINGULAR]= 0.0f;
						}
					}
					if (genero != MASCULINO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = ART_FEM_PLURAL;
							recorre_categorias->probabilidad[ART_FEM_PLURAL]= 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = ART_FEM_SINGULAR;
							recorre_categorias->probabilidad[ART_FEM_SINGULAR]= 0.0f;
						}
					}
				} // if (contraccion == 3)

				if (tipo_contraccion == 4) {

					//fmendez
					mascara_categorias[DET_GEN] = '1';
					//fmendez
					if (genero != FEMININO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = DET_MASC_PLURAL;
							recorre_categorias->probabilidad[DET_MASC_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = DET_MASC_SINGULAR;
							recorre_categorias->probabilidad[DET_MASC_SINGULAR]= 0.0f;
						}
					}
					if (genero != MASCULINO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = DET_FEM_PLURAL;
							recorre_categorias->probabilidad[DET_FEM_PLURAL]= 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = DET_FEM_SINGULAR;
							recorre_categorias->probabilidad[DET_FEM_SINGULAR]= 0.0f;
						}
					}

					//fmendez
					mascara_categorias[PRON_GEN] = '1';
					//fmendez
					if (genero != FEMININO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = PRON_MASC_PLURAL;
							recorre_categorias->probabilidad[PRON_MASC_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = PRON_MASC_SINGULAR;
							recorre_categorias->probabilidad[PRON_MASC_SINGULAR] = 0.0f;
						}
					}
					if (genero != MASCULINO) {
						if (numero != SINGULAR) {
							*nueva_categoria++ = PRON_FEM_PLURAL;
							recorre_categorias->probabilidad[PRON_FEM_PLURAL] = 0.0f;
						}
						if (numero != PLURAL) {
							*nueva_categoria++ = PRON_FEM_SINGULAR;
							recorre_categorias->probabilidad[PRON_FEM_SINGULAR] = 0.0f;
						}
					}

				} // if (contraccion & 0x02)
				*nueva_categoria = 0;
				//fmendez
				calcula_probabilidades_clases_ambiguedad(mascara_categorias,recorre_categorias);
				//fmendez
				recorre_categorias++;
				for (cuenta = 0; cuenta < MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS; cuenta++)
					mascara_categorias[cuenta] = '0';
				(*numero_elementos)++;
			} // if ( (tipo_contraccion = comprueba_contracciones...

		//fmendez
		//        for (cuenta = 0; cuenta < MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS; cuenta++)
		//            mascara_categorias[cuenta] = '0';
		//fmendez
	} // Y éste, del for...

	return 0;

}



/**
 * \fn convierte_a_categorias_modelo_lenguaje_completo
 * \brief función similar a convierte_a_categorias_modelo_lenguaje. Se diferencia en que considera todas las categorías de Cotovía
 * \param[in] frasese_separada array en el que se encuentra la información de todas las categorías que se pueden relacionar con las palabras de la frase de entrada
 * \param numero_elementos número de elementos del array anterior. Se puede modificar su valor, ante la presencia de elementos como comillas simples, dobles, o guiones, que no se tienen en cuenta en el modelo del lenguaje
 * \param lista_categorias lista de todas las categorías que se le pueden atribuir a cada palabra de frase_separada
 * \return En ausencia de error se devuelve un cero
 * \remarks Crea una cadena indexada de estructuras con las categorías posibles para cada palabra. Además, esta función sirve para separar las categorías contempladas en el modelo del lenguaje de las de Cotovía, más específicas.
 */
int Analisis_morfosintactico::convierte_a_categorias_modelo_lenguaje_completo(t_frase_separada *frase_separada, int *numero_elementos, estructura_categorias *lista_categorias) {

	t_frase_separada *recorre_frase_separada = frase_separada;
	estructura_categorias *recorre_categorias = lista_categorias;
	int cuenta_categorias;
	int *numero_categorias;
	int contador, cuenta;
	char signo_puntuacion = 0;
	char locucion;
	unsigned char *punt_categoria_original;
	unsigned char categoria_original;
	unsigned char *nueva_categoria;
	char mascara_categorias[MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS];
	//    unsigned char categoria_verbal,
	int numero_elementos_original = *numero_elementos;

	for (contador = 0; contador < numero_elementos_original; contador++) {

		categoria_original = recorre_frase_separada->cat_gramatical[0];

		punt_categoria_original = recorre_frase_separada->cat_gramatical + 1;
		nueva_categoria = (unsigned char *) recorre_categorias->categoria;
		numero_categorias = (int *) &recorre_categorias->numero_categorias;
		cuenta_categorias = 0;

		for (cuenta = 0; cuenta < MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS; cuenta++)
			mascara_categorias[cuenta] = '0';

		while ( (cuenta_categorias++ < N_CAT) && (categoria_original != 0) &&
				(signo_puntuacion == 0) ) {

			comprueba_locuciones(recorre_frase_separada, &locucion);

			signo_puntuacion = 0;

			*nueva_categoria++ = categoria_original;
			//			*recorre_categorias->probabilidad[0] = 0.0f;
			mascara_categorias[categoria_original] = '1';

			if (categoria_original == PERIFRASE) {
#ifdef _SACA_CLASES_AMBIGUEDAD
				saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
#endif
				while ((++recorre_frase_separada)->cat_gramatical[0] == PERIFRASE) {
					// Esto falla si hay dos perífrasis diferentes seguidas.
#ifdef _SACA_CLASES_AMBIGUEDAD
				saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
#endif
					numero_elementos_original--;
					(*numero_elementos)--;
				}
				recorre_frase_separada--;
				break;
			}
			else
				if (locucion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
					saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias, recorre_frase_separada->clase_ambiguedad);
#endif
					while (esta_palabra_ten_a_categoria_de(categoria_original, ++recorre_frase_separada)) {
#ifdef _SACA_CLASES_AMBIGUEDAD
					saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias, recorre_frase_separada->clase_ambiguedad);
#endif
						numero_elementos_original--;
						(*numero_elementos)--;
					}
					recorre_frase_separada--;
					break;
				}
				else
					categoria_original = *punt_categoria_original++;

		} // Creo que es del While...

		if (signo_puntuacion == 1) {
#ifdef _SACA_CLASES_AMBIGUEDAD
			recorre_frase_separada->clase_ambiguedad[0]=0;
#endif
			signo_puntuacion = 0;
			(*numero_elementos)--;
			recorre_frase_separada++;
			categoria_original = recorre_frase_separada->cat_gramatical[0];
			continue;
		} // if (signo_puntuacion == 1).

		if ( (*numero_categorias = nueva_categoria - recorre_categorias->categoria) == 0) {
			fprintf(stderr, "Error en convierte_a_categorias_modelo_lenguaje: Palabra sin ninguna categoría asignada.\n");
			return 1;
		}
#ifdef _SACA_CLASES_AMBIGUEDAD
		if (locucion == 0)
			saca_clase_ambiguedad(MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS,'0',(unsigned char *)mascara_categorias,recorre_frase_separada->clase_ambiguedad);
#endif

		recorre_frase_separada++;
		//fmendez
		calcula_probabilidades_clases_ambiguedad(mascara_categorias,recorre_categorias);
		//fmendez
		recorre_categorias++;
		categoria_original = recorre_frase_separada->cat_gramatical[0];
		*nueva_categoria = 0; // Indicamos el fin de las categorías posibles con un 0.

	} // Y éste, del for...

	return 0;

}

/**
 * Función:   convierte_modelo_lenguaje_a_categorias
 * \remarks Entrada:
 *			- numero_elementos: número de elementos del array anterior.
 *          - categorias: array con las categorías escogidas.
 * \remarks Entrada y Salida:
 *          - frase_separada: array en el que se encuentra la información de
 *            todas las categorías que se pueden relacionar con las palabras
 *            de la frase de entrada.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Intoducir en el array frase_separada las categorías escogidas en
 *            el modelo de lenguaje.
 */

int convierte_modelo_lenguaje_a_categorias(t_frase_separada *frase_separada,
		int numero_elementos, Categorias_modelo *categorias, char saca_prob) {

	t_frase_separada *recorre_frase_separada = frase_separada;
	Categorias_modelo *recorre_categorias = categorias;
	unsigned char *cat_anterior, cat, avanza_categorias = 1;
	char locucion, perifrasis;
	char salta_enclitico = '0';

	for (int cuenta_palabras = 0; cuenta_palabras < numero_elementos;
			cuenta_palabras++, recorre_frase_separada++, recorre_categorias++) {

		if (recorre_frase_separada->cat_gramatical[0] == RUPTURA_ENTONATIVA) {
            cuenta_palabras--;
            recorre_categorias--;
            continue;
        }
    
		locucion = 0;
		perifrasis = 0;

		if (salta_enclitico == '1') {
			salta_enclitico = '0';
			recorre_frase_separada--;
			continue;
		} // if (salta_enclitico...

		if (avanza_categorias == 0) {
			recorre_categorias--;
			avanza_categorias = 1;
		} // if (avanza_categorias...

		cat = recorre_frase_separada->cat_gramatical[0];

		if (puntuacion_sin_final(cat) || (cat == SIGNO) ) {

			avanza_categorias = 0;
			cuenta_palabras--;
			continue;

		} // if (SIGNO_PUNTUACION...

		switch(recorre_categorias->cat) { // Esto podría hacerse en 2 vueltas:
			// 1º->género y número, 2º->tipo_categoría
			// Permitiría agrupar los case's.

			case ADV:
				cat_anterior = recorre_frase_separada->cat_gramatical;
				//ZA LOC_ADVE...
				while (*cat_anterior != '\0') {
					if ((*cat_anterior >= LOC_ADVE) && (*cat_anterior <= LOC_ADVE_DUBI))
/*					if ( (*cat_anterior == LOC_ADVE_LUG) ||
							(*cat_anterior == LOC_ADVE_TEMP) ||
							(*cat_anterior == LOC_ADVE_CANTI) ||
							(*cat_anterior == LOC_ADVE_MODO) ||
							(*cat_anterior == LOC_ADVE_AFIR) ||
							(*cat_anterior == LOC_ADVE_NEGA) ||
							(*cat_anterior == LOC_ADVE) || 
							(*cat_anterior == LOC_ADVE_DUBI) )*/
						break;
					cat_anterior++;
				} // while (*cat_anterior...

				if (*cat_anterior != '\0')
					locucion = 1;
				else {
					cat_anterior = recorre_frase_separada->cat_gramatical;

					//while (*cat_anterior <= ADVE || *cat_anterior > ADVE_ESPECIFICADOR) // OJO: ¿HOMOGRAFÍA ADV CON OTRA COSA?
					while (*cat_anterior <= ADVE || *cat_anterior > LOC_ADVE_DUBI) // OJO: ¿HOMOGRAFÍA ADV CON OTRA COSA?
						cat_anterior++;
				} // else

				cat = *cat_anterior;

				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;


				break;

			case ART_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == ART_DET) || (cat == ART_INDET) ||
							(cat == CONTR_INDEF_ART_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
							errónea. Palabra = %s.\n", recorre_frase_separada->pal);
					return 1;
				}

				break;

			case ART_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == ART_DET) || (cat == ART_INDET) ||
							(cat == CONTR_INDEF_ART_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
							errónea. Palabra = %s.\n", recorre_frase_separada->pal);
					return 1;
				}

				break;

			case ART_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == ART_DET) || (cat == ART_INDET) ||
							(cat == CONTR_INDEF_ART_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
							errónea. Palabra = %s.\n", recorre_frase_separada->pal);
					return 1;
				}

				break;

			case ART_MASC_SINGULAR:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == ART_DET) || (cat == ART_INDET) ||
							(cat == CONTR_INDEF_ART_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
							errónea. Palabra = %s.\n", recorre_frase_separada->pal);
					return 1;
				}

				break;

			case  ADX_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat = ADXECTIVO;

				break;

			case ADX_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat = ADXECTIVO;

				break;

			case ADX_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat = ADXECTIVO;

				break;

			case ADX_MASC_SINGULAR:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat = ADXECTIVO;

				break;

			case COM:

				if (strcmp(recorre_frase_separada->pal, SIMBOLO_RUPTURA_ENTONATIVA) == 0)
					cat = RUPTURA_ENTONATIVA;
				else
					if (strcmp(recorre_frase_separada->pal, SIMBOLO_RUPTURA_COMA) == 0)
						cat = RUPTURA_COMA;
					else
						cat = COMA;
				break;

			case CONX_COORD:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;

				while (*cat_anterior != '\0') {
					if ( (*cat_anterior == LOC_CONX_COOR_COPU) ||
							(*cat_anterior == LOC_CONX_COOR_ADVERS) )
						break;
					cat_anterior++;
				} // while (*cat_anterior...

				if (*cat_anterior != '\0')
					locucion = 1;
				else {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					while (*cat_anterior != '\0') {
						if ( (*cat_anterior >= CONX_COOR && *cat_anterior  <= CONX_COOR_ADVERS) )
							break;
						cat_anterior++;
					} // while (*cat_anterior != '\0')

					if (*cat_anterior == '\0') {
						cat_anterior = recorre_frase_separada->cat_gramatical;
						if (*cat_anterior != CONTR_CONX_COOR_COP_ART_DET) {
							fprintf(stderr, "Error en convierte_modelo_de_lenguaje_a_categorias: Categoría no válida para Conjunción Coordinada.\n");
							return 1;
						}

						recorre_categorias++;
						cuenta_palabras++;

					} // if (*cat_anterior == '\0')
				} // else

				cat = *cat_anterior;

				break;

			case CONX_SUBORD:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;

				while (*cat_anterior != '\0') {
					if ( *cat_anterior >= LOC_CONX_SUBOR_CAUS  && *cat_anterior <= LOC_CONX_SUBOR_CORREL )
/*					if ( (*cat_anterior == LOC_CONX_SUBOR_CAUS) ||
							(*cat_anterior == LOC_CONX_SUBOR_FINAL) ||
							(*cat_anterior == LOC_CONX_SUBOR_PROPOR) ||
							(*cat_anterior == LOC_CONX_SUBOR_CONCES) ||
							(*cat_anterior == LOC_CONX_SUBOR_CONSE) ||
							(*cat_anterior == LOC_CONX_SUBOR_CONDI) ||
							(*cat_anterior == LOC_CONX_SUBOR_LOCAL) ||
							(*cat_anterior == LOC_CONX_SUBOR_TEMP) ||
							(*cat_anterior == LOC_CONX_SUBOR_MODA) ||
							(*cat_anterior == LOC_CONX_SUBOR_CONTRAP) )*/
						break;
					cat_anterior++;
				} // while (*cat_anterior...

				if (*cat_anterior != '\0')
					locucion = 1;
				else {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					while (*cat_anterior != '\0') {
						if ( (*cat_anterior >= CONX_SUBOR) && (*cat_anterior  <= CONX_SUBOR_CONTI) ) 
							break;
						cat_anterior++;
					} // while (*cat_anterior != '\0')

					if (*cat_anterior == '\0') {
						cat_anterior = recorre_frase_separada->cat_gramatical;
						if (*cat_anterior != CONTR_CONX_ART_DET) {
							fprintf(stderr, "Error en convierte_modelo_de_lenguaje_a_categorias: Categoría no válica para Conjunción Subordinada.\n");
							return 1;
						}

						recorre_categorias++;

						switch (recorre_categorias->cat) {

							case ART_MASC_SINGULAR:
								recorre_frase_separada->xenero = MASCULINO;
								recorre_frase_separada->numero = SINGULAR;
								break;
							case ART_MASC_PLURAL:
								recorre_frase_separada->xenero = MASCULINO;
								recorre_frase_separada->numero = PLURAL;
								break;
							case ART_FEM_SINGULAR:
								recorre_frase_separada->xenero = FEMININO;
								recorre_frase_separada->numero = SINGULAR;
								break;
							case ART_FEM_PLURAL:
								recorre_frase_separada->xenero = FEMININO;
								recorre_frase_separada->numero = PLURAL;
								break;
							default:
								fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: %s: %d : categoría no considerada.\n",
									recorre_frase_separada->pal, *recorre_categorias);
								return 1;
						} // switch (recorre_categorias->categoria)
						cuenta_palabras++;

					} // if (*cat_anterior == '\0')
				} // else

				cat = *cat_anterior;

				break;

			case DET_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == DEMO_DET) || (cat == INDEF_DET) || (cat == ART_DET) ||
							(cat == ART_INDET) || (cat == CONTR_DEMO_INDEF_DET) ||
							(cat == NUME_DET) ||
							(cat == NUME_ORDI_DET) || (cat == NUME_CARDI_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while (cat != 0) {
						if ( (cat == DEMO) || (cat == INDEF) || (cat == CONTR_DEMO_INDEF) ||
								(cat == NUME) || (cat == NUME_PARTI) || (cat == NUME_ORDI) ||
								(cat == NUME_CARDI) || (cat == NUME_PARTI_DET))
							break;

						cat = *(++cat_anterior);
					} // while (cat != 0)

					if (cat == 0) {

						cat = *recorre_frase_separada->cat_gramatical;
						if (cat != CONTR_DEMO_INDEF_DET) {
							fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
									errónea. Palabra = %s.\n", recorre_frase_separada->pal);
							return 1;
						}

					} // if (cat == 0)
				} // if (cat == 0)


				switch (cat) {
					case DEMO:
						cat = DEMO_DET;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_DET;
						break;
					case INDEF:
						cat = INDEF_DET;
						break;
					case NUME:
						cat = NUME_DET;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_DET;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_DET;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_DET;
						break;
					default:
						break;
				} // switch (cat)


				break;

			case DET_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == DEMO_DET) || (cat == INDEF_DET) || (cat == ART_DET) ||
							(cat == ART_INDET) || (cat == CONTR_DEMO_INDEF_DET) ||
							(cat == NUME_DET) ||
							(cat == NUME_ORDI_DET) || (cat == NUME_CARDI_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while (cat != 0) {
						if ( (cat == DEMO) || (cat == INDEF) || (cat == CONTR_DEMO_INDEF) ||
								(cat == NUME) || (cat == NUME_PARTI) || (cat == NUME_ORDI) ||
								(cat == NUME_CARDI) || (cat == NUME_PARTI_DET))
							break;

						cat = *(++cat_anterior);
					} // while (cat != 0)

					if (cat == 0) {

						cat = *recorre_frase_separada->cat_gramatical;
						if (cat != CONTR_DEMO_INDEF_DET) {
							fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
									errónea. Palabra = %s.\n", recorre_frase_separada->pal);
							return 1;
						}

					} // if (cat == 0)
				} // if (cat == 0)


				switch (cat) {
					case DEMO:
						cat = DEMO_DET;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_DET;
						break;
					case INDEF:
						cat = INDEF_DET;
						break;
					case NUME:
						cat = NUME_DET;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_DET;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_DET;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_DET;
						break;
					default:
						break;
				} // switch (cat)


				break;

			case DET_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == DEMO_DET) || (cat == INDEF_DET) || (cat == ART_DET) ||
							(cat == ART_INDET) || (cat == CONTR_DEMO_INDEF_DET) ||
							(cat == NUME_DET) ||
							(cat == NUME_ORDI_DET) || (cat == NUME_CARDI_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while (cat != 0) {
						if ( (cat == DEMO) || (cat == INDEF) || (cat == CONTR_DEMO_INDEF) ||
								(cat == NUME) || (cat == NUME_PARTI) || (cat == NUME_ORDI) ||
								(cat == NUME_CARDI) || (cat == NUME_PARTI_DET))
							break;

						cat = *(++cat_anterior);
					} // while (cat != 0)

					if (cat == 0) {

						cat = *recorre_frase_separada->cat_gramatical;
						if (cat != CONTR_DEMO_INDEF_DET) {
							fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
									errónea. Palabra = %s.\n", recorre_frase_separada->pal);
							return 1;
						}

					} // if (cat == 0)
				} // if (cat == 0)


				switch (cat) {
					case DEMO:
						cat = DEMO_DET;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_DET;
						break;
					case INDEF:
						cat = INDEF_DET;
						break;
					case NUME:
						cat = NUME_DET;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_DET;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_DET;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_DET;
						break;
					default:
						break;
				} // switch (cat)


				break;

			case DET_MASC_SINGULAR:


				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while (cat != 0) {
					if ( (cat == DEMO_DET) || (cat == INDEF_DET) || (cat == ART_DET) ||
							(cat == ART_INDET) || (cat == CONTR_DEMO_INDEF_DET) ||
							(cat == NUME_DET) ||
							(cat == NUME_ORDI_DET) || (cat == NUME_CARDI_DET) )
						break;
					cat = *(++cat_anterior);
				}

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while (cat != 0) {
						if ( (cat == DEMO) || (cat == INDEF) || (cat == CONTR_DEMO_INDEF) ||
								(cat == NUME) || (cat == NUME_PARTI) || (cat == NUME_ORDI) ||
								(cat == NUME_CARDI) || (cat == NUME_PARTI_DET))
							break;

						cat = *(++cat_anterior);
					} // while (cat != 0)

					if (cat == 0) {

						cat = *recorre_frase_separada->cat_gramatical;
						if (cat != CONTR_DEMO_INDEF_DET) {
							fprintf(stderr, "Error en convierte_modelo_lenguaje_a_categorias: categoría \
									errónea. Palabra = %s.\n", recorre_frase_separada->pal);
							return 1;
						}

					} // if (cat == 0)
				} // if (cat == 0)


				switch (cat) {
					case DEMO:
						cat = DEMO_DET;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_DET;
						break;
					case INDEF:
						cat = INDEF_DET;
						break;
					case NUME:
						cat = NUME_DET;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_DET;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_DET;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_DET;
						break;
					default:
						break;
				} // switch (cat)


				break;

			case DET_POSE_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;

				while ( *cat_anterior != POSE_DET &&
						*cat_anterior != POSE_DISTR && *cat_anterior != POSE)
					cat_anterior++;


				if  (*cat_anterior==POSE )
					cat = POSE_DET;
				else
					cat = *cat_anterior;

				break;

			case DET_POSE_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;

				while ( *cat_anterior != POSE_DET &&
						*cat_anterior != POSE_DISTR && *cat_anterior != POSE)
					cat_anterior++;


				if  (*cat_anterior==POSE )
					cat = POSE_DET;
				else
					cat = *cat_anterior;

				break;

			case DET_POSE_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;

				while ( *cat_anterior != POSE_DET &&
						*cat_anterior != POSE_DISTR && *cat_anterior != POSE)
					cat_anterior++;

				if  (*cat_anterior==POSE )
					cat = POSE_DET;
				else
					cat = *cat_anterior;

				break;

			case DET_POSE_MASC_SINGULAR:  // OJO-> posesivos distribuidos... ¿?

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;

				while ( *cat_anterior != POSE_DET &&
						*cat_anterior != POSE_DISTR && *cat_anterior != POSE)
					cat_anterior++;

				if  (*cat_anterior==POSE )
					cat = POSE_DET;
				else
					cat = *cat_anterior;

				break;

			case INTERR:

				cat = INTER;
				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;

				break;

			case INTERXE:

				cat = INTERX;
				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;

				break;

			case NOME_FEM_PLURAL: // OJO: Preguntar NOME y NOME_PROPIO

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != NOME_PROPIO) && (cat != NOME) && (cat != ADXECTIVO) && (cat != PARTICIPIO)
						&& (cat != NUME_MULTI) && (cat != NUME_COLECT)
						&& (cat != INFINITIVO) && (cat != ADXECTIVO) )
					cat = *(++cat_anterior);

				if ( (cat == ADXECTIVO) || (cat == PARTICIPIO) ||
						(cat == NUME_COLECT) )
					cat = NOME;

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)
					cat = NOME_PROPIO;
				else
					if ( (recorre_frase_separada != frase_separada) &&
							//                     (recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION &&
							(((recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION) ||
							 (puntuacion_sin_final((recorre_frase_separada - 1)->cat_gramatical[0]) == 0)) &&
							(recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY) )
						cat = NOME_PROPIO;
					else
						if (cat==INFINITIVO)
							cat = NOME;
				break;

			case NOME_FEM_SINGULAR:


				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != NOME_PROPIO) && (cat != NOME) && (cat != ADXECTIVO) && (cat != PARTICIPIO)
						&& (cat != NUME_MULTI) && (cat != NUME_COLECT)
						&& (cat != INFINITIVO) && (cat != ADXECTIVO) )
					cat = *(++cat_anterior);

				if ( (cat == ADXECTIVO) || (cat == PARTICIPIO) ||
						(cat == NUME_COLECT) )
					cat = NOME;

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)
					cat = NOME_PROPIO;
				else
					if ( (recorre_frase_separada != frase_separada) &&
							//                     (recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION &&
							(((recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION) ||
							 (puntuacion_sin_final((recorre_frase_separada - 1)->cat_gramatical[0]) == 0)) &&
							(recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY) )
						cat = NOME_PROPIO;
					else
						if (cat==INFINITIVO)
							cat = NOME;
				break;

			case NOME_MASC_PLURAL:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != NOME_PROPIO) && (cat != NOME) && (cat != ADXECTIVO) && (cat != PARTICIPIO)
						&& (cat != NUME_MULTI) && (cat != NUME_COLECT)
						&& (cat != INFINITIVO) && (cat != ADXECTIVO) )
					cat = *(++cat_anterior);

				if ( (cat == ADXECTIVO) || (cat == PARTICIPIO) ||
						(cat == NUME_COLECT) )
					cat = NOME;

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)
					cat = NOME_PROPIO;
				else
					if ( (recorre_frase_separada != frase_separada) &&
							//                     (recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION &&
							(((recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION) ||
							 (puntuacion_sin_final((recorre_frase_separada - 1)->cat_gramatical[0]) == 0)) &&
							(recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY) )
						cat = NOME_PROPIO;
					else
						if (cat==INFINITIVO)
							cat = NOME;
				break;

			case NOME_MASC_SINGULAR:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != NOME_PROPIO) && (cat != NOME) && (cat != ADXECTIVO) && (cat != PARTICIPIO)
						&& (cat != NUME_MULTI) && (cat != NUME_COLECT)
						&& (cat != INFINITIVO) && (cat != ADXECTIVO) )
					cat = *(++cat_anterior);

				if ( (cat == ADXECTIVO) || (cat == PARTICIPIO) ||
						(cat == NUME_COLECT) )
					cat = NOME;

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)
					cat = NOME_PROPIO;
				else
					if ( (recorre_frase_separada != frase_separada) &&
							//                     (recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION &&
							(((recorre_frase_separada - 1)->clase_pal != SIGNO_PUNTUACION) ||
							 (puntuacion_sin_final((recorre_frase_separada - 1)->cat_gramatical[0]) == 0)) &&
							(recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY) )
						cat = NOME_PROPIO;
					else
						if (cat==INFINITIVO)
							cat = NOME;

				break;

			case PECHE_EXCLA:

				cat = PECHE_EXCLAMACION;

				break;

			case PECHE_INTERR:

				cat = PECHE_INTERROGACION;

				break;

			case APERTURA_INTERR:

				cat = APERTURA_INTERROGACION;

				break;

			case APERTURA_EXCLA:

				cat = APERTURA_EXCLAMACION;

				break;


			case PREPO:

				if (comprueba_contracciones(recorre_frase_separada, 0)) {

					cat = (recorre_categorias + 1)->cat;

					if ( (cat == DET_MASC_SINGULAR) || (cat == DET_MASC_PLURAL) ||
							(cat == DET_FEM_SINGULAR) || (cat == DET_FEM_PLURAL) ||
							(cat == ART_MASC_SINGULAR) || (cat == ART_MASC_PLURAL) ||
							(cat == ART_FEM_PLURAL) || (cat == ART_FEM_SINGULAR) ) {

						if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, recorre_frase_separada)) {
							cat = CONTR_PREP_ART_DET;
							recorre_categorias++;
							cuenta_palabras++;
						} else
							if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, recorre_frase_separada)) {
								cat = CONTR_PREP_ART_INDET;
								recorre_categorias++;
								cuenta_palabras++;
							} else
								if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, recorre_frase_separada) ||
										esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, recorre_frase_separada) ) {
									cat = CONTR_PREP_DEMO_DET;
									recorre_categorias++;
									cuenta_palabras++;
								} else
									if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, recorre_frase_separada) ||
											esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, recorre_frase_separada) ) {
										cat = CONTR_PREP_INDEF_DET;
										recorre_categorias++;
										cuenta_palabras++;
									} else
										if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, recorre_frase_separada) ||
												esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, recorre_frase_separada) ) {
											cat = CONTR_PREP_DEMO_INDEF_DET;
											recorre_categorias++;
											cuenta_palabras++;
										}
					}
					else
						if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, recorre_frase_separada) ||
								esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, recorre_frase_separada) ) {
							cat = CONTR_PREP_DEMO_PRON;
							recorre_categorias++;
							cuenta_palabras++;
						} else
							if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, recorre_frase_separada) ||
									esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, recorre_frase_separada) ) {
								cat = CONTR_PREP_INDEF_PRON;
								recorre_categorias++;
								cuenta_palabras++;
							} else
								if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, recorre_frase_separada) ||
										esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, recorre_frase_separada) ){
									cat = CONTR_PREP_DEMO_INDEF_PRON;
									recorre_categorias++;
									cuenta_palabras++;
								}

				} //  if (comprueba_contracciones...
				else {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					recorre_frase_separada->xenero = NO_ASIGNADO;
					recorre_frase_separada->numero = NO_ASIGNADO;

					while ( cat < PREP || cat > LOC_PREP_CONDI)
/*					while  ( (cat != PREP) && (cat != LOC_PREP_LUG) &&
							(cat != LOC_PREP_TEMP) && (cat != LOC_PREP_CANT) &&
							(cat != LOC_PREP_MODO) && (cat != LOC_PREP_CAUS) &&
							(cat != LOC_PREP_CONDI)  &&  (cat != LOC_PREP))
						//ZA*/
						cat = *++cat_anterior;

					if (cat != PREP)
						locucion = 1;
				}

				break;

			case PRON_FEM_PLURAL:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				if (recorre_frase_separada->xenero != NEUTRO)
					recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				while (cat != 0) {
					if ( (cat == INDEF) || (cat == INDEF_PRON) ||
							(cat == PRON_PERS_TON) ||
							(cat == DEMO) || (cat == DEMO_PRON) ||
							(cat == CONTR_DEMO_INDEF) || (cat == CONTR_DEMO_INDEF_PRON) )
						break;
					cat = *(++cat_anterior);
				} // while (cat != 0)

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while ( (cat != NUME_PRON)             && (cat != NUME_CARDI_PRON) &&
							(cat != NUME_ORDI_PRON)        && (cat != NUME_PARTI_PRON) &&
							(cat != CONTR_DEMO_INDEF_PRON) && (cat != CONTR_PREP_PRON_PERS_TON) &&
							(cat != CONTR_DEMO_INDEF)      && (cat != CONTR_PREP_DEMO_INDEF) &&
							(cat != NUME)                  && (cat != NUME_CARDI) &&
							(cat != NUME_ORDI)             && (cat != NUME_PARTI) )
						cat = *(++cat_anterior);
				} // if (cat == 0)

				switch (cat) {
					case DEMO:
						cat = DEMO_PRON;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_PRON;
						break;
					case INDEF:
						cat = INDEF_PRON;
						break;
					case NUME:
						cat = NUME_PRON;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_PRON;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_PRON;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_PRON;
						break;
					default:
						break;

				} // switch (cat)...

				break;

			case PRON_FEM_SINGULAR:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				if (recorre_frase_separada->xenero != NEUTRO)
					recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				while (cat != 0) {
					if ( (cat == INDEF) || (cat == INDEF_PRON) || 
							(cat == PRON_PERS_TON) ||
							(cat == DEMO) || (cat == DEMO_PRON) || 
							(cat == CONTR_DEMO_INDEF) || (cat == CONTR_DEMO_INDEF_PRON) )
						break;
					cat = *(++cat_anterior);
				} // while (cat != 0)

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while ( (cat != NUME_PRON)             && (cat != NUME_CARDI_PRON) &&
							(cat != NUME_ORDI_PRON)        && (cat != NUME_PARTI_PRON) &&
							(cat != CONTR_DEMO_INDEF_PRON) && (cat != CONTR_PREP_PRON_PERS_TON) &&
							(cat != CONTR_DEMO_INDEF)      && (cat != CONTR_PREP_DEMO_INDEF) &&
							(cat != NUME)                  && (cat != NUME_CARDI) &&
							(cat != NUME_ORDI)             && (cat != NUME_PARTI) )
						cat = *(++cat_anterior);
				} // if (cat == 0)

				switch (cat) {
					case DEMO:
						cat = DEMO_PRON;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_PRON;
						break;
					case INDEF:
						cat = INDEF_PRON;
						break;
					case NUME:
						cat = NUME_PRON;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_PRON;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_PRON;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_PRON;
						break;
					default:
						break;

				} // switch (cat)...

				break;

			case PRON_MASC_PLURAL:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				if (recorre_frase_separada->xenero != NEUTRO)
					recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				while (cat != 0) {
					if ( (cat == INDEF) || (cat == INDEF_PRON) || 
							(cat == PRON_PERS_TON) ||
							(cat == DEMO) || (cat == DEMO_PRON) || 
							(cat == CONTR_DEMO_INDEF) || (cat == CONTR_DEMO_INDEF_PRON) )
						break;
					cat = *(++cat_anterior);
				} // while (cat != 0)

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while ( (cat != NUME_PRON)             && (cat != NUME_CARDI_PRON) &&
							(cat != NUME_ORDI_PRON)        && (cat != NUME_PARTI_PRON) &&
							(cat != CONTR_DEMO_INDEF_PRON) && (cat != CONTR_PREP_PRON_PERS_TON) &&
							(cat != CONTR_DEMO_INDEF)      && (cat != CONTR_PREP_DEMO_INDEF) &&
							(cat != NUME)                  && (cat != NUME_CARDI) &&
							(cat != NUME_ORDI)             && (cat != NUME_PARTI) )
						cat = *(++cat_anterior);
				} // if (cat == 0)

				switch (cat) {
					case DEMO:
						cat = DEMO_PRON;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_PRON;
						break;
					case INDEF:
						cat = INDEF_PRON;
						break;
					case NUME:
						cat = NUME_PRON;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_PRON;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_PRON;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_PRON;
						break;
					default:
						break;

				} // switch (cat)...

				break;

			case PRON_MASC_SINGULAR:

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				if (recorre_frase_separada->xenero != NEUTRO)
					recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				while (cat != 0) {
					if ( (cat == INDEF) || (cat == INDEF_PRON) || 
							(cat == PRON_PERS_TON) ||
							(cat == DEMO) || (cat == DEMO_PRON) || 
							(cat == CONTR_DEMO_INDEF) || (cat == CONTR_DEMO_INDEF_PRON) )
						break;
					cat = *(++cat_anterior);
				} // while (cat != 0)

				if (cat == 0) {
					cat_anterior = recorre_frase_separada->cat_gramatical;
					cat = *cat_anterior;

					while ( (cat != NUME_PRON)             && (cat != NUME_CARDI_PRON) &&
							(cat != NUME_ORDI_PRON)        && (cat != NUME_PARTI_PRON) &&
							(cat != CONTR_DEMO_INDEF_PRON) && (cat != CONTR_PREP_PRON_PERS_TON) &&
							(cat != CONTR_DEMO_INDEF)      && (cat != CONTR_PREP_DEMO_INDEF) &&
							(cat != NUME)                  && (cat != NUME_CARDI) &&
							(cat != NUME_ORDI)             && (cat != NUME_PARTI) )
						cat = *(++cat_anterior);
				} // if (cat == 0)

				switch (cat) {
					case DEMO:
						cat = DEMO_PRON;
						break;
					case CONTR_DEMO_INDEF:
						cat = CONTR_DEMO_INDEF_PRON;
						break;
					case INDEF:
						cat = INDEF_PRON;
						break;
					case NUME:
						cat = NUME_PRON;
						break;
					case NUME_CARDI:
						cat = NUME_CARDI_PRON;
						break;
					case NUME_ORDI:
						cat = NUME_ORDI_PRON;
						break;
					case NUME_PARTI:
						cat = NUME_PARTI_PRON;
						break;
					default:
						break;

				} // switch (cat)...

				break;

			case PRON_POSE_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat = POSE_PRON;

				break;

			case PRON_POSE_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat = POSE_PRON;

				break;

			case PRON_POSE_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat = POSE_PRON;

				break;

			case PRON_POSE_MASC_SINGULAR:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat = POSE_PRON;

				break;

			case PRON_PROC_MASC_PLURAL:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != PRON_PERS_AT) && (cat != PRON_PERS_AT_REFLEX) &&
						(cat != CONTR_PRON_PERS_AT) && (cat != PRON_PERS_AT_ACUS)
						&& (cat != PRON_PERS_AT_DAT)&& (cat != CONTR_PRON_PERS_AT_DAT_AC)
						&& (cat != CONTR_PRON_PERS_AT_DAT_DAT_AC))
					cat = *(++cat_anterior);

				break;

			case PRON_PROC_MASC_SINGULAR:

				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != PRON_PERS_AT) && (cat != PRON_PERS_AT_REFLEX) &&
						(cat != CONTR_PRON_PERS_AT) && (cat != PRON_PERS_AT_ACUS)
						&& (cat != PRON_PERS_AT_DAT)&& (cat != CONTR_PRON_PERS_AT_DAT_AC)
						&& (cat != CONTR_PRON_PERS_AT_DAT_DAT_AC))
					cat = *(++cat_anterior);

				break;

			case PRON_PROC_FEM_PLURAL:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = PLURAL;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != PRON_PERS_AT) && (cat != PRON_PERS_AT_REFLEX) &&
						(cat != CONTR_PRON_PERS_AT) && (cat != PRON_PERS_AT_ACUS)
						&& (cat != PRON_PERS_AT_DAT)&& (cat != CONTR_PRON_PERS_AT_DAT_AC)
						&& (cat != CONTR_PRON_PERS_AT_DAT_DAT_AC))
					cat = *(++cat_anterior);

				break;

			case PRON_PROC_FEM_SINGULAR:

				recorre_frase_separada->xenero = FEMININO;
				recorre_frase_separada->numero = SINGULAR;

				cat_anterior = recorre_frase_separada->cat_gramatical;
				cat = *cat_anterior;

				while ( (cat != PRON_PERS_AT) && (cat != PRON_PERS_AT_REFLEX) &&
						(cat != CONTR_PRON_PERS_AT) && (cat != PRON_PERS_AT_ACUS)
						&& (cat != PRON_PERS_AT_DAT)&& (cat != CONTR_PRON_PERS_AT_DAT_AC)
						&& (cat != CONTR_PRON_PERS_AT_DAT_DAT_AC))
					cat = *(++cat_anterior);

				break;

			case PUNT:
				cat = recorre_frase_separada->cat_gramatical[0];
				break;

			case RELAT:
				cat = RELA;
				break;

				/*
					 case VERB:

					 if (esta_palabra_ten_a_categoria_de(PERIFRASE,recorre_frase_separada))
					 asigna_categoria(PERIFRASE, recorre_frase_separada);
					 else if (asigna_categoria(VERBO, recorre_frase_separada))
					 return 1;

					 break;
					 */
			case XERUND:
				cat = XERUNDIO;
				break;

			case VERBO_SINGULAR:
			case VERBO_PLURAL:

				recorre_frase_separada->xenero = NO_ASIGNADO;
				recorre_frase_separada->numero = NO_ASIGNADO;

				if (esta_palabra_ten_a_categoria_de(PERIFRASE, recorre_frase_separada)) {
					cat = PERIFRASE;
					perifrasis = 1;
				}
				else {
					if (esta_palabra_ten_a_categoria_de(XERUNDIO, recorre_frase_separada))
						cat = XERUNDIO;
					else
						cat = VERBO;
					if (*recorre_frase_separada->inf_verbal->artigo)
						salta_enclitico = '1';
				}
				break;
				/*
					 case NOM:
					 case ADX:
					 case DET:
					 case DET_POSESIVO:
					 case PRON:
					 case PRON_POSESIVO:
					 case PRON_PROC:
					 case CONTR_CONXUN_ART_DET:
				//        case CONT_CONX_ART:
				break;
				*/

			case EXCLAM:

				cat = EXCLA;
				break;

			default:

				fprintf(stderr, "Error en convierte_a_categorias_modelo_lenguaje: categoría de entrada no válida.\n");
				return 1;

		} // switch(*recorre_categorias...

		if ( (locucion == 0) && (perifrasis == 0) ) {
			if (asigna_categoria(cat, recorre_frase_separada)){
				return 1;
			}
			if (saca_prob){
				recorre_frase_separada->prob = recorre_categorias->prob;
			}
		} // if ( (locucion == 0) && (perifrasis == 0) )
		else {
			if (saca_prob){
				recorre_frase_separada->prob = recorre_categorias->prob;
			}
			asigna_categoria(cat, recorre_frase_separada);
			recorre_frase_separada++;
			while (esta_palabra_ten_a_categoria_de(cat, recorre_frase_separada)) {
				if (saca_prob){
					recorre_frase_separada->prob = recorre_categorias->prob;
				}
				asigna_categoria(cat, recorre_frase_separada);
				(recorre_frase_separada-1)->xenero=NO_ASIGNADO;
				(recorre_frase_separada-1)->numero=NO_ASIGNADO;
				recorre_frase_separada++;
			}
			recorre_frase_separada--;
			if (*recorre_frase_separada->inf_verbal->artigo)
				salta_enclitico = '1';
			if (locucion == 1)
				if (comprueba_contracciones(recorre_frase_separada,1)) {
					recorre_categorias++;
					numero_elementos--;
				} // if (comprueba_contracciones(...
				else {
					recorre_frase_separada->xenero=NO_ASIGNADO;
					recorre_frase_separada->numero=NO_ASIGNADO;
				}
			else {//perifrasis			
				recorre_frase_separada->xenero=NO_ASIGNADO;
				recorre_frase_separada->numero=NO_ASIGNADO;
			}

		} // else


	} // for (int cuenta_palabras...

	return 0;

}


/**
 * Función:   convierte_modelo_lenguaje_a_categorias_completo
 * \remarks	Versión de la función anterior en la que se utiliza el conjunto de
 * categorías completo.
 * \remarks Entrada:
 *			- numero_elementos: número de elementos del array anterior.
 *          - categorias: array con las categorías escogidas.
 * \remarks Entrada y Salida:
 *          - frase_separada: array en el que se encuentra la información de
 *            todas las categorías que se pueden relacionar con las palabras
 *            de la frase de entrada.
 * \remarks Valor devuelto:
 *          - En ausencia de error se devuelve un cero.
 * \remarks Objetivo:  Intoducir en el array frase_separada las categorías escogidas en
 *            el modelo de lenguaje.
 */

int convierte_modelo_lenguaje_a_categorias_completo(t_frase_separada *frase_separada,
		int numero_elementos, Categorias_modelo *categorias) {

	t_frase_separada *recorre_frase_separada = frase_separada;
	Categorias_modelo *recorre_categorias = categorias;
	char locucion;

	for (int cuenta_palabras = 0; cuenta_palabras < numero_elementos;
			cuenta_palabras++, recorre_frase_separada++, recorre_categorias++) {

		locucion = 0;
		asigna_categoria(recorre_categorias->cat, recorre_frase_separada);

        comprueba_locuciones(recorre_frase_separada, &locucion);

        if ( (locucion == 1) || (recorre_frase_separada->cat_gramatical[0] == PERIFRASE) ) {
        	recorre_frase_separada++;
        	while (esta_palabra_ten_a_categoria_de(recorre_categorias->cat, recorre_frase_separada)) {
            	asigna_categoria(recorre_categorias->cat, recorre_frase_separada);
                recorre_frase_separada++;
            }
            recorre_frase_separada--;
        }

	} // for (int cuenta_palabras...

	return 0;

}


/**
 * Función:   aplica_modelo_lenguaje_v1                                        
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Aplicar las probabilidades de los n-gramas obtenidos a partir    
 *            del entrenamiento para encontrar la secuencia de categorías más  
 *            probable para las palabras del texto de entrada.                 
 */

int Analisis_morfosintactico::aplica_modelo_lenguaje_v1(estructura_categorias *lista_categorias, int numero_elementos,
                           Categorias_modelo *categorias) {


    register estructura_categorias *punt_lista_categorias;
    Pentagrama penta_grama;
    Tetragrama tetra_grama;
    Trigrama tri_grama;
    Bigrama bi_grama;
    Categorias_modelo *categoria_escogida = categorias;
    int cuenta_palabras, cuenta_prob;
    unsigned char *cat0, *cat1, *cat2, *cat3, *cat4;
    float maximo, maximo_2, probabilidad_actual, probabilidad_actual_2;
		Categorias_modelo_enum categoria_escogida_2 = PUNT;

    switch (numero_elementos) {

        case 0:
            fprintf(stderr, "Error en aplica_modelo_lenguaje: numero_elementos = 0.\n");
            return 1;

        case 1:

            if (lista_categorias[0].numero_categorias == 1)
                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[0];
            else {
                cat0 = lista_categorias[0].categoria;

                float maximo = -1*FLT_MAX;
                int indice = 0;

                for (cuenta_prob = 0; *cat0 != 0; cat0++, cuenta_prob++) {
                    bi_grama[0] = PUNT;
                    bi_grama[1] = *cat0;
                    probabilidad_actual = modelo_lenguaje->calcula_prob_2_grama(bi_grama);
                    probabilidad_actual += lista_categorias->probabilidad[*cat0];
                    if (probabilidad_actual > maximo) {
                        maximo = probabilidad_actual;
                        indice = cuenta_prob;
                    } // if ( (probabilidad_actual...
                } // for (cuenta_prob...

                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[indice];
            } // else

            break;

        case 2:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                tri_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++) {
                        tri_grama[1] = *cat0;
                        tri_grama[2] = *cat1;

                    // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_3_grama(tri_grama);

                        if (cuenta_palabras == 0)
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0];
                        else
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat1];

                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;
                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat1;

                        } // if ( (probabilidad_actual...

                    } // for (cat2 = lista_categorias[2].categorias...

                categoria_escogida++;

            } // for (cuenta_palabras...


            break;

        case 3:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 3; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                maximo = -1*FLT_MAX;

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                }

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                }

                tetra_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                            tetra_grama[1] = *cat0;
                            tetra_grama[2] = *cat1;
                            tetra_grama[3] = *cat2;

                        // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_4_grama(tetra_grama);

                        if (cuenta_palabras == 0)
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0];
                        else
                            if (cuenta_palabras == 1)
                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat1];
                            else
                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat2];

                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;

                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                if (cuenta_palabras == 1)
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                else
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                        } // if ( (probabilidad_actual...
                    } // for (cat2 = lista_categorias[2].categorias...

               categoria_escogida++;

            } // for (cuenta_palabras...

            break;

        case 4:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 4; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0)
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0];
                                else
                                    if (cuenta_palabras == 1)
                                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1];
                                    else
                                        if (cuenta_palabras == 2)
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat2];
                                        else
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat3];


                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;

                                   switch (cuenta_palabras) {

                                   case 0:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                    break;
                                   case 1:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                    break;
                                   case 2:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    break;
                                   case 3:
                                    categoria_escogida ->cat= (Categorias_modelo_enum) *cat3;
                                    break;
                                   default:
                                    break;

                                   } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                        categoria_escogida++;

                } // for (cuenta_palabras = 0...

            break;

        case 5:

            punt_lista_categorias = lista_categorias;
            
            if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
            } // if (punt_lista_categorias...

            if (punt_lista_categorias->numero_categorias == 1)
                categoria_escogida->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
            else {

                maximo = -1*FLT_MAX;

                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat0];

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                } // if ( (probabilidad_actual...

                            } // for (cat3 = lista_categorias[3].categorias...

            } // else

            categoria_escogida++;

            punt_lista_categorias = lista_categorias + 1;

            for (cuenta_palabras = 1; cuenta_palabras < 5; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                                for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    // Calculamos las probabilidades:

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    if (cuenta_palabras == 1)
                                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1];
                                    else
                                        if (cuenta_palabras == 2)
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat2];
                                        else
                                            if (cuenta_palabras == 3)
                                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat3];
                                            else
                                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat4];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;

                                       switch (cuenta_palabras) {

                                       case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                       case 2:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                        break;
                                       case 3:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                                        break;
                                       case 4:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat4;
                                        break;
                                       default:
                                        break;

                                       } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                        categoria_escogida++;
                } // for (cuenta_palabras = 0...

            break;

        default:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...


                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0)
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0];
                                else
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat1];

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;

                                    switch (cuenta_palabras) {

                                    case 0:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                        break;
                                    case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                    default:
                                        break;
                                    } // switch
                                } // if (probabilidad_actual > maximo...
                            } // for (cat3 = lista_categorias[3].categorias...

                    categoria_escogida++;

            } // for (cuenta_palabras = 1;...

            // Ahora nos referimos a las palabras del medio (Las que no son las 3 primeras
            // ni las 3 últimas.

            for (cuenta_palabras = 2; cuenta_palabras < numero_elementos - 2; cuenta_palabras++,
                                                                    punt_lista_categorias++) {
                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                for (cat0 = (punt_lista_categorias - 2)->categoria; *cat0 != 0; cat0++)
                    for (cat1 = (punt_lista_categorias - 1)->categoria; *cat1 != 0; cat1++)
                        for (cat2 = punt_lista_categorias->categoria; *cat2 != 0; cat2++)
                            for (cat3 = (punt_lista_categorias + 1)->categoria; *cat3 != 0; cat3++)
                                for (cat4 = (punt_lista_categorias + 2)->categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat2];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    } // if (probabilidad_actual...

                                } // for (cat4 = ...

                categoria_escogida++;

            } // for (cuenta_palabras = 3; ...

            // Ahora nos ocupamos de las 3 últimas:


            maximo = -1*FLT_MAX;
            maximo_2 = maximo;

            for (cat0 = (punt_lista_categorias - 3)->categoria; *cat0 != 0; cat0++)
                for (cat1 = (punt_lista_categorias - 2)->categoria; *cat1 != 0; cat1++)
                    for (cat2 = (punt_lista_categorias - 1)->categoria; *cat2 != 0; cat2++)
                        for (cat3 = punt_lista_categorias->categoria; *cat3 != 0; cat3++)
                            for (cat4 = (punt_lista_categorias + 1)->categoria; *cat4 != 0; cat4++) {
                                penta_grama[0] = *cat0;
                                penta_grama[1] = *cat1;
                                penta_grama[2] = *cat2;
                                penta_grama[3] = *cat3;
                                penta_grama[4] = *cat4;

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                probabilidad_actual_2 = probabilidad_actual;

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat3];
                                probabilidad_actual_2 += punt_lista_categorias->probabilidad[*cat4];


                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;

                                } // if (probabilidad_actual...

                                if (probabilidad_actual_2 > maximo_2) {
                                    maximo_2 = probabilidad_actual_2;
                                    categoria_escogida_2 = (Categorias_modelo_enum) *cat4;

                                } // if (probabilidad_actual_2


                            } // for (cat4 = ...

                categoria_escogida++;
                categoria_escogida->cat = categoria_escogida_2;

    } // switch (numero_elementos)

    return 0;

}


/**
 * Función:   aplica_modelo_lenguaje_v2                                        
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Versión de la función anterior en la que se emplea un Viterbi    
 *            para seleccionar la secuencia de categorías óptima.              
 */

int Analisis_morfosintactico::aplica_modelo_lenguaje_v2(estructura_categorias *lista_categorias, int numero_elementos,
                           Categorias_modelo *categorias) {

    int contador;
    unsigned char *secuencia_optima, *apunta_secuencia;
    Categorias_modelo *apunta_categorias = categorias;
    estructura_categorias *recorre_categorias = lista_categorias + 1;
    Viterbi_categorias Viterbi;

    Viterbi.inicializa(modelo_lenguaje);

    if (Viterbi.inicia_algoritmo_Viterbi_categorias(lista_categorias, numero_elementos))
        return 1;

    for (contador = 1; contador < numero_elementos; contador++, recorre_categorias++)
        if (Viterbi.siguiente_recursion_Viterbi_categorias(recorre_categorias, contador))
            return 1;

    if ( (secuencia_optima = Viterbi.devuelve_secuencia_optima(&numero_elementos)) == NULL)
        return 1;

    // Ahora lo convertimos al tipo Categorias_modelo:

    apunta_secuencia = secuencia_optima + 1; // Para eliminar el PUNTO que añadimos al principio.

    for (contador = 1; contador < numero_elementos; contador++)
        apunta_categorias++->cat = (Categorias_modelo_enum) *apunta_secuencia++;


    free(secuencia_optima);

    Viterbi.libera_memoria_algoritmo_categorias();

    return 0;   
}

/**
 * Función:   aplica_modelo_lenguaje_v3                                        
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Aplicar las probabilidades de los n-gramas obtenidos a partir    
 *            del entrenamiento para encontrar la secuencia de categorías más  
 *            probable para las palabras del texto de entrada.                 
 */

int Analisis_morfosintactico::aplica_modelo_lenguaje_v3(estructura_categorias *lista_categorias, int numero_elementos,
                           Categorias_modelo *categorias) {


    register estructura_categorias *punt_lista_categorias;
    Pentagrama penta_grama;
    Tetragrama tetra_grama;
    Trigrama tri_grama;
    Bigrama bi_grama;
    Categorias_modelo *categoria_escogida = categorias;
		Categorias_modelo_enum categoria_escogida_2 = PUNT;
    int cuenta_palabras, cuenta_prob, indice;
    unsigned char *cat0, *cat1, *cat2, *cat3, *cat4;
    float probabilidad_actual, probabilidad_actual_2;


    switch (numero_elementos) {

        case 0:
            fprintf(stderr, "Error en aplica_modelo_lenguaje: numero_elementos = 0.\n");
            return 1;

        case 1:

            if (lista_categorias[0].numero_categorias == 1 && !saca_prob){
               categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[0];
						}
            else {
                cat0 = lista_categorias[0].categoria;

                categoria_escogida->prob = -1 * FLT_MAX;
                //int indice = 0;

                for (cuenta_prob = 0; *cat0 != 0; cat0++, cuenta_prob++) {
                    bi_grama[0] = PUNT;
                    bi_grama[1] = *cat0;
                    probabilidad_actual = modelo_lenguaje->calcula_prob_2_grama(bi_grama);
                    probabilidad_actual += lista_categorias->probabilidad[*cat0];
                    if (probabilidad_actual > categoria_escogida->prob) {
                        categoria_escogida->prob = probabilidad_actual;
                        indice = cuenta_prob;
                    } // if ( (probabilidad_actual...
                } // for (cuenta_prob...

								categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[indice];
            } // else

            break;

        case 2:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                } // if (punt_lista_categorias...

                categoria_escogida->prob = -1*FLT_MAX;

                tri_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++) {
                        tri_grama[1] = *cat0;
                        tri_grama[2] = *cat1;

                    // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_3_grama(tri_grama);

                        if (cuenta_palabras == 0)
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                  (punt_lista_categorias + 1)->probabilidad[*cat1];
                        else
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                  (punt_lista_categorias - 1)->probabilidad[*cat0];

                        if (probabilidad_actual > categoria_escogida->prob) {
                            categoria_escogida->prob = probabilidad_actual;
                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat1;

                        } // if ( (probabilidad_actual...

                    } // for (cat2 = lista_categorias[2].categorias...

								categoria_escogida++;

            } // for (cuenta_palabras...


            break;

        case 3:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 3; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                categoria_escogida->prob = -1*FLT_MAX;

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                }

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                }

                tetra_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                            tetra_grama[1] = *cat0;
                            tetra_grama[2] = *cat1;
                            tetra_grama[3] = *cat2;

                        // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_4_grama(tetra_grama);

                        if (cuenta_palabras == 0)
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                   (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                   (punt_lista_categorias + 2)->probabilidad[*cat2];
                        else
                            if (cuenta_palabras == 1)
                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                       (punt_lista_categorias + 1)->probabilidad[*cat2];
                            else
                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                       (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat1];

                        if (probabilidad_actual > categoria_escogida->prob) {
                            categoria_escogida->prob = probabilidad_actual;

                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                if (cuenta_palabras == 1)
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                else
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                        } // if ( (probabilidad_actual...
                    } // for (cat2 = lista_categorias[2].categorias...

							 categoria_escogida++;

            } // for (cuenta_palabras...

            break;

        case 4:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 4; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                } // if (punt_lista_categorias...

                categoria_escogida->prob = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0)
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                           (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                           (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                           (punt_lista_categorias + 3)->probabilidad[*cat3];
                                else
                                    if (cuenta_palabras == 1)
                                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                               (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                               (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                               (punt_lista_categorias + 2)->probabilidad[*cat3];
                                    else
                                        if (cuenta_palabras == 2)
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat3];
                                        else
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 3)->probabilidad[*cat0];


                                if (probabilidad_actual > categoria_escogida->prob) {
                                    categoria_escogida->prob = probabilidad_actual;

                                   switch (cuenta_palabras) {

                                   case 0:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                    break;
                                   case 1:
                                    categoria_escogida ->cat= (Categorias_modelo_enum) *cat1;
                                    break;
                                   case 2:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    break;
                                   case 3:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                                    break;
                                   default:
                                    break;

                                   } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

												categoria_escogida++;

                } // for (cuenta_palabras = 0...

            break;

        case 5:

            punt_lista_categorias = lista_categorias;
            
            if (punt_lista_categorias->numero_categorias == 0) {
            	fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                return 1;
            } // if (punt_lista_categorias...

            
             if (punt_lista_categorias->numero_categorias == 1 && !saca_prob){
                 categoria_escogida->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
						   }
             else {
             

                categoria_escogida->prob = -1*FLT_MAX;

                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat3];

                                if (probabilidad_actual > categoria_escogida->prob) {
                                    categoria_escogida->prob = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                } // if ( (probabilidad_actual...

                            } // for (cat3 = lista_categorias[3].categorias...


           } // else


						categoria_escogida++;

            punt_lista_categorias = lista_categorias + 1;

            for (cuenta_palabras = 1; cuenta_palabras < 5; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                } // if (punt_lista_categorias...

                categoria_escogida->prob = -1*FLT_MAX;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                                for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    // Calculamos las probabilidades:

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    if (cuenta_palabras == 1)
                                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat3] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat4];
                                    else
                                        if (cuenta_palabras == 2)
                                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                                   (punt_lista_categorias + 2)->probabilidad[*cat4];
                                        else
                                            if (cuenta_palabras == 3)
                                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                                                                       (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                                       (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                                       (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                                                       (punt_lista_categorias + 1)->probabilidad[*cat4];
                                            else
                                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat4] +
                                                                       (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                                                       (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                                                       (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                                                       (punt_lista_categorias - 3)->probabilidad[*cat1];

                                    if (probabilidad_actual > categoria_escogida->prob) {
                                        categoria_escogida->prob = probabilidad_actual;

                                       switch (cuenta_palabras) {

                                       case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                       case 2:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                        break;
                                       case 3:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                                        break;
                                       case 4:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat4;
                                        break;
                                       default:
                                        break;

                                       } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

												categoria_escogida++;
                } // for (cuenta_palabras = 0...

            break;

        default:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                } // if (punt_lista_categorias...


                categoria_escogida->prob = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0)
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                        (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                        (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                        (punt_lista_categorias + 3)->probabilidad[*cat3];
                                else
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                        (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                        (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                        (punt_lista_categorias + 2)->probabilidad[*cat3];

                                if (probabilidad_actual > categoria_escogida->prob) {
                                    categoria_escogida->prob = probabilidad_actual;

                                    switch (cuenta_palabras) {

                                    case 0:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                        break;
                                    case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                    default:
                                        break;
                                    } // switch
                                } // if (probabilidad_actual > categoria_escogida->prob...
                            } // for (cat3 = lista_categorias[3].categorias...

										categoria_escogida++;

            } // for (cuenta_palabras = 1;...

            // Ahora nos referimos a las palabras del medio (Las que no son las 3 primeras
            // ni las 3 últimas.

            for (cuenta_palabras = 2; cuenta_palabras < numero_elementos - 2; cuenta_palabras++,
                                                                    punt_lista_categorias++) {
                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1 && !saca_prob) {
                   categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                   continue;
                } // if (punt_lista_categorias...

                categoria_escogida->prob = -1*FLT_MAX;

                for (cat0 = (punt_lista_categorias - 2)->categoria; *cat0 != 0; cat0++)
                    for (cat1 = (punt_lista_categorias - 1)->categoria; *cat1 != 0; cat1++)
                        for (cat2 = punt_lista_categorias->categoria; *cat2 != 0; cat2++)
                            for (cat3 = (punt_lista_categorias + 1)->categoria; *cat3 != 0; cat3++)
                                for (cat4 = (punt_lista_categorias + 2)->categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                          (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                          (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                          (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                          (punt_lista_categorias + 2)->probabilidad[*cat4];

                                    if (probabilidad_actual > categoria_escogida->prob) {
                                        categoria_escogida->prob = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    } // if (probabilidad_actual...

                                } // for (cat4 = ...

								categoria_escogida++;

            } // for (cuenta_palabras = 3; ...

            // Ahora nos ocupamos de las 3 últimas:


            categoria_escogida->prob = -1*FLT_MAX;
            (categoria_escogida+1)->prob = -1*FLT_MAX;

            for (cat0 = (punt_lista_categorias - 3)->categoria; *cat0 != 0; cat0++)
                for (cat1 = (punt_lista_categorias - 2)->categoria; *cat1 != 0; cat1++)
                    for (cat2 = (punt_lista_categorias - 1)->categoria; *cat2 != 0; cat2++)
                        for (cat3 = punt_lista_categorias->categoria; *cat3 != 0; cat3++)
                            for (cat4 = (punt_lista_categorias + 1)->categoria; *cat4 != 0; cat4++) {
                                penta_grama[0] = *cat0;
                                penta_grama[1] = *cat1;
                                penta_grama[2] = *cat2;
                                penta_grama[3] = *cat3;
                                penta_grama[4] = *cat4;

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                probabilidad_actual_2 = probabilidad_actual;

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                                                       (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                       (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                                       (punt_lista_categorias + 1)->probabilidad[*cat4];
                                probabilidad_actual_2 += punt_lista_categorias->probabilidad[*cat4] +
                                                         (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                                         (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                                         (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                                         (punt_lista_categorias - 3)->probabilidad[*cat1];


                                if (probabilidad_actual > categoria_escogida->prob) {
                                    categoria_escogida->prob = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;

                                } // if (probabilidad_actual...

                                if (probabilidad_actual_2 > (categoria_escogida+1)->prob) {
                                    (categoria_escogida+1)->prob = probabilidad_actual_2;
                                    categoria_escogida_2 = (Categorias_modelo_enum) *cat4;

                                } // if (probabilidad_actual_2


                            } // for (cat4 = ...
                categoria_escogida++;
                categoria_escogida->cat = categoria_escogida_2;

    } // switch (numero_elementos)

    return 0;

}


/**
 * Función:   aplica_modelo_lenguaje_v5                                        
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Aplicar las probabilidades de los n-gramas obtenidos a partir    
 *            del entrenamiento para encontrar la secuencia de categorías más  
 *            probable para las palabras del texto de entrada.                 
 * \remarks NOTA:	- Se diferencia de v3 en que, en este caso, se aceptan las 		   
 *			  decisiones anteriores como buenas.							   
 */

int Analisis_morfosintactico::aplica_modelo_lenguaje_v5(estructura_categorias *lista_categorias, int numero_elementos,
                           Categorias_modelo *categorias) {


    register estructura_categorias *punt_lista_categorias;
    Pentagrama penta_grama;
    Tetragrama tetra_grama;
    Trigrama tri_grama;
    Bigrama bi_grama;
    Categorias_modelo *categoria_escogida = categorias;
    int cuenta_palabras, cuenta_prob;
    unsigned char *cat0, *cat1, *cat2, *cat3, *cat4;
    float maximo, probabilidad_actual;


    switch (numero_elementos) {

        case 0:
            fprintf(stderr, "Error en aplica_modelo_lenguaje: numero_elementos = 0.\n");
            return 1;

        case 1:

            if (lista_categorias[0].numero_categorias == 1)
                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[0];
            else {
                cat0 = lista_categorias[0].categoria;

                float maximo = -1*FLT_MAX;
                int indice = 0;

                for (cuenta_prob = 0; *cat0 != 0; cat0++, cuenta_prob++) {
                    bi_grama[0] = PUNT;
                    bi_grama[1] = *cat0;
                    probabilidad_actual = modelo_lenguaje->calcula_prob_2_grama(bi_grama);
                    probabilidad_actual += lista_categorias->probabilidad[*cat0];
                    if (probabilidad_actual > maximo) {
                        maximo = probabilidad_actual;
                        indice = cuenta_prob;
                    } // if ( (probabilidad_actual...
                } // for (cuenta_prob...

                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[indice];
            } // else

            break;

        case 2:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                tri_grama[0] = PUNT;

                if (cuenta_palabras == 0) {

                    for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                        for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++) {
                            tri_grama[1] = *cat0;
                            tri_grama[2] = *cat1;

                        // Calculamos las probabilidades:

                            probabilidad_actual = modelo_lenguaje->calcula_prob_3_grama(tri_grama);

                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                            					   (punt_lista_categorias + 1)->probabilidad[*cat1];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            } // if ( (probabilidad_actual...

                        } // for (cat2 = lista_categorias[2].categorias...

				} // if (cuenta_palabras == 0)
                else {

                    tri_grama[0] = PUNT;
                	cat0 = (unsigned char *) (categoria_escogida - 1);
                    tri_grama[1] = *cat0;

                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++) {
                        tri_grama[2] = *cat1;

                        // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_3_grama(tri_grama);

                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                        					   (punt_lista_categorias - 1)->probabilidad[*cat0];

                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;
                            categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                        } // if ( (probabilidad_actual...

                    } // for (cat2 = lista_categorias[2].categorias...

                } // else

                categoria_escogida++;

            } // for (cuenta_palabras...


            break;

        case 3:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 3; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                maximo = -1*FLT_MAX;

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                }

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                }

                tetra_grama[0] = PUNT;

                if (cuenta_palabras == 0) {

                    for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                        for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                            for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                                tetra_grama[1] = *cat0;
                                tetra_grama[2] = *cat1;
                                tetra_grama[3] = *cat2;

                            // Calculamos las probabilidades:

                            probabilidad_actual = modelo_lenguaje->calcula_prob_4_grama(tetra_grama);

                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                   (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                   (punt_lista_categorias + 2)->probabilidad[*cat2];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            } // if ( (probabilidad_actual...
                        } // for (cat2 = lista_categorias[2].categorias...

                } // if (cuenta_palabras == 0)
                else
                if (cuenta_palabras == 1) {
					cat0 = (unsigned char *) (categoria_escogida - 1);
                    tetra_grama[1] = *cat0;

                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                    	for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                            tetra_grama[2] = *cat1;
                            tetra_grama[3] = *cat2;

                            // Calculamos las probabilidades:

                            probabilidad_actual = modelo_lenguaje->calcula_prob_4_grama(tetra_grama);

                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                   (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                   (punt_lista_categorias + 1)->probabilidad[*cat2];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                            } // if ( (probabilidad_actual...
                        } // for (cat2 = lista_categorias[2].categorias...

                } // if (cuenta_palabras == 1)
                else {

                	cat0 = (unsigned char *) (categoria_escogida - 2);
                    tetra_grama[1] = *cat0;
                    cat1 = (unsigned char *) (categoria_escogida - 1);
                    tetra_grama[2] = *cat1;

                    for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                    	tetra_grama[3] = *cat2;

                        // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_4_grama(tetra_grama);

                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                        					   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                               (punt_lista_categorias - 1)->probabilidad[*cat1];

                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;
							categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                            } // if ( (probabilidad_actual...
                        } // for (cat2 = lista_categorias[2].categorias...

                } // else

               categoria_escogida++;

            } // for (cuenta_palabras...

            break;

        case 4:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 4; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

				if (cuenta_palabras == 0) {

                    for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                        for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                            for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                                for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                    penta_grama[1] = *cat0;
                                    penta_grama[2] = *cat1;
                                    penta_grama[3] = *cat2;
                                    penta_grama[4] = *cat3;

                                    // Calculamos las probabilidades:

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                           (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                           (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                           (punt_lista_categorias + 3)->probabilidad[*cat3];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                    } // if ( (probabilidad_actual...

                            } // for (cat3 = lista_categorias[3].categorias...

				} // if (cuenta_palabras == 0)
                else
                if (cuenta_palabras == 1) {

					cat0 = (unsigned char *) (categoria_escogida - 1);
                    penta_grama[1] = *cat0;

                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                       (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                       (punt_lista_categorias + 2)->probabilidad[*cat3];

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                } // if (cuenta_palabras == 1)
                else
                if (cuenta_palabras == 2) {

                	cat0 = (unsigned char *) (categoria_escogida - 2);
                    penta_grama[1] = *cat0;
                    cat1 = (unsigned char *) (categoria_escogida - 1);
                    penta_grama[2] = *cat1;

                    for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                        for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                            penta_grama[3] = *cat2;
                            penta_grama[4] = *cat3;

                            // Calculamos las probabilidades:

                            probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                            if (cuenta_palabras == 2)
                            	probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                               (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                               (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                               (punt_lista_categorias + 1)->probabilidad[*cat3];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                            } // if ( (probabilidad_actual...

                    } // for (cat3 = lista_categorias[3].categorias...


                } //if (cuenta_palabras == 2)
                else {

                	cat0 = (unsigned char *) (categoria_escogida - 3);
                    penta_grama[1] = *cat0;
                    cat1 = (unsigned char *) (categoria_escogida - 2);
                    penta_grama[2] = *cat1;
                    cat2 = (unsigned char *) (categoria_escogida - 1);
                    penta_grama[3] = *cat2;

                    for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                    	penta_grama[4] = *cat3;

                        // Calculamos las probabilidades:

                        probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                                                           (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                           (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                           (punt_lista_categorias - 3)->probabilidad[*cat0];


                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;
                            categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                        } // if ( (probabilidad_actual...

                } // for (cat3 = lista_categorias[3].categorias...


                } // else

                categoria_escogida++;

                } // for (cuenta_palabras = 0...

            break;

        case 5:

            punt_lista_categorias = lista_categorias;

            if (punt_lista_categorias->numero_categorias == 0) {
            	fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                return 1;
            } // if (punt_lista_categorias...

            if (punt_lista_categorias->numero_categorias == 1)
                categoria_escogida->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
            else {

                maximo = -1*FLT_MAX;

                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat3];

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                } // if ( (probabilidad_actual...

                            } // for (cat3 = lista_categorias[3].categorias...

            } // else

            categoria_escogida++;

            punt_lista_categorias = lista_categorias + 1;

            for (cuenta_palabras = 1; cuenta_palabras < 5; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;


                if (cuenta_palabras == 1) {

                    cat0 = (unsigned char *) (categoria_escogida - 1);
                    penta_grama[0] = *cat0;

                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                                for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    // Calculamos las probabilidades:

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat3] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat4];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

					} // if (cuenta_palabras == 1)
                    else
                    if (cuenta_palabras == 2) {

						cat0 = (unsigned char *) (categoria_escogida - 2);
                        penta_grama[0] = *cat0;
                        cat1 = (unsigned char *) (categoria_escogida - 1);
                        penta_grama[1] = *cat1;

                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                                for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    // Calculamos las probabilidades:

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                                   (punt_lista_categorias + 2)->probabilidad[*cat4];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                    } // if (cuenta_palabras == 2)
                    else
                    if (cuenta_palabras == 3) {

						cat0 = (unsigned char *) (categoria_escogida - 3);
                        penta_grama[0] = *cat0;
                        cat1 = (unsigned char *) (categoria_escogida - 2);
                        penta_grama[1] = *cat1;
                        cat2 = (unsigned char *) (categoria_escogida - 1);
                        penta_grama[2] = *cat2;

                        for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                        	for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                            	penta_grama[3] = *cat3;
                                penta_grama[4] = *cat4;

                                // Calculamos las probabilidades:

                                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat4];

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                            } // if ( (probabilidad_actual...

                    	} // for (cat3 = lista_categorias[3].categorias...

                    } // if (cuenta_palabras == 3)
                    else {

						cat0 = (unsigned char *) (categoria_escogida - 4);
                        penta_grama[0] = *cat0;
                        cat1 = (unsigned char *) (categoria_escogida - 3);
                        penta_grama[1] = *cat1;
                        cat2 = (unsigned char *) (categoria_escogida - 2);
                        penta_grama[2] = *cat2;
                        cat3 = (unsigned char *) (categoria_escogida - 1);
                        penta_grama[3] = *cat3;

                        for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                        	penta_grama[4] = *cat4;

                            // Calculamos las probabilidades:

                            probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat4] +
                                                               (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                                               (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                                               (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                                               (punt_lista_categorias - 3)->probabilidad[*cat1];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat4;
                        	} // if ( (probabilidad_actual...

                		} // for (cat3 = lista_categorias[3].categorias...

                    } // else

                    categoria_escogida++;
                } // for (cuenta_palabras = 0...

            break;

        default:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...


                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                if (cuenta_palabras == 0) {

                    for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                        for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                            for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                                for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                    penta_grama[1] = *cat0;
                                    penta_grama[2] = *cat1;
                                    penta_grama[3] = *cat2;
                                    penta_grama[4] = *cat3;

                                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat0] +
                                                            (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                            (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                            (punt_lista_categorias + 3)->probabilidad[*cat3];

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                    } // if (probabilidad_actual > maximo...
                                } // for (cat3 = lista_categorias[3].categorias...
						} // if (cuenta_palabras == 0)
                        else {

                        	cat0 = (unsigned char *) (categoria_escogida - 1);
                            penta_grama[1] = *cat0;

                            for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                                for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                                    for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                        penta_grama[2] = *cat1;
                                        penta_grama[3] = *cat2;
                                        penta_grama[4] = *cat3;

                                        probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                        probabilidad_actual += punt_lista_categorias->probabilidad[*cat1] +
                                                                (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                                (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                                (punt_lista_categorias + 2)->probabilidad[*cat3];

                                        if (probabilidad_actual > maximo) {
                                            maximo = probabilidad_actual;
                                            categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        } // if (probabilidad_actual > maximo...
                                    } // for (cat3 = lista_categorias[3].categorias...
                        } // else
                        
                    categoria_escogida++;

            } // for (cuenta_palabras = 0;...

            // Ahora nos referimos a las palabras del medio (Las que no son las 3 primeras
            // ni las 3 últimas.

            for (cuenta_palabras = 2; cuenta_palabras < numero_elementos - 2; cuenta_palabras++,
                                                                    punt_lista_categorias++) {
                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                cat0 = (unsigned char *) (categoria_escogida - 2);
                penta_grama[0] = *cat0;
                cat1 = (unsigned char *) (categoria_escogida - 1);
                penta_grama[1] = *cat1;

                for (cat2 = punt_lista_categorias->categoria; *cat2 != 0; cat2++)
                    for (cat3 = (punt_lista_categorias + 1)->categoria; *cat3 != 0; cat3++)
                        for (cat4 = (punt_lista_categorias + 2)->categoria; *cat4 != 0; cat4++) {
                            penta_grama[2] = *cat2;
                            penta_grama[3] = *cat3;
                            penta_grama[4] = *cat4;

                            probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                            probabilidad_actual += punt_lista_categorias->probabilidad[*cat2] +
                                                  (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                  (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                  (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                  (punt_lista_categorias + 2)->probabilidad[*cat4];

                            if (probabilidad_actual > maximo) {
                                maximo = probabilidad_actual;
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                            } // if (probabilidad_actual...

                        } // for (cat4 = ...

                categoria_escogida++;

            } // for (cuenta_palabras = 3; ...

            // Ahora nos ocupamos de las 3 últimas:


            maximo = -1*FLT_MAX;

            cat0 = (unsigned char *) (categoria_escogida - 3);
            penta_grama[0] = *cat0;
            cat1 = (unsigned char *) (categoria_escogida - 2);
            penta_grama[1] = *cat1;
            cat2 = (unsigned char *) (categoria_escogida - 1);
            penta_grama[2] = *cat2;

            for (cat3 = punt_lista_categorias->categoria; *cat3 != 0; cat3++)
            	for (cat4 = (punt_lista_categorias + 1)->categoria; *cat4 != 0; cat4++) {
                	penta_grama[3] = *cat3;
                    penta_grama[4] = *cat4;

                    probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                    probabilidad_actual += punt_lista_categorias->probabilidad[*cat3] +
                    					   (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                           (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                           (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                           (punt_lista_categorias + 1)->probabilidad[*cat4];

                    if (probabilidad_actual > maximo) {
                    	maximo = probabilidad_actual;
                        categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                    } // if (probabilidad_actual...

                } // for (cat4 = ...

            categoria_escogida++;

            maximo = -1*FLT_MAX;

            cat0 = (unsigned char *) (categoria_escogida - 4);
            penta_grama[0] = *cat0;
            cat1 = (unsigned char *) (categoria_escogida - 3);
            penta_grama[1] = *cat1;
            cat2 = (unsigned char *) (categoria_escogida - 2);
            penta_grama[2] = *cat2;
            cat3 = (unsigned char *) (categoria_escogida - 1);
            penta_grama[3] = *cat3;

            for (cat4 = (punt_lista_categorias + 1)->categoria; *cat4 != 0; cat4++) {
            	penta_grama[4] = *cat4;

                probabilidad_actual = modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                probabilidad_actual += punt_lista_categorias->probabilidad[*cat4] +
                					   (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                       (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                       (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                       (punt_lista_categorias - 3)->probabilidad[*cat1];


                if (probabilidad_actual > maximo) {
                	maximo = probabilidad_actual;
                    categoria_escogida->cat = (Categorias_modelo_enum) *cat4;
                } // if (probabilidad_actual...

            } // for (cat4 = ...


    } // switch (numero_elementos)

    return 0;

}





/**
 * Función:   aplica_modelo_léxico	                                           
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Aplicar las probabilidades del módelo léxico, sin modelo de 	   
 *			  lenguaje, para comprobar su influencia.						   
 */

int aplica_modelo_lexico(estructura_categorias *lista_categorias, int numero_elementos,
                        Categorias_modelo *categorias) {


    register estructura_categorias *punt_lista_categorias;
    Categorias_modelo *categoria_escogida = categorias;
    int cuenta_palabras, contador;
    unsigned char mejor_categoria = 0;
    float maximo, *apunta_probabilidad;


    punt_lista_categorias = lista_categorias;

    for (cuenta_palabras = 0; cuenta_palabras < numero_elementos; cuenta_palabras++,
                                                           punt_lista_categorias++) {

    	if (punt_lista_categorias->numero_categorias == 0) {
        	fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
            return 1;
        } // if (punt_lista_categorias...

        if (punt_lista_categorias->numero_categorias == 1) {
        	categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
            continue;
        } // if (punt_lista_categorias...

        maximo = -1*FLT_MAX;

        apunta_probabilidad = punt_lista_categorias->probabilidad;

        for (contador = 0; contador < MAX_NUM_CATEGORIAS; contador++, apunta_probabilidad++) {

            if (*apunta_probabilidad)
	        	if (*apunta_probabilidad > maximo) {
					maximo = *apunta_probabilidad;
					mejor_categoria = (unsigned char) contador;
	            } // if (*apunta_probabilidad > maximo)

        } // for (contador = 0; ...

		categoria_escogida++->cat = (Categorias_modelo_enum) mejor_categoria;
        
    } // for (cuenta_palabras = 0; ...


	return 0;

}

/**
 * Función:   aplica_modelo_lenguaje_v4                                        
 * \remarks Entrada:
 *			- lista_categorías: cadena indexada en la que se encuentra la      
 *            información de todas las categorías que Cotovía asigna a las     
 *            palabras de la frase de entrada.                                 
 *          - numero_elementos: número de elementos del array anterior.        
 *	\remarks Salida:
 *			- categorias: cadena con las categorías más probables.             
 *            También se podría incluir esta información en frase_separada.    
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error se devuelve un cero.                        
 * \remarks Objetivo:  Aplicar las probabilidades de los n-gramas obtenidos a partir    
 *            del entrenamiento para encontrar la secuencia de categorías más  
 *            probable para las palabras del texto de entrada.                 
 */

int Analisis_morfosintactico::aplica_modelo_lenguaje_v4(estructura_categorias *lista_categorias, int numero_elementos,
                           Categorias_modelo *categorias) {


    register estructura_categorias *punt_lista_categorias;
    Pentagrama penta_grama;
    Tetragrama tetra_grama;
    Trigrama tri_grama;
    Bigrama bi_grama;
    Categorias_modelo *categoria_escogida = categorias;
		Categorias_modelo_enum categoria_escogida_2 = PUNT;
    int cuenta_palabras, cuenta_prob;
    unsigned char *cat0, *cat1, *cat2, *cat3, *cat4;
    float maximo, maximo_2, probabilidad_actual, probabilidad_actual_2;
    float factor;


	//for (int kk=0;kk<MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS;kk++)	factores[kk]=opciones.factor;


	//factores[NOM_GEN]=opciones.factor2;
	//factores[DET_GEN]=opciones.factor;
	switch (numero_elementos) {

        case 0:
            fprintf(stderr, "Error en aplica_modelo_lenguaje: numero_elementos = 0.\n");
            return 1;

        case 1:

            if (lista_categorias[0].numero_categorias == 1)
                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[0];
            else {
                cat0 = lista_categorias[0].categoria;

                float maximo = -1*FLT_MAX;
                int indice = 0;

                for (cuenta_prob = 0; *cat0 != 0; cat0++, cuenta_prob++) {
                    bi_grama[0] = PUNT;
                    bi_grama[1] = *cat0;
                    factor=factores[traduce_cat_modelo_a_generica[*cat0]];

                    probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_2_grama(bi_grama);
                    probabilidad_actual += factor * lista_categorias->probabilidad[*cat0];
                    if (probabilidad_actual > maximo) {
                        maximo = probabilidad_actual;
                        indice = cuenta_prob;
                    } // if ( (probabilidad_actual...
                } // for (cuenta_prob...

                categorias->cat = (Categorias_modelo_enum) lista_categorias[0].categoria[indice];
            } // else

            break;

        case 2:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                tri_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++) {
                        tri_grama[1] = *cat0;
                        tri_grama[2] = *cat1;

                    // Calculamos las probabilidades:

                        //probabilidad_actual = opciones.factor * calcula_prob_3_grama(tri_grama);

                        if (cuenta_palabras == 0){
                            factor=factores[traduce_cat_modelo_a_generica[*cat0]];
                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_3_grama(tri_grama);
                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat0] +
                                                  (punt_lista_categorias + 1)->probabilidad[*cat1]);
                        }
                        else{
                            factor=factores[traduce_cat_modelo_a_generica[*cat1]];
                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_3_grama(tri_grama);
                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat1] +
                                                  (punt_lista_categorias - 1)->probabilidad[*cat0]);
                        }
                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;
                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat1;

                        } // if ( (probabilidad_actual...

                    } // for (cat2 = lista_categorias[2].categorias...

                categoria_escogida++;

            } // for (cuenta_palabras...


            break;

        case 3:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 3; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                maximo = -1*FLT_MAX;

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                }

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                }

                tetra_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++) {
                            tetra_grama[1] = *cat0;
                            tetra_grama[2] = *cat1;
                            tetra_grama[3] = *cat2;

                        // Calculamos las probabilidades:

                        //probabilidad_actual = opciones.factor * calcula_prob_4_grama(tetra_grama);

                        if (cuenta_palabras == 0){
                            factor=factores[traduce_cat_modelo_a_generica[*cat0]];
                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_4_grama(tetra_grama);
                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat0] +
                                                   (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                   (punt_lista_categorias + 2)->probabilidad[*cat2]);
                        }
                        else
                            if (cuenta_palabras == 1){
                                factor=factores[traduce_cat_modelo_a_generica[*cat1]];
                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_4_grama(tetra_grama);
                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat1] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                       (punt_lista_categorias + 1)->probabilidad[*cat2]);
                            }
                            else {
                                factor=factores[traduce_cat_modelo_a_generica[*cat2]];
                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_4_grama(tetra_grama);
                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat2] +
                                                       (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat1]);
                            }
                        if (probabilidad_actual > maximo) {
                            maximo = probabilidad_actual;

                            if (cuenta_palabras == 0)
                                categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                            else
                                if (cuenta_palabras == 1)
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                else
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                        } // if ( (probabilidad_actual...
                    } // for (cat2 = lista_categorias[2].categorias...

               categoria_escogida++;

            } // for (cuenta_palabras...

            break;

        case 4:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 4; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                //probabilidad_actual = opciones.factor * modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0){
                                    factor=factores[traduce_cat_modelo_a_generica[*cat0]];
                                    probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat0] +
                                                           (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                           (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                           (punt_lista_categorias + 3)->probabilidad[*cat3]);
                                }
                                else
                                    if (cuenta_palabras == 1) {
                                        factor=factores[traduce_cat_modelo_a_generica[*cat1]];
                                        probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                        probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat1] +
                                                               (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                               (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                               (punt_lista_categorias + 2)->probabilidad[*cat3]);
                                    }
                                    else
                                        if (cuenta_palabras == 2) {
                                            factor=factores[traduce_cat_modelo_a_generica[*cat2]];
                                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat3]);
                                        }
                                        else {
                                            factor=factores[traduce_cat_modelo_a_generica[*cat3]];
                                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat3] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 3)->probabilidad[*cat0]);

                                        }
                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;

                                   switch (cuenta_palabras) {

                                   case 0:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                    break;
                                   case 1:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                    break;
                                   case 2:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    break;
                                   case 3:
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                                    break;
                                   default:
                                    break;

                                   } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                        categoria_escogida++;

                } // for (cuenta_palabras = 0...

            break;

        case 5:

            punt_lista_categorias = lista_categorias;

            if (punt_lista_categorias->numero_categorias == 0) {
            	fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                return 1;
            } // if (punt_lista_categorias...

            if (punt_lista_categorias->numero_categorias == 1)
                categoria_escogida->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
            else {

                maximo = -1*FLT_MAX;

                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                // Calculamos las probabilidades:

                                factor=factores[traduce_cat_modelo_a_generica[*cat0]];
                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat3]);

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                } // if ( (probabilidad_actual...

                            } // for (cat3 = lista_categorias[3].categorias...

            } // else

            categoria_escogida++;

            punt_lista_categorias = lista_categorias + 1;

            for (cuenta_palabras = 1; cuenta_palabras < 5; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++)
                                for (cat4 = lista_categorias[4].categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    // Calculamos las probabilidades:

                                    //probabilidad_actual = opciones.factor * modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                    if (cuenta_palabras == 1){
                                        factor=factores[traduce_cat_modelo_a_generica[*cat1]];
                                        probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                        probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat1] +
                                                (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                (punt_lista_categorias + 2)->probabilidad[*cat3] +
                                                (punt_lista_categorias + 3)->probabilidad[*cat4]);
                                    }
                                    else
                                        if (cuenta_palabras == 2) {
                                            factor=factores[traduce_cat_modelo_a_generica[*cat2]];
                                            probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                            probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat2] +
                                                                   (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                                   (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                                   (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                                   (punt_lista_categorias + 2)->probabilidad[*cat4]);
                                        }
                                        else
                                            if (cuenta_palabras == 3){
                                                factor=factores[traduce_cat_modelo_a_generica[*cat3]];
                                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat3] +
                                                                       (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                                       (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                                       (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                                                       (punt_lista_categorias + 1)->probabilidad[*cat4]);
                                            }
                                            else{
                                                factor=factores[traduce_cat_modelo_a_generica[*cat4]];
                                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat4] +
                                                                       (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                                                       (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                                                       (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                                                       (punt_lista_categorias - 3)->probabilidad[*cat1]);
                                             }
                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;

                                       switch (cuenta_palabras) {

                                       case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                       case 2:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                        break;
                                       case 3:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat3;
                                        break;
                                       case 4:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat4;
                                        break;
                                       default:
                                        break;

                                       } // switch

                                } // if ( (probabilidad_actual...

                        } // for (cat3 = lista_categorias[3].categorias...

                        categoria_escogida++;
                } // for (cuenta_palabras = 0...

            break;

        default:

            punt_lista_categorias = lista_categorias;

            for (cuenta_palabras = 0; cuenta_palabras < 2; cuenta_palabras++,
                                                           punt_lista_categorias++) {

                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...


                maximo = -1*FLT_MAX;
                penta_grama[0] = PUNT;

                for (cat0 = lista_categorias[0].categoria; *cat0 != 0; cat0++)
                    for (cat1 = lista_categorias[1].categoria; *cat1 != 0; cat1++)
                        for (cat2 = lista_categorias[2].categoria; *cat2 != 0; cat2++)
                            for (cat3 = lista_categorias[3].categoria; *cat3 != 0; cat3++) {
                                penta_grama[1] = *cat0;
                                penta_grama[2] = *cat1;
                                penta_grama[3] = *cat2;
                                penta_grama[4] = *cat3;

                                //probabilidad_actual = opciones.factor * modelo_lenguaje->calcula_prob_5_grama(penta_grama);

                                if (cuenta_palabras == 0){
                                    factor=factores[traduce_cat_modelo_a_generica[*cat0]];
                                    probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat0] +
                                                        (punt_lista_categorias + 1)->probabilidad[*cat1] +
                                                        (punt_lista_categorias + 2)->probabilidad[*cat2] +
                                                        (punt_lista_categorias + 3)->probabilidad[*cat3]);
                                }
                                else {
                                    factor=factores[traduce_cat_modelo_a_generica[*cat1]];
                                    probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat1] +
                                                        (punt_lista_categorias - 1)->probabilidad[*cat0] +
                                                        (punt_lista_categorias + 1)->probabilidad[*cat2] +
                                                        (punt_lista_categorias + 2)->probabilidad[*cat3]);
                                }

                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;

                                    switch (cuenta_palabras) {

                                    case 0:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat0;
                                        break;
                                    case 1:
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat1;
                                        break;
                                    default:
                                        break;
                                    } // switch
                                } // if (probabilidad_actual > maximo...
                            } // for (cat3 = lista_categorias[3].categorias...

                    categoria_escogida++;

            } // for (cuenta_palabras = 1;...

            // Ahora nos referimos a las palabras del medio (Las que no son las 3 primeras
            // ni las 3 últimas.

            for (cuenta_palabras = 2; cuenta_palabras < numero_elementos - 2; cuenta_palabras++,
                                                                    punt_lista_categorias++) {
                if (punt_lista_categorias->numero_categorias == 0) {
                    fprintf(stderr, "Error en aplica_modelo_lenguaje: Número de categorías igual a cero.\n");
                    return 1;
                } // if (punt_lista_categorias...

                if (punt_lista_categorias->numero_categorias == 1) {
                    categoria_escogida++->cat = (Categorias_modelo_enum) punt_lista_categorias->categoria[0];
                    continue;
                } // if (punt_lista_categorias...

                maximo = -1*FLT_MAX;

                for (cat0 = (punt_lista_categorias - 2)->categoria; *cat0 != 0; cat0++)
                    for (cat1 = (punt_lista_categorias - 1)->categoria; *cat1 != 0; cat1++)
                        for (cat2 = punt_lista_categorias->categoria; *cat2 != 0; cat2++)
                            for (cat3 = (punt_lista_categorias + 1)->categoria; *cat3 != 0; cat3++)
                                for (cat4 = (punt_lista_categorias + 2)->categoria; *cat4 != 0; cat4++) {
                                    penta_grama[0] = *cat0;
                                    penta_grama[1] = *cat1;
                                    penta_grama[2] = *cat2;
                                    penta_grama[3] = *cat3;
                                    penta_grama[4] = *cat4;

                                    factor=factores[traduce_cat_modelo_a_generica[*cat2]];
                                   //probabilidad_actual = opciones.factor * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                    probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat2] +
                                                          (punt_lista_categorias - 1)->probabilidad[*cat1] +
                                                          (punt_lista_categorias - 2)->probabilidad[*cat0] +
                                                          (punt_lista_categorias + 1)->probabilidad[*cat3] +
                                                          (punt_lista_categorias + 2)->probabilidad[*cat4]);

                                    if (probabilidad_actual > maximo) {
                                        maximo = probabilidad_actual;
                                        categoria_escogida->cat = (Categorias_modelo_enum) *cat2;
                                    } // if (probabilidad_actual...

                                } // for (cat4 = ...

                categoria_escogida++;

            } // for (cuenta_palabras = 3; ...

            // Ahora nos ocupamos de las 3 últimas:


            maximo = -1*FLT_MAX;
            maximo_2 = maximo;

            for (cat0 = (punt_lista_categorias - 3)->categoria; *cat0 != 0; cat0++)
                for (cat1 = (punt_lista_categorias - 2)->categoria; *cat1 != 0; cat1++)
                    for (cat2 = (punt_lista_categorias - 1)->categoria; *cat2 != 0; cat2++)
                        for (cat3 = punt_lista_categorias->categoria; *cat3 != 0; cat3++)
                            for (cat4 = (punt_lista_categorias + 1)->categoria; *cat4 != 0; cat4++) {
                                penta_grama[0] = *cat0;
                                penta_grama[1] = *cat1;
                                penta_grama[2] = *cat2;
                                penta_grama[3] = *cat3;
                                penta_grama[4] = *cat4;

                                //probabilidad_actual = opciones.factor * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                //probabilidad_actual_2 =  probabilidad_actual;

                                factor=factores[traduce_cat_modelo_a_generica[*cat3]];
                                probabilidad_actual = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                probabilidad_actual += factor * (punt_lista_categorias->probabilidad[*cat3] +
                                                       (punt_lista_categorias - 2)->probabilidad[*cat1] +
                                                       (punt_lista_categorias - 1)->probabilidad[*cat2] +
                                                       (punt_lista_categorias - 3)->probabilidad[*cat0] +
                                                       (punt_lista_categorias + 1)->probabilidad[*cat4]);
                                factor=factores[traduce_cat_modelo_a_generica[*cat4]];
                                probabilidad_actual_2 = (1-factor) * modelo_lenguaje->calcula_prob_5_grama(penta_grama);
                                probabilidad_actual_2 += factor * (punt_lista_categorias->probabilidad[*cat4] +
                                                         (punt_lista_categorias - 1)->probabilidad[*cat3] +
                                                         (punt_lista_categorias - 2)->probabilidad[*cat2] +
                                                         (punt_lista_categorias - 4)->probabilidad[*cat0] +
                                                         (punt_lista_categorias - 3)->probabilidad[*cat1]);


                                if (probabilidad_actual > maximo) {
                                    maximo = probabilidad_actual;
                                    categoria_escogida->cat = (Categorias_modelo_enum) *cat3;

                                } // if (probabilidad_actual...

                                if (probabilidad_actual_2 > maximo_2) {
                                    maximo_2 = probabilidad_actual_2;
                                    categoria_escogida_2 = (Categorias_modelo_enum) *cat4;

                                } // if (probabilidad_actual_2


                            } // for (cat4 = ...

                categoria_escogida++;
                categoria_escogida->cat = categoria_escogida_2;

    } // switch (numero_elementos)

    return 0;

}

/**
 * \author campillo
 * \remarks
 * \param
 */

int Analisis_morfosintactico::analisis_morfosintactico(t_frase_separada * frase,
		char * nombre_fichero_analisis, FILE * f_analisis){

	t_frase_separada * ptr_frase;
	t_infinitivo infor[TEMPOS_POSIBLES];
	char genero[20],numero[20],cate[40], tonic[16],*kk,linea[800];
	unsigned char lista_de_tempos_verdadeiros[TEMPOS_POSIBLES] = "";

	ptr_frase=frase;

	while (*ptr_frase->pal) {
#ifdef _SALTA_SOSTENIDOS_PAUSAS
		if (*ptr_frase->pal == '#'){
			ptr_frase++;
			continue;
		}
#endif
		if (NULL==fgets(linea,800,f_analisis)) {
			fprintf(stderr, "Error en carga_analisis_morfosintatico_de_fichero,\
					fallo lectura de %s.\n", nombre_fichero_analisis);

			return 1;
		}
		while (*linea=='-' && *(linea+1)=='\t' && *(linea+2)!='\t')
			if (NULL==fgets(linea,800,f_analisis)) {
				fprintf(stderr, "Error en carga_analisis_morfosintatico_de_fichero,\
						fallo lectura de %s.\n", nombre_fichero_analisis);
				return 1;
			}
		kk=strchr(linea,'\t');
		kk++;
		kk=strchr(kk,'\t');
		kk++;
		sscanf(kk, "%s\t%s\t%s\t%s", cate, genero, numero, tonic);
		asigna_categoria(traducirCategorias(cate), ptr_frase);
    /*
		 *ptr_frase->cat_gramatical[0]=traducirCategorias(cate);
		 *ptr_frase->cat_gramatical[1]=0;
		 *switch (ptr_frase->cat_gramatical[0]) {
		 *  case VERBO:
		 *  case PERIFRASE:
		 *    lista_de_tempos_verdadeiros[0] = 0;
		 *    memset(infor, 0, sizeof(infor));
		 *    if (ptr_frase->pal_transformada) {
		 *      diccionarios->analizar_verbos(ptr_frase->pal_transformada, lista_de_tempos_verdadeiros, infor, idioma);
		 *    }
		 *    else {
		 *      diccionarios->analizar_verbos(ptr_frase->pal, lista_de_tempos_verdadeiros, infor, idioma);
		 *    }
		 *    memcpy(ptr_frase->inf_verbal, infor, TEMPOS_POSIBLES * sizeof(t_infinitivo));
		 *    strncpy((char *) ptr_frase->cat_verbal, (char *) lista_de_tempos_verdadeiros, TEMPOS_POSIBLES);
		 *    if (ptr_frase->cat_gramatical[0]== PERIFRASE && *infor[0].infinitivo) {
		 *      ptr_frase->cat_gramatical_descartada[0]=VERBO;
		 *    }
		 *  default:
		 *    break;
		 *}
     */

		ptr_frase->xenero=traducirXeneroNumero(genero);
		ptr_frase->numero=traducirXeneroNumero(numero);
		if (tonic[0] == 'T')
			ptr_frase->tonicidad_forzada = 1;
		else
			ptr_frase->tonicidad_forzada = 2;

		ptr_frase++;
	}

	if ( (strcmp(cate, "PUNTO") != 0) && (strcmp(cate, "PUNTOS_SUSPENSIVOS") != 0) &&
			(strcmp(cate, "COMA") != 0) && (strcmp(cate, "PUNTO_E_COMA") != 0) &&
			(strcmp(cate, "DOUS_PUNTOS") != 0) &&
			(strcmp(cate, "DOBLES_COMINNAS") != 0) &&
			(strcmp(cate, "PECHE_INTERROGACION") != 0) && (strcmp(cate, "PECHE_EXCLAMACION") != 0) ) {
		if (fgets(linea,800,f_analisis) != NULL) {
			fprintf(stderr, "Error en carga_analisis_morfosintatico_de_fichero,\
					fichero desincronizado. (%s).\n", nombre_fichero_analisis);
			return 1;
		}
	}

	return 0;

}


/**
 * \author campillo
 * \remarks
 * \param
 */

int Analisis_morfosintactico::analisis_morfosintactico(t_frase_separada * frase, int n_elementos, char *directorio_salida,
							 char *frase_original, char idioma, char ptipo){

    char nombre_fichero_salida[FILENAME_MAX];
    FILE *fichero_errores;


		if (ptipo == 6) {
			saca_prob = 1;
		}
		else {
			saca_prob = 0;
		}

    if (frase_solo_signos_de_puntuacion(frase))
        return 0;

    if (decide_categorias(frase, n_elementos, idioma)) {
        sprintf(nombre_fichero_salida, "%sFrases_errores.txt", directorio_salida);
        if ( (fichero_errores = fopen(nombre_fichero_salida, "at")) == NULL) {
            puts("Error al intentar abrir el fichero de frases que provocan errores.");
            return 1;
        } // if ( (fichero_errores = fopen...

        fprintf(fichero_errores, "%s\n", frase_original);
        fclose(fichero_errores);
        return 0;
    } // if (decide_categorias...
    return 0;
}

int Analisis_morfosintactico::sacar_analisis_morfosintactico( char * frase_extraida,
                                    t_frase_separada * frase_separada,
                                    FILE * &flin,
                                    char * fsalida,
																		char tipo,
																		char separa_lin,
                                    char idioma){
 	char aux[FILENAME_MAX];
	t_frase_separada * elemento_de_frase = frase_separada;
  t_frase_separada ana_enclitico;
  *ana_enclitico.pal=0;
  *ana_enclitico.lema=0;
  *ana_enclitico.cat_gramatical=0;
  *ana_enclitico.cat_gramatical_descartada=0;  
  char aux_nome[10];
//  char cadena[80];
//  int n;

//Abrir fichero a la nueva manera?  
	if (flin==NULL) {
		strcpy(aux,fsalida);
		strcat(aux,".lin");

		if ((flin=fopen(aux,"w"))==NULL) {
			fprintf(stderr,"\nO arquivo de informacion linguistica \"%s\" non se pode crear",aux);
			return 1;
		}
	}

  CotoviaEagles *oCE=CotoviaEagles::getSingleton();



 	while (*elemento_de_frase->pal)  {


    	if (elemento_de_frase->cat_gramatical[0] == VERBO ||
            elemento_de_frase->cat_gramatical[0] == XERUNDIO ||
            elemento_de_frase->cat_gramatical[0] == PARTICIPIO ||
					( elemento_de_frase->cat_gramatical[0] == PERIFRASE &&
						*elemento_de_frase->inf_verbal[0].infinitivo)   ) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->inf_verbal[0].infinitivo);
			}

			if (! *elemento_de_frase->lema || elemento_de_frase->cat_gramatical[0] == NOME_PROPIO) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->pal);
			}


    fprintf(flin,"%s\t%s\t",elemento_de_frase->pal,elemento_de_frase->lema);
// Primero obtenemos las primeras dos letras de la etiqueta, en base a la categoría.
// Luego obtenemos género y número.
      strcpy(aux_nome,oCE->mGetC2E(elemento_de_frase->cat_gramatical[0]).c_str());
/*
    if (elemento_de_frase->cat_gramatical[0]==PERIFRASE) {
      for (n=0;n<N_CAT && elemento_de_frase->cat_gramatical_descartada[n]!=0;n++);
      if (n>0) strcat(aux_nome,oCE->mGetC2E(elemento_de_frase->cat_gramatical_descartada[n-1]).c_str());
    }
    */
    if (elemento_de_frase->cat_gramatical[0]!=VERBO
                        && elemento_de_frase->cat_gramatical[0]!=PERIFRASE
                        && elemento_de_frase->cat_gramatical[0]!=XERUNDIO
                        && elemento_de_frase->cat_gramatical[0]!=INFINITIVO){
      strcat(aux_nome,oCE->mGetG2E(elemento_de_frase->xenero).c_str());
      strcat(aux_nome,oCE->mGetN2E(elemento_de_frase->numero).c_str());
    }
    fprintf(flin,"%s",aux_nome);    //categoría.


// 			if ((elemento_de_frase->cat_gramatical[0]==VERBO || elemento_de_frase->cat_gramatical[0]==PERIFRASE))
  			if (elemento_de_frase->cat_gramatical[0]==VERBO) 
				{
			  	if (elemento_de_frase->cat_verbal[0]){
				  	  string yip=oCE->mGetTVEagles(elemento_de_frase->cat_verbal[0]);
              fprintf(flin,"%s",yip.c_str());
//					putc(' ',flin);
//					fputs(elemento_de_frase->inf_verbal[0].infinitivo,flin);
//					putc('\t',flin);
				  }
        }

			if (elemento_de_frase->inf_verbal[0].enclitico.enclitico[0] &&
					(elemento_de_frase->cat_gramatical[0]==VERBO ||
					 elemento_de_frase->cat_gramatical[0]==PERIFRASE)) {


				int pos;


//				fputs("-\t",flin);
				fputs("\n-",flin);
				//putc('\t',flin);
				fputs(elemento_de_frase->inf_verbal[0].enclitico.enclitico,flin);
				strcpy(ana_enclitico.pal,elemento_de_frase->inf_verbal[0].enclitico.enclitico);
				pos=diccionarios->busca_dic_nomes(ana_enclitico.pal,&ana_enclitico,idioma);
				if (pos==-1)
					fputs("\t>>>ENCLITICO_NO_CLASIFICADO\tNO_ASIGNADO!!!!",flin);
				else {
//          putc('\n',flin);
//          fputs("\n-\t",flin);
					for(int kk=0;kk<15;kk++){
						if (ana_enclitico.cat_gramatical[kk]>=PRON_PERS_TON &&
								ana_enclitico.cat_gramatical[kk]<=CONTR_PRON_PERS_AT){
							//if (kk) putc(' ',flin);
							//escribe_categoria_gramatical(ana_enclitico.cat_gramatical[kk],flin);
              strcpy(aux_nome,oCE->mGetC2E(ana_enclitico.cat_gramatical[kk]).c_str());
              strcat(aux_nome,oCE->mGetG2E(ana_enclitico.xenero).c_str());
              strcat(aux_nome,oCE->mGetN2E(ana_enclitico.numero).c_str());
              fprintf(flin,"\t%s\t%s",ana_enclitico.lema,aux_nome);
							break;
						}
					}
				}
/*				putc('\t',flin);
				fputs(escribe_gen(ana_enclitico.xenero,cadena),flin);
				putc('\t',flin);
				fputs(escribe_num(ana_enclitico.numero,cadena),flin);
				fputs("\t\t\n",flin);
*/
			}

//			if (elemento_de_frase->inf_verbal[0].artigo[0]&& elemento_de_frase->cat_gramatical[0]==VERBO)
			if (elemento_de_frase->inf_verbal[0].artigo[0]&&
					(elemento_de_frase->cat_gramatical[0]==VERBO ||
					 elemento_de_frase->cat_gramatical[0]==PERIFRASE)) 
			{

				fprintf(flin,"\n-%s\to\tAD",elemento_de_frase->inf_verbal[0].artigo);
				if (elemento_de_frase->inf_verbal[0].artigo[0]=='a')
					fputs("F",flin);
				else
					fputs("M",flin);
				if (elemento_de_frase->inf_verbal[0].artigo[1])
					fputs("P",flin);
				else
					fputs("S",flin);
			}

// Se acabó la línea. ;)
/**
    if (elemento_de_frase->cat_gramatical[0]==VERBO){
      fprintf(flin, "\tCANDIDATO");
    }

   */

    fprintf(flin,"\n");

    elemento_de_frase++;
  }
  
  if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
	}

  return 0;
}


int Analisis_morfosintactico::sacar_analisis_morfosintactico( char * frase_extraida,
                                    t_frase_separada * frase_separada,
                                    char * frase_procesada,
                                    char * frase_sil_e_acentuada,
                                    char * frase_fonetica,
                                    t_frase_sintagmada * frase_sintagmada,
                                    t_frase_en_grupos * frase_en_grupos,
                                    t_prosodia * frase_prosodica,
                                    FILE * &flin,
                                    char * fsalida,
																		char tipo,
																		char separa_lin,
																		char idioma){
	char aux[FILENAME_MAX];
	int cont;
	//int cont=1;

	if (tipo==5)
		return  sacar_analisis_morfosintactico(frase_extraida,
				frase_separada,
				flin,
				fsalida,
				tipo,
				separa_lin,
				idioma);


	t_frase_separada * elemento_de_frase = frase_separada;

	if (flin==NULL) {
		strcpy(aux,fsalida);
		if ( (tipo == 4) || (tipo == 8) )
			strcat(aux,".sin");
		else
			strcat(aux,".lin");
		if ((flin=fopen(aux,"w"))==NULL) {
			fprintf(stderr,"\nO arquivo de informacion linguistica \"%s\" non se pode crear",aux);
			return 1;
		}
	}


	if (tipo == 1){
		int i,f,cont_aux,kk,nivel;

		if (elemento_de_frase->pal[0]==0) {
			return 0;
		}
		fputs("###############################################################################",flin);
		fprintf(flin,"\n\nFRASE EXTRAÍDA:  %s",frase_extraida);
		//fprintf(flin,"\nENTORNO DE FRASE:   %s\n", tipo_frase.continuacion_texto);
		fputs("\n          ANÁLISE MORFOLÓXICA E FORMAL\n",flin);
		fputs("          ============================\n\n",flin);

		while (*elemento_de_frase->pal)  {
			fprintf(flin,"\n%-16s  TIPO DE PALABRA= ",elemento_de_frase->pal);
			escribe_tipo_de_pal(elemento_de_frase->clase_pal,flin);
			if(elemento_de_frase->pal_transformada!=NULL)
				fprintf(flin,"\n                  EXTENSIÓN = %-48s",elemento_de_frase->pal_transformada);
			fputs("\n                  CAT. GRAMATICAL REAL=  ",flin);
			escribe_categoria_gramatical(elemento_de_frase->cat_gramatical[0],flin);
			if (elemento_de_frase->cat_gramatical[1]) {
				fputs("\n                  OUTRAS CATEGORIAS POR HOMOGRAFIA:\n",flin);
				fputs("                  =================================",flin);
				for(kk=1;kk<N_CAT;kk++){
					if (elemento_de_frase->cat_gramatical[kk]){
						fprintf(flin,"\n                      ");
						escribe_categoria_gramatical(elemento_de_frase->cat_gramatical[kk],flin);
					}
				}
			}
			if (elemento_de_frase->cat_verbal[0]) {
				fputs("\n                  ANALISE DA PALABRA COMO VERBO",flin);
				fputs("\n                  PERSOA,NUMERO,TEMPO,MODO E CONXUGACIÓN",flin);
				fputs("\n                  --------------------------------------",flin);
				fputs("\n                 ",flin);
				escribe_tempo_verbal(elemento_de_frase->cat_verbal[0],flin);
				if (elemento_de_frase->cat_verbal[1]) {
					fputs("\n                 ",flin);
					escribe_tempo_verbal(elemento_de_frase->cat_verbal[1],flin);
				}
				if (elemento_de_frase->cat_verbal[2]) {
					fputs("\n                 ",flin);
					escribe_tempo_verbal(elemento_de_frase->cat_verbal[2],flin);
				}
				if (elemento_de_frase->cat_verbal[3]) {
					fputs("\n                 ",flin);
					escribe_tempo_verbal(elemento_de_frase->cat_verbal[3],flin);
				}
				if (elemento_de_frase->cat_verbal[4]) {
					fputs("\n                 ",flin);
					escribe_tempo_verbal(elemento_de_frase->cat_verbal[4],flin);
				}
			}
			putc('\n',flin);
			//cont++;
			elemento_de_frase++;
		}
		cont=0;
		fputs("\n           FRASE SINTAGMADA",flin);
		fputs("\n           ================\n",flin);
		//cont_aux=0;
		while(frase_sintagmada[cont].tipo_sintagma) {
			i=frase_sintagmada[cont].inicio;
			f=frase_sintagmada[cont].fin;
			putc('\n',flin);
			escribe_tipo_de_sintagma(frase_sintagmada[cont].tipo_sintagma,flin);
			fprintf(flin," (%d sílabas) =>   ",frase_sintagmada[cont].n_silabas);
			for (cont_aux=i;cont_aux<=f;cont_aux++) {
				if (frase_separada[cont_aux].pal_transformada!=NULL)
					fprintf(flin," %s",frase_separada[cont_aux].pal_transformada);
				else
					fprintf(flin," %s",frase_separada[cont_aux].pal);
			}
			cont++;
		}
		cont=0;
		//cont_aux=0;
		fputs("\n\n         ANALISE DE PAUSAS E ENTOACION\n",flin);
		fputs("         ==============================\n\n",flin);
		while(frase_en_grupos[cont].inicio!=0 && frase_en_grupos[cont].fin!=0 ||
				!(frase_en_grupos[cont].inicio!=0 && frase_en_grupos[cont].fin!=0) && cont==0 )
		{
			fprintf(flin,"\n\nGRUPO %d",cont);
			//fmendez i=frase_en_grupos[cont].inicio;
			//fmendez f=frase_en_grupos[cont].fin;
			fputs("\nTIPO_DE_PROPOSICIÓN  = ",flin);
			escribe_tipo_de_proposicion(frase_en_grupos[cont].tipo_proposicion,flin);
			fputs("\nTIPO_DE_TONEMA       = ",flin);
			escribe_tipo_de_tonema(frase_en_grupos[cont].tipo_tonema,flin);
			fputs("\nTIPO_INICIO_DE_GRUPO = ",flin);
			escribe_tipo_de_sintagma(frase_en_grupos[cont].tipo_inicio[0],flin);
			if(frase_en_grupos[cont].tipo_inicio[1]) {
				fputs("\n                     = ",flin);
				escribe_tipo_de_sintagma(frase_en_grupos[cont].tipo_inicio[1],flin);
			}
			fputs("\nTIPO_FIN_DE_GRUPO    = ",flin);
			escribe_tipo_de_sintagma(frase_en_grupos[cont].tipo_fin[0],flin);
			if(frase_en_grupos[cont].tipo_fin[1]) {
				fputs("\n                     = ",flin);
				escribe_tipo_de_sintagma(frase_en_grupos[cont].tipo_inicio[1],flin);
			}
			if (frase_en_grupos[cont].enlazado)
				fputs("\n### ENLAZADO CO SEGUINTE GRUPO ###",flin);
			fputs("\n\nTROZO_DE_FRASE:\n",flin);
			i=frase_en_grupos[cont].inicio;
			f=frase_en_grupos[cont].fin;
			i=frase_sintagmada[i].inicio;
			f=frase_sintagmada[f].fin;
			for (cont_aux=i;cont_aux<=f;cont_aux++) {
				if (cont_aux!=i) putc(' ',flin);
				if (frase_separada[cont_aux].pal_transformada!=NULL)
					fputs(frase_separada[cont_aux].pal_transformada,flin);
				else
					fputs(frase_separada[cont_aux].pal,flin);
			}
			cont++;
		}
		fputs("\n\n                     RESULTADOS FINAIS",flin);
		fputs("\n                     =================\n",flin);
		fputs("\nFRASE NORMALIZADA\n\n",flin);
		fputs(frase_procesada,flin);
		fputs("\n\nFRASE SILABEADA E ACENTUADA\n\n",flin);
		fputs(frase_sil_e_acentuada,flin);
		fputs("\n\nFRASE FONÉTICA\n\n",flin);
		fputs(frase_fonetica,flin);
		putc('\n',flin);
		fputs("\n      EVOLUCIÓN DO PITCH\n",flin);
		fputs("      ==================\n\n",flin);
		fputs("\n FRE\n",flin);
		fputs("\n+160 | ",flin);
		i=0;
		while (frase_prosodica[i].alof[0]) {
			if (frase_prosodica[i].pitch1>160 )
				putc('^',flin);
			else
				putc(' ',flin);
			i++;
		}
		for (nivel=150;nivel>=0;nivel=nivel-10) {
			i=0;
			fprintf(flin,"\n %3d | ",nivel);

			while (frase_prosodica[i].alof[0]) {
				if (frase_prosodica[i].pitch1>=nivel && frase_prosodica[i].pitch1<(nivel+10))
				{
					if (frase_prosodica[i].pitch1>nivel+5)
						putc('-',flin);
					else
						putc('_',flin);
				}
				else {
					if (frase_prosodica[i].pitch1<nivel)
						putc(' ',flin);
					if (frase_prosodica[i].pitch1>=nivel+10)
						putc('H',flin);
				}
				i++;
			}
		}
		i=0;
		fprintf(flin,"\n_______");
		while (frase_prosodica[i].alof[0]) {
			fprintf(flin,"_");
			i++;
		}

		fputs("\n FON | ",flin);
		i=0;
		while (frase_prosodica[i].alof[0]) {
			if (strcmp(frase_prosodica[i].alof,"tS")==0)
				putc('C',flin);
			else
				if (strcmp(frase_prosodica[i].alof,"rr")==0)
					putc('R',flin);
				else
					fputs(frase_prosodica[i].alof,flin);
			i++;
		}
		putc('\n',flin);
		putc('\n',flin);
		putc('\n',flin);
	}
	else if (tipo == 2){  //fmendez para sacar unicamente las categorias gramaticales (lin 1)
		int kk;
		char cadena[80];
		t_frase_separada ana_enclitico;

		while (*elemento_de_frase->pal)  {

			fputs(elemento_de_frase->pal,flin);
			putc('\t',flin);

			if ((elemento_de_frase->inf_verbal[0].enclitico.enclitico[0] ||
						elemento_de_frase->inf_verbal[0].artigo[0]) &&
					esta_palabra_ten_a_categoria_de(VERBO,elemento_de_frase))
				fputs(elemento_de_frase->inf_verbal[0].reconstruccion,flin);

			putc('\t',flin);
			for(kk=0;kk<N_CAT;kk++){
				if (elemento_de_frase->cat_gramatical[kk]){
					if (kk) putc(' ',flin);
					escribe_categoria_gramatical(elemento_de_frase->cat_gramatical[kk],flin);

				}
			}
			putc('\t',flin);
			//fprintf(flin, "%f\t", elemento_de_frase->prob);
			fputs(escribe_gen(elemento_de_frase->xenero,cadena),flin);
			putc('\t',flin);
			fputs(escribe_num(elemento_de_frase->numero,cadena),flin);
			putc('\t',flin);
#ifdef _SACA_CLASES_AMBIGUEDAD
			for(kk=0;elemento_de_frase->clase_ambiguedad[kk];kk++){
#ifdef _MODELO_POS_COMPLETO
				escribe_categoria_gramatical(elemento_de_frase->clase_ambiguedad[kk],flin);
#else
				fputs(categorias_genericas[elemento_de_frase->clase_ambiguedad[kk]],flin);
#endif
				putc(' ',flin);
			}
#else
			for(kk=0;elemento_de_frase->cat_gramatical_descartada[kk];kk++){
				escribe_categoria_gramatical(elemento_de_frase->cat_gramatical_descartada[kk],flin);
				putc(' ',flin);
			}
#endif

			putc('\t',flin);


			if (elemento_de_frase->cat_gramatical[0] == VERBO ||
					( elemento_de_frase->cat_gramatical[0] == PERIFRASE &&
						*elemento_de_frase->inf_verbal[0].infinitivo)   ) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->inf_verbal[0].infinitivo);
			}

			if (! *elemento_de_frase->lema || elemento_de_frase->cat_gramatical[0] == NOME_PROPIO) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->pal);
			}


			fputs(elemento_de_frase->lema,flin);


			putc('\t',flin);

			for(kk=0;kk<N_CAT;kk++){
				if (elemento_de_frase->cat_verbal[kk]){
					escribe_tempo_verbal(elemento_de_frase->cat_verbal[kk],flin);
					putc(' ',flin);
					fputs(elemento_de_frase->inf_verbal[kk].infinitivo,flin);
					putc('\t',flin);
				}
			}
			putc('\n',flin);

			if (elemento_de_frase->inf_verbal[0].enclitico.enclitico[0] &&
					(elemento_de_frase->cat_gramatical[0]==VERBO ||
					 elemento_de_frase->cat_gramatical[0]==PERIFRASE)) {
				int pos;
				fputs("-\t",flin);
				//putc('\t',flin);
				fputs(elemento_de_frase->inf_verbal[0].enclitico.enclitico,flin);
				strcpy(ana_enclitico.pal,elemento_de_frase->inf_verbal[0].enclitico.enclitico);
				pos=diccionarios->busca_dic_nomes(ana_enclitico.pal,&ana_enclitico,idioma);
				if (pos==-1)
					fputs("\tENCLITICO_NO_CLASIFICADO\tNO_ASIGNADO\tNO_ASIGNADO",flin);
				else {
					putc('\t',flin);
					for(kk=0;ana_enclitico.cat_gramatical[kk];kk++){
						if (ana_enclitico.cat_gramatical[kk]>=PRON_PERS_TON &&
								ana_enclitico.cat_gramatical[kk]<=CONTR_PRON_PERS_AT){
							//if (kk) putc(' ',flin);
							escribe_categoria_gramatical(ana_enclitico.cat_gramatical[kk],flin);
							break;
						}
					}
				}
				putc('\t',flin);
				fputs(escribe_gen(ana_enclitico.xenero,cadena),flin);
				putc('\t',flin);
				fputs(escribe_num(ana_enclitico.numero,cadena),flin);
				fputs("\t\t\n",flin);
			}
			if (elemento_de_frase->inf_verbal[0].artigo[0]&&
					(elemento_de_frase->cat_gramatical[0]==VERBO ||
					 elemento_de_frase->cat_gramatical[0]==PERIFRASE)) {
				fprintf(flin,"-\t%s\tART_DET\t",elemento_de_frase->inf_verbal[0].artigo);
				if (elemento_de_frase->inf_verbal[0].artigo[0]=='a')
					fputs("FEMININO\t",flin);
				else
					fputs("MASCULINO\t",flin);
				if (elemento_de_frase->inf_verbal[0].artigo[1])
					fputs("PLURAL\t\n",flin);
				else
					fputs("SINGULAR\t\n",flin);
			}



			//cont++;
			elemento_de_frase++;
		}
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	}
	else if (tipo == 3){  //fmendez sacar categorias gramaticales + tonicidad (lin 2)
		char cadena[80];
		while (*elemento_de_frase->pal)  {
			fprintf(flin, "%s\t\t", elemento_de_frase->pal);
			for(int kk = 0; kk < N_CAT; kk++){
				if (elemento_de_frase->cat_gramatical[kk]){
					if (kk){ 
						putc(' ', flin);
					}
					escribe_categoria_gramatical(elemento_de_frase->cat_gramatical[kk], flin);
				}
			}
			putc('\t',flin);
			fputs(escribe_gen(elemento_de_frase->xenero,cadena),flin);
			putc('\t',flin);
			fputs(escribe_num(elemento_de_frase->numero,cadena),flin);

			if (elemento_de_frase->tr_fon && strchr(elemento_de_frase->tr_fon,'^')){
				fputs("\tT\t", flin);
			}
			else if (elemento_de_frase->pal_sil_e_acentuada && strchr(elemento_de_frase->pal_sil_e_acentuada,'^')){
				fputs("\tT\t", flin);
			}
			else {
				fputs("\tA\t", flin);
			}
			putc('\n', flin);
			elemento_de_frase++;
		}
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	}
	else if (tipo == 4) {
		int i, f;
		cont=0;
		while(frase_sintagmada[cont].tipo_sintagma) {
			i=frase_sintagmada[cont].inicio;
			f=frase_sintagmada[cont].fin;
			//			putc('\n',flin);
			escribe_tipo_de_sintagma(frase_sintagmada[cont].tipo_sintagma,flin);
			fprintf(flin, "\t");
			for (int cont_aux=i;cont_aux<=f;cont_aux++) {
				if (frase_separada[cont_aux].pal_transformada!=NULL)
					fprintf(flin,"%s ",frase_separada[cont_aux].pal_transformada);
				else
					fprintf(flin,"%s ",frase_separada[cont_aux].pal);
			}
			fprintf(flin, "\t%d\n", frase_sintagmada[cont].n_silabas);
			cont++;
		}
		//		cont=0;
		//		while(frase_sintagmada[cont].tipo_sintagma) {
		//			escribe_tipo_de_sintagma(frase_sintagmada[cont].tipo_sintagma,flin);
		//			putc('\n', flin);
		//			cont++;
		//		}
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	} // else if (tipo == 4)
	else if (tipo == 8) { // campillo: análisis sintáctico con pausas del modelo de pausado
		int cont_aux, i, f;
		bool tonica;
		cont = 0;
		while(frase_sintagmada[cont].tipo_sintagma) {
			i=frase_sintagmada[cont].inicio;
			f=frase_sintagmada[cont].fin;
			//			putc('\n',flin);
			escribe_tipo_de_sintagma(frase_sintagmada[cont].tipo_sintagma,flin);
			fprintf(flin, "\t");
			tonica = false;
			for (cont_aux=i;cont_aux<=f;cont_aux++) {
				if (frase_separada[cont_aux].tonicidad_forzada == 1)
					tonica = true;
				if (frase_separada[cont_aux].pal_transformada!=NULL)
					fprintf(flin,"%s ",frase_separada[cont_aux].pal_transformada);
				else
					fprintf(flin,"%s ",frase_separada[cont_aux].pal);
			}

			if (tonica == true)
				fprintf(flin, "%d\tT\n", frase_sintagmada[cont].n_silabas);
			else
				fprintf(flin, "%d\tA\n", frase_sintagmada[cont].n_silabas);
			if (frase_separada[cont_aux - 1].pausa == DUR_PAUSA_TEXTO)
				fprintf(flin, "PAUSA\t\n");
			cont++;
		}
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	}
	else if (tipo == 6){  //fmendez sacar categorias gramaticales + puntuación (cardenal)
		char c1[20], c2[20];
		float acc = 0.0f;

		while (*elemento_de_frase->pal)  {
			fprintf(flin, "%s\t", elemento_de_frase->pal);
			escribe_categoria_gramatical(elemento_de_frase->cat_gramatical[0], flin);
			fprintf(flin, "\t%s\t%s\t%f\n", escribe_gen(elemento_de_frase->xenero,c1), escribe_num(elemento_de_frase->numero,c2), elemento_de_frase->prob); 
			acc += elemento_de_frase->prob;
			elemento_de_frase++;
		}
		fprintf(flin, "PUNTUACIÓN_ACUMULADA:\t%e\n", acc); 
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	}

	else if (tipo == 7){  //fmendez sacar palabra+lema+raiz+afijo (bea)
		while (*elemento_de_frase->pal)  {
			if (elemento_de_frase->cat_gramatical[0] == VERBO ||
					( elemento_de_frase->cat_gramatical[0] == PERIFRASE &&
						*elemento_de_frase->inf_verbal[0].infinitivo)   ) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->inf_verbal[0].infinitivo);
			}

			if (! *elemento_de_frase->lema || elemento_de_frase->cat_gramatical[0] == NOME_PROPIO) {
				strcpy(elemento_de_frase->lema, elemento_de_frase->pal);
			}
			fprintf(flin, "%s\t%s\t", elemento_de_frase->pal, elemento_de_frase->lema);
			if (elemento_de_frase->cat_gramatical[0]==VERBO ||
					elemento_de_frase->cat_gramatical[0]==PERIFRASE) {
				fprintf(flin, "%s\t%s", elemento_de_frase->inf_verbal->raiz, elemento_de_frase->inf_verbal->desinencia);
				if (*elemento_de_frase->inf_verbal[0].enclitico.enclitico){
					fprintf(flin, "\t%s",elemento_de_frase->inf_verbal[0].enclitico.enclitico);
				}
				if (*elemento_de_frase->inf_verbal->artigo){
					fprintf(flin, "\t%s",elemento_de_frase->inf_verbal->artigo);
				}
				//fputs(elemento_de_frase->inf_verbal[0].enclitico.enclitico,flin);

			}
			else {
				fprintf(flin, "%s\t%s", elemento_de_frase->raiz, elemento_de_frase->sufijo);
			}
			putc('\n', flin);
			elemento_de_frase++;
		}
		if (separa_lin){
			fputs("################################\t\t\t\t\t\t\t\n",flin);
		}
	}
	else if (tipo == 9) { //salida para el traductor
		return sacar_verbos_y_contracciones_expandidas(frase_separada,flin,fsalida,separa_lin,idioma);
	}

	return 0;
}

int Analisis_morfosintactico::sacar_verbos_y_contracciones_expandidas(
		t_frase_separada * it, FILE * &flin, char * fsalida, char separa_lin, char idioma){

	char aux[FILENAME_MAX];
	int kk;
	bool encliticos, artigo;
	if (flin==NULL) {
		strcpy(aux,fsalida);
		strcat(aux,".lin");
		if ((flin=fopen(aux,"w"))==NULL) {
			fprintf(stderr,"\nO arquivo de informacion linguistica \"%s\" non se pode crear",aux);
			return 1;
		}
	}

	while (*it->pal)  {
		if (esta_palabra_ten_a_categoria_de(VERBO,it)||esta_palabra_ten_a_categoria_de(PERIFRASE,it) ) {
			kk=0;
			encliticos=false;
			artigo=false;
			while (*it->inf_verbal[kk].infinitivo){
				if (*it->inf_verbal[kk].enclitico.enclitico) {
					encliticos=true;
				}
				if (*it->inf_verbal[kk].artigo) {
					artigo=true;
				}
				kk++;
			}
			if (encliticos || artigo) {
				fprintf(flin,"<%s",it->pal);
				kk=0;
				while (*it->inf_verbal[kk].infinitivo){
					if (*it->inf_verbal[kk].enclitico.enclitico || *it->inf_verbal[kk].artigo){
						fprintf(flin,",%s",it->inf_verbal[kk].reconstruccion);
						if (*it->inf_verbal[kk].enclitico.enclitico) {
							fprintf(flin,"+%s",it->inf_verbal[kk].enclitico.enclitico);
						}
						if (*it->inf_verbal[kk].artigo) {
							fprintf(flin,"+%s",it->inf_verbal[kk].artigo);
						} 
					}
					kk++;
				}
				fprintf(flin,"/> ");
			}
			else {
				fprintf(flin,"%s ", it->pal);
			}
		}
		else if (es_contraccion(it->cat_gramatical[0])) {
			string sp;
			if (it->pal_transformada) {
				sp=string(it->pal_transformada);
			}
			else {
				sp=string(it->pal);
			}
			fprintf(flin,"<%s,%s/> ", it->pal, contr[sp]);
		}
		else {	
			fprintf(flin,"%s ", it->pal);
		}
		it++;
	}
	fprintf(flin,"\n");
	if (separa_lin){
		fputs("################################\t\t\t\t\t\t\t\n",flin);
	}
	return 0;
}


/** 
 * \author fmendez
 * \remarks Carga el modelo de lenguaje en memoria.
 * 
 * \param ruta_bd ruta del modelo de lenguaje
 * 
 * \return 0 en ausencia de error
 */
int Analisis_morfosintactico::carga(char * ruta_bd, char * lenguaje){

	Modelo_lenguaje * len;

	if (lenguajes_cargados) {
		lenguajes_cargados = (char **) realloc(lenguajes_cargados, (numero_lenguajes_cargados + 1) * sizeof(char *));
	}
	else {
		lenguajes_cargados = (char **) malloc(sizeof(char *));
	}
	//lenguajes_cargados[numero_lenguajes_cargados++] = lenguaje;


	if (NULL != (len = lenguaje_cargado(lenguaje))) {
		len->numero_usuarios++;
		lenguajes_cargados[numero_lenguajes_cargados++] = len->nombre;
	}
	else {
		if (modelos_lenguaje) {
			modelos_lenguaje = (Modelo_lenguaje **) realloc(modelos_lenguaje, (numero_modelos_lenguaje + 1) * sizeof(Modelo_lenguaje *));
		}
		else {
			modelos_lenguaje = (Modelo_lenguaje **) malloc(sizeof(Modelo_lenguaje *));
		}
		modelos_lenguaje[numero_modelos_lenguaje] = new Modelo_lenguaje(ruta_bd, lenguaje);
		lenguajes_cargados[numero_lenguajes_cargados++] = modelos_lenguaje[numero_modelos_lenguaje]->nombre;
		numero_modelos_lenguaje++;
	}
	return 0;
}

/** 
 * \author fmendez
 * \remarks Descarga el modelo de lenguaje de memoria. 
 */
void Analisis_morfosintactico::descarga(){
	Modelo_lenguaje *it;
	for(int i = 0; i < numero_lenguajes_cargados; i++){
		it = lenguaje_cargado(lenguajes_cargados[i]);
		if (it->numero_usuarios > 1) {
			it->numero_usuarios--;
		}
		else {
			Modelo_lenguaje ** mkk = modelos_lenguaje;
			while	(*mkk != it){
				mkk++;
			}
			delete it;
			while (mkk < modelos_lenguaje + numero_modelos_lenguaje - 1){
				*mkk++ = *(mkk + 1);
			}
			numero_modelos_lenguaje--;
			if (numero_modelos_lenguaje){
				modelos_lenguaje = (Modelo_lenguaje **) realloc(modelos_lenguaje, numero_modelos_lenguaje * sizeof(Modelo_lenguaje *));
			}
			else {
				free(modelos_lenguaje);
				modelos_lenguaje = NULL;
			}
		}
	}
	free(lenguajes_cargados);
	CotoviaEagles::disposeSingleton();
}


/**
 * \remarks Comprueba si los modelos de lenguaje para un determinado
 * idioma están cargados en memoria.
 *
 * \author fmendez 
 *
 * \param l lenguaje a comprobar
 *
 * \return devuelve un puntero al modelo de lenguaje si está en memoria;
 * NULL en caso contrario
 */
Modelo_lenguaje *Analisis_morfosintactico::lenguaje_cargado(char * l) {
	
	for(int i = 0; i < numero_modelos_lenguaje; i++){
		if (!strcmp(modelos_lenguaje[i]->nombre, l)) {
			return modelos_lenguaje[i];
		}
	}
	return NULL;
}


/**
 * \brief Selecciona un lenguaje entre los modelos cargados en memoria
 * \author fmendez 
 *
 * \param lenguaje Lengua a seleccionar
 *
 * \return 
 */
void Analisis_morfosintactico::selecciona_lenguaje(char * lenguaje){

	for (int i = 0; i < numero_lenguajes_cargados; i++){
		if (!strcmp(lenguajes_cargados[i], lenguaje)) {
			modelo_lenguaje = lenguaje_cargado(lenguaje);
			return;
		}
	}
	fprintf(stderr, "Lenguaje \"%s\" no está cargado...usando \"%s\"\n", lenguaje, modelos_lenguaje[0]->nombre);
	modelo_lenguaje = modelos_lenguaje[0];

}

