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
 
#include <fstream>
#include <vector>
#include <string.h>
using namespace std;

#include "modos.hpp"
#include "tip_var.hpp"
#include "fonemas.hpp"
#include "descriptor.hpp"
#include "utilidades.hpp"
#include "xmlfile.hpp"
#include "lc-star.hpp"
#include "ecessinterfaces.hpp"

Lcstar *Lcstar::lc=NULL;
invLcstar *invLcstar::invlc=NULL;

/**
 * \brief 
 * \author fmendez 
 *
 * \param file  
 * \param n_tok  
 * \param f_tok  
 *
 * \return 
 */
//EcessInterfaces::EcessInterfaces(int * n_tok, Token * f_tok, int * n_uni, Vector_descriptor_objetivo * uni){
EcessInterfaces::EcessInterfaces(int * n_tok, Token * f_tok){

	lc = Lcstar::getLcstar();
	invlc = invLcstar::getinvLcstar();

	numero_de_tokens = n_tok;
	f_tokenizada = f_tok;
	//numero_de_unidades = n_uni;
	//unidades = uni;
	
	output = NULL;
}

/** 
 * \brief 
 * \author fmendez 
 * \param file 
 * \return 
 */
void EcessInterfaces::Inicio(char * file){
	if (file == NULL) {
		output = new XmlFile();
	}
	else{
		output = new XmlFile(file);
	}
}


/**
 * \brief 
 * \author fmendez 
 *
 * \return 
 */
EcessInterfaces::~EcessInterfaces()
{
	delete lc;
	delete invlc;
	if (output) {
		delete output;
	}
}


/** 
 * \author fmendez
 * \brief Escribe la salida del módulo de procesado de texto. 
 */
void EcessInterfaces::escribe_salida_frase(char mod, vector<Vector_descriptor_objetivo> &objetivo, int n){

	output->openElement("s");
	vector<string> atts;
	//char pepe[FILENAME_MAX];
	vector<Vector_descriptor_objetivo>::iterator  in = objetivo.begin();


	for (int j = 0; j < *numero_de_tokens; j++){
		atts.clear();
		atts.push_back("token");
		if ( *f_tokenizada[j].token == '"') {
			atts.push_back("&quot;");
		}
		else if (*f_tokenizada[j].token == '&'){
			atts.push_back("&amp;");
		}
		else {
			atts.push_back(f_tokenizada[j].token);
		}
		output->openElement("TOKEN", atts, 1);
		for (int i = 0; i < f_tokenizada[j].n_palabras; i++) {
			atts.clear();
			atts.push_back("word");
			if (*f_tokenizada[j].token == '"') {
				atts.push_back("&quot;");
			}
			else if (*f_tokenizada[j].token == '&'){
				atts.push_back("&amp;");
			}
			else {
				atts.push_back(f_tokenizada[j].palabras[i].pal);
			}
			//atts[1] = (f_tokenizada[j].palabras[i].pal);
			output->openElement("WORD", atts, 2);
			atts.clear();
			output->openElement("POS", 3);
			if (f_tokenizada[j].palabras[i].cat_gramatical[0] == VERBO) {
				get_verb_atts(atts, f_tokenizada[j].palabras[i].cat_verbal[0]);
				output->closeElement("VER", atts, 4);
				//output->closeElement("VERBO", atts, 4);
			}
			else {
				if (f_tokenizada[j].palabras[i].xenero) {
					atts.push_back("gender");
					atts.push_back(lc->genero[f_tokenizada[j].palabras[i].xenero]);
				}
				if (f_tokenizada[j].palabras[i].numero) {
					atts.push_back("number");
					atts.push_back(lc->numero[f_tokenizada[j].palabras[i].numero]);
				}
				output->closeElement(lc->categoria[f_tokenizada[j].palabras[i].cat_gramatical[0]].c_str(), atts, 4);
				//output->closeElement(escribe_categoria_gramatical(f_tokenizada[j].palabras[i].cat_gramatical[0], pepe), atts, 4);
			}
			output->closeElement("POS", 3);
			/* 				atts.clear();
			 * 				output->openElement("LEMMA", 3);
			 * 				if (*f_tokenizada[j].palabras[i].lema) {
			 * 					output->write(f_tokenizada[j].palabras[i].lema, 4);
			 * 				}
			 * 				else {
			 * 					output->write(f_tokenizada[j].palabras[i].pal, 4);
			 * 				}
			 * 				output->closeElement("LEMMA", 3);
			 */
      /*
			 *output->openElement("PHONETIC", 3);
			 *output->write(escribe_transcripcion(f_tokenizada[j].palabras[i].tr_fon), 4);
			 *output->closeElement("PHONETIC", 3);
       */
			//output->write(f_tokenizada[j].palabras[i].tr_fon, 4);
			output->closeElement("PHONETIC", 3, escribe_transcripcion(f_tokenizada[j].palabras[i].tr_fon));
			if (mod == 2) {
				escribe_phon(&in, n, f_tokenizada[j].palabras[i].pal);	
			}	
			output->closeElement("WORD", 2);
		}
		output->closeElement("TOKEN", 1);
	}
	output->closeElement("s");
}

