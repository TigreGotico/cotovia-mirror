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
 
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tip_var.hpp>
#include <utilidades.hpp>

void Reset(t_frase_separada *rec,int numero_de_elementos_de_frase_separada, t_opciones opciones) {

	register t_infinitivo *inf;
	unsigned char *c1,*c2;
	int kk=sizeof(t_frase_separada);
	int kk2=sizeof(t_infinitivo);


	for(int i=0;i<numero_de_elementos_de_frase_separada;i++){
		*rec->pal=0;
		*rec->lema=0;
		rec->pal_transformada=NULL;
		*rec->pal_sil_e_acentuada=0;
		rec->clase_pal=0;
		if (!opciones.audio && !opciones.wav &&  !opciones.lin &&
				(opciones.pre || (opciones.tra>0 && opciones.tra<4))) {
			rec= (t_frase_separada *) ((char  *)rec + kk);
			continue;
		}

		c1=rec->cat_gramatical;
		c2=rec->cat_gramatical_descartada;
		for(int j=0;j<N_CAT;j++,c1++,c2++){
			*c1=0;
			*c2=0;
		}
		inf=rec->inf_verbal;
		c1=rec->cat_verbal;
		for(int j=0;j<TEMPOS_POSIBLES;j++,c1++){
			*c1=0;
			*inf->infinitivo=0;
			*inf->raiz=0;
			*inf->enclitico.enclitico=0;
			inf->enclitico.grupo=0;
			*inf->artigo=0;
			*inf->reconstruccion=0;
			inf->modelo=0;
			inf= (t_infinitivo *) ((char  *)inf + kk2);
		}
		rec->xenero=0;
		rec->numero=0;
		rec->tipo_sintagma=0;
		rec->n_silabas=0;
		rec->tonicidad_forzada=0;
		rec= (t_frase_separada *) ((char  *)rec + kk);

	}
}

//1. Resolver lo de los enclíticos.
//2. Hacer una función que sirva para escribir donde haya que escribir. :D
//3. Hablar con Fran para que decide_categorias sea público. ¿O bien, un nuevo constructor...?

void escribe(t_frase_separada *st_fraseSeparada, int vNumero) {

	for (int k=0;k<vNumero;k++) {
		printf("%s",st_fraseSeparada[k].pal);
		printf("\t");
		for (int m=0;st_fraseSeparada[k].cat_gramatical[m]>0;m++) {
			escribe_categoria_gramatical(st_fraseSeparada[k].cat_gramatical[m],stdout);
			printf("\t");
		}
		printf("|");
		for (int m=0;st_fraseSeparada[k].cat_gramatical_descartada[m]>0;m++) {
			escribe_categoria_gramatical(st_fraseSeparada[k].cat_gramatical_descartada[m],stdout);
			printf("\t");
		}
		puts("");
	}
}


void invertir_cadea2( char* pal){
	char *com, *fin, letra;

	com=pal;
	fin=pal+strlen(pal);
	fin--;
	while(com<fin) {
		letra=*com;
		*com++=*fin;
		*fin--=letra;
	}
}


/*
	 void *reservaMemoria(int tamano) {

	 void *puntero;

	 if ((puntero= calloc(tamano,1))!=NULL) return puntero;
	 fprintf(stderr, "ZA: Problemas con la memoria.");
//        exit(1);
return NULL;
}


*/


unsigned char traducirXeneroNumero(char *entrada) {

	if ( *entrada == 'M' || !strcmp(entrada,"MASCULINO")){
		return MASCULINO;
	}
	if ( *entrada == 'F' || !strcmp(entrada,"FEMININO")){
		return FEMININO;
	}
	if (*entrada == 'S' || !strcmp(entrada,"SINGULAR")) {
		return SINGULAR;
	}
	if (*entrada == 'P' || !strcmp(entrada,"PLURAL")){
		return PLURAL;
	}
	if (*entrada == 'A' || !strcmp(entrada,"AMBIGUO")){
		return AMBIGUO;
	}
	if ((*entrada == 'N' && !*(entrada+1)) || !strcmp(entrada,"NEUTRO")){
		return NEUTRO;
	}
	//if (!strcmp(entrada,"NO_ASIGNADO")   || !strcmp(entrada,"NAS"))   return NO_ASIGNADO;
	return NO_ASIGNADO;
}




/******************************************************************************
 *  int comprobar_en_lista_de_palabras(char **inicio_lista,char lonx_item,int *
 * lonx,unsigned char *pal):                                                  *
 *                                                                            *
 *                      EXPLICACION DE PARAMETROS:                            *
 *                                                                            *
 *  inicio_lista: Punteiro a un punteiro a caracteres. Esta funcion busca unha*
 * palabra en arrais de estructuras. O primeiro campo de cada estructura debe *
 * ser un punteiro a caracter, onde se encontrara a palabra segun a cal se en-*
 * -contra ordenado o arrai de caracteres.                                    *
 *                                                                            *
 *  lonx_item: tamaño de cada estructura do arrai. Como normalmente estamos   *
 * buscando en arrais de punteiros a palabras, enton pasarase como parametro  *
 * "sizeof(char*)". En caso de que as estructuras fosen mais complicadas pasa-*
 * -riase como parametro "sizeof(<tipo_estructura>)".                         *
 *                                                                            *
 *  lon_lista: Numero de elementos do arrai (ou lista).                       *
 *                                                                            *
 * Funcion que dada unha palabra trata de localiza-la mediante busqueda bina- *
 * -ria. Devolve o indice da lista onde se encontra se a busqueda tivo exito e*
 * se	non devolve -1.                                                         *
 ******************************************************************************/
int comprobar_en_lista_de_palabras2(const char **inicio_lista,int lonx_item,int lonx, const char * pal){
	int lim_superior=lonx-1;
	int lim_inferior=0;
	int punto_medio;
	int pos_relativa;
	char * bite_inicial_lista= (char *) inicio_lista;
	char ** palabra_punto_medio;

	for(;;){
		punto_medio=(lim_superior+lim_inferior)/2;
		palabra_punto_medio=(char **)(bite_inicial_lista+(punto_medio*lonx_item));
		pos_relativa=strcmp(pal,*palabra_punto_medio);
		if (pos_relativa>0) {
			if(lim_inferior>=lim_superior) return -1;
			lim_inferior=punto_medio+1;
		}
		else if (pos_relativa<0) {
			if(lim_inferior>=lim_superior) return -1;
			lim_superior=punto_medio-1;
		}
		else return punto_medio;
	}
}


//Función para devolver sencillamente el resultado más cercano en la lista, pero por debajo.

int comprobar_en_lista_de_palabras3(char **inicio_lista,int lonx_item,int lonx, char * pal){

	int lim_superior=lonx-1;
	int lim_inferior=0;
	int punto_medio, punto_medio_anterior=0;
	int pos_relativa;
	char * bite_inicial_lista= (char *) inicio_lista;
	char ** palabra_punto_medio;

	for(;;){

		punto_medio=(lim_superior+lim_inferior)/2;
		palabra_punto_medio=(char **)(bite_inicial_lista+(punto_medio*lonx_item));
		pos_relativa=strncmp(pal,*palabra_punto_medio,strlen(pal)-1);
		if (pos_relativa>0) {
			if(lim_inferior>=lim_superior) return punto_medio_anterior;
			lim_inferior=punto_medio+1;
			punto_medio_anterior=punto_medio;  //Solo actualizamos si es inferior.
		}
		else if (pos_relativa<0) {
			if(lim_inferior>=lim_superior) return punto_medio_anterior;
			lim_superior=punto_medio-1;

		}
		else return punto_medio;
	}
}

/******************************************************************************
 *  int comprobar_en_lista_de_inicio_de_palabras (char **inicio_lista, char   *
 * lonx_item, int lon_lista, unsigned char* pal, char tipo_busqueda).         *
 *                                                                            *
 *	Explicacion de parametros:                                                 *
 *                                                                            *
 *  inicio_lista: Punteiro a un punteiro a caracteres. Esta funcion busca unha*
 * palabra en arrais de estructuras. O primeiro campo de cada estructura debe *
 * ser un punteiro a caracter, onde se encontrara a palabra segun a cal se en-*
 * -contra ordenado o arrai de caracteres.                                    *
 *                                                                            *
 *  lonx_item: tamaño de cada estructura do arrai. Como normalmente estamos   *
 * buscando en arrais de punteiros a palabras, enton pasarase como parametro  *
 * "sizeof(char*)".                                                           *
 *                                                                            *
 * En caso de que as estructuras fosen máis complicadas pasariase como parame-*
 * -tro "sizeof(<tipo_estructura>).                                           *
 *                                                                            *
 * lon_lista: Numero de elementos do arrai (ou lista).                        *
 *                                                                            *
 * Esta funcion trata de buscar, dada unha palabra, aquela palabra das que se *
 * encontra na lista que coincide con ela en todolos caracteres iniciais.     *
 * Utiliza nun principio busqueda binaria, e despois secuencial hacia atras.  *
 * Exemplo: trozo de lista: ...                                               *
 *													ar                                     *
 *													art                                    *
 *													arte                                   *
 *													...                                    *
 *	palabra de entrada: artista: =>devolve o indice correspondente o string art*
 *                                                                            *
 * Podese utilizar para encontrar, dada unha palabra, a raiz desta nun diccio-*
 * -nario. Pola simetria do caso tamen se emprega para buscar as desinencias  *
 * verbais nunha palabra se esta esta invertida e tamen a lista na que hai que*
 * buscala.                                                                   *
 *                                                                            *
 *  Devolve o indice da lista onde se encontra se a busqueda tivo exito e se  *
 * non devolve -1.                                                            *
 ******************************************************************************/
