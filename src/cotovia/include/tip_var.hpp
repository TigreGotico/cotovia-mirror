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
 

#ifndef TIP_VAR_H
#define  TIP_VAR_H

#include <stdio.h>
//#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
using namespace std;

#define minimo(a, b) ( ((a) < (b)) ? a : b)
#define maximo(a, b) ( ((a) > (b)) ? a : b)

#define _copia_cadena(dst, src) {char *asdf=(dst);char *asdf2=(src);for(;*asdf2;*asdf++=*asdf2++);*asdf=0;}

#define LOCUCION(c) ( (c >= LOC_ADVE && c <= LOC_ADVE_DUBI) || \
      (c >= LOC_PREP && c<= LOC_PREP_CONDI)||               \
      (c >= LOC_CONX && c <= LOC_CONX_SUBOR_CORREL) )

#define ADVERBIO(c) (c >= ADVE && c <= ADVE_DISTR)

#define CONXUNCION(c) ( ((c) >= CONX_COOR) && ( (c) <= LOC_CONX_SUBOR_CORREL) )



#define TODOS_IDIOMAS    0
#define GALEGO           1
#define CASTELLANO       2


#define SUSPENSIVOS              '?'
#define FIN_DE_PALABRA          '\0'
#define NUM_MAX_FRASE            800
#define NUM_MAX_FRASE_FICH       500

/* Tamaños dos numeros.                                                       */
#define LONX_MAX_NUM              20
#define LONX_MAX_DECIMAL           6
#define MAX_CARAC_CARDORD          9
/* Macros para permitir unha mellor lectura do codigo do programa             */
#define ON                         1
#define OFF                        0

#define CON_ACENTO                 0
#define SIN_ACENTO                 1

#define EN_DICCIONARIO_INVERSO     1
#define EN_DICCIONARIO_NORMAL      2

//fmendez    genero y numero
//#define MASCULINO                  2
//#define FEMININO                   1

//#define SINGULAR                   1
//#define PLURAL                     2


#define NO_ASIGNADO               0
#define AMBIGUO                   1


#define MASCULINO                 2
#define FEMININO                  3
#define NEUTRO                    4
//#define MASCULINO_APOCOPADO		 5
//#define AMBIGUO_FEMININO          6

#define SINGULAR                   2
#define PLURAL                     3



//fmendez


#define ORD                        6

#define CLASIFICA_PALABRA     while(1)
#define SILABIFICAR           while(1)
#define SE_E_VOCAL            while(1)
#define SE_E_CONSONANTE       while(1)



#define VOCAL                      1
#define CONSONANTE                 2

/* Macros xenericas: dimensions de variables,  ect.                           */
#define LONX_VERBA                		  35   // Lonxitude maxima dunha verba

// FRAN_CAMPILLO: Éstaba definido en fun_verb.h, con valor 50. El valor anterior era 20.
#define TEMPOS_POSIBLES           		  20   // Nº. posibles analises da forma verbal
// FRAN_CAMPILLO

#define LONX_MAX_FRASE_SEPARADA      300   // en condicions normais 100
#define LONX_MAX_CARACTERES_FRASE    600//50//800   // en condicions normais 500

// FRAN_CAMPILLO: Aumento el valor de FRASE_SIL_E_ACENTUADA de 1000 a 2000 (Chapuza).
#define LONX_MAX_FRASE_SIL_E_ACENTUADA 5000
// FRAN_CAMPILLO: FRASE_EN_GRUPOS: 40 -> 100
#define LONX_MAX_FRASE_EN_GRUPOS         300


#define LONX_MAX_PALABRA                 60   // antes estaba a 20, pero daba problemas, xa que se presentan palabras mais longas, como transnacionais.
//#define LONX_MAX_PAL_SIL_E_ACENTUADA     50   // antes estaba a 40, pasa agora a 50
#define LONX_MAX_PAL_SIL_E_ACENTUADA     600   // antes estaba a 40, pasa agora a 50

//#define LONX_MAX_PALABRA_PROCESADA            200
#define LONX_MAX_PALABRA_PROCESADA            300

