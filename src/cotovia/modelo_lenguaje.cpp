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
#include <stdlib.h>
#include <string.h>
#include "perfhash.hpp"
#include "categorias_analisis_morfosintactico.hpp"
#include "modelo_lenguaje.hpp"


#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

static float penalizacion_tetragramas;
static float penalizacion_trigramas;
static float penalizacion_bigramas;
#endif



Modelo_lenguaje::Modelo_lenguaje(){
	//Inicializacion de variables de la clase
	pentagramas = NULL;
	pentagramas_real = NULL;
	tetragramas = NULL;
	tetragramas_real = NULL;
	trigramas = NULL;
	trigramas_real = NULL;
	bigramas = NULL;
	bigramas_real = NULL;
	clases_ambiguedad = NULL;
	unigramas = NULL;
	numero_unigramas = 0;
	numero_modelos_cargados = 0;
}

Modelo_lenguaje::Modelo_lenguaje(char * ruta, char *l){
	pentagramas = NULL;
	pentagramas_real = NULL;
	tetragramas = NULL;
	tetragramas_real = NULL;
	trigramas = NULL;
	trigramas_real = NULL;
	bigramas = NULL;
	bigramas_real = NULL;
	clases_ambiguedad = NULL;
	unigramas = NULL;
	numero_unigramas = 0;
	numero_modelos_cargados = 0;
	crea_estructuras_hash(ruta);
	
	nombre = strdup(l);
	numero_usuarios = 1;
}

Modelo_lenguaje::~Modelo_lenguaje(){
	libera_hash();
	free(nombre);
}



#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

/**
 * Función:   establece_penalizaciones_n_gramas	                               
 * \remarks Entrada:
 *			- pen_tetragramas: penalización para las situaciones en las que se 
 *			  desciende al tetragrama para estimar la probabilidad.			   
 *			- pen_trigramas: similar a la anterior, pero referida a los 	   
 *			  trigramas.													   
 *			- pen_bigramas: similar a las anteriores, pero referida a los 	   
 *			  bigramas.														   
 */


void Modelo_lenguaje::establece_penalizaciones_n_gramas(float pen_tetragramas, float pen_trigramas,
									   float pen_bigramas) {

	penalizacion_tetragramas = pen_tetragramas;
    penalizacion_trigramas = pen_trigramas;
	penalizacion_bigramas = pen_bigramas;
    
}

#endif



/**
 * Función:   cambia_claves_y_separacion                                       
 * \remarks Entrada:
 *			- claves: array con las nuevas claves para la estructura hash.     
 *          - separación: nueva separación entre las claves.                   
 *          - n: indica qué estructura hash (2, 3 ó 4) se desea cambiar.       
 * \remarks Valor devuelto:                                                             
 *          - Devuelve un 1 si se escoge un valor de n no válido.              
 */

int Modelo_lenguaje::cambia_claves_y_separacion(unsigned char *claves, unsigned int separacion, unsigned char n) {

    switch (n) {

        case 2:
            free(bigramas->claves);
            bigramas->claves = claves;
            bigramas->sep = separacion;
            return 0;
        case 3:
            free(trigramas->claves);
            trigramas->claves = claves;
            trigramas->sep = separacion;
            return 0;
        case 4:
            free(tetragramas->claves);
            tetragramas->claves = claves;
            tetragramas->sep = separacion;
            return 0;
        case 5:
            free(pentagramas->claves);
            pentagramas->claves = claves;
            pentagramas->sep = separacion;
            return 0;
        default:
            fprintf(stderr, "Error en cambia_claves_y_separacion: valor de entrada no válido.\n");
            return 1;
    }

}


/**
 * Función:   asigna_estructuras                                               
 * \remarks Entrada:
 *			- est5: estructura hash que se desea asignar a la variable del     
 *            módulo estructura_hash_5 (pentagramas).                           
 *          - est4: estructura hash que se desea asignar a la variable del     
 *            módulo estructura_hash_4 (tetragramas).                          
 *          - est3: estructura_hash que se desea asignar a la variable del     
 *            módulo estructura_hash_3 (trigramas).                            
 *          - est2: estructura_hash que se desea asignar a la variable del     
 *            módulo estructura_hash_2 (bigramas).                             
 *          - uni: estructura_unigramas cuyo contenido se desea asignar a la   
 *            variable del módulo unigramas (unigramas).                       
 * \remarks Objetivo:                                                                   
 *          - Cargar los valores de las estructuras estáticas del módulo desde 
 *            el exterior.                                                     
 */

