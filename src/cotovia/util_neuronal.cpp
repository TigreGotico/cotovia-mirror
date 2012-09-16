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
#include <math.h>

#include "red_neuronal.hpp"
#include "util_neuronal.hpp"


/**
 * Función:   tangente_hiperbolica                                            
 * \remarks Entrada:
 *			- a: parámetro de la tangente.                                    
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la tangente hiperbólica en el punto x.              
 */

double tangente_hiperbolica(double a, double x) {

	double exponente = -2.0*a*x;

	if (exponente > 100.0)
		return 0;
	else
		if (exponente < -100.0)
			return 1;

	return (2 / (1 + exp(exponente))) - 1;

}


/**
 * Función:   derivada_tangente_hiperbolica                                   
 * \remarks Entrada:
 *			- a: parámetro de la tangente hiperbólica.                        
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la derivada de la tangente hiperbólica en el punto  
 *			  x.													          
 */

double derivada_tangente_hiperbolica(double a, double x) {

	double f = tangente_hiperbolica(a, x);

	return a*(1 - f)*(1 - f);

}


/**
 * Función:   sigmoide                                                        
 * \remarks Entrada:
 *			- a: parámetro de la sigmoide.                                    
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la sigmoide en el punto x.                          
 */

double sigmoide(double a, double x) {

	double exponente = -1.0*a*x;

	if (exponente > 100.0)
		return 0;
	else
		if (exponente < -100.0)
			return 1;

	return 1.0 / (1 + exp(exponente));

}


/**
 * Función:   derivada_sigmoide                                               
 * \remarks Entrada:
 *			- a: parámetro de la sigmoide.                                    
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la derivada de la sigmoide en el punto x.           
 */

double derivada_sigmoide(double a, double x) {

	double f = sigmoide(a, x);

	return f*(1 - f);

}


/**
 * Función:   lineal                                                          
 * \remarks Entrada:
 *			- a: pendiente de la recta.                                       
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la función lineal en el punto x.                    
 */

double lineal(double a, double x) {

	return a*x;

}


/**
 * Función:   derivada_lineal                                                 
 * \remarks Entrada:
 *			- a: pendiente de la recta.                                       
 *          - x: valor de entrada.                                            
 * \remarks Valor devuelto:                                                            
 *          - El valor de la derivada de la función lineal en el punto x.     
 * \remarks NOTA:      Evidentemente, el valor en este caso va a ser independiente de  
 *            x, pero de esta forma podemos emplear punteros a funciones con  
 *            la estructura Neurona.                                          
 */
double derivada_lineal(double a, double x) {

	return a;

}


/**
 * \author campillo
 * \brief Función: lee_datos_entrada                                                
 * \param[in] nombre_fichero nombre del fichero en el que se encuentran los datos de entrenamiento de la red neuronal.                       
 * \param[in] tamano_vector_entrada número de componentes de cada vector de datos.                                                           
 * \param[in] tamano_vector_salida número de componentes de cada vector de salida.                                                          
 * \param[in] cabecera si es distinto de NULL, se supone que la primera línea es la cabecera y se devuelve su valor.						   	
 * \param[out] datos vector con los datos.                                     
 * \param[out] salida vector con los valores deseados de salida.               
 * \param[out] numero_datos: número de vectores.                                
 * \return En ausencia de error, devuelve un cero.                          
 */

