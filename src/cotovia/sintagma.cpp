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
	O ficheiro contén os datos e funcións necesarias para face-la analise sintáctica.
	Tén dúas vertientes. Unha é a de agrupar en sintagmas como primeiro paso.
	O segundo paso é agrupar os sintagmas en grupos ou proposicións.
	Os grupos e as proposicións caracterizanse por ir entre pausas normalmente
	e por ter asignado un determinado tipo de entonación.
*/
/**************************************************************************/
#include <stdio.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "xen_num.hpp"
#include "pausas.hpp"
//#include "variables_globales.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp" //Después que diccionarios.h...
#include "sintagma.hpp"

/** \fn   int relativo_interrogativo_ou_exclamativo(t_frase_separada *recorredor)

	Esta función empregase para decidir se o relativo é tonico ou átono.
	É tonico se se detecta ca función de interrogativo ou exclamativo.
	Noutro caso é atono.
*/

int relativo_interrogativo_ou_exclamativo(t_frase_separada *recorredor)

{
 while (recorredor->pal[0])
 {
	if (recorredor->cat_gramatical[0]==APERTURA_INTERROGACION ||
			recorredor->cat_gramatical[0]==PECHE_INTERROGACION    ||
			recorredor->cat_gramatical[0]==APERTURA_EXCLAMACION   ||
			recorredor->cat_gramatical[0]==PECHE_EXCLAMACION        )
			return 1;
	recorredor++;
 }
 return 0;
}
/**************************************************************************/
/*int comprobar_locucion_adversativa(t_frase_separada *frase_separada)
 {
	int cont_aux;
	if (
			 (frase_separada->cat_gramatical[0]==PARTICIPIO ||
				frase_separada->cat_gramatical[0]==XERUNDIO   	 )
												 &&
				strcmp(frase_separada->pal,"que")==0                 )
		RECONOCENSE ESTRUCTURAS COMO "sacado que" que son adversativas
	 {
			for(cont_aux=0;cont_aux<2;cont_aux++)
			{
			 (frase_separada+cont_aux)->cat_gramatical[0]=LOC_CONX_COOR;
			 (frase_separada+cont_aux)->cat_gramatical[1]=0;
			}
			return 1;

	 }
 return 0;
 }  */
/**************************************************************************/
/*int locucion_prepositiva_mais_de(t_conxuncion *conxuncion_encontrada,t_frase_separada *frase_separada)
/* Devolve un 1 se termina a locucion na palabra a,á,ó,ás,ós.
{

 if ((frase_separada+(conxuncion_encontrada->num_pal_conxuncion))->pal[0]=='d'
											 &&
   ((frase_separada+conxuncion_encontrada->num_pal_conxuncion)->cat_gramatical[0]==CONTR_PREP_ART_DET ||
	  (frase_separada+conxuncion_encontrada->num_pal_conxuncion)->cat_gramatical[0]==CONTR_PREP_ART_INDET ||
	  (frase_separada+conxuncion_encontrada->num_pal_conxuncion)->cat_gramatical[0]==CONTR_PREP_PRON_PERS_TON ||
	  (frase_separada+conxuncion_encontrada->num_pal_conxuncion)->cat_gramatical[0]==CONTR_PREP_DEMO  ||
	  (frase_separada+conxuncion_encontrada->num_pal_conxuncion)->cat_gramatical[0]==PREP  )																																																												 )
      return 1;
 return 0;
}


/**************************************************************************/
/**************************************************************************/
/***** Funcions para desambigua-las categorias gramaticais ****************/
/**************************************************************************/
int esta_palabra_tivera_a_categoria_de(unsigned char categoria,t_frase_separada *pal_frase_separada){

 unsigned char *categoria_actual=pal_frase_separada->cat_gramatical_descartada;

 while (*categoria_actual) {
  if (*categoria_actual==categoria) return 1;
  categoria_actual++;
 }
 return 0;
}


//int esta_palabra_ten_a_categoria_de(unsigned char categoria,t_frase_separada *pal_frase_separada)
//{
// 
// unsigned char *categoria_actual=pal_frase_separada->cat_gramatical;
//
// while (*categoria_actual)
// {
//  if (*categoria_actual==categoria) return 1;
//  categoria_actual++;
// }
// return 0;
// 
//}
//
/**************************************************************************/

int artigo_que_inicia_frase(t_frase_separada *pal_frase_separada)
{
 if (  strcmp(pal_frase_separada->pal,"O")==0    ||
       strcmp(pal_frase_separada->pal,"A")==0    ||
       strcmp(pal_frase_separada->pal,"Os")==0   ||
       strcmp(pal_frase_separada->pal,"As")==0          )

       return 1;

 return 0;
}
/**************************************************************************/
int precede_a_palabra_inicio_de_sintagma_preposicional(t_frase_separada *pal_frase_separada)
{
 if(esta_palabra_ten_a_categoria_de(CONTR_CONX_ART_DET,pal_frase_separada-1)||
   esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET,pal_frase_separada-1)||
	(esta_palabra_ten_a_categoria_de(PREP,pal_frase_separada-1) &&
	 strcmp((pal_frase_separada-1)->pal,"a")!=0) ||
    esta_palabra_ten_a_categoria_de(PREP,pal_frase_separada-1)||
    esta_palabra_ten_a_categoria_de(LOC_PREP,pal_frase_separada-1)||
    esta_palabra_ten_a_categoria_de(LOC_PREP_LUG,pal_frase_separada-1) ||
    esta_palabra_ten_a_categoria_de(LOC_PREP_TEMP,pal_frase_separada-1) ||
    esta_palabra_ten_a_categoria_de(LOC_PREP_CANT,pal_frase_separada-1) ||
    esta_palabra_ten_a_categoria_de(LOC_PREP_MODO,pal_frase_separada-1) ||
    esta_palabra_ten_a_categoria_de(LOC_PREP_CAUS,pal_frase_separada-1) ||
    esta_palabra_ten_a_categoria_de(LOC_PREP_CONDI,pal_frase_separada-1))
       return 1;
 return 0;
}
/**************************************************************************/
//int palabra_homografa_esta_rodeada_por_dous_sintagmas_nominais(t_frase_separada *punt_pal_actual)
//{
/* if ( (punt_pal_actual+1)->cat_gramatical[0]==ART_DET*/
// FRAN_CAMPILLO: Le añadí un return 0 para evitar el warning.
//   return 0;
// FRAN_CAMPILLO
//}
/**************************************************************************/

int concorda_en_xenero_e_numero_coa_palabra_anterior(t_frase_separada * pal_frase_separada)
{
 t_palabra pal_actual="";
 t_palabra pal_anterior="";

 if ( esta_palabra_ten_a_categoria_de(NOME,pal_frase_separada-1) )
  {

   /*obtemos as palabras a ser analizadas*/
   if ((pal_frase_separada-1)->pal_transformada!=NULL)
// FRAN_CAMPILLO
//     strcpy(pal_anterior,(char*) *((pal_frase_separada-1)->pal_transformada));
       strcpy(pal_anterior,(pal_frase_separada-1)->pal_transformada);
// FRAN_CAMPILLO
   else
     strcpy(pal_anterior,(pal_frase_separada-1)->pal);
   if ((pal_frase_separada)->pal_transformada!=NULL)

// FRAN_CAMPILLO
//     strcpy(pal_actual,(char*) *(pal_frase_separada->pal_transformada));
     strcpy(pal_actual, pal_frase_separada->pal_transformada);
// FRAN_CAMPILLO

   else
     strcpy(pal_actual,(pal_frase_separada)->pal);

   pal_actual[LONX_MAX_PALABRA-1]=0;
   pal_anterior[LONX_MAX_PALABRA-1]=0;

   /* Por fin obtemos as palabras que deben ser analizadas*/

   if (   (  xenero(pal_actual)==xenero(pal_anterior)  )    &&
	  (  numero(pal_actual)==numero(pal_anterior)  )        )

	return 1; /* é dicir, concorda*/
  }
 return 0; /* por defecto supoñemos que non concorda*/
}

/**************************************************************************/

