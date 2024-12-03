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
#include "errores.hpp"
#include "sintagma.hpp"
#include "timbre.hpp"
#include "verbos.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp"    
#include "trat_fon.hpp"


extern t_conxuncion *conx2;


/******************************************************************************
 *                LISTAS DE VERBAS A TRATAR FONETICAMENTE                     *
 *   As palabras contidas nas listas de terminacions encontranse invertidas   *
 * debido a que a ordenacion que se utiliza para ordena-las listas e tamen in-*
 * -versa.                                                                    *
 ******************************************************************************/
static const char *esdr_terminacions_pechadas[NU_ESDR_TERMINACIONS_PECHADAS] = {
  "ué",
  "uó",
  "adebé",
  "adevé",
  "arepsé",
  "ocirtné",
  "olaté",
  "oramó",
  // FRAN_CAMPILLO: Sobraba la última coma.
  "ovacnó"
  // FRAN_CAMPILLO
};

static const char *esdr_palabras_pechadas[NU_ESDR_PALABRAS_PECHADAS] = {
  "alvéolo",
  "améndoa",
  "auténtico",
  "bébedo",
  "bévera",
  "bóchega",
  "céreo",
  "cóbado",
  "cónchega",
  "débeda",
  "etcétera",
  "lévedo",
  "lóstrego",
  "ménade",
  "océano",
  "paréntese",
  "pértega",
  "sésega",
  "trémaro",
  "témporas",
  "tépeda",
  "tódalas",
  "tódolos",
  "véneto",
  "xémara"
};

static const char *graves_terminacions_pechadas[NU_GRAVES_TERMINACIONS_PECHADAS] = {
  "abe",
  "acob",
  "acrec",
  "acse",
  "acso",
  "ade",
  "ado",
  "adro",
  "ae",
  "ahce",
  "ahcoc",
  "ahcre",
  "alle",
  "allo",
  "amo",
  "arep",
  "arie",
  "ario",
  "aro",
  "arro",
  "arte",
  "arto",
  "ase",
  "aso",
  "ate",
  "atse",
  "axe",
  "axo",
  "aze",
  "azo",
  "azro",
  "ede",
  "emon",
  "erbo",
  "ete",
  "etneulf",
  "obe",
  "obo",
  "ocse",
  "ocso",
  "ode",
  "odo",
  "odro",
  "oe",
  "ohce",
  "oio",
  "oixe",
  "olle",
  "ollo",
  "olobec",
  "omert",
  "omo",
  "onro",
  "opo",
  "orde",
  "orie",
  "orio",
  "oro",
  "orro",
  "oso",
  "osro",
  "ote",
  "oto",
  "otrop",
  "otsec",
  "ove",
  "ovoc",
  "oxe",
  "oxo",
  "ozere",
  "ozev",
  "ozo",
  "ozro",
  "sece",
  "sere",
  "sero",
  // FRAN_CAMPILLO: Sobra la última coma.
  "sese"
  // FRAN_CAMPILLO
};

static const char *graves_palabras_pechadas[NU_GRAVES_PALABRAS_PECHADAS] = {
  "aborto",
  "absorto",
  "aceso",
  "achega",
  "achego",
  "adega",
  "agosto",
  "ampola",
  "arrolo",
  "atropelo",
  "bancarrota",
  "becerra",
  "becerro",
  "boa",
  "bocha",
  "bodega",
  "bodego",
  "boga",
  "bola",
  "bolo",
  "botaporela",
  "cabelo",
  "cachelo",
  "carambelo",
  "caramelo",
  "cebola",
  "cebolo",
  "cebra",
  "centola",
  "centolo",
  "cepa",
  "cera",
  "cerro",
  "chepa",
  "coco",
  "cogomelo",
  "colega",
  "corpo",
  "cotelo",
  "crisantemo",
  "desenrolo",
  "deuses",
  "dobra",
  "doce",
  "engorde",
  "escoba",
  "espeso",
  "este",
  "estes",
  "estrela",
  "estrema",
  "estrema",
  "expreso",
  "febra",
  "felo",
  "femia",
  "galego",
  "gota",
  "grego",
  "grelo",
  "grolo",
  "hoxe",
  "ileso",
  "impreso",
  "lagosta",
  "magosto",
  "manchego",
  "mangosta",
  "marmelo",
  "media",
  "medio",
  "mesto",
  "metro",
  "moca",
  "moco",
  "moega",
  "morbo",
  "mosto",
  "moxega",
  "negro",
  "noca",
  "noelo",
  "obseso",
  "oda",
  "ola",
  "omega",
  "peche",
  "pega",
  "pelo",
  "peso",
  "pexego",
  "pola",
  "polo",
  "preso",
  "rebolo",
  "rede",
  "rego",
  "remo",
  "repolo",
  "rolo",
  "rostro",
  "rota",
  "sartego",
  "seca",
  "soba",
  "sopa",
  "sorna",
  "supremo",
  "tempero",
  "teso",
  "testo",
  "toco",
  "tola",
  "tolo",
  "torre",
  "torre",
  "trece",
  "verde",
  "voo",
  "xeso",
  // FRAN_CAMPILLO: Sobra la última coma.
  "xofre"
  // FRAN_CAMPILLO
};

static const char *graves_palabras_abertas[NU_GRAVES_PALABRAS_ABERTAS] = {
  "adrede",
  "alboio",
  "amora",
  "amorodo",
  "anexo",
  "antonte",
  "arestora",
  "beladona",
  "borda",
  "brosa",
  "cadora",
  "calabozo",
  "ceo",
  "choio",
  "cobre",
  "coio",
  "completo",
  "connosco",
  "convexo",
  "convosco",
  "corda",
  "corno",
  "credo",
  "creto",
  "croio",
  "defensa",
  "demora",
  "demoño",
  "diabete",
  "diarrea",
  "dieta",
  "dona",
  "dueto",
  "festa",
  "flexo",
  "foresta",
  "fosa",
  "frecha",
  "fresa",
  "glosa",
  "grosa",
  "groso",
  "heterodoxo",
  "hiena",
  "idea",
  "indefensa",
  "isoglosa",
  "madona",
  "marea",
  "meteoro",
  "moda",
  "modo",
  "mora",
  "nantronte",
  "nexo",
  "noitevella",
  "nora",
  "nosa",
  "noso",
  "noutronte",
  "noutrora",
  "ofensa",
  "ollo",
  "onte",
  "ortodoxo",
  "outrora",
  "paradoxo",
  "pesca",
  "pobre", 
  "pomba",
  "ponla",
  "porra",
  "prensa",
  "preto",
  "prosa",
  "protesta",
  "quente",
  "resta",
  "reto",
  "rosa",
  "sempre",
  "sesta",
  "sexo",
  "toma",
  "trasantonte",
  "vella",
  "vello",
  "venres",
  "vosa",
  "voso",
  "xesta",
  // FRAN_CAMPILLO: Sobra la última coma.
  "zorza"
  // FRAN_CAMPILLO
};

static const char *graves_terminacions_abertas[NU_GRAVES_TERMINACIONS_ABERTAS] = {
  "ednoc",
  "aicne",
  "aine",
  "oine",
  "esne",
  "aino",
  "oino",
  "etnei",
  "etneu",
  "atneroc",
  "atneucnic",
  "atneses",
  "atnetes",
  "atnetio",
  "atnevon",
  "atnec",
  "otnec",
  // FRAN_CAMPILLO: Sobraba la última coma
  "aicneu"
  // FRAN_CAMPILLO
};

static const char *agudas_terminacions_abertas[NU_AGUDAS_TERMINACIONS_ABERTAS] = {
  "le",
  "lo",
  "ne",
  // FRAN CAMPILLO: Faltaba una coma.
  // FRAN_CAMPILLO
  "né",
  "zo",
  "é"
};

