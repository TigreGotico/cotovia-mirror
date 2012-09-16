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
 

#ifndef _VITERBI_HH

#define _VITERBI_HH

#include <set>
using namespace std;

//#define _DEPURA_VITERBI

//#define _DEPURA_CREA_VECTOR_COSTE
//#define _DEPURA_CREA_VECTOR_C_I

//#define _COSTE_F0_RELATIVO

#define _MODIFICACION_F0_SOLO_ZONA_ESTACIONARIA ///< Si est� definido, no se hace modificaci�n de f0 en la zona de transici�n entre fonemas (siempre y cuando no haya una discontinuidad mayor que FREC_UMBRAL entre las unidades)

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION

#define TAMANO_BLOQUE_CACHE     5000    ///< Se emplea en el modo _CALCULA_FRECUENCIA_COSTES_CONCATENACION
                                        ///< y es el tama�o de cada bloque de memoria que se reserva
                                        ///< de golpe.

#define NUMERO_SUBGRUPOS        10000  ///< N�mero de bandas en las que dividimos las unidades seg�n su
                                       ///< identificador. V.gr: N = 3: {0-20000, 20000-40000, 40000-}


/** \struct clave_parametros

    Clave en la que se almacena la informaci�n de los clusteres de unidades de los cuales se computa la
    frecuencia de c�lculo de sus costes de concatenaci�n.
*/

typedef struct {
  unsigned int clave;  ///< entero en el que se almacena la informaci�n de
  ///< los fonemas que forman el semifonema, si �ste es izquierdo o
  ///< derecho, y los datos relativos al tipo de frase, el acento y la
  ///< posici�n.

  int parametros;  ///< entero en el que se almacena informaci�n relativa a la procedencia
  ///< de la unidad (si proviene de un difonema), y del cluster que se selecciona.
} clave_parametros;


/** \struct estructura_concatenacion

    Nodo de la lista enlazada en la que se almacenan los datos de la frecuencia de c�lculo
    del coste de concatenaci�n entre unidades.
*/

typedef struct estructura_concatenacion {
  clave_parametros clave_compleja;  ///< estructura del tipo clave_parametros
  int numero_ocurrencias;           ///< n�mero de veces que se ha calculado ese coste de concatenaci�n.
  struct estructura_concatenacion *siguiente;  ///< puntero al siguiente nodo de la lista.
} estructura_concatenacion;


/*
  typedef struct cache_concatenacion {
  int identificador_1;
  int identificador_2;
  int numero_ocurrencias;
  //    float coste_concatenacion;
  struct cache_concatenacion *siguiente;
  } cache_concatenacion;
*/

#endif


#define NUMERO_MAXIMO_PESOS 15

#define GRUPOS_FONEMAS_SEGUN_PESOS 8  ///< Un poco cr�ptico: Se refiere al n�mero de grupos en los
                                      ///< que dividimos los al�fonos para realizar la regresi�n lineal
                                      ///< que aproxima la distancia espectral; en concreto: Silencio,
                                      ///< vocal, fric_sor, oclu_sor, oclu_son, lateral, nasal y vibrante.



/// Los valores de C_PODA_C_i y C_PODA_UNIDAD se calcularon empleando el script entrena_costes_poda.pl, sobre 500 frases y para los corpus de Freire y Roc�o.

#define C_PODA_C_i 14       // 9 => En un 0.01% de las ocasiones se perd�a la mejor unidad (para el corpus de Freire. Para Roc�o ser�a un 0.00%)
#define C_PODA_UNIDAD 20    // En un 0.087% de las ocasiones se perd�a la mejor unidad (para el corpus de Freire. Para Roc�o, 0.011%)
#define N_PODA_UNIDAD 125 // 200 // 500
#define MIN_NUMERO_PODA_VALOR_C_t 15 // M�nimo n�mero a partir del cual se empieza a aplicar la poda por valor para el vector C_t
#define MIN_NUMERO_PODA_VALOR_C_i 100 // M�nimo n�mero a partir del cual se empieza a aplicar la poda por valor para el vector C_i
#define N_PODA_Ci 1000 // 5000 // 1200

