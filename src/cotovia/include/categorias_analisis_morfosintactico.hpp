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
 
#ifndef _CATEGORIAS_ANALISIS_MORFOSINTACTICO_HPP
#define _CATEGORIAS_ANALISIS_MORFOSINTACTICO_HPP


//#define LONG_CLAVE_CLASE_AMBIGUEDAD 7

#ifdef _MODELO_POS_COMPLETO
#define MAX_NUM_CATEGORIAS 209 ///<  Número de categorías del modelo
#else
#define MAX_NUM_CATEGORIAS 50 ///<  Número de categorías del modelo
#endif

// (Por eso del género y número ambiguos)
//#define MAX_INDICE_CATEGORIAS_MODELO 54



/// Tipo enumerado con las categorías que se consideran
/// en el modelo del análisis morfosintáctico.

typedef enum {
	NULO, // 0
	ADV, // 1
	ADX_FEM_PLURAL, // 2
	ADX_FEM_SINGULAR, // 3
	ADX_MASC_PLURAL, // 4
	ADX_MASC_SINGULAR, // 5
	APERTURA_EXCLA, // 6
	APERTURA_INTERR, // 7
	ART_FEM_PLURAL, // 8
	ART_FEM_SINGULAR, // 9
	ART_MASC_PLURAL, // 10
	ART_MASC_SINGULAR, // 11
	COM, // 12
	CONX_COORD, // 13
	CONX_SUBORD, // 14
	DET_FEM_PLURAL, // 15
	DET_FEM_SINGULAR, // 16
	DET_MASC_PLURAL, // 17
	DET_MASC_SINGULAR, // 18
	DET_POSE_FEM_PLURAL, // 19
	DET_POSE_FEM_SINGULAR, // 20
	DET_POSE_MASC_PLURAL, // 21
	DET_POSE_MASC_SINGULAR, // 22
	EXCLAM, // 23
	INTERR, // 24
	INTERXE, // 25
	NOME_FEM_PLURAL, // 26
	NOME_FEM_SINGULAR, // 27
	NOME_MASC_PLURAL, // 28
	NOME_MASC_SINGULAR, // 29
	PECHE_EXCLA, // 30
	PECHE_INTERR, // 31
	PREPO, // 32
	PRON_FEM_PLURAL, // 33
	PRON_FEM_SINGULAR, // 34
	PRON_MASC_PLURAL, // 35
	PRON_MASC_SINGULAR, // 36
	PRON_POSE_FEM_PLURAL, // 37
	PRON_POSE_FEM_SINGULAR, // 38
	PRON_POSE_MASC_PLURAL, // 39
	PRON_POSE_MASC_SINGULAR, // 40
	PRON_PROC_FEM_PLURAL, // 41
	PRON_PROC_FEM_SINGULAR, // 42
	PRON_PROC_MASC_PLURAL, // 43
	PRON_PROC_MASC_SINGULAR, // 44
	PUNT, // 45
	RELAT, // 46
	VERBO_PLURAL, // 47
	VERBO_SINGULAR, // 48
	XERUND // 49

} Categorias_modelo_enum;

typedef struct {
	Categorias_modelo_enum cat;
	float prob;
} Categorias_modelo;

