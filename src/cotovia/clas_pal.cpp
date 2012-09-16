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
#include <sys/types.h> 
#include <regex.h> 
#include "modos.hpp"
#include "tip_var.hpp"
#include "letras.hpp"
#include "clas_pal.hpp"
/*============================================================================
  =  Unha vez separadas as posibles palabras clasificanse segun a forma que  =
  = teñan os seus caracteres: letras (maiusculas-minusculas..) números ,sig- =
  = -nos, etc. Cada palabra esta ocupa unha estructura que conten outros cam-=
  = -pos como por exemplo a clase de palabra que e segun os seguintes codigos=
  = definidos como macros.                                                   =
  ============================================================================*/

/**
 * \author 
 * \remark  Para ser palabra estandar solo debe ter letras e a primeira pode
 * ser ou non maiuscula. Dependendo de esto clasificaremolas en nome propios ou
 * ou ben palabra normal de todo. O dos nomes propios pode servir no
 * preprocesado para poñer os Orden dos reis ou Papas ,nos que o numero romano
 * lese como ordinal e nos seculos como cardinal. Para ser mais preciso haberia
 * que comprobar se e a primeira palabra da frase e por eso se escribe con
 * maiuscula. Outra clase de palabra e a que ten algun guion polo medio que
 * poden ser palabras compostas ou a segunda forma do artigo.
 *
 * \param palabra  
 *
 * \return Devolve 0 se non é estándar 
 */
unsigned char palabra_estandar( char *palabra){

	unsigned char valor=0,num_guions=0,num_puntos=0;
	char *pal=palabra;

	if (!letra((unsigned char)*pal)) return 0;
	if (maiuscula((unsigned char)*pal)) valor=PALABRA_NORMAL_EMPEZA_MAY;
	if (minuscula((unsigned char)*pal)) valor=PALABRA_NORMAL;
	if (valor) {
		while (*pal && (*pal=='.' || *pal=='-' || letra((unsigned char)*pal) ) ) {
			if (*pal=='-') num_guions++;
			if (*pal=='.') num_puntos++;
			pal++;
		}
	}
	if (*pal==FIN_DE_PALABRA) {
		if (num_puntos) return  PALABRA_CON_PUNTO_NO_MEDIO;
		if (num_guions)//==1)
			return PALABRA_CON_GUION_NO_MEDIO;
		else {
			if (palabra_formada_por_solo_consonantes(palabra))
				return PALABRA_CONSONANTES_MINUSCULAS;
			else
				return valor;
		}
	}
	//return valor;
	return 0;
}

/*****************************************************************************
 *  short int forma_de_abreviatura(t_palabra pal): Para poder ser abreviatura *
 * SOLO pode conter ou ben / ou letras ou puntos ou ben ª ou º . Esta funcion*
 * detecta se a palabra de entrada e ou non. Se se chega a fin de palabra con*
 * estos caracteres solamente enton tratase dunha abreviatura. Para ser con- *
 * -siderada abreviatura (pola forma) debe ter letras e ou algun punto ou al-*
 * -gunha barra. Exemplos:                                                   *
 *                      s/n; Dª., c/(calle), sr.                             *
 *  Devolve o codigo 15 se e unha abreviatura, en caso contrario 0.          *
 *****************************************************************************/
