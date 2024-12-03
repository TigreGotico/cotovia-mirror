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
 
/*
	No arquivo hai as funcions  que permiten analizar o xenero e numero
	dunha palabra.Isto uilizase por exemplo para averiguar con que xenero
	se deben pronunciar os numeros(se conten cifras como o 1 ou 2 e menor
	de cen ou se e maior de cen sempre.
	O numero necesitase para as abreviaturas, cando se trata sobre todo de
	medidas de cousas: km/h, A...

*/
 
#include <stdio.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "letras.hpp"
#include "xen_num.hpp"
/*********************************************************************/
const t_tabla_traduc Exc_Numero[]={
	{"pai","pais"},
	{"esquí","esquís"},
	{"nai","nais"},
	{"rei","reis"},
	{"boi","bois"},
	{"paxe","paxes"},
	{"lei","leis"},
	{"vagalume","vagalumes"},
	{"mestre","mestres"},
	{"",""},
	{"\0","\0"}
};

	/* Lista de excepcions para a detección do xénero.
	tipo1	: palabras de xénero masculino detectadas con xénero
		femenino.
	tipo2	: caso contrario, xénero femenino sendo masculino. */

const char *Exc_Xenero_tipo1[]={"home","compadre","traxe","garaxe","personaxe",
							"paxe","de","poeta","mestre","sacerdote","leite","albeite",
							"rapaz",
							"duque","sofá","nariz","arroz","alférez",
							"amor","\0"};

const char *Exc_Xenero_tipo2[]={"muller",
							"nai","culler","moto","foto","radio","catedral","moral","parte","partes",
							"\0"};
/***************************************************************************/



int grupo_plural(unsigned char letra)
{
	switch(letra)
		{
		case 'b':case 'c':case 'd':case 'f':
		case 'g':case 'h':case 'm':case 'p':
		case 'r':case 's':case 't':case 'z':
		case 'l':	return 1;
		}
	return 0;
}

/*****************************************************************************
*****************************************************************************/

int excepcions_xenero(char *posible_excepcion)
/*  Miramos se a palabra se encontra nas 2 listas  de excepcions
		e se se encontran devolvemos o xenero(dependendo en que lista
		se encontren) ou senon devolvemos un 0-*/

{

 register int i=0;

	/* Buscamos a palabra de entrada na lista de tipo1.*/

 while ( (strcmp(posible_excepcion,Exc_Xenero_tipo1[i]) != 0)
		&& (strcmp(Exc_Xenero_tipo1[i],"\0") != 0) ) i++;
 if (strcmp(Exc_Xenero_tipo1[i],"\0") != 0) return MASCULINO;

	/* Buscamos a palabra de entrada na lista de tipo2.*/

 i=0;
 while ( (strcmp(posible_excepcion,Exc_Xenero_tipo2[i]) != 0)
		&& (strcmp(Exc_Xenero_tipo2[i],"\0") != 0)   ) i++;
 if (strcmp(Exc_Xenero_tipo2[i],"\0") !=0) return FEMININO;

 return 0;
}

/***********************************************************************/
int terminacion(t_palabra pal,const  char *sufixo){
/* Dada unha palabra e unha terminacion a funcion comproba se dita
	 palabra acaba nesa cadea, caso no que devolve un 1. Utilizamola
	 por exemplo para analizar o xenero segun terminacions*/


  char *aux;
  int n,lonx_pal;

	n=strlen(sufixo);
	lonx_pal=strlen(pal);

    if (n > lonx_pal)
        return 0;
        
	aux=pal+lonx_pal-n;
	/*colocamos o punteiro na parte da palabra onde empeza o sufixo
		e comparamos a ultima parte*/
#ifndef  _WIN32
	if (strcasecmp(aux,sufixo)==0)
#else
	if (stricmp(aux,sufixo)==0)
#endif
		return 1;
	else
		return 0;
 }
/*****************************************************************************
*****************************************************************************/

int excepcions_numero( char *posible_excepcion)
{

 register int i=0;


 while ( (strcmp(posible_excepcion,Exc_Numero[i].traduc) != 0)
		&& (strcmp(Exc_Numero[i].traduc,"\0") != 0) ) i++;

		/* En caso de ser unha excepción, copiamos o valor na variable que
		nos daban como parámetro, "posible_excepcion", e decir devolvemos
		o singular da palabra o non axustarse as reglas de formacion de
		numero.Esta funcion debe evolucionar e ter solo aquelas palabras
		que terminan en s sendo singulares. Noutro caso consideraremos
		para analizar simplemente que o plural sempre e cunha palabra
		rematada en s. Pero agora para analizar o xenero deberemos ter
		en conta que as palabras poden ir en singular ou plural. */

 if (strcmp(Exc_Numero[i].traduc,"\0") != 0)
 {
	strcpy(posible_excepcion,Exc_Numero[i].palabra);
	return 1;
 }
 return 0;
}

