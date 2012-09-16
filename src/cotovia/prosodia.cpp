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
//#include "cotovia.hpp"
#include "errores.hpp"
#include "letras.hpp"
#include "fonemas.hpp"
#include "prosodia.hpp"

// Variables globais para leva-la conta de grupos acentuais, palabras, e silabas
// desde o principio. Debense resetear para cada frase.
short int orde_sil;
short int orde_gac;
short int orde_pal;


/*............................................................................
  .                  AS DISTINTAS FUNCIONS DESTE MODULO                      .
  ............................................................................*/
/*****************************************************************************
 *  int alof_vocal_forte(char *c): Recibe como parametro un caracter decindo *
 * logo se se trata dunha vocal forte, devolvendo en tal caso un 1 e un 0 se *
 * non e asi.                                                                *
 *****************************************************************************/
int alof_vocal_forte(char *c)
{
	switch(*c)
	{ case 'a':
		case 'e':
		case 'E':
		case 'o':
		case 'O': return 1;
	};

	return 0;
}


/*****************************************************************************
 *****************************************************************************/
int alof_prosodia_vocalico(char *c)
{
	switch (*c)
	{
		case'a': return 1;
		case'e':return 1;
		case'E':return 1;
		case'i':return 1;
		case'o':return 1;
		case'O':return 1;
		case'u':return 1;
		default:return 0;
	}

}

/*****************************************************************************
 *****************************************************************************/
int semivocal(t_prosodia * alofo)
{
	if ( ( !alofo->tonica)
			&&
			( alofo->alof[0]=='i'  ||  alofo->alof[0]=='u' )
			&&
			( alof_prosodia_vocalico( (alofo-1)->alof ) || alof_prosodia_vocalico( (alofo+1)->alof ) )
	   )

		return 1;


	return 0;
}

/*****************************************************************************
 *****************************************************************************/
int consoante_liquida_ou_nasal(char *c)
{

	switch(*c)
	{
		case 'n':return 1;
		case 'N':return 1;
		case 'm':return 1;
		case 'l':return 1;
		case 'Z':return 1;
		default: return 0;
	};
}

/*****************************************************************************
 *****************************************************************************/
int consoante_xorda(char* c)
{
	if (strcmp(c,"tS")==0) return 1;

	switch(*c)
	{
		case 'p':
			break;
		case 't':
			break;
		case 'k':
			break;
		case 'T':
			break;
		case 'x':
			break;
		case 'f':
			break;
		case 's':
		case 'S':
			break;
		default: return 0;
	}
	return 1;
}

/*****************************************************************************
 *****************************************************************************/
int consoante_sonora(char *c)
{
	if (consoante_xorda(c) || alof_prosodia_vocalico(c) || *c=='#' ) return 0;

	return 1;
}

/*****************************************************************************
 *****************************************************************************/
void adap_fon(char * ent,char *saida)
{
	if (strcmp(ent,"x")==0)
	{ 
		strcpy(saida,"h");
		return;
	}	

	if (strcmp(ent,"Z")==0)
	{
		strcpy(saida,"jj");
		return;
	}

	strcpy (saida,ent);
	return;
}


/*****************************************************************************
 *****************************************************************************/
int Prosodia::encher_arquivo_de_prosodia(t_prosodia *f_prosodica){
	//Obsolete and removed (erbanga)
	return OK;
}


/*****************************************************************************
 *****************************************************************************/
