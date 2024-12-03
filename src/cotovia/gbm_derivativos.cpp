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
 * \file gbm_derivativos.cpp
 * \author giglesia
 * \remarks Desarrolla los métodos de la clase gbm_regexp.
 */

#include <cotovia2eagles.hpp>
#include <interfaz_ficheros.hpp>
#include <gbm_derivativos.hpp>

/**
 * \author giglesia
 * \remarks Método sobrecargado de gbm::procesa. Realiza el procesado específico
 * para un fichero de regexps, a partir del procesado genérico de la clase
 * padre.
 * \param nombre: el nombre del fichero a procesar.
 */
void gbm_regexp::procesa(char *nombre) {

	int k,m;
	int lineaProcesada=-1;
	int comentario=0;
	int subk;
	char *x;



	if (!cargaFichero(nombre)) return;
	gbm::procesa(3,-1);
	tamanio_estructura_lista=sizeof(t_regexp);
	lista=rgp= (t_regexp *) reservaMemoria(tamanio*tamanio_estructura_lista);
	if (lista==NULL) {
		cargado=-1;
		errorGeneralIF=1;
		return;
	}

	for (k=0,m=1;k<parametros;k++){
		if (resultado[k]!=NULL) {
			if (comentario) continue;
			if (*resultado[k]=='@') continue;
			if (*resultado[k]=='#' && m==1) { //Línea comentada.
				comentario=1;
				continue;
			}
			if (*resultado[k]) {
				if (m==1) rgp[++lineaProcesada].regexp=resultado[k]; //Parametro 1
				if (m==2) {
					rgp[lineaProcesada].tp[0]=resultado[k]; //Parametro 2
					for (subk=1,x=&(resultado[k][1]);*x!=0 && subk<3; x++)
						if (*x==';'){
							*x=0;
							rgp[lineaProcesada].tp[subk++]=x+1;
						}
					for (int g=subk;g<3;rgp[lineaProcesada].tp[g++]=NULL);            
				}
				if (m==3) {
					rgp[lineaProcesada].td[0]=resultado[k]; //Parametro 3
					for (subk=1,x=&(resultado[k][1]);*x!=0 && subk<3; x++)
						if (*x==';'){
							*x=0;
							rgp[lineaProcesada].td[subk++]=x+1;
						}
					for (int g=subk;g<3;rgp[lineaProcesada].td[g++]=NULL); 
				}
				if (m>=4) strcpy(rgp[lineaProcesada].info[m-4],resultado[k]);   //Parametro 4 y posteriores
				m++;
			}

		}
		else {
			if (comentario) comentario=0;
			m=1;
		}
	}
	numero_nodos_lista=lineaProcesada+1;
}

/**
 * \author giglesia
 * \remarks Constructor que inicializa la clase con el nombre del
 * fichero a procesar.
 * \param nombre: el nombre del fichero a procesar.
 */
gbm_regexp::gbm_regexp(char *nombre, gbm **dic){

	procesa(nombre);
	this->diccionario=dic;
	//Empieza el registro de diccionarios y ciertas constantes.

	escoge['S']=IF_DIC_N;
	categoria['S']=NOME;
	escoge['X']=IF_DIC_N;
	categoria['X']=ADXECTIVO;
	escoge['V']=IF_DIC_V;
	categoria['V']=VERBO;
	categoria['B']=ADVE_MODO;

	genero['M']=MASCULINO;
	genero['F']=FEMININO;
	genero['N']=NEUTRO;  
	genero['A']=AMBIGUO;
	genero['O']=NO_ASIGNADO;  //Perfecto, el 0 es no_asignado.
	numero['S']=SINGULAR;
	numero['P']=PLURAL;
	numero['A']=AMBIGUO;

	//  persona['1']=
	//    persona['2']=
	//    persona['3']=

}

/** 
 * \author giglesia
 * \remarks 
 */
gbm_regexp::gbm_regexp() {}

int variantes(char *palabra,char *palabra2,int kk);

