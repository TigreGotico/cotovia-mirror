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
 

#ifndef _INFO_ESTRUCTURAS

#define _INFO_ESTRUCTURAS


//#define TAMANO_MAXIMO_FICHERO 1000000 // Se emplea al crear el fichero único de unidades.

#define FICHERO_FRECUENCIA_MUESTREO "frecuencia_muestreo.txt"
#define FICHERO_UNIDADES "fichero_unidades_voz.bin"
#define FICHERO_CORPUS_VOZ "fichero_corpus_voz.bin"
#define FICHERO_SONIDO "corpus_sonido.snd"

#define FICHERO_ORIGEN_GRUPOS_ACENTUALES "origen_acentuales_"

#define MAXIMO_INDICE_FREIRE_800 37400 // Identificador más alto de las unidades del corpus800 de Freire
// Se puede sacar del fichero origen_unidades_fre.txt
#define MAXIMO_INDICE_PAULINO_800 37297 // Igual, pero para Paulino.
#define MAXIMO_INDICE_DAVID_800 113000

#define conjunto_unidades(frase, posicion, acento) (frase*6 + posicion*2 + acento)
#define calcula_indice_en_fichero(numero_fichero,fre) ( (numero_fichero - 1)*sizeof(tabla_ficheros) + \
		fre*509*sizeof(tabla_ficheros))

typedef struct {
    int unidad1;
    int unidad2;
    float distancia;
} estructura_distancias;


int escribe_fonemas_frase(t_prosodia *frase_prosodica, char *fichero_salida, int fstdout);

void filtra_nombre_fichero(char *nombre_fichero);

int incluye_limites_grupo_entonativo(vector<Vector_descriptor_objetivo> &fonemas_objetivo, int numero_fonemas, short int *cadena_posiciones, Grupo_acentual_candidato *grupos_acentuales, int numero_grupos, char paralelo = 0, int indice_primer_fonema = 0);

int incluye_modificacion_entonativa(vector<Vector_descriptor_objetivo> &fonemas_objetivo, Vector_semifonema_candidato *vectores_candidatos, Vector_sintesis *vectores_sintesis, int numero_semifonemas);

int incluye_modificacion_entonativa_sintesis(vector<Vector_descriptor_objetivo> &fonemas_objetivo, Vector_sintesis *vectores_sintesis, int numero_semifonemas, int frecuencia_muestreo);

int lee_ruta_fichero(char *fichero_txt, char *informacion, FILE *fichero_configuracion, char *nombre_salida);
                     
int lee_parametro_configuracion(char *informacion, FILE *fichero_configuracion,
				char *parametro);

int extrae_numero_fichero(char *nombre_fichero, int *numero_fichero);

int encuentra_indice_frase_en_fichero(char *nombre_fichero, FILE *fichero, tabla_ficheros *indice_fichero);

int convierte_fichero_unidades_bin(char *fichero_txt, map<int, int> *origen_unidades,
								   int *numero_unidades);

estructura_unidades *reglas_sustitucion_semifonemas(char fonema1[], char fonema2[], char izq_der, Locutor *locutor_actual);
                                   
int convierte_vector_sintesis_a_candidato(Vector_sintesis *vector_sintesis, Vector_descriptor_objetivo &objetivo, Locutor *locutor, void **semifonema);


int convierte_vector_sintesis_a_candidato_tonicidad(Vector_sintesis *vector_sintesis, vector<Vector_descriptor_objetivo> &objetivo, Locutor *locutor, void **semifonema);

int escribe_discontinuidad_espectral(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, Locutor * locutor_actual, const char *opciones, int numero_coeficientes, char cepstrum_fase);
                                   
void actualiza_estadisticas_uso_unidades(Vector_sintesis *unidades_escogidas, int numero_unidades, map<string, map<int, int> > &map_unidades_escogidas);

int escribe_map_uso_unidades(map<string, map<int, int> > &map_unidades_escogidas, char *nombre_fichero);

