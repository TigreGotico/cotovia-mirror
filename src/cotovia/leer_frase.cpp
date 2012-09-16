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
 * \file leer_frase.cpp
 * \author fmendez
 * \brief Neste arquivo encóntranse aquelas funcións relacionadas ca obtención  dunha
 *  frase do texto de entrada.
 * \remarks As clases de frases consideradas son:
 *  Enunciativas : Empezan por maiuscula e acaban nun punto.
 *  Interrogativas/exclamativas/de dialogo : que estan delimitadas  por  ? ¡ ou
 *  guions simples ou dobles. As frases de dialogo ademais  de empezar por
 *  guions deben seguir por maiuscula e acabar en puntos  ou guions .   
 *  Titulos: Son aquelas que Empezan por maiuscula ou estan en maiuscula  e acaban nun
 *  salto de linea e ademais empeza outra frase das enumeradas  arriba.   
 *  Para a extraccion tamen se eliminan os formatos de cada frase como tabuladores,
 *  saltos de linea que parten palabras ou frases etc. de xeito que a frase
 *  extraida solo conten caracteres con informacion pero non caracteres de
 *  formato.
 */
 
#include <stdio.h>
#include <string.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "errores.hpp"
#include "letras.hpp"
 
#include "verbos.hpp"
#include "gbm_locuciones.hpp"
 
#include "interfaz_ficheros.hpp"
#include "leer_frase.hpp"

char * lee_cadena;

/** 
 * \author fmendez
 * \remarks Toma como parametro a letra a comprobar se e un codigo de formato,
 * ese o e devolve un 1. Caracteres que aparecen nos arquivos de texto para
 * axudar a formatear o que se escribe como tabuladores, saltos de linea,
 * paxina ,espacios, etc.
 * 
 * \param a 
 * 
 * \return 
 */
/*
 *inline int Leer_frase::caracter_de_formato(unsigned char a){
 *  return (a==' ' || a=='\n' || a=='\t' || a=='\r' || a=='\f');
 *}
 */

/**
 * \author fmendez
 * \remarks A partir dun punteiro que apunta a un punto obten toda a cadea
 * anterior desde o ultimo espacio. Utilizase para comprobar se nalguns
 * casos o punto pertence a unha abreviatura de tratamento persoal ou inicial.
 * \return En pal devolvese a palabra
 */
void Leer_frase::obten_pal_anterior(char * c, char * pal, char * ini){

	int lonxi = 0;

	while(c > ini && (!signo((unsigned char)*c) || *c == 'º' ||
				*c == 'ª' || *c == (char)simbolo_de_grados || *c == '.')) {
		c--;
		lonxi++;
		if (c == texto)
			break;
	}
	if (c != ini)
		c++;

	if (lonxi > LONX_MAX_PALABRA-2)
		lonxi = LONX_MAX_PALABRA-2;
	strncpy(pal,c,lonxi+1);
	pal[lonxi+1] = 0;
}

/** 
 * \author fmendez
 * \remarks Obten a palabra seguinte a partir da posicion do punteiro
 * 
 * \param c 
 * \param pal 
 */
void Leer_frase::obten_pal_seguinte( char *c, t_palabra pal){
	char *aux=pal;

	while (!signo((unsigned char)*c))
		*aux++=*c++;
	*aux=0;
}

/** 
 * \author fmendez
 * \remarks Funcion booleana que nos indica se a palabra que se lle pasa como
 * parámetro e unha abreviatura de tratamento persoal das que se encontran na 
 * lista e que indica que o punto non sera fin de frase ainda que despois 
 * seguramente apareza unha maiuscula, que sera nome propio. 
 * 
 * \param p 
 * 
 * \return 
 */
int Leer_frase::abrev_trat_persoal( char *p){
	t_palabra pal_en_minusculas;
	int i = 0;        // Contador para recorrer a tabla
	const char *trat_persoal[] ={ //estas abreviaturas deben ir sen o punto e en minusculas
		"d.",
		"sr.",
		"sra.",
		"dª.",
		"dª",
		"dr.",
		"s.a.r.",
		"dra.",
		"mons.",
		"excmo.",
		"mr.",
		"mrs.",
		"\0"};

	pasar_a_minusculas(p,pal_en_minusculas);
	while(trat_persoal[i][0]){
		if (!strcmp(pal_en_minusculas,trat_persoal[i])){
			return 1;   // se son iguais
		}
		i++;
	}
	return 0;
}

