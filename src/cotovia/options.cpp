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
#include <string.h>
#include <getopt.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "version.hpp"
#include "utilidades.hpp"

void usage(char * exec_file)
{

	fprintf(stdout, "\ncotovia is a bilingual text-to-speech system for Galician and Spanish languages developed by researchers from the Multimedia Technologies Group at the University of Vigo and from the Center 'Ramón Piñeiro' for Research in Humanities, both in Galicia, Spain.\n");
	fprintf(stdout, "\nUsage: %s [options]\n", exec_file);
	fprintf(stdout, "\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "  -i, --input=<filename>                                  Input filename. If no input file is specified, then the standard input is read\n");
	fprintf(stdout, "  -I, --input-file-list                                   Input file is a list of files to be processed.\n"); //-lista-ficheros
//fprintf(stdout, "  -o, --output=filename                                   Output filename. If ommited, output goes to stdout\n");
	fprintf(stdout, "  -s, --soundcard-output                                  Speech output to the soundcard device\n"); //-voz
	fprintf(stdout, "  -w, --wav-file-output                                   Speech output to a wav file\n"); // -wav
	fprintf(stdout, "  -p, --preprocessed-text-output                          Outputs the normalized text input\n"); //-pre
	fprintf(stdout, "  -P, --no-text-preprocessing                             Simplifies the preprocess/normalization of input text\n"); //-noprepro
	fprintf(stdout, "  -t, --phonetic-transcription-output[={0|1|2|3}]         Outputs the phonetic transcription (default:0):\n\t0 => only phonemes;\n\t1 => 0 plus stress markers;\n\t2 => 1 plus syllables;\n\t3 => 2 plus pause markers\n"); // -tra
	fprintf(stdout, "  -A, --alternative-transcriptions[={0|1|2|3}]            Like --phonetic-transcription-output, but providing alternative phonetic transcriptions\n"); //-tralt
	fprintf(stdout, "  -W, --by-words                                          When --phonetic-transcription-output is selected, outputs phonetic transcription word by word\n"); //-porpalabras
  fprintf(stdout, "  -L, --linguistic-analysis-output[={0|1|2|3|4|5|6|7|8}]  Outputs linguistic information (default:0):\n\t0 => full;\n\t1 => Only morphosyntactic analysis;\n\t2 => word, POS, gender, number, lexical stress;\n\t3 => syntagma, words, number of syllables;\n\t4 => word, root, POS in eagles format;\n\t5 => word, POS, gender, number, statistical analysis punctuation;\n\t6 => word, root, lexeme and morphemes;\n\t7 => same as 3 plus lexical stress;\n\t8 => outputs sentences, splitting contractions and verbal forms with enclitics;\n"); //-lin
	fprintf(stdout, "  -f, --phoneme-sequence-output                           Outputs the phoneme sequence for the input text\n"); //-fon
	fprintf(stdout, "  -N, --total-intonation-contours=<value>                 Number of intonation contours for the intonation search (default:50)\n"); // -N
	fprintf(stdout, "  -M, --contours-acoustic-search=<value>                  Number of intonation contours for the acoustic search (default:10)\n"); //-M
	fprintf(stdout, "  -u, --units-info                                        Outputs information about prosodic and acoustic unit selection\n"); //-info_unidades
	fprintf(stdout, "  -m, --morphosyntactic-input-file=<filename>             Loads morphosyntactic analysis from this file\n"); // -fich_mmorfo
	fprintf(stdout, "  -g, --segmentation-files                                Generates output files with phonetic segmentation and synthesis pitchmarks\n"); // -ficheros_segmentacion
	fprintf(stdout, "  -S, --stdout                                            Outputs to stdout\n"); // -stdout
	fprintf(stdout, "  -T, --split-tokens                                      Splits tokens\n"); // -stdout
	fprintf(stdout, "  -n, --line                                              Treats each line as one utterance, ignoring other criteria\n"); //-linea
	fprintf(stdout, "  -r, --morpho-output-separator                           Output from --linguistic-analysis-output includes one separator (################################) between sentences\n"); //-separa_lin
	fprintf(stdout, "  -F, --forced-IB                                         Force the intonation breaks provided in the input text, represented with the symbol ',#R-E#'\n"); //-fuerza_fronteras
	fprintf(stdout, "  -c, --selection-type={0|1|2|3|4|5}                      Unit selection type (default:5):\n\t0 => Parallel computation;\n\t1 => Sequential computation;\n\t2 => Like 1, but splitting acoustic unit selection in segments limited by silences or voiceless oclusive phonemes;\n\t3 => like 0, but splitting acoustic unit selection in segments;\n\t4 => Like 2, but performing the intonation search at phonic group level. It must be combined with --intonation-selection-type=2;\n\t5 => Like 3, but performing the intonation search at phonic group level. It must be combined with --intonation-selection-type=2\n"); // -tipo_seleccion
	fprintf(stdout, "  -C, --intonation-selection-type={0|1|2}                 Intonation selection type (default:2):\n\t0 => Intonation contours are computed by sentence;\n\t1 => Intonation contours are computed by phonic group, returning one set of contours for the sentence;\n\t2 => Contours are computed by phonic group, returning one set of contours for every phonic group.\n"); // -tipo_seleccion_entonativa
	fprintf(stdout, "  -d, --candidate-type={0|1}                              Candidate selection type for Viterbi algorithm (default:0):\n\t0 => Every demiphone with the same lexical stress is a candidate;\n\t1 => Every demiphone with the same lexical stress, position in the sentence and kind of sentence is a candidate;\n"); // -tipo_candidatos
	fprintf(stdout, "  -y, --prosody-modification={0|1|2|3|4}                  Prosody modifications type (default:3):\n\t0 => never modify (raw copy of originally contiguous units);\n\t1 => always modify (synthesis from windowed segments);\n\t2 => no modifications and raw copy of original units (no special consideration about originally contiguous units);\n\t3 => modification when pitch or duration differences exceed some fixed thresholds;\n\t4 => no modifications, but no raw copy of speech units (units are synthesized from their original windowed segments);\n"); // -modif_prosod
	fprintf(stdout, "  -x, --time-scale=<value>                                Time scale factor (default:100)\n"); // -et
	fprintf(stdout, "  -z, --pitch-scale=<value>                               Pitch scale factor (default:100)\n"); //-ep
	fprintf(stdout, "  -D, --data-dir=<pathdir>                                Path to the data directory (default:/usr/share/cotovia/data)\n"); // -dirdata
	fprintf(stdout, "  -O, --output-dir=<pathdir>                              Path to the output directory (default:.)\n"); //-dirsal
	fprintf(stdout, "  -V, --voice={iago|sabela}                               Voice selection (default: iago)\n"); // -bd
	fprintf(stdout, "  -l, --lang={gl|es}                                      Language selection (default: gl)\n"); //-l    
	fprintf(stdout, "  -a, --conf-file=<filename>                              Configuration file for voice data generation\n"); //-fich_conf
	fprintf(stdout, "  -b, --units-input-file=<filename>                       Synthesize the units given in this file\n"); // -fich_unidades
	fprintf(stdout, "  -j, --units-usage-output-file=<filename>                Writes information about the units chosen for synthesis. Useful for acoustic units prunning\n"); // -fich_uso_unidades
	fprintf(stdout, "  -H, --galician-help                                     Help in galician language\n");
	fprintf(stdout, "  -v, --version                                           Prints cotovia's version number\n");
	fprintf(stdout, "  -h, --help                                              This help message\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Examples:\n");
	fprintf(stdout, "  cotovia -i text.txt -s -V sabela                        Outputs speech for the input file text.txt to the soundcard, using sabela's voice\n");
	fprintf(stdout, "  cotovia -i text.txt -w -les -V iago                     Outputs speech for the input file text.txt to the wav file text.wav, using iago's voice and spanish language\n");
	fprintf(stdout, "  cat text.txt |cotovia -Stlgl | less                     Outputs, from stdin to stdout, the phonetic transcription in galician language for the file text.txt\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Report bugs to <cotovia@gts.uvigo.es>\n");

}
void usage_gl(char * exec_file)
{
	fprintf(stdout, "\ncotovia é un sistema de conversión texto-voz bilingüe para galego e castelán desenvolvido por investigadores do Grupo de Tecnoloxías Multimedia da Universidade de Vigo e do Centro Ramón Piñeiro para a Investigación en Humanidades.\n");
	fprintf(stdout, "\nUso: %s [options]\n", exec_file);
	fprintf(stdout, "\n");
	fprintf(stdout, "Opcions:\n");
	fprintf(stdout, "  -i, --input=<arquivo>                                   Arquivo de entrada. Se non se especifica, úsase a entrada estándar stdin\n");
	fprintf(stdout, "  -I, --input-file-list                                   O arquivo de entrada é unha lista de arquivos para procesar.\n"); //-lista-ficheros
//fprintf(stdout, "  -o, --output=<filename>                                 Output filename. If ommited, output goes to stdout\n");
	fprintf(stdout, "  -s, --soundcard-output                                  Saída de voz polo dispositivo de son\n"); //-voz
	fprintf(stdout, "  -w, --wav-file-output                                   Saída de voz nun arquivo wav\n"); // -wav
	fprintf(stdout, "  -p, --preprocessed-text-output                          Xera a frase preprocesada\n"); //-pre
	fprintf(stdout, "  -P, --no-text-preprocessing                             Preprocesado do texto simplificado\n"); //-noprepro
	fprintf(stdout, "  -t, --phonetic-transcription-output[={0|1|2|3}]         Transcrición fonética (por defecto:0):\n\t0 => fonemas;\n\t1 => 0 + acento;\n\t2 => 1 + sílabas;\n\t3 => 2 + pausas\n"); // -tra
	fprintf(stdout, "  -A, --alternative-transcriptions[={0|1|2|3}]            Semellante a --phonetic-transcription-output, proporcionando transcricións fonéticas alternativas\n"); //-tralt
	fprintf(stdout, "  -W, --by-words                                          con --phonetic-transcription-output, saca a transcrición fonética palabra a palabra\n"); //-porpalabras
	fprintf(stdout, "  -L, --linguistic-analysis-output[={0|1|2|3|4|5|6|7|8}]  Xera datos linguisticos (por defecto:0):\n\t0 => completa;\n\t1 => só análise morfolóxica;\n\t2 => categoría, xénero, número, tonicidade;\n\t3 => tipo sintagma, sintagma, no. de silabas do sintagma;\n\t4 => palabra,lema e categoría morfosintáctica en formato eagles;\n\t5 => palabra, xénero, número e puntuación da análise morfosintáctica;\n\t6 => palabra, lema, lexema e morfemas;\n\t7 => análise sintáctico (tipo sintagma, sintagma, no. de sílabas, tonicidade) con pausas do modelo de pausado;\n\t8 => saída traductor: saca a frase amosando a estrutura das contraccións e formas verbais con enclíticos;\n"); //-lin
	fprintf(stdout, "  -f, --phoneme-sequence-output                           Escribe nun arquivo a secuencia de fonemas que compoñen a frase.\n"); //-fon
	fprintf(stdout, "  -N, --total-intonation-contours                         Número total de contornos entonativos resultantes da búsqueda entonativa. (por defecto:200)\n"); // -N
	fprintf(stdout, "  -M, --contours-acoustic-search                          Número de contornos entonativos que pasan á búsqueda de unidades acústicas. (por defecto:100)\n"); //-M
	fprintf(stdout, "  -u, --units-info                                        Arquivos coa información da selección de unidades acústicas e prosódicas.\n"); //-info_unidades
	fprintf(stdout, "  -m, --morphosyntactic-input-file=<arquivo>              En lugar de analizar a frase, lé a análise morfosintáctica do arquivo indicado.\n"); // -fich_morfo
	fprintf(stdout, "  -g, --segmentation-files                                Arquivos coa información da segmentación fonética e as marcas de síntesis do arquivo de son xerado\n"); // -ficheros_segmentacion
	fprintf(stdout, "  -S, --stdout                                            Resultado pola saída estándar\n"); // -stdout
	fprintf(stdout, "  -T, --split-tokens                                      Split tokens\n"); // -stdout
	fprintf(stdout, "  -n, --line                                              Unha liña->unha frase\n"); //-linea
	fprintf(stdout, "  -r, --morpho-output-separator                           Separa as frases na saída tipo --linguistic-analysis-output (separador ################################)\n"); //-separa_lin
	fprintf(stdout, "  -F, --forced-IB                                         Forza que as fronteiras entonativas sexan as indicadas na frase de entrada (co símbolo ,#R-E#)\n"); //-fuerza_fronteras
	fprintf(stdout, "  -c, --selection-type={0|1|2|3|4|5}                      Tipo de selección de unidades (5 por defecto):\n\t0 => Cálculo de contornos en paralelo\n\t1 => Cálculo secuencial para cada contorno.\n\t2 => Cálculo secuencial, pero dividindo a selección de unidades acústicas en segmentos limitados por silencios ou oclusivas xordas.\n\t3 => Cálculo de contornos en paralelo  dividindo as unidades acústicas en segmentos.\n\t4 => Semellante a 2, pero facendo a búsqueda entonativa por grupos fónicos. Debe combinarse con --intonation-selection-type=2.\n\t5 => Semellante a 3, pero facendo a búsqueda entonativa por grupos fónicos. Debe combinarse con --intonation-selection-type=2\n"); // -tipo_seleccion
	fprintf(stdout, "  -C, --intonation-selection-type={0|1|2}                 Tipo de selección entonativa (2 por defecto):\n\t0 => Cálculo dos contornos de forma global, para toda a oración.\n\t1 => Cálculo dos contornos por grupos fónicos, escollendo un conxunto de contornos para toda a frase.\n\t2 => Cálculo dos contornos por grupos fónicos, devolvendo unha serie de posibilidades para cada grupo fónico.\n"); // -tipo_seleccion_entonativa
	fprintf(stdout, "  -d, --candidate-type={0|1}                              Tipo de seleccion de candidatos para o algoritmo de Viterbi (0 por defecto)i:\n\t0 => Todos os semifonemas que coincidan en carácter tónico son candidatos.\n\t1 => Únicamente os que coincidan en carácter tónico, posición na frase e tipo de frase.\n"); // -tipo_candidatos
	fprintf(stdout, "  -y, --prosody-modification={0|1|2|3|4}                  Tipo de modificacións prosódicas (3 por defecto):\n\t0 => nunca se modifican prosódicamente as unidades.\n\t1 => modificanse sempre.\n\t2 => non se modifican, pero non se copia directamente a forma de onda de unidades consecutivas.\n\t3 => modifícanse se a diferencia entre os valores desexados e os buscados é demasiado grande.\n\t4 => non se modifican as unidades, pero xeranse a partir dos seus segmentos enventanados.\n"); // -modif_prosod
	fprintf(stdout, "  -x, --time-scale=<valor>                                Porcentaxe de escala temporal (100 por defecto)\n"); // -et
	fprintf(stdout, "  -z, --pitch-scale=<valor>                               Porcentaxe de escala de pitch (100 por defecto)\n"); //-ep
	fprintf(stdout, "  -D, --data-dir=<ruta>                                   Directorio de base voz e dicionarios (/usr/share/cotovia/data ou ../data por defecto)\n"); // -dirdata
	fprintf(stdout, "  -O, --output-dir=<ruta>                                 Directorio de saida (dir. actual por defecto)\n"); //-dirsal
	fprintf(stdout, "  -V, --voice={iago|sabela}                               Seleccion da base de datos de voz (iago por defecto)\n"); // -bd
	fprintf(stdout, "  -l, --lang={gl|es}                                      Seleccion do idioma (gl por defecto)\n"); //-l 
	fprintf(stdout, "  -a, --conf-file=<arquivo>                               Arquivo de configuración empregado na creación das bases de voz\n"); //-fich_conf
	fprintf(stdout, "  -b, --units-input-file=<arquivo>                        Xera voz sintética a partir das unidades indicadas no arquivo.\n"); // -fich_unidades
	fprintf(stdout, "  -j, --units-usage-output-file=<arquivo>                 Escribe nun arquivo os semifonemas que se escolleron para a síntesis. Emprégase para a poda de unidades acústicas.\n"); // -fich_uso_unidades
	fprintf(stdout, "  -H, --galician-help                                     Mensaxe de axuda en galego\n");
	fprintf(stdout, "  -v, --version                                           Mostra a información da versión de cotovia\n");
	fprintf(stdout, "  -h, --help                                              Axuda en inglés\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Exemplos de uso:\n");
	fprintf(stdout, "  cotovia -i texto.txt -s -V sabela                       Sintetiza o arquivo texto.txt mandando a saída á tarxeta de son, usando a voz 'sabela'\n");
	fprintf(stdout, "  cotovia -i texto.txt -w -les -V iago                    Sintetiza o arquivo texto.txt mandando a saída ao arquivo wav texto.wav, usando a voz 'iago' e lingua española\n");
	fprintf(stdout, "  cat texto.txt |cotovia -Stlgl | less                    Saca pola saída estándar a transcrición fonética en galego do arquivo texto.txt, pasado pola entrada estándar\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Por favor, informe dos erros en <cotovia@gts.uvigo.es>\n");

}



int get_options(int argc, char ** argv, t_opciones * opciones)
{
	
	int i, l;
	bool voice_selected=false;

	static struct option longopts[] = {
		{"input", required_argument, NULL, 'i'},
		{"input-file-list", required_argument, NULL, 'I'},
//		{"output", required_argument, NULL, 'o'},
		{"soundcard-output", no_argument, NULL, 's'},
		//{"voz", no_argument, NULL, 's'},
		{"wav-file-output", no_argument, NULL, 'w'},
		{"preprocessed-text-output", no_argument, NULL, 'p'},
		{"no-text-preprocessing", no_argument, NULL, 'P'},
		{"phonetic-transcription-output", optional_argument, NULL, 't'},
		{"alternative-transcriptions", optional_argument, NULL, 'A'},
		{"by-words", no_argument, NULL, 'W'},
		{"linguistic-analysis-output", optional_argument, NULL, 'L'},
		{"phoneme-sequence-output", no_argument, NULL, 'f'},
		{"total-intonation-contours", required_argument, NULL, 'N'},
		{"contours-acoustic-search", required_argument, NULL, 'M'},
		{"units-info", no_argument, NULL, 'u'},
		{"morphosyntactic-input-file", required_argument, NULL, 'm'},
		{"segmentation-files", no_argument, NULL, 'g'},
		{"stdout", no_argument, NULL, 'S'},
		{"split-tokens", no_argument, NULL, 'T'},
		{"line", no_argument, NULL, 'n'},
		{"morpho-output-separator", no_argument, NULL, 'r'},
		{"forced-IB", no_argument, NULL, 'F'},
		{"selection-type", required_argument, NULL, 'c'},
		{"entonative-selection-type", required_argument, NULL, 'C'},
		{"candidate-type", required_argument, NULL, 'd'},
		{"prosody-modification", required_argument, NULL, 'y'},
		{"time-scale", required_argument, NULL, 'x'},
		{"pitch-scale", required_argument, NULL, 'z'},
		{"data-dir", required_argument, NULL, 'D'},
		{"output-dir", required_argument, NULL, 'O'},
		{"voice", required_argument, NULL, 'V'},
		//{"bd", required_argument, NULL, 'V'},
		{"lang", required_argument, NULL, 'l'},
		{"conf-file", required_argument, NULL, 'a'},
		{"units-input-file", required_argument, NULL, 'b'},
		{"units-usage-output-file", required_argument, NULL, 'j'},
		{"galician-help", no_argument, NULL, 'H'},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};


	opciones->fstdin=1;

	//while ((l = getopt_long_only(argc, argv, "i:IswpPt::A::WL::fN:M:um:gSTnrFc:C:d:y:x:z:D:O:V:l:a:b:j:Hvh?", longopts, &i)) != EOF) {
	while ((l = getopt_long(argc, argv, "i:IswpPt::A::WL::fN:M:um:gSTnrFc:C:d:y:x:z:D:O:V:l:a:b:j:Hvh?", longopts, &i)) != EOF) {
		switch (l) {
			case 'i':
				strcpy(opciones->fentrada, optarg);
				opciones->fstdin=0;
				break;
			case 'I':
				opciones->lista_ficheros=1;
				break;
			case 's':
				opciones->audio=AUDIO;
				break;
			case 'w':
				opciones->wav=FWAV;
				break;
			case 'p':
				opciones->pre=1;
				break;
			case 'P':
				opciones->noprepro=1;
				break;
			case 't':
				opciones->tra=1;
				if (optarg) {
					if (*optarg >= '0' && *optarg<='3') {
						opciones->tra += atoi(optarg);
					}
					else { 
						opciones->tra=0;
					}
				}
				break;
			case 'A':
				opciones->alternativas=1;
				opciones->tra=1;
				if (optarg) {
					if (*optarg >= '0' && *optarg<='3') {
						opciones->alternativas += atoi(optarg);
					}
					else { 
						opciones->alternativas=0;
						opciones->tra=0;
					}
				}
				break;
			case 'W':
				opciones->por_palabras=1;
				break;
			case 'L':
				opciones->lin=1;
				if (optarg) {
					if (*optarg>='0' && *optarg<='8') {
					opciones->lin += atoi(optarg);
					}
					else { 
						opciones->lin=0;
					}
				}
				break;
			case 'f':
				opciones->fon = 1;
				break;
			case 'N':
				opciones->N=atoi(optarg);
				break;
			case 'M':
				opciones->M=atoi(optarg);
				break;
			case 'u':
				opciones->info_unidades = 1;
				break;
			case 'm':
				strcpy(opciones->fich_morfosintactico, optarg);
				break;
			case 'g':
				opciones->ficheros_segmentacion = 1;
				break;
			case 'S':
				opciones->fstdout=1;
				break;
			case 'T':
				opciones->separa_token=1;
				break;
			case 'n':
				opciones->unalinea=1;
				break;
			case 'r':
				opciones->separa_lin=1;
				break;
			case 'F':
				opciones->fuerza_fronteras = 1;
				break;
			case 'c':
				opciones->tipo_seleccion_unidades=atoi(optarg);
				break;
			case 'C':
				opciones->tipo_seleccion_entonativa=atoi(optarg);
				break;
			case 'd':
				opciones->tipo_candidatos=atoi(optarg);
				break;
			case 'y':
				opciones->modificacion_prosodica=atoi(optarg);
				break;
			case 'x':
				opciones->et=atoi(optarg);
				break;
			case 'z':
				opciones->ep=atoi(optarg);
				break;
			case 'D':
				strcpy(opciones->dir_bd,optarg);
				break;
			case 'O':
				strcpy(opciones->dir_sal,optarg);
				break;
			case 'V':
				if (voice_selected){
					strcat(opciones->locutores, " ");
				}
				else {
					*opciones->locutores=0;
					voice_selected = true;
				}
				strcat(opciones->locutores, optarg);
				break;
			case 'l':
				strcpy(opciones->lenguajes, optarg);
				break;
			case 'a':
				strcpy(opciones->fich_conf, optarg);
				break;
			case 'b':
				strcpy(opciones->fich_unidades, optarg);
				break;
			case 'j':
				strcpy(opciones->fich_uso_unidades, optarg);
				break;
      /*
			 *case 'o':
			 *  strcpy(opciones.fsalida, optarg);
			 *  opciones.fstdout=0;
			 *  break;
       */
			case 'H':
				usage_gl(argv[0]);//error_de_sintaxis();
				return 1;
				break;
			case 'v':
				fprintf(stdout, "cotovia %s\n", cotovia_version);
				fprintf(stdout, "Copyright (C): 2000-2012 Group on Multimedia Technologies, Universidade de Vigo, Spain http://webs.uvigo.es/gtm_voz\n               1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades, Xunta de Galicia, Santiago de Compostela, Spain http://www.cirp.es\nLicense: GPL-3.0+\n");
				fprintf(stdout, "%s\n", cotovia_date);
				fprintf(stdout, "%s\n", cotovia_compilation);
				return 1;
				break;
			case 'h':
			case '?':
			default:
				usage(argv[0]);
				return -1;
		}
	}
	set_dir_data(opciones);

	if ( ( (opciones->tipo_seleccion_entonativa == 2) && (opciones->tipo_seleccion_unidades != 4) && (opciones->tipo_seleccion_unidades != 5) ) ||
			( ( (opciones->tipo_seleccion_unidades == 4) || (opciones->tipo_seleccion_unidades == 5) ) && (opciones->tipo_seleccion_entonativa != 2) ) ) {
		fprintf(stderr, "El parámetro -tipo_seleccion [4 5] debe ir combinado con -tipo_seleccion_entonativa 2.\n");
		return 1;
	}

	if ( (opciones->tipo_seleccion_entonativa == 2) && (opciones->tipo_seleccion_unidades < 4) ) {
		fprintf(stderr, "El parámetro -tipo_seleccion [0 1 2 3] debe ir combinado con -tipo_seleccion_entonativa [0 1].\n");
		return 1;
	}

	return 0;
}
