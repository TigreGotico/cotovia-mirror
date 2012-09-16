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
#include "letras.hpp"
#include "preproc.hpp"
//#include "variables_globales.hpp"
#include "sil_acen.hpp"

const char *separador_silabas="-";

const char *acento_prosodico="^";


int guion_de_segunda_forma_do_artigo(char *recorre_frase){
 /*esta funcion_devolve un 1 se o guion pertence a un guion entre letras,
    por exemplo nunha segunda forma do artigo, que sera o unico caso permitido
    a estas alturas do programa*/

   return (letra ((unsigned char)*(recorre_frase-1))  &&
            *(recorre_frase)=='-'  &&
            letra ((unsigned char)*(recorre_frase+1)));
}

/************************************************************************/
void separar_segun_modelo(const char *modelo,t_palabra saida,char *letra) {
/*  indicaselle en modelo unha cadea VV-V e separa deste xeito por exemplo
      ou  ben C-C donde C e consonante e V vocal e o separador e o guion
      pero solamente na cadea modelo*/

  char *axu=letra;
	const char * corre_modelo=modelo;
	char cont = 0;


 while (1){
    while (*corre_modelo!=*separador_silabas && *corre_modelo!=0){
      cont++;
      corre_modelo++;
    }
      strncat(saida,axu,cont);
      if (*corre_modelo!=0) strcat(saida,separador_silabas);
      else return;
      axu=axu+cont;
      corre_modelo++;
      cont=0;
 }
}
/*************************************************************************/
inline short int secuencia_de_(unsigned char cantidade,unsigned char tipo, const char * axu)
/* esta funcion sirve para comprobar se se produce a cantidade de
    vocales ou consoantes que se indica a partir de orixe.A mision
    e facer mais lexible o codigo. Tipo sera ou CONSONANTE OU VOCAL*/
{
 unsigned char cont;
// char *axu;
 //axu=orixe;

  for(cont=0;cont<cantidade;cont++){
    if (tipo==VOCAL) {
      if (!vocal((unsigned char)*axu)) return 0;
    }
    else {
      if (!consonante((unsigned char)*axu)) return 0;
    }
    axu++;
  }
  if (tipo==VOCAL) {
      if (!vocal((unsigned char)*axu)) return 1;
  }
  else  {
      if (!consonante((unsigned char)*axu)) return 1;
  }
  return 0;

}
/***********************************************************************/
/***********************************************************************/
short int  grupo_consonantico_indivisible(char grupo[2])
/* grupos que non se poden partir*/
{
 if ( strcmp(grupo,"bl")==0 ) return 1;
 if ( strcmp(grupo,"br")==0 ) return 1;
 if ( strcmp(grupo,"pl")==0 ) return 1;
 if ( strcmp(grupo,"pr")==0 ) return 1;
 if ( strcmp(grupo,"cl")==0 ) return 1;
 if ( strcmp(grupo,"cr")==0 ) return 1;
 if ( strcmp(grupo,"gl")==0 ) return 1;
 if ( strcmp(grupo,"gr")==0 ) return 1;
 if ( strcmp(grupo,"dr")==0 ) return 1;
 if ( strcmp(grupo,"tl")==0 ) return 1;
 if ( strcmp(grupo,"tr")==0 ) return 1;
 if ( strcmp(grupo,"fl")==0 ) return 1;
 if ( strcmp(grupo,"fr")==0 ) return 1;
 if ( strcmp(grupo,"ps")==0 ) return 1;
 if ( strcmp(grupo,"pn")==0 ) return 1;
 if ( strcmp(grupo,"ch")==0 ) return 1;
 if ( strcmp(grupo,"ck")==0 ) return 1;
 if ( strcmp(grupo,"rr")==0 ) return 1;
 if ( strcmp(grupo,"ll")==0 ) return 1;
 if ( strcmp(grupo,"kr")==0 ) return 1;
 if ( strcmp(grupo,"kl")==0 ) return 1;
 if ( strcmp(grupo,"tr")==0 ) return 1;
//fmendez
 if ( strcmp(grupo,"vl")==0 ) return 1;
 if ( strcmp(grupo,"gh")==0 ) return 1;

 return 0;
}
/******************************************************************/
 short int diptongo(const char *letra)

 /* esta funcion serve tanto para examinar os diptongos como
      os triptongos que se poden descompoñen en dous diptongos*/

 {
 /*   if (consonante( *(letra+2) ) )  return 0; */
   /*so deben ser duas vocais seguidas para que teña sentido esta
      comprobacion e asi non facer comprobacions inutiles e que levan
      tempo*/

   if ( vocal_feble((unsigned char)*letra)  &&  vocal_aberta((unsigned char)*(letra+1))  ) return 1;
   if ( vocal_aberta((unsigned char)*letra)  &&  vocal_feble((unsigned char)*(letra+1))  ) return 1;
   if ( vocal_feble((unsigned char)*letra)  &&  vocal_feble((unsigned char)*(letra+1))  ) return 1;



   return 0;


 }

 /******************************************************************/
