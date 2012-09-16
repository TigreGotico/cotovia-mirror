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
 

#include <cotovia2eagles.hpp>



CotoviaEagles* CotoviaEagles::CE=NULL;
//CotoviaEagles CotoviaEagles::CE;

CotoviaEagles::CotoviaEagles(void){
    /*
//Inicializaciones chungas, solo si hace falta (-lin 4)
  oC2E[NOME]="SC";
  oC2E[NOME_PROPIO]="SP";
  oC2E[ADXECTIVO]="X0";
  oC2E[ADVE_LUG]="BT";
  */  


//Género y Número...

  genero2Eagles[MASCULINO]="M";
  genero2Eagles[FEMININO]="F";
  genero2Eagles[NEUTRO]="N";
  genero2Eagles[AMBIGUO]="A";
  genero2Eagles[NO_ASIGNADO]=" ";

  numero2Eagles[SINGULAR]="S";
  numero2Eagles[PLURAL]="P";
  numero2Eagles[AMBIGUO]="A";
  numero2Eagles[NO_ASIGNADO]=" ";  

  oC2E[PREP]="P";
  oC2E[VERBO]="V";
  oC2E[XERUNDIO]="VX";
  oC2E[PARTICIPIO]="VT";
  oC2E[INFINITIVO]="VI";      
  oC2E[NOME]="SC";
  oC2E[NOME_PROPIO]="SP";
  oC2E[ADXECTIVO]="X0";
  oC2E[ART_DET]="AD";
  oC2E[ART_INDET]="AI";




/*
oC2E[ART_DET]=1
oC2E[CONTR_CONX_ART_DET]=2

oC2E[CONTR_INDEF_ART_DET]=4

oC2E[ART_INDET]=5
oC2E[CONTR_PREP_ART_INDET]=6

oC2E[INDEF]=7
*/
//  oC2E[PERIFRASE]="*";
  oC2E[PERIFRASE]="*VP";
  oC2E[INDEF_PRON]="NI";
  oC2E[INDEF_DET]="DI";
/*
//nuevas
oC2E[CONTR_PREP_INDEF]=35
oC2E[CONTR_PREP_INDEF_DET]=36
oC2E[CONTR_PREP_INDEF_PRON]=37
*/

//oC2E[DEMO]=10
  oC2E[DEMO]="ND";  //por asumir algo.
  oC2E[DEMO_DET]="DD";
  oC2E[DEMO_PRON]="ND";
  oC2E[POSE_DET]="DP";
  oC2E[POSE_PRON]="NP";

  oC2E[CONTR_CONX_ART_DET]="CSP+AD";
  oC2E[CONTR_CONX_COOR_COP_ART_DET]="CCC+AD";
  oC2E[CONTR_DEMO_INDEF]="DD+ZI";
  oC2E[CONTR_DEMO_INDEF_DET]="DD+DI";
  oC2E[CONTR_DEMO_INDEF_PRON]="DD+NI";
  oC2E[CONTR_INDEF_ART_DET]="DI+AD";
  oC2E[CONTR_PREP_ART_DET]="P+AD";
  oC2E[CONTR_PREP_ART_INDET]="P+AI";
  oC2E[CONTR_PREP_DEMO]="P+ZD";
  oC2E[CONTR_PREP_DEMO_DET]="P+DD";
  oC2E[CONTR_PREP_DEMO_INDEF]="P+DD+ZI";
  oC2E[CONTR_PREP_DEMO_INDEF_DET]="P+DD+DI";
  oC2E[CONTR_PREP_DEMO_INDEF_PRON]="P+DD+NI";
  oC2E[CONTR_PREP_DEMO_PRON]="P+ND";
  oC2E[CONTR_PREP_INDEF]="P+ZI";
  oC2E[CONTR_PREP_INDEF_DET]="P+DI";
  oC2E[CONTR_PREP_INDEF_PRON]="P+NI";
  oC2E[CONTR_PREP_PRON_PERS_TON]="P+NT";
  oC2E[CONTR_PRON_PERS_AT]="NA+NA";
  oC2E[CONTR_PRON_PERS_AT_DAT_AC]="NZ+NQ";
  oC2E[CONTR_PRON_PERS_AT_DAT_DAT_AC]="NZ+NZ+NQ";

  /*

//oC2E[POSE]=25
oC2E[POSE]="DP";
oC2E[POSE_DET]="DP";
oC2E[POSE_PRON]="PP";
//oC2E[POSE_DISTR]=28

/*
oC2E[NUME]=180
oC2E[NUME_DET]=181
oC2E[NUME_PRON]=182
oC2E[NUME_CARDI]=183
oC2E[NUME_CARDI_DET]=184
oC2E[NUME_CARDI_PRON]=185
oC2E[NUME_ORDI]=186
oC2E[NUME_ORDI_DET]=187
oC2E[NUME_ORDI_PRON]=188
*/

  oC2E[NUME_DET]= "D?";
  oC2E[NUME_PRON]="N?";
  oC2E[NUME_CARDI_DET]="DC";
  oC2E[NUME_CARDI_PRON]="NC";
  oC2E[NUME_ORDI_DET]="DO";
  oC2E[NUME_ORDI_PRON]="NO";

 /*
oC2E[NUME_PARTI]=189
oC2E[NUME_PARTI_DET]=190
oC2E[NUME_PARTI_PRON]=191
*/

  oC2E[NUME_PARTI_DET]="DR";
  oC2E[NUME_PARTI_PRON]="NR";



/*
oC2E[NUME_MULTI]=192
//oC2E[NUME_MULTI_DET]=193
//oC2E[NUME_MULTI_PRON]=194
oC2E[NUME_COLECT]=195
//oC2E[NUME_COLECT_DET]=196
//oC2E[NUME_COLECT_PRON]=197

oC2E[PRON_PERS_TON]=40
oC2E[CONTR_PREP_PRON_PERS_TON]=41
*/

  oC2E[PRON_PERS_AT]="NA";
  oC2E[PRON_PERS_AT_REFLEX]="NX";
  oC2E[PRON_PERS_AT_ACUS]="NQ";
  oC2E[PRON_PERS_AT_DAT]="NZ";
  oC2E[PRON_PERS_TON]="NT";



/*
oC2E[CONTR_PRON_PERS_AT_DAT_AC]=46
oC2E[CONTR_PRON_PERS_AT_DAT_DAT_AC]=47
oC2E[CONTR_PRON_PERS_AT]=48

oC2E[ADVE]=50
*/
  oC2E[ADVE]="B0?";
  oC2E[ADVE_LUG]="B0L";
  oC2E[ADVE_TEMP]="B0T";
  oC2E[ADVE_CANT]="B0C";
  oC2E[ADVE_MODO]="B0M";


  oC2E[ADVE_AFIRM]="B0A";
  oC2E[ADVE_NEGA]="B0N";
  oC2E[ADVE_DUBI]="B0D";

  oC2E[LOC_ADVE]="*B0";
  oC2E[LOC_ADVE_LUG]="*B0L";
  oC2E[LOC_ADVE_TEMP]="*B0T";
  oC2E[LOC_ADVE_CANTI]="*B0C";
  oC2E[LOC_ADVE_MODO]="*B0M";
  oC2E[LOC_ADVE_AFIR]="*B0A";
  oC2E[LOC_ADVE_NEGA]="*B0N";
  oC2E[LOC_ADVE_DUBI]="*B0D";

/*
oC2E[ADVE_ESPECIFICADOR]=65  //moi e tan
*/
//oC2E[PREP]="PREP";
  oC2E[LOC_PREP]="*P";
  oC2E[LOC_PREP_LUG]="*PL";
  oC2E[LOC_PREP_TEMP]="*PT";
  oC2E[LOC_PREP_CANT]="*PA";
  oC2E[LOC_PREP_MODO]="*PM";
  oC2E[LOC_PREP_CAUS]="*PC";
  oC2E[LOC_PREP_CONDI]="*PO";

  oC2E[RELA]="NR";
  oC2E[INTER]="NG";
  oC2E[EXCLA]="NE";

  oC2E[CONX_COOR]="CC";
  oC2E[CONX_COOR_COPU]="CCC";
  oC2E[CONX_COOR_DISX]="CCD";
  oC2E[CONX_COOR_ADVERS]="CCA";
//nueva



  oC2E[CONX_SUBOR]="CS";
  oC2E[CONX_SUBOR_PROPOR]="CSP";
  oC2E[CONX_SUBOR_FINAL]="CSF";
  oC2E[CONX_SUBOR_CONTRAP]="CSR";
  oC2E[CONX_SUBOR_CAUS]="CSC";
  oC2E[CONX_SUBOR_CONCES]="CSN";
  oC2E[CONX_SUBOR_CONSE]="CSS";
  oC2E[CONX_SUBOR_CONDI]="CSD";
  oC2E[CONX_SUBOR_COMPAR]="CSO";
  oC2E[CONX_SUBOR_LOCA]="CSL";
  oC2E[CONX_SUBOR_TEMP]="CST";
  oC2E[CONX_SUBOR_MODAL]="CSM";
  oC2E[CONX_SUBOR_COMPLETIVA]="CSE";

  oC2E[LOC_CONX]="*C";
  oC2E[LOC_CONX_COOR_COPU]="*CCC";
  oC2E[LOC_CONX_COOR_ADVERS]="*CCA";
  oC2E[LOC_CONX_SUBOR_CAUS]="*CSC";
  oC2E[LOC_CONX_SUBOR_CONCES]="*CSN";
  oC2E[LOC_CONX_SUBOR_CONSE]="*CSS";
  oC2E[LOC_CONX_SUBOR_COMPAR]="*CSO";
  oC2E[LOC_CONX_SUBOR_CONDI]="*CSD";
  oC2E[LOC_CONX_SUBOR_LOCAL]="*CSL";
  oC2E[LOC_CONX_SUBOR_TEMP]="*CST";
  oC2E[LOC_CONX_SUBOR_MODA]="*CSM";
  oC2E[LOC_CONX_SUBOR_CONTRAP]="*CSR";
  oC2E[LOC_CONX_SUBOR_FINAL]="*CSF";
  oC2E[LOC_CONX_SUBOR_PROPOR]="*CSP";
  oC2E[LOC_CONX_SUBOR_CORREL]="*CS?";

  oC2E[PUNTO]="UP";
  oC2E[COMA]= "UC";
  oC2E[PUNTO_E_COMA]= "UN";
  oC2E[DOUS_PUNTOS]="UD";
  oC2E[PUNTOS_SUSPENSIVOS]="US";
  oC2E[APERTURA_INTERROGACION]="UGA";
  oC2E[PECHE_INTERROGACION]= "UGP";
  oC2E[APERTURA_EXCLAMACION]="UEA";
  oC2E[PECHE_EXCLAMACION]= "UEP";
  oC2E[APERTURA_PARENTESE]="URA";
  oC2E[PECHE_PARENTESE]=   "URP";
  oC2E[APERTURA_CORCHETE]= "UTA";
  oC2E[PECHE_CORCHETE]=    "UTP";
  oC2E[GUION]=             "UI";
  oC2E[DOBLES_COMINNAS]=   "UM";
  oC2E[SIMPLES_COMINNAS]=  "UL";
  oC2E[GUION_BAIXO]=       "UB";
  oC2E[SIGNO]="U0";

  oC2E[INTERX]="I";


//Más inicializaciones chungas, al revés...
//Estas harán siempre falta.

 oE2C["X0"]=ADXECTIVO;
 oE2C["SC"]=NOME;
 oE2C["AD"]=ART_DET;
 oE2C["AI"]=ART_INDET;  
 oE2C["SP"]=NOME_PROPIO;
 oE2C["NC"]=NUME_CARDI_PRON;
 oE2C["DC"]=NUME_CARDI_DET;
 oE2C["NP"]=POSE_PRON;
 oE2C["DP"]=POSE_DET;
 oE2C["ND"]=DEMO_PRON;
 oE2C["DD"]=DEMO_DET;
 oE2C["NI"]=INDEF_PRON;
 oE2C["DI"]=INDEF_DET;
 oE2C["ZM"]=NUME_MULTI;
 oE2C["ZP"]=POSE; 
 oE2C["ZD"]=DEMO;
 oE2C["ZI"]=INDEF;
 oE2C["ZC"]=NUME_CARDI;
 oE2C["ZO"]=NUME_ORDI;
 oE2C["ZL"]=NUME_COLECT;   
 oE2C["ZV"]=NUME_PARTI;
 oE2C["NV"]=NUME_PARTI_PRON;
 oE2C["DV"]=NUME_PARTI_DET;
 oE2C["NO"]=NUME_ORDI_PRON;
 oE2C["DO"]=NUME_ORDI_DET;
 oE2C["NA"]=PRON_PERS_AT; 
 oE2C["NX"]=PRON_PERS_AT_REFLEX;
 oE2C["NQ"]=PRON_PERS_AT_ACUS;
 oE2C["NZ"]=PRON_PERS_AT_DAT;
 oE2C["NT"]=PRON_PERS_TON;
 oE2C["P"]=PREP;
 oE2C["P+AD"]=CONTR_PREP_ART_DET;
 oE2C["P+AI"]=CONTR_PREP_ART_INDET;
 oE2C["P+ND"]=CONTR_PREP_DEMO_PRON;
 oE2C["P+DD"]=CONTR_PREP_DEMO_DET;
 oE2C["P+ZD"]=CONTR_PREP_DEMO; 
 oE2C["P+DI"]=CONTR_PREP_INDEF_DET;
 oE2C["P+ZI"]=CONTR_PREP_INDEF; 
 oE2C["DI+AD"]=CONTR_INDEF_ART_DET; 
 oE2C["P+NI"]=CONTR_PREP_INDEF_PRON;
 oE2C["CCC+AD"]=CONTR_CONX_COOR_COP_ART_DET;
 oE2C["P+DD+DI"]=CONTR_PREP_DEMO_INDEF_DET;
 oE2C["P+DD+NI"]=CONTR_PREP_DEMO_INDEF_PRON;
 oE2C["P+DD+ZI"]=CONTR_PREP_DEMO_INDEF;
 oE2C["P+NT"]=CONTR_PREP_PRON_PERS_TON;
 oE2C["DD+DI"]=CONTR_DEMO_INDEF_DET;
 oE2C["DD+NI"]=CONTR_DEMO_INDEF_PRON;
 oE2C["DD+ZI"]=CONTR_DEMO_INDEF;
 oE2C["CSP+AD"]=CONTR_CONX_ART_DET;
 oE2C["NZ+NQ"]=CONTR_PRON_PERS_AT_DAT_AC;
 oE2C["NZ+NZ+NQ"]=CONTR_PRON_PERS_AT_DAT_DAT_AC;
 oE2C["NA+NA"]=CONTR_PRON_PERS_AT;


 oE2C["B0L"]=ADVE_LUG;
 oE2C["B0T"]=ADVE_TEMP;
 oE2C["B0C"]=ADVE_CANT;
 oE2C["B0M"]=ADVE_MODO;


 oE2C["B0A"]=ADVE_AFIRM;
 oE2C["B0N"]=ADVE_NEGA;
 oE2C["B0D"]=ADVE_DUBI;

 oE2C["*B0L"]=LOC_ADVE_LUG;
 oE2C["*B0T"]=LOC_ADVE_TEMP;
 oE2C["*B0C"]=LOC_ADVE_CANTI;
 oE2C["*B0M"]=LOC_ADVE_MODO;
 oE2C["*B0A"]=LOC_ADVE_AFIR;
 oE2C["*B0N"]=LOC_ADVE_NEGA;
 oE2C["*B0D"]=LOC_ADVE_DUBI;

/*
oC2E[ADVE_ESPECIFICADOR]=65  //moi e tan
*/
//oE2C["PREP"]=PREP;

  oE2C["*PL"]=LOC_PREP_LUG;
  oE2C["*PT"]=LOC_PREP_TEMP;
  oE2C["*PA"]=LOC_PREP_CANT;
  oE2C["*PM"]=LOC_PREP_MODO;
  oE2C["*PC"]=LOC_PREP_CAUS;
  oE2C["*PD"]=LOC_PREP_CONDI;

  oE2C["NR"]=RELA;
  oE2C["BR"]=RELA;
  oE2C["NG"]=INTER;
  oE2C["BG"]=INTER;
  oE2C["NE"]=EXCLA;
  oE2C["BE"]=EXCLA;

//  oE2C["CC"]=CONX_COOR;
  oE2C["CCC"]=CONX_COOR_COPU;
  oE2C["CCD"]=CONX_COOR_DISX;
  oE2C["CCA"]=CONX_COOR_ADVERS;
  
//nueva
  oE2C["CCC+AD"]=CONTR_CONX_COOR_COP_ART_DET;

  oE2C["CS"]=CONX_SUBOR;
  oE2C["CSP"]=CONX_SUBOR_PROPOR;
  oE2C["CSF"]=CONX_SUBOR_FINAL;
  oE2C["CSR"]=CONX_SUBOR_CONTRAP;
  oE2C["CSC"]=CONX_SUBOR_CAUS;
  oE2C["CSN"]=CONX_SUBOR_CONCES;
  oE2C["CSS"]=CONX_SUBOR_CONSE;
  oE2C["CSD"]=CONX_SUBOR_CONDI;
  oE2C["CSO"]=CONX_SUBOR_COMPAR;
  oE2C["CSL"]=CONX_SUBOR_LOCA;
  oE2C["CST"]=CONX_SUBOR_TEMP;
  oE2C["CSM"]=CONX_SUBOR_MODAL;
  oE2C["CSE"]=CONX_SUBOR_COMPLETIVA;

  oE2C["V"]=VERBO;
  oE2C["VX"]=XERUNDIO;
  oE2C["VT"]=PARTICIPIO;
  oE2C["VI"]=INFINITIVO;
  oE2C["I"]=INTERX;



  /*
  //oE2C["*C"]=LOC_CONX;
  oE2C["*CCC"]=LOC_CONX_COOR_COPU;
  oE2C["*CCA"]=LOC_CONX_COOR_ADVERS;
  oE2C["*CCU"]=LOC_CONX_SUBOR_CAUS;
  oE2C["*CSS"]=LOC_CONX_SUBOR_CONCES;
  oE2C["*CSV"]=LOC_CONX_SUBOR_CONSE;
  oE2C["*CSP"]=LOC_CONX_SUBOR_COMPAR;
  oE2C["*CSD"]=LOC_CONX_SUBOR_CONDI;
  oE2C["*CSL"]=LOC_CONX_SUBOR_LOCAL;
  oE2C["*CST"]=LOC_CONX_SUBOR_TEMP;
  oE2C["*CSM"]=LOC_CONX_SUBOR_MODA;
  oE2C["*CSP"]=LOC_CONX_SUBOR_CONTRAP;
  oE2C["*CSF"]=LOC_CONX_SUBOR_FINAL;
  oE2C["*CSN"]=LOC_CONX_SUBOR_PROPOR;
  oE2C["*CS?"]=LOC_CONX_SUBOR_CORREL;

  oE2C["UT"]=PUNTO;
  oE2C["UA"]=COMA;
  oE2C["UN"]=PUNTO_E_COMA;
  oE2C["UD"]=DOUS_PUNTOS;
  oE2C["US"]=PUNTOS_SUSPENSIVOS;
  oE2C["UIA"]=APERTURA_INTERROGACION;
  oE2C["UIP"]=PECHE_INTERROGACION;
  oE2C["UEA"]=APERTURA_EXCLAMACION;
  oE2C["UEP"]=PECHE_EXCLAMACION;
  oE2C["UPA"]=APERTURA_PARENTESE;
  oE2C["UPP"]=PECHE_PARENTESE;
  oE2C["UCA"]=APERTURA_CORCHETE;
  oE2C["UCP"]=PECHE_CORCHETE;
  oE2C["UG"]=GUION;
  oE2C["UM"]=DOBLES_COMINNAS;
  oE2C["UU"]=SIMPLES_COMINNAS;
  oE2C["UB"]=GUION_BAIXO;

       */



}