/** 
 * \author fmendez
 * \remarks Corre a posicion do punteiro sobre unha cadea, quitando varios tabuladores
 * seguidos, varios saltos de linea seguidos ou saltos de paxinas. Utilizase en 
 * varios contextos como cando hai un salto de linea ou cando acaba unha frase
 * limpar o formato hasta o comenzo doutra, limpa un so formato de cada vez a 
 * non ser que haixa varios símbolos de formato seguidos. 
 * 
 * \param adiante 
 * 
 * \return 
 */
char * Leer_frase::limpia_formato(char * adiante){
	while (*adiante && caracter_de_formato(*adiante))
		adiante++;
	return adiante;
}

/** 
 * \author fmendez
 * \remarks Se se trata dunha frase (non o casodunha palabra) partida por salto
 * de linea eliminase todolos posibles caracteres de formato 
 * 
 * \return 
 */
int Leer_frase::frase_partida_por_salto_de_linea(){
	recorre_texto=limpia_formato(recorre_texto);
	return 1;
}

/** 
 * \author fmendez
 * \remarks Os principios de palabra sempre son: interrogacion/excclamacion/guions
 * simples/guions_dobles(dialogos)+maiuscula ou ben por maiuscula. Pode haber 
 * espacios en blanco por medio. Se se detecta unha situacion asi enton tratase
 * dun inicio de frase. Ademais se e a ultima frase hemos encontrar un \0 que 
 * indica que e a ultima e por tanto tamen se comproba isto. 
 * 
 * \param recibido 
 * 
 * \return 
 */
int Leer_frase::inicio_de_frase( char *recibido){
	char *cp=recibido;/* copia do punteiro recibido                            */

	while ((*cp==' '||*cp=='¿'||*cp=='¡'|| *cp=='-' || *cp==comillas_simples
				|| *cp=='"')   &&    *cp!='\0') 
	   	cp++;
	if ( *cp =='\n' || *cp==0 || maiuscula( (unsigned char)*cp)  ) 
	//if ( *cp =='\n' || *cp==0 ) 
		return 1;
	else 
		return 0;
}

/** 
 * \author fmendez
 * \remarks Recibe o punteiro ó primeiro caracter despois do punto,ou signo de pechar
 * exclamacion/interrogación e comproba o formato de acabar a frase e se
 * despois empeza unha nova frase. O primeiro caracter que se pode encontrar e
 * ou un espacio ou ben un salto de linea. Despois adiantase o punteiro
 * mediante a funcion_limpia formato hasta o comenzo da nova frase. 
 * 
 * \param anda 
 * 
 * \return 
 */
int Leer_frase::comprobar_fin_frase_estandar(char *anda){
	char *corre=anda;
	t_palabra_proc pal;

	switch (*anda) {
		case ' ':
			strcpy(tipo_frase->continuacion_texto,"Seguido");
			break;
		case '\n':case '\t':
			strcpy(tipo_frase->continuacion_texto,"Aparte");
			break;
		default:
			/*       strcpy(tipo_frase->continuacion_texto,"Sin clasificar");              */
			break;
	}                 /* END SWITCH                                            */

	corre--;
	if (*corre=='\n' || *corre=='\r') {
		if (*(corre+1)=='\n' || *(corre+1)=='\r') return 1;
		else    return inicio_de_frase(corre+1); //return 0;
	}
	if (*corre=='.'){  // && *(corre+1) =='\n')
		corre = strchr(frase_extraida,0) - 1;
		//corre=frase_extraida+strlen(frase_extraida)-1;
		while (corre >frase_extraida && !caracter_de_formato(*corre)){ 
			corre--;
		}
		if (corre>frase_extraida){
			corre++;
		}
		pasar_a_minusculas(corre,pal);
		if (! (diccionarios->existeEnAbreviaturas(pal,opciones->idioma)!=NULL || diccionarios->existeEnAcronimos(pal, opciones->idioma)!=NULL)) {
			return 1;
		}
		else {
			return 0;
		}
		//if (diccionarios->existeEnAbreviaturas(pal,opciones->idioma)!=NULL return 0;
		//if (diccionarios->existeEnAcronimos(pal, opciones->idioma)!=NULL) return 0;
		//else return 1;
	}
	//return 0;
	corre++;
	corre=limpia_formato(corre);
	return inicio_de_frase(corre);
}