int obter_indice(char *alofo)
	/* Se non se atopara ningunha traducci??on posible enton asignanse
	   por defecto as caracteristicas do 'p' */
	/*devólvense tamén os datos das semivocais*/
{
	if (strcmp(alofo,"tS")==0) return 15;
	if (strcmp(alofo,"rr")==0) return 17;

	switch (*alofo) {
		case 'i':
			return 24;
		case'u':
        	return 25;
		case'p':
        	return 0;
		case'b':
        	return 1;
		case'B':
        	return 2;
		case't':
        	return 3;
		case'd':
        	return 4;
		case'D':
        	return 5;
		case'k':
        	return 6;
		case'g':
        	return 7;
		case'G':
        	return 8;
		case'f':
        	return 10;
		case'T':
        	return 11;
		case's':
        	return 12;
		case'x':
        	return 14;
		case'r':
        	return 16;
		case'l':
        	return 18;
		case 'Z':
        	return 19;
		case'm':
        	return 21;
		case'n':
        	return 22;
		case'N':
			/* o ene velar pasa a ter as mesmas caracteristicas c??o ene normal*/
            return 22;
		case'J':
        	return 23;
		case'S':
        	return 24;
		default:
        	return 0;
	}

}



/*****************************************************************************
 *****************************************************************************/
int vocal_inicial_de_grupo_entonativo(t_prosodia * f_prosodica,int i)
{
	if ( alof_prosodia_vocalico( (f_prosodica+i)->alof )	)
	{
		i--; /* retrocedemos ó anterior elemento*/
		while ( ( consoante_sonora((f_prosodica+i)->alof )	 || consoante_xorda((f_prosodica+i)->alof )	  )
				&& i>0) 
		{
			i--;
		}

		if ( (f_prosodica+i)->alof[0]=='#'  )
			return 1;
		else
			return 0;

	}
	else return 0;

}



/*****************************************************************************
 *****************************************************************************/
int onset_nucleo_coda(t_prosodia *f_prosodica)
{
	int i=0;
	int pos_silabica_inicial=f_prosodica->pos_silabica;

	if (alof_prosodia_vocalico(f_prosodica->alof)) return 2;

	do
	{
		if (alof_prosodia_vocalico((f_prosodica+i)->alof)) return 1;

		i++;
	}
	while ( (f_prosodica+i)->alof[0] &&  (f_prosodica+i)->pos_silabica==pos_silabica_inicial  );

	return 3;
}


/*****************************************************************************/
unsigned char adapta_ti_prop_para_duracions(unsigned char in){
	/*Tipo fin de grupo fónico&
	  8     & g.f. final imperativas & -7.20\\
	  7     & g.f. final exclamativas & -3.83\\
	  6     & g.f. final interr. parciais -8.31& \\
	  5      & g.f. final interr. totais & -9.65\\
	  4    & g.f. final suspensivo & 8.36\\
	  3    & g.f. intermedio enumerativo & -7.04 \\
	  2     & g.f. intermedio enunciativas & 10.55\\
	  1     & g.f. final enunciativo & -10.72 \\
	  HAI QUE ARREGLAR A DETECCIÓN DE ENUMERATIVAS QUE ACABAN EN SUSPENSION
	  */

	switch (in){
		case  223: case 225 :
			return 1;
		case  222 : case 224:
			return 2;
		case  220 :
			return 3;
		case 221 : /*SUSPENSIVO*/
			return 4;
		case 202 : case 201: case 203:
			return 5;
		case 226: case 227  : case 228:
			return 6;
		case 204: case 205 : case 206:/*DE MOMENTO NON É POSIBLE DISTINGUIR IMPERATIVAS DE EXCLAMATIVAS, POR ISO O 8 NON SE EMPREGA DE MOMENTO*/
			return 7;
		default :     /*POR DEFECTO TOMAMOS O TONO FINAL DAS ENUNCIATIVAS*/
			return 1;
	}
}


/**
 * \author 
 * \remark 
 *
 * \param f_prosodica  
 *
 * \return 
 */
