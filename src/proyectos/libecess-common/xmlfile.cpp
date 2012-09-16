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
 
#include <iostream>
#include <fstream>
#include <vector>
#include <expat.h>
#include "xmlfile.hpp"


/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 */
void XmlFile::openElement(const char* name) {
	*stream<<"<"<<name<<">\n";
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 */
void XmlFile::openElement(const char* name, int ind) 
{
	while (ind--){
		*stream << "\t";
	}
	*stream << "<" << name << ">\n";
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 * \param atts 
 */
void XmlFile::openElement(const char* name, vector<string> atts) 
{

	*stream<<"<"<<name;
	for (unsigned int i = 0; i < atts.size(); i += 2) {
		*stream << ' ' << atts[i].c_str()<< "=\"" << atts[i + 1].c_str() << "\"" ;
	}
	*stream << ">\n" ;
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 * \param atts 
 * \param ind
 */
void XmlFile::openElement(const char* name, vector<string> atts, int ind)
{

	while (ind--){
		*stream << "\t";
	}
	*stream << "<" << name;
	for (unsigned int i = 0; i < atts.size(); i += 2) {
		*stream << ' ' << atts[i].c_str() << "=\"" << atts[i + 1].c_str() << "\"" ;
	}
	*stream << ">\n" ;
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 */
void XmlFile::closeElement(const char* name) 
{
	*stream << "</" << name << ">\n";
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 */
void XmlFile::closeElement(const char* name, int ind) {
	while (ind--){
		*stream << "\t";
	}
	*stream << "</" << name << ">\n";
}
/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 */
void XmlFile::closeElement(const char* name, int ind, const char *cad) {
	while (ind--){
		*stream << "\t";
	}
	*stream << "<" << name << ">" << cad << "</" << name << ">\n";
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 * \param atts 
 */
void XmlFile::closeElement(const char* name, vector<string> atts) 
{
	*stream << "<" << name;
	for (unsigned int i = 0; i < atts.size(); i += 2) {
		*stream << ' ' << atts[i].c_str() << "=\"" << atts[i + 1].c_str() << "\"" ;
	}
	*stream << "/>\n";
}

/** 
 * \author fmendez
 * \brief 
 * 
 * \param name 
 * \param atts 
 */
void XmlFile::closeElement(const char* name, vector<string> atts, int ind) 
{
	while (ind--){
		*stream << "\t";
	}
	*stream << "<" << name;
	for (unsigned int i = 0; i < atts.size(); i += 2) {
		*stream << ' ' << atts[i].c_str() << "=\"" << atts[i + 1].c_str() << "\"" ;
	}
	*stream << "/>\n";
}

void XmlFile::write(const char* cad)
{
	*stream << cad << '\n';
}

void XmlFile::write(const char* cad, int ind)
{
	while (ind--){
		*stream << "\t";
	}
	*stream << cad << '\n';
}

XmlFile::XmlFile() 
{
	stream=&cout;
	this->write("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>");
	this->write("<!DOCTYPE tts PUBLIC \"ECESS\" \"ecess.dtd\">");
	this->openElement("tts xml:lang=\"gl\"");
}

XmlFile::XmlFile(const char * name) 
{
	file.open(name);
	stream = &file;
	this->write("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>");
	this->write("<!DOCTYPE tts PUBLIC \"ECESS\" \"ecess.dtd\">");
	this->openElement("tts xml:lang=\"gl\"");
}

XmlFile::~XmlFile() 
{
	this->closeElement("tts");
	file.close();
}

