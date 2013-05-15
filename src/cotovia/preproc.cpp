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
 * \author 
 * \remarks Neste arquivo estan as funcions encargadas de extender as palabras 
 * que o precisen. Estan tanto as que tratan con abreviaturas como as que 
 * traballan para transformar os distintos tipos de cadeas numericas.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <iostream>
//#include <vector>
//#include <string>
//#include <sstream>

//using namespace std;

#include "modos.hpp"
#include "tip_var.hpp"
#include "errores.hpp"
#include "letras.hpp"
#include "trat_fon.hpp"
#include "clas_pal.hpp"
#include "xen_num.hpp"
#include "verbos.hpp"
#include "gbm_locuciones.hpp"
#include "interfaz_ficheros.hpp"	//ZA: Para que reconozca ciertas funciones..
#include "preproc.hpp"


const char *decenas[10] = { "", "vinte", "trinta", "corenta", "cincuenta", "sesenta", "setenta",
	"oitenta", "noventa", "cen"
};
const char *ordinales_de_decenas[10] = { "", "décim", "vixésim", "trixésim", "cuadraxésim", "quincuaxésim",
	"sesaxésim", "septuaxésim", "octoxésim", "nonaxésim"
};
const char *especiales[10] = { "dez", "once", "doce", "trece", "catorce", "quince", "dezaseis",
	"dezasete", "dezaoito", "dezanove"
};
const char *meses[13] = { "xaneiro", "febreiro", "marzo", "abril", "maio", "xuño", "xullo",
	"agosto", "setembro", "outubro", "novembro", "decembro"
};
struct numlet todos[] = {
	{"cero", "décim"},
	{"un", "primeir"},
	{"dous", "segund"},
	{"tres", "terceir"},
	{"catro", "cuart"},
	{"cinco", "quint"},
	{"seis", "sext"},
	{"sete", "sétim"},
	{"oito", "oitav"},
	{"nove", "noven"}
};

/* Repetimo-los mesmos pero agora en castelan e con nomes distintos para as va-
   -riables implicadas.                                                       */
const char *dec_cas[10] = { "", "veinti", "treinta", "cuarenta", "cincuenta",
	"sesenta", "setenta", "ochenta", "noventa", "cien"
};
const char *ordinal_decena_cas[10] = { "", "décim", "vigésim", "trigésim",
	"cuadragésim", "quinquagésim", "sexagésim", "septuagésim", "octogésim",
	"nonagésim"
};
const char *especiales_cas[10] = { "diez", "once", "doce", "trece", "catorce",
	"quince", "dieciséis", "diecisiete", "dieciocho", "diecinueve"
};
const char *mes_cas[13] = { "enero", "febrero", "marzo", "abril", "mayo",
	"junio", "julio", "agosto", "septiembre", "octubre", "noviembre",
	"diciembre"
};
struct numlet todos_cas[] = {
	{"cero", "décim"},
	{"uno", "primer"},
	{"dos", "segund"},
	{"tres", "tercer"},
	{"cuatro", "cuart"},
	{"cinco", "quint"},
	{"seis", "sext"},
	{"siete", "sétim"},
	{"ocho", "octav"},
	{"nueve", "noven"}
};

/**
 * \author ?
 * \remark lista de letras del abecedario gallego y su extensión escrita tal y
 * como se pronuncia (d-> de).
 */
const t_tabla_traduc cast_abecedario[NU_CAST_ABECE + 1] = {	/* Abecedario en castelan                 */
	{"a", "a"},
	{"á", "a"},
	{"b", "be"},
	{"c", "ce"},
	{"Ç", "cedilla"},
	{"ç", "cedilla"},
	{"d", "de"},
	{"e", "e"},
	{"é", "e"},
	{"f", "efe"},
	{"g", "ge"},
	{"h", "ache"},
	{"i", "i"},
	{"í", "i"},
	{"j", "jota"},
	{"k", "ka"},
	{"l", "ele"},
	{"ll", "elle"},
	{"m", "eme"},
	{"n", "ene"},
	{"o", "o"},
	{"ó", "o"},
	{"ñ", "eñe"},
	{"p", "pe"},
	{"q", "ku"},
	{"r", "erre"},
	{"rr", "erre doble"},
	{"s", "ese"},
	{"t", "te"},
	{"u", "u"},
	{"ú", "u"},
	{"v", "uve"},
	{"w", "uve doble"},
	{"x", "equis"},
	{"y", "y"},
	{"z", "zeta"},
	{"\0", "\0"}
};

/**
 * \author ?
 * \remark lista de letras del abecedario castellano y su extensión escrita tal y
 * como se pronuncia (d -> de).
 */

const t_tabla_traduc abecedario[NU_ABECEDARIO + 1] = {	/* Abecedario en galego                         */
	{"a", "a"},
	{"á", "a"},
	{"b", "be"},
	{"c", "ce"},
	{"Ç", "cedilla"},
	{"ç", "cedilla"},
	{"d", "de"},
	{"e", "e"},
	{"é", "e"},
	{"f", "efe"},
	{"g", "gue"},
	{"h", "ache"},
	{"i", "i"},
	{"í", "i"},
	{"j", "iota"},
	{"k", "ka"},
	{"l", "ele"},
	{"ll", "elle"},
	{"m", "eme"},
	{"n", "ene"},
	{"o", "o"},
	{"ó", "o"},
	{"ñ", "eñe"},
	{"p", "pe"},
	{"q", "ku"},
	{"r", "erre"},
	{"rr", "erredobre"},
	{"s", "ese"},
	{"t", "te"},
	{"u", "u"},
	{"ú", "u"},
	{"v", "uve"},
	{"w", "uvedobre"},
	{"x", "xe"},
	{"y", "i"},
	{"z", "zeta"},
	{"\0", "\0"}
};


/** \fn  void Preproceso::elimina_caracter(char *entrada, char caracter)
 *
 * Elimina todolos caracteres que coinciden con caracter da cadea apuntada
 * por entrada.
 */

void Preproceso::elimina_caracter(char *entrada, char caracter)
{
	char salida[LONX_MAX_PALABRA] = "";
	int ind_sal, lon;

	ind_sal = 0;
	lon = strlen(entrada);
	while ((salida[ind_sal++] = *entrada++) != '\0') {
		if (*entrada == caracter) {
			entrada++;
		}
	}
	entrada -= (lon + 1);
	strcpy(entrada, salida);
}

/** \fn    short int Preproceso::comprobar_se_palabras_anteriores_empezan_por(const char *comenzo,
 *                                    t_frase_separada * pos_actual_frase, char num_pal_anteriores)
 *
 * Comproba ou compara se n palabras anteriores a posicion actual dentro da
 * frase coincide coa secuencia de caracteres pasada en comenzo, devolvendose
 * un 1 en tal caso e 0 en caso contrario.
 */
short int Preproceso::
comprobar_se_palabras_anteriores_empezan_por(const char *comenzo,
											 Token * pos_actual_frase, char num_pal_anteriores)
{
	register int i;
	int lonx_inicio_pal;
	t_palabra comenzo_en_minuscula = "";
	t_palabra trozo_inicial_palabra = "";
	Token *palabra_analizada = pos_actual_frase;

	lonx_inicio_pal = strlen(comenzo);
	for (i = 1; i <= num_pal_anteriores; i++) {
		if (palabra_analizada == frase_tokenizada) {
			break;
		}
		palabra_analizada--;
		strncpy(trozo_inicial_palabra, palabra_analizada->token, lonx_inicio_pal);
		trozo_inicial_palabra[lonx_inicio_pal] = 0;
		pasar_a_minusculas(trozo_inicial_palabra, comenzo_en_minuscula);
		if (strcmp(comenzo_en_minuscula, comenzo) == 0) {
			return 1;
		}
	}
	return 0;
}

/** \fn  short int Preproceso::necesita_distincion_de_xenero(t_palabra num)
 *
 * Esta funcion indicanos que se hai nun cardinal ou dixito 1 ou 2 en posicions
 * de unidades ou unidades de millar e distinto de cero ou un en centenas ou en centenas
 * de millar o numero ten que concordar en xenero ca palabra a que acompaña.
 * Para isto analizase estas posicions. Se ten estas cifras hai que averiguar
 * o xenero da palabra. Ademais ten que concordar se se sobrepasa o cento para
 * distinguir douscentos de dúascentas, por exemplo. E necesario se hai
 * algun 1 ou dous en posicion de unidades ou centenas ou centenas e distinto
 * de cero (Tamen centenas de millar), devolvendo 1 se lle e menester face-la
 * distincion de xenero e cero en caso contrario.
 */
short int Preproceso::necesita_distincion_de_xenero(const t_palabra num)
{
	char *explorador;

	explorador = (char *) &num[strlen(num) - 1];
	if (strlen(num) > 2) {
		return 1;
	}
	/* Todolos numeros maiores de cen poden pronunciarse catrocentos ou catrocentas
	   (por exemplo) e neste caso debe haber concordancia.                        */
	if (*explorador == '1' || *explorador == '2') {
		return 1;
	}
	/* A regla e que se son cero ou un as posicions de centenas e centenas de mi-
	   -llar tampouco se precisa analizar o xenero.                               */
	if (strlen(num) >= 3) {		/* Igual para galego e castelan           */
		explorador = explorador - 2;
		if (*explorador != '0' && *explorador != '1') {
			return 1;
		}
	}
	else {
		return 0;
	}
	if (strlen(num) >= 4) {		/* Cambia de galego a castelan            */
		explorador = explorador - 1;
		if (*explorador == '1' || *explorador == '2') {
			return 1;
		}
	}
	else {
		return 0;
	}
	if (strlen(num) >= 6) {
		explorador = explorador - 2;
		if (*explorador != '0' && *explorador != '1') {
			return 1;
		}
	}
	return 0;
}

/** \fn  int Preproceso::concordancia_de_xenero(t_palabra numero_a_pronunciar,
 *                      t_palabra palabra_ca_que_concorda, unsigned char clase_de_palabra)
 *
 * Funcion que averigua o xenero da palabra que se lle pasa para que concorde cun numero.
 */
int Preproceso::concordancia_de_xenero(const t_palabra numero_a_pronunciar,
									   const t_palabra palabra_ca_que_concorda, unsigned char clase_de_palabra)
{
	t_palabra pal_en_singular;

	char *palabra_en_singular;
	//int indice;
	int xenero_do_numero_a_pronunciar;
	char *primeira_pal_da_abrev;

/* Se a palabra ca que concorda o numero e unha abreviatura hai que desabre-
   -viala antes de analizala. Deste xeito obtemos a palabra en singular.
   Primeiro comprobamos se a palabra ca que teria que concordar pode ser ana-
   -lizada, senon supoñemos que e masculino o xenero. Non teria sentido inten-
   -tar analizar o xenero a un signo de puntuacion se esta e a particula da
   frase que acompaña o numero.                                               */
	if (clase_de_palabra == ABREVIATURA ||
		clase_de_palabra == PALABRA_NORMAL_EMPEZA_MAY ||
		clase_de_palabra == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA
		|| clase_de_palabra == PALABRA_TODO_MAIUSCULAS || clase_de_palabra == PALABRA_NORMAL) {
	}
	else
		return MASCULINO;
/* Comprobamos agora se hai diferencias de xenero no numero de pronuncialo en
   masculino ou femenino xa que hai casos nos que e igual e enton non se nece-
   -sita analizar o xenero.                                                   */
	if (necesita_distincion_de_xenero(numero_a_pronunciar)) {
		if (clase_de_palabra == ABREVIATURA) {
			palabra_en_singular = (char *) diccionarios->existeEnAbreviaturas(palabra_ca_que_concorda, idioma);
			if (palabra_en_singular != NULL) {
				strcpy(pal_en_singular, palabra_en_singular);
			}
			else {
				*pal_en_singular = 0;
			}
			if ((primeira_pal_da_abrev = strchr(pal_en_singular, ' ')) != NULL) {
				*primeira_pal_da_abrev = '\0';
			}
		}
		else {
			pasar_a_minusculas(palabra_ca_que_concorda, pal_en_singular);
		}
		numero(pal_en_singular);
		xenero_do_numero_a_pronunciar = xenero(pal_en_singular);
	}
	else {
		xenero_do_numero_a_pronunciar = MASCULINO;
	}
	return xenero_do_numero_a_pronunciar;
}