void preparar_datos_adicionais(t_prosodia *f_prosodica){
	int i=0;
	int psil_ant;
	int psil_post;
	int fin;
	unsigned char coda,lonx_sila,cont_aux;
	int sil_tonica;


	while ((f_prosodica+i)->alof[0]!=0 && i < LONX_MAX_FRASE_SIL_E_ACENTUADA){
		while ( (f_prosodica+i)->alof[0]!=0 && i < LONX_MAX_FRASE_SIL_E_ACENTUADA  
				&&  (f_prosodica+i)->ps==0 ){
			i++;
		}
		if ((f_prosodica+i)->alof[0]==0){ 
			break;
		}

		psil_ant=(f_prosodica+i)->ps;
		sil_tonica=0;
		while ((f_prosodica+i)->ps==psil_ant && (f_prosodica+i)->alof[0]!=0 
				&& i < LONX_MAX_FRASE_SIL_E_ACENTUADA){
			psil_ant=(f_prosodica+i)->ps;
			if ( (f_prosodica+i)->tonica==1){ 
				sil_tonica=1;
			}
			i++;
		}

		i--;/*situamonos no final da sílaba e asignamos hacia atras a estructura e contamos a sílaba.*/

		fin=i;
		psil_post= (f_prosodica+i)->ps;
		coda=3;
		lonx_sila=0;
		while ((f_prosodica+i)->ps==psil_post && (f_prosodica+i)->alof[0]!=0 
				&& i < LONX_MAX_FRASE_SIL_E_ACENTUADA )	{
			/*contamos número de alófonos*/
			lonx_sila++;

			/*asignamos estructura*/
			if (alof_prosodia_vocalico((f_prosodica+i)->alof)){
				(f_prosodica+i)->estr_sil=2;
				coda=1;
			}
			else {
				(f_prosodica+i)->estr_sil=coda;
			}
			/*Asignamos a todos os alófonos da sílaba tónica o valor en ->tonica=1*/
			if (sil_tonica) {
				(f_prosodica+i)->tonica=1;
			}

			psil_post=(f_prosodica+i)->ps;
			i--;
		}

		i++;

		cont_aux=0;
		while ((f_prosodica+i)<=(f_prosodica+fin) &&  (f_prosodica+i)->alof[0]!=0 
				&& i < LONX_MAX_FRASE_SIL_E_ACENTUADA){
			(f_prosodica+i)->lonx_sil=lonx_sila;

			/*miramos se é a sílaba final posegmento=3, posilabica=1*/
			if ((f_prosodica+i)->pos_silabica==1 
					&& (f_prosodica+i)->pos_segmento==3){
				(f_prosodica+i)->sil_fin=(unsigned char)(lonx_sila-cont_aux);
				cont_aux++;
			}
			else {
				(f_prosodica+i)->sil_fin=0;
			}
			i++;
		}
	}
}



/*****************************************************************************
 *****************************************************************************/
unsigned char Prosodia::adaptar_grupo_prosodico(unsigned char codigo)
{

    switch (codigo) {

        case INICIO_INTERROGACION_PARCIAL:
        case INTERROGACION_PARCIAL:
        case FIN_INTERROGACION_PARCIAL: return 9;
                                        /*interrogativas directas con tonema final descendente*/

        case INTERROGACION_TOTAL:
        case INICIO_INTERROGACION_TOTAL:return 8;
                                        /*tipo de apelativa pronominal*/

        case FIN_INTERROGACION_TOTAL:return 9;
                                     /*supoñemos que o último tramo dunha indirecta cando é optativa, tamén é descendente ó igual cas directas*/

        case EXCLAMACION:
        case INICIO_EXCLAMACION:
        case FIN_EXCLAMACION: return 	2;
                              /* nas exclamativas por defecto poñeremos un final ascendente*/
        case CORCHETE:
        case INICIO_CORCHETE:
        case FIN_CORCHETE:
        case PARENTESE:
        case INICIO_PARENTESE:
        case FIN_PARENTESE:
        case ENTRE_GUIONS:
        case INICIO_ENTRE_GUIONS:
        case FIN_ENTRE_GUIONS: return 6;
                               /* estilo das parentéticas*/
        case ENUMERATIVO: return 7;
                          /* xustaposición*/
        case FIN_ENUMERATIVO: return 2;
                              /*GRUPO CONTINUATIVO, CON TONEMA ASCENDENTE*/
        case ENTRE_COMINNAS:
        case INICIO_ENTRE_COMINNAS:
        case FIN_ENTRE_COMINNAS:return 1;
                                /* tonema descendente, igual ó das enunciativas*/
        case INICIO_ENUNCIATIVA  :
                                /* as de inicio de enunciativa deberan ter un trozo final ascendente ou descente.
                                   Depende se é unha bipolar e acaba o primeiro trozo en ascendente, ou se ten varios elementos acabará
                                   en descendete, ou suspension.
                                   */
        case ENUNCIATIVA:
        case FIN_ENUNCIATIVA: return 1;
                              /*tonema final descendente*/
        case INCISO:return 4;
                    /* o mesmo co grupo incidental*/
        case DE_SUSPENSE: return 3;
                          /*leva asignado o do grupo  contrastivo*/
        default: return 1;
    }

}


