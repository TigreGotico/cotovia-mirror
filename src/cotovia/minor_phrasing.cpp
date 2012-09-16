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
#include <vector>

using namespace std;

#include "grupos_acentuales.hpp"
#include "minor_phrasing.hpp"


/**
 * \class	POS_frontera
 * \brief	Copy constructor
 */
POS_frontera::POS_frontera(const POS_frontera &original) {
	*this = original;
}

/**
 * \class	POS_frontera
 * \brief	overload of the assignment operator
 */
POS_frontera &POS_frontera::operator= (const POS_frontera &original) {

	if (this != &original) {
    	etiqueta_morfosintactica = original.etiqueta_morfosintactica;
        tipo_pausa = original.tipo_pausa;
		salto_f0 = original.salto_f0;
        factor_caida = original.factor_caida;
    }

    return *this;
}

/**
 * \class	POS_frontera
 * \brief	Writes the content of the object in a text file
 * \param[in]	salida	output text file.
 */
int POS_frontera::escribe_datos_txt(FILE *salida) {

	return fprintf(salida, "%d\t%d\t%f\t%f", etiqueta_morfosintactica, tipo_pausa, salto_f0, factor_caida);

}

/**
 * \class	POS_frontera
 * \brief	Reads the content of the object from a text file
 * \param[in]	entrada	input text file.
 */
int POS_frontera::lee_datos_txt(FILE *entrada) {

	int eti, tp, resultado;

	resultado = fscanf(entrada, "%d\t%d\t%f\t%f\n", &eti, &tp, &salto_f0, &factor_caida);

    etiqueta_morfosintactica = (unsigned char) eti;
    tipo_pausa = (unsigned char) tp;

    return resultado;

}

/**
 * \class	POS_frontera
 * \brief	Writes the content of the object in a binary file
 * \param[in]	salida	output binary file.
 */
int POS_frontera::escribe_datos_bin(FILE *salida) {

	return fwrite(this, sizeof(POS_frontera), 1, salida);

}

/**
 * \class	POS_frontera
 * \brief	Reads the content of the object from a binary file
 * \param[in]	entrada	input binary file.
 */
int POS_frontera::lee_datos_bin(FILE *entrada) {

	return fread(this, sizeof(POS_frontera), 1, entrada);
}


/**
 * \class	Sintagma_frontera
 * \brief	Copy constructor
 */
Sintagma_frontera::Sintagma_frontera(const Sintagma_frontera &original) {
	*this = original;
}

/**
 * \class	Sintagma_frontera
 * \brief	overload of the assignment operator
 */
Sintagma_frontera &Sintagma_frontera::operator= (const Sintagma_frontera &original) {

	if (this != &original) {
    	tipo = original.tipo;
        acento = original.acento;
        elementos = original.elementos;
    }

    return *this;
}


/**
 * \class	Sintagma_frontera
 * \brief	Writes the content of the object in a text file
 * \param[in]	salida	output text file.
 */
int Sintagma_frontera::escribe_datos_txt(FILE *salida) {

	char cadena_categoria[50];
    int tamano = elementos.size();

	if (escribe_tipo_sintagma_grupo((int) tipo, cadena_categoria)) {
    	fprintf(stderr, "Error en Sintagma_frontera::escribe_datos_txt, sintagma (%d) no contemplado.\n", tipo);
    	return 1;
  	}
    
    fprintf(salida, "Sintagma: %s\t%d\n", cadena_categoria, acento);
	fprintf(salida, "Tamaño = %d\n", tamano);

    for (int i = 0; i < tamano; i++) {
    	elementos[i].escribe_datos_txt(salida);
        if (i != tamano - 1)
        	fprintf(salida, "\n");
    }

    return 0;
    
}

/**
 * \class	Sintagma_frontera
 * \brief	Reads the content of the object from a text file
 * \param[in]	entrada	input text file.
 */