// FRAN_CAMPILLO: FRASE_SINTAGMADA: 100 -> 200
#define NUM_ELEMENTOS_FRASE_SINTAGMADA        300
// FRAN_CAMPILLO: FRASE_EN_PROPOSICIONS: 20 -> 50 ->100
#define NUM_ELEMENTOS_FRASE_EN_PROPOSICIONS    100

/* Estes defines utilizanse para abreviar o nome dalgunhas variables.         */
#define     palabra(x)     frase_separada[x].pal
#define     clase_pal(x)   frase_separada[x].clase_pal
#define     TIPO_PAL       elemento_de_frase->clase_pal

/************************************************************************/
/*                                                                      */
/*                   DEFINICION DE CATEGORIAS                           */
/*                                                                      */
/************************************************************************/

/*************************************/
/* CLASIFICACION FORMAL DAS PALABRAS */
/*************************************/

#define      NON_PROCESADA                     0
#define      SIN_CLASIFICAR                    1
#define      NUM_ROMANO                        3
#define      CARDINAL                          4
#define      ORDINAL_MASCULINO                 5
#define      ORDINAL_FEMENINO                  6
#define      VALOR_DE_GRADOS                   7
#define      NUMERO_CON_PUNTO                  8
#define      DATA                              9
#define      DECIMAL                          10
#define      PORCENTUAL                       11
#define      HORAS                            12
#define      PARTITIVO                        13
#define      CADENAS_DE_OPERACIONS            14
#define      ABREVIATURA                      15
#define      ESPACIO_BLANCO                   16
#define      INICIAL                          17
#define      PALABRA_NORMAL                   18
#define      PALABRA_NORMAL_EMPEZA_MAY        19
#define      PALABRA_TODO_MAIUSCULAS          20
#define      PALABRA_CON_GUION_NO_MEDIO       21
#define      SIGNO_PUNTUACION                 22
#define      NUMEROS_SEPARADOS_POR_GUIONS     23
#define      PALABRA_CONSONANTES_MINUSCULAS   24
#define      CARACTER_ESPECIAL                25
#define      PALABRA_CON_PUNTO_NO_MEDIO       26
#define      PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA  27
#define      VALOR_DE_GRADOS_DECIMAL                   28
#define      CADENA_DE_SIGNOS                   29
#define 	 DELETREO 30
#define      CONTRACCION_ARTIGO_MASCULINO_PREPOSICION_A 31
#define	     LETRA 32

/*
 * CATEGORIAS GRAMATICAIS EMPREGADAS
 */
#define N_CAT 15

#ifdef _MODELO_POS_COMPLETO
#define MAX_NUM_CATEGORIAS 209 //estas son las del modelo
#else
#define MAX_NUM_CATEGORIAS 50 // 209 //estas son las del modelo
#endif

#define ART_DET                        1
#define CONTR_CONX_ART_DET             2
#define CONTR_PREP_ART_DET             3
#define CONTR_INDEF_ART_DET            4

#define ART_INDET                      5
#define CONTR_PREP_ART_INDET           6

#define INDEF                          10
#define INDEF_PRON                     11
#define INDEF_DET                      12
#define CONTR_PREP_INDEF               13
#define CONTR_PREP_INDEF_DET           14
#define CONTR_PREP_INDEF_PRON          15

#define DEMO                           20
#define DEMO_DET                       21
#define DEMO_PRON                      22
#define CONTR_PREP_DEMO                23
#define CONTR_PREP_DEMO_DET            24
#define CONTR_PREP_DEMO_PRON           25
#define CONTR_DEMO_INDEF               26
#define CONTR_DEMO_INDEF_DET           27
#define CONTR_DEMO_INDEF_PRON          28
#define CONTR_PREP_DEMO_INDEF          29
#define CONTR_PREP_DEMO_INDEF_DET      30
#define CONTR_PREP_DEMO_INDEF_PRON     31

#define POSE                           35
#define POSE_DET                       36
#define POSE_PRON                      37
#define POSE_DISTR                     38

