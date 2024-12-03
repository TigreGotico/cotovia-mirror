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
#include "tip_var.hpp"
#include "sil_acen.hpp"
#include "timbre.hpp"


/** \fn   int obter_codigo(int codigo,int *conx,temp_verb *tempo,unsigned int *num_persoa)

    Devolve un codigo distinto segundo a conxugacion, o tempo verbal, e a
   persoa do tempo verbal devolvendose o codigo calculado. Parametros:
     - conx: conxugacion a que pertence o verbo (IN).
     - temp_verbal : Tempo verbal codificado nun string segun se indica na
               cabeceira do presente programa (IN).
     - persoa : Persoa a que corresponde esa desinencia, servira para dar un
               codigo unico dentro dun determinado tempo verbal (IN).
     - Como parametro de SAIDA dara o codigo que resulte en funcion dos datos anteriores.
  */

  int obter_codigo(int codigo,int *conx,temp_verb *tempo,unsigned int *num_persoa){

	unsigned int num_tempo;

	if (codigo==180 || codigo==181 || codigo==182) {
		*tempo=XE;                    // Tratamo-lo XErundio.
		*num_persoa=7;
		if (codigo==180)
			*conx=1;
		else if (codigo==181)
			*conx=2;
		else
			*conx=3;
	}
	else if (codigo>=183 && codigo<=194) {
		*tempo=PA;                    // Tratamo-lo PArticipio
		if (codigo>=183 && codigo<=186)
			*conx=1;
		else if (codigo>=187 && codigo<=190)
			*conx=2;
		else
			*conx=3;
		*num_persoa=(codigo-183)%4;
	}
	else {                           // Tratamos demais tempos
		num_tempo=(codigo-1)/6;       // Primeira codificacion do tempo verbal
		*conx=(num_tempo%3)+1;        // Conxugacion a que pertence
		*num_persoa=(codigo-1)-(6*num_tempo);
		num_tempo=(codigo-1)/18;      // Tempo verbal codificado
		*tempo=(temp_verb) num_tempo; // Pomos tempo no enumerado
	}
	return 0;                        // Saida da funcion sen erros
}

/*
 *
 *
 */