/**
 * \author
 * \remark FRAN_CAMPILLO: La declaré void
 *
 * \param f_prosodica
 *
 * \return
 */
void posprocesar_prosodia(t_prosodia *f_prosodica){

	int i=1;
	/*empezamos no un e non no cero para non adaptar nada na pausa inicial
	  que se lle inserta a cada frase ó principio*/
	while ((f_prosodica+i)->alof[0] && i<LONX_MAX_FRASE_SIL_E_ACENTUADA){
		//(f_prosodica+i)->ti_prop=adaptar_grupo_prosodico((f_prosodica+i)->ti_prop);
		/*Non modificamos o valor de grupo prosódico e queda co valor dos defines. Faise esta traducción xusto antes */
		if ((f_prosodica+i)->pos_tonica>3){
			(f_prosodica+i)->pos_tonica=3;
		}
		if ((f_prosodica+i)->pos_silabica>5){
			(f_prosodica+i)->pos_silabica=5;
		}
		i++;
	}
}

/*****************************************************************************
 *****************************************************************************/
void asignar_posicion_grupo_acentual_dentro_da_proposicion(t_prosodia *f_prosodica)
	/* o primeiro grupo acentual da proposicion vai marcado con 1, os intermedios
	   con 2, e o grupo acentual final cun 3.*/
{
	int i=0,cont_parcial;
	int num_grupo_prosodico=1;
	int final_grupo_prosodico,comenzo_segmento_final,fin_segmento_inicial;

	do
	{
		final_grupo_prosodico=i;
		while( (f_prosodica+final_grupo_prosodico)->pos_grupo_prosodico==num_grupo_prosodico &&
				(f_prosodica+final_grupo_prosodico+1)->alof[0])
			final_grupo_prosodico++;
		/*busc??amo-lo fin do grupo_prosodico.Apunta ó ultimo elemento,ainda que
		  sexa unha pausa.*/

		comenzo_segmento_final=final_grupo_prosodico;
		while ( (f_prosodica+comenzo_segmento_final)->pos_silabica<= (f_prosodica+comenzo_segmento_final-1)->pos_silabica )
			comenzo_segmento_final--;
		/*con esto buscamos comenzo do ultimo grupo acentual da proposicion*/

		fin_segmento_inicial=i;
		while ( (f_prosodica+fin_segmento_inicial)->alof[0]=='#')	fin_segmento_inicial++;
		/*recorremos as pausas iniciais que pode haber nun grupo prosodico, como por
		  exemplo cando este é o inicio do texto.*/
		while ( (f_prosodica+fin_segmento_inicial)->pos_silabica>(f_prosodica+fin_segmento_inicial+1)->pos_silabica &&
				(f_prosodica+fin_segmento_inicial+1)->pos_silabica!=0                                                  )
			fin_segmento_inicial++;
		/*con isto busc??amolo final do primeiro grupo acentual*/

		cont_parcial=i;

		/*asignamos segmento inicial*/
		while ( (f_prosodica+cont_parcial)<=(f_prosodica+fin_segmento_inicial) )
		{
			if (  (f_prosodica+cont_parcial)->alof[0]!='#'  )
				(f_prosodica+cont_parcial)->pos_segmento=1;
			cont_parcial++;
		}

		/*asignamos segmento medio*/
		while ( (f_prosodica+cont_parcial)<(f_prosodica+comenzo_segmento_final) )
		{
			if (  (f_prosodica+cont_parcial)->alof[0]!='#'  )
				(f_prosodica+cont_parcial)->pos_segmento=2;
			cont_parcial++;
		}

		/*asignamos segmento final*/
		while ( (f_prosodica+cont_parcial)<=(f_prosodica+final_grupo_prosodico) )
		{
			if (  (f_prosodica+cont_parcial)->alof[0]!='#'  )
				(f_prosodica+cont_parcial)->pos_segmento=3;
			cont_parcial++;
		}
		/* Coas anteriores asignacions, en caso de que o grupo acentual sexa tanto final,
		   medio e inicial, asignase como final sempre.*/


		i=final_grupo_prosodico +1;
		num_grupo_prosodico++;
	}
	while ( (f_prosodica+i)->alof[0]!=0 && i<LONX_MAX_FRASE_SIL_E_ACENTUADA);

	return;
}

