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

#include "descriptor.hpp"
#include "estadistica.hpp"

/**
 * Función:  Constructor de la clase Estadística                                    
 * Clase:    Estadística                                                            
 * \remarks Objetivo: Inicia las variables relativas al almacenamiento de la información.    
 */

Estadistica::Estadistica() {

  // Apuntamos la tabla a NULL, para que el resto de las funciones puedan operar según su valor.

  tabla = NULL;
  tamano = 0;
}

/**
 * Función:   anhade_dato                                                            
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad cuyo número de ocurrencias se desea actualizar.         
 *          - acento: información de tonicidad de la unidad.                         
 *          - contexto: posición de la unidad dentro de la frase.                    
 *          - proposicion: tipo de frase a la que pertenece la unidad.               
 * \remarks Valor devuelto:                                                                   
 *          - En ausencia de error devuelve 0.                                       
 * \remarks Objetivo: actualiza la base de datos de unidades. Si ya está incluida, aumenta su 
 *           número en una unidad. En caso contrario, crea espacio en la tabla para  
 *           ella.                                                                   
 */

int Estadistica::anhade_dato(char *unidad, char acento, char contexto, char proposicion) {

  t_tabla nuevo_elemento;
  t_tabla *auxiliar;
  // Creamos la unidad.

  strcpy(nuevo_elemento.unidad, unidad);

  for (int contador = strlen(unidad); contador < TAMANO_UNIDAD; contador++)
      nuevo_elemento.unidad[contador] = 0;

  nuevo_elemento.unidad[INDICE_ACENTO] = acento;
  nuevo_elemento.unidad[INDICE_CONTEXTO] = contexto;
  nuevo_elemento.unidad[INDICE_PROPOSICION] = proposicion;

  // Comprobamos si ya está en la base de datos.

  if (busca_unidad(nuevo_elemento.unidad, TAMANO_CLAVE)) {
     tamano++;

/*     if (tabla) {
        if ( (tabla = (t_tabla *) realloc(tabla, tamano*TAMANO_TABLA)) == NULL) {
           puts("Error al asignar memoria.");
           return 1;
        }
     }
     else {
        if ( (tabla = (t_tabla *) malloc(TAMANO_TABLA)) == NULL) {
           puts("Error al asignar memoria.");
           return 1;
        }
     }
     nuevo_elemento.contador = 1;
     tabla[tamano - 1] = nuevo_elemento;
*/
  // Hacemos esto en lugar de emplear sencillamente realloc debido a que según
                        // el memproof de vez en cuando perdíamos memoria.

     auxiliar = tabla;

     if ( (tabla = (t_tabla *) malloc(tamano*TAMANO_TABLA)) == NULL) {
        fprintf(stderr, "Error en anhade_dato, al asignar memoria.\n");
        return 1;
     }

     memcpy(tabla, auxiliar, (tamano - 1)*TAMANO_TABLA);
     nuevo_elemento.contador = 1;
     tabla[tamano - 1] = nuevo_elemento;

     if (auxiliar)
        free(auxiliar);

/*
     if ( (auxiliar = (t_tabla *) malloc(tamano*TAMANO_TABLA)) == NULL) {
        puts("Error en anhade_dato, al asignar memoria.");
        return 1;
     }
     if (tabla) {
        memcpy(auxiliar, tabla, (tamano - 1)*TAMANO_TABLA);
        free(tabla);
     }
     nuevo_elemento.contador = 1;
     auxiliar[tamano - 1] = nuevo_elemento;

     tabla = auxiliar;
     auxiliar = NULL;
*/
  }

  return 0;

}


/**
 * Función:   anhade_dato                                                            
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad cuyo número de ocurrencias se desea actualizar.         
 * \remarks Valor devuelto:                                                                   
 *          - En ausencia de error devuelve 0.                                       
 * \remarks Objetivo: actualiza la base de datos de unidades. Si ya está incluida, aumenta su 
 *           número en una unidad. En caso contrario, crea espacio en la tabla para  
 *           ella.                                                                   
 * \remarks NOTA:     Versión sobrecargada de la anterior, por compatibilidad hacia atrás.    
 */