/** \fn void deletrear (t_palabra entrada,t_palabra_proc sal)
 *
 * Devolve a palabra de entrada procesada na variable sal, transfomada
 *  en minusculas e con espacios entre cada letra.
 */
void deletrear(char *entrada, char *sal, char idio)
{
	char letra[2], *ent;
	t_palabra_proc aux;

	ent = entrada;
	*sal = 0;
	while (*ent != 0) {
		letra[0] = to_minusculas(*ent++);
		letra[1] = 0;
		if (dixito(*letra)) {
			(idio == GALEGO) ? strcat(sal, todos[atoi(letra)].card) :
				strcat(sal, todos_cas[atoi(letra)].card);
		}
		else {
			(idio == GALEGO) ? busca_en_lista(abecedario, letra, aux) :
				busca_en_lista(cast_abecedario, letra, aux);
			strcat(sal, aux);
		}
		strcat(sal, " ");
	}
	sal[strlen(sal) - 1] = 0;
}

/** \fn  void Preproceso::transformacion_de_palabra_en_maiusculas(t_palabra_proc sal, t_palabra ent)
 *
 * Intentamos aqui procesar aquelas palabras na que todas as letras estan en
 * maiuscula. Pode ser un titulo, caso no que simplemente haberia que
 * pasalas a minusculas ou ben ser un acronimo, caso no que hai que buscalo
 * na lista para extendelo e se non esta seria mellor pasalo a minusculas e
 * tratar de leelo. Deste xeito solo necesitariamos almacenar nas listas de
 * acronimos aqueles que non coincidan ca sua pronunciacion como acronimo.
 */

void Preproceso::transformacion_de_palabra_en_maiusculas(t_palabra_proc sal, t_palabra ent)
{

	char *aux;

	t_palabra pal_en_minusculas = "";
	if (tipo) {
		char *a = ent, *b = sal;
		while (*a) {			// Neste bucle eliminamos todo o distinto de letras.
			if (letra((unsigned char) *a) || *a == '-') {
				*b = *a;
			}
			else {
				*b = ' ';
			}
			a++;
			b++;
		}
		*b = 0;
		return;
	}
	aux = diccionarios->existeEnAcronimos(ent, idioma);
	if (aux != NULL) {
		strcpy(sal, aux);
	}
	if (strlen(sal) > 0) {
		return;
	}
	pasar_a_minusculas(ent, pal_en_minusculas);
	if (palabra_formada_por_solo_consonantes(pal_en_minusculas)) {
		deletrear(pal_en_minusculas, sal, idioma);
	}
	else {
		strcat(sal, pal_en_minusculas);
	}
}

/** \fn  int Preproceso::averiguar_numero_gramatical(t_palabra palabra_anterior,
 *                                                unsigned char clase_de_pal)
 *
 *   Nas concordancias con abreviaturas para extender estas
 * en casos como o de unidades de medida a abreviatura extendese en singular
 * se a palabra anterior e singular ou esta escrita como "1". Cantidades como
 * 1,8 ou 0,8 son plurais. Devolve SINGULAR (1) ou PLURAL (2).
 */
int Preproceso::averiguar_numero_gramatical(t_palabra palabra_anterior, unsigned char clase_de_pal)
{
	t_palabra aux;

	if (clase_de_pal == CARDINAL || clase_de_pal == DECIMAL || clase_de_pal == NUMERO_CON_PUNTO) {
		if (strcmp(palabra_anterior, "1") == 0) {
			return SINGULAR;
		}
		else {
			return PLURAL;
		}
	}
	else {
		strcpy(aux, palabra_anterior);
		return numero(aux);
	}
}

/** \fn  void Preproceso::adaptar_abreviatura_non_encontrada(char *pal_abreviada,
 *                                                 char *abreviatura_pronunciada)
 *
 * Eliminamos calquera cousa que haia polo
 * medio e sexa distinto a unha letra nestas palabras. Deixando en abreviatura_pronunciada
 * a adaptacion da abreviatura non achada pasada en pal_abreviada.                                                                   *
 */

void Preproceso::adaptar_abreviatura_non_encontrada(char *pal_abreviada, char *abreviatura_pronunciada)
{

	t_palabra pal_aux;
	char *punt_aux = pal_abreviada;

	//ZA: Atencion!!! Definicion mía.
	char *auxiliar;

	if (tipo) {
		/*    char *ent,*sal;
		   ent=pal_abreviada;
		   sal=abreviatura_pronunciada;
		   while  (*ent) {
		   if (*ent!='.') {
		   if (!maiuscula((unsigned char) *ent)) {
		   strcpy(abreviatura_pronunciada,pal_abreviada);
		   return;
		   }
		   *sal=*ent;
		   sal++;
		   }
		   ent++;
		   }
		   *sal=0;
		 */
		strcpy(abreviatura_pronunciada, pal_abreviada);
		return;
	}

	auxiliar = pal_aux;

	while (*punt_aux) {			/* Neste bucle eliminamos todo o distinto de letras. */
		if (letra((unsigned char) *punt_aux)) {
			*auxiliar++ = *punt_aux;
		}
		//strncat(pal_aux,punt_aux,1);
		if (dixito(*punt_aux)) {
			*auxiliar++ = *punt_aux;
		}
		else if (*punt_aux == 'º') {
			*auxiliar++ = 'o';
		}
		else if (*punt_aux == 'ª') {
			*auxiliar++ = 'a';
		}
		punt_aux++;
	}
	*auxiliar = 0;

/*
	if (tipo) {
		strcpy(abreviatura_pronunciada, pal_aux);
		return;
	}
*/

	auxiliar = (char *) diccionarios->existeEnAcronimos(pal_aux, idioma);


	if (auxiliar != NULL) {
		strcpy(abreviatura_pronunciada, auxiliar);
	}
	else {
	//	deletrear(pal_aux, abreviatura_pronunciada, idioma);
	//	token_a_preprocesar->clase_pal = DELETREO;

                adaptacion_por_defecto(abreviatura_pronunciada, pal_aux);
		token_a_preprocesar->clase_pal=PALABRA_NORMAL;
	}
}

/*****************************************************************************
/** \fn    void Preproceso:: transformacion_de_abreviaturas(t_palabra_proc abreviatura_pronunciada,
 *                  t_palabra pal_aux, t_palabra palabra_anterior, unsigned char clase_de_pal)
 *
 * Esta funcion extende as abreviaturas.Esta funcion recibe a palabra anterior
 * para analizala en caso de que necesite concordancia de numero (como
 * xa dixemos antes, necesario sobre todo se son abreviaturas de medidas de
 * cousas. Un problema engadido e que nestes casos o numero gramatical hai
 * que obtelo dun numero cardinal, non dunha palabra. O parametro que recibe
 * esta funcion chamado clase_pal indicanos que clase de palabra e aquela ca
 * que concorda.
*/
void Preproceso::
transformacion_de_abreviaturas(t_palabra_proc abreviatura_pronunciada,
							   t_palabra pal_aux, t_palabra palabra_anterior, unsigned char clase_de_pal)
{
	t_palabra pal_abreviada = "";
	t_palabra_proc kk;

	int num_gram;

	//ZA: Definiciones propias!
	char *auxiliar;
	//int tipo_abreviatura;

/* Xa que non se encontrou nas listas de acronimos buscamola nas distintas lis-
   -tas de abreviaturas. Primeiro eliminamos a variabilidade que puido sufri-la
   palabra e pasamola todo a minusculas.                                      */
	pasar_a_minusculas(pal_aux, pal_abreviada);

	if (*pal_abreviada == 'm' && *(pal_abreviada + 1) == '.' && cadea_numerica(pal_abreviada + 2) == CARDINAL) {
		*kk = 0;
		strcpy(abreviatura_pronunciada, "minuto ");
		numeros(pal_abreviada + 2, MASCULINO, kk);
		strcat(abreviatura_pronunciada, kk);
		return;
	}


	if (palabra_anterior != NULL) {
		num_gram = averiguar_numero_gramatical(palabra_anterior, clase_de_pal);
	}
	else {
		num_gram = SINGULAR;
	}
	auxiliar = diccionarios->existeEnAbreviaturas(pal_abreviada, (char) num_gram, idioma);
	if (auxiliar != NULL) {
		strcpy(abreviatura_pronunciada, auxiliar);
	}
	//ZA: En otro caso se prueban con los acrónimos.
	else {
		adaptar_abreviatura_non_encontrada(pal_aux, abreviatura_pronunciada);
	}
	//deletrear(abreviatura_pronunciada, abreviatura_pronunciada);
}

/* Senon esta nestas listas probamos na lista na que o plural da abreviatura
   non se forma engadindo un ese o final se non que cambia posiblemente algo
   no conxunto de palabras. Exemplo 1 kilometro hora <-> 2 kilometros hora.   */


/** \fn    void Preproceso::transcribe_cardinal_ou_ordinal(short int tip, int num,
 *                      char xenero_do_numero, char *numero_pronunciado, char por_sin_acento)
 *
 * devolve en letra o correspondente a un dixito entre cero e 9 en forma ordinal
 * ou cardinal (devolve somentes a raiz do numero, excepto se os dixitos son 1 ou 2)
 * segun corresponda e ca forma de xenero adecuada. Para iso
 * hai que entregarlle os parametros como xenero do numero para saber en que
 * xenero hai que pronuncialo ou dependendo de certos casos co numero 2 se
 * este necesita que se lle poña acento.
 */

void Preproceso::transcribe_cardinal_ou_ordinal(short int tip, int num,
												char xenero_do_numero,
												char *numero_pronunciado, char por_sin_acento)
{

	char *pnumaux;

	if (num > 9 || num < 0) {
		return;
	}
/* Segun o parametro "num" buscamos o indice da tabla e segun o tipo seleciona-
   -mos na taboa membro ordinal ou cardinal.
   Asignamolo a pnumaux e despois añadimosllo a variable global "numero_pronun-
   -ciado".                                                                   */
	if (tip == CARDINAL) {
		if (idioma == GALEGO) {
			pnumaux = todos[num].card;
		}
		else {
			pnumaux = todos_cas[num].card;
		}
	}
	else if (idioma == GALEGO) {
		pnumaux = todos[num].ord;
	}
	else {
		pnumaux = todos_cas[num].ord;
	}
/* Segun o xenero ou se vai corresponder a cardinal ou ordinal e a onde se vai
   concatenar por se resulta acentuada, escollese o literal a engadir.        */
	if (idioma == GALEGO && tip == CARDINAL && xenero_do_numero == FEMININO && (num == 1 || num == 2)) {
		if (num == 1) {
			strcat(numero_pronunciado, "unha");
		}
		else if (num == 2 && por_sin_acento) {
			strcat(numero_pronunciado, "duas");
		}
		else if (num == 2 && !por_sin_acento) {
			strcat(numero_pronunciado, "dúas");
		}
	}
	else if (idioma == CASTELLANO && tip == CARDINAL && xenero_do_numero == FEMININO && (num == 1)) {
		strcat(numero_pronunciado, "una");
	}
	else {
		strcat(numero_pronunciado, pnumaux);
	}
}