inline  short int caso_de_u_muda(const char * letra){
   /* para separar na palabra aquí,por exemplo uí tomariase como hiato e
       separarianse, sem embargo deben ir unidas*/
  if (!(*letra=='g' || *letra=='q'))  return 0;
  if (!(*(letra+1)=='u')) return 0;
  if (vocal((unsigned char)*(letra+2))) return 1;
  return 0;
}
/******************************************************************/
inline int solo_quedan_consonantes(const char *letra)
{
  unsigned char * copia=(unsigned char*)letra;
  for(;*copia;copia++)
    if (!consonante(*copia)) return 0;
  return 1;
/*
 while (*copia)
 {
   if (!consonante((unsigned char)*copia)) return 0;
   copia++;
 }
 return 1;
*/
}
/******************************************************************/

short int triptongo(const char * letra){
   if ( vocal_feble((unsigned char)*letra) &&
       ( (vocal_aberta((unsigned char)*(letra+1)) && vocal_feble((unsigned char)*(letra+2)) )
                    ||
       (  *(letra+1)=='i' && vocal_aberta((unsigned char)*(letra+2))      )
                                                                                                       )    )
                                 /* TRIPTONGO */
      return 1;
   return 0;
}
/*********************************************************************/
void kkseparar_silabas(char * entrada,char * saida)
{
 /* daselle unha palabra de entrada e separaa en silabas co caracter
 indicado por separador_silabas, variable global definida en var_glo1.c, ainda
 que isto faise na funcion separar_segun_modelo.
 Deixa a palabra separada en silabas en t_palabra saida. */

  char *letra, cacho[4]="";
 letra=entrada;
 strcpy(saida,"");

 while ((!vocal((unsigned char)*letra)) && (*(letra+1)!=FIN_DE_PALABRA) && !(
       (*letra=='q' || *letra=='g') && *(letra+1)=='u')) {
   strncat(saida,letra,1);
   letra++;
 }
/* O principio de palabra avanzamos hasta a primeira vocal, excepto se a pala-
   -bra empeza por q ou g + u , xa que se non hai que ter mais cuidado (debido
   o tratamento da u muda en gui que, qui, que.                               */
 SILABIFICAR {                  /* E un while (1)                             */
   SE_E_VOCAL {                 /* Outro while (1)                            */
      if (*letra==FIN_DE_PALABRA)  return;
      if (*letra=='-' || *letra==' ') {        /* Os guions non contan a hora de silabificar.*/
         letra++;
         break;
      }
      if (!vocal((unsigned char)*letra)) break;
      if (secuencia_de_(1,VOCAL,letra))
    /* unha soa vocal en medio de 2 consoantes*/
      {
         strncat(saida,letra,1);
         letra++;
         break;
      }
    if (vocal_aberta((unsigned char)*letra) && vocal_aberta((unsigned char)*(letra+1))) {
      separar_segun_modelo("V-",saida,letra);
      letra++;
      break;
    }
    if (  vocal((unsigned char)*letra) && vocal((unsigned char)*(letra+1))  && vocal((unsigned char)*(letra+2) )   )
         {
          if (!triptongo(letra))
               {
                if(*(letra+1)==(char )i_con_dierese)
                /* para comprobar 2 3 persoas de imperfectos con dierese*/
                     separar_segun_modelo("V-V-V",saida,letra);
                else
                     separar_segun_modelo("V-VV",saida,letra);
                letra=letra+3;
                break;
               }
/* Mais de tres vocais seguidas que non  formen diptongo separase na primeira
   vocal.                                                                     */

         }


    if (
               (vocal((unsigned char)*letra))   &&  vocal((unsigned char)*(letra+1))
                        &&   !diptongo(letra)              )
    /*se son duas vocais seguidas sin formar diptongo hai que separalas.
       Tamen se van separando deste xeito secuencias de tres ou mais le
       tras que non forman diptongo nin triptongo*/
       {
          separar_segun_modelo("V-",saida,letra);
          letra=letra+1;
          break;
       };

      strncat(saida,letra,1);
      /* se non se axusta a ningun dos casos anteriores non se pon separador*/
      letra++;

   }

   SE_E_CONSONANTE
   {
    if (*letra==FIN_DE_PALABRA )  return;

    if (*letra=='-' || *letra==' ')
    /* os guions non contan a hora de silabificar*/
      {
          letra++;
          break;
      }

    if (! consonante((unsigned char)*letra) )      break;
    if (solo_quedan_consonantes(letra))
    /* se solo quedan consonantes ó final  engadense directamente a
         saida sen separar nada*/
         {
          strcat(saida,letra);
          letra=letra+strlen(letra);
          break;
         };


      if ( caso_de_u_muda(letra) )

      /*caso de quí guí nos que  a u ca letra g ou q e "muda")*/

      {
       if (letra==entrada)
       /* se é a primeira letra*/
       {
             separar_segun_modelo("CV",saida,letra);
             letra=letra+2;

       }
       else
       {
             separar_segun_modelo("-CV",saida,letra);
             letra=letra+2;
       }
       break;

    }


    if ( consonante((unsigned char)*letra) && vocal((unsigned char)*(letra+1))  )
    /* se se trata dunha soa consonante seguida de vocal segmentase
         antes da consonante -c */
       {
            if (strlen(saida)!=0)
           {
             separar_segun_modelo("-C",saida,letra);
             letra=letra+1;
             break;
           }
            else
           {
            separar_segun_modelo("C",saida,letra);
            letra=letra+1;
            break;
           }
       }

    if (secuencia_de_(2,CONSONANTE,letra))
         /* se son 2 consonantes */
    {
      strncpy(cacho,letra,2);
      cacho[2]=0;
      if ( grupo_consonantico_indivisible(cacho))
      /* se e un dos grupos enumerados nesa funcion segmentase antes das
          consoantes  -cc  , senon separase despois da primeira consoante*/
         {
             separar_segun_modelo("-CC",saida,letra);
             letra=letra+2;
                 if ( caso_de_u_muda(letra-1) )
                 {
                  strcat(saida,"u");
                  letra++;
                 }
             break;

         }
    else
         {
             separar_segun_modelo("C-C",saida,letra);
             letra=letra+2;
                 if ( caso_de_u_muda(letra-1) )
                 {
                  strcat(saida,"u");
                  letra++;
                 }
             break;
         }
    }



    if  (secuencia_de_(3,CONSONANTE,letra))
          /* se son 3 consonantes */
    {
       strncpy(cacho,letra+1,2);
       cacho[2]=0;

      if ( grupo_consonantico_indivisible(cacho) )
      /* se e un dos grupos enumerados nesa funcion segmentase antes das
          consoantes c-cc  , senon separase antes da ultima consoante cc-c*/
         {
             separar_segun_modelo("C-CC",saida,letra);
             letra=letra+3;
             break;
         }
    else
          {
             separar_segun_modelo("CC-C",saida,letra);
             letra=letra+3;
                 if ( caso_de_u_muda(letra-1) )
                 {
                  strcat(saida,"u");
                  letra++;
                 }
             break;
       }
    }


    if  (secuencia_de_(4,CONSONANTE,letra))
    /* se son 4 consonantes  enton separase cc-cc  */
    {
      separar_segun_modelo("CC-CC",saida,letra);
      letra=letra+4;
      break;
    }
    else
    {
      strncat(saida,letra,1);
      letra++;
      break;
    }
   // break;
   }
   }

}
/*********************************************************************/
char *grave(char * palabra, char * p)
{
   /*Funcion que busca o acento prosodico de palabras graves que non levan til.
   levan til. Devolve un punteiro a vocal sobre a que recai o acento*/

   while(*p !=*separador_silabas && p > palabra)
      p--;
   while(!vocal((unsigned char)*p) && p > palabra)
      p--;
   switch (*p) {
      case 'i':
      case 'u':
         if( (p>palabra) &&  vocal((unsigned char)*(p-1))) {
	   if (!(( (*(p-1)=='u') || (*(p - 1) == 'ü') ) && p>palabra+1 && ( (*p-2)=='q' || (*p-2)=='g' ) ))
               p--;
         }
      }
   return p;
}
/************************************************************************/