static const char *agudas_palabras_abertas[NU_AGUDAS_PALABRAS_ABERTAS] = {
  "cafés",
  "cempés",
  "chalés",
  "ciprés",
  "comités",
  "có",
  "cós",
  "don",
  "e",
  "fe",
  "fel",
  "gres",
  "ideal",
  "nós",
  "pés",
  "quer",
  "repousapés",
  "res",
  "rés",
  "só",
  "través",
  "tés",
  "vós",
  "ó",
  // FRAN_CAMPILLO: Sobra la última coma.
  "ós"
  // FRAN_CAMPILLO
};

/*****************************************************************************
 *****************************************************************************/
static const char *w_pronunciase_gu[NU_PAL_W_PRON_COMO_GU] = {
  "darwi",
  "hawa",
  "sandwi",
  "taiwa",
  "walk",
  "wash",
  "whisk",
  "winch",
  "wind"
};

static const char *w_pronunciase_u[NU_PAL_W_PRON_COMO_U] = {
  "twist",
  "newto"
};

/*****************************************************************************
 *****************************************************************************/
static const char *x_pasa_a_ks[NU_PAL_X_PRON_COMO_KS] = {
  "amplex",
  "anafilax",
  "anaptix",
  "anex",
  "anorex",
  "anoréx",
  "antitoxi",
  "apirex",
  "apodix",
  "aprox",
  "asex",
  "asfix",
  "atarax",
  "atax",
  "autotax",
  "auxi",
  "auxo",
  // FRAN_CAMPILLO: De momento eliminamos este inicio, ya que se carga axenda, axexar...
  //   "axe",
  // FRAN_CAMPILLO: NOTA: hay que modificar el tip_var.h
  "axes",
  "axia",
  "axila",
  "axio",
  "axoi",
  "axom",
  "baux",
  "biconvex",
  "bisex",
  "boxe",
  "caquex",
  "carbox",
  "catalex",
  "coax",
  "coex",
  // FRAN_CAMPILLO: complexo no es con ks -> cambiamos complex por complexi
  "complexió",
  // FRAN_CAMPILLO
  "conex",
  "convex",
  "coxal",
  "crucifixi",
  "desconex",
  "desintoxi",
  "desoxi",
  "dexio",
  "dislex",
  "dox",
  "eflux",
  "epistax",
  "ex",
  "filoxer",
  "flex",
  "fluxi",
  "fluxom",
  "galaxi",
  "heterodox",
  "heterosex",
  "hexa",
  "hexo",
  "hidroxi",
  "homosex",
  "index",
  "inflex",
  "intermax",
  "intox",
  "irreflex",
  "ixi",
  "ixo",
  "laxan",
  "laxat",
  "laxi",
  "laxa",
  "laxo",
  "lexem",
  "léxic",
  //"lux",
  "luxa",
  "marxis",
  "maxil",
  "máxim",
  "mitilotox",
  "mix",
  "monoxi",
  "morfoxintax",
  "nex",
  "ortodox",
  "ox",
  "paralax",
  "paratax",
  "parox",
  "pirex",
  "profilax",
  "proxene",
  // FRAN_CAMPILLO: Añadimos proxi (por proximamente...)
  "proxi",
  // FRAN_CAMPILLO
  "próxi",
  "radiotax",
  "reex",
  "reflexi",
  "retroflex",
  "sax",
  "sexis",
  "sexo",
  "sext",
  "sexua",
  "sexy",
  "sintax",
  "six",
  "submax",
  "taxat",
  "taxi",
  "taxo",
  "tirox",
  "toxem",
  "toxi",
  "toxop",
  "unisex",
  "ux",
  "vexil",
  //"xero",
  // FRAN_CAMPILLO: Añadimos "éxito".
  "éxito"
  // FRAN_CAMPILLO
};

static const char *pronuncianse_con_xe[NU_PAL_X_PRON_COMO_XE] = {
  "complexo",
  "exacu",
  "execu",
  "exem",
  "exerc",
  "exerd",
  "exip",
  "exérc",
  "oxalá",
  "oxiv",
  "saxit",
  "saxon",
  "uxí",
  "xerogl",
  "xeron",
  "xeros"
};

/*****************************************************************************
 *****************************************************************************/
int hai_grupo_eu_ou_na_antepenultima_silaba(char *pal)
{
  unsigned char n_sil = 1;
  char *fin_silaba, *origen_palabra = pal, terceira_silaba[7];

  pal = pal + strlen(pal) - 1;

  while (pal != origen_palabra && !n_sil == 3) {
    if (*pal == '-')
      n_sil++;
    pal--;
  }
  fin_silaba = pal;
  while (pal != origen_palabra && *(pal - 1) != '-')
    pal--;
  strncpy(terceira_silaba, pal, fin_silaba - pal + 1);
  terceira_silaba[fin_silaba - pal + 1] = 0;

  if (strstr((char *) terceira_silaba, "e^u") ||
      strstr((char *) terceira_silaba, "o^u"))
    return 1;
  return 0;
}

/**   \fn  int timbre_en_agudas(char *palabra)

      TIMBRE_EN_AGUDAS                                
      Funcions que comproban as terminacions das agudas para timbre aberto, ou
      as execepcions para as palabras que son abertas en oposicion a regra xeral
      en que son pechadas.                                                      

      \param unsigned char palabra -> palabra a buscarlle o timbre (IN).
      \return  1 se timbre e aberto e 0 se e pechado (OUT).
*/
int timbre_en_agudas(char *palabra)
{
  t_palabra pal = "";

  strcpy(pal, palabra);
  if (comprobar_en_lista_de_palabras
      ((char **) agudas_palabras_abertas, sizeof(char *),
       NU_AGUDAS_PALABRAS_ABERTAS, pal) >= 0)
    return VOC_ABERTA;
  if (pal[strlen(pal) - 1] == 's')
    pal[strlen(pal) - 1] = 0;	// As terminacions buscanse en singular
  if (comprobar_en_lista_de_inicio_de_palabras
      ((char **) agudas_terminacions_abertas, sizeof(char *),
       NU_AGUDAS_TERMINACIONS_ABERTAS, pal, EN_DICCIONARIO_INVERSO) >= 0)
    return VOC_ABERTA;
  return VOC_PECHADA;
}

/** \fn  int estan_en_contacto_con_nasal(char *pal_sil_ac)

    Funcion que devolve timbre aberto no caso de que tenhamos un m
    en posicion implosiva (m-) sendo o nucleo silabico na penultima silaba, isto e,
    grave, devolvendo pechado en caso contrario.
    \author giglesia (modificado)
    \param pal_sil_ac -> palabra a buscarlle o timbre (IN).
    \return  1 se timbre e aberto e 0 se e pechado (OUT).
*/
int estan_en_contacto_con_nasal(char *pal_sil_ac)
{
  char *anda_hacia_atras = &pal_sil_ac[strlen(pal_sil_ac) - 1];

  while (anda_hacia_atras != pal_sil_ac && *anda_hacia_atras != '-')
    anda_hacia_atras--;
  if (anda_hacia_atras == pal_sil_ac)
    return 0;
  if (*(anda_hacia_atras - 1) == 'm' || *(anda_hacia_atras - 1) == 'n' || *(anda_hacia_atras - 1) == 'ñ') {
    if (*(anda_hacia_atras - 2) == '^'
	&& (*(anda_hacia_atras - 3) == 'é'
	    || *(anda_hacia_atras - 3) == 'ó'
	    || *(anda_hacia_atras - 3) == 'e'
	    || *(anda_hacia_atras - 3) == 'o'))
      return 1;
  }
  else if (*(anda_hacia_atras + 1) == 'n' || *(anda_hacia_atras + 1) == 'ñ') {  //n y ñ también pueden ir en posición explosiva.
    if (*(anda_hacia_atras - 1) == '^'
	&& (*(anda_hacia_atras - 2) == 'é'
	    || *(anda_hacia_atras - 2) == 'ó'
	    || *(anda_hacia_atras - 2) == 'e'
	    || *(anda_hacia_atras - 2) == 'o'))
      return 1;
  }
  return 0;
}

