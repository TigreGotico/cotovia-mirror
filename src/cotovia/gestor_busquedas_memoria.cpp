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
 * \remarks Desarrolla los métodos de la clase padre gbm.
 */
 

#include <gestor_busquedas_memoria.hpp>
//extern char t_opciones::dir_bd[];
/**
 * \author giglesia
 * \remarks Carga en memoria un fichero de texto.
 * \param nombre: Nombre del fichero.
 * \return 1 si cargó correctamente el fichero o 0 en caso contrario.
 */
int gbm::cargaFichero(char *nombre) {

//	char nom[FILENAME_MAX];
	//t_opciones opciones; //ZA: Truco. dir_bd es estático.
	FILE *fichero;
	int tamanio2;

	//fichero=NULL;
	
	if (nombre == NULL){
		fputs("ZA: Nombre nulo de fichero",stderr);
		return cargado=0;
	}



/*
 *
 *#ifdef _WIN32
 *  if (strchr(nombre,'\\')==NULL) {
 *#else
 *  if (strchr(nombre,'/')==NULL) {
 *#endif
 *    sprintf(nom, "%sdiccionario/%s", opciones.dir_bd, nombre);
 *  }
 *  else {
 *    strcpy(nom, nombre);
 *  }
 *
 */

	if ((fichero= fopen(nombre, "rt")) == NULL)   {
		fprintf(stderr,"Error al abrir el fichero %s\n",nombre);
		return cargado=0;
	}
	fseek(fichero,0,SEEK_END);
	tamanio=ftell(fichero);
	rewind(fichero);

	palabras_lista= (char *) reservaMemoria(tamanio+3);
	tamanio=fread(palabras_lista,1,tamanio,fichero);
	tamanio2= (tamanio>250000)? 2*tamanio:tamanio*4; // Antiguamente era tamanio + tamanio/2, pero al cambiar a 64 bits se quedaba corto. Esto habría que cambiarlo...
	resultado=(char **) reservaMemoria(tamanio2);
	palabras_lista[tamanio]=0;
	palabras_lista[tamanio+1]=0;
	palabras_lista[tamanio+2]=0;
	fclose(fichero);

#ifdef IF_DEPURA

	fprintf(stderr,"ZA: procesando Fichero %s.---\n",nombre);

#endif
	return cargado=1;
}

/**
* \author giglesia
* \remarks Establece el intervalo de parámetros y
* su tipología.
* \param minPar: El número mínimo de parámetros.
* \param maxPar: El número máximo de parámetros. Para infinitos parámetros se
* usa la constante IF_INFINITOSPARAMETROS.
* \param tipoPar. Tipo de parámetros atendiendo a su naturaleza (número, carácter, etc.)
* No funcional de momento.
* \return 1 si cargó correctamente el fichero o 0 en caso contrario.
*/

void gbm::estableceCondiciones(int minPar,int maxPar,int tipoPar){

    if ((minPar>0) && ((maxPar>=minPar) ||(maxPar==IF_INFINITOSPARAMETROS))) {
        minParametros=minPar;
    	maxParametros=maxPar;
    }
    tipoParametros=tipoPar;
}

/**
* \author giglesia
* \remarks Procesa el fichero previamente cargado mediante
* el método gbm::cargaFichero().
*/
void gbm::procesa(){

    char *aux,*aux2,*aux3,*aux4;

    if (!cargado) return;
    parametros=0;
//    aux4=palabras_lista;
    for (aux4=palabras_lista;*aux4==' ';aux4++); //Quitamos espacios    
    while (*aux4!=0) {
        for (aux2=aux4;*aux2!=',' && *aux2!='\n' && *aux2!=0;aux2++);
        if (aux2>aux4)
          for (aux3=aux2-1;(*aux3==' ' || *aux3=='\r' || *aux3==0) && aux3>=aux4;*(aux3--)=0);
        resultado[parametros++]=aux4;
        if (*aux2=='\n') *(resultado+(parametros++))=NULL;
        *aux2=0;
        for (aux4=aux2+1; *aux4==' ';aux4++);
    }
    *(resultado+parametros)=NULL;
}