#define N_PODA_PARALELO 50 // N�mero de unidades m�nimo en el vector C_i para que haya poda de contornos al pasar de una iteraci�n a otra.

#define C_PODA_PARALELO 30.0 // Valor empleado para comprobar si se poda un contorno en el c�lculo de contornos en paralelo.

#define COEFICIENTE_INTELIGIBILIDAD 0.7F ///< Dentro de la funci�n de coste de objetivo, la
                                         ///< importancia que se le da a la parte relativa a la
                                         ///< inteligibilidad.

#define COEFICIENTE_PROSODIA 0.3F        ///< Importancia que se le da a la parte relativa a la prosodia.

#define PESO_COSTE_CONCATENACION   0.7F  ///< Importancia relativa del coste de concatenaci�n
// frente al de objetivo.

#define PESO_COSTE_OBJETIVO 0.3F   ///< Importancia relativa del coste de objetivo respecto
                                   ///< al de concatenaci�n.


#define COSTE_FRONTERA_ADYACENTE 20 ///< Coste b�sico para los semifonemas cuya frontera adyacente (la del contexto) no coincide

#define COSTE_FRONTERA_CONTRARIA 15 ///< Coste b�sico para los semifonemas cuya frontera contraria (la de la otra parte del semifonema) no coincide


#define PESO_COSTE_SEMIFONEMA	1.0F	///< Peso del coste del semifonema en el modelo de coarticulaci�n.
#define PESO_COSTE_TRIFONEMA	0.6F	///< Peso del coste de la otra parte del fonema en el modelo de coarticulaci�n.
#define PESO_COSTE_TRANSICION	0.6F	///< Peso del coste de la parte que se corresponde con la zona de transici�n del semifonema.
#define PESO_COSTE_OTRO_EXTREMO	0.2F	///< Peso del coste de la parte de transici�n con la otra parte del fonema.

#define FREC_MINIMA_DISTORSION 70.0F ///< Modificaciones de m�s de un 20% por debajo de este valor producen distorsi�n (emp�rico)

#define FREC_UMBRAL_4 80.0F
#define FREC_UMBRAL_3 50.0F
#define FREC_UMBRAL_2 30.0F
#define FREC_UMBRAL_1 20.0F 
#define FREC_UMBRAL 5.0F //1.0F  ///< Frecuencia umbral, a partir de la cual se supone que ser� necesario
                          ///< realizar una modificaci�n pros�dica para que la unidad se adapte a
                          ///< las estimaciones de Cotov�a.

#define FREC_DIF_COEF_4 1.4F ///< Umbral para aplicar la penalizaci�n m�s alta seg�n el porcentaje de modificaci�n necesario entre la frecuencia objetivo y la candidata
#define FREC_DIF_COEF_3 1.3F ///< Umbral para aplicar la penalizaci�n correspondiente a una modificaci�n de un 30%
#define FREC_DIF_COEF_2 1.2F ///< Umbral para aplicar la penalizaci�n correspondiente a una modificaci�n de un 20%
#define FREC_DIF_COEF_1 1.1F ///< Umbral para aplicar la penalizaci�n correspondiente a una modificaci�n de un 10%
#define FREC_DIF_COEF 1.05F ///< Umbral para aplicar la penalizaci�n correspondiente a una modificaci�n de un 5%

#define FREC_DIF_UMBRAL  5.0F //1.0F ///< An�logo a FREC_UMBRAL, pero referido a la diferencia entre
                              ///< dos unidades consecutivas.