int Estadistica::anhade_dato(char *unidad) {

    // Rellenamos con ceros, para poder hacer bien la comparación en busca unidad.

    for (int contador = strlen(unidad) + 1; contador < TAMANO_UNIDAD; contador++)
       unidad[contador] = 0;

   // Comprobamos si ya está en la base de datos.

  if (busca_unidad(unidad, TAMANO_UNIDAD)) {
     // En este caso, no estaba. Tenemos que asignar espacio para la nueva unidad.
     tamano++;
     if ( (tabla = (t_tabla *) realloc(tabla, tamano*TAMANO_TABLA)) == NULL) {
        fprintf(stderr, "Error en anhade_dato normal, al asignar memoria.\n");
        return 1;
     }
     tabla[tamano - 1].contador = 1;
     memcpy(tabla[tamano - 1].unidad, unidad, TAMANO_UNIDAD);
  }

  // La unidad ya estaba en la tabla. La propia función busca_unidad realiza el incremento, así que
  // aquí no tenemos nada más que hacer.

  return 0;

}


/**
 * Función:   busca_unidad                                                           
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad de la que se quiere comprobar su existencia en la tabla.
 *          - longitud: tamaño de la clave.                                          
 * \remarks Valor devuelto                                                                    
 *          - Devuelve 0 en caso de encontrar la unidad.                             
 * \remarks Objetivo:  Busca la unidad en su memoria. Si la tiene, incrementa su contador. En 
 *            caso contrario, avisa para que la función que la llama la añada a la   
 *            tabla.                                                                 
 */

int Estadistica::busca_unidad(char *unidad, int longitud) {

    char no_encontrado = 1;
    int contador = 0;
    t_tabla *correcaminos = tabla;

    if (tabla) { // La tabla está creada.
       // Recorremos la tabla
       while ( (contador++ < tamano) && no_encontrado ) {
          if (!memcmp(unidad, correcaminos->unidad, longitud)) {
             correcaminos->contador++;
             no_encontrado = 0;
          }
          correcaminos++;
       }
       return no_encontrado;
    }
    // la tabla no está inicializada.

    return 1;

}


/**
 * Función:   busca_unidad                                                           
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad de la que se quiere comprobar su existencia en la tabla.
 *          - longitud: tamaño de la clave.                                          
 *          - ocurrencias: número de ocurrencias que hay que añadir a la unidad.     
 * \remarks Valor devuelto                                                                    
 *          - Devuelve 0 en caso de encontrar la unidad.                             
 * \remarks Objetivo:  Busca la unidad en su memoria. Si la tiene, incrementa su contador.    
 *            según el valor de la variable ocurrencias.                             
 */

int Estadistica::busca_unidad(char unidad[], int longitud, int ocurrencias) {

    char no_encontrado = 1;
    int contador = 0;
    t_tabla *correcaminos = tabla;

    if (tabla) { // La tabla está creada.
       // Recorremos la tabla
       while ( (contador++ < tamano) && no_encontrado ) {
          if (!memcmp(unidad, correcaminos->unidad, longitud)) {
             correcaminos->contador += ocurrencias;
             no_encontrado = 0;
          }
          correcaminos++;
       }
       return no_encontrado;
    }
    // la tabla no está inicializada.

    return 1;

}


/**
 * Función:   busca_unidad                                                           
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad de la que se quiere comprobar su existencia en la tabla.
 *          - longitud: tamaño de la clave.                                          
 *	\remarks Salida:
 *			- ocurrencias: número de ocurrencias que hay que añadir a la unidad.     
 * \remarks Valor devuelto                                                                    
 *          - Devuelve 0 en caso de encontrar la unidad.                             
 * \remarks Objetivo:  Busca la unidad en su memoria. Si la tiene, devuelve su número de      
 *            ocurrencias.
 */

int Estadistica::busca_unidad(char unidad[], int longitud, int *ocurrencias) {

    char no_encontrado = 1;
    int contador = 0;
    t_tabla *correcaminos = tabla;

    if (tabla) { // La tabla está creada.
       // Recorremos la tabla
       while ( (contador++ < tamano) && no_encontrado ) {
          if (!memcmp(unidad, correcaminos->unidad, longitud)) {
             *ocurrencias = correcaminos->contador;
             no_encontrado = 0;
          }
          correcaminos++;
       }
       return no_encontrado;
    }
    // la tabla no está inicializada.

    return 1;

}


