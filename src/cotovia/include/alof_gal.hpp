/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ram�n Pi�eiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigaci�n en Humanidades,
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
 

//static char *rcs_id = "$Id: alof_gal.hpp,v 1.5 2010/03/17 11:41:00 fmendez Exp $"; 
/*
 ESTE ARQUIVO CONT�N AS REGLAS ALOF�NICAS VERDADEIRAS DA LINGUA GALEGA
*/


/* As vocais '�' e '�' prove�en dunha etapa anterior e indican que son
	 abertas.*/

/* Este arquivo conten as tablas e funcions necesarias para realiza-la
	 transcripcion fonetica. Suponse que xa ven�en acentuadas e separadas
	 as palabras. */

/*const t_regla regla_cadeas_iniciadas_por_a_super[]={
{"�",1,"a"} , {0,0,0}    } ;
const t_regla regla_cadeas_iniciadas_por_o_super[]={
{"�",1,"o"} , {0,0,0}    } ;
*/

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_a[]={
  {"a^i", 3, "a^j"}, {"ai", 2, "aj"},
  {"a^u", 3, "a^w"}, {"au", 2, "aw"},
  {"a^",2,"a^"} , {"a",1,"a"} , {0,0,0}    } ;
#else
const t_regla regla_cadeas_iniciadas_por_a[]={
  {"a^",2,"a^"} , {"a",1,"a"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_e[]={
  {"e^i", 3, "e^j"}, {"ei", 2, "ej"},
  {"e^y", 3, "e^j"}, {"ey", 2, "ej"},
  {"e^u", 3, "e^w"}, {"eu", 2, "ew"},
  {"e^",2,"e^"} , {"e",1,"e"} , {0,0,0}    } ;
#else
const t_regla regla_cadeas_iniciadas_por_e[]={
  {"e^",2,"e^"} , {"e",1,"e"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_e_acentuado[]={
  {"�^i",3,"E^j"} , {"�i",2,"Ej"},
  {"�^u",3,"E^w"} , {"�u",2,"Ew"},  
  {"�^",2,"E^"} , {"�",1,"E"} , {0,0,0}    } ;
#else
const t_regla regla_cadeas_iniciadas_por_e_acentuado[]={
  {"�^",2,"E^"} , {"�",1,"E"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_i[]={  
  {"ia^i", 4, "ja^j"}, {"iai", 3, "jaj"},
  {"ia^u", 4, "ja^w"}, {"iau", 3, "jaw"},
  {"ie^i", 4, "je^j"}, {"iei", 3, "jej"},
  {"i�^i", 4, "jE^j"}, {"i�i", 3, "jEj"},
  {"io^u", 4, "jo^w"}, {"iou", 3, "jow"},
  {"i�^u", 4, "jO^w"}, {"i�u", 3, "jOw"},
  {"ia^", 3, "ja^"}, {"ia", 2, "ja"},
  {"ie^", 3, "je^"}, {"ie", 2, "je"},
  {"i�^", 3, "jE^"}, {"i�", 2, "jE"},
  {"io^", 3, "jo^"}, {"io", 2, "jo"}, 
  {"i�^", 3, "jO^"}, {"i�", 2, "jO"}, 
  {"iu^", 3, "ju^"}, {"iu", 2, "ju"}, 
  {"i^u", 3, "i^w"},
  {"i^",2,"i^"} , {"i",1,"i"} , 
  {0,0,0}    } ;
#else
const t_regla regla_cadeas_iniciadas_por_i[]={
  {"i^",2,"i^"} , {"i",1,"i"} , {0,0,0}    } ;
#endif



const t_regla regla_cadeas_iniciadas_por_y[]={
{"y^",2,"i^"} ,
//fmendez
{"ya",2,"Za"},{"ye",2,"Ze"},{"yE",2,"ZE"},{"yi",2,"Zi"},
{"yo",2,"Zo"},{"yO",2,"ZO"},{"yu",2,"Zu"} ,
//fmendez
{"y",1,"i"} , {0,0,0}    } ;

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_o[]={
  {"o^i", 3, "o^j"}, {"oi", 2, "oj"},
  {"o^u", 3, "o^w"}, {"ou", 2, "ow"},
  {"o^",2,"o^"} , {"o",1,"o"}, 
  {0,0,0} };
#else
const t_regla regla_cadeas_iniciadas_por_o[]={
  {"o^",2,"o^"} , {"o",1,"o"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_o_acentuado[]={
  {"�^i", 3, "O^j"}, {"�i", 2, "Oj"},
  {"�^u", 3, "O^w"}, {"�u", 2, "Ow"},
  {"�^",2,"O^"} , {"�",1,"O"} , 
  {0,0,0}    };
#else
const t_regla regla_cadeas_iniciadas_por_o_acentuado[]={
{"�^",2,"O^"} , {"�",1,"O"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_u[]={
  {"ua^u", 4, "wa^w"}, {"uau", 3, "waw"},
  {"ua^i", 4, "wa^j"}, {"uai", 3, "waj"},
  {"ue^i", 4, "we^j"}, {"uei", 3, "wej"},
  {"u�^i", 4, "wE^j"}, {"u�i", 3, "wEj"},
  {"ue^y-", 4, "we^j-"}, {"uey-", 3, "wej-"},
  {"ue^y ", 4, "we^j "}, {"uey ", 3, "wej "},
  {"ua^", 3, "wa^"}, {"ua", 2, "wa"},
  {"ue^", 3, "we^"}, {"ue", 2, "we"},
  {"u�^", 3, "wE^"}, {"u�", 2, "wE"},
  {"ui^", 3, "wi^"}, {"ui", 2, "wi"},
  {"u^i", 3, "u^j"},
  {"uo^", 3, "wo^"}, {"uo", 2, "wo"},
  {"u�^", 3, "wO^"}, {"u�", 2, "wO"},
  {"u^",2,"u^"} , {"u",1,"u"} , {0,0,0}    } ;
#else
const t_regla regla_cadeas_iniciadas_por_u[]={
  {"u^",2,"u^"} , {"u",1,"u"} , {0,0,0}    } ;
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_u_con_dierese[]={
  {"�^i", 3, "u^j"}, {"�i", 2, "wi"},
  {  "�",1,"u" } , {0,0,0}  };
#else
const t_regla regla_cadeas_iniciadas_por_u_con_dierese[]={
  {  "�",1,"u" } , {0,0,0}  };
#endif

const t_regla regla_cadeas_iniciadas_por_e_con_dierese[]={
{  "�",1,"e" } , {0,0,0}  };
const t_regla regla_cadeas_iniciadas_por_i_con_dierese[]={
{  "�",1,"i" } , {0,0,0}  };
const t_regla regla_cadeas_iniciadas_por_espacio[]={
{ " sp",2," es" } ,{ " sq",2," es" } ,{ " st",2," es" } ,{ " sl",2," es" } ,{ " s-q",2," es" } , 
{ " r",2," rr" } , { " ps",3," s" } , { " mn",3," n" } ,{ " pn",3," n" } ,
/* se unha palabra comenza por y seguida de consonante enton a funcion do
   y e como vocal ainda que vaia no principio da palabra. Aparecer� en
   palabras como Yglesias, Ysabel.*/
{ " ya",3," Za" } , { " y�",3," ZE" } , { " ye",3," Ze" } ,{ " yi",3," Zi" } ,
{ " yo",3," Zo" } , { " y�",3," ZO" } , { " yu",3," Zu" } ,
{ " ",1," " } , {0,0,0}    };
/* para os casos nos que o grupo e principio de palabra xa se sustitue
	 o espacio o grupo nm ou ps co espacio,*/

const t_regla cast_regla_cadeas_iniciadas_por_espacio[]={
{ " sp",2," es" } ,{ " sq",2," es" } ,{ " st",2," es" } ,{ " sl",2," es" } , 
{ " r",2," rr" } , { " ps",3," s" } , { " mn",3," n" } ,
/* se unha palabra comenza por y seguida de consonante enton a funcioon do
	 y e como vocal ainda que vaia no principio da palabra. Aparecer� en
	 palabras como Yglesias, Ysabel.*/
{ " ya",3," Za" } , { " y�",3," ZE" } , { " ye",3," Ze" } ,{ " yi",3," Zi" } ,
{ " yo",3," Zo" } , { " y�",3," ZO" } , { " yu",3," Zu" } ,
{ " x",2," S" } , { " ",1," " } , {0,0,0}    };
/* para os casos nos que o grupo e principio de palabra xa se sustitue
	 o espacio o grupo nm ou ps co espacio,*/

const t_regla regla_cadeas_iniciadas_por_acento_prosodico[]={

{ "^",1,"^" } , {0,0,0}   };

const t_regla regla_cadeas_iniciadas_por_p[]={

{ "p",1,"p" } , {0,0,0}     };

#ifdef _CONSIDERA_SEMIVOCALES

const t_regla regla_cadeas_iniciadas_por_q[]={
  {"que^i", 5, "ke^j"}, {"quei", 4, "kej"},
  {"qu�^i", 5, "kE^j"}, {"qu�i", 4, "kEj"},
  {"que^u", 5, "ke^w"}, {"queu", 4, "kew"},
  {"qu�^u", 5, "kE^w"}, {"qu�u", 4, "kEw"},
  {"quia^", 5, "kja^"}, {"quia", 4, "kja"},
  {"quie^", 5, "kje^"}, {"quie", 4, "kje"},
  {"qui�^", 5, "kjE^"}, {"qui�", 4, "kjE"},
  {"quio^", 5, "kjo^"}, {"quio", 4, "kjo"},
  {"qui�^", 5, "kjO^"}, {"qui�", 4, "kjO"},
  { "que",3,"ke" } , { "qu�",3,"kE" } , { "qui",3,"ki" } , { "q",1,"k" } , {0,0,0}  };
#else
const t_regla regla_cadeas_iniciadas_por_q[]={
  
  { "que",3,"ke" } , { "qu�",3,"kE" } , { "qui",3,"ki" } , { "q",1,"k" } , {0,0,0}  };
#endif

const t_regla regla_cadeas_iniciadas_por_h[]={

{ "h",1,"" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_t[]={

{ "t",1,"t" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_k[]={

{ "k",1,"k" } , {0,0,0}     };

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_c[]={
  {"cia^i", 5, "Tja^j"}, {"ciai", 4, "Tjaj"},
  {"cia^u", 5, "Tja^w"}, {"ciau", 4, "Tjaw"},
  {"cie^i", 5, "Tje^j"}, {"ciei", 4, "Tjej"},
  {"ci�^i", 5, "TjE^j"}, {"ci�i", 4, "TjEj"},
  {"cio^u", 5, "Tjo^w"}, {"ciou", 4, "Tjow"},
  {"ci�^u", 5, "TjO^w"}, {"ci�u", 4, "TjOw"},
  { "ce^i", 4, "Te^j"}, { "cei", 3, "Tej"},
  { "c�^i", 4, "TE^j"}, { "c�i", 3, "TEj"},
  { "ce^u", 4, "Te^w"}, { "ceu", 3, "Tew"},
  { "cia^", 4, "Tja^"}, { "cia", 3, "Tja"},
  { "cie^", 4, "Tje^"}, { "cie", 3, "Tje"},  
  { "ci�^", 4, "TjE^"}, { "ci�", 3, "TjE"},  
  { "cio^", 4, "Tjo^"}, { "cio", 3, "Tjo"},
  { "ci�^", 4, "TjO^"}, { "ci�", 3, "TjO"},  
  { "ci^u", 4, "Ti^w"},
  { "ch",2,"tS" } , { "ce",2,"Te" } ,  { "c�",2,"TE" } ,{ "ci",2,"Ti" } ,
  { "c�",2,"Ti" } , { "c",1,"k" } , {0,0,0}    };
#else
const t_regla regla_cadeas_iniciadas_por_c[]={
  { "ch",2,"tS" } , { "ce",2,"Te" } ,  { "c�",2,"TE" } ,{ "ci",2,"Ti" } ,
  { "c�",2,"Ti" } , { "c",1,"k" } , {0,0,0}    };
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_g[]={
  { "gui^u", 5, "Gi^w"},
  { "gu�i", 4, "GEj"}, { "guei", 4, "Gej"},
  { "gu�u", 4, "GEw"}, { "gueu", 4, "Gew"},
  { "guia", 4, "Gja"},
  { "guie", 4, "Gje"}, { "gui�", 4, "Gje"},
  { "guio^", 5, "Gjo^"}, { "gui�^", 5, "GjO^"},
  { "guio", 4, "Gjo"}, { "gui�", 4, "GjO"},
  { "gei", 3, "xej"}, { "g�i", 3, "xEj"},
  { "geu", 3, "xew"}, { "g�u", 3, "xEw"},
  { "gia", 3, "xja"},
  { "gie", 3, "xje"}, { "gi�", 3, "xjE"},
  { "gio", 3, "xjo"}, { "gi�", 3, "xjO"},
  { "giu", 3, "xju"},
  { "gu�",3,"GE" } , { "gue",3,"Ge" } , { "gui",3,"Gi" } , { "gu�",3,"Gi" } ,
  { "ge",2,"xe" }, { "g�",2,"xE" } , { "gi",2,"xi" }, {"gh", 2, "x"} , { "g",1,"G" } , {0,0,0}     };

#else
const t_regla regla_cadeas_iniciadas_por_g[]={
  { "gu�",3,"GE" } , { "gue",3,"Ge" } , { "gui",3,"Gi" } , { "gu�",3,"Gi" } ,
  { "ge",2,"xe" }, { "g�",2,"xE" } , { "gi",2,"xi" }, {"gh", 2, "x"} , { "g",1,"G" } , {0,0,0}     };
/* se non leva antes vocal sona g pequena. O acabar en vocal o antecedente
	 e ser necesario en alguns casos como cando despois de vocal ven o d ou
	 b,v etc, non hai que sustitui-la vocal, senon no caso seguinte*/
#endif

const t_regla regla_cadeas_iniciadas_por_l[]={

{"l-d",3,"l-d"} , {"l d",3,"l d"} , {"ld",2,"ld"} ,
{"l-r",3,"l-rr" } , { "ll",2,"Z" } , { "l",1,"l" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_r[]={

{ "rr",2,"rr" } , { "r",1,"r" } , {0,0,0}     };

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_n[]={
/* n+(b,p)-> m*/
{"n p",3,"m p"} , {"n b",3,"m b"} , {"n v",3,"m b"} ,
{"n-p",3,"m-p"} , {"n-b",3,"m-b"} , {"n-v",3,"m-b"} ,
{"np",2,"mp"} , {"nb",2,"mb"} , {"nv",2,"mb"} ,

{"n-d",3,"n-d"} , {"n d",3,"n d"} , {"nd",2,"nd"} ,

// FRAN_CAMPILLO // Cambi� la regla {"n i)", 2, "N "} por "n i"...
/* 'n' final de palabra mais vocal empezando a seguinte*/
{"n a^i", 5, "N a^j"}, {"n ai", 4, "N aj"},
{"n a^u", 5, "N a^w"}, {"n au", 4, "N aw"},
{"n e^i", 5, "N e^j"}, {"n ei", 4, "N ej"},
{"n �^i", 5, "N E^j"}, {"n �i", 4, "N Ej"},
{"n o^i", 5, "N o^j"}, {"n oi", 4, "N oj"},
{"n �^i", 5, "N O^j"}, {"n �i", 4, "N Oj"},
{"n o^u", 5, "N o^w"}, {"n ou", 4, "N ow"},
{"n �^u", 5, "N O^w"}, {"n �u", 4, "N Ow"},
{"n ia^", 5, "N ja^"}, {"n ia", 4, "N ja"},
{"n ie^", 5, "N je^"}, {"n ie", 4, "N je"},
{"n i�^", 5, "N jE^"}, {"n i�", 4, "N jE"},
{"n io^", 5, "N jo^"}, {"n io", 4, "N jo"},
{"n i�^", 5, "N jO^"}, {"n iO", 4, "N jO"},
{"n iu^", 5, "N ju^"}, {"n iu", 4, "N ju"},
{"n ua^", 5, "N wa^"}, {"n ua", 4, "N wa"},
{"n ue^", 5, "N we^"}, {"n ue", 4, "N we"},
{"n u�^", 5, "N wE^"}, {"n u�", 4, "N wE"},
{"n uo^", 5, "N wo^"}, {"n uo", 4, "N wo"},
{"n u�^", 5, "N wO^"}, {"n u�", 4, "N wO"},
{"n ui^", 5, "N wi^"}, {"n ui", 4, "N wi"},
{ "n a",3,"N a" } , { "n e",3,"N e" } , { "n �",3,"N E" } ,
{ "n i",3,"N i" } , { "n o",3,"N o" } , { "n �",3,"N O" } ,{ "n u",3,"N u" } ,
// FRAN_CAMPILLO
/* n mais consonante distinta de t,d,p,b,v.*/

{"n-c",2,"N-"} , {"n-d",3,"N-d"} , {"n-f",3,"N-f"},

{"n-gue",5,"N-ge"} , {"n-gu�",5,"N-gE"} , {"n-gui",5,"N-gi"} , {"n-ge",4,"N-xe"} ,
{"n-g�",4,"N-xE"} ,{"n-gi",4,"N-xi"} , {"n-gu�",5,"N-gi"} , {"n-g",3,"N-g"} ,

{"n-j",3,"N-x"} , {"n-k",3,"N-k"} , {"n-l",2,"N-"} , {"n-m",2,"N-"} , {"n-n",2,"N-"} ,
{"n-q",2,"N-"} , {"n-r",3,"N-rr"} , {"n-s",2,"N-"} , 
//prueba para Montse {"n-t",3,"n_t-t"}, 
{"n-w",3,"N-w"} , {"n-x",2,"N-"} , {"n-z",3,"N-T"} , {"n-�",3,"N-s"} ,

/* o mesmo ca antes pero pertencendo a d�as palabras */

{"n c",2,"N "} , {"n d",3,"N d"} , {"n f",2,"N "} ,

{"n gue",5,"N ge"} , {"n gu�",5,"N gE"} , {"n gui",5,"N gi"} , {"n ge",4,"N xe"} ,
{"n gi",4,"N xi"} ,{"n gu�",5,"N gi"}  ,{"n gh",4,"N x"},{"n g",3,"N g"}  ,


{"n j",3,"N x"} ,
{"n k",3,"N k"} , {"n l",2,"N "} , {"n m",2,"N "} , {"n n",2,"N "} ,
{"n q",2,"N "} , {"n r",3,"N rr"} , {"n s",2,"N "} , {"n w",2,"N "} ,
{"n x",2,"N "} , {"n z",3,"N T"} , {"n �",3,"N s"} ,

/* No caso hipot�tico de que vaian na mesma silaba. Non se producira en palabras
	 galegas.*/

{"nc",1,"N"},{"nd",2,"Nd"},{"nf",1,"N"},{"ng",2,"Ng"},{"nj",1,"N"},
{"nk",1,"N"},{"nl",1,"N"},{"nm",1,"N"},{"nn",1,"N"},
{"nq",1,"N"},{"nr",2,"Nrr"},{"ns-rr",5,"Ns-rr"},{"ns-r",4,"Ns-rr"},/*o anterior aparece en transrexional*/{"ns",1,"N"},{"nw",1,"N"},
{"nx",1,"N"},{"nz",2,"NT"},{"n�",2,"Ns"},

{"n-h",3,"N-" } , { "n h",3,"N " } ,

/* n + silencio => N */

{"n #",3,"N #" } , /*Agora as seguintes creo que sobran*/ {"n .",1,"N" } , {"n ,",1,"N" } , {"n ;",1,"N" } ,{"n !",1,"N" } ,{"n ?",1,"N" } ,{"n \"",1,"N" } ,{"n �",1,"N" } ,{"n /",1,"N" } ,{"n :",1,"N" } ,{"n (",1,"N" } ,{"n )",1,"N" } , {"n -",1,"N" } ,{"n �",1,"N" } , /* O caracter '�' (punto fino a media altura) � o que representa os puntos suspensivos.*/

/* caso por defecto*/

{"n",1,"n"} , {"F",0,"" }    };

#else
const t_regla regla_cadeas_iniciadas_por_n[]={
/* n+(b,p)-> m*/
{"n p",3,"m p"} , {"n b",3,"m b"} , {"n v",3,"m b"} ,
{"n-p",3,"m-p"} , {"n-b",3,"m-b"} , {"n-v",3,"m-b"} ,
{"np",2,"mp"} , {"nb",2,"mb"} , {"nv",2,"mb"} ,

{"n-d",3,"n-d"} , {"n d",3,"n d"} , {"nd",2,"nd"} ,

// FRAN_CAMPILLO // Cambi� la regla {"n i)", 2, "N "} por "n i"...
/* 'n' final de palabra mais vocal empezando a seguinte*/
{ "n a",3,"N a" } , { "n e",3,"N e" } , { "n �",3,"N E" } ,
{ "n i",3,"N i" } , { "n o",3,"N o" } , { "n �",3,"N O" } ,{ "n u",3,"N u" } ,
// FRAN_CAMPILLO
/* n mais consonante distinta de t,d,p,b,v.*/

{"n-c",2,"N-"} , {"n-d",3,"N-d"} , {"n-f",3,"N-f"},

{"n-gue",5,"N-ge"} , {"n-gu�",5,"N-gE"} , {"n-gui",5,"N-gi"} , {"n-ge",4,"N-xe"} ,
{"n-g�",4,"N-xE"} ,{"n-gi",4,"N-xi"} , {"n-gu�",5,"N-gi"} , {"n-g",3,"N-g"} ,

{"n-j",3,"N-x"} , {"n-k",3,"N-k"} , {"n-l",2,"N-"} , {"n-m",2,"N-"} , {"n-n",2,"N-"} ,
{"n-q",2,"N-"} , {"n-r",3,"N-rr"} , {"n-s",2,"N-"} , {"n-w",3,"N-w"} ,
{"n-x",2,"N-"} , {"n-z",3,"N-T"} , {"n-�",3,"N-s"} ,

/* o mesmo ca antes pero pertencendo a d�as palabras */

{"n c",2,"N "} , {"n d",3,"N d"} , {"n f",2,"N "} ,

{"n gue",5,"N ge"} , {"n gu�",5,"N gE"} , {"n gui",5,"N gi"} , {"n ge",4,"N xe"} ,
{"n gi",4,"N xi"} ,{"n gu�",5,"N gi"}  ,{"n gh",4,"N x"},{"n g",3,"N g"}  ,


{"n j",3,"N x"} ,
{"n k",3,"N k"} , {"n l",2,"N "} , {"n m",2,"N "} , {"n n",2,"N "} ,
{"n q",2,"N "} , {"n r",3,"N rr"} , {"n s",2,"N "} , {"n w",2,"N "} ,
{"n x",2,"N "} , {"n z",3,"N T"} , {"n �",3,"N s"} ,

/* No caso hipot�tico de que vaian na mesma silaba. Non se producira en palabras
	 galegas.*/

{"nc",1,"N"},{"nd",2,"Nd"},{"nf",1,"N"},{"ng",2,"Ng"},{"nj",1,"N"},
{"nk",1,"N"},{"nl",1,"N"},{"nm",1,"N"},{"nn",1,"N"},
{"nq",1,"N"},{"nr",2,"Nrr"},{"ns-rr",5,"Ns-rr"},{"ns-r",4,"Ns-rr"},/*o anterior aparece en transrexional*/{"ns",1,"N"},{"nw",1,"N"},
{"nx",1,"N"},{"nz",2,"NT"},{"n�",2,"Ns"},

{"n-h",3,"N-" } , { "n h",3,"N " } ,

/* n + silencio => N */

{"n #",3,"N #" } , /*Agora as seguintes creo que sobran*/ {"n .",1,"N" } , {"n ,",1,"N" } , {"n ;",1,"N" } ,{"n !",1,"N" } ,{"n ?",1,"N" } ,{"n \"",1,"N" } ,{"n �",1,"N" } ,{"n /",1,"N" } ,{"n :",1,"N" } ,{"n (",1,"N" } ,{"n )",1,"N" } , {"n -",1,"N" } ,{"n �",1,"N" } , /* O caracter '�' (punto fino a media altura) � o que representa os puntos suspensivos.*/

/* caso por defecto*/

{"n",1,"n"} , {"F",0,"" }    };

#endif

/*
const t_regla cast_regla_cadeas_iniciadas_por_n[]={
// n+(b,p)-> m
{"n p",3,"m p"} , {"n b",3,"m b"} , {"n v",3,"m b"} ,
{"n-p",3,"m-p"} , {"n-b",3,"m-b"} , {"n-v",3,"m-b"} ,
{"np",2,"mp"} , {"nb",2,"mb"} , {"nv",2,"mb"} ,

{"n-d",3,"n-d"} , {"n d",3,"n d"} , {"nd",2,"nd"} ,

// 'n' final de palabra mais vocal empezando a seguinte
{ "n a",2,"n " } , { "n e",2,"n " } , { "n �",2,"n " } ,
{ "n i",2,"n " } , { "n o",2,"n " } , { "n �",2,"n " } ,{ "n u",2,"n " } ,

// n mais consonante distinta de t,d,p,b,v.

{"n-c",2,"N-"} , {"n-d",3,"N-d"} , {"n-f",2,"N-"},

{"n-gue",4,"N-g"} , {"n-gu�",4,"N-g"} , {"n-gui",4,"N-g"} , {"n-ge",3,"N-x"} ,
{"n-g�",3,"N-x"} ,{"n-gi",3,"N-x"} , {"n-gu�",4,"N-g"} , {"n-g",3,"N-g"} ,

{"n-j",2,"N-"} , {"n-k",2,"N-"} , {"n-l",2,"N-"} , {"n-m",2,"N-"} , {"n-n",2,"N-"} ,
{"n-q",2,"N-"} , {"n-r",3,"N-rr"} , {"n-s",2,"N-"} , {"n-w",2,"N-"} ,
{"n-x",2,"N-"} , {"n-z",2,"N-"} , {"n-�",2,"N-"} ,

// o mesmo ca antes pero pertencendo a d�as palabras

{"n c",2,"N "} , {"n d",3,"N d"} , {"n f",2,"N "} ,

{"n gue",4,"N g"} , {"n gu�",4,"N g"} , {"n gui",4,"N g"} , {"n ge",3,"N x"} ,
{"n gi",3,"N x"} ,{"n gu�",4,"N g"} , {"n g",3,"N g"} ,


{"n j",2,"N "} ,
{"n k",2,"N "} , {"n l",2,"N "} , {"n m",2,"N "} , {"n n",2,"N "} ,
{"n q",2,"N "} , {"n r",3,"N rr"} , {"n s",2,"N "} , {"n w",2,"N "} ,
{"n x",2,"N "} , {"n z",2,"N "} , {"n �",2,"N "} ,

// No caso hipot�tico de que vaian na mesma silaba. Non se producira en palabras
	 galegas.

{"nc",1,"N"},{"nd",2,"Nd"},{"nf",1,"N"},{"ng",1,"N"},{"nj",1,"N"},
{"nk",1,"N"},{"nl",1,"N"},{"nm",1,"N"},{"nn",1,"N"},
{"nq",1,"N"},{"nr",2,"Nrr"},{"ns-rr",5,"Ns-rr"},{"ns-r",4,"Ns-rr"}, //o anterior aparece en transrexional
{"ns",1,"N"},{"nw",1,"N"},
{"nx",1,"N"},{"nz",1,"N"},{"n�",1,"N"},

{"n-h",3,"N-" } , { "n h",3,"n " } ,

// n + silencio => N

{"n #",3,"N #" } , 

// caso por defecto

{"n",1,"n"} , {"F",0,"" }    };

*/

//�OJO! giglesia.
// Las n velares en castellano solo se aceptan con n-g, n-k, n-x

const t_regla cast_regla_cadeas_iniciadas_por_n[]={
// n+(b,p)-> m
{"n p",3,"m p"} , {"n b",3,"m b"} , {"n v",3,"m b"} ,
{"n-p",3,"m-p"} , {"n-b",3,"m-b"} , {"n-v",3,"m-b"} ,
{"np",2,"mp"} , {"nb",2,"mb"} , {"nv",2,"mb"} ,

{"n-d",3,"n-d"} , {"n d",3,"n d"} , {"nd",2,"nd"} ,

// 'n' final de palabra mais vocal empezando a seguinte
{ "n a",2,"n " } , { "n e",2,"n " } , { "n �",2,"n " } ,
{ "n i)",2,"n " } , { "n o",2,"n " } , { "n �",2,"n " } ,{ "n u",2,"n " } ,

// n mais consonante distinta de t,d,p,b,v.

// giglesia: No vale cualquier n -c, s�lo las c's fuertes.
//{"n-c",2,"N-"} ,
{"n-ca",2,"N-"} ,
{"n-co",2,"N-"} ,
{"n-cu",2,"N-"} ,

//giglesia: este caso hay que explicitarlo aunque la n siga siendo n...
{"n-r",3,"n-rr"},
{"n r",3,"n rr"},
{"nr",2,"nrr"},

{"n-gue",4,"N-g"} , {"n-gu�",4,"N-g"} , {"n-gui",4,"N-g"} , {"n-ge",3,"N-x"} ,
{"n-g�",3,"N-x"} ,{"n-gi",3,"N-x"} , {"n-gu�",4,"N-g"} , {"n-g",3,"N-g"} ,

{"n-j",2,"N-"} , {"n-k",2,"N-"} , 
{"n-q",2,"N-"} ,
{"n-x",2,"N-"} ,

// o mesmo ca antes pero pertencendo a d�as palabras

{"n ca",2,"N "} ,
{"n co",2,"N "} ,
{"n cu",2,"N "} ,


{"n gue",4,"N g"} , {"n gu�",4,"N g"} , {"n gui",4,"N g"} , {"n ge",3,"N x"} ,
{"n gi",3,"N x"} ,{"n gu�",4,"N g"} , {"n g",3,"N g"} ,


{"n j",2,"N "} ,
{"n k",2,"N "} ,
{"n q",2,"N "} ,
{"n x",2,"N "} ,

// No caso hipot�tico de que vaian na mesma silaba. Non se producira en palabras	 galegas.

{"nc",1,"N"},
{"ng",1,"N"},
{"nj",1,"N"},
{"nk",1,"N"},
{"nq",1,"N"},
{"nx",1,"N"},

// caso por defecto

{"n",1,"n"} , {"F",0,"" }    };


const t_regla regla_cadeas_iniciadas_por_m[]={

{"m-b",3,"m-b"} , {"m-v",3,"m-b"} , {"m-d",3,"m-d"} ,
{"m b",3,"m b"} , {"m v",3,"m b"} , {"m d",3,"m d"} ,
{"mb",2,"mb"} ,  {"mv",2,"mb"} ,{"md",2,"md"} ,

{"m-gue",5,"m-ge"} , {"m-gu�",5,"m-gE"} , {"m-gui",5,"m-gi"} ,
{"m-gu�",5,"m-g"} , {"m-g",3,"m-g"} ,
{"m gue",5,"m ge"} , {"m gu�",5,"m gE"} , {"m gui",5,"m gi"} ,
{"m gu�",5,"m g"} , {"m g",3,"m g"} ,

{ "m",1,"m" } , {0,0,0}    };


const t_regla regla_cadeas_iniciadas_por_j[]={

{ "j",1,"x" } , {0,0,0}    };
/* para palabras que provenhan do castelan*/

const t_regla regla_cadeas_iniciadas_por_s[]={

{"s-r",3,"s-rr"},{"s",1,"s" } , {0,0,0}    };

const t_regla regla_cadeas_iniciadas_por_x[]={

{ "x-",1,"ks" } , { "x",1,"S" } , {0,0,0}    };
/* Seguida de vocal sona como /xe/ senon como ks. As excepcions (casos nos
   que x+vocal sona como  ks, como por exemplo sexo) deben incluirse ou ben
   enteiras ou un grupo ou entorno que as distinga de outros casos. Isto non
   lle afecta o castelan, posto que tamen se pronunciaran igual.              */
const t_regla cast_regla_cadeas_iniciadas_por_x[]={
{ "x-",1,"ks" } ,{ "x",1,"ks" } , {0,0,0}    };
const t_regla regla_cadeas_iniciadas_por_b[]={
{ "b",1,"B" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_d[]={
{ "d",1,"D" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_f[]={
{ "f",1,"f" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_enne[]={
{ "�",1,"J" } , {0,0,0}    };
const t_regla regla_cadeas_iniciadas_por_w[]={
{ "w",1,"B" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_z[]={
{ "z",1,"T" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_cedilla[]={
{"�",1,"s" }, {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_v[]={
{ "v",1,"B" } , {0,0,0}     };
const t_regla regla_cadeas_iniciadas_por_vocal_minuscula_rara[]={
{ "�",1,"a" } ,{ "�",1,"a" } ,{ "�",1,"a" } , 
{ "�",1,"e" } ,{ "�",1,"e" } , 
{ "�",1,"i" } ,{ "�",1,"i" } , 
{ "�",1,"o" } ,{ "�",1,"o" } ,{ "�",1,"o" } , 
{ "�",1,"u" } ,{ "�",1,"u" } , 
{0,0,0}     };
/*
 *const t_regla regla_cadeas_iniciadas_por_vocal_maiuscula_rara[]={
 *{ "�",1,"a" } ,{ "�",1,"a" } ,{ "�",1,"a" } , 
 *{ "�",1,"e" } ,{ "�",1,"e" } ,{ "�",1,"e" } , 
 *{ "�",1,"i" } ,{ "�",1,"i" } ,{ "�",1,"i" } , 
 *{ "�",1,"o" } ,{ "�",1,"o" } ,{ "�",1,"o" } , 
 *{ "�",1,"u" } ,{ "�",1,"u" } ,{ "�",1,"u" } , 
 *{0,0,0}     };
 */

/********************SIGNOS*******************/

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla regla_cadeas_iniciadas_por_silencio[]={
  { "# d" , 3 , "# d" } ,
  { "# v" , 3 , "# b" } ,
  { "# b" , 3 , "# b" } ,
  { "# pn",4,"# n" } ,
  { "# gu�i", 6, "# gEj"}, { "# guei", 6, "# gej"},
  { "# gu�u", 6, "# gEw"}, { "# guew", 6, "# gew"},
  { "# guia", 6, "# gja"},
  { "# guie", 6, "# gje"}, { "# gui�", 6, "# gjE"},
  { "# guio", 6, "# gjo"}, { "# gui�", 6, "# gjO"},
  { "# gu�",5,"# gE" } , { "# gue",5,"# ge" } , { "# gui",5,"# gi" } , { "# gu�",5,"# gi" } ,
  { "# gei", 5, "# xej"}, { "# g�i", 5, "# xEj"},
  { "# geu", 5, "# xew"}, { "# g�u", 5, "# xEw"},
  { "# gia", 5, "# xja"},
  { "# gie", 5, "# xje"}, { "# gi�", 5, "# xjE"},
  { "# gio", 5, "# xjo"}, { "# gi�", 5, "# xjO"},
  { "# giu", 5, "# xju"},
  { "# ge",4,"# xe" } , { "# g�",4,"# xE" } , { "# gi",4,"# xi" } , { "# g�",4,"# xi" } ,
  { "# gh",4,"# x" } ,
  { "# g",3,"# g" } ,
  { "# r",3,"# rr" } ,
  //fmendez{ "# ",2,"# " } ,
  { "#",1,"#" } ,
  {0,0,0}
};
#else
const t_regla regla_cadeas_iniciadas_por_silencio[]={
  { "# d" , 3 , "# d" } ,
  { "# v" , 3 , "# b" } ,
  { "# b" , 3 , "# b" } ,
  { "# pn",4,"# n" } ,
  { "# gu�",5,"# gE" } , { "# gue",5,"# ge" } , { "# gui",5,"# gi" } , { "# gu�",5,"# gi" } ,
  { "# ge",4,"# xe" } , { "# g�",4,"# xE" } , { "# gi",4,"# xi" } , { "# g�",4,"# xi" } ,
  { "# gh",4,"# x" } ,
  { "# g",3,"# g" } ,
  { "# r",3,"# rr" } ,
  //fmendez{ "# ",2,"# " } ,
  { "#",1,"#" } ,
  {0,0,0}
};
#endif

#ifdef _CONSIDERA_SEMIVOCALES
const t_regla cast_regla_cadeas_iniciadas_por_silencio[]={
  { "# d" , 3 , "# d" } ,
  { "# v" , 3 , "# b" } ,
  { "# b" , 3 , "# b" } ,
  { "# gu�i", 6, "# gEj"}, { "# guei", 6, "# gej"},
  { "# gu�u", 6, "# gEw"}, { "# guew", 6, "# gew"},
  { "# guia", 6, "# gja"},
  { "# guie", 6, "# gje"}, { "# gui�", 6, "# gjE"},
  { "# guio", 6, "# gjo"}, { "# gui�", 6, "# gjO"},
  { "# gu�",5,"# gE" } , { "# gue",5,"# ge" } , { "# gui",5,"# gi" } , { "# gu�",5,"# gi" } ,
  { "# gei", 5, "# xej"}, { "# g�i", 5, "# xEj"},
  { "# geu", 5, "# xew"}, { "# g�u", 5, "# xEw"},
  { "# gia", 5, "# xja"},
  { "# gie", 5, "# xje"}, { "# gi�", 5, "# xjE"},
  { "# gio", 5, "# xjo"}, { "# gi�", 5, "# xjO"},
  { "# giu", 5, "# xju"},
  { "# gu�",4,"# g" } , { "# gue",4,"# g" } , { "# gui",4,"# g" } , { "# gu�",4,"# g" } ,
  { "# ge",4,"# xe" } ,
  { "# gi",4,"# xi" } ,
  { "# g",3,"# g" } ,
  { "# r",3,"# rr" } ,
  { "# x",3,"# S" } ,
  //{ "# y ",4,"# i" } ,
  //{ "# y",3,"# Z" } ,
  //fmendez{ "# ",2,"# " } ,
  { "#",1,"#" } ,
  {0,0,0}
};
#else
const t_regla cast_regla_cadeas_iniciadas_por_silencio[]={
  { "# d" , 3 , "# d" } ,
  { "# v" , 3 , "# b" } ,
  { "# b" , 3 , "# b" } ,
  { "# gu�",4,"# g" } , { "# gue",4,"# g" } , { "# gui",4,"# g" } , { "# gu�",4,"# g" } ,
  { "# ge",4,"# xe" } ,
  { "# gi",4,"# xi" } ,
  { "# g",3,"# g" } ,
  { "# r",3,"# rr" } ,
  { "# x",3,"# S" } ,
  //{ "# y ",4,"# i" } ,
  //{ "# y",3,"# Z" } ,
  //fmendez{ "# ",2,"# " } ,
  { "#",1,"#" } ,
  {0,0,0}
};
#endif

const t_regla regla_cadeas_iniciadas_por_guion[]={

{ "-- d",4,"--  d" } ,{ "-- v",4,"--  b" } ,{ "-- b",4,"--  b" } ,{ "-- gu�",5,"-- g" } ,{ "-- gue",5,"-- g" } ,{ "-- gui",5,"-- g" } ,{ "-- gu�",5,"-- g" } ,{ "-- g",4,"-- g" } ,{ "--",2,"--" } ,
{ "- d",3,"- d" } ,{ "- v",3,"- b" } ,{ "- b",3,"- b" } ,{ "- gu�",4,"- g" } ,{ "- gue",4,"- g" } ,{ "- gui",4,"- g" } ,{ "- gu�",4,"- g" } ,{ "- g",3,"- g" } ,
//{ "-psi",3,"-s" } , //parapesicologo
{ "-",1,"-" } ,
{0,0,0}
};

const t_regla cast_regla_cadeas_iniciadas_por_guion[]={

{ "-- d",4,"--  d" } ,{ "-- v",4,"--  b" } ,{ "-- b",4,"--  b" } ,{ "-- gu�",5,"-- g" } ,{ "-- gue",5,"-- g" } ,{ "-- gui",5,"-- g" } ,{ "-- gu�",5,"-- g" } ,{ "-- g",4,"-- g" } ,{ "--",2,"--" } ,
{ "- d",3,"- d" } ,{ "- v",3,"- b" } ,{ "- b",3,"- b" } ,{ "- gu�",4,"- g" } ,{ "- gue",4,"- g" } ,{ "- gui",4,"- g" } ,{ "- gu�",4,"- g" } ,{ "- g",3,"- g" } ,
{ "-y",2,"-Z" } , { "-",1,"-" } ,
{0,0,0}
};
const t_regla regla_cadeas_iniciadas_por_punto[]={

{ ". d",3,". d" } ,{ ". v",3,". b" } ,{ ". b",3,". b" } ,{ ". gu�",4,". g" } ,{ ". gue",4,". g" } ,{ ". gui",4,". g" } ,{ ". gu�",4,". g" } ,{ ". g",3,". g" } ,  { ".",1,"." },
{0,0,0}
};

const t_regla regla_cadeas_iniciadas_por_coma[]={

{ ", d",3,", d" } ,{ ", v",3,", b" } ,{ ", b",3,", b" } ,{ ", gu�",4,", g" } ,{ ", gue",4,", g" } ,{ ", gui",4,", g" } ,{ ", gu�",4,", g" } ,{ ", g",3,", g" } ,
{ ",",1,"," } ,
{0,0,0}
};

const t_regla regla_cadeas_iniciadas_por_punt_e_coma[]={

{ "; d",3,"; d" } ,{ "; v",3,"; b" } ,{ "; b",3,"; b" } ,{ "; gu�",4,"; g" } ,{ "; gue",4,"; g" } ,{ "; gui",4,"; g" } ,{ "; gu�",4,"; g" } ,{ "; g",3,"; g" } ,
{ ";",1,";" } ,
{0,0,0}

};
const t_regla regla_cadeas_iniciadas_por_apertura_interrogacion[]={

{ "� d",3,"� d" } ,{ "� v",3,"� b" } ,{ "� b",3,"� b" } ,{ "� gu�",4,"� g" } ,{ "� gue",4,"� g" } ,{ "� gui",4,"� g" } ,{ "� gu�",4,"� g" } ,{ "� g",3,"� g" } ,
{ "�",1,"�" } ,
{0,0,0}   };

const t_regla regla_cadeas_iniciadas_por_peche_interrogacion[]={

{ "? d",3,"? d" } ,{ "? v",3,"? b" } ,{ "? b",3,"? b" } ,{ "? gu�",4,"? g" } ,{ "? gue",4,"? g" } ,{ "? gui",4,"? g" } ,{ "? gu�",4,"? g" } ,{ "? g",3,"? g" } ,
{ "?",1,"?" } ,
{0,0,0}   };


const t_regla regla_cadeas_iniciadas_por_comillas_dobles[]={
{ "\" d",3,"\" d" } ,{ "\" v",3,"\" b" } ,{ "\" b",3,"\" b" } ,{ "\" gu�",4,"\" g" } ,{ "\" gue",4,"\" g" } ,{ "\" gui",4,"\" g" } ,{ "\" gu�",4,"\" g" } ,{ "\" g",3,"\" g" } ,
{ "\"",1,"\"" } ,
{0,0,0}
};

const t_regla regla_cadeas_iniciadas_por_exclamacion_apertura[]={

{ "� d",3,"� d" } ,{ "� v",3,"� b" } ,{ "� b",3,"� b" } ,{ "� gu�",4,"� g" } ,{ "� gue",4,"� g" } ,{ "� gui",4,"� g" } ,{ "� gu�",4,"� g" } ,{ "� g",3,"� g" } ,  { "�",1,"�" } ,
{0,0,0}
};

const t_regla regla_cadeas_iniciadas_por_pech_exclamacion[]={
{ "! d",3,"! d" } ,{ "! v",3,"! b" } ,{ "! b",3,"! b" } ,{ "! gu�",4,"! g" } ,{ "! gue",4,"! g" } ,{ "! gui",4,"! g" } ,{ "! gu�",4,"! g" } ,{ "! g",3,"! g" } ,
{ "!",1,"!" } ,
{0,0,0} };

const t_regla regla_cadeas_iniciadas_por__apertura_parentese[]={

{ "( d",3,"( d" } ,{ "( v",3,"( b" } ,{ "( b",3,"( b" } ,{ "( gu�",4,"( g" } ,{ "( gue",4,"( g" } ,{ "( gui",4,"( g" } ,{ "( gu�",4,"( g" } ,{ "( g",3,"( g" } ,
{ "(",1,"(" } ,
{0,0,0} };

/*{ "�",1,"�" } , non creo que xa se empregue*/
const t_regla regla_cadeas_iniciadas_po_peche_parentese[]={

{ ") d",3,") d" } ,{ ") v",3,") b" } ,{ ") b",3,") b" } ,{ ") gu�",4,") g" } ,{ ") gue",4,") g" } ,{ ") gui",4,") g" } ,{ ") gu�",4,") g" } ,{ ") g",3,") g" } ,
{")",1,")" } ,
{0,0,0} };


const t_regla regla_cadeas_iniciadas_por_simples_cominhas[]={
{ "' d",3,"' d" } ,{ "' v",3,"' b" } ,{ "' b",3,"' b" } ,{ "' gu�",4,"' g" } ,{ "' gue",4,"' g" } ,{ "' gui",4,"' g" } ,{ "' gu�",4,"' g" } ,{ "' g",3,"' g" } ,
{0,0,0}
};

const t_regla regla_cadeas_iniciadas_por_asterisco[]={
{ "* d",3,"* d" } ,{ "* v",3,"* b" } ,{ "* b",3,"* b" } ,{ "* gu�",4,"* g" } ,{ "* gue",4,"* g" } ,{ "* gui",4,"* g" } ,{ "* gu�",4,"* g" } ,{ "* g",3,"* g" } ,
{"*",1,"*" } ,
{0,0,0} };

const t_regla regla_cadeas_iniciadas_por_dous_puntos[]={

{ ": d",3,": d" } ,{ ": v",3,": b" } ,{ ": b",3,": b" } ,{ ": gu�",4,": g" } ,{ ": gue",4,": g" } ,{ ": gui",4,": g" } ,{ ": gu�",4,": g" } ,{ ": g",3,": g" } ,
{":",1,":" } ,
{0,0,0} };

const t_regla regla_cadeas_iniciadas_por_suspensivos[]={
{ "�",1,"�" },
{0,0,0} };

/****************************************************************/
 