#define DUR_DIF_COEF_1 3.0F ///< Umbral para aplicar la penalizaci�n m�s alta penalizaci�n seg�n el porcentaje de modificaci�n necesario seg�n la duraci�n objetivo y la candidata
#define DUR_DIF_COEF_2 2.0F ///< Umbral de penalizaci�n intermedio.
#define DUR_DIF_COEF_3 1.5F ///< Umbral de penalizaci�n intermedio.
#define DUR_DIF_COEF_MENOR_3 0.33F ///< Umbral para aplicar la penalizaci�n m�s alta penalizaci�n seg�n el porcentaje de modificaci�n necesario seg�n la duraci�n objetivo y la candidata
#define DUR_DIF_COEF_MENOR_2 0.5F ///< Umbral de penalizaci�n intermedio.
#define DUR_DIF_COEF_MENOR_1 0.7F ///< Umbral de penalizaci�n intermedio.

#define DUR_UMBRAL  0.040F//0.001F ///< An�logo a FREC_UMBRAL, pero referido a la duraci�n (en segundos).

#define ENERGIA_UMBRAL 10.0F // 6.0F //1.0F ///< An�logo a FREC_UMBRAL, pero referido a la energ�a (en dBs)

#define ENERGIA_UMBRAL_CONCATENACION 3.0F //1.0F ///< An�logo a ENERGIA_UMBRAL, pero aplicado a la concatenaci�n entre fonemas

#define ENERGIA_UMBRAL_CONCATENACION_2 6.0F //1.0F ///< An�logo a ENERGIA_UMBRAL, pero aplicado a la concatenaci�n entre fonemas

#define ENERGIA_UMBRAL_CONCATENACION_3 9.0F //1.0F ///< An�logo a ENERGIA_UMBRAL, pero aplicado a la concatenaci�n entre fonemas

#define ENERGIA_UMBRAL_CONCATENACION_4 15.0F //1.0F ///< An�logo a ENERGIA_UMBRAL, pero aplicado a la concatenaci�n entre fonemas

#define UMBRAL_FRECUENCIA_MAXIMA_PENALIZACION 150.0F  ///< Diferencia de frecuencias a partir de la cual
///< se introduce la penalizaci�n m�s alta en la subfunci�n de coste de frecuencia.

#define UMBRAL_DURACION_MAXIMA_PENALIZACION 0.20F ///< Diferencia de duraciones a partir de la cual se
///< introduce la penalizaci�n m�s alta en la subfunci�n de coste de duraci�n.

#define UMBRAL_ENERGIA_MAXIMA_PENALIZACION 15.0F ///< Diferencia de energ�a a partir de la cual se
///< introduce la penalizaci�n m�s alta en la subfunci�n de coste de duraci�n.

#define MINIMA_ENERGIA 20.0F ///< M�nima energ�a de una unidad para que la consideremos v�lida.

#define MAXIMO_VALOR_ZONA_LINEAL 5.0F  ///< Valor m�s alto de la zona lineal de las funciones de coste
                                       ///< de prosodia.

#define MAXIMO_COSTE_SUBFUNCION 20.0F  ///< Valor m�ximo que pueden tomar las funciones de coste de prosodia.

#define PENDIENTE_FRECUENCIA 2.0F      ///< Pendiente de la subfunci�n de coste de frecuencia en el
                                       ///< tramo intermedio.

#define PENDIENTE_DURACION 2.0F ///< Pendiente de la subfunci�n de coste de duraci�n en el tramo
                                ///< intermedio.

//#define NUMERO_CAMINOS_PODA_PARALELO 10 ///< En el c�lculo de caminos en paralelo, se refiere al m�ximo n�mero de contornos por unidad que se consideran para 

#define PODA_C_t_POR_NUMERO
#define PODA_C_t_POR_VALOR
#define PODA_C_i_POR_NUMERO
#define PODA_C_i_POR_VALOR

#if defined(PODA_C_t_POR_NUMERO) || defined(PODA_C_t_POR_VALOR)

#define PODA_C_t

#endif

#if defined(PODA_C_i_POR_NUMERO) || defined(PODA_C_i_POR_VALOR)

#define PODA_C_i

#endif

#if defined(PODA_C_i) || defined(PODA_C_t)

