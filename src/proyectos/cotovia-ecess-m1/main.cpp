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
#include <getopt.h>

using namespace std;
#ifdef _WIN32
#include <windows.h>
#endif

#include "modos.hpp"
#include "tip_var.hpp"
#include "errores.hpp"
#include "interfaz_ficheros.hpp"
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
#include "version.hpp"
#include "cotovia.hpp"
#include "xmlfile.hpp"
#include "lc-star.hpp"
#include "ecessinterfaces.hpp"
#include "EcessTP.hpp"
//char t_opciones::dir_bd[]="";


void uso(char * exec_file)
{

	fprintf(stdout, "\ncotovia is a bilingual text-to-speech system for Galician and Spanish languages developed by the Multimedia Technologies Group at the University of Vigo (Spain) and a group of researchers of the University of Santiago de Compostela with support from Centro Ramon Pineiro para a Investigacion en Humanidades.\nThis program performs the tokenization, POS tagging and phonetic transcription of the input text, following the ECESS specifications for the symbolic pre-processing module.\n");
	fprintf(stdout, "\nUsage: %s [options]\n", exec_file);
	fprintf(stdout, "\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "  -i, --input=filename          input filename. If no input file is specified, then the standard input is read\n");
	fprintf(stdout, "  -o, --output=filename         output filename. If ommited, output goes to stdout\n");
	fprintf(stdout, "  -l, --lang=(gl|es)            language (default: gl)\n");
	fprintf(stdout, "  -v, --version                 prints version number\n");
	fprintf(stdout, "  -h, --help                    this help message\n");
	fprintf(stdout, "\n");
}




int main(int argc, char ** argv) {


	EcessTP * ecesstp;
	t_opciones opciones = opciones_por_defecto;
	opciones.fstdin=1;
	opciones.fstdout=1;
	int i, l;
	static struct option longopts[] = {
		{"input", required_argument, NULL, 'i'},
		{"output", required_argument, NULL, 'o'},
		{"lang", required_argument, NULL, 'l'},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};


	while ((l = getopt_long(argc, argv, "i:o:l:vh?", longopts, &i)) != EOF) {
		switch (l) {
			case 'i':
				strcpy(opciones.fentrada, optarg);
				opciones.fstdin=0;
				break;
			case 'o':
				strcpy(opciones.fsalida, optarg);
				opciones.fstdout=0;
				break;
			case 'l':
				strcpy(opciones.lenguajes, optarg);
				break;
			case 'v':
				fprintf(stdout, "cotovia-ecess-m1 %s\n", cotovia_version);
				fprintf(stdout, "Copyright (C): 2000-2012 Group on Multimedia Technologies, Universidade de Vigo, Spain http://webs.uvigo.es/gtm_voz\n               1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades, Xunta de Galicia, Santiago de Compostela, Spain http://www.cirp.es\nLicense: GPL-3.0+\n");
				fprintf(stdout, "%s\n", cotovia_date);
				fprintf(stdout, "%s\n", cotovia_compilation);
				return 1;
				break;
			case 'h':
			case '?':
			default:
				uso(argv[0]);
				exit(1);
		}
	}

	char kk = opciones.lin;
	opciones.lin = 2;
	
	set_dir_data(&opciones);

	ecesstp = new EcessTP();

	if (ecesstp->Carga(opciones) != 0) {
		delete ecesstp;
		return 1;
	}
	opciones.lin = kk;
	if (ecesstp->ecesstp(opciones.fentrada, opciones) != 0) {
		delete ecesstp;
		return 1;
	}
	if (ecesstp->Descarga() != 0) {
		delete ecesstp;
		return 1;
	}
	
	delete ecesstp;
	return 0;
}

