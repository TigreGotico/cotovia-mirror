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
* Contiene la clase padre para la gestión de búsquedas de datos
* cargados por fichero de texto.
*/


 
#ifndef IF_GESTORBUSQUEDASMEMORIA
#define IF_GESTORBUSQUEDASMEMORIA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "modos.hpp"
//#include "tip_var.hpp"
//#include "cotovia.hpp"
//#include "variables_globales.hpp"
#include <utilidades.hpp>




#define IF_TODOPALABRAS 1
#define IF_PRIMERAPALABRAOCERO 2
#define IF_PRIMERAPALABRA 4
#define IF_INFINITOSPARAMETROS -1


#define IF_BUSQUEDA_INVERSA 1



/**
*  \author Zaral Arelsiak.
*  \brief Constante que activa la herramienta de depuración de gbm.
*  \remark Permite a la clase gbm realizar un análisis minucioso previo de los
*  ficheros que va a cargar en memoria. Utilizado con la constante
*  IF_ORDEN_ESTRICTO se exigirá orden estricto entre el primer campo de cada
* línea y la siguiente.
*/

//#define IF_DEPURA
/**
*  \author Zaral Arelsiak.
*  \brief Constante que pide orden estricto de líneas en la depuración gbm.
*  \remark Si está activado, se exigirá orden estricto entre el primer
* campo de cada línea y la siguiente. Salvo el fichero de verbos, en sus
* primeras líneas, esta condición es necesaria para la total corrección de los
* mismos.
*/
#define IF_ORDEN_ESTRICTO

//extern char dir_bd[FILENAME_MAX];


extern int errorGeneralIF;


/**
*  \author Zaral Arelsiak.
*  \brief Clase padre con herramientas básicas para gestionar búsquedas.
*  \remark El objetivo de esta clase es hacer totalmente transparente
*   la gestión de un fichero cargado en memoria, exista ese fichero o no.
*  Para ello se proporcionan métodos de carga y procesado de un fichero
* de texto cualquiera a través del constructor de esta clase.
* En cada línea, los parámetros se separan entre comas.
* Una línea puede comentarse con el carácter # al inicio de la misma.
* De igual manera, un parámetro cualquiera puede comentarse con el carácter @.
* Además, si se activa la constante IF_DEPURA, gbm proporciona depuración
* de estos mismos ficheros, atendiendo a ordenación, caracteres incorrectos,
* número de parámetro incorrectos, etcétera. Por último, métodos de búsqueda
* adecuados a elegir por el usuario de la clase; el destructor libera
* automáticamente la memoria.
* \note Si el fichero no existe o se hizo carga vacía (constructor sin parámetros),
* todas las búsquedas devolverán siempre negativo (no encontrado), ajenas a que
* el fichero exista o no.
*/
class gbm {

protected:

        int tamanio_estructura_lista;
        void *lista;
        char *palabras_lista;
        int numero_nodos_lista;
				FILE *fichero;   //Esta como que no va a hacer falta.
        int tamanio;  //Tb sobrará.
        int parametros;
        int minParametros;
        int maxParametros;
        int tipoParametros;   //Prescindible.
        int cargado;
        int posicionBusqueda;

        char **resultado;

        int cargaFichero(char *nombre);
        void estableceCondiciones(int minPar,int maxPar,int tipoPar);
        void procesa();
        void procesa(int minPar,int maxPar);   //Este depura.
        void depura(int minPar,int maxPar);
        int compruebaPalabra(char *palabra);
        gbm(void);


public:
        int busca(const char *nombre);
        virtual char *devuelveLema()=0;
        virtual int buscar(char *nombre,t_frase_separada *frase_separada)=0;        
        int busca(char *patron, char tipo);
        int busca(char *patron, char tipo_busqueda,int tamanio);
        int numero(void);
        void *asignaPuntero(void);
        gbm(void *estructura,int sizeStruct,int sizeTotal);
        virtual ~gbm(void);        
};


#endif
 