int lee_datos_entrada(char *nombre_fichero, int tamano_vector_entrada,
		int tamano_vector_salida, double **datos, double **salida,
		int *numero_datos, char *cabecera) {

	int contador, numero_datos_local = 0;
	int tamano_reservado_entrada = 10000, tamano_reservado_salida = 1000;
	double *apunta_entrada, *apunta_salida;
	FILE *fichero;


	if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
		fprintf(stderr, "Error en lee_datos_entrada, al intentar abrir el fichero %s.\n", nombre_fichero);
		return 1;
	} // if ( (fichero = ...


	if (cabecera)
		fgets(cabecera, 1000, fichero);

	if ( (*datos = (double *) malloc(tamano_reservado_entrada*sizeof(double)))
			== NULL) {
		fprintf(stderr, "Error en lee_datos_entrada, al asignar memoria.\n");
		return 1;
	} // if ( (*datos = ...

	apunta_entrada = *datos;

	if ( (*salida = (double *) malloc(tamano_reservado_salida*sizeof(double)))
			== NULL) {
		fprintf(stderr, "Error en lee_datos_entrada, al asignar memoria.\n");
		return 1;
	} // if ( (*datos = ...

	apunta_salida = *salida;

	while (!feof(fichero)) {

		for (contador = 0; contador < tamano_vector_entrada; contador++, apunta_entrada++) {
			fscanf(fichero, "%lf ", apunta_entrada);
			//            *apunta_entrada /= 10;
		} // for (contador = 0; ...

		if (feof(fichero))
			break;

		for (contador = 0; contador < tamano_vector_salida; contador++, apunta_salida++) {
			fscanf(fichero, "%lf ", apunta_salida);
			//            *apunta_salida /= 10;
		} // for (contador = 0; ...

		numero_datos_local++;

		if ( (numero_datos_local + 1)*tamano_vector_entrada > tamano_reservado_entrada) {

			tamano_reservado_entrada += 10000;

			if ( (*datos = (double *) realloc(*datos, tamano_reservado_entrada*sizeof(double)))
					== NULL) {
				fprintf(stderr, "Error en lee_datos_entrada, al asignar memoria.\n");
				return 1;
			} // if ( (*datos = ...

			apunta_entrada = *datos + numero_datos_local*tamano_vector_entrada;

		} // if ( (*numero_datos + 1)...

		if ( (numero_datos_local + 1)*tamano_vector_salida > tamano_reservado_salida) {

			tamano_reservado_salida += 10000;

			if ( (*salida= (double *) realloc(*salida, tamano_reservado_salida*sizeof(double)))
					== NULL) {
				fprintf(stderr, "Error en lee_datos_entrada, al asignar memoria.\n");
				return 1;
			} // if ( (*datos = ...

			apunta_salida = *salida + numero_datos_local*tamano_vector_salida;

		} // if ( (*numero_datos + 1)...

	} // while (!feof(fichero))

	*numero_datos = numero_datos_local;

	fclose(fichero);

	return 0;

}



/**
 * Función:   escoge_vector_aleatorio                                          
 * \remarks Entrada:
 *			- entrada: cadena de vectores de entrada.                          
 *          - salida: cadena de vectores de salida.                            
 *          - tamano_vector_entrada: número de elementos de cada vector de     
 *            entrada.                                                         
 *          - tamano_vector_salida: número de elementos de cada vector de      
 *            salida.                                                          
 *          - número_datos: número de vectores de las cadenas entrada y salida.
 *	\remarks Salida:
 *			- vector_entrada: vector escogido aleatoriamente de la cadena de   
 *            entrada.                                                         
 *          - vector_salida: vector de salida que se corresponde con el de     
 *            entrada.                                                         
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un 0.                             
 * \remarks NOTA:
 *			- Se escoge un índice al azar. Si ya se ha escogido en la época    
 *            actual, se escoge el siguiente, y así sucesivamente.             
 */

void reordena_vectores(double *entrada, int tamano_vector_entrada,
		double *salida, int tamano_vector_salida,
		int numero_datos) {


	int indice_aleatorio, contador, cuenta, numero_aleatorio;
	double *apunta_dato, *correcaminos;
	double intercambio;

	for (contador = 0; contador < numero_datos; contador++) {

		numero_aleatorio = rand();
		indice_aleatorio = (int) ( (double) numero_aleatorio*(numero_datos - contador) ) / (RAND_MAX + 1) + contador;

		// Intercambiamos los elementos en la entrada:

		apunta_dato = entrada + indice_aleatorio*tamano_vector_entrada;
		correcaminos = entrada + contador*tamano_vector_entrada;

		for (cuenta = 0; cuenta < tamano_vector_entrada; cuenta++) {
			intercambio = *apunta_dato;
			*apunta_dato++ = *correcaminos;
			*correcaminos++ = intercambio;
		} // for (cuenta = 0; cuenta < tamano_vector_entrada; ...

		// Ahora, los elementos de la salida:

		apunta_dato = salida + indice_aleatorio*tamano_vector_salida;
		correcaminos = salida + contador*tamano_vector_salida;

		for (cuenta = 0; cuenta < tamano_vector_salida; cuenta++) {
			intercambio = *apunta_dato;
			*apunta_dato++ = *correcaminos;
			*correcaminos++ = intercambio;
		} // for (cuenta = 0; cuenta < tamano_vector_entrada; ...

	} // for (contador = 0; contador < numero_datos...

}


/**
 * Función:   calcula_media                                                    
 * \remarks Entrada:
 *			- entrada: cadena de vectores de entrada.                          
 *          - tamano_entrada: número de elementos de cada vector de entrada.   
 *          - número_vectores: número de vectores de las cadenas entrada y     
 *            salida.                                                          
 *	\remarks Salida:
 *			- media: varianza de la columna de elementos de entrada.           
 */