/**  \fn void Preproceso::transformacion_de_pal_empeza_maiusculas(t_palabra_proc pal_pronunciada,
 *                                                                               t_palabra pal)
 *
 * Simplemente pasamos a minusculas a palabra.
*/

void Preproceso::transformacion_de_pal_empeza_maiusculas(t_palabra_proc pal_pronunciada, t_palabra pal)
{

	t_palabra pal_en_minusculas;

	pasar_a_minusculas(pal, pal_en_minusculas);

	if (palabra_formada_por_solo_consonantes(pal_en_minusculas)) {
		deletrear(pal_en_minusculas, pal_pronunciada, idioma);
	}
	else {
		//strcat(pal_pronunciada, pal_en_minusculas);
		strcpy(pal_pronunciada, pal_en_minusculas);
	}
}

/**  \fn   void Preproceso::transformacion_de_ordinal(t_palabra_proc numero_pronunciado,
 *                                                      t_palabra num, unsigned char xen)
 *
 * En num recibese o numero a transformar en ordinal (IN), onde a devandita
 * transformacion vai en numero_pronunciado (OUT)
 * asi como o xenero (IN) que hai que acoplarlle o ordinal.
 */
void Preproceso::transformacion_de_ordinal(t_palabra_proc numero_pronunciado, t_palabra num, unsigned char xen)
{
	/* Declaracion de variables:
	   digito: array de dous caracteres, para conter o digito a transformar.
	   n     : valor que lle pasamos a funcion "transcribe_cardinal_ou_ordinal". */

	char digito[2];
	int ord_dec;
	t_palabra num_aux;
	int n;

	if (*num == '-')
		num++;

	strcpy(num_aux, num);
	num_aux[strlen(num_aux) - 1] = 0;

	if (*num == '-')
		num++;
/* Comprobacion de valores posibles de transformacion. En caso de ter valores
   incorrectos, abandona o programa, cunha mensaxe.                           */
	if (strlen(num_aux) > 2) {
		numeros(num_aux, xen, numero_pronunciado);
		strcat(numero_pronunciado, "avo");
		return;
	}
/* En castelan debemos ter en conta os casos particulares do 11 e 12 que non
   se dicen "decimo primero" e "decimo segundo" respectivamente, senon que
   son "undecimo" e "duodecimo", de ahí o tratamento especial destes dous or-
   -dinais.                                                                   */
	if (strlen(num_aux) == 2 && atoi(num_aux) >= 11) {
		ord_dec = atoi(num_aux) / 10;
		if (idioma == GALEGO)
			strcat(numero_pronunciado, ordinales_de_decenas[ord_dec]);
		else {
			if (atoi(num_aux) == 11)
				strcat(numero_pronunciado, "undécim");
			else if (atoi(num_aux) == 12)
				strcat(numero_pronunciado, "duodécim");
			else
				strcat(numero_pronunciado, ordinal_decena_cas[ord_dec]);
		}
		if (xen == MASCULINO)
			strcat(numero_pronunciado, "o ");
		else
			strcat(numero_pronunciado, "a ");
	}
	digito[0] = num[strlen(num_aux) - 1];
	digito[1] = '\0';
	n = atoi(digito);
	// FRAN_CAMPILLO: Me temo que aquí se refiere a *digito.
	//   if (galego || (!galego && !(digito==11 || digito ==12))) -> ORIGINAL
	if (idioma == GALEGO || (idioma == CASTELLANO && !(digito[0] == 11 || digito[0] == 12)))
		// FRAN_CAMPILLO
		transcribe_cardinal_ou_ordinal(ORD, n, xen, numero_pronunciado, SIN_ACENTO);
/* Segun o xenero engadimos o "o" ou "a".                                     */
	if (xen == MASCULINO)
		strcat(numero_pronunciado, "o");
	else
		strcat(numero_pronunciado, "a");
	return;
}

/**    \fn   void Preproceso::centos(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
 *
 * Esta funcion fai a transformacion de numeros de tres cifras ou
 * inferiores. Onde as variables teñen o seguinte senso:
 *  + num: E o numero a transformar, e un parametro de entrada.
 *  + xenero_do_numero: Parametro de entrada que indica se e feminino ou masculino.
 *  Para saber en que xenero hai que pronuncialo.
 *  + numero_pronunciado: Aqui e onde se deixa ou devolve o numero pronunciado.
 */

void Preproceso::centos(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
{
	char p[4] = "";
	char *n = p;
//   short int lon,m,olloun,non_houbo_decenas;
	int lon, m, olloun, non_houbo_decenas;
/* Significado  da derradeira linea de variables:
      olloun           : dice se temos que transformar solo un numero.
      non_houbo_decenas: dice se temos que engadir o literal "e" ou  non.
   Exemplo: "102"->"cento duas", non "cento e duas".                          */

	non_houbo_decenas = 0;
	lon = strlen(num);
	olloun = lon;
/* Neste bloque traballamos co dixito correspondente as centenas e comprobamos
   se e 100 pois enton lese cen.                                              */
	if (!strcmp(num, "100")) {
		(idioma == GALEGO) ? strcat(numero_pronunciado, "cen") : strcat(numero_pronunciado, "cien");
		lon = 0;
	}
	if (lon == 3) {				/* BEGIN IF 1                             */
		*n++ = *num++;
		*n-- = '\0';
		m = atoi(n);			/* Collo o primeiro dos tres nº: centenas */
		if (m) {				/* BEGIN IF 2                             */
			if (m != 1) {		/* BEFIN IF 3                             */
				if (idioma == CASTELLANO) {	/* Tratase o 9 para castelan: novecient@s */
					if (m == 9)
						strcat(numero_pronunciado, "nove");
					else if (m == 7)
						strcat(numero_pronunciado, "sete");
					else if (m == 5)
						strcat(numero_pronunciado, "quinient");
					else
						transcribe_cardinal_ou_ordinal(CARDINAL, m,
													   xenero_do_numero, numero_pronunciado, SIN_ACENTO);
				}
				else
					transcribe_cardinal_ou_ordinal(CARDINAL, m,
												   xenero_do_numero, numero_pronunciado, SIN_ACENTO);
				if (xenero_do_numero == FEMININO) {
					if (idioma == GALEGO)
						strcat(numero_pronunciado, "centas ");
					else {
						if (m == 5)
							strcat(numero_pronunciado, "as ");
						else
							strcat(numero_pronunciado, "cientas ");
					}
				}
				else {
					if (idioma == GALEGO)
						strcat(numero_pronunciado, "centos ");
					else {
						if (m == 5)
							strcat(numero_pronunciado, "os ");
						else
							strcat(numero_pronunciado, "cientos ");
					}
				}
			}					/* END IF 3                               */
			else {
				if (idioma == GALEGO)
					strcat(numero_pronunciado, "cento");
				else
					strcat(numero_pronunciado, "ciento");
			}
		}						/* END IF 2                               */
		lon--;
	}							/* END IF 1                               */
	/* Aqui traballamos cas decenas , e decir con numeros entre 0 e 99. Os numeros
	   entre 10 e vinte lense de xeito irregular. Cos numeros da decena de vinte
	   forman unha soa palabra.                                                   */
	if (lon == 2 && idioma == GALEGO) {	/* BEGIN IF 1                             */
		if (!strcmp(num, "21") && xenero_do_numero == MASCULINO) {
			strcat(numero_pronunciado, "vinteún");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "21") && xenero_do_numero == FEMININO) {
			strcat(numero_pronunciado, "vinteúnha");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "23")) {
			strcat(numero_pronunciado, "vintetrés");
			//         lon=0;
			return;
		}
		*n++ = *num++;
		*n-- = '\0';
		m = atoi(n);			/* Quedase coas decenas                   */
		if (m) {				/* BEGIN IF 2                             */
			if (m == 1) {		/* Decenas de 10 -> casos especiales      */
				*n++ = *num++;
				*n-- = '\0';
				m = atoi(n);
				if (idioma == GALEGO)
					strcat(numero_pronunciado, especiales[m]);
				else
					strcat(numero_pronunciado, especiales_cas[m]);
				lon = 0;
			}					/*END IF: Decenas, casos especiales       */
			else
				strcat(numero_pronunciado, decenas[m - 1]);
			if (m == 2)
				non_houbo_decenas = 1;
		}						/* END IF 2                               */
		else
			non_houbo_decenas = 1;
		lon--;
	}							/* END IF 1                               */
	else if (lon == 2 && idioma == CASTELLANO) {	/* BEGIN IF 1                             */
		if (!strcmp(num, "20")) {
			strcat(numero_pronunciado, "veinte");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "21") && xenero_do_numero == MASCULINO) {
			strcat(numero_pronunciado, "veintiún");
			//         lon=0;                     /* Se non e adxectivo e VEINTIUNO         */
			return;
		}
		if (!strcmp(num, "21") && xenero_do_numero == FEMININO) {
			strcat(numero_pronunciado, "veintiuna");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "22")) {
			strcat(numero_pronunciado, "veintidós");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "23")) {
			strcat(numero_pronunciado, "veintitrés");
			//         lon=0;
			return;
		}
		if (!strcmp(num, "26")) {
			strcat(numero_pronunciado, "veintiséis");
			//         lon=0;
			return;
		}
		*n++ = *num++;
		*n-- = '\0';
		m = atoi(n);
		if (m) {				/* BEGIN IF 2                             */
			if (m == 1) {
				*n++ = *num++;
				*n-- = '\0';
				m = atoi(n);
				if (idioma == GALEGO)
					strcat(numero_pronunciado, especiales[m]);
				else
					strcat(numero_pronunciado, especiales_cas[m]);
				lon = 0;
			}
			else
				strcat(numero_pronunciado, dec_cas[m - 1]);
			if (m == 2)
				non_houbo_decenas = 1;
		}						/* END IF 2                               */
		else
			non_houbo_decenas = 1;
		lon--;
	}							/* END IF 1                               */
/* Bloque para as unidades. Controla si "num" e dun solo dixito e nese caso
   non pon a cadena "e" en "numero_pronunciado".                           */
	if (lon == 1) {
		m = atoi(num);
		if (m) {
			if (olloun != 1) {
				if (non_houbo_decenas)
					strcat(numero_pronunciado, "\0");
				else if (idioma == GALEGO)
					strcat(numero_pronunciado, " e \0");
				else
					strcat(numero_pronunciado, " y \0");
			}
			transcribe_cardinal_ou_ordinal(CARDINAL, m, xenero_do_numero, numero_pronunciado, CON_ACENTO);
		}
	}
	return;
}

/**  \fn void Preproceso::miles(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
 *
 *  Funcion que calcula os miles. A funcion comproba a entrada. Se e
 * un "1" transformao no seu literal correspondente que e "mil" e se non chama
 * a funcion "centos" que transforma "num" no seu literal e posteriormente engadelle "mil".
 * Por ex.: num=12 <-> numero_pronunciado="..doce mil". *
 */
 
