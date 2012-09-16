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
 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "modos.hpp"
#include "fonemas.hpp"
#include "info_corpus_voz.hpp"
#include "matriz.hpp"
#include "path_list.hpp"
#include "energia.hpp"
#include "red_neuronal.hpp"
#include "util_neuronal.hpp"
#include "modelo_duracion.hpp"
#include "minor_phrasing.hpp"
#include "locutor.hpp"
#include "grupos_acentuales.hpp"
#include "distancia_espectral.hpp"
#include "descriptor.hpp"
#include "Viterbi.hpp"
#include "info_estructuras.hpp"


// Vector del que se obtendrán los contrastes empleados en la codificacíón
// de los factores.

#ifdef _MODO_NORMAL

static int contraste[15][14]={{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 6,-1,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 7,-1,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 8,-1,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 9,-1,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,-1,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11,-1,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,-1,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13,-1},
                              { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,14}};


#endif

/**
 *  Función:   lee_modelo                                                      
 *  \remarks Entrada:
 *			- archivo: nombre del archivo con la información del modelo de    
 *             duración.                                                       
 *	\remarks Salida:
 *			- En ausencia de error, devuelve un 0.                            
 */

int Modelo_duracion::lee_modelo(char *archivo) {


  FILE *fi;
  int i;
  float *temp;
  char variable[100];

  fi=fopen(archivo,"r");
  if (fi==NULL) {
    fprintf(stderr, "Error en lee_modelo, al intentar abrir el fichero %s.\n",archivo);
    return 1;
  }

  fscanf(fi, "%s %d\n", variable, &MAX_SilabasInicio);
  fscanf(fi, "%s %d\n", variable, &MAX_SilabasFinal);
  fscanf(fi, "%s %d\n", variable, &MAX_TonicasInicio);
  fscanf(fi, "%s %d\n", variable, &MAX_TonicasFinal);

  fscanf(fi,"%d",&num_alof);
  for(i=0;i<num_alof;++i){
    /*        alof[i]=(char*) malloc(LON_ALOF);
	      if(alof[i]==NULL) {
	      printf("\n Error al reservar memoria modelos duracion");
	      return 1;
	      }
    */
    fscanf(fi,"%s",alof[i]);
  }

  fscanf(fi,"%d",&num_coef);
  coef=(float*) malloc(num_coef*sizeof(float));
  if(coef==NULL) {
    fprintf(stderr, "Error en lee_modelo, al reservar memoria modelos duracion.\n");
    return 1;
  }
  temp=(float *) coef;
  for(i=0;i<num_coef;++i) fscanf(fi,"%f",temp++);


  fscanf(fi,"%d",&num_var);
  for(i=0;i<num_var;++i){
    /*        variables[i]=(char*) malloc(MAX_CHAR_VAR);
	      if(variables[i]==NULL) {
	      printf("\n Error al reservar memoria modelos duracion");
	      return 1;
	      }
    */
    fscanf(fi,"%s",variables[i]);
  }

  fclose(fi);

  return 0;

}


/**
 * Función:   devuelve_cadena_alofono_duracion                                      
 * \remarks Entrada:
 *			- clase: clase del fonema, según la clasificación empleada para el      
 *            modelo de duración.                                                   
 * \remarks Valor devuelto:                                                                  
 *          - Una cadena de caracteres que representa el nombre de la clase, y NULL 
 *            en caso de error.                                                     
 */

const char *devuelve_cadena_alofono_duracion(clase_duracion clase) {

  switch (clase) {

  case SILENCIO_DUR:
    return "#";
  case VOC_AB_DUR:
    return "vocabier";
  case VOC_MED_DUR:
    return "vocmed";
  case VOC_CER_DUR:
    return "voccerr";
  case VOC_DUR:
    return "vocal";
  case OCLU_SON_DUR:
    return "ocluson";
  case OCLU_SOR_DUR:
    return "oclusor";
  case FRIC_SOR_DUR:
    return "fricsor";
  case LATERAL_DUR:
    return "lateral";
  case LIQUIDA_DUR:
    return "liquida";
  case NASAL_DUR:
    return "nasal";
  case VIBRANTE_DUR:
    return "vibrante";
#ifdef _CONSIDERA_SEMIVOCALES
  case SEMI_VOCAL_DUR:
    return "semivocal";
#endif
  default:
    fprintf(stderr, "Error en devuelve_cadena_alofono_duracion: Tipo (%d) no contemplado.\n",
	    (int) clase);
    return NULL;
  } // switch

}


