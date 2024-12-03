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
 
#ifndef ECESSINTERFACES_HPP
#define ECESSINTERFACES_HPP

#define STRESS_MARK '\''

/**
 * \author Francisco Mendez Pazo <fmendez@gts.tsc.uvigo.es>
 */
class
#ifdef _WIN32
__declspec(dllexport) 
#endif
EcessInterfaces{

	XmlFile* output;
	Lcstar *lc;
	invLcstar *invlc;
	Token * f_tokenizada;
	int * numero_de_tokens;

	char * escribe_transcripcion(char *tr);
	void escribe_phon(	vector<Vector_descriptor_objetivo>::iterator* in, int &n, char * pal);
	void get_verb_atts(vector<string> &atts, unsigned char codigo);

	public:
	void escribe_salida_frase(char mod, vector<Vector_descriptor_objetivo> &objetivo, int n);
	char * lee_transcripcion(char *tr);
	void lcstar2cotovia(const char *pos, const char **atts, t_frase_separada * fs);	

	EcessInterfaces(int * n_tok, Token * f_tok);
	void Inicio(char * file);
	~EcessInterfaces();

};
#endif