char *aguda(char * palabra, char * p)
{
   /* Funcion que busca o acento fonetico daquelas palabras agudas que non
      levan til. Devolve un punteiro a vocal sobre a que recae o acento*/
   while(!vocal((unsigned char)*p) && p > palabra)
      p--;
   switch(*p) {
      case 'i':
      case 'u':
         if( (p>palabra) &&  vocal((unsigned char)*(p-1))) {
            if (!((*(p-1)=='u') && p>palabra+1 && ( (*p-2)=='q' || (*p-2)=='g' ) ))
               p--;
         }
      }
   return p;
}

/********************************************************************/
 short int numero_silabas(t_palabra pal)
 {
   /* funcion que conta o numero de silabas dunha palabra xa separada.
       Inicializamos a 1 o contador e cada vez que encontre un separador
       de silabas incrementase o numero de silabas*/

   char cont=1,*axu=pal;
   while (*axu)
   {
    if (*axu==*separador_silabas) cont++;
    axu++;
   };
   return cont;
 }
/********************************************************************/
 void eliminar_til(char *vocal_acentuada)
 /* dada unha vocal acentuada desacentuase graficamente*/
 {
   switch((unsigned char)*vocal_acentuada)
   {
    case a_acentuada:
    {
      *vocal_acentuada='a';
      break;
    }
    case e_acentuada:
    {
      *vocal_acentuada='e';
      break;
    }
    case i_acentuada:
    {
    *vocal_acentuada='i';
      break;
    }
    case e_con_dierese:
    {
    *vocal_acentuada='e';
      break;
    }
    case i_con_dierese:
    {
    *vocal_acentuada='i';
      break;
    }
    case o_acentuada:
    {
    *vocal_acentuada='o';
      break;
    }
    case u_acentuada:
    {
      *vocal_acentuada='u';
      break;
    }
   }
   return;
 }