inline short int forma_de_abreviatura(char * pal){
	char  num_barras=0,num_puntos=0,num_letras=0,num_ordinales=0;
	//char pf = (pal[strlen(pal)-1] == '.'); 
	for(;*pal;) {
		if (*pal=='/') num_barras++;
		else if (*pal=='.') num_puntos++;
		else if ((unsigned char) *pal==simbolo_ordinal_masculino || (unsigned char) *pal==simbolo_ordinal_femenino)
			num_ordinales++;
		else if (letra((unsigned char)*pal)) num_letras++;
		else if (dixito((unsigned char)*pal)) num_letras++;
		else break;
		pal++;
	}
	if (num_letras>=1 && *pal==FIN_DE_PALABRA && (num_barras>0 || ( num_puntos>0 && *(pal-1) == '.') || num_ordinales>0) ){
	//if (num_letras>=1 && *pal==FIN_DE_PALABRA && (num_barras>0 || ( num_puntos>0 && pf ) || num_ordinales>0) ){
 		return ABREVIATURA;
	}
	else {
		return 0;
	}

	/*
	   while ((letra((unsigned char)*pal) || *pal=='/' || (unsigned char) *pal==simbolo_ordinal_masculino ||
	   (unsigned char) *pal==simbolo_ordinal_femenino || *pal=='.') && *(pal)!=FIN_DE_PALABRA) {
	   if (letra((unsigned char)*pal)) num_letras++;
	   if (*pal=='/') num_barras++;
	   if (*pal=='.') num_puntos++;
	   if ((unsigned char) *pal==simbolo_ordinal_masculino || (unsigned char) *pal==simbolo_ordinal_femenino)
	   num_ordinales++;
	   pal++;
	   }
	   if ((num_barras>0 || num_puntos>0 || num_ordinales>0) &&   num_letras>=1
	   && *pal==FIN_DE_PALABRA)   return ABREVIATURA;
	   else
	   return 0;
	   */
}

/*****************************************************************************
 *  short int forma_de_data(unsigned char* n): Comproba se o que se lle pasa *
 * ten forma de data. Para ter forma de data ten que ter a seguinte forma:   *
 *                      12-11-96 ou 12/11/90.                                *
 *  Devolve 1 se se trata dunha data e 0 en caso contrario.                  *
 *****************************************************************************/
short int forma_de_data( char* n)
{
	regex_t date;
	regcomp(&date, "^(0*[1-9]|[12][0-9]|3[01])[-/](0*[1-9]|1[012])[-/]([[:digit:]]{1,3}\\.[[:digit:]]{3}|[[:digit:]]+)$", REG_EXTENDED|REG_NOSUB);
	if (!regexec(&date, n, 0, NULL, 0)){
		return 1;
	}
	else {
	 return 0;
	}	 
	
	//short int numero;
	///* Se e unha data tera que ser unha secuencia de numero /- numero /- numero   */
	
	//numero=0;
	//[>   if (( dixito(*n) && dixito(*(n+1))) && ( *(n+2)=='/' || *(n+2)=='-') &&
		 //(dixito(*(n+3)) && dixito(*(n+4))) && (*(n+5)=='/' || *(n+5)=='-') &&
		 //(( dixito(*(n+6)) && dixito(*(n+7))) || (dixito(*(n+8)) && dixito(*(n+9)))))*/
	//[>fmen	if (dixito(*n) && (dixito(*(n+1)) || *(n+1)=='-' || *(n+1)=='/') &&
		//(dixito(*(n+2)) || *(n+2)=='-' || *(n+2)=='/') &&
		//(dixito(*(n+3)) || *(n+3)=='-' || *(n+3)=='/') &&
		//(dixito(*(n+4)) || *(n+4)=='-' || *(n+4)=='/') &&
		//(dixito(*(n+5)) || *(n+5)=='-' || *(n+5)=='/'|| *(n+5)=='.') &&
		//(dixito(*(n+6)) || *(n+6)=='-' || *(n+6)=='/'|| *(n+6)=='.')){     */
	//if (!dixito(*n)) return 0;
	//else if (((*(n+1)=='-' || *(n+1)=='/') &&
				//dixito(*(n+2)) && (*(n+3)=='-' || *(n+3)=='/') &&
				//dixito(*(n+4)) && dixito(*(n+5)))
			//||
			//( ((dixito(*(n+1)) && (*(n+2)=='-' || *(n+2)=='/')) ||
				 //((*(n+1)=='-' || *(n+1)=='/') &&  dixito(*(n+2)))) && dixito(*(n+3))
				//&& (*(n+4)=='-' || *(n+4)=='/') && dixito(*(n+5)) && dixito(*(n+6)))
			//||
			//(dixito(*(n+1)) &&(*(n+2)=='-' || *(n+2)=='/') &&
			 //dixito(*(n+3)) && dixito(*(n+4)) && (*(n+5)=='-' || *(n+5)=='/') &&
			 //dixito(*(n+6)) && dixito(*(n+7))))
	//{
		//if (numero==0) {           [> Extraemo-lo primeiro numero            <]
			//while ( *n!='-' && *n!='/' && dixito(*n)) n++;
			//++n;                    [> Pasamos do separador de fecha          <]
			//numero++;
		//}
		//while ( *n!='-' && *n!='/' && dixito(*n))  n++;
		//n++;                       [> Pasamos do separador de fecha          <]
		//numero++;                  [> Queda extraido o segundo numero        <]
		//if (numero==2 && strlen(n)&& dixito(*n)){
			//while(dixito(*n++));
			//return (*n == 0);
		//}              [> Se ainda quedan numeros sera o ano     <]
		//else return 0;             [> Senon non segue o formato dunha data   <]
	//}
	//else
		//return 0;
}