#define PODA

#endif


#define absoluto(a) (((a) >= 0) ? (a) : -(a))
#define maximo(a, b) ( ((a) > (b)) ? a : b)


/** \class Viterbi

    Ver tambi�n Viterbi.cpp

*/
class Viterbi {

  Locutor * locutor_actual;

  char tipo_candidatos; ///< Forma de seleccionar los precandidatos para el algoritmo de Viterbi. Si es cero, se consideran como candidatos todos los semifonemas disponibles del mismo tipo. En caso contrario, �nicamente los que coincidan en car�cter t�nico, tipo de frase y posici�n en la misma.
#ifdef _DEPURA_VITERBI
  FILE *fichero_viterbi;
  char nombre_fichero_depura[FILENAME_MAX];
  int iteracion;
  int iteracion_contorno;
#endif

  static unsigned char fuerza_fronteras_entonativas; // Si vale cero, deja que el algoritmo de selecci�n decida si se introduce o no una frontera entonativa. En caso contrario, fuerza que se introduzca la frontera objetivo.

#ifdef _CALCULA_INDICES
  float umbral_duracion;   ///< M�xima diferencia de duraci�n sin que sea preciso realizar
  ///< una modificaci�n pros�dica; se emplea en el modo de funcionamiento
  ///< _CALCULA_INDICES, en el cual se estudian las variaciones en el
  ///< n�mero de unidades modificadas
  ///< pros�dicamente en funci�n de los umbrales de modificaci�n.

  float umbral_frecuencia; ///< M�xima diferencia de frecuencia sin que sea preciso realizar
  ///< una modificaci�n pros�dica; se emplea en el modo de funcionamiento
  ///< _CALCULA_INDICES, en el cual se estudian las variaciones en el
  ///< n�mero de unidades modificadas
  ///< pros�dicamente en funci�n de los umbrales de modificaci�n.
#endif

#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES
  int menor_identificador; ///< Identificador de la primera unidad de la frase del Corpus que se
  ///< est� considerando. Se emplea en el modo _SINTETIZA_SIN_UNIDADES_ORIGINALES,
  ///< en el cual se sintetizan las frases del Corpus, pero evitando emplear las
  ///< unidades que originalmente pertenec�an a ellas.

  int mayor_identificador; ///< Identificador de la �ltima unidad de la frase del Corpus que se
  ///< est� considerando. Se emplea en el modo _SINTETIZA_SIN_UNIDADES_ORIGINALES,
  ///< en el cual se sintetizan las frases del Corpus, pero evitando emplear las
  ///< unidades que originalmente pertenec�an a ellas.
#endif

  vector<Estructura_C_i> C_i;             ///<  Cadena de estructuras con la informaci�n de los mejores caminos y costes hasta cada unidad de la Iteraci�n actual del algoritmo de Viterbi.

  set<int> set_C_i; ///< Estructura de tipo set en la que se almacenan las unidades que pasan de una iteraci�n a la siguiente. Es redundante con respecto a C_i, pero permite un acceso r�pido para ver cuando una unidad es podable.

  vector<Estructura_coste> vector_coste; ///<  Cadena con la informaci�n de los costes de objetivo de las unidades candidatas en la iteraci�n actual.

  Path_list caminos_optimos;   ///< Nueva implementaci�n de la matriz de caminos �ptimos

#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION


  estructura_concatenacion **bloques_cache;   ///< Cadena din�mica de bloques de memoria que se
  ///< reservan para almacenar la frecuencia de los
  ///< costes de concatenaci�n entre unidades en el
  ///< modo _CALCULA_FRECUENCIA_COSTES_CONCATENACION.

  int numero_bloques_cache;                   ///< N�mero de bloques de la cadena anterior.

  estructura_concatenacion *memoria_reservada_cache;   ///< Puntero al inicio del bloque actual
  ///< de la cadena bloques_cache.

  int numero_elementos_cache;    ///< N�mero de elementos ocupados en el bloque actual.