/********************************************************************/
 short int diacritico_dif_aberta_pechada(t_palabra pal_entrada,t_palabra pal_saida){
 /* Esta funcion chamase para que naqueles casos nos que o diacritico e
      para diferenciar aberta-pechada  e non se elimine o til para dispor
      desta informacion no momento da conversion en alofonos. A funcion
      devolve a palabra xa acentuada prosodicamente e separada*/
   const char *diacriticos_oposicion_aberta_pechada[]={"é","ó","ós",
             "có","cós","nós","vós","vén","vés","pré-sa","bó-la","bó-las","ó-so","cóm-pre","pó-la","tén","té","dó","sé","nó","só","\0"};
   char *pos_acento;
   int cont;

   if ((pos_acento=strpbrk(pal_entrada,"éó"))==NULL) return 0;

   cont=0;
   while (*diacriticos_oposicion_aberta_pechada[cont++]!=0 ){
      if (strcmp(diacriticos_oposicion_aberta_pechada[cont],pal_entrada)==0){
          strncpy(pal_saida,pal_entrada,(pos_acento+1)-pal_entrada);
          pal_saida[(pos_acento+1)-pal_entrada]=0;
          strcat(pal_saida,acento_prosodico);
          strcat(pal_saida,pos_acento+1);
          return 1;
      }
   }
   return 0;
}
 /*******************************************************************/

