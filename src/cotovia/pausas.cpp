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
#include "modos.hpp"
#include "tip_var.hpp"
#include "pausas.hpp"

t_pausas frase_pausas[LONX_MAX_FRASE_SEPARADA];

static FILE *fp=NULL;

char imprime_cat_p[MAX_NUM_CATEGORIAS_PAUSAS_CONSIDERADAS][16]={
  "NULA_P",
  "ADVE_P",       
  "ADXE_P",
  "APERT_EXCLA_P",
  "APERT_INTER_P",
  "APERT_PAREN_P",
  "COMA_P",       
  "COMINNAS_P",   
  "CONX_COOR_P",  
  "CONX_SUBOR_P", 
  "DET_NUME_P",   
  "DET_P",        
  "DET_POSE_P",   
  "EXCLA_P",      
  "GUION_P",      
  "INTER_P",      
  "INTERX_P",     
  "NOME_P",       
  "PECHE_EXCLA_P",
  "PECHE_INTER_P",
  "PECHE_PAREN_P",
  "PREP_P",       
  "PRON_NUME_P",  
  "PRON_P",       
  "PRON_POSE_P",  
  "PRON_PROC_P",  
  "PUNTO_P",      
  "RELA_P",       
  "SIGNO_P",      
  "VERBO_P",      
  "XERUNDIO_P"    
};

/** 
 * \author fmendez
 * \brief 
 * 
 * \param tipo 
 * 
 * \return 
 */
