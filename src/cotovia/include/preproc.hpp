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
 
 
#ifndef PREPROC_H
   #define PREPROC_H

#ifdef isdigit
   #undef isdigit
#endif
#define isdigit(a) (a>='0' && a<='9')

#include "interfaz_ficheros.hpp"

int busca_en_lista(const t_tabla_traduc *ent, const  char *palabra,char *sal);
void deletrear (char * entrada,char * sal, char idio);


#ifdef _WIN32
class __declspec(dllexport) Preproceso : public Interfaz {
#else
class Preproceso : public Interfaz {
#endif
	private:
		FILE * fd;
		char * fsalida;
		t_palabra_proc * palabras_procesadas;
		Token * frase_tokenizada;
		Token * token_a_preprocesar;
		t_frase_separada * frase_separada;
		char * frase_procesada;
		bool pre;
		char tipo; //opciones.noprepro
		char idioma;
		char separa_token;

		//funcioncillas
		void elimina_caracter(char *entrada,char caracter);
		short int comprobar_se_palabras_anteriores_empezan_por(const char *comenzo,	Token *pos_actual_frase,char num_pal_anteriores);
		short int necesita_distincion_de_xenero(const t_palabra num);
		int concordancia_de_xenero(const t_palabra numero_a_pronunciar,const t_palabra palabra_ca_que_concorda,unsigned char clase_de_palabra);
		void transformacion_de_palabra_en_maiusculas(t_palabra_proc sal, t_palabra ent);
		int averiguar_numero_gramatical(t_palabra palabra_anterior, unsigned char clase_de_pal);
		void adaptar_abreviatura_non_encontrada( char * pal_abreviada,char *abreviatura_pronunciada);
		void transformacion_de_abreviaturas(t_palabra_proc abreviatura_pronunciada,t_palabra pal_aux, t_palabra palabra_anterior, unsigned char clase_de_pal);
		void transcribe_cardinal_ou_ordinal(short int tip,int num,char xenero_do_numero,
				char * numero_pronunciado, char por_sin_acento);
		void transformacion_de_pal_empeza_maiusculas(t_palabra_proc pal_pronunciada, t_palabra pal);
        void transformacion_de_contraccion(t_palabra_proc pal_pronunciada, t_palabra pal);
		void transformacion_de_ordinal(t_palabra_proc numero_pronunciado,t_palabra num, unsigned char xen);
		void centos(char *num,char xenero_do_numero,t_palabra_proc numero_pronunciado);
		void miles(char *num,char xenero_do_numero, t_palabra_proc numero_pronunciado);
		void millons(char *num,char xenero_do_numero, t_palabra_proc numero_pronunciado);
		void numeros(char *num,char xenero_do_numero, t_palabra_proc	numero_pronunciado);
		void lee_mes(char *mes,t_palabra_proc data_pronunciada);
		int datas(char *dia,char *mes,char *anno,t_palabra_proc data_pronunciada);
		void transformacion_numeros_con_guion_no_medio(t_palabra_proc cadea_pronunciada,	char *punt_numeros);
		int transformacion_de_datas(t_palabra_proc data_pronunciada, char *data);
		void transformacion_horas(t_palabra_proc hora_pronunciada,  char *num);
		void transformacion_de_partitivo(t_palabra_proc fraccion_pronunciada, t_palabra palabra_posterior, unsigned char clase_pal_posterior, t_palabra fraccion);
		void transformacion_de_numero_con_punto(t_palabra_proc num_pronunciado,char * num, t_palabra palabra_ca_que_concorda,unsigned char clase_de_pal);
		void transformacion_decimal(t_palabra_proc numero_pronunciado,char *num,const t_palabra palabra_ca_que_concorda, unsigned char clase_de_pal);
		void transformacion_de_tanto_por_cento(t_palabra_proc numero_en_letra,t_palabra porcento);
		void transformacion_de_valor_de_grados(t_palabra_proc numero_en_letra,t_palabra grados,char decimal);
		short int valor_equivalente(unsigned char letra);
		unsigned short int pasar_a_arabigo(t_palabra numero_romano);
		void transformacion_de_romanos(t_palabra_proc numero_pronunciado, t_palabra numero_romano,t_palabra palabra_anterior,unsigned char clase_pal_anterior);
		short int comprobar_se_se_trata_de_num_telefono(t_palabra numero_a_pronunciar, Token *pos_actual_frase);
		void transformar_num_de_telefono(t_palabra_proc numero_en_letra, t_palabra numero_a_pronunciar);
		void transformacion_de_cardinal(t_palabra_proc numero_en_letra, t_palabra	numero_a_pronunciar, t_palabra pal_ca_que_concorda,unsigned char clase_de_palabra, Token *pos_actual_frase);
		void transformacion_de_inicial(t_palabra_proc letra_pronunciada, t_palabra pal);
		int unidades_de_medida_sin_punto(char *entrada,  char *saida,char *pal_anterior);
		void transformacion_de_caracter_especial(char *signo_especial_procesado,char * caracter,  char * palabra_anterior);
		void transformacion_de_palabra_con_guion_no_medio(t_palabra_proc pal_saida,t_palabra pal_entrada);
		void adaptacion_por_defecto( char *abreviatura_pronunciada, char *pal_abreviada);
		void elemento_a_preprocesar(Token *elemento_de_frase, int *orden,	int no_primera);
		int deletrea_numero(char * entrada, char *salida);
		void transformacion_de_palabra_con_punto_no_medio(t_palabra_proc pal_saida,t_palabra pal_entrada);
		int escribe_fichero(char * nombre_fichero);



	public:
		Preproceso();
		~Preproceso();
		void inicializa(t_opciones * opciones);
		int preprocesa(Token * ft, char * fp, t_frase_separada *fs, int *n);
		void finaliza();
};



#endif