/**
 * Función:   tipo_fon_duracion                                                     
 * \remarks Entrada:
 *			- fon: representación gráfica de un alófono, según la clasificación del 
 *            modelo espectral.                                                     
 * \remarks Valor devuelto:                                                                  
 *          - El tipo de fonema. NULL en caso de error.                             
 */

const char *tipo_fon_duracion(char *fon)
{
  if(!strcmp(fon,"a"))
    return("vocabier");
  else if(!strcmp(fon,"e") || !strcmp(fon,"o") || !strcmp(fon,"E")|| !strcmp(fon,"O"))
    return("vocmed");
  else if(!strcmp(fon,"i") || !strcmp(fon,"u"))
    return("voccerr");
#ifdef _CONSIDERA_SEMIVOCALES
  else if (!strcmp(fon, "j") || !strcmp(fon, "w"))
    return("semivocal");
#endif
#ifdef _MODOA_EU
  else if(!strcmp(fon,"p") || !strcmp(fon,"t") || !strcmp(fon,"k")|| !strcmp(fon,"c")|| !strcmp(fon,"tS")|| !strcmp(fon, "ts")|| !strcmp(fon, "ts`"))
#else
  else if(!strcmp(fon,"p") || !strcmp(fon,"t") || !strcmp(fon,"k")|| !strcmp(fon,"tS"))
#endif
    return("oclusor");
  else if(!strcmp(fon,"b") || !strcmp(fon,"d") || !strcmp(fon,"g")|| !strcmp(fon,"B")||
	  !strcmp(fon,"D") || !strcmp(fon,"G"))
    return("ocluson");
#ifdef _MODOA_EU
  else if(!strcmp(fon,"f") || !strcmp(fon,"s") || !strcmp(fon,"s`")|| !strcmp(fon,"S")|| !strcmp(fon,"T") || !strcmp(fon, "x"))
#else	
  else if(!strcmp(fon,"f") || !strcmp(fon,"s") || !strcmp(fon,"S")|| !strcmp(fon,"T") || !strcmp(fon, "x"))
#endif
    return("fricsor");
#ifdef _MODOA_EU
  else if(!strcmp(fon,"l") || !strcmp(fon,"Z") || !strcmp(fon,"jj")|| !strcmp(fon,"gj")|| !strcmp(fon,"L"))
#else
  else if(!strcmp(fon,"l") || !strcmp(fon,"Z") )
#endif
    return("lateral");
  else if(!strcmp(fon,"m") || !strcmp(fon,"n") || !strcmp(fon,"J")|| !strcmp(fon,"N"))
    return("nasal");
  else if(!strcmp(fon,"r") || !strcmp(fon,"rr"))
    return("vibrante");
  else if(!strcmp(fon,"#")) return("silencio");
  else {
    fprintf(stderr, "Error en tipo_fon_duracion: Fonema (%s) no contemplado.\n", fon);
    return NULL;
  }

}


/**
 * Función:   tipo_de_fonema_duracion                                               
 * \remarks Entrada:
 *			- fonema: representación gráfica de un alófono.                         
 *	\remarks Salida:
 *			- clase_alofono: tipo de fonema, según el modelo de duración.           
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un 0.                                  
 */