#define NUME                           40
#define NUME_DET                       41
#define NUME_PRON                      42
#define NUME_CARDI                     43
#define NUME_CARDI_DET                 44
#define NUME_CARDI_PRON                45
#define NUME_ORDI                      46
#define NUME_ORDI_DET                  47
#define NUME_ORDI_PRON                 48
#define NUME_PARTI                     49
#define NUME_PARTI_DET                 50
#define NUME_PARTI_PRON                51
#define NUME_MULTI                     52
#define NUME_COLECT                    53

#define PRON_PERS_TON                   60
#define CONTR_PREP_PRON_PERS_TON        61
#define PRON_PERS_AT                    62
#define PRON_PERS_AT_REFLEX             63
#define PRON_PERS_AT_ACUS               64
#define PRON_PERS_AT_DAT                65
#define CONTR_PRON_PERS_AT_DAT_AC       66
#define CONTR_PRON_PERS_AT_DAT_DAT_AC   67
#define CONTR_PRON_PERS_AT              68
#define INDET_PRON                      69
#define PRON_CORREL                     70
//#define PRON                            71 

#define ADVE                            80
#define ADVE_LUG                        81
#define ADVE_TEMP                       82
#define ADVE_CANT                       83
#define ADVE_MODO                       84
#define ADVE_AFIRM                      85
#define ADVE_NEGA                       86
#define ADVE_DUBI                       87
#define ADVE_ESPECIFICADOR              88
#define ADVE_ORD                        89
#define ADVE_MOD                        90
#define ADVE_COMP                       91
#define ADVE_CORREL                     92
#define ADVE_DISTR                      93

#define LOC_ADVE                        100
#define LOC_ADVE_LUG                    101
#define LOC_ADVE_TEMP                   102
#define LOC_ADVE_CANTI                  103
#define LOC_ADVE_MODO                   104
#define LOC_ADVE_AFIR                   105
#define LOC_ADVE_NEGA                   106
#define LOC_ADVE_DUBI                   107


#define PREP                            110
#define LOC_PREP                        111
#define LOC_PREP_LUG                    112
#define LOC_PREP_TEMP                   113
#define LOC_PREP_CANT                   114
#define LOC_PREP_MODO                   115
#define LOC_PREP_CAUS                   116
#define LOC_PREP_CONDI                  117


#define CONX_COOR                       120
#define CONX_COOR_COPU                  121
#define CONX_COOR_DISX                  122
#define CONX_COOR_ADVERS                123
#define CONX_COOR_DISTRIB               124
#define CONTR_CONX_COOR_COP_ART_DET     125

#define CONX_SUBOR                      130
#define CONX_SUBOR_PROPOR               131
#define CONX_SUBOR_FINAL                132
#define CONX_SUBOR_CONTRAP              133
#define CONX_SUBOR_CAUS                 134
#define CONX_SUBOR_CONCES               135
#define CONX_SUBOR_CONSE                136
#define CONX_SUBOR_CONDI                137
#define CONX_SUBOR_COMPAR               138
#define CONX_SUBOR_LOCA                 139
#define CONX_SUBOR_TEMP                 140
#define CONX_SUBOR_MODAL                141
#define CONX_SUBOR_COMPLETIVA           142
#define CONX_SUBOR_CONTI                143

#define LOC_CONX                        145
#define LOC_CONX_COOR_COPU              146
#define LOC_CONX_COOR_ADVERS            147

#define LOC_CONX_SUBOR_CAUS             150
#define LOC_CONX_SUBOR_CONCES           151
#define LOC_CONX_SUBOR_CONSE            152
#define LOC_CONX_SUBOR_COMPAR           153
#define LOC_CONX_SUBOR_CONDI            154
#define LOC_CONX_SUBOR_LOCAL            155
#define LOC_CONX_SUBOR_TEMP             156
#define LOC_CONX_SUBOR_MODA             157
#define LOC_CONX_SUBOR_CONTRAP          158
#define LOC_CONX_SUBOR_FINAL            159
#define LOC_CONX_SUBOR_PROPOR           160
#define LOC_CONX_SUBOR_CORREL           161

#define RELA                            170
#define INTER                           171
#define EXCLA                           172

#define NOME                            173
#define LOC_SUST                        174
#define NOME_PROPIO                     175

