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
* \author giglesia
* \remarks Desarrolla los métodos de la clase gbm_locuciones.
*/
#include "modos.hpp"
#include "tip_var.hpp"
#include "gbm_locuciones.hpp"

/**
* \author giglesia
* \remarks Método sobrecargado de gbm::procesa. Realiza el procesado específico
* para un fichero de locuciones, a partir del procesado genérico de la clase
* padre.
* \param nombre: el nombre del fichero a procesar.
*/
void gbm_locuciones::procesa(char *nombre) {

  int k,m;//,n=0;
  //int p=0;
  //char *aux;
  int lineaProcesada=-1;
  int comentario=0;
  //char viejo[100]="";
  t_locucion *loc;




  if (!cargaFichero(nombre)) return;
  if (!tamanio) return;
  gbm::procesa(2,-1);
  tamanio_estructura_lista=sizeof(t_locucion);
  lista=loc= (t_locucion *) reservaMemoria(tamanio*tamanio_estructura_lista/2);
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
                    if (m==1) {
                            //aux=loc[++lineaProcesada].palabras=resultado[k]; //Parametro 1
                            loc[++lineaProcesada].palabras=resultado[k]; //Parametro 1
                            /*for (n=1;*aux!=0;aux++){
                                    if (*aux==' ') n++;
                            }
                            loc[lineaProcesada].num_pal_conxuncion=(char) n; */
                    }
                    else if (m==2) {
                            loc[lineaProcesada].num_pal_conxuncion=(unsigned char)atoi(resultado[k]);
                    }
                    else if (m==3) {
                            loc[lineaProcesada].tonicidad=resultado[k];
                    }
//                    else if (m==4){
//                            loc[lineaProcesada].tipo_conxuncion=traducirCategorias(resultado[k]);
//                    }
                    else {
                            loc[lineaProcesada].tipo_conxuncion[m-4]=traducirCategorias(resultado[k]);
                    }

//Esto es para la versión siguiente.
    /*
                            if (*resultado[k]>=65 && *resultado[k]<=92) {
                                 loc[lineaProcesada].t_c[n][p++]=resultado[k];   //Parametro 2 y siguientes
                                 if (m==2) loc[lineaProcesada].preps[0]=NULL;
                            }
                            else {
                                    p=0;
                                    if (m>2) n++;
                                    loc[lineaProcesada].preps[n]=resultado[k];
                            }

    */
                //    }
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
gbm_locuciones::gbm_locuciones(char *nombre){

        procesa(nombre);
}

/**
* \author giglesia
* \remarks Método sobrecargado de gbm:busca, adaptado para buscar patrones en locuciones. Se ofrece
* un patrón para que el resultado quede en una expresión razonablemente parecida.
* \param patron: candidato a locución.
*/
int gbm_locuciones::busca(char * patron) {

        return (cargado)? comprobar_en_lista_de_palabras3( (char **) lista,tamanio_estructura_lista,numero_nodos_lista,patron):NULL;
}
/**
* \author giglesia
* \remarks Constructor que inicializa una instancia vacía.
*/
gbm_locuciones::gbm_locuciones() {}


/**
 * \author giglesia
 * \remarks Método público sobrecargado de gbm:busca, adaptado para detectar si es locución
 * y asignar las categorías correspondientes a la estructura. El problema de la
 * detección de locuciones a la que una expresión se puede asociar se divide en
 * pequeños problemas, cada uno con su método protegido asociado para el uso
 * interno de este método que carecen por tanto de significado para el exterior
 * fuera de la utilidad que suponen aquí. Para el exterior, el que resuelve el
 * problema es este.
 * \param frase_separada: candidato a locución con todos los datos asociados.
 * \retval frase_separada: de ser alguna locución, se asignarán internamente estas
 * categorías. Ver la estructura t_frase_separada para más detalles.
 */
int gbm_locuciones::busca(t_frase_separada *frase_separada) {

	//t_conxuncion *conx2,*conx3,*conx4;
	t_locucion *conx2,*conx3,*conx4;
	int k;
	int cont_aux;


	/*formar as posibles frases cas 1, 2,  e tres palabras_seguintes*/


	if (frase_separada==NULL)  return 0;
	if (frase_separada->pal_transformada!=NULL)
		strcpy(frase[0],frase_separada->pal_transformada);
	else
		strcpy(frase[0],frase_separada->pal);
	/* Necesario por se a primeira palabra esta en maiuscula e por tanto en
		 minuscula esta no campo pal_transformada*/



	for (k=1;k<N_MAX_PALABRAS && (frase_separada+k)!=0;k++){
		sprintf(frase[k],"%s %s",frase[k-1],(frase_separada +k)->pal);
	//	strcpy(frase[k],frase[k-1]);
	//	strcat(frase[k]," ");
	//	strcat(frase[k],(frase_separada+k)->pal);
	}


	//ZA: nuevo sistema de locuciones.

	conx2=conx3=conx4=NULL;

	if (buscarConxunciones(frase[N_MAX_PALABRAS-1]))
		for (k=N_MAX_PALABRAS-1;k>=0 && conx2==NULL;k--){
			conx2=refinaBusquedaLoc(frase[k]);
		}
	while (conx2!=NULL){
		if (buscarLocEspeciales(frase_separada,conx2,frase[(k==N_MAX_PALABRAS-1)?k:k+1])){
			for(cont_aux=0;cont_aux<conx2->num_pal_conxuncion;cont_aux++) {
				(frase_separada+cont_aux)->tipo_sintagma=conx2->tipo_conxuncion[0];

				//anade_categoria(conx2->tipo_conxuncion,frase_separada+cont_aux);
				//asigna_categoria(conx2->tipo_conxuncion[0],frase_separada+cont_aux);
				anade_categoria(conx2->tipo_conxuncion[0],frase_separada+cont_aux);
				unsigned char *kk=&(conx2->tipo_conxuncion[1]);
				while (*kk) {
					anade_categoria(*kk,frase_separada+cont_aux);
					kk++;
				}
				(frase_separada+cont_aux)->tonicidad_forzada=(unsigned char)(conx2->tonicidad[cont_aux]-48);
			}
			conx4=conx2;
		}
		conx3=conx2;
		if ((conx2=refinaBusquedaLoc(frase[(k==N_MAX_PALABRAS-1)?k:k+1]))==conx3) break;
		if (conx4!=NULL && conx2!=NULL)
			if (conx3==conx4 && conx2->num_pal_conxuncion<conx3->num_pal_conxuncion) break;
	}

	return conx4==NULL? 1: conx4->num_pal_conxuncion;
}

/**
* \author giglesia
* \remarks Método interno para refinar una búsqueda intentando detectar si hay
* posibles contracciones que oculten la locución a una inspección directa.
* \param frase_separada: candidato a locución con todos los datos asociados.
*  \return 0 si no encuentra nada, 1 si detecta.
*/
int gbm_locuciones::buscarLocEspeciales(t_frase_separada *frase_separada,t_locucion *conxuncion, char *posibleLoc){


        char auxi[50];


      if (!*conxuncion->palabras) return 0;
      strcpy(&auxi[0],posibleLoc);
      if (
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_DET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_ART_INDET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_DET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_INDEF_PRON,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_DET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_PRON,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_DET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_DEMO_INDEF_PRON,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_PREP_PRON_PERS_TON,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(CONTR_CONX_COOR_COP_ART_DET,frase_separada+conxuncion->num_pal_conxuncion-1) ||
                esta_palabra_ten_a_categoria_de(PREP,frase_separada+conxuncion->num_pal_conxuncion-1) ) {


                switch((frase_separada+conxuncion->num_pal_conxuncion-1)->pal[0]){

                case 'd':
                        strcat(auxi," de");
                        break;
                case 'a':
                case 'á':
                case 'ó':
                        strcat(auxi," a");
                        break;
                case 'c':
                        strcat(auxi," con");
                        break;
                case 'm':
                        strcat(auxi," mais");
                        break;
                }
        }
        if ((strcmp(conxuncion->palabras,auxi))==0) return 1;
        strcpy(&auxi[0],posibleLoc);
        if ((strcmp(conxuncion->palabras,auxi))==0) return 1;
        return 0;

}

/**
* \author giglesia
* \remarks Otro refinado de búsqueda serie, usado internamente en el método público
* busca(t_frase_separada).
* \param patron: patrón de la posible locución.
*/
void *gbm_locuciones::buscarConxunciones2(char *patron) {

        int yupi;
        t_locucion *cosa;

//        resultadosPrimero=resultadosUltimo=resultadosMedio=NULL;

//ZA        yupi=if_locuciones->busca(patron);
        yupi=busca(patron);
//ZA        cosa=(t_locucion *) if_locuciones->asignaPuntero();
        cosa=(t_locucion *) asignaPuntero();
        if (cosa==NULL) return NULL;
        yupi-=(yupi>10)? 10:0;    //Le resto 10 para que busque a saco.
        return (void *) &cosa[yupi];
}

/**
* \author giglesia
* \remarks Método interno de aproximación rápida y directa al área donde podría encontrarse la locución.
* \param patron: patrón de posible locución.
* \return 0 si no encuentra nada, 1 si detecta.
*/
int gbm_locuciones::buscarConxunciones(char* patron){

        int algoHay=0;
        t_locucion *aux;
        char patr2[4*LONX_MAX_PALABRA_PROCESADA], *punt;

        if ((aux=(t_locucion *) buscarConxunciones2(patron))==NULL) return 0;

        strcpy(patr2,patron);
        for (punt=patr2;*punt!=0;punt++)
                if (*punt==' ') *punt=0;


        if (*patron==' ' || *patron==0) return 0;
//        if (loc_ini==NULL) return 0;

        strcpy(patr,patron);
        resultadosPrimero=resultadosUltimo=resultadosMedio=NULL;

        while(aux->num_pal_conxuncion!=0 ){
                if ((strncmp(aux->palabras,patron, strlen(aux->palabras)/2))==0) {
                        algoHay=1;
                        resultadosPrimero=resultadosUltimo=resultadosMedio=aux++;
                        break;
                }
                aux++;
        }
        if (!algoHay) return 0; //No se encontraron coincidencias.

        while(aux->num_pal_conxuncion!=0){
                if ((strncmp(aux->palabras,patron,strlen(aux->palabras)/2))==0){
                        resultadosMedio=resultadosUltimo=aux;
                }
                if (*aux->palabras!= *patron) break;
                aux++;
        }
        return 1;
}

/**
* \author giglesia
* \remarks Refinado mediante búsqueda serie en un ámbito reducido. Útil
* sólo para el método público de búsqueda de locuciones. 
* \return 0 si no encuentra nada, 1 si detecta.
*/

t_locucion *gbm_locuciones::refinaBusquedaLoc(char * patron){

        int encontrado=0;
        t_locucion *aux;
        if (resultadosPrimero==NULL) return NULL;

        if (strcmp(patr,patron)) {
                resultadosMedio=resultadosUltimo;
                strcpy(patr,patron);
        }

        aux=resultadosMedio;
        do {
                if (!strncmp(aux->palabras,patron, strlen(patron))){
                        encontrado=1;
                        break;
                }
                if (!strcmp(aux->palabras,resultadosPrimero->palabras)) break;
                aux--;
        }
        while(!encontrado);
        resultadosMedio=aux;
        if (resultadosMedio!=resultadosPrimero) resultadosMedio--;
        if (encontrado) return aux;
        return NULL;
}