/*****************************************************************************
 *  int numeros_separados_por_guions(unsigned char *c): Comproba se se da tal*
 * secuencia, este e o caso de resultados (2-1), etapas de anos (1987-1996)  *
 * ou "esquemas" deportivos, p. ex., defensa 2-2-1 ou 4-4-2.                 *
 *   Devolve un 1 se se segue tal formato e 0 en caso contrario.             *
 *****************************************************************************/
int numeros_separados_por_guions( char *c){
	int num=0,guion=0;//,comas=0;

	while (*c) {
		if (*c=='-' || *c=='/') 
			guion++;
		else if (dixito(*c)) 
			num++;
		else if (*c==',' || *c=='.') ;//comas++;
		else 
			return 0;
		c++;
	}
	if (num && guion) 
		return 1;
	else 
		return 0;

}

/*****************************************************************************
 *                ¡¡¡¡ OLLO CREO QUE NON SE USA !!!                          *
 *  short int forma_de_partitivo(unsigned char* n): Esta funcion detecta se o*
 * que se lle pasa ten ou non forma de partitivo, representando fraccions,   *
 * divisions,ect., asi p. ex. ten tal forma: 78/6.                           *
 *  Devolve o codigo de partitivo en caso afirmativo e 0 en caso contrario.  *
 *****************************************************************************/
short int forma_de_partitivo( char* n)
{
	unsigned char num_barras=0;
	int lonxitude;

	lonxitude=strlen(n);
	while ((dixito(*n) || *n=='/') && & *(n)!=FIN_DE_PALABRA) {
		/* Caracteres que aparecen nun decimal ou cadena "punto".                     */
		if (*n=='/') num_barras++;
		n++;
	}
	if (num_barras==1 && *n==FIN_DE_PALABRA && dixito(*(n-1)) &&
			dixito(*(n-lonxitude)))    return PARTITIVO;
	else
		return 0;
}

/*****************************************************************************
 *  int forma_de_horas(unsigned char* n): Comproba se o que se lle pasa ten  *
 * formato de hora, por exemplo: 22:30.                                      *
 *  Devolve 1 se segue tal formato e 0 en caso contrario.                    *
 *****************************************************************************/
/*
 *int forma_de_horas( char* n){
 *  //return (( dixito(*n) && dixito(*(n+1))) && (*(n+2)==':') && (dixito(*(n+3)) && dixito(*(n+4))));
 *  return ( dixito(*n) && (( dixito(*(n+1)) && *(n+2)==':' && dixito(*(n+3)) && dixito(*(n+4)) && !dixito(*(n+5))) || ( *(n+1)==':' && dixito(*(n+2)) && dixito(*(n+3)) && !dixito(*(n+4)))) );
 *}
 */

/*****************************************************************************
 *  short int despois_do_ultimo_punto_hai_tres_cifras(unsigned char *c): Com-*
 * -proba se o que se lle pasa e un punto con numeros.                       *
 *  Devolve 1 se segue tal formato e 0 en caso contrario.                    *
 *****************************************************************************/