#define ADXECTIVO                       176
#define LOC_ADXE                        177

#define VERBO                           178
#define PERIFRASE                       179
#define INFINITIVO                      180
#define XERUNDIO                        181
#define PARTICIPIO                      182

#define INTERX                          183
#define LOC_INTERX                      184

#define LAT                             185
#define LOC_LAT                         186

//viejas categorías (es para que compile; hay que eliminarlas cuando se corrija todo
//#define RELATIVO_TONICO               187
#define NEXO_SUBORDINANTE               188
#define NEXO_COORDINANTE                189

// PALABRAS CON SIGNIFICADO LEXICO
#define SIGNO                              190
#define PUNTO                              191
#define COMA                               192
#define PUNTO_E_COMA                       193
#define DOUS_PUNTOS                        194
#define PUNTOS_SUSPENSIVOS                 195
#define APERTURA_INTERROGACION             196
#define PECHE_INTERROGACION                197
#define APERTURA_EXCLAMACION               198
#define PECHE_EXCLAMACION                  199
#define APERTURA_PARENTESE                 200
#define PECHE_PARENTESE                    201
#define APERTURA_CORCHETE                  202
#define PECHE_CORCHETE                     203
#define GUION                              204
#define DOBLES_COMINNAS                    205
#define SIMPLES_COMINNAS                   206
#define GUION_BAIXO                        207
// Fran Campillo: Para designar la ruptura prosódica que no coincide con pausa.
// Para poder reconocer estas situaciones al extraer la información del corpus
// se introduce un nuevo símbolo.
#define RUPTURA_ENTONATIVA                 208
// Fran Campillo: Designa aquellas comas que se realizan como una ruptura
// entonativa sin pausa.
#define RUPTURA_COMA                       209


// TIPOS DE SINTAGMAS
#define SINTAGMA_NOMINAL                     211
#define NEXO                                 212
#define SINTAGMA_PREPOSICIONAL               213
#define SINTAGMA_ADXECTIVAL                  214
#define SINTAGMA_VERBAL                      215
#define SINTAGMA_ADVERBIAL                   216

// DEFINES DE CATEGORIAS DE COMO SON AS PAUSAS E A ENTONACION
#define INICIO_INTERROGACION_TOTAL           221
#define FIN_INTERROGACION_TOTAL              222
#define INTERROGACION_TOTAL                  223
#define INICIO_EXCLAMACION                   224
#define FIN_EXCLAMACION                      225
#define EXCLAMACION                          226
#define INICIO_CORCHETE                      227
#define FIN_CORCHETE                         228
#define CORCHETE                             229
#define INICIO_PARENTESE                     230
#define FIN_PARENTESE                        231
#define PARENTESE                            232
#define INICIO_ENTRE_COMINNAS                233
#define FIN_ENTRE_COMINNAS                   234
#define ENTRE_COMINNAS                       235
#define INICIO_ENTRE_GUIONS                  236
#define FIN_ENTRE_GUIONS                     237
#define ENTRE_GUIONS                         238
#define ENUMERATIVO                          239
#define FIN_ENUMERATIVO                      240
#define DE_SUSPENSE                          241
#define INCISO                               242
#define ENUNCIATIVA                          243
#define INICIO_ENUNCIATIVA                   244
#define FIN_ENUNCIATIVA                      245
#define INICIO_INTERROGACION_PARCIAL         246
#define FIN_INTERROGACION_PARCIAL            247
#define INTERROGACION_PARCIAL                248

// TIPOS DE PROPOSICIONS
#define INT_DIRECTA                           250
#define INT_INDIRECTA                         251

#define CAT_NON_CLASIFICADA                   254
#define SINTAGMA_NON_CLASIFICADO              255




/*
 *
 * TAMAÑO DE LISTAS DE PALABRAS
 * OU DICCIONARIOS
 *
 */

