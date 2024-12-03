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
 

#ifndef _MINOR_PHRASING

#define _MINOR_PHRASING

#include <stdio.h>
#include <vector>

using namespace std;

#include "grupos_acentuales.hpp"

#define TIPOS_GRUPO_FONICO_SEGUN_POSICION 4

#define GRUPO_FONICO_INICIAL 0
#define GRUPO_FONICO_MEDIO 1
#define GRUPO_FONICO_FINAL 2
#define GRUPO_FONICO_INICIAL_Y_FINAL 3

//! a = sentence type (declarative, interrogative, exclamatory or ellipsis), b = position within the sentence (initial, final, medial or initial-final)
#define calcula_indice_GF(a, b)  (((a)<<2) | (b))

#define NUMERO_GF_POSIBLES 16

//! Name of the file where the information of the sorted Grupo_fonico_frontera objects will be saved
#define NOMBRE_FICHERO_GF	"clusters_GF"
#define FICHERO_GF	"clusters_GF.bin"
#define NOMBRE_FICHERO_CLUSTERS_GF "catalogo_GF"

class Grupo_fonico_frontera;

/**
 * \brief	Struct that holds every Grupo_fonico_frontera belonging to the same group.
 * \remarks	The group is defined in terms of the sentence type and the position within the sentence
 * (see macro calcula_indice_GF)
 */
typedef struct {
	vector<Grupo_fonico_frontera> elementos;
} estructura_grupos_fonicos;

/**
 * \class	POS_frontera
 * \brief	Class that holds information about the accented word in an accent group.
 * In particular, the POS tag, the boundary type, the \Delta_{f0} and the slope factor.
 * \remarks	It's used in the minor phrasing algorithm.
 */
class POS_frontera {

	public:

    //! POS of the accented word in the accent group.
    unsigned char etiqueta_morfosintactica;

	//! Type of boundary right after the accented word.
    unsigned char tipo_pausa;

	//! Difference (in Hz) between the f0 at the nucleus of the syllable preceeding the accented syllable, and the value at the nucleus of the accented one.
	float salto_f0;

	//! Slope of the imaginary line joining the f0 value in the middle of the nucleus of the accented syllable and the value at the end of the AG.
	float factor_caida;


    //! Basic constructor
    POS_frontera() {};

    //! Copy constructor
    POS_frontera(const POS_frontera &original);
    
    //! Writes the content of the object in a text file.
	int escribe_datos_txt(FILE *salida);

    //! Reads the content of the object from a text file.
    int lee_datos_txt(FILE *entrada);

    //! Writes the content of the object in a binary file.
	int escribe_datos_bin(FILE *salida);

    //! Reads the content of the object from a binary file.
    int lee_datos_bin(FILE *entrada);

    //! Assignment operator
    POS_frontera &operator=(const POS_frontera &original);


};


/**
 * \class	Sintagma_frontera
 * \brief	Class that holds information about a syntagma, consisting of a sequence of
 * POS_frontera objects.
 * \remarks	It's used in the minor phrasing algorithm.
 */
class Sintagma_frontera {

	public:

    //! Type of syntagma:
    unsigned char tipo;

    //! Accented (1) or unaccented (0)
    unsigned char acento;
    
    //! Vector of POS_frontera objects.
    vector <POS_frontera> elementos;

 	//! Basic constructor
    Sintagma_frontera(){
    	elementos.clear();
    }

    //! Copy constructor
    Sintagma_frontera(const Sintagma_frontera &original);

    //! Writes the content of the object in a text file.
	int escribe_datos_txt(FILE *salida);

    //! Reads the content of the object from a text file.
    int lee_datos_txt(FILE *entrada);

    //! Writes the content of the object in a binary file.
	int escribe_datos_bin(FILE *salida);

    //! Reads the content of the object from a binary file.
    int lee_datos_bin(FILE *entrada);

    //! Assignment operator
	Sintagma_frontera &operator= (const Sintagma_frontera &original);

};


class Grupo_fonico_frontera {

	public:

    //! Unique identifier for the object.
    int identificador;

    //! Type of phonic group: declarative, interrogative, exclamatory or ellipsis.
    unsigned char tipo_grupo;

    //! First syntagma of the next phonic group.
	unsigned char siguiente_sintagma;

    //! Position within the sentence.
    char posicion;

    //! Vector of Sintagma_frontera objects.
    vector <Sintagma_frontera> elementos;

    //! Basic constructor
    Grupo_fonico_frontera() {
    	elementos.clear();
    }

    //! Copy constructor
    Grupo_fonico_frontera(const Grupo_fonico_frontera &original);

	//! Writes the content of the Grupo_fonico_frontera database in a file
	static int escribe_fichero_unico(FILE *fichero, estructura_grupos_fonicos *grupos_fonicos, int format);

	//! Reads the content of the Grupo_fonico_frontera database from a binary file
    static int lee_fichero_unico(FILE *fichero, estructura_grupos_fonicos *grupos_fonicos);

    //! Writes the content of the object in a text file.
	int escribe_datos_txt(FILE *salida);

    //! Reads the content of the object from a text file.
    int lee_datos_txt(FILE *entrada);

    //! Writes the content of the object in a binary file.
	int escribe_datos_bin(FILE *salida);

    //! Reads the content of the object from a binary file.
    int lee_datos_bin(FILE *entrada);

    //! Assignment operator
    Grupo_fonico_frontera &operator=(const Grupo_fonico_frontera &original);

};

/**
 * class Frase_frontera
 * \remarks	It's used in the minor phrasing algorithm.
 */
class Frase_frontera {

	public:

    //! Vector of Grupo_fonico_frontera objects.
    vector <Grupo_fonico_frontera> elementos;

    //! Basic constructor
    Frase_frontera() {
    	elementos.clear();
    }

    //! Copy constructor
	Frase_frontera(const Frase_frontera &original);

    //! Checks if a sequence of Grupo_fonico_frontera objects is valid for a target Frase_frontera object
    static int candidato_valido(const Frase_frontera &objetivo, const Frase_frontera *candidato);

    //! Copies the prosodic structure from a candidate Frase_frontera object to a sequence of Grupo_acentual_objetivo objects 
	static int establece_estructura_prosodica(Frase_frontera *frase_candidata, Frase_frontera &frase_objetivo, Grupo_acentual_objetivo *grupos_objetivo, int numero_grupos);

    //! Sets the default prosodic structure (so that the intonation unit selection algorithm decides by itself)
    static void establece_estructura_prosodica_por_defecto(Grupo_acentual_objetivo *grupos_objetivo, int numero_grupos);

    //! Writes the content of the object in a text file.
	int escribe_datos_txt(FILE *salida);

    //! Reads the content of the object from a text file.
    int lee_datos_txt(FILE *entrada);

    //! Writes the content of the object in a binary file.
	int escribe_datos_bin(FILE *salida);

    //! Reads the content of the object from a binary file.
    int lee_datos_bin(FILE *entrada);

    //! Assignment operator
    Frase_frontera &operator=(const Frase_frontera &original);

};


#endif
