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
 * \remarks Este módulo proporciona una interfaz transparente entre el programa
 * Cotovía y los ficheros que contienen verbos y abreviaturas, sustantivos... etc.
 * De esta manera, cada vez que arranca el
 * programa se cargará en memoria solo aquellos ficheros deseados, según el idioma
 * elegido (en este caso castellano o gallego). En caso de que esté compilado en modo
 * _DLL se cargarán ambos idiomas para permitir al usuario la conmutación de idioma.
 * \warning
 * 	Gracias a la variable opciones.idioma se sabe en qué idioma trabajaremos y
 *    carga solamente los ficheros asociados a ese idioma, de tal manera que para
 *    Cotovia y fuera de este modulo siempre es transparente en qué idioma estamos
 *    trabajando (a efectos de las listas manejadas a través de este interfaz).
 */



#include <interfaz_ficheros.hpp> //Después que diccionarios.h...

  Interfaz::Interfaz(){
    diccionarios=Lexicon::getLexiconSingleton();
  }
  Interfaz::~Interfaz(){
    diccionarios->dispose();
  }


int errorGeneralIF=0;
//Inicialización...

/**
 * \author giglesia
 * \remarks sirve para liberar la memoria con las listas creadas
 *	anteriormente medíante la función pública interfaz().
 * \warning Imprescindible utilizarla antes de terminar el programa.
 */


//NOTA: esto podría ser el destructor.

//int errorGeneralIF=0;

int Lexicon::instancias=0;

//map <string, gbm *> Lexicon::dictio[IF_NUM_IDIOMAS];

gbm *Lexicon::dictio[IF_NUM_IDIOMAS][IF_MAX_DICTIO];


Lexicon *Lexicon::lex=NULL;

Lexicon::Lexicon() {

//  cout << "Hola mundo" << endl;
}

Lexicon *Lexicon::getLexiconSingleton() {

  instancias++;
//  cout << "Instanciando " << instancias << endl;
  if (!lex) lex=new Lexicon();
  return lex;
}
void Lexicon::dispose() {

  instancias--;
//  cout << "Bajando" << instancias << endl;
  if (lex && !instancias) {
    delete lex;
    lex=NULL;
  //  cout << "borrando recursos " << endl;
  }

}

Lexicon::~Lexicon() {
//  cout << "Adiós Mundo Cruel" << endl;
  borraListas();

}


int Lexicon::borraListas(void){

	//  if (instanciasCotovia) return 0;

	/*
	 *for (int k = 0; k<IF_MAX_DICTIO;k++) {
	 * if (dictio[GALEGO][k]) {
	 *   delete dictio[GALEGO][k];
	 *   dictio[GALEGO][k]=NULL;
	 * }
	 * if (dictio[CASTELLANO][k]) {
	 *   delete dictio[CASTELLANO][k];
	 *   dictio[CASTELLANO][k]=NULL;
	 * }
	 *}
	 */
	for (int j = 0; j < IF_NUM_IDIOMAS; j++) {
		for (int k = 0; k<IF_MAX_DICTIO;k++) {
			if (dictio[j][k]) {
				delete dictio[j][k];
				dictio[j][k]=NULL;
			}
		}
	}
	delete if_transcripcion;

	return 0;
}

/**
 * \author giglesia
 * \remarks Busca y devuelve la extensión de la abreviatura.
 * \param pal: Nombre de la abreviatura del que se busca extensión.
 * \return Extensión de esa abreviatura (si no fuera única siempre el caso
 *	singular). Si no se encontró devuelve NULL.
 *
 */
char * Lexicon::existeEnAbreviaturas(char *pal,char genero, char idioma) {

  return (dic[IF_DIC_ABR]->busca(pal)==-1)? NULL:((gbm_abreviaturas *) dic[IF_DIC_ABR])->devuelveExtension(genero);

}
/**
 * \author giglesia
 * \param pal: Nombre de la abreviatura del que se busca extensión.
 *
 * \return Extensión de esa abreviatura (si no fuera única siempre el caso
 *	singular). Si no se encontró devuelve NULL.
 *
 */
