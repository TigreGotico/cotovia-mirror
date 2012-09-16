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
 
 
#ifndef SIL_ACENT_H
   #define SIL_ACENT_H

int guion_de_segunda_forma_do_artigo(char *recorre_frase);
void separar_segun_modelo(const char *modelo,t_palabra saida,char *letra);
short int secuencia_de_(unsigned char cantidade,unsigned char tipo, const char * orixe);
short int  grupo_consonantico_indivisible(char grupo[2]);
short int diptongo(const char *letra);
short int caso_de_u_muda(const char * letra);
int solo_quedan_consonantes(const char *letra);
short int triptongo(const char * letra);
void separar_silabas(const char * entrada,char * saida);
char *grave(char * palabra, char * p);
char *aguda(char * palabra, char * p);
short int numero_silabas(t_palabra pal);
void eliminar_til(char *vocal_acentuada);
short int diacritico_dif_aberta_pechada(t_palabra pal_entrada,t_palabra pal_saida);
int acentuar_prosodicamente(t_palabra_proc  pal_entrada, char *pal_saida, char idioma);
void silabificar_e_acentuar(t_frase_separada * frase_separada, char idioma);
int legibilidad(t_palabra_proc ent, t_palabra_proc sal,char idioma);
int comprueba_palabra_esdrujula(char *palabra);
#endif
 