void Modelo_lenguaje::asigna_estructuras(Perfhash *est5, Perfhash *est4, Perfhash *est3,
                        Perfhash *est2, estructura_unigramas *uni,
                        int num_unigramas) {
/*
    estructura_hash_5 = est5;
    estructura_hash_4 = est4;
    estructura_hash_3 = est3;
    estructura_hash_2 = est2;
    unigramas = uni;
    numero_unigramas = num_unigramas;
*/
    pentagramas = est5;
    tetragramas = est4;
    trigramas = est3;
    bigramas = est2;
    unigramas = uni;
    numero_unigramas = num_unigramas;

}



/**
 * Función:   escribe_estructura_hash_interna                                  
 * \remarks Entrada:
 *			- opcion: indica la estructura_interna que se desea escribir en el 
 *            fichero.                                                         
 * \remarks Entrada y Salida:                                                           
 *          - fichero: fichero en el que se va a escribir la información.      
 * \remarks Valor devuelto:                                                             
 *          - Si la opción es válida (1, 2, 3, 4), devuelve un cero.           
 */

int Modelo_lenguaje::escribe_estructura_hash_interna(char opcion, FILE *fichero) {

    switch (opcion) {

        case 1:
            fwrite(&numero_unigramas, sizeof(int), 1, fichero);
            fwrite(unigramas, sizeof(estructura_unigramas), numero_unigramas, fichero);
            return 0;
        case 2:
            ph_escribe_hash(bigramas, fichero);
            return 0;
        case 3:
            ph_escribe_hash(trigramas, fichero);
            return 0;
        case 4:
            ph_escribe_hash(tetragramas, fichero);
            return 0;
        case 5:
            ph_escribe_hash(pentagramas, fichero);
            return 0;
        default:
            fprintf(stderr, "Error en escribe_estructura_hash_interna: opción de entrada no válida.\n");
            return 1;
    }

}


/**
 * Función:   crea_estructuras_hash                                            
 * \remarks Entrada:
 *			- ruta_datos: ruta de la base de datos.						   	   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error devuelve un cero.                           
 * \remarks Objetivo:  Carga en memoria las estructuras de tipo hash.                   
 */

int Modelo_lenguaje::crea_estructuras_hash(char *ruta_datos) {

    FILE *fichero;
    char nombre_fichero[FILENAME_MAX];


/*
 *    if (numero_modelos_cargados > 0 ) {
 *        numero_modelos_cargados++;
 *        return 0;
 *    }
 *
 */

    if (clases_ambiguedad == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_CLASES_AMBIGUEDAD);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&clases_ambiguedad, fichero))
            return 1;

        fclose(fichero);

    }
    if (pentagramas == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_5);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&pentagramas, fichero))
            return 1;

        fclose(fichero);

    }


    if (pentagramas_real == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_5_real);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&pentagramas_real, fichero))
            return 1;

        fclose(fichero);

    }

    if (tetragramas == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_4);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&tetragramas, fichero))
            return 1;

        fclose(fichero);

    }

    if (tetragramas_real == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_4_real);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&tetragramas_real, fichero))
            return 1;

        fclose(fichero);

    }

    if (trigramas == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_3);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&trigramas, fichero))
            return 1;

        fclose(fichero);

    }

    if (trigramas_real == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_3_real);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&trigramas_real, fichero))
            return 1;

        fclose(fichero);

    }


    if (bigramas == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_2);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&bigramas, fichero))
            return 1;

        fclose(fichero);

    }

    if (bigramas_real == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_HASH_2_real);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
            fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero hash %s.", nombre_fichero);
            return 1;
        }

        if (carga_hash(&bigramas_real, fichero))
            return 1;

        fclose(fichero);

    }

    if (unigramas == NULL) {

        strcpy(nombre_fichero, ruta_datos);
        strcat(nombre_fichero, FICHERO_UNIGRAMAS);

        if ( (fichero = fopen(nombre_fichero, "rb")) == NULL) {
			fprintf(stderr, "Error en crea_estructuras_hash, al intentar abrir el fichero de unigramas %s.\n",
            	nombre_fichero);
            return 1;
        }

        fread(&numero_unigramas, sizeof(int), 1, fichero);


        if ( (unigramas = (estructura_unigramas *) malloc(
                            (numero_unigramas + 1)*sizeof(estructura_unigramas))) == NULL) {
              puts("Error en crea_estructuras_hash, al asignar memoria.");
              return 1;
        }

        fread(unigramas, sizeof(estructura_unigramas), numero_unigramas, fichero);
        fclose(fichero);

    }

    numero_modelos_cargados++;
    return 0;

}