int acentuar_prosodicamente(t_palabra_proc  pal_entrada, char *pal_saida, char idioma){
/* Función que busca o acento fonético da palabra e devolve  a palabra
   co seu acento. O acento escribese despois da vocal acentuada, co
   carácter acento_prosodico, definido como macro.  */

   char *punt1, *punt2, *punt_primeiros_acentos;   /* Punteiros auxiliares para recorre-la */
   /* punt_primeiro_acento apunta ó primeiro acento dunha palabra que
      conteña dous acentos gráficos como político-económico, deixando como
      valido só o ultimo deles. Tamén serve como medida de precaución para
      que toda palabra con dous acentos gráficos ou mais leve só un ó final
      desta etapa.*/
   int orde_acento;
   char* pos_acento;                                        /*  palabra de entrada             */

   short int i = 0;           /* Variable auxiliar contador */
   *pal_saida=0;       /* Inicialización da pal_saida resultante */

//   #ifdef REGLAS_DE_ALOFONOS_DO_GALEGO
   if (idioma==GALEGO && diacritico_dif_aberta_pechada(pal_entrada,pal_saida))
      return 0;
   /* nestes casos o acento grafico conservase para a etapa final de conver
       ter a alofonos pois diferencia aberta pechada. No resto dos casos
       eliminase o acento grafico e colocase o acento_prosodico*/
//   #endif

//fmendez die   if((punt1=strpbrk(pal_entrada,"áéíóúï"))!=NULL)    /* Palabra con acento gráfico  */
   if((punt1=strpbrk(pal_entrada,"áéíóú"))!=NULL)    /* Palabra con acento gráfico  */
      {
       punt_primeiros_acentos=punt1;
       while(punt_primeiros_acentos!=NULL)
       {
///fmendez die        punt1 = strpbrk(punt_primeiros_acentos+1,"áéíóúï");  /* Apunta a vocal acentuada */
        punt1 = strpbrk(punt_primeiros_acentos+1,"áéíóú");  /* Apunta a vocal acentuada */
        if (punt1==NULL)
         {
          punt1=punt_primeiros_acentos;
         break;
         }
        else
         {
            eliminar_til(punt_primeiros_acentos);
          punt_primeiros_acentos=punt1;
         }
       }/*while*/
      }
   else
   {                       /* Non ten acento gráfico */

      punt2 = &pal_entrada[strlen(pal_entrada)-1];
                  /* punt2 apunta a terminación      */
                  /* aplicamos as reglas de acentuación */

      if(consonante((unsigned char)*punt2))
      {
         switch(*punt2)
         {
            case 'n':
               if(strstr(pal_entrada,separador_silabas)!=NULL)
                  punt1 = grave(pal_entrada,punt2);
               else
                  punt1 = aguda(pal_entrada,punt2);
               break;
            case 's':
               if(strstr(pal_entrada,separador_silabas)==NULL)
                  punt1 = aguda(pal_entrada,punt2);
               else {
                  punt1 = punt2-2;
                  if(vocal((unsigned char)*punt1) &&
                        ((*(punt1+1)=='i')||(*(punt1+1)=='u')))
                     punt1 = aguda(pal_entrada,punt2);
                  else
                     punt1 = grave(pal_entrada,punt2);
               }
               break;
            default:
               punt1 = aguda(pal_entrada,punt2);
               break;
         }
      }
      else if(vocal((unsigned char)*punt2))
      {
         switch(*punt2)
         {
            case 'i':
            case 'u':
               if(strlen(pal_entrada)>1 && vocal((unsigned char)*(punt2-1)))
               {
                  punt1 = aguda(pal_entrada,punt2);
                  break;
               }
            default:
               if(strstr(pal_entrada,separador_silabas)!=NULL)
                  punt1 = grave(pal_entrada,punt2);
               else
                  punt1 = aguda(pal_entrada,punt2);
         }
      }
   }
   /* A saida do bucle cas reglas de acentuación
      punt1 apunta a vocal sobre a que recae o acento fonético */

   while(pal_entrada<punt1)
   {     /* Escribe os caracteres anteriores o acento */
      pal_saida[i++] = *pal_entrada;
      pal_entrada++;
   }
   switch ((unsigned char) *punt1)
   {  /* Escribe a vocal, seguida da marca de acento_prosodico */
      case a_acentuada:
         pal_saida[i++] = 'a';
         pal_saida[i++] =*acento_prosodico;
         break;
      case e_acentuada:
         pal_saida[i++] = 'e';
         pal_saida[i++] =*acento_prosodico;
         break;
      case i_acentuada:
      //case i_con_dierese:
         pal_saida[i++] = 'i';
         pal_saida[i++] =*acento_prosodico;
         break;
      case o_acentuada:
         pal_saida[i++] = 'o';
         pal_saida[i++] =*acento_prosodico;
         break;
      case u_acentuada:
         pal_saida[i++] = 'u';
         pal_saida[i++] =*acento_prosodico;
         break;
      default:
         pal_saida[i++] = *punt1;
         pal_saida[i++] =*acento_prosodico;
   }

   pal_saida[i] = '\0';
   /* Terminación da pal_saida resultante */

   strcat(pal_saida,(pal_entrada+1));
   /* Escribe o resto de caracteres na pal_saida resultante      */

   /* Agora devolvemo-la posicion do acento, indicada cun número que
      nos indica a orde desta sílaba empezando polo final*/
      pos_acento=&pal_saida[strlen(pal_saida)];

      orde_acento=1;
      while (*pos_acento!='^')
      {
       if (*pos_acento=='-') orde_acento++;
       pos_acento--;
      }

  return orde_acento;
}

/******************************************************************************
 *  void silabificar_e_acentuar(t_frase_separada * frase_separada): Nesta fun- *
 * -cion primeiro extraemos as palabras que estan separadas por un blanco. Se *
 * algunha destas palabras ten algun componente que non sexa letra enton a u- *
 * -nica posibilidade e que esta palabra  ou mellor dito, grupo de caracteres *
 * e un signo de puntuacion (punto,coma, dous guions, etc, e obviamente estas *
 * palabras non hai que separalas nin acentualas senon que se copian directa- *
 * -mente na frase_sil_e_acentuada. As outras palabras separanse e acentuanse *
 * prosodicamente.                                                            *
 ******************************************************************************/
