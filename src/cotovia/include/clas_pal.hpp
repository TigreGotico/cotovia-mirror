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
 
 
#ifndef CLAS_PAL_H
   #define CLAS_PAL_H
unsigned char palabra_estandar( char *palabra);
inline short int forma_de_abreviatura(char *pal);
short int forma_de_data( char* n);
int numeros_separados_por_guions( char *c);
short int forma_de_partitivo( char* n);
//int forma_de_horas( char* n);
int despois_do_ultimo_punto_hai_tres_cifras( char *c);
unsigned char cadea_numerica( char *inicial);
int todo_maiusculas( char  *pal);
short int letra_romana( char l);
short int numero_romano( char *pal);
int forma_de_inicial(char  *pal);
unsigned char cadena_de_signos(char * pal);
void clasificar_palabras(Token * token);
#define forma_de_horas(n) ( dixito(*n) && (( dixito(*(n+1)) && *(n+2)==':' && dixito(*(n+3)) && dixito(*(n+4)) && !dixito(*(n+5))) || ( *(n+1)==':' && dixito(*(n+2)) && dixito(*(n+3)) && !dixito(*(n+4)))) )
#endif
 