int hai_antes_adverbio_ou_pronome(t_frase_separada *pal_frase_separada)
{
 if ((esta_palabra_ten_a_categoria_de(PRON_PERS_AT,pal_frase_separada-1)||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_REFLEX,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_ACUS,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_DAT,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(CONTR_PRON_PERS_AT_DAT_AC,pal_frase_separada-1) ) &&
      ! esta_palabra_ten_a_categoria_de(ART_DET,pal_frase_separada-1) )

       return 1;
 if (adverbio((pal_frase_separada-1)->cat_gramatical[0])) return 1;

 if ( (esta_palabra_ten_a_categoria_de(ADVE_AFIRM,pal_frase_separada-2) ||
      esta_palabra_ten_a_categoria_de(ADVE_NEGA,pal_frase_separada-2)) &&
      (esta_palabra_ten_a_categoria_de(PRON_PERS_AT,pal_frase_separada-1)||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_REFLEX,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_ACUS,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_DAT,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(CONTR_PRON_PERS_AT_DAT_AC,pal_frase_separada-1) ))

      return 1;

 if ( esta_palabra_ten_a_categoria_de(PRON_PERS_TON,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(INDEF_PRON,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(DEMO_PRON,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT,pal_frase_separada-1)||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_REFLEX,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_ACUS,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(PRON_PERS_AT_DAT,pal_frase_separada-1) ||
      esta_palabra_ten_a_categoria_de(CONTR_PRON_PERS_AT_DAT_AC,pal_frase_separada-1))

      return 1;


 return 0;
}
/**************************************************************************/
/**************************************************************************/
int homografa_nome_verbo(t_frase_separada * item){
  char nome=0,verbo=0;
  unsigned char *categoria=(unsigned char *)&(item->cat_gramatical[0]);

  while(*categoria){
    if (*categoria==NOME ) nome=1;
    if (*categoria==VERBO) verbo=1;
    categoria++;
  }
  if (item->cat_verbal[0]) verbo=1;

  if (verbo==1 && nome==1) return 3;
  if (verbo==1 && nome==0) return 2;
  if (verbo==0 && nome==1) return 1;
  if (verbo==0 && nome==0) return 0;

  return 0;
}

/**  \fn  void cambiar_categoria_gramatical(t_frase_separada *punt_frase_separada,unsigned char cat_nova)

  Esta funcion coloca en primeiro lugar na lista de categorias gramaticais aquela
  que se lle pasa como parametro.
 */

// FRAN_CAMPILLO: La declaré como void, para evitar el warning.
void cambiar_categoria_gramatical(t_frase_separada *punt_frase_separada,unsigned char cat_nova)
// FRAN_CAMPILLO
{
 unsigned char *punt_aux;
 unsigned char intercambio;

 punt_aux=&punt_frase_separada->cat_gramatical[0];
 while ((*punt_aux!=cat_nova ) && *punt_aux) punt_aux++;
 if ( *punt_aux ==cat_nova)
       {
	intercambio=punt_frase_separada->cat_gramatical[0];
	punt_frase_separada->cat_gramatical[0]=cat_nova;
	*punt_aux=intercambio;
       }
}
/**************************************************************************/

void desambiguar_se_non_hai_verbo_seguro_e_si_homografas(t_frase_separada *punt_recorre,char* num_verbos)
{
 if ( (punt_recorre->clase_pal==PALABRA_NORMAL_EMPEZA_MAY) ||
      (punt_recorre->clase_pal==PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA) )  
    {
     cambiar_categoria_gramatical(punt_recorre,VERBO);
     (*num_verbos)++;
     return;
    }
/*
 if(palabra_homografa_esta_rodeada_por_dous_sintagmas_nominais(punt_recorre))
    {
     cambiar_categoria_gramatical(punt_recorre,VERBO);
     (*num_verbos)++;
     return;
    }
*/
 if (artigo_que_inicia_frase(punt_recorre-1))
    {
     cambiar_categoria_gramatical(punt_recorre,NOME);
     return;
    }
 if (precede_a_palabra_inicio_de_sintagma_preposicional(punt_recorre))
    {
     cambiar_categoria_gramatical(punt_recorre,NOME);
     return;
    }
 if (hai_antes_adverbio_ou_pronome(punt_recorre))
    {
     cambiar_categoria_gramatical(punt_recorre,VERBO);
     (*num_verbos)++;
     return;
    }
 if (!concorda_en_xenero_e_numero_coa_palabra_anterior(punt_recorre))
    {
     cambiar_categoria_gramatical(punt_recorre,VERBO);
     (*num_verbos)++;
     return;
    }

 return;
}
/**************************************************************************/
/**************************************************************************/
int contar_verbos_e_homografas(t_frase_separada * punt_fin,char *num_verbos,char *num_homografas)
{
 int tipo;

   tipo=homografa_nome_verbo(punt_fin);
   switch(tipo)
     {
      case 0:return 0;
      case 1:return 1;
      case 2:(*num_verbos)++;return 2;
      case 3:(*num_homografas)++;return 3;
      default: break;
     }
// FRAN_CAMPILLO: Le puse un return 0. Por poner algo, ya que luego pasa de ello.
   return 0;
// FRAN_CAMPILLO
}

/**************************************************************************/
// FRAN_CAMPILLO: La declaré void, para evitar el warning
void desambiguar_homografia_nome_verbo(t_frase_separada *frase_separada)
// FRAN_CAMPILLO
{
 t_frase_separada *punt_inicio;
 t_frase_separada *punt_fin=frase_separada;
 t_frase_separada *punt_recorre;
 char num_verbos;
 char num_homografas;


 while (punt_fin->pal[0])
 {
  num_verbos=0;
  num_homografas=0;

  punt_inicio=punt_fin;
  while( nexo_ou_pausa(punt_inicio->cat_gramatical[0]) && punt_inicio->pal[0] ) punt_inicio++;
  punt_fin=punt_inicio;
  while( !nexo_ou_pausa(punt_fin->cat_gramatical[0]) && punt_fin->pal[0] )
  {
    contar_verbos_e_homografas(punt_fin,&num_verbos,&num_homografas);
    punt_fin++;
  }

  punt_recorre=punt_inicio;
  while(punt_recorre<punt_fin)
  {
   if (num_verbos==1)
     if (homografa_nome_verbo(punt_recorre)==3)
     /*
       Se é homografa a palabra, enton eliminámo-la posibilidade de verbo,
       poñendo en primeiro lugar a categoria de nome.
     */
      cambiar_categoria_gramatical(punt_recorre,NOME);

   if (num_verbos==0   &&
       esta_palabra_ten_a_categoria_de(NOME,punt_recorre) &&
       esta_palabra_ten_a_categoria_de(VERBO,punt_recorre)     )

     desambiguar_se_non_hai_verbo_seguro_e_si_homografas(punt_recorre,&num_verbos);

   punt_recorre++;
  }

  num_verbos=0;
  num_homografas=0;

 }

}


/**************************************************************************/
/**************************************************************************/
void desambiguar_categorias_gramaticais(t_frase_separada *frase_separada)
{
  desambiguar_homografia_nome_verbo(frase_separada);
  return;
}
/**************************************************************************/
/**** Fin de funcions para desambigua-las categorias gramaticais **********/
/**************************************************************************/
/*int adverbio(unsigned char codigo){
   if (codigo >= ADVE && codigo <=ADVE_ESPECIFICADOR ) return 1;
	else return 0;
}
*//**************************************************************************/


/** \fn  int por_contexto_e_nome_e_morfoloxicamente_verbo(t_frase_separada *recorre_frase_separada)

  Esta funcion utilizase para comprobar se unha forma que ten forma de verbo
 pero polo contexto ten que ser nome. Isto sabemolo se o a palabra que a
 precede e un artigo indeterminado ou contraccions de preposicions con artigos.
 */

int por_contexto_e_nome_e_morfoloxicamente_verbo(t_frase_separada *recorre_frase_separada)

{
 if ( (recorre_frase_separada-1)->cat_gramatical[0]==ART_INDET           ||
      (recorre_frase_separada-1)->cat_gramatical[0]==CONTR_PREP_ART_INDET||
      (recorre_frase_separada-1)->cat_gramatical[0]==INDEF_DET ||
      (recorre_frase_separada-1)->cat_gramatical[0]==PREP ||
      (recorre_frase_separada-1)->cat_gramatical[0]==CONTR_PREP_ART_DET  )
      {
       elimina_categoria(VERBO,recorre_frase_separada);
       recorre_frase_separada->cat_gramatical[0]=NOME;
       return 1;
      }
 return 0;
}

/** \fn  t_frase_separada *comprobar_sintagma_nominal(t_frase_separada*frase_separada)

	Esta función detecta se o conxunto de palabras pertencen a un sintagma
	nominal. Devolve a ultima palabra que se detectou como pertencente ó SN.
	Noutro caso devolve o punteiro apuntado o mesmo sitio ondeº empezou o
	posible SN.
 */

t_frase_separada *comprobar_sintagma_nominal(t_frase_separada*frase_separada)

{
 t_frase_separada *recorre_frase_separada=(frase_separada+1);
 unsigned char patron_total_aceptado=0,patron_parcial_aceptado=0;
 unsigned char *categoria_gram_actual;

/* Comprobamos primeiro se hai un determinante deste tipo. Este determinante
	 pode ser optativo.*/

 categoria_gram_actual=recorre_frase_separada->cat_gramatical;

	while(*categoria_gram_actual && patron_parcial_aceptado==0)
 {
	 switch(*categoria_gram_actual)
		{
		 case ART_DET:case ART_INDET:
			{
			 patron_parcial_aceptado=1;
			 recorre_frase_separada++;
			 break;
			}
		 }
		categoria_gram_actual++;
 }

 categoria_gram_actual=recorre_frase_separada->cat_gramatical;
 patron_total_aceptado=(unsigned char)(patron_parcial_aceptado || patron_total_aceptado);
 patron_parcial_aceptado=0;

 while(*categoria_gram_actual && patron_parcial_aceptado==0)
 {
	 switch(*categoria_gram_actual)
		{
     case POSE:
		 case POSE_DET:
     case INDEF:
		 case INDEF_DET:
// FRAN_CAMPILLO: Añado aquí CONTR_INDEF_ART_DET y CONTR_DEMO_INDEF_DET
	 case CONTR_DEMO_INDEF_DET:
     case CONTR_INDEF_ART_DET:
// FRAN_CAMPILLO: también DEMO_DET
	 case DEMO_DET:
     case NUME_CARDI:
     case NUME_ORDI:
     case NUME_PARTI:
     case NUME_CARDI_DET:
     case NUME_ORDI_DET:
     case NUME_PARTI_DET:
			{
			 patron_parcial_aceptado=1;
			 recorre_frase_separada++;
			 break;
			}
		 }
		categoria_gram_actual++;
 }

 //categoria_gram_actual=recorre_frase_separada->cat_gramatical;
 patron_total_aceptado=(unsigned char) (patron_parcial_aceptado || patron_total_aceptado);
 patron_parcial_aceptado=0;


// FRAN_CAMPILLO: Antes del nombre puede haber un adjetivo:

if (recorre_frase_separada->cat_gramatical[0] == ADXECTIVO)
	recorre_frase_separada++;

 /* Comprobamos se hai algun nome*/

 while (recorre_frase_separada->cat_gramatical[0]==NOME ||
 // FRAN_CAMPILLO: Añado los pronombres:
 				recorre_frase_separada->cat_gramatical[0]==NUME_CARDI_PRON ||
                recorre_frase_separada->cat_gramatical[0]==NUME_ORDI_PRON ||
 				recorre_frase_separada->cat_gramatical[0]==POSE_PRON ||
 				recorre_frase_separada->cat_gramatical[0]==DEMO_PRON ||
 				recorre_frase_separada->cat_gramatical[0]==INDEF_PRON ||
 				recorre_frase_separada->cat_gramatical[0]==PRON_PERS_AT ||
                recorre_frase_separada->cat_gramatical[0]==PRON_PERS_TON ||
				recorre_frase_separada->cat_gramatical[0]==CAT_NON_CLASIFICADA   ||
				recorre_frase_separada->cat_gramatical[0]==NOME_PROPIO /*||
				(
				 recorre_frase_separada->cat_gramatical[0]==VERBO &&
				 por_contexto_e_nome_e_morfoloxicamente_verbo(recorre_frase_separada)) */ )
			{
				recorre_frase_separada++;
				patron_parcial_aceptado=1;

				if (recorre_frase_separada->cat_gramatical[0]==NUME_CARDI_PRON ||
 					recorre_frase_separada->cat_gramatical[0]==POSE_PRON ||
 					recorre_frase_separada->cat_gramatical[0]==DEMO_PRON ||
 					recorre_frase_separada->cat_gramatical[0]==INDEF_PRON ||
 					recorre_frase_separada->cat_gramatical[0]==PRON_PERS_AT ||
                	recorre_frase_separada->cat_gramatical[0]==PRON_PERS_TON )
                    break;

			}

 categoria_gram_actual=recorre_frase_separada->cat_gramatical;
 patron_total_aceptado=(unsigned char) (patron_parcial_aceptado || patron_total_aceptado);
 patron_parcial_aceptado=0;

/* Comprobamos primeiro se hai un determinante deste tipo. Este determinante
	 pode ser optativo.*/


 while(*categoria_gram_actual && patron_parcial_aceptado==0)
 {
	 switch(*categoria_gram_actual)
		{
//     case POSE:
		 case POSE_DET:
//     case INDEF:
		 case INDEF_DET:
//     case NUME_CARDI:
//     case NUME_ORDI:
//     case NUME_PARTI:
     case NUME_CARDI_DET:
     case NUME_ORDI_DET:
     case NUME_PARTI_DET:
// FRAN_CAMPILLO: Quito los casos generales y dejo los determinantes:
//		 case DEMO:
	 case DEMO_DET:
			{
			if ((recorre_frase_separada + 1)->pal[0])
            	if ( ((recorre_frase_separada + 1)->cat_gramatical[0] == NOME) ||
                     ((recorre_frase_separada + 1)->cat_gramatical[0] == NOME_PROPIO) )
                	break;
			 patron_parcial_aceptado=1;
			 recorre_frase_separada++;
			 break;
			}
		 }
	 categoria_gram_actual++;
 }

// patron_total_aceptado=patron_parcial_aceptado || patron_total_aceptado;

 if (patron_parcial_aceptado || patron_total_aceptado)
      return recorre_frase_separada-1;
 return frase_separada;
}


/** \fn  t_frase_separada *adiantar_as_preposicions_ou_locucions_prepositivas(t_frase_separada *frase_separada)

    	Esta función avanza mentres a palabra que se detecte sexa unha preposición
	ou pertenza a unha locución prepositiva.
 */

t_frase_separada *adiantar_as_preposicions_ou_locucions_prepositivas(t_frase_separada *frase_separada)

{
 t_frase_separada *recorre_frase_separada=frase_separada;
	while ((recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_LUG ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_TEMP ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_CANT ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_MODO ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_CAUS ||
            (recorre_frase_separada+1)->cat_gramatical[0]==LOC_PREP_CONDI ||
            (recorre_frase_separada+1)->cat_gramatical[0]==PREP ||
				(recorre_frase_separada+1)->cat_gramatical[0]==CONTR_PREP_ART_DET  ||
				(recorre_frase_separada+1)->cat_gramatical[0]==CONTR_PREP_ART_INDET  ||
				(recorre_frase_separada+1)->cat_gramatical[0]==CONTR_PREP_DEMO ||
				(recorre_frase_separada+1)->cat_gramatical[0]==CONTR_PREP_PRON_PERS_TON )

      recorre_frase_separada++;

 return recorre_frase_separada;
}

/***************************************************************************/
int locucion(t_frase_separada *item) {
/*
   if ((item->tipo_sintagma >= LOC_ADVE_LUG && item->tipo_sintagma <= LOC_ADVE_DUBI)||
      (item->tipo_sintagma >= LOC_PREP_LUG && item->tipo_sintagma <= LOC_PREP_MODO)||
      (item->tipo_sintagma >= LOC_CONX_COOR_COPU && item->tipo_sintagma <= LOC_CONX_SUBOR_MODA))
*/
   if ((item->cat_gramatical[0] >= LOC_ADVE && item->cat_gramatical[0] <= LOC_ADVE_DUBI)||
      (item->cat_gramatical[0] >= LOC_PREP && item->cat_gramatical[0] <= LOC_PREP_CONDI)||
      (item->cat_gramatical[0] >= LOC_CONX && item->cat_gramatical[0] <= LOC_CONX_SUBOR_CORREL))

      return 1;
   else return 0;
}
/***************************************************************************/
int sintagma_de_puntuacion(unsigned char codigo){
// Fran Campillo: ahora consideramos también la ruptura entonativa:
	if( (codigo >= PUNTO) && (codigo <= RUPTURA_COMA) )
		return 1;
    else
    	return 0;
//	if(codigo>=PUNTO  &&  codigo<=GUION_BAIXO) return 1;
//	else return 0;
}


/** 
 * \fn t_frase_separada *clasificar_sintagma_funcion(t_frase_separada *recorre_frase_separada,t_frase_sintagmada *frase_sintagmada,int *cont)
 *
 * \author fmendez
 *
 * \remarks Esta función vai collendo as palabras de frase separada e según a
 * súa categoría gramatical agrúpaas en sintagmas.Tamén será considerados como
 * sintagmas independientes os nexos ou os signos de puntuación.  O valor de
 * cont modifica o valor da variable ca que se chama para actualiza-lo indice
 * de frase separada no que imos. 
 * 
 * \param recorre_frase_separada[in]
 * \param frase_sintagmada[in,out] 
 * \param cont[in,out] o valor de cont modifica o valor da variable ca que se chama
 * para actualiza-lo indice de frase separada no que imos. 
 * 
 * \return Puntero a la palabra siguiente en frase_separada al sintagma detectado
 */
t_frase_separada *clasificar_sintagma_funcion(t_frase_separada *recorre_frase_separada,
		t_frase_sintagmada *frase_sintagmada, int *cont){

	t_frase_separada *frase_separada;
	t_frase_separada *punt_movil;
	t_frase_separada *referencia_de_frase_separada=recorre_frase_separada;
	unsigned char *categoria_gram_actual;
	unsigned char sintagma_xa_asignado=0;

	categoria_gram_actual=recorre_frase_separada->cat_gramatical;


	while(*categoria_gram_actual && !sintagma_xa_asignado)
		/* Para unha palabra examinamos toda a sua lista de posibles categorías
			 gramaticais.*/
	{
		frase_separada=punt_movil=recorre_frase_separada;
		switch(*categoria_gram_actual)
		{
			case NOME_PROPIO: case NOME:
				{
					while ((recorre_frase_separada+1)->cat_gramatical[0]==NOME_PROPIO  ||
							(recorre_frase_separada+1)->cat_gramatical[0]==NOME  )
						recorre_frase_separada++;

					frase_sintagmada->inicio=*cont;

					if ( (recorre_frase_separada + 1)->pal[0]) {

						categoria_gram_actual = (recorre_frase_separada + 1)->cat_gramatical;


						switch(*categoria_gram_actual) {

							case POSE_DET:
							case INDEF_DET:
							case NUME_CARDI_DET:
							case NUME_ORDI_DET:
							case NUME_PARTI_DET:
							case DEMO_DET:
								if ((recorre_frase_separada + 2)->pal[0]) {
									if ( ((recorre_frase_separada + 1)->cat_gramatical[0] != NOME) ||
											((recorre_frase_separada + 1)->cat_gramatical[0] != NOME_PROPIO) )
										recorre_frase_separada++;
								}
								else
									recorre_frase_separada++;
						}
					}
					frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					*cont=1+frase_sintagmada->fin;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;

				}
			case ART_DET: //case CONTR_CONX_ART_DET: // Fran Campillo: Quito de aquí la contracción de conjunción,
        	// ya que me parece más útil para el análisis que aparezca como conjunción.
				/* Debe haber algo mais ca esta cabeceira xa que non poden ser
					 pronomes.
					 Palabras como "mailos" ou "cá" son conxuncions da preposicion co
					 artigo e funcionan como artigos*/

				{
					recorre_frase_separada=comprobar_sintagma_nominal(recorre_frase_separada);
					if(recorre_frase_separada!=frase_separada)
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
						frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada++;
						sintagma_xa_asignado=1;
						break;
					}
					if(  recorre_frase_separada->cat_gramatical[0]==ART_DET       &&
							(recorre_frase_separada+1)->cat_gramatical[0]==RELA &&
							!relativo_interrogativo_ou_exclamativo(referencia_de_frase_separada)   )
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+1;
						frase_sintagmada->tipo_sintagma=NEXO_SUBORDINANTE;
						*cont=*cont+2;
						recorre_frase_separada=recorre_frase_separada+2;
						sintagma_xa_asignado=1;
						break;
					}
					if(  recorre_frase_separada->cat_gramatical[0]==ART_DET       &&
							(recorre_frase_separada+1)->cat_gramatical[0]==RELA )
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+1;
						// FRAN_CAMPILLO: Cambio Sintagma Preposicional por Nominal
						frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
						//					frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
						*cont=*cont+2;
						recorre_frase_separada=recorre_frase_separada+2;
						sintagma_xa_asignado=1;
						break;
					}

					break;
				}
				// FRAN_CAMPILLO: Añadimos DEMO_DET. Esto es un caos.
				// FRAN_CAMPILLO: (años más tarde). Añadimos CONTR_INDEF_ART_DET. Esto sique siendo un caos...
				// FRAN_CAMPILLO: CONTR_DEMO_INDEF_DET, NUME_CARDI_DET, POSE_DET
			case INDEF_DET: case DEMO_DET: case CONTR_INDEF_ART_DET:
			case CONTR_DEMO_INDEF_DET: case NUME_CARDI_DET: case POSE_DET:
				/*certos, sempre funciona como determinante, nunca como pronome*/
				{
					recorre_frase_separada=comprobar_sintagma_nominal(frase_separada);
					if(recorre_frase_separada!=frase_separada)
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
						frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada++;
						sintagma_xa_asignado=1;
						break;
					}
					break;
				}
				// FRAN_CAMPILLO: Añado los pronombres numerales:  NUME_CARDI_PRON, NUME_ORDI_PRON
				// FRAN_CAMPILLO: También los posesivos: POSE_PRON
			case DEMO_PRON:case  INDEF_PRON:
			case NUME_ORDI_PRON: case NUME_CARDI_PRON:
			case POSE_PRON:
				/* son sempre pronomes*/
				{
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
			case DEMO:case POSE:case INDEF:case NUME:
			case POSE_DISTR:case ART_INDET:
				/* Poden ser pronomes, e polo tanto solo haber esta cabeceira*/
				{
					recorre_frase_separada=comprobar_sintagma_nominal(frase_separada);
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					*cont=1+frase_sintagmada->fin;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
				// FRAN_CAMPILLO: Añado interrogativos y exclamativos:
			case INTER:
			case EXCLA:
				recorre_frase_separada=comprobar_sintagma_nominal(frase_separada);
				if(recorre_frase_separada!=frase_separada)
				{
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					*cont=1+frase_sintagmada->fin;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
				}
				else {
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
				}
				break;

			case CONTR_PREP_ART_DET :
			case PREP:
			case LOC_PREP:
			case LOC_PREP_LUG:
			case LOC_PREP_TEMP:
			case LOC_PREP_CANT:
			case LOC_PREP_MODO:
			case LOC_PREP_CAUS:
			case LOC_PREP_CONDI:
				/*Por exemplo "a forza de", "a rentes de","cara a"*/
				/*Ademais de ir formando sintagmas preposicionais poden introducir
					unha subordinada de relativo. "... en que..." "do que". "a por do que fixeron...".
					Non poden
					ir solos. Necesitan doutro determinante pronomizado ou dun nome*/
				{
					/*				 if ( (recorre_frase_separada+1)->cat_gramatical==PREPOSICION_SEN_SIGNIFICADO)
										 CONTR_PREP_DEMO
										 Despois dunha preposicion pode ir outra preposicion. "Estaba sobre da mesa".*/
					recorre_frase_separada=adiantar_as_preposicions_ou_locucions_prepositivas(frase_separada);
					punt_movil=recorre_frase_separada;

					recorre_frase_separada=comprobar_sintagma_nominal(recorre_frase_separada);
					if (recorre_frase_separada!=punt_movil)
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
						frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada++;
						sintagma_xa_asignado=1;
						break;
					}
                                        
					if ((recorre_frase_separada + 1)->cat_gramatical[0] == ADXECTIVO)
					{
                                                recorre_frase_separada++;
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
						frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada++;
						sintagma_xa_asignado=1;
						break;
					}

					if ( (recorre_frase_separada+1)->cat_gramatical[0]==DEMO_PRON ||
							(recorre_frase_separada+1)->cat_gramatical[0]==INDEF_PRON ||
							(recorre_frase_separada+1)->cat_gramatical[0]==PRON_PERS_TON ||
							((recorre_frase_separada+1)->cat_gramatical[0]<=PRON_PERS_AT &&
							 (recorre_frase_separada+1)->cat_gramatical[0]>=CONTR_PRON_PERS_AT_DAT_AC))
					{
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada)+1;
						frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada=recorre_frase_separada+2;
						sintagma_xa_asignado=1;
						break;
					}

					if ( adverbio((recorre_frase_separada+1)->cat_gramatical[0]))
					{
						frase_sintagmada->inicio=*cont;
						recorre_frase_separada++;
						if (locucion(recorre_frase_separada)) {
							while (locucion(recorre_frase_separada))
								recorre_frase_separada++;

							frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada)-1;

							*cont=frase_sintagmada->fin+1;
						}
						else {
							frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
							*cont=1+frase_sintagmada->fin;
							recorre_frase_separada++;
						}
						frase_sintagmada->tipo_sintagma=SINTAGMA_ADVERBIAL;
						sintagma_xa_asignado=1;
						break;

					}

//					if ( (recorre_frase_separada+1)->cat_gramatical[0]==RELA &&
//							!relativo_interrogativo_ou_exclamativo(referencia_de_frase_separada)   )
// FRAN_CAMPILLO: He cambiado esto. Prefiero que falle alguna vez en las interrogativas y
// exclamativas a que en esos casos siempre quede como NO_ASIGNADO
					if ( (recorre_frase_separada + 1)->cat_gramatical[0] == RELA) {
						frase_sintagmada->inicio=*cont;
						frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada)+1;
						frase_sintagmada->tipo_sintagma=NEXO_SUBORDINANTE;
						*cont=1+frase_sintagmada->fin;
						recorre_frase_separada=recorre_frase_separada+2;
						sintagma_xa_asignado=1;
						break;
					}


					recorre_frase_separada=frase_separada;
					break;
				}
				// FRAN_CAMPILLO: Añado unas cuantas contracciones que no se están teniendo en cuenta:
				// Es un coñazo tener que andar considerando todas las posibles contracciones en todos
				// lados, habría que buscar una forma de que fuese más cómodo.
			case CONTR_PREP_INDEF_DET:
			case CONTR_PREP_INDEF:
			case CONTR_PREP_INDEF_PRON:
			case CONTR_PREP_DEMO_DET:
			case CONTR_PREP_DEMO_PRON:
			case CONTR_PREP_DEMO_INDEF:
			case CONTR_PREP_DEMO_INDEF_DET:
			case CONTR_PREP_DEMO_INDEF_PRON:

				// FRAN_CAMPILLO: Hasta aquí.
			case CONTR_PREP_ART_INDET: case CONTR_PREP_DEMO:
				/*Sempre funciona cun nome ou como pronome toda a palabra. Non admite este
					tipo de sintagma preposicional un pronome persoal.*/
				{

					recorre_frase_separada=comprobar_sintagma_nominal(frase_separada);
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont+(recorre_frase_separada-frase_separada);
					frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
					*cont=1+frase_sintagmada->fin;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
			case CONTR_PREP_PRON_PERS_TON:
				/*dela*/
				{
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=SINTAGMA_PREPOSICIONAL;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
			case PRON_PERS_TON:
			case PRON_PERS_AT:
			case PRON_PERS_AT_REFLEX:
			case PRON_PERS_AT_ACUS:
			case PRON_PERS_AT_DAT:
			case CONTR_PRON_PERS_AT_DAT_AC:
				/* PODERA SER UTIL FACE-LA CLASIFICACION SE E UN SN ATONO OU NON
					 nun FUTURO. O pronome indefinido é "ninguen"*/
				{
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
			case ADVE_ESPECIFICADOR:
				/*Moi alto, tan baixo*/
				{
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont+1;
					frase_sintagmada->tipo_sintagma=SINTAGMA_ADXECTIVAL;
					*cont=*cont+2;
					recorre_frase_separada=recorre_frase_separada+2;
					sintagma_xa_asignado=1;
					break;
				}
			case ADVE:
			case ADVE_LUG:
			case ADVE_TEMP:
			case ADVE_CANT:
			case ADVE_MODO:
			case ADVE_AFIRM:
			case ADVE_NEGA:
			case ADVE_DUBI:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=SINTAGMA_ADVERBIAL;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
				// FRAN_CAMPILLO: Añado aquí la conjunción subordinada completiva.
				// No lo tengo claro, la verdad. En realidad habría que comprobar todas estas
				// reglas.
			case CONX_SUBOR_COMPLETIVA:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=CONX_SUBOR;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;

			case RELA:
				if(!relativo_interrogativo_ou_exclamativo(referencia_de_frase_separada)){
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=NEXO_SUBORDINANTE;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;
				}
				else{
					//recorre_frase_separada->cat_gramatical[0]=RELATIVO_TONICO;
					frase_sintagmada->inicio=*cont;
					frase_sintagmada->fin=*cont;
					frase_sintagmada->tipo_sintagma=SINTAGMA_NOMINAL;
					(*cont)++;
					recorre_frase_separada++;
					sintagma_xa_asignado=1;
					break;

				}
			case CAT_NON_CLASIFICADA:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=SINTAGMA_NON_CLASIFICADO;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
			case ADXECTIVO:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=SINTAGMA_ADXECTIVAL;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
			case CONX_COOR:
			case CONX_COOR_COPU:
			case CONX_COOR_DISX:
			case CONX_COOR_ADVERS:

			case CONX_SUBOR:
			case CONX_SUBOR_CAUS:
			case CONX_SUBOR_CONCES:
			case CONX_SUBOR_CONSE:
			case CONX_SUBOR_CONDI:
			case CONX_SUBOR_COMPAR:
			case CONX_SUBOR_LOCA:
			case CONX_SUBOR_TEMP:
			case CONX_SUBOR_MODAL:
            case CONX_SUBOR_PROPOR:
            case CONX_SUBOR_CONTRAP:
            case CONX_SUBOR_FINAL:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=frase_separada->cat_gramatical[0];
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
// Fran Campillo: Añado aquí la contracción de conjunción con artículo. La asimilo a conjunción
// subordinada comparativa (a nivel de sintagma), en ausencia de más información.
            case CONTR_CONX_ART_DET:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma = CONX_SUBOR_COMPAR;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
				// Añadimos la perífrasis:
			case PERIFRASE:
				frase_sintagmada->inicio = *cont;
				while ((++recorre_frase_separada)->cat_gramatical[0] == PERIFRASE)
					(*cont)++;
				frase_sintagmada->fin = *cont;
				(*cont)++;
				frase_sintagmada->tipo_sintagma = SINTAGMA_VERBAL;
				sintagma_xa_asignado = 1;
				break;
			case VERBO:
				// FRAN_CAMPILLO: Añado XERUNDIO. Esto hay que cambiarlo. El análisis morfosintáctico
				// debería decidir si es verbo o adverbio.
			case XERUNDIO:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=SINTAGMA_VERBAL;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
			default:
				frase_sintagmada->inicio=*cont;
				frase_sintagmada->fin=*cont;
				frase_sintagmada->tipo_sintagma=SINTAGMA_NON_CLASIFICADO;
				(*cont)++;
				recorre_frase_separada++;
				sintagma_xa_asignado=1;
				break;
		}  /* fin do switch*/

		categoria_gram_actual++;

	} /* fin do while*/

	if (  recorre_frase_separada==referencia_de_frase_separada &&
			*categoria_gram_actual==0				)

		/* Isto indicanos que non se encontrou un modelo en alguns casos como
			 modelos de snominais ou preposicionais. Enton asignamos un sintagma
			 sin clasificar e seguimos
			 */

	{

		frase_sintagmada->inicio=*cont;
		frase_sintagmada->fin=*cont;
        if (recorre_frase_separada->pal_transformada) {
    	    if ( (strcmp(recorre_frase_separada->pal_transformada, "para") == 0) &&
	        	  (recorre_frase_separada->cat_gramatical[0] == PREP) )
        		frase_sintagmada->tipo_sintagma = CONX_SUBOR_FINAL;
        	else
            	if ( (strcmp(recorre_frase_separada->pal_transformada, "de") == 0) &&
                	 ((recorre_frase_separada + 1)->cat_gramatical[0] == CONX_SUBOR_COMPLETIVA) ) {
                	frase_sintagmada->tipo_sintagma = CONX_SUBOR;
                    (*cont)++;
                    frase_sintagmada->fin = *cont;
                    recorre_frase_separada++;
                }
                else
					frase_sintagmada->tipo_sintagma=SINTAGMA_NON_CLASIFICADO;
        }
        else
    	    if ( (strcmp(recorre_frase_separada->pal, "para") == 0) &&
	        	  (recorre_frase_separada->cat_gramatical[0] == PREP) ) {
                asigna_categoria(CONX_SUBOR_FINAL, recorre_frase_separada);
        		frase_sintagmada->tipo_sintagma = CONX_SUBOR_FINAL;
            }
        	else
            	if ( (strcmp(recorre_frase_separada->pal, "de") == 0) &&
                	 ((recorre_frase_separada + 1)->cat_gramatical[0] == CONX_SUBOR_COMPLETIVA) ) {
                	frase_sintagmada->tipo_sintagma = CONX_SUBOR;
                    (*cont)++;
                    frase_sintagmada->fin = *cont;
                    recorre_frase_separada++;
                }
                else
					frase_sintagmada->tipo_sintagma=SINTAGMA_NON_CLASIFICADO;


		(*cont)++;
		recorre_frase_separada++;
	}

	return recorre_frase_separada;
}