int Sintagma_frontera::lee_datos_txt(FILE *entrada) {

	char cadena_categoria[50];
	int tamano, ac;
    POS_frontera pos_tag;

    fscanf(entrada, "Sintagma: %s\t%d\nTamaño = %d\n", cadena_categoria, &ac, &tamano);

    acento = (unsigned char) ac;

	if (lee_tipo_sintagma_grupo(cadena_categoria, &tipo)) {
    	fprintf(stderr, "Error en Sintagma_frontera::lee_datos_txt, sintagma (%s) no contemplado.\n", cadena_categoria);
    	return 1;
  	}

    elementos.clear();
    elementos.reserve(tamano);

    for (int i = 0; i < tamano; i++) {
    	pos_tag.lee_datos_txt(entrada);
        elementos.push_back(pos_tag);
//		elementos[i].lee_datos_txt(entrada);
	}

    return feof(entrada);

}

/**
 * \class	Sintagma_frontera
 * \brief	Writes the content of the object in a binary file
 * \param[in]	salida	output binary file.
 */
int Sintagma_frontera::escribe_datos_bin(FILE *salida) {

    unsigned int tamano = elementos.size();

    fwrite(&tipo, sizeof(unsigned char), 1, salida);
    fwrite(&acento, sizeof(unsigned char), 1, salida);

    fwrite(&tamano, sizeof(unsigned int), 1, salida);

    for (unsigned int i = 0; i < tamano; i++)
    	elementos[i].escribe_datos_bin(salida);

    return 0;

}

/**
 * \class	Sintagma_frontera
 * \brief	Reads the content of the object from a binary file
 * \param[in]	entrada	input binary file.
 */
int Sintagma_frontera::lee_datos_bin(FILE *entrada) {

	unsigned int tamano;
    POS_frontera pos_tag;

	fread(&tipo, sizeof(unsigned char), 1, entrada);
    fread(&acento, sizeof(unsigned char), 1, entrada);
    fread(&tamano, sizeof(unsigned int), 1, entrada);

    elementos.clear();
    elementos.reserve(tamano);

    for (unsigned int i = 0; i < tamano; i++) {
    	pos_tag.lee_datos_bin(entrada);
        elementos.push_back(pos_tag);
//		elementos[i].lee_datos_bin(entrada);
	}

    return feof(entrada);

}


/**
 * \class	Grupo_fonico_frontera
 * \brief	Copy constructor
 */
Grupo_fonico_frontera::Grupo_fonico_frontera(const Grupo_fonico_frontera &original) {
	*this = original;
}

/**
 * \class	Grupo_fonico_frontera
 * \brief	Overload of the assignment operator
 */
Grupo_fonico_frontera &Grupo_fonico_frontera::operator=(const Grupo_fonico_frontera &original) {

    if (this != &original) {
        identificador = original.identificador;
        tipo_grupo = original.tipo_grupo;
        siguiente_sintagma = original.siguiente_sintagma;
        posicion = original.posicion;
        elementos = original.elementos;
    }

    return *this;
}


/**
 * \brief Writes the content of the Grupo_fonico_frontera database in a file
 * \param[in]	fichero	output file
 * \param[in]	grupos_fonicos	database of Grupo_fonico_frontera objects
 * \param[in]	format	0=> text file. Binary file otherwise
 */
int Grupo_fonico_frontera::escribe_fichero_unico(FILE *fichero, estructura_grupos_fonicos *grupos_fonicos,
				int format) {

	unsigned int tamano, j;

    for (int i = 0; i < NUMERO_GF_POSIBLES; i++) {
    	tamano = grupos_fonicos[i].elementos.size();

        (format == 0) ? fprintf(fichero, "%d\n", tamano) :
				        fwrite(&tamano, sizeof(unsigned int), 1, fichero);

        if (format)
	        for (j = 0; j < tamano; j++)
	        	grupos_fonicos[i].elementos[j].escribe_datos_bin(fichero);
        else
	        for (j = 0; j < tamano; j++) {
	        	grupos_fonicos[i].elementos[j].escribe_datos_txt(fichero);
				fprintf(fichero, "\n");
            }

    } // for (int i = 0; ...
    
	return 0;

}


/**
 * \brief Reads the content of the Grupo_fonico_frontera database from a binary file
 * \param[in]	fichero	binary file
 * \param[out]	grupos_fonicos	database of Grupo_fonico_frontera objects
 */