int tipo_de_fonema_duracion(char *fonema, clase_duracion *clase_alofono) {

  if (strlen(fonema) >= 2) {
#ifdef _MODOA_EU
    if (!strcmp(fonema, "tS") || !strcmp(fonema, "ts")|| !strcmp(fonema, "ts`")) 
#else
      if (!strcmp(fonema, "tS")) 
#endif
	{
	  *clase_alofono = OCLU_SOR_DUR;
	  return 0;
	}
#ifdef _MODOA_EU
      else if (!strcmp(fonema, "s`")) {
	*clase_alofono = FRIC_SOR_DUR;
	return 0;
      }
#endif
      else if (!strcmp(fonema, "rr")) {
	*clase_alofono = VIBRANTE_DUR;
	return 0;
      }
#ifdef _MODOA_EU
      else if (!strcmp(fonema, "jj")) {
	*clase_alofono = LATERAL_DUR;
	return 0;
      }
      else if (!strcmp(fonema, "gj")) {
	*clase_alofono = LATERAL_DUR;
	return 0;
      }
#endif 
      else {
	fprintf(stderr, "Error en tipo_de_fonema_duracion: tipo de fonema (%s) no válido.\n",
		fonema);
	return 1;
      }
  }

  switch (fonema[0]) {

  case 'a':
    *clase_alofono = VOC_AB_DUR;
    return 0;
  case 'e':
  case 'E':
  case 'o':
  case 'O':
    *clase_alofono = VOC_MED_DUR;
    return 0;
  case 'i':
  case 'u':
#ifndef _CONSIDERA_SEMIVOCALES
  case 'j':
  case 'w':
#endif
    *clase_alofono = VOC_CER_DUR;
    return 0;
#ifdef _CONSIDERA_SEMIVOCALES
  case 'j':
  case 'w':
    *clase_alofono = SEMI_VOCAL_DUR;
    return 0;
#endif
  case 'S':
  case 'T':
  case 'f':
  case 's':
  case 'x':
    *clase_alofono = FRIC_SOR_DUR;
  return 0;
  case 'Z':
  case 'l':
#ifdef _MODOA_EU
  case 'L': //euskera
#endif
    *clase_alofono = LATERAL_DUR;
  return 0;
  case 'J':
  case 'N':
  case 'm':
  case 'n':
    *clase_alofono = NASAL_DUR;
  return 0;
  case 'B':
  case 'D':
  case 'G':
  case 'b':
  case 'd':
  case 'g':
    *clase_alofono = OCLU_SON_DUR;
  return 0;
  case 'k':
#ifdef _MODOA_EU
  case 'c':
#endif
  case 'p':
  case 't':
    *clase_alofono = OCLU_SOR_DUR;
  return 0;
  case 'r':
    *clase_alofono = VIBRANTE_DUR;
    return 0;
  case '#':
    *clase_alofono = SILENCIO_DUR;
    return 0;
  default:
    fprintf(stderr, "Error en tipo_de_fonema_duracion: tipo de fonema (%s) no válido.\n",
	    fonema);
    return 1;
  }

}


#ifdef _MODO_NORMAL

/**
 * Función:   estima_duracion_ms                                                    
 * Entrada y Salida                                                                 
 *          - datos: estructura con la información de los predictores y en la que   
 *            se devuelve la duración estimada, en milisegundos.                    
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, se devuelve un 0.                               
 */

