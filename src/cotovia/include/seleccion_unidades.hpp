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
 

#ifndef _SELECCION_UNIDADES_HPP

#define _SELECCION_UNIDADES_HPP

#include "modos.hpp"

#ifndef _MODELO_ENTONATIVO_ANTERIOR

#define MEJORES_CONTORNOS 5		// Mejores contornos, resultantes de la selección de grupos
// acentuales, entre los que se escogen los más adecuados para
// la selección de semifonemas.

#define NUMERO_RUTAS_OPTIMAS 2 // Número de contornos de frecuencia diferentes que se consideran
// en la selección de semifonemas.

#else

#define MEJORES_CONTORNOS 1
#define NUMERO_RUTAS_OPTIMAS 1

#endif


class  Seleccion_unidades {

  Viterbi viterbi;
  Viterbi_acentual viterbi_acentual;

public:

  int seleccion_unidades(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_contorno_total(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, char *dir_salida, char * frase_fonetica, unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_contorno_fichero(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, float escalado_frecuencia, char *dir_salida, char *nombre_fichero_grupos, char * frase_fonetica, Locutor * locutor, char tipo_candidatos, unsigned char fuerza_fronteras,  Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

#ifdef _MINOR_PHRASING
  int seleccion_unidades_entonativas(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, Grupo_acentual_candidato ***contornos_escogidos,  float **costes_minimos, int *numero_contornos, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras);

  int seleccion_unidades_entonativas_por_grupos_fonicos_total(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, Grupo_acentual_candidato ***contornos_escogidos,  float **costes_minimos, int *numero_contornos, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras);

  int seleccion_unidades_entonativas_por_grupos_fonicos(Grupo_acentual_objetivo *cadena_acentuales, int numero_acentuales, Frase_frontera &frase_objetivo, vector<Frase_frontera> &frase_candidata, int numero_total_contornos_entonativos, int numero_rutas_optimas, Locutor * locutor, vector<vector<Grupo_acentual_candidato *> > &vector_grupos,  vector<vector<float> > &vector_costes, vector<int> &tamano_grupo_fonico, char *nombre_fichero_entrada, int opciones, unsigned char fuerza_fronteras);

  int seleccion_unidades_acusticas(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_acusticas_segmentos(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_acusticas_segmentos_por_grupos_fonicos(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_caminos_acentuales, vector<int> &tamano_grupo_fonico, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_acusticas_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis); 

  int seleccion_unidades_acusticas_segmentos_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato **acentuales_cand, float *costes_caminos_acentuales, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

  int seleccion_unidades_acusticas_segmentos_por_grupos_fonicos_paralelo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_objetivo *cadena_acentuales, vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_caminos_acentuales, vector<int> &tamano_grupo_fonico, int numero_acentuales, float escalado_frecuencia, int numero_rutas_optimas, Locutor * locutor, char tipo_candidatos, int opciones, char *frase_fonetica, char *frase_entrada, char *nombre_fichero_entrada, FILE *ficheros[], unsigned char fuerza_fronteras, Vector_sintesis **cadena_sintesis, int *numero_vectores_sintesis);

#endif

};

#endif