int Grupo_fonico_frontera::lee_fichero_unico(FILE *fichero, estructura_grupos_fonicos *grupos_fonicos) {

	unsigned int tamano, j;
    Grupo_fonico_frontera grupo_fonico;

    for (int i = 0; i < NUMERO_GF_POSIBLES; i++) {

		fread(&tamano, sizeof(unsigned int), 1, fichero);

		grupos_fonicos[i].elementos.clear();
        grupos_fonicos[i].elementos.reserve(tamano);
        
        for (j = 0; j < tamano; j++) {
        	grupo_fonico.lee_datos_bin(fichero);
        	grupos_fonicos[i].elementos.push_back(grupo_fonico);
        }

    } // for (int i = 0; ...

	return 0;

}


/**
 * \class	Grupo_fonico_frontera
 * \brief	Writes the content of the object in a text file
 * \param[in]	salida	output text file.
 */
int Grupo_fonico_frontera::escribe_datos_txt(FILE *salida) {

	char cadena_categoria[50];
    int tamano = elementos.size();

	if (escribe_tipo_sintagma_grupo((int) siguiente_sintagma, cadena_categoria)) {
    	fprintf(stderr, "Error en Grupo_fonico_frontera::escribe_datos_txt, sintagma (%d) no contemplado.\n", siguiente_sintagma);
    	return 1;
  	}

    fprintf(salida, "Grupo fónico:\t%d\nTipo = %d\t%s\tPosición = %d\n", identificador, tipo_grupo, cadena_categoria, posicion);
	fprintf(salida, "Longitud = %d\n", tamano);
    for (int i = 0; i < tamano; i++) {
    	elementos[i].escribe_datos_txt(salida);
        if (i != tamano - 1)
        	fprintf(salida, "\n");
    }

    return 0;

}

/**
 * \class	Grupo_fonico_frontera
 * \brief	Reads the content of the object from a text file
 * \param[in]	entrada	input text file.
 */
int Grupo_fonico_frontera::lee_datos_txt(FILE *entrada) {

   	char cadena_categoria[50];
	int tamano, pos, tg;
    Sintagma_frontera sintagma_tag;

    fscanf(entrada, "Grupo fónico:\t%d\nTipo = %d\t%s\tPosición = %d\n", &identificador, &tg, cadena_categoria, &pos);
    fscanf(entrada, "Longitud = %d\n", &tamano);

    posicion = (unsigned char) pos;
    tipo_grupo = (unsigned char) tg;

	if (lee_tipo_sintagma_grupo(cadena_categoria, &siguiente_sintagma)) {
    	fprintf(stderr, "Error en Grupo_fonico_frontera::lee_datos_txt, sintagma (%s) no contemplado.\n", cadena_categoria);
    	return 1;
  	}

    elementos.clear();
	elementos.reserve(tamano);

    for (int i = 0; i < tamano; i++) {
    	sintagma_tag.lee_datos_txt(entrada);
        elementos.push_back(sintagma_tag);
//		elementos[i].lee_datos_txt(entrada);
	}

 	return feof(entrada);

}


/**
 * \class	Grupo_fonico_frontera
 * \brief	Writes the content of the object in a binary file
 * \param[in]	salida	output binary file.
 */
int Grupo_fonico_frontera::escribe_datos_bin(FILE *salida) {

    unsigned int tamano = elementos.size();

	fwrite(&identificador, sizeof(int), 1, salida);
    fwrite(&tipo_grupo, sizeof(unsigned char), 1, salida);
    fwrite(&siguiente_sintagma, sizeof(unsigned char), 1, salida);
    fwrite(&posicion, sizeof(char), 1, salida);

    fwrite(&tamano, sizeof(unsigned int), 1, salida);

    for (unsigned int i = 0; i < tamano; i++)
    	elementos[i].escribe_datos_bin(salida);

    return 0;

}

/**
 * \class	Grupo_fonico_frontera
 * \brief	Reads the content of the object from a binary file
 * \param[in]	entrada	input binary file.
 */
