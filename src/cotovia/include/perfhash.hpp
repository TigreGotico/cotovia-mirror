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
 
#ifndef PERFHASH_H
#define PERFHASH_H

/* perfhash.h */

#define SIN_PRODUCTORIO

//typedef unsigned int tipo_clave;
typedef unsigned char tipo_clave;

typedef struct perfhash {
  unsigned int dim;       //dim de las claves: bigramas=2, trigramas=3
  unsigned int sep;       //distancia en bytes entre clave y clave en el array
  unsigned int m;         //num de claves
  unsigned int n;         //valor optimo tiempo calculo*espacio = 2m
#ifndef _x86_64
	int dummy_1;
#endif
  unsigned char *claves;  // [0..m - 1][0..dim-1]
#ifndef _x86_64
	int dummy_2;
#endif
  unsigned int *k1;  // [0..dim-1]
#ifndef _x86_64
	int dummy_3;
#endif
  unsigned int *k2;  // [0..dim-1]
#ifndef _x86_64
	int dummy_4;
#endif
  unsigned int *g;   // [0..n - 1]
} Perfhash;


/* Prototipos */

struct perfhash *ph_crea_hash(
			      unsigned int dimension,
			      unsigned char *claves,
			      unsigned int sep_claves,
			      unsigned int m,
			      unsigned int n,
			      unsigned int tipo_phash);
void ph_destruye_hash(struct perfhash *ph);

//Tipos de perfhash
#define STANDARD 0
#define BITRIGRAMAS 1

int ph_usa_hash(struct perfhash *ph, tipo_clave *clave);

//devuelve la posicion 0..m-1 si existe. Si no, devuelve -1.

int ph_prueba_integridad(struct perfhash *ph);
//devuelve 0 sii el hash está bien construido.

void ph_escribe_hash(Perfhash *ph, FILE *fichero);

int ph_lee_hash(Perfhash **ph, FILE *fichero);
static void aleatorizar_tablas(void);
static void construir_hash(void);
static void reinicializar_grafo(void);
static int hay_ciclos(void);
static int probar_integridad(void);
#endif












