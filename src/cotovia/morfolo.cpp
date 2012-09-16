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
 
#include <stdio.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "sintagma.hpp"
#include "verbos.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp"
#include "morfolo.hpp"


/**  \fn  void Analisis_morfologico::identificar_grupos_conxuntivos(t_frase_separada * frase_separada)

	Esta función detecta e asigna o código correspondente as palabras que
	forman unha locución conxuntiva. Todas as palabras que pertenzan a unha
	locución quedan marcadas co mesmo código.
	*/
void Analisis_morfologico::identificar_grupos_conxuntivos(t_frase_separada * frase_separada)
{

	int numero_de_avances;

	while (*frase_separada->pal) {
		numero_de_avances = diccionarios->buscarConxunciones(frase_separada, idioma);
		frase_separada += numero_de_avances;
	}
}

void Analisis_morfologico::analise_morfoloxica(t_frase_separada *
		frase_separada, t_opciones opciones) {
	/*
		 Esta función recibe como entrada frase_separada e analiza morfoloxicamente
		 todolos elementos deste arrai. Pon en cat_gramatical a posible ou posibles
		 categorias gramaticais. Non emprega reglas de contexto para desambiguar
		 estas posibles categorías.
		 */
	t_palabra_proc palabra_auxiliar;
	t_infinitivo infor[TEMPOS_POSIBLES];

	unsigned char index, genero, numero;

	unsigned char lista_de_tempos_verdadeiros[TEMPOS_POSIBLES] = "";
	int num_cat;
	t_frase_separada *recorre_frase_separada = frase_separada;
	int pos; //, pos1, pos2;

	idioma = opciones.idioma;

	//ZA:

	/*
		 if (dic_cargados==0)
		 if (-1==carga_diccionarios()) {
		 printf("No pude cargar diccionarios. No se analiza morfológicamente\n");
		 return;
		 }
		 if (dic_cargados==-1) return;
		 */

	while (recorre_frase_separada->pal[0]) {
		switch (recorre_frase_separada->clase_pal) {
			case NUM_ROMANO:
				if (recorre_frase_separada != frase_separada) {
					if ((idioma == GALEGO && strstr((recorre_frase_separada - 1)->lema, "século") != NULL) ||
							(idioma == CASTELLANO && strstr((recorre_frase_separada - 1)->lema, "siglo") != NULL)) {
						recorre_frase_separada->cat_gramatical[0] = NUME_CARDI_DET;
					}
					else {
						recorre_frase_separada->cat_gramatical[0] = NUME_ORDI;
					}
				}
				else {
					recorre_frase_separada->cat_gramatical[0] = NUME_ORDI;
				}
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			case CARDINAL:
			case NUMERO_CON_PUNTO:
			case DECIMAL:
				recorre_frase_separada->cat_gramatical[0] = NUME_CARDI;
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			case ORDINAL_MASCULINO:
			case ORDINAL_FEMENINO:
				recorre_frase_separada->cat_gramatical[0] = NUME_ORDI;
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			case VALOR_DE_GRADOS:
			case DATA:
			case HORAS:
			case CADENAS_DE_OPERACIONS:
			case NUMEROS_SEPARADOS_POR_GUIONS:
			case INICIAL:
			case ABREVIATURA:
				//    case PALABRA_CONSONANTES_MINUSCULAS:/* caso de "ph" ou "o s" */
				/* Estas unidades para a analise sintactica poden ser tratados como un nome
					 xa que o conteñen o final (caso de " o 3%" => o tres por cento" ou
					 len a un */
				recorre_frase_separada->cat_gramatical[0] = NOME;
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			case PORCENTUAL:
				recorre_frase_separada->cat_gramatical[0] = NUME_CARDI_PRON;
				recorre_frase_separada->cat_gramatical[1] = 0;
				recorre_frase_separada->xenero = MASCULINO;
				recorre_frase_separada->numero = SINGULAR;
				break;
			case PALABRA_CONSONANTES_MINUSCULAS:	/* caso de "ph" ou "o s" */
			case PALABRA_NORMAL_EMPEZA_MAY:
			case PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA:
			case PALABRA_TODO_MAIUSCULAS:
				/* Creo que non se debe identificar que unha palabra que empece por maiuscula
					 sexa un nome, ainda que non estea no princio do programa, xa que pode
					 haber cousas como "...A súa Maxestade..." e "a" non e nome propio, qinda que
					 forma parte del */
			case PALABRA_CON_GUION_NO_MEDIO:
			case PALABRA_NORMAL:
				/*Neste tipo de palabras búscase primeiro se se encontran na lista
					de palabras función. Se non estan aquí enton solo poden ser ou verbos
					adxectivos ou nomes. Mírase primeiro se son verbos e se non asignaselles
					a categoría de nomes. */
				if (recorre_frase_separada->pal_transformada != NULL)
					strcpy(palabra_auxiliar, recorre_frase_separada->pal_transformada);
				else
					strcpy(palabra_auxiliar, recorre_frase_separada->pal);
				/* Comenzámola análise morfolóxica propiamente dita                           */
				//fmendez nuevo
				// se mira si es palabra funcion
				pos = diccionarios->busca_dic_nomes(palabra_auxiliar, recorre_frase_separada, idioma);
				//si no es palabra funcion o bien es verbo, se analiza como verbo
				//#ifdef REGLAS_DE_ALOFONOS_DO_GALEGO
				if (pos==-1 || esta_palabra_ten_a_categoria_de(VERBO, recorre_frase_separada)
                      || esta_palabra_ten_a_categoria_de(PARTICIPIO, recorre_frase_separada)
                      || esta_palabra_ten_a_categoria_de(XERUNDIO, recorre_frase_separada)) {
//        if (!esta_palabra_tivera_a_categoria_de(VERBO, recorre_frase_separada)) {
					lista_de_tempos_verdadeiros[0] = 0;
					memset(infor, 0, sizeof(infor));
					diccionarios->analizar_verbos(palabra_auxiliar, lista_de_tempos_verdadeiros, infor, opciones.idioma);
					if (lista_de_tempos_verdadeiros[0] > 0) {
						//index=0;
						if ((num_cat = strlen((char *) recorre_frase_separada->cat_gramatical)) < N_CAT) {
							if (!esta_palabra_ten_a_categoria_de(VERBO, recorre_frase_separada))
								recorre_frase_separada->cat_gramatical[num_cat] = VERBO;
						}
						strncpy((char *) recorre_frase_separada->cat_verbal,
								(char *) lista_de_tempos_verdadeiros, TEMPOS_POSIBLES);
						recorre_frase_separada->cat_verbal[TEMPOS_POSIBLES] = 0;
						memcpy(recorre_frase_separada->inf_verbal, infor,
								TEMPOS_POSIBLES * sizeof(t_infinitivo));
						/*
							 while(strcmp(infor[index].infinitivo,"")) {
							 strcpy(recorre_frase_separada->inf_verbal[index].infinitivo,infor[index].infinitivo);
							 strcpy(recorre_frase_separada->inf_verbal[index].raiz,infor[index].raiz);
							 recorre_frase_separada->inf_verbal[index].modelo=infor[index].modelo;
							 index++;
							 }
							 */
						char otros_tiempos;
						genero = recorre_frase_separada->xenero;
						numero = recorre_frase_separada->numero;
						index = formas_no_personales(lista_de_tempos_verdadeiros, &genero, &numero, &otros_tiempos);
						if (index) {
							if ((strstr(recorre_frase_separada->pal, "-") == NULL)
									&& (*(recorre_frase_separada->inf_verbal[0].enclitico.enclitico) == 0)) {
								anade_categoria(index, recorre_frase_separada);
								if (otros_tiempos == 0)
									elimina_categoria(VERBO, recorre_frase_separada);
								recorre_frase_separada->xenero = genero;
								recorre_frase_separada->numero = numero;
							}

						}
					}
				}
				//#endif
				//si no es palabra funcion, (aunque puede que ya sea verbo),se mira si es nombre
				//se buscan adjetivos
				//      if (pos==-1)
				//pos1 = diccionarios->busca_dic_adxe(palabra_auxiliar, recorre_frase_separada, idioma);
				//				if (pos == -1)
				//pos2 = diccionarios->busca_dic_nomes(palabra_auxiliar, recorre_frase_separada, idioma);
//ZA. Derivativos.
//De momento, los derivativos sólo entran si no se ha encontrado nada
//(palabra completamente desconocida).
				if ( pos == -1 && *lista_de_tempos_verdadeiros == 0 )
					diccionarios->buscaDerivativos(palabra_auxiliar,recorre_frase_separada,idioma);

				//  Adverbios acabados en mente
				//za: 1ª condición, chorras convergencia con gbm_regexp
				if (!esta_palabra_ten_a_categoria_de(VERBO, recorre_frase_separada)
						&& !esta_palabra_ten_a_categoria_de(NOME, recorre_frase_separada)
						&& !esta_palabra_ten_a_categoria_de(ADXECTIVO, recorre_frase_separada)) {
					if (strlen(recorre_frase_separada->pal) > 5
							&& !strcmp(&recorre_frase_separada->pal[strlen(recorre_frase_separada->pal) - 5], "mente")
							&& !esta_palabra_ten_a_categoria_de(ADVE_MODO, recorre_frase_separada))
						anade_categoria(ADVE_MODO, recorre_frase_separada);
					//recorre_frase_separada->cat_gramatical[0]=ADVE;
					else if (recorre_frase_separada->cat_gramatical[0] == 0) {

						if (((recorre_frase_separada != frase_separada) &&
									(recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY))
								|| (recorre_frase_separada->clase_pal == PALABRA_TODO_MAIUSCULAS)) {

							if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY)
								recorre_frase_separada->clase_pal =
									PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA;

							recorre_frase_separada->cat_gramatical[0] = NOME;
							recorre_frase_separada->cat_gramatical[1] = 0;
						}
						else {
							recorre_frase_separada->cat_gramatical[0] = NOME;
							recorre_frase_separada->cat_gramatical[1] = ADXECTIVO;
							recorre_frase_separada->cat_gramatical[2] = 0;

							if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY)
								recorre_frase_separada->clase_pal =
									PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA;
						}
					}

				}
				//      busca_dic_ambiguas_poco(palabra_auxiliar,recorre_frase_separada);

				break;


			case SIGNO_PUNTUACION:
				switch ((unsigned char) recorre_frase_separada->pal[0]) {
					/* encher cun codigo para coma, outro para punto,etc */
					case '.':
						recorre_frase_separada->cat_gramatical[0] = PUNTO;
						break;
					case ',':
						if (strcmp(recorre_frase_separada->pal, SIMBOLO_RUPTURA_ENTONATIVA) == 0)
							recorre_frase_separada->cat_gramatical[0] = RUPTURA_ENTONATIVA;
						else
							if (strcmp(recorre_frase_separada->pal, SIMBOLO_RUPTURA_COMA) == 0)
								recorre_frase_separada->cat_gramatical[0] = RUPTURA_COMA;
							else
								recorre_frase_separada->cat_gramatical[0] = COMA;
						break;
					case ';':
						recorre_frase_separada->cat_gramatical[0] = PUNTO_E_COMA;
						break;
					case ':':
						recorre_frase_separada->cat_gramatical[0] = DOUS_PUNTOS;
						break;
					case (unsigned char) '·':
						recorre_frase_separada->cat_gramatical[0] = PUNTOS_SUSPENSIVOS;
						break;
					case apertura_interrogacion:
						recorre_frase_separada->cat_gramatical[0] = APERTURA_INTERROGACION;
						break;
					case '?':
						recorre_frase_separada->cat_gramatical[0] = PECHE_INTERROGACION;
						break;
					case apertura_exclamacion:
						recorre_frase_separada->cat_gramatical[0] = APERTURA_EXCLAMACION;
						break;
					case '!':
						recorre_frase_separada->cat_gramatical[0] = PECHE_EXCLAMACION;
						break;
					case '(':
						recorre_frase_separada->cat_gramatical[0] = APERTURA_PARENTESE;
						break;
					case ')':
						recorre_frase_separada->cat_gramatical[0] = PECHE_PARENTESE;
						break;
					case '[':
						recorre_frase_separada->cat_gramatical[0] = APERTURA_CORCHETE;
						break;
					case ']':
						recorre_frase_separada->cat_gramatical[0] = PECHE_CORCHETE;
						break;
					case '-':
						recorre_frase_separada->cat_gramatical[0] = GUION;
						break;
					case '_':
						recorre_frase_separada->cat_gramatical[0] = GUION_BAIXO;
						break;
					case '\'':
						recorre_frase_separada->cat_gramatical[0] = SIMPLES_COMINNAS;
						break;
					case '"':
						recorre_frase_separada->cat_gramatical[0] = DOBLES_COMINNAS;
						break;
					default:
						//recorre_frase_separada->cat_gramatical[0]=GUION;
						recorre_frase_separada->cat_gramatical[0] = SIGNO;
						//recorre_frase_separada->cat_gramatical[1]=GUION;
						break;
				}
				break;
			case CADENA_DE_SIGNOS:
				recorre_frase_separada->cat_gramatical[0] = SIGNO;
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			case SIN_CLASIFICAR:
				//recorre_frase_separada->cat_gramatical[0]=CAT_NON_CLASIFICADA;
				recorre_frase_separada->cat_gramatical[0] = NOME;
				recorre_frase_separada->cat_gramatical[1] = 0;
				break;
			default:
				recorre_frase_separada->cat_gramatical[0] = NOME;
				recorre_frase_separada->cat_gramatical[1] = 0;

				if (recorre_frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY)
					recorre_frase_separada->clase_pal =
						PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA;

				//          recorre_frase_separada->cat_gramatical[1] = ADXECTIVO;
		}						/* fin do switch */
		recorre_frase_separada++;
		//cont++;
	}

	if (!(opciones.tra && opciones.tra < 4 && !opciones.lin && !opciones.audio && !opciones.wav)) {

		reglas_desam(frase_separada, 1);
		reglas_desam(frase_separada, 1);
		reglas_desam(frase_separada, 2);

		recorre_frase_separada = frase_separada;
		while (*recorre_frase_separada->pal) {
			recorre_frase_separada += diccionarios->busca_perifrase(recorre_frase_separada, idioma);
		}
		identificar_grupos_conxuntivos(frase_separada);
		//reglas_desam(frase_separada,0);

		recorre_frase_separada = frase_separada;
		while (*recorre_frase_separada->pal) {
			if (*recorre_frase_separada->cat_gramatical == 0) {
				recorre_frase_separada->cat_gramatical[0] = NOME;
				recorre_frase_separada->cat_gramatical[1] = 0;
			}
			recorre_frase_separada++;
		}

	}
}