int  deteccion_timbre_verbal (t_infinitivo *info, temp_verb tempo,
		unsigned int persoa,char *palabra){

	unsigned char aberto=OFF;
	char *remate=0,*remate_2=0;

	persoa++;                        // Pois decodificamos persoa-1 sempre
	switch (info->modelo) {          // BEGIN SWITCH
		case 1:
		case 4:               // Modelos M1 e M4 seguen as mesmas regras
			if ((tempo==IPRE || tempo==SPRE) && (persoa==1 || persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			else if (tempo==IMPE && persoa==2)
				aberto=ON;
			break;
		case 5:                       // Para o modelo M5
			if ((tempo==SPRE) && (persoa==1 || persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 8:
		case 17:     // Modelos: M8, M17 e M45 coas mesmas regras
			if ((tempo==IPRE) && (persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 9:                       // Modelo M9
			if ((tempo==IPRE) && (persoa==2 || persoa==3 || persoa==6)) {         // BEGIN IF 1
				if (strlen(info->infinitivo)>=4)
					remate=&(info->infinitivo[(strlen(info->infinitivo))-4]);
				if (!strcmp(info->infinitivo,"aquecer") ||
						!strcmp(info->infinitivo,"arrequecer") ||
						!strcmp(info->infinitivo,"quecer") ||
						!strcmp(info->infinitivo,"convencer") ||
						!strcmp(info->infinitivo,"crecer") ||
						!strcmp(info->infinitivo,"decrecer") ||
						!strcmp(info->infinitivo,"destecer") ||
						!strcmp(info->infinitivo,"entretecer") ||
						!strcmp(info->infinitivo,"estremecer") ||
						!strcmp(info->infinitivo,"exercer") ||
						!strcmp(info->infinitivo,"ofrecer") ||
						!strcmp(info->infinitivo,"pertencer") ||
						!strcmp(info->infinitivo,"requecer") ||
						!strcmp(info->infinitivo,"tecer") ||
						!strcmp(info->infinitivo,"vencer") ||
						!strcmp(info->infinitivo,"esquecer"))
					aberto =ON;
				else if (remate && strcmp(remate,"ecer") &&
						(strlen(info->infinitivo)>=5) && strcmp(remate-1,"eicer"))
					aberto =ON;
			}                          // END IF 1
			else if ((tempo==IMPE) && (persoa==2) && (
						!strcmp(info->infinitivo,"aquecer") ||
						!strcmp(info->infinitivo,"arrequecer") ||
						!strcmp(info->infinitivo,"quecer") ||
						!strcmp(info->infinitivo,"esquecer") ||
						!strcmp(info->infinitivo,"requecer") ))
				aberto =ON;
			break;
		case 10:                      // Modelo M10
			if (!strcmp(info->raiz,"oi") || !strcmp(info->raiz,"desoi")||
					!strcmp(info->raiz,"entreoi") || !strcmp(info->raiz,"aquez")||
					!strcmp(info->raiz,"arrequez") || !strcmp(info->raiz,"esquez")||
					!strcmp(info->raiz,"quez")|| !strcmp(info->raiz,"requez"))
				aberto =ON;
			break;
		case 16:
			if (tempo!=SIM && tempo!=IPER)
				aberto=ON;
			else if (tempo==IPER && persoa!=1 && persoa!=3)
				aberto=ON;
			else if (tempo==SIM && persoa!=4 && persoa!=5)
				aberto=ON;
			break;
		case 18:
		case 44:
		case 49:
		case 50:
			if (((tempo==IPRE) && (persoa==2 || persoa==3 || persoa==6)) || tempo==PA)
				aberto=ON;
			break;
		case 22:                      // Regras para o M22
			if (tempo==IPLUS || tempo==SFU)
				aberto=ON;
			else if (tempo==IPER && persoa!=1)
				aberto=ON;
			else if ((tempo==SIM) && (persoa==1 || persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 25:
			if (tempo==IPRE && persoa==6)
				aberto=ON;
			break;
		case 26:
		case 51:             // Modelos: M26 e M51
			if ((tempo==IPRE) && (persoa==2 || persoa==3))
				aberto=ON;
			break;
			//     case 34:                      // Regras para o M34
			//        if (!strcmp(info->raiz,"entreo") || !strcmp(info->raiz,"o"))
			//           aberto=ON;
			//       break;
		case 34:
		case 35:
		case 55:
		case 56:    // Modelos: M35, M55 e M56
			aberto=ON;
			break;
		case 40:                      // Regras do M40
			if ((tempo==IPRE) && (persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			else if ((tempo==IPLUS || tempo==SIM) && (persoa==1 || persoa==2 ||
						persoa==3 || persoa==6))
				aberto=ON;
			else if (tempo==IPER || tempo==SFU)
				aberto=ON;
			break;
		case 45:                      // Regras do M40
			if ((tempo==IPRE) && (persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 54:                      // Regras para o M54
			if ((tempo==IPRE) && (persoa==2 || persoa==3))
				aberto=ON;
			else if ((tempo==IIM) && (persoa==1 || persoa==2 || persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 60:                      // Modelo M60
			if (tempo==IPRE && (persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 61:                      // Modelo M61
			if (tempo==IPER  && (persoa==2 || persoa==4 ||persoa==5 || persoa==6))
				aberto=ON;
			else if ((tempo==IPLUS || tempo==SIM)  &&
					(persoa==1 || persoa==2 ||persoa==3 || persoa==6))
				aberto=ON;
			else if (tempo==SFU )
				aberto=ON;
			break;
		case 62:                      // Modelo M62
			remate=NULL;
			if (strlen(info->infinitivo)>4)
				remate=&(info->infinitivo[(strlen(info->infinitivo))-4]);
			if (!remate && tempo==IPRE && (persoa==3 || persoa==6))
				aberto=ON;
			else if (strcmp(remate,"ecer") && (tempo==IPRE) && (persoa==3 || persoa==6))
				aberto =ON;
			break;
		case 67:                      // Modelo M67
			if ((tempo==IPRE) && (persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 68:
		case 69:             // Modelos M68 e M69
			if ((tempo==IPRE || tempo==IPER || tempo==IPLUS || tempo==SIM ||
						tempo==SFU) && (persoa==3 || persoa==6))
				aberto=ON;
			break;
		case 73:                      // Modelo M73
			if ((tempo==IPRE || tempo==SPRE) && (persoa==3))
				aberto=ON;
			break;
		case 76:                      // Modelo M76
			if (tempo==IPRE && persoa==3)
				aberto=ON;
			break;
		case 78:                      // Regras para o M78
			if (!strcmp(info->raiz,"amenz"))
				aberto=ON;
			break;
		case 80:                      // Modelo M80
			if (tempo==IPRE && (persoa==4 || persoa==5))
				aberto=ON;
			break;
		case 88:                      // Modelo M88
			if (strlen(palabra)<4 || !strcmp(palabra,"roto") ||!strcmp(palabra,"rota") ||
					!strcmp(palabra,"rotos") ||!strcmp(palabra,"rotas") )
				break;
			remate=&(palabra[(strlen(palabra))-4]);
			if (strlen(palabra)>=5)
				remate_2=&(palabra[(strlen(palabra))-5]);
			if (!strcmp(remate,"eito") ||
					!strcmp(remate,"oito") ||
					!strcmp(remate,"esto") ||
					!strcmp(remate,"ento") ||
					!strcmp(remate,"eita") ||
					!strcmp(remate,"oita") ||
					!strcmp(remate,"esta") ||
					!strcmp(remate,"enta") ||
					(*remate_2 && (
								   !strcmp(remate_2,"eitos") ||
								   !strcmp(remate_2,"oitos") ||
								   !strcmp(remate_2,"estos") ||
								   !strcmp(remate_2,"entos") ||
								   !strcmp(remate_2,"eitas") ||
								   !strcmp(remate_2,"oitas") ||
								   !strcmp(remate_2,"estas") ||
								   !strcmp(remate_2,"entas"))
					))
				break;
			else
				aberto=ON;
			break;
		case 89:                      // Modelo M89
			if (!strcmp(info->raiz,"conver") || !strcmp(info->raiz,"pose"))
				aberto=ON;
			break;
		case 90:                      // Modelo M90
			if (!strcmp(info->raiz,"choc"))
				aberto=ON;
			break;
		case 92:
			aberto=ON;
			break;
	}                                // END SWITCH
	if (aberto)
		return 1;            // Devolve 1 (ON) se e aberta
	return 0;                        // Devolve 0 (OFF) se e pechada
}
/**
 *
 *
 */
int manexo_do_timbre_verbal(char * palabra,t_infinitivo *inf,temp_verb *tempo,
		unsigned int *persoa, unsigned int *posicion, unsigned int *pos_sil,
        char idioma){

	unsigned int index1, index2=0;
	t_palabra_proc silabeada, acentuada;
	int timbre=OFF;

	separar_silabas(palabra,acentuada);
	acentuar_prosodicamente(acentuada,silabeada,idioma);
	for (index1=0;(index1<strlen(silabeada));index1++)
		if (silabeada[index1]!='-') { // Copio acentuada sen guions
			acentuada[index2]=silabeada[index1];
			if (silabeada[index1]=='^'){
				*posicion=index2;       // Gardo a posicion da til sin silabear
				*pos_sil=index1;        // Gardo a posicion da til na silabeada
			}
			index2++;
		}
	acentuada[index2]='\0';
	if ((acentuada[(*posicion)-1]=='e') || (acentuada[(*posicion)-1]=='o')||
			(acentuada[(*posicion)-1]=='é') || (acentuada[(*posicion)-1]=='ó')) {
		timbre=deteccion_timbre_verbal(inf,*tempo,*persoa,palabra);
	}

	if (timbre && (acentuada[(*posicion)-1]=='e'))
		acentuada[(*posicion)-1]='é';
	else if (timbre && (acentuada[(*posicion)-1]=='o'))
		acentuada[(*posicion)-1]='ó';
	return timbre;
}

