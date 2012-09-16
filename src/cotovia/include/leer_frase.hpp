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
 
 
#ifndef LEER_FRASE_HPP
    #define LEER_FRASE_HPP

#define LONX_BUFER_TEXTO              8192  //7000


#include "interfaz_ficheros.hpp"
/**
 * \class Leer_frase
 * 
 * \author fmendez
 * 
 * \brief Neste arquivo encóntranse aquelas funcións relacionadas ca obtención  dunha
 *  frase do texto de entrada.   
 *
 *  As clases de frases consideradas son:
 *  Enunciativas : Empezan por maiuscula e acaban nun punto.
 *  Interrogativas/exclamativas/de dialogo : que estan delimitadas  por  ? ¡ ou
 *  guions simples ou dobles. As frases de dialogo ademais  de empezar por
 *  guions deben seguir por maiuscula e acabar en puntos  ou guions .   
 *  Titulos: Son aquelas que Empezan por maiuscula ou estan en maiuscula  e acaban nun
 *  salto de linea e ademais empeza outra frase das enumeradas  arriba.   
 *  Para a extraccion tamen se eliminan os formatos de cada frase como tabuladores,
 *  saltos de linea que parten palabras ou frases etc. de xeito que a frase
 *  extraida solo conten caracteres con informacion pero non caracteres de
 *  formato.
 */



#ifdef _WIN32
class __declspec(dllexport) Leer_frase : public Interfaz {
#else
class Leer_frase : public Interfaz {
#endif
	private:
		FILE * fent; ///< Fichero de entrada
		char * texto;  ///< Buffer de texto de entrada.
		char * recorre_texto; ///< Puntero para recorrer el texto de entrada.
		char * frase_extraida;  ///< Frase extraída de la entrada.
		t_tipo_frase * tipo_frase; ///< Tipo de frase extraída.
		t_opciones * opciones; ///< Opciones globales de cotovía.
		unsigned int lonx_arqui;  ///< Longitud del archivo de entrada.
		bool arquivo_totalmente_leido; ///< Booleano que indica el final del archivo de entrada.


		//inline int caracter_de_formato(unsigned char a); 

		void obten_pal_anterior( char *c,char pal[20],char *ini); ///< Obtiene la palabra anterior a una dada.
		void obten_pal_seguinte( char *c, char pal[20]); ///< Obtiene la palabra siguiente a una dada.
		int abrev_trat_persoal( char *p); ///< Comprueba si se trata de una abreviatura de trato personal
		char* limpia_formato( char *adiante); 
		int frase_partida_por_salto_de_linea(void); ///< Comprueba si se trata de una frase partida por un salto de linea 
		int inicio_de_frase( char *recibido); 
		int comprobar_fin_frase_estandar( char *anda);
		int inicial_de_nome_propio(char * pala);
		int punto_entre_letras( char *aux); ///< Comprueba
		int punto_entre_numeros( char *aux); ///< Comprueba
		short int comprobar_saltolinea( char *aux); ///< Comprueba si un cambio de linea implica final de frase.
		int secuencia_de_espacios_mais_maiuscula( char *axu);
		int comprobar_punto( char *aux); ///< Comprueba si un punto es fin de frase.
		short int palabra_partida_por_salto_de_linea( char *recorre_texto); ///< Comprueba si una palabra aparece partida por un cambio de linea
		short int comprobar_se_carac_antes_salto_linea_permiten_fin_de_frase( char *axu);
		char *cargar_bufer_de_arquivo( char *pos_actual_texto); ///< Lee un trozo del fichero de entrada al buffer.
		void filtrar_caracteres ( char * inicio, char * fin); ///< Filtra caracteres extraños.

	public:

		int inicializa(t_tipo_frase * tipo, t_opciones *opc); ///< Inicializa la clase.
		int leer_frase(char * entrada, char * frase, bool &ultima); ///< Extrae una nueva frase del texto de entrada.
		void finaliza(); ///< Finaliza. 

		Leer_frase(); ///< Constructor de la clase Leer_frase.
		~Leer_frase(); ///< Destructor de la clase Leer_frase.
};
#endif
 