int Grupo_fonico_frontera::lee_datos_bin(FILE *entrada) {

	unsigned int tamano;
    Sintagma_frontera sintagma_tag;


	fread(&identificador, sizeof(int), 1, entrada);
    fread(&tipo_grupo, sizeof(unsigned char), 1, entrada);
    fread(&siguiente_sintagma, sizeof(unsigned char), 1, entrada);
    fread(&posicion, sizeof(char), 1, entrada);

    fread(&tamano, sizeof(unsigned int), 1, entrada);

    elementos.clear();
	elementos.reserve(tamano);

    for (unsigned int i = 0; i < tamano; i++) {
    	sintagma_tag.lee_datos_bin(entrada);
        elementos.push_back(sintagma_tag);
//		elementos[i].lee_datos_bin(entrada);
	}

 	return feof(entrada);

}

/**
 * \class	Frase_frontera
 * \brief	Copy constructor
 */
Frase_frontera::Frase_frontera(const Frase_frontera &original) {
	*this = original;
}

/**
 * \class	Frase_frontera
 * \brief	Overload of the assignment operator
 */
Frase_frontera &Frase_frontera::operator=(const Frase_frontera &original) {

    if (this != &original) {
        elementos = original.elementos;
    }

    return *this;

}


/**
 * \class	Frase_frontera
 * \brief	Checks if a sequence of Grupo_fonico_frontera objects is valid for a target Frase_frontera object
 * \remarks	A candidate Frase_frontera is considered to be valid for a target Frase frontera if:
 		1.- It has the same number of phonic groups.
        2.- Each phonic group has the same sequence of syntagmas.
 * \param[in]	objetivo	target Frase_frontera object
 * \param[in]	candidato	candidate Frase_frontera object
 * \return	0, if the candidate is not valid. 1, otherwise
 */
int Frase_frontera::candidato_valido(const Frase_frontera &objetivo, const Frase_frontera *candidato) {

	const Sintagma_frontera *apunta_sintagma_objetivo, *apunta_sintagma_candidato;

	if (objetivo.elementos.size() != candidato->elementos.size())
    	return 0;

    // Grupos fónicos
    for (unsigned int i = 0; i < objetivo.elementos.size(); i++) {

		if (objetivo.elementos[i].elementos.size() != candidato->elementos[i].elementos.size())
        	return 0;

        // Sintagmas
        for (unsigned int j = 0; j < objetivo.elementos[i].elementos.size(); j++) {

        	apunta_sintagma_objetivo = &objetivo.elementos[i].elementos[j];
            apunta_sintagma_candidato = &candidato->elementos[i].elementos[j];

            if ( (apunta_sintagma_objetivo->tipo != apunta_sintagma_candidato->tipo) ||
            	 (apunta_sintagma_objetivo->acento != apunta_sintagma_candidato->acento) )
                 if ( ( (apunta_sintagma_objetivo->tipo == GA_RELATIVO) && (apunta_sintagma_candidato->tipo == GA_CONJ_SUBOR) ) ||
					  ( (apunta_sintagma_objetivo->tipo == GA_CONJ_SUBOR) && (apunta_sintagma_candidato->tipo == GA_RELATIVO)) )
                    continue;
                 else
	            	return 0;

        } // for (unsigned int j = 0;...

    } // for (unsigned int = 0;

	return 1;

}


/**
 * \class	Frase_frontera
 * \brief	Copies the prosodic structure from a candidate Frase_frontera object to a sequence of Grupo_acentual_objetivo objects
 * \param[in]	frase_candidata	Frase_frontera object with the information of the contour around the accent (\Delta_{f0] and
slope factor) and prosodic boundaries
 * \param[in]	objetivo	target Frase_frontera object
 * \param	grupos_objetivo	array of target accent groups
 * \param[in]	numero_grupos	number of elements of the preceding array
 * \return	0, in absence of errors
 */