/**
 * Función:   busca_unidad_y_decrementa                                              
 * Clase:     Estadística                                                            
 * \remarks Entrada:
 *			- unidad: unidad de la que se quiere comprobar su existencia en la tabla.
 *          - longitud: tamaño de la clave.                                          
 *          - resta: número de ocurrencias que hay que añadir a la unidad.     
 * \remarks Valor devuelto                                                                    
 *          - Devuelve 0 en caso de encontrar la unidad.                             
 * \remarks Objetivo:  Busca la unidad en su memoria. Si la tiene, decrementa su valor según  
 *            la variable resta.                                                     
 */

t_tabla *Estadistica::busca_unidad_y_decrementa(char *unidad, int longitud, int resta) {

    int contador = 0;
    t_tabla *correcaminos = tabla;
    t_tabla *auxiliar;

    if (tabla) { // La tabla está creada.
       // Recorremos la tabla
       while (contador++ < tamano) {
          if (!memcmp(unidad, correcaminos->unidad, longitud)) {
             if ( (correcaminos->contador -= resta) <= 0) { // Ya tenemos las copias requeridas.
                                                            // Eliminamos el nodo.
                 for (int cuenta = contador - 1; cuenta < tamano - 1; cuenta++) {
                     *correcaminos = *(correcaminos + 1);
                     correcaminos++;
                 }
                 tamano--;

                 if (tamano) {
                    if ( (auxiliar = (t_tabla *) malloc(tamano*TAMANO_TABLA)) == NULL) {
                    	fprintf(stderr, "Error en busca_unidad_y_decrementa, al asignar memoria.\n");
	                    return NULL;
                    }
                    memmove(auxiliar, tabla, tamano*TAMANO_TABLA);
                    free(tabla);
                    tabla = auxiliar;
                    return tabla + contador - 1;
                 }
                 else {
                    free(tabla);
                    return correcaminos;
                 }
             }
             return correcaminos + 1;
          }
          correcaminos++;
       }
    }
    // la tabla no está inicializada.

    return NULL;

}


/**
 * Función:   escribe_datos_en_fichero                                              
 * Clase:     Estadística                                                           
 * \remarks Entrada y Salida:                                                                
 *          - fichero: fichero en el que se escribe la tabla con las unidades y su  
 *            número de ocurrencias.                                                
 * \remarks Valor devuelto:                                                                  
 *          - En caso de error al escribir en el fichero, devuelve un 1.            
 * \remarks Objetivo:  Escribir por fichero el contenido de la tabla de unidades y           
 *            ocurrencias.                                                          
 */

int Estadistica::escribe_datos_en_fichero(FILE *fichero) {

    if (fwrite(tabla, TAMANO_TABLA, tamano, fichero) == (unsigned int) tamano)
       return 0;
    return 1;

}


/**
 * Función:   lee_tabla_de_fichero_bin                                              
 * Clase:     Estadística                                                           
 * \remarks Entrada:
 *			- fichero: fichero binario en el que se encuentra almacenada la tabla   
 *            de unidades.                                                          
 * \remarks Valor devuelto:                                                                  
 *          - En caso de error devuelve un 1.                                       
 * \remarks Objetivo:  Cargar en memoria el resultado de otras ejecuciones, por si se desea  
 *            considerar el contenido de diferentes archivos de texto.              
 */

int Estadistica::lee_tabla_de_fichero_bin(FILE *fichero) {

    int longitud;

    // Calculamos el tamaño del fichero.

    fseek(fichero, 0, SEEK_END);
    longitud = ftell(fichero);

    // Comprobamos si el fichero no está vacío.

    if (longitud) {

       // Asignamos la memoria necesaria para la tabla. Longitud ya es el valor total en octetos.

       if ( (tabla = (t_tabla *) malloc(longitud)) == NULL) {
          fprintf(stderr, "Error en lee_tabla_de_fichero_bin, al asignar memoria.\n");
          return 1;
       }

       longitud /= TAMANO_TABLA; // Calculamos el número de elementos de la tabla.
       tamano = longitud;

       rewind(fichero); // Rebobinamos el fichero, para la siguiente lectura.

       // Leemos los datos del fichero.
       if ( (fread(tabla, TAMANO_TABLA, longitud, fichero)) != (unsigned int) longitud )
          return 1;

    }

    return 0;

}


/**
 * Función:   lee_tabla_de_fichero_txt                                              
 * Clase:     Estadística                                                           
 * \remarks Entrada:
 *			- fichero: fichero de texto en el que se encuentra almacenada la tabla  
 *            de unidades.                                                          
 * \remarks Valor devuelto:                                                                  
 *          - En caso de error devuelve un 1.                                       
 * \remarks Objetivo:  Cargar en memoria el resultado de otras ejecuciones, por si se desea  
 *            considerar el contenido de diferentes archivos de texto.              
 */