/**
 * \author fmendez
 * \brief 
 * 
 * \param pala 
 * 
 * \return A funcion devolve se o string que se lle pasou era unha maiuscula
 * mais un punto (nese caso da un 1).
 */
int Leer_frase::inicial_de_nome_propio(char * pala)
{
	if (maiuscula((unsigned char)*pala) && strlen(pala)==2 && pala[1]=='.')
		return 1;      // caso de  J.L. Vázquez
	if (strlen(pala)==4 &&
			maiuscula((unsigned char)*pala) && pala[1]=='.' &&
			maiuscula((unsigned char)pala[2]) && pala[3]=='.' )
		return 1; //caso J.L. Vázquez
	return 0;
}

/**
 * \author fmendez
 * \brief 
 * 
 * \param aux 
 * 
 * \return Devolve un 1 se o punto está entre dúas letras.
 */
inline int Leer_frase::punto_entre_letras( char *aux){
	return ( letra((unsigned char)*aux) &&  *(aux-1)=='.' && letra((unsigned char)*(aux-2)) );
}


inline int Leer_frase::punto_entre_numeros( char *aux){
	return ( dixito((unsigned char)*aux) &&  *(aux-1)=='.' && dixito((unsigned char)*(aux-2)) );
}

/**
 * \author fmendez
 * \remarks Se despois dun salto de linea hai unha maiscula iso implica que o
 * anterior foi por exemplo un titulo e e polo tanto unha frase .
 * \warning ¡¡¡OLLO POIDA QUE NON SE USE!!!.
 * \param aux 
 * 
 * \return devolve un 1 se habia maiuscula ou salto de liña.
 */
short int Leer_frase::comprobar_saltolinea( char *aux){

	if (maiuscula((unsigned char)*aux)) return 1;
	if (caracter_de_formato(*aux)){
		recorre_texto++;
		return 1;
	}
	return 0;
}

/**
 * \author fmendez
 * \remarks Esta funcion detecta se se produce esta secuencia a partir da posicion
 * do punteiro que se lle envia como parametro.
 * 
 * \param axu 
 * 
 * \return 
 */
int Leer_frase::secuencia_de_espacios_mais_maiuscula(char * axu){
	char * explorador = axu;

	while (caracter_de_formato(*explorador)) 
		explorador++;
	if (maiuscula((unsigned char)*explorador)) 
		return 1;
	return 0;
}

/**
 * \author fmendez
 * \remarks Esta función utilízase para comprobar se o punto que se encontrou
 * indica que acaba a frase ou o punto se utiliza para outras cousas como por
 * exemplo en decimais, abreviaturas, iniciais de nome propio. (1 -> si fin de frase).
 * 
 * \param aux 
 * 
 * \return 
 */