void escribe_cadena_unidades_escogidas(vector<Vector_descriptor_objetivo> &cadena_unidades,
		Vector_sintesis *unidades_escogidas,
		int indice_fichero,
		int numero_unidades, int indice_menor,
		FILE *fichero_unidades);

int devuelve_tipo_frase_cadena(int tipo_frase, char **cadena_tipo_frase);

int devuelve_tipo_posicion_cadena(int tipo_posicion, char **cadena_tipo_posicion);

int devuelve_tipo_frontera_prosodica_cadena(int tipo_frontera_prosodica, char **cadena_tipo_frontera);

int devuelve_indices_menor_y_mayor(int numero_frase, char *nombre_fichero,
		int *indice_menor, int *indice_mayor);

//int carga_unidades_en_memoria(char * dir_bd, FILE *fichero, Locutor *locutor);

int obten_unidad_de_nombre_fichero(char *nombre_fichero, char primer_fonema[], 
		char segundo_fonema[],
		unsigned char *izq_der);

void crea_nombre_fichero(char nombre[], char primero[], char segundo[],
		char opcion, char path[], const char *extension);

int crea_nombre_fichero(int indice1, int indice2, char izq_der, char *nombre, const char *extension);

//int decide_siguiente_tipo_unidad(char inicio_busqueda, unsigned char *tipo_frase,
//		unsigned char *posicion, unsigned char *acento);

estructura_unidades *devuelve_puntero_a_unidades(const char *primero, const char *segundo, char izq_der,
		Locutor *locutor);

estructura_unidades *devuelve_puntero_a_unidades(const char *primero, const char *segundo, char izq_der);

int devuelve_vector_unidades_fonemas(char fonema[],  Locutor *locutor, vector<Vector_semifonema_candidato *> &vector_unidades);

Vector_semifonema_candidato **crea_cadena_unidades_candidatas(estructura_unidades *unidad, unsigned char frase, unsigned char posicion, unsigned char acento, int *tamano, char tipo_candidatos);

Vector_semifonema_candidato **crea_cadena_unidades_candidatas_tonicidad(estructura_unidades *unidad, unsigned char acento, int *tamano);
        
int convierte_a_cadena_vectores(estructura_matriz *inicio, int tamano, Vector_sintesis **cadena_salida);

int convierte_a_cadena_vectores(estructura_matriz *inicio, int tamano, Vector_semifonema_candidato **cadena_salida);

int convierte_a_cadena_acentuales(estructura_matriz *inicio, int tamano, Grupo_acentual_candidato **cadena_salida);

int convierte_a_cadena_acentuales_GF(estructura_matriz *inicio, vector<int> &tamano_grupos_fonicos, int tamano, Grupo_acentual_candidato **cadena_salida, int *numero_grupos_acentuales);

void extrae_identificador(char *nombre_fichero, int *identificador);

//Vector_semifonema_candidato **obten_candidato_de_lista_difonemas(
//		Vector_descriptor_objetivo *objetivo,
//		int *tamano, char izq_der,
//		Locutor *locutor);

/*
   int convierte_difonema_a_candidato(Vector_difonema *difonemas, int tamano, char izq_der,
   Vector_descriptor_candidato ***candidato);
   */

Vector_semifonema_candidato **convierte_difonema_a_candidato(Vector_difonema *difonemas, 
		int tamano,
		char izq_der);

int lee_unidad_y_crea_candidato(int posicion, Vector_semifonema_candidato *candidato);

void construye_nombre_unidad(Vector_descriptor_objetivo &objetivo,
		char  izq_der, char nombre[]);

int imprime_energia_senhal(FILE *fichero, int numero_unidades);

int crea_vector_frecuencias(vector<Vector_descriptor_objetivo> &cadena_unidades,
		int numero_unidades,
		Grupo_acentual_candidato *acentual_cand,
		Grupo_acentual_objetivo *acentual_obj,
		float esc_pitch,
			    estructura_frecuencias **frecuencias, int indice_fonema = 0);

int crea_vector_frecuencias_V2(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, Grupo_acentual_candidato *acentual_cand, Grupo_acentual_objetivo *acentual_obj, float esc_pitch, estructura_frecuencias **frecuencias, int indice_fonema = 0);

