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
 

#ifndef _MATRIZ_HH

#define _MATRIZ_HH


typedef struct {
  unsigned char categoria;
  float peso;
} estructura_pesos;



/** \class Mariz
 *
 * Esta clase se encarga de almacenar los caminos de coste m�nimo del
 * algoritmo de Viterbi, tanto en la selecci�n de unidades ac�sticas como
 * en la de unidades pros�dicas. 
 * 
 */  

#ifdef _WIN32
class __declspec(dllexport) Matriz {
#else
  class Matriz {
#endif

    int filas;    ///< N�mero actual de filas de la matriz. Puede variar  de iteraci�n en iteraci�n.
      
    int columnas; ///< N�mero de columnas de la matriz. Se fija su valor al inicio, por simplicidad.
    ///< Cada columna se refiere a una iteraci�n del algoritmo, por lo que su n�mero 
    ///< debe coincidir con el n�mero de semifonemas de la frase.
                    
    int columna_actual; ///< Columna sobre la que se est� trabajando.
      
    estructura_matriz **matriz; ///< Cadena de punteros a los inicios de cada fila. Cada elemento de
    ///< la fila es una estructura de tipo estructura_matriz, en la que se
    ///< codifica la informaci�n de una unidad, ya sea ac�stica o pros�dica.
                                  
    estructura_matriz **antigua_matriz; ///< Matriz de caminos �ptimos del paso anterior.
      
    int antigua_filas;                  ///< N�mero de filas de antigua_matriz;

  public:

    /// Constructor de la clase.
      
    Matriz();
      
    /// Destructor de la clase.
    ~Matriz();

    /// libera la memoria reservada anteriormente y reserva la necesaria para una 
    /// nueva ejecuci�n del algoritmo. Como ya se coment� anteriormente, el n�mero de 
    /// columnas permanece constante en todas las iteraciones, mientras que el de filas 
    /// puede variar.

    int inicializa_matriz(int numero_filas, int numero_columnas);
      
    /// Reserva memoria para el siguiente paso del algoritmo de Viterbi, y salva el 
    /// contenido de la matriz del paso anterior para construir la nueva secuencia de 
    /// caminos seg�n los datos de la iteraci�n actual.

    int reinicia_ciclo(int numero_filas);

    /// Actualiza el n�mero de filas, para esos casos en los que hay una poda posterior
    /// a la asignaci�n de memoria.

    int actualiza_numero_de_filas(int nuevo_numero_filas);

      
    /// En sus diferentes versiones, concatena una unidad al camino �ptimo cuya fila se 
    /// indica como par�metro.

    int introduce_elemento(int pos_fila, Vector_semifonema_candidato *unidad, int posicion,
			   char izq_der);
                             
    /// Variante de la funci�n introduce_elemento, que actualiza toda una columna de la matriz en 
    /// lugar de ir elemento por elemento.

    int introduce_conjunto_elementos(Estructura_C_i *C_i, int numero_vectores, char izq_der);
      
    /// En sus diferentes versiones, concatena una unidad al camino �ptimo cuya fila se 
    /// indica como par�metro.

    int introduce_elemento(int pos_fila, Grupo_acentual_candidato *unidad, int posicion);
      
    /// Variante de la funci�n introduce_elemento, que actualiza toda una columna de la matriz en 
    /// lugar de ir elemento por elemento.

    int introduce_conjunto_elementos(Estructura_C_i_acentual *C_i, int numero_grupos);

    /// Devuelve la fila que se indica como par�metro.
      
    estructura_matriz *saca_fila(int fila, int *numero_unidades, int *numero_real_caminos);
      
    /// Actualiza la columna sobre la que se est� trabajando.

    void avanza_columna();
      
    /// Libera la memoria reservada por el objeto.

    void libera_memoria();
  };

  /** \class Matriz_categorias
   *
   * Se trata de una variante de la clase anterior, empleada en la selecci�n del 
   * conjunto de categor�as morfofuncionales m�s probables para una frase de 
   * entrada. No es exactamente igual, como consecuencia de que el equivalente al 
   * coste de concatenaci�n de la selecci�n de unidades ac�sticas s�lo involucra a 
   * dos unidades consecutivas, mientras que en este caso el hecho de tratar con 
   * pentagramas hace que esa influencia se extienda a m�s unidades.
   *
   */ 
 
  class Matriz_categorias {

    int filas;                           ///<  An�loga a la de la clase Matriz.
      
    int columnas;                        ///<  An�loga a la de la clase Matriz.
      
    int fila_actual;                     ///<  Fila sobre la que se est� trabajando.
      
    int columna_actual;                  ///< Columna sobre la que se est� trabajando.
      
    int n_grama;                         ///< Tama�o, en n�mero de categor�as, del m�ximo n_grama.
      
    estructura_pesos **matriz;           ///< An�loga a la de la clase Matriz pero con estructuras �
    ///< de tipo estructura_pesos.
                                           
    estructura_pesos **antigua_matriz;   ///< An�loga a la de la clase Matriz pero con estructuras �
    ///< de tipo estructura_pesos.
                                           
    short int *caminos_poda;             ///< Cadena intermedia empleada para marcar aquellos caminos que se deben podar. 

    int tamano_caminos_poda;             ///< N�mero de posiciones de memoria reservadas para la cadena anterior. 

    unsigned char *numero_categorias;    ///< Matriz en la que se almacena el n�mero de posibles categor�as de cada
    ///< palabra (o conjunto de palabras) de la frase de entrada.

    int antigua_filas;                   ///< N�mero de filas de la variable antigua_matriz;


  public:
      
    /// Constructor de la clase Matriz_categorias   

    Matriz_categorias();
      
    /// Inicializa variables y asigna memoria din�mica.      

    int inicializa_matriz(int numero_filas, int numero_columnas, int tamano_n_grama);
      
    /// Introduce nueva categor�a en la posici�n dada por pos_fila y columna_actual.

    int introduce_elemento(int pos_fila, unsigned char categoria, int posicion, float peso);
      
    /// Poda los caminos que no pueden formar parte de la ruta �ptima. 

    int poda_matriz();
      
    /// Indica al usuario si el camino al que se refiere puede formar parte de
    /// la ruta �ptima.   

    int camino_podable(int posicion);
      
    /// Obtiene la secuencia de categor�as de mayor probabilidad.
      
    int saca_fila(int fila, int tamano_maximo_n_grama, unsigned char *secuencia);
      
    /// Obtiene la secuencia de categor�as de mayor probabilidad.  

    int saca_fila(int fila, int *numero_unidades, unsigned char **secuencia);
      
    /// Reserva la memoria necesaria para almacenar los caminos �ptimos de 
    ///cada recursi�n del algoritmo de Viterbi, y liberar la que sobre. 

    int reinicia_ciclo(int numero_filas, unsigned char categorias);
      
    /// Escribe en un fichero el contenido actual de la matriz de secuencias de 
    /// categor�as �ptimas.                               

    void escribe_contenido_matriz(FILE *fichero);
      
    /// Incrementa en una unidad la columna sobre la que se est� trabajando. 

    void avanza_columna();
      
    /// Libera la memoria reservada para las matrices.

    void libera_memoria();
  };


  void crea_parametros_estructura_matriz(char izq_der, char acento, char frase,
					 char posicion, Vector_semifonema_candidato *vector,
					 estructura_matriz *estructura);


  void devuelve_parametros_estructura_matriz(estructura_matriz estructura,
					     char *izq_der, char *acento, char *frase,
					     char *posicion, char *validez, char **fonema_1,
					     char **fonema_2, int *contador_posicion);



#endif
