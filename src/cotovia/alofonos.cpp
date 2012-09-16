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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tip_var.hpp"
#include "alofonos.hpp"


/**
	\fn void insertar_string(char * pos_insertar,char * insercion)

	Insertamos a continuacion do caracter apuntado
	*/

void insertar_string(char * pos_insertar,const char * insercion){


	char cacho_final[200];

	strcpy(cacho_final,pos_insertar);
	strcpy(pos_insertar,insercion);
	strcat(pos_insertar,cacho_final);
}

/**   \fn   detectar_comentario(char *comentario, unsigned char *pos_comentario,
	char *bufer_de_texto_a_sustituir,char *insertar_en)

	Buscamo-lo comentario en "bufer de Texto a sustituir", eliminamolo para aplica-la regla
	e obtemo-la sua posicion para saber despois se hai que volver a insertala ou non.
	So obteremos o comentario se a regra pode saltar entre as duas partes antes
	e despois do comentario.Para iso consideramos que unha regla tera como moito
	unha lonxitude de dez caracteres.
	*/

void detectar_comentario(char *comentario, unsigned char *pos_comentario,
		char *bufer_de_texto_a_sustituir,char *insertar_en){


	char * pos_tanto_por_cen, * pos_tanto_por_cen_final;
	char *segundo_comentario;

	*pos_comentario=0;
	*comentario=0;

	pos_tanto_por_cen=strchr(bufer_de_texto_a_sustituir,'%');
	if ( pos_tanto_por_cen==NULL) 
		return;
	if ( (pos_tanto_por_cen-bufer_de_texto_a_sustituir)>10) 
		return;

	pos_tanto_por_cen_final=(char *)strchr((char *)(pos_tanto_por_cen+1),'%');
	// Comprobamos si hay dos comentarios seguidos... (esto habría que hacerlo más general, para n comentarios.

	if (pos_tanto_por_cen[-1] != '#') {
		segundo_comentario = (char *) strchr(pos_tanto_por_cen_final + 1, '%');

		if (segundo_comentario != NULL) {
			if (*(segundo_comentario - 1) != '#') {
				if (segundo_comentario - pos_tanto_por_cen_final < 3) {
					// Hay un segundo comentario
					pos_tanto_por_cen_final = (char *) strchr(segundo_comentario + 1, '%');
				}
			}

		}
	}

	//fmendez
	if (*(pos_tanto_por_cen_final+1)==' ')
		pos_tanto_por_cen_final++; 
	//fmendez


	strncpy(comentario,pos_tanto_por_cen,(pos_tanto_por_cen_final-pos_tanto_por_cen+1));
	*(comentario+(pos_tanto_por_cen_final-pos_tanto_por_cen+1))=0;
	/*aseguramonos de que estea null terminada*/

	*pos_comentario=(unsigned char)(pos_tanto_por_cen-bufer_de_texto_a_sustituir);

	if (!*pos_comentario && strstr(comentario, "pausa"))
		*insertar_en = '#';
	else {
		if  (pos_tanto_por_cen!=bufer_de_texto_a_sustituir)
			*insertar_en = *(pos_tanto_por_cen - 1);
		else 
			*insertar_en=0;
	}
	/*
		 if(*insertar_en=='#')
		 despois_do_comentario=pos_tanto_por_cen_final+1;
		 else
	//orig despois_do_comentario=pos_tanto_por_cen_final+2;
	despois_do_comentario=pos_tanto_por_cen_final+1;
	*/

	//strcpy(pos_tanto_por_cen,despois_do_comentario);
	_copia_cadena(pos_tanto_por_cen,pos_tanto_por_cen_final+1);

	if (strncmp(comentario, "%ruptu", 6) == 0) {

		if (bufer_de_texto_a_sustituir[2] == '#') {

			char *apunta_siguiente_fonema = strstr(pos_tanto_por_cen, "%");

			*insertar_en = bufer_de_texto_a_sustituir[1];

			if (apunta_siguiente_fonema == NULL)
				bufer_de_texto_a_sustituir[2] = pos_tanto_por_cen[2];
			else {
				apunta_siguiente_fonema = strstr(apunta_siguiente_fonema + 1, "%");
				bufer_de_texto_a_sustituir[2] = apunta_siguiente_fonema[2];
			}

		}

	}

	// *pos_comentario=situacion_relativa_do_comentario;

}