/**  \fn int timbre_en_graves(char *pal, char *pal_sil_ac)
     Devolve 1 en abertas e 0 en pechadas.
*/
int timbre_en_graves(char *pal, char *pal_sil_ac)

{
  t_palabra pal_en_singular = "";
  strcpy(pal_en_singular, pal);
  if (pal_en_singular[strlen(pal_en_singular) - 1] == 's')
    pal_en_singular[strlen(pal_en_singular) - 1] = 0;
  /*se e plural eliminase xa que nas listas estan en singular as terminacions
    podendo ser plural */

  if (comprobar_en_lista_de_palabras
      ((char **) graves_palabras_abertas, sizeof(char *),
       NU_GRAVES_PALABRAS_ABERTAS, pal_en_singular) >= 0)
    return VOC_ABERTA;

  if (comprobar_en_lista_de_palabras
      ((char **) graves_palabras_pechadas, sizeof(char *),
       NU_GRAVES_PALABRAS_PECHADAS, pal_en_singular) >= 0)
    return VOC_PECHADA;

  if (comprobar_en_lista_de_inicio_de_palabras
      ((char **) graves_terminacions_abertas, sizeof(char *),
       NU_GRAVES_TERMINACIONS_ABERTAS, pal_en_singular,
       EN_DICCIONARIO_INVERSO) >= 0)
    return VOC_ABERTA;

  if (estan_en_contacto_con_nasal(pal_sil_ac))
    return VOC_PECHADA;


  if (comprobar_en_lista_de_inicio_de_palabras
      ((char **) graves_terminacions_pechadas, sizeof(char *),
       NU_GRAVES_TERMINACIONS_PECHADAS, pal_en_singular,
       EN_DICCIONARIO_INVERSO) >= 0)
    return VOC_PECHADA;


  return VOC_ABERTA;

}

/**  \fn   void cambiar_por_timbre_aberto(char *palabra)

     Dada unha palabra cambia o caracter normal da 'e' ou 'o' por 'é' ou 'ó' cando
     esta vocal e aberta. Se e pechada o caracter que representa as abertas ou pechadas
     e o caracter sin acentuar. Non entra en conflicto ca acentuacion xa
     que o acento prosodico esta sempre marcado polo caracter '^' a continuaciom
     ca vocal acentuada.
*/
void cambiar_por_timbre_aberto(char *palabra)

{
  char *pos_vocal_aberta;

  pos_vocal_aberta = strchr(palabra, '^');
  pos_vocal_aberta--;
  if ((*pos_vocal_aberta) == 'e') {
    (*pos_vocal_aberta) = 'é';
    return;
  }
  if (*pos_vocal_aberta == 'o') {
    (*pos_vocal_aberta) = 'ó';
  }
  return;
}

/*****************************************************************************
 *****************************************************************************/

/** \fn  int timbre_en_esdruxulas(char *palabra, char *pal_acentuada)
 *
 * \return Devolve 1 en abertas e 0 en pechadas.
 */

int timbre_en_esdruxulas(char *palabra, char *pal_acentuada)

{
  t_palabra pal = "";

  strcpy(pal, palabra);
  if (pal[strlen(pal) - 1] == 's')
    pal[strlen(pal) - 1] = 0;
  /*se e plural eliminase xa que nas listas estan en singular as terminacions
    podendo ser plural */

  if (hai_grupo_eu_ou_na_antepenultima_silaba(pal_acentuada))
    return VOC_PECHADA;

  if (comprobar_en_lista_de_inicio_de_palabras
      ((char **) esdr_terminacions_pechadas, sizeof(char *),
       NU_ESDR_TERMINACIONS_PECHADAS, pal, EN_DICCIONARIO_INVERSO) >= 0)
    return VOC_PECHADA;
  if (comprobar_en_lista_de_palabras
      ((char **) esdr_palabras_pechadas, sizeof(char *),
       NU_ESDR_PALABRAS_PECHADAS, pal) >= 0)
    return VOC_PECHADA;

  return VOC_ABERTA;
}

/*****************************************************************************
 *****************************************************************************/

/** \fn  int posicion_acento(char *pal)

    Devolve o orde da silaba acentuada empezando polo final.
*/
int posicion_acento(char *pal)
{
  int orde_silaba = 1;
  char *inicio = pal;

  pal = pal + strlen(pal) - 1;

  while (pal != inicio) {
    if (*pal == '-')
      orde_silaba++;
    if (*pal == '^')
      break;
    pal--;
  }
  if (*pal == '^')
    return orde_silaba;
  return 0;
}


/*****************************************************************************
 *****************************************************************************/

/** \fn  int e_sustantivo(t_frase_separada * frase_separada)

    \return Devolve 1 se a palabra ten o caracter de sustantivo. Isto sera tanto en
    nomes propios e adxectivos como nos sustantivos.
    \author giglesia (reforma)
*/

int e_sustantivo(t_frase_separada * frase_separada) {

  unsigned char categoria;
  int k;

  if (!LOCUCION(frase_separada->cat_gramatical[0]))
    categoria=frase_separada->cat_gramatical[0];
  else {

    for (k=0;k<N_CAT;k++){
      if (frase_separada->cat_gramatical_descartada[k] == 0) {
	break;
      }
    }
    categoria=frase_separada->cat_gramatical_descartada[k-1];
  }
  if (ADVERBIO(categoria) ||
      categoria == NOME ||
      categoria == ADXECTIVO ||
      categoria == INDEF_DET ||
      categoria == INDEF_PRON || 
      /*          categoria == PRON_PERS_TON || */          
      categoria == NUME_CARDI_PRON ||          
      categoria == NOME_PROPIO) {

    if (frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY ||
	frase_separada->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA ||
	frase_separada->clase_pal == PALABRA_NORMAL)
      return 1;
    else
      return 0;
  } else {
    return 0;
  }

}

/*****************************************************************************
 *****************************************************************************/

/** \fn  void asignar_timbre_a_sustantivos(char *palabra, char *palabra_acentuada)

    Dada unha palabra tal e como se escribe e a mesma palabra separada en silabas
    e acentuada asigna as vocais 'e' e 'o' o grao de apertura adecuado.
*/

void asignar_timbre_a_sustantivos(char *palabra, char *palabra_acentuada)
{
  int lugar_acento, timbre_aberto = 0;
  if (!(strstr((char *) palabra_acentuada, "e^") != NULL ||
	strstr((char *) palabra_acentuada, "o^") != NULL))
    return;
  /* se non hai e ou o tonica enton xa non se estudia o caso, xa que enton
     a oposicion aberta/pechada neutralizase. */
  if ((strstr((char *) palabra_acentuada, "é") != NULL ||
       strstr((char *) palabra_acentuada, "ó") != NULL))
    return;
  /* se chegaron aqui marcadas co 'é' ou co 'ó' enton tratase
     de diacriticos abertos e non fai falta ningunha comprobacion mais. */

  if ((strstr((char *) palabra_acentuada, "e^i") != NULL ||
       strstr((char *) palabra_acentuada, "o^u") != NULL))
    return;
  /* As palabras con diptongo decrecente tonico sempre son pechadas.
     e non fai falta facer nada mais. */
  lugar_acento = posicion_acento(palabra_acentuada);
  switch (lugar_acento) {
  case 1:
    {
      timbre_aberto = timbre_en_agudas(palabra);
      break;
    }
  case 2:
    {
      timbre_aberto = timbre_en_graves(palabra, palabra_acentuada);
      break;
    }
  case 3:
    {
      timbre_aberto = timbre_en_esdruxulas(palabra, palabra_acentuada);
      break;
    }
  }							/*switch */
  if (timbre_aberto) {
    cambiar_por_timbre_aberto(palabra_acentuada);
  }

  return;
}