int Leer_frase::comprobar_punto(char *aux){
	t_palabra palabra_anterior="";

	if (*aux==0){  //se se trata dun punto final
		strcpy(tipo_frase->continuacion_texto,"Frase final");
		return 1;
	}
	if ( *aux=='.' && *(aux+1)=='.' ){ // se se trata de puntos suspensivos
		frase_extraida[strlen(frase_extraida)-1]=0;
		strcat(frase_extraida," ·");
		recorre_texto += 2;
		strcpy(tipo_frase->prosodia_frase,"Suspensiva");
		return 1;
	}
	if (punto_entre_letras(aux)){
		return 0;
	}
	if (punto_entre_numeros(aux)){
		return 0;
	}
	/*
	   if (punto_entre_letras(aux)) {
	   char *corre;
	   t_palabra pal;
	   int cuenta_puntos;

	   corre=frase_extraida+strlen(frase_extraida)-1;
	   while (corre >frase_extraida && (letra(*corre) || *corre=='.' )) corre--;
	   if (corre>frase_extraida) corre++;
	   strcpy(pal,corre);
	   corre=aux;
	   while (*corre && (letra(*corre) || *corre=='.')) corre++;
	   strncat(pal,aux,corre-aux);
	//pasar_a_minusculas(pal,pal);
	if (existeEnAbreviaturasTodas(pal)!=NULL) return 0;
	if (existeEnAcronimos(pal)!=NULL) return 0;
	return inicio_de_frase(aux);

	cuenta_puntos=0;
	corre=pal;
	while (*corre) {
	if (*corre == '.') cuenta_puntos++;
	corre++;
	}
	if (cuenta_puntos*4 > strlen(pal)) return 0;//muchos puntos, se deletrea
	else return 1;//inicio_de_frase(aux);
	}
	*/
	/* O punto entre letras nunca pode ser fin de frase, sexan estas maiusculas
	   ou minusculas. Exemplo: N.A.S.A.                                           */
	if ( *(aux+1)=='\n' || secuencia_de_espacios_mais_maiuscula(aux)){
		// caso de punto+espacios+maiuscula
		//      char *corre;
		//      int cuenta_puntos;
		if (aux>texto+2) {
			obten_pal_anterior(aux-1,palabra_anterior,texto); // palabra anterior ó punto
			if ( abrev_trat_persoal(palabra_anterior) )
				return 0;   // caso de Sr. Fernández
			if (inicial_de_nome_propio(palabra_anterior))
				return 0;    //caso de  J. L. Vázquez

			if (diccionarios->existeEnAbreviaturas(palabra_anterior,opciones->idioma)!=NULL)
				return 0;
			if (diccionarios->existeEnAcronimos(palabra_anterior,opciones->idioma)!=NULL)
				return 0;
		}
		if (*(aux+1)=='\n'){ 
			return 1;
		}
		
		return inicio_de_frase(aux);
		/*
		   cuenta_puntos=0;
		   corre=palabra_anterior;
		   while (*corre) {
		   if (*corre == '.') cuenta_puntos++;
		   corre++;
		   }
		   if (cuenta_puntos*4 > strlen(palabra_anterior)) return 0;//muchos puntos, se deletrea
		   else return 1;//inicio_de_frase(aux);

		//return 1;
		*/
	}
	/*  Se non se produciron os casos particulares anteriores do punto ainda
		temos que comprobar se se produce un fin de frase estandar                */
	return comprobar_fin_frase_estandar(aux);
}

/**
 * \author fmendez
 * \remarks Comproba se ésta e a secuencia no que hai unha palabra partida entre
 * dúas lineas, é decir : letra-guion-salto de linea-letra (1 -> se se cumple).
 * 
 * \param recorre_texto 
 * 
 * \return 
 */
short int Leer_frase::palabra_partida_por_salto_de_linea(char *recorre_texto){

	return (short int)(recorre_texto>texto+2 
			&& letra((unsigned char)*(recorre_texto-3)) 
			&& *(recorre_texto-2)=='-' 
			&& *(recorre_texto-1)=='\n'  
			&&  letra((unsigned char)*recorre_texto));
}
/**
 * \author fmendez
 * \remarks Para que un salto de linea sexa fin de linea, despois debe haber a
 * secuencia necesaria de que empece outra frase e ademais antes do salto de linea
 * debe haber ou unha letra, ou un numero ou ben dous puntos.
 * \param axu 
 * 
 * \return 
 */
short int Leer_frase::comprobar_se_carac_antes_salto_linea_permiten_fin_de_frase(char * axu){

	//char cont=0;

	unsigned char *retrocede= (unsigned char *) axu-2;

	while (*retrocede==' ') 
		retrocede--;
	/*   
		 while (*retrocede==' ' && cont<5 ){
		 axu--; 
		 cont++;
		 };
		 */
	/* axu posicionase asi no primeiro caracter antes do salto de linea
	   non blanco ---> ¡¡¡ Ollo se ten menos de 5caracteres!!!                 */
	if (dixito(*retrocede) || letra(*retrocede) || *retrocede==':') 
		return 1;
	return 0;
}

/**
 * \author fmendez
 * \remarks O que quedaba no bufer pasamolo o principio do bufer, e unha vez trasladado,
 *  leemos mais datos do arquivo hasta encher de novo o bufer.
 * 
 * \param pos_actual_texto 
 * 
 * \return 
 */
