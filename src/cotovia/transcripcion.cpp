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
 

/**
 * \file transcripcion.cpp
 * \author fmendez
 * \brief Transcripcion fonética.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <pcreposix.h>
#include <sys/types.h>
//#define _POSIX_C_SOURCE
#include <regex.h>
#include "variantes.tab.hpp"
#include "alternativas.hpp"
#include "gbm_transcripcion.hpp"
#include "tip_var.hpp"
#include "trat_fon.hpp"
/* Compilanse as reglas adecuadas segun a aplicación do programa.*/
#ifdef REGLAS_DE_ALOFONOS_DO_GALEGO
#include "alof_gal.hpp"
#else
#include "alof_tel.hpp"
#endif
#include "sil_acen.hpp"
#include "alofonos.hpp"
#include "transcripcion.hpp"


int Transcripcion::variantes_cargadas = 0; 

//char transcripcion_vacia[] = "<>";
char transcripcion_vacia[] = "";

Variante *variantes = NULL; ///< Contiene las expresiones regulares para generar transcripciones fonéticas alternativas
int nvariantes = 0; ///< Número de reglas para generar transcripciones fonéticas alternativas
extern FILE *yyin;
extern int yyparse();
//int importancia = 0;

/** 
 * \author fmendez
 * \author cardenal
 * \remarks Carga las reglas necesarias para generar transcripciones fonéticas alternativas.
 * 
 * \param nombre_fichero Fichero de reglas para generar transcripciones fonéticas alternativas
 * \param imp Importancia de las reglas de transcripción fonética alternativa a cargar
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::lee_variantes(char *nombre_fichero, int imp)
{

	importancia = imp;
	if (variantes_cargadas) {
		variantes_cargadas++;
		return 0;
	}

	if ((yyin = fopen(nombre_fichero, "r")) == NULL) {
		fprintf(stderr, "Error al intentar abrir el fichero de transcripciones %s\n",
				nombre_fichero);
		return 1;
	}
	//fprintf(stderr,"Leyendo variantes.\n");
	yyparse();
	//fprintf(stderr,"Leidas %d variantes\n",nvariantes);
	//fprintf(stderr,"Compilando reglas.\n");
	CompilaVariantes(variantes, nvariantes);
	fclose(yyin);
	//transcripciones=(char **)calloc(sizeof(char *),500);
	variantes_cargadas++;
	return 0;
}

/**
 * \author fmendez
 * \remarks Libera memoria de las reglas de generación de transcripciones fonéticas alternativas
 *
 * \return 0 en ausencia de error
 */
int Transcripcion::libera_variantes(void)
{
	variantes_cargadas--;

	if (variantes_cargadas) {
		return 0;
	}

	while (nvariantes) {
		nvariantes--;
		LiberaVariante(variantes + nvariantes);
	}
	free(variantes);
	return 0;
}

