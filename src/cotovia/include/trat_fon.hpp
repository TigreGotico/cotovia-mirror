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
 
 
#ifndef TRAT_FON_H
   #define TRAT_FON_H

#define  VOC_ABERTA                    1
#define  VOC_PECHADA                   0

// #define loc(c) ( (c >= LOC_ADVE && c <= LOC_ADVE_DUBI) || \
//       (c >= LOC_PREP && c<= LOC_PREP_CONDI)||               \
//       (c >= LOC_CONX && c <= LOC_CONX_SUBOR_CORREL) )
// 
// #define adv(c) (c>=ADVE && c<=ADVE_DISTR)

#include "interfaz_ficheros.hpp"

int hai_grupo_eu_ou_na_antepenultima_silaba(char *pal);
int timbre_en_agudas( char *palabra);
int estan_en_contacto_con_nasal( char *pal_sil_ac);
int timbre_en_graves( char *pal, char *pal_sil_ac);
void cambiar_por_timbre_aberto(char *palabra);
int timbre_en_esdruxulas(char *palabra, char * pal_acentuada);
int posicion_acento(char *pal);
int e_sustantivo(t_frase_separada *frase_separada);
void asignar_timbre_a_sustantivos(char *palabra, char * palabra_acentuada);
void asignar_timbre_en_frase_separada(t_frase_separada *frase_separada);
//int comprobar_en_lista_de_verbos(char **inicio_lista,int lonx_item,int lonx,char *pal);
int comprobar_en_lista_de_palabras(char **inicio_lista,int lonx_item,int lonx,char *pal);
void invertir_cadea( char* pal);
int comprobar_en_lista_de_inicio_de_palabras(char **inicio_lista,int lonx_item,
                            int lon_lista, char* pal,char tipo_busqueda);
void tratamento_das_excepcions_da_w( char *palabra, char *pal_sil_acen);
void tratamento_das_excepcions_da_xe( char *palabra, char *pal_sil_acen);
char *extraer_texto_entre_espacios( char *entrada, char* saida);
inline void insertar_silencio_inicial(char *f_sil_e_acentuada);
int duracion_pausa(t_frase_en_grupos *f_en_grupos);
void insertar_pausa_entre_grupos(t_frase_en_grupos *f_en_grupos,char *f_sil_e_acentuada);
void saca_tipo_prop(char * cad);
void insertar_pausa_entre_sintagmas(t_frase_sintagmada sintag,char *f_sil_e_acentuada);

//éstas 3 hay que exportarlas en la dll win32

void
#ifdef _WIN32
__declspec(dllexport) 
#endif
insertar_pausa_entre_palabras(t_frase_separada *pal,char *f_sil_e_acentuada);
void
#ifdef _WIN32
__declspec(dllexport) 
#endif
asignar_pausa_entre_frases(char *f_sil_e_acentuada);
void
#ifdef _WIN32
__declspec(dllexport) 
#endif
insertar_tipo_de_proposicion(t_frase_en_grupos *f_en_grupos, char *f_sil_e_acentuada);


class Trat_fon : Interfaz{
	private:
		t_frase_separada * frase_separada;
		char * frase_sil_e_acentuada;

		int tonica(t_frase_separada *palabra,unsigned char categoria);
		void trat_fonetico( char *palabra, char *pal_sil_acen, int tonicidade, char idioma, char se_e_sustantivo);

	public:
		void
#ifdef _WIN32
__declspec(dllexport) 
#endif 
		atono_ou_tonico_aberto_ou_pechado_e_w_x(t_frase_separada *f_sep, char idioma);
		int arregla_timbre(t_frase_separada *cp_frase_separada);
		void tratamento_fonetico_previo(t_frase_separada *rec_frase_separada, t_frase_sintagmada *rec_frase_sintagmada, t_frase_en_grupos *rec_frase_en_grupos, char * frase_sil_e_acentuada, char idioma);
};


#endif
 

