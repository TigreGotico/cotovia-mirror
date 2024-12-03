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
 * \file sep_pal.cpp
 * \remarks Aquí temos as funcións empregadas para separar palabras, mellor dito
 * as unidades que se extraen como tal. Deste xeito separaremos ou extraeremos
 * como unha unidade cadeas de números ou caracteres especiais, independientemente
 * de que estean separados ou formando parte doutras cadeas.
 */

#include <stdio.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "letras.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp"
#include "sep_pal.hpp"

/** 
 * \author fmendez
 * \brief Esta funcion mira se estamos nunha secuencia de  caracteres que fai
 * acabar unha palabra e enpezar unha nova. Cando se esta no posible último
 * caracter da palabra hai que mira-la secuencia a partir da seguinte. Devolve
 * 1 se estamos no ultimo caracter.  
 * 
 * \param aux 
 * \param inicio 
 * 
 * \return Devolve 1 se estamos no último caracter. 
 */
inline short int fin_pal(char * aux, char * inicio)
{

	if (*aux==0) {
		return 1;
	}
	/* Xa que unha grande maioria dos signos ortograficos axudan a separar as pala-
		 bras, por exemplo os espacios, as interro/EXclamacions.                    */
	if (signo_puntuacion_non_ambiguo((unsigned char)*aux) || *aux==' ' ) {
		return 1;
	}
	/* Miramos se a coma pertence a unha secuencia numerica.                      */
	if ( *aux==',' ){
		if (dixito(*(aux-1)) && dixito(*(aux+1) )){
			return 0;
		}
		else {
			return 1;
		}
	}


	if (*(aux-1)==',') {

		if (aux>inicio+1 && dixito(*(aux-2)) && dixito(*(aux))){
			return 0;
		}
		else {
			return 1;
		}
	}
	/* Neste caso actua como separador non como pertencente a cifra decimal, coma
		 seguida de outro signo como espacio por exemplo.                           */

	/* AGORA EMPEZAN CASOS ONDE O GUION PODE APARECER TRABALLANDO COMO PARENTESES
		 --dixo el-- ou en commenzos de dialogos -- ¡Pois non!.                     */
	if ((*aux=='-') && (*(aux+1)=='-')) {
		return 1;
	}
	if (aux>inicio+1 &&(*(aux-1)=='-') && (*(aux-2)=='-')) {
		return 1;
	}

	//fmendez cosas como A-9 se separaban

	if (*aux=='-' && letra((unsigned char)*(aux-1)) &&
			!(letra((unsigned char)*(aux+1)) || dixito((unsigned char)*(aux+1)))){
		return 1;                 /* Ex dixo- .Separa este guion.               */
	}
	if (aux>inicio+1 &&(*(aux-1)=='-')&&(!letra((unsigned char)*(aux-2))&&(*(aux-2)!='-')&&letra((unsigned char)*(aux)))){
		return 1;                 /* EX. -pero. Separa este guion.              */
	}
	/* Este caso analizase cando os guions actúan como parenteses e non como algo
		 que une unha palabra composta ou ben pertence a unha cadea de formato como
		 unha data por exemplo.
		 Os signos como o guion deben separarse sempre a non ser que estean entre
		 letras(palabras compostas) ou entre numeros(datas por exemplo).            */
	//fmendez



	if (*aux=='.' && (*(aux+1)==0 ||*(aux+1)=='.')) {
		return 1;
	}
	/* O punto solo e final de palabra cando e o punto final que vai pegado a ulti-
		 -ma palabra e podeo confundir senon cunha abreviatura. Se e abreviatura des-
		 -doblaremos este punto para que na frase separada se vexan o da abreviatura
		 e o punto final.                                                           */
	/* Neste caso o punto final e tamen un final de palabra pero este punto pode
		 ser o mesmo tempo o punto dunha abreviatura o coincidir ambos por tanto hai
		 que considerar este caso para desdoblar o punto. Ademais os puntos suspensi-
		 -vos e como se non constituisen unha unidade e considérase cada un deles co-
		 -mo unha entidade gramatical.                                              */
	//if ( *aux==':' && letra((unsigned char)*(aux-1)))   return 1;
	if ( *aux==':' && !(dixito((unsigned char)*(aux-1))&& dixito((unsigned char)*(aux+1)))) {
		return 1;
	}
	/* Os dous puntos solo son fin de palabra se antes vai unha letra. Senon forman
		 parte da cadea especial de horas (23:30) e non se deben separar.           */
	if ((letra((unsigned char)*(aux-1)) && dixito(*aux))||
			(letra((unsigned char)*(aux)) && dixito(*(aux-1)))) {
		return 1;
	}
	/* No hipotetico caso de que unha palabra fose mixta, e decir estivese formada
		 por letras e numeros estes separarianse para lelos despois por separado.   */
	return 0;
}


/** 
 * \author fmendez
 * \remarks Collemo-las palabras de frase extraída e separámolas nun array, no
 * que cada elemento contén unha palabra (ou un signo de puntuación ou un
 * espacio) e ademais contén o tipo de clase de palabra da que se trata:
 * acrónimo, abreviatura, data, número, ordinal ,etc. 
 *  
 * \param frase_extraida 
 * \param frase_tokenizada 
 * \param n 
 * \param pidioma 
 */