void silabificar_e_acentuar(t_frase_separada * frase_separada, char idioma)
{
   char *recorre_frase;
   unsigned char cont;
   t_palabra_proc pal_extraida="",aux;
   t_palabra_sil_e_acentuada pal_separada_silabas="";
   t_palabra_sil_e_acentuada pal_acentuada="";
   int num_items=0;


   while (frase_separada->pal[0] && num_items<LONX_MAX_FRASE_SEPARADA) {
      if (frase_separada->pal_transformada!=NULL) {
          recorre_frase=frase_separada->pal_transformada;
      }
      else {
          recorre_frase=frase_separada->pal;
      }
      frase_separada->n_silabas = 0;
/* A continuacion pasamos dos caracteres en blanco que hai entre palabras     */
      while(*recorre_frase) {       /* BEGIN WHILE                            */
         cont=0;
         while (*recorre_frase==' ') {
            recorre_frase++;
         }
         if (*recorre_frase=='\0') {
            break;
         }
/* Extraemos palabra entre espacios                                           */
         while (*recorre_frase!=' ' &&  *recorre_frase!='\0') {
            pal_extraida[cont]=*recorre_frase;
            recorre_frase++;
            cont++;
         }
         pal_extraida[cont]=0;
/* Se a palabra non e un signo de puntuacion separamola en silabas e despois
   acentuamola prosodicamente.                                                */
         if (frase_separada->clase_pal!=SIGNO_PUNTUACION && frase_separada->clase_pal!=CADENA_DE_SIGNOS) {
            separar_silabas(pal_extraida,pal_separada_silabas);
            legibilidad(pal_separada_silabas,aux,idioma);
            separar_silabas(aux,pal_separada_silabas);
            char *kk=pal_separada_silabas;
            frase_separada->n_silabas++;
            while (*kk++)
                if (*kk=='-')
                    frase_separada->n_silabas++;
            acentuar_prosodicamente(pal_separada_silabas,pal_acentuada,idioma);
            if (frase_separada->pal_transformada!=NULL) {
               if (strlen(frase_separada->pal_sil_e_acentuada)<=LONX_MAX_PAL_SIL_E_ACENTUADA-30) {
                  if (*frase_separada->pal_sil_e_acentuada) {
                     strcat(frase_separada->pal_sil_e_acentuada," ");
                  }
                  strcat(frase_separada->pal_sil_e_acentuada,pal_acentuada);
               }
            }
            else {
               strcpy(frase_separada->pal_sil_e_acentuada,pal_acentuada);
            }
         }
         else {
            strcpy(frase_separada->pal_sil_e_acentuada,pal_extraida);
         }
      } // END WHILE:Cada elem. de frase_separada
      frase_separada++;
      num_items++;
      if (frase_separada->pal_sil_e_acentuada) {
         strcpy(frase_separada->pal_sil_e_acentuada,"");
      }
   } // END WHILE PRINCIPAL
}
/******************************************************************************
 *                         END  MODULO: SIL_ACEN.                             *
 ******************************************************************************/
int legibilidad(t_palabra_proc ent, t_palabra_proc sal, char idioma){
   t_palabra_proc aux;
   char * p;
   char grupo[3];
   char * escribe=sal;
   char l[2],l_deletreada[30];


   strcpy(aux,ent);
   if (palabra_formada_por_solo_consonantes(ent)) {
      deletrear(aux,sal,idioma);
      return 0;
   }

   l[1]=0;
   *escribe=0;
   grupo[2]=0;

   p=strtok(aux,"-");
   while (*p && !vocal((unsigned char) *p)){
      if (*p!='y' && *(p+1)!='y' && *p!='h' && *(p+1)!='h' && consonante((unsigned char)*p) && consonante((unsigned char)*(p+1))) {
         *grupo=*p;
         *(grupo+1)=*(p+1);
         if (!grupo_consonantico_indivisible(grupo)){
            *l=*p;
            if (*l=='s') strcpy(l_deletreada,"es");
            else deletrear(l,l_deletreada,idioma);
            strcat(escribe,l_deletreada);
            escribe+=strlen(l_deletreada);
            *escribe=0;
            p++;
         }
         else {
            *escribe++=*p++;
            *escribe++=*p++;
            *escribe=0;
         }
      }
      else if (*(p+1)) {
         *escribe++=*p++;
         *escribe=0;
      }
      else {
         if (*p!='y' && consonante((unsigned char) *p)) {
            *l=*p;
            if (*l=='s') strcpy(l_deletreada,"es");
            else deletrear(l,l_deletreada,idioma);
            strcat(escribe,l_deletreada);
            escribe+=strlen(l_deletreada);
            p++;
         }
         else
            *escribe++=*p++;
         *escribe=0;
      }
   }
   strcat(escribe,p);
   escribe+=strlen(p);
   while (NULL!=(p=strtok(NULL,"-"))) {
      while (*p && !vocal((unsigned char) *p)){
         if (*p!='y' && *(p+1)!='y' && *p!='h' && *(p+1)!='h' && consonante((unsigned char)*p) && consonante((unsigned char)*(p+1))) {
            *grupo=*p;
            *(grupo+1)=*(p+1);
            if (!grupo_consonantico_indivisible(grupo)){
               *l=*p;
               if (*l=='s') strcpy(l_deletreada,"es");
               else deletrear(l,l_deletreada,idioma);
               strcat(escribe,l_deletreada);
               escribe+=strlen(l_deletreada);
               *escribe=0;
               p++;
            }
            else {
               *escribe++=*p++;
               *escribe++=*p++;
               *escribe=0;
            }
         }
         else if (*(p+1)) {
            *escribe++=*p++;
            *escribe=0;
         }
         else  {
            if (*p!='y' && consonante((unsigned char) *p)) {
               *l=*p;
               if (*l=='s') strcpy(l_deletreada,"es");
               else deletrear(l,l_deletreada,idioma);
               strcat(escribe,l_deletreada);
               escribe+=strlen(l_deletreada);
               p++;
            }
            else
               *escribe++=*p++;
            *escribe=0;
         }
      }
      strcat(escribe,p);
      escribe+=strlen(p);
   }
   *escribe=0;
   //strcpy(aux,sal);
   //separar_silabas(aux,sal);
   return 0;
}


