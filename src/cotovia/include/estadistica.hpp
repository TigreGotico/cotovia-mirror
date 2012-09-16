/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ram�n Pi�eiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigaci�n en Humanidades,
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
 
 
 /** \file estadistica.hpp
  * Uno de los factores m�s determinantes en la calidad final de un sistema de
  * conversi�n texto-voz es el de un dise�o adecuado del Corpus, en el cual 
  * estar�n recogidas todas las unidades a partir de las que se generar� la voz
  * sint�tica. Dado que tener una base de datos demasiado grande ser�a perjudicial
  * tanto en t�rminos de memoria como de carga computacional para seleccionar la
  * secuencia de unidades que proporcione un coste de distorsi�n m�nimo respecto
  * a lo que buscamos, se hace preciso realizar un estudio de la frecuencia de 
  * aparici�n de cada unidad en el idioma en cuesti�n, de tal forma que se pueda
  * dise�ar un Corpus que tenga un n�mero de copias elevado de las unidades m�s 
  * frecuentes, y al menos una peque�a cantidad de las menos comunes.    
  * 
  * Precisamente �sta es la funci�n del m�dulo de estad�sticas: extraer el 
  * n�mero de ocurrencias de cada unidad de un texto de entrada, de tal forma 
  * que se pueda emplear esta informaci�n para dise�ar correctamente el Corpus 
  * al que se acudir� en el proceso de s�ntesis.
  */      

#ifndef _ESTADISTICA_HH

#include "descriptor.hpp"

#define _ESTADISTICA_HH

/* \item \verb@TAMANO_CLAVE@: n�mero de octetos reservados para almacenar
  en memoria cada unidad.
\item \verb@INDICE_ACENTO@: posici�n del indicador de acento en la
  clave.
\item \verb@INDICE_PROPOSICION@: posici�n del indicador de tipo de
  proposici�n en la clave.
\item \verb@INDICE_CONTEXTO@: �ndice en la clave del tipo de unidad
  seg�n su posici�n.
\item \verb@TAMANO_TABLA@: tama�o de la estructura \verb@t_tabla@.

*/

#define TAMANO_CLAVE TAMANO_UNIDAD + 3          ///< N�mero de octetos reservados para almacenar
                                                ///< en memoria cada unidad.
                                                
#define INDICE_ACENTO TAMANO_UNIDAD             ///< Posici�n del indicador de acento en la clave.

#define INDICE_PROPOSICION INDICE_ACENTO + 1    ///< Posici�n del indicador de tipo de proposici�n en la clave.

#define INDICE_CONTEXTO INDICE_PROPOSICION + 1  ///< �ndice en la clave del tipo de unidad seg�n su
                                                ///< posici�n.
                                                
#define TAMANO_TABLA sizeof(t_tabla)            ///< Tama�o de la estructura t_tabla.


/** \struct t_tabla
 *
 * Estructura en la que se almacena el nombre de la unidad y su n�mero de 
 * ocurrencias.
 * 
 */  

typedef struct {
       char unidad[TAMANO_CLAVE];
       int contador;
} t_tabla;

/** \struct unidad_extendida
 * Estructura similar a la anterior, en la que se almacenan las unidades de la 
 * frase actual que se desea que formen parte del Corpus, junto con su car�cter
 * t�nico o �tono, el tipo de proposici�n al que pertenece y la posici�n de la
 * unidad en la frase
 * 
 **/   

typedef struct {
       char unidad[TAMANO_UNIDAD];
       unsigned char acento;
       unsigned char proposicion;
       unsigned char contexto;
} unidad_extendida;


/**  \class Estadistica 
 *
 * La clase Estadistica es la encargada de almacenar la informaci�n relativa al 
 * nombre de las unidades y a su n�mero de ocurrencias. La clase simplemente 
 * comprueba si la unidad que se le entrega ya est� contenida en su memoria. Si lo 
 * est�, incrementa su contador, y si no, crea espacio para ella. La clase no hace 
 * ning�n tipo de comprobaci�n acerca del formato de las nuevas unidades de 
 * entrada, por lo que es responsabilidad del usuario llevar este control.
 */ 

   
class Estadistica {

      t_tabla *tabla;  ///< Puntero a la tabla de estructuras en la que se van 
                       ///< almacenando las unidades junto con su n�mero de ocurrencias.
                       
      int tamano; ///< N�mero de elementos actualmente guardados en la
                           ///< tabla de tipo t_tabla.
                           
      /// En sus diferentes versiones, se encarga de buscar una unidad en su memoria, y 
      /// aumentar su contador de apariciones en la cantidad indicada o informar de su 
      /// valor, seg�n el caso.