int despois_do_ultimo_punto_hai_tres_cifras( char *c){
	/* O punteiro apunta o \0 da cadea e os anteriores son os ultimos caracteres. */
	return (dixito(*(c-1)) && dixito(*(c-2)) && dixito(*(c-3)) && *(c-4)=='.');
}

/*****************************************************************************
 *  unsigned char cadea_numerica(unsigned char *inicial): Esta funcion detec-*
 * -ta se a palabra que se lle pasa como parametro e unha cadea que conten   *
 * algun dos formatos numericos soportados (decimal,quebrado ...) clasifican-*
 * -doos gracias as exploracions que fai.                                    *
 *  Devolve o codigo que lle corresponde o formato de cadea numerica, sendo  *
 * un cero se non se trata dunha cadea numerica cun formato soportado.       *
 *****************************************************************************/
unsigned char cadea_numerica( char * inicial){
	int numero_con_punto=0, numero_con_coma=0,numero_con_dous_puntos=0;
	int numero_con_barra=0;
	int numero_digitos=0;
	char *c,*index=inicial;

	//if (!dixito(*c) && *c!='-') return 0;
	if (*inicial=='-') inicial++;
	//fmen
	c=inicial;
	//   while ((dixito(*c) || *c=='.' || *c==','||*c==':') && *(c+1)!=FIN_DE_PALABRA){
	while ((dixito(*c) || *c=='.' || *c==','||*c==':') && *c!=FIN_DE_PALABRA){
		/* Caracteres que aparecen nun decimal ou cadena "punto".                     */
		if (*c=='.') numero_con_punto++;
		else if (*c==',') numero_con_coma++;
		else if (*c==':') numero_con_dous_puntos++;
		else  numero_digitos++;
		//      if (*c=='/') numero_con_barra++;
		c++;
	}
	//if (dixito(*c))  numero_digitos++;
	if (numero_digitos==0) return 0;
	c--;
	if (*(c+1)==FIN_DE_PALABRA) {
		if (numero_con_punto==0 && numero_con_coma==0 && numero_con_dous_puntos==0){
			if (dixito(*c)) return CARDINAL;
			if (*c==(char)simbolo_ordinal_femenino) return ORDINAL_FEMENINO;
			if (*c==(char)simbolo_ordinal_masculino ) return ORDINAL_MASCULINO;
		}
		if (*c==(char)simbolo_de_grados || *c==(char)simbolo_ordinal_masculino)
			if (numero_con_punto==0 && numero_con_coma==0) return VALOR_DE_GRADOS;
			else return VALOR_DE_GRADOS_DECIMAL;
			if (*c=='%') return PORCENTUAL;
	}
	else if ((*(c+1)=='º' || *(c+1)=='ª') && *(c+2)==FIN_DE_PALABRA) {
		if (*(c+1)=='ª') return ORDINAL_FEMENINO;
		if (*(c+1)=='º') return ORDINAL_MASCULINO;
	}
	else if ( (*c==(char)simbolo_de_grados || *c==(char)simbolo_ordinal_masculino)
			&& (*(c+1)=='C' || *(c+1)=='c') && *(c+2)==0) {
		if (numero_con_punto==0 && numero_con_coma==0) return VALOR_DE_GRADOS;
		else return VALOR_DE_GRADOS_DECIMAL;
	}

	c++;


	//fmen
	/*
	   while (dixito(*c) && *(c+1)!=FIN_DE_PALABRA) c++;
	// Se son todos numeros menos o ultimo paramonos no ultimo.
	if (*(c+1)==FIN_DE_PALABRA) {
	if (dixito(*c)) return CARDINAL;
	if (*c==(char)simbolo_ordinal_femenino) return ORDINAL_FEMENINO;
	if (*c==(char)simbolo_ordinal_masculino ) return ORDINAL_MASCULINO;
	if (*c==(char)simbolo_de_grados) return VALOR_DE_GRADOS ;
	if (*c=='%') return PORCENTUAL;
	}
	// Se non foi ningun dos anteriores enton comprobamos se corresponde a un
	//   decimal ou a algo que se debe ler "noventa e un punto oito".
	c=inicial;
	while (dixito(*c) || *c=='.' || *c==','||*c==':'){
	// Caracteres que aparecen nun decimal ou cadena "punto".
	if (*c=='.') numero_con_punto++;
	if (*c==',') numero_con_coma++;
	if (*c==':') numero_con_dous_puntos++;
	//      if (*c=='/') numero_con_barra++;
	c++;
	}
	*/
	//fmende
	/* Aqui para ser mais exacto hai que comprobar tamen que o ultimo caracter
	   de cada cadea sexa un dixito e que se numero con punto==1 os outros estan
	   a cero.
	   NOS TEMOS EN CONTA QUE A COMA SEMPRE INDICA DECIMAL SE SE PON E OS PUNTOS
	   SE SE USAN SON PARA  SEPARAR EN GRUPOS DE TRES.                            */

	if (*c==FIN_DE_PALABRA) {
		if (numero_con_punto>=1 && numero_con_coma==0){
			/* Se ten mais de un punto estes son para separar (SERIA CLASIFICADO COMO CAR-
			   -DINAL) grupos de 3 ou ben se solo ten un punto este e para indicar mil
			   (21.000) ("vinteun mil") ou se non hai tres cifras despois do unico punto
			   debe entenderse que e decimal e que se lee 91.8, "noventa e un punto oito".*/
			if (numero_con_punto==1 &&
					(! despois_do_ultimo_punto_hai_tres_cifras(c)))
				return NUMERO_CON_PUNTO;
			return CARDINAL;
		}
		if (numero_con_coma==1)  return  DECIMAL;
		if (numero_con_coma>1)  return  NUMEROS_SEPARADOS_POR_GUIONS;

		/* Se hai unha coma independiente de se hai puntos antes este e un numero de-
		   cimal.                                                                     */
		if (numero_con_dous_puntos>=1)  {
			if (forma_de_horas(inicial)) {
				return HORAS;
			}
		}
		if (forma_de_data(index)) return DATA;
		/*
		   c=inicial;
		   while ((*c=='/') && & *(c)!=FIN_DE_PALABRA) {
		   numero_con_barra++;
		   c++;
		   }
		   */
		if (numero_con_barra==1)  return PARTITIVO;
	}
	c=inicial;
	if (forma_de_data(c)) return DATA;
	if (forma_de_partitivo(c)) return PARTITIVO;
	c=inicial;
	if (numeros_separados_por_guions(c)) return NUMEROS_SEPARADOS_POR_GUIONS;
	return 0;
}

