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
 
%option batch never-interactive
%{
#include <stdio.h>
#include <string.h>
#include "variantes.tab.hpp"
int num_lin=0;
%}

COMENTARIO [ \t]*#[^\n]*
LINEA	\n 
VACIA [ \t]+
NUMERO [0-9]+
PALABRA [a-záéíóúñüç]+ 
REGEXP [^\n\"\.\,\t\* ]+
COMILLA \"
COMA \,
PUNTO \.

%%
{COMENTARIO}	{
		}
{NUMERO}	{
		yylval.num=atoi(yytext);
		//fprintf(stdout,"NUMERO %s\n",yytext);
		return(NUMERO);
		}

{PALABRA}  {
		yylval.str=strdup(yytext);
		////fprintf(stdout,"PALABRA %s\n",yytext);
		return(PALABRA);
		}
{REGEXP}  {
		yylval.str=strdup(yytext);
		//fprintf(stdout,"STRING %s\n",yytext);
		return(REGEXP);
		}
{COMILLA}  {
		yylval.num=COMILLA;
		//fprintf(stdout,"COMILLA %s\n",yytext);
		return(COMILLA);
		}
{COMA}  {
		yylval.num=COMA_V;
		//fprintf(stdout,"COMA %s\n",yytext);
		return(COMA_V);
		}
{PUNTO}  {
		yylval.num=PUNTO_V;
		//fprintf(stdout,"PUNTO %s\n",yytext);
		return(PUNTO_V);
		}
{LINEA}		{
			//printf("linea %s\n",yytext);
				num_lin++;
		}
{VACIA}		{
		}

%%

int yywrap(void) {return 1 ;}

/*
main()
{
	FILE *faux;
	int i;

	yyin=fopen("variantes.txt","r");
	yylex();
	fprintf(stdout," %d lineas procesadas\n" ,num_lin);
	fclose(yyin);

}
*/