int anhade_frecuencias_objetivo(vector<Vector_descriptor_objetivo> &cadena_unidades, int numero_unidades, estructura_frecuencias *frecuencias, int indice_primer_fonema = 0);

int crea_contorno_objetivo(Frecuencia_estilizada *original,
                            estructura_silaba *objetivo,
                            int nuevo_numero,
                            int silaba_acentuada,
                            Frecuencia_estilizada *nuevo_contorno);

int crea_fichero_contorno_frecuencia(vector<Vector_descriptor_objetivo> &cadena_unidades,
		Vector_sintesis *cadena_escogidas,
		int numero_unidades,
		estructura_frecuencias *frecuencias,
        Locutor *locutor_actual,
		const char *opcion,
		char *nombre_fichero, int indice);

int crea_fichero_grupos_acentuales(Grupo_acentual_candidato *candidatos,
								   int numero_acentuales,
								   const char *opcion,
                                   char *nombre_fichero, int indice);


int encuentra_indice_coste_minimo(float *costes_minimos, int numero);

int encuentra_indice_coste_minimo(float *costes_minimos, int numero, char *mascara_contornos);

void comprueba_continuidad_frecuencia(Vector_semifonema_candidato *cadena_candidatos,
		int numero_candidatos);

int escribe_cadena_escogida_final(int numero_unidades, 
				  char *nombre_fichero,
				  vector<Vector_descriptor_objetivo> &cadena_objetivo,
				  Vector_sintesis *cadena_sintesis, 
				  Locutor * locutor, char informacion, 
				  const char *opciones);

int escribe_f0_objetivo(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, const char *opciones);

int escribe_duraciones_objetivo(int numero_unidades, char *nombre_fichero,
				vector<Vector_descriptor_objetivo> &cadena_objetivo,
				Vector_sintesis *cadena_sintesis,
				const char *opciones);

int escribe_potencias_objetivo(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, const char *opciones);

int escribe_costes_coarticulacion(int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Vector_sintesis *cadena_sintesis, Locutor * locutor_actual, char *mascara_contornos, int indice_coste_minimo, const char *opciones);

int escribe_coste(char *nombre_fichero, float coste, int indice_contorno, const char *opciones);

int escribe_coste(char *nombre_fichero, float coste, vector<int> &caminos_escogidos, const char *opciones);

int escribe_distancia_f0(vector<Vector_descriptor_objetivo> &objetivos, Vector_sintesis *candidatos, int numero_candidatos, char *nombre_fichero, const char *opciones);

int escribe_contornos_fichero(vector<Vector_descriptor_objetivo> &objetivos, char *nombre_fichero, const char *opciones);


int escribe_identificadores_contornos_fichero(Grupo_acentual_candidato **cadena_acentuales, int numero_contornos, int numero_acentuales, char *nombre_fichero);

int escribe_identificadores_contornos_fichero(vector<vector<Grupo_acentual_candidato *> > &vector_grupos, vector<vector<float> > &costes_grupos, vector<int> &tamano_grupo_fonico, char *nombre_fichero);

int escribe_contorno_escogido_final(int numero_acentuales, int numero_unidades,
				    char *nombre_fichero, char * frase_fonetica,
				    vector<Vector_descriptor_objetivo> &cadena_objetivo,
				    Grupo_acentual_candidato *cadena_acentuales,
				    Grupo_acentual_objetivo *acentuales_objetivo, 
				    const char *opciones);

int escribe_secuencia_grupos_acentuales(int numero_acentuales, int numero_unidades, char *nombre_fichero, char * frase_fonetica, vector<Vector_descriptor_objetivo> &cadena_objetivo, int primera);

int escribe_frase_con_rupturas_y_pausas(int numero_acentuales, int numero_unidades, char *nombre_fichero, vector<Vector_descriptor_objetivo> &cadena_objetivo, Grupo_acentual_candidato *cadena_acentuales, Grupo_acentual_objetivo *acentuales_objetivo, const char *opciones);