/** 
 * \fn void Sintagma::identificar_sintagmas(t_frase_separada *frase_separada,t_frase_sintagmada *frase_sintagmada)
 * 
 * \author fmendez
 *
 * \remarks  A variable frase_separada colocase sempre no elemento de inicio do
 * sintagma, e a variable recorre_frase_separada vai escaneando ata o elemento
 * final de do sintagma.  A variable cont é o indice que nos indica a orde de
 * cada elemento da variable global frase_separada. Este valor vaise
 * actualizando, indicandonos o lugar onde imos analizando. 
 * 
 * \param frase_separada 
 * \param frase_sintagmada 
 */
void Sintagma::identificar_sintagmas(void){

	t_frase_separada	*ptr_fseparada = frase_separada; 
							//		*frase_separada_inicial = frase_separada;
	t_frase_sintagmada *ptr_fsintagmada = frase_sintagmada;
	int cont = 0;
	unsigned char categoria_original;
	t_frase_separada *fin_locucion, *inicio_locucion;

	while (*ptr_fseparada->pal) {
		if (locucion(ptr_fseparada)) {
			// As palabras que forman parte dunha locucion agrupanse nun sintagma
			categoria_original = *ptr_fseparada->cat_gramatical;
			inicio_locucion = ptr_fseparada;
			ptr_fsintagmada->inicio = cont;

			do {
				ptr_fseparada++;
			} while ( (locucion(ptr_fseparada)) && (*ptr_fseparada->cat_gramatical == categoria_original) );

			fin_locucion = ptr_fseparada - 1;

			if ( *fin_locucion->cat_gramatical >= LOC_ADVE && *fin_locucion->cat_gramatical <= LOC_ADVE_DUBI ) {
				ptr_fsintagmada->tipo_sintagma = SINTAGMA_ADVERBIAL;
				ptr_fsintagmada->fin = fin_locucion - frase_separada;
				cont = ptr_fsintagmada->fin + 1;
			}
			else if ( *fin_locucion->cat_gramatical >= LOC_PREP && *fin_locucion->cat_gramatical <= LOC_PREP_CONDI ) {
				ptr_fseparada = clasificar_sintagma_funcion(inicio_locucion, ptr_fsintagmada, &cont);
				if (ptr_fsintagmada->tipo_sintagma == SINTAGMA_NON_CLASIFICADO) {
					ptr_fsintagmada->fin = fin_locucion - frase_separada;
					cont = ptr_fsintagmada->fin + 1;
					ptr_fseparada = fin_locucion + 1;
				}
			}
			else {
				if ( *fin_locucion->cat_gramatical >= LOC_CONX_SUBOR_CAUS && *fin_locucion->cat_gramatical <= LOC_CONX_SUBOR_CORREL) 
					ptr_fsintagmada->tipo_sintagma = CONX_SUBOR;
				else
					if (*fin_locucion->cat_gramatical == LOC_CONX_COOR_COPU)
						ptr_fsintagmada->tipo_sintagma = CONX_COOR_COPU;
					else
						if (*fin_locucion->cat_gramatical == LOC_CONX_COOR_ADVERS)
							ptr_fsintagmada->tipo_sintagma = CONX_COOR_ADVERS;
						else
							ptr_fsintagmada->tipo_sintagma = (ptr_fseparada - 1)->tipo_sintagma;

				ptr_fsintagmada->fin = fin_locucion - frase_separada;
				cont = ptr_fsintagmada->fin + 1;
			}
			cuenta_silabas_sintagma(ptr_fsintagmada);
			ptr_fsintagmada++;
		}
		else { // no es locución
			if (sintagma_de_puntuacion(*ptr_fseparada->cat_gramatical)) {
				ptr_fsintagmada->inicio = cont;
				ptr_fsintagmada->fin = cont;
				ptr_fsintagmada->tipo_sintagma = *ptr_fseparada->cat_gramatical;
				cont++;
				ptr_fseparada++;
				ptr_fsintagmada++;
			}
			else { // senon tratase dun sintagma funcion( S. prep., SN, nexos dunha palabra.
				ptr_fseparada = clasificar_sintagma_funcion(ptr_fseparada, ptr_fsintagmada, &cont);
				cuenta_silabas_sintagma(ptr_fsintagmada);
				ptr_fsintagmada++;
			}
		}
	}

	// FRAN CAMPILLO: Rellenamos el campo tipo_sintagma de frase_separada:
	ptr_fseparada = frase_separada;
	ptr_fsintagmada = frase_sintagmada;
	for (cont = 0; *ptr_fseparada->pal != '\0'; cont++, ptr_fseparada++) {

		if (ptr_fsintagmada->fin < cont)
			ptr_fsintagmada++;

		ptr_fseparada->tipo_sintagma = ptr_fsintagmada->tipo_sintagma;

	}
	// FRAN CAMPILLO
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param codigo 
 * 
 * \return 
 */
int nexo_bipolar(unsigned char codigo){
	if (codigo >= CONX_COOR && codigo <= LOC_CONX_SUBOR_PROPOR){
		return 1;
	}
	else {
		return 0;
	}
}
/**************************************************************************/
int signo_fin_proposicion(unsigned char clase_signo){
	if(clase_signo>=PUNTO  &&  clase_signo<=GUION_BAIXO) return 1;
	else return 0;
}
/*************************************************************************/
int nexo_coordinativo(unsigned char codigo){
  switch(codigo){
    case CONX_COOR:
    case CONX_COOR_COPU:
      case CONX_COOR_DISX:
      case CONX_COOR_ADVERS:
      return 1;
  }
  return 0;
}
/**************************************************************************/
/**************************AGRUPAR ENTRE PAUSAS OU NEXOS ***********************/
/************************************************************************/
int nexo(unsigned char codigo){
	 if ( nexo_bipolar(codigo) ) return 1;
    if ( nexo_coordinativo(codigo) )  return 1;
	 if ( codigo==NEXO_SUBORDINANTE ) return 1;
	 if ( codigo>=CONX_SUBOR && codigo<=CONX_SUBOR_MODAL) return 1;

	 return 0;
 }
/************************************************************************/

/** \fn   int nexo_ou_pausa(unsigned char codigo)

 Detecta se o codigo que se lle pasa e o correspondente a unha palabra nexo
 ou a un signo que indica pausa.
 */

int nexo_ou_pausa(unsigned char codigo)

 {
	 if ( nexo(codigo) ) return 1;
	 if ( sintagma_de_puntuacion(codigo) ) return 1;

	 return 0;
 }
/************************************************************************/

/** \fn   unsigned char clasifica_segun_caracter_de_pausa_e_entonacion(unsigned char codigo)

                Como saida indicase se o inicio,fin,o inicio do grupo seguinte ou
		o fin do anterior grupo e por palabras, por nexo,  por que tipo de
		nexo,etc.
*/


unsigned char clasifica_segun_caracter_de_pausa_e_entonacion(unsigned char codigo)
 {

 return codigo;
 }

/************************************************************************/

/** \fn   int inicio_de_grupo(unsigned char codigo)
	 Detectamos se o signo ou nexo pertence a aqueles os que se lle permite
	 iniciar un grupo.
*/
int inicio_de_grupo(unsigned char codigo)

{
	 if ( nexo_bipolar(codigo) ) return 1;
	 if ( nexo_coordinativo(codigo) )  return 1;
	 if ( codigo==NEXO_SUBORDINANTE ) return 1;
	 if ( codigo>=CONX_SUBOR && codigo<=CONX_SUBOR_MODAL) return 1;

	 switch(codigo)
	 {
		 case APERTURA_INTERROGACION: return 1;
		 case APERTURA_EXCLAMACION: return 1;
		 case APERTURA_PARENTESE: return 1;
		 case APERTURA_CORCHETE:return 1;
	 }
	return 0;
}
/************************************************************************/
/************************************************************************/
void Sintagma::identificar_grupos_entre_pausas(t_frase_sintagmada *frase_sintagmada,
		t_frase_en_grupos *frase_en_grupos){

	t_frase_sintagmada *aux_frase_sintagmada=frase_sintagmada;
	t_frase_en_grupos *aux_frase_en_grupos=frase_en_grupos;
	int sintagma_final=0,sintagma_inicial;/*orde dos sintagmas primeiro e ultimo en cada grupo*/
	unsigned char comillas_abertas=0,guion_aberto=0;
	/* como o signo e o mesmo para abrir e pechar hai que distinguir se
		 o caso e o de abrir ou pechar.*/
	while ( aux_frase_sintagmada->tipo_sintagma!=0){
		sintagma_inicial=sintagma_final;

		if(((inicio_de_grupo(aux_frase_sintagmada->tipo_sintagma)) ||
					(aux_frase_sintagmada->tipo_sintagma==DOBLES_COMINNAS && comillas_abertas==0 ) ||
					(aux_frase_sintagmada->tipo_sintagma==GUION && guion_aberto==0) )
				&&  aux_frase_sintagmada->tipo_sintagma!=0 ){
			/* Se se trata dun nexo coordinativo ("e" OU" ou") ou unha locucion
				 que  comenza unha subordinada ou bipolar enton este elemento non
				 debe ser considerado como fin dun grupo se estamos empezando a
				 comprobalo como inicio de grupo.*/
			if (aux_frase_sintagmada->tipo_sintagma==DOBLES_COMINNAS && comillas_abertas==0 )
				comillas_abertas=1;
			if (aux_frase_sintagmada->tipo_sintagma==GUION && guion_aberto==0 )
				guion_aberto=1;
			sintagma_final++;
			aux_frase_sintagmada ++;
		}
		if(nexo_coordinativo(aux_frase_sintagmada->tipo_sintagma)){
			sintagma_final++;
			aux_frase_sintagmada ++;
		}
		/* Despois de calquer signo tamen se permite un nexo coordinativo e que
			 non indique o fin do grupo iniciado.
			 */
		while ( !nexo_ou_pausa(aux_frase_sintagmada->tipo_sintagma) &&
				(aux_frase_sintagmada+1)->tipo_sintagma!=0  ) {
			/*Deteccion de fin dun grupo se hai algo que nos indique que acaba este
				grupo, como sería unha coma, ou ben hai algo que nos indica que comenza
				outro, como poderia ser un nexo relativo.*/
			if (aux_frase_sintagmada->tipo_sintagma==DOBLES_COMINNAS &&
					comillas_abertas )
				comillas_abertas=0;
			if (aux_frase_sintagmada->tipo_sintagma==GUION &&
					guion_aberto )
				guion_aberto=0;
			sintagma_final++;
			aux_frase_sintagmada ++;
		}
		/*parase o encontrar un nexo,inicio de subordinante ou bipolar,ou nos
			signos que poden implicar pausa ou cambio de entonacion*/

		if (  inicio_de_grupo(aux_frase_sintagmada->tipo_sintagma) ||
				(aux_frase_sintagmada->tipo_sintagma==DOBLES_COMINNAS && comillas_abertas==0 ) ||
				(aux_frase_sintagmada->tipo_sintagma==GUION && guion_aberto==0 )  )
		{
			sintagma_final--;
			aux_frase_sintagmada--;
		}

		// FRAN_CAMPILLO: Añado la ruptura entonativa:

		while ( (aux_frase_sintagmada+1)->tipo_sintagma==PUNTO ||
				(aux_frase_sintagmada+1)->tipo_sintagma==PECHE_INTERROGACION  ||
				(aux_frase_sintagmada+1)->tipo_sintagma==PECHE_CORCHETE  ||
				(aux_frase_sintagmada+1)->tipo_sintagma==COMA  ||
				(aux_frase_sintagmada+1)->tipo_sintagma==RUPTURA_ENTONATIVA  ||
				(aux_frase_sintagmada+1)->tipo_sintagma==RUPTURA_COMA  ||
				(aux_frase_sintagmada+1)->tipo_sintagma==PUNTO_E_COMA )
			/*Se despois de detectar un nexo ou pausa ainda queda un destes signos
				(exemplo: se xogamos así, e, ante todo ...// en ,e, esta coma tamen
				pertence o grupo, non se acaba no nexo "e" o grupo) este signo agrupase
				co grupo anterior.*/
		{
			sintagma_final++;
			aux_frase_sintagmada++;
		}

		if (nexo((frase_sintagmada+sintagma_inicial)->tipo_sintagma))
			aux_frase_en_grupos->tipo_proposicion=clasifica_segun_caracter_de_pausa_e_entonacion((frase_sintagmada+sintagma_inicial)->tipo_sintagma);


		/* Agora enchemos os campos que indican que sintagmas abarca cada grupo*/
		/* Se hai varios signos ou nexos que poidan aportar informacion enton al-
			 macenanse nos arrays tipo_inicio ou tipo_final, polo orden en que aparecen*/

		aux_frase_en_grupos->tipo_inicio[0]=clasifica_segun_caracter_de_pausa_e_entonacion(((frase_sintagmada+sintagma_inicial)->tipo_sintagma));
		if(sintagma_inicial+1<sintagma_final)
			aux_frase_en_grupos->tipo_inicio[1]=clasifica_segun_caracter_de_pausa_e_entonacion(((frase_sintagmada+sintagma_inicial+1)->tipo_sintagma));

		if (sintagma_final>0 && nexo_ou_pausa((frase_sintagmada+sintagma_final-1)->tipo_sintagma))
		{
			aux_frase_en_grupos->tipo_fin[0]=clasifica_segun_caracter_de_pausa_e_entonacion((frase_sintagmada+sintagma_final-1)->tipo_sintagma);
			//#ifndef _MODO_NORMAL
			if ( (aux_frase_en_grupos->tipo_fin[0] == RUPTURA_ENTONATIVA) ||
					(aux_frase_en_grupos->tipo_fin[0] == RUPTURA_COMA) )
				aux_frase_en_grupos->tipo_fin[0] = 0;
			//#endif
			aux_frase_en_grupos->tipo_fin[1]=clasifica_segun_caracter_de_pausa_e_entonacion((frase_sintagmada+sintagma_final)->tipo_sintagma);
		}
		else{
			aux_frase_en_grupos->tipo_fin[0]=clasifica_segun_caracter_de_pausa_e_entonacion((frase_sintagmada+sintagma_final)->tipo_sintagma);
		}

		//fmendez
		if (aux_frase_sintagmada->tipo_sintagma==0) {
			aux_frase_sintagmada--;
			sintagma_final--;
		}
		//fmendez
		aux_frase_en_grupos->inicio=sintagma_inicial;
		aux_frase_en_grupos->fin=sintagma_final;
		aux_frase_en_grupos++;
		aux_frase_sintagmada++;
		sintagma_final++;
	}/* do while final*/
	return;
}

/**************************************************************************/
/**************************************************************************/
/*********          FUNCIONS DE ASIGNACION DE TONEMAS       ***************/
/**************************************************************************/
int signo_de_peche(unsigned char codigo){
	switch(codigo){
		case DOBLES_COMINNAS:
		case GUION:
		case PECHE_EXCLAMACION:
		case PECHE_INTERROGACION:
		case PECHE_PARENTESE:
		case PECHE_CORCHETE:
			return 1;
	}
	return 0;
}

/**************************************************************************/
int signo_de_apertura(unsigned char codigo) {
	switch(codigo){
		case DOBLES_COMINNAS:
		case GUION:
		case APERTURA_EXCLAMACION:
		case APERTURA_INTERROGACION:
		case APERTURA_PARENTESE:
		case APERTURA_CORCHETE:
			return 1;
	}
	return 0;
}
/**************************************************************************/

/** \fn   int Sintagma::ultimo_grupo(t_frase_en_grupos *a_frase_en_grupos,int cont)

 	Detecta se se trata do ultimo grupo que hai nunha frase.
*/

int Sintagma::ultimo_grupo(t_frase_en_grupos *a_frase_en_grupos,int cont){
	if (cont>LONX_MAX_FRASE_EN_GRUPOS) return 1;
	if (a_frase_en_grupos->inicio==0 && a_frase_en_grupos->fin==0 &&
			cont!=0) return 1;
	return 0;
}


/** 
 * \author fmendez
 * \brief Asigna o tipo de tonema que lle corresponde a un grupo delimitado 
 * por signos delimitadores do mesmo caracter: Interrogación, exclamación, 
 * entre guións...
 * 
 * \param tipo 
 * \param a_frase_en_grupos 
 * \param signo 
 */
void Sintagma::asignar_tonema_de_apertura_peche(char tipo,t_frase_en_grupos *a_frase_en_grupos,
		unsigned char signo){

	int indireccion;
	
	switch (signo) {
		case APERTURA_INTERROGACION:
		case PECHE_INTERROGACION:
			indireccion=a_frase_en_grupos->inicio;
			indireccion=frase_sintagmada[indireccion].inicio;
			switch(tipo) {
				case 1:
					if (  //frase_separada[ indireccion +1 ].cat_gramatical[0]==RELATIVO_TONICO
							//                          ||
							frase_separada[ indireccion +1 ].cat_gramatical[0]==RELA
							||
							//frase_separada[ indireccion +2 ].cat_gramatical[0]==RELATIVO_TONICO
							//                          ||
							frase_separada[ indireccion +2 ].cat_gramatical[0]==RELA)

						a_frase_en_grupos->tipo_tonema=INICIO_INTERROGACION_PARCIAL;
					else
						a_frase_en_grupos->tipo_tonema=INICIO_INTERROGACION_TOTAL;
					break;
				case 2:
					if (  //frase_separada[ indireccion +1].cat_gramatical[0]==RELATIVO_TONICO
							//                          ||
							frase_separada[ indireccion +1].cat_gramatical[0]==RELA )

						a_frase_en_grupos->tipo_tonema=FIN_INTERROGACION_PARCIAL;
					else {
						a_frase_en_grupos->tipo_tonema=FIN_INTERROGACION_TOTAL;
					}
					break;
				case 3:
					if (  //frase_separada[ indireccion +1].cat_gramatical[0]==RELATIVO_TONICO
							//                         ||
							frase_separada[ indireccion +1].cat_gramatical[0]==RELA
							||
							// frase_separada[ indireccion +2].cat_gramatical[0]==RELATIVO_TONICO
							//                           ||
							frase_separada[ indireccion +2].cat_gramatical[0]==RELA)

						a_frase_en_grupos->tipo_tonema=INTERROGACION_PARCIAL;
					else
						a_frase_en_grupos->tipo_tonema=INTERROGACION_TOTAL;

					break;
			}//switch(tipo)
			break;
		case APERTURA_EXCLAMACION:
		case PECHE_EXCLAMACION:
			switch(tipo) {
				case 1:
					a_frase_en_grupos->tipo_tonema=INICIO_EXCLAMACION;
					break;
				case 2:
					a_frase_en_grupos->tipo_tonema=FIN_EXCLAMACION;
					break;
				case 3:
					a_frase_en_grupos->tipo_tonema=EXCLAMACION;
					break;
			}
			break;
		case APERTURA_CORCHETE:
		case PECHE_CORCHETE:
			switch(tipo) {
				case 1:
					a_frase_en_grupos->tipo_tonema=INICIO_CORCHETE;
					break;
				case 2:
					a_frase_en_grupos->tipo_tonema=FIN_CORCHETE;
					break;
				case 3:
					a_frase_en_grupos->tipo_tonema=CORCHETE;
					break;
			}
			break;
		case APERTURA_PARENTESE:
		case PECHE_PARENTESE:
			switch(tipo) {
				case 1:
					a_frase_en_grupos->tipo_tonema=INICIO_PARENTESE;
					break;
				case 2:
					a_frase_en_grupos->tipo_tonema=FIN_PARENTESE;
					break;
				case 3:
					a_frase_en_grupos->tipo_tonema=PARENTESE;
					break;
			}
			break;
		case DOBLES_COMINNAS:
			switch(tipo) {
				case 1:
					a_frase_en_grupos->tipo_tonema=INICIO_ENTRE_COMINNAS;
					break;
				case 2:
					a_frase_en_grupos->tipo_tonema=FIN_ENTRE_COMINNAS;
					break;
				case 3:
					a_frase_en_grupos->tipo_tonema=ENTRE_COMINNAS;
					break;
			}
			break;
		case GUION:
			switch(tipo) {
				case 1:
					a_frase_en_grupos->tipo_tonema=INICIO_ENTRE_GUIONS;
					break;
				case 2:
					a_frase_en_grupos->tipo_tonema=FIN_ENTRE_GUIONS;
					break;
				case 3:
					a_frase_en_grupos->tipo_tonema=ENTRE_GUIONS;
					break;
			}
			break;
	}
}
/**************************************************************************/
/**************************************************************************/

/** \fn  int Sintagma::grupo_entre_comas_ou_nexo_e_ou(t_frase_en_grupos *a_frase_en_grupos)

 	Detecta se un grupo esta esta delimitado no principio ou fin por comas
	ou palabras que funcionan como nexos.
 */


int Sintagma::grupo_entre_comas_ou_nexo_e_ou(t_frase_en_grupos *a_frase_en_grupos)

{
	// FRAN_CAMPILLO: Añadí aquí la ruptura entonativa.
	if (  a_frase_en_grupos!= frase_en_grupos && (
				(a_frase_en_grupos-1)->tipo_fin[0]==COMA ||
				(a_frase_en_grupos-1)->tipo_fin[1]==COMA ||
				(a_frase_en_grupos-1)->tipo_fin[0]==RUPTURA_ENTONATIVA ||
				(a_frase_en_grupos-1)->tipo_fin[1]==RUPTURA_ENTONATIVA ||
				(a_frase_en_grupos-1)->tipo_fin[0]==RUPTURA_COMA ||
				(a_frase_en_grupos-1)->tipo_fin[1]==RUPTURA_COMA     )
			&&
			(
			 (a_frase_en_grupos)->tipo_fin[0]==COMA ||
			 (a_frase_en_grupos)->tipo_fin[1]==COMA ||
			 (a_frase_en_grupos)->tipo_fin[0]==RUPTURA_ENTONATIVA ||
			 (a_frase_en_grupos)->tipo_fin[1]==RUPTURA_ENTONATIVA || 
			 (a_frase_en_grupos)->tipo_fin[0]==RUPTURA_COMA ||
			 (a_frase_en_grupos)->tipo_fin[1]==RUPTURA_COMA ||
			 nexo_coordinativo((a_frase_en_grupos+1)->tipo_inicio[0])  )
		 )
		return 1;
	return 0;
}


/** 
 * \author fmendez
 * \remarks Os grupos entre comas poden pertencer a enumeracions "comeron pan,
 * leite, mazas e galletas" ou ben a incisos " Tu, ainda que veñas, non 
 * recibirás nada".  Por tanto hai que clasificar os grupos entre 
 * comas adecuadamente.
 * 
 * \param frase_en_grupos 
 */
void Sintagma::asignar_tonemas_en_enumeracions_ou_incisos(t_frase_en_grupos *frase_en_grupos){

	t_frase_en_grupos *a_frase_en_grupos=frase_en_grupos;
	int cont=0,i;
	int primeiro_grupo_enumerativo=0,ultimo_grupo_enumerativo=0;
	char grupo_enumerativo_empezado=0;


	while(!ultimo_grupo(a_frase_en_grupos,cont)) {
		if (grupo_entre_comas_ou_nexo_e_ou(a_frase_en_grupos)) { /*, ...,  ,... e,  ,...ou */
			if (primeiro_grupo_enumerativo==0) {
				primeiro_grupo_enumerativo=cont;
				ultimo_grupo_enumerativo=cont;
				if(nexo_coordinativo((a_frase_en_grupos+1)->tipo_inicio[0]) ) {
                	(frase_en_grupos+ultimo_grupo_enumerativo)->tipo_tonema=FIN_ENUMERATIVO;
					primeiro_grupo_enumerativo=0;
					grupo_enumerativo_empezado=0;
				}
				else
					grupo_enumerativo_empezado=1;
			}
			else {
				ultimo_grupo_enumerativo++;
				if(nexo_coordinativo((a_frase_en_grupos+1)->tipo_inicio[0]) ) {
					for(i=primeiro_grupo_enumerativo;i<ultimo_grupo_enumerativo;i++) {
                    	(frase_en_grupos+i)->tipo_tonema=ENUMERATIVO;
					}
                    (frase_en_grupos+ultimo_grupo_enumerativo)->tipo_tonema=FIN_ENUMERATIVO;
					primeiro_grupo_enumerativo=0;
					grupo_enumerativo_empezado=0;
				}
			}
		}
		else {
			if (grupo_enumerativo_empezado) {
				//grupo_enumerativo_empezado=0;
				if (primeiro_grupo_enumerativo==ultimo_grupo_enumerativo) {
					(frase_en_grupos+primeiro_grupo_enumerativo)->tipo_tonema=INCISO;
				}
				else {
					for(i=primeiro_grupo_enumerativo;i<=ultimo_grupo_enumerativo;i++) {
                    	(frase_en_grupos+i)->tipo_tonema=ENUMERATIVO;
					}
				}
				primeiro_grupo_enumerativo=0;
				grupo_enumerativo_empezado=0;

			}
		}
		a_frase_en_grupos++;
		cont++;
	}
	return;
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param frase_en_grupos 
 */
void Sintagma::asignacion_de_tonemas_final(t_frase_en_grupos *frase_en_grupos){

	int cont=0;
	t_frase_en_grupos *a_frase_en_grupos=frase_en_grupos;

	while(!ultimo_grupo(a_frase_en_grupos,cont)){

		if ( (a_frase_en_grupos->tipo_fin[0] == RUPTURA_ENTONATIVA) ||
				(a_frase_en_grupos->tipo_fin[1] == RUPTURA_ENTONATIVA) )
			a_frase_en_grupos->ruptura_entonativa = 1;

		if ( (a_frase_en_grupos->tipo_fin[0] == RUPTURA_COMA) ||
				(a_frase_en_grupos->tipo_fin[1] == RUPTURA_COMA) )
			a_frase_en_grupos->ruptura_entonativa = 2;

		if (signo_de_apertura(a_frase_en_grupos->tipo_inicio[0])){
			if (signo_de_peche(a_frase_en_grupos->tipo_fin[0]))
				asignar_tonema_de_apertura_peche(3,a_frase_en_grupos,
						a_frase_en_grupos->tipo_inicio[0]);

			else
				asignar_tonema_de_apertura_peche(1,a_frase_en_grupos,
						a_frase_en_grupos->tipo_inicio[0]);
		}
		else if (signo_de_peche(a_frase_en_grupos->tipo_fin[0]))
			asignar_tonema_de_apertura_peche(2,a_frase_en_grupos,a_frase_en_grupos->tipo_fin[0]);
		else if ( a_frase_en_grupos->tipo_fin[0]==PUNTOS_SUSPENSIVOS ||
				a_frase_en_grupos->tipo_fin[1]==PUNTOS_SUSPENSIVOS)
			a_frase_en_grupos->tipo_tonema=DE_SUSPENSE;
		else if (a_frase_en_grupos > frase_en_grupos)
			if ( (a_frase_en_grupos->tipo_fin[0] == COMA) ||
					(a_frase_en_grupos->tipo_fin[1] == COMA) ||
					(a_frase_en_grupos->tipo_fin[0] == RUPTURA_COMA) ||
					(a_frase_en_grupos->tipo_fin[1] == RUPTURA_COMA) ) {

				if ( ((a_frase_en_grupos - 1)->tipo_tonema == INICIO_INTERROGACION_TOTAL) ||
						((a_frase_en_grupos - 1)->tipo_tonema == INTERROGACION_TOTAL) )
					a_frase_en_grupos->tipo_tonema = FIN_INTERROGACION_TOTAL;
				else
					if ( ((a_frase_en_grupos - 1)->tipo_tonema == INICIO_EXCLAMACION) ||
							((a_frase_en_grupos - 1)->tipo_tonema == EXCLAMACION) )
						a_frase_en_grupos->tipo_tonema = FIN_EXCLAMACION;
			}

		a_frase_en_grupos++;
		cont++;
	}

	a_frase_en_grupos=frase_en_grupos;
	cont=0;
	while(!ultimo_grupo(a_frase_en_grupos,cont)){
		if (a_frase_en_grupos->tipo_tonema==0){
			/*
				 if ( a_frase_en_grupos->tipo_fin[0]==PUNTOS_SUSPENSIVOS ||
				 a_frase_en_grupos->tipo_fin[1]==PUNTOS_SUSPENSIVOS)
				 a_frase_en_grupos->tipo_tonema=DE_SUSPENSE;
				 */
			if (cont==0  && !ultimo_grupo(a_frase_en_grupos+1,cont+1))//comprobamos que a continuación non hai nada máis
				a_frase_en_grupos->tipo_tonema=INICIO_ENUNCIATIVA;
			if (cont==0  && ultimo_grupo(a_frase_en_grupos+1,cont+1))//comprobamos que a continuación non hai nada máis
				a_frase_en_grupos->tipo_tonema=ENUNCIATIVA;
			if (ultimo_grupo(a_frase_en_grupos+1,cont+1))//comprobamos que a continuación non hai nada máis
				a_frase_en_grupos->tipo_tonema=FIN_ENUNCIATIVA;
			if (a_frase_en_grupos->tipo_tonema==0) //por defecto
	            if ( (a_frase_en_grupos + 1)->tipo_tonema == FIN_INTERROGACION_TOTAL) {
                	if ( (a_frase_en_grupos->tipo_inicio[0] >= CONX_COOR) &&
                    	 (a_frase_en_grupos->tipo_inicio[0] <= CONTR_CONX_COOR_COP_ART_DET) )
	    	        	a_frase_en_grupos->tipo_tonema = INICIO_INTERROGACION_TOTAL;
                }
        	    else
		            if ( (a_frase_en_grupos + 1)->tipo_tonema == FIN_INTERROGACION_PARCIAL)
    		        	a_frase_en_grupos->tipo_tonema = INICIO_INTERROGACION_PARCIAL;
                    else
			            if ( (a_frase_en_grupos + 1)->tipo_tonema == FIN_INTERROGACION_TOTAL)
    		    	    	a_frase_en_grupos->tipo_tonema = INICIO_INTERROGACION_TOTAL;
	                    else
                        	if (a_frase_en_grupos->inicio != 0) {
    							if ( ((a_frase_en_grupos - 1)->tipo_tonema == INICIO_INTERROGACION_TOTAL) &&
                                	 (a_frase_en_grupos->tipo_fin[0] == RUPTURA_ENTONATIVA) )
                                	a_frase_en_grupos->tipo_tonema = FIN_INTERROGACION_TOTAL;
                                else
									a_frase_en_grupos->tipo_tonema=ENUNCIATIVA;
                            }
                            else
								a_frase_en_grupos->tipo_tonema=ENUNCIATIVA;
		}
		a_frase_en_grupos++;
		cont++;
	}
}


/**
 * \author fmendez
 * \brief Ata agora as subordinadas (empezan por "que" ou "o que" etc) 
 * formaron un grupo independiente para distinguilas como proposicions. 
 * En caso de que non haia explicitamente algo que a separe do grupo 
 * anterior, como un signo, enton na cadea falada pertencen ó grupo 
 * anterior e isto indicase poñendo a 1 o campo enlazado da estructura 
 * pertencente ó grupo anterior. 
 * 
 * \param a_frase_en_grupos 
 */
void Sintagma::unir_subordinadas_dependentes(t_frase_en_grupos *a_frase_en_grupos){
	int cont=0;

	while(!ultimo_grupo(a_frase_en_grupos,cont)){
		if ( a_frase_en_grupos->tipo_inicio[0]==NEXO_SUBORDINANTE)
			if ( a_frase_en_grupos > frase_en_grupos && sintagma_de_puntuacion((a_frase_en_grupos-1)->tipo_fin[1] ) )
				(a_frase_en_grupos-1)->enlazado=1;

		a_frase_en_grupos++;
		cont++;
	}
}

/** 
 * \author fmendez
 * 
 * \brief Función principal para agrupar os sintagmas en grupos. O grupo 
 * pode ser unha unidade entonativa ou ben unha unidade como proposición. 
 * Correspóndese normalmente co conxunto de palabras entre pausas.  
 * 
 * \param frase_en_grupos 
 */
void Sintagma::identificar_tonemas(t_frase_en_grupos *frase_en_grupos){

   unir_subordinadas_dependentes(frase_en_grupos);
   asignar_tonemas_en_enumeracions_ou_incisos(frase_en_grupos);
   asignacion_de_tonemas_final(frase_en_grupos);
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param categoria 
 * \param item 
 * 
 * \return 
 */
int elimina_categoria(unsigned char categoria,t_frase_separada *item){

	unsigned char *cat=item->cat_gramatical;
	unsigned char *cat_des=item->cat_gramatical_descartada;
	int i,j;

	for (i=0;i<N_CAT;i++) {
		if (*cat!=categoria) {
			cat++;
			continue;
		}
		for(j=0;j<N_CAT;j++) {
			if (*cat_des==0) break;
			cat_des++;
		}
		*cat_des=categoria;
		while(i<N_CAT-1) {
			*cat=*(cat+1);
			cat++;
			i++;
		}
		//*(cat+1)=0;
		break;
	}
	return 0;
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param categoria 
 * \param item 
 * 
 * \return 
 */
int asigna_categoria(unsigned char categoria,t_frase_separada *item){
	int i;
	unsigned char *cat=item->cat_gramatical;

	for(i=0;i<N_CAT;i++){
		if (*cat!=categoria) elimina_categoria(*cat,item);
		else cat++;
	}
	item->cat_gramatical[0]=categoria;
	return 0;
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param categoria 
 * \param item 
 * 
 * \return 
 */
int anade_categoria(unsigned char categoria,t_frase_separada *item){
	int i;

	for(i=0;i<N_CAT;i++)
		if (item->cat_gramatical[i]==categoria) return 0;

	for(i=N_CAT-1;i>0;i--)
		item->cat_gramatical[i]=item->cat_gramatical[i-1];
	item->cat_gramatical[0]=categoria;
	return 0;
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param categoria 
 * \param item 
 * 
 * \return 
 */
inline int unica_categoria(unsigned char categoria,t_frase_separada *item){

	return (!item->cat_gramatical[1] && item->cat_gramatical[0]==categoria);
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param item 
 * 
 * \return 
 */
int n_categorias(t_frase_separada *item){
	int i,j=0;

	for(i=0;i<N_CAT;i++)
		if (item->cat_gramatical[i]) j++;
	return j;
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param item 
 * \param comienzo 
 * 
 * \return 
 */
int reglas_desam(t_frase_separada *item,int comienzo) {
//   int i=comienzo;
   unsigned char k;
   char *pala;
   t_frase_separada *elem,*primero=item;
   elem=primero;
   elem+=comienzo;

if (!comienzo) {

//ZA: OJO con los valores de k, se corresponden a las constantes
//asociadas a todos los LOC_*
//       while(i<LONX_MAX_FRASE_SEPARADA) {
      while(*elem->pal) {

        for (k=LOC_ADVE;k<=LOC_ADVE_DUBI;k++)
              if (esta_palabra_ten_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
              else if (esta_palabra_tivera_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
        for (k=LOC_PREP;k<=LOC_PREP_CONDI;k++)
              if (esta_palabra_ten_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
              else if (esta_palabra_tivera_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
        for (k=LOC_CONX;k<=LOC_CONX_SUBOR_CORREL;k++)
              if (esta_palabra_ten_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
              else if (esta_palabra_tivera_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
        for (k=LOC_CONX_SUBOR_FINAL;k<=LOC_ADVE;k++)
              if (esta_palabra_ten_a_categoria_de(k,elem)){
                        elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }
              else if (esta_palabra_tivera_a_categoria_de(k,elem)){
                        //elimina_categoria(k,elem);
                        anade_categoria(k,elem);
              }

         elem++;
         //i++;
        }
}

if (comienzo==1) {
//   while(i<LONX_MAX_FRASE_SEPARADA) {
  while(*elem->pal) {
      if (elem->clase_pal == SIGNO_PUNTUACION ||
          elem->clase_pal == VALOR_DE_GRADOS ||
          elem->clase_pal == DATA ||
          elem->clase_pal == PORCENTUAL ||
          elem->clase_pal == HORAS ||
          elem->clase_pal == CADENAS_DE_OPERACIONS ||
          elem->clase_pal == NUMEROS_SEPARADOS_POR_GUIONS ||
          elem->clase_pal == INICIAL ||
          elem->clase_pal == ABREVIATURA ||
          elem->clase_pal == PALABRA_CONSONANTES_MINUSCULAS ||
          elem->clase_pal == CADENA_DE_SIGNOS) {
        elem++;
        //i++;
        continue;
      }
// FRAN_CAMPILLO: Provisional, mientras los diccionarios sigan hechos un cristo.
/*
    if ( (strcmp(elem->pal, "o") == 0) || (strcmp(elem->pal, "que")== 0) ||
    	 (strcmp(elem->pal, "miña") == 0) || (strcmp(elem->pal, "súa")== 0) ||
    	 (strcmp(elem->pal, "súas")== 0) )
        if ( (esta_palabra_ten_a_categoria_de(NOME, elem)) &&
              !unica_categoria(NOME, elem) )
            elimina_categoria(NOME, elem);
*/

// FRAN_CAMPILLO: Hasta aquí.

      if ((elem-1)->pal_transformada) pala=(elem-1)->pal_transformada;
      else pala=(elem-1)->pal;

      if (strlen(pala)>3 && (
         !strcmp("-lo",&pala[strlen(pala)-3]) ||
         !strcmp(&pala[strlen(pala)-3],"-la"))) {
         asigna_categoria(VERBO,elem-1);
         if (!unica_categoria(VERBO,elem) && esta_palabra_ten_a_categoria_de(VERBO,elem))
            elimina_categoria(VERBO,elem);
      }
      if (strlen(pala)>4 && (
         !strcmp("-los",&pala[strlen(pala)-3]) ||
         !strcmp(&pala[strlen(pala)-3],"-las"))) {
         asigna_categoria(VERBO,elem-1);
         if (!unica_categoria(VERBO,elem) && esta_palabra_ten_a_categoria_de(VERBO,elem))
            elimina_categoria(VERBO,elem);
      }

	  if (esta_palabra_ten_a_categoria_de(ART_DET, elem) &&
      	  esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) &&
          !unica_categoria(ART_INDET, elem - 1))
          elimina_categoria(ART_INDET, elem - 1);

	  if (esta_palabra_ten_a_categoria_de(INFINITIVO, elem)) {

      	  if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1)) {

              if ( (strcmp((elem - 1)->lema, "do") == 0) ||
                   (strcmp((elem - 1)->lema, "co") == 0) ||
                   (strcmp((elem - 1)->lema, "polo") == 0) )
                  asigna_categoria(NOME, elem);
              else
                  if (!esta_palabra_ten_a_categoria_de(NOME, elem))
	                  asigna_categoria(VERBO, elem);
          } // 	  if (esta_palabra_ten_a_categoria_de(INFINITIVO, elem) &&
          else
          	if (esta_palabra_ten_a_categoria_de(ADVE_NEGA, elem - 1))
            	asigna_categoria(VERBO, elem);
          else
          	if ( ((strcmp((elem - 1)->lema, "a") == 0) ||
				  (strcmp((elem - 1)->lema, "de") == 0) ) &&
				!esta_palabra_ten_a_categoria_de(NOME, elem) )
                asigna_categoria(VERBO, elem);
          else
          	if (strcmp((elem - 1)->lema, "que") == 0)
                asigna_categoria(VERBO, elem);
          else
          	if (((elem - 1)->clase_pal == SIGNO_PUNTUACION) &&
            	 !esta_palabra_ten_a_categoria_de(NOME, elem))
            	asigna_categoria(VERBO, elem);

	  } // if (esta_palabra_ten_a_categoria_de(INFINITIVO, elem) {

      if (unica_categoria(VERBO, elem)) {

		  if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) &&
          	  !unica_categoria(CONTR_PREP_ART_DET, elem - 1))
          		elimina_categoria(CONTR_PREP_ART_DET, elem - 1);
          if (esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) &&
          	  !unica_categoria(ART_DET, elem - 1) )
		  		elimina_categoria(ART_DET, elem - 1);

     } // if (unica_categoria(VERBO, elem)

	 if ( (esta_palabra_ten_a_categoria_de(ART_DET, elem) ||
     	   esta_palabra_ten_a_categoria_de(ART_INDET, elem) ) &&
           esta_palabra_ten_a_categoria_de(INDEF, elem - 1) &&
           ((elem - 1)->numero == elem->numero) )
           asigna_categoria(INDEF_DET, elem - 1);

	 if ( (elem + 1)->pal) {
		 if ( (elem > item + 1) && ((elem + 1)->clase_pal == SIGNO_PUNTUACION) )
		     if (esta_palabra_ten_a_categoria_de(PREP, elem - 1) &&
	         	 ( esta_palabra_ten_a_categoria_de(PUNTO, elem + 1) ||
	    	           esta_palabra_ten_a_categoria_de(DOUS_PUNTOS, elem + 1) ||
	               esta_palabra_ten_a_categoria_de(COMA, elem + 1) ||
	               esta_palabra_ten_a_categoria_de(RUPTURA_ENTONATIVA, elem + 1) ||
	               esta_palabra_ten_a_categoria_de(RUPTURA_COMA, elem + 1) ||
				   esta_palabra_ten_a_categoria_de(PUNTOS_SUSPENSIVOS, elem + 1) ||
	    	           esta_palabra_ten_a_categoria_de(PUNTO_E_COMA, elem + 1) ||
                       esta_palabra_ten_a_categoria_de(PECHE_INTERROGACION, elem + 1) ||
                       esta_palabra_ten_a_categoria_de(PECHE_EXCLAMACION, elem + 1) ) &&
	               esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) )
	    	           asigna_categoria(NUME_CARDI_PRON, elem);

         if (esta_palabra_ten_a_categoria_de(APERTURA_INTERROGACION, elem - 1))
         	if (strcmp((elem + 1)->lema, "que") == 0)
            	if ( (strcmp(elem->lema, "por") == 0) ||
                	 (strcmp(elem->lema, "en") == 0) ||
                     (strcmp(elem->lema, "de") == 0) )
                	asigna_categoria(INTER, elem + 1);

        if (unica_categoria(PREP, elem - 1) ||
        	unica_categoria(VERBO, elem - 1) ||
            unica_categoria(CONX_COOR_COPU, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem -1 ) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem - 1)) {


                if (unica_categoria(PREP, elem + 1) ||
                    unica_categoria(VERBO, elem + 1) ||
                    unica_categoria(CONX_COOR_COPU, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem -1 ) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(PUNTO, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(PUNTO_E_COMA, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(DOUS_PUNTOS, elem + 1) ||
                        esta_palabra_ten_a_categoria_de(COMA, elem + 1) ) {


                    if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
                        elimina_categoria(INDEF, elem);
                        anade_categoria(INDEF_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(INDEF_DET, elem)) {
                        elimina_categoria(INDEF_DET, elem);
                        anade_categoria(INDEF_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(POSE, elem)) {
                        elimina_categoria(POSE, elem);
                        anade_categoria(POSE_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(POSE_DET, elem)) {
                        elimina_categoria(POSE_DET, elem);
                        anade_categoria(POSE_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(DEMO, elem)) {
                        elimina_categoria(DEMO, elem);
                        anade_categoria(DEMO_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(DEMO_DET, elem)) {
                        elimina_categoria(DEMO_DET, elem);
                        anade_categoria(DEMO_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem)) {
                        elimina_categoria(NUME_CARDI, elem);
                        anade_categoria(NUME_CARDI_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(NUME_CARDI_DET, elem)) {
                        elimina_categoria(NUME_CARDI_DET, elem);
                        anade_categoria(NUME_CARDI_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(NUME_ORDI, elem)) {
                        elimina_categoria(NUME_ORDI, elem);
                        anade_categoria(NUME_ORDI_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(NUME_ORDI_DET, elem)) {
                        elimina_categoria(NUME_ORDI_DET, elem);
                        anade_categoria(NUME_ORDI_PRON, elem);
                    }


                    if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem) &&
                        !unica_categoria(CONTR_PREP_ART_INDET, elem) )
                        elimina_categoria(CONTR_PREP_ART_INDET, elem);

                    if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem)) {
                        elimina_categoria(CONTR_PREP_INDEF, elem);
                        anade_categoria(CONTR_PREP_INDEF_PRON, elem);
                    }

                    if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem)) {
                        elimina_categoria(CONTR_PREP_DEMO, elem);
                        anade_categoria(CONTR_PREP_DEMO_PRON, elem);
                    }


				}

		}

  	} // if ( (elem + 1)->pal)

	if (unica_categoria(PREP, elem) ||
        	esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem) ||
         	esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem) ||
    	 	esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem)) {


		if (esta_palabra_ten_a_categoria_de(PRON_PERS_AT_ACUS, elem - 1) &&
        	!esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) &&
            !esta_palabra_ten_a_categoria_de(NOME, elem) &&
            !esta_palabra_tivera_a_categoria_de(NOME, elem))
        	asigna_categoria(PRON_PERS_AT_ACUS, elem - 1);
            
        if (esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) &&
        	!unica_categoria(ART_DET, elem - 1) &&
            !esta_palabra_ten_a_categoria_de(NOME, elem) &&
            !esta_palabra_tivera_a_categoria_de(NOME, elem))
            elimina_categoria(ART_DET, elem - 1);
            
        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem - 1) &&
        	!unica_categoria(CONTR_PREP_ART_INDET, elem - 1) )
            elimina_categoria(CONTR_PREP_ART_INDET, elem - 1);

        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1)) {
			elimina_categoria(CONTR_PREP_DEMO, elem - 1);
            anade_categoria(CONTR_PREP_DEMO_PRON, elem - 1);
    	}

        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem - 1)) {
			elimina_categoria(CONTR_PREP_INDEF, elem - 1);
            anade_categoria(CONTR_PREP_INDEF_PRON, elem - 1);
    	}

        if (esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) &&
        	!unica_categoria(ART_INDET, elem - 1) )
            elimina_categoria(ART_INDET, elem - 1);
        else
        	if (elem > item + 1)
	            if (!esta_palabra_ten_a_categoria_de(NOME, elem - 2))
	            	if (esta_palabra_ten_a_categoria_de(INDEF, elem - 1))
	                	asigna_categoria(INDEF_PRON, elem - 1);
    } // if ( (unica_categoria(PREP, elem) ||

	if (unica_categoria(VERBO, elem)){

        if (esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) &&
        	!unica_categoria(ART_DET, elem - 1) &&
            !esta_palabra_ten_a_categoria_de(NOME, elem) &&
            !esta_palabra_tivera_a_categoria_de(NOME, elem))
            elimina_categoria(ART_DET, elem - 1);

        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem - 1) &&
        	!unica_categoria(CONTR_PREP_ART_INDET, elem - 1) )
            elimina_categoria(CONTR_PREP_ART_INDET, elem - 1);

        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1)) {
			elimina_categoria(CONTR_PREP_DEMO, elem - 1);
            anade_categoria(CONTR_PREP_DEMO_PRON, elem - 1);
    	}

        if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem - 1)) {
			elimina_categoria(CONTR_PREP_INDEF, elem - 1);
            anade_categoria(CONTR_PREP_INDEF_PRON, elem - 1);
    	}

        if (esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) &&
        	!unica_categoria(ART_INDET, elem - 1) )
            elimina_categoria(ART_INDET, elem - 1);
        else
        	if (elem > item + 1)
	            if (!esta_palabra_ten_a_categoria_de(NOME, elem - 2))
	            	if (esta_palabra_ten_a_categoria_de(INDEF, elem - 1))
	                	asigna_categoria(INDEF_PRON, elem - 1);
    } // if ( (unica_categoria(VERBO, elem) 


	if (esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) &&
    	!unica_categoria(ART_INDET, elem - 1) )
		if (!esta_palabra_ten_a_categoria_de(INDEF, elem) &&
        	!esta_palabra_ten_a_categoria_de(NOME, elem) &&
        	 ( esta_palabra_ten_a_categoria_de(ADVE_AFIRM, elem) ||
        	  esta_palabra_ten_a_categoria_de(ADVE_NEGA, elem) ))

            elimina_categoria(ART_INDET, elem - 1);



    if ((elem + 1)->pal) {

        if ( ((elem - 1)->clase_pal == SIGNO_PUNTUACION) &&
             (esta_palabra_ten_a_categoria_de(PUNTO, elem - 1) ||
              esta_palabra_ten_a_categoria_de(DOUS_PUNTOS, elem - 1) ||
			  esta_palabra_ten_a_categoria_de(RUPTURA_ENTONATIVA, elem - 1) ||
			  esta_palabra_ten_a_categoria_de(RUPTURA_COMA, elem - 1) ||
              esta_palabra_ten_a_categoria_de(COMA, elem - 1) ||
              esta_palabra_ten_a_categoria_de(PUNTOS_SUSPENSIVOS, elem - 1) ||
              esta_palabra_ten_a_categoria_de(PUNTO_E_COMA, elem - 1) ) ) {


            if (unica_categoria(VERBO, elem + 1) ||
            	(((elem + 1)->cat_gramatical[1] == '\0') &&
            	 ((elem + 1)->cat_gramatical[0] >= CONX_COOR) &&
                 ((elem + 1)->cat_gramatical[0] <= LOC_CONX_SUBOR_CORREL))) {
            
                if (esta_palabra_ten_a_categoria_de(ART_INDET, elem) &&
                    !unica_categoria(ART_INDET, elem) )
                    elimina_categoria(ART_INDET, elem);

                if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) ) {
                    elimina_categoria(NUME_CARDI, elem);
                    anade_categoria(NUME_CARDI_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(NUME_ORDI, elem) ) {
                    elimina_categoria(NUME_ORDI, elem);
                    anade_categoria(NUME_ORDI_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(DEMO, elem) ) {
                    elimina_categoria(DEMO, elem);
                    anade_categoria(DEMO_PRON, elem);
                }


            } // if (unica_categoria(VERBO, elem + 1)) {

            if (unica_categoria(VERBO, elem + 1) ||
            	unica_categoria(PREP, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem + 1)) {


                if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
                    elimina_categoria(INDEF, elem);
                    anade_categoria(INDEF_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(POSE, elem)) {
                    elimina_categoria(POSE, elem);
                    anade_categoria(POSE_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(DEMO, elem)) {
                    elimina_categoria(DEMO, elem);
                    anade_categoria(DEMO_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem)) {
                    elimina_categoria(NUME_CARDI, elem);
                    anade_categoria(NUME_CARDI_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem) &&
                    !unica_categoria(CONTR_PREP_ART_INDET, elem) )
                    elimina_categoria(CONTR_PREP_ART_INDET, elem);

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem)) {
                    elimina_categoria(CONTR_PREP_INDEF, elem);
                    anade_categoria(CONTR_PREP_INDEF_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem)) {
                    elimina_categoria(CONTR_PREP_DEMO, elem);
                    anade_categoria(CONTR_PREP_DEMO_PRON, elem);
                }

            }  // if (unica_categoria(PREP, elem + 1) ||

		} //   	if ( ((elem - 1)->clase_pal == SIGNO_PUNTUACION) &&

        if (!esta_palabra_ten_a_categoria_de(NOME, elem - 1) &&
        	!esta_palabra_ten_a_categoria_de(NOME_PROPIO, elem - 1) &&
            !esta_palabra_ten_a_categoria_de(ADXECTIVO, elem - 1) &&
            !esta_palabra_ten_a_categoria_de(INFINITIVO, elem - 1) ) {

            if (unica_categoria(VERBO, elem + 1) ||
            	unica_categoria(PREP, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem + 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem + 1)) {


                if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
                    elimina_categoria(INDEF, elem);
                    anade_categoria(INDEF_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(POSE, elem)) {
                    elimina_categoria(POSE, elem);
                    anade_categoria(POSE_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(DEMO, elem)) {
                    elimina_categoria(DEMO, elem);
                    anade_categoria(DEMO_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem)) {
                    elimina_categoria(NUME_CARDI, elem);
                    anade_categoria(NUME_CARDI_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(NUME_ORDI, elem)) {
                    elimina_categoria(NUME_ORDI, elem);
                    anade_categoria(NUME_ORDI_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem) &&
                    !unica_categoria(CONTR_PREP_ART_INDET, elem) )
                    elimina_categoria(CONTR_PREP_ART_INDET, elem);

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem)) {
                    elimina_categoria(CONTR_PREP_INDEF, elem);
                    anade_categoria(CONTR_PREP_INDEF_PRON, elem);
                }

                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem)) {
                    elimina_categoria(CONTR_PREP_DEMO, elem);
                    anade_categoria(CONTR_PREP_DEMO_PRON, elem);
                }

            }  // if (unica_categoria(PREP, elem + 1) ||

		} // if (!esta_palabra_ten_a_categoria_de(NOME, elem - 1) &&


        if (esta_palabra_ten_a_categoria_de(ART_INDET, elem))
                if ( esta_palabra_ten_a_categoria_de(NUME_CARDI, elem + 1) ||
                     esta_palabra_ten_a_categoria_de(NUME_CARDI_DET, elem + 1) ||
                     esta_palabra_ten_a_categoria_de(NUME_CARDI_PRON, elem + 1) ) {
                        if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
                                elimina_categoria(INDEF, elem);
                                anade_categoria(INDEF_DET, elem);
                        }
                        else
                          if (esta_palabra_ten_a_categoria_de(INDEF_PRON, elem))
                                  elimina_categoria(INDEF_PRON, elem);
                }


        if ( ((elem + 1)->clase_pal == SIGNO_PUNTUACION) &&
             (esta_palabra_ten_a_categoria_de(PUNTO, elem + 1) ||
              esta_palabra_ten_a_categoria_de(DOUS_PUNTOS, elem + 1) ||
              esta_palabra_ten_a_categoria_de(COMA, elem + 1) ||
              esta_palabra_ten_a_categoria_de(RUPTURA_ENTONATIVA, elem + 1) ||
              esta_palabra_ten_a_categoria_de(RUPTURA_COMA, elem + 1) ||
              esta_palabra_ten_a_categoria_de(PUNTOS_SUSPENSIVOS, elem + 1) ||
              esta_palabra_ten_a_categoria_de(PUNTO_E_COMA, elem + 1) ) ) {


            if (esta_palabra_ten_a_categoria_de(PREP, elem) &&
            	!unica_categoria(PREP, elem) &&
                !esta_palabra_ten_a_categoria_de(COMA, elem + 1) )
                elimina_categoria(PREP, elem);
                
            if (unica_categoria(PREP, elem - 1) ||
            	unica_categoria(VERBO, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET, elem - 1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON, elem - 1) ) {


            	if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem)) {
                	elimina_categoria(NUME_CARDI, elem);
                    anade_categoria(NUME_CARDI_PRON, elem);
                    if (elem->xenero == NO_ASIGNADO)
                    	elem->xenero = (elem - 1)->xenero;
                    if (elem->numero == NO_ASIGNADO)
                    	elem->numero = (elem - 1)->numero;
                }

            	if (esta_palabra_ten_a_categoria_de(INDEF_DET, elem)) {
                	elimina_categoria(INDEF_DET, elem);
                    anade_categoria(INDEF_PRON, elem);
                }


				if (esta_palabra_ten_a_categoria_de(NUME_ORDI_DET, elem)) {
					elimina_categoria(NUME_ORDI_DET, elem);
					anade_categoria(NUME_ORDI_PRON, elem);
                    if (elem->xenero == NO_ASIGNADO)
                    	elem->xenero = (elem - 1)->xenero;
                    if (elem->numero == NO_ASIGNADO)
                    	elem->numero = (elem - 1)->numero;
                }

				if (esta_palabra_ten_a_categoria_de(NUME_ORDI, elem)) {
					elimina_categoria(NUME_ORDI, elem);
					anade_categoria(NUME_ORDI_PRON, elem);
                    if (elem->xenero == NO_ASIGNADO)
                    	elem->xenero = (elem - 1)->xenero;
                    if (elem->numero == NO_ASIGNADO)
                    	elem->numero = (elem - 1)->numero;
                }

				if (esta_palabra_ten_a_categoria_de(DEMO, elem)) {
					elimina_categoria(DEMO, elem);
					anade_categoria(DEMO_PRON, elem);
                }

				if (esta_palabra_ten_a_categoria_de(POSE, elem)) {
					elimina_categoria(POSE, elem);
					anade_categoria(POSE_PRON, elem);
                }

				if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
					elimina_categoria(INDEF, elem);
					anade_categoria(INDEF_PRON, elem);
                    if (elem->xenero == NO_ASIGNADO)
                    	elem->xenero = (elem - 1)->xenero;
                    if (elem->numero == NO_ASIGNADO)
                    	elem->numero = (elem - 1)->numero;
                }


			} // if (unica_categoria(PREP, elem + 1) || ...

            if (esta_palabra_ten_a_categoria_de(VERBO, elem - 1) &&
                 unica_categoria(VERBO, elem - 1)) {

            	unsigned char *apunta_categoria_verbal = (elem - 1)->cat_verbal;
                char persona;

            	if (esta_palabra_ten_a_categoria_de(NUME_CARDI, elem)) {
                	elimina_categoria(NUME_CARDI, elem);
                    anade_categoria(NUME_CARDI_PRON, elem);
                    if (elem->numero == NO_ASIGNADO)
                        do {
                        	persona = (char) ((*apunta_categoria_verbal - 1) % 6);
							if (persona < 3)
		                    	elem->numero = SINGULAR;
                            else
                            	elem->numero = PLURAL;
                        } while (*(++apunta_categoria_verbal) != 0);
                }

				if (esta_palabra_ten_a_categoria_de(NUME_ORDI, elem)) {
					elimina_categoria(NUME_ORDI, elem);
					anade_categoria(NUME_ORDI_PRON, elem);
                    if (elem->numero == NO_ASIGNADO)
                        do {
                        	persona = (char) ((*apunta_categoria_verbal - 1) % 6);
							if (persona < 3)
		                    	elem->numero = SINGULAR;
                            else
                            	elem->numero = PLURAL;
                        } while (*(++apunta_categoria_verbal) != 0);
                }

				if (esta_palabra_ten_a_categoria_de(DEMO, elem)) {
					elimina_categoria(DEMO, elem);
					anade_categoria(DEMO_PRON, elem);
                }

				if (esta_palabra_ten_a_categoria_de(POSE, elem)) {
					elimina_categoria(POSE, elem);
					anade_categoria(POSE_PRON, elem);
                }

				if (esta_palabra_ten_a_categoria_de(INDEF, elem)) {
					elimina_categoria(INDEF, elem);
					anade_categoria(INDEF_PRON, elem);
                    if (elem->numero == NO_ASIGNADO)
                        do {
                        	persona = (char) ((*apunta_categoria_verbal - 1) % 6);
							if (persona < 3)
		                    	elem->numero = SINGULAR;
                            else
                            	elem->numero = PLURAL;
                        } while (*(++apunta_categoria_verbal) != 0);
                }

				if (esta_palabra_ten_a_categoria_de(INDEF_DET, elem)) {
					elimina_categoria(INDEF_DET, elem);
					anade_categoria(INDEF_PRON, elem);
                    if (elem->numero == NO_ASIGNADO)
                        do {
                        	persona = (char) ((*apunta_categoria_verbal - 1) % 6);
							if (persona < 3)
		                    	elem->numero = SINGULAR;
                            else
                            	elem->numero = PLURAL;
                        } while (*(++apunta_categoria_verbal) != 0);
                }

			} // if (esta_palabra_ten_a_categoria_de(VERBO, elem - 1)...

            if (esta_palabra_ten_a_categoria_de(POSE, elem) &&
            	esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) )
                asigna_categoria(POSE_PRON, elem);

            if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem) &&
                esta_palabra_tivera_a_categoria_de(NOME, elem) )
                    asigna_categoria(NOME, elem);


            if (esta_palabra_ten_a_categoria_de(ART_INDET, elem) &&
                !unica_categoria(ART_INDET, elem) )
                    elimina_categoria(ART_INDET, elem);

            if (esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) &&
                esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) ) {

                       asigna_categoria(NUME_CARDI_PRON, elem);
                       asigna_categoria(ART_INDET, elem - 1);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;
                   }
             else
                 if (esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) &&
                     esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) ) {

                       asigna_categoria(NUME_CARDI_PRON, elem);
                       asigna_categoria(ART_DET, elem - 1);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;
                   }
             else
                 if (esta_palabra_ten_a_categoria_de(INDEF, elem - 1) &&
                     esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) ) {

                       asigna_categoria(NUME_CARDI_PRON, elem);
                       asigna_categoria(INDEF_DET, elem - 1);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;
                   }
             else
                 if (esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) &&
                     esta_palabra_ten_a_categoria_de(NUME_CARDI, elem) ) {

                       asigna_categoria(NUME_CARDI_PRON, elem);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;
                 }
             else
                if (esta_palabra_ten_a_categoria_de(INDEF, elem) &&
                     (  esta_palabra_ten_a_categoria_de(ART_DET, elem - 1) ||
                        esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1) )) {

                       asigna_categoria(INDEF_PRON, elem);

                       if (unica_categoria(ART_DET, elem - 1) ||
                           unica_categoria(ART_INDET, elem - 1) ) {
                            elem->xenero = (elem - 1)->xenero;
                            elem->numero = (elem - 1)->numero;
                       } // if (unica_categoria...

                }
             else
                if (esta_palabra_ten_a_categoria_de(INDEF, elem) &&
                     (  esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET, elem - 1) ||
                        esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET, elem - 1) )) {

                       asigna_categoria(INDEF_PRON, elem);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;


                }
             else
                if (esta_palabra_ten_a_categoria_de(INDEF, elem) &&
                    esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem - 1) ) {

                       asigna_categoria(INDEF_PRON, elem);
                       asigna_categoria(CONTR_PREP_INDEF_DET, elem - 1);
                       elem->xenero = (elem - 1)->xenero;
                       elem->numero = (elem - 1)->numero;
                }
             else
                if (esta_palabra_ten_a_categoria_de(INDEF, elem) &&
                    ( (esta_palabra_ten_a_categoria_de(VERBO, elem - 1) && unica_categoria(VERBO, elem - 1)) ||
                     esta_palabra_ten_a_categoria_de(PREP, elem -1) ) ) {

                       asigna_categoria(INDEF_PRON, elem);
                     }
             else
                if (esta_palabra_ten_a_categoria_de(DEMO, elem) &&
                    (esta_palabra_ten_a_categoria_de(VERBO, elem - 1) ||
                     esta_palabra_ten_a_categoria_de(PREP, elem - 1)) )

                        asigna_categoria(DEMO_PRON, elem);
            else
                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO, elem))
                    asigna_categoria(CONTR_PREP_DEMO_PRON, elem);
            else
                if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF, elem))
                    asigna_categoria(CONTR_PREP_INDEF_PRON, elem);
            else
                if (esta_palabra_ten_a_categoria_de(POSE, elem) &&
                    unica_categoria(VERBO, elem - 1) )
                    asigna_categoria(POSE_PRON, elem);


         } // if ((elem + 1)->clase_pal == SIGNO_PUNTUACION)

    } // if ((elem + 1)->pal
    
