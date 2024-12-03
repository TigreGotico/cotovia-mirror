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
* \remark Desarrolla los métodos de la clase gbm_desinencias.
*/

#include <gbm_desinencias.hpp>

/**
* \author giglesia
* \remark Método sobrecargado de gbm::procesa. Realiza el procesado específico
* para un fichero de desinencias, a partir del procesado genérico de la clase
* padre.
* \param nombre: el nombre del fichero a procesar.
*/
void gbm_desinencias::procesa(char *nombre) {

        int k,m;
        int lineaProcesada=-1;
        int comentario=0;

        if (!cargaFichero(nombre)) return;
        gbm::procesa(3,-1);
        tamanio_estructura_lista=sizeof(t_desinencia);
        lista=des= (t_desinencia *) reservaMemoria(tamanio*tamanio_estructura_lista);
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
                                if (m==1) des[++lineaProcesada].desinencia=resultado[k]; //Parametro 1
                                if (m>=2) des[lineaProcesada].info[m-2]=(char) atoi(resultado[k]);   //Parametro 2 y posteriores
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
gbm_desinencias::gbm_desinencias(char *nombre){

        procesa(nombre);
}
/**
* \author giglesia
* \remark Constructor que inicializa una instancia vacía.
*/
gbm_desinencias::gbm_desinencias() {}

/*
int gbm_desinencias::buscar(char *nombre,t_frase_separada *frase_separada) {

  return -1;
}

*/