/*****************************************************************************
 *  short int todo_maiusculas(unsigned char  *pal):  Detecta se todalas le-  *
 * -tras son maiusculas, devolvendo un 1 en tal caso e 0 en caso contrario.  *
 *****************************************************************************/
int todo_maiusculas(char * pal){
	int mai=0;

	while (*pal) {
    	if (maiuscula((unsigned char) *pal) == 0) {
        	return 0;
        }
        mai++;
        pal++;
    }

    if (mai == 1)
    	return 0;
        
    return 1;

/*
	while (*pal){
		if (maiuscula((unsigned char)*pal)){
			mai++;
			if (mai>1) return 1;
		}
		else if (!letra((unsigned char) *pal)) return 0;
		pal++;
	}

*/

	/*
	   while (*pal){
	   if (!letra((unsigned char) *pal)) return 0;
	   if (maiuscula((unsigned char)*pal)) mai++;
	   if (mai>1) return 1;
	   pal++;
	   }
	   */
//	return 0;
	/*/
	  while (maiuscula((unsigned char)*pal)) pal++;
	  return (*pal==0 && pal>cp+1);
	  */
	/* O de strlen(pal)>1 e para que palabras como os artigos cando van encetando
	   unha frase e por tanto en maiuscula, non os considere como palabra todo
	   maiusculas senon que as clasifique como pal_empeza maiusculas.             */
}