/*
      if (  ((elem->clase_pal==PALABRA_NORMAL_EMPEZA_MAY) ||
            (elem->clase_pal==PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)) &&
            elem != primero &&
            ((elem-1)->clase_pal != SIGNO_PUNTUACION) &&
            ! unica_categoria(DOBLES_COMINNAS,elem-1))  {
         asigna_categoria(NOME_PROPIO,elem);
         //continue;
      }
*/
      if (!unica_categoria(INTER,elem) &&
          esta_palabra_ten_a_categoria_de(INTER,elem)){
         if (esta_palabra_ten_a_categoria_de(APERTURA_INTERROGACION,elem-1))
             asigna_categoria(INTER,elem);
         else  elimina_categoria(INTER,elem);
         //continue;
      }
      if (esta_palabra_ten_a_categoria_de(EXCLA,elem)){
         if (esta_palabra_ten_a_categoria_de(APERTURA_EXCLAMACION,elem-1))
             asigna_categoria(EXCLA,elem);
         else  elimina_categoria(EXCLA,elem);
         //continue;
      }
      if (!esta_palabra_ten_a_categoria_de(VERBO,elem-1) &&
           esta_palabra_ten_a_categoria_de(PARTICIPIO,elem) &&
           (elem->cat_verbal[1] == '\0') ) {
           elimina_categoria(VERBO,elem);
           //elimina_categoria(PARTICIPIO,elem);
           anade_categoria(ADXECTIVO,elem);
//           anade_categoria(NOME,elem);
           //continue;
      }

      if (!esta_palabra_ten_a_categoria_de(VERBO,elem) &&
      	  !esta_palabra_ten_a_categoria_de(INFINITIVO,elem)	
          /*&& ( (elem - 1)->cat_gramatical[1] != 0) */ ) {
         if (esta_palabra_ten_a_categoria_de(PRON_PERS_AT,elem-1) &&
         	 !unica_categoria(PRON_PERS_AT, elem - 1)){
            elimina_categoria(PRON_PERS_AT,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(PRON_PERS_AT_REFLEX,elem-1) &&
         	 !unica_categoria(PRON_PERS_AT_REFLEX, elem - 1)){
            elimina_categoria(PRON_PERS_AT_REFLEX,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(PRON_PERS_AT_ACUS,elem-1) &&
         	 !unica_categoria(PRON_PERS_AT_ACUS, elem - 1) ){
            elimina_categoria(PRON_PERS_AT_ACUS,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(PRON_PERS_AT_DAT,elem-1) &&
         	 !unica_categoria(PRON_PERS_AT_DAT, elem - 1) ){
            elimina_categoria(PRON_PERS_AT_DAT,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PRON_PERS_AT_DAT_AC,elem-1) &&
         	!unica_categoria(CONTR_PRON_PERS_AT_DAT_AC, elem - 1) ) {
            elimina_categoria(CONTR_PRON_PERS_AT_DAT_AC,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PRON_PERS_AT_DAT_DAT_AC,elem-1) &&
          	 !unica_categoria(CONTR_PRON_PERS_AT_DAT_DAT_AC, elem - 1) ) {
            elimina_categoria(CONTR_PRON_PERS_AT_DAT_DAT_AC,elem-1);
         }
      }
      else  if (esta_palabra_ten_a_categoria_de(VERBO,elem) && !unica_categoria(VERBO, elem)) {//elem tiene la cat de verbo
         if ( elem != primero &&
              (elem->clase_pal==PALABRA_NORMAL_EMPEZA_MAY ||
               elem->clase_pal==PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA) &&
              esta_palabra_ten_a_categoria_de(NOME,elem) ) {
            elimina_categoria(VERBO,elem);
         }
         else if (unica_categoria(POSE,elem-1) ||
               unica_categoria(POSE_DISTR,elem-1)) {
            elimina_categoria(VERBO,elem);
         }
         else if (unica_categoria(PREP,elem-1) &&
             !esta_palabra_ten_a_categoria_de(INFINITIVO,elem) &&
            !esta_palabra_ten_a_categoria_de(PARTICIPIO,elem)) {
            elimina_categoria(VERBO,elem);
          }
      }
      /*este bloque casi se puede quitar
      if (esta_palabra_ten_a_categoria_de(VERBO,elem) && !unica_categoria(VERBO, elem)){
          if (unica_categoria(PRON_PERS_AT,elem-1)) {
             asigna_categoria(VERBO,elem);
          }
          else if (unica_categoria(PRON_PERS_AT_REFLEX,elem-1)) {
             asigna_categoria(VERBO,elem);
          }
          else if (unica_categoria(PRON_PERS_AT_ACUS,elem-1)) {
             asigna_categoria(VERBO,elem);
          }
          else if (unica_categoria(PRON_PERS_AT_DAT,elem-1)) {
             asigna_categoria(VERBO,elem);
          }
          else if (unica_categoria(CONTR_PRON_PERS_AT_DAT_AC,elem-1)) {
             asigna_categoria(VERBO,elem);
          }
      } */
      
#ifndef _WIN32
      if(!strcasecmp((elem-1)->pal,"non") &&
#else
      if (!stricmp((elem-1)->pal,"non") &&
#endif
         esta_palabra_ten_a_categoria_de(NOME,elem) &&
         !unica_categoria(NOME,elem)) {
         elimina_categoria(NOME,elem);
         //continue;
      }
      if (esta_palabra_ten_a_categoria_de(VERBO,elem-1) &&
         !strcmp((elem-1)->inf_verbal->infinitivo,"ser") &&
         esta_palabra_ten_a_categoria_de(VERBO,elem) &&
         esta_palabra_ten_a_categoria_de(NOME,elem)) {
         asigna_categoria(NOME,elem);
         //continue;
      }
      if ((unica_categoria(ART_DET,elem-1) ||
         unica_categoria(CONTR_PREP_ART_DET,elem-1)) &&
         !unica_categoria(VERBO, elem) &&
         esta_palabra_ten_a_categoria_de(VERBO,elem) &&
         !esta_palabra_ten_a_categoria_de(INFINITIVO,elem) &&
         !esta_palabra_ten_a_categoria_de(PARTICIPIO,elem)) {
         elimina_categoria(VERBO,elem);
         //continue;
      }

#ifndef _WIN32
      if(!strcasecmp((elem)->pal,"para")) {
#else
      if (!stricmp((elem)->pal,"para")) {
#endif
         if (!unica_categoria(VERBO,elem)) elimina_categoria(VERBO,elem);
         //continue;
      }

/*
      if (esta_palabra_ten_a_categoria_de(PARTICIPIO,elem-1) &&
//          !unica_categoria(PARTICIPIO, elem - 1) &&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem)) {
//          asigna_categoria(ADXECTIVO,elem-1);

          if (esta_palabra_ten_a_categoria_de(NOME, elem - 1))
          	anade_categoria(ADXECTIVO,elem-1);
          else
          	asigna_categoria(ADXECTIVO, elem - 1);
//          anade_categoria(NOME,elem-1);

          if (elem > item + 1) {
          		if (esta_palabra_ten_a_categoria_de(VERBO, elem - 2))
                	if ( (strcmp((elem - 2)->inf_verbal->infinitivo, "ser") == 0) ||
                    	 (strcmp((elem - 2)->inf_verbal->infinitivo, "estar") == 0) ||
                    	 (strcmp((elem - 2)->inf_verbal->infinitivo, "resultar") == 0) ||
						 (strcmp((elem - 2)->inf_verbal->infinitivo, "parecer") == 0))
                    asigna_categoria(ADXECTIVO, elem - 1);
          }
      }
*/

      if (esta_palabra_ten_a_categoria_de(PARTICIPIO,elem) &&
//          !unica_categoria(PARTICIPIO, elem - 1) &&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem - 1)) {
//          asigna_categoria(ADXECTIVO,elem-1);

          if ( esta_palabra_ten_a_categoria_de(NOME, elem) ||
          	   (elem->cat_verbal[1] != 0) )
          	anade_categoria(ADXECTIVO,elem);
          else
          	asigna_categoria(ADXECTIVO, elem);
//          anade_categoria(NOME,elem-1);

        if (esta_palabra_ten_a_categoria_de(VERBO, elem - 1))
            if ( (strcmp((elem - 1)->inf_verbal->infinitivo, "ser") == 0) ||
                 (strcmp((elem - 1)->inf_verbal->infinitivo, "estar") == 0) ||
                 (strcmp((elem - 1)->inf_verbal->infinitivo, "resultar") == 0) ||
                 (strcmp((elem - 1)->inf_verbal->infinitivo, "parecer") == 0))
            asigna_categoria(ADXECTIVO, elem);

      }

/*
      else if (esta_palabra_ten_a_categoria_de(INFINITIVO,elem-1)) {
          elimina_categoria(INFINITIVO,elem-1);
          anade_categoria(NOME,elem-1);
      }
*/

/*
      if (!esta_palabra_ten_a_categoria_de(NOME,elem) &&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem) &&
          !esta_palabra_ten_a_categoria_de(INDEF,elem) &&
          !esta_palabra_ten_a_categoria_de(NUME,elem) &&
          !esta_palabra_ten_a_categoria_de(NUME_ORDI,elem) &&
          !esta_palabra_ten_a_categoria_de(NUME_CARDI,elem)) {
         if (esta_palabra_ten_a_categoria_de(INDEF,elem-1)){
             elimina_categoria(INDEF,elem-1);
             anade_categoria(INDEF_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(DEMO,elem-1)){
             elimina_categoria(DEMO,elem-1);
             anade_categoria(DEMO_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,elem-1)){
             elimina_categoria(CONTR_PREP_DEMO,elem-1);
             anade_categoria(CONTR_PREP_DEMO_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_DEMO_INDEF,elem-1)){
             elimina_categoria(CONTR_DEMO_INDEF,elem-1);
             anade_categoria(CONTR_DEMO_INDEF_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,elem-1)){
             elimina_categoria(CONTR_PREP_DEMO_INDEF,elem-1);
             anade_categoria(CONTR_PREP_DEMO_INDEF_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,elem-1)){
             elimina_categoria(CONTR_PREP_INDEF,elem-1);
             anade_categoria(CONTR_PREP_INDEF_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(POSE,elem-1)){
             elimina_categoria(POSE,elem-1);
             anade_categoria(POSE_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_CARDI,elem-1)){
             elimina_categoria(NUME_CARDI,elem-1);
             anade_categoria(NUME_CARDI_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_ORDI,elem-1)){
             elimina_categoria(NUME_ORDI,elem-1);
             anade_categoria(NUME_ORDI_PRON,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_PARTI,elem-1)){
             elimina_categoria(NUME_PARTI,elem-1);
             anade_categoria(NUME_PARTI_PRON,elem-1);
         }
      }
      else
*/
      if (unica_categoria(NOME,elem) ||
         (n_categorias(elem)==2 &&
          esta_palabra_ten_a_categoria_de(NOME,elem) &&
          esta_palabra_ten_a_categoria_de(ADXECTIVO,elem))){
         if (esta_palabra_ten_a_categoria_de(INDEF,elem-1)){
             elimina_categoria(INDEF,elem-1);
             anade_categoria(INDEF_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(DEMO,elem-1)){
             elimina_categoria(DEMO,elem-1);
             anade_categoria(DEMO_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,elem-1)){
             elimina_categoria(CONTR_PREP_DEMO,elem-1);
             anade_categoria(CONTR_PREP_DEMO_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_DEMO_INDEF,elem-1)){
             elimina_categoria(CONTR_DEMO_INDEF,elem-1);
             anade_categoria(CONTR_DEMO_INDEF_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,elem-1)){
             elimina_categoria(CONTR_PREP_DEMO_INDEF,elem-1);
             anade_categoria(CONTR_PREP_DEMO_INDEF_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,elem-1)){
             elimina_categoria(CONTR_PREP_INDEF,elem-1);
             anade_categoria(CONTR_PREP_INDEF_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(POSE,elem-1)){
             elimina_categoria(POSE,elem-1);
             anade_categoria(POSE_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_CARDI,elem-1)){
             elimina_categoria(NUME_CARDI,elem-1);
             anade_categoria(NUME_CARDI_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_ORDI,elem-1)){
             elimina_categoria(NUME_ORDI,elem-1);
             anade_categoria(NUME_ORDI_DET,elem-1);
         }
         if (esta_palabra_ten_a_categoria_de(NUME_PARTI,elem-1)){
             elimina_categoria(NUME_PARTI,elem-1);
             anade_categoria(NUME_PARTI_DET,elem-1);
         }
      }
      if (unica_categoria(POSE_DET,elem) &&
          esta_palabra_ten_a_categoria_de(PREP,elem-1) &&
          !unica_categoria(PREP, elem - 1) )
          elimina_categoria(PREP,elem-1);
/*
      else if ((unica_categoria(NOME,elem) || unica_categoria(NOME_PROPIO,elem)) &&
          esta_palabra_ten_a_categoria_de(NOME,elem-1)&&
          esta_palabra_ten_a_categoria_de(ADXECTIVO,elem-1))
          elimina_categoria(NOME,elem-1);
*/

      if ((unica_categoria(NOME,elem-1) || unica_categoria(NOME_PROPIO,elem-1)) &&
          esta_palabra_ten_a_categoria_de(NOME,elem)&&
          esta_palabra_ten_a_categoria_de(ADXECTIVO,elem))
          	if ( ( (elem->numero == (elem - 1)->numero) ||
            	 (elem->numero == NO_ASIGNADO) ||
                 (elem->numero == AMBIGUO) ||
				 ((elem - 1)->numero == AMBIGUO) ||
                 ((elem - 1)->numero == NO_ASIGNADO)) &&
				 ( (elem->xenero == (elem - 1)->xenero) ||
            	 (elem->xenero == AMBIGUO) ||
            	 (elem->xenero == NO_ASIGNADO) ||
                 ((elem - 1)->xenero == NO_ASIGNADO) ||
                 ((elem - 1)->xenero == AMBIGUO)) )
          elimina_categoria(NOME,elem);

      if (!strcmp(pala,"un") &&
         (unica_categoria(NOME,elem) || unica_categoria(NOME_PROPIO,elem)))
         if (esta_palabra_ten_a_categoria_de(ART_INDET, elem - 1))
	         asigna_categoria(ART_INDET,elem-1);

      if ( //(esta_palabra_ten_a_categoria_de(INDEF,elem-1) ||
            esta_palabra_ten_a_categoria_de(ART_INDET,elem-1) &&  (
              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON,elem) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,elem)))
                asigna_categoria(INDEF_PRON,elem-1);

	  if (elem > item + 1)
	      if ( (esta_palabra_ten_a_categoria_de(INDEF,elem-1) ||
	            esta_palabra_ten_a_categoria_de(ART_INDET,elem-1) ) &&
                unica_categoria(PREP, elem - 2) &&
	              (
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON,elem) ||
	              esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,elem)))
	                asigna_categoria(INDEF_PRON,elem-1);

#ifndef _WIN32
      if (!strcasecmp((elem)->pal,"que")) {
#else
      if (!stricmp((elem)->pal,"que")) {
#endif
        if (unica_categoria(ART_DET,elem-1) ||
            unica_categoria(NOME,elem-1) ||
            unica_categoria(NOME_PROPIO,elem-1) ||
            unica_categoria(CONTR_CONX_ART_DET,elem-1) ||
            unica_categoria(CONTR_PREP_ART_DET,elem-1) ||
            unica_categoria(CONTR_INDEF_ART_DET,elem-1) ||
            unica_categoria(ART_INDET,elem-1) ||
            unica_categoria(CONTR_PREP_ART_INDET,elem-1) ||
            unica_categoria(INDEF_DET,elem-1) ||
            unica_categoria(CONTR_PREP_INDEF_DET,elem-1) ||
            unica_categoria(DEMO_DET,elem-1) ||
            unica_categoria(CONTR_PREP_DEMO_DET,elem-1) ||
            unica_categoria(CONTR_DEMO_INDEF_DET,elem-1) ||
            unica_categoria(CONTR_PREP_DEMO_INDEF_DET,elem-1) ||
            unica_categoria(POSE_DET,elem-1) ||
            unica_categoria(NUME_DET,elem-1) ||
            unica_categoria(NUME_CARDI_DET,elem-1) ||
            unica_categoria(NUME_ORDI_DET,elem-1) ||
            unica_categoria(NUME_PARTI_DET,elem-1)
            )
          asigna_categoria(RELA,elem);
        else if (unica_categoria(VERBO,elem-1))
          asigna_categoria(CONX_SUBOR_COMPLETIVA,elem);

       
        else if ( (!strcmp(pala,"de") || !strcmp(pala,"en")) && !unica_categoria(INTER, elem))
               asigna_categoria(CONX_SUBOR_COMPLETIVA,elem);
        else if (!strcmp(pala,"para"))
               asigna_categoria(CONX_SUBOR_FINAL,elem);
		
		else if ( (pala[0]=='d' || pala[0]=='n' ) && (
              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON,elem-1) ||
              esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,elem-1)))
                asigna_categoria(RELA,elem);

      }

/*
      if (!strcmp(pala,"que")) {
        if (unica_categoria(VERBO,elem) ||
            unica_categoria(PRON_PERS_AT,elem) ||
            unica_categoria(PRON_PERS_AT_REFLEX,elem) ||
            unica_categoria(PRON_PERS_AT_ACUS,elem) ||
            unica_categoria(PRON_PERS_AT_DAT,elem) ||
            unica_categoria(CONTR_PRON_PERS_AT_DAT_AC,elem) ||
            unica_categoria(CONTR_PRON_PERS_AT_DAT_DAT_AC,elem) ||
            unica_categoria(CONTR_PRON_PERS_AT,elem) )
          asigna_categoria(RELA,elem-1);
      }
*/
      //i++;
      elem++;
   }
}
else {
//   while(i<LONX_MAX_FRASE_SEPARADA) {
  while(*elem->pal) {
        if ((unica_categoria(ART_DET,elem-2) ||
         unica_categoria(CONTR_CONX_ART_DET,elem-2) ||
         unica_categoria(CONTR_PREP_ART_DET,elem-2) ||
         unica_categoria(CONTR_INDEF_ART_DET,elem-2) ||
         unica_categoria(ART_INDET,elem-2) ||
         unica_categoria(CONTR_PREP_ART_INDET,elem-2) ||
         unica_categoria(INDEF_DET,elem-2) ||
         unica_categoria(CONTR_CONX_COOR_COP_ART_DET,elem-2) ||
         unica_categoria(DEMO_DET,elem-2) ||
         unica_categoria(CONTR_PREP_DEMO_DET,elem-2) ||
         unica_categoria(CONTR_DEMO_INDEF_DET,elem-2) ||
         unica_categoria(CONTR_PREP_DEMO_INDEF_DET,elem-2) ||
         unica_categoria(CONTR_PREP_INDEF_DET,elem-2) ||
         unica_categoria(POSE_DET,elem-2) ||
         unica_categoria(NUME_CARDI_DET,elem-2) ||
         unica_categoria(NUME_ORDI_DET,elem-2) ||
         unica_categoria(NUME_PARTI_DET,elem-2)) &&
          esta_palabra_ten_a_categoria_de(NOME,elem-1)&&
          esta_palabra_ten_a_categoria_de(ADXECTIVO,elem-1) &&
          !esta_palabra_ten_a_categoria_de(NOME,elem)&&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem)&&
          !esta_palabra_ten_a_categoria_de(NOME_PROPIO,elem))
          elimina_categoria(ADXECTIVO,elem-1);
/*
      if(!esta_palabra_ten_a_categoria_de(NOME,elem-2)&&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem-2)&&
          !esta_palabra_ten_a_categoria_de(NOME_PROPIO,elem-2)&&
          esta_palabra_ten_a_categoria_de(NOME,elem-1)&&
          esta_palabra_ten_a_categoria_de(ADXECTIVO,elem-1) &&
          !esta_palabra_ten_a_categoria_de(NOME,elem)&&
          !esta_palabra_ten_a_categoria_de(ADXECTIVO,elem)&&
          !esta_palabra_ten_a_categoria_de(NOME_PROPIO,elem))
          elimina_categoria(ADXECTIVO,elem-1);
*/
/*
#ifndef _WIN32
      if (!strcasecmp(elem->pal,"que")) {
#else
      if (!stricmp(elem->pal,"que")) {
#endif
			if ( esta_palabra_ten_a_categoria_de(VERBO,elem-2) &&
				*(elem-1)->pal=='a' && (elem-1)->pal[1]==0 )
				asigna_categoria(CONX_SUBOR_COMPLETIVA,elem);
			if ( esta_palabra_ten_a_categoria_de(VERBO,elem-2) &&
				adverbio((elem-2)->cat_gramatical[0])  )
				asigna_categoria(CONX_SUBOR_COMPLETIVA,elem);

	  }
*/
      //i++;
      elem++;
   }

}
   return 0;
}



