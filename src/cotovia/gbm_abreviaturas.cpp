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
* \remark Desarrolla los métodos de la clase gbm_abreviaturas.
*/


 #include <gbm_abreviaturas.hpp>



/**
* \author giglesia
* \remark Método privado que procesa un fichero de abreviaturas y lo
* carga en memoria, listo para realizar búsquedas.
* \param nombre: Nombre del fichero a procesar.
*/
void gbm_abreviaturas::procesa(char *nombre) {

        int k,m;
        int lineaProcesada=-1;
        int comentario=0;


        if (!cargaFichero(nombre)) return;
        gbm::procesa(2,3);
        tamanio_estructura_lista=sizeof(t_abreviatura);
        lista=abr= (t_abreviatura *) reservaMemoria(tamanio*tamanio_estructura_lista);
//		lista=NULL;
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
                                if (m==1) abr[++lineaProcesada].abreviatura=resultado[k]; //Parametro 1
                                if (m==2) abr[lineaProcesada].extension[0]=resultado[k];   //Parametro 2
                                if (m==3) abr[lineaProcesada].extension[1]=resultado[k];   //Parametro 3 para la extensión plural
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
* \remark Constructor público para procesar un fichero de abreviaturas.
* \param nombre: Nombre del fichero a procesar.
*/
gbm_abreviaturas::gbm_abreviaturas(char *nombre){

        procesa(nombre);

}

/**
* \author giglesia
* Devuelve la extensión de la abreviatura previamente buscada.
* \return Si la última búsqueda tuvo exito (ver método gbm::busca()), entonces
* devolverá la extensión de la abreviatura. De lo contrario, un NULL.

*/
char *gbm_abreviaturas::devuelveExtension() {

        if (posicionBusqueda==-1) return NULL;
        return abr[posicionBusqueda].extension[0];
}

/**
* \author giglesia
* \remark * Devuelve la extensión de la abreviatura previamente buscada.
* \param numero: SINGULAR (devuelve la primera extensión) o PLURAL (devuelve la
* segunda extensión (plural explícita) o la primera en su defecto (número ambiguo).
* \return Si la última búsqueda tuvo exito (ver método gbm::busca()), entonces
* devolverá la extensión de la abreviatura: De lo contrario, un NULL.
*/
char *gbm_abreviaturas::devuelveExtension(char numero) {

        if (posicionBusqueda==-1) return NULL;
        if (numero==SINGULAR) return abr[posicionBusqueda].extension[0];
        if (numero==PLURAL) return (abr[posicionBusqueda].extension[1]==NULL)? abr[posicionBusqueda].extension[0]:abr[posicionBusqueda].extension[1];
//        if (numero==PLURAL) return abr[posicionBusqueda].extension[1];
        return abr[posicionBusqueda].extension[0];
}
 

