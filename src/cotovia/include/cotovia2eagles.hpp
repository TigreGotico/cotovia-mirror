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
 

#ifndef COTOVIAEAGLES
#define COTOVIAEAGLES
#include <tip_var.hpp>
typedef struct {

  char *palabra;
  char categoria[5][10];     //Eagles style ;).
  char *lema;
} categoriaGenerica;

class CotoviaEagles {

private:
  map<int,string> oC2E;
  map<string,int> oE2C;
  map<int,string> numero2Eagles;
  map<int,string> genero2Eagles;
  CotoviaEagles(void);
  static CotoviaEagles *CE;

public:
  static inline CotoviaEagles *getSingleton(void){return (NULL==CE)? CE=new CotoviaEagles():CE;};
  static inline void disposeSingleton(void){delete CE;CE=NULL;};
  inline string mGetC2E(int idx){ return oC2E[idx];};
  unsigned char mGetE2C(string sIdx);
  inline string mGetN2E(int sIdx) {return numero2Eagles[sIdx];};
  inline string mGetG2E(int sIdx) {return genero2Eagles[sIdx];};
  int mDG(string Idx);
  unsigned int mGetTVCotovia(string categoriaEagles);
  string mGetTVEagles(unsigned char codigo);     
};





#endif