int Estadistica::lee_tabla_de_fichero_txt(FILE *fichero) {

    t_tabla *nuevo_elemento;
    int cuenta;
    char unidad[TAMANO_UNIDAD];
    unsigned char acento, contexto, proposicion;

    // Leemos el número de unidades:

    fscanf(fichero, "%d\n", &tamano);

    if ( (tabla = (t_tabla *) malloc(tamano*sizeof(t_tabla))) == NULL) {
       fprintf(stderr, "Error en lee_tabla_de_fichero_txt, al asignar memoria.\n");
       return 1;
    }
    nuevo_elemento = tabla;

    for (int contador = 0; contador < tamano; contador++) {
        fscanf(fichero, "%s %c %c %c %d\n", unidad, &acento, &proposicion, &contexto, &nuevo_elemento->contador);
        for (cuenta = 0; cuenta < TAMANO_UNIDAD; cuenta++)
            nuevo_elemento->unidad[cuenta] = '0';

        memcpy(nuevo_elemento->unidad, unidad, TAMANO_UNIDAD);

        if (acento == '1')
           nuevo_elemento->unidad[INDICE_ACENTO] = TONICA;
        else
           nuevo_elemento->unidad[INDICE_ACENTO] = ATONA;

        if (contexto == '0')
           nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_INICIAL;
        else
           if (contexto == '1')
              nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_MEDIA;
           else
               nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_FINAL;

        if (proposicion == '0')
           nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_ENUNCIATIVA;
        else
           if (proposicion == '1')
              nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_EXCLAMATIVA;
           else
              if (proposicion == '2')
                  nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_INTERROGATIVA;
              else
                  nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_INACABADA;

        nuevo_elemento++;
    }

    return 0;

}


/**
 * Función:   escribe_estadística                                                   
 * Clase:     Estadística                                                           
 * \remarks Entrada y Salida:                                                                
 *          - fichero: fichero de texto en el que se escribe la salida.             
 * \remarks Objetivo:  Escribir, de forma legible por el usuario, el resultado de la         
 *            ejecución del programa sobre un texto.                                
 * \remarks NOTA:      Esta función actualmente sólo sirve para el tipo de clave que estamos 
 *            empleando (fonema + acento + contexto + proposición)                  
 */

void Estadistica::escribe_estadistica(FILE *fichero) {

    t_tabla *correcaminos = tabla;
//    int acento, proposicion = -1, contexto = -1;

//    fprintf(fichero, "Unidad\tAcento\tProposición\tPosición\tNúmero\n");
    fprintf(fichero, "Unidad\tNúmero\n");
    
    for (int contador = 0; contador < tamano; contador++) {
/*
        if (correcaminos->unidad[INDICE_ACENTO])
           acento = 1;
        else
           acento = 0;
        switch (correcaminos->unidad[INDICE_PROPOSICION]) {
        case FRASE_ENUNCIATIVA:
           proposicion = 0;
           break;
        case FRASE_EXCLAMATIVA:
           proposicion = 1;
           break;
        case FRASE_INTERROGATIVA:
           proposicion = 2;
           break;
        case FRASE_INACABADA:
           proposicion = 3;
        }
        switch (correcaminos->unidad[INDICE_CONTEXTO]) {
        case POSICION_INICIAL:
           contexto = 0;
           break;
        case POSICION_MEDIA:
           contexto = 1;
           break;
        case POSICION_FINAL:
           contexto = 2;
        }

        fprintf(fichero, "%s\t%d\t%d\t%d\t%d\n", correcaminos->unidad, acento,
                         proposicion, contexto, correcaminos->contador);
*/
        fprintf(fichero, "%s\t%d\n", correcaminos->unidad, correcaminos->contador);
        correcaminos++;
    }
}

/**
 * Función:   escribe_estadística_generico                                          
 * Clase:     Estadística                                                           
 * \remarks Entrada y Salida:                                                                
 *          - fichero: fichero de texto en el que se escribe la salida.             
 * \remarks Objetivo:  Escribir, de forma legible por el usuario, el resultado de la         
 *            ejecución del programa sobre un texto.                                
 * \remarks NOTA:      Se diferencia de la función anterior en que no se hace ningún tipo de 
 *            suposición acerca del formato de la clave (sólo que acabe en NULL).   
 *            Simplemente se escribe su contenido y su número de ocurrencias.       
 */

