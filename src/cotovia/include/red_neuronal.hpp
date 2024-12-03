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
 

#ifndef _RED_NEURONAL

#define _RED_NEURONAL

#define ETA 0.01F // Tasa de aprendizaje

#define ALPHA_MOMENTO 0.01F // "Constante del momento". Usease, momentum constant.

//#define ALPHA_MOMENTO 0.0F // "Constante del momento". Usease, momentum constant.

// Distribuciones para la inicialización de los pesos.

#define NORMAL 0
#define UNIFORME 1
#define CONSTANTE 2

#define PARAMETRO_TANGENTE_HIPERBOLICA 1.0F

#define PARAMETRO_SIGMOIDE 1.0F

#define PENDIENTE_RECTA 1.0F

#define NUMERO_ENTRADAS_RED_ESPECTRAL 22

#define MAXIMO_NUMERO_CAPAS_OCULTAS 10 // Por poner algo

// FICHEROS EN LOS QUE SE ENCUENTRA LA INFORMACIÓN DE LAS REDES ENTRENADAS

#define FICHERO_RED_VOCAL_ABIERTA "red_funcion_objetivo_vocal_abierta.txt"
#define FICHERO_RED_VOCAL_MEDIA "red_funcion_objetivo_vocal_media.txt"
#define FICHERO_RED_VOCAL_CERRADA "red_funcion_objetivo_vocal_cerrada.txt"
#define FICHERO_RED_NASAL "red_funcion_objetivo_nasal.txt"
#define FICHERO_RED_LATERAL "red_funcion_objetivo_lateral.txt"
#define FICHERO_RED_SILENCIO "red_funcion_objetivo_silencio.txt"
#define FICHERO_RED_APROX_SON "red_funcion_objetivo_aprox_son.txt"
#define FICHERO_RED_OCLU_SON "red_funcion_objetivo_oclu_son.txt"
#define FICHERO_RED_OCLU_SOR "red_funcion_objetivo_oclu_sor.txt"
#define FICHERO_RED_FRIC_SOR "red_funcion_objetivo_fric_sor.txt"
#define FICHERO_RED_VIBRANTE "red_funcion_objetivo_vibrante.txt"


#define FICHERO_RED_VOCAL_ABIERTA_FRE "pesos_fre_200_red_vocal_abierta_perceptron_22_14_9.txt"
#define FICHERO_RED_VOCAL_MEDIA_FRE "pesos_fre_200_red_vocal_media_perceptron_22_14_9.txt"
#define FICHERO_RED_VOCAL_CERRADA_FRE "pesos_fre_200_red_vocal_cerrada_perceptron_22_14_9.txt"
#define FICHERO_RED_NASAL_FRE "pesos_fre_200_red_nasal_perceptron_22_14_9.txt"
#define FICHERO_RED_LATERAL_FRE "pesos_fre_200_red_lateral_perceptron_22_14_9.txt"
#define FICHERO_RED_SILENCIO_FRE "pesos_fre_200_red_silencio_perceptron_22_14_9.txt"
#define FICHERO_RED_APROX_SON_FRE "pesos_fre_200_red_aprox_son_perceptron_22_14_9.txt"
#define FICHERO_RED_OCLU_SON_FRE "pesos_fre_200_red_oclu_son_perceptron_22_14_9.txt"
#define FICHERO_RED_OCLU_SOR_FRE "pesos_fre_200_red_oclu_sor_perceptron_22_14_9.txt"
#define FICHERO_RED_FRIC_SOR_FRE "pesos_fre_200_red_fric_sor_perceptron_22_14_9.txt"
#define FICHERO_RED_VIBRANTE_FRE "pesos_fre_200_red_vibrante_perceptron_22_14_9.txt"

#define FICHERO_RED_VOCAL_ABIERTA_PAU "pesos_pau_200_red_vocal_abierta_perceptron_22_14_9.txt"
#define FICHERO_RED_VOCAL_MEDIA_PAU "pesos_pau_200_red_vocal_media_perceptron_22_14_9.txt"
#define FICHERO_RED_VOCAL_CERRADA_PAU "pesos_pau_200_red_vocal_cerrada_perceptron_22_14_9.txt"
#define FICHERO_RED_NASAL_PAU "pesos_pau_200_red_nasal_perceptron_22_14_9.txt"
#define FICHERO_RED_LATERAL_PAU "pesos_pau_200_red_lateral_perceptron_22_14_9.txt"
#define FICHERO_RED_SILENCIO_PAU "pesos_pau_200_red_silencio_perceptron_22_14_9.txt"
#define FICHERO_RED_APROX_SON_PAU "pesos_pau_200_red_aprox_son_perceptron_22_14_9.txt"
#define FICHERO_RED_OCLU_SON_PAU "pesos_pau_200_red_oclu_son_perceptron_22_14_9.txt"
#define FICHERO_RED_OCLU_SOR_PAU "pesos_pau_200_red_oclu_sor_perceptron_22_14_9.txt"
#define FICHERO_RED_FRIC_SOR_PAU "pesos_pau_200_red_fric_sor_perceptron_22_14_9.txt"
#define FICHERO_RED_VIBRANTE_PAU "pesos_pau_200_red_vibrante_perceptron_22_14_9.txt"