void Preproceso::miles(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
{
	int lon;					/* Lonxitude da cadea parametro "num".    */

	if (!strcmp(num, "000")){
		return;
	}
	lon = strlen(num);
	if ((lon != 1) || (*num != '1')) {
		centos(num, xenero_do_numero, numero_pronunciado);
		if (*numero_pronunciado && !strcmp("uno", numero_pronunciado + (strlen(numero_pronunciado) - 3))){
			numero_pronunciado[strlen(numero_pronunciado) - 1] = 0;
		}
	}
	strcat(numero_pronunciado, " mil ");
}

/** \fn  void Preproceso::millons(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
 *
 * Funcion que calcula os millons. A funcion comproba a entrada. Se
 * e un "1" transformao no seu literal correspondente que e "un millon" e se
 * non chama a funcion "centos" que transforma "num" no seu literal e
 * posteriormente engadelle "millons".
 * Por exemplo: num=12 <-> numero_pronunciado="..doce millons".
 */
void Preproceso::millons(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
{
	int lon;					/* lonxitude da cadena parametro "num".  */

	lon = strlen(num);
	if ((lon != 1) || (*num != '1')) {
		centos(num, xenero_do_numero, numero_pronunciado);
		if (!strcmp("uno", numero_pronunciado + (strlen(numero_pronunciado) - 3))){
			numero_pronunciado[strlen(numero_pronunciado) - 1] = 0;
		}
		(idioma == GALEGO) ? strcat(numero_pronunciado,
									" millóns ") : strcat(numero_pronunciado, " millones ");
	}
	else {
		strcat(numero_pronunciado, "un millón ");
	}
}


/** \fn void Preproceso::numeros(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
 *
 *  Funcion que transforma a cadea "num", formada por dixitos, en letra.                                                           *
 */

void Preproceso::numeros(char *num, char xenero_do_numero, t_palabra_proc numero_pronunciado)
{
//   short int lon,lon2;
	int lon, lon2;
	char n[4];
/* Declaracion de variables:
      lon   : longitud da cadena parametro de entrada.
      lon2  : variable auxiliar que resulta o dividir a cadea grande en grupos
              de tres, para o seu posterior tratamento.
      n     : array de caracteres auxiliar para que a funcion vaia tratando a
              cadea principal en grupos de tres.                              */

	elimina_caracter(num, '.');
	lon = strlen(num);
/* Por se trae puntos ca mision de separar os grupos de 3 dixitos.            */
	if (lon > (LONX_MAX_NUM - LONX_MAX_DECIMAL - 2)
		|| (lon > 1 && *num == '0')) {
		//num[LONX_MAX_NUM-LONX_MAX_DECIMAL-2]=0;
		deletrea_numero(num, numero_pronunciado);
		return;
		//fprintf(stderr,"Este número e demasiado grande. \n");
		//fprintf(stderr,"Repase o texto de entrada. Ten un número de máis de doce díxitos. \n");
		//exit(0);
	}
	if (strcmp(num, "0") == 0 || strcmp(num, "00") == 0 || strcmp(num, "000") == 0) {
		transcribe_cardinal_ou_ordinal(CARDINAL, 0, xenero_do_numero, numero_pronunciado, SIN_ACENTO);
		return;
	}
/* Neste bloque buscamos se temos que leer números de doce cifras. Si ocurre,
   metemolo na variable "n" e chamamo-la funcion "miles". A variable "lon" vai
   ir sendo modificada, unha vez que se vaian transformando os bloques de tres
   cifras.                                                                    */
	if ((lon > 9) && (lon <= 12)) {
		lon2 = lon - 9;
		strncpy(n, num, lon2);
		n[lon2] = 0;
		num += lon2;
		miles(n, MASCULINO, numero_pronunciado);
		lon = 9;
	}
	n[0] = '\0';
/* Neste bloque miramos se temos que tratar numeros de  entre 6 e nove cifras
   e chamamos a funcion millons                                               */
	if ((lon > 6) && (lon <= 9)) {
		lon2 = lon - 6;
		strncpy(n, num, lon2);
		n[lon2] = 0;
		num += lon2;
		millons(n, MASCULINO, numero_pronunciado);
		lon = 6;
	}
	n[0] = '\0';
/* Se temos que ler numeros cero e un millon chamamos a funcion miles         */
	if ((lon > 3) && (lon <= 6)) {
		lon2 = lon - 3;
		strncpy(n, num, lon2);
		n[lon2] = 0;
		num += lon2;
		miles(n, xenero_do_numero, numero_pronunciado);
		lon = 3;
	}
	n[0] = '\0';
	if (lon <= 3)
		centos(num, xenero_do_numero, numero_pronunciado);
	return;
}

/**  \fn   void Preproceso::lee_mes(char *mes, t_palabra_proc data_pronunciada)
 *
 * Funcion que lee o mes a partir do numero especificado. Lee da taboa "meses".
 */
 
void Preproceso::lee_mes(char *mes, t_palabra_proc data_pronunciada)
{
	int nom = atoi(mes);
	(idioma == GALEGO) ? strcat(data_pronunciada, meses[nom - 1]) : strcat(data_pronunciada, mes_cas[nom - 1]);
}

/** \fn    int Preproceso::datas(char *dia, char *mes, char *anno, t_palabra_proc data_pronunciada)
 *
 * Traduce a data, deixandoa en data_pronunciada.
 */
int Preproceso::datas(char *dia, char *mes, char *anno, t_palabra_proc data_pronunciada)
{
	const char *pegar_de = " de ";
	const char *pegar_del = " del ";
	const char *pegar_do = " do ";
	char temp[3];
	int erro = OK;
	/* Declaracion de variables:
	   pegar_de: variable que conten a cadena " de ".
	   pegar_do: variable que conten a cadena " do ".                          */

	if ((atoi(dia)) > 31) {		/* Comprobamos se a data esta no rango adecuado */
		//      fprintf(stderr,"Error na data, o día ten que ser menor que 31");
		adaptacion_por_defecto(data_pronunciada, dia);
		erro = ERROR_FECHAS;
	}
	if ((atoi(mes)) > 12) {
		//      fprintf(stderr,"Error na data,o mes ten que ser menor que 13");
		if ((atoi(dia)) < 12 && (!erro) && (atoi(mes)) <= 31) {
			strcpy(temp, mes);	/* Data en formato americano-> intercambiamos   */
			strcpy(mes, dia);	/* dia e mes, axudandonos da var temporal temp  */
			strcpy(dia, temp);
		}
		else {
			if ((atoi(dia)) < 31)
				adaptacion_por_defecto(data_pronunciada, dia);
			adaptacion_por_defecto(data_pronunciada, mes);
			adaptacion_por_defecto(data_pronunciada, anno);
			erro = ERROR_FECHAS;
		}
	}
	/* 28-12-1995->numero_pronunciado="vinteoito de Decembro do mil novecentos
	   noventa e cinco".                                                          */
	if (!erro) {
		numeros(dia, MASCULINO, data_pronunciada);
		strcat(data_pronunciada, pegar_de);
		lee_mes(mes, data_pronunciada);
		if (idioma == GALEGO)
			strcat(data_pronunciada, pegar_do);
		else {
			if (strlen(anno) < 4)	/* Se ten so duas cifras lese: del 99     */
				strcat(data_pronunciada, pegar_del);
			else				/* Se ten o ano completo lese de 1.999    */
				strcat(data_pronunciada, pegar_de);
		}
		numeros(anno, MASCULINO, data_pronunciada);
	}
	return erro;
}

/** \fn  void Preproceso::transformacion_numeros_con_guion_no_medio(t_palabra_proc cadea_pronunciada,
 *                                                              char *punt_numeros)
 *
 * Fai a transformacion de numeros con guions no medio.                                                          *
 */

 void Preproceso::
transformacion_numeros_con_guion_no_medio(t_palabra_proc cadea_pronunciada, char *punt_numeros)
{
	char *aux = punt_numeros;
	t_palabra cifra;

	while (*aux) {
		while (!dixito(*aux) && *aux != 0)
			aux++;
		if (*aux == 0){
			break;
		}
		*cifra = 0;
		while (dixito(*aux) && *aux != 0) {
			strncat(cifra, aux, 1);
			aux++;
		}
		if (*cadea_pronunciada && cadea_pronunciada[strlen(cadea_pronunciada) - 1] != ' '){
			strcat(cadea_pronunciada, " ");
		}
		numeros(cifra, MASCULINO, cadea_pronunciada);
	}
}

/** \fn int Preproceso::transformacion_de_datas(t_palabra_proc data_pronunciada, char *data)
 *
 * Funcion que transforma unha data no formato numerico, con "/"
 * o "-", e deixaa en "data_pronunciada".
 */
 
int Preproceso::transformacion_de_datas(t_palabra_proc data_pronunciada, char *data)
{
	char dia[3], mes[3], anno[LONX_MAX_NUM], *a=anno;
	short int i, d, m;
	//   int erro=OK;

	i = 1;
	d = 0;
	m = 0;
	while (!dixito(*data)|| *data=='0'){
		data++;
	}
	if (i == 1) {				/* Obtemos o dia                          */
		while (*data != '-' && *data != '/')
			dia[d++] = *data++;
		++data;
		dia[d] = '\0';
		i++;
	}
	if (i == 2 && strlen(dia) <= 2) {	/* Obter mes                              */
		while (*data != '-' && *data != '/')
			mes[m++] = *data++;
		data++;
		mes[m] = '\0';
	}
	while(dixito(*data) || *data=='.') {
		*a++ = *data++;
	}
	*a=0;
	//strcpy(anno, data);			/* O resto e o ano                        */
	/*
	   erro=datas(dia,mes,anno,data_pronunciada);
	   return erro;
	 */
	return datas(dia, mes, anno, data_pronunciada);
}

/** \fn  void Preproceso::transformacion_horas(t_palabra_proc hora_pronunciada, char *num)
 *
 * Funcion que lee as horas segun o formato acordado. O caracter separador
 * son os dous puntos. O primeiro grupo son as horas en femenino e os
 * minutos en masculino
 */

void Preproceso::transformacion_horas(t_palabra_proc hora_pronunciada, char *num)
{
	t_palabra kk;
	char *pos;

	strcpy(kk, num);
	pos = strtok(kk, ":");
	if (*(pos + 1) && *pos == '0'){
		pos++;
	}
	numeros(pos, FEMININO, hora_pronunciada);
	while ((pos = strtok(NULL, ":")) != NULL) {
		strcat(hora_pronunciada, " ");
		numeros(pos, MASCULINO, hora_pronunciada);
	}
}

/** \fn void Preproceso::transformacion_de_partitivo(t_palabra_proc fraccion_pronunciada,
 *          t_palabra palabra_posterior, unsigned char clase_pal_posterior, t_palabra fraccion)
 *
 * As reglas para pronunciar os quebrados son un medio,tercio e despois ordinal.
 * No caso que o numerador sexa maior ca un o denominador lese en plural.
 * O numerador lese en cardinal.
 */
void Preproceso:: transformacion_de_partitivo(t_palabra_proc fraccion_pronunciada,
							t_palabra palabra_posterior, unsigned char clase_pal_posterior, t_palabra fraccion) {

	char numerador[LONX_MAX_NUM], denominador[LONX_MAX_NUM];
	char *pos_barra;
	int xen_partitivo;
	int n;
	int plural;

	pos_barra = strchr(fraccion, '/');
	strncpy(numerador, fraccion, pos_barra - fraccion);
	numerador[pos_barra - fraccion] = 0;
	pos_barra++;
	strcpy(denominador, pos_barra);
	xen_partitivo = concordancia_de_xenero("1", palabra_posterior, clase_pal_posterior);
/* Supoñenos que a palabra ca que concorda e a posterior. E a que analizamos
   se esta palabra ten sentido analizala.                                     */
	numeros(numerador, (char) xen_partitivo, fraccion_pronunciada);
	strcat(fraccion_pronunciada, " ");
	n = atoi(denominador);
	plural = atoi(numerador);
	if (n < 100) {				/* BEGIN IF                               */
		switch (n) {			/* BEGIN SWITCH                           */
		case 2:
				if (xen_partitivo == MASCULINO){
				strcat(fraccion_pronunciada, "medio");
				}
				else{
				strcat(fraccion_pronunciada, "media");
				}
			break;
		case 3:
			strcat(fraccion_pronunciada, "tercio");
			break;
		default:
			if (n < 11) {
					transcribe_cardinal_ou_ordinal(ORD, n, (char) xen_partitivo, fraccion_pronunciada, SIN_ACENTO);
					if (xen_partitivo == MASCULINO){
					strcat(fraccion_pronunciada, "o");
					}
					else{
					strcat(fraccion_pronunciada, "a");
			}
				}
			else {
				numeros(denominador, (char) xen_partitivo, fraccion_pronunciada);
					if (xen_partitivo == MASCULINO){
					strcat(fraccion_pronunciada, "avo");
					}
					else{
					strcat(fraccion_pronunciada, "ava");
					}
				}
		}						/* END SWITCH                             */
		if (plural > 1){
			strcat(fraccion_pronunciada, "s");
		}
	}							/* END IF                                 */
}

/**  \fn  void Preproceso:: transformacion_de_numero_con_punto(t_palabra_proc num_pronunciado,
 *                       char *num, t_palabra palabra_ca_que_concorda, unsigned char clase_de_pal)
 *
 * Nos numeros con punto lese primeiro a parte anterior, despois lese punto e
 * mais tarde engadimos a parte posterior. Para que necesite un
 * analisis de xenero debe necesitalo ou a parte posterior ou a anterior.
 * Lense por separado e concatenanse as duas partes ca palabra ".. punto .."
 */
void Preproceso::
transformacion_de_numero_con_punto(t_palabra_proc num_pronunciado, char *num,
								   t_palabra palabra_ca_que_concorda, unsigned char clase_de_pal)
{
	char antes_punto[LONX_MAX_NUM], despois_punto[LONX_MAX_NUM];
	char *pos_punto, negativo;
	int xen = NO_ASIGNADO;

	if (*num == '-') {
		num++;
		negativo = 1;
	}
	else
		negativo = 0;

	pos_punto = strchr(num, '.');
	strncpy(antes_punto, num, pos_punto - num);
	antes_punto[pos_punto - num] = 0;
	pos_punto++;
	strcpy(despois_punto, pos_punto);
	if ((*antes_punto && necesita_distincion_de_xenero(antes_punto)) ||
		(*despois_punto && necesita_distincion_de_xenero(despois_punto)))
		xen = concordancia_de_xenero("1", palabra_ca_que_concorda, clase_de_pal);
	if (negativo)
		strcpy(num_pronunciado, "menos ");
	numeros(antes_punto, (char) xen, num_pronunciado + 6 * negativo);
	if (*pos_punto)
		strcat(num_pronunciado, " punto ");
	numeros(despois_punto, (char) xen, num_pronunciado + 6 * negativo);
}

/**  \fn  void Preproceso::transformacion_decimal(t_palabra_proc numero_pronunciado,
 *                      char *num, t_palabra palabra_ca_que_concorda, unsigned char clase_de_pal)
 *
 * Obtenhense por separado a parte enteira e decimal mirando se necesita
 * concordancia. Isto pode ser se o necesitan ou a parte enteira  ou a decimal.
 * Se a parte decimal ten mais de tres dixitos simplemente enumeranse os
 * seguintes como se fosen independentes.
 */
 
void Preproceso::transformacion_decimal(t_palabra_proc numero_pronunciado,
										char *num,
										const t_palabra palabra_ca_que_concorda, unsigned char clase_de_pal)
{
	int xenero_do_numero = NO_ASIGNADO;
	char *par_ent, *par_dec, digito[2] = "";
	int ind_coma, param_cardord;

	elimina_caracter(num, '.');	//Obtemos parte enteira e decimal
	ind_coma = strcspn(num, ",");

	par_ent = num;
	par_dec = num + ind_coma + 1;
	num[ind_coma] = 0;
	if (necesita_distincion_de_xenero(par_ent) || necesita_distincion_de_xenero(par_dec)) {
		xenero_do_numero = concordancia_de_xenero(par_ent, palabra_ca_que_concorda, clase_de_pal);
	}
/* Transformamo-la parte enteira, seguida do literal "coma".                  */
	numeros(par_ent, (char) xenero_do_numero, numero_pronunciado);
	strcat(numero_pronunciado, " coma ");
/* Os tres primeiros dixitos forman unha unidade e os seguintes enumeramolos
   simplemente despois do primeiro grupo de tres pero sin especificar se se 
   trata de decimas,centesimas,etc.                                           */

	if (strlen(par_dec) > (LONX_MAX_DECIMAL)) {	//fmendez
		deletrea_numero(par_dec, &numero_pronunciado[strlen(numero_pronunciado) - 1]);
		*(par_dec - 1) = ',';
		return;
	}
	ind_coma = 0;
	if (strlen(par_dec) > 3) {	// BEGIN IF 1
		digito[1] = 0;
		while (par_dec[ind_coma]) {	// BEGIN WHILE
			digito[0] = par_dec[ind_coma++];
			if (isdigit(digito[0])) {	// BEGIN IF 2
				param_cardord = atoi(digito);
				numero_pronunciado[strlen(numero_pronunciado)] = 0;
				transcribe_cardinal_ou_ordinal(CARDINAL, param_cardord,
											   (char) xenero_do_numero, numero_pronunciado, CON_ACENTO);
				if (par_dec[ind_coma])
					strcat(numero_pronunciado, " ");
			}					// END IF 2
		}						// END WHILE
	}							// END IF 1
	else
		numeros(par_dec, (char) xenero_do_numero, numero_pronunciado);
	*(par_dec - 1) = ',';
	return;
}


/**  \fn void Preproceso:: transformacion_de_tanto_por_cento(t_palabra_proc numero_en_letra,
 *                                                               t_palabra porcento)
 *
 * Lese a primeira parte en numero e despois engadese "por
 * cento". Non necesita analise de xenero.
 */
 
void Preproceso:: transformacion_de_tanto_por_cento(t_palabra_proc numero_en_letra, t_palabra porcento)
{
	t_palabra aux;
	strcpy(aux, porcento);
	aux[strlen(porcento) - 1] = 0;
	if (strchr(aux, ','))
		transformacion_decimal(numero_en_letra, aux, "o", PALABRA_NORMAL);
	//else if (strchr(aux,'.'))
	//   transformacion_de_numero_con_punto(numero_en_letra, aux,"o",PALABRA_NORMAL);
	else
		numeros(aux, MASCULINO, numero_en_letra);
#ifdef _MODO_NORMAL
	(idioma == GALEGO) ? strcat(numero_en_letra, " por cento") : strcat(numero_en_letra, " por ciento");
#else
	(idioma == GALEGO) ? strcat(numero_en_letra, " por cen") : strcat(numero_en_letra, " por ciento");
#endif
}

/** \fn void Preproceso::
 *    transformacion_de_valor_de_grados(t_palabra_proc numero_en_letra, t_palabra grados, char decimal)
 *
 * Pronunciar o numero mais " graos".
 */
 
void Preproceso::
transformacion_de_valor_de_grados(t_palabra_proc numero_en_letra, t_palabra grados, char decimal)
{
	t_palabra kk;
	char *aux = kk;
	t_palabra_proc aux2;
	char *simbolo;

	*aux2 = 0;
	strcpy(kk, grados);
	if (*aux == '-') {
		strcpy(numero_en_letra, "menos ");
		aux++;
	}
	else
		*numero_en_letra = 0;

	if ((simbolo = strrchr(aux, simbolo_de_grados)) == NULL)
		simbolo = strrchr(aux, simbolo_ordinal_masculino);
	if (simbolo != NULL)
		*simbolo = 0;


	//aux[strlen(aux)-1]=0;
	if (decimal == 0)
		numeros(aux, MASCULINO, aux2);
	else
		transformacion_decimal(aux2, aux, "graos", PALABRA_NORMAL);

	strcat(numero_en_letra, aux2);
	(idioma == GALEGO) ? strcat(numero_en_letra, " graos") : strcat(numero_en_letra, " grados");
}

/** \fn  short int Preproceso::valor_equivalente(unsigned char letra)
 *
 * Valor equivalente a cada letra romana(ou numero romano). Como entrada a
 * letra e saida o valor nun enteiro.Esta funcion e chamada por Pasar_a_arabigo.
 */
short int Preproceso::valor_equivalente(unsigned char letra)
{

	switch (letra) {
	case 'M':
		return 1000;
	case 'D':
		return 500;
	case 'C':
		return 100;
	case 'L':
		return 50;
	case 'X':
		return 10;
	case 'V':
		return 5;
	case 'I':
		return 1;
	default:
		return 0;
	}
}

/**   \fn  unsigned short int Preproceso::pasar_a_arabigo(t_palabra numero_romano)
 *
 * Devolve o enteiro equivalente a un numero romano. Empezamos a ler pola dereita e se
 * o valor da letra e maior ou igual incrementase o acumulado e se non disminuese nese valor.                                                        *
 */
 
unsigned short int Preproceso::pasar_a_arabigo(t_palabra numero_romano)
{

	unsigned short int total = 0, valor_anterior = 0, valor_actual;
	int i, num_carac;
	char *romano;

	romano = &numero_romano[strlen(numero_romano) - 1];
	num_carac = strlen(numero_romano);
	for (i = 1; i <= num_carac; i++) {
		valor_actual = valor_equivalente(*romano);
		if (valor_actual >= valor_anterior)
			total += valor_actual;
		else
			total -= valor_actual;
		valor_anterior = valor_actual;
		if (romano > numero_romano)
			romano--;
	}
	return total;
}

/** \fn  void Preproceso::transformacion_de_romanos(t_palabra_proc numero_pronunciado,
 *            t_palabra numero_romano, t_palabra palabra_anterior, unsigned char clase_pal_anterior)
 *
 * Esta funcion debe analizar como se debe pronunciar un numero romano,
 * e decir, se e menor que 10 en ordinal, senon en cardinal (para casos de
 * nomes propios). Tamen  analiza o xenero da palabra que o antecede
 * para concordar con el.
 */
 
void Preproceso::transformacion_de_romanos(t_palabra_proc numero_pronunciado,
										   t_palabra numero_romano,
										   t_palabra palabra_anterior, unsigned char clase_pal_anterior)
{
	unsigned short int arabigo;
	t_palabra numero_aux;
	int xen_numero_romano;

	arabigo = pasar_a_arabigo(numero_romano);
	sprintf(numero_aux, "%d", arabigo);
	xen_numero_romano = concordancia_de_xenero(numero_aux, palabra_anterior, clase_pal_anterior);

// FRAN_CAMPILLO: Le he añadido la condición de que la palabra_anterior sea NULL
// para el caso de la primera palabra de la frase. Me parece que también se podría
// cambiar el atoi(numero_aux), ya que tenemos el mismo valor en arabigo.

	if ((clase_pal_anterior == PALABRA_NORMAL_EMPEZA_MAY ||
		 (clase_pal_anterior == PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA)
		 || !palabra_anterior) && atoi(numero_aux) < 11) {
		strcat(numero_aux, "º");
		transformacion_de_ordinal(numero_pronunciado, numero_aux, (unsigned char) xen_numero_romano);
	}
	else
		numeros(numero_aux, (char) xen_numero_romano, numero_pronunciado);
}

/** 
 * \author 
 * \remark 
 * 
 * \param numero_a_pronunciar 
 * \param pos_actual_frase 
 * 
 * \return 
 */
short int Preproceso::
comprobar_se_se_trata_de_num_telefono(t_palabra numero_a_pronunciar, Token * pos_actual_frase)
{
	int lonx;

	lonx = strlen(numero_a_pronunciar);
	if (lonx == 6 || lonx == 7) {	/* Os nums de telfno son desta lonxitude  */
		if (comprobar_se_palabras_anteriores_empezan_por("tel", pos_actual_frase, 10))
			/* Comprobamos se das palabras que a preceden (10 neste caso) por se hai a 
			   secuencia "tel 234567 ou 234890 "  na que o segundo numero non leva o la-
			   -do ningunha palabra que empeza por tel e sen embargo e un numero de te-
			   -lefono, para indicarnos que se trata dun numero de telefono               */
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

/**   \fn  void Preproceso::transformar_num_de_telefono(t_palabra_proc numero_en_letra,
 *                                                      t_palabra numero_a_pronunciar)
 *
 * Exemplo de lectura 340840 "trinta e catro cero oito
 * corenta". Sempre en grupos de dous excepto se a lonxitude e impar caso no
 * que se lee o primeiro dixito separado ou se nun grupo de dous o primeiro
 * e cero, caso no que se lee "cero... oito ".
 */

void Preproceso::transformar_num_de_telefono(t_palabra_proc numero_en_letra, t_palabra numero_a_pronunciar)
{
	int lonx_pal, i;
	char trozo_numero[3];
	char numero_individual[2];
	char *pos_grupo_a_pronunciar = numero_a_pronunciar;

	lonx_pal = strlen(numero_a_pronunciar);
	//fmendez   if ( fmod(lonx_pal,2)==1) {
	if (lonx_pal % 2 == 1) {
		/* Se e impar a primeira cifra lese solta e despois en grupos de dous         */
		lonx_pal--;
		//fmendez      strncpy(trozo_numero,pos_grupo_a_pronunciar,1);
		trozo_numero[0] = *pos_grupo_a_pronunciar;
		trozo_numero[1] = 0;
		numeros(trozo_numero, MASCULINO, numero_en_letra);
		strcat(numero_en_letra, " ");
		pos_grupo_a_pronunciar++;
	};
	for (i = 1; i <= lonx_pal / 2; i++) {	/* BEGIN FOR                              */
		strncpy(trozo_numero, pos_grupo_a_pronunciar, 2);
		trozo_numero[2] = 0;
		if (trozo_numero[0] == '0') {	/* BEGIN IF                               */
			/* Se en cada grupo de dous numeros o primeiro e un cero enton non se lee co-
			   -mo  grupo de dous senon individualmente.                                  */
			strcat(numero_en_letra, "cero ");
			/* Leemos agora o segundo dixito deste grupo de dous que conten un cero       */
			// fmendez        strncpy(numero_individual,trozo_numero+1,1);
			numero_individual[0] = trozo_numero[1];
			numero_individual[1] = 0;
			numeros(numero_individual, MASCULINO, numero_en_letra);
			strcat(numero_en_letra, " ");
		}						/* END IF                                 */
		else {
			numeros(trozo_numero, MASCULINO, numero_en_letra);
			strcat(numero_en_letra, " ");
		}
		pos_grupo_a_pronunciar = pos_grupo_a_pronunciar + 2;
	}							/* END FOR                                */
	return;
}

/** \fn void Preproceso::transformacion_de_cardinal(t_palabra_proc numero_en_letra,
 *                            t_palabra numero_a_pronunciar, t_palabra pal_ca_que_concorda,
 *                            unsigned char clase_de_palabra, t_frase_separada * pos_actual_frase)
 *
 * Transforma un numero cardinal, facendo as concordancias ca palabra que se
 * lle pasa como parametro.
 */
void Preproceso::transformacion_de_cardinal(t_palabra_proc numero_en_letra,
											t_palabra numero_a_pronunciar,
											t_palabra pal_ca_que_concorda,
											unsigned char clase_de_palabra,
											Token * pos_actual_frase)
{
	int xenero;
	char negativo;

	if (*numero_a_pronunciar == '-')
		negativo = 1;
	else
		negativo = 0;


	if (comprobar_se_se_trata_de_num_telefono(numero_a_pronunciar + negativo, pos_actual_frase)) {
		transformar_num_de_telefono(numero_en_letra, numero_a_pronunciar + negativo);
		return;
	}
	/* Se non e numero de telefono leeremolo normalmente
	 */
	if (negativo)
		strcpy(numero_en_letra, "menos ");
	xenero = concordancia_de_xenero(numero_a_pronunciar + negativo, pal_ca_que_concorda, clase_de_palabra);
	numeros(numero_a_pronunciar + negativo, (char) xenero, numero_en_letra + 6 * negativo);
}

/** \fn  void Preproceso::transformacion_de_inicial(t_palabra_proc letra_pronunciada, t_palabra pal)
 *
 *J.R.  hai que lelo iota erre, e decir deletrealo, normalmente sen
 * pronuncialo punto.
 */
void Preproceso::transformacion_de_inicial(t_palabra_proc letra_pronunciada, t_palabra pal)
{
	t_palabra aux = "";
	char *auxi;

	strcpy(aux, pal);
	pasar_a_minusculas(aux, aux);
	/* Comprobamos primeiro se esta inicial é en realidade unha abreviatura (por
	   exemplo "D.").                                                             */
	/*
	   (idioma==GALEGO)?busca_en_lista(abreviaturas,aux,letra_pronunciada):busca_en_lista(cast_abrevs,aux,letra_pronunciada);
	   if (strlen(letra_pronunciada)!=0) return;
	   (idioma==GALEGO)?busca_en_lista(abrev_con_ext_simple,aux,letra_pronunciada):busca_en_lista(cast_abrev_con_ext_simple,aux,letra_pronunciada);
	   if (strlen(letra_pronunciada)!=0) return;
	 */
	//ZA.

	auxi = diccionarios->existeEnAbreviaturas(aux, idioma);
	if (auxi == NULL)
		strcpy(letra_pronunciada, auxi);
	else
		*letra_pronunciada = 0;
	if (strlen(letra_pronunciada) != 0)
		return;
	/* Se non é abreviatura deletreámola.                                         */
	strcpy(aux, pal);
	aux[1] = 0;					/* Quitamo-lo punto                       */
	deletrear(aux, letra_pronunciada, idioma);
	return;
}

/**  \fn  int Preproceso::unidades_de_medida_sin_punto(char *entrada, char *saida, char *pal_anterior)
 *
 * Funcion que recibe en entrada a palabra a analizar, suposta unidade de medida,
 * en pal_anterior a verba anterior, que nos servira para desambigualo numero da unidade,
 * buscando na taboa adecuada segundo o idioma, reflexado na variable galego, e dandose
 * o resultado na variable saida, devolvendose ademais un 1 se se trataba dunha unidade.
 * (Lembrar que a variable galego e global e esta definida en var_glo2.c).                                                       *
 */

int Preproceso::unidades_de_medida_sin_punto(char *entrada, char *saida, char *pal_anterior)
{

	//int pos_lista;
	//   t_palabra_proc pal_extendida="";

	char *pal_extendida;

	t_palabra pal_buscada = "";

	//char numero;

	char *auxi;

	strcpy(pal_buscada, entrada);
	pasar_a_minusculas(pal_buscada, pal_buscada);
	strcat(pal_buscada, ".");
	/* Engadimos un punto xa que na lista estan todas con punto.                  */
	/*
	   if (idioma==GALEGO)
	   pos_lista=busca_en_lista(abrev_con_ext_simple,pal_buscada,pal_extendida);
	   else
	   pos_lista=busca_en_lista(cast_abrev_con_ext_simple,pal_buscada,pal_extendida);

	 */
	//ZA.

	pal_extendida = (char *) diccionarios->existeEnAbreviaturas(pal_buscada, idioma);
	if (pal_extendida != NULL) {
		if (pal_anterior) {
			// FRAN_CAMPILLO: Aquí controlamos si es la primera palabra de cada frase.
			if (idioma == GALEGO) {	// Tratamo-lo caso do galego
				if (!
					(strcmp(pal_anterior, "1") == 0
					 || strcmp(pal_anterior, "un") == 0 || strcmp(pal_anterior, "unha") == 0))
					//strcat(pal_extendida,"s"); // Se non e singular engadimos -s
					if ((auxi = diccionarios->devuelveExtensionPlural(idioma)) != NULL)
						pal_extendida = auxi;
			}					// END IF galego
			else				// Tratamo-lo caso do castelan
			if (!
					(strcmp(pal_anterior, "1") == 0
						 || strcmp(pal_anterior, "un") == 0
						 || strcmp(pal_anterior, "una") == 0 || strcmp(pal_anterior, "uno") == 0))
				//strcat(pal_extendida,"s"); // Se non e singular engadimos -s
				if ((auxi = diccionarios->devuelveExtensionPlural(idioma)) != NULL)
					pal_extendida = auxi;
		}
		strcpy(saida, pal_extendida);
		return 1;
	}
	return 0;
}

/** 
 * \author 
 * \remark 
 * 
 * \param signo_especial_procesado 
 * \param caracter 
 * \param palabra_anterior 
 */
void Preproceso::
transformacion_de_caracter_especial(char *signo_especial_procesado, char *caracter, char *palabra_anterior)
{

	switch ((unsigned char) *caracter) {	/* BEGIN SWITCH                           */
	case '%':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "por cen") :
			strcpy(signo_especial_procesado, "por cien");
		break;
	case '{':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado,
									"apertura de chave") :
			strcpy(signo_especial_procesado, "abertura de llave");
		break;
	case '}':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado,
									"peche de chave") : strcpy(signo_especial_procesado, "cierre de llave");
		break;
	case '+':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "máis") :
			strcpy(signo_especial_procesado, "más");
		break;
	case '=':
		strcpy(signo_especial_procesado, "igual a");
		break;
	case '>':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "maior que") :
			strcpy(signo_especial_procesado, "mayor que");
		break;
	case '<':
		strcpy(signo_especial_procesado, "menor que");
		break;
	case '#':
		strcpy(signo_especial_procesado, "número");
		break;
	case '&':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "e") : strcpy(signo_especial_procesado, "y");
		break;
	case '*':
		if (palabra_anterior) {
			if (dixito(palabra_anterior[strlen(palabra_anterior) - 1]))
				strcpy(signo_especial_procesado, "por");
			else
				strcpy(signo_especial_procesado, "asterisco");
		}
		else
			strcpy(signo_especial_procesado, "asterisco");
		break;
	case '@':
		strcpy(signo_especial_procesado, "arroba");
		break;
	case '\\':
		strcpy(signo_especial_procesado, "barra invertida");
		break;
	case '^':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado,
									"acento circunflexo") :
			strcpy(signo_especial_procesado, "acento circunflejo");
		break;
	case '_':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "subliñado") :
			strcpy(signo_especial_procesado, "subrayado");
		break;
	case '|':
		strcpy(signo_especial_procesado, "barra vertical");
		break;
	case '~':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "til") :
			strcpy(signo_especial_procesado, "tilde");
		break;
	case '©':
		strcpy(signo_especial_procesado, "copirait");
		break;
	case '°':
		if (palabra_anterior) {
			if (strcmp(palabra_anterior, "1") == 0 || strcmp(palabra_anterior, "un") == 0)
				(idioma == GALEGO) ? strcpy(signo_especial_procesado,
											"grao") : strcpy(signo_especial_procesado, "grado");
			else
				(idioma == GALEGO) ? strcpy(signo_especial_procesado,
											"graos") : strcpy(signo_especial_procesado, "grados");
		}
		else
			(idioma == GALEGO) ? strcpy(signo_especial_procesado, "graos") :
				strcpy(signo_especial_procesado, "grados");
		break;
	case '±':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "maismenos") :
			strcpy(signo_especial_procesado, "masmenos");
		break;
	case '²':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "ó cadrado") :
			strcpy(signo_especial_procesado, "al cuadrado");
		break;
	case '³':
		(idioma == GALEGO) ? strcpy(signo_especial_procesado, "ó cubo") :
			strcpy(signo_especial_procesado, "al cubo");
		break;
	case '¼':
		strcpy(signo_especial_procesado, "un cuarto");
		break;
	case '½':
		strcpy(signo_especial_procesado, "un medio");
		break;
	case '$':
		if (palabra_anterior) {
			if (strcmp(palabra_anterior, "1") == 0 || strcmp(palabra_anterior, "un") == 0)
				strcpy(signo_especial_procesado, "dólar");
			else
				strcpy(signo_especial_procesado, "dólares");
		}
		else
			strcpy(signo_especial_procesado, "dólar");
		break;
	case '£':
		if (palabra_anterior) {
			if (strcmp(palabra_anterior, "1") == 0
				|| strcmp(palabra_anterior, "unha") == 0 || strcmp(palabra_anterior, "una") == 0)
				strcpy(signo_especial_procesado, "libra");
			else
				strcpy(signo_especial_procesado, "libras");
		}
		else
			strcpy(signo_especial_procesado, "libra");
		break;
	case (unsigned char) 'µ':
		strcpy(signo_especial_procesado, "micro");
		break;
	case '¥':
		if (palabra_anterior) {
			if (strcmp(palabra_anterior, "1") == 0 || strcmp(palabra_anterior, "un") == 0)
				strcpy(signo_especial_procesado, "ien");
			else
				strcpy(signo_especial_procesado, "iens");
		}
		else
			strcpy(signo_especial_procesado, "ien");
		break;
	}							/* END SWITCH                             */
	return;
}