void calcula_media(double *entrada, int tamano_entrada, int numero_vectores,
		double *media) {

	int contador;
	double media_local;
	double *apunta_entrada;

	media_local = *entrada;

	apunta_entrada = entrada + tamano_entrada;

	for (contador = 2; contador <= numero_vectores;
			contador++, apunta_entrada += tamano_entrada) {
		media_local = (media_local*(contador - 1) + *apunta_entrada) / contador;
	} // for (contador =0; ...

	*media = media_local;

}


/**
 * Función:   calcula_varianza                                                 
 * \remarks Entrada:
 *			- entrada: cadena de vectores de entrada.                          
 *          - tamano_entrada: número de elementos de cada vector de entrada.   
 *          - número_vectores: número de vectores de las cadenas entrada y     
 *            salida.                                                          
 *          - medias: media de la columna de la que se desea calcular la       
 *            varianza.                                                        
 *	\remarks Salida:
 *			- varianza: varianza de la columna de elementos de entrada.        
 */

void calcula_varianza(double *entrada, int tamano_entrada, int numero_vectores,
		double media, double *varianza) {

	double diferencia, varianza_local;
	double *apunta_entrada = entrada;

	diferencia = *apunta_entrada - media;

	varianza_local = diferencia*diferencia;

	apunta_entrada += tamano_entrada;

	for (int contador = 2; contador <= numero_vectores;
			contador++, apunta_entrada += tamano_entrada) {
		diferencia = *apunta_entrada - media;
		varianza_local = (varianza_local*(contador - 1) + diferencia*diferencia) /
			contador;
	} // for (int contador = 0; ...

	*varianza = varianza_local;
}


/**
 * Función:   normaliza_vectores                                               
 * \remarks Entrada:
 *			- tamano_vector: número de elementos de cada vector.               
 *          - número_vectores: número de vectores de las cadenas entrada y     
 *            salida.                                                          
 *          - nombre_fichero: nombre del fichero en el que se almacenarán los  
 *            valores de las medias y las varianzas.                           
 * \remarks Entrada y Salida:                                                           
 *          - vector: cadena de vectores de entrada.                           
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 */

int normaliza_vectores(double *vector, int tamano_vector, int numero_vectores,
		char *nombre_fichero) {

	int contador, cuenta;
	double *medias, *varianzas;
	double *apunta_entrada;
	FILE *fichero_sal;

	if ( (medias = (double *) malloc(tamano_vector*sizeof(double)))
			== NULL) {
		fprintf(stderr, "Error en normaliza_vectores, al asignar memoria.\n");
		return 1;
	} // if ( (medias = ...

	if ( (varianzas = (double *) malloc(tamano_vector*sizeof(double)))
			== NULL) {
		fprintf(stderr, "Error en normaliza_vectores, al asignar memoria.\n");
		return 1;
	} // if ( (varianzas = ...

	for (contador = 0; contador < tamano_vector; contador++)
		calcula_media(vector + contador, tamano_vector, numero_vectores,
				&medias[contador]);

	for (contador = 0; contador < tamano_vector; contador++)
		calcula_varianza(vector + contador, tamano_vector, numero_vectores,
				medias[contador], &varianzas[contador]);

	for (contador = 0; contador < tamano_vector; contador++)
		varianzas[contador] = sqrt(varianzas[contador]);

	apunta_entrada = vector;

	for (contador = 0; contador < numero_vectores; contador++) {
		for (cuenta = 0; cuenta < tamano_vector; cuenta++, apunta_entrada++)
			if (varianzas[cuenta]) {
				*apunta_entrada = (*apunta_entrada - medias[cuenta]) / varianzas[cuenta];
			} // if (varianzas[cuenta...
			else
				*apunta_entrada = 0.0;
	} // for (contador = 0; contador < ...


	// Escribimos los valores de las medias y las varianzas:

	if (*nombre_fichero != '\0') {

		if ( (fichero_sal = fopen(nombre_fichero, "wt")) == NULL) {
			fprintf(stderr, "Error en normaliza_vectores, al intentar crear el fichero \"medias_var.txt\".\n");
			return 1;
		} // if ( (fichero_sal = ...

		for (contador = 0; contador < tamano_vector; contador++)
			fprintf(fichero_sal, "%lf\t%lf\n", medias[contador], varianzas[contador]);

		fclose(fichero_sal);

	} // if (*nombre_fichero ...

	free(medias);
	free(varianzas);

	return 0;

}