char *Lexicon::existeEnAbreviaturas(const char *pal, char idioma) {

  return (dic[IF_DIC_ABR]->busca(pal)==-1)? NULL:((gbm_abreviaturas *) dic[IF_DIC_ABR])->devuelveExtension();

}


/**
 * \author giglesia
 * \remarks Función pública que busca acrónimo y devuelve su extensión.
 * \param pal: acrónimo que se ha de buscar.
 * \return Devuelve la extensión del Acrónimo si figura en la lista y NULL en
 *	caso contrario.
 */
char *Lexicon::existeEnAcronimos(char *pal, char idioma){

        if (dic[IF_DIC_ACR]->busca(pal)==-1) return NULL;
        return ((gbm_abreviaturas *) dic[IF_DIC_ACR])->devuelveExtension();
}

/**
 * \author giglesia
 * \remarks Esta función se encarga de leer los ficheros necesarios y
 * crear las estructuras necesarias con abreviaturas, acrónimos, perífrasis
 * verbos y desinencias.
 * \warning
 * 	Gracias a la variable opciones.idioma se sabe en qué idioma trabajaremos y
 *    carga solamente los ficheros asociados a ese idioma, de tal manera que para
 *    Cotovia y fuera de este modulo siempre es transparente en qué idioma estamos
 *    trabajando (a efectos de las listas manejadas a través de este interfaz).
 */
int Lexicon::interfaz(t_opciones opciones, unsigned char id) {

	//char * id = lang[opciones.idioma];
	char ruta[FILENAME_MAX];
	char nombre[FILENAME_MAX];
	char nombre2[FILENAME_MAX];

	sprintf(ruta, "%slang/%s/", opciones.dir_bd, opciones.lenguajes);



	if (!instancias) {
		for(int j = 0; j < IF_NUM_IDIOMAS; j++){
			for (int k=0;k<IF_MAX_DICTIO;k++) {
				dictio[j][k]=NULL;
				//dictio[GALEGO][k]=NULL;
				//dictio[CASTELLANO][k]=NULL;
			}
		}
	}
	//	instanciasCotovia++;

	//iniciaCliticos();



	if (opciones.ftra){ //fichero con transcripciones fonéticas
		if (*opciones.fich_tra){
			if_transcripcion = new gbm_transcripcion(opciones.fich_tra);
		}
		else {
			sprintf(nombre, "%s%s", ruta, "transcripciones");
			if_transcripcion = new gbm_transcripcion(nombre);
		}
	}

	sprintf(nombre, "%s%s", ruta, "abr.txt");
	if (!dictio[id][IF_DIC_ABR]) dictio[id][IF_DIC_ABR]=new gbm_abreviaturas(nombre);
	sprintf(nombre, "%s%s", ruta, "acron.txt");
	if (!dictio[id][IF_DIC_ACR]) dictio[id][IF_DIC_ACR]=new gbm_abreviaturas(nombre);

	if (!(opciones.carga_todo || opciones.audio || opciones.wav ||
				 opciones.lin || opciones.tra>=4))  {

		//sprintf(nombre, "%s%s", ruta, "verbos.txt");
		//sprintf(nombre2, "%s%s", ruta, "desinencias.txt");
		//if (!dictio[id][IF_DIC_V]) dictio[id][IF_DIC_V]=new gbm_verbos(nombre,nombre2);
		if (!dictio[id][IF_DIC_V]) dictio[id][IF_DIC_V]=new gbm_verbos();
		if (!dictio[id][IF_DIC_N]) dictio[id][IF_DIC_N]=new gbm_sustantivos();
//		if (!dictio[id][IF_DIC_A]) dictio[id][IF_DIC_A]=new gbm_sustantivos();
//		if (!dictio[id][IF_DIC_F]) dictio[id][IF_DIC_F]=new gbm_palabras_funcion();
		if (!dictio[id][IF_DIC_L]) dictio[id][IF_DIC_L]=new gbm_locuciones();
		if (!dictio[id][IF_DIC_P]) dictio[id][IF_DIC_P]=new gbm_perifrase();
		if (!dictio[id][IF_DIC_DER]) dictio[id][IF_DIC_DER]=new gbm_regexp();

	}
	else {
		sprintf(nombre, "%s%s", ruta, "verbos.txt");
		sprintf(nombre2, "%s%s", ruta, "desinencias.txt");
		if (!dictio[id][IF_DIC_V]) dictio[id][IF_DIC_V]=new gbm_verbos(nombre,nombre2);
		sprintf(nombre, "%s%s", ruta, "principal.txt");
//		if (!dictio[id][IF_DIC_N]) dictio[id][IF_DIC_N]=new gbm_sustantivos();
		sprintf(nombre, "%s%s", ruta, "adxectivos.txt");
//		if (!dictio[id][IF_DIC_A]) dictio[id][IF_DIC_A]=new gbm_sustantivos(nombre,ADXECTIVO);
		sprintf(nombre, "%s%s", ruta, "principal.txt");
//		if (!dictio[id][IF_DIC_F]) dictio[id][IF_DIC_F]=new gbm_palabras_funcion(nombre);
		if (!dictio[id][IF_DIC_N]) dictio[id][IF_DIC_N]=new gbm_sustantivos(nombre);
		sprintf(nombre, "%s%s", ruta, "locucions.txt");
		if (!dictio[id][IF_DIC_L]) dictio[id][IF_DIC_L]=new gbm_locuciones(nombre);
		sprintf(nombre, "%s%s", ruta, "perifrase.txt");
		if (!dictio[id][IF_DIC_P]) dictio[id][IF_DIC_P]=new gbm_perifrase(nombre);
		sprintf(nombre, "%s%s", ruta, "derivativos.txt");
		if (!dictio[id][IF_DIC_DER]) dictio[id][IF_DIC_DER]=new gbm_regexp(nombre,dictio[id]);
	}



	return errorGeneralIF;
}