/**
 * \author 
 * \remark 
 *
 * \param f_fonetica  
 *
 * \return 
 */
int empeza_grupo_prosodico_a_continuacion(char *f_fonetica){
	char inicio_comentario[6];

	if (*(f_fonetica+1)=='#') 
		return 1;

	strncpy(inicio_comentario,(f_fonetica+1),5);
	inicio_comentario[5]=0;

	if (strcmp(inicio_comentario,"%prop")==0)	
		return 1;

	return 0;
}

/**
 * \author 
 * \remark 
 *
 * \param f_prosodica  
 * \param f_fonetica  
 * \param inicio_grupo_acentual  
 * \param fin_grupo_acentual  
 * \param tipo_grupo_acentual  
 *
 * \return 
 */
void asignar_tipo_de_segmento_acentual(t_prosodia *f_prosodica, char *f_fonetica,
		int inicio_grupo_acentual,int fin_grupo_acentual,
		unsigned char *tipo_grupo_acentual){
	
	int i;

	if ( empeza_grupo_prosodico_a_continuacion(f_fonetica)){
		*tipo_grupo_acentual=3;
	}

	for (i=inicio_grupo_acentual;i<=fin_grupo_acentual;i++){
		(f_prosodica+i)->pos_segmento=*tipo_grupo_acentual;
	}

	if (*tipo_grupo_acentual<2){
	   	((*tipo_grupo_acentual)++);
	}
	if (*tipo_grupo_acentual==3) {
		*tipo_grupo_acentual=1;
	}
}

/**
 * \author 
 * \remark 
 *
 * \param f_prosodica  
 * \param inicio_grupo_acentual  
 * \param fin_grupo_acentual  
 * \param pos_acento_grupo_acentual  
 *
 * \return 
 */
void asignar_orde_silabica_inversa(t_prosodia *f_prosodica,
		int inicio_grupo_acentual,int fin_grupo_acentual,
		int pos_acento_grupo_acentual){
	
	unsigned char minuendo=(unsigned char)((f_prosodica+fin_grupo_acentual)->pos_silabica + 1);
	unsigned char orde_silabica_real_do_acento=(unsigned char)(minuendo-(f_prosodica+pos_acento_grupo_acentual)->pos_silabica);
	int i;

	for(i=inicio_grupo_acentual;i<=fin_grupo_acentual;i++){
		(f_prosodica+i)->pos_silabica=(unsigned char)(minuendo-(f_prosodica+i)->pos_silabica);
		/*asignamos a orde silabica dese alofono dentro do grupo acentual empezando
		  polo final do grupo fonico.*/
		(f_prosodica+i)->pos_tonica=orde_silabica_real_do_acento;
	}
}

/*****************************************************************************
 *****************************************************************************/