int escribe_contorno_escogido_calculo_distancia(int numero_acentuales, char *nombre_fichero, Grupo_acentual_candidato *cadena_acentuales, char *frase_entrada, float coste_camino, const char *opciones);

void escribe_indices_modificados(Vector_sintesis *unidades, int numero_unidades,
		int indice_frase, FILE *fichero);

void escribe_factores_modificacion(Vector_sintesis *unidades, 
				   Vector_semifonema_candidato *vectores,
				   vector<Vector_descriptor_objetivo> &objetivos,
				   int numero_unidades,
				   int indice_frase, FILE *fichero);

void escribe_factores_funcion_coste_entonativa(Grupo_acentual_objetivo *cadena_acentuales, Grupo_acentual_candidato *contorno_escogido, int numero_acentuales, FILE *fichero_ent_dur, FILE *fichero_pos_acentual, FILE *fichero_pos_fonico);

int escribe_factores_f0_duracion_y_tamano_continuo(Vector_sintesis *vectores,
						   vector<Vector_descriptor_objetivo> &objetivos,
		Locutor * locutor_actual,
		int numero_unidades,
		FILE *fichero_f0,
		FILE *fichero_dur, FILE *fichero_rafaga,
		FILE *fichero_concatena,
		FILE *fichero_fonema_coart,
		FILE *fichero_contextual,
		FILE *fichero_func_concatena,
        FILE *fichero_diferencias_prosodicas);

int escribe_factores_calidad_voz_sintetica(Vector_sintesis *vectores, vector<Vector_descriptor_objetivo> &objetivos, Locutor *locutor_actual, int numero_unidades, float coste_entonativo, float coste_acustico, char *nombre_fichero);

float calcula_C_c(Vector_semifonema_candidato *unidad1, Vector_semifonema_candidato *unidad2,
						 unsigned char izq_der, Locutor *locutor);

void calcula_costes_objetivo_regresion_lineal(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der, Locutor *locutor, float *coste_prosodico, float *coste_contextual, float *coste_f0, float *coste_duracion, float *coste_energia, float *diferencia_f0_est, float *diferencia_f0_tran, float *diferencia_duracion, float *diferencia_energia);

void calcula_costes_objetivo_red_neuronal(Vector_semifonema_candidato *candidato, Vector_descriptor_objetivo &objetivo, char izq_der, Locutor *locutor, float *coste_prosodico, float *coste_contextual, float *coste_f0, float *coste_duracion, float *coste_energia, float *diferencia_f0_est, float *diferencia_f0_tran, float *diferencia_duracion, float *diferencia_energia);

int crea_cadena_grupos_acentuales_originales(char * locutores, int numero_fichero,
		Grupo_acentual_objetivo *acentuales_obj,
		Grupo_acentual_candidato **acentuales_cand,
		int numero_acentuales);

int escribe_etiquetas_inicio_fonemas(Vector_sintesis *vector_sintesis,
				     vector<Vector_descriptor_objetivo> &vector_objetivo,
		int numero_unidades,
		char nombre_fichero[]);

int escribe_etiquetas_inicio_semifonemas(Vector_sintesis *vector_sintesis,
					 vector<Vector_descriptor_objetivo> &vector_objetivo,
		int numero_unidades,
		char nombre_fichero[]);

int escribe_concatenacion_grupos_acentuales(Grupo_acentual_candidato *acentuales, int numero_grupos,
                                            FILE *fichero);

void escribe_fichero_centroides_clase(Vector_cepstrum centroides[], FILE *fichero);

int organiza_base_de_datos_segun_centroides_clases(char *inicio_nombre_fichero,
												   char *nombre_fichero_errores,
                                                   Locutor *locutor);

int organiza_base_de_datos_segun_centroides_semifonemas(vector<string> &corpus,
														char *inicio_nombre_fichero,
														char *nombre_fichero_errores,
                                                        char *nombre_fichero_origen_unidades,
                                                        char *nombre_fichero_unidades_anuladas,
                                                        char *inicio_nombre_ficheros_salida,
                                                        float umbral_eliminacion,
                                                        Locutor *locutor);