/**  \fn void Preproceso::transformacion_de_palabra_con_guion_no_medio(t_palabra_proc pal_saida,
 *                                                                      t_palabra pal_entrada)
 *
 * En palabras con guion no medio distinguimos
 * dous casos: (1) que se trate de palabras compostas,caso nos que as separamos
 * en duas palabras independientes ou (2) que se trate de segundas formas do artigo,
 * caso no que suprimimos o guion e unimos o -lo -la ... directamente a palabra.
 * Se se quixera facer unha analise sintactica posterior o mellor o que deberiamos
 * facer era desfacer a segunda forma do artigo no verbo mais o artigo separado.
 * Na ultima revision do programa decidin que o guion no medio debe deixarse no
 * caso da segunda forma do artigo por dous motivos: Axuda xa a silabificacion pois
 * coincide ca separacion de silabas, e permite na analise sintactica detectar que se trata
 * dun verbo e que despois ben un nome.
 */
void Preproceso::transformacion_de_palabra_con_guion_no_medio(t_palabra_proc pal_saida, t_palabra pal_entrada)
{

	int kk = strlen(pal_entrada);

	if ((kk > 3 && (terminacion(pal_entrada, "-lo")
					|| terminacion(pal_entrada, "-la"))) || (kk > 4 && (terminacion(pal_entrada, "-los")
																		|| terminacion(pal_entrada, "-las"))))
		pasar_a_minusculas(pal_entrada, pal_saida);
	else {
		char *pos_guion;
		t_palabra pal;
		strcpy(pal, pal_entrada);
		while (NULL != (pos_guion = strchr(pal, '-')))
			*pos_guion = ' ';
		adaptacion_por_defecto(pal_saida, pal);
		//pasar_a_minusculas(pal,pal_saida);
	}
}