/**
* \author giglesia
* \remarks Esta función busca si el candidato a locución
* existe, asignando a su vez las categorías correspondientes.
* \param frase_separada: estructura con todos los datos de la posible locución.
* \retval frase_separada: si procede, la estructura será modificada con las locuciones encontradas
* para el candidato.
*/

 int Lexicon::buscarConxunciones(t_frase_separada *frase_separada, char idioma){

        return ((gbm_locuciones *) dic[IF_DIC_L])->busca(frase_separada);

}



int Lexicon::busca(char *palabra,char diccionarios){

return -1;
//        return ((gbm_regexp *) dictio[CASTELLANO][IF_DIC_DER])->busca(palabra);

}

int Lexicon::buscaDerivativos(char *palabra,t_frase_separada *f,char idioma){

  return ((gbm_regexp *) dic[IF_DIC_DER])->buscar(palabra,f);

}



/**
* \author giglesia
* \author fmendez
* \remarks Esta función se encarga de leer los ficheros necesarios y
* crear las estructuras necesarias con abreviaturas, acrónimos, perífrasis
* verbos y desinencias.
* \param palabra: cadena con el candidato a sustantivo.
* \param frase_separada: estructura con todos los datos del posible sustantivo.
* \retval frase_separada: si procede, la estructura será modificada con la categoría
* de sustantivo, su género y número correspondiente.
*/
int Lexicon::busca_dic_nomes(char *palabra,t_frase_separada *rec, char idioma){
//   int num_cat_asignadas;

      int chapuza=strlen(palabra);
      if (!chapuza) return -1;
      for (int i=chapuza-1;palabra[i]==' ' && i>0;palabra[i--]=0);

   gbm_sustantivos *dnomes=(gbm_sustantivos *)dic[IF_DIC_N];
   return dnomes->buscar(palabra,rec);

/*
   if (dnomes->busca(palabra,rec)!=-1){
      rec->xenero=dnomes->devuelveGenero();
      rec->numero=dnomes->devuelveNumero();
      strcpy(rec->lema,dnomes->devuelveLema());
      if ((num_cat_asignadas=strlen((char *)& (rec->cat_gramatical[0])))<N_CAT &&
          !esta_palabra_ten_a_categoria_de2(NOME,rec) ){
         rec->cat_gramatical[num_cat_asignadas]=NOME;
         rec->cat_gramatical[num_cat_asignadas+1]=0;
      }
      return 1;
   }
   else return -1;
*/   
}