void Tokenizar::tokenizar(char * frase_extraida, Token *frase_tokenizada, int &n,char pidioma)
{
	t_palabra posible_abreviatura_final="";
	static int lrup = strlen(SIMBOLO_RUPTURA_ENTONATIVA);
	static int lcom = strlen(SIMBOLO_RUPTURA_COMA);
	char *aux=frase_extraida;
	t_palabra pal_extr;
	unsigned int cont_letras=0,cont_pal=0;
	n=0;

	idioma=pidioma;

	if (*frase_extraida==0) {
		return;
	}
	do {                                              /* BEGIN DO-WHILE         */
		frase_tokenizada->pal_transformada = NULL;
		pal_extr[cont_letras++]=*aux;

		if (*aux == ',') {
			if (strncmp(aux, SIMBOLO_RUPTURA_ENTONATIVA, lrup) == 0) {
				aux += lrup;
				cont_letras = 0;
				strcpy(frase_tokenizada->token, SIMBOLO_RUPTURA_ENTONATIVA);
				frase_tokenizada++;
				cont_pal++;
				continue;
			}

			if (strncmp(aux, SIMBOLO_RUPTURA_COMA, lcom) == 0) {
				aux += lcom;
				cont_letras = 0;
				strcpy(frase_tokenizada->token, SIMBOLO_RUPTURA_COMA);
				frase_tokenizada++;
				cont_pal++;
				continue;
			}
		}

		if ((*aux==' ')||(fin_pal(aux+1, frase_extraida))||(cont_letras==LONX_MAX_PALABRA-1) ||
				(signo_puntuacion_non_ambiguo((unsigned char)*aux))||(caracter_especial(*aux)) ||
				(caracter_especial(*(aux+1)))){            /* BEGIN IF 1             */
			/* Se os seguintes caracteres marcan marcan un fin de palabra ou ben tamen os
				 signos ortograficos que teñen fins prosodicos tamen se consideran como pa-
				 -labras independientes.                                                    */
			pal_extr[cont_letras]=0;
			cont_letras=0;
			if (pal_extr[0]!=' '){                     /* BEGIN IF 2             */
				if ( *(aux+1)=='.' && (
							*(aux+2)==0 || (*(aux+2)=='.' && *(aux+3)==0))) { /* BEGIN IF 3             */
					/* Comprobamos se toda palabra anterior e unha abreviatura, caso no que desdo-
						 -blamos o punto final no que pertence a abreviatura, e o propio signo de
						 puntuacion. Neste punto teremos en conta o idioma, para ver onde debe bus-
						 - ca-las abreviaturas, se galego ou se castelan.                           */
					//strcpy(posible_abreviatura_final,pal_extr);
					//pasar_a_minusculas(posible_abreviatura_final,posible_abreviatura_final);
					pasar_a_minusculas(pal_extr,posible_abreviatura_final);
					strcat(posible_abreviatura_final,".");

					//ZA: Sustituyo lo que sobra por mi propia función de búsqueda (gallego/castellano)
					if (diccionarios->existeEnAbreviaturas(posible_abreviatura_final,idioma)!=NULL)
						strcat(pal_extr,".");
					if  (*(aux+2)=='.'){ 
						aux++;
					}
					/* Se se trata dunha abreviatura final desdoblamos o punto como punto da abre-
						 -viatura e punto final. Podiamos facer iso pero optamos xa por sustituila
						 polo seu valor xa que facemos asi despois unha busqueda menos.
						 Busca_en_lista devolve -1 se non se encontra esa palabra na lista e o indice
						 se se encontra.                                                            */
				}                                      /* END IF  3              */
				if (!(cont_pal && !strcmp(pal_extr,"-") && !strcmp((frase_tokenizada-1)->token,"-"))){
					strcpy(frase_tokenizada->token, pal_extr);
					frase_tokenizada++;
					cont_pal++;
				}
				if (*(aux+1)=='.' && *(aux+2)=='.') {
					aux++;
				}
				if (cont_pal==LONX_MAX_FRASE_SEPARADA-1) {
					/* Non se poden gardar mais elementos que para os que hai sitio. Se hai mais,
						 truncase aqui a frase.                                                     */
					break;
				}
			}                                         /* END IF 2               */
			*pal_extr=0;
		}                                            /* END IF 1               */
		/* se non é un signo ortografico ou espacio continuamos acumulando na palabra */
		else if (cont_letras==1 && *aux=='-' && !dixito(*(aux+1))) {
			pal_extr[cont_letras]=0;
			strcpy(frase_tokenizada->token, pal_extr);
			frase_tokenizada++;
			cont_pal++;
			if (cont_pal==LONX_MAX_FRASE_SEPARADA-1) break;
			cont_letras=0;
			*pal_extr=0;

		}
		aux++;
	} while  ( *(aux)!=0 );                       /* END DO-WHILE           */
	n = cont_pal;
	/* Asignamos o numero de elementos de frase_tokenizada.                         */
	*(frase_tokenizada->token) = 0;
}