/** \fn void Preproceso::adaptacion_por_defecto(char *abreviatura_pronunciada, char *pal_abreviada)
 *
 * En adaptacion por defecto eliminase todo o que non
 * sexan letras e despois estas pasanse a minusculas.
 */
void Preproceso::adaptacion_por_defecto(char *abreviatura_pronunciada, char *pal_abreviada)
{
	t_palabra_proc pal_aux, pal_aux2, pal;
	char *punt_aux;

	pasar_a_minusculas(pal_abreviada, pal);
	*pal_aux = 0;
	*pal_aux2 = 0;
	punt_aux = pal;
	while (*punt_aux) {
		/* Neste bucle eliminamos todo o distinto de letras e dixitos. Os dixitos pro-
		   -nunciamolos e as letras se non son todas consonantes deixanse e senon de-
		   -letreanse.                                                                */
		*pal_aux = 0;
		*pal_aux2 = 0;
		if (letra((unsigned char) *punt_aux)) {
			while (letra((unsigned char) *punt_aux)) {
				strncat(pal_aux, punt_aux, 1);
				punt_aux++;
			}
			//  pasar_a_minusculas(pal_aux,pal_aux2);
			if (palabra_formada_por_solo_consonantes(pal_aux))
				deletrear(pal_aux, pal_aux2, idioma);
			else
				strcpy(pal_aux2, pal_aux);
			//        if (*abreviatura_pronunciada!=0 &&
			//              abreviatura_pronunciada[strlen(abreviatura_pronunciada)-1]!=' ')
			//            strcat(abreviatura_pronunciada," ");
			strcat(abreviatura_pronunciada, pal_aux2);
		}
		if (dixito(*punt_aux)) {
			while (dixito(*punt_aux)) {
				strncat(pal_aux, punt_aux, 1);
				punt_aux++;
			}
			numeros(pal_aux, MASCULINO, pal_aux2);
			if (*abreviatura_pronunciada != 0 &&
				abreviatura_pronunciada[strlen(abreviatura_pronunciada) - 1] != ' ')
				strcat(abreviatura_pronunciada, " ");
			strcat(abreviatura_pronunciada, pal_aux2);
		}
		if (*punt_aux == ' ')
			strncat(abreviatura_pronunciada, punt_aux, 1);
		while (*punt_aux != 0 && !letra((unsigned char) *punt_aux)
			   && !dixito(*punt_aux))
			punt_aux++;
		/* Eliminamos todo o distinto de cifras e letras                              */
	}							/* END DO WHILE (*punt_aux)               */
}

