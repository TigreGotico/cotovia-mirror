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
 
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#endif

#include "modos.hpp"
#include "tip_var.hpp"
#include "errores.hpp"
#include "leer_frase.hpp"
#include "sep_pal.hpp"
#include "clas_pal.hpp"
#include "preproc.hpp"
#include "sil_acen.hpp"
#include "transcripcion.hpp"
#include "alofonos.hpp"
#include "crea_descriptores.hpp"
#include "estadistica.hpp"
#include "frecuencia.hpp"
#include "audio.hpp"
#include "procesado_senhal.hpp"
#include "modelo_duracion.hpp"
#include "morfolo.hpp"
#include "categorias_analisis_morfosintactico.hpp"
#include "perfhash.hpp"
#include "modelo_lenguaje.hpp"
#include "matriz.hpp"
#include "path_list.hpp"
#include "Viterbi_categorias.hpp"
#include "analisis_morfosintactico.hpp"
//#include "sintesis_difonemas.hpp"
#include "sintagma.hpp"
#include "pausas.hpp"
#include "trat_fon.hpp"
#include "prosodia.hpp"
#include "info_corpus_voz.hpp"
#include "matriz.hpp"
#include "cache.hpp"
#include "info_corpus_voz.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "locutor.hpp"
#include "Viterbi_acentual.hpp"
#include "Viterbi.hpp"
#include "info_estructuras.hpp"
#include "seleccion_unidades.hpp"
#include "gbm_locuciones.hpp"
//#include "difonemas.hpp"
#include "configuracion.hpp"
#include "rupturas_entonativas.hpp"
#include "cotovia.hpp"
#include "xmlfile.hpp"
#include "lc-star.hpp"
#include "ecessinterfaces.hpp"
#include "EcessTP.hpp"

/** 
 * \author fmendez
 * \brief 
 * 
 * \param entrada 
 * \param opc 
 * 
 * \return 
 */
int EcessTP::ecesstp(char * entrada, t_opciones opc){

	opciones=opc;
	opciones.separa_token = 1; //ojo, 0 para mocosoft, m1 sin separa_token
	f_analisis = NULL;
	bool ultima_frase = false;

	if (0 != Inicio(entrada)) {
		return -1;
	}
	opciones.audio = 1;
	do {
		Reset();
		if (leer_frase.leer_frase(entrada, frase, ultima_frase)){
			return -1;
		}
		if (*frase == 0) {
			break;
		} 
		if (procesado_linguistico()){
			fprintf(stderr, "Error en el procesado lingüístico\n");
			return 1;
		}

#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
		if (opciones.fich_morfosintactico[0] == '\0') {
			sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
			crea_frase_pausas(f_separada, f_sintagmada);
		}
		else {
			sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
			poner_pausas(f_sintagmada, f_separada);
		}
#endif

#ifdef _SOLO_PAUSAS_ORTOGRAFICAS
		sintagma.analise_sintagmatico(f_separada,f_sintagmada,f_en_grupos,opciones.idioma);
		poner_pausas(f_sintagmada,f_separada);
#endif

		trat_fon.atono_ou_tonico_aberto_ou_pechado_e_w_x(f_separada, opciones.idioma);
		construye_frase_fonetica();

#ifndef _CORPUS_PROSODICO
		if ( (opciones.lin==3 || opciones.lin==4 || opciones.lin == 8) && opciones.tra <4 && !(opciones.audio || opciones.wav )){
			analisis_morfosintactico.sacar_analisis_morfosintactico(frase, f_separada,
					f_procesada, f_silabas, f_fonetica, f_sintagmada, f_en_grupos,
					f_prosodica, flin, opciones.fsalida, opciones.lin,opciones.separa_lin,opciones.idioma);
			continue;
		}
#endif
		if (opciones.tra==4){ //tra
			transcripcion.vuelca_transcripcion(f_fonetica,f_separada);
			if (!(opciones.audio || opciones.wav || opciones.lin))
				continue;
		}

		//saca_tipo_prop(f_fonetica);
		//transcripcion.divide_frase_fonetica(f_fonetica, f_separada); //ojo, lo comento para mocosoft, m1 sin separa_token
		transcripcion.divide_frase_fonetica(f_fonetica, f_separada); 
		output->escribe_salida_frase(1, cadena_unidades, 0);
	} while(!ultima_frase);

	opciones.audio = 0;
	Fin();
	return 0;
}


/** 
 * \author fmendez
 * \brief 
 * 
 * \param entrada 
 * 
 * \return 
 */
int EcessTP::Inicio(char * entrada){

	output = new EcessInterfaces(&numero_de_tokens, f_tokenizada);
	if (opciones.fstdout) {
		output->Inicio(NULL);
	}
	else {
		 output->Inicio(opciones.fsalida);
	}

	Cotovia::Inicio(entrada);
	analisis_morfosintactico.selecciona_lenguaje(opciones.lenguajes);
	
	return 0;
}

/**
 * \author fmendez
 * \remarks Cierra dispositivo de audio, ficheros y
 * descarga diccionarios, modelos y voces...
 */
void EcessTP::Fin(void){
	delete output;
	Cotovia::Fin();
}

