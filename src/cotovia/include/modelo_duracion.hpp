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
 
#ifndef _MODELO_DURACION

#define _MODELO_DURACION


#define MAX_CHAR_VAR 20
#ifdef _CONSIDERA_SEMIVOCALES
#define NUM_NIVELES_TIPOFON 11
#else
#define NUM_NIVELES_TIPOFON 10
#endif
#define NUM_NIVELES_TIPOPROP 4
#define NUM_NIVELES_TIPOPAUSA 3
#define NUM_NIVELES_TIPOFRONTERA 4
#define MAX_DIM_X 100
#define MAX_NUM_ALOF 30
#define LON_ALOF OCTETOS_POR_FONEMA

#define DURACION_MINIMA_FONEMA 20.0F // Duración mínima para un fonema, en miisegundos.
 
#define FICHERO_DURACION_SILENCIO "mod_dur_silencio.txt"
#define FICHERO_DURACION_VOCAL "mod_dur_vocal.txt"
#define FICHERO_DURACION_FRIC_SOR "mod_dur_fricsor.txt"
#define FICHERO_DURACION_OCLU_SOR "mod_dur_oclusor.txt"
#define FICHERO_DURACION_OCLU_SON "mod_dur_ocluson.txt"
#define FICHERO_DURACION_VIBRANTE "mod_dur_vibrante.txt"
#define FICHERO_DURACION_LATERAL "mod_dur_lateral.txt"
#define FICHERO_DURACION_NASAL "mod_dur_nasal.txt"
#define FICHERO_DURACION_SEMIVOCAL "mod_dur_semivocal.txt"

#ifdef _CONSIDERA_SEMIVOCALES
#define NUMERO_ELEMENTOS_CLASE_DURACION 13
#else
#define NUMERO_ELEMENTOS_CLASE_DURACION 12
#endif

typedef enum {SILENCIO_DUR, VOC_AB_DUR, VOC_MED_DUR, VOC_CER_DUR, VOC_DUR, OCLU_SOR_DUR, OCLU_SON_DUR, FRIC_SOR_DUR, LATERAL_DUR, NASAL_DUR, VIBRANTE_DUR, LIQUIDA_DUR, SEMI_VOCAL_DUR} clase_duracion;

struct Modelo_duracion{
  int num_alof;  // Número de alófonos en este tipo de sonido. Ej: 4
  char alof[MAX_NUM_ALOF][LON_ALOF]; //Etiquetas de los alófonos en este modelo. Ej: k,p,t,tS
  int num_var; // Número de variables de predicción en el modelo de regresión.
  char variables[MAX_NUM_ALOF][MAX_CHAR_VAR]; // Nombres de las variables de predicción. Ej: "Fut1".
  int num_coef; // Número de coeficientes del modelo de regresión.
  float *coef;  //Coeficientes del modelo.
  int MAX_SilabasInicio;
  int MAX_SilabasFinal;
  int MAX_TonicasInicio;
  int MAX_TonicasFinal;
  int lee_modelo(char *archivo); // Función que lee todos los parámetros de un modelo desde archivo.
};

typedef struct {
  float Duracion;
  float DuracionDesdePausa;
  char Alofono[OCTETOS_POR_FONEMA];
  char Fut1[20]; // puede contener tanto un fonema como la clase a la que pertenece
  char Fut2[20];
  char Pas1[20];
  char Pas2[20];
  char Tonica;
  char TipoProposicion;
  char FronteraInicial;
  char FronteraFinal;
  char AnteriorPausa;
  char SiguientePausa;
  //    char InicioSilaba;
  //  char FinSilaba;
  char sordo_sonoro; // 1 -> sordo, 0->sonoro.
  char FinalFrase;
  short int FonemasPalabra;
  short int SilabasPalabra;
  short int FonemasSilaba;
  short int PosEnSilaba;
  short int SilabasInicio;
  short int SilabasFinal;
  short int TonicasInicio;
  short int TonicasFinal;
  char palabra[100];
  char fichero_origen[200]; // Este tamaño debería llegar.
} Parametros_duracion_entrenamiento;

typedef struct {
  float Duracion;
  float DuracionDesdePausa;
  char Alofono[OCTETOS_POR_FONEMA];
  char Fut1[20]; // puede contener tanto un fonema como la clase a la que pertenece
  char Fut2[20];
  char Pas1[20];
  char Pas2[20];
  char Tonica;
  char TipoProposicion;
  char FronteraInicial;
  char FronteraFinal;
  char AnteriorPausa;
  char SiguientePausa;
  //  char InicioSilaba;
  //char FinSilaba;
  char FinalFrase;
  short int FonemasPalabra;
  short int SilabasPalabra;
  short int FonemasSilaba;
  short int PosEnSilaba;
  short int SilabasInicio;
  short int SilabasFinal;
  short int TonicasInicio;
  short int TonicasFinal;
} Parametros_duracion;


int estima_duracion_ms(Parametros_duracion *datos, struct Modelo_duracion * modelo_duracion, char convierte_clases = 0);

const char *tipo_fon_duracion(char *fon);

const char *devuelve_cadena_alofono_duracion(clase_duracion clase);

int tipo_de_fonema_duracion(char *fonema, clase_duracion *clase_fonema);

void escribe_parametros_duracion(Parametros_duracion_entrenamiento *datos, FILE *fichero);

int devuelve_tipo_alofono_duracion(char *nombre, clase_duracion *clase);

const char *devuelve_cadena_alofono_duracion(clase_duracion clase);

int lee_tabla_datos_duracion(char *nombre_fichero, Parametros_duracion_entrenamiento **datos, int *numero_datos);

void convierte_Parametros_duracion_entrenamiento_a_Parametros_duracion(Parametros_duracion_entrenamiento *entrada, Parametros_duracion &salida); 

#endif 