/** 
 * \author fmendez
 * \remarks
 * 
 * \param f_fon 
 * \param f_sep 
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::vuelca_transcripcion(char *f_fon, t_frase_separada * f_sep)
{
	if (alternativas) {
		sacar_transcripcion_con_alternativas(f_fon, f_sep);
	}
	else {
		sacar_transcripcion(f_fon, f_sep);
	}
	return 0;
}

/** 
 * \author fmendez
 * \remarks
 * 
 * \param frase 
 * \param aux 
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::sacar_transcripcion(char *frase, t_frase_separada * aux)
{

	char ff[LONX_MAX_FRASE_SIL_E_ACENTUADA], *aux1, *aux2;


	if (fd == NULL) {
		strcpy(ff, fsalida);
		strcat(ff, ".tra");
		if ((fd = fopen(ff, "w")) == NULL) {
			fprintf(stderr,
					"\nO arquivo de transcricion \"%s\" non se pode crear",
					ff);
			return 1;
		}
	}

	if (tra < 4) {
		aux1 = ff;
		aux2 = frase;

		while (*aux2) {
			switch (*aux2) {
				case '^':
					if (tra > 1){
						*aux1++ = *aux2;
					}
					break;
				case '-':
					if (tra > 2){
						*aux1++ = *aux2;
					}
					break;
				case '#':
					aux2++;
					while (*aux2 != '#'){
						aux2++;
					}
					aux2++;
					break;
				case '%':
					aux2++;
					while (*aux2 != '%'){
						aux2++;
					}
					aux2++;
					break;
				default:
					*aux1++ = *aux2;
			}
			if (*aux2) {
				aux2++;
			}
		}
		*aux1 = 0;

		if (!por_palabras) {
			fputs(ff, fd);
			putc('\n', fd);
		}
		else {
			char *transc, *esp;
			char *pal;
			transc = strtok(ff, " ");
			while (*aux->pal) {
				fputs(aux->pal, fd);
				putc('\t', fd);
				if (aux->clase_pal == SIGNO_PUNTUACION ||
						aux->clase_pal == CADENA_DE_SIGNOS ||
						*aux->pal_sil_e_acentuada == 0) {
					aux++;
					putc('\n', fd);
					continue;
				}
				if (aux->pal_transformada){
					pal = aux->pal_transformada;
				}
				else {
					pal = aux->pal;
				}
				esp = pal;
				while (*esp == ' '){
					esp++;
				}
				while ((esp = strchr(esp, ' ')) != NULL) {
					while (*esp == ' '){
						esp++;
					}
					if (*esp == 0){
						break;
					}
					fputs(transc, fd);
					putc(' ', fd);
					transc = strtok(NULL, " ");
				}
				fputs(transc, fd);
				putc('\n', fd);
				transc = strtok(NULL, " ");
				aux++;
			}
		}
	}
	else {
		fputs(frase, fd);
		putc('\n', fd);
	}
	return 0;
}

/**
 * \author cardenal
 * \author fmendez
 * \remarks
 * 
 * \param frase 
 * \param aux 
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::sacar_transcripcion_con_alternativas(char *frase,
		t_frase_separada * aux)
{

	char ff[LONX_MAX_FRASE_SIL_E_ACENTUADA], *aux1, *aux2;
	t_vocabulario *ptr_voc = vocabulario;
	char *transc, *esp;
	//char *pal;

	if (fd == NULL) {
		strcpy(ff, fsalida);
		strcat(ff, ".tra");
		if ((fd = fopen(ff, "w")) == NULL) {
			fprintf(stderr, "\nO arquivo de transcricion \"%s\" non se pode crear", ff);
			return 1;
		}
	}

	aux1 = ff;
	aux2 = frase;

	while (*aux2) {
		switch (*aux2) {
			//case '^':
			case '-':
				break;
			case '#':
				aux2++;
				while (*aux2 != '#'){
					aux2++;
				}
				aux2++;
				break;
			case '%':
				aux2++;
				while (*aux2 != '%'){
					aux2++;
				}
				aux2++;
				break;
			default:
				*aux1++ = *aux2;
		}
		if (*aux2)
			aux2++;
	}
	*aux1 = 0;

	*palabras = 0;
	//  *transcripcion=0;

	transc = strtok(ff, " ");
	while (*aux->pal) {
		if (aux->pal_transformada) {
			esp = aux->pal_transformada;
			strcat(palabras, aux->pal_transformada);
		}
		else {
			esp = aux->pal;
			strcat(palabras, aux->pal);
		}
		strcat(palabras, " ");


		if (aux->clase_pal == SIGNO_PUNTUACION ||
				aux->clase_pal == CADENA_DE_SIGNOS ||
				*aux->pal_sil_e_acentuada == 0) {
			aux++;
			ptr_voc->final_atono = ptr_voc->inicio_atono = 0;
			//strcpy(ptr_voc++->tra, transcripcion_vacia);
			ptr_voc++->tra = transcripcion_vacia;
			continue;
		}


		while (*esp == ' '){
			esp++;
		}
		while ((esp = strchr(esp, ' ')) != NULL) {
			while (*esp == ' '){
				esp++;
			}
			if (*esp == 0){
				break;
			}
			ptr_voc++->tra = transc;
			transc = strtok(NULL, " ");
		}
		ptr_voc++->tra = transc;
		transc = strtok(NULL, " ");
		aux++;
	}

	ptr_voc = vocabulario;
	aux1 = strtok(palabras, " ");
	while (aux1 != NULL) {
		ptr_voc++->pal = aux1;
		aux1 = strtok(NULL, " ");
	}
	ptr_voc->pal = NULL;

	ptr_voc = vocabulario;

	while (ptr_voc->pal) {
		if (ptr_voc->tra[strlen(ptr_voc->tra) - 1] != '^'){
			ptr_voc->final_atono = 1;
		}
		else {
			ptr_voc->final_atono = 0;
		}
		if (ptr_voc->tra[1] != '^'){
			ptr_voc->inicio_atono = 1;
		}
		else {
			ptr_voc->inicio_atono = 0;
		}
		if (ftra && if_transcripcion->busca(ptr_voc->pal) >= 0){
			ptr_voc->tra = if_transcripcion->devuelveTranscripcion();
		}
//		if ((aux1 = strchr(ptr_voc->tra, '^')) != NULL){
//			_copia_cadena(aux1, (aux1 + 1));
//		}
		ptr_voc++;
	}

	ptr_voc = vocabulario;

	while (ptr_voc->pal) {
		ntransc = 1;
		transcripciones[0] = ptr_voc->tra;

		if (alternativas) {
			if (ptr_voc->final_atono && (ptr_voc + 1)->pal
					&& (ptr_voc + 1)->inicio_atono) {
				char *fin = &ptr_voc->tra[strlen(ptr_voc->tra) - 1];
				char *com = (ptr_voc + 1)->tra;

				if ((*fin == 'e'
							&& (*com == 'a' || *com == 'e' || *com == 'E'
								|| *com == 'i' || *com == 'o' || *com == 'O'
								|| *com == 'u')) || (*fin == 'o' && (*com == 'o'
									|| *com == 'O'
									|| *com == 'u'))
						|| (*fin == 'a'
							&& (*com == 'a' || *com == 'O' || *com == 'E'))) {

					transcripciones[1] = strdup(ptr_voc->tra);
					fin = strchr(transcripciones[1], 0);
					fin--;
					*fin = 0;
					ntransc++;
				}
			}

			for (int j = 0; j < nvariantes; j++) {
				//Si la variante es aplicable
				if (TestVariante(ptr_voc->pal, &variantes[j], importancia)) {
					//Se recorren todas las transcripciones
					int nt1;
					nt1 = ntransc;
					for (int k = 0; k < nt1; k++) {
						char *kk;
						kk = AplicaVariante(transcripciones[k], &variantes[j]);
						//Si ha sido correctamente aplicada
						if (kk != NULL) {
							if (!TranscIgual(kk, transcripciones, ntransc)){
								transcripciones[ntransc++] = kk;
							}
							else{
								free(kk);
							}
						}
					}
				}
			}					//for n_variantes
		}						//if (opciones.alternativas)

		fprintf(fd, "%s\t", ptr_voc->pal);
		for (int k = 0; k < ntransc; k++)
			fprintf(fd, "%s ", transcripciones[k]);

		putc('\n', fd);

		for (int k = 1; k < ntransc; k++)
			free(transcripciones[k]);

		ptr_voc++;
	}
	return 0;
}


/** 
 * \author fmendez
 * \remarks
 * 
 * \param opciones 
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::inicializa(t_opciones * opciones)
{

	opciones->fstdout ? fd = stdout : fd = NULL;
	tra = opciones->tra;
	por_palabras = opciones->por_palabras;
	alternativas = opciones->alternativas;
	ftra = opciones->ftra;
	fsalida = opciones->fsalida;
	inicializar_reglas_transcripcion(opciones->idioma);
	return 0;
}


/** 
 * \author fmendez
 * \remarks
 */
