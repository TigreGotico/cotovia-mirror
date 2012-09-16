/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ram�n Pi�eiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigaci�n en Humanidades,
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
* \remark Desarrolla los m�todos de la clase gbm_raices_verbales.
*/



#include <gbm_raices_verbales.hpp>

/**
* \author giglesia
* \remark M�todo sobrecargado de gbm::procesa. Realiza el procesado espec�fico
* para un fichero de ra�ces verbales, a partir del procesado gen�rico de la clase
* padre.
* \param nombre: el nombre del fichero a procesar.
*/
void gbm_raices_verbales::procesa(char *nombre) {

        int k,m,p,q;
        int lineaProcesada=-1;
        int comentario=0;


        if (!cargaFichero(nombre)) return;
        gbm::procesa(3,-1);
        tamanio_estructura_lista=sizeof(t_raices_verbales);
        lista=rv= (t_raices_verbales *) reservaMemoria(tamanio*tamanio_estructura_lista);

        if (lista==NULL) {
		  	cargado=-1;
	      	errorGeneralIF=1;
     		return;
  		}



        for (k=0,m=1;k<parametros;k++){
                 if (*(resultado+k)!=NULL) {
                        if (comentario) continue;
                        if (**(resultado+k)=='@') continue;
                        if (**(resultado+k)=='#' && m==1) { //L�nea comentada.
                                comentario=1;
                                continue;
                        }
                        if (**(resultado+k)) {
                                if (m==1) {
                                        rv[++lineaProcesada].raiz=*(resultado+k); //Parametro 1
                                        p=q=-1;
                                }
                                if (m>=2) { 
                                        if (p<IF_NUM_INFINITIVO && q<=IF_NUM_MODELOS)
                                                if (q<IF_NUM_MODELOS && *resultado[k]<58 && *resultado[k]>47) rv[lineaProcesada].modelo[p].grupos[++q]= (char) atoi(resultado[k]);
                                                else {
                                                        rv[lineaProcesada].modelo[++p].inf=*(resultado+k);
                                                        q=-1;
                                                }
                                }
                                m++;
                        }
                        else m=1;

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
* \remark Constructor que inicializa la clase con el nombre del
* fichero a procesar.
* \param nombre: el nombre del fichero a procesar.
*/
gbm_raices_verbales::gbm_raices_verbales(char *nombre){

        procesa(nombre);
        lema[0]=0;
}

/*
t_raices_verbales *gbm_raices_verbales::asignaPuntero(){

        return rv;

}

 int gbm_raices_verbales::numero_raices_verbales(){


        return numero_nodos_lista;

 }

 */

/**
* \author giglesia
* \remark Constructor que inicializa una instancia vac�a.
*/
gbm_raices_verbales::gbm_raices_verbales(){}


int gbm_raices_verbales::buscar(char *nombre,t_frase_separada *frase_separada) {

    char yipicallei[LONX_MAX_PALABRA];


    if ((strlen(nombre)<2)) return -1; 

    strcpy(yipicallei,nombre);
    yipicallei[strlen(nombre)-2]=0;

    int posicion=gbm::busca(yipicallei);
    if (posicion==-1) return -1;

    for (int k=0;k<IF_NUM_INFINITIVO && rv[posicionBusqueda].modelo[k].inf!=NULL;k++)
      if (!strcmp(rv[posicionBusqueda].modelo[k].inf,nombre)){
        strcpy(lema,nombre);
        return posicionBusqueda;
      }
    return -1;

}


//Nota: esta funci�n s�lo tiene sentido si se ejecuta despu�s de "buscar(...)".

char * gbm_raices_verbales::devuelveLema() {

        if (posicionBusqueda==-1) return NULL;        
        return lema;
}
