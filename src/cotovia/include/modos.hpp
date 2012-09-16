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
 
#ifndef _MODOS_FUNCIONAMIENTO

#define _MODOS_FUNCIONAMIENTO

//#define _DLL

#define _CONSIDERA_SEMIVOCALES // Si est� definido, se consideran las semivocales en el repertorio de fonemas

#define _NO_RETORNOS     //no considera los cambios de linea como fin de frase
//#define _elimina_sil_sil
//#define _MODO_NORMAL     // Modo de estudio de las unidades objetivo.
//#define _ESTADISTICA       // Modo de c�lculo de estad�sticas de las unidades.
//#define _SCRIPT            // Modo de funcionamiento de entrenamiento del corpus mediante el programa
                            // script.cpp, que simula el t�pico script de Linux.
//#define _CALCULA_DATOS_MODELO_ENERGIA // Modo en el que se obtiene la informaci�n para el entrenamiento
								// del modelo de estimaci�n de la energ�a.
//#define _SCRIPT_DURACION    // Modo similar al anterior, pero empleado con _info_duracion.
//#define _GENERA_CORPUS     // Modo de selecci�n de frases para formar el corpus
//#define _CORPUS_PROSODICO   // Modo de selecci�n de frases para el corpus pros�dico.
//#define _SECUENCIAS_FONETICAS	// Modo en el que se seleccionan frases con unas determinadas secuencias
								// foneticas.

//#define _OBVIA_FRECUENCIA     // Se activa para evitar el c�lculo de marcas de pitch.
#define _INCLUYE_POTENCIA     // Se activa si se desea incluir la informaci�n de la potencia,
                              // en la clase Vector_descriptor_objetivo.

//#define _INCLUYE_SILABAS	// Se activa si se desea incluir la s�laba en la estructura_silaba de
							// grupos_acentuales.hpp
//#define _SOLO_COSTE_INTELIGIBILIDAD ///< Se hace la s�ntesis sin tener en cuenta la estimaci�n pros�dica (frecuencia fundamental, duraci�n y energ�a).

#define _CONSIDERA_GRUPOS_HUERFANOS ///< Si no est� activado, se supone que todas las palabras que preceden pausa son t�nicas

#ifndef _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL
#define _INCLUYE_INFORMACION_MORFOSINTACTICA_EN_GRUPO_ACENTUAL // Se activa si se desea incluir la informaci�n
#endif
							// morfosint�ctica en la estructura Grupo_acentual_candidato de grupos_acentuales.hpp

//#define _MODOA_EU

//#define _PARA_ELISA         // Pues eso, un modo en el que se realiza el c�lculo de la duraci�n
                              // sil�bica.
//#define _GRUPOS_ACENTUALES   // modo de c�lculo de grupos acentuales.
//#define _CALCULA_INDICES        // modo de c�lculo del n�mero de unidades que son modificadas
                                // para lograr una prosodia determinada.

//#define _CALCULA_PENALIZACIONES_OPTIMAS // modo en el que se var�an las penalizaciones que se
										// introducen en el an�lisis morfosint�ctico al tener que
                                        // que recurrir a los tetragramas y trigramas.

//#define _CALCULA_FACTORES   // modo de c�lculo de factores relacionados con la prosodia estimada y
                            //las r�fagas de unidades consecutivas en el Corpus empleadas en la s�ntesis.

//#define _FUERZA_CONTORNOS_ORIGINALES   // modo de s�ntesis de las frases del Corpus que fuerza la
                                        // elecci�n de los contornos entonativos originales.


//#define _ESCRIBE_CONTINUIDAD_ESPECTRAL	// modo en el que se escribe la continuidad espectral entre las
										// unidades escogidas para la s�ntesis.

//#define _ESCRIBE_FRECUENCIA_GRUPOS_ACENTUALES   // modo de escritura de la frecuencia de aparici�n de cada grupo
                                                // acentual en el fichero de texto de entrada.
//#define _SINTETIZA_SIN_UNIDADES_ORIGINALES  // modo que sintetiza las frases del Corpus eliminando
                                            // las unidades originales de cada una de ellas.

//#define _SACA_CLASES_AMBIGUEDAD   //saca en cat_gramatical_descartada  la clase de ambiguedad correspondiente

//#define _CALCULA_FRECUENCIA_COSTES_CONCATENACION    // modo en el que se calculan las unidades
                                    // entre las que se calcula m�s veces el coste de concatenaci�n.

//#define _DIFONEMAS_VARIOS_CONTORNOS		// modo en el que se emplean s�lo las unidades de la
										// base de difonemas, pero con la frecuencia fundamental
                                        // basada en selecci�n de contornos.

//#define _MODELO_ENTONATIVO_ANTERIOR		// modo en el que se se estima la f0 con el modelo
										// de la versi�n de difonemas.

//#define _MODELO_POS_COMPLETO			// modo en el que se utiliza el modelo de an�lisis morfosint�ctico con
										// todas las categor�as. Deber�a llamarse a cotov�a con la opci�n -l glcom.
                                        
//#define _LEE_ENTONACION_DE_FICHERO	// modo en el que se leen los grupos acentuales con los que
									// se va a crear el contorno de frecuencia de un fichero.

//#define _MODIFICA_PROSODIA_ABUELO		// modo en el que se modifica la prosodia respetando la
										// zona de la coarticulaci�n.