/*****************************************************************************
 *  short int letra_romana(unsigned char l):  Detecta se o parametro que se  *
 * lle pasa pertence o conxunto de letras que se empregan nos numeros romanos*
 * devolvendo un 1 en tal caso e 0 en caso contrario.                        *
 *****************************************************************************/
short int letra_romana( char l)
{
	switch(l) {
		case'M':case'D':case 'C':case'L':case'X':case'V':case'I': return 1;
	}
	return 0;
}

/*****************************************************************************
 *  short int numero_romano(unsigned char*pal):  Comproba se a cadena que se *
 * lle pasa conten somentes letras ou numeros romanos. Para isto chama a fun-*
 * -cion anterior. Devolvendo un 1 en tal caso e 0 en caso contrario.        *
 *****************************************************************************/
short int numero_romano( char*pal)
{
	while (letra_romana(*pal) && *(pal)!=FIN_DE_PALABRA)  pal++;
	if (*pal==FIN_DE_PALABRA) return 1;
	else return 0;
}

/*****************************************************************************
 *  short int forma_de_inicial(t_palabra pal):  Detecta se a cadea esta forma-*
 * -da por unha maiuscula mais un punto, nese caso devolve 1 e se non 0.     *
 *****************************************************************************/
int forma_de_inicial(char * pal)
{
	return (*(pal+1)=='.' && maiuscula((unsigned char)*pal) && strlen(pal)==2 );
	/*   if (strlen(pal)==2 && maiuscula(pal[0]) && pal[1]=='.')
		 return 1;
		 else
		 return 0;
		 */
}
unsigned char cadena_de_signos(char * pal){
	unsigned char * rec=(unsigned char *)pal;

	while (*rec){
		if (!signo(*rec)) return 0;
		rec++;
	}
	return CADENA_DE_SIGNOS;
}
/*****************************************************************************
 *  void clasificar_palabras(t_frase_separada *elemento_de_frase):  Imos co-  *
 * -llendo cada palabra e clasificandoa. O array de estructuras recorremolo  *
 * co punteiro elemento_de_frase que apunta as estructuras que conteñen nun  *
 * campo a palabra e no campo clas_pal asignaselles o codigo correspondente o*
 * tipo de palabra formal que son. Outro campo e palabra_transformada que se-*
 * -ra un punteiro a unha zona de memoria que conten esa palabra se foi nece-*
 * -sario  procesala para extendela e senon este campo esta a null.          *
 *****************************************************************************/
void clasificar_palabras(Token * token){

	while (*token->token) {
		if (numero_romano(token->token)) {
			token->clase_pal=NUM_ROMANO;
		}
		else if (*(token->token+strlen(token->token)-1)=='%'){
			token->clase_pal=PORCENTUAL;
		}
		else if (caracter_especial(token->token[0])){
			token->clase_pal=CARACTER_ESPECIAL;
		}
		else if ((signo((unsigned char)*token->token) && strlen(token->token)<2) ||
        		  (strcmp(token->token, SIMBOLO_RUPTURA_ENTONATIVA) == 0) ||
                  (strcmp(token->token, SIMBOLO_RUPTURA_COMA) == 0)){
			token->clase_pal=SIGNO_PUNTUACION;
		}
		else if ((token->clase_pal=cadea_numerica(token->token))!=0);
		else if (forma_de_abreviatura(token->token)){
			token->clase_pal=ABREVIATURA;
		}
		else if (forma_de_inicial(token->token)){
			token->clase_pal=INICIAL;
		}
		else if (todo_maiusculas (token->token)){
			token->clase_pal=PALABRA_TODO_MAIUSCULAS;
		}
		else if ((token->clase_pal=palabra_estandar(token->token))!=0);
		else if ((token->clase_pal=cadena_de_signos(token->token))!=0);
		else{
			token->clase_pal=SIN_CLASIFICAR;
		}
		
		token++;
	}
}
 

