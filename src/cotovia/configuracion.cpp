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
 
//#include <cstring>
#include <string.h>
#include <fstream>
#include <tip_var.hpp>
#include "configuracion.hpp"
/**
 * \author	Fran Campillo (ideólogo Gonzalo)
 * \remarks	Constructor de la clase Configuracion.
 */

Configuracion::Configuracion() {

	parametros_configuracion.clear();

}

/**
 * \author	Fran Campillo (ideólogo Gonzalo)
 * \remarks	Destructor de la clase Configuracion.
 */

Configuracion::~Configuracion() {

}


/**
 * \author	Fran Campillo (ideólogo Gonzalo)
 * \remarks	Función que carga el fichero de configuración.
 * \param	nombre_fichero: nombre del fichero de configuración.
 * \return	En ausencia de error, devuelve un cero.
 */

int Configuracion::carga_fichero(char *nombre_fichero) {

	int indice;
	//	int numLinea=0;
	unsigned int k;
	char linea[1000];
	vector<string> parametros;

	//  cout << "[" << nombre_fichero << "]" << endl;
	ifstream fichero(nombre_fichero);
	string nombre_parametro, auxiliar;
	string sNomFich(nombre_fichero);
	string::size_type pos=sNomFich.find_last_of('/');
	string::size_type pos_2 = sNomFich.find_last_of('\\');

	//Detectar si encontró algo.
	//Si no encontró, es que la direccionbase es "".
   	if( pos != string::npos )
    	direccionBase=sNomFich.substr(0,pos) + '/';
    else
   		if ( pos_2 != string::npos )
        	direccionBase=sNomFich.substr(0,pos_2) + '\\';
        else
        	direccionBase="";


	if (!fichero.is_open()) {
		//cout<<"Error al intentar abrir el fichero!!! "<<*nombre_fichero<<endl;
		fprintf(stderr, "Error al intentar abrir el fichero %s !!!\n", nombre_fichero);
		return 1;
	}

	while (!fichero.eof()) {

		fichero.getline(linea, 1000);

		for (k=0;k<strlen(linea) && (linea[k]==' ' || linea[k]=='\t');k++);
		if (k>0) strcpy(linea, &linea[k]);
		if (*linea==0 || *linea=='#') continue;
		auxiliar = linea;
		//          cout << linea<< endl;
		parametros.clear();

		if ( (indice = auxiliar.find(",")) < 0) {
			//cout<<"Error: línea sin delimitadores:"<<endl;
			fprintf(stderr, "Error: línea sin delimitadores:\n");
			//cout <<"[" << linea<< "]"<<endl;
			fprintf(stderr,"[%s]\n",linea);
			return 1;
		}

		nombre_parametro = auxiliar.substr(0, indice);

		while ( (indice = auxiliar.find_last_of(',')) > -1) {
			parametros.push_back(auxiliar.substr(indice + 1));
			auxiliar = auxiliar.substr(0, indice);
		}

		parametros_configuracion[nombre_parametro] = parametros;



	} // while (!fichero.eof())

	fichero.close();

	return 0;

}


/**
 * \author	Fran Campillo (ideólogo Gonzalo)
 * \remarks	Función que devuelve la dirección completa del fichero que se le
 * pide.
 * \param	informacion: nombre del parámetro que se busca.
 * \param	nombre_base: nombre del fichero del que se extrae el nombre base para
 * crear la ruta completa que se pide. Si es NULL, se devuelve sólo la dirección.
 * \retval	direccion_salida: dirección buscada.
 * \return	En ausencia de error se devuelve un cero.
 */