//#define _ESCRIBE_FICHERO_PAUSAS   // modo autoexplicativo
//#define _SALTA_SOSTENIDOS_PAUSAS

//#define _ESCRIBE_SITUACIONES_ANOMALAS // modo de cotovia_informacion_semifonemas en el que se
									// escriben en el fichero salida_anomala.txt aquellas unidades
                                    // que se alejan de los valores de f0 y duraci�n t�picos.

#define _FICHEROS_WAV				// Si est� definido, se supone que los ficheros con la forma de onda
									// est�n en formato wav. En otro caso, datos en bruto.

#define _INFORMACION_MORFOSINTACTICA_AUTOMATICA	// No es un nombre especialmente certero: se refiere a la inclusi�n de la
							// informaci�n morfosint�ctica a partir del fichero generado autom�ticamente. Si no est�
                            // definido, se utiliza la funci�n original, creada a mano y ajustada a los valores de Freire.

#define _MINOR_PHRASING_TEMPLATE	// Modo en el que se emplea el m�dulo de rupturas entonativas basado en plantillas.

#if defined(_CALCULA_INDICES) || defined(_CALCULA_FACTORES) || defined(_FUERZA_CONTORNOS_ORIGINALES) || \
    defined(_ESCRIBE_FRECUENCIA_GRUPOS_ACENTUALES) || defined(_SINTETIZA_SIN_UNIDADES_ORIGINALES) || \
    defined(_CALCULA_FRECUENCIA_COSTES_CONCATENACION)
#define _SIN_VOZ
//#define _MODO_NORMAL
#endif

// MODOS_REFERIDOS A LA FORMA DE ESTIMAR LA ENERGIA

#ifdef _MODO_NORMAL

#define _ESTIMA_ENERGIA_RED_NEURONAL // Se estima la energ�a mediante redes neuronales. Si no
									// est� definida, se calcula mediante regresi�n lineal.

#endif


// MODOS REFERIDOS A LAS FUNCIONES DE COSTE DE OBJETIVO EMPLEADAS

#ifdef _MODO_NORMAL

//#define _UTILIZA_CENTROIDES  // modo en el que se emplean los centroides de los semifonemas para
                               // el coste fon�tico de la funci�n de coste de objetivo.
//#define _UTILIZA_CENTROIDES_Y_VECTORES_MEDIOS // modo similar al anterior, en el que se emplean los
                               // vectores medios cepstrales para los candidatos en lugar de los
                               // centroides (�stos se siguen utilizando para los objetivos)
//#define _UTILIZA_REGRESION   // modo en el que se emplea la funci�n de coste de objetivo basada en
                               // regresi�n lineal.
//#define _UTILIZA_RED_NEURONAL   // modo en el que se emplea la funci�n de coste de objetivo basada en
                                // redes neuronales.

//#define _UTILIZA_RED_NEURONAL_Y_MODELO_COARTICULACION		// modo en el que se emplea como modelo de coarticulaci�n
								// la unidad candidata m�s pr�xima al contexto fon�tico deseado. El modelo antiguo de
                                // redes neuronales se emplea cuando no se encuentra una unidad adecuada.

#define _UTILIZA_C_t_PESO_COARTICULACION_SIN_REDES_NEURONALES    // modo en el que se emplea la funci�n de coste de objetivo sin medidas espectrales. El coste de coarticulaci�n se calcula a partir de la diferencia de contextos fon�ticos

#define _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA
								// se considera un modelo de red neuronal para modelar la continuidad de f0 tras una coma.
                                // (en la funci�n de coste de concatenaci�n entonativa)

//#define _CONSIDERA_CONTINUIDAD_FONICA_TRAS_PAUSA_RED_NEURONAL // Se emplea una red neuronal para estimar el reinicio de f0 tras la pausa. No funciona correctamente, as� que es mejor pasar de ello...

#endif


#define _CONSIDERA_FRONTERA_PROSODICA_EN_FONEMAS_SORDOS // Si est� definido, se considera el campo frontera_prosodica en los fonemas sordos.

#define _SELECCION_ANTERIOR	// modo que controla el conjunto de unidades que se escogen previamente de un tipo para la selecci�n.

//#define _METODO_ANTIGUO_INTERPOLADO_CONTORNOS	// modo en el que se utiliza la funci�n crea_vector_frecuencias para extraer
							// los valores adecuados de los contornos escogidos en la b�squeda entonativa. Si no est� definido,
                            // se emplea la funci�n crea_vector_frecuencias_V2
                            
//#define _SOLO_PAUSAS_ORTOGRAFICAS
                                
//#define _SIN_VOZ

 /*
  * #if defined(_SCRIPT) || defined(_GRUPOS_ACENTUALES) || \
  *	 defined(_PARA_ELISA) || defined(_SCRIPT_DURACION) || \
  *     defined(_CALCULA_DATOS_MODELO_ENERGIA) //|| defined(_CORPUS_PROSODICO)
  * #define _SOLO_PAUSAS_ORTOGRAFICAS
  * #endif
  */

/*#define OEM */
#define ANSI
#ifdef  OEM
   #include "oem.hpp"
#else
   #include "ansi.hpp"

#endif

#define REGLAS_DE_ALOFONOS_DO_GALEGO
#endif