char *interpretar_espacio(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,int *inicio_grupo_acentual,int *fin_grupo_acentual,int *pos_acento_grupo_acentual,char *palabra_tonica,unsigned char * orde_silabica,
		unsigned char *tipo_grupo_acentual)
{

	if (*palabra_tonica) {
		// Se a palabra_anterior foi tonica e se detecta un espacio (fin de palabra)
		// enton asignamos adecuadamente a orde silabica dentro deste grupo silabico.
		*fin_grupo_acentual=*pos_f_prosodica -1;
		asignar_orde_silabica_inversa(f_prosodica,*inicio_grupo_acentual,*fin_grupo_acentual,*pos_acento_grupo_acentual);
		asignar_tipo_de_segmento_acentual(f_prosodica,f_fonetica,*inicio_grupo_acentual,*fin_grupo_acentual,tipo_grupo_acentual);
		*inicio_grupo_acentual=*fin_grupo_acentual=*pos_acento_grupo_acentual=0;
		*orde_silabica=0;
		*palabra_tonica=0;
		orde_gac++;                      // Incrementamos a orde do grupo acentual
	}
	if (*orde_silabica==0 && letra((unsigned char)*(f_fonetica+1)))
		*inicio_grupo_acentual=*pos_f_prosodica;
	if (letra((unsigned char)*(f_fonetica+1))) {
		(*orde_silabica)++;
		orde_sil++;
		orde_pal++;                      // Este e o uníco sitio onde imos contando a orde das palabras
	}
	f_fonetica++;
	return f_fonetica;
}

/**
 * \author 
 * \remark 
 *
 * \param f_fonetica  
 * \param alofono  
 *
 * \return 
 */
void obter_alofono(char *f_fonetica,char *alofono){
	
	switch (*f_fonetica){
		case 't':
#ifdef _MODOA_EU
			if (!strncmp(f_fonetica, "ts`",3)){
				strcpy(alofono,"ts`");
				break;
			}
#endif
			if (!strncmp(f_fonetica, "tS",2)){
				strcpy(alofono,"tS");
			}
#ifdef _MODOA_EU
			else if (!strncmp(f_fonetica, "ts",2)){
				strcpy(alofono,"ts");
			}
#endif
			else {
				strcpy(alofono,"t");
			}
			break;
#ifdef _MODOA_EU
		case 's': 
			if (*(f_fonetica+1)=='`') { 
				strcpy(alofono,"s`");
			}
			else {
				strcpy(alofono,"s");
			}
			break;
		case 'g': 
			if (*(f_fonetica+1)=='j') { 
				strcpy(alofono,"gj");
			}
			else {
				strcpy(alofono,"g");
			}
			break;
		case 'j': 
			if (*(f_fonetica+1)=='j') { 
				strcpy(alofono,"jj");
			}
			else {
				strcpy(alofono,"j");
			}
			break;
#endif
		case 'r': 
			if (*(f_fonetica+1)=='r') { 
				strcpy(alofono,"rr");
			}
			else {
				strcpy(alofono,"r");
			}
			break;
		default:
			alofono[0]=*f_fonetica;
			alofono[1]=0;
	}
}

/*****************************************************************************
 *****************************************************************************/
//char *interpretar_alofonos(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,unsigned char *orde_silabica,unsigned char tipo_proposicion,char *palabra_tonica,int *inicio_grupo_acentual,int* fin_grupo_acentual,int *pos_grupo_acentual,unsigned char pos_grupo_prosodi)
char *interpretar_alofonos(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,unsigned char *orde_silabica,unsigned char tipo_proposicion,char *palabra_tonica,int *pos_grupo_acentual,unsigned char pos_grupo_prosodi)
{
	char alofono[OCTETOS_POR_FONEMA]="";
	char silaba_tonica=0;

	obter_alofono(f_fonetica,alofono);
	if ( (*(f_fonetica+strlen(alofono)))==(char)'^')
	{
		*palabra_tonica=1;
		silaba_tonica=1;
	}
	// Enchemos agora f_prosodica
	strcpy((f_prosodica+*pos_f_prosodica)->alof,alofono);
	if (silaba_tonica) {
		(f_prosodica+*pos_f_prosodica)->tonica=1;
		(*pos_grupo_acentual)=*pos_f_prosodica;
	}
	(f_prosodica+*pos_f_prosodica)->ti_prop=tipo_proposicion;
	(f_prosodica+*pos_f_prosodica)->pos_grupo_prosodico=pos_grupo_prosodi;
	(f_prosodica+*pos_f_prosodica)->pos_silabica=*orde_silabica;
	// Agora metemos o orden do grupo_acentual,da palabra e da silaba dentro da frase. Son as fronteiras
	(f_prosodica+*pos_f_prosodica)->pga=orde_gac;
	(f_prosodica+*pos_f_prosodica)->pp=orde_pal;
	(f_prosodica+*pos_f_prosodica)->ps=orde_sil;
	(*pos_f_prosodica)++;
	// Fin de encher frase_prosodica
	f_fonetica=f_fonetica + strlen(alofono);
	if (silaba_tonica) f_fonetica++;
	return f_fonetica;
}