/**
 * \author fmendez
 * \remark Esto no sirve para nada, así que no se utiliza
 *
 * \param frase_separada  
 *
 * \return 
 */

/*
  void asignar_timbre_en_frase_separada(t_frase_separada * frase_separada){
  while (*frase_separada->pal) {
  if (e_sustantivo(frase_separada)) {
  (frase_separada->pal_transformada == NULL) ?
  asignar_timbre_a_sustantivos(frase_separada->pal, frase_separada-> pal_sil_e_acentuada) :
  asignar_timbre_a_sustantivos(frase_separada->pal_transformada, frase_separada-> pal_sil_e_acentuada);
  }
  frase_separada++;
  }
  }
*/

/** \fn int comprobar_en_lista_de_palabras(char **inicio_lista, int lonx_item,
 *                                               int lonx, char *pal)
 *                                                                                                                   *
 * \remarks Funcion que dada unha palabra trata de localiza-la mediante busqueda binaria.
 *
 * \param  inicio_lista: Punteiro a un punteiro a caracteres. Esta funcion busca unha
 * palabra en arrais de estructuras. O primeiro campo de cada estructura debe
 * ser un punteiro a caracter, onde se encontrara a palabra segun a cal se
 * encontra ordenado o arrai de caracteres.
 *
 * \param  lonx_item: tamaño de cada estructura do arrai. Como normalmente estamos
 * buscando en arrais de punteiros a palabras, enton pasarase como parametro
 * "sizeof(char*)". En caso de que as estructuras fosen mais complicadas
 * pasariase como parametro "sizeof(<tipo_estructura>)".
 *
 * \param lon_lista: Numero de elementos do arrai (ou lista).
 *
 * \return  Devolve o indice da lista onde se encontra se a busqueda tivo exito e
 * se	non devolve -1.
 */
 
int comprobar_en_lista_de_palabras(char **inicio_lista, int lonx_item,
				   int lonx, char *pal)
{
  int lim_superior = lonx - 1;
  int lim_inferior = 0;
  int punto_medio;
  int pos_relativa;
  char *bite_inicial_lista = (char *) inicio_lista;
  char **palabra_punto_medio;

  for (;;) {
    punto_medio = (lim_superior + lim_inferior) / 2;
    palabra_punto_medio =
      (char **) (bite_inicial_lista + (punto_medio * lonx_item));
    pos_relativa = strcmp(pal, *palabra_punto_medio);
    if (pos_relativa > 0) {
      if (lim_inferior >= lim_superior)
	return -1;
      lim_inferior = punto_medio + 1;
    }
    else if (pos_relativa < 0) {
      if (lim_inferior >= lim_superior)
	return -1;
      lim_superior = punto_medio - 1;
    }
    else
      return punto_medio;
  }
  //return -1;
}

/*************************************************************************/

void invertir_cadea(char *pal)
{
  char *com, *fin, letra;

  //ZA. En algún momento se le pasa un pal="" y casca.

  if (!*pal)
    return;

  com = pal;
  fin = pal + strlen(pal);
  fin--;
  while (com < fin) {
    letra = *com;
    *com++ = *fin;
    *fin-- = letra;
  }
}

/**   \fn int comprobar_en_lista_de_inicio_de_palabras(char **inicio_lista,
 *                       int lonx_item, int lon_lista,char *pal, char tipo_busqueda)
 *
 *
 *  \param inicio_lista: Punteiro a un punteiro a caracteres. Esta funcion busca unha
 * palabra en arrais de estructuras. O primeiro campo de cada estructura debe
 * ser un punteiro a caracter, onde se encontrara a palabra segun a cal se
 * encontra ordenado o arrai de caracteres.
 *
 *  \param lonx_item: tamaño de cada estructura do arrai. Como normalmente estamos
 * buscando en arrais de punteiros a palabras, enton pasarase como parametro
 * "sizeof(char*)". En caso de que as estructuras fosen máis complicadas pasariase como
 *  parametro "sizeof(<tipo_estructura>)".
 *
 * \param lon_lista: Numero de elementos do arrai (ou lista).
 *
 * \remarks Esta funcion trata de buscar, dada unha palabra, aquela palabra das que se
 * encontra na lista que coincide con ela en todolos caracteres iniciais.
 * Utiliza nun principio busqueda binaria, e despois secuencial hacia atras. Podese utilizar
 * para encontrar, dada unha palabra, a raiz desta nun
 * diccionario. Pola simetria do caso tamen se emprega para buscar as desinencias
 * verbais nunha palabra se esta esta invertida e tamen a lista na que hai que
 * buscala.
 *
 * \remarks Exemplo: trozo de lista: ...
 *       - ar
 *       - art                                    													arte                                   *
 *       - ...
 *       - palabra de entrada: artista: =>devolve o indice correspondente o string art
 *
 * \return Devolve o indice da lista onde se encontra se a busqueda tivo exito e se
 * non devolve -1.
 */
int comprobar_en_lista_de_inicio_de_palabras(char **inicio_lista,
					     int lonx_item, int lon_lista,
					     char *pal, char tipo_busqueda)
{

  int lim_superior = lon_lista - 1;
  int lim_inferior = 0;
  int punto_medio;
  //char encontrado=0;
  //char fin_busqueda=0;
  int pos_relativa;
  t_palabra_proc palabra = "";
  t_palabra_proc pal_aux = "";
  char *bite_inicial_lista = (char *) inicio_lista;
  char *palabra_punto_medio;

  strcpy(palabra, pal);
  if (tipo_busqueda == EN_DICCIONARIO_INVERSO)
    invertir_cadea(palabra);	// Invertir palabra (diccionario inverso)
  for (;;) {
    punto_medio = (lim_superior + lim_inferior) / 2;
    palabra_punto_medio = bite_inicial_lista + (punto_medio * lonx_item);
    pos_relativa = strcmp(palabra, *((char **) palabra_punto_medio));
    if (pos_relativa > 0) {
      if (lim_inferior >= lim_superior)
	break;
      lim_inferior = punto_medio + 1;
    }
    else if (pos_relativa < 0) {
      if (lim_inferior >= lim_superior)
	break;
      lim_superior = punto_medio - 1;
    }
    else
      return punto_medio;
  }
  /* Comparamos agora se coinciden os inicios das duas palabras. Para eso
     recortamos a mais larga e comparamolas de novo                             */
  strcpy(pal_aux, palabra);
  pal_aux[strlen(*((char **) palabra_punto_medio))] = 0;
  if (strcmp(pal_aux, *((char **) palabra_punto_medio)) == 0)
    return punto_medio;
  /* Comparamos se o sitio no que parou e o mais proximo ou ben comenzamos a 
     busqueda hacia atras.                                                      */
  if (punto_medio == 0)
    return -1;
  while (palabra[0] == (char)
	 **((char **) (palabra_punto_medio -
		       (1 * (unsigned char) lonx_item)))) {
    /* A busca hacia atras sempre e posible se polo menos coinciden as duas palabras 
       no primeiro caracter. Para isto comprobamos se a palabra que se encontra in-
       mediatamente antes ca que esta (por iso restamos o punteiro 1*(unsigned char) lonx_item) 
       tamen ten comun o primeiro caracter.                                       */
    punto_medio--;
    palabra_punto_medio =
      palabra_punto_medio - (1 * (unsigned char) lonx_item);
    /* Decrementamos este punteiro en tantos bytes como a lonxitude de cada estruc-
       -tura para que apunte a palabra anterior.                                  */
    strcpy(pal_aux, palabra);	// Igualamo-la lonxitude das duas palabras
    pal_aux[strlen(*((char **) palabra_punto_medio))] = 0;
    if (strcmp(pal_aux, *((char **) palabra_punto_medio)) == 0)
      return punto_medio;
    if (punto_medio == 0)
      return -1;
  }
  return -1;
}