void Estadistica::escribe_estadistica_generico(FILE *fichero) {

    t_tabla *correcaminos = tabla;

    fprintf(fichero, "Unidad\tNúmero\n");

    for (int contador = 0; contador < tamano; contador++) {
        fprintf(fichero, "%s\t%d\n", correcaminos->unidad, correcaminos->contador);
        correcaminos++;
    }
}

/**
 * Función:   lee_estadística_generico                                              
 * Clase:     Estadística                                                           
 * \remarks Entrada:
 *			- fichero: fichero de texto del que se leen las estadísticas.         
 * \remarks Objetivo:  Leer el resultado de otras ejecuciones del programa.                  
 * \remarks Valor devuelto:                                                                  
 *          - En ausencia de error, devuelve un 0.                                  
 * \remarks NOTA:      No se hace ninguna suposición acerca del formato concreto de la clave.
 */

int Estadistica::lee_estadistica_generico(FILE *fichero) {

    t_tabla *correcaminos;
    int contador;
    char cadena1[30], cadena2[30];

    int tamano_reservado = 4;

    if ( (tabla = (t_tabla *) malloc(tamano_reservado*TAMANO_TABLA)) == NULL) {
        fprintf(stderr, "Error en lee_estadistica_generico, al asignar memoria.\n");
        return 1;
    }

    // Eliminamos la cabecera:

    fscanf(fichero, "%s %s\n", cadena1, cadena2);

    correcaminos = tabla;

    while (!feof(fichero)) {
      fscanf(fichero, "%s\t%d\n", correcaminos->unidad, &correcaminos->contador);
      for (contador = strlen(correcaminos->unidad) + 1; contador < TAMANO_UNIDAD; contador++)
        correcaminos->unidad[contador] = 0;
      if (++tamano >= tamano_reservado) {
        tamano_reservado += 4;
        if ( (tabla = (t_tabla  *) realloc(tabla, tamano_reservado*TAMANO_TABLA)) == NULL) {
          fprintf(stderr, "Error en lee_estadistica_generico, al asignar memoria.\n");
          return 1;
        }
        correcaminos = tabla + tamano;
      }
      else
        correcaminos++;
    }

    return 0;

}


/**
 * Función:   devuelve_tamano                                                        
 * Clase:     Estadistica                                                            
 * \remarks Valor devuelto:                                                                   
 *          - El número de unidades contenidas en la memoria.                        
 */

int Estadistica::devuelve_tamano() {

    return tamano;

}


/**
 * Función:   sobrecarga del operador +=                                             
 * Clase:     Estadistica                                                            
 * \remarks Entrada:
 *			- sumando: objeto Estadistica cuya información se desea añadir a la del  
 *            original.                                                              
 * \remarks Valor devuelto:                                                                   
 *          - Objeto con toda la información.                                        
 * \remarks Objetivo:  Permite hacer una búsqueda de unidades en un texto y luego añadir la   
 *            información recogida si se desea a la que ya teníamos.                 
 * \remarks NOTA:      La salida "poco elegante" si falla el malloc() hace que sea más        
 *            aconsejable emplear la otra función de suma. (suma_estadisticas)       
 */

Estadistica Estadistica::operator +=(Estadistica sumando) {

   if (sumando.tabla) {
      t_tabla *correcaminos = sumando.tabla;
      t_tabla nuevo_elemento;
      for (int contador = 0; contador < sumando.tamano; contador++) {
          if (busca_unidad(correcaminos->unidad, TAMANO_CLAVE, correcaminos->contador)) {
              tamano++;
              t_tabla *auxiliar; // Hacemos esto en lugar de emplear sencillamente realloc debido a que según
                        // el memproof de vez en cuando perdíamos memoria.
              if ( (auxiliar = (t_tabla *) malloc(tamano*TAMANO_TABLA)) == NULL) {
                 puts("Error en operator+=, al asignar memoria.");
                 getchar();
                 return *this; // Esto no me convence nada.
              }
              memmove(auxiliar, tabla, (tamano - 1)*TAMANO_TABLA);
              memcpy(nuevo_elemento.unidad, correcaminos->unidad, TAMANO_CLAVE);
              nuevo_elemento.contador = correcaminos->contador;
              auxiliar[tamano - 1] = nuevo_elemento;
              if (tabla)
                 free(tabla);
              tabla = auxiliar;
          }
          correcaminos++;
      }
   }

   return *this;

}