  int indice_bloque_actual;     ///< �ndice del bloque en el que se puede introducir la informaci�n.

        
  estructura_concatenacion *inicio_cache;  ///< Puntero a la lista enlazada que se organiza a
  ///< partir de la memoria de la variable bloques_cache.


#endif

  /*
    #ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION
    cache_concatenacion *inicio_cache[NUMERO_SUBGRUPOS];
    int numero_elementos_cache[NUMERO_SUBGRUPOS];
    cache_concatenacion *memoria_reservada_cache[NUMERO_SUBGRUPOS];
    int indice_bloque_actual[NUMERO_SUBGRUPOS];
    cache_concatenacion **bloques_cache[NUMERO_SUBGRUPOS];
    int numero_bloques_cache[NUMERO_SUBGRUPOS];

    #endif
  */

  //FILE *fichero_c = NULL;

#ifdef PODA_C_t_POR_VALOR
  float minimo_c_t;     ///< Coste de objetivo m�s bajo en la iteraci�n actual del
  ///< algoritmo de Viterbi.
#endif

#ifdef PODA_C_i_POR_VALOR
  float minimo_c_i;    ///< Coste acumulado m�s bajo hasta la iteraci�n actual del algoritmo.
#endif

#ifdef PODA_C_i_POR_NUMERO
  char maxima_memoria_reservada; ///< variable que controla la memoria que se reserva para
  ///< la cadena C_i cuando se est� realizando una poda por n�mero.
#endif


#ifdef _SINTETIZA_SIN_UNIDADES_ORIGINALES

  /// Asigna valores a las variables del m�dulo menor_identificador y mayor_identificador.

  void establece_valores_identificadores(int identificador_menor, int identificador_mayor);

#endif

  /// Calcula la distancia eucl�dea entre los vectores mel-cepstrum que se le pasan
  /// como par�metro (s�lo funciona con vectores de 12 coeficientes).

  inline float calcula_distancia_euclidea_optimizada(Vector_cepstrum vector1, Vector_cepstrum vector2);

  inline float calcula_distancia_euclidea_optimizada_fase(Vector_cepstrum vector1, Vector_cepstrum vector2);

  /// Devuelve el �ndice del fonema que se le pasa como par�metro en funci�n de la
  /// tabla indices_fonemas.

  //inline int devuelve_indice_Vit(char *unidad);

  /// Devuelve el vector de pesos de la clase espectral para la versi�n de la funci�n
  /// de coste de objetivo basada en regresi�n lineal.

  inline float *devuelve_vector_pesos(clase_espectral clase);

  /// Devuelve un puntero a la red neuronal y a los estad�sticos del tipo de fonema
  /// indicado como par�metro.

  inline int devuelve_red(clase_espectral clase, Red_neuronal **red,
			  Media_varianza **estadisticos);

  /// Devuelve las caracter�sticas de cada fonema, dentro de las siguientes categor�as:
  /// VOC_ABIER, SONORO, VOCAL, VOC_MED, VOC_CERR, FRIC_SOR, CONSONANTE, LIQUIDA, NASAL,
  /// OCLU_SON, OCLU_SOR, SORDO, VIBRANTE y SILENCIO.

  int caracteristicas_de_fonema(char *fonema, clase_duracion *tipo_alofono, char *sordo_sonoro, char *vocal_consonante);

  /// Inicializa la variable minimo_C_t, necesaria para la poda por valor de la cadena
  /// vector_coste; es preciso llamar a esta funci�n en cada iteraci�n del algoritmo.

  void inicia_poda_C_t();

  /// Similar a  inicia_poda_C_t pero referida a la cadena C_i.

  void inicia_poda_C_i();

  // Carga de un fichero la memoria cach� de costes de concatenaci�n.

  // int carga_tabla_cache();


  /// Funci�n encargada de encontrar en la base de datos el conjunto de unidades
  /// candidatas m�s pr�ximas al objetivo.