int adapta_identificador(int identificador, char *cadena_identificador);

int apunta_grupos_acentuales_escogidos(int numero_acentuales, char * locutores,
		Grupo_acentual_candidato *cadena_acentuales);

int escribe_grupos_acentuales_escogidos(char *nombre_fichero);

int calcula_distancias(clase_espectral tipo_alofono, map<int, int> &cadena_origen_unidades,
		vector<string> &corpus, char path_cepstrum[], FILE *fichero_sal, Locutor *locutor);

int calcula_distancias_y_subfunciones(clase_espectral tipo_alofono,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus,
		char path_cepstrum[], FILE *fichero_sal, Locutor *locutor,
        int unidades_por_banda);

int calcula_distancias_y_subfunciones_acentual(int posicion_grupo, Locutor *locutor,
		    								   FILE *fichero_sal);

int calcula_distancias_y_subfunciones(char alofono1[], char alofono2[], char izquierdo_derecho,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus,
		char path_cepstrum[], FILE *fichero_sal,
		Locutor *locutor);

int calcula_distancias_y_subfunciones_vector_medio(clase_espectral tipo_alofono,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus,
		char path_cepstrum[], FILE *fichero_sal,
		Locutor *locutor, int unidades_por_banda);

int calcula_distancias_y_subfunciones_vector_medio_valores(clase_espectral tipo_alofono,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus,
		char path_cepstrum[], FILE *fichero_sal,
		Locutor *locutor, int unidades_por_banda);

int calcula_distancias(char alofono1[], char alofono2[], char izquierdo_derecho,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
		FILE *fichero_sal, Locutor *locutor);

int calcula_distancias(char alofono1[], char alofono2[], char izquierdo_derecho,
					   Vector_semifonema_candidato *semifonema,	
                       FILE *fichero_sal, Locutor *locutor);

int calcula_distancias_centroide(char alofono1[], char alofono2[], char izquierdo_derecho,
		map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
		FILE *fichero_sal, Locutor *locutor);

int calcula_centroides(map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
		FILE *fichero_sal, char incluye_varianza, Locutor *locutor);

int calcula_centroides_generico(map<int, int> &cadena_origen_unidades, vector<string> &corpus, char path_cepstrum[],
					   char nombre_locutor[], FILE *fichero_sal, char incluye_varianza, Locutor *locutor);

void carga_centroides_semifonemas(FILE *fichero, int incluye_varianza);

int calcula_distancia_entre_unidades(char alofono1[], char alofono2[], char izquierdo_derecho,
		map<int, int> &cadena_origen_unidades, char path_cepstrum[],
		int identificador_escogido, int identificador_original,
		Locutor *locutor,
		float *distancia, char *unidad_valida, FILE *fichero_errores);

int escribe_informacion_clustering(char unidad_1[], char unidad_2[], char izquierdo_derecho,
		Locutor *locutor, FILE *fichero_salida);

int calcula_histogramas_semifonemas(map<int, int> &origen_unidades, vector<string> &corpus, char path_cepstrum[],
		char *nombre_fichero, Locutor *locutor, char opcion_salida);

#ifdef _UTILIZA_RED_NEURONAL

void calcula_coste_inteligibilidad_red_neuronal(Vector_semifonema_candidato *candidato, vector<Vector_descriptor_objetivo> &objetivo, char izq_der, Locutor *locutor, float *coste_prosodico, float *coste_contextual);

#endif

float calcula_C_c(Vector_semifonema_candidato *unidad1, Vector_semifonema_candidato *unidad2, unsigned char izq_der, Locutor *locutor);

int encuentra_candidato_con_coarticulacion_adecuada(Vector_descriptor_objetivo &objetivo, Locutor *locutor, unsigned char izq_der, Vector_semifonema_candidato **candidato);

int crea_vectores_sintesis_desde_fichero(char *nombre_fichero, Vector_sintesis **vectores_sintesis, int *numero_vectores_sintesis, vector<Vector_descriptor_objetivo> &vectores_objetivo, int *numero_vectores_objetivo);


#endif