int pon_pausa_segun_tipo(unsigned char tipo){

  switch(tipo){
  case  PUNTO:return 800;
  case  COMA: return 100;
  case  DOUS_PUNTOS:return 200;
  case  PUNTO_E_COMA:return 300;
  case  PUNTOS_SUSPENSIVOS:return 200;

#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
    //#if !defined(_SCRIPT) && !defined(_GRUPOS_ACENTUALES) && !defined(_PARA_ELISA) && !defined(_SCRIPT_DURACION)
  case  APERTURA_PARENTESE:return 50;
  case  PECHE_PARENTESE:return 50;
  case  APERTURA_INTERROGACION:return 100;
  case  PECHE_INTERROGACION:return 300;
  case  APERTURA_EXCLAMACION:return 100;
  case  PECHE_EXCLAMACION :return 100;
  case  APERTURA_CORCHETE:return 50;
  case  PECHE_CORCHETE :return 50;
  case  GUION:return 20;
  case  GUION_BAIXO:return 20;
#endif
    // #else
    // #if !defined(_SCRIPT) && !defined(_GRUPOS_ACENTUALES) && !defined(_PARA_ELISA) && !defined(_SCRIPT_DURACION)
    // 		case  APERTURA_PARENTESE:return 100;
    // 		case  PECHE_PARENTESE:return 100;
    // #endif

    //    case  DOBLES_COMINNAS:return 20;
    //    case  SIMPLES_COMINNAS:return 20;
    //    case  GUION:return 20;
    //    case  GUION_BAIXO:return 20;
    //#endif
  default : return 0;
  }
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param frase_sintagmada 
 * \param f_sep 
 */
void poner_pausas(t_frase_sintagmada *frase_sintagmada, t_frase_separada *f_sep){
	t_frase_sintagmada * rec=frase_sintagmada;
	t_frase_separada *aux;
#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
#ifdef _MODO_NORMAL
	t_frase_sintagmada * pos_sig_posible_pausa=NULL;
	int n_silabas_entre_pausas=0;
#endif
#endif

	while(rec->tipo_sintagma){
		rec->pausa=pon_pausa_segun_tipo(rec->tipo_sintagma);
		if (rec->pausa) {
			aux = &f_sep[rec->fin];
			while (aux > f_sep  && aux->clase_pal == SIGNO_PUNTUACION) aux--;
			aux->tonicidad_forzada =1;
			rec++;
#ifndef _SOLO_PAUSAS_ORTOGRAFICAS
#ifdef _MODO_NORMAL
			n_silabas_entre_pausas=0;
#endif
#endif
			continue;
		}

#ifdef _MODO_NORMAL

#ifndef _SOLO_PAUSAS_ORTOGRAFICAS

		n_silabas_entre_pausas+=rec->n_silabas;

		/*
			 if (n_silabas_entre_pausas>MAX_SILABAS_ENTRE_PAUSAS && rec>frase_sintagmada){
			 (rec-1)->pausa+=DUR_PAUSA_DEMASIADAS_SILABAS;
			 n_silabas_entre_pausas=rec->n_silabas;
			 }

*/
		if (n_silabas_entre_pausas>MAX_SILABAS_ENTRE_PAUSAS_2 && rec>frase_sintagmada) {
			if (pos_sig_posible_pausa==NULL) {
				rec->pausa+=DUR_PAUSA_DEMASIADAS_SILABAS;
				//f_sep[rec->fin].pausa+=DUR_PAUSA_DEMASIADAS_SILABAS;
				n_silabas_entre_pausas=0;
				aux = &f_sep[rec->fin];
				while (aux > f_sep  && aux->clase_pal == SIGNO_PUNTUACION) aux--;
				aux->tonicidad_forzada =1;
			}
			else {
				(pos_sig_posible_pausa-1)->pausa+=DUR_PAUSA_DEMASIADAS_SILABAS;
				//f_sep[(pos_sig_posible_pausa-1)->fin].pausa+=DUR_PAUSA_DEMASIADAS_SILABAS;
				n_silabas_entre_pausas=pos_sig_posible_pausa->n_silabas;
				rec=pos_sig_posible_pausa;
				pos_sig_posible_pausa=NULL;
				aux = &f_sep[(rec-1)->fin];
				while (aux > f_sep  && aux->clase_pal == SIGNO_PUNTUACION) aux--;
				aux->tonicidad_forzada =1;

			}
		}
		else if (n_silabas_entre_pausas>MAX_SILABAS_ENTRE_PAUSAS_1 && rec>frase_sintagmada)
			pos_sig_posible_pausa=rec;
#endif
#endif

		rec++;
	}
}



/**
 * \author fmendez
 * \brief 
 * 
 * \param p_frase
 * \param p_frase_sintagmada
 * 
 * \return 
 */
int crea_frase_pausas(t_frase_separada * p_frase, t_frase_sintagmada *p_frase_sintagmada){

	t_frase_separada *p_frase_inicial = p_frase;
	t_pausas *rec;
#ifndef _SALTA_SOSTENIDOS_PAUSAS
	t_pausas *aux;
#endif
	int n_silabas_acumulado, n_pal;
	Categorias_pausas categorias[LONX_MAX_FRASE_SEPARADA+OFFSET];
	int i;

#ifdef _ESCRIBE_FICHERO_PAUSAS
	if (fp==NULL){
		if ( (fp = fopen("fichero_pausas.csv", "w")) == NULL) {
			fputs("Error en crea_frase_pausas, al intentar abrir el fichero fichero_pausas.csv.", stderr);
			return 1;
		}
		fputs("pausa\tpalabra\tcat_pas_3\tcat_pas_2\tcat_pas_1\tcat_act\tcat_fut_1\tcat_fut_2\tcat_fut_3\tn_sil_pau_ant\tn_sil_pau_sig\n", fp);
	}
#endif
	for(i=0;i<OFFSET;i++)
		categorias[i]=NULA_P;

	n_silabas_acumulado=0;
	n_pal=0;

	// p_frase=f_sep;
	rec=frase_pausas;

	while (*p_frase->pal) {

#ifdef _SALTA_SOSTENIDOS_PAUSAS
		if (*p_frase->pal == '#'){
			if (p_frase > p_frase_inicial){
				(rec-1)->pal->pausa=1;
				(rec-1)->n_silabas_pausa_ant=n_silabas_acumulado;
				n_silabas_acumulado=0;
			}
			p_frase++;
			continue;
		}
#endif

		rec->pal=p_frase;
		rec->pal->pausa=0;
		categorias[n_pal+OFFSET]=traduce_a_categorias_pausas(p_frase->cat_gramatical[0]);
		n_pal++;
		n_silabas_acumulado+=p_frase->n_silabas;
		rec->n_silabas_pausa_ant=n_silabas_acumulado;
		if (pon_pausa_segun_tipo(p_frase->cat_gramatical[0])){
			rec->pal->pausa=1;
			n_silabas_acumulado=0;
		}
		p_frase++;
		rec++;
	}
	for(i=0;i<OFFSET;i++)
		categorias[n_pal+i+OFFSET]=NULA_P;

	n_silabas_acumulado=0;
	rec->pal=NULL;

	while (rec>frase_pausas){
		rec--;
		rec->n_silabas_pausa_pos=n_silabas_acumulado;
		//    	if (rec->pausa)  //todas pausas futuras
		if (pon_pausa_segun_tipo(rec->pal->cat_gramatical[0]))  //solo puntuación
			n_silabas_acumulado=rec->pal->n_silabas;
		else
			n_silabas_acumulado+=rec->pal->n_silabas;
	}

	n_pal = 0;
	n_silabas_acumulado = 0;

	while (rec->pal){
		for(i=-OFFSET;i<=OFFSET;i++)
			rec->categorias[i+OFFSET]=categorias[n_pal+OFFSET+i];

#ifndef _SALTA_SOSTENIDOS_PAUSAS
		n_silabas_acumulado += rec->pal->n_silabas;
		rec->n_silabas_pausa_ant = n_silabas_acumulado;

		rec->pal->pausa = pon_pausa_segun_tipo(rec->pal->cat_gramatical[0]);

		if (rec->pal->pausa) {

			if (n_pal != 0) {
				if ( ( (rec - 1)->pal->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
						( (rec - 1)->pal->cat_gramatical[0] == RUPTURA_COMA) ) {

					rec->pal->pausa = 0;
					rec++;
					n_pal++;
					continue;
				}
			}

		}

		if (rec->pal->pausa) {
			rec->pal->tonicidad_forzada = 1;
			n_silabas_acumulado = 0; 
			aux = rec;
			while (aux > frase_pausas && aux->pal->clase_pal == SIGNO_PUNTUACION) {
				aux--;
			}
#ifndef _CONSIDERA_GRUPOS_HUERFANOS
			aux->pal->tonicidad_forzada = 1;
#else
			if (aux == frase_pausas)
				aux->pal->tonicidad_forzada = 1;
			else 
				if ( (aux - 1)->pal->pausa != 0)
					aux->pal->tonicidad_forzada = 1;
#endif

		}
		else if ( (rec->pal->cat_gramatical[0] != CONX_SUBOR_FINAL) && (pausador_arbol(rec)) ) {
			// Comprobar si es final de sintagma:
			int indice_palabra = rec->pal - p_frase_inicial;
			t_frase_sintagmada *sintagma_actual = p_frase_sintagmada;
			while (sintagma_actual->inicio < indice_palabra)
				sintagma_actual++;
			if (sintagma_actual->fin != indice_palabra) {
				rec->pal->pausa = 0;
			}
			else {

				if (n_pal != 0) {
					if ( ( (rec - 1)->pal->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
							( (rec - 1)->pal->cat_gramatical[0] == RUPTURA_COMA) ) {

						rec->pal->pausa = 0;
						rec++;
						n_pal++;
						continue;
					}
				}

				rec->pal->pausa = DUR_PAUSA_TEXTO;
				rec->pal->tonicidad_forzada = 1;
				n_silabas_acumulado = 0;
				aux = rec;
				while (aux > frase_pausas && aux->pal->clase_pal == SIGNO_PUNTUACION) {
					aux--;
				}
				aux->pal->tonicidad_forzada = 1;
			}
		}
		else {
			rec->pal->pausa = 0;
		}
		/*
			 if (pausador_arbol(rec) || pon_pausa_segun_tipo(rec->pal->cat_gramatical[0])) {
			 rec->pal->pausa = 1;
			 rec->pal->tonicidad_forzada = 1;
			 n_silabas_acumulado = 0;
			 if ((rec->pal->pausa=pon_pausa_segun_tipo(rec->pal->cat_gramatical[0]))!=0 ){
			 aux = rec;
			 while (aux>frase_pausas && aux->pal->clase_pal==SIGNO_PUNTUACION)
			 aux--;
			 aux->pal->tonicidad_forzada = 1;
			 }
			 else
			 rec->pal->pausa = DUR_PAUSA_TEXTO;
			 }
			 else
			 rec->pal->pausa = 0;
			 */
#endif

#ifdef _ESCRIBE_FICHERO_PAUSAS
		fprintf(fp,"%d\t%s\t",rec->pal->pausa,rec->pal->pal);
		for(i=-OFFSET;i<=OFFSET;i++)
			fprintf(fp,"%s\t",imprime_cat_p[categorias[n_pal+OFFSET+i]]);
		fprintf(fp,"%d\t%d\n",rec->n_silabas_pausa_ant,rec->n_silabas_pausa_pos);
#endif
		n_pal++;
		rec++;
	}

	return 0;
}

/**
 * \author fmendez
 * \remarks
 * \param categoria
 */

Categorias_pausas traduce_a_categorias_pausas(unsigned char categoria){

  switch(categoria){
  case ART_DET:
  case CONTR_CONX_ART_DET:
  case CONTR_PREP_ART_DET:
  case CONTR_INDEF_ART_DET:
  case ART_INDET:
  case CONTR_PREP_ART_INDET:
  case INDEF:
  case INDEF_DET:
  case CONTR_PREP_INDEF:
  case CONTR_PREP_INDEF_DET:
  case DEMO:
  case DEMO_DET:
  case CONTR_PREP_DEMO:
  case CONTR_PREP_DEMO_DET:
  case CONTR_DEMO_INDEF:
  case CONTR_DEMO_INDEF_DET:
  case CONTR_PREP_DEMO_INDEF:
  case CONTR_PREP_DEMO_INDEF_DET:
  case CONTR_CONX_COOR_COP_ART_DET:
    return DET_P;

  case PRON_PERS_TON:
  case CONTR_PREP_PRON_PERS_TON:
  case INDEF_PRON:
  case CONTR_PREP_INDEF_PRON:
  case DEMO_PRON:
  case CONTR_PREP_DEMO_PRON:
  case CONTR_DEMO_INDEF_PRON:
  case CONTR_PREP_DEMO_INDEF_PRON:
  case INDET_PRON:
  case PRON_CORREL:
    return PRON_P;

  case POSE:
  case POSE_DET:
  case POSE_DISTR:
    return DET_POSE_P;

  case POSE_PRON:
    return PRON_POSE_P;

  case NUME:
  case NUME_DET:
  case NUME_CARDI:
  case NUME_CARDI_DET:
  case NUME_ORDI:
  case NUME_ORDI_DET:
  case NUME_PARTI:
  case NUME_PARTI_DET:
    return DET_NUME_P;

  case NUME_PRON:
  case NUME_CARDI_PRON:
  case NUME_ORDI_PRON:
  case NUME_PARTI_PRON:
    return PRON_NUME_P;

  case PRON_PERS_AT:
  case PRON_PERS_AT_REFLEX:
  case PRON_PERS_AT_ACUS:
  case PRON_PERS_AT_DAT:
  case CONTR_PRON_PERS_AT_DAT_AC:
  case CONTR_PRON_PERS_AT_DAT_DAT_AC:
  case CONTR_PRON_PERS_AT:
    return PRON_PROC_P;

  case ADVE:
  case ADVE_LUG:
  case ADVE_TEMP:
  case ADVE_CANT:
  case ADVE_MODO:
  case ADVE_AFIRM:
  case ADVE_NEGA:
  case ADVE_DUBI:
  case LOC_ADVE:
  case LOC_ADVE_LUG:
  case LOC_ADVE_TEMP:
  case LOC_ADVE_CANTI:
  case LOC_ADVE_MODO:
  case LOC_ADVE_AFIR:
  case LOC_ADVE_NEGA:
  case LOC_ADVE_DUBI:
  case ADVE_ESPECIFICADOR:
  case ADVE_ORD:
  case ADVE_MOD:
  case ADVE_COMP:
  case ADVE_CORREL:
  case ADVE_DISTR:
  case LAT:
  case LOC_LAT:
    return ADVE_P;

  case PREP:
  case LOC_PREP:
  case LOC_PREP_LUG:
  case LOC_PREP_TEMP:
  case LOC_PREP_CANT:
  case LOC_PREP_MODO:
  case LOC_PREP_CAUS:
  case LOC_PREP_CONDI:
    return PREP_P;

  case RELA:
    return RELA_P;
  case INTER:
    return INTER_P;
  case EXCLA:
    return EXCLA_P;

  case CONX_COOR:
  case CONX_COOR_COPU:
  case CONX_COOR_DISX:
  case CONX_COOR_ADVERS:
  case LOC_CONX_COOR_COPU:
  case LOC_CONX_COOR_ADVERS:
  case CONX_COOR_DISTRIB:
    return CONX_COOR_P;


  case CONX_SUBOR:
  case CONX_SUBOR_PROPOR:
  case CONX_SUBOR_FINAL:
  case CONX_SUBOR_CONTRAP:
  case CONX_SUBOR_CAUS:
  case CONX_SUBOR_CONCES:
  case CONX_SUBOR_CONSE:
  case CONX_SUBOR_CONDI:
  case CONX_SUBOR_COMPAR:
  case CONX_SUBOR_LOCA:
  case CONX_SUBOR_TEMP:
  case CONX_SUBOR_MODAL:
  case CONX_SUBOR_COMPLETIVA:
  case LOC_CONX_SUBOR_CAUS:
  case LOC_CONX_SUBOR_CONCES:
  case LOC_CONX_SUBOR_CONSE:
  case LOC_CONX_SUBOR_CONDI:
  case LOC_CONX_SUBOR_LOCAL:
  case LOC_CONX_SUBOR_TEMP:
  case LOC_CONX_SUBOR_MODA:
  case LOC_CONX_SUBOR_CONTRAP:
  case LOC_CONX_SUBOR_FINAL:
  case LOC_CONX_SUBOR_PROPOR:
  case CONX_SUBOR_CONTI:
  case LOC_CONX_SUBOR_CORREL:
  case LOC_CONX:
    return CONX_SUBOR_P;

  case NOME:
  case NOME_PROPIO:
  case NUME_MULTI:
  case NUME_COLECT:
  case LOC_SUST:
    return NOME_P;

  case PARTICIPIO:
  case ADXECTIVO:
  case LOC_ADXE:
    return ADXE_P;

  case VERBO:
  case INFINITIVO:
  case PERIFRASE:
    return VERBO_P;

  case XERUNDIO:
    return XERUNDIO_P;

  case INTERX:
  case LOC_INTERX:
    return INTERX_P;


  case PUNTO:
  case PUNTO_E_COMA:
  case DOUS_PUNTOS:
  case PUNTOS_SUSPENSIVOS:
    return PUNTO_P;

  case COMA:
    return COMA_P;


  case APERTURA_INTERROGACION:
    return APERT_INTER_P;

  case PECHE_INTERROGACION:
    return PECHE_INTER_P;

  case APERTURA_EXCLAMACION:
    return APERT_EXCLA_P;

  case PECHE_EXCLAMACION:
    return PECHE_EXCLA_P;

  case APERTURA_PARENTESE:
  case APERTURA_CORCHETE:
    return APERT_PAREN_P;

  case PECHE_PARENTESE:
  case PECHE_CORCHETE:
    return PECHE_PAREN_P;

  case GUION:
  case GUION_BAIXO:
    return GUION_P;

  case DOBLES_COMINNAS:
  case SIMPLES_COMINNAS:
    return COMINNAS_P;

  case SIGNO:
    return SIGNO_P;

  default :
    return NULA_P;
  }
}

/**
 * \author fmendez
 * \remarks
 * \param
 */

void cierra_ficheros_pausas(void){
  if (fp != NULL)
    fclose(fp);
}

/**
 * \author erbanga
 * \remarks
 * \param datos
 */
int pausador_arbol(t_pausas *datos){

  	if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==ADXE_P)||(datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==DET_P)||(datos->categorias[3]==DET_POSE_P)||(datos->categorias[3]==GUION_P)||(datos->categorias[3]==INTER_P)||(datos->categorias[3]==NOME_P)||(datos->categorias[3]==PREP_P)||(datos->categorias[3]==PRON_NUME_P)||(datos->categorias[3]==PRON_P)||(datos->categorias[3]==PRON_POSE_P)||(datos->categorias[3]==PRON_PROC_P)||(datos->categorias[3]==RELA_P)||(datos->categorias[3]==VERBO_P)||(datos->categorias[3]==XERUNDIO_P)){
		if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==ADXE_P)||(datos->categorias[4]==APERT_EXCLA_P)||(datos->categorias[4]==APERT_PAREN_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==DET_POSE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==INTER_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PECHE_EXCLA_P)||(datos->categorias[4]==PECHE_INTER_P)||(datos->categorias[4]==PECHE_PAREN_P)||(datos->categorias[4]==PREP_P)||(datos->categorias[4]==PRON_NUME_P)||(datos->categorias[4]==PRON_P)||(datos->categorias[4]==PRON_POSE_P)||(datos->categorias[4]==PRON_PROC_P)||(datos->categorias[4]==PUNTO_P)||(datos->categorias[4]==VERBO_P)){
			if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==DET_P)||(datos->categorias[3]==DET_POSE_P)||(datos->categorias[3]==INTER_P)||(datos->categorias[3]==PREP_P)||(datos->categorias[3]==PRON_NUME_P)||(datos->categorias[3]==PRON_P)||(datos->categorias[3]==PRON_POSE_P)||(datos->categorias[3]==PRON_PROC_P)||(datos->categorias[3]==RELA_P)||(datos->categorias[3]==VERBO_P)){
				if((datos->categorias[3]==DET_P)||(datos->categorias[3]==DET_POSE_P)||(datos->categorias[3]==INTER_P)||(datos->categorias[3]==PREP_P)||(datos->categorias[3]==PRON_POSE_P)||(datos->categorias[3]==PRON_PROC_P)){
					if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==DET_POSE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==INTER_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PECHE_PAREN_P)||(datos->categorias[4]==PREP_P)||(datos->categorias[4]==PRON_NUME_P)||(datos->categorias[4]==PRON_PROC_P)||(datos->categorias[4]==PUNTO_P)||(datos->categorias[4]==VERBO_P)){
						if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==DET_POSE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==INTER_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PECHE_PAREN_P)||(datos->categorias[4]==PREP_P)||(datos->categorias[4]==PRON_NUME_P)||(datos->categorias[4]==PRON_PROC_P)||(datos->categorias[4]==PUNTO_P))return(0);
						if((datos->categorias[4]==VERBO_P)){
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==VERBO_P)||(datos->categorias[2]==XERUNDIO_P)){
								if((datos->categorias[3]==PREP_P)||(datos->categorias[3]==PRON_PROC_P))return(0);
								if((datos->categorias[3]==DET_P)){
									if((datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P))return(0);
									if((datos->categorias[1]==CONX_COOR_P))return(1);
								}
							}
							if((datos->categorias[2]==DET_P))return(1);
						}
					}
					if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==PRON_P)){
						if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_NUME_P)||(datos->categorias[0]==VERBO_P)){
							if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==APERT_INTER_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==VERBO_P))return(0);
							if((datos->categorias[2]==ADVE_P)){
								if((datos->categorias[0]==ADVE_P))return(0);
								if((datos->categorias[0]==VERBO_P))return(1);
							}
						}
						if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==PRON_PROC_P))return(1);
					}
				}
				if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==PRON_NUME_P)||(datos->categorias[3]==PRON_P)||(datos->categorias[3]==RELA_P)||(datos->categorias[3]==VERBO_P)){
					if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==ADXE_P)||(datos->categorias[4]==APERT_EXCLA_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==DET_POSE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PRON_NUME_P)||(datos->categorias[4]==PRON_P)||(datos->categorias[4]==PRON_POSE_P)||(datos->categorias[4]==PRON_PROC_P)||(datos->categorias[4]==PUNTO_P)||(datos->categorias[4]==VERBO_P)){
						if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==APERT_INTER_P)||(datos->categorias[2]==APERT_PAREN_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==INTER_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PECHE_PAREN_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==PRON_PROC_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==XERUNDIO_P)){
							if((datos->categorias[0]==APERT_INTER_P)||(datos->categorias[0]==APERT_PAREN_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==INTER_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PECHE_PAREN_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_NUME_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)||(datos->categorias[0]==XERUNDIO_P)){
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==APERT_INTER_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==NULA_P)||(datos->categorias[5]==PECHE_PAREN_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_NUME_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==PUNTO_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)||(datos->categorias[5]==XERUNDIO_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PECHE_PAREN_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P))return(0);
									if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)){
										if((datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==VERBO_P))return(0);
										if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==RELA_P)){
											if(datos->n_silabas_pausa_pos < 33)return(0);
											if(datos->n_silabas_pausa_pos > 33)return(1);
										}
									}
								}
								if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_POSE_P)){
									if(datos->n_silabas_pausa_ant < 22){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)||(datos->categorias[6]==XERUNDIO_P)){
											if((datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==VERBO_P))return(0);
											if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==RELA_P)){
												if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P)||(datos->categorias[6]==XERUNDIO_P))return(0);
												if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)){
													if((datos->categorias[0]==VERBO_P))return(0);
													if((datos->categorias[0]==NOME_P))return(1);
												}
											}
										}
										if((datos->categorias[6]==CONX_SUBOR_P)){
											if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==VERBO_P))return(0);
											if((datos->categorias[0]==PREP_P))return(1);
										}
									}
									if(datos->n_silabas_pausa_ant > 22){
										if((datos->categorias[1]==ADXE_P))return(0);
										if((datos->categorias[1]==NOME_P))return(1);
									}
								}
							}
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NULA_P)){
								if((datos->categorias[4]==DET_P)||(datos->categorias[4]==DET_POSE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PRON_P)||(datos->categorias[4]==PUNTO_P)||(datos->categorias[4]==VERBO_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==APERT_INTER_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
										if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==APERT_INTER_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_NUME_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_POSE_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)||(datos->categorias[6]==XERUNDIO_P)){
											if(datos->n_silabas_pausa_ant < 23.5){
												if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)||(datos->categorias[5]==XERUNDIO_P))return(0);
												if((datos->categorias[5]==ADVE_P)){
													if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_POSE_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P))return(0);
													if((datos->categorias[6]==ADXE_P))return(1);
												}
											}
											if(datos->n_silabas_pausa_ant > 23.5){
												if((datos->categorias[2]==GUION_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PRON_PROC_P)||(datos->categorias[2]==RELA_P))return(0);
												if((datos->categorias[2]==CONX_SUBOR_P))return(1);
											}
										}
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NULA_P)){
											if((datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==PRON_PROC_P))return(0);
											if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==RELA_P)){
												if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==DET_P)){
													if(datos->n_silabas_pausa_ant < 7.5)return(0);
													if(datos->n_silabas_pausa_ant > 7.5){
														if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==NULA_P))return(0);
														if((datos->categorias[5]==NOME_P)||(datos->categorias[5]==PUNTO_P)){
															if(datos->n_silabas_pausa_pos < 30)return(1);
															if(datos->n_silabas_pausa_pos > 30){
																if(datos->n_silabas_pausa_pos < 50)return(0);
																if(datos->n_silabas_pausa_pos > 50)return(1);
															}
														}
													}
												}
												if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==NULA_P))return(1);
											}
										}
									}
									if((datos->categorias[1]==COMINNAS_P)){
										if((datos->categorias[0]==DET_P))return(0);
										if((datos->categorias[0]==ADXE_P))return(1);
									}
								}
								if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==ADXE_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==PRON_PROC_P)){
									if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_NUME_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_POSE_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==PUNTO_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)||(datos->categorias[5]==XERUNDIO_P)){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==APERT_PAREN_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==NULA_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
											if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==APERT_INTER_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PRON_P)){
												if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PECHE_INTER_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_NUME_P)||(datos->categorias[1]==VERBO_P)){
													if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NULA_P))return(0);
													if((datos->categorias[0]==ADXE_P)){
														if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P))return(0);
														if((datos->categorias[1]==NOME_P)){
															if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P))return(0);
															if((datos->categorias[2]==NOME_P)){
																if((datos->categorias[4]==ADVE_P))return(0);
																if((datos->categorias[4]==ADXE_P))return(1);
															}
														}
													}
												}
												if((datos->categorias[1]==ADXE_P)){
													if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P))return(0);
													if((datos->categorias[6]==ADXE_P))return(1);
												}
											}
											if((datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==RELA_P)){
												if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==VERBO_P)){
													if(datos->n_silabas_pausa_ant < 2.5){
														if((datos->categorias[0]==ADXE_P))return(0);
														if((datos->categorias[0]==DET_P))return(1);
													}
													if(datos->n_silabas_pausa_ant > 2.5)return(0);
												}
												if((datos->categorias[1]==DET_P))return(1);
											}
										}
										if((datos->categorias[6]==RELA_P))return(1);
									}
									if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==NOME_P)){
										if(datos->n_silabas_pausa_ant < 7.5)return(0);
										if(datos->n_silabas_pausa_ant > 7.5)return(1);
									}
								}
							}
						}
						if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_NUME_P)||(datos->categorias[2]==VERBO_P)){
							if((datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==PRON_NUME_P)||(datos->categorias[3]==RELA_P)||(datos->categorias[3]==VERBO_P)){
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==APERT_EXCLA_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==NULA_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_NUME_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PUNTO_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==XERUNDIO_P)){
									if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)){
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P))return(0);
										if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NULA_P)){
											if(datos->n_silabas_pausa_ant < 8.5){
												if((datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P))return(0);
												if((datos->categorias[2]==VERBO_P)){
													if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P))return(0);
													if((datos->categorias[0]==NULA_P)){
														if((datos->categorias[3]==CONX_SUBOR_P))return(0);
														if((datos->categorias[3]==VERBO_P))return(1);
													}
												}
											}
											if(datos->n_silabas_pausa_ant > 8.5){
												if((datos->categorias[0]==NULA_P))return(0);
												if((datos->categorias[0]==NOME_P))return(1);
											}
										}
									}
									if((datos->categorias[0]==PREP_P)){
										if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==NULA_P)||(datos->categorias[6]==PUNTO_P))return(0);
										if((datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P)){
											if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==VERBO_P))return(0);
											if((datos->categorias[4]==DET_P))return(1);
										}
									}
								}
								if((datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==VERBO_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==APERT_INTER_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P)){
										if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P)){
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P))return(0);
											if((datos->categorias[6]==ADXE_P))return(1);
										}
										if((datos->categorias[0]==RELA_P))return(1);
									}
									if((datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==PRON_PROC_P))return(1);
								}
							}
							if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==PRON_P)){
								if(datos->n_silabas_pausa_pos < 81){
									if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==APERT_INTER_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NULA_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==PUNTO_P)||(datos->categorias[5]==XERUNDIO_P)){
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==PRON_PROC_P)){
											if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P))return(0);
											if((datos->categorias[2]==NULA_P)){
												if(datos->n_silabas_pausa_ant < 2.5)return(0);
												if(datos->n_silabas_pausa_ant > 2.5){
													if((datos->categorias[4]==ADVE_P))return(0);
													if((datos->categorias[4]==VERBO_P))return(1);
												}
											}
										}
										if((datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P))return(0);
											if((datos->categorias[0]==NOME_P)){
												if((datos->categorias[4]==ADVE_P))return(0);
												if((datos->categorias[4]==NOME_P)||(datos->categorias[4]==VERBO_P))return(1);
											}
										}
									}
									if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
												if(datos->n_silabas_pausa_pos < 28.5)return(0);
												if(datos->n_silabas_pausa_pos > 28.5){
													if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)){
														if(datos->n_silabas_pausa_pos < 54)return(0);
														if(datos->n_silabas_pausa_pos > 54){
															if(datos->n_silabas_pausa_pos < 56.5)return(1);
															if(datos->n_silabas_pausa_pos > 56.5){
																if((datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P))return(0);
																if((datos->categorias[6]==PREP_P)){
																	if(datos->n_silabas_pausa_ant < 2.5)return(0);
																	if(datos->n_silabas_pausa_ant > 2.5)return(1);
																}
															}
														}
													}
													if((datos->categorias[0]==DET_P)){
														if(datos->n_silabas_pausa_ant < 9.5)return(0);
														if(datos->n_silabas_pausa_ant > 9.5)return(1);
													}
												}
											}
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==NOME_P)){
												if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==GUION_P))return(0);
												if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P)){
													if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==APERT_EXCLA_P))return(0);
													if((datos->categorias[4]==DET_P)||(datos->categorias[4]==NOME_P)){
														if(datos->n_silabas_pausa_pos < 70){
															if((datos->categorias[2]==NULA_P)){
																if((datos->categorias[6]==NOME_P))return(0);
																if((datos->categorias[6]==ADVE_P))return(1);
															}
															if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==VERBO_P))return(1);
														}
														if(datos->n_silabas_pausa_pos > 70)return(0);
													}
												}
											}
										}
										if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)){
											if((datos->categorias[2]==VERBO_P))return(0);
											if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_NUME_P))return(1);
										}
									}
								}
								if(datos->n_silabas_pausa_pos > 81){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==NULA_P)){
										if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==VERBO_P))return(0);
										if((datos->categorias[6]==PREP_P))return(1);
									}
									if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NOME_P))return(1);
								}
							}
						}
					}
					if((datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==PREP_P)){
						if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PRON_NUME_P)||(datos->categorias[2]==PRON_PROC_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==VERBO_P)){
								if(datos->n_silabas_pausa_pos < 108){
									if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P))return(0);
									if((datos->categorias[0]==PREP_P)){
										if((datos->categorias[5]==NOME_P))return(0);
										if((datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)){
											if((datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==PRON_PROC_P))return(0);
											if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==VERBO_P))return(1);
										}
									}
								}
								if(datos->n_silabas_pausa_pos > 108)return(1);
							}
							if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_P)){
								if(datos->n_silabas_pausa_ant < 11.5){
									if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P))return(0);
									if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P)){
										if(datos->n_silabas_pausa_ant < 5.5){
											if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==PRON_P)||(datos->categorias[3]==VERBO_P))return(0);
												if((datos->categorias[3]==RELA_P)){
													if(datos->n_silabas_pausa_ant < 1.5)return(1);
													if(datos->n_silabas_pausa_ant > 1.5)return(0);
												}
											}
											if((datos->categorias[1]==CONX_COOR_P))return(1);
										}
										if(datos->n_silabas_pausa_ant > 5.5){
											if((datos->categorias[1]==DET_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)){
													if((datos->categorias[2]==NOME_P))return(0);
													if((datos->categorias[2]==CONX_COOR_P))return(1);
												}
												if((datos->categorias[0]==VERBO_P))return(1);
											}
											if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P))return(1);
										}
									}
								}
								if(datos->n_silabas_pausa_ant > 11.5){
									if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)){
										if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P))return(0);
										if((datos->categorias[0]==DET_P)||(datos->categorias[0]==DET_POSE_P)){
											if(datos->n_silabas_pausa_ant < 12.5)return(0);
											if(datos->n_silabas_pausa_ant > 12.5)return(1);
										}
									}
									if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==VERBO_P)){
										if((datos->categorias[5]==PRON_P))return(0);
										if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P))return(1);
									}
								}
							}
						}
						if((datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==PRON_PROC_P))return(1);
					}
				}
			}
			if((datos->categorias[3]==ADXE_P)||(datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P)||(datos->categorias[3]==XERUNDIO_P)){
				if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==APERT_PAREN_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==NOME_P)||(datos->categorias[4]==PECHE_EXCLA_P)||(datos->categorias[4]==PECHE_INTER_P)||(datos->categorias[4]==PECHE_PAREN_P)||(datos->categorias[4]==PREP_P)||(datos->categorias[4]==PRON_NUME_P)||(datos->categorias[4]==PRON_P)||(datos->categorias[4]==PUNTO_P)){
					if((datos->categorias[6]==APERT_INTER_P)||(datos->categorias[6]==APERT_PAREN_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==NULA_P)||(datos->categorias[6]==PECHE_INTER_P)||(datos->categorias[6]==PECHE_PAREN_P)){
						if(datos->n_silabas_pausa_ant < 2.5){
							if((datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P))return(0);
							if((datos->categorias[0]==CONX_SUBOR_P))return(1);
						}
						if(datos->n_silabas_pausa_ant > 2.5)return(0);
					}
					if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
						if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==APERT_EXCLA_P)||(datos->categorias[2]==APERT_PAREN_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==PECHE_INTER_P)||(datos->categorias[2]==PECHE_PAREN_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==PRON_PROC_P)||(datos->categorias[2]==VERBO_P)){
							if(datos->n_silabas_pausa_ant < 10.5){
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PECHE_EXCLA_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P)){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==RELA_P)){
											if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PECHE_PAREN_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P))return(0);
											if((datos->categorias[0]==DET_P)){
												if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P))return(0);
												if((datos->categorias[6]==ADXE_P)){
													if((datos->categorias[2]==APERT_PAREN_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==VERBO_P))return(0);
													if((datos->categorias[2]==DET_P)){
														if(datos->n_silabas_pausa_pos < 48.5)return(1);
														if(datos->n_silabas_pausa_pos > 48.5)return(0);
													}
												}
											}
										}
										if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
											if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PECHE_PAREN_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==RELA_P)){
												if(datos->n_silabas_pausa_ant < 9.5)return(0);
												if(datos->n_silabas_pausa_ant > 9.5){
													if((datos->categorias[5]==NOME_P))return(0);
													if((datos->categorias[5]==VERBO_P))return(1);
												}
											}
											if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P)){
												if(datos->n_silabas_pausa_pos < 100){
													if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
														if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==PECHE_EXCLA_P)||(datos->categorias[5]==RELA_P))return(0);
														if((datos->categorias[5]==NOME_P)||(datos->categorias[5]==VERBO_P)){
															if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
																if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==VERBO_P)){
																	if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PUNTO_P))return(0);
																	if((datos->categorias[6]==VERBO_P)){
																		if(datos->n_silabas_pausa_ant < 8.5)return(0);
																		if(datos->n_silabas_pausa_ant > 8.5){
																			if((datos->categorias[4]==PREP_P))return(0);
																			if((datos->categorias[4]==NOME_P))return(1);
																		}
																	}
																}
																if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==PRON_PROC_P)){
																	if((datos->categorias[1]==DET_P)||(datos->categorias[1]==VERBO_P)){
																		if(datos->n_silabas_pausa_ant < 7.5)return(1);
																		if(datos->n_silabas_pausa_ant > 7.5)return(0);
																	}
																	if((datos->categorias[1]==NOME_P))return(1);
																}
															}
															if((datos->categorias[1]==ADVE_P))return(1);
														}
													}
													if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==PREP_P)){
														if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P))return(0);
														if((datos->categorias[0]==VERBO_P))return(1);
													}
												}
												if(datos->n_silabas_pausa_pos > 100)return(1);
											}
										}
									}
									if((datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==PRON_PROC_P)){
										if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P))return(0);
										if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P))return(1);
									}
								}
								if((datos->categorias[5]==PRON_PROC_P))return(1);
							}
							if(datos->n_silabas_pausa_ant > 10.5){
								if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
											if(datos->n_silabas_pausa_pos < 126.5){
												if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==PREP_P)){
													if((datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P)){
														if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P))return(0);
														if((datos->categorias[6]==PREP_P)){
															if(datos->n_silabas_pausa_pos < 63)return(0);
															if(datos->n_silabas_pausa_pos > 63){
																if((datos->categorias[0]==NOME_P))return(0);
																if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==PREP_P))return(1);
															}
														}
													}
													if((datos->categorias[3]==ADXE_P)){
														if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P))return(0);
														if((datos->categorias[0]==DET_P))return(1);
													}
												}
												if((datos->categorias[4]==NOME_P)){
													if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==PREP_P))return(0);
													if((datos->categorias[5]==NOME_P)){
														if((datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P))return(0);
														if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==NOME_P)){
															if((datos->categorias[6]==PREP_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
																if((datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P)){
																	if((datos->categorias[3]==NOME_P))return(0);
																	if((datos->categorias[3]==ADXE_P))return(1);
																}
																if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P)){
																	if(datos->n_silabas_pausa_ant < 20.5)return(1);
																	if(datos->n_silabas_pausa_ant > 20.5)return(0);
																}
															}
															if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P))return(1);
														}
													}
												}
											}
											if(datos->n_silabas_pausa_pos > 126.5)return(1);
										}
										if((datos->categorias[6]==ADXE_P)){
											if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P))return(0);
											if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P)){
													if(datos->n_silabas_pausa_ant < 18.5){
														if(datos->n_silabas_pausa_pos < 76.5)return(1);
														if(datos->n_silabas_pausa_pos > 76.5)return(0);
													}
													if(datos->n_silabas_pausa_ant > 18.5)return(0);
												}
												if((datos->categorias[0]==VERBO_P))return(1);
											}
										}
									}
									if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)){
										if((datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)){
											if(datos->n_silabas_pausa_ant < 42.5){
												if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==PREP_P))return(0);
												if((datos->categorias[4]==NOME_P))return(1);
											}
											if(datos->n_silabas_pausa_ant > 42.5)return(1);
										}
										if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==PREP_P))return(1);
									}
								}
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
									if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
										if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P)){
											if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NOME_P))return(0);
											if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P)){
													if(datos->n_silabas_pausa_pos < 15)return(1);
													if(datos->n_silabas_pausa_pos > 15)return(0);
												}
												if((datos->categorias[6]==ADVE_P))return(1);
											}
										}
										if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P)){
											if(datos->n_silabas_pausa_pos < 19.5)return(0);
											if(datos->n_silabas_pausa_pos > 19.5)return(1);
										}
									}
									if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P))return(1);
								}
							}
						}
						if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)){
							if(datos->n_silabas_pausa_ant < 17.5){
								if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
									if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==APERT_INTER_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
											if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==RELA_P))return(0);
											if((datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P)){
												if(datos->n_silabas_pausa_ant < 7.5){
													if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
														if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P))return(0);
														if((datos->categorias[6]==PREP_P)){
															if(datos->n_silabas_pausa_ant < 5.5)return(0);
															if(datos->n_silabas_pausa_ant > 5.5)return(1);
														}
													}
													if((datos->categorias[1]==ADXE_P)){
														if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==DET_P))return(0);
														if((datos->categorias[0]==CONX_COOR_P))return(1);
													}
												}
												if(datos->n_silabas_pausa_ant > 7.5){
													if((datos->categorias[3]==ADXE_P))return(0);
													if((datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==NOME_P)){
														if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==PREP_P)){
															if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
																if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==NOME_P)){
																	if((datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P))return(0);
																	if((datos->categorias[5]==DET_P)){
																		if(datos->n_silabas_pausa_pos < 35)return(0);
																		if(datos->n_silabas_pausa_pos > 35)return(1);
																	}
																}
																if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
																	if((datos->categorias[5]==NOME_P)||(datos->categorias[5]==VERBO_P)){
																		if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==PREP_P)){
																			if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_NUME_P))return(0);
																			if((datos->categorias[2]==PREP_P)){
																				if(datos->n_silabas_pausa_pos < 43.5){
																					if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P))return(0);
																					if((datos->categorias[6]==PREP_P)){
																						if(datos->n_silabas_pausa_ant < 10.5)return(0);
																						if(datos->n_silabas_pausa_ant > 10.5)return(1);
																					}
																				}
																				if(datos->n_silabas_pausa_pos > 43.5){
																					if(datos->n_silabas_pausa_ant < 11)return(1);
																					if(datos->n_silabas_pausa_ant > 11)return(0);
																				}
																			}
																		}
																		if((datos->categorias[0]==CONX_COOR_P))return(1);
																	}
																	if((datos->categorias[5]==PREP_P)){
																		if((datos->categorias[0]==DET_P))return(0);
																		if((datos->categorias[0]==PREP_P))return(1);
																	}
																}
															}
															if((datos->categorias[1]==ADXE_P))return(1);
														}
														if((datos->categorias[2]==NOME_P)){
															if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==DET_P))return(0);
															if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P))return(1);
														}
													}
												}
											}
										}
										if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==RELA_P)){
											if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==PREP_P)){
												if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==RELA_P))return(0);
												if((datos->categorias[6]==DET_NUME_P)){
													if((datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P))return(0);
													if((datos->categorias[1]==NOME_P))return(1);
												}
											}
											if((datos->categorias[4]==NOME_P)){
												if((datos->categorias[0]==CONX_COOR_P))return(0);
												if((datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P))return(1);
											}
										}
									}
									if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==VERBO_P)){
										if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==APERT_PAREN_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P))return(0);
											if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P)){
												if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)){
													if(datos->n_silabas_pausa_pos < 124){
														if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P)){
															if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P))return(0);
															if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==NOME_P)){
																if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P)){
																	if((datos->categorias[4]==PREP_P)){
																		if((datos->categorias[3]==NOME_P))return(0);
																		if((datos->categorias[3]==ADXE_P))return(1);
																	}
																	if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==NOME_P)){
																		if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==PREP_P))return(0);
																		if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==VERBO_P)){
																			if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==VERBO_P)){
																				if((datos->categorias[5]==ADXE_P))return(0);
																				if((datos->categorias[5]==NOME_P)){
																					if(datos->n_silabas_pausa_pos < 53.5){
																						if(datos->n_silabas_pausa_pos < 19.5)return(0);
																						if(datos->n_silabas_pausa_pos > 19.5)return(1);
																					}
																					if(datos->n_silabas_pausa_pos > 53.5)return(0);
																				}
																			}
																			if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==DET_P))return(1);
																		}
																	}
																}
																if((datos->categorias[0]==ADXE_P))return(1);
															}
														}
														if((datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_PROC_P)){
															if(datos->n_silabas_pausa_ant < 4.5){
																if((datos->categorias[1]==NULA_P)||(datos->categorias[1]==RELA_P)){
																	if((datos->categorias[3]==ADXE_P))return(0);
																	if((datos->categorias[3]==NOME_P)){
																		if((datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==PREP_P))return(0);
																		if((datos->categorias[4]==NOME_P)){
																			if((datos->categorias[5]==PREP_P))return(0);
																			if((datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)){
																				if((datos->categorias[2]==NULA_P)){
																					if((datos->categorias[5]==NOME_P))return(0);
																					if((datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_P))return(1);
																				}
																				if((datos->categorias[2]==PREP_P))return(1);
																			}
																		}
																	}
																}
																if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P))return(1);
															}
															if(datos->n_silabas_pausa_ant > 4.5){
																if((datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)){
																	if(datos->n_silabas_pausa_ant < 12.5){
																		if((datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P))return(0);
																		if((datos->categorias[5]==CONX_SUBOR_P)){
																			if((datos->categorias[0]==NULA_P))return(0);
																			if((datos->categorias[0]==VERBO_P))return(1);
																		}
																	}
																	if(datos->n_silabas_pausa_ant > 12.5){
																		if((datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P)){
																			if((datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==NOME_P))return(0);
																			if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==PREP_P)){
																				if(datos->n_silabas_pausa_ant < 15.5){
																					if(datos->n_silabas_pausa_pos < 99)return(1);
																					if(datos->n_silabas_pausa_pos > 99)return(0);
																				}
																				if(datos->n_silabas_pausa_ant > 15.5)return(0);
																			}
																		}
																		if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==PREP_P))return(1);
																	}
																}
																if((datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P))return(1);
															}
														}
													}
													if(datos->n_silabas_pausa_pos > 124)return(1);
												}
												if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NOME_P)){
													if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
														if(datos->n_silabas_pausa_pos < 76.5){
															if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P)){
																if(datos->n_silabas_pausa_pos < 70){
																	if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==VERBO_P))return(0);
																	if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)){
																		if(datos->n_silabas_pausa_ant < 8.5){
																			if((datos->categorias[3]==NOME_P)){
																				if(datos->n_silabas_pausa_pos < 8)return(1);
																				if(datos->n_silabas_pausa_pos > 8){
																					if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P))return(0);
																					if((datos->categorias[1]==NOME_P)){
																						if(datos->n_silabas_pausa_pos < 15.5)return(1);
																						if(datos->n_silabas_pausa_pos > 15.5){
																							if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==NOME_P))return(0);
																							if((datos->categorias[4]==PREP_P)){
																								if((datos->categorias[5]==NOME_P))return(0);
																								if((datos->categorias[5]==DET_P))return(1);
																							}
																						}
																					}
																				}
																			}
																			if((datos->categorias[3]==ADXE_P)){
																				if(datos->n_silabas_pausa_ant < 7.5){
																					if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)){
																						if((datos->categorias[4]==NOME_P)||(datos->categorias[4]==PREP_P))return(0);
																						if((datos->categorias[4]==ADXE_P))return(1);
																					}
																					if((datos->categorias[0]==ADXE_P)){
																						if(datos->n_silabas_pausa_pos < 19.5)return(1);
																						if(datos->n_silabas_pausa_pos > 19.5)return(0);
																					}
																				}
																				if(datos->n_silabas_pausa_ant > 7.5)return(1);
																			}
																		}
																		if(datos->n_silabas_pausa_ant > 8.5){
																			if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==APERT_PAREN_P)||(datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==PREP_P))return(0);
																			if((datos->categorias[4]==NOME_P)){
																				if(datos->n_silabas_pausa_ant < 13)return(0);
																				if(datos->n_silabas_pausa_ant > 13)return(1);
																			}
																		}
																	}
																}
																if(datos->n_silabas_pausa_pos > 70){
																	if(datos->n_silabas_pausa_ant < 13)return(1);
																	if(datos->n_silabas_pausa_ant > 13)return(0);
																}
															}
															if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)){
																if((datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P)){
																	if((datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==NOME_P))return(0);
																	if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==PREP_P)){
																		if((datos->categorias[0]==NULA_P))return(0);
																		if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==VERBO_P))return(1);
																	}
																}
																if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P))return(1);
															}
														}
														if(datos->n_silabas_pausa_pos > 76.5)return(0);
													}
													if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==PREP_P)){
														if(datos->n_silabas_pausa_ant < 10){
															if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==NOME_P))return(0);
															if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P))return(1);
														}
														if(datos->n_silabas_pausa_ant > 10){
															if((datos->categorias[1]==ADXE_P))return(0);
															if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P))return(1);
														}
													}
												}
											}
										}
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==XERUNDIO_P)){
											if(datos->n_silabas_pausa_ant < 6.5)return(0);
											if(datos->n_silabas_pausa_ant > 6.5){
												if((datos->categorias[6]==PUNTO_P))return(0);
												if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P))return(1);
											}
										}
									}
								}
								if((datos->categorias[6]==PRON_P))return(1);
							}
							if(datos->n_silabas_pausa_ant > 17.5){
								if((datos->categorias[4]==COMINNAS_P)||(datos->categorias[4]==GUION_P)||(datos->categorias[4]==NOME_P)){
									if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==APERT_PAREN_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P))return(0);
										if((datos->categorias[2]==DET_POSE_P))return(1);
									}
									if((datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==NOME_P)){
										if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)){
											if((datos->categorias[3]==NOME_P))return(0);
											if((datos->categorias[3]==ADXE_P))return(1);
										}
										if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P))return(1);
									}
								}
								if((datos->categorias[4]==ADXE_P)||(datos->categorias[4]==PREP_P)){
									if(datos->n_silabas_pausa_pos < 14.5){
										if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==NOME_P))return(0);
										if((datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P)){
											if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P))return(0);
											if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P))return(1);
										}
									}
									if(datos->n_silabas_pausa_pos > 14.5){
										if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)){
											if(datos->n_silabas_pausa_pos < 39.5){
												if((datos->categorias[1]==PREP_P))return(0);
												if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)){
													if((datos->categorias[3]==ADXE_P))return(0);
													if((datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P))return(1);
												}
											}
											if(datos->n_silabas_pausa_pos > 39.5)return(0);
										}
										if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P)){
											if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_NUME_P)){
												if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P))return(0);
												if((datos->categorias[6]==PRON_PROC_P))return(1);
											}
											if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)){
												if((datos->categorias[0]==DET_P)){
													if(datos->n_silabas_pausa_pos < 27){
														if((datos->categorias[2]==PREP_P))return(0);
														if((datos->categorias[2]==NOME_P))return(1);
													}
													if(datos->n_silabas_pausa_pos > 27)return(1);
												}
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P))return(1);
											}
										}
									}
								}
							}
						}
					}
				}
				if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==PRON_PROC_P)||(datos->categorias[4]==VERBO_P)){
					if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PRON_NUME_P)||(datos->categorias[5]==PUNTO_P)){
						if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==APERT_PAREN_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==NULA_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==XERUNDIO_P)){
							if((datos->categorias[3]==ADXE_P)||(datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P)||(datos->categorias[3]==XERUNDIO_P)){
								if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)||(datos->categorias[2]==XERUNDIO_P)){
									if((datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PUNTO_P)){
										if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PRON_NUME_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P)){
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==APERT_PAREN_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==NULA_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==XERUNDIO_P)){
												if((datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P))return(0);
												if((datos->categorias[3]==ADXE_P)){
													if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==VERBO_P))return(0);
													if((datos->categorias[2]==PREP_P))return(1);
												}
											}
											if((datos->categorias[6]==DET_NUME_P)){
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==VERBO_P))return(0);
												if((datos->categorias[0]==DET_P))return(1);
											}
										}
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==PREP_P)){
											if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P)){
												if(datos->n_silabas_pausa_pos < 28){
													if(datos->n_silabas_pausa_pos < 17)return(0);
													if(datos->n_silabas_pausa_pos > 17)return(1);
												}
												if(datos->n_silabas_pausa_pos > 28)return(0);
											}
											if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==PREP_P)){
												if(datos->n_silabas_pausa_pos < 65){
													if((datos->categorias[1]==PREP_P))return(0);
													if((datos->categorias[1]==ADVE_P))return(1);
												}
												if(datos->n_silabas_pausa_pos > 65)return(0);
											}
										}
									}
									if((datos->categorias[5]==ADXE_P)){
										if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P))return(0);
										if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==PREP_P))return(1);
									}
								}
								if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)){
									if(datos->n_silabas_pausa_ant < 16.5){
										if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P)){
											if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==VERBO_P))return(0);
											if((datos->categorias[4]==DET_NUME_P))return(1);
										}
										if((datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P)){
											if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[3]==GUION_P)||(datos->categorias[3]==NOME_P))return(0);
												if((datos->categorias[3]==ADXE_P))return(1);
											}
											if((datos->categorias[1]==DET_P)||(datos->categorias[1]==PRON_P))return(1);
										}
									}
									if(datos->n_silabas_pausa_ant > 16.5){
										if(datos->n_silabas_pausa_pos < 10)return(0);
										if(datos->n_silabas_pausa_pos > 10)return(1);
									}
								}
							}
							if((datos->categorias[3]==DET_NUME_P))return(1);
						}
						if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
							if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==XERUNDIO_P)){
								if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)){
									if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)){
										if((datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P))return(0);
										if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)){
											if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P))return(0);
											if((datos->categorias[2]==DET_P)){
												if(datos->n_silabas_pausa_ant < 8.5)return(0);
												if(datos->n_silabas_pausa_ant > 8.5){
													if(datos->n_silabas_pausa_ant < 14){
														if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==VERBO_P))return(0);
														if((datos->categorias[4]==DET_P))return(1);
													}
													if(datos->n_silabas_pausa_ant > 14)return(0);
												}
											}
										}
									}
									if((datos->categorias[0]==PREP_P)){
										if((datos->categorias[1]==DET_NUME_P))return(0);
										if((datos->categorias[1]==ADVE_P))return(1);
									}
								}
								if((datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==NOME_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)){
										if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==VERBO_P))return(0);
										if((datos->categorias[6]==DET_P)){
											if((datos->categorias[3]==NOME_P))return(0);
											if((datos->categorias[3]==ADXE_P))return(1);
										}
									}
									if((datos->categorias[1]==ADXE_P)){
										if(datos->n_silabas_pausa_ant < 16){
											if(datos->n_silabas_pausa_ant < 3.5)return(0);
											if(datos->n_silabas_pausa_ant > 3.5){
												if(datos->n_silabas_pausa_pos < 102)return(1);
												if(datos->n_silabas_pausa_pos > 102)return(0);
											}
										}
										if(datos->n_silabas_pausa_ant > 16)return(0);
									}
								}
							}
							if((datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==VERBO_P)){
								if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==VERBO_P)){
									if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)){
										if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==VERBO_P))return(0);
										if((datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)){
											if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
												if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==NOME_P)){
													if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P))return(0);
													if((datos->categorias[1]==CONX_SUBOR_P)){
														if((datos->categorias[0]==VERBO_P))return(0);
														if((datos->categorias[0]==CONX_SUBOR_P))return(1);
													}
												}
												if((datos->categorias[5]==PRON_NUME_P))return(1);
											}
											if((datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)){
												if(datos->n_silabas_pausa_ant < 3.5){
													if((datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)){
														if(datos->n_silabas_pausa_ant < 2.5)return(0);
														if(datos->n_silabas_pausa_ant > 2.5){
															if((datos->categorias[6]==PREP_P)){
																if(datos->n_silabas_pausa_pos < 23)return(1);
																if(datos->n_silabas_pausa_pos > 23)return(0);
															}
															if((datos->categorias[6]==DET_P))return(1);
														}
													}
													if((datos->categorias[6]==NOME_P))return(1);
												}
												if(datos->n_silabas_pausa_ant > 3.5){
													if(datos->n_silabas_pausa_pos < 12)return(0);
													if(datos->n_silabas_pausa_pos > 12){
														if(datos->n_silabas_pausa_pos < 15.5){
															if((datos->categorias[4]==DET_P))return(0);
															if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==VERBO_P))return(1);
														}
														if(datos->n_silabas_pausa_pos > 15.5){
															if((datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
																if(datos->n_silabas_pausa_ant < 4.5)return(0);
																if(datos->n_silabas_pausa_ant > 4.5){
																	if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==VERBO_P)){
																		if((datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
																			if(datos->n_silabas_pausa_pos < 27.5){
																				if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P))return(0);
																				if((datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P)){
																					if((datos->categorias[0]==NULA_P))return(0);
																					if((datos->categorias[0]==VERBO_P))return(1);
																				}
																			}
																			if(datos->n_silabas_pausa_pos > 27.5){
																				if(datos->n_silabas_pausa_ant < 16.5)return(0);
																				if(datos->n_silabas_pausa_ant > 16.5){
																					if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P))return(0);
																					if((datos->categorias[6]==PREP_P))return(1);
																				}
																			}
																		}
																		if((datos->categorias[1]==NULA_P)){
																			if((datos->categorias[3]==NOME_P)){
																				if((datos->categorias[5]==NOME_P))return(0);
																				if((datos->categorias[5]==ADXE_P))return(1);
																			}
																			if((datos->categorias[3]==ADXE_P))return(1);
																		}
																	}
																	if((datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P)){
																		if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==DET_P)){
																			if(datos->n_silabas_pausa_pos < 69.5)return(0);
																			if(datos->n_silabas_pausa_pos > 69.5){
																				if((datos->categorias[0]==DET_POSE_P))return(0);
																				if((datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P))return(1);
																			}
																		}
																		if((datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==VERBO_P))return(1);
																	}
																}
															}
															if((datos->categorias[1]==PRON_P))return(1);
														}
													}
												}
											}
										}
									}
									if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==XERUNDIO_P)){
										if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==RELA_P)){
											if(datos->n_silabas_pausa_ant < 11.5){
												if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==VERBO_P))return(0);
												if((datos->categorias[2]==ADXE_P)){
													if((datos->categorias[0]==NOME_P))return(0);
													if((datos->categorias[0]==ADXE_P))return(1);
												}
											}
											if(datos->n_silabas_pausa_ant > 11.5){
												if(datos->n_silabas_pausa_pos < 14)return(0);
												if(datos->n_silabas_pausa_pos > 14){
													if((datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==PREP_P))return(0);
													if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P)){
														if(datos->n_silabas_pausa_pos < 41.5){
															if(datos->n_silabas_pausa_ant < 15.5){
																if((datos->categorias[1]==DET_P))return(0);
																if((datos->categorias[1]==NOME_P))return(1);
															}
															if(datos->n_silabas_pausa_ant > 15.5)return(1);
														}
														if(datos->n_silabas_pausa_pos > 41.5){
															if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_P))return(0);
															if((datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==VERBO_P)){
																if(datos->n_silabas_pausa_ant < 16)return(1);
																if(datos->n_silabas_pausa_ant > 16){
																	if((datos->categorias[3]==ADXE_P))return(0);
																	if((datos->categorias[3]==NOME_P))return(1);
																}
															}
														}
													}
												}
											}
										}
										if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P)){
											if((datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==GUION_P))return(0);
											if((datos->categorias[3]==ADXE_P)||(datos->categorias[3]==NOME_P)){
												if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)){
													if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P))return(0);
													if((datos->categorias[1]==PREP_P))return(1);
												}
												if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==VERBO_P)){
													if((datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
														if((datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)){
															if(datos->n_silabas_pausa_ant < 10.5)return(0);
															if(datos->n_silabas_pausa_ant > 10.5){
																if(datos->n_silabas_pausa_pos < 24.5){
																	if((datos->categorias[4]==ADVE_P))return(0);
																	if((datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==VERBO_P))return(1);
																}
																if(datos->n_silabas_pausa_pos > 24.5)return(0);
															}
														}
														if((datos->categorias[2]==ADVE_P))return(1);
													}
													if((datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)){
														if(datos->n_silabas_pausa_pos < 137.5){
															if(datos->n_silabas_pausa_ant < 14.5){
																if(datos->n_silabas_pausa_pos < 29.5){
																	if(datos->n_silabas_pausa_pos < 11.5){
																		if((datos->categorias[4]==VERBO_P))return(0);
																		if((datos->categorias[4]==DET_P))return(1);
																	}
																	if(datos->n_silabas_pausa_pos > 11.5)return(1);
																}
																if(datos->n_silabas_pausa_pos > 29.5){
																	if(datos->n_silabas_pausa_pos < 88){
																		if((datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P))return(0);
																		if((datos->categorias[6]==CONX_COOR_P)){
																			if((datos->categorias[3]==NOME_P))return(0);
																			if((datos->categorias[3]==ADXE_P))return(1);
																		}
																	}
																	if(datos->n_silabas_pausa_pos > 88)return(1);
																}
															}
															if(datos->n_silabas_pausa_ant > 14.5)return(1);
														}
														if(datos->n_silabas_pausa_pos > 137.5)return(0);
													}
												}
											}
										}
									}
								}
								if((datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==PREP_P)){
									if(datos->n_silabas_pausa_ant < 16){
										if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==DET_P)){
											if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_P)||(datos->categorias[0]==RELA_P)){
												if(datos->n_silabas_pausa_pos < 92){
													if(datos->n_silabas_pausa_ant < 8.5){
														if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==RELA_P))return(0);
														if((datos->categorias[0]==DET_P))return(1);
													}
													if(datos->n_silabas_pausa_ant > 8.5)return(0);
												}
												if(datos->n_silabas_pausa_pos > 92){
													if((datos->categorias[6]==DET_P))return(0);
													if((datos->categorias[6]==NOME_P))return(1);
												}
											}
											if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)){
												if((datos->categorias[6]==CONX_COOR_P))return(0);
												if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
													if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==PREP_P)){
														if((datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P))return(0);
														if((datos->categorias[1]==NOME_P)){
															if((datos->categorias[6]==VERBO_P))return(0);
															if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P))return(1);
														}
													}
													if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==PRON_PROC_P))return(1);
												}
											}
										}
										if((datos->categorias[4]==VERBO_P))return(1);
									}
									if(datos->n_silabas_pausa_ant > 16){
										if(datos->n_silabas_pausa_ant < 22.5)return(1);
										if(datos->n_silabas_pausa_ant > 22.5)return(0);
									}
								}
							}
						}
					}
					if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P)){
						if(datos->n_silabas_pausa_ant < 9.5){
							if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PECHE_INTER_P)||(datos->categorias[0]==VERBO_P)){
								if((datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==VERBO_P)){
									if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==APERT_INTER_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==RELA_P)){
										if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P))return(0);
										if((datos->categorias[6]==ADXE_P)){
											if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==DET_POSE_P))return(0);
											if((datos->categorias[5]==PREP_P))return(1);
										}
									}
									if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==VERBO_P)){
										if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P))return(0);
										if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==PREP_P))return(1);
									}
								}
								if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_POSE_P)){
									if((datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_P))return(0);
										if((datos->categorias[6]==ADVE_P))return(1);
									}
									if((datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==PRON_PROC_P))return(1);
								}
							}
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PECHE_PAREN_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)){
								if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_NUME_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
									if((datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==RELA_P)){
										if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P)){
											if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_NUME_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
												if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==VERBO_P))return(0);
												if((datos->categorias[2]==PREP_P)){
													if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P))return(0);
													if((datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==NOME_P)){
														if((datos->categorias[6]==RELA_P))return(0);
														if((datos->categorias[6]==NOME_P))return(1);
													}
												}
											}
											if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)){
												if(datos->n_silabas_pausa_ant < 6.5)return(1);
												if(datos->n_silabas_pausa_ant > 6.5)return(0);
											}
										}
										if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==PRON_PROC_P))return(1);
									}
									if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==VERBO_P))return(0);
											if((datos->categorias[6]==NOME_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)){
												if(datos->n_silabas_pausa_ant < 8.5)return(1);
												if(datos->n_silabas_pausa_ant > 8.5)return(0);
											}
										}
										if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PRON_NUME_P)||(datos->categorias[1]==PRON_P)){
											if(datos->n_silabas_pausa_pos < 67.5){
												if((datos->categorias[0]==DET_P)){
													if((datos->categorias[2]==DET_P)||(datos->categorias[2]==PREP_P))return(0);
													if((datos->categorias[2]==NOME_P))return(1);
												}
												if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PECHE_PAREN_P)||(datos->categorias[0]==PREP_P))return(1);
											}
											if(datos->n_silabas_pausa_pos > 67.5){
												if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_POSE_P))return(0);
												if((datos->categorias[5]==VERBO_P))return(1);
											}
										}
									}
								}
								if((datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==PREP_P)){
									if(datos->n_silabas_pausa_pos < 74.5){
										if((datos->categorias[4]==PRON_PROC_P))return(0);
										if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==VERBO_P))return(1);
									}
									if(datos->n_silabas_pausa_pos > 74.5)return(0);
								}
							}
						}
						if(datos->n_silabas_pausa_ant > 9.5){
							if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==XERUNDIO_P)){
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==VERBO_P))return(0);
								if((datos->categorias[5]==PREP_P)){
									if((datos->categorias[0]==XERUNDIO_P))return(0);
									if((datos->categorias[0]==CONX_SUBOR_P))return(1);
								}
							}
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P)){
								if((datos->categorias[4]==ADVE_P)||(datos->categorias[4]==DET_NUME_P)||(datos->categorias[4]==DET_P)||(datos->categorias[4]==PRON_PROC_P)){
									if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==VERBO_P))return(0);
												if((datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)){
													if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==RELA_P)){
														if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)){
															if(datos->n_silabas_pausa_pos < 106)return(0);
															if(datos->n_silabas_pausa_pos > 106)return(1);
														}
														if((datos->categorias[2]==DET_P)){
															if((datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P))return(0);
															if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==VERBO_P)){
																if((datos->categorias[0]==PREP_P))return(0);
																if((datos->categorias[0]==DET_P))return(1);
															}
														}
													}
													if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==VERBO_P)){
														if((datos->categorias[2]==ADVE_P))return(0);
														if((datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P))return(1);
													}
												}
											}
											if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_SUBOR_P)){
												if((datos->categorias[0]==PRON_PROC_P))return(0);
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==VERBO_P))return(1);
											}
										}
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==PREP_P)){
											if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==DET_POSE_P)||(datos->categorias[5]==PREP_P))return(0);
											if((datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==VERBO_P))return(1);
										}
									}
									if((datos->categorias[5]==PRON_P)||(datos->categorias[5]==RELA_P)){
										if((datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==VERBO_P)){
											if((datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P))return(0);
											if((datos->categorias[2]==ADXE_P))return(1);
										}
										if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PRON_PROC_P))return(1);
									}
								}
								if((datos->categorias[4]==VERBO_P)){
									if((datos->categorias[3]==GUION_P))return(0);
									if((datos->categorias[3]==ADXE_P)||(datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==NOME_P)){
										if((datos->categorias[1]==CONX_SUBOR_P))return(0);
										if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
											if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==RELA_P)){
												if((datos->categorias[2]==NOME_P))return(0);
												if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==PREP_P))return(1);
											}
											if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P)){
												if((datos->categorias[0]==ADVE_P))return(0);
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P)){
													if((datos->categorias[2]==ADVE_P)){
														if((datos->categorias[0]==DET_P))return(0);
														if((datos->categorias[0]==CONX_COOR_P))return(1);
													}
													if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)){
														if(datos->n_silabas_pausa_ant < 10.5){
															if((datos->categorias[2]==PREP_P))return(0);
															if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==VERBO_P))return(1);
														}
														if(datos->n_silabas_pausa_ant > 10.5){
															if((datos->categorias[6]==DET_P)){
																if((datos->categorias[5]==DET_P))return(0);
																if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P))return(1);
															}
															if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_NUME_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P))return(1);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if((datos->categorias[4]==APERT_INTER_P)||(datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==CONX_SUBOR_P)||(datos->categorias[4]==RELA_P)||(datos->categorias[4]==XERUNDIO_P)){
			if((datos->categorias[3]==CONX_COOR_P)||(datos->categorias[3]==CONX_SUBOR_P)||(datos->categorias[3]==DET_P)||(datos->categorias[3]==GUION_P)||(datos->categorias[3]==PREP_P)||(datos->categorias[3]==PRON_NUME_P)||(datos->categorias[3]==PRON_P)||(datos->categorias[3]==RELA_P)||(datos->categorias[3]==VERBO_P)||(datos->categorias[3]==XERUNDIO_P)){
				if(datos->n_silabas_pausa_ant < 6.5){
					if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
						if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==PUNTO_P)||(datos->categorias[5]==VERBO_P)||(datos->categorias[5]==XERUNDIO_P)){
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==APERT_INTER_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==RELA_P)||(datos->categorias[2]==VERBO_P)||(datos->categorias[2]==XERUNDIO_P))return(0);
							if((datos->categorias[2]==CONX_COOR_P)){
								if(datos->n_silabas_pausa_ant < 5.5){
									if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==CONX_SUBOR_P))return(0);
									if((datos->categorias[4]==RELA_P)){
										if((datos->categorias[3]==DET_P)||(datos->categorias[3]==PREP_P))return(0);
										if((datos->categorias[3]==VERBO_P))return(1);
									}
								}
								if(datos->n_silabas_pausa_ant > 5.5)return(1);
							}
						}
						if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)){
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==ADXE_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==VERBO_P)||(datos->categorias[2]==XERUNDIO_P))return(0);
							if((datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)){
								if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P))return(0);
								if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)){
									if((datos->categorias[4]==CONX_SUBOR_P))return(0);
									if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==RELA_P))return(1);
								}
							}
						}
					}
					if((datos->categorias[1]==PRON_P))return(1);
				}
				if(datos->n_silabas_pausa_ant > 6.5){
					if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==VERBO_P)){
						if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==VERBO_P)){
							if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
								if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==GUION_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==VERBO_P))return(0);
								if((datos->categorias[5]==PREP_P)){
									if((datos->categorias[2]==NOME_P)||(datos->categorias[2]==VERBO_P))return(0);
									if((datos->categorias[2]==PREP_P))return(1);
								}
							}
							if((datos->categorias[6]==PRON_P))return(1);
						}
						if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==CONX_SUBOR_P)){
							if((datos->categorias[0]==DET_P))return(0);
							if((datos->categorias[0]==APERT_INTER_P)||(datos->categorias[0]==NOME_P))return(1);
						}
					}
					if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==RELA_P)){
						if((datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==GUION_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)){
							if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==VERBO_P))return(0);
							if((datos->categorias[0]==XERUNDIO_P))return(1);
						}
						if((datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==VERBO_P)){
							if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==GUION_P)||(datos->categorias[2]==NOME_P)){
								if((datos->categorias[4]==CONX_SUBOR_P))return(0);
								if((datos->categorias[4]==RELA_P))return(1);
							}
							if((datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==PRON_PROC_P)||(datos->categorias[2]==VERBO_P)){
								if((datos->categorias[6]==ADXE_P))return(0);
								if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P))return(1);
							}
						}
					}
				}
			}
			if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==ADXE_P)||(datos->categorias[3]==DET_NUME_P)||(datos->categorias[3]==NOME_P)){
				if(datos->n_silabas_pausa_ant < 11.5){
					if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==VERBO_P)){
						if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P)){
							if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==COMINNAS_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)){
								if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==VERBO_P)){
									if((datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==VERBO_P)){
										if((datos->categorias[3]==NOME_P)){
											if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==COMINNAS_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)||(datos->categorias[1]==XERUNDIO_P))return(0);
											if((datos->categorias[1]==PREP_P)){
												if((datos->categorias[4]==CONX_COOR_P))return(0);
												if((datos->categorias[4]==RELA_P))return(1);
											}
										}
										if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==ADXE_P)){
											if((datos->categorias[1]==DET_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==RELA_P)){
												if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==VERBO_P))return(0);
												if((datos->categorias[2]==DET_P))return(1);
											}
											if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==NOME_P))return(1);
										}
									}
									if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==PREP_P)){
										if((datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)){
											if(datos->n_silabas_pausa_pos < 15){
												if((datos->categorias[0]==NULA_P))return(0);
												if((datos->categorias[0]==NOME_P))return(1);
											}
											if(datos->n_silabas_pausa_pos > 15)return(0);
										}
										if((datos->categorias[5]==DET_P)||(datos->categorias[5]==VERBO_P)){
											if(datos->n_silabas_pausa_ant < 4.5)return(0);
											if(datos->n_silabas_pausa_ant > 4.5){
												if(datos->n_silabas_pausa_pos < 45){
													if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==PREP_P))return(0);
													if((datos->categorias[6]==ADVE_P))return(1);
												}
												if(datos->n_silabas_pausa_pos > 45)return(1);
											}
										}
									}
								}
								if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P)){
									if((datos->categorias[6]==DET_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
										if((datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P))return(0);
										if((datos->categorias[1]==NOME_P)){
											if(datos->n_silabas_pausa_pos < 23){
												if((datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==VERBO_P)){
													if((datos->categorias[2]==DET_P))return(0);
													if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==PREP_P)){
														if(datos->n_silabas_pausa_ant < 5)return(1);
														if(datos->n_silabas_pausa_ant > 5){
															if((datos->categorias[4]==CONX_COOR_P))return(0);
															if((datos->categorias[4]==RELA_P))return(1);
														}
													}
												}
												if((datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P))return(1);
											}
											if(datos->n_silabas_pausa_pos > 23){
												if((datos->categorias[4]==CONX_COOR_P))return(0);
												if((datos->categorias[4]==RELA_P)){
													if(datos->n_silabas_pausa_ant < 6)return(0);
													if(datos->n_silabas_pausa_ant > 6){
														if(datos->n_silabas_pausa_pos < 36)return(0);
														if(datos->n_silabas_pausa_pos > 36)return(1);
													}
												}
											}
										}
									}
									if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==ADXE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==CONX_SUBOR_P))return(1);
								}
							}
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P)){
								if(datos->n_silabas_pausa_pos < 17){
									if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==VERBO_P)){
										if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==NULA_P))return(0);
										if((datos->categorias[2]==CONX_COOR_P)){
											if((datos->categorias[1]==ADVE_P))return(0);
											if((datos->categorias[1]==NOME_P))return(1);
										}
									}
									if((datos->categorias[5]==PREP_P))return(1);
								}
								if(datos->n_silabas_pausa_pos > 17){
									if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==VERBO_P)){
										if(datos->n_silabas_pausa_pos < 30.5){
											if((datos->categorias[3]==ADXE_P))return(0);
											if((datos->categorias[3]==NOME_P))return(1);
										}
										if(datos->n_silabas_pausa_pos > 30.5){
											if((datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==VERBO_P)){
												if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P))return(0);
												if((datos->categorias[0]==DET_P)){
													if(datos->n_silabas_pausa_ant < 8.5)return(0);
													if(datos->n_silabas_pausa_ant > 8.5)return(1);
												}
											}
											if((datos->categorias[6]==ADVE_P))return(1);
										}
									}
									if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)){
										if(datos->n_silabas_pausa_ant < 2)return(0);
										if(datos->n_silabas_pausa_ant > 2)return(1);
									}
								}
							}
						}
						if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==PECHE_PAREN_P)||(datos->categorias[1]==PRON_P)){
							if((datos->categorias[4]==CONX_SUBOR_P))return(0);
							if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==RELA_P)){
								if(datos->n_silabas_pausa_pos < 21)return(0);
								if(datos->n_silabas_pausa_pos > 21)return(1);
							}
						}
					}
					if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==INTER_P)||(datos->categorias[5]==PRON_NUME_P)||(datos->categorias[5]==PRON_PROC_P)){
						if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==VERBO_P)){
							if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)){
								if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==RELA_P))return(0);
								if((datos->categorias[4]==CONX_SUBOR_P))return(1);
							}
							if((datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P))return(1);
						}
						if((datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==XERUNDIO_P)){
							if((datos->categorias[1]==DET_NUME_P)||(datos->categorias[1]==DET_P)){
								if((datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P)){
									if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==ADXE_P)||(datos->categorias[3]==NOME_P))return(0);
									if((datos->categorias[3]==DET_NUME_P))return(1);
								}
								if((datos->categorias[0]==NOME_P))return(1);
							}
							if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==NULA_P)||(datos->categorias[1]==PECHE_PAREN_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
								if((datos->categorias[6]==NOME_P)){
									if((datos->categorias[0]==NOME_P))return(0);
									if((datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==NULA_P)||(datos->categorias[0]==PREP_P))return(1);
								}
								if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==VERBO_P)||(datos->categorias[6]==XERUNDIO_P))return(1);
							}
						}
					}
				}
				if(datos->n_silabas_pausa_ant > 11.5){
					if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==PREP_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==RELA_P)||(datos->categorias[6]==XERUNDIO_P)){
						if((datos->categorias[6]==ADXE_P)||(datos->categorias[6]==COMINNAS_P)||(datos->categorias[6]==CONX_COOR_P)||(datos->categorias[6]==DET_POSE_P)||(datos->categorias[6]==GUION_P)||(datos->categorias[6]==PUNTO_P)||(datos->categorias[6]==XERUNDIO_P)){
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P))return(0);
							if((datos->categorias[0]==NOME_P)||(datos->categorias[0]==VERBO_P)){
								if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==VERBO_P))return(0);
								if((datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)){
									if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==DET_P)){
										if((datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==PREP_P))return(0);
										if((datos->categorias[2]==DET_P))return(1);
									}
									if((datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P))return(1);
								}
							}
						}
						if((datos->categorias[6]==PREP_P)||(datos->categorias[6]==RELA_P)){
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==RELA_P)||(datos->categorias[0]==VERBO_P)){
								if(datos->n_silabas_pausa_ant < 19.5){
									if((datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P))return(0);
									if((datos->categorias[2]==ADXE_P))return(1);
								}
								if(datos->n_silabas_pausa_ant > 19.5){
									if((datos->categorias[1]==GUION_P))return(0);
									if((datos->categorias[1]==ADXE_P)||(datos->categorias[1]==DET_P))return(1);
								}
							}
							if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==PREP_P)){
								if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==NOME_P)){
									if(datos->n_silabas_pausa_pos < 32.5){
										if((datos->categorias[2]==NOME_P))return(0);
										if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==DET_P))return(1);
									}
									if(datos->n_silabas_pausa_pos > 32.5)return(0);
								}
								if((datos->categorias[5]==DET_P)||(datos->categorias[5]==VERBO_P))return(1);
							}
						}
					}
					if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P)||(datos->categorias[6]==VERBO_P)){
						if((datos->categorias[2]==ADXE_P)||(datos->categorias[2]==CONX_SUBOR_P)||(datos->categorias[2]==PRON_P)||(datos->categorias[2]==XERUNDIO_P)){
							if((datos->categorias[1]==DET_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==VERBO_P)){
								if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==VERBO_P))return(0);
								if((datos->categorias[5]==DET_P)){
									if((datos->categorias[1]==PREP_P))return(0);
									if((datos->categorias[1]==DET_P))return(1);
								}
							}
							if((datos->categorias[1]==DET_POSE_P)||(datos->categorias[1]==NOME_P)){
								if((datos->categorias[2]==CONX_SUBOR_P))return(0);
								if((datos->categorias[2]==ADXE_P))return(1);
							}
						}
						if((datos->categorias[2]==ADVE_P)||(datos->categorias[2]==CONX_COOR_P)||(datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)||(datos->categorias[2]==VERBO_P)){
							if((datos->categorias[0]==ADVE_P)||(datos->categorias[0]==ADXE_P)||(datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_COOR_P)||(datos->categorias[0]==CONX_SUBOR_P)||(datos->categorias[0]==GUION_P)||(datos->categorias[0]==VERBO_P)){
								if((datos->categorias[1]==DET_P)||(datos->categorias[1]==VERBO_P)){
									if((datos->categorias[6]==ADVE_P)||(datos->categorias[6]==DET_P)||(datos->categorias[6]==NOME_P)||(datos->categorias[6]==VERBO_P)){
										if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==VERBO_P)){
											if((datos->categorias[2]==DET_NUME_P)||(datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P)||(datos->categorias[2]==PREP_P)){
												if((datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==PREP_P)||(datos->categorias[5]==PRON_PROC_P))return(0);
												if((datos->categorias[5]==ADXE_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==VERBO_P)){
													if((datos->categorias[3]==ADVE_P)||(datos->categorias[3]==NOME_P))return(0);
													if((datos->categorias[3]==ADXE_P)){
														if(datos->n_silabas_pausa_ant < 13.5)return(0);
														if(datos->n_silabas_pausa_ant > 13.5){
															if((datos->categorias[0]==COMINNAS_P)||(datos->categorias[0]==CONX_SUBOR_P)){
																if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==RELA_P))return(0);
																if((datos->categorias[4]==CONX_SUBOR_P))return(1);
															}
															if((datos->categorias[0]==ADXE_P)||(datos->categorias[0]==VERBO_P))return(1);
														}
													}
												}
											}
											if((datos->categorias[2]==DET_POSE_P)||(datos->categorias[2]==VERBO_P))return(1);
										}
										if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==NOME_P))return(1);
									}
									if((datos->categorias[6]==CONX_SUBOR_P)||(datos->categorias[6]==PRON_P)||(datos->categorias[6]==PRON_PROC_P))return(1);
								}
								if((datos->categorias[1]==ADVE_P)||(datos->categorias[1]==ADXE_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P))return(1);
							}
							if((datos->categorias[0]==DET_NUME_P)||(datos->categorias[0]==DET_P)||(datos->categorias[0]==DET_POSE_P)||(datos->categorias[0]==NOME_P)||(datos->categorias[0]==PREP_P)||(datos->categorias[0]==PRON_PROC_P)||(datos->categorias[0]==RELA_P)){
								if((datos->categorias[1]==ADXE_P)){
									if(datos->n_silabas_pausa_pos < 14.5)return(0);
									if(datos->n_silabas_pausa_pos > 14.5)return(1);
								}
								if((datos->categorias[1]==CONX_COOR_P)||(datos->categorias[1]==CONX_SUBOR_P)||(datos->categorias[1]==DET_P)||(datos->categorias[1]==NOME_P)||(datos->categorias[1]==PREP_P)||(datos->categorias[1]==PRON_NUME_P)||(datos->categorias[1]==PRON_P)||(datos->categorias[1]==PRON_PROC_P)||(datos->categorias[1]==RELA_P)||(datos->categorias[1]==VERBO_P)){
									if(datos->n_silabas_pausa_ant < 30.5){
										if((datos->categorias[5]==PREP_P)){
											if((datos->categorias[2]==PREP_P))return(0);
											if((datos->categorias[2]==DET_P)||(datos->categorias[2]==NOME_P))return(1);
										}
										if((datos->categorias[5]==ADVE_P)||(datos->categorias[5]==ADXE_P)||(datos->categorias[5]==COMINNAS_P)||(datos->categorias[5]==CONX_COOR_P)||(datos->categorias[5]==CONX_SUBOR_P)||(datos->categorias[5]==DET_NUME_P)||(datos->categorias[5]==DET_P)||(datos->categorias[5]==NOME_P)||(datos->categorias[5]==PRON_P)||(datos->categorias[5]==PRON_PROC_P)||(datos->categorias[5]==RELA_P)||(datos->categorias[5]==VERBO_P))return(1);
									}
									if(datos->n_silabas_pausa_ant > 30.5){
										if(datos->n_silabas_pausa_pos < 43.5)return(1);
										if(datos->n_silabas_pausa_pos > 43.5){
											if((datos->categorias[4]==CONX_COOR_P)||(datos->categorias[4]==RELA_P))return(0);
											if((datos->categorias[4]==CONX_SUBOR_P))return(1);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if((datos->categorias[3]==APERT_EXCLA_P)||(datos->categorias[3]==APERT_INTER_P)||(datos->categorias[3]==APERT_PAREN_P)||(datos->categorias[3]==PECHE_EXCLA_P)||(datos->categorias[3]==PECHE_INTER_P)||(datos->categorias[3]==PECHE_PAREN_P)||(datos->categorias[3]==PUNTO_P))return(1);
	
	if(datos->n_silabas_pausa_ant> 40) return(1);
	return(0);
}


