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
* \remark Desarrolla los métodos de la clase gbm_palabras_funcion.
*/

#include <gbm_palabras_funcion.hpp>

/**
* \author giglesia
* \remark Método sobrecargado de gbm::procesa. Realiza el procesado específico
* para un fichero de palabras_funcion, a partir del procesado genérico de la clase
* padre.
* \param nombre: el nombre del fichero a procesar.
*/
void gbm_palabras_funcion::procesa(char *nombre) {

        int k,m;
        int lineaProcesada=-1;
        int comentario=0;
        int juas,cont;

        if (!cargaFichero(nombre)) return;
        gbm::procesa(4,-1);
        tamanio_estructura_lista=sizeof(t_palabra_funcion);
        lista=plf= (t_palabra_funcion *) reservaMemoria(tamanio*tamanio_estructura_lista/2);

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
                                switch (m) {
                                case 1:
                                        plf[++lineaProcesada].palabra=resultado[k]; //Parametro 1
                                        break;
                                case 2:
                                        plf[lineaProcesada].xen=resultado[k];   //Parametro 2
                                        break;
                                case 3:
                                        plf[lineaProcesada].num=resultado[k];
                                        break;
                                default:
                                        juas=strlen(resultado[k]);
                                        for (cont=0;cont<juas;cont++) {
                                           if (resultado[k][cont] & 32 && resultado[k][cont]!='&') {
                                                plf[lineaProcesada].lema=resultado[k]; //Parametro 1
                                                break;
                                           }
                                        }
                                        if (cont==juas) {
                                                if (m>N_CAT+4) break;
                                                plf[lineaProcesada].otros[m-4]=resultado[k];
                                        }
                                }
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
* \remark Constructor que inicializa la clase con el nombre del
* fichero a procesar.
* \param nombre: el nombre del fichero a procesar.
*/
gbm_palabras_funcion::gbm_palabras_funcion(char *nombre){

        procesa(nombre);
}

/**
* \author giglesia
* \remark Devuelve el lemade una palabra_funcion encontrada
* \return 0 si la búsqueda falló; 
*/


char *gbm_palabras_funcion::devuelveLema(){

        if (posicionBusqueda==-1) return 0;
        if (plf[posicionBusqueda].lema!=NULL)
                return plf[posicionBusqueda].lema;
        return plf[posicionBusqueda].palabra;  //Si no hay, asumimos que la misma palabra es el lema.
}



/**
* \author giglesia
* \remark Devuelve el género de una palabra_funcion encontrada
* \return 0 si la búsqueda falló; el género correspondiente con el valor
* de los defines MASCULINO,FEMININO,NEUTRO,AMBIGUO,NO_ASIGNADO.
*/
unsigned char gbm_palabras_funcion::devuelveGenero(){

        if (posicionBusqueda==-1) return 0;
        if (!plf[posicionBusqueda].xenero)
                plf[posicionBusqueda].xenero=traducirXeneroNumero(plf[posicionBusqueda].xen);
        return plf[posicionBusqueda].xenero;
}



/**
* \author giglesia
* \remark Devuelve el número de una palabra_funcion encontrado.
* \return 0 si la búsqueda falló; el número correspondiente con el valor
* de los defines SINGULAR,PLURAL,AMBIGUO,NO_ASIGNADO.
*/
unsigned char gbm_palabras_funcion::devuelveNumero(){

        if (posicionBusqueda==-1) return 0;
        if (!plf[posicionBusqueda].numero)
                plf[posicionBusqueda].numero=traducirXeneroNumero(plf[posicionBusqueda].num);
        return plf[posicionBusqueda].numero;
}
/**
* \author giglesia
* \remark Devuelve las categorías asignadas a la palabra_función encontrada.
* \return NULL si la búsqueda falló; de lo contrario devuelve una cadena en que
* cada char es una categoría.
*/
unsigned char *gbm_palabras_funcion::devuelveCategoriasAsignadas(){

        int k;          
        int nCats,nCatsDesc;
        unsigned char auxi;

        if (posicionBusqueda==-1) return NULL;
        if (!plf[posicionBusqueda].cat[0])
                for (auxi=1,k=0,nCats=nCatsDesc=0;k<N_CAT*2 && auxi!=0; k++) {
                        if (plf[posicionBusqueda].otros[k]!=NULL)
                                if ( *plf[posicionBusqueda].otros[k]!='&') plf[posicionBusqueda].cat[nCats++]= auxi= traducirCategorias(plf[posicionBusqueda].otros[k]);
                                else plf[posicionBusqueda].cat_desc[nCatsDesc++]=auxi=traducirCategorias(plf[posicionBusqueda].otros[k]+1);
                        else break;
                }
        return plf[posicionBusqueda].cat;

}
/**
* \author giglesia
* \remark Devuelve las categorías descartadas a la palabra_función que descartó.
* \return NULL si la búsqueda falló; de lo contrario, devuelve una cadena
* en el que cada char representa una categoría.
*/
unsigned char *gbm_palabras_funcion::devuelveCategoriasDescartadas(){

        int k;
        int nCats,nCatsDesc;
        unsigned char auxi;

        if (posicionBusqueda==-1) return NULL;
        if (plf[posicionBusqueda].cat_desc[0]==0)
                for (auxi=1,k=0,nCats=nCatsDesc=0;k<N_CAT*2 && auxi!=0; k++) {
                        if (plf[posicionBusqueda].otros[k]!=NULL)
                                if (*plf[posicionBusqueda].otros[k]!='&') plf[posicionBusqueda].cat[nCats++]=auxi=traducirCategorias(plf[posicionBusqueda].otros[k]);
                                else plf[posicionBusqueda].cat_desc[nCatsDesc++]=auxi=traducirCategorias(plf[posicionBusqueda].otros[k]+1);
                        else break;
                }
        return plf[posicionBusqueda].cat_desc;
}
/**
* \author giglesia
* \remark Constructor que inicializa una instancia vacía.
*/
gbm_palabras_funcion::gbm_palabras_funcion(){}