int Frase_frontera::establece_estructura_prosodica(Frase_frontera *frase_candidata,
												   Frase_frontera &frase_objetivo,
													Grupo_acentual_objetivo *grupos_objetivo,
                                                    int numero_grupos) {

	int numero_acentos_en_frase_candidata = 0, numero_pos, p;
    Grupo_fonico_frontera *apunta_GF_cand;
    Grupo_fonico_frontera *apunta_GF_obj;
    Sintagma_frontera *apunta_sintagma_cand, *apunta_sintagma_obj;
    POS_frontera *apunta_pos;
    Grupo_acentual_objetivo *apunta_grupos_objetivo = grupos_objetivo;

    // Grupos fónicos
    for (unsigned int i = 0; i < frase_candidata->elementos.size(); i++) {

    	apunta_GF_cand = &frase_candidata->elementos[i];
		apunta_GF_obj = &frase_objetivo.elementos[i];
		// Sintagmas
    	for (unsigned int j = 0; j < apunta_GF_cand->elementos.size(); j++) {

        	apunta_sintagma_cand = &apunta_GF_cand->elementos[j];
			apunta_sintagma_obj = &apunta_GF_obj->elementos[j];

            if (apunta_sintagma_cand->acento == 0) {
				if (apunta_sintagma_obj->acento != 0) {
                	numero_pos = apunta_grupos_objetivo->numero_elementos_sintagma;
	            	numero_acentos_en_frase_candidata += apunta_grupos_objetivo->numero_elementos_sintagma;
                    for (p = 0; p < numero_pos; p++) {
                    	apunta_grupos_objetivo->tipo_pausa = SIN_PAUSA;
	                    if (apunta_grupos_objetivo != grupos_objetivo)
							apunta_grupos_objetivo->tipo_pausa_anterior = (apunta_grupos_objetivo - 1)->tipo_pausa;
                        apunta_grupos_objetivo->salto_f0 = 0.0;
                        apunta_grupos_objetivo++->factor_caida = 0.0;
                    }
                    continue;
                }
                else
	            	continue; // Sintagma átono
            }
            else
            	if (apunta_sintagma_obj->acento == 0)
                	continue;

            numero_pos = (int) apunta_sintagma_cand->elementos.size();

            numero_acentos_en_frase_candidata += apunta_grupos_objetivo->numero_elementos_sintagma;

            if (numero_acentos_en_frase_candidata > numero_grupos) {
                fprintf(stderr, "Error en establece_estructura_prosodica: número de acentos (%d) superior al \
número de grupos objetivo (%d).\n", numero_acentos_en_frase_candidata, numero_grupos);
                return 1;
            }

            if (numero_pos != apunta_grupos_objetivo->numero_elementos_sintagma) {

				if (numero_pos > apunta_grupos_objetivo->numero_elementos_sintagma) {

                	// Nos quedamos con los últimos:

                    for (p = numero_pos - apunta_grupos_objetivo->numero_elementos_sintagma;
                    	p < numero_pos; p++) {

                    	apunta_pos = &apunta_sintagma_cand->elementos[p];
                        apunta_grupos_objetivo->tipo_pausa = apunta_pos->tipo_pausa;
	                    if (apunta_grupos_objetivo != grupos_objetivo)
							apunta_grupos_objetivo->tipo_pausa_anterior = (apunta_grupos_objetivo - 1)->tipo_pausa;
                        apunta_grupos_objetivo->salto_f0 = apunta_pos->salto_f0;
                        apunta_grupos_objetivo++->factor_caida = apunta_pos->factor_caida;

                    }

                }
                else {

                	// Avanzamos los grupos acentuales:

                    for (p = 0; p < apunta_grupos_objetivo->numero_elementos_sintagma - numero_pos; p++) {
                    	apunta_grupos_objetivo->tipo_pausa = SIN_PAUSA;
	                    if (apunta_grupos_objetivo != grupos_objetivo)
							apunta_grupos_objetivo->tipo_pausa_anterior = (apunta_grupos_objetivo - 1)->tipo_pausa;
                        apunta_grupos_objetivo->salto_f0 = 0.0;
                        apunta_grupos_objetivo++->factor_caida = 0.0;
                    }

                    for (p = 0; p < numero_pos; p++) {

                    	apunta_pos = &apunta_sintagma_cand->elementos[p];
                        apunta_grupos_objetivo->tipo_pausa = apunta_pos->tipo_pausa;
	                    if (apunta_grupos_objetivo != grupos_objetivo)
							apunta_grupos_objetivo->tipo_pausa_anterior = (apunta_grupos_objetivo - 1)->tipo_pausa;
                        apunta_grupos_objetivo->salto_f0 = apunta_pos->salto_f0;
                        apunta_grupos_objetivo++->factor_caida = apunta_pos->factor_caida;


                    }

                }

            } // if (numero_pos !=
            else {

            	for (int k = 0; k < numero_pos; k++) {

                	apunta_pos = &apunta_sintagma_cand->elementos[k];

                    apunta_grupos_objetivo->tipo_pausa = apunta_pos->tipo_pausa;
                    if (apunta_grupos_objetivo != grupos_objetivo)
						apunta_grupos_objetivo->tipo_pausa_anterior = (apunta_grupos_objetivo - 1)->tipo_pausa;
                    apunta_grupos_objetivo->salto_f0 = apunta_pos->salto_f0;
                    apunta_grupos_objetivo++->factor_caida = apunta_pos->factor_caida;

                } // for (unsigned int k = 0; ...

            } // else

        } // for (unsigned int j = 0; ...

    } // for (unsigned int i = 0; ...

    if (numero_acentos_en_frase_candidata != numero_grupos) {
    	fprintf(stderr, "Error en establece_estructura_prosodica: número de acentos (%d) distinto del \
número de grupos objetivo (%d).\n", numero_acentos_en_frase_candidata, numero_grupos);
		return 1;
	}

    return 0;

}