/**
 * Función:   normaliza_vectores                                               
 * \remarks Entrada:
 *			- tamano_vector: número de elementos de cada vector.               
 *          - número_vectores: número de vectores de las cadenas entrada y     
 *            salida.                                                          
 *          - nombre_fichero: nombre del fichero en el que se almacenarán los  
 *            valores de las medias y las varianzas.                           
 * \remarks Entrada y Salida:                                                           
 *          - vector: cadena de vectores de entrada.                           
 *	\remarks Salida:	- medias: vector con las medias de cada columna.				   
 *			- desviaciones: vector con las desviaciones de cada columna.	   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, devuelve un cero.                          
 */

int normaliza_vectores(double *vector, int tamano_vector, int numero_vectores,
		char *nombre_fichero, double *medias, double *desviaciones) {

	int contador, cuenta;
	double *apunta_entrada;
	FILE *fichero_sal;

	for (contador = 0; contador < tamano_vector; contador++)
		calcula_media(vector + contador, tamano_vector, numero_vectores,
				&medias[contador]);

	for (contador = 0; contador < tamano_vector; contador++)
		calcula_varianza(vector + contador, tamano_vector, numero_vectores,
				medias[contador], &desviaciones[contador]);

	for (contador = 0; contador < tamano_vector; contador++)
		desviaciones[contador] = sqrt(desviaciones[contador]);

	apunta_entrada = vector;

	for (contador = 0; contador < numero_vectores; contador++) {
		for (cuenta = 0; cuenta < tamano_vector; cuenta++, apunta_entrada++)
			if (desviaciones[cuenta]) {
				*apunta_entrada = (*apunta_entrada - medias[cuenta]) / desviaciones[cuenta];
			} // if (varianzas[cuenta...
			else
				*apunta_entrada = 0.0;
	} // for (contador = 0; contador < ...


	// Escribimos los valores de las medias y las varianzas:

	if (*nombre_fichero != '\0') {

		if ( (fichero_sal = fopen(nombre_fichero, "wt")) == NULL) {
			fprintf(stderr, "Error en normaliza_vectores, al intentar crear el fichero \"medias_var.txt\".\n");
			return 1;
		} // if ( (fichero_sal = ...

		for (contador = 0; contador < tamano_vector; contador++)
			fprintf(fichero_sal, "%lf\t%lf\n", medias[contador], desviaciones[contador]);

		fclose(fichero_sal);

	} // if (*nombre_fichero ...

	return 0;

}



/**
 * Función:   genera_variable_normal                                           
 * \remarks Entrada:
 *			- media: media de la distribución normal deseada.                  
 *          - desviacion: desviación típica de la distribución deseada.        
 * \remarks Valor devuelto:                                                             
 *          - Una muestra de la distribución normal con la media y varianza    
 *            deseadas.                                                        
 * \remarks NOTA:
 *			- Se genera por el método de Box-Muller.                           
 */

double genera_variable_normal(double media, double desviacion) {

	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;

	if (use_last) {
		y1 = y2;
		use_last = 0;
	}
	else {

		do {
			x1 = 2.0 * ((double) rand() / RAND_MAX) - 1.0;
			x2 = 2.0 * ((double) rand() / RAND_MAX) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return  media + y1*desviacion;

}

/**
 * Función:   lee_medias_y_desviaciones                                        
 * \remarks Entrada:
 *			- nombre_fichero: nombre del fichero del que se van a leer las     
 *            medias y las varianzas.                                          
 *			- numero_entradas: número de variables de entrada de la red.	   
 *	\remarks Salida:
 *			- estadisticos: cadena con las medias y las varianzas de cada      
 *            parámetro de entrada a la red.                                   
 * \remarks Valor devuelto:                                                             
 *          - En ausencia de error, se devuelve un cero.                       
 */

int lee_medias_y_desviaciones(char *nombre_fichero, int numero_entradas,
		Media_varianza *estadisticos) {

	Media_varianza *correcaminos = estadisticos;
	FILE *fichero;

	if ( (fichero = fopen(nombre_fichero, "rt")) == NULL) {
		fprintf(stderr, "Error en lee_medias_y_desviaciones, al intentar abrir \
				el fichero %s.\n", nombre_fichero);
		return 1;
	} // if ( (fichero = fopen(...


	for (int contador = 0; contador < numero_entradas; contador++, correcaminos++) {
		fscanf(fichero, "%lf %lf\n", &correcaminos->media, &correcaminos->desviacion);
		if (correcaminos->desviacion == 0)
			correcaminos->desviacion = 1.0;
	} // for (int contador = 0; ...

	fclose(fichero);

	return 0;

}