void Transcripcion::finaliza()
{

	if (fd != NULL && fd != stdout) {
		fclose(fd);
		fd = NULL;
	}
}

/**
 * \author cardenal
 * \author fmendez
 * \remarks
 * 
 * \param transc 
 * \param *lista 
 * \param n 
 * 
 * \return 
 */
int TranscIgual(char *transc, char **lista, int n)
{
	for (int i = 0; i < n; i++)
		if (!strcmp(transc, lista[i]))
			return 1;
	return 0;
}

/** 
 * \author fmendez
 * \remarks Constructor de la clase Transcripción. Asigna memoria para las
 * palabras, transcripciones y vocabulario.
 */
Transcripcion::Transcripcion(){
	importancia = 0;

	if ((palabras = (char *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA * sizeof(char))) == NULL) {
		fprintf(stderr, "Transcripcion: Error al asignar memoria en palabras\n");
	}

	if ((transcripciones = (char **) malloc(MAX_TRANSCRIPCIONES * sizeof(char *))) == NULL) {
		fprintf(stderr, "Transcripcion: Error al asignar memoria en transcripciones\n");
	}

	if ((vocabulario = (t_vocabulario *) malloc(LONX_MAX_FRASE_SIL_E_ACENTUADA / 2 * sizeof(t_vocabulario))) == NULL) {
		fprintf(stderr, "Transcripcion: Error al asignar memoria en vocabulario\n");
	}
}

