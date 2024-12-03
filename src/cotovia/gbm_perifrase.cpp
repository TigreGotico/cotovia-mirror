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
* \remark Desarrolla los métodos de la clase gbm_perifrase.
*/


#include <stdio.h>
#include "modos.hpp"
#include "tip_var.hpp"
#include "sintagma.hpp"
#include <utilidades.hpp>
//#include "morfolo.hpp"
#include <gestor_busquedas_memoria.hpp>
#include <gbm_perifrase.hpp>

//constructores
gbm_perifrase::gbm_perifrase(char * nombre){
  procesa(nombre);
}
gbm_perifrase::gbm_perifrase(void){
}
//carga
void gbm_perifrase::procesa(char * nombre) {

  int k,m;
  int lineaProcesada=-1;
  int comentario=0;

  if(!cargaFichero(nombre)) return;
  gbm::procesa(2,2);
  tamanio_estructura_lista=sizeof(t_perifrase);
  lista=nom= (t_perifrase *) reservaMemoria(tamanio*tamanio_estructura_lista);

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
      if (**(resultado+k)) {
        if (m==1) (nom+(++lineaProcesada))->palabra=*(resultado+k); //Parametro 1: verbo
        else if (m==2) (nom+(lineaProcesada))->tipo=*(resultado+k);   //Parametro 2 : tipo peri
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
/*
 * busca perifrase: asigna la categ. de PERIFRASE . Devuelve el nº de pal. de la
 * perifrasis o 1 si no detecta
 */
int gbm_perifrase::busca(t_frase_separada * item){
  int pos;
  char i=0;
  char * tipo;
  char * pal_sig;
  unsigned char fnp1,genero1,numero1;
  unsigned char fnp2,genero2,numero2;
  char otros_tiempos;
  //if (!esta_palabra_ten_a_categoria_de(VERBO,item))
  if (!*item->cat_verbal)
    return 1;

  while (item->cat_verbal[i]) {
    if ((pos=gbm::busca(item->inf_verbal[i].infinitivo))==-1){
      i++;
      continue;
    }

    if ((item+1)->pal_transformada != NULL)
      pal_sig=(item+1)->pal_transformada;
    else
      pal_sig=(item+1)->pal;

    if (*(item+1)->pal)
      fnp1=formas_no_personales((item+1)->cat_verbal,&genero1,&numero1,&otros_tiempos);
    else
      fnp1=0;

    if (*(item+2)->pal)
      fnp2=formas_no_personales((item+2)->cat_verbal,&genero2,&numero2,&otros_tiempos);
    else
      fnp2=0;

    tipo=(nom+pos)->tipo;
    while (*tipo){
      switch(*tipo) {
        case 'a':
          if (fnp1==INFINITIVO) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            (item+1)->tonicidad_forzada=1;
            return 2;
          }
          break;
        case 'b':
          if (fnp1==PARTICIPIO ) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            (item+1)->tonicidad_forzada=1;
            return 2;
          }
          break;
        case 'c':
          if (fnp1==XERUNDIO) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            (item+1)->tonicidad_forzada=1;
            return 2;
          }
          break;
        case 'd':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"a")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'e':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"de")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'f':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"en")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'g':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"para")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'h':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"por")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'i':
          if (fnp2==INFINITIVO &&
            !strcmp(pal_sig,"que")) {
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        case 'j':
//          printf("LALALALA");
//          if (fnp2==PARTICIPIO && !strcmp(pal_sig,"por")) {
//giglesia. Una pequeña chapucilla que parece ser útil: considerar adjetivo en lugar de participio.        
          if ((fnp2==PARTICIPIO || esta_palabra_ten_a_categoria_de(ADXECTIVO,item+2)
                                || esta_palabra_ten_a_categoria_de(PARTICIPIO,item+2))
                  && !strcmp(pal_sig,"por")) { 
            asigna_categoria(PERIFRASE,item);
            item->tonicidad_forzada=1;
            asigna_categoria(PERIFRASE,item+1);
            asigna_categoria(PERIFRASE,item+2);
            (item+2)->tonicidad_forzada=1;
            return 3;
          }
          break;
        default:
          break;

      }
      tipo++;
    }
    i++;
  }
  return 1;
}