#define NUM_ENTRADAS_DICCIONARIO_PAL_FUNCION  271
/*
 TAMAÑO DAS LISTAS QUE CONTEÑEN TERMINACIONS E PALABRAS CUN DETERMINADO
 TIPO DE ABERTURA DAS VOCAIS E e O. O TAMAÑO E NECESARIO PARA PODER IM-
 PLEMENTAR UNHA BUSCA BINARIA.
*/
#define NU_ESDR_PALABRAS_PECHADAS              25
#define NU_ESDR_TERMINACIONS_PECHADAS           9
#define NU_GRAVES_PALABRAS_PECHADAS           121
#define NU_GRAVES_TERMINACIONS_ABERTAS         18
#define NU_GRAVES_PALABRAS_ABERTAS             92
#define NU_GRAVES_TERMINACIONS_PECHADAS        77
#define NU_AGUDAS_PALABRAS_ABERTAS             25
#define NU_AGUDAS_TERMINACIONS_ABERTAS          6
/*
 Tamaño dos diccionarios de palabras nos que o x e o w se pronuncia de
 distintas formas.
*/
// FRAN_CAMPILLO: Cambiamos las dimensiones de esta lista:
//#define NU_PAL_X_PRON_COMO_KS                 113
#define NU_PAL_X_PRON_COMO_KS                 114
// FRAN_CAMPILLO

#define NU_PAL_X_PRON_COMO_XE                  16
#define NU_PAL_W_PRON_COMO_GU                   9
#define NU_PAL_W_PRON_COMO_U                    2
/*
 Tamaño das listas de verbos,desinencias, e encliticos.
*/
// FRAN_CAMPILLO: Ya están definidas en verbos.h
//#define NU_RAICES_DE_VERBOS                  8243
//#define NU_DESINENCIAS                        738
// FRAN_CAMPILLO
#define NU_VERBOS_NON_DESCOMPONIBLES          112
//#define NU_PRON_ENCLITICOS                     119
/*
  Tamaño das listas de "traduccions", para galego e castelan.
*/
#define NU_ABECEDARIO                          36    // Para GALEGO
#define NU_CAST_ABECE                          36     // Para castelan
//#define NU_ACRONIMOS                          129     // Para GALEGO
//#define NU_CAST_ACRON                           8     // Para castelan
//#define NU_ABREVIATURAS                       557     // Para GALEGO
//#define NU_ABREVS_CAST                        381     // Para castelan
// FRAN_CAMPILLO: Modificación para el corpus: 33 -> 32.
//#define NU_ABREVIATURAS_EXT_SIMPLE             32     // Para GALEGO
// FRAN_CAMPILLO.
//#define NU_ABREVS_EXT_SIMPLE_CAS               33     // Para castelan
//#define NU_ABREVIATURAS_EXT_COMPLE             14     // Para GALEGO
//#define NU_ABREVS_EXT_COMPLE_CAS                4     // Para castelan



/*
  Tamaño da lista de palabras que presentan homografia na forma como nome/verbo.
*/
#define NU_HOMOGRAFAS_NOME_VERBO               26


/************************************************************************/
/*                                                                      */
/*                   DEFINICION TIPOS DE VARIABLES                      */
/*                                                                      */
/************************************************************************/

typedef  char t_palabra[LONX_MAX_PALABRA];
typedef  char t_palabra_proc[LONX_MAX_PALABRA_PROCESADA];
typedef  char t_palabra_sil_e_acentuada[LONX_MAX_PAL_SIL_E_ACENTUADA];

// FRAN CAMPILLO // Tuve que aumentar el tamaño de cadena a 21, porque en un momento escribe en una
                 // variable de ese tipo "Final sin puntuación", que es de 20 caracteres.
typedef  char cadena[21]; // Anteriormente era 20.

// FRAN CAMPILLO

typedef  struct {
   unsigned char *pal;
   unsigned char cat_gramatical[4];
}  t_rexistro_diccionario_pal_funcion;
/* Tipo de rexistro que se emprega na lista que conten o diccionario de
    palabras-funcion ca sua categoria gramatical.                             */

typedef struct {
   const char *palabra;
   const char *traduc;
}t_tabla_traduc;

struct numlet {
   char card[7];
   char ord[9];
 };

struct tbb {
   char abr[10];
   char ext[40];
};
/* Tipo das estructuras que conteñen os datos das abreviaturas. No primeiro
    campo esta a abreviatura e no segundo campo a extension.                  */