unsigned int CotoviaEagles::mGetTVCotovia(string categoriaEagles) {

  //Formato de categoriaEagles: codigo_eagles/lema_verbal

  unsigned int codigo;

  if (categoriaEagles[0]!='V') {
   printf("ERROR:¡No es un verbo!");
   return 0;
  }

//NOTA: hace falta el verbo para extraer la conjugación a la que pertenece!!!

//s-ar,-er,-ir.

//Tiempo y modo verbal...
//  categoriaEagles[5]
  int modo,tiempo;
  switch (categoriaEagles[1]) {
  case 'D': //indicativo
    modo=1;
    break;
  case 'S': //subjuntivo
    modo=109;
    break;
  case 'V': //imperativo
    modo=164;
    break;
  case 'I': //infinitivo
    modo=199;
    break;
  case 'C': //infinitivo conjugado
    modo=199;
    break;
  case 'T': //participio
    modo=183;
    break;
  case 'X': //xerundio.
    modo=180;
    break;
  }

  switch (categoriaEagles[5]) {
  case 'P': //presente
    tiempo=0;
    break;
  case 'R': //Pretérito.
    if (109==modo) tiempo=18;
    else tiempo=36;
    break;
  case 'C': //copretérito
    tiempo=18;
    break;
  case 'A': //antepretérito
    tiempo=54;
    break;
  case 'T': //pospretérito
    tiempo=90;
    break;
  case 'F': //futuro
    if (109==modo) tiempo=36;
    else tiempo=72;
    break;
  }


//El número de persona.
  int persona= (int) categoriaEagles[4] - 1 ;
//El plural siempre suma 3.
  int numero=('P'==categoriaEagles[3])?3:0;
  if (183==tiempo && 3==numero) numero=2;  //excepción, participios. Suma 2.
  int genero=(183==tiempo && 'F'==categoriaEagles[2])?1:0;

  int conjugacion=1;
  int ultimo=categoriaEagles.length()-1;

//La conjugación suma 0/6/12,
// gerundios 1/2
// participios 4/8

  int factor=6; //caso general.
  if (183==tiempo) factor=4; //participios
  else if (180==tiempo) factor=1; //gerundios


  if (categoriaEagles[ultimo-1]=='a') conjugacion=0;
  else if (categoriaEagles[ultimo-1]=='e') conjugacion=factor;
  else if (categoriaEagles[ultimo-1]=='i') conjugacion=2*factor;   

  codigo=modo+tiempo+persona+genero+numero+conjugacion;
  return codigo;

} 