/**
 * \author 
 * \remark 
 *
 * \param f_fonetica  
 * \param orde_silabica  
 *
 * \return 
 */
char *interpretar_guion(char *f_fonetica,unsigned char *orde_silabica){
	
	(*orde_silabica)++;
	orde_sil++;
	f_fonetica++;
	return f_fonetica;
}

/*****************************************************************************
 *****************************************************************************/
char * interpretar_pausa(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,unsigned char num_grupo_prosodico,unsigned char tipo_proposicion)
{
	// FRAN_CAMPILLO : Tuve que modificar esta función debido a un error anterior en la transcripción anterior
	// que se produce en ciertos casos en los que al final de la fase hay dos pausas y ésta termina de la forma ....
	// o ".

	f_fonetica++;

//	if (strncmp(f_fonetica, "%rupt", 5) == 0) { // Se trata de una ruptura entonativa, no de una pausa.
//    	strcpy((f_prosodica+*pos_f_prosodica)->alof, "#R#");
//        (f_prosodica + *pos_f_prosodica)->pos_grupo_prosodico = num_grupo_prosodico;
//        (f_prosodica + *pos_f_prosodica)->ti_prop = tipo_proposicion;
//        (*pos_f_prosodica)++;
//    }
//    else {
	if (strncmp(f_fonetica, "%rupt", 5)) { // No es una ruptura prosodica
        if (*f_fonetica) {
            f_fonetica=f_fonetica+strlen("%pausa ");
            strcpy((f_prosodica+*pos_f_prosodica)->alof,"#");
            (f_prosodica+*pos_f_prosodica)->pos_grupo_prosodico=num_grupo_prosodico;
            (f_prosodica+*pos_f_prosodica)->ti_prop=tipo_proposicion;
            sscanf(f_fonetica,"%d",&(f_prosodica+*pos_f_prosodica)->dur1);
            (*pos_f_prosodica)++;
        }
        else {
            //  strcpy((f_prosodica+*pos_f_prosodica)->alof,"#");
            (f_prosodica+*pos_f_prosodica)->pos_grupo_prosodico=num_grupo_prosodico;
            (f_prosodica+*pos_f_prosodica)->ti_prop=tipo_proposicion;
            (f_prosodica+*pos_f_prosodica)->dur1 = 0;
            //(*pos_f_prosodica)++;
            return f_fonetica;
        }
    } // else
    
	/* Código anterior.
	   f_fonetica=f_fonetica+strlen("%pausa ")+1;
	   strcpy((f_prosodica+*pos_f_prosodica)->alof,"#");
	   (f_prosodica+*pos_f_prosodica)->pos_grupo_prosodico=num_grupo_prosodico;
	   (f_prosodica+*pos_f_prosodica)->ti_prop=tipo_proposicion;
	   sscanf(f_fonetica,"%d",&(f_prosodica+*pos_f_prosodica)->dur1);
	   (*pos_f_prosodica)++;
	   */ // Fin de código anterior.

	while (*f_fonetica!='#') f_fonetica++;
	f_fonetica++;
	/*adiantamos ata o seguinte caracter despois do # final  */

	return f_fonetica;
}