/**
* \author giglesia
* \author fmendez
* \remarks Esta función se encarga de leer los ficheros necesarios y
* crear las estructuras necesarias con abreviaturas, acrónimos, perífrasis
* verbos y desinencias.
* \param palabra: cadena con el candidato a adjetivo.
* \param frase_separada: estructura con todos los datos del posible adjetivo.
* \retval frase_separada: si procede, la estructura será modificada con la categoría
* de adjetivo, su género y número correspondiente.
*/
int Lexicon::busca_dic_adxe(char *palabra,t_frase_separada *rec, char idioma){

      int num_cat_asignadas;

       gbm_sustantivos *dadxe=(gbm_sustantivos *)dic[IF_DIC_A];

      if (dadxe->busca(palabra)!=-1){
      rec->xenero=dadxe->devuelveGenero();
      rec->numero=dadxe->devuelveNumero();
      strcpy(rec->lema,dadxe->devuelveLema());
      if ((num_cat_asignadas=strlen((char *)& (rec->cat_gramatical[0])))<N_CAT &&
          !esta_palabra_ten_a_categoria_de2(ADXECTIVO,rec) ){
         rec->cat_gramatical[num_cat_asignadas]=ADXECTIVO;
         rec->cat_gramatical[num_cat_asignadas+1]=0;
      }
      return 1;
   }
   else return -1;
}

/**
* \author giglesia
* \author fmendez
* \remarks Esta función se encarga de leer los ficheros necesarios y
* crear las estructuras necesarias con abreviaturas, acrónimos, perífrasis
* verbos y desinencias.
* \param palabra: cadena con el candidato a palabra-función.
* \param frase_separada: estructura con todos los datos de la posible palabra-función.
* \retval frase_separada: si procede, la estructura será modificada con la categoría
* de palabra-función, su género, número u otras categorías asociadas que correspondan.
*/

/*
int Lexicon::busca_dic_pal_funcion(char * palabra,t_frase_separada *rec, char idioma){


//ZA: comienzo de chapuza.
      int chapuza=strlen(palabra);
      if (!chapuza) return -1;
      for (int i=chapuza-1;palabra[i]==' ' && i>0;palabra[i--]=0);
//ZA: fin de chapuza.    

   gbm_sustantivos *dnomes=(gbm_sustantivos *)dic[IF_DIC_F];
   return dnomes->buscar(palabra,rec);


//    return busca_dic_nomes(palabra,rec, idioma);

//Pasa algo curioso, en castellano algunas de las palabras no están bien recortadas
//y hay espacios al final.


      //if (!strcmp(palabra,"y")) puts("BUSCANDO y");
      gbm_palabras_funcion *dpc=(gbm_palabras_funcion *) dic[IF_DIC_F];
//      gbm *dpc=dictio[idioma][IF_DIC_F];
      if (dpc->busca(palabra)!=-1){
//      if (!strcmp(palabra,"y")) puts("ENCONTRADO y");
      rec->xenero=dpc->devuelveGenero();
      rec->numero=dpc->devuelveNumero();
      strcpy(rec->lema,dpc->devuelveLema());
      strcpy((char *)rec->cat_gramatical,(char *)dpc->devuelveCategoriasAsignadas());
      strcpy((char *)rec->cat_gramatical_descartada,(char *)dpc->devuelveCategoriasDescartadas());
      return 1;
   }
   else return -1;
}

/**
* \author giglesia
* \remarks Esta función devuelve la extensión en plural
* de la abreviatura correspondiente. Por ejemplo, para
* km/h devolvería "kilómetros por hora".
* \return la extensión singular si no hay plural o la cadena correspondiente
* al plural, de haberla.
*/
char *Lexicon::devuelveExtensionPlural(char idioma){

        return ((gbm_abreviaturas *) dic[IF_DIC_ABR])->devuelveExtension(PLURAL);
}