/** 
 * \author giglesia
 * \remarks 
 * 
 * \param palabra 
 * \param palabra2 
 * \param kk 
 * 
 * \return 
 */
int variantes(char *palabra,char *palabra2,int kk) {

	strcpy (palabra2,palabra);
	switch (palabra[kk]) {
		case 'a':
			palabra2[kk]='á';
			break;
		case 'e':
			palabra2[kk]='é';
			break;
		case 'i':
			palabra2[kk]='í';
			break;
		case 'o':
			palabra2[kk]='ó';
			break;
		case 'u':
			palabra2[kk]='ú';
			break;
		case 'á':
			palabra2[kk]='a';
			break;
		case 'é':
			palabra2[kk]='e';
			break;
		case 'í':
			palabra2[kk]='i';
			break;
		case 'ó':
			palabra2[kk]='o';
			break;
		case 'ú':
			palabra2[kk]='u';
			break;
		default:
			break;
	}
}

/** 
 * \author giglesia
 * \remarks 
 * 
 * \param nombre 
 * \param frase_separada 
 * 
 * \return 
 */
int gbm_regexp::buscar(char *nombre,t_frase_separada *frase_separada) {

//	char yipicallei[100];

	int candi=0;
	int indice=0;
	char auxiliar[LONX_MAX_FRASE_SEPARADA];
	char auxiliar2[LONX_MAX_FRASE_SEPARADA];
	char candiAuxiliar[LONX_MAX_FRASE_SEPARADA];
	//	int posicion=-1;
//	t_frase_separada *frase_auxiliar=NULL;
	t_frase_separada frase2;
	memset(&frase2, 0, sizeof(t_frase_separada));

	*frase2.cat_gramatical = 0;

	char yipi[LONX_MAX_FRASE_SEPARADA];
	char anterior[LONX_MAX_FRASE_SEPARADA]="";

	strcpy(auxiliar,nombre);
	indice=busca(nombre,1);  //Búsqueda inversa por patrón.
	if (indice==-1) return -1;
	int dp=strlen(nombre)-strlen(rgp[indice].regexp);
  if (dp>1)auxiliar[dp]=0; //Resuelve fallos, pero fallarían palabras como "peciño".
  else return -1;
	if (dp>0)auxiliar[dp]=0;
	//	int longitud;
	int td;
	int cat;
	int kk;
	int largo;
  bool auxi;

  int auxilargo;


  CotoviaEagles *oCE=CotoviaEagles::getSingleton();


	for (int yuk=0;rgp[indice].td[yuk]!=NULL;yuk++) {

		td=escoge[rgp[indice].td[yuk][0]];
		cat=categoria[rgp[indice].tp[yuk][0]];
    if (!cat) {
     fprintf(stderr,"Posible error de formato en diccionario derivativos, línea %d",indice);
     return 1;
    }
		if (esta_palabra_ten_a_categoria_de(cat, frase_separada)) return 1;
		for (int k=0;k<IF_REGEXP && rgp[indice].info[k][0]!=0;k++) {
			candidatos[candi][0]=(char) 0;
			strcpy(candiAuxiliar,auxiliar);
			//	  	longitud=strlen(candiAuxiliar);
			if (strcmp(rgp[indice].info[k],"%")) {
				strcat(candiAuxiliar,rgp[indice].info[k]);
			}

			//ZA: Ojito con esto, tener en cuenta si el diccionario es NULL.
			/*
				 if (rgp[indice].tp[yuk][0]=='=') frase_auxiliar=frase_separada;
				 else frase_auxiliar=NULL;
				 */
			//    int kk_aux=kk=strlen(candiAuxiliar)-1;
			int kk_aux=kk=dp-1;  //El juego de tildes, solo con el lexema.
			strcpy(yipi,candiAuxiliar);
//      if (td==2) continue; //chapuza para evitar carga de adjetivos.
			do {
				if (strcmp(anterior,yipi) && (kk==kk_aux || strcmp(candiAuxiliar,yipi)) ) {
					strcpy (anterior,yipi);
//					strcpy(yipicallei,yipi);

            if (diccionario[td]->buscar(yipi,&frase2)>=0){
            auxi=false;
            for (int kpt=0;frase2.cat_gramatical[kpt]!=0;kpt++){
            auxilargo=strlen(rgp[indice].td[yuk]);
            if (auxilargo>1) auxilargo=2;

              if (!strncmp(oCE->mGetC2E(frase2.cat_gramatical[kpt]).c_str(),rgp[indice].td[yuk],auxilargo)){
                //if (kpt>0) frase2.cat_gramatical[0]=frase2.cat_gramatical[kpt];
                frase2.cat_gramatical[0]=oCE->mGetE2C(rgp[indice].tp[yuk]);
                frase2.cat_gramatical[1]=0;
                auxi=true;
                break;
              }
            }
            if (!auxi) break;
						strcpy (candidatos[candi++],yipi);
						//printf("\nkk=%d::yuk=%d::CANDIDATO %d:%s para %s en diccionario %c",kk,yuk,k,candidatos[candi-1],nombre,rgp[indice].td[yuk][0]);
						//De momento... A saco.
						candidatos[candi][0]=0;
						// Comprobamos que género y número sean los esperados.
						largo=strlen(rgp[indice].td[yuk]);
						if (largo>2 && rgp[indice].td[yuk][2]!='=') {
							if (frase2.xenero!=genero[rgp[indice].td[yuk][2]]
									&& frase2.xenero!=AMBIGUO) {
								variantes(candiAuxiliar,yipi,kk--);
								continue;
							}
						}
						if (largo>3 && rgp[indice].td[yuk][3]!='=') {
							if (frase2.numero!=numero[rgp[indice].td[yuk][3]]
									&& frase2.numero!=AMBIGUO) {
								variantes(candiAuxiliar,yipi,kk--);
								continue;
							}
						}
						//printf("-->OK!");
						strcpy(frase_separada->lema,diccionario[td]->devuelveLema());
						//strcat(frase_separada->lema, "*");
						anade_categoria(cat,frase_separada);
						// } cierre de llave comentado 
						largo=strlen(rgp[indice].tp[yuk]);
						if (largo>2)
							if (rgp[indice].tp[yuk][2]!='=') frase_separada->xenero=genero[rgp[indice].tp[yuk][2]];
							else frase_separada->xenero= frase2.xenero;  //Se le asigna lo que dio la búsqueda.
						if (largo>3)
								if (rgp[indice].tp[yuk][3]!='=') frase_separada->numero=numero[rgp[indice].tp[yuk][3]];
								else frase_separada->numero=frase2.numero;
								//Y luego, persona.

								//ZA. Pensamiento: igual lo más lógico es seguir...
						continue;
					}
				}
				//Puede que el problema esté en alguna tilde perdida...
				//Se hace una comprobación bruta de esto.

				variantes(candiAuxiliar,yipi,kk--);
			} while (kk>=0);
		}
	}

	//¿Toma de decisiones?
	//
	strcpy(frase_separada->raiz, auxiliar);
	strcpy(frase_separada->sufijo,rgp[indice].regexp); 
	invertir_cadea2(frase_separada->sufijo);
	return 1;
}

/** 
 * \author giglesia
 * \remarks
 * 
 * \return 
 */
char **gbm_regexp::devuelveCandidatos(void) {
	return (char **)candidatos;
}

/** 
 * \author giglesia
 * \remarks 
 * 
 * \param patron 
 * \param tipo_busqueda 
 * 
 * \return 
 */
int gbm_regexp::busca(char *patron,char tipo_busqueda) {
	return posicionBusqueda=(cargado)? comprobar_en_lista_de_inicio_de_palabras3( (char **) lista,tamanio_estructura_lista,numero_nodos_lista,patron,tipo_busqueda):-1;
}