/**
 * Función:   suma_estadisticas                                                      
 * Clase:     Estadistica                                                            
 * \remarks Entrada:
 *			- sumando: objeto Estadistica cuya información se desea añadir a la del  
 *            original.                                                              
 * \remarks Valor devuelto:                                                                   
 *          - En ausencia de error se devuelve un 0.                                 
 * \remarks Objetivo:  Permite hacer una búsqueda de unidades en un texto y luego añadir la   
 *            información recogida si se desea a la que ya teníamos.                 
 * \remarks NOTA:      En esta función se arregla el problema de la sobrecarga del operador   
 *            +=.                                                                    
 */

int Estadistica::suma_estadisticas(Estadistica sumando) {

    if (sumando.tabla) {
      t_tabla *correcaminos = sumando.tabla;
      t_tabla nuevo_elemento;
      for (int contador = 0; contador < sumando.tamano; contador++) {
          if (busca_unidad(correcaminos->unidad, TAMANO_CLAVE, correcaminos->contador)) {
              tamano++;
              t_tabla *auxiliar; // Hacemos esto en lugar de emplear sencillamente realloc debido a que según
                        // el memproof de vez en cuando perdíamos memoria.
              if ( (auxiliar = (t_tabla *) malloc(tamano*TAMANO_TABLA)) == NULL) {
                 fprintf(stderr, "Error en suma_estadistica, al asignar memoria.\n");
                 return 1;
              }
              memmove(auxiliar, tabla, (tamano - 1)*TAMANO_TABLA);
              memcpy(nuevo_elemento.unidad, correcaminos->unidad, TAMANO_CLAVE);
              nuevo_elemento.contador = correcaminos->contador;
              auxiliar[tamano - 1] = nuevo_elemento;
              if (tabla)
                 free(tabla);
              tabla = auxiliar;
          }
          correcaminos++;
      }
   }

   return 0;

}


/**
 * Función:   existe_lista                                                           
 * Clase:     Estadistica                                                            
 * \remarks Entrada:
 *			- lista_unidades: objeto de tipo Estadistica en el que se recogen las    
 *            unidades que se desean encontrar.                                      
 *	\remarks Salida:
 *			- lista: cadena con las unidades que se encontraron.                     
 *          - tamano_lista: número de unidades de la lista.                          
 * \remarks Valor devuelto:                                                                   
 *          - En ausencia de error se devuelve un 0.                                 
 * \remarks Objetivo:  Crear una cadena con todas las unidades de lista_unidades que se han   
 *            encontrado.                                                            
 */

int Estadistica::existe_lista(Estadistica *lista_unidades, unidad_extendida **lista,
                              int *tamano_lista) {

    t_tabla *correcaminos = lista_unidades->tabla;
    int numero_ocurrencias = 0;
    unidad_extendida nuevo_nodo;
    unidad_extendida *auxiliar;
    unsigned char *puntero;

    for (int contador = 0; contador < lista_unidades->tamano; contador++) {
        if (!busca_unidad(correcaminos->unidad, TAMANO_CLAVE, &numero_ocurrencias)) {
            // Se ha encontrado la unidad. La añadimos a lista.
            (*tamano_lista)++;
            if ( (auxiliar = (unidad_extendida *) malloc(*tamano_lista*sizeof(unidad_extendida)))
                 == NULL) {
                fprintf(stderr, "Error en existe_lista, al asignar memoria.\n");
                return 1;
            }
            if (*lista != NULL) {
               memmove(auxiliar, *lista, (*tamano_lista - 1)*sizeof(unidad_extendida));
               free(*lista);
            }
            memcpy(nuevo_nodo.unidad, correcaminos->unidad, TAMANO_UNIDAD);
            puntero = (unsigned char *) &(correcaminos->unidad[INDICE_ACENTO]);
            nuevo_nodo.acento = *puntero++;
            nuevo_nodo.proposicion = *puntero++;
            nuevo_nodo.contexto = *puntero;
            auxiliar[*tamano_lista - 1] = nuevo_nodo;
            *lista = auxiliar;

            // Ahora tenemos que decrementar el contador de lista_unidades.

            if ( (correcaminos = lista_unidades->busca_unidad_y_decrementa(correcaminos->unidad,
                 TAMANO_CLAVE, numero_ocurrencias)) == NULL)  {
                 fprintf(stderr, "Error en existe_lista, al asignar memoria.\n");
                 return 1;
            }

        }
        else
            correcaminos++; // Si se libera arriba el último nodo, estaríamos accediendo a memoria
                        // liberada, pero no sucede nada.
    }

    return 0;

}