typedef struct {
   cadena continuacion_texto;
   cadena prosodia_frase;
} t_tipo_frase;

typedef struct {
   char enclitico[LONX_VERBA];
//   char reconstruccion[LONX_VERBA];
   int grupo;
}t_enclitico;
typedef struct {
   const char * enclitico;
   int grupo;
}t_dic_enclitico;

typedef struct {                    
   char infinitivo[LONX_VERBA];
   char raiz[LONX_VERBA];
	 char desinencia[LONX_VERBA];
   char reconstruccion[LONX_VERBA];
   //fmendez
   t_enclitico enclitico;
   char artigo[LONX_VERBA];
   //fmendez
   int modelo;
} t_infinitivo; /// Estructura da info. verbal complementaria

typedef enum {IPRE=0,IIM,IPER,IPLUS,IFU,IFUPRE,SPRE,SIM,SFU,IMPE,INCON=11,XE,PA} temp_verb;


typedef struct {
	t_palabra pal; ///A palabra tal e como se extrae da frase
	char *pal_transformada;/// A palabra normalizada (todo en minusculas) ou como no caso dos números estarán en letra.
	char *tr_fon;///transcripción fonética
	t_palabra_sil_e_acentuada pal_sil_e_acentuada;/// Silabificacion da palabra
	unsigned char clase_pal;/// Clasificacion da palabra atendendo o tipo dos caracteres que a forman: Palabra que empeza por maiusculas, abreviatura, numero... 
	unsigned char cat_gramatical[N_CAT];/// Distintas posibilidades das categorias gramaticais que pode ter unha palabra; Por exemplo que=>Pronome relativo ou conxuncion relativa.

   unsigned char cat_gramatical_descartada[N_CAT];
	 //#ifdef _SACA_CLASES_AMBIGUEDAD // lo quito porque da problemillas
   unsigned char clase_ambiguedad[MAX_NUM_CATEGORIAS];
//#endif
	 unsigned char cat_verbal[TEMPOS_POSIBLES]; ///En caso de ser verbo este e un codigo que indica o tempo, modo, conxugacion,
   ///numero,persoa,modo e conxugacion; se esta a cero indica que non e verbo. Este 
	 ///campo haino que modificar segundo se cambie tamen o valor da variable
   ///lista_de_tempos_verdadeiros no modulo morfolo.c, así mesmo tamen hai que ter
   ///coidado coa copia da variable mencionada a este campo pois faise cun strncpy
	 ///co que hai que ter coidado de que se copia a mesma lonxitude.
   t_infinitivo inf_verbal[TEMPOS_POSIBLES];

//giglesia: Añadimos un campo para el lema.
	 t_palabra lema;///lema
	 t_palabra raiz;///raíz
	 t_palabra sufijo;///sufijo
	 unsigned char xenero;///Variable par conter a analise de xenero se se necesita
	 unsigned char numero;/// Variable par conter a analise de xenero se se necesita
	 unsigned char tipo_sintagma;/// Pode conter o tipo de sintagma o que pertence esta palabra
   unsigned char tonicidad_forzada;
   int n_silabas;
   int pausa;
   int anade_categoria(char cat);
	 float prob;
}t_frase_separada;/// Tipo da variable que contén información sobre a frase.


typedef struct {
	t_palabra token;
	int n_palabras;
	char * pal_transformada;
	//vector<string> palabras;
	t_frase_separada *palabras;
	unsigned char clase_pal;
} Token;


typedef struct {
   const char* antecedente; /// Trozo de string que se se cumple indica que se debe sustituir polo consecuente.
   char lonx_a_sustituir;///Trozo de string anterior que se debe adiantar.
   const char* consecuente;/// Trozo sustitutivo
} t_regla;           /// Estructura que se usa para o paso de letras a alofonos

typedef struct {
   char *palabras;
   unsigned char tipo_conxuncion;
   unsigned char num_pal_conxuncion;
} t_conxuncion;
/* Tipo de variable usada para conter os datos na lista de conxuncions. O pri-
   -meiro elemento campo contén as palabras que forman a locucion conxuntiva,
   o segundo un codigo que a clasifica e o terceiro o numero de palabras da
   conxuncion.                                                                */