/**
 * Función:   carga_hash                                                       
 * \remarks Entrada:
 *			- fichero: fichero binario en el que se encuentra la información   
 *            de la tabla de probabilidades de n-gramas.                       
 * Entrada y salida:                                                           
 *          - estructura_hash: estructura de tipo Perfhash en la que se desea  
 *            almacenar la información de la tabla hash.                       
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, se devuelve un 0.                          
 * \remarks NOTA:    Si ya hay una tabla cargada, devuelve también un 0. Si se desea    
 *          cargar una nueva, es necesario llamar antes a libera_hash().       
 */

int Modelo_lenguaje::carga_hash(Perfhash **estructura_hash, FILE *fichero) {

    if (*estructura_hash != NULL)
        return 0;

    return ph_lee_hash(estructura_hash, fichero);

}


/**
 * Función:   libera_hash                                                      
 * \remarks Objetivo:  liberar los recursos relacionados con la tabla de probabilidades 
 *            del modelo de análisis morfosintáctico.                          
 */

void Modelo_lenguaje::libera_hash() {

    /*
     *if (numero_modelos_cargados > 1) {
     *    numero_modelos_cargados--;
     *    return;
     *}
     */
    if (clases_ambiguedad != NULL) {

        if (clases_ambiguedad->claves != NULL) {
            free(clases_ambiguedad->claves);
            clases_ambiguedad->claves = NULL;
        }

        ph_destruye_hash(clases_ambiguedad);

        clases_ambiguedad = NULL;

    }
    if (pentagramas != NULL) {

        if (pentagramas->claves != NULL) {
            free(pentagramas->claves);
            pentagramas->claves = NULL;
        }

        ph_destruye_hash(pentagramas);

        pentagramas = NULL;

    }


    if (pentagramas_real != NULL) {

        if (pentagramas_real->claves != NULL) {
            free(pentagramas_real->claves);
            pentagramas_real->claves = NULL;
        }

        ph_destruye_hash(pentagramas_real);

        pentagramas_real = NULL;

    }

    if (tetragramas != NULL) {

        if (tetragramas->claves != NULL) {
            free(tetragramas->claves);
            tetragramas->claves = NULL;
        }

        ph_destruye_hash(tetragramas);

        tetragramas = NULL;

    }


    if (tetragramas_real != NULL) {

        if (tetragramas_real->claves != NULL) {
            free(tetragramas_real->claves);
            tetragramas_real->claves = NULL;
        }

        ph_destruye_hash(tetragramas_real);

        tetragramas_real = NULL;

    }

    if (trigramas != NULL) {

        if (trigramas->claves != NULL) {
            free(trigramas->claves);
            trigramas->claves = NULL;
        }

        ph_destruye_hash(trigramas);

        trigramas = NULL;

    }

    if (trigramas_real != NULL) {

        if (trigramas_real->claves != NULL) {
            free(trigramas_real->claves);
            trigramas_real->claves = NULL;
        }

        ph_destruye_hash(trigramas_real);

        trigramas_real = NULL;

    }
    if (bigramas != NULL) {

        if (bigramas->claves != NULL) {
            free(bigramas->claves);
            bigramas->claves = NULL;
        }

        ph_destruye_hash(bigramas);

        bigramas = NULL;

    }

    if (bigramas_real != NULL) {

        if (bigramas_real->claves != NULL) {
            free(bigramas_real->claves);
            bigramas_real->claves = NULL;
        }

        ph_destruye_hash(bigramas_real);

        bigramas_real = NULL;

    }

    if (unigramas != NULL) {

        numero_unigramas = 0;
        free(unigramas);
        unigramas = NULL;
    }

    numero_modelos_cargados = 0;

}