void separar_silabas(const char * entrada,char * saida){
  char cacho[4];
  const char * letra=entrada;
  register char * escribe_sal=saida;
  char resto_palabra[1000];

  *saida=0;
// O principio de palabra avanzamos hasta a primeira vocal, excepto se a pala-
//  -bra empeza por q ou g + u , xa que se non hai que ter mais cuidado (debido
//  o tratamento da u muda en gui que, qui, que.
  while ((!vocal((unsigned char)*letra)) && (*(letra+1)!=FIN_DE_PALABRA) && !(
       (*letra=='q' || *letra=='g') && *(letra+1)=='u')) {
    *escribe_sal=*letra;
    letra++;
    escribe_sal++;
  }

  while (*letra){
    if (vocal((unsigned char)*letra)) {
      if (!vocal((unsigned char)*(letra+1))){ // unha soa vocal en medio de 2 consoantes
         *escribe_sal++=*letra;
      }
      else if (vocal_aberta((unsigned char)*letra) && vocal_aberta((unsigned char)*(letra+1))) {
        *escribe_sal++=*letra;
        *escribe_sal++='-';
        //separar_segun_modelo("V-",saida,letra);
      }
      else if (vocal((unsigned char)*(letra+1)) && vocal((unsigned char)*(letra+2))){
        if (!triptongo(letra)){
          if ( ((*(letra+1)==(char )i_con_dierese) || ((unsigned char) *(letra + 1) == i_acentuada) || ((unsigned char) *(letra + 1) == I_acentuada) ) &&
	       ( (!vocal_feble((unsigned char) *letra) || !vocal_feble((unsigned char) *(letra + 2)) ) ) ) {
            *escribe_sal++=*letra++;
            *escribe_sal++='-';
            *escribe_sal++=*letra++;
            *escribe_sal++='-';
            *escribe_sal++=*letra;
                // para comprobar 2 3 persoas de imperfectos con dierese
            //separar_segun_modelo("V-V-V",saida,letra);
          }
	  else if (diptongo(letra) && !vocal_aberta((unsigned char) *letra)) {
	    *escribe_sal++ = *letra++;
	    *escribe_sal++ = *letra;
	    *escribe_sal++ = '-';
	  }
          else {
            *escribe_sal++=*letra++;
            *escribe_sal++='-';
            *escribe_sal++=*letra++;
            *escribe_sal++=*letra;
            //separar_segun_modelo("V-VV",saida,letra);
          }
        }
        else {
            *escribe_sal++=*letra++;
            *escribe_sal++=*letra++;
            *escribe_sal++=*letra;
        }
      }
      else if ((vocal((unsigned char)*letra))   &&  vocal((unsigned char)*(letra+1))
                        &&   !diptongo(letra)){
    //se son duas vocais seguidas sin formar diptongo hai que separalas.
    //   Tamen se van separando deste xeito secuencias de tres ou mais le
    //   tras que non forman diptongo nin triptongo
      	separar_silabas(letra + 1, resto_palabra);
      	if ( ( ((*letra == 'u') || (*letra == 'ü')) && (*(letra + 1) == 'í') ) && comprueba_palabra_esdrujula(resto_palabra) ) {
	  *escribe_sal++=*letra++;
      	  *escribe_sal++=*letra;
      	}
      	else {
	  *escribe_sal++=*letra;
      	  *escribe_sal++='-';
      	  //separar_segun_modelo("V-",saida,letra);
      	}
      }
      else { // se non se axusta a ningun dos casos anteriores non se pon separador
         *escribe_sal++=*letra;
      }
    }//si vocal
    else if (consonante((unsigned char)*letra)) {
      if (solo_quedan_consonantes(letra)){
       // se solo quedan cons ó final engadense directamente a saida sen separar nada
        while (*letra) {
          *escribe_sal++=*letra++;
        }
        letra--;
        //escribe_sal--;
      }
      else if (caso_de_u_muda(letra)){ //casos quí guí nos que a u ca letra g ou q e "muda"
        if (letra==entrada){ //se é a primeira letra
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra;
          //separar_segun_modelo("CV",saida,letra);
        }
        else {
          if (escribe_sal != saida)
          	if (*(escribe_sal - 1) != '-')
	          *escribe_sal++='-';
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra;
          //separar_segun_modelo("-CV",saida,letra);
        }

      }
      else if (consonante((unsigned char)*letra) && vocal((unsigned char)*(letra+1))){
      //se se trata dunha soa consonante seguida de vocal segmentase antes da consonante -c
        if (*saida){
          if (escribe_sal != saida)
          	if (*(escribe_sal - 1) != '-')
		          *escribe_sal++='-';
          *escribe_sal++=*letra;
          //separar_segun_modelo("-C",saida,letra);
        }
        else{
          *escribe_sal++=*letra;
          //separar_segun_modelo("C",saida,letra);
        }
      }
      else if (secuencia_de_(2,CONSONANTE,letra)){ //se son 2 consonantes
        strncpy(cacho,letra,2);
        cacho[2]=0;
        if ( grupo_consonantico_indivisible(cacho)){
      /* se e un dos grupos enumerados nesa funcion segmentase antes das
          consoantes  -cc  , senon separase despois da primeira consoante*/
          if (escribe_sal != saida)
          	if (*(escribe_sal - 1) != '-')
	          *escribe_sal++='-';
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra;
          //separar_segun_modelo("-CC",saida,letra);
          if ( caso_de_u_muda(letra) ){
            *escribe_sal++='u';
            letra++;
          }
        }
        else {
          *escribe_sal++=*letra++;
          *escribe_sal++='-';
          *escribe_sal++=*letra;
          //separar_segun_modelo("C-C",saida,letra);
          if ( caso_de_u_muda(letra) ){
            *escribe_sal++='u';
            letra++;
          }
        }
      }
      else if  (secuencia_de_(3,CONSONANTE,letra)) {//se son 3 consonantes
        strncpy(cacho,letra+1,2);
        cacho[2]=0;
        if ( grupo_consonantico_indivisible(cacho) ){
      /* se e un dos grupos enumerados nesa funcion segmentase antes das
          consoantes c-cc  , senon separase antes da ultima consoante cc-c*/
          *escribe_sal++=*letra++;
          *escribe_sal++='-';
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra;
          //separar_segun_modelo("C-CC",saida,letra);
        }
        else{
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra++;
          *escribe_sal++='-';
          *escribe_sal++=*letra;
          //separar_segun_modelo("CC-C",saida,letra);
          if ( caso_de_u_muda(letra) ){
            *escribe_sal++='u';
            letra++;
          }
        }
      }
      else if  (secuencia_de_(4,CONSONANTE,letra)) {// 4 consonantes enton separase cc-cc
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra++;
          *escribe_sal++='-';
          *escribe_sal++=*letra++;
          *escribe_sal++=*letra;
        //separar_segun_modelo("CC-CC",saida,letra);
      }
      else {
        *escribe_sal++=*letra;
      }
    }//si consonante
	else
    	if (letra[0] == ' ')
            if (letra < entrada + strlen(entrada) - 1)
	        	*escribe_sal++ = '-';
    letra++;

  }//while (*letra)

  *escribe_sal=0;
}
 