/*****************************************************************************
 *****************************************************************************/
//char * interpretar_comentario(char *f_fonetica,t_prosodia *f_prosodica,unsigned char *tipo_prop,unsigned char *num_grupos_proso)
char * interpretar_comentario(char *f_fonetica,unsigned char *tipo_prop,unsigned char *num_grupos_proso)
{
	char *pos_final_comentario;
	char campo[10]="";
	int codigo=0;
	char comentario [200];

	f_fonetica++;
	pos_final_comentario=strchr(f_fonetica,'%');
	strncpy(comentario,f_fonetica,pos_final_comentario-f_fonetica);
	comentario[pos_final_comentario-f_fonetica]=0;
	sscanf(comentario,"%s %i",campo,&codigo);
	/*lemos o string campo,  e o codigo */

	if (strcmp(campo,"prop")==0)
	{
		*tipo_prop=(unsigned char)codigo;
		(*num_grupos_proso)++;
	}


	return (pos_final_comentario+1);
}

/*****************************************************************************
 *****************************************************************************/
void encher_frase_prosodica(char *inicio_f_fonetica,t_prosodia *f_prosodica)
{
	unsigned char ord_sil=0;
	unsigned char num_grupos_prosodicos=0;
	unsigned char tipo_de_segmento_acentual=1; // dentro do grupo prosodico
	/*
	   1 inicial
	   2 intermedio
	   3 final
	   */
	int pos_f_prosodica=0;
	unsigned char tipo_proposicion=0;
	char palabra_tonica=0;
	int inicio_grupo_acentual=0,fin_grupo_acentual=0,pos_acento_grupo_acentual;
	char *f_fonetica=inicio_f_fonetica;

	// Reseteamo-las variables globais contadoras declaradas o principio deste arquivo
	orde_sil=0;
	orde_gac=1;
	orde_pal=0;
	while (*f_fonetica) {
		switch(*f_fonetica) {
			//case '%':f_fonetica=interpretar_comentario(f_fonetica,f_prosodica,&tipo_proposicion,&num_grupos_prosodicos);
			case '%':f_fonetica=interpretar_comentario(f_fonetica,&tipo_proposicion,&num_grupos_prosodicos);
					 break;
			case '#':f_fonetica=interpretar_pausa(f_fonetica,f_prosodica,&pos_f_prosodica,num_grupos_prosodicos,tipo_proposicion);
					 break;
			case '-':f_fonetica=interpretar_guion(f_fonetica,&ord_sil);
					 break;
			case ' ':f_fonetica=interpretar_espacio(f_fonetica,f_prosodica,&pos_f_prosodica,&inicio_grupo_acentual,&fin_grupo_acentual,&pos_acento_grupo_acentual,&palabra_tonica,&ord_sil,&tipo_de_segmento_acentual);
					 break;
			default: f_fonetica=interpretar_alofonos(f_fonetica,f_prosodica,&pos_f_prosodica,&ord_sil,tipo_proposicion,&palabra_tonica,&pos_acento_grupo_acentual,num_grupos_prosodicos);
		}
	}
	*(f_prosodica+pos_f_prosodica)->alof=0;
	return;
}

/**
 * \author 
 * \remark 
 *
 * \param f_fonetica  
 * \param f_prosodica  
 *
 * \return 
 */
void Prosodia::xerar_prosodia(char *f_fonetica,t_prosodia *f_prosodica){

	if (*f_fonetica==0) {
		return;
	}

	encher_frase_prosodica(f_fonetica,f_prosodica);
	posprocesar_prosodia(f_prosodica);
}


/**
 * \author 
 * \remark 
 *
 * \param opciones  
 *
 * \return 
 */
void Prosodia::inicializa(t_opciones * opciones){

	bddur = opciones->bddur;
	fsalida = opciones->fsalida;

	opciones->fstdout ? fd = stdout: fd = NULL;
}

void Prosodia::finaliza(){
	
	if (fd != NULL && fd != stdout) {
		fclose(fd);
        fd = NULL;
	}
}


