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
 
#ifndef _TRANSCRIPCION_HPP
#define _TRANSCRIPCION_HPP

#define _copia_cadena(dst, src) {char *asdf=(dst);char *asdf2=(src);for(;*asdf2;*asdf++=*asdf2++);*asdf=0;}

#define MAX_TRANSCRIPCIONES 500

typedef struct vocabulario
{
	char *pal;
	char *tra;
	char final_atono;
	char inicio_atono;
}
t_vocabulario;

int TranscIgual(char *transc, char **lista, int n);

#ifdef _WIN32
class __declspec(dllexport) Transcripcion
#else
class Transcripcion
#endif
{
	private:

		t_regla * reglas_transcripcion[256];

		t_regla * regla;
		char bufer_texto_a_sustituir[41];
		char * pos_actual_frase_sil;

		static int variantes_cargadas;
		FILE * fd;
		char *fsalida;				///nombre del fichero de salida
		char *palabras;
		char transcripcion[LONX_MAX_FRASE_SIL_E_ACENTUADA];
		t_vocabulario *vocabulario;
		char **transcripciones;
		t_frase_separada *frase_separada;
		int importancia;

		int ntransc;
		char tra;					///tipo de transcripcion
		char por_palabras;			///se imprime palabra <tab> transcripcion
		char alternativas;			///se imprimen transcripciones alternativas
		char ftra;					///se usa un fichero con transcripciones

		int sacar_transcripcion_con_alternativas(char *frase, t_frase_separada * aux);
		int obten_transcripciones_alternativas(char *ff);
		int sacar_transcripcion(char *frase, t_frase_separada * aux);
		int elimina_comentarios(char *fi, char *fo);

		void inicializar_reglas_transcripcion(char idioma);
		inline void busqueda_de_regla();
		inline void busqueda_de_regla(char * bufer);

	public:

		int divide_frase_fonetica(char * ffon, t_frase_separada * fs);
		int transcribe(t_frase_separada * f_sep, char * f_sil, char * f_fon, char idioma);
		int transcribir(t_frase_separada * fs, char * f_sil, char * f_fon, char idioma);
		int transformar_a_alofonos(char * pos_actual_frase_sil,  char * f_fon, char idioma);
		int vuelca_transcripcion(char *f_fon, t_frase_separada * f_sep);
		int inicializa(t_opciones * opciones);
		void finaliza();
		int lee_variantes(char *nombre_fichero, int imp);
		int libera_variantes(void);
		Transcripcion();
		~Transcripcion();

};
#endif

