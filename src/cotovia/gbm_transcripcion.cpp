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
 * \author fmendez
 * \brief Desarrolla los métodos de la clase gbm_transcripcion.
 */

#include <gbm_transcripcion.hpp>

gbm_transcripcion *if_transcripcion;

/**
 * \author fmendez
 * \brief Constructor público para procesar un fichero de transcripciones.
 * \param nombre: Nombre del fichero a procesar.
 */
gbm_transcripcion::gbm_transcripcion(char * nombre){
	procesa(nombre);
}

/**
 * \author fmendez
 * \brief Método privado que procesa un fichero de palabras con sus
 * transcripciones fonéticas y lo carga en memoria, listo para realizar búsquedas.
 * \param nombre: Nombre del fichero a procesar.
 */
void gbm_transcripcion::procesa(char * nombre) {

	int k,m;
	int lineaProcesada = -1;
	int comentario = 0;

	if (!cargaFichero(nombre))
		return;
	gbm::procesa(2,-1);
	tamanio_estructura_lista=sizeof(t_transcripcion);
	lista=tra= (t_transcripcion *) reservaMemoria(tamanio*tamanio_estructura_lista);

	for (k=0,m=1;k<parametros;k++){
		if (resultado[k]!=NULL) {
			if (comentario)
				continue;
			if (*resultado[k]=='@')
				continue;
			if (*resultado[k]=='#' && m==1) { //Línea comentada.
				comentario=1;
				continue;
			}
			if (*resultado[k]) {
				switch (m) {
					case 1:
						tra[++lineaProcesada].palabra=resultado[k]; //Parametro 1 palabra
						tra[lineaProcesada].n_transcripciones=0;
                        break;
					case 2:
						tra[lineaProcesada].transcripciones=resultado[k];   //Parametro 2 primera transcripcion
						tra[lineaProcesada].n_transcripciones++;
                        break;
					default:
						tra[lineaProcesada].transcripciones=resultado[k];   //Parametro 3 alternativas
						tra[lineaProcesada].n_transcripciones++;
                        break;
				}
				m++;
			}
		}
		else {
			if (comentario)
				comentario=0;
			m=1;
		}
	}
	numero_nodos_lista=lineaProcesada+1;
}


/**
 * \author fmendez
 * \brief Devuelve las transcripciones fonéticas de la palabra previamente buscada.
 * 
 * \return Si la última búsqueda tuvo exito (ver método gbm::busca()), entonces
 * devolverá la(s) transcripcion(es) de la palabra. De lo contrario, un NULL.
 */
char *gbm_transcripcion::devuelveTranscripcion() {
	if (posicionBusqueda == -1)
		return NULL;
	return tra[posicionBusqueda].transcripciones;
}

/*
int gbm_transcripcion::buscar(char *nombre,t_frase_separada *frase_separada) {

  return -1;
}
*/