/*******************************************************************************
 * Función:    calcula_prob_clase_ambiguedad
 * Entrada:  - clave: clave que indica el par clase de ambiguedad/cat_correcta
 *             cuya probabilidad se desea calcular.
 * Valor devuelto:
 *           - La probabilidad p(c_1,c_2,... ,c_n,c_correcta).
 *            - En caso de que no exista esa clase de ambiguedad, devuelve 1
 * NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.
 ******************************************************************************/

float Modelo_lenguaje::calcula_prob_clase_ambiguedad(unsigned char *clave) {

   int indice;
   //estructura_probabilidades_clase_ambiguedad *est_clase_ambiguedad;

   if ( (indice = ph_usa_hash(clases_ambiguedad, clave)) != -1) {
       //est_clase_ambiguedad = ((estructura_probabilidades_clase_ambiguedad *) clases_ambiguedad->claves) + indice;
       //return est_clase_ambiguedad->probabilidad;
			 return *(float *)(clases_ambiguedad->claves + indice * (clases_ambiguedad->dim  + sizeof(float)) + clases_ambiguedad->dim);
			 
   }

   return 1;

}

/**
 * Función:    calcula_prob_5_grama                                           
 * \remarks Entrada:
 *			- clave: clave que indica el pentagrama cuya probabilidad se     
 *             desea calcular.                                                
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w5,w4,w3,w2,w1).                             
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_5_grama(unsigned char *clave) {

   int indice;
   estructura_probabilidades_5 *est5;

   if ( (indice = ph_usa_hash(pentagramas_real, clave)) != -1) {
       est5 = ((estructura_probabilidades_5 *) pentagramas_real->claves) + indice;
       return est5->probabilidad;
   }

   return calcula_prob_w5_dados_w4_w3_w2_w1(clave) + calcula_prob_4_grama(clave);


}


/**
 * Función:    calcula_prob_4_grama                                           
 * \remarks Entrada:
 *			- clave: clave que indica el tetragrama cuya probabilidad se     
 *             desea calcular.                                                
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w4,w3,w2,w1).                                
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_4_grama(unsigned char *clave) {

    int indice;
    estructura_probabilidades_generica *est4;

    if ( (indice = ph_usa_hash(tetragramas_real, clave)) != -1) {
       est4 = ((estructura_probabilidades_generica *) tetragramas_real->claves) + indice;
       return est4->probabilidad;
    }

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

	return penalizacion_tetragramas + calcula_prob_w4_dados_w3_w2_w1(clave) + calcula_prob_3_grama(clave);

#else

	return PENALIZACION_4_GRAMAS + calcula_prob_w4_dados_w3_w2_w1(clave) + calcula_prob_3_grama(clave);

#endif
   //   return calcula_prob_w4_dados_w3_w2_w1(clave) + calcula_prob_3_grama(clave);

}



/**
 * Función:    calcula_prob_3_grama                                           
 * \remarks Entrada:
 *			- clave: clave que indica el trigrama cuya probabilidad se       
 *             desea calcular.                                                
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w3,w2,w1).                                   
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_3_grama(unsigned char *clave) {

    int indice;
    estructura_probabilidades_generica *est3;

    if ( (indice = ph_usa_hash(trigramas_real, clave)) != -1) {
        est3 = ((estructura_probabilidades_generica *) trigramas_real->claves) + indice;
        return est3->probabilidad;
    }

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

	return penalizacion_trigramas + calcula_prob_w3_dados_w2_w1(clave) + calcula_prob_2_grama(clave);

#else

	return PENALIZACION_3_GRAMAS + calcula_prob_w3_dados_w2_w1(clave) + calcula_prob_2_grama(clave);

#endif

}

/**
 * Función:    calcula_prob_2_grama                                           
 * \remarks Entrada:
 *			- clave: clave que indica el bigrama cuya probabilidad se        
 *             desea calcular.                                                
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w2,w1).                                      
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_2_grama(unsigned char *clave) {

    int indice;
    estructura_probabilidades_generica *est2;

    if ( (indice = ph_usa_hash(bigramas_real, clave)) != -1) {
        est2 = ((estructura_probabilidades_generica *) bigramas_real->claves) + indice;
        return est2->probabilidad;
    }

#ifdef _CALCULA_PENALIZACIONES_OPTIMAS

    return penalizacion_bigramas + calcula_prob_w2_dado_w1(clave) + unigramas[clave[0] - 1].probabilidad;

#else

    return PENALIZACION_2_GRAMAS + calcula_prob_w2_dado_w1(clave) + unigramas[clave[0] - 1].probabilidad;

#endif

}


/**
 * Función:    calcula_prob_1_grama                                           
 * \remarks Entrada:
 *			- clave: clave que indica el unigrama cuya probabilidad se       
 *             desea calcular.                                                
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w1).                                         
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_1_grama(unsigned char clave) {

    return unigramas[clave - 1].probabilidad;

}


/**
 * Función:    calcula_prob_w2_dado_w1                                        
 * \remarks Entrada:
 *			- clave: clave que indica el bigrama cuya probabilidad se desea  
 *             calcular.                                                      
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w2/w1).                                      
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_w2_dado_w1(unsigned char *clave) {

    int indice;
    estructura_probabilidades_2 *est2;

    if ( (indice = ph_usa_hash(bigramas, clave)) != -1) {
        est2 = ((estructura_probabilidades_2 *) bigramas->claves) + indice;
        return est2->probabilidad;
    }

    return unigramas[clave[0] - 1].back_off + unigramas[clave[1] - 1].probabilidad;

}


/**
 * Función:    calcula_prob_w3_dados_w2_w1                                    
 * \remarks Entrada:
 *			- clave: clave que indica el trigrama cuya probabilidad se desea 
 *             calcular.                                                      
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w3/w2,w1).                                   
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_w3_dados_w2_w1(unsigned char *clave) {

    int indice;
    estructura_probabilidades_3 *est3;
    estructura_probabilidades_2 *est2;

    if ( (indice = ph_usa_hash(trigramas, clave)) != -1) {
        est3 = ( (estructura_probabilidades_3 *) trigramas->claves) + indice;
        return est3->probabilidad;
    }

    if ( (indice = ph_usa_hash(bigramas, clave)) != -1) {
        est2 = ( (estructura_probabilidades_2 *) bigramas->claves) + indice;
        return est2->back_off + calcula_prob_w2_dado_w1(clave + 1);
    }

    return calcula_prob_w2_dado_w1(clave + 1);

}


/**
 * Función:    calcula_prob_w4_dados_w3_w2_w1                                 
 * \remarks Entrada:
 *			- clave: clave que indica el tetragrama cuya probabilidad se    
 *             desea  calcular.                                               
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w4/w3,w2,w1).                                
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_w4_dados_w3_w2_w1(unsigned char *clave) {

    int indice;
    estructura_probabilidades_4 *est4;
    estructura_probabilidades_3 *est3;

    if ( (indice = ph_usa_hash(tetragramas, clave)) != -1) {
        est4 = ( (estructura_probabilidades_4 *) tetragramas->claves) + indice;
        return est4->probabilidad;

    }

    if ( (indice = ph_usa_hash(trigramas, clave)) != -1) {
        est3 = ( (estructura_probabilidades_3 *) trigramas->claves) + indice;
        return est3->back_off + calcula_prob_w2_dado_w1(clave + 1);
    }

    return calcula_prob_w3_dados_w2_w1(clave + 1);

}


/**
 * Función:    calcula_prob_w5_dados_w4_w3_w2_w1                              
 * \remarks Entrada:
 *			- clave: clave que indica el pentagrama cuya probabilidad se     
 *             desea  calcular.                                               
 * \remarks Valor devuelto:                                                            
 *           - La probabilidad p(w5/w4,w3,w2,w1).                             
 * \remarks NOTA:       Se devuelve el logaritmo decimal de dicha probabilidad.        
 */

float Modelo_lenguaje::calcula_prob_w5_dados_w4_w3_w2_w1(unsigned char *clave) {

    int indice;
    estructura_probabilidades_5 *est5;
    estructura_probabilidades_4 *est4;

    if ( (indice = ph_usa_hash(pentagramas, clave)) != -1) {
        est5 = ( (estructura_probabilidades_5 *) pentagramas->claves) + indice;
        return est5->probabilidad;

    }


    if ( (indice = ph_usa_hash(tetragramas, clave)) != -1) {
        est4 = ( (estructura_probabilidades_4 *) tetragramas->claves) + indice;
        return est4->back_off + calcula_prob_w4_dados_w3_w2_w1(clave + 1);
    }

    return calcula_prob_w4_dados_w3_w2_w1(clave + 1);

}