#define FICHERO_MEDIA_VARIANZA_VOCAL_ABIERTA "media_varianza_red_vocal_abierta.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_MEDIA "media_varianza_red_vocal_media.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_CERRADA "media_varianza_red_vocal_cerrada.txt"
#define FICHERO_MEDIA_VARIANZA_NASAL "media_varianza_red_nasal.txt"
#define FICHERO_MEDIA_VARIANZA_LATERAL "media_varianza_red_lateral.txt"
#define FICHERO_MEDIA_VARIANZA_SILENCIO "media_varianza_red_silencio.txt"
#define FICHERO_MEDIA_VARIANZA_APROX_SON "media_varianza_red_aprox_son.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SON "media_varianza_red_oclu_son.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SOR "media_varianza_red_oclu_sor.txt"
#define FICHERO_MEDIA_VARIANZA_FRIC_SOR "media_varianza_red_fric_sor.txt"
#define FICHERO_MEDIA_VARIANZA_VIBRANTE "media_varianza_red_vibrante.txt"

#define FICHERO_MEDIA_VARIANZA_VOCAL_ABIERTA_FRE "media_varianza_fre_200_red_vocal_abierta_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_MEDIA_FRE "media_varianza_fre_200_red_vocal_media_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_CERRADA_FRE "media_varianza_fre_200_red_vocal_cerrada_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_NASAL_FRE "media_varianza_fre_200_red_nasal_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_LATERAL_FRE "media_varianza_fre_200_red_lateral_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_SILENCIO_FRE "media_varianza_fre_200_red_silencio_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_APROX_SON_FRE "media_varianza_fre_200_red_aprox_son_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SON_FRE "media_varianza_fre_200_red_oclu_son_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SOR_FRE "media_varianza_fre_200_red_oclu_sor_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_FRIC_SOR_FRE "media_varianza_fre_200_red_fric_sor_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VIBRANTE_FRE "media_varianza_fre_200_red_vibrante_perceptron_22_14_9.txt"


#define FICHERO_MEDIA_VARIANZA_VOCAL_ABIERTA_PAU "media_varianza_pau_200_red_vocal_abierta_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_MEDIA_PAU "media_varianza_pau_200_red_vocal_media_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VOCAL_CERRADA_PAU "media_varianza_pau_200_red_vocal_cerrada_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_NASAL_PAU "media_varianza_pau_200_red_nasal_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_LATERAL_PAU "media_varianza_pau_200_red_lateral_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_SILENCIO_PAU "media_varianza_pau_200_red_silencio_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_APROX_SON_PAU "media_varianza_pau_200_red_aprox_son_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SON_PAU "media_varianza_pau_200_red_oclu_son_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_OCLU_SOR_PAU "media_varianza_pau_200_red_oclu_sor_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_FRIC_SOR_PAU "media_varianza_pau_200_red_fric_sor_perceptron_22_14_9.txt"
#define FICHERO_MEDIA_VARIANZA_VIBRANTE_PAU "media_varianza_pau_200_red_vibrante_perceptron_22_14_9.txt"

// Modelo de continuidad fónica:

#define NUMERO_ENTRADAS_RED_CONTINUIDAD_FONICA	4
#define FICHERO_CONTINUIDAD_FONICA	"red_continuidad_fonica.txt"
#define FICHERO_MEDIA_VARIANZA_PARAMETROS_CONTINUIDAD_FONICA	"media_varianza_continuidad_fonica.txt"
//

typedef struct {
    double *pesos;
    double *correccion_pesos;
    double *pesos_nulos;
} Nodo_entrada;

typedef struct {
    double entrada;
    double salida;
    double peso_0;
    double correccion;
    double *pesos;
    double *correccion_pesos;
    double *pesos_nulos;
    double gradiente_local;
    double parametro;
    double (* funcion_activacion) (double a, double x);
    double (* funcion_derivada) (double a, double x);
} Neurona;

typedef Neurona *Capa_neuronas;

class Red_neuronal {

    int *numero_neuronas;
    Capa_neuronas *capa_neuronas;
    Nodo_entrada *entrada;
    int numero_ejemplos;
    int numero_capas_ocultas;
    double error_medio;
    double error_anterior;
    double *conjunto_pesos;

    public:

    int numero_entradas;

    Red_neuronal();

	void inicia_red();

    int asigna_funcion(char *nombre_funcion, Neurona *neurona);

    int lee_estructura_perceptron_multicapa(char *nombre_fichero, int semilla,
                                        int *tamano_vector_entrada,
                                        int *tamano_vector_salida,
                                        int inicializa_pesos);

    void libera_memoria_red();

    void etapa_forward(double *datos);

    void simula_red(double *datos, double *salida_red);

    void etapa_backward(double *datos_entrada, double *salida);

    void actualiza_error_medio(double *salida);

    void actualiza_error_medio(double *salida, double *media, double *desviacion);

	void actualiza_error_medio(double *salida, double *error_iteracion);

	void actualiza_error_medio_transformacion(double dato_entrada, double *salida);

    int condicion_finalizacion();

    void reinicia_epoca();

    void devuelve_salida_red(double *salida);

    int imprime_pesos(char *nombre_fichero);

    void imprime_error_medio();

    int lee_fichero_pesos(char *nombre_fichero);

    int guarda_conjunto_pesos();

    int recupera_conjunto_pesos();

    double devuelve_error_medio_red();

    int copia_datos_red(Red_neuronal *red);

    int asigna_memoria_red(int num_entradas, int num_capas_ocultas, int *num_neuronas);

    void inicia_error();

    int escribe_pesos_capa_matriz(char *nombre_fichero, int numero_capa, char *opciones);    

	int calcula_varianza_explicada(double *entrada, double *salida,
											 int numero_vectores,
                                             int indice_salida,
                                             double *coeficiente);

};

#endif