/**
 * Función:   lee_tabla_de_fichero_corpus                                           
 * Clase:     Estadística                                                           
 * \remarks Entrada:
 *			- fichero: fichero de texto en el que se encuentra almacenada la tabla  
 *            de unidades.                                                          
 * \remarks Valor devuelto:                                                                  
 *          - En caso de error devuelve un 1.                                       
 * \remarks Objetivo:  Cargar en memoria la tabla de unidades a partir de la cual se desea   
 *            generar el corpus.                                                    
 */

int Estadistica::lee_tabla_de_fichero_corpus(FILE *fichero) {

    t_tabla *nuevo_elemento;
    int cuenta;
    char unidad[TAMANO_UNIDAD], cadena1[10], cadena2[20];
    unsigned char acento, contexto, proposicion;
    int tamano_actual;

    // Comprobamos si la tabla ya ha sido creada.

    if (!tabla) {

       // Asignamos inicialmente una memoria de 100 posiciones. Luego actualizamos.
       tamano = 100;
       if ( (tabla = (t_tabla *) malloc(tamano*sizeof(t_tabla))) == NULL) {
          fprintf(stderr, "Error en lee_tabla_de_fichero_txt, al asignar memoria.\n");
          return 1;
       }
       nuevo_elemento = tabla;
       tamano_actual = 0;
    }
    else {
       tamano_actual = tamano;
       tamano += 100;
       if ( (tabla = (t_tabla *) realloc(tabla, tamano*TAMANO_TABLA)) == NULL) {
          fprintf(stderr, "Error en lee_tabla_de_fichero_txt, al asignar memoria.\n");
          return 1;
       }
       nuevo_elemento = tabla + tamano_actual;
    }

    // Vamos añadiendo los elementos.

    while (!feof(fichero)) {

        if (!fscanf(fichero, "%s %c %c %c %s %s\n", unidad, &acento, &proposicion, &contexto,
                                               cadena1, cadena2))
            break;

        // Creamos la unidad.

        nuevo_elemento->contador = 1;

        for (cuenta = 0; cuenta < TAMANO_UNIDAD; cuenta++)
            nuevo_elemento->unidad[cuenta] = '0';

        memcpy(nuevo_elemento->unidad, unidad, TAMANO_UNIDAD);

        if (acento == '1')
           nuevo_elemento->unidad[INDICE_ACENTO] = TONICA;
        else
           nuevo_elemento->unidad[INDICE_ACENTO] = ATONA;

        if (contexto == '0')
           nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_INICIAL;
        else
           if (contexto == '1')
              nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_MEDIA;
           else
               nuevo_elemento->unidad[INDICE_CONTEXTO] = POSICION_FINAL;

        if (proposicion == '0')
           nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_ENUNCIATIVA;
        else
           if (proposicion == '1')
              nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_EXCLAMATIVA;
           else
              if (proposicion == '2')
                 nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_INTERROGATIVA;
              else
                 nuevo_elemento->unidad[INDICE_PROPOSICION] = FRASE_INACABADA;


        if (++tamano_actual >= tamano) { // Tenemos que reasignar memoria.
           tamano += 100;
           if ( (tabla = (t_tabla *) realloc(tabla, tamano*TAMANO_TABLA)) == NULL) {
              fprintf(stderr, "Error en lee_tabla_de_fichero_corpus, al reasignar memoria.\n");
              return 1;
           }
           nuevo_elemento = tabla + tamano_actual;
        }
        else
           nuevo_elemento++;

    }
    
    if (tamano_actual != tamano) {
       tamano = tamano_actual;
       if ( (tabla = (t_tabla *) realloc(tabla, tamano*TAMANO_TABLA)) == NULL) {
          fprintf(stderr, "Error en lee_tabla_de_fichero_corpus, al reasignar memoria.\n");
          return 1;
       }
    }

    return 0;

}


/**
 * Función:   libera_memoria                                                         
 * Clase:     Estadistica                                                            
 * \remarks Objetivo:  Liberar la memoria en la que se almacenaba la información del objeto.  
 */

void Estadistica::libera_memoria() {

     if (tabla)
        free(tabla);

}