int estima_duracion_ms(Parametros_duracion *datos, struct Modelo_duracion * modelo_duracion, char convierte_clases) {


  clase_duracion futuro1, futuro2, pasado1, pasado2, actual;

  struct Modelo_duracion *modelo_temp;

  /* Clasificación de los sonidos. Puede ser necesario modificarla.
     char niv_tipofon[NUM_NIVELES_TIPOFON][MAX_CHAR_VAR]={"#","fricsor","lateral","nasal",
     "ocluson", "oclusor","vibrante",
     "vocabier","voccerr","vocmed"};
  */

#ifdef _CONSIDERA_SEMIVOCALES
  clase_duracion niv_tipofon[NUM_NIVELES_TIPOFON] = {SILENCIO_DUR, FRIC_SOR_DUR, LATERAL_DUR, NASAL_DUR, OCLU_SON_DUR, OCLU_SOR_DUR, VIBRANTE_DUR, VOC_AB_DUR, VOC_CER_DUR, VOC_MED_DUR, SEMI_VOCAL_DUR};
#else
  clase_duracion niv_tipofon[NUM_NIVELES_TIPOFON] = {SILENCIO_DUR, FRIC_SOR_DUR, LATERAL_DUR, NASAL_DUR, OCLU_SON_DUR, OCLU_SOR_DUR, VIBRANTE_DUR, VOC_AB_DUR, VOC_CER_DUR, VOC_MED_DUR};
#endif

  // Incluso podríamos aprovecharnos de que es un tipo enumerado, si lo declaramos en el orden
  // de arriba.


  // Niveles asignados a Enunciativa, Exclamativa, Interrogativa, Inacabada
  int niv_tipoprop[NUM_NIVELES_TIPOPROP]={0,1,2,3};
  // Niveles asignados a PAUSA_COMA, PAUSA_PUNTO, PAUSA_PUNTOS_SUSPENSIVOS.
  int niv_tipopausa[NUM_NIVELES_TIPOPAUSA] = {1, 2, 3};
  // Niveles asignados a SIN_FRONTERA, FRONTERA_SILABA, FRONTERA_PALABRA, FRONTERA_GRUPO_ENTONATIVO
  int niv_tipofrontera[NUM_NIVELES_TIPOFRONTERA] = {0, 1, 2, 3};
  int niv_finalfrase[3] = {1, 2, 3};

  float datos_x[MAX_DIM_X], *temp_x, logdur;
  int i,k,j,fila=0;

  if (convierte_clases == 0) {
    if ( tipo_de_fonema_duracion(datos->Alofono, &actual) || tipo_de_fonema_duracion(datos->Fut1, &futuro1) ||
	 tipo_de_fonema_duracion(datos->Fut2, &futuro2)   || tipo_de_fonema_duracion(datos->Pas1, &pasado1) ||
	 tipo_de_fonema_duracion(datos->Pas2, &pasado2) )
      return 1;

    switch (actual) {
    case VOC_DUR:
    case VOC_AB_DUR:
    case VOC_CER_DUR:
    case VOC_MED_DUR:
      modelo_temp = &modelo_duracion[VOC_DUR];
      break;
    case SEMI_VOCAL_DUR:
      modelo_temp = &modelo_duracion[SEMI_VOCAL_DUR];
      break;
    case OCLU_SOR_DUR:
      modelo_temp = &modelo_duracion[OCLU_SOR_DUR];
      break;
    case OCLU_SON_DUR:
      modelo_temp = &modelo_duracion[OCLU_SON_DUR];
      break;
    case FRIC_SOR_DUR:
      modelo_temp = &modelo_duracion[FRIC_SOR_DUR];
      break;
    case LIQUIDA_DUR:
    case LATERAL_DUR:
      modelo_temp = &modelo_duracion[LATERAL_DUR];
      break;
    case VIBRANTE_DUR:
      modelo_temp = &modelo_duracion[VIBRANTE_DUR];
      break;
    case NASAL_DUR:
      modelo_temp = &modelo_duracion[NASAL_DUR];
      break;
    case SILENCIO_DUR:
      modelo_temp = &modelo_duracion[SILENCIO_DUR];
      break;
    default:
      fprintf(stderr, "Error en estima_duracion: tipo de fonema (%d) no válido.\n", (int) actual);
      return 1;
    }
    
  }
  else {
    if ( tipo_de_fonema_duracion(datos->Alofono, &actual) ||
	 devuelve_tipo_alofono_duracion(datos->Fut1, &futuro1) ||
	 devuelve_tipo_alofono_duracion(datos->Fut2, &futuro2) ||
	 devuelve_tipo_alofono_duracion(datos->Pas1, &pasado1) ||
	 devuelve_tipo_alofono_duracion(datos->Pas2, &pasado2) )
      return 1;
    modelo_temp = modelo_duracion;

  }


  //  fprintf(stderr, "El fonema es: %s.\n", datos->Alofono);

  temp_x=(float *) datos_x;
  *temp_x++=1; // El primer elemento de datos_x es siempre 1.

  if (datos->SilabasInicio > modelo_temp->MAX_SilabasInicio)
    datos->SilabasInicio = (short int) modelo_temp->MAX_SilabasInicio;

  if (datos->SilabasFinal > modelo_temp->MAX_SilabasFinal)
    datos->SilabasFinal = (short int) modelo_temp->MAX_SilabasFinal;

  if (datos->TonicasInicio > modelo_temp->MAX_TonicasInicio)
    datos->TonicasInicio = (short int) modelo_temp->MAX_TonicasInicio;

  if (datos->TonicasFinal > modelo_temp->MAX_TonicasFinal)
    datos->TonicasFinal = (short int) modelo_temp->MAX_TonicasFinal;

  // El siguiente bucle comienza en i=1 para saltar (Intercept)
  for(i = 1;i < modelo_temp->num_var; ++i) {

    if(!strcmp(modelo_temp->variables[i],"Alofono")) {
      if (datos->Alofono[0] == 'x'){
	fila = 2; // Así estamos escogiendo la f. (Chapuza).
      }
#ifdef _MODOA_EU
      else if (!strcmp(datos->Alofono, "s'")){
	fila = 2; // Así estamos escogiendo la f. (Chapuza).
      }
      else if (!strcmp(datos->Alofono, "ts") || !strcmp(datos->Alofono, "ts'")){
	fila = 3; //igual a tS
      }
      else if (datos->Alofono[0] == 'c'){
	fila = 2; // igual a t
      }
      else if (datos->Alofono[0] == 'L' ||!strcmp(datos->Alofono, "jj") || !strcmp(datos->Alofono, "gj")){
	fila = 0; //igual a Z
      }
#endif
      else
	for(j=0;j< modelo_temp->num_alof ; ++j) {
	  if(!strcmp(datos->Alofono, modelo_temp->alof[j])) {
	    fila = j;
	    break;
	  }
	}
      //Se rellena el vector datos_x con el contraste correspondiente.
      for(k = 0;k < modelo_temp->num_alof-1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if(!strcmp(modelo_temp->variables[i],"Fut1")) {
      for(j = 0;j < NUM_NIVELES_TIPOFON; ++j) {
	if (futuro1 == niv_tipofon[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUM_NIVELES_TIPOFON - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if(!strcmp(modelo_temp->variables[i],"Fut2")) {
      for (j = 0;j < NUM_NIVELES_TIPOFON; ++j){
	if (futuro2 == niv_tipofon[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUM_NIVELES_TIPOFON - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if(!strcmp(modelo_temp->variables[i],"Pas1")) {
      for(j=0;j< NUM_NIVELES_TIPOFON ; ++j) {
	if (pasado1 == niv_tipofon[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUM_NIVELES_TIPOFON - 1; ++k)
	*temp_x++=contraste[fila][k];
    }

    else if(!strcmp(modelo_temp->variables[i],"Pas2")) {
      for(j = 0;j < NUM_NIVELES_TIPOFON; ++j) {
	if (pasado2 == niv_tipofon[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUM_NIVELES_TIPOFON - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if (!strcmp(modelo_temp->variables[i], "AntPausa")) {
      for (j = 0; j < NUM_NIVELES_TIPOPAUSA; j++) {
	if (datos->AnteriorPausa ==  niv_tipopausa[j]) {
	  fila = j;
	  break;
	}
      }
      for (k = 0; k < NUM_NIVELES_TIPOPAUSA - 1; k++)
	*temp_x++ = contraste[fila][k];

    }

    else if (!strcmp(modelo_temp->variables[i], "SigPausa")) {
      for (j = 0; j < NUM_NIVELES_TIPOPAUSA; j++) {
	if (datos->SiguientePausa ==  niv_tipopausa[j]) {
	  fila = j;
	  break;
	}
      }
      for (k = 0; k < NUM_NIVELES_TIPOPAUSA - 1; k++)
	*temp_x++ = contraste[fila][k];

    }

    else if(!strcmp(modelo_temp->variables[i],"Tonica")) {
      if (datos->Tonica == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
    }

    // else if (!strcmp(modelo_temp->variables[i], "InicioSilaba")) {
    // 	if (datos->InicioSilaba == 0)
    // 		*temp_x++ = -1;
    // 	else
    // 		*temp_x++ = 1;
    // }

    // else if (!strcmp(modelo_temp->variables[i], "FinSilaba")) {
    // 	if (datos->FinSilaba == 0)
    // 		*temp_x++ = -1;
    // 	else
    // 		*temp_x++ = 1;
    // }

    else if (!strcmp(modelo_temp->variables[i], "FronteraInicial")) {
      for (j = 0; j < NUM_NIVELES_TIPOFRONTERA; ++j) {
	if (datos->FronteraInicial == niv_tipofrontera[j]) {
	  fila = j;
	  break;
	}
      }
      for (k = 0; k < NUM_NIVELES_TIPOFRONTERA - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if (!strcmp(modelo_temp->variables[i], "FronteraFinal")) {
      for (j = 0; j < NUM_NIVELES_TIPOFRONTERA; ++j) {
	if (datos->FronteraFinal == niv_tipofrontera[j]) {
	  fila = j;
	  break;
	}
      }
      for (k = 0; k < NUM_NIVELES_TIPOFRONTERA - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if (!strcmp(modelo_temp->variables[i], "FinalFrase")) {
      for (j = 0; j < 3; ++j) {
	if (datos->FronteraFinal == niv_finalfrase[j]) {
	  fila = j;
	  break;
	}
      }
      for (k = 0; k < 2; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if(!strcmp(modelo_temp->variables[i],"TipoProposicion")) {
      for(j = 0;j < NUM_NIVELES_TIPOPROP; ++j){
	if (datos->TipoProposicion == niv_tipoprop[j]) {
	  fila = j;
	  break;
	}
      }
      for(k = 0;k < NUM_NIVELES_TIPOPROP - 1; ++k)
	*temp_x++ = contraste[fila][k];
    }

    else if (!strcmp(modelo_temp->variables[i], "DurDesdePausa"))
      *temp_x++ = datos->DuracionDesdePausa;

    else if (!strcmp(modelo_temp->variables[i],"FonemasSilaba"))
      *temp_x++ = datos->FonemasSilaba;

    else if (!strcmp(modelo_temp->variables[i],"PosEnSilaba"))
      *temp_x++ = datos->PosEnSilaba;

    else if (!strcmp(modelo_temp->variables[i], "SilPalabra"))
      *temp_x++ = datos->SilabasPalabra;

    else if (!strcmp(modelo_temp->variables[i], "FonPalabra"))
      *temp_x++ = datos->FonemasPalabra;

    else if (!strcmp(modelo_temp->variables[i],"SilabasInicio"))
      *temp_x++ = datos->SilabasInicio;

    else if (!strcmp(modelo_temp->variables[i],"SilabasFinal"))
      *temp_x++ = datos->SilabasFinal;

    else if (!strcmp(modelo_temp->variables[i],"TonicasInicio"))
      *temp_x++ = datos->TonicasInicio;

    else if (!strcmp(modelo_temp->variables[i],"TonicasFinal"))
      *temp_x++ = datos->TonicasFinal;

    else if(!strcmp(modelo_temp->variables[i],"FronteraInicial")) {
      if (datos->FronteraInicial == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
    }

    else if(!strcmp(modelo_temp->variables[i],"FronteraFinal")) {
      if (datos->FronteraFinal == 0)
	*temp_x++ = -1;
      else
	*temp_x++ = 1;
    }

    else {
      datos->Duracion = 80.0;
      return 1;
    }
  } //end for

  temp_x = (float *) datos_x;
  logdur=0;

  float *correcaminos = modelo_temp->coef;

  //Se realiza la multiplicación del vector  datos_x por los coef. del modelo.

  for(i = 1;i <= modelo_temp->num_coef; ++i) {
    //    fprintf(stderr, "%d => %f, %f.\n", i, *temp_x, *correcaminos);
    logdur = logdur + (*temp_x++)*(*correcaminos++);
    //    getchar();
  }
	

  //Como se modela log(Dur) se aplica la transformación inversa.

  datos->Duracion = exp(logdur);

  if (datos->Duracion < DURACION_MINIMA_FONEMA)
    datos->Duracion = DURACION_MINIMA_FONEMA;

  return 0;

}


#endif

/**
 * Función:   escribe_parametros_duracion                                           
 * \remarks Entrada:
 *			- datos: estructura de tipo Parametros_duracion_entrenamiento en la que 
 *            se encuentra toda la información relativa a la duración de un fonema  
 *            y a los parámetros que están relacionados con ella.                   
 *	\remarks Salida:
 *			- fichero: fichero de texto en el que se va a escribir la información.  
 */

void escribe_parametros_duracion(Parametros_duracion_entrenamiento *datos, FILE *fichero) {

  fprintf(fichero, "%c\t", datos->sordo_sonoro + '0');
  fprintf(fichero, "%s\t%f", datos->Alofono, datos->Duracion);
  //    fprintf(fichero, "\t%f", tiempo_acumulado);
  fprintf(fichero, "\t%s\t%s\t%s\t%s", datos->Fut1, datos->Fut2, datos->Pas1, datos->Pas2);
  fprintf(fichero, "\t%c\t%d\t%d", datos->Tonica + '0', datos->FonemasSilaba,
	  datos->PosEnSilaba);
  fprintf(fichero, "\t%d\t%d", datos->SilabasInicio, datos->SilabasFinal);
  fprintf(fichero, "\t%c\t%d\t%d", datos->TipoProposicion + '0', datos->TonicasInicio, datos->TonicasFinal);
  fprintf(fichero, "\t%c\t%c", datos->FronteraInicial + '0', datos->FronteraFinal + '0');
  //  fprintf(fichero, "\t%c\t%c", datos->InicioSilaba + '0', datos->FinSilaba + '0');
  fprintf(fichero, "\t%f", datos->DuracionDesdePausa);
  fprintf(fichero, "\t%c\t%c", datos->AnteriorPausa + '0', datos->SiguientePausa + '0');
  fprintf(fichero, "\t%d\t%d", datos->FonemasPalabra, datos->SilabasPalabra);
  fprintf(fichero, "\t%d", datos->FinalFrase);
  fprintf(fichero, "\t%s", datos->palabra);
  fprintf(fichero, "\t%s\n", datos->fichero_origen);

}

/**
 * Función:   devuelve_tipo_alofono_duracion                                        
 * \remarks Entrada:
 *			- nombre: nombre del fonema.                                            
 *	\remarks Salida:
 *			- clase: tipo del fonema, según la clasificación empleada para el       
 *            modelo de duración.                                                   
 */

int devuelve_tipo_alofono_duracion(char *nombre, clase_duracion *clase) {

  if (strcmp(nombre, "#") == 0)
    *clase = SILENCIO_DUR;
  else if (strcmp(nombre, "vocabier") == 0)
    *clase = VOC_AB_DUR;
  else if (strcmp(nombre, "vocmed") == 0)
    *clase = VOC_MED_DUR;
  else if (strcmp(nombre, "voccerr") == 0)
    *clase = VOC_CER_DUR;
  else if (strcmp(nombre, "ocluson") == 0)
    *clase = OCLU_SON_DUR;
  else if (strcmp(nombre, "oclusor") == 0)
    *clase = OCLU_SOR_DUR;
  else if (strcmp(nombre, "fricsor") == 0)
    *clase = FRIC_SOR_DUR;
  else if (strcmp(nombre, "lateral") == 0)
    *clase = LATERAL_DUR;
  else if (strcmp(nombre, "nasal") == 0)
    *clase = NASAL_DUR;
  else if (strcmp(nombre, "vibrante") == 0)
    *clase = VIBRANTE_DUR;
#ifdef _CONSIDERA_SEMIVOCALES
  else if (strcmp(nombre, "semivocal") == 0)
    *clase = SEMI_VOCAL_DUR;
#endif
  else {
    fprintf(stderr, "Error en devuelve_tipo_alofono_duracion: clase no contemplada (%s).\n", nombre);
    return 1;
  } // else
  return 0;

}

/**
 * \brief Función que lee una tabla de datos con el formato de escribe_parametros_duracion y devuelve una cadena de estructuras de tipo Parametros_duracion.
 * \param[in] nombre_fichero nombre del fichero con los datos.
 * \param[out] *datos cadena con los datos.
 * \param[out] numero_datos número de elementos de la cadena anterior.
 * \return En ausencia de error, devuelve un cero.
 */
int lee_tabla_datos_duracion(char *nombre_fichero, Parametros_duracion_entrenamiento **datos, int *numero_datos) {

  int memoria_reservada = 1000;
  int tamano_actual = 0;

  char linea[2000];
  FILE *fichero_datos;

  Parametros_duracion_entrenamiento *apunta_entrada;

  if ( (fichero_datos = fopen(nombre_fichero, "rt")) == NULL) {
    fprintf(stderr, "Error lee_tabla_datos_duracion, al abrir el fichero %s.\n", nombre_fichero);
    return 1;
  }

  // Leemos la cabecera:
  fgets(linea, 2000, fichero_datos);

  // Reservamos memoria:
  if ( (*datos = (Parametros_duracion_entrenamiento *) malloc(memoria_reservada*sizeof(Parametros_duracion_entrenamiento))) == NULL) {
    fprintf(stderr, "Error en lee_tabla_datos_duracion, al asignar memoria.\n");
    return 1;
  }

  apunta_entrada = *datos;

  while (fgets(linea, 2000, fichero_datos)) {
    
    sscanf(linea, "%d\t%s\t%f\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%d\t%s\t%s\n", &apunta_entrada->sordo_sonoro, 
	   apunta_entrada->Alofono, 
	   &apunta_entrada->Duracion, 
	   apunta_entrada->Fut1, apunta_entrada->Fut2, 
	   apunta_entrada->Pas1, apunta_entrada->Pas2, 
	   &apunta_entrada->Tonica, 
	   &apunta_entrada->FonemasSilaba, &apunta_entrada->PosEnSilaba, 
	   &apunta_entrada->SilabasInicio, &apunta_entrada->SilabasFinal, 
	   &apunta_entrada->TipoProposicion, 
	   &apunta_entrada->TonicasInicio, &apunta_entrada->TonicasFinal, 
	   &apunta_entrada->FronteraInicial, &apunta_entrada->FronteraFinal, 
	   &apunta_entrada->DuracionDesdePausa, 
	   &apunta_entrada->AnteriorPausa, &apunta_entrada->SiguientePausa, 
	   &apunta_entrada->FonemasPalabra, &apunta_entrada->SilabasPalabra, 
	   &apunta_entrada->FinalFrase, 
	   apunta_entrada->palabra, apunta_entrada->fichero_origen);

    if (++tamano_actual >= memoria_reservada) {
      memoria_reservada += 1000;

      if ( (*datos = (Parametros_duracion_entrenamiento *) realloc(*datos, memoria_reservada*sizeof(Parametros_duracion_entrenamiento))) == NULL) {
	fprintf(stderr, "Error en lee_tabla_datos_duracion, al reasignar memoria.\n");
	return 1;
      }
      apunta_entrada = *datos + tamano_actual;

    }
    else
      apunta_entrada++;

  }

  fclose(fichero_datos);

  *numero_datos = tamano_actual;

  return 0;

} 

/**
 * \brief Función que convierte una estructura de tipo Parametros_duracion_entrenamiento en otra de tipo Parametros_duracion
 * \param[in] entrada estructura de entrada
 * \param[out] salida estructura de salida
 */
void convierte_Parametros_duracion_entrenamiento_a_Parametros_duracion(Parametros_duracion_entrenamiento *entrada, Parametros_duracion &salida) {

  
  salida.Duracion = entrada->Duracion;
  salida. DuracionDesdePausa = entrada->DuracionDesdePausa;
  strcpy(salida.Alofono, entrada->Alofono);
  strcpy(salida.Fut1, entrada->Fut1);
  strcpy(salida.Fut2, entrada->Fut2);
  strcpy(salida.Pas1, entrada->Pas1);
  strcpy(salida.Pas2, entrada->Pas2);
  salida.Tonica = entrada->Tonica;
  salida.TipoProposicion = entrada->TipoProposicion;
  salida.FronteraInicial = entrada->FronteraInicial;
  salida.FronteraFinal = entrada->FronteraFinal;
  salida.AnteriorPausa = entrada->AnteriorPausa;
  salida.SiguientePausa = entrada->SiguientePausa;
  salida.FinalFrase = entrada->FinalFrase;
  salida.FonemasPalabra = entrada->FonemasPalabra;
  salida.SilabasPalabra = entrada->SilabasPalabra;
  salida.FonemasSilaba = entrada-> FonemasSilaba;
  salida.PosEnSilaba = entrada->PosEnSilaba;
  salida.SilabasInicio = entrada->SilabasInicio;
  salida.SilabasFinal = entrada->SilabasFinal;
  salida.TonicasInicio = entrada->TonicasInicio;
  salida.TonicasFinal = entrada->TonicasFinal;

}
