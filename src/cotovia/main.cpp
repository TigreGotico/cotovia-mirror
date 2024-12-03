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
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "modos.hpp"
#include "tip_var.hpp"
#include "interfaz_ficheros.hpp"
#include "utilidades.hpp"
#include "sep_pal.hpp"
#include "preproc.hpp"
#include "transcripcion.hpp"
#include "prosodia.hpp"
#include "trat_fon.hpp"
#include "audio.hpp"
#include "procesado_senhal.hpp"
#include "morfolo.hpp"
#include "sintagma.hpp"
#include "info_corpus_voz.hpp"
#include "path_list.hpp"
#include "matriz.hpp"
#include "cache.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "descriptor.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "Viterbi_acentual.hpp"
#include "Viterbi.hpp"
#include "seleccion_unidades.hpp"
#include "categorias_analisis_morfosintactico.hpp"
#include "perfhash.hpp"
#include "modelo_lenguaje.hpp"
#include "analisis_morfosintactico.hpp"
#include "leer_frase.hpp"
#include "crea_descriptores.hpp"
#include "options.hpp"
#include "cotovia.hpp"
#include "path_list.hpp"

#ifndef _DLL

int main(int argc, char ** argv) {


	Cotovia *cotovia=new Cotovia();
	t_opciones opciones = opciones_por_defecto;


#ifdef _LEE_ENTONACION_DE_FICHERO
	argc -= 1;
#endif

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS
	argc -= 3;
#endif

#if defined(_ESTADISTICA) || defined(_SCRIPT) || \
	defined(_SCRIPT_DURACION) || defined(_GENERA_CORPUS) || \
	defined(_GRUPOS_ACENTUALES) || defined(_PARA_ELISA) || defined(_CALCULA_INDICES) ||  \
	defined(_CALCULA_FACTORES) || defined(_CALCULA_DATOS_MODELO_ENERGIA) || \
	defined(_CALCULA_CONCATENACION_GRUPOS)
	opciones.carga_todo = 1;
#endif

	/*
	 *if (coge_parametros(argc,argv,&opciones) != 0) {
	 *  delete cotovia;
	 *  return 1;
	 *}
	 */
	if (get_options(argc,argv,&opciones) != 0) {
		delete cotovia;
		//fprintf(stderr,"%s: wrong options\n", argv[0]);
		//usage(argv[0]); //error_de_sintaxis();
		return 1;
	}

#ifdef _MODO_NORMAL
	if (opciones.pre==0 && opciones.tra==0 && opciones.lin==0 && opciones.audio==0 &&
			opciones.wav==0 && opciones.carga_todo==0 && opciones.fon==0) {
		delete cotovia;
		fprintf(stderr,"%s: missing options\n", argv[0]);
		usage(argv[0]); //error_de_sintaxis();
		return -1;
	}
#endif

#ifdef _SIN_VOZ
	opciones.audio = 1;
#endif

#if defined(_SCRIPT) || defined(_GRUPOS_ACENTUALES) || defined(_SCRIPT_DURACION) || \
	defined(_CALCULA_DATOS_MODELO_ENERGIA) || defined(_PARA_ELISA)
	opciones.audio = 1;
#endif

	if (cotovia->Carga(opciones) != 0) {
		delete cotovia;
		return 1;
	}


#if defined(_ESTADISTICA)  || \
	defined(_GENERA_CORPUS) || defined(_CORPUS_PROSODICO) || \
	defined(_LEE_ENTONACION_DE_FICHERO) || \
	defined(_SECUENCIAS_FONETICAS)

#if !defined(_CORPUS_PROSODICO) && !defined(_SECUENCIAS_FONETICAS)
	opciones.audio = 1;
#endif

	if (cotovia->cotovia(argc, argv, opciones.fentrada,opciones)!=0)
		delete cotovia;
	return 1;

#elif defined(_CALCULA_INDICES)

	int frecuencia, duracion;
	char fichero_salida[FILENAME_MAX], fichero[FILENAME_MAX], *puntero_fichero;

	strcpy(fichero, argv[1]);
	puntero_fichero = strstr(fichero, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';

	for (duracion = 10; duracion <= 10; duracion += 5)
		for (frecuencia = 0; frecuencia <= 10; frecuencia += 5) {
			sprintf(fichero_salida, "%s_%d_%d.sal", fichero, duracion, frecuencia);

			if (cotovia->cotovia(argv[1], fichero_salida, opciones, (float) duracion,
						(float) frecuencia)!=0)
				delete cotovia;
			return 1;

		} // for (frecuencia = 0;...


#elif defined(_CALCULA_PENALIZACIONES_OPTIMAS)

	float pen_4_gramas, pen_3_gramas, pen_2_gramas;

	pen_4_gramas = atof(argv[argc + 2]);
	pen_3_gramas = atof(argv[argc + 1]);
	pen_2_gramas = atof(argv[argc]);

	if (cotovia->cotovia(argv[1], opciones, pen_4_gramas, pen_3_gramas, pen_2_gramas)!=0)
		delete cotovia;
	return 1;

#elif defined(_CALCULA_FACTORES)

	char fichero_f0[FILENAME_MAX], fichero_dur[FILENAME_MAX], fichero_rafaga[FILENAME_MAX],
			 fichero_concatena[FILENAME_MAX], fichero_fonema_coart[FILENAME_MAX],
			 fichero_func_concatena[FILENAME_MAX], fichero_contextual[FILENAME_MAX],
			 fichero_ent_dur[FILENAME_MAX], fichero_pos_acentual[FILENAME_MAX],
			 fichero_pos_fonico[FILENAME_MAX], fichero_conc_acentual[FILENAME_MAX],
			 fichero_diferencias_prosodicas[FILENAME_MAX];

	char *puntero_fichero;

	strcpy(fichero_f0, argv[1]);
	puntero_fichero = strstr(fichero_f0, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_f0, ".f0");

	strcpy(fichero_dur, argv[1]);
	puntero_fichero = strstr(fichero_dur, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_dur, ".dur");

	strcpy(fichero_rafaga, argv[1]);
	puntero_fichero = strstr(fichero_rafaga, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_rafaga, ".raf");

	strcpy(fichero_concatena, argv[1]);
	puntero_fichero = strstr(fichero_concatena, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_concatena, ".conc");

	strcpy(fichero_fonema_coart, argv[1]);
	puntero_fichero = strstr(fichero_fonema_coart, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_fonema_coart, ".fon_coart");

	strcpy(fichero_contextual, argv[1]);
	puntero_fichero = strstr(fichero_contextual, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_contextual, ".contex");


	strcpy(fichero_func_concatena, argv[1]);
	puntero_fichero = strstr(fichero_func_concatena, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_func_concatena, ".fconc");

	strcpy(fichero_ent_dur, argv[1]);
	puntero_fichero = strstr(fichero_ent_dur, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_ent_dur, ".ent_dur");

	strcpy(fichero_pos_acentual, argv[1]);
	puntero_fichero = strstr(fichero_pos_acentual, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_pos_acentual, ".pos_ac");

	strcpy(fichero_pos_fonico, argv[1]);
	puntero_fichero = strstr(fichero_pos_fonico, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_pos_fonico, ".pos_fon");

	strcpy(fichero_conc_acentual, argv[1]);
	puntero_fichero = strstr(fichero_conc_acentual, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_conc_acentual, ".conc_grupos");

	strcpy(fichero_diferencias_prosodicas, argv[1]);
	puntero_fichero = strstr(fichero_diferencias_prosodicas, ".");
	if (puntero_fichero != NULL)
		*puntero_fichero = '\0';
	strcat(fichero_diferencias_prosodicas, ".dif_prosodia");

	if (cotovia->cotovia(argv[1], fichero_f0, fichero_dur, fichero_rafaga, fichero_concatena,
				fichero_fonema_coart, fichero_contextual, fichero_func_concatena,
				fichero_ent_dur, fichero_pos_acentual, fichero_pos_fonico,
				fichero_conc_acentual, fichero_diferencias_prosodicas, opciones))
		delete cotovia;
	return 1;

#else


	if (opciones.lista_ficheros) {
		FILE * fp;
		int kk;
		int fstdin = opciones.fstdin;

		opciones.fstdin = 0;
		if (fstdin) {
			fp=stdin;
		}
		else {
			if ( (fp = fopen(opciones.fentrada, "rt")) == NULL) {
				fprintf(stderr,"Error al intentar abrir el fichero %s.\n", opciones.fentrada);
				delete cotovia;
				return 1;
			}
		}
		while (fgets(opciones.fentrada,FILENAME_MAX,fp) != NULL){
			kk=strlen(opciones.fentrada)-1;
			if (opciones.fentrada[kk]=='\n')
				opciones.fentrada[kk]=0;
			if (cotovia->cotovia(opciones.fentrada,opciones)!=0)    {
				fclose(fp);
				delete cotovia;
				return 1;
			}
			*opciones.fsalida=0;
		}

		if (!fstdin) {
			fclose(fp);
		}
	}
	else {
		if (cotovia->cotovia(opciones.fentrada, opciones) != 0) {
			delete cotovia;
			return 1;
		}
	}

#endif

	if (cotovia->Descarga() != 0) {
		delete cotovia;
		return 1;
	}
	delete cotovia;
	return 0;

}

#else //si es una libreria
/*
#include "cotovia-dll.hpp"


Cotovia *cotovia;
//cotovia = new Cotovia();
t_opciones opciones = opciones_por_defecto;


int 
#ifdef _WIN32
 __declspec(dllexport) 
#endif
carga(char idioma) {
	
	opciones.idioma = idioma;
	opciones.entrada_por_fichero = 0;  //esto es para que lea cadenas
  //opciones.wav = 1;
	return cotovia->Carga(opciones);
}

int
#ifdef _WIN32
 __declspec(dllexport) 
#endif
sintetiza(char * texto, int et, int ep, char tipo_salida) {

	opciones.et = et;
	opciones.ep = ep;
	if (tipo_salida == WAV) {
		opciones.wav = 1;
		opciones.audio = 0;
	}
	cotovia->cotovia(texto, opciones);
	return 0;
}

int
#ifdef _WIN32
 __declspec(dllexport) 
#endif
descarga(void) {

	cotovia->Descarga();
	return 0;
}
*/
#endif