int Configuracion::devuelve_direccion(char *informacion, char *nombre_base, char *direccion_salida) {

	char nombre_local[FILENAME_MAX], *apunta_fichero, *nombre_sin_ruta, *elimina_extension = NULL;
    int indice;
	vector <string> parametros;

	parametros = parametros_configuracion[informacion];

    if (nombre_base != NULL) {
		if (parametros.size() != 2) {
			//cout<<"Error en devuelve_direccion: fichero de configuración no válido."<<endl;
			fprintf(stderr, "Error en devuelve_direccion: fichero de configuración no válido.\n");
			return 1;
		}
    }
    else {
		if (parametros.size() < 1) {
			//cout<<"Error en devuelve_direccion: fichero de configuración no válido."<<endl;
			fprintf(stderr, "Error en devuelve_direccion: fichero de configuración no válido.\n");
			return 1;
		}
    }

	if (parametros.size() == 2)
    	indice = 1;
    else
    	indice = 0;
        
	//Curiosamente... ;)
	//  cout << parametros[0] << endl;  //Esto es la extensión.
	//  cout << parametros[1] << endl;  //Esto es la dirección
	/**
	 * Averiguar si la dirección es absoluta o no.
	 * Caso Windows: E:\\ o E:/
	 * Caso Linux /home/...
	 * Caso contrario, es una dirección relativa.
	 */

	bool bEsAbsoluto=false;
	string auxi = parametros[indice].substr(0,3);


	//Linux...
	if (auxi[0]=='/') {
		bEsAbsoluto=true;
	}
	//Windows...
	if (auxi[1]==':' && (auxi[2]=='\\' || auxi[2]=='/')) {
		bEsAbsoluto=true;
	}

	/**
	 * Finalmente, si la dirección es relativa, entonces le añadimos la dirección
	 * base del fichero
	 * de configuración.
	 */

    if (nombre_base != NULL) {
    	if (bEsAbsoluto == false)
			parametros[indice]= direccionBase +  parametros[indice];
    }
    else {
    	if (bEsAbsoluto == false)
	    	parametros[indice]= direccionBase +  parametros[indice];
    	strcpy(direccion_salida, parametros[indice].c_str());
     	return 0;
    }
	//  cout << parametros[1] << endl;  //Esto es la dirección

	strcpy(nombre_local, nombre_base);
	apunta_fichero = (char *) nombre_local;

	while ( (apunta_fichero = strstr(apunta_fichero, "\\")) != NULL)
		*apunta_fichero++= '/';

	apunta_fichero = (char *) nombre_local;
	nombre_sin_ruta = apunta_fichero;

	while ( (apunta_fichero = strstr(apunta_fichero, "/")) != NULL)
		nombre_sin_ruta = ++apunta_fichero;

	apunta_fichero = nombre_sin_ruta;

	while ( (apunta_fichero = strstr(apunta_fichero, ".")) != NULL)
		elimina_extension = apunta_fichero++;

	if (elimina_extension)
		*elimina_extension = '\0';

	sprintf(direccion_salida, "%s%s%s", parametros[1].c_str(), nombre_sin_ruta, parametros[0].c_str());

	return 0;

}

/**
 * \author	Fran Campillo (ideólogo Gonzalo)
 * \remarks	Función que devuelve la dirección completa del fichero que se le
 * pide.
 * \param	informacion: nombre del parámetro que se busca.
 * \retval	parametro: parámetro buscado.
 * \retval  indice:
 * \return	En ausencia de error se devuelve un cero.
 */

int Configuracion::devuelve_parametro(char *informacion, char *parametro, unsigned int indice) {

	vector <string> parametros;

	parametros = parametros_configuracion[informacion];
	if (parametros.empty()) {
		//cout <<" Error en devuelve_parametro: "<< informacion << " no existe." << endl;
		fprintf (stderr, "Error en devuelve_parametro: %s no existe\n", informacion);
		return -1;
	}
	if (parametros.size() < indice) {
		//cout<<"Error en devuelve_parametro: " << informacion <<"fichero de configuración no válido."<<endl;
		fprintf(stderr, "Error en devuelve_parametro: %s fichero de configuración no válido\n", informacion);
		return 1;
	}


	strcpy(parametro, parametros[parametros.size() - indice - 1].c_str());

	return 0;
}

int Configuracion::devuelve_parametro(char *informacion, vector<string> &parametros) {

	parametros = parametros_configuracion[informacion];
	return 0;
}

