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
 
#ifndef XMLFILE_HPP
#define XMLFILE_HPP

using namespace std;

class
#ifdef _WIN32
__declspec(dllexport) 
#endif
XmlFile {
	private:
		std::ostream*  stream;
		std::ofstream file;
	public:
		XmlFile();
		XmlFile(const char* name);
		~XmlFile();
		void write(const char* cad);
		void openElement(const char* name);
		void openElement(const char* name, vector<string> atts);
		void closeElement(const char* name);
		void closeElement(const char* name, vector<string> atts);
		void write(const char* cad, int ind);
		void openElement(const char* name, int ind);
		void openElement(const char* name, vector<string> atts, int ind);
		void closeElement(const char* name, int ind);
		void closeElement(const char* name, vector<string> atts, int ind);
		void closeElement(const char* name, int ind, const char *cad);
};
#endif

