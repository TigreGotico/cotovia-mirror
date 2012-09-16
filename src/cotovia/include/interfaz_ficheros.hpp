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
 


/**
* \file
* \author giglesia
* \remark define las funciones externas de la unidad interfaz_ficheros.cpp
*/



#ifndef INTERFAZFICHEROS
#define INTERFAZFICHEROS


#define IF_NUM_IDIOMAS 10
#define IF_MAX_DICTIO 11

#define IF_DIC_N 1   //Nomes
#define IF_DIC_A 2   //Adxectivos
#define IF_DIC_F 3    //Palabras función
#define IF_DIC_P 4   //perífrase
#define IF_DIC_V 5    //verbos
#define IF_DIC_D 6   //desinencias
#define IF_DIC_L 7   //locucións
#define IF_DIC_ABR 8  //abreviaturas
#define IF_DIC_ACR 9  //acrónimos
#define IF_DIC_DER 10  //palabras derivadas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
//Nuevos include...
#include "letras.hpp"
#include "sil_acen.hpp"


//#include "variables_globales.hpp"
#include "utilidades.hpp"
#include "gbm_abreviaturas.hpp"
#include "gbm_sustantivos.hpp"
//#include "gbm_palabras_funcion.hpp"
#include "verbos.hpp"
#include "gbm_locuciones.hpp"
#include "gbm_perifrase.hpp"
#include "gbm_transcripcion.hpp"
#include "gbm_derivativos.hpp"


// O nº. codigos desinencias: codigos des)*2 (modelos conx)+1(para NULL) = NUM_MODEL_DES+1

class Lexicon {

private:

  static int instancias;
  static gbm *dictio[IF_NUM_IDIOMAS][IF_MAX_DICTIO];
//  int NU_RAICES_DE_VERBOS[IF_NUM_IDIOMAS];
//  int NU_DESINENCIAS[IF_NUM_IDIOMAS];
  t_raices_de_verbos *raices_de_verbos;
  t_desinencias *desinencias;
  char idioma;
  int borraListas(void);
  Lexicon();
  ~Lexicon();
  static Lexicon *lex;
	gbm ** dic;
public:

  static Lexicon *getLexiconSingleton();
  static void dispose();

  char *existeEnAcronimos(char *pal, char idioma);
  char *existeEnAbreviaturas(char *pal,char genero, char idioma);
  char *existeEnAbreviaturas(const char *pal, char idioma);
 

//int devuelveTipoAbreviaturaEncontrada(void);
 
  char *devuelveExtensionPlural(char idioma);

  int interfaz(t_opciones opciones, unsigned char id);
  int inicia_diccionario(t_opciones opciones, unsigned char id);
 
  int busca_dic_pal_funcion(char *palabra,t_frase_separada *rec, char idioma);
  int busca_dic_nomes(char *palabra,t_frase_separada *rec, char idioma);
  int busca_dic_adxe(char *palabra,t_frase_separada *rec, char idioma);
  t_locucion *refinaBusquedaLoc(char *patron, char idioma);
  int busca_perifrase(t_frase_separada * item, char idioma);
  int buscarConxunciones(t_frase_separada *frase_separada, char idioma);
  int busca(char *palabra, char diccionario=IF_DIC_DER);
  int buscaDerivativos(char *palabra,t_frase_separada *f,char idioma);  

//ZA. provisional
int analizar_verbos(char *palabra,unsigned char* tempos_verdadeiros,t_infinitivo* inf, char idioma);

};

class
#ifdef _WIN32
__declspec(dllexport) 
#endif
Interfaz {

protected:
  char idioma;
  Lexicon *diccionarios;
public:

  Interfaz();
  ~Interfaz();
};
#endif