int comprobar_en_lista_de_inicio_de_palabras3(char **inicio_lista,int lonx_item,
		int lon_lista, char* pal,char tipo_busqueda)
{
	int lim_superior=lon_lista-1;
	/* Debido a que o ultimo elemento do arrai e a cantidade total de elementos -1,
		 xa que se empeza a almacenar no elemento cero.                             */
	int lim_inferior=0;
	int punto_medio;
	char encontrado=0;
	char fin_busqueda=0;
	int pos_relativa;

	t_palabra_proc palabra="";
	t_palabra_proc pal_aux="";
	char * bite_inicial_lista=(char*) inicio_lista;
	char *palabra_punto_medio;

	strcpy(palabra,pal);
	if (tipo_busqueda==EN_DICCIONARIO_INVERSO)
		invertir_cadea2(palabra);      /* Invertir palabra (diccionario inverso) */
	do {                             // BEGIN DO-WHILE: Metodo de busqueda binaria
		punto_medio=(lim_superior+lim_inferior)/2;
		palabra_punto_medio=bite_inicial_lista+(punto_medio*lonx_item);

		// FRAN_CAMPILLO Nos cargamos el strcmp()
		//      if (strcmp(palabra_punto_medio,""))
		pos_relativa=strcmp(palabra, *((char**)palabra_punto_medio));
		//      else                          // Se nos saimos de rango -> rematamos a busca
		//         return -1;                 // e devolvemos que non esta na lista
		// FRAN_CAMPILLO

		while(1) {                    // BEGIN WHILE: Modificacion do intervalo de busca
			if (pos_relativa>0) {      // BEGIN IF: Posicion relativa > 0
				if(lim_superior==lim_inferior || lim_inferior>lim_superior) {
					fin_busqueda=1;
					break;
				}
				lim_inferior=punto_medio+1;
				break;
			}                          // END IF: Posicion relativa > 0
			if (pos_relativa<0) {      // BEGIN IF: Posicion relativa < 0
				if(lim_superior==lim_inferior || lim_inferior>lim_superior) {
					fin_busqueda=1;
					break;
				}
				lim_superior=punto_medio-1;
				break;
			}                          // END IF: Posicion relativa < 0
			if (pos_relativa==0) {     // BEGIN IF: Posicion relativa = 0
				fin_busqueda=1;
				encontrado=1;
				break;
			}                          // END IF: Posicion relativa = 0
		}                             // END WHILE: Modificacion do intervalo de busca
	} while((!encontrado) && fin_busqueda==0);
	if (encontrado) return punto_medio;
	/* Comparamos agora se coinciden os inicios das duas palabras. Para eso
		 recortamos a mais larga e comparamolas de novo                             */
	strcpy(pal_aux,palabra);
	pal_aux[strlen(*((char**)palabra_punto_medio))]=0;
	if (strcmp(pal_aux,*((char**)palabra_punto_medio))==0) return punto_medio;
	/* Comparamos se o sitio no que parou e o mais proximo ou ben comenzamos a
		 busqueda hacia atras.                                                      */
	if (punto_medio==0) return -1;

	do {
		//   while (palabra[0]== (char)**((char**)(palabra_punto_medio-(1*(unsigned char) lonx_item)))) {
		/* A busca hacia atras sempre e posible se polo menos coinciden as duas palabras
			 no primeiro caracter. Para isto comprobamos se a palabra que se encontra in-
			 mediatamente antes ca que esta (por iso restamos o punteiro 1*(unsigned char) lonx_item)
			 tamen ten comun o primeiro caracter.                                       */
		punto_medio--;
		//    palabra_punto_medio=palabra_punto_medio-(1*(unsigned char) lonx_item);
		// ZA. Línea anterior fallaba ¿? en algunos casos...
		// Se lo decimos de otra manera.
		palabra_punto_medio=bite_inicial_lista+(punto_medio*lonx_item);
		/* Decrementamos este punteiro en tantos bytes como a lonxitude de cada estruc-
			 -tura para que apunte a palabra anterior.                                  */
		strcpy(pal_aux,palabra);        // Igualamo-la lonxitude das duas palabras
		pal_aux[strlen(*((char**)palabra_punto_medio))]=0;
		if (strcmp(pal_aux,*((char**)palabra_punto_medio))==0) return punto_medio;
		if (punto_medio==0) return -1;
	}while (palabra[0]== *((char**)palabra_punto_medio)[0]);
	return -1;
	}


	int comprobar_en_lista_de_inicio_de_palabras2(char **inicio_lista,int lonx_item,
			int lon_lista, char* pal,char tipo_busqueda)
	{
		int lim_superior=lon_lista-1;
		/* Debido a que o ultimo elemento do arrai e a cantidade total de elementos -1,
			 xa que se empeza a almacenar no elemento cero.                             */
		int lim_inferior=0;
		int punto_medio;
		char encontrado=0;
		char fin_busqueda=0;
		int pos_relativa;

		t_palabra_proc palabra="";
		t_palabra_proc pal_aux="";
		char * bite_inicial_lista=(char*) inicio_lista;
		char *palabra_punto_medio;

		strcpy(palabra,pal);
		if (tipo_busqueda==EN_DICCIONARIO_INVERSO)
			invertir_cadea2(palabra);      /* Invertir palabra (diccionario inverso) */
		do {                             // BEGIN DO-WHILE: Metodo de busqueda binaria
			punto_medio=(lim_superior+lim_inferior)/2;
			palabra_punto_medio=bite_inicial_lista+(punto_medio*lonx_item);

			// FRAN_CAMPILLO Nos cargamos el strcmp()
			//      if (strcmp(palabra_punto_medio,""))
			pos_relativa=strcmp(palabra, *((char**)palabra_punto_medio));
			//      else                          // Se nos saimos de rango -> rematamos a busca
			//         return -1;                 // e devolvemos que non esta na lista
			// FRAN_CAMPILLO

			while(1) {                    // BEGIN WHILE: Modificacion do intervalo de busca
				if (pos_relativa>0) {      // BEGIN IF: Posicion relativa > 0
					if(lim_superior==lim_inferior || lim_inferior>lim_superior) {
						fin_busqueda=1;
						break;
					}
					lim_inferior=punto_medio+1;
					break;
				}                          // END IF: Posicion relativa > 0
				if (pos_relativa<0) {      // BEGIN IF: Posicion relativa < 0
					if(lim_superior==lim_inferior || lim_inferior>lim_superior) {
						fin_busqueda=1;
						break;
					}
					lim_superior=punto_medio-1;
					break;
				}                          // END IF: Posicion relativa < 0
				if (pos_relativa==0) {     // BEGIN IF: Posicion relativa = 0
					fin_busqueda=1;
					encontrado=1;
					break;
				}                          // END IF: Posicion relativa = 0
			}                             // END WHILE: Modificacion do intervalo de busca
		} while((!encontrado) && fin_busqueda==0);
		if (encontrado) return punto_medio;
		/* Comparamos agora se coinciden os inicios das duas palabras. Para eso
			 recortamos a mais larga e comparamolas de novo                             */
		strcpy(pal_aux,palabra);
		pal_aux[strlen(*((char**)palabra_punto_medio))]=0;
		if (strcmp(pal_aux,*((char**)palabra_punto_medio))==0) return punto_medio;
		/* Comparamos se o sitio no que parou e o mais proximo ou ben comenzamos a
			 busqueda hacia atras.                                                      */
		if (punto_medio==0) return -1;
		while (palabra[0]== (char)**((char**)(palabra_punto_medio-(1*(unsigned char) lonx_item)))) {
			/* A busca hacia atras sempre e posible se polo menos coinciden as duas palabras
				 no primeiro caracter. Para isto comprobamos se a palabra que se encontra in-
				 mediatamente antes ca que esta (por iso restamos o punteiro 1*(unsigned char) lonx_item)
				 tamen ten comun o primeiro caracter.                                       */
			punto_medio--;
			palabra_punto_medio=palabra_punto_medio-(1*(unsigned char) lonx_item);
			/* Decrementamos este punteiro en tantos bytes como a lonxitude de cada estruc-
				 -tura para que apunte a palabra anterior.                                  */
			strcpy(pal_aux,palabra);        // Igualamo-la lonxitude das duas palabras
			pal_aux[strlen(*((char**)palabra_punto_medio))]=0;
			if (strcmp(pal_aux,*((char**)palabra_punto_medio))==0) return punto_medio;
			if (punto_medio==0) return -1;
		}
		return -1;
	}




	int esta_palabra_ten_a_categoria_de2(unsigned char categoria,t_frase_separada *pal_frase_separada)
	{
		unsigned char *categoria_actual=pal_frase_separada->cat_gramatical;

		while (*categoria_actual)
		{
			if (*categoria_actual==categoria) return 1;
			categoria_actual++;
		}
		return 0;
	}


	/**
	 * \brief 
	 * \author fmendez 
	 *
	 * \param tiempo  
	 * \param genero  
	 * \param numero  
	 * \param otros_tiempos  
	 *
	 * \return 
	 */
	unsigned char formas_no_personales(unsigned char *tiempo, unsigned char *genero, 
			unsigned char *numero,char *otros_tiempos){

		int kk;
		unsigned char forma_no_personal = 0;

		*otros_tiempos = 0;
		while (*tiempo) {
			if (*tiempo >= 199 && *tiempo <= 216) {
				//*genero=MASCULINO;
				if (*tiempo == 200 || *tiempo == 206 || *tiempo == 212 ){
					*numero = PLURAL;
				}
				else {
					*numero = SINGULAR;
				}
				forma_no_personal = INFINITIVO;
			}
			else if (*tiempo >= 183 && *tiempo <= 194) {
				kk=(*tiempo - 183) % 4;
				switch (kk) {
					case 0:
						*genero = MASCULINO;
						*numero = SINGULAR;
						break;
					case 1:
						*genero = FEMININO;
						*numero = SINGULAR;
						break;
					case 2:
						*genero = MASCULINO;
						*numero = PLURAL;
						break;
					case 3:
						*genero = FEMININO;
						*numero = PLURAL;
						break;
					default:
						*genero = NO_ASIGNADO;
						*numero = NO_ASIGNADO;
						break;
				}
				forma_no_personal = PARTICIPIO;
			}
			else if (*tiempo >= 180 && *tiempo <= 182){
				*genero = NO_ASIGNADO;
				*numero = NO_ASIGNADO;
				forma_no_personal = XERUNDIO;
			}
			else  {
				*otros_tiempos = 1;
			}
			tiempo++;
		}
		return forma_no_personal;
	}

	/******************************************************************************
	 *  escribe_tempo_verbal(unsigned char codigo): Recibe como parametro un codi-*
	 * -go, en funcion do cal diranos a conxugacion e forma verbal que temos en-  *
	 * -tre mans (tempo verbal, modo, numero e persoa).                           *
	 ******************************************************************************/

	void escribe_tempo_verbal(unsigned char codigo,FILE *fich_sal)
	{
		unsigned char tempo,persoa,conj;

		//tempo=(unsigned char) ((codigo-1)/6);
		tempo=(unsigned char) ((codigo-1)/18);
		//   persoa=(unsigned char) ((codigo-1)-tempo*6);
		persoa=(unsigned char) ((codigo-1)%6);
		conj=(unsigned char)((((codigo-1)/6) % 3)+1);

		if (codigo==180)  {
			fputs("\tXERUNDIO\t1ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==181)  {
			fputs("\tXERUNDIO\t2ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==182)  {
			fputs("\tXERUNDIO\t3ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==183)  {
			fputs("\tPARTICIPIO\t1ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==184)  {
			fputs("\tPARTICIPIO\t1ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==185)  {
			fputs("\tPARTICIPIO\t1ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==186)  {
			fputs("\tPARTICIPIO\t1ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==187)  {
			fputs("\tPARTICIPIO\t2ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==188)  {
			fputs("\tPARTICIPIO\t2ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==189)  {
			fputs("\tPARTICIPIO\t2ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==190)  {
			fputs("\tPARTICIPIO\t2ª CONXUGACIÓN\t",fich_sal);
			return;
		}

		if (codigo==191)  {
			fputs("\tPARTICIPIO\t3ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==192)  {
			fputs("\tPARTICIPIO\t3ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==193)  {
			fputs("\tPARTICIPIO\t3ª CONXUGACIÓN\t",fich_sal);
			return;
		}
		if (codigo==194)  {
			fputs("\tPARTICIPIO\t3ª CONXUGACIÓN\t",fich_sal);
			return;
		}

		// PERSOA E NUMERO

		switch(persoa) {
			case 0:
				fputs("1ª DO SINGULAR\t",fich_sal);
				break;
			case 1:
				fputs("2ª DO SINGULAR\t",fich_sal);
				break;
			case 2:
				fputs("3ª DO SINGULAR\t",fich_sal);
				break;
			case 3:
				fputs("1ª DO PLURAL\t",fich_sal);
				break;
			case 4:
				fputs("2ª DO PLURAL\t",fich_sal);
				break;
			case 5:
				fputs("3ª DO PLURAL\t",fich_sal);
				break;
		}

		// TEMPO MODO E CONXUGACION

		switch(tempo)  {
			case 0:
				fputs("PRESENTE DO INDICATIVO",fich_sal);
				break;
			case 1:
				fputs("IMPERFECTO DO INDICATIVO",fich_sal);
				break;
			case 2:
				fputs("PERFECTO DO INDICATIVO",fich_sal);
				break;
			case 3:
				fputs("PLUSCUAMPERFECTO DO INDICATIVO",fich_sal);
				break;
			case 4:
				fputs("FUTURO DO INDICATIVO",fich_sal);
				break;
			case 5:
				fputs("CONDICIONAL DE INDICATIVO",fich_sal);
				break;
			case 6:
				fputs("PRESENTE DO SUBXUNTIVO",fich_sal);
				break;
			case 7:
				fputs("IMPERFECTO DO SUBXUNTIVO",fich_sal);
				break;
			case 8:
				fputs("FUTURO DO SUBXUNTIVO",fich_sal);
				break;
			case 9:
				fputs("IMPERATIVO",fich_sal);
				break;
			case 11:
				fputs("INFINITIVO_CONXUGADO",fich_sal);
				break;
		}

		switch(conj) {
			case 1:
				fputs("\t1ª CONXUGACION\t",fich_sal);
				break;
			case 2:
				fputs("\t2ª CONXUGACION\t",fich_sal);
				break;
			case 3:
				fputs("\t3ª CONXUGACION\t",fich_sal);
				break;
		}

		/*
			 switch(tempo)  {
			 case 0:
			 fputs("PRESENTE DO INDICATIVO, 1ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 1:
			 fputs("PRESENTE DO INDICATIVO, 2ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 2:
			 fputs("PRESENTE DO INDICATIVO, 3ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 3:
			 fputs("IMPERFECTO DO INDICATIVO, 1ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 4:
			 fputs("IMPERFECTO DO INDICATIVO, 2ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 5:
			 fputs("IMPERFECTO DO INDICATIVO, 3ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 6:
			 fputs("PERFECTO DO INDICATIVO, 1ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 7 :
			 fputs("PERFECTO DO INDICATIVO, 2ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 8:
			 fputs("PERFECTO DO INDICATIVO, 3ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 9:
			 fputs("PLUSCUAMPERFECTO DO INDICATIVO, 1ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 10:
			 fputs("PLUSCUAMPERFECTO DO INDICATIVO, 2ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 11:
			 fputs("PLUSCUAMPERFECTO DO INDICATIVO, 3ª CONXUGACIÓN ",fich_sal);
			 break;
			 case 12:
			 fputs("FUTURO DO INDICATIVO, 1ª CONXUGACIÓN",fich_sal);
			 break;
			 case 13:
			 fputs("FUTURO DO INDICATIVO, 2ª CONXUGACIÓN",fich_sal);
			 break;
			 case 14:
			 fputs("FUTURO DO INDICATIVO, 3ª CONXUGACIÓN",fich_sal);
			 break;
			 case 15:
			 fputs("CONDICIONAL DE INDICATIVO, 1ª CONXUGACIÓN",fich_sal);
			 break;
			 case 16:
			 fputs("CONDICIONAL DE INDICATIVO, 2ª CONXUGACIÓN",fich_sal);
			 break;
			 case 17:
			 fputs("CONDICIONAL DE INDICATIVO, 3ª CONXUGACIÓN",fich_sal);
			 break;
			 case 18:
			 fputs("PRESENTE DO SUBXUNTIVO, 1ª CONXUGACIÓN",fich_sal);
			 break;
			 case 19:
			 fputs("PRESENTE DO SUBXUNTIVO, 2ª CONXUGACIÓN",fich_sal);
			 break;
			 case 20:
			 fputs("PRESENTE DO SUBXUNTIVO, 3ª CONXUGACIÓN",fich_sal);
			 break;
			 case 21:
			 fputs("IMPERFECTO DO SUBXUNTIVO, 1ª CONXUGACIÓN",fich_sal);
			 break;
			 case 22:
			 fputs("IMPERFECTO DO SUBXUNTIVO, 2ª CONXUGACIÓN",fich_sal);
			 break;
	case 23:
		fputs("IMPERFECTO DO SUBXUNTIVO, 3ª CONXUGACIÓN",fich_sal);
		break;
	case 24:
		fputs("FUTURO DO SUBXUNTIVO, 1ª CONXUGACIÓN",fich_sal);
		break;
	case 25:
		fputs("FUTURO DO SUBXUNTIVO, 2ª CONXUGACIÓN",fich_sal);
		break;
	case 26:
		fputs("FUTURO DO SUBXUNTIVO, 3ª CONXUGACIÓN",fich_sal);
		break;
	case 27:
		fputs("IMPERATIVO, 1ª CONXUGACIÓN",fich_sal);
		break;
	case 28:
		fputs("IMPERATIVO, 2ª CONXUGACIÓN",fich_sal);
		break;
	case 29:
		fputs("IMPERATIVO, 3ª CONXUGACIÓN",fich_sal);
		break;
	case 33:
		fputs("INFINITIVO_CONXUGADO, 1ª CONXUGACIÓN",fich_sal);
		break;
	case 34:
		fputs("INFINITIVO_CONXUGADO, 2ª CONXUGACIÓN",fich_sal);
		break;
	case 35:
		fputs("INFINITIVO_CONXUGADO, 3ª CONXUGACIÓN",fich_sal);
		break;
}
*/
return;
}


/******************************************************************************
 *  escribe_categoria_gramatical(unsigned char codigo): Recibe como parametro *
 * un codigo, en funcion do cal dirano-la categoria gramatical.               *
 ******************************************************************************/
void escribe_categoria_gramatical(unsigned char codigo,FILE *fich_sal){

	char kk[FILENAME_MAX];
	fputs(escribe_categoria_gramatical(codigo, kk), fich_sal);
}
/******************************************************************************
 *  escribe_tipo_de_pal(unsigned char codigo): Recibe como parametro un codi- *
 * -go, en funcion do cal dirano-lo tipo de palabra da que se trata.          *
 ******************************************************************************/
void escribe_tipo_de_pal(unsigned char codigo,FILE *fich_sal)
{
	switch(codigo) {
		case NON_PROCESADA:
			fputs("NON_PROCESADA",fich_sal);
			break;
		case SIN_CLASIFICAR:
			fputs("SIN_CLASIFICAR",fich_sal);
			break;
		case NUM_ROMANO:
			fputs("NUM_ROMANO",fich_sal);
			break;
		case CARDINAL:
			fputs("CARDINAL",fich_sal);
			break;
		case ORDINAL_MASCULINO:
			fputs("ORDINAL_MASCULINO",fich_sal);
			break;
		case ORDINAL_FEMENINO:
			fputs("ORDINAL_FEMENINO",fich_sal);
			break;
		case VALOR_DE_GRADOS:
			fputs("VALOR_DE_GRADOS",fich_sal);
			break;
		case VALOR_DE_GRADOS_DECIMAL:
			fputs("VALOR_DE_GRADOS_DECIMAL",fich_sal);
			break;
		case CADENA_DE_SIGNOS:
			fputs("CADENA_DE_SIGNOS",fich_sal);
			break;
		case NUMERO_CON_PUNTO:
			fputs("NUMERO_CON_PUNTO",fich_sal);
			break;
		case DATA:
			fputs("DATA",fich_sal);
			break;
		case DECIMAL:
			fputs("DECIMAL",fich_sal);
			break;
		case PORCENTUAL:
			fputs("PORCENTUAL",fich_sal);
			break;
		case HORAS:
			fputs("HORAS",fich_sal);
			break;
		case PARTITIVO:
			fputs("PARTITIVO",fich_sal);
			break;
		case CADENAS_DE_OPERACIONS:
			fputs("CADENAS_DE_OPERACIONS",fich_sal);
			break;
		case ABREVIATURA:
			fputs("ABREVIATURA",fich_sal);
			break;
		case INICIAL:
			fputs("INICIAL",fich_sal);
			break;
		case PALABRA_NORMAL:
			fputs("PALABRA_NORMAL",fich_sal);
			break;
		case PALABRA_NORMAL_EMPEZA_MAY:
		case PALABRA_NORMAL_EMPEZA_MAY_Y_CATEGORIA_DESCONOCIDA:
			fputs("PALABRA_NORMAL_EMPEZA_MAIUSCULAS",fich_sal);
			break;
		case PALABRA_TODO_MAIUSCULAS:
			fputs("PALABRA_TODO_MAIUSCULAS",fich_sal);
			break;
		case PALABRA_CON_GUION_NO_MEDIO :
			fputs("PALABRA_CON_GUION_NO_MEDIO",fich_sal);
			break;
		case SIGNO_PUNTUACION:
			fputs("SIGNO_PUNTUACION",fich_sal);
			break;
		case NUMEROS_SEPARADOS_POR_GUIONS:
			fputs("NUMEROS_SEPARADOS_POR_GUIONS",fich_sal);
			break;
		case PALABRA_CONSONANTES_MINUSCULAS:
			fputs("PALABRA_CONSONANTES_MINUSCULAS",fich_sal);
			break;
		case DELETREO:
			fputs("DELETREO",fich_sal);
			break;
	}
}

/******************************************************************************
 *  escribe_tipo_de_proposicion(unsigned char codigo): Recibe como parametro  *
 * un codigo e en funcion del escribe no arquivo de saida o tipo de proposi-  *
 * -cion de que se trata.                                                     *
 ******************************************************************************/
void escribe_tipo_de_proposicion(unsigned char codigo,FILE *fich_sal)
{
	switch(codigo) {
		case CONX_SUBOR_COMPAR:
			fputs("PROPOSICION_COMPARATIVA",fich_sal);
			break;
		case CONX_COOR_COPU:
			fputs("PROPOSICION COORDINADA COPULATIVA",fich_sal);
			break;
		case CONX_COOR_DISX:
			fputs("PROPOSICION_DISXUNTIVA",fich_sal);
			break;
		case CONX_SUBOR_CONDI:
			fputs("PROPOSICION CONDICIONAL",fich_sal);
			break;
		case CONX_SUBOR:
			fputs("PROPOSICION_SUBORDINADA",fich_sal);
			break;
		case CONX_SUBOR_CAUS:
			fputs("PROPOSICION CAUSAL",fich_sal);
			break;
		case CONX_COOR_ADVERS:
			fputs("PROPOSICION_ADVERSATIVA",fich_sal);
			break;
		default:
			fputs("PROPOSICION NON CLASIFICADA",fich_sal);
	}
}
/******************************************************************************
 *  escribe_tipo_de_sintagma(unsigned char codigo): Recibe como parametro un  *
 * codigo e en funcion del escribe no arquivo de saida o tipo de sintagma de  *
 * que se trata.                                                              *
 ******************************************************************************/
void escribe_tipo_de_sintagma(unsigned char codigo,FILE *fich_sal)
{
	switch (codigo) {
		case SINTAGMA_NOMINAL:
			fputs("S.N.",fich_sal);
			break;
		case CONX_SUBOR:
			fputs("NEXO_CONX_SUBOR",fich_sal);
			break;
		case NEXO_SUBORDINANTE:
			fputs("NEXO_SUBORDINANTE", fich_sal);
			break;
		case SINTAGMA_PREPOSICIONAL:
			fputs("S.PREP",fich_sal);
			break;
		case SINTAGMA_ADXECTIVAL:
			fputs("S.ADXECT.",fich_sal);
			break;
		case SINTAGMA_VERBAL:
			fputs("S.VERBAL",fich_sal);
			break;
		case SINTAGMA_ADVERBIAL:
			fputs("S.ADVERBIAL",fich_sal);
			break;
		case SINTAGMA_NON_CLASIFICADO:
			fputs("NON_CLASIFICADO",fich_sal);
			break;
		case PUNTO:
			fputs("PUNTO",fich_sal);
			break;
		case COMA:
			fputs("COMA",fich_sal);
			break;
		case RUPTURA_ENTONATIVA:
			fputs("RUPTURA_ENTONATIVA", fich_sal);
			break;
		case RUPTURA_COMA:
			fputs("RUPTURA_COMA", fich_sal);
			break;
		case PUNTO_E_COMA:
			fputs("PUNTO_E_COMA",fich_sal);
			break;
		case DOUS_PUNTOS:
			fputs("DOUS_PUNTOS",fich_sal);
			break;
		case PUNTOS_SUSPENSIVOS:
			fputs("PUNTOS_SUSPENSIVOS",fich_sal);
			break;
		case APERTURA_INTERROGACION:
			fputs("APERTURA_INTERROGACION",fich_sal);
			break;
		case PECHE_INTERROGACION:
			fputs("PECHE_INTERROGACION",fich_sal);
			break;
		case APERTURA_PARENTESE:
			fputs("APERTURA_PARENTESE",fich_sal);
			break;
		case PECHE_PARENTESE:
			fputs("PECHE_PARENTESE",fich_sal);
			break;
		case APERTURA_EXCLAMACION:
			fputs("APERTURA_EXCLAMACION",fich_sal);
			break;
		case PECHE_EXCLAMACION:
			fputs("PECHE_EXCLAMACION",fich_sal);
			break;
		case APERTURA_CORCHETE:
			fputs("APERTURA_CORCHETE",fich_sal);
			break;
		case PECHE_CORCHETE:
			fputs("PECHE_CORCHETE",fich_sal);
			break;
		case GUION:
			fputs("GUION",fich_sal);
			break;
		case DOBLES_COMINNAS:
			fputs("DOBLES_COMINNAS",fich_sal);
			break;
		case SIMPLES_COMINNAS:
			fputs("SIMPLES_COMINNAS",fich_sal);
			break;
		case GUION_BAIXO:
			fputs("GUION_BAIXO",fich_sal);
			break;
		case CONX_COOR_COPU:
			fputs("CONX_COOR_COPU",fich_sal);
			break;
		case CONX_COOR_DISX:
			fputs("CONX_COOR_DISX",fich_sal);
			break;
		case CONX_SUBOR_FINAL:
			fputs("CONX_SUBOR_FINAL",fich_sal);
			break;
		case CONX_SUBOR_CONDI:
			fputs("CONX_SUBOR_CONDI",fich_sal);
			break;
		case CONX_SUBOR_CAUS:
			fputs("CONX_SUBOR_CAUS",fich_sal);
			break;
		case CONX_SUBOR_CONCES:
			fputs("CONX_SUBOR_CONCES",fich_sal);
			break;
		case CONX_SUBOR_CONSE:
			fputs("CONX_SUBOR_CONSE",fich_sal);
			break;
		case CONX_SUBOR_TEMP:
			fputs("CONX_SUBOR_TEMP",fich_sal);
			break;
		case CONX_SUBOR_LOCA:
			fputs("CONX_SUBOR_LOCA",fich_sal);
			break;
		case CONX_SUBOR_COMPAR:
			fputs("CONX_SUBOR_COMPAR",fich_sal);
			break;
		case CONX_SUBOR_MODAL:
			fputs("CONX_SUBOR_MODAL",fich_sal);
			break;
		case CONX_COOR_ADVERS:
			fputs("CONX_COOR_ADVERS",fich_sal);
			break;
		case LOC_CONX_COOR_COPU:
			fputs("LOC_CONX_COOR_COPU",fich_sal);
			break;
		case LOC_CONX_COOR_ADVERS:
			fputs("LOC_CONX_COOR_ADVERS",fich_sal);
			break;
		case LOC_CONX_SUBOR_CAUS:
			fputs("LOC_CONX_SUBOR_CAUS",fich_sal);
			break;
		case LOC_CONX_SUBOR_CONCES:
			fputs("LOC_CONX_SUBOR_CONCES",fich_sal);
			break;
		case LOC_CONX_SUBOR_CONSE:
			fputs("LOC_CONX_SUBOR_CONSE",fich_sal);
			break;
		case LOC_CONX_SUBOR_CONDI:
			fputs("LOC_CONX_SUBOR_CONDI",fich_sal);
			break;
		case LOC_CONX_SUBOR_LOCAL:
			fputs("LOC_CONX_SUBOR_LOCAL",fich_sal);
			break;
		case LOC_CONX_SUBOR_TEMP:
			fputs("LOC_CONX_SUBOR_TEMP",fich_sal);
			break;
		case LOC_CONX_SUBOR_MODA:
			fputs("LOC_CONX_SUBOR_MODA",fich_sal);
			break;

	}
}
/******************************************************************************
 *  escribe_tipo_de_tonema(unsigned char codigo): Recibe como parametro un co-*
 * -digo e en funcion del escribe no arquivo de saida o tipo de tonema.       *
 ******************************************************************************/
void escribe_tipo_de_tonema(unsigned char codigo,FILE *fich_sal)
{
	switch(codigo) {
		case INICIO_INTERROGACION_PARCIAL:
			fputs("INICIO_INTERROGACIÓN_PARCIAL",fich_sal);
			break;
		case FIN_INTERROGACION_PARCIAL:
			fputs("FIN_INTERROGACIÓN_PARCIAL",fich_sal);
			break;
		case INTERROGACION_PARCIAL:
			fputs("INTERROGACIÓN_PARCIAL",fich_sal);
			break;
		case INICIO_INTERROGACION_TOTAL:
			fputs("INICIO_INTERROGACIÓN_TOTAL",fich_sal);
			break;
		case FIN_INTERROGACION_TOTAL:
			fputs("FIN_INTERROGACIÓN_TOTAL",fich_sal);
			break;
		case INTERROGACION_TOTAL:
			fputs("INTERROGACIÓN_TOTAL",fich_sal);
			break;
		case INICIO_EXCLAMACION:
			fputs("INICIO_EXCLAMACIÓN",fich_sal);
			break;
		case FIN_EXCLAMACION:
			fputs("FIN_EXCLAMACIÓN",fich_sal);
			break;
		case EXCLAMACION:
			fputs("EXCLAMACIÓN",fich_sal);
			break;
		case INICIO_CORCHETE:
			fputs("INICIO_CORCHETE",fich_sal);
			break;
		case FIN_CORCHETE:
			fputs("FIN_CORCHETE",fich_sal);
			break;
		case CORCHETE:
			fputs("CORCHETE",fich_sal);
			break;
		case INICIO_PARENTESE:
			fputs("INICIO_PARENTESE",fich_sal);
			break;
		case FIN_PARENTESE:
			fputs("FIN_PARENTESE",fich_sal);
			break;
		case PARENTESE:
			fputs("PARENTESE",fich_sal);
			break;
		case INICIO_ENTRE_COMINNAS:
			fputs("INICIO_ENTRE_COMINNAS",fich_sal);
			break;
		case FIN_ENTRE_COMINNAS:
			fputs("FIN_ENTRE_COMINNAS",fich_sal);
			break;
		case ENTRE_COMINNAS:
			fputs("ENTRE_COMINNAS",fich_sal);
			break;
		case INICIO_ENTRE_GUIONS:
			fputs("INICIO_ENTRE_GUIÓNS",fich_sal);
			break;
		case FIN_ENTRE_GUIONS:
			fputs("FIN_ENTRE_GUIÓNS",fich_sal);
			break;
		case ENTRE_GUIONS:
			fputs("ENTRE_GUIÓNS",fich_sal);
			break;
		case ENUMERATIVO:
			fputs("ENUMERATIVO",fich_sal);
			break;
		case FIN_ENUMERATIVO:
			fputs("FIN_ENUMERATIVO",fich_sal);
			break;
		case DE_SUSPENSE:
			fputs("DE_SUSPENSE",fich_sal);
			break;
		case INCISO:
			fputs("INCISO",fich_sal);
			break;
		case ENUNCIATIVA:
			fputs("ENUNCIATIVA",fich_sal);
			break;
		case FIN_ENUNCIATIVA:
			fputs("FIN_ENUNCIATIVA",fich_sal);
			break;
		case INICIO_ENUNCIATIVA:
			fputs("INICIO_ENUNCIATIVA",fich_sal);
			break;
		default:
			fputs("TONEMA NON CLASIFICADO",fich_sal);
			break;
	}
}
/******************************************************************************
 ******************************************************************************/

/*----------------------------------------------------------------------------
	-  Nesta parte do arquivo encontranse funcions para pedir memoria e colocar-
	- en memoria o lido desde o arquivo no que se encontra o texto.            -
	- Tamén hai funcions para abrir e pecha-los arquivos, resetea-las variables-
	- globais, etc. Non hai funcions que manexen os verdadeiros datos do progra-
	- ma.                                                                      -
	----------------------------------------------------------------------------*/


char *escribe_gen(unsigned char codigo,char *aux){

	switch(codigo) {
		case MASCULINO:
			strcpy(aux,"MASCULINO");
			break;

		case FEMININO:
			strcpy(aux,"FEMININO");
			break;

		case NEUTRO:
			strcpy(aux,"NEUTRO");
			break;
		case AMBIGUO:
			strcpy(aux,"AMBIGUO");
			break;

		case NO_ASIGNADO:
			strcpy(aux,"NO_ASIGNADO");
			break;
		default:
			strcpy(aux,"CATEGORIA_NON_CLASIFICADA");
			break;
	}
	return aux;
}
char *escribe_num(unsigned char codigo,char *aux){

	switch(codigo) {
		case AMBIGUO:
			strcpy(aux,"AMBIGUO");
			break;
		case NO_ASIGNADO:
			strcpy(aux,"NO_ASIGNADO");
			break;
		case SINGULAR:
			strcpy(aux,"SINGULAR");
			break;
		case PLURAL:
			strcpy(aux,"PLURAL");
			break;
		default:
			strcpy(aux,"CATEGORIA_NON_CLASIFICADA");
			break;
	}
	return aux;
}

int coge_parametros(int ac,char **av, t_opciones * opciones){
	int cont;
	char kk[FILENAME_MAX];

	if (ac==1) {
		return error_de_sintaxis();
	}

	if (av[1][0]=='-') {
		opciones->fstdin=1;
	}
	else {
		strcpy(opciones->fentrada,av[1]);

#ifdef _WIN32
		char *apunta_path = (char *) opciones->fentrada;

		while ( (apunta_path = strstr(apunta_path + 1, "/")) != NULL)
			*apunta_path = '\\';
#endif

	}

	for (cont=1;cont<ac;cont++) {
		if (strcmp(av[cont],"-dirsal")==0)  {
			sscanf(av[cont+1],"%s",opciones->dir_sal);
			cont+=2;
			while (cont<ac && av[cont][0]!='-') {
				strcat(opciones->dir_sal," ");
				sscanf(av[cont],"%s",kk);
				strcat(opciones->dir_sal,kk);
				cont++;
			}
			cont--;
		}
		else if (strcmp(av[cont],"-dirdata")==0){
			sscanf(av[cont+1],"%s",opciones->dir_bd);
			cont+=2;
			while (cont<ac && av[cont][0]!='-') {
				strcat(opciones->dir_bd," ");
				sscanf(av[cont],"%s",kk);
				strcat(opciones->dir_bd,kk);
				cont++;
			}
			cont--;
		}
		else if (strcmp(av[cont],"-et")==0){
			sscanf(av[cont+1],"%d",&(opciones->et));
			if (opciones->et == 0) {
				fprintf(stderr, "Error, el escalado temporal debe ser un entero.\n");
				return 1;
			}
		}
		else if (strcmp(av[cont],"-ep")==0){
			sscanf(av[cont+1],"%d",&(opciones->ep));
			if (opciones->ep == 0) {
				fprintf(stderr, "Error, el escalado de f0 debe ser un entero.\n");
				return 1;
			}
		}
		else if (strcmp(av[cont],"-fuerza_fronteras")==0){
			opciones->fuerza_fronteras = 1;
		}
		else if (strcmp(av[cont], "-tipo_seleccion") == 0) {
			if (cont >= ac - 1) {
				fprintf(stderr, "Error, la opción -tipo_seleccion debe ir seguida de un parámetro.\n");
				return 1;
			}
			sscanf(av[cont + 1], "%d", &(opciones->tipo_seleccion_unidades));
		}
		else if (strcmp(av[cont], "-tipo_seleccion_entonativa") == 0) {
			if (cont >= ac - 1) {
				fprintf(stderr, "Error, la opción -tipo_seleccion_entonativa debe ir seguida de un parámetro.\n");
				return 1;
			}
			sscanf(av[cont + 1], "%d", &(opciones->tipo_seleccion_entonativa));
		}
		else if (strcmp(av[cont], "-tipo_candidatos") == 0) {
			if (cont >= ac - 1) {
				fprintf(stderr, "Error, la opción -tipo_candidatos debe ir seguida de un parámetro.\n");
				return 1;
			}
			sscanf(av[cont + 1], "%d", &(opciones->tipo_candidatos));
		}
		else if (strcmp(av[cont], "-modif_prosod") == 0) {
			if (cont >= ac - 1) {
				fprintf(stderr, "Error, la opción -modif_prosod debe ir seguida de un parámetro.\n");
				return 1;
			}
			sscanf(av[cont + 1], "%d", &(opciones->modificacion_prosodica));
		}
		else if (strcmp(av[cont],"-noprepro")==0){
			opciones->noprepro=1;
		}
		else if (strcmp(av[cont],"-info_unidades")==0){
			if (cont >= ac - 1)
				opciones->info_unidades = 1;
			else
				if (av[cont + 1][0] != '-')
					sscanf(av[cont + 1], "%d", &(opciones->info_unidades));
				else
					opciones->info_unidades = 1;
		}
		else if (strcmp(av[cont], "-N") == 0) {
			sscanf(av[cont+1], "%d", &opciones->N);
		}
		else if (strcmp(av[cont], "-M") == 0) {
			sscanf(av[cont + 1], "%d", &opciones->M);
		}
		else if (strcmp(av[cont],"-lista_ficheros")==0){
			opciones->lista_ficheros=1;
		}
		else if (strcmp(av[cont],"-ficheros_segmentacion")==0){
			opciones->ficheros_segmentacion = 1;
		}
		else if (strcmp(av[cont], "-fich_morfo") == 0) {
			if (cont < ac - 1)
				strcpy(opciones->fich_morfosintactico, av[++cont]);
			else
				return error_de_sintaxis();
		}
		else if (strcmp(av[cont], "-fich_conf") == 0) {
			if (cont < ac - 1)
				strcpy(opciones->fich_conf, av[++cont]);
			else
				return error_de_sintaxis();
		}
		else if (strcmp(av[cont], "-fich_unidades") == 0) {
			if (cont < ac - 1)
				strcpy(opciones->fich_unidades, av[++cont]);
			else
				return error_de_sintaxis();
		}
		else if (strcmp(av[cont], "-fich_uso_unidades") == 0) {
			if (cont < ac - 1)
				strcpy(opciones->fich_uso_unidades, av[++cont]);
			else
				return error_de_sintaxis();
		}
		else if (strcmp(av[cont],"-stdout")==0){
			opciones->fstdout=1;
		}
		else if (strcmp(av[cont],"-linea")==0){
			opciones->unalinea=1;
		}
		else if (strcmp(av[cont],"-separa_lin")==0){
			opciones->separa_lin=1;
		}
		else if (strcmp(av[cont],"-voz")==0){
			opciones->audio=AUDIO;
		}
		else if (strcmp(av[cont],"-g")==0){
			opciones->idioma=GALEGO;
		}
		else if (strcmp(av[cont],"-c")==0){
			opciones->idioma=CASTELLANO;
		}
		else if (strcmp(av[cont],"-f8")==0){
			opciones->f8=F8KHZ;
		}
		else if (strcmp(av[cont],"-wav")==0){
			opciones->wav=FWAV;
		}
		else if (strcmp(av[cont],"-porpalabras")==0){
			opciones->por_palabras=1;
		}
		else if (strcmp(av[cont],"-difo")==0){
			opciones->difo=1;
		}
		else if (strcmp(av[cont],"-tra")==0) {
			if (cont<ac-1){
				sscanf(av[cont+1],"%s",kk);
				opciones->tra=(char) atoi(kk);
				if (opciones->tra<0 || opciones->tra>3){ 
					opciones->tra=0;
				}
			}
			opciones->tra++;
		}
		else if (strcmp(av[cont],"-tralt")==0) {
			if (cont<ac-1){
				sscanf(av[cont+1],"%s",kk);
				opciones->alternativas=(char) atoi(kk);
				if (opciones->alternativas<0 || opciones->alternativas>4){
					opciones->alternativas=0;
				}
			}
			opciones->alternativas++;
			opciones->tra = 1;
		}
		else if (strcmp(av[cont],"-ftra")==0){
			if (cont < ac-1){
				sscanf(av[cont+1],"%s",kk);
				if (*kk != '-') {
#ifdef _WIN32
					if (strchr(kk,'\\')==NULL) {
						strcpy(opciones->fich_tra,".\\");
					}
#else
					if (strchr(kk,'/')==NULL) {
						strcpy(opciones->fich_tra,"./");
					}
#endif
					strcat(opciones->fich_tra,kk);
				}
			}
			opciones->tra = 1;
			opciones->ftra = 1;
		}


		else if (strcmp(av[cont],"-pre")==0) {
			opciones->pre=1;
		}
		else if (strcmp(av[cont],"-fon")==0) {
			opciones->fon = 1;
		}
		else if (strcmp(av[cont],"-lin")==0) {
			if (cont<ac-1){
				sscanf(av[cont+1],"%s",kk);
				opciones->lin=(char)atoi(kk);
				if (opciones->lin<0 || opciones->lin>9){ 
					opciones->lin=0;
				}
			}
			opciones->lin++;
		}
		else if (strcmp(av[cont],"-factor")==0) {
			sscanf(av[cont+1],"%f",&(opciones->factor));
			//sscanf(av[cont+2],"%f",&(opciones->factor2));
		}
		else if (strcmp(av[cont],"-bd")==0) {
			*opciones->locutores=0;
			while (cont<ac-1 && *av[cont+1]!='-'){
				if (*opciones->locutores){
					strcat(opciones->locutores, " ");
				}
				strcat(opciones->locutores, av[cont+1]);
				cont++;
			}
			if (!*opciones->locutores){
				return error_de_sintaxis();
			}
		}
		else if (strcmp(av[cont],"-l")==0) {
			*opciones->lenguajes= 0;
			while (cont < ac - 1 && *av[cont + 1] != '-'){
				if (*opciones->lenguajes){
					strcat(opciones->lenguajes, " ");
				}
				strcat(opciones->lenguajes, av[cont+1]);
				cont++;
			}
			if (!*opciones->lenguajes){
				return error_de_sintaxis();
			}
		}

		else if (strcmp(av[cont],"-bddur")==0){
			sscanf(av[cont+1],"%d",&(opciones->bddur));
			if (opciones->bddur<0 || opciones->bddur>2){ 
				opciones->bddur=0;
			}
		}
		else if (strcmp(av[cont],"-s")==0 || strcmp(av[cont],"--separa_tokens")==0){
			opciones->separa_token = 1;
		}
		else if (strcmp(av[cont],"-h")==0 || strcmp(av[cont],"--help")==0){
			return error_de_sintaxis();
		}
		else {
			if (av[cont][0]=='-'){
				fprintf(stderr,"Opcion %s incorrecta\n",av[cont]);
				return error_de_sintaxis();
			}
		}
	}

	set_dir_data(opciones);

	if ( ( (opciones->tipo_seleccion_entonativa == 2) && (opciones->tipo_seleccion_unidades != 4) && (opciones->tipo_seleccion_unidades != 5) ) ||
			( ( (opciones->tipo_seleccion_unidades == 4) || (opciones->tipo_seleccion_unidades == 5) ) && (opciones->tipo_seleccion_entonativa != 2) ) ) {
		fprintf(stderr, "El parámetro -tipo_seleccion [4 5] debe ir combinado con -tipo_seleccion_entonativa 2.\n");
		return 1;
	}

	if ( (opciones->tipo_seleccion_entonativa == 2) && (opciones->tipo_seleccion_unidades < 4) ) {
		fprintf(stderr, "El parámetro -tipo_seleccion [0 1 2 3] debe ir combinado con -tipo_seleccion_entonativa [0 1].\n");
		return 1;
	}


	return 0;

}


void set_dir_data(t_opciones * opciones){

	if (*opciones->dir_bd==0) {
		char *barra, path[FILENAME_MAX];
#ifndef _WIN32	
		struct stat buf;
		int cacatua;
		if (cacatua=stat("/usr/share/cotovia/data", &buf)) {
			int length = readlink("/proc/self/exe", path, FILENAME_MAX);
			path[length] = '\0';
			//fprintf(stderr, "ejecutable: %s\n", path);
		
			strcpy(opciones->dir_bd, path);
			if ((barra = strrchr(opciones->dir_bd,'/')) == NULL){
				strcpy(opciones->dir_bd,"../data/");
			}
			else{
				strcpy(barra, "/../data/");
			}
		}
		else {
			strcpy(opciones->dir_bd,"/usr/share/cotovia/data/");
		}
    //fprintf(stderr, "%d - dirdata: %s\n", cacatua,opciones->dir_bd);
#else
		GetModuleFileName(GetModuleHandle(NULL), path, FILENAME_MAX);
		//fprintf(stderr, "ejecutable: %s\n", path);
		strcpy(opciones->dir_bd, path);
		if ((barra = strrchr(opciones->dir_bd,'\\')) == NULL){
			strcpy(opciones->dir_bd,"..\\data\\");
		}
		else{
			strcpy(barra, "\\..\\data\\");
		}
#endif
	}


}	

/**
 * \author fmendez
 * \remarks Esta funcion da unha mensaxe da sintaxis do programa, cos seus
 * distintos argumentos de chamada, e acto seguido produce o fin do programa.
 */
int error_de_sintaxis (void){

	fprintf(stderr,"OPCIONS DE LINEA DE COMANDOS:\n");
	fprintf(stderr,"Primeiro parametro: arquivo de entrada: se non\n\
			se pon se toma a entrada por defecto (stdin)\n");
	fprintf(stderr,"-voz      Xera voz\n");
	fprintf(stderr,"-wav      Xera arquivo de son tipo wav\n");
	fprintf(stderr,"-f8       Xera voz a 8 KHz (16 KHz por defecto)\n");
	fprintf(stderr,"-pre      Xera frase preprocesada\n");
	fprintf(stderr,"-tra      Transcricion fonetica:\n\
			0 minima (por defecto)\n\
			1 + acento\n\
			2 + silabas\n\
			3 + pausas\n");
	fprintf(stderr,"-lin      Xera datos linguisticos\n\
			0 completa (por defecto)\n\
			1 so analise morfoloxica\n\
			2 categoria, xenero, numero, tonicidade\n\
			3 tipo sintagma, sintagma, no. de silabas do sintagma\n\
			4 saida eagles\n\
			5 saida cardenal\n\
			6 saida bea\n\
			7 analise sintactico con pausas do modelo de pausado\n\
			8 salida traductor\n");
	fprintf(stderr, "-fon   Escribe nun arquivo a secuencia de fonemas que compoñen a frase.\n");
	fprintf(stderr, "-N	[número] 	Número total de contornos entonativos resultantes da búsqueda entonativa.\n");
	fprintf(stderr, "-M	[número]	Número de contornos entonativos que pasan á búsqueda de unidades acústicas.\n");
	fprintf(stderr, "-info_unidades		Arquivos coa información da selección de unidades acústicas e prosódicas.\n");
	fprintf(stderr, "-fich_morfo [fichero]	En lugar de analizar a frase, lé a análise morfosintáctica do arquivo indicado.\n");
	fprintf(stderr, "-ficheros_segmentacion	Arquivos coa información da segmentación fonética e as marcas de síntesis do arquivo de son xerado.\n");
	fprintf(stderr,"-stdout   Resultado pola saida estandar\n");
	fprintf(stderr,"-linea    Unha liña->unha frase\n");
	fprintf(stderr,"-separa_lin    Separa as frases na saida -lin x (separador ################################)\n");
	//   fprintf(stderr,"-noprepro Sin preprocesado\n");
	fprintf(stderr, "-fuerza_fronteras Forza que as fronteiras entonativas sexan as indicadas na frase de entrada (co símbolo ,#R-E#).\n");
	fprintf(stderr,"-tipo_seleccion    [0 1 2 3 4 5] (5 por defecto) Tipo de seleccion de unidades:\n\t0 => Cálculo de contornos en paralelo\n\t1 => Cálculo secuencial para cada contorno.\n\t2 => Cálculo secuencial, pero dividiendo a selección de unidades acústicas en segmentos limitados por silencios ou oclusivas xordas.\n\t3 => Cálculo de contornos en paralelo  dividindo as unidades acústicas en segmentos.\n\t4 => Similar a 2, pero facendo a búsqueda entonativa por grupos fónicos. Debe combinarse con -tipo_seleccion_entonativa 2.\n\t5 => Simellantea 3, pero facendo a búsqueda entonativa por grupos fónicos. Debe combinarse con -tipo_seleccion_entonativa 2.\n");
	fprintf(stderr, "-tipo_seleccion_entonativa [0 1 2] (2 por defecto) Tipo de selección entonativa:\n\t0 => Cálculo dos contornos de forma global, para toda a oración.\n\t1 => Cálculo dos contornos por grupos fónicos, escollendo un conxunto de contornos para toda a frase.\n\t2 => Cálculo dos contornos por grupos fónicos, devolvendo unha serie de posibilidades para cada grupo fónico.\n");
	fprintf(stderr,"-tipo_candidatos    [0 1] (0 por defecto) Tipo de seleccion de candidatos para o algoritmo de Viterbi:\n\t0 => Todos os semifonemas que coincidan en carácter tónico son candidatos.\n\t1 => Únicamente os que coincidan en carácter tónico, posición na frase e tipo de frase.\n");
	fprintf(stderr, "-modif_prosod [0 1 2 3 4] (3 por defecto): \n\t0 => nunca se modifican prosódicamente as unidades.\n\t1 => se modifican sempre.\n\t2 => non se modifican, pero non se copia directamente a forma de onda de unidades consecutivas.\n\t3 => modificanse se a diferencia entre os valores desexados e os buscados é demasiado grande.\n\t4 => non se modifican as unidades, pero xeranse a partir das súas marcas de pitch.\n");
	fprintf(stderr,"-et       Porcentaxe de escala temporal (100 por defecto)\n");
	fprintf(stderr,"-ep       Porcentaxe de escala de pitch (100 por defecto)\n");
#ifndef _WIN32
	fprintf(stderr,"-dirdata  Directorio de base voz e dicionarios (../data por defecto)\n");
	fprintf(stderr,"-dirsal   Directorio de saida (dir. actual por defecto)\n");
#else
	fprintf(stderr,"-dirdata  Directorio da base de voz e dicionarios (..\\data por defecto)\n");
	fprintf(stderr,"-dirsal   Directorio de saida (dir. actual por defecto)\n");
#endif
	//   fprintf(stderr,"-bddur          base de datos de duracion (0/1/2) 0: Mod. ad. e prod.; 1: Zscores ; 2: Inicial \n");
	fprintf(stderr,"-difo     Sintetizador baseado en difonemas.\n");
	//   fprintf(stderr,"-estatisticas   \n");
	fprintf(stderr,"-bd       Seleccion da base de datos de voz:\n");
	fprintf(stderr,"            Freire (por defecto)\n");
	fprintf(stderr,"            Paulino\n");
	fprintf(stderr,"            David\n");
	fprintf(stderr,"            Sonia\n");
	fprintf(stderr,"-l        Seleccion do idioma:\n");
	fprintf(stderr,"            gl (por defecto)\n");
	fprintf(stderr,"            es\n");
	fprintf(stderr, "-fich_conf	[Arquivo de configuración]. Empleado na creación das bases de voz.\n");
	fprintf(stderr, "-fich_unidades [Arquivo unidades]. Xera voz sintética a partir das unidades indicadas no arquivo.\n");
	fprintf(stderr, "-fich_uso_unidades [Arquivo]. Escribe nun arquivo os semifonemas que se escolleron para a síntesis. Empreégase para a poda de unidades acústicas.\n");
	fprintf(stderr,"Exemplos: cotovia texto.txt -voz -bd Paulino\n");
#ifndef _WIN32
	fprintf(stderr,"          /home/cotovia texto -voz -tra -dirdata /home/data -dirsal /home/transcripciones\n");
	fprintf(stderr,"          cat ../texto | cotovia -pre -stdout >texto_preproc\n");
#else
	fprintf(stderr,"          c:\\programas\\cotovia.exe texto.txt -voz -tra -dirdata c:\\data -dirsal c:\\transcripciones\n");
	fprintf(stderr,"          type ..\\texto | cotovia.exe -pre -stdout >texto_preproc\n");
#endif
	return -1;
}




/**
 * \fn escribe_categoria_gramatical
 * \remarks Traduce categorías: código -> cadena.
 * \author fmendez
 * \param[in] codigo Categoría a traducir
 * \param[out] cadena Categoría imprimible
 * \return categoría (cadena)
 */
inline char *escribe_categoria_gramatical(unsigned char codigo, char *cadena){

	switch(codigo) {
		case ART_DET:
			return strcpy(cadena,"ART_DET");
		case CONTR_CONX_ART_DET:
			return strcpy(cadena,"CONTR_CONX_ART_DET");
		case CONTR_PREP_ART_DET:
			return strcpy(cadena,"CONTR_PREP_ART_DET");
		case CONTR_INDEF_ART_DET:
			return strcpy(cadena,"CONTR_INDEF_ART_DET");
		case ART_INDET:
			return strcpy(cadena,"ART_INDET");
		case CONTR_PREP_ART_INDET:
			return strcpy(cadena,"CONTR_PREP_ART_INDET");
		case INDEF:
			return strcpy(cadena,"INDEF");
		case INDEF_PRON:
			return strcpy(cadena,"INDEF_PRON");
		case INDEF_DET:
			return strcpy(cadena,"INDEF_DET");
		case CONTR_PREP_INDEF:
			return strcpy(cadena,"CONTR_PREP_INDEF");
		case CONTR_PREP_INDEF_DET:
			return strcpy(cadena,"CONTR_PREP_INDEF_DET");
		case CONTR_PREP_INDEF_PRON:
			return strcpy(cadena,"CONTR_PREP_INDEF_PRON");
		case DEMO:
			return strcpy(cadena,"DEMO");
		case DEMO_DET:
			return strcpy(cadena,"DEMO_DET");
		case DEMO_PRON:
			return strcpy(cadena,"DEMO_PRON");
		case CONTR_PREP_DEMO:
			return strcpy(cadena,"CONTR_PREP_DEMO");
		case CONTR_PREP_DEMO_DET:
			return strcpy(cadena,"CONTR_PREP_DEMO_DET");
		case CONTR_PREP_DEMO_PRON:
			return strcpy(cadena,"CONTR_PREP_DEMO_PRON");
		case CONTR_DEMO_INDEF:
			return strcpy(cadena,"CONTR_DEMO_INDEF");
		case CONTR_DEMO_INDEF_DET:
			return strcpy(cadena,"CONTR_DEMO_INDEF_DET");
		case CONTR_DEMO_INDEF_PRON:
			return strcpy(cadena,"CONTR_DEMO_INDEF_PRON");
		case CONTR_PREP_DEMO_INDEF:
			return strcpy(cadena,"CONTR_PREP_DEMO_INDEF");
		case CONTR_PREP_DEMO_INDEF_DET:
			return strcpy(cadena,"CONTR_PREP_DEMO_INDEF_DET");
		case CONTR_PREP_DEMO_INDEF_PRON:
			return strcpy(cadena,"CONTR_PREP_DEMO_INDEF_PRON");
		case POSE:
			return strcpy(cadena,"POSE");
		case POSE_DET:
			return strcpy(cadena,"POSE_DET");
		case POSE_PRON:
			return strcpy(cadena,"POSE_PRON");
		case POSE_DISTR:
			return strcpy(cadena,"POSE_DISTR");
		case NUME:
			return strcpy(cadena,"NUME");
		case NUME_DET:
			return strcpy(cadena,"NUME_DET");
		case NUME_PRON:
			return strcpy(cadena,"NUME_PRON");
		case NUME_CARDI:
			return strcpy(cadena,"NUME_CARDI");
		case NUME_CARDI_DET:
			return strcpy(cadena,"NUME_CARDI_DET");
		case NUME_CARDI_PRON:
			return strcpy(cadena,"NUME_CARDI_PRON");
		case NUME_ORDI:
			return strcpy(cadena,"NUME_ORDI");
		case NUME_ORDI_DET:
			return strcpy(cadena,"NUME_ORDI_DET");
		case NUME_ORDI_PRON:
			return strcpy(cadena,"NUME_ORDI_PRON");
		case NUME_PARTI:
			return strcpy(cadena,"NUME_PARTI");
		case NUME_PARTI_DET:
			return strcpy(cadena,"NUME_PARTI_DET");
		case NUME_PARTI_PRON:
			return strcpy(cadena,"NUME_PARTI_PRON");
		case NUME_MULTI:
			return strcpy(cadena,"NUME_MULTI");
		case NUME_COLECT:
			return strcpy(cadena,"NUME_COLECT");
		case PRON_PERS_TON:
			return strcpy(cadena,"PRON_PERS_TON");
		case CONTR_PREP_PRON_PERS_TON:
			return strcpy(cadena,"CONTR_PREP_PRON_PERS_TON");
		case PRON_PERS_AT:
			return strcpy(cadena,"PRON_PERS_AT");
		case PRON_PERS_AT_REFLEX:
			return strcpy(cadena,"PRON_PERS_AT_REFLEX");
		case PRON_PERS_AT_ACUS:
			return strcpy(cadena,"PRON_PERS_AT_ACUS");
		case PRON_PERS_AT_DAT:
			return strcpy(cadena,"PRON_PERS_AT_DAT");
		case CONTR_PRON_PERS_AT_DAT_AC:
			return strcpy(cadena,"CONTR_PRON_PERS_AT_DAT_AC");
		case CONTR_PRON_PERS_AT_DAT_DAT_AC:
			return strcpy(cadena,"CONTR_PRON_PERS_AT_DAT_DAT_AC");
		case CONTR_PRON_PERS_AT:
			return strcpy(cadena,"CONTR_PRON_PERS_AT");
		case INDET_PRON:
			return strcpy(cadena,"INDET_PRON");
		case PRON_CORREL:
			return strcpy(cadena,"PRON_CORREL");
		case ADVE:
			return strcpy(cadena,"ADVE");
		case ADVE_LUG:
			return strcpy(cadena,"ADVE_LUG");
		case ADVE_TEMP:
			return strcpy(cadena,"ADVE_TEMP");
		case ADVE_CANT:
			return strcpy(cadena,"ADVE_CANT");
		case ADVE_MODO:
			return strcpy(cadena,"ADVE_MODO");
		case ADVE_AFIRM:
			return strcpy(cadena,"ADVE_AFIRM");
		case ADVE_NEGA:
			return strcpy(cadena,"ADVE_NEGA");
		case ADVE_DUBI:
			return strcpy(cadena,"ADVE_DUBI");
		case ADVE_ESPECIFICADOR:
			return strcpy(cadena,"ADVE_ESPECIFICADOR");
		case ADVE_ORD:
			return strcpy(cadena,"ADVE_ORD");
		case ADVE_MOD:
			return strcpy(cadena,"ADVE_MOD");
		case ADVE_COMP:
			return strcpy(cadena,"ADVE_COMP");
		case ADVE_CORREL:
			return strcpy(cadena,"ADVE_CORREL");
		case ADVE_DISTR:
			return strcpy(cadena,"ADVE_DISTR");
		case LOC_ADVE:
			return strcpy(cadena,"LOC_ADVE");
		case LOC_ADVE_LUG:
			return strcpy(cadena,"LOC_ADVE_LUG");
		case LOC_ADVE_TEMP:
			return strcpy(cadena,"LOC_ADVE_TEMP");
		case LOC_ADVE_CANTI:
			return strcpy(cadena,"LOC_ADVE_CANTI");
		case LOC_ADVE_MODO:
			return strcpy(cadena,"LOC_ADVE_MODO");
		case LOC_ADVE_AFIR:
			return strcpy(cadena,"LOC_ADVE_AFIR");
		case LOC_ADVE_NEGA:
			return strcpy(cadena,"LOC_ADVE_NEGA");
		case LOC_ADVE_DUBI:
			return strcpy(cadena,"LOC_ADVE_DUBI");
		case PREP:
			return strcpy(cadena,"PREP");
		case LOC_PREP:
			return strcpy(cadena,"LOC_PREP");
		case LOC_PREP_LUG:
			return strcpy(cadena,"LOC_PREP_LUG");
		case LOC_PREP_TEMP:
			return strcpy(cadena,"LOC_PREP_TEMP");
		case LOC_PREP_CANT:
			return strcpy(cadena,"LOC_PREP_CANT");
		case LOC_PREP_MODO:
			return strcpy(cadena,"LOC_PREP_MODO");
		case LOC_PREP_CAUS:
			return strcpy(cadena,"LOC_PREP_CAUS");
		case LOC_PREP_CONDI:
			return strcpy(cadena,"LOC_PREP_CONDI");
		case CONX_COOR:
			return strcpy(cadena,"CONX_COOR");
		case CONX_COOR_COPU:
			return strcpy(cadena,"CONX_COOR_COPU");
		case CONX_COOR_DISX:
			return strcpy(cadena,"CONX_COOR_DISX");
		case CONX_COOR_ADVERS:
			return strcpy(cadena,"CONX_COOR_ADVERS");
		case CONX_COOR_DISTRIB:
			return strcpy(cadena,"CONX_COOR_DISTRIB");
		case CONTR_CONX_COOR_COP_ART_DET:
			return strcpy(cadena,"CONTR_CONX_COOR_COP_ART_DET");
		case CONX_SUBOR:
			return strcpy(cadena,"CONX_SUBOR");
		case CONX_SUBOR_PROPOR:
			return strcpy(cadena,"CONX_SUBOR_PROPOR");
		case CONX_SUBOR_FINAL:
			return strcpy(cadena,"CONX_SUBOR_FINAL");
		case CONX_SUBOR_CONTRAP:
			return strcpy(cadena,"CONX_SUBOR_CONTRAP");
		case CONX_SUBOR_CAUS:
			return strcpy(cadena,"CONX_SUBOR_CAUS");
		case CONX_SUBOR_CONCES:
			return strcpy(cadena,"CONX_SUBOR_CONCES");
		case CONX_SUBOR_CONSE:
			return strcpy(cadena,"CONX_SUBOR_CONSE");
		case CONX_SUBOR_CONDI:
			return strcpy(cadena,"CONX_SUBOR_CONDI");
		case CONX_SUBOR_COMPAR:
			return strcpy(cadena,"CONX_SUBOR_COMPAR");
		case CONX_SUBOR_LOCA:
			return strcpy(cadena,"CONX_SUBOR_LOCA");
		case CONX_SUBOR_TEMP:
			return strcpy(cadena,"CONX_SUBOR_TEMP");
		case CONX_SUBOR_MODAL:
			return strcpy(cadena,"CONX_SUBOR_MODAL");
		case CONX_SUBOR_COMPLETIVA:
			return strcpy(cadena,"CONX_SUBOR_COMPLETIVA");
		case CONX_SUBOR_CONTI:
			return strcpy(cadena,"CONX_SUBOR_CONTI");
		case LOC_CONX:
			return strcpy(cadena,"LOC_CONX");
		case LOC_CONX_COOR_COPU:
			return strcpy(cadena,"LOC_CONX_COOR_COPU");
		case LOC_CONX_COOR_ADVERS:
			return strcpy(cadena,"LOC_CONX_COOR_ADVERS");
		case LOC_CONX_SUBOR_CAUS:
			return strcpy(cadena,"LOC_CONX_SUBOR_CAUS");
		case LOC_CONX_SUBOR_CONCES:
			return strcpy(cadena,"LOC_CONX_SUBOR_CONCES");
		case LOC_CONX_SUBOR_CONSE:
			return strcpy(cadena,"LOC_CONX_SUBOR_CONSE");
		case LOC_CONX_SUBOR_COMPAR:
			return strcpy(cadena,"LOC_CONX_SUBOR_COMPAR");
		case LOC_CONX_SUBOR_CONDI:
			return strcpy(cadena,"LOC_CONX_SUBOR_CONDI");
		case LOC_CONX_SUBOR_LOCAL:
			return strcpy(cadena,"LOC_CONX_SUBOR_LOCAL");
		case LOC_CONX_SUBOR_TEMP:
			return strcpy(cadena,"LOC_CONX_SUBOR_TEMP");
		case LOC_CONX_SUBOR_MODA:
			return strcpy(cadena,"LOC_CONX_SUBOR_MODA");
		case LOC_CONX_SUBOR_CONTRAP:
			return strcpy(cadena,"LOC_CONX_SUBOR_CONTRAP");
		case LOC_CONX_SUBOR_FINAL:
			return strcpy(cadena,"LOC_CONX_SUBOR_FINAL");
		case LOC_CONX_SUBOR_PROPOR:
			return strcpy(cadena,"LOC_CONX_SUBOR_PROPOR");
		case LOC_CONX_SUBOR_CORREL:
			return strcpy(cadena,"LOC_CONX_SUBOR_CORREL");
		case RELA:
			return strcpy(cadena,"RELA");
		case INTER:
			return strcpy(cadena,"INTER");
		case EXCLA:
			return strcpy(cadena,"EXCLA");
		case NOME:
			return strcpy(cadena,"NOME");
		case LOC_SUST:
			return strcpy(cadena,"LOC_SUST");
		case NOME_PROPIO:
			return strcpy(cadena,"NOME_PROPIO");
		case ADXECTIVO:
			return strcpy(cadena,"ADXECTIVO");
		case LOC_ADXE:
			return strcpy(cadena,"LOC_ADXE");
		case VERBO:
			return strcpy(cadena,"VERBO");
		case PERIFRASE:
			return strcpy(cadena,"PERIFRASE");
		case INFINITIVO:
			return strcpy(cadena,"INFINITIVO");
		case XERUNDIO:
			return strcpy(cadena,"XERUNDIO");
		case PARTICIPIO:
			return strcpy(cadena,"PARTICIPIO");
		case INTERX:
			return strcpy(cadena,"INTERX");
		case LOC_INTERX:
			return strcpy(cadena,"LOC_INTERX");
		case LAT:
			return strcpy(cadena,"LAT");
		case LOC_LAT:
			return strcpy(cadena,"LOC_LAT");
		case NEXO_SUBORDINANTE:
			return strcpy(cadena,"NEXO_SUBORDINANTE");
		case NEXO_COORDINANTE:
			return strcpy(cadena,"NEXO_COORDINANTE");
		case SIGNO:
			return strcpy(cadena,"SIGNO");
		case PUNTO:
			return strcpy(cadena,"PUNTO");
		case COMA:
			return strcpy(cadena,"COMA");
		case PUNTO_E_COMA:
			return strcpy(cadena,"PUNTO_E_COMA");
		case DOUS_PUNTOS:
			return strcpy(cadena,"DOUS_PUNTOS");
		case PUNTOS_SUSPENSIVOS:
			return strcpy(cadena,"PUNTOS_SUSPENSIVOS");
		case APERTURA_INTERROGACION:
			return strcpy(cadena,"APERTURA_INTERROGACION");
		case PECHE_INTERROGACION:
			return strcpy(cadena,"PECHE_INTERROGACION");
		case APERTURA_EXCLAMACION:
			return strcpy(cadena,"APERTURA_EXCLAMACION");
		case PECHE_EXCLAMACION:
			return strcpy(cadena,"PECHE_EXCLAMACION");
		case APERTURA_PARENTESE:
			return strcpy(cadena,"APERTURA_PARENTESE");
		case PECHE_PARENTESE:
			return strcpy(cadena,"PECHE_PARENTESE");
		case APERTURA_CORCHETE:
			return strcpy(cadena,"APERTURA_CORCHETE");
		case PECHE_CORCHETE:
			return strcpy(cadena,"PECHE_CORCHETE");
		case GUION:
			return strcpy(cadena,"GUION");
		case DOBLES_COMINNAS:
			return strcpy(cadena,"DOBLES_COMINNAS");
		case SIMPLES_COMINNAS:
			return strcpy(cadena,"SIMPLES_COMINNAS");
		case GUION_BAIXO:
			return strcpy(cadena,"GUION_BAIXO");
		case RUPTURA_ENTONATIVA:
			return strcpy(cadena,"RUPTURA_ENTONATIVA");
		case RUPTURA_COMA:
			return strcpy(cadena,"RUPTURA_COMA");
		case CAT_NON_CLASIFICADA:
			return strcpy(cadena,"CAT_NON_CLASIFICADA");
		case SINTAGMA_NON_CLASIFICADO:
			return strcpy(cadena,"SINTAGMA_NON_CLASIFICADO");
	}
	return strcpy(cadena,"CATEGORIA_NON_CLASIFICADA");
}

/**
 * \fn traducirCategorias
 * \remarks Traduce categorías: cadena -> código.
 * \author fmendez
 * \param entrada Categoría a traducir
 * \return Código de la categoría
 */
unsigned char traducirCategorias(char *entrada) {

	if (!strcmp(entrada,"ART_DET")) {
		return ART_DET;
	}
	else if (!strcmp(entrada,"CONTR_CONX_ART_DET")) {
		return CONTR_CONX_ART_DET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_ART_DET")) {
		return CONTR_PREP_ART_DET;
	}
	else if (!strcmp(entrada,"CONTR_INDEF_ART_DET")) {
		return CONTR_INDEF_ART_DET;
	}
	else if (!strcmp(entrada,"ART_INDET")) {
		return ART_INDET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_ART_INDET")) {
		return CONTR_PREP_ART_INDET;
	}
	else if (!strcmp(entrada,"INDEF")) {
		return INDEF;
	}
	else if (!strcmp(entrada,"INDEF_PRON")) {
		return INDEF_PRON;
	}
	else if (!strcmp(entrada,"INDEF_DET")) {
		return INDEF_DET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_INDEF")) {
		return CONTR_PREP_INDEF;
	}
	else if (!strcmp(entrada,"CONTR_PREP_INDEF_DET")) {
		return CONTR_PREP_INDEF_DET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_INDEF_PRON")) {
		return CONTR_PREP_INDEF_PRON;
	}
	else if (!strcmp(entrada,"DEMO")) {
		return DEMO;
	}
	else if (!strcmp(entrada,"DEMO_DET")) {
		return DEMO_DET;
	}
	else if (!strcmp(entrada,"DEMO_PRON")) {
		return DEMO_PRON;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO")) {
		return CONTR_PREP_DEMO;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO_DET")) {
		return CONTR_PREP_DEMO_DET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO_PRON")) {
		return CONTR_PREP_DEMO_PRON;
	}
	else if (!strcmp(entrada,"CONTR_DEMO_INDEF")) {
		return CONTR_DEMO_INDEF;
	}
	else if (!strcmp(entrada,"CONTR_DEMO_INDEF_DET")) {
		return CONTR_DEMO_INDEF_DET;
	}
	else if (!strcmp(entrada,"CONTR_DEMO_INDEF_PRON")) {
		return CONTR_DEMO_INDEF_PRON;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO_INDEF")) {
		return CONTR_PREP_DEMO_INDEF;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO_INDEF_DET")) {
		return CONTR_PREP_DEMO_INDEF_DET;
	}
	else if (!strcmp(entrada,"CONTR_PREP_DEMO_INDEF_PRON")) {
		return CONTR_PREP_DEMO_INDEF_PRON;
	}
	else if (!strcmp(entrada,"POSE")) {
		return POSE;
	}
	else if (!strcmp(entrada,"POSE_DET")) {
		return POSE_DET;
	}
	else if (!strcmp(entrada,"POSE_PRON")) {
		return POSE_PRON;
	}
	else if (!strcmp(entrada,"POSE_DISTR")) {
		return POSE_DISTR;
	}
	else if (!strcmp(entrada,"NUME")) {
		return NUME;
	}
	else if (!strcmp(entrada,"NUME_DET")) {
		return NUME_DET;
	}
	else if (!strcmp(entrada,"NUME_PRON")) {
		return NUME_PRON;
	}
	else if (!strcmp(entrada,"NUME_CARDI")) {
		return NUME_CARDI;
	}
	else if (!strcmp(entrada,"NUME_CARDI_DET")) {
		return NUME_CARDI_DET;
	}
	else if (!strcmp(entrada,"NUME_CARDI_PRON")) {
		return NUME_CARDI_PRON;
	}
	else if (!strcmp(entrada,"NUME_ORDI")) {
		return NUME_ORDI;
	}
	else if (!strcmp(entrada,"NUME_ORDI_DET")) {
		return NUME_ORDI_DET;
	}
	else if (!strcmp(entrada,"NUME_ORDI_PRON")) {
		return NUME_ORDI_PRON;
	}
	else if (!strcmp(entrada,"NUME_PARTI")) {
		return NUME_PARTI;
	}
	else if (!strcmp(entrada,"NUME_PARTI_DET")) {
		return NUME_PARTI_DET;
	}
	else if (!strcmp(entrada,"NUME_PARTI_PRON")) {
		return NUME_PARTI_PRON;
	}
	else if (!strcmp(entrada,"NUME_MULTI")) {
		return NUME_MULTI;
	}
	else if (!strcmp(entrada,"NUME_COLECT")) {
		return NUME_COLECT;
	}
	else if (!strcmp(entrada,"PRON_PERS_TON")) {
		return PRON_PERS_TON;
	}
	else if (!strcmp(entrada,"CONTR_PREP_PRON_PERS_TON")) {
		return CONTR_PREP_PRON_PERS_TON;
	}
	else if (!strcmp(entrada,"PRON_PERS_AT")) {
		return PRON_PERS_AT;
	}
	else if (!strcmp(entrada,"PRON_PERS_AT_REFLEX")) {
		return PRON_PERS_AT_REFLEX;
	}
	else if (!strcmp(entrada,"PRON_PERS_AT_ACUS")) {
		return PRON_PERS_AT_ACUS;
	}
	else if (!strcmp(entrada,"PRON_PERS_AT_DAT")) {
		return PRON_PERS_AT_DAT;
	}
	else if (!strcmp(entrada,"CONTR_PRON_PERS_AT_DAT_AC")) {
		return CONTR_PRON_PERS_AT_DAT_AC;
	}
	else if (!strcmp(entrada,"CONTR_PRON_PERS_AT_DAT_DAT_AC")) {
		return CONTR_PRON_PERS_AT_DAT_DAT_AC;
	}
	else if (!strcmp(entrada,"CONTR_PRON_PERS_AT")) {
		return CONTR_PRON_PERS_AT;
	}
	else if (!strcmp(entrada,"INDET_PRON")) {
		return INDET_PRON;
	}
	else if (!strcmp(entrada,"PRON_CORREL")) {
		return PRON_CORREL;
	}
	else if (!strcmp(entrada,"ADVE")) {
		return ADVE;
	}
	else if (!strcmp(entrada,"ADVE_LUG")) {
		return ADVE_LUG;
	}
	else if (!strcmp(entrada,"ADVE_TEMP")) {
		return ADVE_TEMP;
	}
	else if (!strcmp(entrada,"ADVE_CANT")) {
		return ADVE_CANT;
	}
	else if (!strcmp(entrada,"ADVE_MODO")) {
		return ADVE_MODO;
	}
	else if (!strcmp(entrada,"ADVE_AFIRM")) {
		return ADVE_AFIRM;
	}
	else if (!strcmp(entrada,"ADVE_NEGA")) {
		return ADVE_NEGA;
	}
	else if (!strcmp(entrada,"ADVE_DUBI")) {
		return ADVE_DUBI;
	}
	else if (!strcmp(entrada,"ADVE_ESPECIFICADOR")) {
		return ADVE_ESPECIFICADOR;
	}
	else if (!strcmp(entrada,"ADVE_ORD")) {
		return ADVE_ORD;
	}
	else if (!strcmp(entrada,"ADVE_MOD")) {
		return ADVE_MOD;
	}
	else if (!strcmp(entrada,"ADVE_COMP")) {
		return ADVE_COMP;
	}
	else if (!strcmp(entrada,"ADVE_CORREL")) {
		return ADVE_CORREL;
	}
	else if (!strcmp(entrada,"ADVE_DISTR")) {
		return ADVE_DISTR;
	}
	else if (!strcmp(entrada,"PREP")) {
		return PREP;
	}
	else if (!strcmp(entrada,"CONX_COOR")) {
		return CONX_COOR;
	}
	else if (!strcmp(entrada,"CONX_COOR_COPU")) {
		return CONX_COOR_COPU;
	}
	else if (!strcmp(entrada,"CONX_COOR_DISX")) {
		return CONX_COOR_DISX;
	}
	else if (!strcmp(entrada,"CONX_COOR_ADVERS")) {
		return CONX_COOR_ADVERS;
	}
	else if (!strcmp(entrada,"CONX_COOR_DISTRIB")) {
		return CONX_COOR_DISTRIB;
	}
	else if (!strcmp(entrada,"CONTR_CONX_COOR_COP_ART_DET")) {
		return CONTR_CONX_COOR_COP_ART_DET;
	}
	else if (!strcmp(entrada,"CONX_SUBOR")) {
		return CONX_SUBOR;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_PROPOR")) {
		return CONX_SUBOR_PROPOR;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_FINAL")) {
		return CONX_SUBOR_FINAL;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CONTRAP")) {
		return CONX_SUBOR_CONTRAP;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CAUS")) {
		return CONX_SUBOR_CAUS;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CONCES")) {
		return CONX_SUBOR_CONCES;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CONSE")) {
		return CONX_SUBOR_CONSE;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CONDI")) {
		return CONX_SUBOR_CONDI;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_COMPAR")) {
		return CONX_SUBOR_COMPAR;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_LOCA")) {
		return CONX_SUBOR_LOCA;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_TEMP")) {
		return CONX_SUBOR_TEMP;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_MODAL")) {
		return CONX_SUBOR_MODAL;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_COMPLETIVA")) {
		return CONX_SUBOR_COMPLETIVA;
	}
	else if (!strcmp(entrada,"CONX_SUBOR_CONTI")) {
		return CONX_SUBOR_CONTI;
	}
	else if (!strcmp(entrada,"RELA")) {
		return RELA;
	}
	else if (!strcmp(entrada,"INTER")) {
		return INTER;
	}
	else if (!strcmp(entrada,"EXCLA")) {
		return EXCLA;
	}
	else if (!strcmp(entrada,"NOME")) {
		return NOME;
	}
	else if (!strcmp(entrada,"NOME_PROPIO")) {
		return NOME_PROPIO;
	}
	else if (!strcmp(entrada,"ADXECTIVO")) {
		return ADXECTIVO;
	}
	else if (!strcmp(entrada,"VERBO")) {
		return VERBO;
	}
	else if (!strcmp(entrada,"PERIFRASE")) {
		return PERIFRASE;
	}
	else if (!strcmp(entrada,"INFINITIVO")) {
		return INFINITIVO;
	}
	else if (!strcmp(entrada,"XERUNDIO")) {
		return XERUNDIO;
	}
	else if (!strcmp(entrada,"PARTICIPIO")) {
		return PARTICIPIO;
	}
	else if (!strcmp(entrada,"INTERX")) {
		return INTERX;
	}
	else if (!strcmp(entrada,"LAT")) {
		return LAT;
	}
	else if (!strcmp(entrada,"NEXO_SUBORDINANTE")) {
		return NEXO_SUBORDINANTE;
	}
	else if (!strcmp(entrada,"NEXO_COORDINANTE")) {
		return NEXO_COORDINANTE;
	}
	else if (!strcmp(entrada,"SIGNO")) {
		return SIGNO;
	}
	else if (!strcmp(entrada,"PUNTO")) {
		return PUNTO;
	}
	else if (!strcmp(entrada,"COMA")) {
		return COMA;
	}
	else if (!strcmp(entrada,"PUNTO_E_COMA")) {
		return PUNTO_E_COMA;
	}
	else if (!strcmp(entrada,"DOUS_PUNTOS")) {
		return DOUS_PUNTOS;
	}
	else if (!strcmp(entrada,"PUNTOS_SUSPENSIVOS")) {
		return PUNTOS_SUSPENSIVOS;
	}
	else if (!strcmp(entrada,"APERTURA_INTERROGACION")) {
		return APERTURA_INTERROGACION;
	}
	else if (!strcmp(entrada,"PECHE_INTERROGACION")) {
		return PECHE_INTERROGACION;
	}
	else if (!strcmp(entrada,"APERTURA_EXCLAMACION")) {
		return APERTURA_EXCLAMACION;
	}
	else if (!strcmp(entrada,"PECHE_EXCLAMACION")) {
		return PECHE_EXCLAMACION;
	}
	else if (!strcmp(entrada,"APERTURA_PARENTESE")) {
		return APERTURA_PARENTESE;
	}
	else if (!strcmp(entrada,"PECHE_PARENTESE")) {
		return PECHE_PARENTESE;
	}
	else if (!strcmp(entrada,"APERTURA_CORCHETE")) {
		return APERTURA_CORCHETE;
	}
	else if (!strcmp(entrada,"PECHE_CORCHETE")) {
		return PECHE_CORCHETE;
	}
	else if (!strcmp(entrada,"GUION")) {
		return GUION;
	}
	else if (!strcmp(entrada,"DOBLES_COMINNAS")) {
		return DOBLES_COMINNAS;
	}
	else if (!strcmp(entrada,"SIMPLES_COMINNAS")) {
		return SIMPLES_COMINNAS;
	}
	else if (!strcmp(entrada,"GUION_BAIXO")) {
		return GUION_BAIXO;
	}
	else if (!strcmp(entrada,"RUPTURA_ENTONATIVA")) {
		return RUPTURA_ENTONATIVA;
	}
	else if (!strcmp(entrada,"RUPTURA_COMA")) {
		return RUPTURA_COMA;
	}
	else if (!strcmp(entrada,"CAT_NON_CLASIFICADA")) {
		return CAT_NON_CLASIFICADA;
	}
	else if (!strcmp(entrada,"SINTAGMA_NON_CLASIFICADO")) {
		return SINTAGMA_NON_CLASIFICADO;
	}
	else {
		return traducirCategoriasLocuciones(entrada);
	}
}

/**
 * \fn traducirCategoriasLocuciones
 * \remarks Traduce categorías: cadena -> código. Procesa las locuciones
 * \author fmendez
 * \param entrada Categoría a traducir
 * \return Código de la categoría
 */
unsigned char traducirCategoriasLocuciones(char *entrada) {

	if (!strcmp(entrada,"LOC_ADVE")) {
		return LOC_ADVE;
	}
	else if (!strcmp(entrada,"LOC_ADVE_LUG")) {
		return LOC_ADVE_LUG;
	}
	else if (!strcmp(entrada,"LOC_ADVE_TEMP")) {
		return LOC_ADVE_TEMP;
	}
	else if (!strcmp(entrada,"LOC_ADVE_CANTI")) {
		return LOC_ADVE_CANTI;
	}
	else if (!strcmp(entrada,"LOC_ADVE_MODO")) {
		return LOC_ADVE_MODO;
	}
	else if (!strcmp(entrada,"LOC_ADVE_AFIR")) {
		return LOC_ADVE_AFIR;
	}
	else if (!strcmp(entrada,"LOC_ADVE_NEGA")) {
		return LOC_ADVE_NEGA;
	}
	else if (!strcmp(entrada,"LOC_ADVE_DUBI")) {
		return LOC_ADVE_DUBI;
	}
	else if (!strcmp(entrada,"LOC_PREP")) {
		return LOC_PREP;
	}
	else if (!strcmp(entrada,"LOC_PREP_LUG")) {
		return LOC_PREP_LUG;
	}
	else if (!strcmp(entrada,"LOC_PREP_TEMP")) {
		return LOC_PREP_TEMP;
	}
	else if (!strcmp(entrada,"LOC_PREP_CANT")) {
		return LOC_PREP_CANT;
	}
	else if (!strcmp(entrada,"LOC_PREP_MODO")) {
		return LOC_PREP_MODO;
	}
	else if (!strcmp(entrada,"LOC_PREP_CAUS")) {
		return LOC_PREP_CAUS;
	}
	else if (!strcmp(entrada,"LOC_PREP_CONDI")) {
		return LOC_PREP_CONDI;
	}
	else if (!strcmp(entrada,"LOC_CONX")) {
		return LOC_CONX;
	}
	else if (!strcmp(entrada,"LOC_CONX_COOR_COPU")) {
		return LOC_CONX_COOR_COPU;
	}
	else if (!strcmp(entrada,"LOC_CONX_COOR_ADVERS")) {
		return LOC_CONX_COOR_ADVERS;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CAUS")) {
		return LOC_CONX_SUBOR_CAUS;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CONCES")) {
		return LOC_CONX_SUBOR_CONCES;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CONSE")) {
		return LOC_CONX_SUBOR_CONSE;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_COMPAR")) {
		return LOC_CONX_SUBOR_COMPAR;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CONDI")) {
		return LOC_CONX_SUBOR_CONDI;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_LOCAL")) {
		return LOC_CONX_SUBOR_LOCAL;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_TEMP")) {
		return LOC_CONX_SUBOR_TEMP;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_MODA")) {
		return LOC_CONX_SUBOR_MODA;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CONTRAP")) {
		return LOC_CONX_SUBOR_CONTRAP;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_FINAL")) {
		return LOC_CONX_SUBOR_FINAL;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_PROPOR")) {
		return LOC_CONX_SUBOR_PROPOR;
	}
	else if (!strcmp(entrada,"LOC_CONX_SUBOR_CORREL")) {
		return LOC_CONX_SUBOR_CORREL;
	}
	else if (!strcmp(entrada,"LOC_SUST")) {
		return LOC_SUST;
	}
	else if (!strcmp(entrada,"LOC_ADXE")) {
		return LOC_ADXE;
	}
	else if (!strcmp(entrada,"LOC_INTERX")) {
		return LOC_INTERX;
	}
	else if (!strcmp(entrada,"LOC_LAT")) {
		return LOC_LAT;
	}
	else {
		fprintf(stderr, "traducirCategoriasLocuciones: Categoría inválida: %s\n", entrada);
		return 0;
	}
}

