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
 
#ifndef LC_STAR_HPP
#define LC_STAR_HPP

#include	<map>

class Lcstar {

//	static int inicializada;
	static Lcstar *lc;
	Lcstar () {
		genero[MASCULINO]="masculine";
		genero[FEMININO]="feminine";
		genero[NEUTRO]="neuter";
		genero[AMBIGUO]="invariant";

		numero[SINGULAR]="singular";
		numero[PLURAL]="plural";
		numero[AMBIGUO]="invariant";

		categoria[ART_DET]="ART type=\"definite\"";
		categoria[CONTR_CONX_ART_DET]="CON";
		categoria[CONTR_PREP_ART_DET]="ADP type=\"articulated\"";
		categoria[CONTR_INDEF_ART_DET]="DET type=\"indefinite\"";
		categoria[ART_INDET]="ART type=\"indefinite\"";
		categoria[CONTR_PREP_ART_INDET]="ADP type=\"articulated\"";
		categoria[INDEF]="PRO type=\"indefinite\"";
		categoria[INDEF_PRON]="PRO type=\"indefinite\"";
		categoria[INDEF_DET]="DET type=\"indefinite\"";
		categoria[CONTR_PREP_INDEF]="ADP type=\"articulated\"";
		categoria[CONTR_PREP_INDEF_DET]="ADP type=\"articulated\"";
		categoria[CONTR_PREP_INDEF_PRON]="ADP type=\"articulated\"";
		categoria[DEMO]="PRO type=\"demonstrative\"";
		categoria[DEMO_DET]="DET type=\"demonstrative\"";
		categoria[DEMO_PRON]="PRO type=\"demonstrative\"";
		categoria[CONTR_PREP_DEMO]="ADP";
		categoria[CONTR_PREP_DEMO_DET]="ADP";
		categoria[CONTR_PREP_DEMO_PRON]="ADP";
		categoria[CONTR_DEMO_INDEF]="PRO type=\"demonstrative\"";
		categoria[CONTR_DEMO_INDEF_DET]="DET type=\"demonstrative\"";
		categoria[CONTR_DEMO_INDEF_PRON]="PRO type=\"demonstrative\"";
		categoria[CONTR_PREP_DEMO_INDEF]="ADP";
		categoria[CONTR_PREP_DEMO_INDEF_DET]="ADP";
		categoria[CONTR_PREP_DEMO_INDEF_PRON]="ADP";
		categoria[POSE]="PRO type=\"possessive\"";
		categoria[POSE_DET]="DET type=\"possessive\"";
		categoria[POSE_PRON]="PRO type=\"possessive\"";
		categoria[POSE_DISTR]="DET type=\"possessive\"";
		categoria[NUME]="NUM";/////////
		categoria[NUME_DET]="NUM type=\"cardinal\"";
		categoria[NUME_PRON]="PRO type=\"quantifying\"";/////////
		categoria[NUME_CARDI]="NUM type=\"cardinal\"";
		categoria[NUME_CARDI_DET]="NUM type=\"cardinal\"";
		categoria[NUME_CARDI_PRON]="PRO type=\"quantifying\"";/////////
		categoria[NUME_ORDI]="NUM type=\"ordinal\"";
		categoria[NUME_ORDI_DET]="NUM type=\"ordinal\"";
		categoria[NUME_ORDI_PRON]="PRO type=\"quantifying\"";/////////
		categoria[NUME_PARTI]="NUM";/////////
		categoria[NUME_PARTI_DET]="NUM type=\"ratio\"";
		categoria[NUME_PARTI_PRON]="NUM";///////////////
		categoria[NUME_MULTI]="NUM type=\"multiplicative\"";
		categoria[NUME_COLECT]="NUM type=\"collective\"";
		categoria[PRON_PERS_TON]="PRO type=\"personal\"";
		categoria[CONTR_PREP_PRON_PERS_TON]="PRO type=\"personal\"";
		categoria[PRON_PERS_AT]="PRO";////////////////
		categoria[PRON_PERS_AT_REFLEX]="PRO type=\"reflexive\"";
		categoria[PRON_PERS_AT_ACUS]="PRO case=\"accusative\"";
		categoria[PRON_PERS_AT_DAT]="PRO case=\"dative\"";
		categoria[CONTR_PRON_PERS_AT_DAT_AC]="PRO case=\"dative\"";
		categoria[CONTR_PRON_PERS_AT_DAT_DAT_AC]="PRO case=\"dative\"";
		categoria[CONTR_PRON_PERS_AT]="PRO";/////////
		categoria[ADVE]="ADV";
		categoria[ADVE_LUG]="ADV type=\"place\"";
		categoria[ADVE_TEMP]="ADV type=\"time\"";
		categoria[ADVE_CANT]="ADV";
		categoria[ADVE_MODO]="ADV type=\"manner\"";
		categoria[ADVE_AFIRM]="ADV";
		categoria[ADVE_NEGA]="ADV";
		categoria[ADVE_DUBI]="ADV";
		categoria[LOC_ADVE_LUG]="ADV type=\"place\"";
		categoria[LOC_ADVE_TEMP]="ADV type=\"time\"";
		categoria[LOC_ADVE_CANTI]="ADV";
		categoria[LOC_ADVE_MODO]="ADV type=\"manner\"";
		categoria[LOC_ADVE_AFIR]="ADV";
		categoria[LOC_ADVE_NEGA]="ADV";
		categoria[LOC_ADVE_DUBI]="ADV";
		categoria[ADVE_ESPECIFICADOR]="ADV";
		categoria[PREP]="ADP type=\"simple\"";
		categoria[LOC_PREP_LUG]="ADP";
		categoria[LOC_PREP_TEMP]="ADP";
		categoria[LOC_PREP_CANT]="ADP";
		categoria[LOC_PREP_MODO]="ADP";
		categoria[LOC_PREP_CAUS]="ADP";
		categoria[LOC_PREP_CONDI]="ADP";
		categoria[RELA]="PRO type=\"relative\"";
		categoria[INTER]="PRO type=\"interrogative\"";
		categoria[EXCLA]="PRO type=\"exclamative\"";
		categoria[CONX_COOR]="CON type=\"coordinating\"";
		categoria[CONX_COOR_COPU]="CON type=\"coordinating\"";
		categoria[CONX_COOR_DISX]="CON type=\"coordinating\"";
		categoria[CONX_COOR_ADVERS]="CON type=\"coordinating\"";
		categoria[CONTR_CONX_COOR_COP_ART_DET]="CON type=\"coordinating\"";
		categoria[CONX_SUBOR]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_PROPOR]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_FINAL]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_CONTRAP]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_CAUS]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_CONCES]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_CONSE]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_CONDI]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_COMPAR]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_LOCA]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_TEMP]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_MODAL]="CON type=\"subordinating\"";
		categoria[CONX_SUBOR_COMPLETIVA]="CON type=\"subordinating\"";
		categoria[LOC_CONX_COOR_COPU]="CON type=\"coordinating\"";
		categoria[LOC_CONX_COOR_ADVERS]="CON type=\"coordinating\"";
		categoria[LOC_CONX_SUBOR_CAUS]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_CONCES]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_CONSE]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_CONDI]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_LOCAL]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_TEMP]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_MODA]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_CONTRAP]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_FINAL]="CON type=\"subordinating\"";
		categoria[LOC_CONX_SUBOR_PROPOR]="CON type=\"subordinating\"";
		categoria[NOME]="NOM class=\"common\"";
		categoria[ADXECTIVO]="ADJ";
		categoria[NOME_PROPIO]="NOM class=\"PER\"";
		categoria[VERBO]="VER";
		categoria[INFINITIVO]="VER mood=\"infinitive\"";
		categoria[XERUNDIO]="VER mood=\"gerund\"";
		categoria[PARTICIPIO]="VER mood=\"participe\"";
		categoria[INTERX]="INT";
		categoria[PERIFRASE]="VER";
		categoria[SIGNO]="PUN";
		categoria[PUNTO]="PUN";
		categoria[COMA]="PUN";
		categoria[PUNTO_E_COMA]="PUN";
		categoria[DOUS_PUNTOS]="PUN";
		categoria[PUNTOS_SUSPENSIVOS]="PUN";
		categoria[APERTURA_INTERROGACION]="PUN";
		categoria[PECHE_INTERROGACION]="PUN";
		categoria[APERTURA_EXCLAMACION]="PUN";
		categoria[PECHE_EXCLAMACION]="PUN";
		categoria[APERTURA_PARENTESE]="PUN";
		categoria[PECHE_PARENTESE]="PUN";
		categoria[APERTURA_CORCHETE]="PUN";
		categoria[PECHE_CORCHETE]="PUN";
		categoria[GUION]="PUN";
		categoria[DOBLES_COMINNAS]="PUN";
		categoria[SIMPLES_COMINNAS]="PUN";
		categoria[GUION_BAIXO]="PUN";
		categoria[ADVE_ORD]="ADV";
		categoria[ADVE_MOD]="ADV";
		categoria[ADVE_COMP]="ADV";
		categoria[ADVE_CORREL]="ADV";
		categoria[ADVE_DISTR]="ADV";
		categoria[LAT]="LAT";
		categoria[LOC_CONX_SUBOR_CORREL]="CON type=\"subordinating\"";
		categoria[LOC_INTERX]="INT";
		categoria[LOC_ADXE]="ADJ";
		categoria[LOC_CONX]="CON";
		categoria[LOC_SUST]="LOC_SUST";
		categoria[LOC_PREP]="ADP";
		categoria[LOC_ADVE]="ADV";
		categoria[LOC_LAT]="LOC_LAT";
		categoria[CONX_COOR_DISTRIB]="CON type=\"coordinating\"";
		categoria[CONX_SUBOR_CONTI]="CON type=\"subordinating\"";
		categoria[INDET_PRON]="INDET_PRON";
		categoria[PRON_CORREL]="PRON_CORREL";
	  };