/*
	 typedef enum {NULO, //0
	 ADV, // 1
	 ADX_FEM_PLURAL, // 2
	 ADX_FEM_SINGULAR, // 3
	 ADX_MASC_PLURAL, // 4
	 ADX_MASC_SINGULAR, // 5
	 APERTURA_EXCLA, // 6
	 APERTURA_INTERR, // 7
	 COM, // 8
	 CONTR_CONXUN_ART_DET_FEM_PLURAL, // 9
	 CONTR_CONXUN_ART_DET_FEM_SINGULAR, // 10
	 CONTR_CONXUN_ART_DET_MASC_PLURAL, // 11
	 CONTR_CONXUN_ART_DET_MASC_SINGULAR, // 12
	 CONX_COORD, // 13
	 CONX_SUBORD, // 14
	 DET_FEM_PLURAL, // 15
	 DET_FEM_SINGULAR, // 16
	 DET_MASC_PLURAL, // 17
	 DET_MASC_SINGULAR, // 18
	 DET_POSE_FEM_PLURAL, // 19
	 DET_POSE_FEM_SINGULAR, // 20
	 DET_POSE_MASC_PLURAL, // 21
	 DET_POSE_MASC_SINGULAR, // 22
	 EXCLAM, // 23
	 INTERR, // 24
	 INTERXE, // 25
	 NOME_FEM_PLURAL, // 26
	 NOME_FEM_SINGULAR, // 27
	 NOME_MASC_PLURAL, // 28
	 NOME_MASC_SINGULAR, // 29
	 PECHE_EXCLA, // 30
	 PECHE_INTERR, // 31
	 PREPO, // 32
	 PRON_FEM_PLURAL, // 33
	 PRON_FEM_SINGULAR, // 34
	 PRON_MASC_PLURAL, // 35
	 PRON_MASC_SINGULAR, // 36
	 PRON_POSE_FEM_PLURAL, // 37
	 PRON_POSE_FEM_SINGULAR, // 38
	 PRON_POSE_MASC_PLURAL, // 39
	 PRON_POSE_MASC_SINGULAR, // 40
	 PRON_PROC_FEM_PLURAL, // 41
	 PRON_PROC_FEM_SINGULAR, // 42
	 PRON_PROC_MASC_PLURAL, // 43
	 PRON_PROC_MASC_SINGULAR, // 44
	 PUNT, // 45
	 RELAT, // 46
	 VERBO_PLURAL, // 47
	 VERBO_SINGULAR, // 48
	 XERUND, // 49
	 } Categorias_modelo;
	 */



/// Tipo enumerado con el conjunto de categorías que se consideran para las 
/// clases de ambigüedad. Básicamente, se eliminan las variaciones de género 
/// y número, y se introducen otras nuevas que contemplan posibilidades como 
/// la sustantivación de adjetivos y verbos.

typedef enum {
	NULA, // 0
	ADV_GEN, // 1
	ADX_GEN, // 2
	ADX_SUSTANTIVADO_GEN, //3
	APERTURA_EXCLA_GEN, //4
	APERTURA_INTERR_GEN, // 5
	ART_GEN, // 6
	COMA_GEN, // 7
	CONX_COORD_GEN, // 8
	CONX_SUBORD_GEN, // 9
	DET_GEN, //  10
	DET_POSESIVO_GEN, // 11
	EXCLAM_GEN, // 12
	INTERR_GEN, // 13
	INTERXE_GEN, // 14
	NOM_GEN, //15
	PECHE_EXCLA_GEN, // 16
	PECHE_INTERR_GEN, // 17
	PREPO_GEN, // 18
	PRON_GEN, // 19
	PRON_POSESIVO_GEN, // 20
	PRON_PROC_GEN, // 21
	PUNTO_GEN, // 22
	RELAT_GEN, // 23
	VERB_GEN, //  24
	VERB_SUSTANTIVADO_GEN, //25
	XERUNDIO_GEN // 26
} Categorias_genericas ;

#ifdef _MODELO_POS_COMPLETO
#define MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS 209 //27 ///< Número de categorías genéricas del modelo,
                                                  ///< sin género ni número.
#else
#define MAX_NUM_CATEGORIAS_MODELO_CONSIDERADAS 27
#endif
                                                  
/** \struct  estructura_categorias
 *
 * Estructura con el conjunto de funciones que puede cumplir una palabra o 
 * conjunto de palabras en el contexto de la frase actual.
 * 
 */
                                                     
typedef struct {
	unsigned char categoria[MAX_NUM_CATEGORIAS]; ///< Cadena con las categorías del tipo Categorias_modelo
                                               ///< que pueden ser asignadas a la palabra o conjunto de 
                                               ///< palabras a las que se refiere la estructura.
                                               
	float probabilidad[MAX_NUM_CATEGORIAS]; ///< Probabilidad de que la palabra o conjunto de palabras
                                          ///< tenga esa categoría.
                                          
	int numero_categorias;                  ///< Número de elementos de la cadena categoria.
} estructura_categorias;


#endif