/** \fn void Preproceso::elemento_a_preprocesar(t_frase_separada * elemento_de_frase, int *orden, int no_primera)
 *
 * Nesta funcion aquelas palabras que se deben transformar en algo "en letra "
 * e decir leible, transformanse a unha zona de memoria comun e cada palabra xa
 * preprocesada enlazase cun punteiro a aquela palabra da que proven para tela xa
 * asi accesible nun dos campos das estructuras das que se compon a variable global
 * frase separada.
 * Imos comprobando segun a clase e chamamos a funcion correspondente que o
 * transforma que se chamara " transformacion de ...cardinal, ...abreviatura.
 * Esta funcion chama as outras que son as auxiliares.
 */
void Preproceso::elemento_a_preprocesar(Token * elemento_de_frase, int *orden, int no_primera)
{

	token_a_preprocesar = elemento_de_frase;

	palabras_procesadas[*orden][0] = 0;
	switch (elemento_de_frase->clase_pal) {
	case PALABRA_NORMAL:
			pasar_a_minusculas(elemento_de_frase->token, elemento_de_frase->token);
		return;
	case PALABRA_CONSONANTES_MINUSCULAS:
		if (tipo) {
				pasar_a_minusculas(elemento_de_frase->token, (char *) &palabras_procesadas[*orden]);
			break;
		}
		if (no_primera) {
				if (unidades_de_medida_sin_punto(elemento_de_frase->token, (char *)
							&palabras_procesadas[*orden], (elemento_de_frase - 1)->token))
				break;
		}
			else if (unidades_de_medida_sin_punto(elemento_de_frase->token, (char *)
											  &palabras_procesadas[*orden], NULL))
			break;

			deletrear(elemento_de_frase->token, (char *) &palabras_procesadas[*orden], idioma);
		break;
	case PALABRA_NORMAL_EMPEZA_MAY:
	case PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA:
		if (no_primera) {
				if (unidades_de_medida_sin_punto(elemento_de_frase->token, (char *)
							&palabras_procesadas[*orden], (elemento_de_frase - 1)->token))
				break;
		}
			else if (unidades_de_medida_sin_punto(elemento_de_frase->token, (char *)
											  &palabras_procesadas[*orden], NULL))
			break;
		transformacion_de_pal_empeza_maiusculas((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		/* transformacion_de_pal_empeza_maiusculas(elemento_de_frase->pal,&palabras_procesadas[*orden]); */
		break;
	case NUM_ROMANO:
		if (tipo) {
				strcpy((char *) &palabras_procesadas[*orden], elemento_de_frase->token);
			break;
		}
		if (no_primera)
			transformacion_de_romanos((char *) &palabras_procesadas[*orden],
						elemento_de_frase->token,
						(elemento_de_frase - 1)->token, (elemento_de_frase - 1)->clase_pal);
		else
				transformacion_de_romanos((char *) &palabras_procesadas[*orden], elemento_de_frase->token, NULL, 0);
		break;
	case CARDINAL:
		transformacion_de_cardinal((char *) &palabras_procesadas[*orden],
					elemento_de_frase->token,
					(elemento_de_frase + 1)->token,
								   (elemento_de_frase + 1)->clase_pal, elemento_de_frase);
		break;
	case ORDINAL_MASCULINO:
			transformacion_de_ordinal((char *) &palabras_procesadas[*orden], elemento_de_frase->token, MASCULINO);
		break;
	case ORDINAL_FEMENINO:
			transformacion_de_ordinal((char *) &palabras_procesadas[*orden], elemento_de_frase->token, FEMININO);
		break;
	case VALOR_DE_GRADOS:
		transformacion_de_valor_de_grados((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token, 0);
		break;
	case VALOR_DE_GRADOS_DECIMAL:
		transformacion_de_valor_de_grados((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token, 1);
		break;
	case NUMERO_CON_PUNTO:
		transformacion_de_numero_con_punto((char *)
										   &palabras_procesadas[*orden],
					elemento_de_frase->token,
					(elemento_de_frase + 1)->token, (elemento_de_frase + 1)->clase_pal);
		break;
	case DATA:
			transformacion_de_datas((char *) &palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case DECIMAL:
		transformacion_decimal((char *) &palabras_procesadas[*orden],
					elemento_de_frase->token,
					(elemento_de_frase + 1)->token, (elemento_de_frase + 1)->clase_pal);
		break;
	case PORCENTUAL:
		transformacion_de_tanto_por_cento((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case HORAS:
		//adaptacion_por_defecto((char *)&palabras_procesadas[*orden],elemento_de_frase->pal);
			transformacion_horas((char *) &palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case PARTITIVO:
		transformacion_de_partitivo((char *) &palabras_procesadas[*orden],
					(elemento_de_frase + 1)->token,
					(elemento_de_frase + 1)->clase_pal, elemento_de_frase->token);
		break;
	case CADENAS_DE_OPERACIONS:
		return;
	case ABREVIATURA:
		if (no_primera)
			transformacion_de_abreviaturas((char *)
										   &palabras_procesadas[*orden],
						elemento_de_frase->token,
						(elemento_de_frase - 1)->token, (elemento_de_frase - 1)->clase_pal);
		else
			transformacion_de_abreviaturas((char *)
						&palabras_procesadas[*orden], elemento_de_frase->token, NULL, 0);
		break;
	case ESPACIO_BLANCO:
		return;
	case PALABRA_CON_GUION_NO_MEDIO:
		transformacion_de_palabra_con_guion_no_medio((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case PALABRA_CON_PUNTO_NO_MEDIO:
		transformacion_de_palabra_con_punto_no_medio((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case PALABRA_TODO_MAIUSCULAS:

		transformacion_de_palabra_en_maiusculas((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case CARACTER_ESPECIAL:
		if (no_primera)
			transformacion_de_caracter_especial((char *)
												&palabras_procesadas[*orden],
						elemento_de_frase->token, (elemento_de_frase - 1)->token);
		else
			transformacion_de_caracter_especial((char *)
						&palabras_procesadas[*orden], elemento_de_frase->token, NULL);
		break;
	case INICIAL:
			transformacion_de_inicial((char *) &palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	case CADENA_DE_SIGNOS:
		return;
	case SIGNO_PUNTUACION:
		/* As cadeas de cousas como signos moi largos son recortadas o primeiro. Este
		   caso podese dar cando a xente introduce dous guions seguidos en vez dun só
		   para dar mais enfase a alogo, ou unha cadea rara de signos sen sentido.    */
        if ( (strcmp(elemento_de_frase->token, SIMBOLO_RUPTURA_ENTONATIVA)) &&
        	 (strcmp(elemento_de_frase->token, SIMBOLO_RUPTURA_COMA)) )
			elemento_de_frase->token[1] = 0;
		return;
	case NUMEROS_SEPARADOS_POR_GUIONS:
		transformacion_numeros_con_guion_no_medio((char *)
					&palabras_procesadas[*orden], elemento_de_frase->token);
		break;
	default:
			adaptacion_por_defecto((char *) &palabras_procesadas[*orden], elemento_de_frase->token);
	}							/* END SWITCH                             */
	/* En caso de que se extenda unha palabra gardamoslle sitio para a transfor-
	   -mación e tamén para cando despois se acentue e silabifique.               */
	//if (*palabras_procesadas[*orden]){
	elemento_de_frase->pal_transformada = palabras_procesadas[*orden];
	*orden = *orden + 1;
	//}
}

int Preproceso::deletrea_numero(char *entrada, char *salida)
{
	char *aux = entrada;

	if (idioma == GALEGO) {
		while (*aux) {
			switch (*aux) {
			case '0':
				strcat(salida, "cero ");
				break;
			case '1':
				strcat(salida, "un ");
				break;
			case '2':
				strcat(salida, "dous ");
				break;
			case '3':
				strcat(salida, "tres ");
				break;
			case '4':
				strcat(salida, "catro ");
				break;
			case '5':
				strcat(salida, "cinco ");
				break;
			case '6':
				strcat(salida, "seis ");
				break;
			case '7':
				strcat(salida, "sete ");
				break;
			case '8':
				strcat(salida, "oito ");
				break;
			case '9':
				strcat(salida, "nove ");
				break;
			default:
				break;
			}
			aux++;
		}
	}
	else if (idioma == CASTELLANO) {
		while (*aux) {
			switch (*aux) {
			case '0':
				strcat(salida, "cero ");
				break;
			case '1':
				strcat(salida, "uno ");
				break;
			case '2':
				strcat(salida, "dos ");
				break;
			case '3':
				strcat(salida, "tres ");
				break;
			case '4':
				strcat(salida, "cuatro ");
				break;
			case '5':
				strcat(salida, "cinco ");
				break;
			case '6':
				strcat(salida, "seis ");
				break;
			case '7':
				strcat(salida, "siete ");
				break;
			case '8':
				strcat(salida, "ocho ");
				break;
			case '9':
				strcat(salida, "nueve ");
				break;
			default:
				break;
			}
			aux++;
		}
	}
	salida[strlen(salida) - 1] = 0;
	return 0;
}

void Preproceso::transformacion_de_palabra_con_punto_no_medio(t_palabra_proc pal_saida, t_palabra pal_entrada)
{
	t_palabra pal, aux2;
	t_palabra_proc aux;
	char *p, *p2;

	p = p2 = pal_entrada;
	while (NULL != (p=strpbrk(p2,"./-"))){
		strncpy(aux2, p2, p - p2);
		aux2[p - p2] = 0;
		if (palabra_formada_por_solo_consonantes(aux2)){
			deletrear(aux2, aux, idioma);
		}
		else {
			pasar_a_minusculas(aux2, aux);
		}
		strcat(pal_saida, aux);
		p2 = p + 1;
		switch (*p) {
			case '.':
				strcat(pal_saida, " punto ");
				break;
			case '/':
				strcat(pal_saida, " barra ");
				break;
			case '-':
				strcat(pal_saida, " guion ");
				break;
		}
	}
	if (palabra_formada_por_solo_consonantes(p2)){
		deletrear(p2, aux, idioma);
	}
	else {
		pasar_a_minusculas(p2, aux);
	}
	strcat(pal_saida, aux);

	/*
	 *  strcpy(pal, pal_entrada);
	 *  p = strtok(pal, "./-");
	 *  if (palabra_formada_por_solo_consonantes(p))
	 *    deletrear(p, aux, idioma);
	 *  else
	 *    pasar_a_minusculas(p, aux);
	 *  strcat(pal_saida, aux);
	 *
	 *  while (NULL != (p = strtok(NULL, "./-"))) {
	 *    strcat(pal_saida, " punto ");
	 *    if (palabra_formada_por_solo_consonantes(p))
	 *      deletrear(p, aux, idioma);
	 *    else
	 *      pasar_a_minusculas(p, aux);
	 *    strcat(pal_saida, aux);
	 *  }
	 */

}

/**  \fn int busca_en_lista(const t_tabla_traduc * ent, const char *palabra, char *sal)
 *
 * Funcion para buscar nas listas de maneira
 * secuencial. Cando se queiran engadir elementos as listas non hai mais que
 * escribilo na lista, e que esta estea terminada en '\0'. Devolve a palabra
 * traducida da lista.
 */
int busca_en_lista(const t_tabla_traduc * ent, const char *palabra, char *sal)
{

	for (int i = 0; *((ent + i)->palabra);) {
		if (strcmp((ent + i)->palabra, palabra) == 0) {
			strcpy(sal, (ent + i)->traduc);
			return i;
		}
		i++;
	}
	return -1;
}



/** 
 * \author fmendez
 * \remarks Vuelca la frase preprocesada en el fichero de salida
 * 
 * \param nombre_fichero Fichero de salida
 * 
 * \return 0 en ausencia de error
 */
int Preproceso::escribe_fichero(char *nombre_fichero)
{
	char aux[FILENAME_MAX];

	if (fd == NULL) {
		sprintf(aux, "%s.pre", nombre_fichero);
		if ((fd = fopen(aux, "w")) == NULL) {
			fprintf(stderr, "\nO arquivo de preprocesado \"%s\" non se pode crear", aux);
			return -1;
		}
	}
	fprintf(fd, "%s\n", frase_procesada);
	return 0;
}


/** 
 * \author fmendez
 * \remarks Transformamos as palabras que o necesiten, e decir todas menos as que estan 
 * enteiramente en minusculas e unha vez transformadas concatenanse no string 
 * fp. Este string e o que sera o punto de partida para separar en silabas e acentuar
 * 
 * \param[in] fs Frase a preprocesar (entrada)
 * \param[out] fp Frase preprocesada (salida)
 * 
 * \return 0 en ausencia de error
 */
int Preproceso::preprocesa(Token * ft, char * fp, t_frase_separada *fs, int * n)
{

	int cont = 0, orden = 0;

	char pp[LONX_MAX_PALABRA_PROCESADA];

	frase_procesada = fp;
	frase_tokenizada = ft;
	frase_separada = fs;
	//palabras_procesadas = pp;

	while (*ft->token) {
		elemento_a_preprocesar(ft, &orden, cont);
		ft->n_palabras = 1;
		if (ft->pal_transformada != NULL) {
			char *a = ft->pal_transformada;
			strcat(frase_procesada, ft->pal_transformada);

			while (*a) {
				if (!letra((unsigned char) *a) && *a != '-') {
					*a = ' ';
					ft->n_palabras++;
				}
				if (*a == '-' && strcmp(a, "-lo") && strcmp(a, "-los") &&
					strcmp(a, "-la") && strcmp(a, "-las")) {
					*a = ' ';
					ft->n_palabras++;
				}
				a++;
			}
		}
		else {
			strcat(frase_procesada, ft->token);
		}
		ft++;
		cont++;
		if (*ft->token && frase_procesada[strlen(frase_procesada) - 1] != ' ') {
			strcat(frase_procesada, " ");
		}
	}

/* 	if (separa_token){
 * 		string buf;
 * 		stringstream ss;
 * 		ft = frase_tokenizada;
 * 		*n = 0;
 * 		while (*ft->token) {
 * 			ft->palabras = fs;
 * 			if (ft->pal_transformada) {
 * 				ss << ft->pal_transformada;
 * 				while (ss >> buf){
 * 					strcpy(fs->pal, buf.c_str());
 * 					fs->pal_transformada = fs->pal;
 * 					fs->clase_pal = PALABRA_NORMAL;
 * 					(*n)++;
 * 					fs++;
 * 				}
 * 				ss.clear();
 * 			}
 * 			else {
 * 				strcpy(fs->pal, ft->token);
 * 				fs->clase_pal = ft->clase_pal;
 * 				(*n)++;
 * 				fs++;
 * 			}
 * 			ft++;
 * 		}
 * 	}
 * 	else {
 * 		*n = cont;
 * 	}
 */

	if (separa_token){
		ft = frase_tokenizada;
		*n = 0;
		while (*ft->token) {
			ft->palabras = fs;
			ft->n_palabras = 0;
			if (ft->pal_transformada) {
				strcpy(pp, ft->pal_transformada);
				char* kk;
				kk = strtok(pp, " ");
				while (kk != NULL) {
					strcpy(fs->pal, kk);
					fs->pal_transformada = fs->pal;
					fs->clase_pal = PALABRA_NORMAL;
					//fs->clase_pal = ft->clase_pal;
					(*n)++;
					fs++;
					ft->n_palabras++;
					kk = strtok(NULL, " ");
				}
			}
			else {
				strcpy(fs->pal, ft->token);
				fs->clase_pal = ft->clase_pal;
				(*n)++;
				ft->n_palabras++;
				fs++;
			}
			if (ft->n_palabras == 1) {
				ft->palabras->clase_pal =  ft->clase_pal;
			}
			ft++;
		}
	}
	else {
		ft = frase_tokenizada;
		while (*ft->token) {
			ft->palabras = fs;
			ft->n_palabras = 1;
			if (ft->pal_transformada) {
				fs->pal_transformada = ft->pal_transformada;
			}
			strcpy(fs->pal, ft->token);
			fs->clase_pal = ft->clase_pal;
			fs++;
			ft++;
		}
		*n = cont;
	}

	if (pre) {
		if (escribe_fichero(fsalida)) {
			return -1;
		}
	}

	return 0;
}

/** 
 * \author fmendez
 * \remarks Abre el fichero de salida y establece las opciones de preprocesado.
 * 
 * \param opciones 
 */
void Preproceso::inicializa(t_opciones * opciones)
{

	opciones->fstdout ? fd = stdout : fd = NULL;

	pre = opciones->pre;
	fsalida = opciones->fsalida;
	tipo = opciones->noprepro;
	idioma = opciones->idioma;
	separa_token = opciones->separa_token;

}

/** 
 * \author fmendez 
 * \remarks Cierra el fichero de salida de preprocesado
 */
void Preproceso::finaliza()
{
	if (fd != NULL && fd != stdout) {
		fclose(fd);
		fd = NULL;
	}
}

/** 
 * \author fmendez 
 * \remarks Constructor de la clase Preproceso. Asigna memoria para extender las palabras.
 */
Preproceso::Preproceso()
{

	if ((palabras_procesadas =
		 (t_palabra_proc *) malloc(LONX_MAX_FRASE_SEPARADA * sizeof(t_palabra_proc))) == NULL) {
		fprintf(stderr, "Preproceso: Error al asignar memoria en palabras_procesadas\n");
	}
}

/** 
 * \author fmendez
 * \remarks Destructor de la clase Preproceso. Libera memoria.
 */
Preproceso::~Preproceso()
{

	free(palabras_procesadas);
}

