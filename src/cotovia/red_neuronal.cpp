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
 


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util_neuronal.hpp"
#include "red_neuronal.hpp"


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                    
/// Función:   Red_neuronal                                                    
/////////////////////////////////////////////////////////////////////////////

Red_neuronal::Red_neuronal() {

    entrada = NULL;
    conjunto_pesos = NULL;
    numero_neuronas = NULL;
    capa_neuronas = NULL;
    
    numero_ejemplos = 0;
    error_anterior = 0.0;
    error_medio = 0.0;
        
}

/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                    
/// Función:   inicia_red
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::inicia_red() {

    entrada = NULL;
    conjunto_pesos = NULL;
    numero_neuronas = NULL;
    capa_neuronas = NULL;
    
    numero_ejemplos = 0;
    error_anterior = 0.0;
    error_medio = 0.0;
        
}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                      
/// Función:   devuelve_error_medio_red                                        
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::devuelve_salida_red(double *salida) {

    int contador;
    Neurona *apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas]; contador++,
                                    salida++, apunta_neurona++)
        *salida = apunta_neurona->salida;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                    
/// Función:   devuelve_error_medio_red                                        
/////////////////////////////////////////////////////////////////////////////

double Red_neuronal::devuelve_error_medio_red() {

    return error_medio;
    
}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                    
/// Función:   lee_estructura_perceptron_multicapa
/// Entrada: - nombre_fichero: nombre del fichero en el que se encuentra       
///            definida la estructura del perceptrón.                          
///          - semilla: semilla para el generador de números pseudoaleatorios. 
///          - inicializa_pesos: distribución empleada para inicializar los    
///            pesos (NORMAL, UNIFORME o CONSTANTE)
/// Salida:  - tamano_vector_entrada: número de componentes del vector de      
///            entrada a la red.                                               
///          - tamano_vector_salida: número de salidas de la red.              
/// Valor devuelto:                                                            
///          - En ausencia de error, devuelve un cero.                         
/////////////////////////////////////////////////////////////////////////////
int Red_neuronal::lee_estructura_perceptron_multicapa(char *nombre_fichero, int semilla,
                                        int *tamano_vector_entrada,
                                        int *tamano_vector_salida,
                                        int inicializa_pesos) {

    int contador, cuenta, cuenta_2;
    int numero,  capa_actual;
    double *apunta_pesos, *apunta_pesos_nulos, *apunta_correccion;
    char cadena_auxiliar[1000];
    Capa_neuronas apunta_capa;
    Neurona *apunta_neurona;
    Nodo_entrada *apunta_entrada;
    FILE *fichero;

    if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
        fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al abrir el fichero %s.\n",
                nombre_fichero);

        return 1;
    } // if ( (fichero = ...

    // Leemos el número de capas ocultas:

    fscanf(fichero, "%d\n", &numero_capas_ocultas);

    if ( (numero_neuronas = (int *) malloc((numero_capas_ocultas + 1)*sizeof(int)))
          == NULL) {
        fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al asignar memoria.\n");
        return 1;
    } // if ( (numero_neuronas = ...

    // Leemos el número de entradas:

    fscanf(fichero, "%d ", &numero_entradas);

    // Ahora, el número de neuronas de cada capa, incluida la de salida:

    for (contador = 0; contador <= numero_capas_ocultas; contador++)
        fscanf(fichero, "%d ", &numero_neuronas[contador]);

    *tamano_vector_entrada = numero_entradas;
    *tamano_vector_salida = numero_neuronas[numero_capas_ocultas];

    if ( (entrada = (Nodo_entrada *) malloc(numero_entradas*sizeof(Nodo_entrada)))
          == NULL) {
        fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
        return 1;
    } // if ( (entradas = ...

    srand(semilla);

    apunta_entrada = entrada;

    for (contador = 0; contador < numero_entradas; contador++, apunta_entrada++) {

        if ( (apunta_entrada->pesos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

        if ( (apunta_entrada->correccion_pesos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

        if ( (apunta_entrada->pesos_nulos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

        apunta_pesos = apunta_entrada->pesos;

        apunta_pesos_nulos = apunta_entrada->pesos_nulos;

        apunta_correccion = apunta_entrada->correccion_pesos;

        for (cuenta_2 = 0; cuenta_2 < numero_neuronas[0]; cuenta_2++) {

            switch (inicializa_pesos) {
            case NORMAL:
                *apunta_pesos++ = genera_variable_normal(0.0, 1.0);
                break;
            case UNIFORME:
                *apunta_pesos++ = ( (double) rand() / RAND_MAX) - 0.5;
                break;
            case CONSTANTE:
                *apunta_pesos++ = 1.0;
                break;
            default:
                fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: Valor de \
                        inicializa_pesos (%d) no válido.\n", inicializa_pesos);
                return 1;
            } // switch (inicializa_pesos)

            *apunta_correccion++ = 0.0;
            *apunta_pesos_nulos++ = 1.0;
        } // for (cuenta_2 = 0; ...

    } // for (contador = 0; contador < numero_entradas ...

    // Reservamos memoria para las capas de neuronas:

    if ( (capa_neuronas = (Neurona **) malloc((numero_capas_ocultas + 1)*sizeof(Neurona *)))
          == NULL) {
        fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al asignar memoria.\n");
        return 1;
    } // if ( (capa_neuronas = ...

    for (contador = 0; contador < numero_capas_ocultas; contador ++) {

        if ( (capa_neuronas[contador] = (Neurona *) malloc(numero_neuronas[contador]*sizeof(Neurona)))
              == NULL) {
            fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
            return 1;
        } // if ( (capa_neuronas[0] ...

        apunta_neurona = capa_neuronas[contador];

        for (cuenta = 0; cuenta < numero_neuronas[contador]; cuenta++, apunta_neurona++) {

			switch (inicializa_pesos) {
		    case NORMAL:
		        apunta_neurona->peso_0 = genera_variable_normal(0.0, 1.0);
		        break;
		    case UNIFORME:
		        apunta_neurona->peso_0 = ( (double) rand() / RAND_MAX) - 0.5;
		        break;
		    case CONSTANTE:
		        apunta_neurona->peso_0 = 1.0;
		        break;
		    default:
	    	    fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: Valor de \
		                inicializa_pesos (%d) no válido.\n", inicializa_pesos);
		        return 1;
		    } // switch (inicializa_pesos)

//            apunta_neurona->peso_0 = ( (double) rand() / RAND_MAX) - 0.5;
//            apunta_neurona->peso_0 = 1.0;
            apunta_neurona->correccion = 0.0;
//            apunta_neurona->funcion_activacion = lineal;
            apunta_neurona->funcion_activacion = sigmoide;
            apunta_neurona->funcion_derivada = derivada_sigmoide;
//            apunta_neurona->funcion_derivada = derivada_lineal;
            apunta_neurona->parametro = PARAMETRO_SIGMOIDE;

            if ( (apunta_neurona->pesos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

            apunta_pesos = apunta_neurona->pesos;

            if ( (apunta_neurona->correccion_pesos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

            apunta_correccion = apunta_neurona->correccion_pesos;

            if ( (apunta_neurona->pesos_nulos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

            apunta_pesos_nulos = apunta_neurona->pesos_nulos;

            for (cuenta_2 = 0; cuenta_2 < numero_neuronas[contador + 1]; cuenta_2++) {

                switch (inicializa_pesos) {
                case NORMAL:
                    *apunta_pesos++ = genera_variable_normal(0.0, 1.0);
                    break;
                case UNIFORME:
                    *apunta_pesos++ = ( (double) rand() / RAND_MAX) - 0.5;
                    break;
                case CONSTANTE:
                    *apunta_pesos++ = 1.0;
                    break;
                default:
                    fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: Valor de \
                            inicializa_pesos (%d) no válido.\n", inicializa_pesos);
                    return 1;
                } // switch (inicializa_pesos)

                *apunta_correccion++ = 0.0;
                *apunta_pesos_nulos++ = 1.0;
            } // for (cuenta_2 = 0; ...

        } // for (cuenta = 0; ...

    } // for (contador = 0; contador <= NUMERO_CAPAS...

    // Ahora, la capa de salida:

    if ( (capa_neuronas[numero_capas_ocultas] = (Neurona *)
                malloc(numero_neuronas[numero_capas_ocultas]*sizeof(Neurona)))
        == NULL) {
        fprintf(stderr, "Error en lee_estructura_perceptron_multicapa, al reservar memoria.\n");
        return 1;
    } // if ( (capa_neuronas[0] ...

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

	for (cuenta = 0; cuenta < numero_neuronas[numero_capas_ocultas]; cuenta++, apunta_neurona++) {

	    switch (inicializa_pesos) {
	    case NORMAL:
	        apunta_neurona->peso_0 = genera_variable_normal(0.0, 1.0);
	        break;
	    case UNIFORME:
	        apunta_neurona->peso_0 = ( (double) rand() / RAND_MAX) - 0.5;
	        break;
	    case CONSTANTE:
	        apunta_neurona->peso_0 = 1.0;
	        break;
	    default:
    	    fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: Valor de \
	                inicializa_pesos (%d) no válido.\n", inicializa_pesos);
	        return 1;
	    } // switch (inicializa_pesos)


    	apunta_neurona->parametro = PENDIENTE_RECTA;
	    apunta_neurona->funcion_activacion = lineal;
	    apunta_neurona->funcion_derivada = derivada_lineal;
	/*
		apunta_neurona->parametro = PARAMETRO_TANGENTE_HIPERBOLICA;
	    apunta_neurona->funcion_activacion = tangente_hiperbolica;
	    apunta_neurona->funcion_derivada = derivada_tangente_hiperbolica;
	*/
	    apunta_neurona->pesos = NULL;
	    apunta_neurona->correccion_pesos = NULL;
	    apunta_neurona->pesos_nulos = NULL;
	    apunta_neurona->correccion = 0.0;

	} // for (cuenta = 0; ...

    fgets(cadena_auxiliar, 1000, fichero);

//    fscanf(fichero, "%s %d\n", cadena_auxiliar, &numero);
    fscanf(fichero, "%s %d", cadena_auxiliar, &numero);

    if (feof(fichero)) {
    // La red está totalmente interconectada entre capas consecutivas.
        fclose (fichero);
        return 0;
    } // if (feof(fichero))

    if (strcmp(cadena_auxiliar, "Capa") != 0)
        do  {
            if (strcmp(cadena_auxiliar, "Entrada") == 0) {

                if (numero >= numero_entradas) {
                    fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: Entrada(%d) \
                        errónea.\n", numero);
                    return 1;
                } // if (numero >= numero_entradas);

                apunta_pesos_nulos = entrada[numero].pesos_nulos;

                fscanf(fichero, "%d\n", &numero);

                while (numero >= 0) {
                    if (numero >= numero_neuronas[0]) {
                        fprintf(stderr, "Error en lee_estructura_perceptrón_multicapa: Formato de fichero erróneo.\n");
                        return 1;
                    } // if (numero >= numero_neuronas[capa_actual + 1])
                    apunta_pesos_nulos[numero] = 0.0;
                    fscanf(fichero, "%d\n", &numero);
                } // while (numero >= 0)

            } // if (strcmp(cadena_auxiliar, "Entrada") == 0)

            fscanf(fichero, "%s %d\n", cadena_auxiliar, &numero);

            if (feof(fichero))
                break;

        } while(strcmp(cadena_auxiliar, "Entrada") == 0);


    do {

        if (feof(fichero))
            break;

        if (strcmp(cadena_auxiliar, "Capa") != 0) {
            fprintf(stderr, "Error en lee_estructura_perceptrón_multicapa: Formato de fichero erróneo.\n");
            return 1;
        } // if (strcmp(cadena_auxiliar, "Capa") == 0)

        if (numero > numero_capas_ocultas) {
        	fprintf(stderr, "Error en lee_estructura_perceptron_multicapa: índice de capa mayor que el \
número de capas.\n");
			return 1;
        } // if (numero_capas_ocultas)

        capa_actual = numero;

        apunta_capa = capa_neuronas[numero];

        fscanf(fichero, "%s %d\n", cadena_auxiliar, &numero);

        do {

            if (feof(fichero))
                break;

            if (strcmp(cadena_auxiliar, "Capa") == 0) {
//                apunta_capa = capa_neuronas[numero];
                break;
            } // if (strcmp(cadena_auxiliar, "Capa")...

            if (strcmp(cadena_auxiliar, "Neurona") == 0) {
                apunta_neurona = apunta_capa + numero;
                fscanf(fichero, "%s", cadena_auxiliar);
                if (strcmp(cadena_auxiliar, "Función") == 0) {
                    fscanf(fichero, "%s\n", cadena_auxiliar);
                    if (asigna_funcion(cadena_auxiliar, apunta_neurona))
                        return 1;
                    if (feof(fichero))
                        break;
                    fscanf(fichero, "%s", cadena_auxiliar);
                    if (feof(fichero))
                        break;
                    if (strcmp(cadena_auxiliar, "Capa") == 0) {
                        fscanf(fichero, "%d\n", &numero);
                        break;
                    } // if (strcmp(cadena_auxiliar, "Capa") == 0)
                    if (strcmp(cadena_auxiliar, "Neurona") == 0) {
                        fscanf(fichero, "%d\n", &numero);
                        continue;
                    } // if (strcmp(cadena_auxiliar, "Neurona") == 0)
                    numero = atoi(cadena_auxiliar);
                } // if (strcmp(cadena_auxiliar, "Función")
                else
                    numero = atoi(cadena_auxiliar);
            } // if (strcmp(cadena_auxiliar, "Neurona") == 0)
            else {
                fprintf(stderr, "Error en lee_estructura_perceptrón_multicapa: Formato de fichero erróneo.\n");
                return 1;
            } // else


            apunta_pesos_nulos = apunta_neurona->pesos_nulos;

            while (numero >= 0) {
                if (numero >= numero_neuronas[capa_actual + 1]) {
                    fprintf(stderr, "Error en lee_estructura_perceptrón_multicapa: Formato de fichero erróneo.\n");
                    return 1;
                } // if (numero >= numero_neuronas[capa_actual + 1])
                apunta_pesos_nulos[numero] = 0.0;
                fscanf(fichero, "%d\n", &numero);
            } // while (numero >= 0)

            fscanf(fichero, "%s %d\n", cadena_auxiliar, &numero);

        } while (1);

    } while (feof(fichero) == 0);

    fclose(fichero);

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   asigna_funcion
/// Entrada: - nombre_funcion: nombre de la función de activación que se desea
///            asignar a la neurona.
///          - neurona: nodo al que se le quiere asignar cierta función de
///            activación.
/// Valor devuelto:
///          - En ausencia de error, devuelve un cero.
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::asigna_funcion(char *nombre_funcion, Neurona *neurona) {

    if (strcmp(nombre_funcion, "lineal") == 0) {
        neurona->parametro = PENDIENTE_RECTA;
        neurona->funcion_activacion = lineal;
        neurona->funcion_derivada = derivada_lineal;
        return 0;
    } // if (strmcp(nombre_funcion, "lineal")

    if (strcmp(nombre_funcion, "sigmoide") == 0) {
        neurona->parametro = PARAMETRO_SIGMOIDE;
        neurona->funcion_activacion = sigmoide;
        neurona->funcion_derivada = derivada_sigmoide;
        return 0;
    } // if (strcmp(nombre_funcion, "sigmoide")

    if (strcmp(nombre_funcion, "tangente") == 0) {
    	neurona->parametro = PARAMETRO_TANGENTE_HIPERBOLICA;
        neurona->funcion_activacion = tangente_hiperbolica;
        neurona->funcion_derivada = derivada_tangente_hiperbolica;
        return 0;
    } // if (strcmp(nombre_funcion, "tangente")

    fprintf(stderr, "Error en asigna_funcion. Función (%s) no contemplada.\n", nombre_funcion);

    return 1;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   libera_memoria_red
/// Objetivo:  Liberar los recursos asignados a la red.
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::libera_memoria_red() {

    int contador;
    Neurona *apunta_neurona;
    Nodo_entrada  *apunta_entradas;

    if (entrada != NULL) {
        apunta_entradas = entrada;
        for (contador = 0; contador < numero_entradas; contador++, apunta_entradas++) {
            free(apunta_entradas->pesos);
            free(apunta_entradas->correccion_pesos);
            free(apunta_entradas->pesos_nulos);
        } // for (contador = 0; contador < numero_entradas...
        free(entrada);
	entrada = NULL;
    } // if (apunta_entradas != NULL)

 
    if (capa_neuronas != NULL) {
        for (contador = 0; contador < numero_capas_ocultas; contador++) {
            apunta_neurona = capa_neuronas[contador];
            for (int cuenta = 0; cuenta < numero_neuronas[contador]; cuenta++, apunta_neurona++) {
                free(apunta_neurona->pesos);
                free(apunta_neurona->correccion_pesos);
                free(apunta_neurona->pesos_nulos);
            } // for (int cuenta = 0; ...

            free(capa_neuronas[contador]);
        } // for (int contador = 0; ...

        free(capa_neuronas[numero_capas_ocultas]);

        free(capa_neuronas);

        capa_neuronas = NULL;

    } // if (capa_neuronas[0] != NULL)

    if (numero_neuronas != NULL) {
        free(numero_neuronas);
        numero_neuronas = NULL;
    } // if (numero_neuronas != NULL)

    if (conjunto_pesos != NULL) {
        free(conjunto_pesos);
        conjunto_pesos = NULL;
    } // if (conjunto_pesos != NULL)

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   etapa_forward                                                    
/// Entrada: - datos: vector con los datos de entrada.                          
/// Objetivo:  Realizar la etapa forward del perceptrón multicapa con el        
///            algoritmo forward-backward.                                      
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::etapa_forward(double *datos) {

    int cuenta_neuronas, cuenta_capas;
    int cuenta_datos;
    double *apunta_datos;
    Capa_neuronas capa_anterior;
    Neurona *apunta_neurona, *apunta_neurona_anterior;
    Nodo_entrada *apunta_nodo_entrada;

    // Incrementamos el número de ejemplos:

    numero_ejemplos++;

    // Etapa de entrada, para cada neurona de la capa oculta:

    apunta_neurona = capa_neuronas[0];

    for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[0];
                                cuenta_neuronas++, apunta_neurona++) {

        apunta_datos = datos;

        apunta_nodo_entrada = entrada;
        apunta_neurona->entrada = apunta_neurona->peso_0; // La constante.

        for (cuenta_datos = 0; cuenta_datos < numero_entradas;
                                cuenta_datos++, apunta_datos++, apunta_nodo_entrada++) {
            if (apunta_nodo_entrada->pesos_nulos[cuenta_neuronas] != 0.0)
                apunta_neurona->entrada += *apunta_datos *
                    apunta_nodo_entrada->pesos[cuenta_neuronas];
        } // for (cuenta_datos = 0; cuenta_datos < numero_entradas; ...

        apunta_neurona->salida =
            apunta_neurona->funcion_activacion(apunta_neurona->parametro, apunta_neurona->entrada);

    } // for (cuenta_neuronas = 0; ...

    // Ahora, las capas ocultas:


    for (cuenta_capas = 1; cuenta_capas <= numero_capas_ocultas; cuenta_capas++) {

        capa_anterior = capa_neuronas[cuenta_capas - 1];

        apunta_neurona = capa_neuronas[cuenta_capas];

        for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[cuenta_capas];
                    cuenta_neuronas++, apunta_neurona++) {

            apunta_neurona->entrada = apunta_neurona->peso_0;
            apunta_neurona_anterior = capa_anterior;

            for (cuenta_datos = 0; cuenta_datos < numero_neuronas[cuenta_capas - 1];
                            cuenta_datos++, apunta_neurona_anterior++)
                if (apunta_neurona_anterior->pesos_nulos[cuenta_neuronas] != 0.0)
                    apunta_neurona->entrada += apunta_neurona_anterior->salida *
                                apunta_neurona_anterior->pesos[cuenta_neuronas];

            apunta_neurona->salida = apunta_neurona->funcion_activacion(apunta_neurona->parametro,
                                apunta_neurona->entrada);
    //        printf("Salida: %lf.\n", apunta_neurona->salida);

        } // for (cuenta_neuronas = 0; ...

    } // for (contador = 0; contador < ...

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   simula_red                                                       
/// Entrada: - datos: vector con los datos de entrada.                          
/// Salida:  - salida_red: vector con la salida producida por la red.           
/// Objetivo:  Simular el comportamiento de la red.                             
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::simula_red(double *datos, double *salida_red) {

    int contador, cuenta_neuronas, cuenta_capas;
    int cuenta_datos;
    double *apunta_datos;
    Capa_neuronas capa_anterior;
    Neurona *apunta_neurona, *apunta_neurona_anterior;
    Nodo_entrada *apunta_nodo_entrada;

    // Incrementamos el número de ejemplos:

    numero_ejemplos++;

    // Etapa de entrada, para cada neurona de la capa oculta:

    apunta_neurona = capa_neuronas[0];

    for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[0];
                                cuenta_neuronas++, apunta_neurona++) {

        apunta_datos = datos;

        apunta_nodo_entrada = entrada;
        apunta_neurona->entrada = apunta_neurona->peso_0; // La constante.

        for (cuenta_datos = 0; cuenta_datos < numero_entradas;
                                cuenta_datos++, apunta_datos++, apunta_nodo_entrada++) {
            if (apunta_nodo_entrada->pesos_nulos[cuenta_neuronas] != 0.0)
                apunta_neurona->entrada += *apunta_datos *
                        apunta_nodo_entrada->pesos[cuenta_neuronas];
        } // for (cuenta_datos = 0; cuenta_datos < numero_entradas; ...

        apunta_neurona->salida =
            apunta_neurona->funcion_activacion(apunta_neurona->parametro, apunta_neurona->entrada);

    } // for (cuenta_neuronas = 0; ...

    // Ahora, las capas ocultas:

    for (cuenta_capas = 1; cuenta_capas <= numero_capas_ocultas; cuenta_capas++) {

        capa_anterior = capa_neuronas[cuenta_capas - 1];

        apunta_neurona = capa_neuronas[cuenta_capas];

        for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[cuenta_capas];
                    cuenta_neuronas++, apunta_neurona++) {

            apunta_neurona->entrada = apunta_neurona->peso_0;
            apunta_neurona_anterior = capa_anterior;

            for (cuenta_datos = 0; cuenta_datos < numero_neuronas[cuenta_capas - 1];
                            cuenta_datos++, apunta_neurona_anterior++)
                if (apunta_neurona_anterior->pesos_nulos[cuenta_neuronas] != 0.0)
                    apunta_neurona->entrada += apunta_neurona_anterior->salida *
                                        apunta_neurona_anterior->pesos[cuenta_neuronas];

            apunta_neurona->salida = apunta_neurona->funcion_activacion(apunta_neurona->parametro,
                                apunta_neurona->entrada);
        } // for (cuenta_neuronas = 0; ...

    } // for (int contador = 1; ...

    // Copiamos la salida:

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                        contador++, apunta_neurona++)
        *salida_red++ = apunta_neurona->salida;

//        printf("Salida: %lf.\n", apunta_neurona->salida);

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   etapa_backward                                                   
/// Entrada: - datos_entrada: vector con los datos de entrada.                  
///          - salida: vector con los datos de salida.                          
/// Objetivo:  Realizar la etapa backward del perceptrón multicapa con el       
///            algoritmo forward-backward.                                      
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::etapa_backward(double *datos_entrada, double *salida) {

    int cuenta_neuronas, contador, cuenta_anterior, cuenta_capas;
    double *apunta_salida = salida;
    double error, delta_auxiliar;
    double correccion_anterior;
    Neurona *apunta_neurona, *apunta_anterior, *apunta_siguiente;
    Nodo_entrada *apunta_entrada;

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    // Comenzamos por los nodos de salida:

    for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[numero_capas_ocultas];
                cuenta_neuronas++, apunta_neurona++, apunta_salida++) {

        // Error de la última etapa.

        error = *apunta_salida - apunta_neurona->salida;

        apunta_neurona->gradiente_local = error *
            apunta_neurona->funcion_derivada(apunta_neurona->parametro, apunta_neurona->entrada);

        // Actualizamos los pesos que van a ese nodo:

        // Primero, la constante (podemos actualizarlo ya, puesto que no influye en
        // la etapa backward)

        correccion_anterior = apunta_neurona->correccion;

        apunta_neurona->correccion = ALPHA_MOMENTO * correccion_anterior +
                                     ETA * apunta_neurona->gradiente_local;

        apunta_neurona->peso_0 += apunta_neurona->correccion;

        // Luego, los que vienen de la capa anterior: (sólo el incremento, la actualización real
        // se hace al final.

        apunta_anterior = capa_neuronas[numero_capas_ocultas - 1];

        for (cuenta_anterior = 0; cuenta_anterior < numero_neuronas[numero_capas_ocultas - 1];
                    cuenta_anterior++, apunta_anterior++) {
            correccion_anterior = apunta_anterior->correccion_pesos[cuenta_neuronas];
            apunta_anterior->correccion_pesos[cuenta_neuronas] = ALPHA_MOMENTO * correccion_anterior +
                        ETA * apunta_neurona->gradiente_local * apunta_anterior->salida;
        } // for (cuenta_anterior = 0; ...

    } // for (cuenta_neuronas = 0;

    // Y ahora, los ocultos:

    for (cuenta_capas = numero_capas_ocultas - 1; cuenta_capas > 0; cuenta_capas--) {

        apunta_neurona = capa_neuronas[cuenta_capas];

        for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[cuenta_capas];
                    cuenta_neuronas++, apunta_neurona++) { //, apunta_salida++) {

            delta_auxiliar = 0.0;
            apunta_siguiente = capa_neuronas[cuenta_capas + 1];

            for (contador = 0; contador < numero_neuronas[cuenta_capas + 1];
                    contador++, apunta_siguiente++)
                if (apunta_neurona->pesos_nulos[contador] != 0.0)
                    delta_auxiliar += apunta_siguiente->gradiente_local *
                                      apunta_neurona->pesos[contador];

            apunta_neurona->gradiente_local = delta_auxiliar *
                apunta_neurona->funcion_derivada(apunta_neurona->parametro, apunta_neurona->entrada);

            // Actualizamos la constante:

            correccion_anterior = apunta_neurona->correccion;

            apunta_neurona->correccion = ALPHA_MOMENTO * correccion_anterior +
                                         ETA * apunta_neurona->gradiente_local;

            apunta_neurona->peso_0 += apunta_neurona->correccion;

            // Luego, los que vienen de la capa anterior: (sólo el incremento, la actualización real
            // se hace al final.

            apunta_anterior = capa_neuronas[cuenta_capas - 1];

            for (cuenta_anterior = 0; cuenta_anterior < numero_neuronas[cuenta_capas - 1];
                        cuenta_anterior++, apunta_anterior++) {
                correccion_anterior = apunta_anterior->correccion_pesos[cuenta_neuronas];
                apunta_anterior->correccion_pesos[cuenta_neuronas] = ALPHA_MOMENTO * correccion_anterior +
                            ETA * apunta_neurona->gradiente_local * apunta_anterior->salida;
            } // for (cuenta_anterior = 0; ...

        } // for (cuenta_neurona = 0; ...
        
    } // for (contador = 0; ...


    // Por último, los de la entrada:

    apunta_neurona = capa_neuronas[0];

    for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[0];
                cuenta_neuronas++, apunta_neurona++) { //, apunta_salida++) {

        delta_auxiliar = 0.0;
        apunta_siguiente = capa_neuronas[1];

        for (contador = 0; contador < numero_neuronas[1];
                contador++, apunta_siguiente++)
            if (apunta_neurona->pesos_nulos[contador] != 0.0)
                delta_auxiliar += apunta_siguiente->gradiente_local *
                                  apunta_neurona->pesos[contador];

        apunta_neurona->gradiente_local = delta_auxiliar *
            apunta_neurona->funcion_derivada(apunta_neurona->parametro, apunta_neurona->entrada);

        // Actualizamos los pesos que van a ese nodo:

        // Primero, la constante:

        correccion_anterior = apunta_neurona->correccion;

        apunta_neurona->correccion = ALPHA_MOMENTO * correccion_anterior +
                                     ETA * apunta_neurona->gradiente_local;

        apunta_neurona->peso_0 += apunta_neurona->correccion;

        // Luego, los que vienen de la entrada:

        apunta_entrada = entrada;

        for (cuenta_anterior = 0; cuenta_anterior < numero_entradas;
                    cuenta_anterior++, apunta_entrada++)
            if (apunta_entrada->pesos_nulos[cuenta_neuronas]) {
                correccion_anterior = apunta_entrada->correccion_pesos[cuenta_neuronas];
                apunta_entrada->correccion_pesos[cuenta_neuronas] = ALPHA_MOMENTO * correccion_anterior +
                            ETA * apunta_neurona->gradiente_local * datos_entrada[cuenta_anterior];
            } // if (apunta_entrada->pesos_nulos[cuenta_neuronas])

    } // for (cuenta_neuronas = 0;

    // Al final, actualizamos los pesos:

    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        apunta_neurona = capa_neuronas[cuenta_capas];

        for (cuenta_neuronas = 0; cuenta_neuronas < numero_neuronas[cuenta_capas];
                    cuenta_neuronas++, apunta_neurona++)
            for (contador = 0; contador < numero_neuronas[cuenta_capas + 1]; contador++)
                apunta_neurona->pesos[contador] += apunta_neurona->correccion_pesos[contador];

    } // for (cuenta_capas = 0;...

    apunta_entrada = entrada;

    for (cuenta_neuronas = 0; cuenta_neuronas < numero_entradas;
                cuenta_neuronas++, apunta_entrada++)
        for (contador = 0; contador < numero_neuronas[0]; contador++)
            apunta_entrada->pesos[contador] += apunta_entrada->correccion_pesos[contador];

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                      
/// Función:   actualiza_error_medio                                            
/// Entrada: - salida: datos correspondientes a la salida.                      
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::actualiza_error_medio(double *salida) {

    int contador;
    double *apunta_datos = salida;
    double diferencia, error_actual_local = 0.0;
    Neurona *apunta_neurona = capa_neuronas[numero_capas_ocultas];

    // Calculamos el error medio:

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                            contador++, apunta_neurona++) {
        diferencia = *apunta_datos++ - apunta_neurona->salida;
        error_actual_local += diferencia*diferencia;
    } // for (contador = 0; contador...
  
	// Ya no lo dividimos por 2.
    
//    error_actual_local /= 2;

//    printf("Error actual: %lf.\n", error_actual);

    if (numero_ejemplos > 1)
        error_medio = (error_medio*(numero_ejemplos - 1) + error_actual_local) /
                numero_ejemplos;
    else
        error_medio = error_actual_local;

//    printf("Salida = %lf. Red = %lf. Error medio = %lf.\n", *salida, (apunta_neurona - 1)->salida, error_medio);
//    getchar();

}

/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   actualiza_error_medio_transformacion
/// Entrada: - salida: datos correspondientes a la salida.
///			 - dato_entrada: dato de entrada sobre el que se aplica el resultado
///			   de la red.
/// Nota:	 - Se diferencia de la función anterior en que en este caso se
/// 		   realiza una transformación en la salida de la red para compararlo
///			   con el resultado deseado.
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::actualiza_error_medio_transformacion(double dato_entrada, double *salida) {

    int contador;
    double *apunta_datos = salida;
    double diferencia, error_actual_local = 0.0;
    Neurona *apunta_neurona = capa_neuronas[numero_capas_ocultas];

    // Calculamos el error medio:

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                            contador++, apunta_neurona++, apunta_datos++) {
		diferencia = (apunta_neurona->salida * dato_entrada) - *apunta_datos*dato_entrada;
        error_actual_local += diferencia*diferencia;
    } // for (contador = 0; contador...

	// Ya no lo dividimos por 2.

//    error_actual_local /= 2;

//    printf("Error actual: %lf.\n", error_actual);

    if (numero_ejemplos > 1)
        error_medio = (error_medio*(numero_ejemplos - 1) + error_actual_local) /
                numero_ejemplos;
    else
        error_medio = error_actual_local;

//    printf("Salida = %lf. Red = %lf. Error medio = %lf.\n", *salida, (apunta_neurona - 1)->salida, error_medio);
//    getchar();

}

/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   actualiza_error_medio
/// Entrada: - salida: datos correspondientes a la salida.
///          - media: media de los vectores de salida.
///          - desviacion: desviación típica de los vectores de salida.
/// NOTA:    - Función análoga a la anterior, pero que desnormaliza los datos
///            obtenidos a la salida de la red para calcular el error.
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::actualiza_error_medio(double *salida, double *media, double *desviacion) {

    int contador;
    double *apunta_datos = salida;
    double diferencia, error_actual = 0.0;
    Neurona *apunta_neurona = capa_neuronas[numero_capas_ocultas];

    // Calculamos el error medio:

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                            contador++, apunta_neurona++, desviacion++, media++) {
        diferencia = *apunta_datos++ - ( (apunta_neurona->salida * *desviacion) + *media);
        error_actual += diferencia*diferencia;
    } // for (contador = 0; contador...

//    error_actual /= 2;

//    printf("Error actual: %lf.\n", error_actual);

    if (numero_ejemplos > 1)
        error_medio = (error_medio*(numero_ejemplos - 1) + error_actual) /
                 numero_ejemplos;
    else
        error_medio = error_actual;

}

/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   actualiza_error_medio
/// Entrada: - salida: datos correspondientes a la salida.
///          - media: media de los vectores de salida.
///          - desviacion: desviación típica de los vectores de salida.
///	Salida:	 - error_iteración: error cometido en esa iteración.
/// NOTA:    - En este caso se devuelve el error cometido en esa iteración.
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::actualiza_error_medio(double *salida, double *error_iteracion) {

    int contador;
    double *apunta_datos = salida;
    double diferencia, error_actual = 0.0;
    Neurona *apunta_neurona = capa_neuronas[numero_capas_ocultas];

    // Calculamos el error medio:

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                            contador++, apunta_neurona++) {
        diferencia = *apunta_datos++ - apunta_neurona->salida;
        *error_iteracion = diferencia;
        error_actual += diferencia*diferencia;
    } // for (contador = 0; contador...

//    error_actual /= 2;

//    printf("Error actual: %lf.\n", error_actual);

    if (numero_ejemplos > 1)
        error_medio = (error_medio*(numero_ejemplos - 1) + error_actual) /
                 numero_ejemplos;
    else
        error_medio = error_actual;

}



/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                        
/// Función:   condicion_finalizacion                                           
/// Valor devuelto:                                                             
///          - Si se ha alcanzado la condición de finalización, devuelve un     
///            valor distinto de cero.                                          
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::condicion_finalizacion() {

    static int epocas = 0;

    epocas++;

    if (error_anterior)
        if ( (error_medio < 0.0001) ||
                ( (epocas > 2000) &&
                  (absoluto(error_anterior - error_medio) / error_anterior) < 0.0001) ) {
//        if ( (absoluto(error_anterior - error_medio) / error_anterior) < 0.00001)  {

            printf("Error = %lf.\n", error_medio);
            return 1;
        } // if ( (absoluto(...
/*
    if ( (error_medio > 100.0) && (error_medio > error_anterior) ) {
        printf("El algoritmo no converge. Error = %lf.\n", error_medio);
        return 1;
    } // if ( (error_medio > ...
*/
//    printf("Error anterior = %lf.\n", error_anterior);

    error_anterior = error_medio;


    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   reinicia_epoca                                                   
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::reinicia_epoca() {

    numero_ejemplos = 0;
    error_medio = 0.0;

}

/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                        
/// Función:   imprime_pesos                                                    
/// Entrada: - nombre_fichero: nombre del fichero en el que se escriben los     
///            pesos obtenidos en el entrenamiento.                             
/// Valor devuelto:                                                             
///          - En ausencia de error se devuelve un cero.                        
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::imprime_pesos(char *nombre_fichero) {

    int contador, cuenta, cuenta_capas;
    double *apunta_pesos, *apunta_pesos_nulos;
    Nodo_entrada *apunta_entrada = entrada;
    Neurona *apunta_neurona;
    FILE *fichero;

    if ( (fichero = fopen(nombre_fichero, "wt")) == NULL) {
        fprintf(stderr, "Error en imprime_pesos, al intentar abrir el fichero %s.\n", nombre_fichero);
        return 1;
    } // if ( (fichero = ...


    fprintf(fichero, "Error medio: %lf\n\n", error_medio);

    fprintf(fichero, "%d\n", numero_capas_ocultas);

    fprintf(fichero, "%d ", numero_entradas);

    for (contador = 0; contador <= numero_capas_ocultas; contador++)
        fprintf(fichero, "%d ", numero_neuronas[contador]);

    fprintf(fichero, "\n\nEntrada:\n");

    for (cuenta = 0; cuenta < numero_entradas; cuenta++, apunta_entrada++) {

        apunta_pesos = apunta_entrada->pesos;
        apunta_pesos_nulos = apunta_entrada->pesos_nulos;

        fprintf(fichero, "\nNúmero %d:\n", cuenta);

        for (contador = 0; contador < numero_neuronas[0]; contador++)
            fprintf(fichero, "%lf\t", *apunta_pesos++ * *apunta_pesos_nulos++);

    } // for (cuenta = 0;

    fprintf(fichero, "\n\nCapas ocultas:");

    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        fprintf(fichero, "\n\nCapa %d:\n", cuenta_capas);
        
        apunta_neurona = capa_neuronas[cuenta_capas];

        for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas];
                                    cuenta++, apunta_neurona++) {

            apunta_pesos = apunta_neurona->pesos;
            apunta_pesos_nulos = apunta_neurona->pesos_nulos;

            fprintf(fichero, "\nNúmero %d:\n", cuenta);

            if (apunta_neurona->funcion_activacion == lineal)
                fprintf(fichero, "Función lineal\n");
            else
            	if (apunta_neurona->funcion_activacion == sigmoide)
	                fprintf(fichero, "Función sigmoide\n");
                else
                	fprintf(fichero, "Función tangente\n");

            fprintf(fichero, "Constante: %lf\n", apunta_neurona->peso_0);

            for (contador = 0; contador < numero_neuronas[cuenta_capas + 1]; contador++)
                fprintf(fichero, "%lf\t", *apunta_pesos++ * *apunta_pesos_nulos++);

        } // for (cuenta = 0; cuenta < ...

    } // for (cuenta_capas = 0; ...
    
    fprintf(fichero, "\n\nCapa de salida:\n");

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (cuenta = 0; cuenta < numero_neuronas[numero_capas_ocultas];
                                    cuenta++, apunta_neurona++) {

	    if (apunta_neurona->funcion_activacion == lineal)
    	    fprintf(fichero, "Función lineal\n");
	    else
        	if (apunta_neurona->funcion_activacion == sigmoide)
		        fprintf(fichero, "Función sigmoide\n");
            else
            	fprintf(fichero, "Función tangente\n");

    	fprintf(fichero, "Constante: %lf\n", apunta_neurona->peso_0);

    } // for (cuenta = 0; cuenta < ...
    
    fclose(fichero);

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   lee_fichero_pesos                                                
/// Entrada: - nombre_fichero: nombre del fichero en el que se encuentran los   
///            pesos de la red.                                                 
/// Valor devuelto:                                                             
///          - En ausencia de error, devuelve un cero.                          
/// NOTA:    - Se supone que el fichero está en el formato de la función        
///            imprime_pesos.                                                   
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::lee_fichero_pesos(char *nombre_fichero) {

    int numero;
    int contador, cuenta, cuenta_capas;
    int num_neuronas[MAXIMO_NUMERO_CAPAS_OCULTAS + 1];
    char cadena_auxiliar[1000];
    Nodo_entrada *apunta_entrada;
    Neurona *apunta_neurona;
    FILE *fichero;

//    if (entrada != NULL)
//        libera_memoria_red();

    if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
        fprintf(stderr, "Error en lee_fichero_pesos, al intentar abrir el fichero %s.\n", nombre_fichero);
        return 1;
    } // if ( (fichero = ...

    fgets(cadena_auxiliar, 1000, fichero);

    fscanf(fichero, "%d\n", &numero_capas_ocultas);

    if (numero_capas_ocultas > MAXIMO_NUMERO_CAPAS_OCULTAS) {
        fprintf(stderr, "Número de capas ocultas (%d) mayor que el permitido (%d). Hay que cambiar \
        la macro MAXIMO_NUMERO_CAPAS_OCULTAS en red_neuronal.hpp.\n", numero_capas_ocultas,
                    MAXIMO_NUMERO_CAPAS_OCULTAS);
        return 1;
    } // if (numero_capas_ocultas > MAXIMO_NUMERO_CAPAS_OCULTAS)

    fscanf(fichero, "%d ", &numero_entradas);

    for (contador = 0; contador <= numero_capas_ocultas; contador++)
        fscanf(fichero, "%d ", &num_neuronas[contador]);

    if (asigna_memoria_red(numero_entradas, numero_capas_ocultas, num_neuronas))
        return 1;

    fscanf(fichero, "%s\n", cadena_auxiliar);

    if (strcmp(cadena_auxiliar, "Entrada:")) {
        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
        		nombre_fichero);
        return 1;
    } // if (strcmp(cadena_auxiliar, ...

    apunta_entrada = entrada;

    for (contador = 0; contador  < numero_entradas; contador++, apunta_entrada++) {

        fscanf(fichero, "%s %d:\n", cadena_auxiliar, &numero);

        if ( (strcmp(cadena_auxiliar, "Número")) || (numero != contador) ) {
            fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
            		nombre_fichero);
            return 1;
        } // if ( (strcmp(cadena_auxiliar, ...

        for (cuenta = 0; cuenta < numero_neuronas[0]; cuenta++) {
            fscanf(fichero, "%lf", &apunta_entrada->pesos[cuenta]);
            apunta_entrada->pesos_nulos[cuenta] = 1.0;
        } // for (cuenta = 0; ...

    } // for (contador = 0; contador < numero_entradas; ...

    if (feof(fichero)) {
        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
        		nombre_fichero);
        return 1;
    } // if (feof(fichero))

    fgets(cadena_auxiliar, 1000, fichero);
    fgets(cadena_auxiliar, 1000, fichero);
    fgets(cadena_auxiliar, 1000, fichero);

    if (strcmp(cadena_auxiliar, "Capas ocultas:\n")) {
        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
        		nombre_fichero);
        return 1;
    } // if (strcmp(cadena_auxiliar, ...


    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        fscanf(fichero, "%s %d:\n", cadena_auxiliar, &numero);

        if ( (strcmp(cadena_auxiliar, "Capa")) || (numero != cuenta_capas) ) {
	        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
        		nombre_fichero);
            return 1;
        } // if ( (strcmp(cadena_auxiliar, "Capa")) ...

        apunta_neurona = capa_neuronas[cuenta_capas];

        for (contador = 0; contador < numero_neuronas[cuenta_capas]; contador++, apunta_neurona++) {

            fscanf(fichero, "%s %d:\n", cadena_auxiliar, &numero);

            if ( (strcmp(cadena_auxiliar, "Número")) || (numero != contador) ) {
		        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
		        		nombre_fichero);
                return 1;
            } // if ( (strcmp(cadena_auxiliar, ...

            fscanf(fichero, "%s", cadena_auxiliar);

            if (strcmp(cadena_auxiliar, "Función")) {
		        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
		        		nombre_fichero);
                return 1;
            } // if (strcmp(cadena_auxiliar, ...

            fscanf(fichero, "%s", cadena_auxiliar);

            if (asigna_funcion(cadena_auxiliar, apunta_neurona))
                return 1;

            fscanf(fichero, "%s %lf\n", cadena_auxiliar, &apunta_neurona->peso_0);

            if (strcmp(cadena_auxiliar, "Constante:")) {
		        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
	    	    		nombre_fichero);
                return 1;
            } //if (strcmp(cadena_auxiliar, ...

            for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas + 1]; cuenta++) {
                fscanf(fichero, "%lf", &apunta_neurona->pesos[cuenta]);
                apunta_neurona->pesos_nulos[cuenta] = 1.0;
            } // for (cuenta = 0; cuenta < ...

        } // for (contador = 0; contador < numero_entradas; ...

    } // for (cuenta_capas = 0; ...
    
    fgets(cadena_auxiliar, 1000, fichero);
    fgets(cadena_auxiliar, 1000, fichero);
    fgets(cadena_auxiliar, 1000, fichero);

    if (strcmp(cadena_auxiliar, "Capa de salida:\n")) {
        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
        		nombre_fichero);
        return 1;
    } // if (strcmp(cadena_auxiliar, ...

	apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
    			contador++, apunta_neurona++) {

	    fscanf(fichero, "%s", cadena_auxiliar);

	    if (strcmp(cadena_auxiliar, "Función")) {
	        fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
	        		nombre_fichero);
	        return 1;
	    } //if (strcmp(cadena_auxiliar, ...

	    fscanf(fichero, "%s", cadena_auxiliar);

    	if (asigna_funcion(cadena_auxiliar, apunta_neurona))
	        return 1;

    	fscanf(fichero, "%s %lf\n", cadena_auxiliar, &apunta_neurona->peso_0);

    	if (strcmp(cadena_auxiliar, "Constante:")) {
        	fprintf(stderr, "Error en lee_fichero_pesos: Formato de fichero (%s) erróneo.\n",
    	    		nombre_fichero);
	        return 1;
    	} // if (strcmp(cadena_auxiliar, ...

    } // for (contador = 0; ...
    
    fclose(fichero);

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   imprime_error_medio                                              
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::imprime_error_medio() {

    printf("Error actual: %lf.\n", error_medio);

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   guarda_conjunto_pesos                                            
/// Valor devuelto:                                                             
///          - En ausencia de error, devuelve un cero.                          
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::guarda_conjunto_pesos() {

    int contador, cuenta, cuenta_capas;
    int numero_pesos;
    double *apunta_peso, *apunta_pesos_red;
    Neurona *apunta_neurona;
    Nodo_entrada *apunta_entrada = entrada;

    numero_pesos = numero_entradas*numero_neuronas[0];

    for (contador = 0; contador < numero_capas_ocultas; contador++)
        numero_pesos += numero_neuronas[contador]*numero_neuronas[contador + 1] +
                        numero_neuronas[contador];

    numero_pesos += numero_neuronas[numero_capas_ocultas];

    if (conjunto_pesos == NULL)
        if ( (conjunto_pesos = (double *) malloc(numero_pesos*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en guarda_conjunto_pesos, al asignar memoria.\n");
            return 1;
        } // if (conjunto_pesos == NULL)

    apunta_peso = conjunto_pesos;

    for (contador = 0; contador < numero_entradas; contador++, apunta_entrada++) {
        apunta_pesos_red = apunta_entrada->pesos;
        for (cuenta = 0; cuenta < numero_neuronas[0]; cuenta++, apunta_peso++)
            *apunta_peso = *apunta_pesos_red++;
    } // for (contador = 0; ...


    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        apunta_neurona = capa_neuronas[cuenta_capas];
        for (contador = 0; contador < numero_neuronas[cuenta_capas]; contador++, apunta_neurona++) {
            apunta_pesos_red = apunta_neurona->pesos;
            *apunta_peso++ = apunta_neurona->peso_0;
            for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas + 1]; cuenta++, apunta_peso++)
                *apunta_peso = *apunta_pesos_red++;
        } // for (contador = 0; ...

    } // for (cuenta_capas = 0; ...
    
    // Por último, la constante de las capas de salida:

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                        contador++, apunta_neurona++)
        *apunta_peso++ = apunta_neurona->peso_0;

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   recupera_conjunto_pesos                                          
/// Valor devuelto:                                                             
///          - En ausencia de error, devuelve un cero.                          
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::recupera_conjunto_pesos() {

    int contador, cuenta, cuenta_capas;
    double *apunta_peso, *apunta_pesos_red;
    Neurona *apunta_neurona;
    Nodo_entrada *apunta_entrada = entrada;

    if (conjunto_pesos == NULL) {
        fprintf(stderr, "Error en recupera_conjunto_pesos: conjunto_pesos = NULL.\n");
        return 1;
    } // if (conjunto_pesos == NULL)


    apunta_peso = conjunto_pesos;

    for (contador = 0; contador < numero_entradas; contador++, apunta_entrada++) {
        apunta_pesos_red = apunta_entrada->pesos;
        for (cuenta = 0; cuenta < numero_neuronas[0]; cuenta++, apunta_peso++)
            *apunta_pesos_red++ = *apunta_peso;
    } // for (contador = 0; ...

    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        apunta_neurona = capa_neuronas[cuenta_capas];
        for (contador = 0; contador < numero_neuronas[cuenta_capas]; contador++, apunta_neurona++) {
            apunta_pesos_red = apunta_neurona->pesos;
            apunta_neurona->peso_0 = *apunta_peso++;
            for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas + 1]; cuenta++, apunta_peso++)
                *apunta_pesos_red++ = *apunta_peso;
        } // for (contador = 0; ...

    } // for (cuenta_capas = 0; ...
    
    // Por último, la constante de las capas de salida:

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                        contador++, apunta_neurona++)
        apunta_neurona->peso_0 = *apunta_peso++;

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   copia_conjunto_pesos                                             
/// Entrada: - red: objeto de la clase red cuyos datos se quieren copiar.       
/// Valor devuelto:                                                             
///          - En ausencia de error, devuelve un 0.                             
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::copia_datos_red(Red_neuronal *red) {

    int contador, cuenta, cuenta_capas;
    double *apunta_peso, *apunta_peso_original;
    Nodo_entrada *entrada_propia, *entrada_original;
    Neurona *apunta_neurona, *apunta_neurona_original;

    if (entrada == NULL)
        if (asigna_memoria_red(red->numero_entradas, red->numero_capas_ocultas, red->numero_neuronas))
            return 1;

    entrada_propia = entrada;
    entrada_original = red->entrada;

    for (contador = 0; contador < numero_entradas;
                    contador++, entrada_propia++, entrada_original++) {
        apunta_peso = entrada_propia->pesos;
        apunta_peso_original = entrada_original->pesos;
        for (cuenta = 0; cuenta < numero_neuronas[0]; cuenta++, apunta_peso++)
            *apunta_peso = *apunta_peso_original++;

        apunta_peso = entrada_propia->pesos_nulos;
        apunta_peso_original = entrada_original->pesos_nulos;

        for (cuenta = 0; cuenta < numero_neuronas[0]; cuenta++, apunta_peso++)
            *apunta_peso = *apunta_peso_original++;

    } // for (contador = 0; ...

    for (cuenta_capas = 0; cuenta_capas < numero_capas_ocultas; cuenta_capas++) {

        apunta_neurona = capa_neuronas[cuenta_capas];
        apunta_neurona_original = red->capa_neuronas[cuenta_capas];

        for (contador = 0; contador < numero_neuronas[cuenta_capas];
                           contador++, apunta_neurona++, apunta_neurona_original++) {

            apunta_neurona->peso_0 = apunta_neurona_original->peso_0;
            apunta_neurona->funcion_activacion = apunta_neurona_original->funcion_activacion;
                apunta_neurona->funcion_derivada = apunta_neurona_original->funcion_derivada;
            apunta_neurona->parametro = apunta_neurona_original->parametro;

            apunta_peso = apunta_neurona->pesos;
                apunta_peso_original = apunta_neurona_original->pesos;

            for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas + 1]; cuenta++, apunta_peso++)
                *apunta_peso = *apunta_peso_original++;

            apunta_peso = apunta_neurona->pesos_nulos;
            apunta_peso_original = apunta_neurona_original->pesos_nulos;

            for (cuenta = 0; cuenta < numero_neuronas[cuenta_capas + 1]; cuenta++, apunta_peso++)
                *apunta_peso = *apunta_peso_original++;

        } // for (contador = 0; ...

    } // for (cuenta_capas = 0; ...
    
    apunta_neurona = capa_neuronas[numero_capas_ocultas];
    apunta_neurona_original = red->capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas]; contador++,
    				apunta_neurona++, apunta_neurona_original++) {

        apunta_neurona->peso_0 = apunta_neurona_original->peso_0;
        apunta_neurona->funcion_activacion = apunta_neurona_original->funcion_activacion;
        apunta_neurona->funcion_derivada = apunta_neurona_original->funcion_derivada;
        apunta_neurona->parametro = apunta_neurona_original->parametro;

	} // for (contador = 0; ...

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   asigna_memoria_red
/// Entrada: - num_entradas: número de entradas de la red.
///          - num_capas_ocultas: número de capas ocultas de la red.
///          - num_neuronas: número de neuronas de cada capa.
/// Valor devuelto:
///          - En ausencia de error, devuelve un 0.
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::asigna_memoria_red(int num_entradas, int num_capas_ocultas,
                                     int *num_neuronas) {

    int contador, cuenta;
    Nodo_entrada *apunta_entrada;
    Neurona *apunta_neurona;

    numero_entradas = num_entradas;
    numero_capas_ocultas = num_capas_ocultas;

	if (numero_neuronas != NULL)
    	return 0;
        
    if ( (numero_neuronas = (int *) malloc((numero_capas_ocultas + 1)*sizeof(int)))
          == NULL) {
        fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
        return 1;
    } // if ( (numero_neuronas = ...

    for (contador = 0; contador <= numero_capas_ocultas; contador++)
        numero_neuronas[contador] = num_neuronas[contador];

    if ( (entrada = (Nodo_entrada *) malloc(numero_entradas*sizeof(Nodo_entrada)))
          == NULL) {
        fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
        return 1;
    } // if ( (entradas = ...

    apunta_entrada = entrada;

    for (contador = 0; contador < numero_entradas; contador++, apunta_entrada++) {

        if ( (apunta_entrada->pesos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

        if ( (apunta_entrada->correccion_pesos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

        if ( (apunta_entrada->pesos_nulos = (double *) malloc(numero_neuronas[0]*sizeof(double)))
              == NULL) {
            fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
            return 1;
        } // if ( (apunta_entrada->pesos = ...

    } // for (contador = 0; contador < numero_entradas ...

    if ( (capa_neuronas = (Neurona **) malloc((numero_capas_ocultas + 1)*sizeof(Neurona *)))
          == NULL) {
        fprintf(stderr, "Error en asigna_memoria_red, al asignar memoria.\n");
        return 1;
    } // if ( (capa_neuronas = ...

    for (contador = 0; contador < numero_capas_ocultas; contador ++) {

        if ( (capa_neuronas[contador] = (Neurona *) malloc(numero_neuronas[contador]*sizeof(Neurona)))
              == NULL) {
            fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
            return 1;
        } // if ( (capa_neuronas[0] ...

        apunta_neurona = capa_neuronas[contador];

        for (cuenta = 0; cuenta < numero_neuronas[contador]; cuenta++, apunta_neurona++) {

            if ( (apunta_neurona->pesos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en asigna_memoria_red, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

            if ( (apunta_neurona->correccion_pesos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en asigna_memoria_red, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

            if ( (apunta_neurona->pesos_nulos = (double *)
                        malloc(numero_neuronas[contador + 1]*sizeof(double)))
                  == NULL) {
                fprintf(stderr, "Error en asigna_memoria_red, al asignar memoria.\n");
                return 1;
            } // if ( (capa_neuronas ...

        } // for (cuenta = 0; ...

    } // for (contador = 0; contador <= NUMERO_CAPAS...

    // Ahora, la capa de salida:

    if ( (capa_neuronas[numero_capas_ocultas] = (Neurona *)
                malloc(numero_neuronas[numero_capas_ocultas]*sizeof(Neurona)))
        == NULL) {
        fprintf(stderr, "Error en asigna_memoria_red, al reservar memoria.\n");
        return 1;
    } // if ( (capa_neuronas[0] ...

    apunta_neurona = capa_neuronas[numero_capas_ocultas];

    for (contador = 0; contador < numero_neuronas[numero_capas_ocultas];
                        contador++, apunta_neurona++) {

        apunta_neurona->pesos = NULL;
        apunta_neurona->correccion_pesos = NULL;
        apunta_neurona->pesos_nulos = NULL;

    } // for (contador = 0; ...

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                     
/// Función:   inicia_error
/////////////////////////////////////////////////////////////////////////////

void Red_neuronal::inicia_error() {

    error_medio = 0.0;
    numero_ejemplos = 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal
/// Función:   escribe_pesos_capa_matriz
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::escribe_pesos_capa_matriz(char *nombre_fichero,
                                            int numero_capa, char *opciones) {

    int contador, cuenta;
    Nodo_entrada *apunta_entrada;
    Neurona *apunta_neurona, *apunta_capa_anterior;
    FILE *fichero;

    if (numero_capa > numero_capas_ocultas) {
        fprintf(stderr, "Error en escribe_pesos_capa_matriz: número de capa (%d) mayor que el permitido (%d).\n",
                numero_capa, numero_capas_ocultas);
        return 1;
    } // if (numero_capa > numero_capas_ocultas)

    if ( (fichero = fopen(nombre_fichero, opciones)) == NULL) {
        fprintf(stderr, "Error en escribe_pesos_capa_matriz, al intentar abrir el fichero %s \
                con las opciones %s.\n", nombre_fichero, opciones);
        return 1;
    } // if ( (fichero = fopen(...

    if (numero_capa == 0) { // Se trata de la capa de entrada.

        apunta_neurona = capa_neuronas[0];

        for (contador = 0; contador < numero_neuronas[0]; contador++, apunta_neurona++) {

            fprintf(fichero, "%lf\t", apunta_neurona->peso_0);

            apunta_entrada = entrada;

            for (cuenta = 0; cuenta < numero_entradas; cuenta++, apunta_entrada++)
                fprintf(fichero, "%lf\t", apunta_entrada->pesos[contador]*apunta_entrada->pesos_nulos[contador]);

            fprintf(fichero, "\n");

        } // for (contador = 0; ...

    } // if (numero_capa == 0)
    else {

        apunta_neurona = capa_neuronas[numero_capa];

        for (contador = 0; contador < numero_neuronas[numero_capa]; apunta_neurona++) {

            fprintf(fichero, "%lf\t", apunta_neurona->peso_0);

            apunta_capa_anterior = capa_neuronas[numero_capa - 1];

            for (cuenta = 0; cuenta < numero_neuronas[numero_capa - 1];
                            cuenta++, apunta_capa_anterior++)
                fprintf(fichero, "%lf\t", apunta_capa_anterior->pesos[contador]*apunta_capa_anterior->pesos_nulos[contador]);

            fprintf(fichero, "\n");
            
        } // for (contador = 0; contador < numero_neuronas[numero_capa]; ...
    } // else

    fclose(fichero);

    return 0;

}


/////////////////////////////////////////////////////////////////////////////
/// Clase:     Red_neuronal                                                //
/// Función:   calcula_varianza_explicada								   //
/// Entrada: - entrada: cadena con los vectores de datos de entrada. 	   //
///			 - salida: cadena con los vectores de salida.				   //
///			 - numero_vectores_entrada: número de vectores de las cadenas  //
///			   anteriores.												   //
///			 - indice_salida: índice de la salida a la que se refiere la   //
/// 		   varianza explicada.										   //
/// Salida:	 - coeficiente: coeficiente de varianza explicada.			   //
/// Valor devuelto:														   //
///			 - En ausencia de error, devuelve un cero.					   //
/////////////////////////////////////////////////////////////////////////////

int Red_neuronal::calcula_varianza_explicada(double *entrada, double *salida,
											 int numero_vectores,
                                             int indice_salida,
                                             double *coeficiente) {

	int contador;
    int numero_salidas = numero_neuronas[numero_capas_ocultas];
	double *apunta_salida, *apunta_entrada;
    double *salida_red, *errores, *apunta_errores_red, error;
	double diferencia;
    double error_medio_local = 0.0;
    double varianza_error = 0.0;
    double varianza_salida = 0.0;
    double media_salida = 0.0;

	if (indice_salida > numero_salidas) {
		fprintf(stderr, "Error en calcula_varianza_explicada: índice de salida (%d) mayor que \
        número de salidas (%d).\n", indice_salida, numero_neuronas[numero_capas_ocultas]);
        return 1;
    } // if (indice_salida > ...

    if ( (salida_red = (double *) malloc(numero_salidas*sizeof(double))) == NULL) {
    	fprintf(stderr, "Error en calcula_varianza_explicada, al asignar memoria.\n");
        return 1;
    } // if ( (salida_red = ...

    if ( (errores = (double *) malloc(numero_vectores*sizeof(double))) == NULL) {
    	fprintf(stderr, "Error en calcula_varianza_explicada, al asignar memoria.\n");
        return 1;
    } // if ( (salidas = ...

    apunta_errores_red = errores;

    // Apuntamos la salida al índice deseado.

	salida += indice_salida;

    // Calculamos la media de la salida:

    for (apunta_salida = salida, contador = 0; contador < numero_vectores;
    							contador++, apunta_salida += numero_salidas)
    	media_salida += *apunta_salida;

	media_salida /= numero_vectores;

    // Ahora, la varianza:

    for (apunta_salida = salida, contador = 0; contador < numero_vectores;
    							contador++, apunta_salida += numero_salidas) {
		diferencia = *apunta_salida - media_salida;
        varianza_salida += diferencia*diferencia;
    } // for (apunta_salida = salida, ...

	varianza_salida /= numero_vectores;

	// Ahora, la varianza del error:

    apunta_salida = salida;
    apunta_entrada = entrada;


    for (contador = 0; contador < numero_vectores;
    				contador++, apunta_entrada += numero_entradas, apunta_salida += numero_salidas) {
		this->simula_red(apunta_entrada, salida_red);
        error = *apunta_salida - *(salida_red + indice_salida);
		*apunta_errores_red++ = error;
        error_medio_local += error;
    } // for (contador = 0; ...

	error_medio_local /= numero_vectores;

	apunta_errores_red = errores;

    for (contador = 0; contador < numero_vectores; contador++) {
		diferencia = *apunta_errores_red++ - error_medio_local;
        varianza_error += diferencia*diferencia;
    } // for (contador = 0; contador < numero_vectores ...

    varianza_error /= numero_vectores;
    
    *coeficiente = (varianza_salida - varianza_error) / varianza_salida;

	free(salida_red);
    free(errores);

	return 0;

}