/** \fn   char *insertar_comentario(char *comentario,unsigned char pos_comentario,
	char *frase_fonetica,t_regla *regla,char insertar_en,char *pos_actual_frase_sil)

	Se a regla que se aplicou, afectou a ambolos dous lados do comentario, ent´´on
	temos que inserta-lo comentario na posici´´on onde lle corresponde (entre ##
	se se refire a unha pausa, ou despois da palabra onde ´´ia.
	*/
char *insertar_comentario(char *comentario,unsigned char pos_comentario,
		char *frase_fonetica,t_regla *regla,char insertar_en,char *pos_actual_frase_sil){


	char * lugar_para_insertar;

	if (*comentario==0) 
		return pos_actual_frase_sil;
	if (regla==NULL || regla->lonx_a_sustituir>=pos_comentario){
		/*enton debemos insertar neste momento o comentario*/
		lugar_para_insertar=frase_fonetica+strlen(frase_fonetica)-1;
		if (insertar_en=='#'){
			while(*lugar_para_insertar!='#' && lugar_para_insertar!=frase_fonetica)
				lugar_para_insertar--;
			if (lugar_para_insertar != frase_fonetica) {
				if (*(lugar_para_insertar - 1) != '#')
					lugar_para_insertar++;
			}
			else
				lugar_para_insertar++;
		}
		else {
			while(*lugar_para_insertar!=' ' && lugar_para_insertar!=frase_fonetica)
				lugar_para_insertar--;
			lugar_para_insertar++;
		}

		insertar_string(lugar_para_insertar,comentario);
		pos_actual_frase_sil=pos_actual_frase_sil+strlen(comentario);
	}
	return pos_actual_frase_sil;
}




/**
 * \remarks	Función que crea una cadena de estructuras con la información de la
 * posición de las rupturas entonativas y comas que no se realizaron como una
 * pausa. También crea una nueva frase silabeada sin las rupturas, para que su
 * presencia no afecte a la transcripción fonética.
 * \param	frase_sil: frase silabeada y acentuada.
 * \param	nueva_frase_sil: la cadena anterior, pero sin las rupturas.
 * \retval	rupturas: cadena con las rupturas de la frase.
 * \retval	numero_rupturas: número de elementos de la cadena anterior.
 * \return	En ausencia de error, devuelve un cero.
 */

int crea_cadena_rupturas(char *frase_sil, char **nueva_frase_sil,
		Estructura_rupturas **rupturas, int *numero_rupturas) {

	unsigned int contador;
	int memoria_reservada = 50, tamano_actual = 0;
	char *actual, *modifica;
	Estructura_rupturas *correcaminos;

	if ( (*nueva_frase_sil = (char *) malloc((strlen(frase_sil) + 1)*sizeof(char)))
			== NULL) {
		fprintf(stderr, "Error en crea_cadena_rupturas, al asignar memoria.\n");
		return 1;
	}

	actual = *nueva_frase_sil;

	for (contador = 0; contador <= strlen(frase_sil); contador++)
		(*nueva_frase_sil)[contador] = frase_sil[contador];

	if ( (*rupturas = (Estructura_rupturas *) malloc(memoria_reservada*sizeof(Estructura_rupturas)))
			== NULL) {
		fprintf(stderr, "Error en crea_cadena_rupturas, al asignar memoria.\n");
		return 1;
	}

	correcaminos = *rupturas;

	while ( (actual = strstr(actual + 1, "#%rupt")) != NULL) {

		modifica = actual - 1;

		if (strncmp(actual, "#%ruptura-en", 11) == 0) {
			correcaminos->tipo = 0;
			for (; *(modifica + 22) != '\0'; modifica++)
				*modifica = *(modifica + 23);
		}
		else {
			correcaminos->tipo = 1;
			for (; *(modifica + 16) != '\0'; modifica++)
				*modifica = *(modifica + 17);

		}

		tamano_actual++;
		correcaminos->posicion = actual - *nueva_frase_sil;

		if (tamano_actual == memoria_reservada) {

			memoria_reservada += 50;

			if ( (*rupturas = (Estructura_rupturas *) realloc(*rupturas, memoria_reservada*sizeof(Estructura_rupturas)))
					== NULL) {
				fprintf(stderr, "Error en crea_cadena_rupturas, al reasignar memoria.\n");
				return 1;
			}
			correcaminos = *rupturas + tamano_actual;
		}
		else
			correcaminos++;

	} // while ( (actual = ...

	*numero_rupturas = tamano_actual;

	return 0;

}

