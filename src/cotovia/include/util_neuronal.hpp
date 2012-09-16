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
 

#ifndef _UTIL_NEURONAL_HPP

#define _UTIL_NEURONAL_HPP

#include "red_neuronal.hpp"

#define absoluto(a) (((a) >= 0) ? (a) : -(a))

typedef struct {
    double media;
    double desviacion;
} Media_varianza;


double tangente_hiperbolica(double a, double x);

double derivada_tangente_hiperbolica(double a, double x);

double derivada_sigmoide(double a, double x);

double sigmoide(double a, double x);

double derivada_sigmoide(double a, double x);

double lineal(double a, double x);

double derivada_lineal(double a, double x);

int lee_datos_entrada(char *nombre_fichero, int tamano_vector_entrada,
                      int tamano_vector_salida, double **datos, double **salida,
                      int *numero_datos, char *cabecera);

void reordena_vectores(double *entrada, int tamano_vector_entrada,
                       double *salida, int tamano_vector_salida,
                       int numero_datos);

void calcula_media(double *entrada, int tamano_entrada, int numero_vectores,
                   double *media);

void calcula_varianza(double *entrada, int tamano_entrada, int numero_vectores,
                      double media, double *varianza);

int normaliza_vectores(double *vector, int tamano_vector, int numero_vectores,
                       char *nombre_fichero);

int normaliza_vectores(double *vector, int tamano_vector, int numero_vectores,
                       char *nombre_fichero, double *medias, double *desviaciones);

double genera_variable_normal(double media, double desviacion);

int lee_medias_y_desviaciones(char *nombre_fichero, int numero_entradas,
							  Media_varianza *estadisticos);

#endif

