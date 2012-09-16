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
 

#ifndef _CACHE_HH

#define _CACHE_HH

// Fichero de texto en el que se encuentra almacenada la información de costes de concatenación
// más típicos.
#define FICHERO_CACHE "cache.txt"

typedef struct {
        int id_secundario; // Identificador de la segunda unidad.
        float coste;       // Coste de concatenación.
} nodo_lista;

typedef struct {
        int id_principal;     // Identificador numérico de la primera unidad.
        int numero_elementos; // Número de elementos con los que ya está calculado el Cc.
        nodo_lista *lista;    // Cadena indexada de nodo_lista.
} nodo_cache;

class Cache {

      nodo_cache *tabla_cache; // cadena indexada en la que se encuentra la información.
      int numero_nodos;        // tamaño, en número de nodos, de la memoria.

      float busqueda_binaria(nodo_lista *nodo_inicial, int elementos, int identificador);

      public:

      int inicia_cache(char *nombre_fichero);

      float comprueba_existencia(int identificador1, int identificador2);

};


#endif