typedef struct {
   unsigned char tipo_sintagma;
   unsigned char funcion;
   int inicio;
   int fin;
   int pausa;
   int n_silabas;

} t_frase_sintagmada;
/* Tipo de variable que contera un sintagma. Inicio e fin son os indices de
   frase separada onde empeza e acaba. Os outros dous campos serven para
   clasificala.                                                               */
typedef struct {
   int inicio;
   int fin;
   unsigned char tipo_proposicion;
   unsigned char tipo_tonema;
   unsigned char tipo_inicio[2];
   unsigned char tipo_fin[2];
   unsigned char enlazado;
// FRAN_CAMPILLO: Para considerar la ruptura entonativa:
   unsigned char ruptura_entonativa;
// Si vale 1, hay ruptura entonativa. Si vale 2, implica que hay una coma que se
// realizó como una ruptura entonativa, sin pausa. En otro caso vale 0.
// En principio pensé en añadir
// esa información en el tipo de tonema, pero cualquier tonema puede acabar
// en una ruptura entonativa en lugar de pausa.
}t_frase_en_grupos;
/* Tipo de variable que contera o que denominamos grupo. O grupo estara forma-
   -do por varios sintagmas entre pausas ou que constituen unha proposicion co-
   -mo no caso das subordinadas. Inicio e fin son os indices de frase_sintagna-
   -da onde empeza e acaba este grupo. Os outros campos serven para clasificala*/

typedef struct{
   char alof[4];
   int dur1;
   int dur2;
   int pitch1;
   int pitch2;
   int pitch3;
   unsigned char tonica;
   unsigned char ti_prop;
   unsigned char pos_grupo_prosodico;
   unsigned char pos_silabica;
   unsigned char pos_segmento;
   unsigned char pos_tonica;
	 short int pga;               /// Posicion do grupo acentual dentro de toda a frase
	 short int pp;                /// Posicion da palabra  dentro de toda a frase
	 short int ps;                /// Posicion da silaba dentro de toda a frase
	 unsigned char sil_fin;           /// Indica a distancia o final na silaba final. O rango variacion: 1(ultimo) a 4
	 unsigned char lonx_sil;          /// Lonxitude da silaba en numero de alofonos
	 unsigned char estr_sil;          /// 1->onset 2=nucleo, 3->coda
} t_prosodia;

/************************************************************************/
/*                                                                      */
/*                DEFINICIONES DE CONSTANTES Y ESTRUCTURAS              */
/*                                                                      */
/************************************************************************/
#define  ALPHA                   1
#define  FACTOR                  1
#define  FS                  16000
#define  TSORDOS              0.01
#define  NUM_SONS_MAX            4  /// num. max. de alofonos dentro de una unidad
#define  NUM_FON_MAX          1000
#define  NUM_UNITS_MAX        1000
#define  TAM_INDICE           1120
#define  NUM_BLQ_MAX_UNIT        5
#define  NUM_BLQ_MAX_UNIT_ESPERA 2
#define  PI             3.14159265

#ifdef _MODOA_EU
#define  PITCH_M_MAX           512 //256  /// num. max. de marcas de pitch en una unidad
#else
#define PITCH_M_MAX           256
#endif

#define  TAM_VENTANA ((short int) (2*FACTOR*MAXPITCH+1))
#define  TAM_MED_VENTANA ((short int) (FACTOR*MAXPITCH))
#define  MAXPITCH (FS/50)
#define  MEDPITCH (FS/100)
#define  MINPITCH (FS/150)
//#define  tam_unidad (18+8*NUM_SONS_MAX+8*PITCH_M_MAX)
//#define  MOSTRAS               256
//#define  TAM_BLOQUE (MOSTRAS*sizeof(short int))
//#define  TAM_VECTOR (TAM_BLOQUE*100)
//#define TAM_VECTOR (25600 * sizeof(short int))
#define TAM_VECTOR (100 * PITCH_M_MAX * sizeof(short int))
//fmendez
#define AUDIO   0x01
#define F8KHZ   0x02
#define FWAV    0x04