int comprueba_palabra_esdrujula(char *palabra) {

  int caracteres_sin_tilde = strcspn(palabra, "áÁéÉíÍóÓúÚ");
  int numero_guiones = 1;
  char *ultima_silaba, *posicion_guion;

  if (caracteres_sin_tilde == strlen(palabra))
    // Si no hay tilde, no puede ser esdrújula
    return 0;

  posicion_guion = strstr(palabra, "-");

  if (posicion_guion == NULL)
    return 0;

  if (posicion_guion - palabra < caracteres_sin_tilde)
    return 0;

  while ( (posicion_guion = strstr(posicion_guion + 1, "-")) != NULL) {
    ultima_silaba = posicion_guion;
    numero_guiones ++;
  }

  if (numero_guiones < 2)
    return 0;

  // Por último, comprobamos si la última sílaba es un pronombre enclítico
  // Esto desde luego que no es un análisis correcto, pero en este momento del procesado no tenemos mucha más información.
  if ( (strcmp(ultima_silaba, "-se") == 0) ||
       (strcmp(ultima_silaba, "-te") == 0) ||
       (strcmp(ultima_silaba, "-nos") == 0) ||
       (strcmp(ultima_silaba, "-vos") == 0) ||
       (strcmp(ultima_silaba, "-me") == 0) ||
       (strcmp(ultima_silaba, "-che") == 0) ||
       (strcmp(ultima_silaba, "-lle") == 0) ||
       (strstr(ultima_silaba, "-lo") != NULL) ||
       (strstr(ultima_silaba, "-la") != NULL) )
    return 0;
  
  return 1;

}