/** 
 * \author fmendez
 * \brief 
 * 
 * \param sint 
 */
void Sintagma::cuenta_silabas_sintagma(t_frase_sintagmada *sint){

  for(int i=sint->inicio;i<=sint->fin;i++)
    sint->n_silabas+=frase_separada[i].n_silabas;
}
/** 
 * \author fmendez
 *
 * \remarks As etapas que se describen a continuación permiten facer a analise
 * de sintagmas, grupos e proposicións. Identificar_grupos_conxuntivos é unha
 * función que detecta aqu el conxunto de palabras que teñen unha función
 * conxuntamente. Este é o caso das locucións prepositivas e locucións
 * conxuntivas.  
 * 
 * \param fseparada 
 * \param fsintagmada 
 * \param fgrupos 
 * \param pidioma 
 */
void Sintagma::analise_sintagmatico(t_frase_separada *fseparada,
                            t_frase_sintagmada *fsintagmada,
                            t_frase_en_grupos * fgrupos, char pidioma){

    frase_separada   = fseparada;
    frase_sintagmada = fsintagmada;
    frase_en_grupos  = fgrupos;
    idioma=pidioma;

  if (frase_separada->pal[0]==0) return;
/*
  if (!(opciones.tra && opciones.tra<4 && !opciones.lin && !opciones.audio && !opciones.wav))
  {
    identificar_grupos_conxuntivos(frase_separada);
    reglas_desam(frase_separada,1);
    reglas_desam(frase_separada,1);
    reglas_desam(frase_separada,2);
    //ZA:
    reglas_desam(frase_separada,0);
    //desambiguar_categorias_gramaticais(frase_separada);
  }
*/
  identificar_sintagmas();
  identificar_grupos_entre_pausas(frase_sintagmada,frase_en_grupos);
  identificar_tonemas(frase_en_grupos);
}