/**
* \author fmendez
* \author giglesia
* \remarks Esta función permite la conmutación de idioma y carga, en su caso, de
* los ficheros necesarios. Utilizada para trabajar en modo _DLL.
*/
int Lexicon::inicia_diccionario(t_opciones opciones, unsigned char id){

/*
 *    char idiomas[3][10];
 *
 *    strcpy(idiomas[GALEGO],"galego");
 *    strcpy(idiomas[CASTELLANO],"castelán");
 *
 *    if (!dictio[opciones.idioma][IF_DIC_ABR]) {
 *        	fprintf(stderr,"Non cargou o idioma %s. Se carga agora\n",idiomas[opciones.idioma]);
 *      if (interfaz(opciones)) return 1;
 *    }
 *
 */
/*

  if (opciones->idioma==GALEGO) {
		if (!dictio[idioma]["ABR"]) {
        	fprintf(stderr,"Non cargou o idioma galego. Se carga agora\n");
			if (interfaz(opciones)) return 1;
        }

    if_abreviaturas=g_abreviaturas;
    if_acronimos=g_acronimos;

    if_locuciones=g_locuciones;
    if_desinencias=g_desinencias;
    if_raices_verbales=g_raices_verbales;
    if_sustantivos=g_sustantivos;
    if_adjetivos=g_adjetivos;
    if_palabras_funcion=g_palabras_funcion;
    if_perifrase=g_perifrase;


  }
  else if (opciones->idioma==CASTELLANO) {
        if (!dictio[idioma]["ABR"]){
                fprintf(stderr,"No ha cargado idioma castellano. Se carga ahora\n");
                interfaz(opciones);
        }

    if_abreviaturas=c_abreviaturas;
    if_acronimos=c_acronimos;

    if_locuciones=c_locuciones;
    if_desinencias=c_desinencias;
    if_raices_verbales=c_raices_verbales;
    if_sustantivos=c_sustantivos;
    if_adjetivos=c_adjetivos;
    if_palabras_funcion=c_palabras_funcion;
    if_perifrase=c_perifrase;

  }
  */
//  raices_de_verbos=(t_raices_de_verbos *) if_raices_verbales->asignaPuntero();
//  desinencias=(t_desinencias *) if_desinencias->asignaPuntero();
//  NU_DESINENCIAS[opciones->idioma]= dictio[opciones->idioma][IF_DIC_D]->numero();
//  NU_RAICES_DE_VERBOS[opciones->idioma]=dictio[opciones->idioma][IF_DIC_D]->numero();
  iniciaCliticos(opciones.idioma);
	dic = dictio[id];

  return 0;
}

/**
* \author fmendez
* \remarks Para buscar perífrasis.
*/
int Lexicon::busca_perifrase(t_frase_separada * item, char idioma){
  return ((gbm_perifrase *)dic[IF_DIC_P])->busca(item);
}


int Lexicon::analizar_verbos(char *palabra,unsigned char* tempos_verdadeiros,t_infinitivo* inf, char idioma) {


  gbm_verbos * v=(gbm_verbos *) dic[IF_DIC_V];
  return v->analizar_verbos(palabra,tempos_verdadeiros,inf, idioma);

}




