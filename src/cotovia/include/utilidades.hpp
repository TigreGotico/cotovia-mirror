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
 



#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#define reservaMemoria(tamano) calloc(tamano,1)

#include <stdio.h>
#include <tip_var.hpp>

void escribe(t_frase_separada *st_fraseSeparada, int vNumero);
void Reset(t_frase_separada *rec,int numero_de_elementos_de_frase_separada, t_opciones opciones);
char * escribe_categoria_gramatical(unsigned char codigo, char* cadena);
unsigned char traducirCategorias(char *entrada);
unsigned char traducirCategoriasLocuciones(char *entrada);
unsigned char traducirXeneroNumero(char *entrada);
int comprobar_en_lista_de_palabras2(const char **inicio_lista,int lonx_item,int lonx, const char *pal);
int comprobar_en_lista_de_inicio_de_palabras2(char **inicio_lista,int lonx_item,int lon_lista, char* pal,char tipo_busqueda);
int comprobar_en_lista_de_inicio_de_palabras3(char **inicio_lista,int lonx_item, int lon_lista, char* pal,char tipo_busqueda);
int esta_palabra_ten_a_categoria_de2(unsigned char categoria,t_frase_separada *pal_frase_separada);

int comprobar_en_lista_de_palabras3(char **inicio_lista,int lonx_item,int lonx, char * pal);
unsigned char formas_no_personales(unsigned char *lista_de_tempos_verdadeiros,
            unsigned char *genero, unsigned char *numero,char *otros_tiempos);
void escribe_tempo_verbal(unsigned char codigo,FILE *fich_sal);
void escribe_categoria_gramatical(unsigned char codigo,FILE *fich_sal);
void escribe_tipo_de_pal(unsigned char codigo,FILE *fich_sal);
void escribe_tipo_de_sintagma(unsigned char codigo,FILE *fich_sal);
void escribe_tipo_de_tonema(unsigned char codigo,FILE *fich_sal);
int error_de_sintaxis (void);
void escribe_tipo_de_proposicion(unsigned char codigo,FILE *fich_sal);
char *escribe_gen(unsigned char codigo,char *aux);
char *escribe_num(unsigned char codigo,char *aux);
#ifdef _WIN32
__declspec(dllexport) int coge_parametros(int ac,char **av, t_opciones * opciones);
__declspec(dllexport) void set_dir_data(t_opciones * opciones);
#else
int coge_parametros(int ac,char **av, t_opciones * opciones);
void set_dir_data(t_opciones * opciones);
#endif
void invertir_cadea2( char* pal);

#endif