  int crea_delta_u(Vector_semifonema_candidato ***cadena_id, int *tamano, Vector_descriptor_objetivo &objetivo);


  // Comprueba si el coste de concatenaci�n entre las unidades que se le pasan como
  // par�metro est� almacenado en la memoria cach�. Si lo encuentra, devuelve su valor,
  //y un -1 en caso contrario.

  // float comprueba_entrada_en_cache(int identificador1, int identificador2);



  void calcula_coste_minimo_hasta_unidad(Estructura_coste * C_ii);

  int calcula_coste_minimo_hasta_unidades(vector<Estructura_coste> &vector_coste, int numero_candidatos, char *mascara_contornos = NULL, int numero_maximo_contornos = 0);

  int calcula_coste_minimo_hasta_unidades_paralelo(vector<Estructura_coste> &vector_coste, int numero_candidatos, char *mascara_contornos, int numero_maximo_contornos);

  int crea_vector_C_t(vector<Estructura_coste> &costes, int *tamano, Vector_semifonema_candidato **delta_u, Vector_descriptor_objetivo &objetivo, char *mascara_contornos = NULL);


  /// Funci�n encargada de la poda en el vector C_t.

  int ordena_y_poda_C_t(vector<Estructura_coste> &actual, int posicion);

  /// Funci�n encargada de la poda en el vector C_i.

  int ordena_y_poda_C_i(vector<Estructura_C_i> &inicio, int posicion);

  /// Funci�n encargada de crear el vector C_i, en el que se almacenan los caminos
  /// �ptimos a cada unidad de la iteraci�n actual.

  int crea_vector_C_i(vector<Estructura_coste> &C_concatena, int tamano, Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad, char *mascara_contornos = NULL);

  ///< Funci�n encargada de crear el vector C_i, en el que se almacenan los caminos �ptimos a cada unidad de la iteraci�n actual. Se emplea en el modo de c�lculo de contornos en paralelo.

  int crea_vector_C_i_paralelo(vector<Estructura_coste> &C_concatena, int tamano, char *mascara_contornos, Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad);

  /// Escribe en el fichero que se le pasa como par�metro la informaci�n de la
  /// secuencia de objetos Vector_semifonema_candidato que forman el camino �ptimo
  /// del algoritmo de Viterbi.

  int escribe_cadena_escogida(int numero_unidades, char *nombre_fichero, int *numero_real_caminos);

  /// Asigna los valores indicados a las variables umbral_duracion y umbral_frecuencia,
  /// empleadas en el modo _CALCULA_INDICES.

  void especifica_umbrales(float umbr_duracion, float umbr_frecuencia);


#ifdef _CALCULA_FRECUENCIA_COSTES_CONCATENACION

  /// Se emplea en el modo _CALCULA_FRECUENCIA_COSTES_CONCATENACION@. En lugar
  /// de computar el n�mero de veces que se calcula el coste de concatenaci�n
  /// entre cada par de unidades, se realiza una medici�n equivalente por clusteres
  /// de unidades; en concreto, en esta funci�n se incrementa el n�mero de veces que
  /// se calculan los costes de concatenaci�n entre los semifonemas de dos clusteres
  /// de unidades.

  int anhade_conjunto_unidades_coste_concatenacion(Vector_semifonema_candidato *unidad_actual,
						   Estructura_C_i *C_i,
						   unsigned char frase,
						   unsigned char posicion,
						   unsigned char acento,
						   unsigned char frase_anterior,
						   unsigned char posicion_anterior,
						   unsigned char acento_anterior);

  /// A�ade una clave a la lista enlazada de los bloques de concatenaci�n.

  int anhade_clave_concatenacion(unsigned int clave, int parametros);

  /// Imprime en formato texto en el fichero que se le pasa como par�metro la
  /// informaci�n de la frecuencia de costes de concatenaci�n almacenada
  /// en la lista a la que apunta inicio_cache.