//fmendez


typedef struct {
	int bddur;
	int bd;
	int et;           /// Valor da escala temporal
	int ep;           /// Valor da escala de Pitch
	char idioma;
	char wav;         /// 0 sin fichero wav 1 se genera fichero wav
	char f8;          /// 0 frecuencia 16 khz 1 frec. 8 khz
	char audio;       /// 1 se saca audio por tarjeta sonido, 0 no
	char info_unidades; /// == 1 saca información sobre las unidades escogidas.
	char noprepro;
	char unalinea;
	char separa_lin;
	char tra;
	char lin;
	char fon; /// == 1 escribe en un fichero la secuencia de fonemas que componen la frase
	char pre;
	char fstdin;
	char fstdout;
	char entrada_por_fichero;
	char locutores[FILENAME_MAX];
	char lenguajes[FILENAME_MAX];
	char dir_sal[FILENAME_MAX];
	char dir_bd[FILENAME_MAX];
	char fsalida[FILENAME_MAX];
	char fentrada[FILENAME_MAX];
	char fich_tra[FILENAME_MAX];
	char fich_morfosintactico[FILENAME_MAX]; /// si se incluye como parámetro, se lee el
	/// análisis morfosintáctico del fichero indicado.
	char fich_conf[FILENAME_MAX];	/// Nombre del fichero de configuración del proyecto del corpus. Se emplea
	/// en los modos destinados a extraer información del corpus.
	char fich_unidades[FILENAME_MAX];	/// Nombre del fichero con las unidades candidatas que se desean emplear en la síntesis.
	/// (crea la voz sintética obviando el módulo lingüístico)
	char fich_uso_unidades[FILENAME_MAX]; /// Nombre del fichero en el que se escribe la información de las unidades acústicas (semifonemas) que se escogieron para la síntesis.
	int N; /// Número total de contornos generados en la búsqueda entonativa.
	int M; /// Número de contornos que pasan a la búsqueda acústica.
	char carga_todo;
	char entrada_con_timbre;
	char por_palabras;
	char difo;
	char alternativas;
	char ftra;
	char lista_ficheros;
	char ficheros_segmentacion;
	char separa_token;
	unsigned char fuerza_fronteras; /// Si es cero, se deja que el modelo entonativo decida la posición de las fronteras entonativas. En otro caso, se decide según lo que se indique en la frase de entrada.
	int tipo_seleccion_unidades; /// Si es 0, se hace el cálculo en paralelo. 
	/// 1: cálculo secuencial para cada contorno.
	/// 2: cálculo secuencial, pero dividiendo la selección de unidades acústicas en segmentos limitados por silencios, oclusivas sordas o fricativas sordas.
	/// 3: cálculo en paralelo y dividiendo en segmentos.
	/// 4: cálculo dividiendo en segmentos, y recibiendo los contornos organizados por grupos fónicos.
	/// 5: cálculo en paralelo y dividiendo en segmentos, recibiendo los contornos organizados por grupos fónicos.
	int tipo_seleccion_entonativa; /// Si es 0, se calcula un contorno global
	/// 1: Cálculo por grupos fónicos, escogiendo un contorno global para toda la frase.
	/// 2: Cálculo por grupos fónicos, devolviendo variedad para cada grupo fónico por separado.
	int tipo_candidatos; /// Si es 0, se consideran como candidatos todos los semifonemas de un tipo. Si no, sólo los que coincidan en carácter tónico, posición y tipo de frase
	float factor;
	float factor2;
	int modificacion_prosodica; // 0, no se realiza nunca; 1, se realiza siempre; 2, no se realiza, pero no se copian tal cual unidades consecutivas; otro valor: se realiza según los umbrales.
	int hts;
} t_opciones;


#define opciones_por_defecto {0,0,100,100,GALEGO,0,0,0,0,0,0,0,0,0,0,0,0,0,1,"sabela","gl",".","","","","","","","","",50,10,0,0,0,0,0,0,0,0,0,0,5,2,0,0.5,0.5,3,0}


#endif