/**
* \author giglesia
* \remarks Procesa el fichero previamente cargado mediante
* el método gbm::cargaFichero(); además, utiliza para fines de depuración las
* las condiciones establecidas mediante el método gbm::estableceCondiciones().
* \param minPar: El número mínimo de parámetros.
* \param maxPar: El número máximo de parámetros. Para infinitos parámetros se
* usa la constante IF_INFINITOSPARAMETROS.
*/
void gbm::procesa(int minPar,int maxPar){

	procesa();
	if (!cargado || (maxPar>0 && minPar>maxPar)) return; //la segunda parte calla al compilador.
#ifdef IF_DEPURA
	depura(minPar,maxPar);
#endif

}

/**
* \author giglesia
* \remarks Depura y escribe todos los avisos necesarios respecto
* al contenido del fichero, línea a línea, señalando comentarios, errores en el
* número de parámetros, líneas desordenadas, caracteres no aceptados, etc.
* \param minPar: El número mínimo de parámetros.
* \param maxPar: El número máximo de parámetros. Para infinitos parámetros se
* usa la constante IF_INFINITOSPARAMETROS.
* \warning la constante IF_ORDEN_ESTRICTO hace que este método exija orden ascii
* estricto del primer campo (cada campo viene separado por una coma del siguiente)
* de cada línea.
*/
void gbm::depura(int minPar,int maxPar) {

        int k,m;
        int lineaProcesada=1;
        int comentario=0;
        char *anterior=NULL;

        if (!cargado) return;
        for (k=0,m=1;k<parametros;k++){

                if (resultado[k]!=NULL) {
                        if (comentario) continue;
                        if (*resultado[k]=='@'){
                                fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                                fprintf(stderr,"Parametro comentado.\n");
                                m++;
                                continue;
                        }
                        if (*resultado[k]=='#' && m==1) { //Línea comentada.
                                fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                                fprintf(stderr,"Linea comentada.\n");
                                comentario=1;
                                continue;
                        }
                        if (m==1) {

#ifdef IF_ORDEN_ESTRICTO

                                if (anterior!=NULL &&
                                        strcmp(resultado[k],anterior)<=0) {

#else
                                if (anterior!=NULL &&
                                        strcmp(resultado[k],anterior)<0) {

#endif
                                        fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                                        fprintf(stderr,"Linea desordenada.\n");
                                }
                                anterior=resultado[k];

                        }
                        if (*resultado[k]) m++;

                        if (!compruebaPalabra(resultado[k])) continue;
                        fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                        fprintf(stderr,"Parametro [%d] no valido.\n",m-1);
                }
                else {
                        if (comentario) comentario=0;
                        else if (maxPar>0 && !((m-1>=minPar) &&(m-1<=maxPar))) {
                                fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                                fprintf(stderr,"Numero de parámetros [%d] incorrecto.\n",m-1);
                        }
                        else if (m==1){
                                fprintf(stderr,"ZA: Linea %d:",lineaProcesada);
                                fprintf(stderr,"Linea en blanco\n");
                        }
                        lineaProcesada++;
                        m=1;
                }
        }
}

/**
* \author giglesia
* \remarks Comprueba que una palabra tenga caracteres aceptables.
* \param palabra: La palabra a comprobar.
* \return  0 si la palabra es correcta y un valor positivo en caso contrario.
*/
int gbm::compruebaPalabra(char *palabra){

        char *letra;

        for (letra=palabra;*letra!=0;letra++){
	        switch (*letra){
                case '.':
                case 'º':
                case 'ª':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '/':
                case '\\':
                case 'á':
                case 'é':
                case 'í':
                case 'ó':
                case 'ú':
                case 'ü':
                case 'ï':
                case ' ':
                case '-':
                case '_':
                case '+':
                case 'ñ':
                case 'Ñ':
                case '&':
                        continue;
	        default:
		        if ((*letra>='A') && (*letra<='Z')) continue;
                        if ((*letra>='a') && (*letra<='z')) continue;
                        if ((*letra>='0') && (*letra<='9'))  continue;
                return 2;
	}

  }

        return 0;
}
/**
* \author giglesia
* \remarks Constructor de carga vacío. La instancia de la clase siempre devolverá
* resultados negativos en sus búsquedas.
*/
gbm::gbm(void) {

        cargado=0; //O sea.
        lista=NULL;
        palabras_lista=NULL;
        resultado=NULL;

}
/**
* \author giglesia
* \remarks Destructor para liberar todos los recursos. Debe ser invocado para que
* que se libere adecuadamente la memoria.
*/
gbm::~gbm(void){

    if (lista!=NULL) free(lista);
    if (palabras_lista!=NULL) free(palabras_lista);
    if (resultado!=NULL) free(resultado);

}

/**
* \author giglesia
* \remarks Busca si en la lista genérica existe la entrada pedida.
* \param nombre: la expresión que se busca en la lista.
* \return -1 si no lo encuentra; un número mayor o igual que cero
* si lo encuentra.
*/
int gbm::busca(const char *nombre) {

        return posicionBusqueda=(cargado)? comprobar_en_lista_de_palabras2( (const char **) lista,tamanio_estructura_lista,numero_nodos_lista,nombre):-1;

}

/**
* \author giglesia
* \remarks Busca si en la lista genérica existe la entrada pedida. Esta variante
* del método base permite búsquedas inversas o directas, y búsquedas por patrón.
* \param patrón: la expresión que se busca en la lista, del que se espera resultados
* no solo identicos, sino también parecidos (empiezan con las mismas letras).
* \param tipo_busqueda: invertida (IF_BUSQUEDA_INVERTIDA) o directa (IF_BUSQUEDA_DIRECTA).
* \return -1 si no lo encuentra; un número mayor o igual que cero
* si lo encuentra.
*/
int gbm::busca(char *patron,char tipo_busqueda) {

    return posicionBusqueda=(cargado)? comprobar_en_lista_de_inicio_de_palabras2( (char **) lista,tamanio_estructura_lista,numero_nodos_lista,patron,tipo_busqueda):-1;
/*
  char yipi[100];

  for (int k=strlen(patron);k>=0;k--) {
    strcpy(yipi,patron);
    switch (patron[k]) {
    case 'a':
      yipi[k]='á';
      break;
    case 'e':
      yipi[k]='é';
      break;
    case 'i':
      yipi[k]='í';
      break;
    case 'o':
      yipi[k]='ó';
      break;
    case 'u':
      yipi[k]='ú';
      break;
    case 'á':
      yipi[k]='a';
      break;
    case 'é':
      yipi[k]='e';
      break;
    case 'í':
      yipi[k]='i';
      break;
    case 'ó':
      yipi[k]='o';
      break;
    case 'ú':
      yipi[k]='u';
      break;
    default:
      continue;
    }
    posicionBusqueda=(cargado)? comprobar_en_lista_de_inicio_de_palabras2( (char **) lista,tamanio_estructura_lista,numero_nodos_lista,yipi,IF_BUSQUEDA_INVERSA):-1;
    if (posicionBusqueda>-1) return posicionBusqueda;
  }

  return -1;
  */  
}

/**
* \author giglesia
* \remarks Busca si en la lista genérica existe la entrada pedida. Esta variante
* del método base permite búsquedas inversas o directas, por patrón y afinando
* el ámbito de búsqueda de la lista.
* \param patrón: la expresión que se busca en la lista, del que se espera resultados
* no solo identicos, sino también parecidos (empiezan con las mismas letras).
* \param tipo_busqueda: invertida (IF_BUSQUEDA_INVERTIDA) o directa (IF_BUSQUEDA_DIRECTA).
* \param tamanio: para afinar la búsqueda en etapas sucesivas reduciendo el ámbito de nodos.
* \return -1 si no lo encuentra; un número mayor o igual que cero
* si lo encuentra.
*/
int gbm::busca(char *patron, char tipo_busqueda,int tamanio) {

  return posicionBusqueda=(cargado)? comprobar_en_lista_de_inicio_de_palabras2( (char **) lista,tamanio_estructura_lista,(tamanio<numero_nodos_lista)? tamanio:numero_nodos_lista,patron,tipo_busqueda):-1;


}
/**
* \author giglesia
* \remarks Método público que devuelve el número de nodos de la lista cargada en
* la instancia de la clase.
* \warning Esté método existe con fines de convergencia transitoria con el código viejo de
* Cotovia; se desaconseja su utilización, pues no es un dato necesario para el una clase
* cliente de gbm o sus derivados.
*/
int gbm::numero(void){

        return numero_nodos_lista;

}
/**
* \author giglesia
* \remarks Método público que devuelve el puntero a la estructura dentro de la clase que contiene
* la lista para realizar búsquedas.
* \warning Esté método existe con fines de convergencia transitoria con el código viejo de
* Cotovia; se desaconseja su utilización salvo casos extremos porque infringe
* el concepto de transparencia.
*/

void *gbm::asignaPuntero(void) {

        return lista;

}