char *Leer_frase::cargar_bufer_de_arquivo( char *pos_actual_texto){
	int talla_do_que_quedaba_no_bufer,octetos_lidos;

	talla_do_que_quedaba_no_bufer=(texto+strlen(texto))-pos_actual_texto;
	memcpy(texto,pos_actual_texto,talla_do_que_quedaba_no_bufer);
	if (opciones->entrada_por_fichero) {
		octetos_lidos=fread((texto+talla_do_que_quedaba_no_bufer),1,
				(LONX_BUFER_TEXTO-1)-talla_do_que_quedaba_no_bufer,fent);
	}
	else {
		if ((octetos_lidos=strlen(lee_cadena))<LONX_BUFER_TEXTO-1-talla_do_que_quedaba_no_bufer)
			strcpy(texto+talla_do_que_quedaba_no_bufer,lee_cadena);
		else {
			strncpy(texto+talla_do_que_quedaba_no_bufer,lee_cadena,LONX_BUFER_TEXTO-1-talla_do_que_quedaba_no_bufer);
			octetos_lidos=LONX_BUFER_TEXTO-1-talla_do_que_quedaba_no_bufer;
			lee_cadena+=octetos_lidos;
		}
	}

	*(texto+octetos_lidos+talla_do_que_quedaba_no_bufer)=0;
	if (octetos_lidos<((LONX_BUFER_TEXTO-1)-talla_do_que_quedaba_no_bufer)) {
		arquivo_totalmente_leido=true;
        fclose(fent);
        fent = NULL;
    }
	else {
		arquivo_totalmente_leido=false;
    }

	filtrar_caracteres(texto,texto+octetos_lidos+talla_do_que_quedaba_no_bufer);
	return texto;
}


/**
 * \author fmendez 
 * \remarks So permitimos que os caracteres a tratar estean dentro do xogo
 * previsto senon pasanse a blancos.
 *
 * \param inicio Principio del texto a filtrar  
 * \param fin  Posición final del texto a filtrar
 *
 * \return 
 */
void Leer_frase::filtrar_caracteres(char * inicio, char * fin){
	register char * escribe=inicio;
	register char * copia=inicio;
	register char anteriorr;
	char n_puntos=0;

	anteriorr=0;
	while(copia<fin){
		if (letra((unsigned char)*copia) || dixito(*copia)) {
			n_puntos = 0;
			*escribe=*copia;
		}
		else if (*copia=='.') {
			n_puntos++;
			if (n_puntos>3)
				escribe--;
			else
				*escribe=*copia;
		}
		else {
			n_puntos=0;

			if (*copia=='\\')
				escribe--;//me cargo barras invertidas
			else if (*copia=='/' && anteriorr=='/')
				escribe--;//me cargo barras invertidas

			else if (*copia=='_')
				*escribe=' ';
			else if (*copia=='-' && escribe>inicio && *(escribe-1)=='-')
				escribe--;
			else if (*copia=='·')
				*escribe=' ';
/*
 *      else if (*copia=='à')
 *        *escribe='a';
 *      else if (*copia=='è')
 *        *escribe='e';
 *      else if (*copia=='ì')
 *        *escribe='i';
 *      else if (*copia=='ò')
 *        *escribe='o';
 *      else if (*copia=='ù')
 *        *escribe='u';
 *
 */
			else if ((*copia=='\'' && *(copia+1)=='\'')  ||
					(*copia=='`' && *(copia+1)=='`')){
				*escribe=' ';
				//*escribe='"';
				*(escribe+1)='"';
				copia++;
			}
			else if (!(signo((unsigned char)*copia)||	caracter_especial(*copia)))
				*escribe=' '; // cambiamos este valor raro por un blanco
			else if (*copia==(char)equival_apert_comillas || *copia==(char)equival_cerr_comillas ||
					*copia=='\'' || *copia=='`'){
				*escribe='"';
			}
//			else if (*copia==0){
//				escribe--;
//				fprintf(stderr, "cero %c\n", *copia);
//			}
			else if (*copia =='\r')
				escribe--;
			//*escribe='\n';
			else {
				*escribe=*copia;
				//fprintf(stderr, "jaja %c\n", *copia);
			}
		}
		anteriorr=*copia;
		copia++;
		escribe++;

	} // END WHILE
	*escribe=*copia;
	return;
}