/** 
 * \author fmendez
 * \remarks Destructor de la clase Transcripción. Libera memoria de las
 * palabras, transcripciones y vocabulario.
 */
Transcripcion::~Transcripcion(){
	free(palabras);
	free(transcripciones);
	free(vocabulario);
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param idioma  
 *
 * \return 
 */
void Transcripcion::inicializar_reglas_transcripcion(char idioma) {

	if (idioma==GALEGO) {                   // Regras para o galego
		reglas_transcripcion[' '] = (t_regla *)&regla_cadeas_iniciadas_por_espacio[0];
		reglas_transcripcion['n']	= (t_regla *)&regla_cadeas_iniciadas_por_n[0];
		reglas_transcripcion['x']	= (t_regla *)&regla_cadeas_iniciadas_por_x[0];
		reglas_transcripcion['#']	= (t_regla *)&regla_cadeas_iniciadas_por_silencio[0];
		reglas_transcripcion['-'] = (t_regla *)&regla_cadeas_iniciadas_por_guion[0];
	}
	else {                          // Regras para o castelan
		reglas_transcripcion[' '] = (t_regla *)&cast_regla_cadeas_iniciadas_por_espacio[0];
		reglas_transcripcion['n']	= (t_regla *)&cast_regla_cadeas_iniciadas_por_n[0];
		reglas_transcripcion['x']	= (t_regla *)&cast_regla_cadeas_iniciadas_por_x[0];
		reglas_transcripcion['#']	= (t_regla *)&cast_regla_cadeas_iniciadas_por_silencio[0];
		reglas_transcripcion['-'] = (t_regla *)&cast_regla_cadeas_iniciadas_por_guion[0];
	}
	reglas_transcripcion[(unsigned char)'^'] = (t_regla *)&regla_cadeas_iniciadas_por_acento_prosodico[0];
	reglas_transcripcion['a'] = (t_regla *)&regla_cadeas_iniciadas_por_a[0];
	reglas_transcripcion['e'] = (t_regla *)&regla_cadeas_iniciadas_por_e[0];
	reglas_transcripcion[e_acentuada] = (t_regla *)&regla_cadeas_iniciadas_por_e_acentuado[0];
	reglas_transcripcion['i'] = (t_regla *)&regla_cadeas_iniciadas_por_i[0];
	reglas_transcripcion['o'] = (t_regla *)&regla_cadeas_iniciadas_por_o[0];
	reglas_transcripcion[o_acentuada] = (t_regla *)&regla_cadeas_iniciadas_por_o_acentuado[0];
	reglas_transcripcion['u'] = (t_regla *)&regla_cadeas_iniciadas_por_u[0];
	reglas_transcripcion[','] = (t_regla *)&regla_cadeas_iniciadas_por_coma[0];
	reglas_transcripcion['b'] = (t_regla *)&regla_cadeas_iniciadas_por_b[0];
	reglas_transcripcion['c'] = (t_regla *)&regla_cadeas_iniciadas_por_c[0];
	reglas_transcripcion['d'] = (t_regla *)&regla_cadeas_iniciadas_por_d[0];
	reglas_transcripcion['f'] = (t_regla *)&regla_cadeas_iniciadas_por_f[0];
	reglas_transcripcion['g'] = (t_regla *)&regla_cadeas_iniciadas_por_g[0];
	reglas_transcripcion['h'] = (t_regla *)&regla_cadeas_iniciadas_por_h[0];
	reglas_transcripcion['j'] = (t_regla *)&regla_cadeas_iniciadas_por_j[0];
	reglas_transcripcion['k'] = (t_regla *)&regla_cadeas_iniciadas_por_k[0];
	reglas_transcripcion['l'] = (t_regla *)&regla_cadeas_iniciadas_por_l[0];
	reglas_transcripcion['m'] = (t_regla *)&regla_cadeas_iniciadas_por_m[0];
	reglas_transcripcion[enne_minuscula]= (t_regla *)&regla_cadeas_iniciadas_por_enne[0];
	reglas_transcripcion['p'] = (t_regla *)&regla_cadeas_iniciadas_por_p[0];
	reglas_transcripcion['q'] = (t_regla *)&regla_cadeas_iniciadas_por_q[0];
	reglas_transcripcion['r'] = (t_regla *)&regla_cadeas_iniciadas_por_r[0];
	reglas_transcripcion['s'] = (t_regla *)&regla_cadeas_iniciadas_por_s[0];
	reglas_transcripcion['t'] = (t_regla *)&regla_cadeas_iniciadas_por_t[0];
	reglas_transcripcion['v'] = (t_regla *)&regla_cadeas_iniciadas_por_v[0];
	reglas_transcripcion['w'] = (t_regla *)&regla_cadeas_iniciadas_por_w[0];
	reglas_transcripcion['y'] = (t_regla *)&regla_cadeas_iniciadas_por_y[0];
	reglas_transcripcion['z'] = (t_regla *)&regla_cadeas_iniciadas_por_z[0];
	reglas_transcripcion[u_con_dierese] = (t_regla *)&regla_cadeas_iniciadas_por_u_con_dierese[0];
	reglas_transcripcion[i_con_dierese] = (t_regla *)&regla_cadeas_iniciadas_por_i_con_dierese[0];
	reglas_transcripcion[e_con_dierese] = (t_regla *)&regla_cadeas_iniciadas_por_e_con_dierese[0];
	reglas_transcripcion[cedilla_minuscula] = (t_regla *)&regla_cadeas_iniciadas_por_cedilla[0];
	reglas_transcripcion['.'] = (t_regla *)&regla_cadeas_iniciadas_por_punto[0];
	reglas_transcripcion[';'] = (t_regla *)&regla_cadeas_iniciadas_por_punt_e_coma[0];
	reglas_transcripcion[':'] = (t_regla *)&regla_cadeas_iniciadas_por_dous_puntos[0];
	reglas_transcripcion[comillas_simples] = (t_regla *)&regla_cadeas_iniciadas_por_simples_cominhas[0];
	reglas_transcripcion[comillas_dobles] = (t_regla *)&regla_cadeas_iniciadas_por_comillas_dobles[0];
	reglas_transcripcion[apertura_interrogacion] = (t_regla *)&regla_cadeas_iniciadas_por_apertura_interrogacion[0];
	reglas_transcripcion[apertura_exclamacion] = (t_regla *)&regla_cadeas_iniciadas_por_exclamacion_apertura[0];
	reglas_transcripcion[(unsigned char)'?'] = (t_regla *)&regla_cadeas_iniciadas_por_peche_interrogacion[0];
	reglas_transcripcion[(unsigned char)'!'] = (t_regla *)&regla_cadeas_iniciadas_por_pech_exclamacion[0];
	reglas_transcripcion[(unsigned char)'*'] = (t_regla *)&regla_cadeas_iniciadas_por_asterisco[0];
	reglas_transcripcion[(unsigned char)'('] = (t_regla *)&regla_cadeas_iniciadas_por__apertura_parentese[0];
	reglas_transcripcion[(unsigned char)')'] = (t_regla *)&regla_cadeas_iniciadas_po_peche_parentese[0];
	reglas_transcripcion[(unsigned char)'à'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'è'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ì'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ò'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ù'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'â'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ê'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'î'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ô'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'û'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ä'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'ö'] = (t_regla *)&regla_cadeas_iniciadas_por_vocal_minuscula_rara[0];
	reglas_transcripcion[(unsigned char)'·'] = (t_regla *)&regla_cadeas_iniciadas_por_suspensivos[0];
	reglas_transcripcion['%'] = NULL;

}


/** 
 * \remarks Collemos un trozo de frase_sil_e_acentuada de lonxitude igual a do
 * consecuente e comparamola co antecedente de cada regla hasta que sexan
 * iguais ou ben hasta que non haia mais reglas nas que o consecuente empece
 * pola letra actual de frase fonetica. Nese caso saltase esa letra e volvese a
 * intentar buscar outra regla que empece polo seguinte caracter.  
 * \author fmendez 
 *
 * \return 
 */
inline void Transcripcion::busqueda_de_regla(){

	const char * aux;
	char * aux2;

	if (regla == NULL) {
		return;
	}

	aux = regla->antecedente;
	while (aux){
		aux2 = pos_actual_frase_sil;
		for(;*aux;){
			if (*aux != *aux2){
				regla++;
				aux = regla->antecedente;
				break;
			}
			aux++;
			if (*aux == 0) { 
				return;
			}
			aux2++;
		}
	}
	regla = NULL;
}


/** 
 * \remarks Collemos un trozo de frase_sil_e_acentuada de lonxitude igual a do
 * consecuente e comparamola co antecedente de cada regla hasta que sexan
 * iguais ou ben hasta que non haia mais reglas nas que o consecuente empece
 * pola letra actual de frase fonetica. Nese caso saltase esa letra e volvese a
 * intentar buscar outra regla que empece polo seguinte caracter.  
 * \author fmendez 
 * \param bufer trozo de frase_sil_e_acentuada
 * \return 
 */
inline void Transcripcion::busqueda_de_regla(char * bufer){

	const char * aux;
	char * aux2;

	if (regla == NULL) {
		return;
	}

	aux = regla->antecedente;
	while (aux){
		aux2 = bufer;
		for(;*aux;){
			if (*aux != *aux2){
				regla++;
				aux = regla->antecedente;
				break;
			}
			aux++;
			if (*aux == 0) { 
				return;
			}
			aux2++;
		}
	}
	regla = NULL;
}

/**
 * \brief Realiza la transcripción fonética
 * \author fmendez 
 *
 * \param f_sep  
 * \param f_sil  
 * \param f_fon  
 * \param idioma  
 *
 * \return 
 */
int Transcripcion::transcribe(t_frase_separada * f_sep, char * f_sil, char * f_fon, char idioma){

	char * rec_pal;
	char * rec_pal_sil_acen;
	t_palabra_proc pal;
	t_palabra_proc pal_sil_acen;

	t_frase_separada *item=f_sep;
	strcpy(f_sil,"## ");

	while(*item->pal){
		//atono_ou_tonico_aberto_ou_pechado_e_w_x(frase_sil_e_acentuada,item);
		if (item->clase_pal!=SIGNO_PUNTUACION && item->clase_pal!=CADENA_DE_SIGNOS){
			item->pal_transformada != NULL ?
				rec_pal = item->pal_transformada:
				rec_pal = item->pal;
			rec_pal_sil_acen = item->pal_sil_e_acentuada;

			while (*rec_pal){
				*pal=0;
				*pal_sil_acen=0;
				rec_pal=extraer_texto_entre_espacios(rec_pal,pal);
				rec_pal_sil_acen=extraer_texto_entre_espacios(rec_pal_sil_acen,pal_sil_acen);
				tratamento_das_excepcions_da_xe(pal,pal_sil_acen);
				tratamento_das_excepcions_da_w(pal,pal_sil_acen);
				//strcpy(pal,pal_sil_acen);
				//acentuar_prosodicamente(pal,pal_sil_acen);
				strcat(f_sil,pal_sil_acen);
				strcat(f_sil," ");
			}
		}
		item++;
	}

	strcat(f_sil,"##");
	pos_actual_frase_sil=f_sil;

	while(*pos_actual_frase_sil != 0){
		regla = reglas_transcripcion[(unsigned char)*pos_actual_frase_sil];
		busqueda_de_regla();
		if (regla!=NULL){
			strcat(f_fon,regla->consecuente);
			pos_actual_frase_sil+=regla->lonx_a_sustituir;

		}

		if (strlen(f_fon)>LONX_MAX_FRASE_SIL_E_ACENTUADA-15) break;
	}

	return 0;
}

/**
 * \brief  Realiza la transcripción fonética
 * \author fmendez 
 *
 * \remarks Para transformar a alofonos primeiro metemos nun bufer de 40 posicions
 *  o cacho de texto desde o ultimo lugar transformado (isto e para selecionar
 *  despoios de este bufer o tama±o adecuado para comparalo cos antecedentes).
 *  Cada regla consta de:
 *  Antecedente: trozo de texto que ten que ser igual para poder aplicar a sus
 *  titucion de texto.
 *  lonx_texto_a_sustituir: trozo do antecedente que se vai sustituir se este
 *  e igual o trozo inicial de bufer. Normalmente e igual a lonxitude do ante-
 *  cedente pero pode ser menor para por exemplo permitir despois a aplicacion
 *  da seguinte regla para non solapala. Exemplo:
 *
 *
 *  ca^-be
 *  Reglas aplicadas
 *  antecedente    lonx_a_sustituir     consecuente
 *  ca              1                    k
 *  a^-b            4                    a^-B
 *  e               1                    e
 *
 *
 *  Consecuente: trozo polo que se sustitue. A transcripcion sempre e menor
 *  ou igual ca transcripcion de todo o antecedente.
 *
 *  As reglas agrupanse pola letra inicial do antecedente, pois estes son
 *  os casos que hai que comprobar para sustituir un trozo de texto. Orde-
 *  nanse  de casos mais especificos (trozo de texto mais longo) ate os mais xerais.
 *        Sempre hai que
 *  ter en conta todolos casos, e decir, hai sempre que ter en conta os mais
 *  especificos. Asi examinanse estes antes. O primeiro caso que se encontre
 *  asegura que esta e a regla que hai que aplicar, e asi xa non se necesita
 *  considerar mais casos e a busqueda (no sentido de menor numero de compara-
 *  cions e optima.
 *  Que existan moitos antecedentes que empecen igual, implica que hai que
 *  comparar mais antecedentes o mellor para chegar ate o caso que buscamos
 *  pero estes tamen implican unha sustitucion de texto moito maior e por
 *  tanto tamen un menor numero de reglas que hai que aplicar.
 *
 *  A ordenacion  das reglas nas que o antecedente empeza pola mesma letra,
 *  pode ser de calquer xeito (podemos colocar primeiro as mais frecuentes
 *  que deste xeito se encontrarìan antes) pero se un antecedente esta contido
 *  en outro, sempre se ten que colocar primeiro o mais longo e despois o mais
 *  corto. Isto e asi porque como a busqueda finaliza o encontrar o primeiro
 *  antecedente que adapte, asi asegurase que se te±an sempre en conta os
 *  casos mais especificos antes cos mais xerais.
 *  En caso de que non se encontre un antecedente avanzase un caracter e empeza
 *  a busqueda de novo.
 *
 * \param f_sil  
 * \param f_fon  
 * \param idioma  
 *
 * \return 
 */
int Transcripcion::transformar_a_alofonos(char * f_sil,  char * f_fon, char idioma){


	//char bufer_texto_a_sustituir[41];
	//t_regla *regla;
	char comentario[LONX_MAX_FRASE_SIL_E_ACENTUADA];
	unsigned char pos_comentario=0;
	char insertar_en;
	char *nueva_frase_sil; //, *pos_actual_nueva_frase_sil;
	Estructura_rupturas *rupturas, *apunta_ruptura;
	int numero_rupturas;


	if (crea_cadena_rupturas(f_sil, &nueva_frase_sil, &rupturas, &numero_rupturas))
		return 1;

	apunta_ruptura = rupturas;
	//pos_actual_nueva_frase_sil = nueva_frase_sil;
	pos_actual_frase_sil = nueva_frase_sil;

	//strcpy(bufer_texto_a_sustituir,pos_actual_frase_sil);
	strncpy(bufer_texto_a_sustituir,pos_actual_frase_sil,40);
	*(bufer_texto_a_sustituir+40)=0;
	while(*bufer_texto_a_sustituir!=0){

		if ((numero_rupturas) && (apunta_ruptura - rupturas < numero_rupturas) ) {
			if ((int) (pos_actual_frase_sil - nueva_frase_sil) > apunta_ruptura->posicion) {
				if (f_fon[0] == '\0') {
					fprintf(stderr, "Error en transformar_a_alofonos: frase fonética nula.\n");
					return 1;
				}

				char *apunta_f_fon = f_fon + strlen(f_fon) - 1;
				while (*apunta_f_fon != ' ') {
					apunta_f_fon--;
					if (apunta_f_fon < f_fon) {
						fprintf(stderr, "Error en transformar_a_alofonos: frase fonética nula.\n");
						return 1;
					}
				}

				// Comprobamos si antes hay un comentario:
				if (apunta_f_fon > f_fon + 1)
					if (*(apunta_f_fon - 1) == '%') {
						apunta_f_fon -= 2;
						while (*apunta_f_fon != '%') {
							apunta_f_fon--;
							if (apunta_f_fon < f_fon) {
								fprintf(stderr, "Error en transformar_a_alofonos: frase fonética nula.\n");
								return 1;
							}
						}
						apunta_f_fon--;

					}

				if (apunta_ruptura->tipo == 0)
					insertar_string(apunta_f_fon, " #%ruptura-entonativa%#");
				else
					insertar_string(apunta_f_fon, " #%ruptura-coma%#");
				apunta_ruptura++;
			}

		}
		detectar_comentario(comentario,&pos_comentario,bufer_texto_a_sustituir,&insertar_en);
		//regla=asignar_principio_busqueda_regla(*bufer_texto_a_sustituir, idioma);
		regla = reglas_transcripcion[(unsigned char)*bufer_texto_a_sustituir];
		busqueda_de_regla(bufer_texto_a_sustituir);
		if (regla!=NULL){
			strcat(f_fon,regla->consecuente);
			pos_actual_frase_sil+=regla->lonx_a_sustituir;

			pos_actual_frase_sil=insertar_comentario(comentario,pos_comentario,f_fon,regla,insertar_en,pos_actual_frase_sil);
		}
		/*Insertarase o comentario so se se detectou e se a regla aplicada o sobrepasou.*/
		else {
			pos_actual_frase_sil=insertar_comentario(comentario,pos_comentario,f_fon,regla,insertar_en,pos_actual_frase_sil);
		}
		// strcpy(bufer_texto_a_sustituir,pos_actual_frase_sil);
		strncpy(bufer_texto_a_sustituir,pos_actual_frase_sil,40);
		*(bufer_texto_a_sustituir+40)=0;

		if (strlen(f_fon)>LONX_MAX_FRASE_SIL_E_ACENTUADA-15) return 0;
		/* intentamos non truncar a ultima palabra se hai que truncar a frase
			 o non haber suficiente espacio no array. Consideramos que se nece
			 sitan como max. 15 caracteres para unha palabra*/
	}

	free(rupturas);
	free(nueva_frase_sil);

	return 0;

}

/**
 * \author fmendez
 * \remarks Elimina los comentarios enmarcados en #...# de una frase fonética
 * \param[in] fi frase de entrada
 * \param[out] fo frase de salida, sin comentarios
 * \return 0 en ausencia de error
 */
int Transcripcion::elimina_comentarios(char * fi, char * fo)
{

	while (*fi) {
		if (*fi == '#') {
			fi++;
			while (*fi && *fi != '#') {
				fi++;
			}
			if (*fi) {
				fi++;
				continue;
			}
			else {
				fprintf(stderr, "Error Transcripcion::elimina_comentarios: Comentario sin cerrar: %s \n", fi);
				return 1;
			}
		}
		*fo++ = *fi++;
	}
	*fo = 0;
	return 0;
}


/**
 * \author fmendez
 * \remarks Extrae y asigna a cada palabra en frase_separada su transcripción a partir de frase_fonetica
 * \param ffon frase fonética de entrada
 * \return 0 en ausencia de error 
 */
int Transcripcion::divide_frase_fonetica(char * ffon, t_frase_separada * fsep)
{

	t_frase_separada *fs = fsep;
	char *pt;

	if (elimina_comentarios(ffon, transcripcion)){
		return 1;
	}
	saca_tipo_prop(transcripcion);

	pt = strtok(transcripcion, " ");
	while (pt != NULL) {
		while (fs->clase_pal == SIGNO_PUNTUACION || fs->clase_pal == CADENA_DE_SIGNOS || *fs->pal_sil_e_acentuada == 0) {
			fs->tr_fon = transcripcion_vacia;
			fs++;
		}
		fs->tr_fon = pt;
		pt = strtok(NULL, " ");
		fs++;
	}
	while (*fs->pal && (fs->clase_pal == SIGNO_PUNTUACION || fs->clase_pal == CADENA_DE_SIGNOS || *fs->pal_sil_e_acentuada == 0)) {
		fs->tr_fon = transcripcion_vacia;
		fs++;
	}

	return 0;
}

/** 
 * \author fmendez
 * \brief Realiza la transcripción fonética 
 * 
 * \param[in] fs frase separada
 * \param[in] f_sil frase silabeada y acentuada
 * \param[out] f_fon transcripción fonética de la frase
 * \param[in] idioma idioma 
 * 
 * \return 0 en ausencia de error
 */
int Transcripcion::transcribir(t_frase_separada * fs, char * f_sil, char * f_fon, char idioma)
{
	if (transcribe(fs, f_sil, f_fon, idioma)){
		fprintf(stderr, "Error en Transcripcion::transcribir: %s\n", f_sil);
		return 1;
	}
	//frase_separada = fs;
	return divide_frase_fonetica(f_fon, fs);
}