/**
 * \class	Frase_frontera
 * \brief	Sets the default prosodic structure (so that the intonation unit selection algorithm decides by itself)
 * \param	grupos_objetivo	array of target accent groups
 * \param[in]	numero_grupos	number of elements of the preceding array
 */
void Frase_frontera::establece_estructura_prosodica_por_defecto(Grupo_acentual_objetivo *grupos_objetivo, int numero_grupos) {

    for (int contador = 0; contador < numero_grupos; contador++, grupos_objetivo++) {

    	grupos_objetivo->salto_f0 = 0;
        grupos_objetivo->factor_caida = 0;

    } // for (int contador = 0; ...

}


/**
 * \class	Frase_frontera
 * \brief	Writes the content of the object in a text file
 * \param[in]	salida	ouput text file.
 */
int Frase_frontera::escribe_datos_txt(FILE *salida) {

    int tamano = elementos.size();

	fprintf(salida, "Frase:\nTamaño = %d\n", tamano);
    for (int i = 0; i < tamano; i++) {
    	elementos[i].escribe_datos_txt(salida);
        if (i != tamano - 1)
        	fprintf(salida, "\n");
    }

    return 0;

}

/**
 * \class	Frase_frontera
 * \brief	Reads the content of the object from a text file
 * \param[in]	entrada	input text file.
 */
int Frase_frontera::lee_datos_txt(FILE *entrada) {

	int tamano;
	int resultado;

    Grupo_fonico_frontera grupo_fonico_tag;

    resultado = fscanf(entrada, "Frase:\nTamaño = %d\n", &tamano);

    if ( (resultado == 0) || (resultado == EOF) )
    	return -2;

	elementos.clear();
    elementos.reserve(tamano);

    for (int i = 0; i < tamano; i++) {
        grupo_fonico_tag.lee_datos_txt(entrada);
        elementos.push_back(grupo_fonico_tag);
	}

    return feof(entrada);

}

/**
 * \class	Frase_frontera
 * \brief	Writes the content of the object in a binary file
 * \param[in]	salida	ouput text file.
 */
int Frase_frontera::escribe_datos_bin(FILE *salida) {

    unsigned int tamano = elementos.size();

	fwrite(&tamano, sizeof(unsigned int), 1, salida);
    
    for (unsigned int i = 0; i < tamano; i++)
    	elementos[i].escribe_datos_bin(salida);

    return 0;

}

/**
 * \class	Frase_frontera
 * \brief	Reads the content of the object from a binary file
 * \param[in]	entrada	input binary file.
 */
int Frase_frontera::lee_datos_bin(FILE *entrada) {

	unsigned int tamano;
    Grupo_fonico_frontera grupo_fonico_tag;

    fread(&tamano, sizeof(unsigned int), 1, entrada);

    elementos.clear();
    elementos.reserve(tamano);

    for (unsigned int i = 0; i < tamano; i++) {
        grupo_fonico_tag.lee_datos_txt(entrada);
        elementos.push_back(grupo_fonico_tag);
//		elementos[i].lee_datos_bin(entrada);
	}

    return feof(entrada);

}