/**
 * \author fmendez
 * \brief .
 * Inicialización de objetos de clase Leer_frase
 *
 * \param tipo  Estructura
 * \param opc  Opciones del sintetizador
 *
 * \return Distinto de cero en caso de error
 */
int Leer_frase::inicializa(t_tipo_frase * tipo, t_opciones *opc){

	tipo_frase = tipo;
	opciones = opc;

	arquivo_totalmente_leido = false;
	lonx_arqui = 0;

	opciones->fstdin ? fent = stdin : fent = NULL;

	return 0;
}

/**
 * \author fmendez
 * \remarks Extrae frase do texto e deixaa en frase extraida, para iso limpa
 * formatos, e busca o posible fin de frase.
 *
 * \param entrada  
 * \param frase  
 * \param ultima  
 *
 * \return Distinto de cero en caso de error 
 */
int Leer_frase::leer_frase(char * entrada, char * frase, bool &ultima){

	char *axu,posible_finfrase;
	char *espacio_texto=NULL,*espacio_frase=NULL;
	char *coma_texto=NULL,*coma_frase=NULL;
	char *barra_texto=NULL,*barra_frase=NULL;
	int f=0, fin;
	char alternativa[100];

	frase_extraida = frase;
	
	if (lonx_arqui==0){
		if (opciones->entrada_por_fichero) {
			if (fent==NULL) {
				if ((fent=fopen(opciones->fentrada,"r"))==NULL){
					strcpy(alternativa,"../input/");
					strcat(alternativa,opciones->fentrada);
					if ((fent=fopen(alternativa,"r"))==NULL){
						fprintf(stderr,"Non se encontra o arquivo de entrada: %s\n",opciones->fentrada);
						return ERROR_FICHERO_DE_ENTRADA;
					}
				}
			}
			lonx_arqui=fread(texto,1,LONX_BUFER_TEXTO-1,fent);
		}
		else {
			lee_cadena=entrada;
			if ((lonx_arqui=strlen(lee_cadena))<LONX_BUFER_TEXTO-1) {
				strcpy(texto,lee_cadena);
			}
			else {
				strncpy(texto,lee_cadena,LONX_BUFER_TEXTO-1);
				lonx_arqui=LONX_BUFER_TEXTO-1;
				lee_cadena+=LONX_BUFER_TEXTO-1;
			}
		}
		*(texto+lonx_arqui)=0;
		recorre_texto=texto;
		if (lonx_arqui<LONX_BUFER_TEXTO-1) {
			arquivo_totalmente_leido=true;
		}
		filtrar_caracteres(texto,texto+lonx_arqui);
	}

	/*  Antes de empezar limpiamos todo o que hai antes do inicio da 1ª frase     */
	*frase_extraida='\0';
	recorre_texto=limpia_formato(recorre_texto);
	/*  Se quedan menos caracteres que os que poden existir nunha frase e ainda
		non se acabou de leer o arquivo totalmente volvemos a encher o bufer de
		memoria de novo                                                           */
	if (arquivo_totalmente_leido==false  &&
			((texto+LONX_BUFER_TEXTO)-recorre_texto<2*LONX_MAX_CARACTERES_FRASE))
		recorre_texto=(char*)cargar_bufer_de_arquivo(recorre_texto);
	/* Imos examinando os caracteres hasta encontrar un posible fin de frase.
	   Comprobase isto, e se non o é séguese a frase.                             */
	do{
		/* Iniciamos as variables de tipo de frase cada vez que se comenza unha frase
		   ou cando se continua despois dun posible fin de frase que non o foi.       */
		*(tipo_frase->continuacion_texto)=0;
		*(tipo_frase->prosodia_frase)=0;

		//strcpy(tipo_frase->continuacion_texto,"");
		//strcpy(tipo_frase->prosodia_frase,"");


		/* Por se hai ó principio do texto algun salto de linea ou espacio para elimi-
		   -nalos e empezar onde está de verdade a primeira frase.                    */
		//      while (!signo_fin_frase((frase_extraida[f]=*recorre_texto))){
		while (!((signo_fin_frase((frase_extraida[f]=*recorre_texto)) && opciones->unalinea==0) ||
					((*recorre_texto=='\n' || *recorre_texto==0) && opciones->unalinea) )){

			if (*recorre_texto==':' && opciones->unalinea==0 )//mira si es una posible hora, si no, pues fin de frase
			//if (*recorre_texto==':')//mira si es una posible hora, si no, pues fin de frase
				if (!(recorre_texto>texto && dixito(*(recorre_texto-1)) && dixito(*(recorre_texto+1))))
					break;

			if (*recorre_texto==' ' ||*recorre_texto=='\t' ||*recorre_texto=='\n'){
				espacio_frase=frase_extraida+f;
				espacio_texto=recorre_texto;
			}
			else if ( (*recorre_texto==',') &&
            		  (strncmp(recorre_texto, SIMBOLO_RUPTURA_ENTONATIVA, strlen(SIMBOLO_RUPTURA_ENTONATIVA)) != 0) &&
                      (strncmp(recorre_texto, SIMBOLO_RUPTURA_COMA, strlen(SIMBOLO_RUPTURA_COMA)) != 0) ) {
				coma_frase=frase_extraida+f;
				coma_texto=recorre_texto;
			}
			else if (*recorre_texto=='/') {
				barra_frase=frase_extraida+f;
				barra_texto=recorre_texto;
			}
#ifdef _NO_RETORNOS
			if (frase_extraida[f]=='\n' || frase_extraida[f]=='\r') 
				frase_extraida[f]=' ';
#endif
			recorre_texto++;
			if (f++ >= LONX_MAX_CARACTERES_FRASE-3) {
				if (coma_texto!=NULL) {
					f=coma_frase-frase_extraida+1;
					recorre_texto=coma_texto+1;
					//frase_extraida[f]=0;fprintf(stderr,"fmendezcoma:\n%s\n",frase_extraida);
				}
				else if (barra_texto!=NULL) {
					f=barra_frase-frase_extraida+1;
					recorre_texto=barra_texto+1;
					//frase_extraida[f]=0;fprintf(stderr,"fmendezbarra:\n%s\n",frase_extraida);
				}
				else if (espacio_texto!=NULL) {
					f=espacio_frase-frase_extraida+1;
					recorre_texto=espacio_texto+1;
					//frase_extraida[f]=0;fprintf(stderr,"fmendezespacio:\n%s\n",frase_extraida);
				}
				frase_extraida[f]=0;
				/*

				   char *aux=recorre_texto;
				   while (aux>inicio && *aux!=' ' && *aux!='\t' && *aux!='\n')
				   aux--;
				   if (aux!=inicio) {
				   f-=recorre_texto-aux;
				   recorre_texto=aux;
				   }
				   else {
				   frase_extraida[f]=0;
				//fprintf(stderr,"fmendez: %s\n",frase_extraida);
				}
				frase_extraida[f]=0;
				//fprintf(stderr,"fmendez: %s\n",frase_extraida);
				}
				frase_extraida[f]=0;
				if (execucion_supervisada) {
				fprintf(stderr,"\nEsta frase é demasiado larga para esta variable.");
				fprintf(stderr,"\nProba a modificar a macro LONX_MAX_CARACTERES_FRASE que \
				está en VARIAB.H");
				fprintf(stderr,"\nTruncámo-la frase unha vez que esta chea a variable e a \
				seguinte frase empezará a partir do lugar onde se truncou esta");
				fprintf(stderr,"\nTROZO ONDE SE TRUNCOU : \n%s",&frase_extraida[f-10]);
				fprintf(stderr,"\nPulsa unha tecla para seguir");
				getchar();
				}
				*/
				recorre_texto = limpia_formato(recorre_texto);
				return OK;
		}           /* END IF                                                */
	}             /* END WHILE                                             */
	/* Do bucle anterior no que se vai copiando en frase extraida so se sae ante
	   un signo de posible fin de frase.                                          */
	if (*recorre_texto=='\n' && opciones->unalinea) {
		recorre_texto++;
		char *kk = frase_extraida + f - 1;
		while (caracter_de_formato(*kk)){
			kk--;
		}
		*(kk+1) = 0;
		//frase_extraida[f] = 0;
		return OK;
	}
	if (*recorre_texto==0 ){ /* se se chegou o fin do texto                 */
		strcpy(tipo_frase->continuacion_texto,"Frase final");
		strcpy(tipo_frase->prosodia_frase,"Final sin puntuacion");
		if (arquivo_totalmente_leido)
			ultima=true;
		return OK;
	}
	if (*recorre_texto=='\n'){
		espacio_frase=frase_extraida+f;
		espacio_texto=recorre_texto;
	}

	recorre_texto++;
	frase_extraida[++f] =0;
	axu=recorre_texto;
	posible_finfrase=frase_extraida[f-1];
	/* Comprobamos segun o caracter que se encontrou se este nos conduce a un fin
	   de frase ou ben debemos seguir engadindo caracteres á frase.
	   Posible_fin_frase e o ultimo caracter extraido                             */
	switch (posible_finfrase){
		case '.':
			fin=comprobar_punto(axu);
			if ((fin) && (strlen(tipo_frase->prosodia_frase)==0) )
				strcpy(tipo_frase->prosodia_frase,"Enunciativa");
			/* E para comprobar que non se lle asignara xa antes outro tipo de prosodia
			   ó corpo principal da frase.                                                */
			break;
		case '\n':
			/* Para titulos pois acaban en salto de linea sen punto, ou ben en dialogos
			   " e dixo: (salto_de_linea) 'Moi ben'...                                    */

			fin=comprobar_fin_frase_estandar(axu);
			if (fin){

				//fmendez paso de esto  if (comprobar_se_carac_antes_salto_linea_permiten_fin_de_frase(axu)){
				strcpy(tipo_frase->prosodia_frase,"Título");
				frase_extraida[f-1]='\0';
				/* quitamos o \n que era o caracter de fin de frase nos títulos xa que carece
				   de informacion                                                             */
				//fmendez paso de esto                }
				//fmendez paso de esto               else fin=0;
			}
			/* se hai un salto de linea e non se acaba a frase enton e unha frase partida
			   por salto de linea e debemos elimina-lo salto de linea para extrae-la frase
			   e ademais se partia unha palabra unir estas duas mitades                   */
			if (!fin && palabra_partida_por_salto_de_linea(recorre_texto))
				f=f-2;  /* copia a palabra que sigue do -\n en frase extraida */
			if (!fin && frase_partida_por_salto_de_linea())
				frase_extraida[f-1]=' ';
			/* neste caso non parte palabras solo separa palabras o salto de linea. Hai
			   que cambiar o \n e por un espacio en frase extraida e limpiar os formatos
			   se se encontran                                                            */
			break;
		case '?':
			// FRAN_CAMPILLO: Antes no se asignaba el valor a fin, y no detectaba el fin de frase.
			if ( (fin = comprobar_fin_frase_estandar(axu)) == 1)
				strcpy(tipo_frase->prosodia_frase,"Interrogativa");
			break;
		case '!':
			if ( (fin = comprobar_fin_frase_estandar(axu)) == 1)
				strcpy(tipo_frase->prosodia_frase,"Exclamativa");
			break;
		default:
			fin=1;
	}
	if (fin) recorre_texto=limpia_formato(recorre_texto);
	/* unha vez que se detectou un fin de frase corrense os caracteres inutiles
	   que poden ser tabuladores, espacios, saltos de linea, etc                  */
	} while (!fin); /* "fin" a 1 significa fin de frase. */
	return OK;
}


/**
 * \author	Fran Campillo
 * \remarks Función encargada de liberar los recursos del objeto.
 */

void Leer_frase::finaliza() {

	if (fent) {
    	fclose(fent);
        fent = NULL;
    } // if (fent)

}

/**
 * \author	fmendez
 * \remarks Constructor de la clase Leer_frase.
 */
Leer_frase::Leer_frase() {

    if ((texto = (char *) malloc(LONX_BUFER_TEXTO * sizeof(char))) == NULL) {
		fprintf(stderr, "Leer_frase: Error al asignar memoria en texto\n");
	}
    fent = NULL;
}

/**
 * \author	campillo
 * \remarks Destructor de la clase Leer_frase. Pepe
 */

Leer_frase::~Leer_frase() {

	if (fent)
    	fclose(fent);
    free(texto);
}