/*****************************************************************************
****************************************************************************/

int xenero(const char *palabra_entrada)
{

	t_palabra palabra="\0";


	strcpy(palabra,palabra_entrada);

		/* Tratamento das excepcions. */

	switch (excepcions_xenero(palabra) )
	{
	 case MASCULINO	: return MASCULINO;
	 case FEMININO : return FEMININO;
	}
	/*
		Suponse por defecto que a palabra e masculina a non ser que
		se axuste as seguintes reglas*/

	if (   terminacion(palabra,"a") ||
				 terminacion(palabra,"as")		 ) return FEMININO;

	if (   terminacion(palabra,"axe") ||
				 terminacion(palabra,"axes")   ) return FEMININO;

	if (  terminacion(palabra,"se") ||
				terminacion(palabra,"ses")  )    return FEMININO;

//	if (  terminacion(palabra,"ite") ||
//				terminacion(palabra,"ites")    ) return FEMININO;

	if (  terminacion(palabra,"á") ||
				terminacion(palabra,"ás")  )     return FEMININO;

//	if (  terminacion(palabra,"ei") ||
//				terminacion(palabra,"eis") )     return FEMININO;

//	if (  terminacion(palabra,"z")  ||
//				 terminacion(palabra,"ces")  )   return FEMININO;
//	if (  terminacion(palabra,"or") ||
//				terminacion(palabra,"ores")  )   return FEMININO;

	if (  terminacion(palabra,"ción") ||
				terminacion(palabra,"cións") )   return FEMININO;

	else return MASCULINO;
}

/****************************************************************************/
void cambiar_terminacion(t_palabra palabra,const  char *term_vella,
																	const char *term_nova){
 char *letra_anterior;
 letra_anterior=palabra+strlen(palabra)-strlen(term_vella);
		 /* situamons sobre a i e cambiamola por l*/
 strcpy(letra_anterior,term_nova);
}
/*****************************************************************************/
int numero( char *palabra){

	 char *letra_anterior;
	 int lonx_pal;

	/* Comprobamos se a palabra esta na lista de excepcions e senon miramos
		 as distintas terminacions. Ademais devolvemos as palabras en singular
			de xeito que a deteccion de xenero e mais facil xa que solo hai que
			ter en conta as terminacions de singular*/
	if ( excepcions_numero(palabra) ){
		*palabra='\0';
		return PLURAL;
	}
	lonx_pal=strlen(palabra);

	if ( terminacion(palabra,"les")){
		 cambiar_terminacion(palabra,"les","l");
//		 letra_anterior_terminacion=palabra+strlen(palabra)-2;
//situamonos sobre a e e borramos es
//		 strcpy(letra_anterior,"\0");
//		 strcpy(palabra_entrada,palabra);
		 return PLURAL;
		};
	 /// quitamos -es en:   sol-soles, animal-animales

	if (
				terminacion(palabra,"ais") ||
				terminacion(palabra,"eis") ||
				terminacion(palabra,"ois")     )
		{
		 cambiar_terminacion(palabra,"is","l");
		 return PLURAL;
		};
	/* caso de español-españois, animal-animais, cartel-carteis*/

	if ( terminacion(palabra,"ís") )
		{
		 cambiar_terminacion(palabra,"ís","il");
		 return PLURAL;
		};
	/* caso de mandil-mandís, cadril-cadrís */


	if ( lonx_pal>2 && terminacion(palabra,"es") &&
				( palabra[lonx_pal-3]=='r' ||  palabra[lonx_pal-3]=='c' ||
					palabra[lonx_pal-3]=='m' ||  palabra[lonx_pal-3]=='d'      )
																																		 )
	/* os sustantivos rematados en consoante distinta de l ou s fan
		o plural engadindo -es. Concretamente nos casos de calores -calor,
			rapaces-rapaz, laudes-laud ,albumes album . Nestes casos hai que
			retirar -es*/
	{
		 letra_anterior=palabra+strlen(palabra)-2;
		 /*situamonos sobre a e e borramos es*/
     *letra_anterior=0;
		 if (palabra[strlen(palabra)-1]=='c')
				palabra[strlen(palabra)-1]='z';
				/* este e o caso de rapaces --> rapac -->rapaz*/
		 return PLURAL;
	}

	if (lonx_pal>1 &&  palabra[lonx_pal-1]=='s' &&
				(vocal((unsigned char)palabra[lonx_pal-2]) ||  palabra[lonx_pal-2]=='n')
																																		 )
	/* se acaba en vocal ou -n mais un ese pasa o singular deste xeito*/
	{
		 letra_anterior=palabra+strlen(palabra)-1;
		 /*situamonos sobre a e e borramos s*/
		 strcpy(letra_anterior,"\0");
		 return PLURAL;
	}


 return SINGULAR;
}
 