void EcessInterfaces::escribe_phon(	vector<Vector_descriptor_objetivo>::iterator* in, int &n, char * pal) {
	vector<string> a;
	char duracion[6];
	char d[6];
	char f[3];
	float fi,fm,ff;
	int dur;
	//Vector_descriptor_objetivo * u = unidades;
	while (n && !strcmp((*in)->palabra, pal)){
	//for(int i = 0; i < n; i++, in++){
		a.clear();
		a.push_back("phoneme");
		a.push_back((*in)->semifonema);
		a.push_back("duration");
		dur = (int) (((*in)->duracion_1 + (*in)->duracion_2) * 1000.0);
		sprintf(duracion, "%d", dur);
		a.push_back(duracion);
		output->openElement("PHON", a, 3);
		output->openElement("frequency", 4);
		a.clear();
		a.push_back("time");
		a.push_back("0");
		a.push_back("value");
		(*in)->devuelve_frecuencias(&fi, &fm, &ff);
		sprintf(f, "%d", (int) fi);
		a.push_back(f);
		output->closeElement("pair", a, 5);
		a.clear();
		a.push_back("time");
		sprintf(d, "%d", (int) ((*in)->duracion_1 * 1000.0));
		a.push_back(d);
		a.push_back("value");
		sprintf(f, "%d", (int) fm);
		a.push_back(f);
		output->closeElement("pair", a, 5);
		a.clear();
		a.push_back("time");
		a.push_back(duracion);
		a.push_back("value");
		sprintf(f, "%d", (int) ff);
		a.push_back(f);
		output->closeElement("pair", a, 5);
		output->closeElement("frequency", 4);
		output->openElement("energy", 4);
		a.clear();
		a.push_back("time");
		a.push_back("0");
		a.push_back("value");
		sprintf(f, "%d", (int) (*in)->potencia);
		a.push_back(f);
		output->closeElement("pair", a, 5);
		output->closeElement("energy", 4);
		if ((*in)->info_silaba) {
			a.clear();
			a.push_back("last-syllable");
			if ((*in)->info_silaba == 2) {
				a.push_back("true");
			}
			else {
				a.push_back("false");
			}
			if ((*in)->acento) {
				a.push_back("accent");
				a.push_back("primary");
			}
			output->openElement("syllable", a, 4);
			//ésto es para las pausas
      /*
			 *a.clear();
			 *a.push_back("time");
			 *a.push_back("0");
			 *a.push_back("strength");
			 *a.push_back("weak");
			 *output->closeElement("break", a, 5);
       */
			//fin pausas
			output->closeElement("syllable", 4);
		}
		output->closeElement("PHON", 3);
		n--;
		(*in)++;
	}
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param tr  
 *
 * \return 
 */
char *EcessInterfaces::escribe_transcripcion(char *tr){
	char *str = tr;
	char *pp;
	pp = strchr(str, '^');
	while (pp!=NULL) {
		*pp = *(pp - 1);
		*(pp - 1) = STRESS_MARK;
		str = pp;
		pp = strchr(str, '^');
	}
	return tr;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param tr  
 *
 * \return 
 */
char *EcessInterfaces::lee_transcripcion(char *tr){
	char *str = tr;
	char *pp;
	pp = strchr(str, STRESS_MARK);
	while (pp != NULL) {
		*pp = *(pp + 1);
		*(pp + 1) = '^';
		str = pp ;
		pp = strchr(str, STRESS_MARK);
	}
	return tr;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param atts  
 * \param codigo  
 *
 * \return 
 */
void EcessInterfaces::get_verb_atts(vector<string> &atts, unsigned char codigo){
	switch (codigo) {
		case 180:
		case 181:
		case 182:
			atts.push_back("mood");
			atts.push_back("gerund");
			break;
		case 183:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 184:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		case 185:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 186:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		case 187:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 188:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		case 189:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 190:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		case 191:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 192:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("singular");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		case 193:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("masculine");			
			break;
		case 194:
			atts.push_back("mood");
			atts.push_back("participle");
			atts.push_back("number");			
			atts.push_back("plural");			
			atts.push_back("gender");			
			atts.push_back("feminine");			
			break;
		default :	
			unsigned char tense  = (unsigned char) ((codigo - 1) / 18);
			unsigned char person = (unsigned char) ((codigo - 1) % 6);
			switch (person) {
				case 0:
					atts.push_back("number");			
					atts.push_back("singular");			
					atts.push_back("person");			
					atts.push_back("1");			
					break;
				case 1:
					atts.push_back("number");			
					atts.push_back("singular");			
					atts.push_back("person");			
					atts.push_back("2");			
					break;
				case 2:
					atts.push_back("number");			
					atts.push_back("singular");			
					atts.push_back("person");			
					atts.push_back("3");			
					break;
				case 3:
					atts.push_back("number");			
					atts.push_back("plural");			
					atts.push_back("person");			
					atts.push_back("1");			
					break;
				case 4:
					atts.push_back("number");			
					atts.push_back("plural");			
					atts.push_back("person");			
					atts.push_back("2");			
					break;
				case 5:
					atts.push_back("number");			
					atts.push_back("plural");			
					atts.push_back("person");			
					atts.push_back("3");			
					break;
			}
			switch (tense) {
				case 0:
					atts.push_back("mood");
					atts.push_back("indicative");
					atts.push_back("tense");
					atts.push_back("present");
					break;
				case 1:
					atts.push_back("mood");
					atts.push_back("indicative");
					atts.push_back("tense");
					atts.push_back("imperfect");
					break;
				case 2:
					atts.push_back("mood");
					atts.push_back("indicative");
					atts.push_back("tense");
					atts.push_back("past");
					break;
				case 3:
					atts.push_back("mood");
					atts.push_back("indicative");
					atts.push_back("tense");
					atts.push_back("pluperfect");
					break;
				case 4:
					atts.push_back("mood");
					atts.push_back("indicative");
					atts.push_back("tense");
					atts.push_back("future");
					break;
				case 5:
					atts.push_back("mood");
					atts.push_back("conditional");
					break;
				case 6:
					atts.push_back("mood");
					atts.push_back("subjunctive");
					atts.push_back("tense");
					atts.push_back("present");
					break;
				case 7:
					atts.push_back("mood");
					atts.push_back("subjunctive");
					atts.push_back("tense");
					atts.push_back("imperfect");
					break;
				case 8:
					atts.push_back("mood");
					atts.push_back("subjunctive");
					atts.push_back("tense");
					atts.push_back("future");
					break;
				case 9:
					atts.push_back("mood");
					atts.push_back("imperative");
					break;
				case 11:
					atts.push_back("mood");
					atts.push_back("infinitive");
					break;
			}
	}
}


/**
 * \brief 
 * \author fmendez 
 *
 * \param[in] pos  
 * \param[in] atts
 * \param[out] fs
 *
 */
void EcessInterfaces::lcstar2cotovia(const char *pos, const char **atts, t_frase_separada * fs){
	
	for (int i = 0; atts[i]; i += 2) {
#ifdef _TRAZA
		fprintf(stderr," %s='%s'", atts[i], atts[i + 1]);
#endif
		if (!strcasecmp(atts[i], "gender")) {
			fs->xenero = invlc->genero[*(atts[i + 1])];
		}
		else if (!strcasecmp(atts[i], "number")) {
			fs->numero = invlc->numero[*(atts[i + 1])];
		}

	}
	fs->cat_gramatical[0] = invlc->categoria[string(pos)];
	
}		/* -----  end of method EcessInterfaces::lcstar2cotovia  ----- */

