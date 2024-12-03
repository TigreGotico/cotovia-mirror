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
#include "tip_var.hpp"
//#include "cotovia.hpp"
//#include "modos.hpp"

//Nuevos include...
#include "letras.hpp"
#include "ansi.hpp"
#include "sil_acen.hpp"
#include "verbos.hpp"

//#define _TRAZA_ANALISIS_VERBAL

char matriz_cliticos_tiempos_gallego[26][217]={
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1},
	{1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1},
	{1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1},
	{0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1},
	{0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1},
	{1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
	{1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
	{1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
	{1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
	{1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1},
	{1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1},
	{1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
	{1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
	{0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1},
	{1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
	{0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
};

t_dic_enclitico pron_encliticos_gallego[]={
	/* a */			{"a", 1},
	/* cha */		{"ahc", 8},
	/* la */			{"al", 2},
	/* llela */		{"alell", 17},
	/* chellela */	{"alellehc", 11},
	/* lla */		{"all", 16},
	/* chella */	{"allehc", 10},
	/* mella */		{"allem", 5},
	/* nola */		{"alon", 20},
	/* chenola */	{"alonehc", 14},
	/* llenola */	{"alonell", 18},
	/* chellenola */{"alonellehc", 15},
	/* vola */		{"alov", 22},
	/* chevola */	{"alovehc", 11},
	/* llevola */	{"alovell", 18},
	/* chellevola */{"alovellehc", 12},
	/* mevola */	{"alovem", 6},
	/* ma */			{"am", 4},
	/* chema */		{"amehc", 9},
	/* llema */		{"amell", 17},
	/* na */			{"an", 3},
	/* che */		{"ehc", 8},
	/* seche */		{"ehces", 24},
	/* lle */		{"ell", 16},
	/* chelle */	{"ellehc", 10},
	/* sechelle */	{"ellehces", 25},
	/* melle */		{"ellem", 5},
	/* chemelle */	{"ellemehc", 11},
	/* selle */		{"elles", 24},
	/* me */			{"em", 4},
	/* cheme */		{"emehc", 9},
	/* secheme */	{"emehces", 25},
	/* lleme */		{"emell", 17},
	/* chelleme */	{"emellehc", 14},
	/* seme */		{"emes", 24},
	/* se */			{"es", 23},
	/* te */			{"et", 7},
	/* o */			{"o", 1},
	/* cho */		{"ohc", 8},
	/* lo */			{"ol", 2},
	/* llelo */		{"olell", 17},
	/* chellelo */	{"olellehc", 11},
	/* llo */		{"oll", 16},
	/* chello */	{"ollehc", 10},
	/* mello */		{"ollem", 5},
	/* nolo */		{"olon", 20},
	/* chenolo */	{"olonehc", 14},
	/* llenolo */	{"olonell", 18},
	/* chellenolo */{"olonellehc", 15},
	/* volo */		{"olov", 22},
	/* chevolo */	{"olovehc", 11},
	/* llevolo */	{"olovell", 18},
	/* chellevolo */{"olovellehc", 12},
	/* mevolo */	{"olovem", 6},
	/* mo */			{"om", 4},
	/* chemo */		{"omehc", 9},
	/* llemo */		{"omell", 17},
	/* no */			{"on", 3},
	/* as */			{"sa", 1},
	/* chas */		{"sahc", 8},
	/* las */		{"sal", 2},
	/* llelas */	{"salell", 17},
	/* chellelas */{"salellehc", 11},
	/* llas */		{"sall", 16},
	/* chellas */	{"sallehc", 10},
	/* mellas */	{"sallem", 5},
	/* nolas */		{"salon", 20},
	/* chenolas */	{"salonehc", 14},
	/* llenolas */	{"salonell", 18},
	/* chellenolas */{"salonellehc", 15},
	/* volas */		{"salov", 22},
	/* chevolas */	{"salovehc", 11},
	/* llevolas */	{"salovell", 18},
	/* chellevolas */{"salovellehc", 12},
	/* mevolas */	{"salovem", 6},
	/* mas */		{"sam", 4},
	/* chemas */	{"samehc", 9},
	/* llemas */	{"samell", 17},
	/* nas */		{"san", 3},
	/* lles */		{"sell", 16},
	/* chelles */	{"sellehc", 10},
	/* sechelles */{"sellehces", 25},
	/* melles */	{"sellem", 5},
	/* chemelles */{"sellemehc", 11},
	/* selles */	{"selles", 24},
	/* os */			{"so", 1},
	/* chos */		{"sohc", 8},
	/* los */		{"sol", 2},
	/* llelos */	{"solell", 17},
	/* chellelos */{"solellehc", 11},
	/* llos */		{"soll", 16},
	/* chellos */	{"sollehc", 10},
	/* mellos */	{"sollem", 5},
	/* nolos */		{"solon", 20},
	/* chenolos */	{"solonehc", 14},
	/* llenolos */	{"solonell", 18},
	/* chellenolos */{"solonellehc", 15},
	/* volos */		{"solov", 22},
	/* chevolos */	{"solovehc", 11},
	/* llevolos */	{"solovell", 18},
	/* chellevolos */{"solovellehc", 12},
	/* mevolos */	{"solovem", 6},
	/* mos */		{"som", 4},
	/* chemos */	{"somehc", 9},
	/* sechemos */	{"somehces", 25},
	/* llemos */	{"somell", 17},
	/* nos */		{"son", 19},
	/* chenos */	{"sonehc", 13},
	/* llenos */	{"sonell", 17},
	/* chellenos */{"sonellehc", 14},
	/* senos */		{"sones", 24},
	/* vos */		{"sov", 21},
	/* chevos */	{"sovehc", 10},
	/* sechevos */	{"sovehces", 25},
	/* llevos */	{"sovell", 17},
	/* chellevos */{"sovellehc", 11},
	/* mevos */		{"sovem", 5},
	/* chemevos */	{"sovemehc", 11},
	/* sevos */		{"soves", 24}
};					/* Numero de encliticos: 119 */

char matriz_cliticos_tiempos_castellano[5][217]={
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

t_dic_enclitico pron_encliticos_castellano[]={
	{"al",1},
	{"alem",1},
	{"alemes",3},
	{"ales",3},
	{"alet",1},
	{"aletes",3},
	{"also",4},
	{"alson",2},
	{"el",1},
	{"elem",1},
	{"elemes",3},
	{"eles",3},
	{"elet",1},
	{"eletes",3},
	{"elso",4},
	{"elson",2},
	{"em",1},
	{"emes",3},
	{"es",3},
	{"et",1},
	{"etes",3},
	{"ol",1},
	{"olem",1},
	{"olemes",3},
	{"oles",3},
	{"olet",1},
	{"oletes",3},
	{"olso",4},
	{"olson",2},
	{"sal",1},
	{"salem",1},
	{"salemes",3},
	{"sales",3},
	{"salet",1},
	{"saletes",3},
	{"salso",4},
	{"salson",2},
	{"sel",1},
	{"selem",1},
	{"selemes",3},
	{"seles",3},
	{"selet",1},
	{"seletes",3},
	{"selso",4},
	{"selson",2},
	{"so",4},
	{"soes",3},
	{"sol",1},
	{"solem",1},
	{"solemes",3},
	{"soles",3},
	{"solet",3},
	{"soletes",3},
	{"solso",4},
	{"solson",2},
	{"son",2},
	{"sones",3}
};


chapuza *matriz_cliticos_tiempos;
t_dic_enclitico *pron_encliticos;
int NU_PRON_ENCLITICOS;

int iniciaCliticos(char idioma) {

	if (idioma==GALEGO){
		matriz_cliticos_tiempos=matriz_cliticos_tiempos_gallego;
		pron_encliticos=pron_encliticos_gallego;
		NU_PRON_ENCLITICOS=sizeof(pron_encliticos_gallego)/sizeof(pron_encliticos_gallego[0]);
	}
	if (idioma==CASTELLANO) {
		matriz_cliticos_tiempos=matriz_cliticos_tiempos_castellano;
		pron_encliticos=pron_encliticos_castellano;
		NU_PRON_ENCLITICOS=sizeof(pron_encliticos_castellano)/sizeof(pron_encliticos_castellano[0]);

	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////



const char *excepciones_verbos[]={
"apostolo",
"apostolos",
"arbórea",
"arbóreas",
"arbóreo",
"arbóreos",
"arxéntea",
"arxénteas",
"arxénteo",
"arxénteos",
"bulinte",
"bálano",
"bálanos",
"búfalo",
"búfalos",
"cabelo",
"cabelos",
"carroucha",
"carrouchas",
"carroucho",
"carrouchos",
"catarse",
"cilíndrea",
"cilíndreas",
"cilíndreo",
"cilíndreos",
"concluínte",
"constituínte",
"contribuínte",
"cucaracha",
"cucarachas",
"cérea",
"céreas",
"céreo",
"céreos",
"deporte",
"derme",
"descascaros",
"descrente",
"devesas",
"doela",
"doelas",
"doelo",
"doelos",
"escaparate",
"escándalo",
"escándalos",
"espadarte",
"espumarallo",
"espumarallos",
"espádoa",
"espádoas",
"este",
"forma",
"formas",
"fornos",
"forte",
"fuxinte",
"gálea",
"gáleas",
"instituínte",
"latina",
"latinas",
"latino",
"latinos",
"leste",
"mariscala",
"medoucho",
"medouchos",
"mentirana",
"mentiranas",
"míase",
"nacela",
"nacelas",
"palabra",
"ponche",
"poncho",
"ponchos",
"pétalo",
"pétalos",
"reconstituínte",
"referinte",
"relente",
"relevos",
"revella",
"revellas",
"revello",
"revellos",
"rexurdinte",
"rinte",
"rosea",
"roseas",
"roseo",
"roseos",
"ríos",
"saíncho",
"saínchos",
"sonse",
"sorrinte",
"sorríos",
"subseguinte",
"sécola",
"sécolas",
"síntoma",
"síntomas",
"tortola",
"tortolas",
"tortolo",
"tortolos",
"tremelo",
"tremelos",
"térrea",
"térreas",
"térreo",
"térreos",
"veste",
"vídeo",
"vídeos",
"xemina",
"xeminas",
"xurdinte",
"xácea",
"xáceas",
"xémea",
"xémeas",
"xémeo",
"xémeos",
"xénese",
"átomo",
"átomos",
"átonos",
"ídolo",
"ídolos",
"íngreme",
""
};




/*****************************************************************************
 *  void recompor_verbo_sen_segunda_forma(char *pal_analizada,char * pal_nor-*
 * -mal): Se existe a segunda forma do artigo pegado o verbo, hai que elimi- *
 * -na-la e restitui-la forma orixinal sen o artigo. Os parametros:          *
 *    - pal_normal: pteiro. o verbo coa segunda forma do artigo. Parametro de*
 *          entrada. (IN).                                                   *
 *    - pal_analizada: pteiro. no que se devolve o verbo restituido a forma  *
 *          orixinal, e pois o parametro de saida da funcion. (OUT).         *
 *****************************************************************************/
void gbm_verbos::recompor_verbo_sen_segunda_forma(char *pal_analizada,char * pal_normal)
{
 char *pos_acento, forma_en_as=-1,index1;
 int pos_lista, index;

 pos_acento=strpbrk(pal_normal,"áéíóú");
 if (pos_acento!=NULL)              // 2ª forma do artigo formouse cun -s
      strcat(pal_analizada,"s");
 else {                             // 2ª forma do artigo cun -r (infinitivo)
   if (strlen(pal_analizada) >2 && strcmp(pal_analizada+strlen(pal_analizada)-2,"ra")==0) {
      pal_analizada[strlen(pal_analizada)-1]=0;
/* Quitamo-lo 'a' para buscar na lista da primeira conxugacion.               */
/*
      if ((pos_lista=comprobar_en_lista_de_verbos((char**)raices_de_verbos,
           sizeof(t_raices_de_verbos),NU_RAICES_DE_VERBOS, pal_analizada))<0) {
*/
//ZA
      if ((pos_lista=comprobar_en_lista_de_verbos(pal_analizada, idioma))<0) {



/* Comprobamo-lo caso de verbos como xerar no que a terminacion "-era" non se
   debe a desinencia senon a raiz, polo que non e futuro e polo tanto engade
   "s". Estes casos presentanse nos verbos da primeira conxugacion rematados
   en -erar.                                                                  */
         for (index=0;index<NUM_INFINITIVO;index++)
            for (index1=0;index1<NUM_MODELOS;index1++) {
               if(raices_de_verbos[pos_lista].modelo[index].grupos[index1])
                  forma_en_as=ON;
               else if(forma_en_as!=ON)
                  forma_en_as=OFF;
            }                       // END FOR de index1
            if (forma_en_as) strcat(pal_analizada,"as");
            else if (!forma_en_as) strcat(pal_analizada,"ar");
      }                             // END IF comprobar_en_lista_de_verbos
      else
         strcat(pal_analizada,"ar");
   }                                // END IF terminacion -ra
   else
      strcat(pal_analizada,"r");
 }                                  // END ELSE 2ª forma do artigo cun -r
 return;
}

/*****************************************************************************
 *  int ditongo_decrecente(char * primeira_vocal): Funcion booleana que de-  *
 * -volve un 1  se efectivamente se trata dun diptongo decrecente, e un 0 en *
 * caso contrario.                                                           *
 *****************************************************************************/
int gbm_verbos::ditongo_decrecente(char * primeira_vocal)
{
   return  ((vocal_feble((unsigned char)*primeira_vocal) || vocal_aberta((unsigned char)*primeira_vocal)) &&
            vocal_feble((unsigned char)*(primeira_vocal+2)));
}

/*****************************************************************************
 *  int acentua_graficamente(char * pos_acento): Pon un acento grafico sobre *
 * a vocal que se lle pasa como parametro.                                   *
 *****************************************************************************/
int gbm_verbos::acentua_graficamente(char * pos_acento)
{
   switch (*(pos_acento)) {         // BEGIN SWITCH
      case 'a':
         *(pos_acento)='á';
         return 1;
      case 'e':
         *(pos_acento)='é';
         return 1;
      case 'o':
         *(pos_acento)='ó';
         return 1;
      case 'i':
         *(pos_acento)='í';
         return 1;
      case 'u':
         *(pos_acento)='ú';
         return 1;

   }                                // END SWITCH
   return 0;
}

/*****************************************************************************
 *  int acento_para_desfacer_ditongo(char *recorredor): Devolve un 1 se hai  *
 * que por un acento para desface-lo diptongo e cero en caso contrario.      *
 *****************************************************************************/
int gbm_verbos::acento_para_desfacer_ditongo(char * pal,char *rec){
// recorredor apunta o ^
	if ( rec > pal && (*(rec-1)=='u' || *(rec-1)=='i') ) {
   	if ((rec>= (pal+3) && *(rec-2)=='-' && vocal((unsigned char)*(rec-3)) )||
        (rec < (pal+ strlen(pal) -2) && *(rec+1)=='-' && vocal((unsigned char)*(rec+2)) )  )
        return 1;
   	else
        return 0;
  }
  else
  	return 0;
}
/*****************************************************************************
 *  int recortar_e_acentuar_palabra(char * pal,char* recorte): recorta da pa-*
 * -labra que se lle pasa en pal (IN/OUT), o recorte que e pasado en recorte,*
 * parametros so de entrada (IN). Acentuando a palabra correctamente, e de-  *
 * -volve sempre 1, polo que a saida non aporta nada, podese cambiar.        *
 *****************************************************************************/
int gbm_verbos::recortar_e_acentuar_palabra(char * pal, const char * recorte, char idioma){
//   char recorte_aumentado[20]="";
   t_palabra_proc palabra_recortada="", pal_kk;
   char rabo[20]="";
   int acento_palabra_recortada;
   char *recorredor;
   char num_silabas_rabo=1;
   char num_silabas_palabra_recortada=1;
   char procesado=0;
   char* letra_final;
   char pal_intermedia[LONX_MAX_PAL_SIL_E_ACENTUADA]="";
/*
   if (strcmp(recorte,"o")==0  || strcmp(recorte,"a")==0 ||
       strcmp(recorte,"os")==0 || strcmp(recorte,"as")==0 ) {
//fmendez
//      if (strcmp(pal+strlen(pal)-3,"eun")==0 ||
//          strcmp(pal+strlen(pal)-3,"ein")==0 ) {
//fmendez
      if (strlen(pal)>3 &&( !strcmp(pal+strlen(pal)-3,"eun") ||
          !strcmp(pal+strlen(pal)-3,"ein") )) {
         strcpy(recorte_aumentado,"n");
         strcat(recorte_aumentado,recorte);
      }
      else {
         strcpy(recorte_aumentado,recorte);
      }
   }
*/
// Preprocesado no caso  de "colleino ou colleuno"
 separar_silabas(pal,pal_intermedia);
 recorredor=pal_intermedia;
  while (*recorredor) {              // BEGIN WHILE: Conta si'labas
    if (*recorredor=='-')
       num_silabas_palabra_recortada++;
    recorredor++;
 }                                  //END WHILE: Conta si'labas
 acento_palabra_recortada=acentuar_prosodicamente(pal_intermedia,palabra_recortada,idioma);
 separar_silabas(recorte,rabo);
 recorredor=rabo;
 while (*recorredor) {              // BEGIN WHILE: Conta si'labas
    if (*recorredor=='-')
       num_silabas_rabo++;
    recorredor++;
 }                                  //END WHILE: Conta si'labas
 if (palabra_recortada[strlen(palabra_recortada)-strlen(rabo)]=='^')
    palabra_recortada[strlen(palabra_recortada)-strlen(rabo)-1]=0;
 else
    palabra_recortada[strlen(palabra_recortada)-strlen(rabo)]=0;
// Recortamola palabra.
/*
 if ( (strcmp(rabo,"lo")==0)  || (strcmp(rabo,"la")==0) ||  (strcmp(rabo,"los")==0)
   || (strcmp(rabo,"las")==0) )
// Se se necesita reconstruir con r ou s
  recompor_verbo_sen_segunda_forma(palabra_recortada,pal);
*/
// Quitamos guions finais
 while(palabra_recortada[strlen(palabra_recortada)-1]=='-') palabra_recortada[strlen(palabra_recortada)-1]=0;
//recorredor=palabra_recortada;
 //fmendez ojo
// num_silabas_palabra_recortada=1;
// while (*recorredor){ if (*recorredor=='-') num_silabas_palabra_recortada++;recorredor++;}

   num_silabas_palabra_recortada-=num_silabas_rabo;
   if (num_silabas_palabra_recortada==0) num_silabas_palabra_recortada=1;
 // Contamolo numero de silabas de palabra recortada.

//fmendez ojo
//ojo
 recorredor=palabra_recortada;
 // Eliminamola separacion de silabas
 *pal=0;
 strcpy(pal_kk,palabra_recortada);
 while (*recorredor) {
  if (*recorredor!='-')
      strncat(pal,recorredor,1);
  recorredor++;
 }
 strcpy(palabra_recortada,pal);

//ojo



 recorredor=palabra_recortada;
 char *rec_kk=pal_kk;
 while (*recorredor && *recorredor!='^') recorredor++;
 while (*rec_kk && *rec_kk!='^') rec_kk++;
// Obtemos o valor actual de aguda,cha ou esdruxula, unha vez que lle quitamos
 acento_palabra_recortada=acento_palabra_recortada-num_silabas_rabo;
 //if ( ((recorredor-palabra_recortada)>=3)&& acento_para_desfacer_ditongo(palabra_recortada,recorredor)) {
 if ( ((recorredor-palabra_recortada)>=3)&& acento_para_desfacer_ditongo(pal_kk,rec_kk)) {
  procesado=1;
  acentua_graficamente(recorredor-1);
 }
 letra_final=&(palabra_recortada[strlen(palabra_recortada)-1]);
 while(*letra_final=='-' || *letra_final=='^') letra_final--;
 if ( procesado==0 && acento_palabra_recortada==1) {
  if (( *letra_final=='n' || *letra_final=='s' || vocal((unsigned char)*letra_final))
                        &&
              (idioma==CASTELLANO || ! ditongo_decrecente(recorredor-1))
                        &&
               num_silabas_palabra_recortada>1       )
  {
   acentua_graficamente(recorredor-1);
   procesado=1;
  }
 }
 if ( procesado==0 && acento_palabra_recortada==2) {
  if ( *letra_final!='n' &&  *letra_final!='s' && !vocal((unsigned char)*letra_final)) {
   acentua_graficamente(recorredor-1);
   procesado=1;
  }
 }
 if (procesado==0 && acento_palabra_recortada>2) {
  acentua_graficamente(recorredor-1);
  procesado=1;
 }
 recorredor=palabra_recortada;
 // Eliminamola separacion de silabas
 *pal=0;
 while (*recorredor) {
  if (*recorredor!='-' && *recorredor!='^')
      strncat(pal,recorredor,1);
  recorredor++;
 }
 return procesado;
}

/*****************************************************************************
 *  int obter_conxugacion_de_codigo (unsigned char codigo): A partir do codi-*
 * -go e facendo as operacions oportunas obten a conxugacion correspondente  *
 * a unha desinencia. Isto e debido a ordenacion e asignacion intelixente de *
 * codigos a cada desinencia verbal.                                         *
 *****************************************************************************/
int gbm_verbos::obter_conxugacion_de_codigo (unsigned char codigo)
{
//fmendez
/*
   unsigned char resto_1,grupo,grupo_1;

   grupo=(codigo-1)/18;
   resto_1=(codigo-1)-(grupo*18);
   grupo_1=resto_1/6;
   return grupo_1+1;
*/
//fmendez
   if ((codigo==180)||(codigo>=183 && codigo<=186)) return 1;
   if ((codigo==181)||(codigo>=187 && codigo<=190)) return 2;
   if ((codigo==182)||(codigo>=191 && codigo<=194)) return 3;

   return (((codigo-1)/6)%3)+1;
}

/*****************************************************************************
 *  int copia_en_estructura (char *infinitivo, char *raiz, int modelo, t_in- *
 * finitivo *inf): Enche os campos da estructura t_infinitivo, recibida como *
 * parametro, de feito os parametros recibidos son os distintos campos a en- *
 * -cher da estructura. O feito de facelo asi e debido a aparicion reiterada *
 * desta operacion.                                                          *
 * ------------------------ PARAMETROS DE ENTRADA -------------------------- *
 *  + *infinitivo: Conten infinitivo a copiar nese mesmo campo do struct.    *
 *  + *raiz      : Conten a raiz a copiar no campo co mesmo nome no struct.  *
 *  + modelo     : Conten o modelo a copiar no campo do mesmo nome do struct.*
 * ------------------------ PARAMETROS DE SAIDA ---------------------------- *
 *  + *inf       : Zona de memoria onde se deixa a estructura copiada.       *
 *  + RETURN     : Un enteiro, sempre 1, por se se quere controlar algo.     *
 *****************************************************************************/
int gbm_verbos::copia_en_estructura(char *infinitivo, char *raiz,int modelo, t_infinitivo *inf)
{
   strcpy(inf->infinitivo,infinitivo);
   strcpy(inf->raiz,raiz);
   inf->modelo=modelo;
   return 1;
}

/**
 * \brief  Devolvemos a posicion no arrai onde se encontra a raiz verbal. O lugar 
 * dependera tamen da conxugacion. Se se devolve negativo e que non hai esa raiz 
 * verbal, e polo tanto non existe ese verbo ainda que as desinencias coincidisen.
 *
 * \author fmendez 
 *
 * \param[in] lista_de_tempos_posibles Recibe a lista dos posibles tempos verbais.
 * \param[in] verbo_e_desinencia  E o posible verbo atopado no texto
 * \param[out] tempo_verdadeiro Aqui van aqueles tempos que poden ser validos dos 
 * que foron recibidos 
 * \param[out] info  Aqui vai informacion correspondente o verbo, tal como infinitivo, 
 * raiz e modelo. Sendo cada elemento do arrai correspondendo coa desinencia que ocupa 
 * a mesma posicion no arrai de desinencias.
 *
 * \return Devolve -1 se non se atopa ningunha posibilidade, e 1 en caso contrario
 */
int gbm_verbos::comprobar_raiz_verbal(t_posibles_desinencias *lista_de_tempos_posibles, 
			char *verbo_e_desinencia, unsigned char * tempo_verdadeiro, t_infinitivo* info) {
	
	int cont_tempos,cont_grupos;
	//char cont,index,*aux,flag=OFF;
	int cont,index;
	char *aux,flag=OFF;
	int pos_lista;
	char cantidade_de_tempos_verdadeiros=0;
	// char num_tempos=0; // POIDA QUE NON SE EMPREGUE E SE POIDA SACAR
	char num_tempos_posibles=0;
	t_palabra_proc pal_analizada;
	unsigned char *grupo_de_conxugacion;
	t_infinitivo *copy=info;              // Copia punteiro a lista info verbal

	raices_de_verbos=(t_raices_de_verbos *)gbrv->asignaPuntero();

	while((lista_de_tempos_posibles+num_tempos_posibles)->desinencia[0]) { // BEGIN WHILE 1
		/* Mentres queden desinencias, comprobamos no seguinte bucle os posibles tem-
			 -pos asociados a cada desinencia.                                          */
		cont_tempos=0;                    // Para os tempos do campo cod_desinencia
		cont_grupos=1;                    // Para os modelos do campo cod_desinencia
		strcpy((char *)pal_analizada,(char *)verbo_e_desinencia);
		// Quitamoslle a posible desinencia o verbo, obtendo asi unha posible raiz.
		pal_analizada[strlen(pal_analizada)-strlen((lista_de_tempos_posibles+num_tempos_posibles)->desinencia)]=0;
		
		while (copy->infinitivo[0]){  // Ponhome no primeiro nodo valeiro
			copy++;
		}
		
		if (*pal_analizada){     // Cumplese -> posible verbo regular
			pos_lista=comprobar_en_lista_de_verbos(pal_analizada,idioma);
		}
		else { // BEGIN ELSE: irregular ou ICON do M43
			pos_lista=comprobar_en_lista_de_verbos(verbo_e_desinencia,idioma);

			/* Esto houbo que cambialo por culpa de ide e vai pois hai verbos que teñen
				 tamen esas raices, ademais das desinencias nas que xa aparece              */
			if ((pos_lista>=0) && !strcmp(raices_de_verbos[pos_lista].raiz,raices_de_verbos[pos_lista].modelo[0].inf)){
				pos_lista=-1;               // Tratamos cun verbo do modelo M43
			}
			else {
				pos_lista=-2;               // Tratamos cun verbo irregular
			}
		} // END ELSE: irregular ou ICON do M43
		
		if (pos_lista>=0) {               // BEGIN IF: Verbo regular
			while((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_tempos]) {// BEGIN WHILE 2
				for (index=0;index<NUM_INFINITIVO;index++) {
					grupo_de_conxugacion=(unsigned char*)&(raices_de_verbos[pos_lista].modelo[index].grupos[0]);
					if ((raices_de_verbos[pos_lista].modelo[index].grupos[0])==0) {
						break;
					}
					for(cont=0;cont<NUM_MODELOS;cont++) { // BEGIN FOR 2
						if ((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_grupos]==*(grupo_de_conxugacion+cont)) {// BEGIN IF: Copia os que coincidan
							strcpy(copy->reconstruccion,verbo_e_desinencia);
							strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
							copia_en_estructura(raices_de_verbos[pos_lista].modelo[index].inf, raices_de_verbos[pos_lista].raiz,
									(int) (raices_de_verbos[pos_lista].modelo[index].grupos[cont]),copy);
							copy++;              // A seguinte zona onde gardar estructura
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=(lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_tempos];
							cantidade_de_tempos_verdadeiros++;
						}  // END IF: Copia os que coincidan
					} // END FOR 2
				} // END FOR 1
				// Pares son codigos de tempos, e impares, codigos de grupos de conxugacions
				cont_tempos=cont_tempos+2;
				cont_grupos=cont_grupos+2;
			}                              // END WHILE 2
			if (!strlen((char *)tempo_verdadeiro)) {
				pos_lista=-1;
			}
		} // END IF: Verbo regular
		
		if (pos_lista==-2) {              // BEGIN IF: Tratamos caso irregular
			while((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_tempos]) { // BEGIN WHILE 2
				index=cont=0;
				while (!strcmp(raices_de_verbos[index].raiz,"0") &&
						((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_grupos])!=(int) (raices_de_verbos[index].modelo[0].grupos[cont])){ // BEGIN WHILE 3
					while ((int) raices_de_verbos[index].modelo[0].grupos[cont]){
						if (((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_grupos])==(raices_de_verbos[index].modelo[0].grupos[cont])){
							break;                // Para sairmos do primeiro bucle
						}
						else {
							cont++;
						}
					}
					if (((lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_grupos])==(raices_de_verbos[index].modelo[0].grupos[cont])){
						break;                   // Para sairmos do segundo bucle
					}
					cont=0;
					index++;
				} // END WHILE 3
				if (!strcmp(raices_de_verbos[index].raiz,"0")) {
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=(lista_de_tempos_posibles+num_tempos_posibles)->cod_desinencia[cont_tempos];
					strcpy(copy->reconstruccion,verbo_e_desinencia);
					strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
					copia_en_estructura(raices_de_verbos[index].modelo[0].inf,raices_de_verbos[index].raiz,(int) (raices_de_verbos[index].modelo[0].grupos[cont]),copy);
					copy++;                     // A seguinte zona onde gardar estructura
					cantidade_de_tempos_verdadeiros++;
				}
				cont_tempos=cont_tempos+2;
				cont_grupos=cont_grupos+2;
			} // END WHILE 2
		} // END IF: Tratamos caso irregular
		else if (pos_lista==-1) {         // BEGIN ELSE IF: Modelo 43
			// Para aqueles verbos que tenhen o infinitivo conxugado distinto do futuro de subxuntivo (M43). 
			pos_lista=comprobar_en_lista_de_verbos(verbo_e_desinencia,idioma);

			// FRAN_CAMPILLO: AL hacer el for sin comprobar anteriormente el valor de pos_lista nos arriesgamos
			// a que tenga un  valor negativo, saliéndonos de la zona reservada para raices_de_verbos.
			// Lo más cómodo es comprobar antes del bucle su valor y no tenerlo en cuenta dentro.

			if (pos_lista >= 0) {

				for (index=0;(flag==OFF && raices_de_verbos[pos_lista].modelo[index].inf && index<3);index++) {
					if (!strcmp(raices_de_verbos[pos_lista].raiz,raices_de_verbos[pos_lista].modelo[index].inf)) { // if (!strcmp...
						flag=ON;
						strcpy(copy->reconstruccion,verbo_e_desinencia);
						strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
						copia_en_estructura(raices_de_verbos[pos_lista].modelo[index].inf,raices_de_verbos[pos_lista].raiz,43,copy);
						copy++;                 // A seguinte zona onde gardar estructura
						aux=(char *)&verbo_e_desinencia[strlen((char *)verbo_e_desinencia)-2];
						if (!strcmp(aux,"ar")) {
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=199;
							cantidade_de_tempos_verdadeiros++;
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=201;
						}
						else if (!strcmp(aux,"ir")) {
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=211;
							cantidade_de_tempos_verdadeiros++;
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=213;
						}
						else {
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=205;
							cantidade_de_tempos_verdadeiros++;
							*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=207;
						}
						cantidade_de_tempos_verdadeiros++;
						strcpy(copy->reconstruccion,verbo_e_desinencia);
						strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
						copia_en_estructura(raices_de_verbos[pos_lista].modelo[index].inf,raices_de_verbos[pos_lista].raiz,43,copy);
						copy++;                 // A seguinte zona onde gardar estructura
					}                           // END IF !strcmp
				}                              // END FOR
			}
			if (flag && strlen((char *)tempo_verdadeiro)) {
				return 1;
			}
		} // END ELSE IF: Modelo 43
		num_tempos_posibles++;
	} // END WHILE 1

	/* Pois puido non entrar no bucle WHILE 1, co que asi non detectara os infi-
		 -vos conxugados do modelo 43 dalguns verbos. Esto soluciona o problema de
		 momento. Hai que optimiza-lo codigo nesta parte.                           */

	pos_lista=comprobar_en_lista_de_verbos(verbo_e_desinencia,idioma);

	// FRAN_CAMPILLO: Aquí vuelve a dar por supuesto que pos_lista es >= 0.

	if (pos_lista >= 0) {
		for (index=0;(raices_de_verbos[pos_lista].modelo[index].inf && index<3);index++) {
			if (!strcmp(raices_de_verbos[pos_lista].raiz,
						raices_de_verbos[pos_lista].modelo[index].inf)) {
				strcpy(copy->reconstruccion,verbo_e_desinencia);
				strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
				copia_en_estructura(raices_de_verbos[pos_lista].modelo[index].inf,raices_de_verbos[pos_lista].raiz,43,copy);
				copy++;                      // A seguinte zona onde gardar estructura
				aux=(char *)&verbo_e_desinencia[strlen((char *)verbo_e_desinencia)-2];
				if (!strcmp(aux,"ar")) {
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=199;
					cantidade_de_tempos_verdadeiros++;
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=201;
				}
				else if (!strcmp(aux,"ir")) {
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=211;
					cantidade_de_tempos_verdadeiros++;
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=213;
				}
				else {
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=205;
					cantidade_de_tempos_verdadeiros++;
					*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=207;
				}
				cantidade_de_tempos_verdadeiros++;
				strcpy(copy->reconstruccion,verbo_e_desinencia);
				strcpy(copy->desinencia, (lista_de_tempos_posibles+num_tempos_posibles)->desinencia);
				copia_en_estructura(raices_de_verbos[pos_lista].modelo[index].inf,raices_de_verbos[pos_lista].raiz,43,copy);
				copy++;                      // A seguinte zona onde gardar estructura
			}                                // END IF strcmp
		}                                   // END FOR
	}

	*(tempo_verdadeiro+cantidade_de_tempos_verdadeiros)=0;
	
	if (*tempo_verdadeiro){
		return 1;
	}
	else {	
		return -1;
	}
}

/**
 * \brief  Devolve todalas posibles desinencias (que coincidan coa terminacion
 * desa palabra) e para cada unha devolve unha lista cos posibles tempos que 
 * lle poden corresponder a esa desinencia. Por exemplo se entra a palabra 
 * "había" devolveria: 
 *      "-ía"-> tempos asociados
 *      "-a" ->   "         "   
 
 * \author fmendez 
 *
 * \param[in] pal_analizada  punteiro onde recibe o posible verbo. 
 * \param[out] lista_desinencias struct onde se reciben as posibles desinencias
 * que pode levar ese verbo  
 *
 * \return 
 */
int gbm_verbos::detectar_desinencias_verbais( char *pal_analizada,t_posibles_desinencias *lista_desinencias){

	char num_desinencias_distintas=0;
	t_palabra desinencia_aux="";
	int orde_lista_de_desinencias;
	char hai_desinencias_mais_pequenas;
	t_palabra terminacion_inversa="";
	int inicio_das_posibles_desinencias,conta;

	int trozo_restrinxido_do_diccionario=gbd->numero();
	desinencias=(t_desinencias *) gbd->asignaPuntero();
#ifdef _TRAZA_ANALISIS_VERBAL
	fprintf(stderr, "TRAZA_VERBAL: ===buscando desinencias...");
#endif
	conta=sizeof(lista_desinencias->cod_desinencia)-1;
	hai_desinencias_mais_pequenas=1;
	while(hai_desinencias_mais_pequenas) {         // BEGIN WHILE
		
		orde_lista_de_desinencias=buscar_en_desinencias(pal_analizada,trozo_restrinxido_do_diccionario,idioma);

		if (orde_lista_de_desinencias < 0){
#ifdef _TRAZA_ANALISIS_VERBAL
			fprintf(stderr, " ...no encuentro desinencias\n");
#endif
			return orde_lista_de_desinencias;
		}
#ifdef OEM
		_fstrcpy(terminacion_inversa,desinencias[orde_lista_de_desinencias].desinencia);
#else
		strcpy(terminacion_inversa,desinencias[orde_lista_de_desinencias].desinencia);
#endif
		/* Facemos unha lista con todolos posibles tempos e numero que poden correspon-
			 -der a unha mesma desinencia.                                              */
		inicio_das_posibles_desinencias=orde_lista_de_desinencias;
		strcpy(desinencia_aux,terminacion_inversa);
		/* Copiamos no arrai de saida todalas posibilidades de desinencias. Devolvense
			 os codigos correspondentes. Para iso invertimos antes a desinencia.        */
		invertir_cadea2( desinencia_aux);
		strcpy((lista_desinencias+num_desinencias_distintas)->desinencia,desinencia_aux);
		strncpy((char*)(lista_desinencias+num_desinencias_distintas)->cod_desinencia,
				desinencias[orde_lista_de_desinencias].info,conta);
		(lista_desinencias+num_desinencias_distintas)->cod_desinencia[conta]=0;
		/* Miramos agora se quedan outras desinencias mais pequenas. Por exemplo: en-
			 -contrada a desinencia: "-abades", tamen e posible so a desinencia "ades",
			 que se encontra xusto antes na lista de desinencias, polo que tamen hai que
			 comprobar este caso.                                                       */
		if (inicio_das_posibles_desinencias>0) {     // BEGIN IF 1
			terminacion_inversa[strlen(desinencias[inicio_das_posibles_desinencias-1].desinencia)]=0;
			if (terminacion_inversa[0]==desinencias[inicio_das_posibles_desinencias-1].desinencia[0]){
				// Se pode haber desinencias mais pequenas ainda
				hai_desinencias_mais_pequenas=1;
				trozo_restrinxido_do_diccionario=inicio_das_posibles_desinencias;
			}
			else {
				hai_desinencias_mais_pequenas=0;
			}
		}                                            // END IF 1
		else {
			hai_desinencias_mais_pequenas=0;
		}
		num_desinencias_distintas++;
		(lista_desinencias+num_desinencias_distintas)->desinencia[0]=0;
	} // END WHILE
#ifdef _TRAZA_ANALISIS_VERBAL
	fprintf(stderr, " ... %d encontradas\n", num_desinencias_distintas);
	if (num_desinencias_distintas) {
		for(int kk = 0; kk < num_desinencias_distintas; kk++) {
			fprintf(stderr, "TRAZA_VERBAL: ===desinencia %s modelo:", lista_desinencias[kk].desinencia);
			for(int kk2 = 0; kk2 < strlen((char *)lista_desinencias[kk].cod_desinencia); kk2++){
				fprintf(stderr, " %d", lista_desinencias[kk].cod_desinencia[kk2]);
			}
			fprintf(stderr, "\n");
		}
	}
#endif

	return lista_desinencias->cod_desinencia[0];
}

/**
 * \brief Procura elimina-los posibles encliticos ou pronomes da palabra que 
 * se lle pasa como parámetro. 
 * \author fmendez 
 *
 * \param[in] palabra_analizada  Verbo o que se lle eliminan posibles pronomes
 * \param[out] enclitico_encontrado  
 * \param[in/out] orden  Devolve a posición do enclítico dentro da lista de 
 * enclíticos, inicialmente o seu valor e -1, para logo toma-lo valor que lle 
 * corresponda se atopa un enclítico
 * \param[in] eliminada_segunda_forma Que recibirá un 1 se foi eliminada e 0 
 * en caso contrario
 * \param[in/out] tratar_de_eliminar_enclitico Controla se hai que eliminar ou 
 * non o enclítico, controla pois o proceso de búsqueda e eliminación do mesmo,
 * inicialmente estará a 1  
 *
 * \return 
 */
int gbm_verbos::eliminar_encliticos(t_palabra_proc palabra_analizada, t_enclitico *enclitico_encontrado,	
		int *orden,int eliminada_segunda_forma, char *tratar_de_eliminar_enclitico) {

	t_palabra_proc palabra,pal_aux=""; // O silabificar con 20 -t_palabra- casca
	//int resultado;

	if (!tratar_de_eliminar_enclitico){ 
		return 0;
	}
	if (*orden==-1) {
		*orden=comprobar_en_lista_de_inicio_de_palabras2((char**)pron_encliticos,
				sizeof(t_dic_enclitico),NU_PRON_ENCLITICOS,palabra_analizada,EN_DICCIONARIO_INVERSO);
	}
	else {                             // BEGIN ELSE 1
		strcpy(palabra,palabra_analizada);
		invertir_cadea2(palabra);
		// fmendez repetia las formas con el enclitico 0 "a"
		if (*orden==0) {                 // Quedase so co pronome enclitico
			//palabra[strlen(pron_encliticos[*orden])]=0;
			//resultado=strcmp(palabra,pron_encliticos[*orden]);
			*tratar_de_eliminar_enclitico=0;
			//if (resultado!=0) return 0;
			return  0;
		}
		//fmendez
		while (*orden>=1) {              // BEGIN WHILE 1
			if(palabra[0]==pron_encliticos[*orden-1].enclitico[0]) {
				(*orden)--;
				*pal_aux=0;
				strcpy(pal_aux,palabra);
				pal_aux[strlen(pron_encliticos[*orden].enclitico)]=0;
				if (strcmp(pal_aux,pron_encliticos[*orden].enclitico)==0){ 
					break;
				}
				if (*orden == 0  &&  strcmp(pal_aux,pron_encliticos[*orden].enclitico)!=0) {
					*tratar_de_eliminar_enclitico=0;
					return 0;
				}
			}
			else {
				*tratar_de_eliminar_enclitico=0;
				return 0;
			}
		}                                // END WHILE 1
	}                                  // END ELSE 1
	if (*orden>=0) {                   // BEGIN IF orden
#ifdef OEM
		_fstrcpy(enclitico_encontrado->enclitico,pron_encliticos[*orden].enclitico);
#else
		strcpy(enclitico_encontrado->enclitico,pron_encliticos[*orden].enclitico);
		invertir_cadea2(enclitico_encontrado->enclitico);
#endif
		enclitico_encontrado->grupo=pron_encliticos[*orden].grupo;
		if ((strcmp(enclitico_encontrado->enclitico,"o")==0||strcmp(enclitico_encontrado->enclitico,"a")==0
					||strcmp(enclitico_encontrado->enclitico,"os")==0||strcmp(enclitico_encontrado->enclitico,"as")==0)
				&&
				(eliminada_segunda_forma)) {
			enclitico_encontrado->enclitico[0]=0;
			return 0;
		}
		/* No caso de que xa se eliminase a segunda forma estes encliticos non se poden
			 volver a eliminar xa que son incompatibles, gramaticalmente imposible, non
			 pode ser un enclitico.                                                     */
		palabra_analizada[strlen(palabra_analizada)-strlen(enclitico_encontrado->enclitico)]=0;
		/* Eliminamo-la terminacion.                                                  */
		return 1;
	}  // END IF orden
	return 0;
}

/******************************************************************************
 *  int eliminar_segunda_forma(unsigned char *pal_analizada): Funcion boolea- *
 * que elimina a segunda forma do artigo na pal_analizada que se lle pasa co- *
 * -mo parametro. Devolve 0 se non se eliminou a segunda forma do artigo e 1  *
 * se se fixo tal operacion.                                                  *
 ******************************************************************************/
int gbm_verbos::eliminar_segunda_forma(char *pal_analizada, char idioma)
{
   int eliminada_seg_forma=OFF;

// FRAN_CAMPILLO: Aquí llegan palabras que pueden tener menos de 3 ó 4 caracteres, lo
// que provoca un Pointer Arithmetic Underrun.

   if (strstr(pal_analizada, "-")) {
// FRAN_CAMPILLO

      if (strcmp(pal_analizada+strlen(pal_analizada)-3,"-lo")==0) {
         strcpy(pal_analizada+strlen(pal_analizada)-3,"lo");
         recortar_e_acentuar_palabra(pal_analizada,"lo",idioma);
         eliminada_seg_forma=ON;
      }
      else if (strcmp(pal_analizada+strlen(pal_analizada)-3,"-la")==0) {
           strcpy(pal_analizada+strlen(pal_analizada)-3,"la");
           recortar_e_acentuar_palabra(pal_analizada,"la",idioma);
           eliminada_seg_forma=ON;
      }
      else if (strcmp(pal_analizada+strlen(pal_analizada)-4,"-los")==0) {
           strcpy(pal_analizada+strlen(pal_analizada)-4,"los");
           recortar_e_acentuar_palabra(pal_analizada,"los",idioma);
           eliminada_seg_forma=ON;
      }
      else if (strcmp(pal_analizada+strlen(pal_analizada)-4,"-las")==0) {
           strcpy(pal_analizada+strlen(pal_analizada)-4,"las");
           recortar_e_acentuar_palabra(pal_analizada,"las",idioma);
           eliminada_seg_forma=ON;
      }
   }
   return eliminada_seg_forma;
}

/*****************************************************************************
 *  void decidir_entre_varias_posibilidades_de_verbos(                       *
 * t_posibles_desinencias *pos_desinencias, char *enclitico):  Dada informa- *
 * -cion como as posibilidades de distintos tempos verbais (distintas perso- *
 * -as, ou numero), e aproveitando informacion como a de se habia enclitico, *
 * seleccionase a analise correcta.                                          *
 *                                                                           *
 *  + forma_verbal: verbo no que se van a comproba-las distintas formas ter- *
 *     -minacions verbais.                                                   *
 *  + pos_desinencias: array de structs que conten as distintas desinencias, *
 *     asi como os codigos que lle corresponden.                             *
 *  + enclitico: recibe aqui o enclitico detectado no verbo, serve para eli- *
 *     -minar algunhas das posibilidades.                                    *
 *****************************************************************************/
void gbm_verbos::decidir_entre_varias_posibilidades_de_verbos(t_posibles_desinencias *pos_desinencias,
		char *enclitico){
	
	int cont;
	unsigned char tempo,persoa,codigo;

	if (strcmp(enclitico,"se")==0) {                         // BEGIN IF 1
		/* Tratar casos como o de sentía-se, no que sentía non pode ser primeira persoa
		 * mais se. O mesmo sucede con "tira-se" no que non e posible a combinacion de
		 * enclitico "-se" mais imperativo "tira".
		 * Se o verbo non e terceira persoa(singular/plural), ou primeira de singular,
		 * pois tamen aparece o enclitico "se" nese caso no imperfecto de subxuntivo,
		 * enton eliminamos ese tempo, pero colocamos a partir dese lugar o resto de
		 * tempos que ainda quedan por examinar.
		 */
		while (pos_desinencias->desinencia[0]) {              // BEGIN WHILE 1
			cont=0;
			while(pos_desinencias->cod_desinencia[cont]) {     // BEGIN WHILE 2
				codigo=pos_desinencias->cod_desinencia[cont];
				tempo=(unsigned char)((codigo-1)/6);
				persoa=(unsigned char)((codigo-1)-tempo*6);
				if ( !(persoa==0 || persoa==2 || persoa==5) ||
						(persoa==0 && (tempo==3||tempo==4||tempo==5))) {// BEGIN IF 2
					_copia_cadena((char *)&(pos_desinencias->cod_desinencia[cont]),
							(char *) &(pos_desinencias->cod_desinencia[cont+2]));
					cont-=2;
					/* Hai que retrasa-lo contador para que non se salte ningun dos que quedan por
						 examinar.                                                                  */
				}                       // END IF 2
				cont+=2;                // Avanzo 2 posicions para sgte. tempo
			}                          // END WHILE 2
			pos_desinencias++;
		}                             // END WHILE 1
	}                                // END IF 1
	else if (strcmp(enclitico,"mos")==0) {                   // BEGIN ELSE IF
		/*
		 *Se o verbo e primeira ou terceira do infinitivo conxugado de calquera das
		 *tres conxugacions enton non e posible a union co enclitico -mos, senon que
		 *se trata da primeira persoa do plural.
		 *
		 *                 raiz             desinencia                enclitico
		 *---------------------------------------------------------------------
		 *correcto          and-            -armos                      -----
		 *incorrecto        and-            -ar-                        mos      
		 */
		while (pos_desinencias->desinencia[0]) {              // BEGIN WHILE 1
			cont=0;
			while(pos_desinencias->cod_desinencia[cont]) {     // BEGIN WHILE 2
				codigo=pos_desinencias->cod_desinencia[cont];
				tempo=(unsigned char)((codigo-1)/6);
				persoa=(unsigned char)((codigo-1)-tempo*6);
				if ((persoa==0||persoa==2) && (tempo==24||tempo==25||tempo==26||
							tempo==33||tempo==34||tempo==35)){ // BEGIN IF 2
					_copia_cadena((char *)&(pos_desinencias->cod_desinencia[cont]),
							(char *)&(pos_desinencias->cod_desinencia[cont+2]));
					cont-=2;
				} // END IF 2
				cont+=2;
			}   // END WHILE 2
			pos_desinencias++;
		} // END WHILE 1
	}   // END ELSE IF
	else if (!strcmp(enclitico,"a")||!strcmp(enclitico,"as")|| !strcmp(enclitico,"o")) {// BEGIN ELSE IF
		/*
		 *Pois no caso no do pluscuamperfecto, primeira, segunda e terceira persoas
		 *de singular: cantara-cantaras-cantara, sendo posibles as descomposicions:
		 *                  raiz             desinencia                enclitico
		 *---------------------------------------------------------------------
		 *correcto          and-            -ara / -aras                -----
		 *incorrecto        and-            -ar-                       -a / -as 
		 *
		 */
		while (pos_desinencias->desinencia[0]) {              // BEGIN WHILE 1
			cont=0;
			while(pos_desinencias->cod_desinencia[cont]) {     // BEGIN WHILE 2
				codigo=pos_desinencias->cod_desinencia[cont];
				tempo=(unsigned char)((codigo-1)/6);
				persoa=(unsigned char)((codigo-1)-tempo*6);
				if ((persoa==0||persoa==2) && (tempo==24||tempo==25||tempo==26||
							tempo==33||tempo==34||tempo==35)) {
					_copia_cadena((char *)&(pos_desinencias->cod_desinencia[cont]),
							(char *)&(pos_desinencias->cod_desinencia[cont+2]));
					cont-=2;
				}
				cont+=2;
			}                                                  // END WHILE 2
			pos_desinencias++;
		}                                                     // END WHILE 1
	}                                                        // END ELSE IF
}


/******************************************************************************
 *  int analizar_verbos(unsigned char *palabra, unsigned char * palabra_acen_ *
 * e_sil,unsigned char* tempos_verdadeiros): Analiza todolos posibles tempos  *
 * para o suposto verbo.                                                      *
 *                                                                            *
 *  + palabra : Punteiro que conten o posible verbo (IN).                     *
 *  + palabra_acen_e_sil: Conten a palabra silabeada e acentuada. Non se em-  *
 *    -ga en todo a funcion e modulo.                                         *
 *  + tempos_verdadeiros: Aqui devolvense os posibles tempos verbais para o   *
 *    posible verbo que se lle pasa, e pois o parametro de saida (OUT).       *
 *                                                                            *
 ******************************************************************************/
int gbm_verbos::analizar_verbos(char * palabra,unsigned char* tempos_verdadeiros,t_infinitivo* inf, char pidioma){

	t_palabra_proc pal_analizada="";
	int tempo_num_persoa;
	t_enclitico enclitico;
	t_palabra_proc artigo;
	int pos_lista_enclitico=-1;
	t_palabra_proc posible_verbo="";
	t_palabra_proc posibles[4],posibles_sin_art[4];
	int ind_posibles,ind_posibles_sin_art,kk,kk2,kk3,diac,lenPosibleVerbo;
	int segunda_forma_do_artigo_eliminada=0;
	int enclitico_eliminado;
	char tratar_de_eliminar_enclitico=1, tieneTilde=0,esA=0,esE=0;
	t_posibles_desinencias lista_posibles_tempos_verbais[TEMPOS_POSIBLES];
	unsigned char tempos_parciais[TEMPOS_POSIBLES]="";

	char * pos_tilde;

	idioma=pidioma;

	//if (raices_de_verbos==NULL)
	// return *tempos_verdadeiros;

	//ZA: ahora el castellano ha de reconocer sus propios enclíticos.
	// if (opciones.idioma==CASTELLANO)
	//    tratar_de_eliminar_enclitico=1;

	enclitico.grupo=enclitico.enclitico[0]=0;
	artigo[0]=0;
	memset(posibles[0],0,sizeof(posibles));



	lista_posibles_tempos_verbais[0].desinencia[0]=0;
	/* Reseteamo-lo primeiro elemento                                             */

	strcpy(pal_analizada,palabra);

#ifdef _TRAZA_ANALISIS_VERBAL
	fprintf(stderr,"====================================\nTRAZA_VERBAL: Palabra a analizar: %s\n", palabra);
#endif
	/*
	//fmendez para monosilabos
	{
	char mono=1,kk[200];
	char *rec;
	FILE *fmono;

	separar_silabas(palabra,kk);
	fmono=fopen("f_monosilabos.txt","a+t");

	rec=kk;
	while (*rec) {
	if (*rec=='-') {
	mono=0;
	break;
	}
	rec++;
	}
	if (mono) fprintf(fmono,"%s\n",palabra);
	fclose(fmono);
	}
	//fmendez para monosilabos
	*/
	//fmendez 2a forma

	//ZA: el análisis 2ª forma confunde para el castellano, por eso se mete en una
	//condición de idioma.
	//¿La función  "eliminar_segunda_forma" sirve para algo? :P


	if (strstr(pal_analizada, "-")) {
		if (strcmp(pal_analizada+strlen(pal_analizada)-3,"-lo")==0) {
			*(pal_analizada+strlen(pal_analizada)-3)=0;
			strcpy(artigo,"o");
			segunda_forma_do_artigo_eliminada=ON;
		}
		else if (strcmp(pal_analizada+strlen(pal_analizada)-3,"-la")==0) {
			*(pal_analizada+strlen(pal_analizada)-3)=0;
			strcpy(artigo,"a");
			segunda_forma_do_artigo_eliminada=ON;
		}
		else if (strcmp(pal_analizada+strlen(pal_analizada)-4,"-los")==0) {
			*(pal_analizada+strlen(pal_analizada)-4)=0;
			strcpy(artigo,"os");
			segunda_forma_do_artigo_eliminada=ON;
		}
		else if (strcmp(pal_analizada+strlen(pal_analizada)-4,"-las")==0) {
			*(pal_analizada+strlen(pal_analizada)-4)=0;
			strcpy(artigo,"as");
			segunda_forma_do_artigo_eliminada=ON;
		}
	}



	if (segunda_forma_do_artigo_eliminada) {
#ifdef _TRAZA_ANALISIS_VERBAL
		fprintf(stderr, "TRAZA_VERBAL: segunda_forma_do_artigo_eliminada: %s\n", artigo);
#endif
		strcpy(posibles_sin_art[0],pal_analizada);
		strcat(posibles_sin_art[0],"r");
		strcpy(posibles_sin_art[1],pal_analizada);
		strcat(posibles_sin_art[1],"s");
		ind_posibles_sin_art=2;

		strcat(pal_analizada,"l");
		strcat(pal_analizada,artigo);
		diac=diacritico(pal_analizada,idioma);

		if (diac==0){
			for (kk=0;kk<ind_posibles_sin_art;kk++){
				strcat(posibles_sin_art[kk],artigo);
				recortar_e_acentuar_palabra(posibles_sin_art[kk],artigo,idioma);
			}
		}
		else if (diac==2) {     //esdrújula
			if (strcmp(posibles_sin_art[0],"dár") &&
					strcmp(posibles_sin_art[0],"pór") &&
					strcmp(posibles_sin_art[0],"vér")) {
				for (kk=0;kk<ind_posibles_sin_art;kk++){
					strcat(posibles_sin_art[kk],artigo);
					recortar_e_acentuar_palabra(posibles_sin_art[kk],artigo,idioma);
				}
			}
			else if  (!strcmp(posibles_sin_art[0],"vér")) {
				kk2=ind_posibles_sin_art;
				for (kk=0;kk<kk2;kk++){
					strcpy(posibles_sin_art[ind_posibles_sin_art],posibles_sin_art[kk]);
					pos_tilde=strpbrk(posibles_sin_art[ind_posibles_sin_art],"áéíóú");
					switch(*pos_tilde) {
						case 'á' :
							*pos_tilde='a';
							break;
						case 'é' :
							*pos_tilde='e';
							break;
						case 'í' :
							*pos_tilde='i';
							break;
						case 'ó' :
							*pos_tilde='o';
							break;
						case 'ú' :
							*pos_tilde='u';
							break;
					}
					ind_posibles_sin_art++;
				}
			}
		}
	}
	else {
		strcpy(posibles_sin_art[0],pal_analizada);
		ind_posibles_sin_art=1;
		artigo[0]=0;
	}

	//  segunda_forma_do_artigo_eliminada=eliminar_segunda_forma((unsigned char*) pal_analizada);

#ifdef _TRAZA_ANALISIS_VERBAL
	fprintf(stderr, "TRAZA_VERBAL: número de reconstrucciones sin artículo posibles %d\n", ind_posibles_sin_art);
#endif

	//fmendez 2a forma
	for(kk3=0;kk3<ind_posibles_sin_art;kk3++){
		strcpy(pal_analizada,posibles_sin_art[kk3]);
		/* Se hai unha segunda forma do artigo o primeiro que hai que facer e elimi-
			 -nala.                                                                     */

#ifdef _TRAZA_ANALISIS_VERBAL
		fprintf(stderr, "TRAZA_VERBAL: posibilidad sin artículo %d: %s\n", kk3+1,pal_analizada);
#endif

		do {                               // BEGIN DO-WHILE
			/* Examinanse todolos posibles encliticos, e para cada enclitico examinanse
				 todolos posibles verbos que pode haber detras.                             */
			strcpy(posible_verbo,pal_analizada);

			enclitico_eliminado=0;
			//fmendez
			enclitico.grupo=enclitico.enclitico[0]=0;
			//fmendez
			if (tratar_de_eliminar_enclitico)
				enclitico_eliminado=eliminar_encliticos(posible_verbo,&enclitico,&pos_lista_enclitico,
						segunda_forma_do_artigo_eliminada,&tratar_de_eliminar_enclitico);
			/* Se non se detectou ningun posible mais enclitico e xa se fixo algunha anali-
				 -se anterior enton xa non quedan mais posibilidades de ser verbo.          */
			if (!posible_verbo[0]) continue;
			ind_posibles=0;
			if (enclitico_eliminado) {       // posible_verbo ten a palabra a recortar
#ifdef _TRAZA_ANALISIS_VERBAL
				fprintf(stderr, "TRAZA_VERBAL: elimino enclitico %s: queda %s\n", enclitico.enclitico, posible_verbo);
#endif
				lenPosibleVerbo=strlen(posible_verbo);
				//ZA
				if (lenPosibleVerbo>2) {  //pfff para evitar que nos pasen verbos de una sola letra.
					if (enclitico.grupo>1) {
						if (!strcmp(&posible_verbo[lenPosibleVerbo-2],"mo")){ // 4ª persona, en -mos
							strcpy(posibles[ind_posibles],posible_verbo);
							strcat(posibles[ind_posibles++],"s");
						}
						if (enclitico.grupo==3) {
							if (!strcmp(&posible_verbo[lenPosibleVerbo-2],"ai") ||
									!strcmp(&posible_verbo[lenPosibleVerbo-2],"ei") ||
									!strcmp(&posible_verbo[lenPosibleVerbo-2],"ái") ||
									!strcmp(&posible_verbo[lenPosibleVerbo-2],"éi") ||
									!strcmp(&posible_verbo[lenPosibleVerbo-1],"í") ){ //terminaría en áis,ais,eis,éis,ís
								strcpy(posibles[ind_posibles],posible_verbo);
								strcat(posibles[ind_posibles++],"s");
							}
						}
						else if (enclitico.grupo==4) {
							if ((esA = (char) strcmp(&posible_verbo[lenPosibleVerbo-1],"a")) == 0 ||
									(esE = (char) strcmp(&posible_verbo[lenPosibleVerbo-1],"e")) == 0 ||
									(tieneTilde= (char) strcmp(&posible_verbo[lenPosibleVerbo-1],"í")) == 0){ //terminaría en -a/e/id
								strcpy(posibles[ind_posibles],posible_verbo);
								strcat(posibles[ind_posibles++],"d");
								if (!tieneTilde && esA && esE) {
									posibles[ind_posibles-1][lenPosibleVerbo-1]='i';
								}
							}
						}
						//Única excepción, idos, pero esa ya la va a aceptar por defecto.
					}
				}
				if ((!strcmp(enclitico.enclitico,"o") || !strcmp(enclitico.enclitico,"a") ||
							!strcmp(enclitico.enclitico,"os") || !strcmp(enclitico.enclitico,"as") ) &&
						(!strcmp(&posible_verbo[strlen(posible_verbo)-1],"s") ||
						 (!strcmp(&posible_verbo[strlen(posible_verbo)-1],"d") &&  strcmp(posible_verbo,"id"))||
						 !strcmp(&posible_verbo[strlen(posible_verbo)-1],"r"))){
					enclitico_eliminado=1;
					enclitico.enclitico[0]=0;
					ind_posibles=-1;
					strcpy(posible_verbo,pal_analizada);
				}
				else if (excep_verbo(palabra)) {
					enclitico_eliminado=1;
					enclitico.enclitico[0]=0;
					ind_posibles=-1;
					strcpy(posible_verbo,pal_analizada);
				}
				else {  //ZA El análisis de estos pronombres no sirve para el castellano

					if (strcmp(enclitico.enclitico,"lo")==0  || strcmp(enclitico.enclitico,"la")==0 ||
							strcmp(enclitico.enclitico,"los")==0 || strcmp(enclitico.enclitico,"las")==0  ){
						if (idioma==GALEGO) {
							//recompor_verbo_sen_segunda_forma(posible_verbo,pal_analizada);
							strcpy(posibles[ind_posibles],posible_verbo);
							strcat(posibles[ind_posibles++],"r");
							strcpy(posibles[ind_posibles],posible_verbo);
							strcat(posibles[ind_posibles++],"s");
						}
						else {
							strcpy(posibles[ind_posibles++],posible_verbo);
							//      strcat(posibles[ind_posibles++],"r");
							//      strcpy(posibles[ind_posibles],posible_verbo);
							//      strcat(posibles[ind_posibles++],"s");


						}
					}
					else if (strcmp(enclitico.enclitico,"o")==0  || strcmp(enclitico.enclitico,"a")==0 ||
							strcmp(enclitico.enclitico,"os")==0 || strcmp(enclitico.enclitico,"as")==0 )  {
						//En palabras como colleu+o =>colleuno, hai que eliminar o -n tamen.
						if (strlen(posible_verbo)>3 &&
								(!strcmp(posible_verbo+strlen(posible_verbo)-3,"ain") ||
								 !strcmp(posible_verbo+strlen(posible_verbo)-3,"ein") ||
								 !strcmp(posible_verbo+strlen(posible_verbo)-3,"oin") ||
								 !strcmp(posible_verbo+strlen(posible_verbo)-3,"aun") ||
								 !strcmp(posible_verbo+strlen(posible_verbo)-3,"eun") ||
								 !strcmp(posible_verbo+strlen(posible_verbo)-3,"oun") )){

							posible_verbo[strlen(posible_verbo)-1]=0;
							strcpy(posibles[ind_posibles++],posible_verbo);
						}
						else {
							strcpy(posibles[ind_posibles++],posible_verbo);
						}
					}
					else {//ZA Otra vez, cosa que no se aplica al castellano...
						if (strcmp(enclitico.enclitico,"nos")==0) {
							strcpy(posibles[ind_posibles++],posible_verbo);
							strcpy(posibles[ind_posibles],posible_verbo);
							strcat(posibles[ind_posibles++],"s");
						}
						else { 
							strcpy(posibles[ind_posibles++],posible_verbo);
						}
					}
					diac=diacritico(pal_analizada,idioma);
					if (diac==0){
						for (kk=0;kk<ind_posibles;kk++){
							strcat(posibles[kk],enclitico.enclitico);
							recortar_e_acentuar_palabra(posibles[kk],enclitico.enclitico,idioma);
						}
					}
					else if (diac==2) {     //esdrújula
						if (strcmp(posibles[0],"dá") &&
								strcmp(posibles[0],"dás") &&
								strcmp(posibles[0],"pór") &&
								strcmp(posibles[0],"sé") &&
								strcmp(posibles[0],"vén") &&
								strcmp(posibles[0],"vés") &&
								strcmp(posibles[0],"cómpre") &&
								strcmp(posibles[0],"cómpren") &&
								strcmp(posibles[0],"é")) {
							for (kk=0;kk<ind_posibles;kk++){
								strcat(posibles[kk],enclitico.enclitico);
								recortar_e_acentuar_palabra(posibles[kk],enclitico.enclitico,idioma);
							}
						}
						else if  (!strcmp(posibles[0],"vér")||!strcmp(posibles[0],"vén")||
								!strcmp(posibles[0],"cómpre")||!strcmp(posibles[0],"cómpren")) {
							kk2=ind_posibles;
							for (kk=0;kk<kk2;kk++){
								strcpy(posibles[ind_posibles],posibles[kk]);
								pos_tilde=strpbrk(posibles[ind_posibles],"áéíóú");
								switch(*pos_tilde) {
									case 'á' :
										*pos_tilde='a';
										break;
									case 'é' :
										*pos_tilde='e';
										break;
									case 'í' :
										*pos_tilde='i';
										break;
									case 'ó' :
										*pos_tilde='o';
										break;
									case 'ú' :
										*pos_tilde='u';
										break;
								}
								ind_posibles++;
							}
						}
					} //else if (diac==2)
				}
			} //if (enclitico_eliminado)
			if (ind_posibles==-1) {
				continue;
			}
#ifdef _TRAZA_ANALISIS_VERBAL
			fprintf(stderr, "TRAZA_VERBAL: número de reconstrucciones sin enclítico posibles %d\n", ind_posibles);
#endif
			if (ind_posibles==0) {
				ind_posibles++;
				strcpy(posibles[0],posible_verbo);
			}
			for (kk=0;kk<ind_posibles;kk++){
				strcpy(posible_verbo,posibles[kk]);
#ifdef _TRAZA_ANALISIS_VERBAL
				fprintf(stderr, "TRAZA_VERBAL: reconstruccion sin enclítico %d:%s ....\n", kk+1, posible_verbo);
#endif
				/* Resetear esta variable en cada iteracion.                                  */
				lista_posibles_tempos_verbais[0].desinencia[0]=0;
				lista_posibles_tempos_verbais[0].cod_desinencia[0]=0;
				tempo_num_persoa=detectar_desinencias_verbais(posible_verbo,lista_posibles_tempos_verbais);
				/* A seguinte funcion serve para eliminar xa certas posibilidades a analizar. Por
					 exemplo, se se detectou como enclitico "-se", enton o verbo so pode ser de ter-
					 -ceira persoa, polo que xa non se analizan as outras posibilidades.        */
				decidir_entre_varias_posibilidades_de_verbos(lista_posibles_tempos_verbais,enclitico.enclitico);
				if (tempo_num_persoa) {
					//#ifdef _TRAZA_ANALISIS_VERBAL
					//			fprintf(stderr, "tempo_num_persoa  %d\n", tempo_num_persoa);
					//#endif					
					/* Se non se encontrou coincidencia da terminacion do verbo cunha desinencia,
						 enton xa non pode ser verbo. En tal caso non entraria aqui.                */
					tempos_parciais[0]=0;
#ifdef _TRAZA_ANALISIS_VERBAL
					fprintf(stderr, "TRAZA_VERBAL: Comprobando raíces...\n");
#endif
					comprobar_raiz_verbal(lista_posibles_tempos_verbais,posible_verbo,tempos_parciais,inf);
					/* Para que vaia acumulando os tempos neste arrai en distintas iteracions.    */


					//fmendez caso de nos
					if (strcmp(enclitico.enclitico,"nos")==0) {
						if (kk==0 && *tempos_parciais%4==0) {
							continue;
						}
					}
					//fmendez caso de nos

					strcat((char *)tempos_verdadeiros,(char *)tempos_parciais);
					//fmendez se copia el enclitico
					if (tempos_parciais[0]) {
#ifdef _TRAZA_ANALISIS_VERBAL
						fprintf(stderr, "TRAZA_VERBAL:...es verbo: %d posibles tiempos\n", strlen((char *)tempos_parciais));
#endif							
#ifdef _TRAZA_ANALISIS_VERBAL
						for(int kk=0;tempos_parciais[kk];kk++) {
							fprintf(stderr, "TRAZA_VERBAL: =%d=infinitivo:%s\traiz: %s\tmodelo %d\n",kk+1, inf[kk].infinitivo,inf[kk].raiz, inf[kk].modelo);
						}
#endif
						//strcpy(ptr->reconstruccion,posible_verbo);
						mete_enclitico(enclitico,inf,strlen((char *)tempos_parciais),idioma);
						if (artigo[0]) {
							mete_artigo(artigo,inf,strlen((char *)tempos_parciais));
						}
						//fmendez: se descartan los tiempos que no corresponden a la conjugación del verbo
						comprueba_validez_tiempo_conj(tempos_verdadeiros,inf);
						//ZA: en castellano, la idea es que la matriz ya hace lo que tiene que hacer.          
						if (idioma==GALEGO)
							//comprueba_validez_tiempo_encl(posible_verbo,tempos_verdadeiros,inf);
							comprueba_validez_tiempo_encl(tempos_verdadeiros,inf);
						//fmendez
					} //if (tempos_parciais[0])
#ifdef _TRAZA_ANALISIS_VERBAL
					else {
						fprintf(stderr, "TRAZA_VERBAL:...no es verbo\n");
					}
#endif							
					//#ifdef _TRAZA_ANALISIS_VERBAL
					//				else {
					//			fprintf(stderr, "no: no existe ese tiempo verbal\n");
					//			}
					//#endif					
				}
#ifdef _TRAZA_ANALISIS_VERBAL
				else {
					fprintf(stderr, "no: no se encontró desinencia\n");
				}
#endif					
			}//for (kk=0;kk<ind_posibles;kk++)
		} while (enclitico_eliminado==1);  // END DO-WHILE
	}//for(kk3=0;kk3<ind_posibles_sin_art;kk3++)

	/* Condicion de que ainda haia que facer todolos pasos porque a terminacion
		 pode ser varios encliticos contidos uns noutros. Exemplo en comelo a priori
		 non podemos saber se o 'o' final e desinencia ou enclitico ou o enclitico
		 e "lo".                                                                    */
	return *tempos_verdadeiros;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param verb  
 *
 * \return 
 */
int gbm_verbos::conjugacion(char *verb){
   char final[3];
   if (verb[strlen(verb)-1]==')') strncpy(final,&verb[strlen(verb)-5],2);
   else strncpy(final,&verb[strlen(verb)-2],2);
   final[2]=0;
   if (!strcmp(final,"ar")) return 1;
   else if (!strcmp(final,"ir")) return 3;
   else if (!strcmp(final,"ír")) return 3;
   else return 2;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param tempos_parciais  
 * \param inf  
 *
 * \return 
 */
int gbm_verbos::comprueba_validez_tiempo_conj(unsigned char *tempos_parciais, t_infinitivo *inf){
	int i;
	int n_temps=strlen((char *)tempos_parciais);
	for(i=0;tempos_parciais[i];i++) {
		if (obter_conxugacion_de_codigo(tempos_parciais[i])!=
				conjugacion(inf[i].infinitivo)){
			_copia_cadena((char *)&tempos_parciais[i],(char *)&tempos_parciais[i+1]);
#ifdef _TRAZA_ANALISIS_VERBAL
			fprintf(stderr, "TRAZA_VERBAL: comprueba_validez_tiempo_conj: tiempo %d inválido: ", tempos_parciais[i]);
			escribe_tempo_verbal(tempos_parciais[i], stderr);
			fprintf(stderr, " %s\n", inf[i].infinitivo);
#endif	         
			memmove(&inf[i],&inf[i+1],sizeof(t_infinitivo)*(n_temps-i));
			i--;
		}
#ifdef _TRAZA_ANALISIS_VERBAL
		else{
			fprintf(stderr, "TRAZA_VERBAL: comprueba_validez_tiempo_conj: tiempo %d válido: ", tempos_parciais[i]);
			escribe_tempo_verbal(tempos_parciais[i], stderr);
			fprintf(stderr, " %s\n", inf[i].infinitivo);
		}
#endif	         
	}
	return 0;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param tempos_parciais  
 * \param inf  
 *
 * \return 
 */
int gbm_verbos::comprueba_validez_tiempo_encl(unsigned char *tempos_parciais, t_infinitivo *inf){
	int i;
	int n_temps=strlen((char *)tempos_parciais);

	for(i=0;tempos_parciais[i];i++) {
		if (inf[i].enclitico.enclitico[0]==0) 
			continue;
		if (matriz_cliticos_tiempos[inf[i].enclitico.grupo][tempos_parciais[i]]==0){
			_copia_cadena((char *)&tempos_parciais[i],(char *)&tempos_parciais[i+1]);
#ifdef _TRAZA_ANALISIS_VERBAL
			fprintf(stderr, "TRAZA_VERBAL: comprueba_validez_tiempo_encl: tiempo %d inválido", tempos_parciais[i]);
			escribe_tempo_verbal(tempos_parciais[i], stderr);
			fprintf(stderr, " %s\n", inf[i].infinitivo);
#endif	         
			memmove(&inf[i],&inf[i+1],sizeof(t_infinitivo)*(n_temps-i));
			i--;
		}
		/*
			 else if (inf[i].enclitico.grupo==19 //nos
			 && strlen(verbo)>3 && strcmp(&verbo[strlen(verbo)-3],"mos")) {
			 strcpy((char *)&tempos_parciais[i],(char *)&tempos_parciais[i+1]);
			 memcpy(&inf[i],&inf[i+1],sizeof(t_infinitivo)*(n_temps-i));
			 i--;
			 }
			 */
		else if (((tempos_parciais[i]>=183 && tempos_parciais[i]<=194)||  //participios
					(tempos_parciais[i]>=145 && tempos_parciais[i]<=162))   //fut subj
				&& inf[i].enclitico.enclitico[0]) {
#ifdef _TRAZA_ANALISIS_VERBAL
			fprintf(stderr, "TRAZA_VERBAL: comprueba_validez_tiempo_encl: tiempo %d inválido: ", tempos_parciais[i]);
			escribe_tempo_verbal(tempos_parciais[i], stderr);
			fprintf(stderr, " %s\n", inf[i].infinitivo);
#endif	         
			_copia_cadena((char *)&tempos_parciais[i],(char *)&tempos_parciais[i+1]);
			memmove(&inf[i],&inf[i+1],sizeof(t_infinitivo)*(n_temps-i));
			i--;
		}
#ifdef _TRAZA_ANALISIS_VERBAL
		else {
			fprintf(stderr, "TRAZA_VERBAL: comprueba_validez_tiempo_encl: tiempo %d válido: ", tempos_parciais[i]);
			escribe_tempo_verbal(tempos_parciais[i], stderr);
			fprintf(stderr, " %s\n", inf[i].infinitivo);
		}
#endif	         
	}
	return 0;
}


/**
 * \brief 
 * \author fmendez 
 *
 * \param enclitico  
 * \param inf  
 * \param n  
 * \param idioma  
 *
 * \return 
 */
int gbm_verbos::mete_enclitico(t_enclitico enclitico,t_infinitivo *inf,int n, char idioma) {
	t_infinitivo *ptr=inf;

	while(ptr->infinitivo[0]){ 
		ptr++;
	}

	ptr--;

	//ZA lo de quitar las l's no es para el castellano.

	if (idioma==GALEGO) {
		if (strlen(enclitico.enclitico)==2 && (!strcmp(enclitico.enclitico,"lo") || !strcmp(enclitico.enclitico,"la"))){
			_copia_cadena(enclitico.enclitico,(enclitico.enclitico)+1);
		}
		else if (strlen(enclitico.enclitico)==3 &&
				(!strcmp(enclitico.enclitico,"los") || !strcmp(enclitico.enclitico,"las"))){
			_copia_cadena(enclitico.enclitico,(enclitico.enclitico)+1);
		}
	}
	while (n--) {
		strcpy(ptr->enclitico.enclitico,enclitico.enclitico);
		//strcpy(ptr->enclitico.reconstruccion,enclitico.reconstruccion);
		ptr->enclitico.grupo=enclitico.grupo;
		if (n) {
			ptr--;
		}
	}
	return 0;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param artigo  
 * \param inf  
 * \param n  
 *
 * \return 
 */
int gbm_verbos::mete_artigo(char *artigo,t_infinitivo *inf,int n) {
	t_infinitivo *ptr=inf;
	while(ptr->infinitivo[0]) ptr++;
	ptr--;
	while (n--) {
		strcpy(ptr->artigo,artigo);
		if (n) ptr--;
	}
	return 0;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param verbo  
 *
 * \return 
 */
int gbm_verbos::excep_verbo(char *verbo){
	
	const char **kk = excepciones_verbos;
	int tam_lista=0;
	while (*kk[0]) {
		kk++;
		tam_lista++;
	}
	if (comprobar_en_lista_de_palabras2(excepciones_verbos,sizeof(char*), tam_lista,verbo)>=0) return 1;
	else return 0;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param pal  
 * \param idioma  
 *
 * \return 
 */
int gbm_verbos::diacritico(char * pal, char idioma){
	char * pos_acento;
	char pal_aux[200];

	pos_acento=strpbrk(pal,"áéíóú");

	if (pos_acento== NULL) return 0;

	if (reacentua(pal,pal_aux, idioma)>=3) return 2;
	if (strcmp(pal,pal_aux)) return 1;
	else return 0;
}

/**
 * \brief 
 * \author fmendez 
 *
 * \param pal  
 * \param pal_sal  
 * \param idioma  
 *
 * \return 
 */
int gbm_verbos::reacentua(char * pal,char * pal_sal, char idioma){
	char aux[200],aux2[200],*rec,*letra_final;

	int pos_acent,num_silabas_pal=1,procesado=0;

	separar_silabas(pal,aux);
	pos_acent=acentuar_prosodicamente(aux,aux2,idioma);
	rec=aux2;
	while (*rec){
		if (*rec=='-') num_silabas_pal++;
		rec++;
	}
	rec=aux2;
	while (*rec && *rec!='^') rec++;

	if ( acento_para_desfacer_ditongo(aux2,rec)) {
		procesado=1;
		acentua_graficamente(rec-1);
	}
	letra_final=&(aux2[strlen(aux2)-1]);
	while(*letra_final=='-' || *letra_final=='^') letra_final--;

	if ( procesado==0 && pos_acent==1) {
		if (( *letra_final=='n' || *letra_final=='s' || vocal((unsigned char)*letra_final))
				&& !ditongo_decrecente(rec-1) && num_silabas_pal>1 ){
			acentua_graficamente(rec-1);
		}
	}
	if ( procesado==0 && pos_acent==2) {
		if ( *letra_final!='n' &&  *letra_final!='s' && !vocal((unsigned char)*letra_final)) {
			acentua_graficamente(rec-1);
		}
	}
	if (procesado==0 && pos_acent>2) {
		acentua_graficamente(rec-1);
	}

	rec=aux2;
	// Eliminamola separacion de silabas
	*pal_sal=0;
	while (*rec) {
		if (*rec!='-' && *rec!='^')
			strncat(pal_sal,rec,1);
		rec++;
	}


	return pos_acent;
}



/**
* \author giglesia
* \remarks Esta función busca una raíz verbal.
* \param pal: el candidato a raíz verbal que se verificará.
* \return -1 si no lo ha encontrado; o un número mayor o igual que cero
* correspondiente a la posición en la lista de raíces.
*/
int gbm_verbos::comprobar_en_lista_de_verbos(char *pal, char idioma){

        return gbrv->busca(pal);
}


/**
* \author giglesia
* \remarks Esta función se encarga de buscar una desinencia
* en la lista de desinencias gestionada por el gbm correspondiente.
* \param pal: desinencia a buscar.
* \param tamanio: un reductor del ámbito a buscar, para hacer más
* eficientes las búsquedas.
* \return -1 si no lo ha encontrado un un número mayor o igual que cero que
* indique su posición en la lista de desinencias.
*/
int gbm_verbos::buscar_en_desinencias(char *pal, int tamanio, char idioma) {

        return gbd->busca(pal,EN_DICCIONARIO_INVERSO,tamanio);

}

gbm_verbos::gbm_verbos() {

  gbrv=new gbm_raices_verbales();
  gbd=new gbm_desinencias();



}


gbm_verbos::gbm_verbos(char *raices, char *desinencias){

// Coger punteros los diccionarios de raíces y de desinencias.

  gbrv=new gbm_raices_verbales(raices);
  gbd=new gbm_desinencias(desinencias);


}


gbm_verbos::~gbm_verbos() {

  delete gbrv;
  delete gbd;


}