public:
  static Lcstar *getLcstar() {return (lc)? lc:lc=new Lcstar();}
  map<char,string> numero;
  map<char,string> genero;
  map<unsigned char,string> categoria;
//  ~Lcstar () {inicializada=0;}
};

class invLcstar {

	static invLcstar *invlc;
	invLcstar () {
		genero['0'] = NO_ASIGNADO;
		genero['M'] = MASCULINO;
		genero['F'] = FEMININO;
		genero['I'] = AMBIGUO;
		genero['N'] = NEUTRO;
		genero['m'] = MASCULINO;
		genero['f'] = FEMININO;
		genero['i'] = AMBIGUO;
		genero['n'] = NEUTRO;

		numero['0'] = NO_ASIGNADO;
		numero['S'] = SINGULAR;
		numero['P'] = PLURAL;
		numero['I'] = AMBIGUO;
		numero['s'] = SINGULAR;
		numero['p'] = PLURAL;
		numero['i'] = AMBIGUO;

		categoria["ART"] = ART_DET;
		categoria["CON"] = CONX_COOR;
		categoria["ADP"] = PREP;
		categoria["ADV"] = ADVE;
		categoria["NOM"] = NOME;
		categoria["VER"] = VERBO;
		categoria["AUX"] = VERBO;
		categoria["ADJ"] = ADXECTIVO;
		categoria["PUN"] = PUNTO;
		categoria["PRO"] = PRON_PERS_TON;
		categoria["NUM"] = NUME_DET;

	};
	public:
	static invLcstar *getinvLcstar() {return (invlc)? invlc:invlc=new invLcstar();}
	map<char,unsigned char> numero;
	map<char,unsigned char> genero;
	map<string, unsigned char> categoria;
};

#endif