void tratamento_das_excepcions_da_w(char *palabra, char *pal_sil_acen)
{
  char *pos_w;
  t_palabra_proc resto_da_palabra;

  while ((pos_w = strchr(pal_sil_acen, 'w')) != NULL) {
    if (comprobar_en_lista_de_inicio_de_palabras
	((char **) w_pronunciase_u, sizeof(char *), NU_PAL_W_PRON_COMO_U,
	 palabra, EN_DICCIONARIO_NORMAL) >= 0) {
      *pos_w = 'u';
    }
    else if (comprobar_en_lista_de_inicio_de_palabras
	     ((char **) w_pronunciase_gu, sizeof(char *),
	      NU_PAL_W_PRON_COMO_GU, palabra,
	      EN_DICCIONARIO_NORMAL) >= 0) {
      strcpy(resto_da_palabra, pos_w + 1);
      strcpy(pos_w, "gu");
      strcpy(pos_w + 2, resto_da_palabra);
    }
    else {
      *pos_w = 'b';
    }
  }
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param palabra 
 * \param pal_sil_acen 
 */
void tratamento_das_excepcions_da_xe(char *palabra, char *pal_sil_acen)
{
  int pos_lista;
  char * pos_x;
  t_palabra_proc resto_da_palabra;

  pos_x = strchr((char *) pal_sil_acen, 'x');
  if (pos_x == NULL){
    return;
  }

  if (pal_sil_acen[strlen(pal_sil_acen) - 1] == 'x'){
    /*todalas terminadas en -x sonan como ks. */
    strcpy(pal_sil_acen + strlen(pal_sil_acen) - 1, "ks");
    return;
  }
  pos_lista = comprobar_en_lista_de_inicio_de_palabras((char **)
						       pronuncianse_con_xe,
						       sizeof(char *),
						       NU_PAL_X_PRON_COMO_XE,
						       palabra,
						       EN_DICCIONARIO_NORMAL);
  if (pos_lista >= 0){
    return;
  }
  pos_lista = comprobar_en_lista_de_inicio_de_palabras((char **) x_pasa_a_ks,
						       sizeof(char *), NU_PAL_X_PRON_COMO_KS, palabra, EN_DICCIONARIO_NORMAL);
  if (pos_lista >= 0) {
    strcpy(resto_da_palabra, pos_x + 1);
    if (pos_x > pal_sil_acen && *(pos_x - 1) == '-'){
      strcpy(pos_x - 1, "k-s");
    }
    else{
      strcpy(pos_x, "ks");
    }
    strcat(pos_x + 2, resto_da_palabra);
  }
}


/** 
 * \author fmendez
 * \brief 
 * 
 * \param palabra 
 * \param categoria 
 * 
 * \return 
 */
int Trat_fon::tonica(t_frase_separada * palabra, unsigned char categoria)
{
  if (locucion(palabra)) {
    categoria = palabra->cat_gramatical[1];
  }
  /*

  // Tras comprobar unos cuantos ficheros de Paulino y Freire, parece que los posesivos son
  // tónicos.

  if (palabra > frase_separada) {
  if ((categoria == POSE || categoria == POSE_DET || categoria == POSE_PRON)
  && ((palabra - 1)->cat_gramatical[0] == NOME
  || (palabra - 1)->cat_gramatical[0] == CAT_NON_CLASIFICADA)) {
  return 1;
  }
  }
  */

  if (adverbio(categoria)) {
    return 1;
  }

  switch (categoria) {
    // posesivos
  case POSE_DET:
  case POSE_PRON:
  case POSE:
    // contracciones
  case CONTR_PREP_ART_INDET: // En fin, a veces sí, a veces no...
    // puede que dependa de la distancia al siguiente acento.
    //indefinidos
  case INDEF:
  case INDEF_DET:
  case INDEF_PRON:
  case CONTR_PREP_INDEF:
  case CONTR_PREP_INDEF_PRON:
  case CONTR_PREP_INDEF_DET:
  case CONTR_DEMO_INDEF:
  case CONTR_DEMO_INDEF_PRON:
  case CONTR_PREP_DEMO_INDEF:
  case CONTR_PREP_DEMO_INDEF_PRON:
  case CONTR_INDEF_ART_DET:
    //numerales
  case NUME:
  case NUME_DET:
  case NUME_PRON:
  case NUME_CARDI:
  case NUME_CARDI_DET:
  case NUME_CARDI_PRON:
  case NUME_ORDI:
  case NUME_ORDI_DET:
  case NUME_ORDI_PRON:
  case NUME_PARTI:
  case NUME_PARTI_DET:
  case NUME_PARTI_PRON:
  case NUME_MULTI:
    //    case NUME_MULTI_DET:
    //    case NUME_MULTI_PRON:
  case NUME_COLECT:
    //    case NUME_COLECT_DET:
    //    case NUME_COLECT_PRON:
    //pronombres
  case PRON_PERS_TON:
  case CONTR_PREP_PRON_PERS_TON:
    //resto
    //case RELATIVO_TONICO:
    //    case RELA:
  case INTER:
  case EXCLA:
  case NOME:
  case ADXECTIVO:
  case NOME_PROPIO:
  case VERBO:
  case INFINITIVO:
  case XERUNDIO:
  case PARTICIPIO:
  case INTERX:
  case CAT_NON_CLASIFICADA:
  case 0:
    return 1;
    // demostrativos
  case DEMO:
  case DEMO_DET:
  case DEMO_PRON:
  case CONTR_PREP_DEMO_PRON:
  case CONTR_PREP_DEMO:
  case CONTR_PREP_DEMO_DET:
    return 1;
    //	case DEMO:
    //	case DEMO_DET:
    //	case CONTR_PREP_DEMO:
    //	case CONTR_PREP_DEMO_DET:
    //		return 0;

  default:
    return 0;
  }
  /* antes

     case NOME:
     case VERBO:
     case ADXECTIVO:
     case NOME_PROPIO:
     //	 case ART_INDET:
     case RELATIVO_TONICO:
     case CONTR_PREP_ART_INDET:
     case INDEF_DET:
     case PRON_PERS_TON:
     case PRON_PERS_AT:
     case PRON_PERS_AT_REFLEX:
     case PRON_PERS_AT_ACUS:
     case PRON_PERS_AT_DAT:
     case CONTR_PRON_PERS_AT_DAT_AC:
     case INDEF_PRON:
     case CONTR_PREP_PRON_PERS_TON:
     case CONTR_PREP_DEMO:
     case INDEF:
     case NUME:
     case DEMO_PRON:
     case NUME_DET:
     return 1;

     case DEMO:
     case DEMO_DET:

     // FRAN_CAMPILLO: Hemos cambiado la consideración de la tonicidad de los demostrativos.
     #ifdef _DEMOSTRATIVOS_ATONOS
     if ( (f_separada+1)->cat_gramatical[0]==NOME) // Aquí nos podemos salir de memoria, pero para
     return 0;
     // las pruebas que nos interesan no importa demasiado.
     #endif
     return 1;
     default:
     return 0;
     // FRAN_CAMPILLO.
     }
     // return 0;

     */
}


/** 
 * \author fmendez
 * \brief 
 * 
 * \param entrada 
 * \param saida 
 * 
 * \return 
 */
char *extraer_texto_entre_espacios(char *entrada, char *saida)
{

  while (*entrada == ' '){
    entrada++;
  }
  while (*entrada != 0 && *entrada != ' ') {
    //strncat(saida,entrada,1);
    *saida++ = *entrada++;
  }
  *saida = 0;
  if (*entrada == ' '){
    entrada++;
  }
  return entrada;
}




/**
 * \author 
 * \remark 
 *
 * \param palabra  
 * \param pal_sil_acen  
 * \param tonicidade  
 * \param idioma  
 * \param se_e_sustantivo  
 *
 * \return 
 */
void Trat_fon::trat_fonetico(char * palabra, char * pal_sil_acen,
			     int tonicidade, char idioma, char se_e_sustantivo)
{

  char * pos_acento;
  char resto_da_palabra[LONX_MAX_PALABRA];

  if (!tonicidade){
    /*enton eliminamolo acento */
    pos_acento = strchr((char *) pal_sil_acen, '^');
    //ZA: Underrun de los ...
    if (pos_acento != NULL) {
      strcpy(resto_da_palabra, pos_acento + 1);
      strcpy(pos_acento, resto_da_palabra);
    }
  }
  tratamento_das_excepcions_da_xe(palabra, pal_sil_acen);
  tratamento_das_excepcions_da_w(palabra, pal_sil_acen);
  // #ifdef REGLAS_DE_ALOFONOS_DO_GALEGO
  if (idioma == GALEGO && se_e_sustantivo){
    asignar_timbre_a_sustantivos(palabra, pal_sil_acen);
  }
  // #endif
}

/**
 * \author 
 * \remark 
 *
 * \param f_sep  
 * \param idioma  
 *
 * \return 
 */
void Trat_fon::atono_ou_tonico_aberto_ou_pechado_e_w_x(t_frase_separada * f_sep, char idioma)
{
  t_palabra_proc palabra;
  t_palabra_sil_e_acentuada palabra_sil_acen, kk;
  char tratase_de_sustantivo;
  unsigned char *punt_arrai_cat;
  char * punt_palabra;
  char * punt_palabra_sil_acen;
  int tonicidade=0;
  int codigo, conxug;
  unsigned int pos_timbre, pos_timbre_sil, cod_persoa;
  temp_verb cod_tempo;
  t_frase_separada aux, *aux2;


  frase_separada = f_sep;

  while (*f_sep->pal) {
    f_sep->pal_transformada == NULL ?
      punt_palabra = f_sep->pal :
      punt_palabra = f_sep->pal_transformada;

    punt_palabra_sil_acen = f_sep->pal_sil_e_acentuada;
    //tratase_de_sustantivo = 0;
    //giglesia: aquí siempre inicializado a 0? Luego no se modifica.
    tratase_de_sustantivo = e_sustantivo(f_sep);
    if (f_sep->clase_pal != DELETREO && idioma == GALEGO && (f_sep->cat_gramatical[0] == VERBO || (f_sep->cat_gramatical[0] == PERIFRASE && esta_palabra_tivera_a_categoria_de(VERBO, f_sep) ))) {

      codigo = (unsigned char) f_sep->cat_verbal[0];
      obter_codigo(codigo, &conxug, &cod_tempo, &cod_persoa);
      if (cod_tempo == PA) {
	cod_persoa += 6;
      }

      if (manexo_do_timbre_verbal(punt_palabra, f_sep->inf_verbal, &cod_tempo,
				  &cod_persoa, &pos_timbre, &pos_timbre_sil, idioma)) {
	// Se houbo timbre abrimo-la vocal correspondente
	if (f_sep->pal_sil_e_acentuada[(pos_timbre_sil) - 1] == 'e') {
	  f_sep->pal_sil_e_acentuada[(pos_timbre_sil) - 1] = 'é';
	}
	else if (f_sep->pal_sil_e_acentuada[(pos_timbre_sil) - 1] == 'o') {
	  f_sep->pal_sil_e_acentuada[(pos_timbre_sil) - 1] = 'ó';
	}
      }
    }
    if (f_sep->pal_transformada != NULL &&
	!(f_sep->clase_pal == PALABRA_NORMAL_EMPEZA_MAY ||
	  f_sep->clase_pal == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA ||
	  //f_sep->clase_pal==PALABRA_CON_GUION_NO_MEDIO  ||
	  f_sep->clase_pal == DELETREO  ||
	  f_sep->clase_pal == PALABRA_NORMAL)) {
      *kk = 0;

      punt_palabra = extraer_texto_entre_espacios(punt_palabra, palabra);
      punt_palabra_sil_acen = extraer_texto_entre_espacios(punt_palabra_sil_acen, palabra_sil_acen);
			
      while (*palabra) {
	*aux.cat_gramatical = 0;
	*aux.cat_gramatical_descartada = 0;

	if (-1 == diccionarios->busca_dic_nomes(palabra, &aux, idioma)){
	  punt_arrai_cat = NULL;
	}
	else {
	  punt_arrai_cat = aux.cat_gramatical;
	}
	if (punt_arrai_cat == NULL){// || aux.tonicidad_forzada)
	  tonicidade = 1;
	}
	else {
	  if ( (*punt_arrai_cat == CONX_COOR_COPU) &&
	       ( (f_sep->cat_gramatical[0] == NUME_CARDI) ||
		 (f_sep->cat_gramatical[0] == NUME_ORDI_DET) ||
		 (f_sep->cat_gramatical[0] == NUME_CARDI_DET) ||
		 (f_sep->cat_gramatical[0] == NUME_CARDI_PRON) ) )
	    // Éste es el caso de los numerales extendidos (v.gr 1985), en los que la conjunción se ponía tónica.
	    tonicidade = 0;
	  else {
	    if ( (*punt_arrai_cat == ART_INDET) &&
		 ( (f_sep->cat_gramatical[0] == NUME_CARDI) ||
		   (f_sep->cat_gramatical[0] == NUME_ORDI_DET) ||
		   (f_sep->cat_gramatical[0] == NUME_CARDI_DET) ||
		   (f_sep->cat_gramatical[0] == NUME_CARDI_PRON) ) ) {
	      tonicidade = 1;
	    }
	    else {

	      aux2 = f_sep + 1;
	      while (*aux2->pal && aux2->clase_pal == SIGNO_PUNTUACION) {
		aux2++;
	      }
	      *aux2->pal ?
		tonicidade = tonica(f_sep, *punt_arrai_cat) :
		tonicidade = 1;
	    }
	  }
	}
	if ( (*punt_palabra == 0) && (f_sep->tonicidad_forzada == 1 || f_sep->pausa != 0) ) {
	  tonicidade = 1;
	}

	if (f_sep->tonicidad_forzada == 2) {
	  tonicidade = 0;
	  f_sep->tonicidad_forzada = 0;
	}

	// Fran Campillo: Esto habría que estudiarlo: me parece que a partir de un número de
	// sílabas, todas las palabras son tónicas. Supongo que para mantener la estructura
	// rítmica (como el foot structure del inglés).
#ifdef _MODO_NORMAL
	if ( (strcmp(punt_palabra, "durante") == 0) || (strcmp(punt_palabra, "incluso") == 0) )
	  tonicidade = 1;
#endif 
	trat_fonetico(palabra, palabra_sil_acen, tonicidade, idioma, tratase_de_sustantivo);
	strcat(kk, " ");
	strcat(kk, palabra_sil_acen);
	//fmen
	punt_palabra = extraer_texto_entre_espacios(punt_palabra, palabra);
	punt_palabra_sil_acen = extraer_texto_entre_espacios(punt_palabra_sil_acen, palabra_sil_acen);
	//fmen
      }
      strcpy(f_sep->pal_sil_e_acentuada, kk + 1);
    }
    else {
      if (f_sep->clase_pal != SIGNO_PUNTUACION && f_sep->clase_pal != CADENA_DE_SIGNOS) {
	if (f_sep->tonicidad_forzada == 2) {
	  tonicidade = 0;
	  f_sep->tonicidad_forzada = 0;
	}
	else if (f_sep->clase_pal == DELETREO) {
	  tonicidade = 1;
	  f_sep->tonicidad_forzada = 1;
	}
	else {
	  aux2 = f_sep + 1;
	  while (*aux2->pal && (aux2->clase_pal == SIGNO_PUNTUACION || aux2->clase_pal == CADENA_DE_SIGNOS)) {
	    aux2++;
	  }
	  if (*aux2->pal) {
	    if (LOCUCION(f_sep->cat_gramatical[0]) || f_sep->cat_gramatical[0] == PERIFRASE) {
	      tonicidade = f_sep->tonicidad_forzada;
	    }
	    else {
	      f_sep->tonicidad_forzada ?
		tonicidade = 1 :
		tonicidade = tonica(f_sep, f_sep->cat_gramatical[0]);
	    }
	  }
	  else {				//es la ultima palabra
	    tonicidade = 1;
	  }
	}

	// Fran Campillo: Esto habría que estudiarlo: me parece que a partir de un número de
	// sílabas, todas las palabras son tónicas. Supongo que para mantener la estructura
	// rítmica (como el foot structure del inglés).
#ifdef _MODO_NORMAL
	if ( (strcmp(punt_palabra, "durante") == 0) || (strcmp(punt_palabra, "incluso") == 0) )
	  tonicidade = 1;

	// if ( (tonicidade == 0) && ( (f_sep + 1)->clase_pal == SIGNO_PUNTUACION) ) {
	//   tonicidade = 1;
	//   aux2 = f_sep - 1;
	//   while (aux2 >= frase_separada) {
	//     if ( (aux2->clase_pal == SIGNO_PUNTUACION) && (aux2 != f_sep) )
	//       break;
	//     if (aux2->tonicidad_forzada == 1) {
	//       tonicidade = 0;
	//       break;
	//     }
	//     aux2--;
	//   }
	// }
	// else
	  f_sep->tonicidad_forzada = tonicidade;

#endif
	if ((f_sep + 1)->pal) // Ojo aquí: esto puede no ser correcto
	  if ( ( (f_sep + 1)->cat_gramatical[0] == RUPTURA_ENTONATIVA) ||
	       ( (f_sep + 1)->cat_gramatical[0] == RUPTURA_COMA) )
	    tonicidade = 1;
                        
	trat_fonetico(punt_palabra, punt_palabra_sil_acen, tonicidade, idioma, tratase_de_sustantivo);

      }
    }
#ifdef _MODO_NORMAL

    f_sep->tonicidad_forzada = tonicidade;

#endif

    f_sep++;
  }
}

/*****************************************************************************
 *****************************************************************************/
inline void insertar_silencio_inicial(char *f_sil_e_acentuada)
{
  strcat(f_sil_e_acentuada, "#%pausa 50%#");
}

/*****************************************************************************
 *****************************************************************************/
//void asignar_pausa_entre_frases(char *f_sil_e_acentuada,char * continuacion_texto,char * prosodia)
void asignar_pausa_entre_frases(char *f_sil_e_acentuada)
/*este valor de pausa engadirase o que xa se asigna como pausa de grupo final */
{
  // unsigned int d_pausa=0;
  // char num_en_caracteres[10]="";
  // char insercion[50] = "";

  //--------------------------------------------------
  // if (strlen(f_sil_e_acentuada) && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
  // 	strcat(insercion, " ");
  // }
  //-------------------------------------------------- 

  if (strlen(f_sil_e_acentuada) && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
	   	strcat(f_sil_e_acentuada, " ");
	  }

  strcat(f_sil_e_acentuada, "#%pausa 500%#");
  /*
    if (strcmp(continuacion_texto,"Aparte")==0) d_pausa=500;
    else if (strcmp(prosodia,"Final sin puntuacion")==0) d_pausa=800;
    else d_pausa=10;


    sprintf(num_en_caracteres,"%d",d_pausa);
    strcat(insercion,"#%pausa ");
    strcat(insercion,num_en_caracteres);
    strcat(insercion,"%#");

    if ( strlen(insercion)<=(LONX_MAX_FRASE_SIL_E_ACENTUADA-1-strlen(f_sil_e_acentuada)) )
    //se queda abondo espacio na frase, entón insertamos.
    strcat(f_sil_e_acentuada,insercion);
  */
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param f_en_grupos 
 * 
 * \return 
 */
int duracion_pausa(t_frase_en_grupos * f_en_grupos)
{
  /*

    PUNTOS_SUSPENSIVOS
    APERTURA_INTERROGACION
    PECHE_INTERROGACION
    APERTURA_EXCLAMACION
    PECHE_EXCLAMACION
    APERTURA_PARENTESE
    PECHE_PARENTESE
    APERTURA_CORCHETE
    PECHE_CORCHETE
    GUION
    DOBLES_COMINNAS
    SIMPLES_COMINNAS      
    GUION_BAIXO           

  */
  unsigned char tipo_fin;

  if (f_en_grupos->tipo_fin[1])
    tipo_fin = f_en_grupos->tipo_fin[1];
  else
    tipo_fin = f_en_grupos->tipo_fin[0];

  switch (tipo_fin) {
  case PUNTO:
    return 800;
  case COMA:
    return 100;
  case DOUS_PUNTOS:
    return 200;
  case PUNTO_E_COMA:
    return 300;
  case PUNTOS_SUSPENSIVOS:
    return 200;
  case APERTURA_INTERROGACION:
    return 100;
  case PECHE_INTERROGACION:
    return 100;
  case APERTURA_EXCLAMACION:
    return 100;
  case PECHE_EXCLAMACION:
    return 100;
  case APERTURA_PARENTESE:
    return 100;
  case PECHE_PARENTESE:
    return 100;
  case APERTURA_CORCHETE:
    return 100;
  case PECHE_CORCHETE:
    return 100;
  case GUION:
    return 100;
    /*case  DOBLES_COMINNAS:return 100;       
      case  SIMPLES_COMINNAS:return 100;  AS comiñas non  xenerarán pausa */
  case GUION_BAIXO:
    return 100;
  }

  /* Hai que construir ainda a asignacion de pausas correcta */

  return 0;
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param f_en_grupos 
 * \param f_sil_e_acentuada 
 */
void insertar_pausa_entre_grupos(t_frase_en_grupos * f_en_grupos, char *f_sil_e_acentuada)
{
  char st_nu[10] = "";
  unsigned int dur_pausa;
  char insercion[50] = "";

  dur_pausa = duracion_pausa(f_en_grupos);
  /* 
   * Averiguamos primeiro se se debe insertar unha pausa entre as proposicións
   * nas que está dividida a frase.
   */

  if (dur_pausa > 0){
    /*se hai que introducir pausa. */
    if (*f_sil_e_acentuada && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
      strcat(insercion, " ");
    }
    sprintf(st_nu, "%d", dur_pausa);
    strcat(insercion, "#%pausa ");
    strcat(insercion, st_nu);
    strcat(insercion, "%#");

    if (strlen(insercion) <= (LONX_MAX_FRASE_SIL_E_ACENTUADA - 1 - strlen(f_sil_e_acentuada))){
      strcat(f_sil_e_acentuada, insercion); //se queda abondo espacio na frase, entón insertamos.
    }
  }
}


/** 
 * \author fmendez
 * \brief 
 * 
 * \param f_en_grupos 
 * \param f_sil_e_acentuada 
 */
void insertar_tipo_de_proposicion(t_frase_en_grupos * f_en_grupos, char * f_sil_e_acentuada)
{
  //char num_en_caracteres[5] = "";
  char insercion[50];

  // if (duracion_pausa((f_en_grupos-1))>0)
  /*Insertamos unha nova proposicion so se houbo unha pausa o final
    do grupo anterior */
  {
    if (*f_sil_e_acentuada && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
      strcat(f_sil_e_acentuada, " ");
    }
    sprintf(insercion,"%%prop %d%%", f_en_grupos->tipo_tonema);
    //--------------------------------------------------
    // sprintf(num_en_caracteres, "%d", f_en_grupos->tipo_tonema);
    // strcat(insercion, "%prop ");
    // strcat(insercion, num_en_caracteres);
    // strcat(insercion, "%");
    //-------------------------------------------------- 

    if (strlen(insercion) <= (LONX_MAX_FRASE_SIL_E_ACENTUADA - 1 - strlen(f_sil_e_acentuada))){
      /*se queda abondo espacio na frase, enton insertamos. */
      strcat(f_sil_e_acentuada, insercion);
    }
  }
}

/**
 * \author 
 * \remark 
 *
 * \param rec_frase_separada  
 * \param rec_frase_sintagmada  
 * \param rec_frase_en_grupos  
 * \param frase_sil_e_acentuada  
 * \param idioma  
 *
 * \return 
 */
void Trat_fon::tratamento_fonetico_previo(t_frase_separada * rec_frase_separada,
					  t_frase_sintagmada * rec_frase_sintagmada,
					  t_frase_en_grupos * rec_frase_en_grupos,
					  char *frase_sil_e_acentuada, char idioma)
{

  t_frase_en_grupos *inicio_frase_en_grupos = rec_frase_en_grupos;
  int i, j;

  if (rec_frase_separada->pal[0] == 0) {
    return;					//non hai nada que procesar
  }

  frase_separada = rec_frase_separada;

  insertar_silencio_inicial(frase_sil_e_acentuada);
  while (rec_frase_en_grupos->fin != 0 || rec_frase_en_grupos == inicio_frase_en_grupos) {
    insertar_tipo_de_proposicion(rec_frase_en_grupos, frase_sil_e_acentuada);
    for (i = rec_frase_en_grupos->inicio; i <= rec_frase_en_grupos->fin; i++) {
      for (j = rec_frase_sintagmada[i].inicio; j <= rec_frase_sintagmada[i].fin; j++) {
	if (frase_sil_e_acentuada[strlen(frase_sil_e_acentuada) - 1] != ' ') {

	  strcat(frase_sil_e_acentuada, " ");

	}
	if ((rec_frase_separada + j)->clase_pal != SIGNO_PUNTUACION && 
	    (rec_frase_separada + j)->clase_pal != CADENA_DE_SIGNOS) {
	  strcat(frase_sil_e_acentuada, (rec_frase_separada + j)->pal_sil_e_acentuada);
	  strcat(frase_sil_e_acentuada, " ");
	}



	//atono_ou_tonico_aberto_ou_pechado_e_w_x(frase_sil_e_acentuada, (rec_frase_separada + j), idioma);
	insertar_pausa_entre_palabras(rec_frase_separada + j, frase_sil_e_acentuada);
      }
#ifdef _SOLO_PAUSAS_ORTOGRAFICAS
      insertar_pausa_entre_sintagmas(rec_frase_sintagmada[i], frase_sil_e_acentuada);
#endif
    }
    if (strlen(frase_sil_e_acentuada) && frase_sil_e_acentuada[strlen(frase_sil_e_acentuada) - 1] != ' '){
          strcat(frase_sil_e_acentuada, " ");
        }
    //insertar_pausa_entre_grupos(rec_frase_en_grupos,frase_sil_e_acentuada);
    if (rec_frase_en_grupos->ruptura_entonativa == 1)
      strcat(frase_sil_e_acentuada, "#%ruptura-entonativa%#");
    else
      if (rec_frase_en_grupos->ruptura_entonativa == 2)
	strcat(frase_sil_e_acentuada, "#%ruptura-coma%#");

    rec_frase_en_grupos++;
  }
  if (frase_sil_e_acentuada[strlen(frase_sil_e_acentuada) - 1] != '#')
    asignar_pausa_entre_frases(frase_sil_e_acentuada);

}


/**
 * \author 
 * \remark 
 *
 * \param cad  
 *
 * \return 
 */
void saca_tipo_prop(char *cad)
{
  char *lee, *escribe, pausa = 0;

  lee = escribe = cad;
  while (*lee) {
    if (*lee == '#') {
      if (pausa) {		//dos pausas seguidas
	if (*(escribe - 1) == ' ')
	  escribe--;
	lee++;
	while (*lee != '#')
	  lee++;
	lee++;			//salto el #  y el espacio
      }
      else {
	*escribe++ = *lee++;
	while (*lee != '#')
	  *escribe++ = *lee++;
	*escribe++ = *lee++;	//el #
	*escribe = *lee;	//el espacio blanco
	pausa = 1;
      }
    }
    else if (*lee == '%') {
      escribe -= 2;		//quito el espacio anterior
      lee++;
      while (*lee != '%')
	lee++;
    }
    else {
      if (*lee != ' ')
	pausa = 0;
      *escribe = *lee;
    }
    if (*lee == 0)
      break;
    lee++;
    escribe++;
  }
  *escribe = 0;
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param cp_frase_separada 
 * 
 * \return 
 */
int Trat_fon::arregla_timbre(t_frase_separada * cp_frase_separada)
{
  char *cacho_frase_sil = frase_sil_e_acentuada;
  char *pal_acent, *pos_acento, *pos_pal_acent;
  t_frase_separada *punteiro, *pcopia;

  punteiro = frase_separada;
  pcopia = cp_frase_separada;

  while (pcopia->pal[0]) {
    if (punteiro->pal_sil_e_acentuada[0] &&
	strcmp(punteiro->pal, punteiro->pal_sil_e_acentuada))
      pal_acent = punteiro->pal_sil_e_acentuada;
    else {
      pcopia++;
      punteiro++;
      continue;
    }
    if (NULL == (pos_pal_acent = strstr(cacho_frase_sil, pal_acent))) {
      pcopia++;
      punteiro++;
      continue;
    }
    cacho_frase_sil = pos_pal_acent + strlen(pal_acent);	//preparo *cacho para sig. pal

    pos_acento = strstr(pal_acent, "^");
    pos_acento--;
    if (strpbrk(pcopia->pal, "EOÉÓ")) {
      if (*pos_acento == 'e')
	*pos_acento = 'é';
      else if (*pos_acento == 'o')
	*pos_acento = 'ó';
    }
    else {
      if (*pos_acento == 'é')
	*pos_acento = 'e';
      else if (*pos_acento == 'ó')
	*pos_acento = 'o';
    }
    memcpy(pos_pal_acent, pal_acent, strlen(pal_acent));
    pcopia++;
    punteiro++;
  }
  return OK;

}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param sintag 
 * \param f_sil_e_acentuada 
 */
void insertar_pausa_entre_sintagmas(t_frase_sintagmada sintag, char *f_sil_e_acentuada)
{
  char st_nu[10] = "";
  char insercion[50] = "";
	
  if (sintag.pausa) {
    if (strlen(f_sil_e_acentuada) && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
      strcat(insercion, " ");
    }
    sprintf(st_nu, "%d", sintag.pausa);
    strcat(insercion, "#%pausa ");
    strcat(insercion, st_nu);
    strcat(insercion, "%#");
    if (strlen(insercion) <= (LONX_MAX_FRASE_SIL_E_ACENTUADA - 1 - strlen(f_sil_e_acentuada))){
      /*se queda abondo espacio na frase, entón insertamos. */
      strcat(f_sil_e_acentuada, insercion);
    }
  }
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param pal 
 * \param f_sil_e_acentuada 
 */
void insertar_pausa_entre_palabras(t_frase_separada * pal, char *f_sil_e_acentuada)
{
  char st_nu[10];
  char insercion[50] = "";

  if (pal->pausa) {
    if (strlen(f_sil_e_acentuada) && f_sil_e_acentuada[strlen(f_sil_e_acentuada) - 1] != ' '){
      strcpy(insercion, " ");
    }
    //        if (pal->clase_pal == SIGNO_PUNTUACION) {
    sprintf(st_nu, "%d", pal->pausa);
    strcat(insercion, "#%pausa ");
    strcat(insercion, st_nu);
    strcat(insercion, "%#");
    //        }
    //        else
    //			strcat(insercion, "#%ruptura-entonativa%# ");
    if (strlen(insercion) <= (LONX_MAX_FRASE_SIL_E_ACENTUADA - 1 - strlen(f_sil_e_acentuada))){
      //se queda abondo espacio na frase, entón insertamos.
      strcat(f_sil_e_acentuada, insercion);
    }
  }
}