  int imprime_frecuencia_costes_concatenacion(char *nombre_fichero);

  /// Imprime en formato binario en el fichero que se le pasa como par�metro la
  /// informaci�n de la frecuencia de costes de concatenaci�n almacenada
  /// en la lista a la que apunta inicio_cache.

  int imprime_frecuencia_costes_concatenacion_bin(char *nombre_fichero);

  /// Libera la memoria asignada a las variables en las que se almacena la informaci�n de la frecuencia de los costes de concatenaci�n.

  void borra_cache_frecuencia_costes();

  ///  Funci�n que compara dos claves del tipo clave_parametros; se emplea para
  /// ordenar la lista de frecuencia de costes de concatenaci�n.

  int compara_claves_complejas(clave_parametros operando1, clave_parametros operando2);

  /// Funci�n que empaqueta en una variable del tipo clave_parametros la informaci�n de los dos clusteres de unidades de los cuales se calculan los costes de concatenaci�n.

  void empaqueta_clave(int fonema_1, int fonema_2, int tipo_unidad,
		       char izq_der, unsigned int *clave);

  /// Funci�n que desempaqueta de una variable del tipo clave_parametros la informaci�n de los dos clusteres de unidades de los cuales se calculan los costes de concatenaci�n.

  int desempaqueta_clave(unsigned int clave, int parametros, char *izq_der, char *acento, char *posicion, char *frase, char *fonema_1, char *fonema_2, char *grupo_completo, char *difonema);

#endif

public:

  char izq_der;                   ///<  Parte del fonema considerada en la iteraci�n actual del algoritmo.

  Viterbi() {
    fuerza_fronteras_entonativas = 0;
  }

  /// Permite modificar la variable que controla el coste relacionado con la utilizaci�n de unidades en una frontera distinta a la objetivo
  void modifica_fuerza_fronteras_entonativas(unsigned char fuerza);

  /// Versi�n original de la funci�n de coste de objetivo, en la que los pesos est�n ajustados a mano.

  inline float calcula_C_t(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der);

  /// Versi�n de la funci�n de coste de objetivo en la que el coste relacionado con el contexto se realiza por medio de los centroides de los semifonemas circundantes; los pesos tambi�n est�n ajustados a mano.

  inline float calcula_C_t_centroide(Vector_semifonema_candidato *candidato,
				     Vector_descriptor_objetivo *objetivo,
				     char izq_der);

  /// Variante de la funci�n  calcula_C_t_centroide en la que el coste relacionado con el contexto se calcula entre los centroides de los semifonemas circundantes del objetivo y los vectores medios de los semifonemas que rodean al candidato.

  inline float calcula_C_t_centroide_vector_medio(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo, char izq_der);

  /// Variante de calcula_C_t_centroide_vector_medio en la que los pesos est�n
  /// ajustados mediante regresi�n lineal.

  inline float calcula_C_t_regresion(Vector_semifonema_candidato *candidato,
				     Vector_descriptor_objetivo *objetivo,
				     char izq_der);

  /// Variante de la funci�n calcula_C_t_centroide_vector_medio en la que la importancia de las diferentes partes se calcula por medio de una red neuronal.

  inline float calcula_C_t_red_neuronal(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo *objetivo, char izq_der);

  /// Variante de la funci�n anterior, en la que se selecciona el semifonema del corpus m�s parecido al contexto fon�tico deseado, y se utiliza como objetivo. Cuando no hay un semifonema adecuado, se recurre al modelo de redes neuronales.
                
  inline float calcula_C_t_red_neuronal_y_modelo_coarticulacion(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *semifonema_contexto, char izq_der, vector<float> *costes_c, char *mascara_contornos = NULL);
                    
  /// Variante de la funci�n calcula_C_t_red_neuronal_y_modelo_coarticulacion, en la que no se emplean medidas espectrales, sino un peso de coarticulacion calculado a partir de la diferencia de contextos fon�ticos.