      int busca_unidad(char *unidad, int longitud);
      
      /// En sus diferentes versiones, se encarga de buscar una unidad en su memoria, 
      /// aumentar su contador de apariciones en la cantidad indicada o informar de 
      /// valor, seg�n el caso.

      int busca_unidad(char *unidad, int longitud, int ocurrencias);
      
      /// En sus diferentes versiones, se encarga de buscar una unidad en su memoria, 
      /// aumentar su contador de apariciones en la cantidad indicada o informar de 
      /// valor, seg�n el caso.

      int busca_unidad(char *unidad, int longitud, int *ocurrencias);
      
      /// Funci�n similar a la anterior, pero encargada de buscar una unidad en su tabla 
      /// y decrementar su n�mero de ocurrencias en el valor indicado; Se emplea en el 
      /// modo _GENERA_CORPUS, cuando un objeto de esta clase almacena el n�mero y 
      /// tipo de unidades que se desean encontrar para constituir el Corpus de voz.

      t_tabla *busca_unidad_y_decrementa(char *unidad, int longitud, int resta);

      public:

      /// Constructor de la clase.
      
      Estadistica();
      
  
      /// En sus diferentes versiones, actualiza el contenido de la tabla 
      /// t_tabla. En el caso de las unidades ya inclu�das se incrementa su 
      /// contador, mientras que para las nuevas se crea una posici�n en la 
      /// tabla.

      int anhade_dato(char *unidad, char acento, char contexto, char proposicion);
      
      /// En sus diferentes versiones, actualiza el contenido de la tabla 
      /// t_tabla. En el caso de las unidades ya inclu�das se incrementa su 
      /// contador, mientras que para las nuevas se crea una posici�n en la 
      /// tabla.

      int anhade_dato(char *unidad);
      
      /// Escribe en un fichero binario el contenido de la tabla de unidades y n�mero de 
      /// ocurrencias.

      int escribe_datos_en_fichero(FILE *fichero);
      
      /// Carga en la variable tabla el contenido de un fichero en binario. Permite 
      /// guardar el resultado de diversas ejecuciones del programa sobre diferentes 
      /// ficheros de texto.

      int lee_tabla_de_fichero_bin(FILE *fichero);

      /// Similar a la funci�n lee_tabla_de_fichero_bin, pero lee el contenido de un 
      /// fichero de texto; Esta funci�n s�lo se puede emplear con el tipo de clave 
      /// definido en las estructuras de datos, es decir, fonema + acento + contexto + 
      /// proposici�n.

      int lee_tabla_de_fichero_txt(FILE *fichero);
      
      /// Carga en memoria la tabla de unidades a partir de la cual se desea generar el 
      /// Corpus de voz.

      int lee_tabla_de_fichero_corpus(FILE *fichero);
      
      /// Escribe, de forma legible por el usuario el resultado de la ejecuci�n del 
      /// programa sobre un fichero de texto. Tiene la misma limitaci�n que la funci�n 
      /// lee_tabla_de_fichero_txt.

      void escribe_estadistica(FILE *fichero);
      
      /// Funci�n similar a escribe_estadistica, pero que no hace ning�n tipo de suposici�n 
      /// sobre el formato de la clave; Simplemente, se limita a escribir en el fichero de 
      /// salida el contenido de cada clave y su n�mero de ocurrencias.

      void escribe_estadistica_generico(FILE *fichero);
      
      /// Lee un fichero creado con escribe_estadistica_generico

      int lee_estadistica_generico(FILE *fichero);
      
      /// Devuelve el n�mero de unidades almacenadas actualmente en el objeto.
      
      int devuelve_tamano();
      
      /// Sobrecarga del operador += que permite acumular las estad�sticas de dos objetos 
      /// de esta clase; Se suman las ocurrencias de las unidades que existen en ambos 
      /// objetos y se crea espacio para aquellas que no existen en el objeto destino; 
      /// Precisamente, el hecho de que pueda haber errores al asignar memoria y no poder 
      /// avisar de ello al usuario, supone que no sea una buena elecci�n.

      Estadistica operator += (Estadistica sumando);
      
      /// Funci�n que soluciona el problema anterior. Cumple el mismo cometido que la 
      /// sobrecarga del operador += , pero avisa al usuario cuando se produce alg�n 
      /// error interno.

      int suma_estadisticas(Estadistica sumando);
      
      /// Crea una cadena con las unidades de la frase actual que se encuentran 
      /// tambi�n en la tabla t_tabla del objeto.

      int existe_lista(Estadistica *lista_unidades, unidad_extendida **lista, int *tamano_lista);
      
      /// Libera la memoria reservada para la tabla de unidades.

      void libera_memoria();

};

#endif