string CotoviaEagles::mGetTVEagles(unsigned char codigo){

  string categoriaEagles;

   unsigned char tempo;
   unsigned char persoa;
//   unsigned char conj;

   tempo=(unsigned char) ((codigo-1)/18);
   persoa=(unsigned char) ((codigo-1)%6);
   //conj=(unsigned char)((((codigo-1)/6) % 3)+1);
   char tiempo;
   if (codigo==180 || codigo==181 || codigo==182)  {
    return categoriaEagles="X";
   } else if (codigo>=183 && codigo <=194)  {
      string genero =(codigo%2)?"M":"F";
      string numero=((codigo-183)%4<=1)? "S":"P";
      categoriaEagles="T"+ genero + numero;
      return categoriaEagles;
   }
// TEMPO MODO E CONXUGACION

   switch(tempo)  {
      case 0:
         categoriaEagles="D0";
         tiempo='P';
         break;
      case 1:
         categoriaEagles="D0";
         tiempo='C';
         break;
      case 2:
         categoriaEagles="D0";
         tiempo='R';
         break;
      case 3:
         categoriaEagles="D0";
         tiempo='A';
         break;
      case 4:
         categoriaEagles="D0";
         tiempo='F';
         break;
      case 5:
         categoriaEagles="D0";
          tiempo='T';
         break;
      case 6:
         categoriaEagles="S0";
         tiempo='P';
         break;
      case 7:
         categoriaEagles="S0";
         tiempo='R';
         break;
      case 8:
         categoriaEagles="S0";
         tiempo='F';
         break;
     case 9:
         categoriaEagles="V0";
         tiempo='0';
         break;
     case 11:
         categoriaEagles="C0";
         tiempo='0';
         break;
      default:
        tiempo='0';
   }


// PERSOA E NUMERO

   switch(persoa) {
      case 0:
        categoriaEagles+="S1";
         break;
      case 1:
        categoriaEagles+="S2";
         break;
      case 2:
        categoriaEagles+="S3";
         break;
      case 3:
        categoriaEagles+="P1";
         break;
      case 4:
        categoriaEagles+="P2";
         break;
      case 5:
        categoriaEagles+="P3";
         break;
   }

   categoriaEagles+=tiempo;

/*
   switch(conj) {
      case 1:
        fputs("\t1ª CONXUGACION\t",fich_sal);
        break;
      case 2:
        fputs("\t2ª CONXUGACION\t",fich_sal);
        break;
      case 3:
        fputs("\t3ª CONXUGACION\t",fich_sal);
        break;
   }
   */
//   categoriaEagles+=0; //Transitividad.

   return categoriaEagles;
}


 unsigned char CotoviaEagles::mGetE2C(string sIdx){

  for (int k=sIdx.length();k--;k>=0) {
    if (oE2C[sIdx]) return oE2C[sIdx];
    sIdx.erase(k);
  }
  return 0;
 }

  int CotoviaEagles::mDG(string Idx) {

   unsigned int pos;

    pos=Idx.find_last_of("+");
    if (pos >Idx.length()-4) pos=-1;
    switch (Idx[pos+1]){
    case 'S':
    case 'X':
    case 'N':
    case 'Z':
    case 'D':
    case 'A':
      return pos+3; //Posición de Género. Siguiente es número.
    }

    return -1;
  }