  float calcula_C_t_peso_coarticulacion_texto(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der, vector<float> *costes_c, char *mascara_contornos);

  /// Calcula el coste de concatenaci�n entre las dos unidades que se le pasan como par�metro.

  float calcula_C_c(Vector_semifonema_candidato *unidad1, Vector_semifonema_candidato *unidad2);

#ifdef _DEPURA_VITERBI
  void avanza_contorno();
#endif

  int inicializa(Locutor * locutor, char tipo_cand);

  /// Se encarga de la primera iteraci�n del algorimo de Viterbi.

  int inicia_algoritmo_Viterbi(Vector_descriptor_objetivo &objetivo, int numero_objetivos, char izquierdo_derecho, char *mascara_contornos = NULL, int numero_maximo_contornos = 1);

  /// Realiza cada iteraci�n del algoritmo de Viterbi.

  int siguiente_recursion_Viterbi(Vector_descriptor_objetivo &objetivo, unsigned char ultima_unidad, char *mascara_contornos = NULL, int numero_maximo_contornos = 1, float *costes_acumulados = NULL);

  /// Poda los contornos que pasen de un cierto coste (c�lculo de contornos en paralelo)

  int poda_contornos_paralelo(char *mascara_contornos, float *costes_acumulados, int numero_maximo_contornos, Vector_descriptor_objetivo &objetivo );

  /// Devuelve la secuencia de objetos Vector_sintesis que proporciona un camino
  /// de coste m�nimo con respecto a la frase objetivo de entrada.

  int devuelve_camino_optimo_sintesis(int *numero_unidades, float *coste_minimo, Vector_sintesis **cadena_vectores, int *numero_real_caminos, int *indice_contorno = NULL);

  /// Versi�n sobrecargada de la anterior, que se emplea en el modo de c�lculo de contornos en paralelo.
  int devuelve_camino_optimo_sintesis_paralelo(int *numero_unidades, float *coste_minimo, Vector_sintesis **cadena_vectores, int *numero_real_caminos, int indice_camino, char *mascara_contornos);

  /// Conjunto de funciones similares a la anterior, pero que devuelven una cadena
  /// de objetos Vector_semifonema_candidato.
                
  int devuelve_camino_optimo_candidato(int *numero_unidades, float *coste_minimo,
				       Vector_semifonema_candidato **camino_vectores, int *numero_real_caminos);
                                
  /// Conjunto de funciones similares a la anterior, pero que devuelven una cadena
  /// de objetos Vector_semifonema_candidato.

  Vector_semifonema_candidato * devuelve_camino_optimo_candidato(int *numero_unidades, int *numero_real_caminos);

  /// Libera la memoria reservada para las diferentes estructuras empleadas en la
  /// ejecuci�n del algoritmo.

  void libera_memoria_algoritmo();

  /// Escribe en el fichero que se le pasa como par�metro los subcostes de las diferentes
  /// partes de la funci�n de costes de objetivo para generar un fichero con el que se
  /// pueda entrenar un modelo de la distancia espectral entre unidades, ya sea
  /// estad�stico o basado en redes neuronales.

  int escribe_subcostes_C_t(Vector_semifonema_candidato *candidato, Vector_semifonema_candidato *objetivo,
			    char izq_der, FILE *fichero);

  static float calcula_coste_coarticulacion(Vector_semifonema_candidato *vector_1, Vector_semifonema_candidato *vector_2);

  static float decide_peso_coarticulacion(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato);

  static float decide_peso_fronteras(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato);

  static  float decide_peso_posicion_y_tipo_frase(Vector_descriptor_objetivo &objetivo, Vector_semifonema_candidato *candidato);

  static float decide_peso_frontera_prosodica(char frontera_objetivo, Vector_semifonema_candidato *candidato);

  void muestra_contenido_vector_costes(vector<Estructura_coste> &costes, FILE *fichero);

  void muestra_contenido_vector_C_i(vector<Estructura_C_i> &C_i, FILE *fichero);
};


#endif

