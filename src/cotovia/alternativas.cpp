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
 * \file
 * \author cardenal
 * \author fmendez
 * \remark Transcripcion fonética.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <pcreposix.h>
#include <sys/types.h>
#include <regex.h>
#include "alternativas.hpp"

int CompilaVariante(Variante *variante){
	if (regcomp(&variante->regort,variante->exp_ort,0))
		return(1);
	if (regcomp(&variante->regfon,variante->exp_fon,0))
		return(2);
	return(0);
}

int CompilaVariantes(Variante *variantes, int nvariantes){
	int i;

	for (i=0;i<nvariantes;i++){
		if (CompilaVariante(&variantes[i])){
			fprintf(stderr,"Error compilando %s %s\n",variantes[i].exp_ort,
					variantes[i].exp_fon);
			return(1);
		}
	}
	return(0);
}

//Devuelve uno si es necesario aplicar esta variante, cero si no
int TestVariante(char *ortografica,Variante *variante,int importancia){
	regmatch_t pmatch[10];
	int i;

	if (variante->importancia<importancia) 
		return(0);

	if (regexec(&variante->regort,ortografica,10,pmatch,0))
		return(0);
	else {
		for (i=0;i<variante->n_excep;i++){
			if (!strcmp(variante->excepciones[i],ortografica))
				return(0);
		}
	}
	return(1);
}

/*
   char *AplicaVariante(char *fonetica, Variante *variante){

   size_t nmatch;
   regmatch_t pmatch[10];
   int aplica;
   int principio,final;
   char salida[100];    

   if (regexec(&variante->regfon,fonetica,10,pmatch,0))
   return(NULL);

   principio=pmatch[0].rm_so;
   final=pmatch[0].rm_eo;
   strncpy(salida,fonetica,principio);
   salida[principio]='\0';
   strcat(salida,variante->sust);
   strcat(salida,fonetica+final);
   return((char *)strdup(salida));
   }
   */
char *AplicaVariante(char *fonetica, Variante *variante){

	regmatch_t pmatch[10];
	int principio,final;
	char salida[100];    
	char *trabajo;
	int longitud;

	trabajo=fonetica;
	longitud=strlen(trabajo);
	salida[0]='\0';

	while (longitud > 1){
		if (regexec(&variante->regfon,trabajo,10,pmatch,0))
			break;

		principio=pmatch[0].rm_so;
		final=pmatch[0].rm_eo;
		strncat(salida,trabajo,principio);
		strcat(salida,variante->sust);
		trabajo=trabajo+final;
		longitud=strlen(trabajo);
	}
	strcat(salida,trabajo);
	return((char *)strdup(salida));
}

int LiberaVariante(Variante *var){

	regfree(&var->regort);
	regfree(&var->regfon);
	for(int k=0;k<var->n_excep;k++)
		free(*(var->excepciones+k));
	free(var->excepciones);
	return 0;
}

/*
   int AnhadeTransc(Vocabulario *voc, int palabra, char *fonetica)
   {
//Comprobar primero si la transcripcion está repetida
for (i=0;i<voc->ort.ntransc[palabra];i++)
{
if (strcmp(voc->fon.transcrip[
voc->ort.pal2trans[palabra][i]]))
return(0);
}

//Anhadirla
voc.fon

//Actualizar pal2transc
*/
