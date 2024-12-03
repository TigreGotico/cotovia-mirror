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
* Desarrolla los métodos de la clase gbm_sustantivos.
*/


#include <cotovia2eagles.hpp>
#include <sintagma.hpp>
#include <gbm_sustantivos.hpp>


/**
* \author giglesia
* \remark Método sobrecargado de gbm::procesa. Realiza el procesado específico
* para un fichero de sustantivos/adjetivos, a partir del procesado genérico de la clase
* padre.
* \param nombre: el nombre del fichero a procesar.
*/
void gbm_sustantivos::procesa(char *nombre) {

        int k,m;
        int lineaProcesada=-1;
        int comentario=0;
        int pos;
        bool auxi;
        char *viejoLema=NULL;
//        printf("Cargando %s\n",nombre);
        if (!cargaFichero(nombre))return;
        gbm::procesa(2,-1);
        tamanio_estructura_lista=sizeof(t_nombre);
        lista=nom= (t_nombre *) reservaMemoria(tamanio*tamanio_estructura_lista/4);
        g2c=CotoviaEagles::getSingleton();
		if (lista==NULL) {
		  	cargado=-1;
	        	errorGeneralIF=1;
     		        return;
  		}


        for (k=0,m=1;k<parametros;k++){
                 if (*(resultado+k)!=NULL) {
                        if (comentario) continue;
                        if (**(resultado+k)=='@') continue;
                        if (**(resultado+k)=='#' && m==1) { //Línea comentada.
                                comentario=1;
                                continue;
                        }
                        if (*resultado[k]) {
                                if (m==1) {
                                  (nom+(++lineaProcesada))->palabra=*(resultado+k); //Parametro 1
                                  viejoLema=NULL;
                                }
                                if (m>=2) {
                                  (nom+lineaProcesada)->categoria[m-2]=*(resultado+k); //2 y posteriores.
                                  pos=g2c->mDG(string(*(resultado+k)));
                                  if (pos>-1) {
                                    (nom+lineaProcesada)->xen=&(nom+lineaProcesada)->categoria[m-2][pos];
                                    (nom+lineaProcesada)->num=&(nom+lineaProcesada)->categoria[m-2][pos+1];
                                  } else {
//                                    (nom+lineaProcesada)->xenero=NO_ASIGNADO;
//                                    (nom+lineaProcesada)->numero=NO_ASIGNADO;

                                  }

                                  if (viejoLema!=NULL)viejoLema=(nom+lineaProcesada)->lema[m-2]=viejoLema;
                                  else viejoLema=(nom+lineaProcesada)->lema[m-2]= (nom+lineaProcesada)->palabra; //por defecto.
                                  for (int f=1;f<strlen(*(resultado+k));f++){
                                   if (*(*(resultado+k) + f)!='/') continue; //si luego encontramos un /, es que hay lema.
                                    if (&(nom+lineaProcesada)->categoria[m-2][f+1]!=NULL)
                                      viejoLema=(nom+lineaProcesada)->lema[m-2]=&(nom+lineaProcesada)->categoria[m-2][f+1];
                                    else viejoLema=(nom+lineaProcesada)->lema[m-2]=(nom+lineaProcesada)->palabra;
                                    break;
                                  }

                                }
                                m++;
                        }

/*
                        if (**(resultado+k)) {
                                if (m==1) (nom+(++lineaProcesada))->palabra=*(resultado+k); //Parametro 1
                                if (m==2) (nom+(lineaProcesada))->xen=*(resultado+k);   //Parametro 2
                                if (m==3) (nom+(lineaProcesada))->num=*(resultado+k);   //Parametro 3 para la extensión plural
                                if (m==4) (nom+(lineaProcesada))->lema=*(resultado+k);   //Parametro 4 para el lema.
                                m++;
                        }
*/                        
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
gbm_sustantivos::gbm_sustantivos(char *nombre, int cat){
        procesa(nombre);
//        gbmrgp=NULL;
        categoria=cat;
//        rgp=new gbm_regexp();

}

/*
gbm_sustantivos::gbm_sustantivos(char *nombre, char *nombreDiminutivos){
        procesa(nombre);
//        this->gbmrgp= new gbm_regexp(nombreDiminutivos,this);
}
   */
/**
* \author giglesia
* \remark Devuelve el lema de un sustantivo/adjetivo encontrado.
* \return 0 si la búsqueda falló.
*/
char *gbm_sustantivos::devuelveLema(){
  return mGetLemma(0);
}

char *gbm_sustantivos::mGetLemma(int idx){

      if (posicionBusqueda==-1) return 0;
        if (nom[posicionBusqueda].lema[idx]!=NULL)
                return nom[posicionBusqueda].lema[idx];
        return nom[posicionBusqueda].palabra;  //Si no hay, asumimos que la misma palabra es el lema.
}



/**
* \author giglesia
* \remark Devuelve el género de un sustantivo/adjetivo encontrado.
* \return 0 si la búsqueda falló; el género correspondiente con el valor
* de los defines MASCULINO,FEMININO,NEUTRO,AMBIGUO,NO_ASIGNADO.
*/
unsigned char gbm_sustantivos::devuelveGenero(){

        if (posicionBusqueda==-1) return 0;
        if (!nom[posicionBusqueda].xenero && nom[posicionBusqueda].xen!=NULL)
                nom[posicionBusqueda].xenero=traducirXeneroNumero(nom[posicionBusqueda].xen);
        return nom[posicionBusqueda].xenero;
}

/**
* \author giglesia
* \remark Devuelve el número de un sustantivo/adjetivo encontrado.
* \return 0 si la búsqueda falló; el número correspondiente con el valor
* de los defines SINGULAR,PLURAL,AMBIGUO,NO_ASIGNADO.
*/

unsigned char gbm_sustantivos::devuelveNumero(){

        if (posicionBusqueda==-1) return 0;
        if (!nom[posicionBusqueda].numero && nom[posicionBusqueda].num!=NULL)
                nom[posicionBusqueda].numero=traducirXeneroNumero(nom[posicionBusqueda].num);
        return nom[posicionBusqueda].numero;
}

/**
* \author giglesia
* \remark Traduce cadenas de texto de los ficheros en los valores asociados a los
* defines con idéntico nombre (masculino, feminino,singular,plural,ambiguo,neutro)
* \param entrada: candidato a un #define.
* \return si un define con nombre "entrada", su valor. Si no, NO_ASIGNADO.
*
*/
inline unsigned char gbm_sustantivos::traducirXeneroNumero(char *entrada) {

      if (!strncmp(entrada,"M",1) )return MASCULINO;
      if (!strncmp(entrada,"F",1) ) return FEMININO;
      if (!strncmp(entrada,"S",1) ) return SINGULAR;
      if (!strncmp(entrada,"P",1) )   return PLURAL;
      if (!strncmp(entrada,"A",1) )  return AMBIGUO;
      if (!strncmp(entrada,"N",1) )   return NEUTRO;
      return NO_ASIGNADO;

}


int gbm_sustantivos::busca(char *nombre) {

    return gbm::busca(nombre);

//    if (posicion==-1 && gbmrgp!=NULL) posicion=this->gbmrgp->calculaCandidatos(nombre);

//    return posicion;

}

int gbm_sustantivos::buscar(char *palabra,t_frase_separada *rec){
   int num_cat;
   int posicion=0;
   string auxi;
   unsigned char cate;



   if ((posicion=busca(palabra))!=-1){
      if (rec==NULL) return posicion;

      for (int k=0;k<NUM_MAX_CATEGORIA;k++) {
        if (nom[posicion].categoria[k]==0) break;
/*
        auxi=nom[posicion].categoria[k][0];
        auxi+=nom[posicion].categoria[k][1];
  */
        auxi=nom[posicion].categoria[k];


        if (auxi[0]=='&'){
          auxi=auxi.substr(1,auxi.length()-1);
          cate=  g2c->mGetE2C(auxi);
          if ((num_cat=strlen((char *) &(rec->cat_gramatical_descartada[0])))<N_CAT &&
                           !esta_palabra_tivera_a_categoria_de(cate,rec) ){
            rec->cat_gramatical_descartada[num_cat]=cate;
            rec->cat_gramatical_descartada[num_cat+1]=0;
           }
//        anade_categoria(g2c->mGetE2C(auxi),rec);
        } else {
          cate=  g2c->mGetE2C(auxi);        
          if ((num_cat=strlen((char *) &(rec->cat_gramatical[0])))<N_CAT &&
                           !esta_palabra_ten_a_categoria_de(cate,rec) ){
           rec->cat_gramatical[num_cat]=cate;
           rec->cat_gramatical[num_cat+1]=0;
          }
        }

      }
//giglesia: a partir de aquí no se está aprovechando la información que contiene
//el diccionario porque t_frase_separada no lo soporta (e.g. distintos lemas, género
//y número diferente para cada categoría)
      rec->xenero=devuelveGenero();
      rec->numero=devuelveNumero();
      strcpy(rec->lema,devuelveLema());
      return posicion;
   }
   else return -1;
}



/**
* \author giglesia
* \remark Constructor que inicializa una instancia vacía.
*/
gbm_sustantivos::gbm_sustantivos(void){}


gbm_sustantivos::~gbm_sustantivos(void){

//  delete gbmrgp;
}

