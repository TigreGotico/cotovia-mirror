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
 

#ifndef _PROCESADO_SENHAL_HH

#include "descriptor.hpp"

#define FICHERO_SONIDO "corpus_sonido.snd"


void ventana_hanning(float *vent, int lonx, int pos);
void ventana_rectangu_hanning(float *vent, int tamano, int pitch_inicial, int pitch_final);
void crear_cab_wav(FILE *ffwav, short int wav8);

void devuelve_valor_maximo_senhal(short int *datos, int longitud, short int *valor_maximo);

void crear_cab_wav(FILE *ffwav, int frecuencia);

int filtra_diezma(short int * entrada, int tam);


class Procesado_senhal{

private:

  short int * buffer; // Buffer para el solapamiento de los segmentos de voz sintética.
  int tamano_ventana, tamano_media_ventana;

  int memoria_reservada; // Memoria reservada para el buffer.
  short int * datos;

  float * vent;
  char * datos_audio;
  unsigned int num_dat_fwav;

  //FILE *fichero_datos;

  bool audio;
  bool wav;
  bool f8;
  char * fsalida;
  FILE * fwav;
  int frecuencia_muestreo;
  Audio * dispositivo_audio;

  char * dir_sal;
  char * fentrada;

public:

  void inicializa(bool paudio, bool pwav, bool pf8, int fm, char * fichero_sal,
		  char * dir_salida, char * fichero_entrada, Audio * pdispositivo_audio);

  void reinicia(bool paudio, bool pwav, bool pf8, int fm, char * fichero_sal, char * dir_salida, char * fichero_entrada);

  int genera_senhal_sintetizada(Vector_sintesis *cadena_sintesis, vector<Vector_descriptor_objetivo> &objetivo,  int numero_unidades, unsigned char modif_prosod, int minpitch, int maxpitch, FILE * fichero_sonido, char ficheros_segmentacion);

  int modifica_f0_no_lineal(int *marcas_originales, int numero_marcas,
			    float f_inicial, float f_final,
			    float *marcas_intermedias,
			    int tamano_maximo);

  int modifica_prosodia_corpus(Vector_descriptor_objetivo &objetivo,
			       Vector_sintesis *unidad_sintesis,
			       unsigned char frec_tiempo);

  int prosodia_original(Vector_sintesis *vector);

  int sintesis_unidad(Vector_sintesis *unidad_sintesis,
		      int minpitch, int maxpitch,
		      short int *senhal,
		      int *total_datos,  int *ultimo_pitch_sintesis,
		      int *anterior_pitch_analisis,
		      int *posicion_primera_marca_sintesis,
		      FILE * fichero_sonido);

  int extrae_forma_de_onda_original(Vector_sintesis *primer_vector, Vector_sintesis *ultimo_vector,
				    int minpitch, int maxpitch,
				    short int *senhal, int *total_datos, int *ultimo_pitch_sintesis,
				    int *anterior_pitch_analisis,
				    int *posicion_primera_marca_sintesis,
				    FILE * fichero_sonido);

  void libera_memoria_procesado_senhal();
  void finaliza();

};

#endif
