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
 
#ifndef _PAUSAS_HPP
  #define _PAUSAS_HPP

#define MAX_SILABAS_ENTRE_PAUSAS 30

#define MAX_SILABAS_ENTRE_PAUSAS_1 30
#define MAX_SILABAS_ENTRE_PAUSAS_2 40

#define DUR_PAUSA_DEMASIADAS_SILABAS 102
#define DUR_PAUSA_TEXTO   101


#define ANCHO_VENTANA	7
#define OFFSET 3

typedef enum {
    NULA_P,				//0
    ADVE_P,				//1
    ADXE_P,				//2
    APERT_EXCLA_P,		//3
    APERT_INTER_P,		//4
    APERT_PAREN_P,		//5
    COMA_P,				//6
    COMINNAS_P,			//7
    CONX_COOR_P,		//8
    CONX_SUBOR_P,		//9
    DET_NUME_P,			//10
    DET_P,				//11
    DET_POSE_P,			//12
    EXCLA_P,			//13
    GUION_P,			//14
    INTER_P,			//15
    INTERX_P,			//16
    NOME_P,				//17
    PECHE_EXCLA_P,		//18
    PECHE_INTER_P,		//19
    PECHE_PAREN_P,		//20
    PREP_P,				//21
    PRON_NUME_P,		//22
    PRON_P,				//23
    PRON_POSE_P,		//24
    PRON_PROC_P,		//25
    PUNTO_P,			//26
    RELA_P,				//27
    SIGNO_P,			//28
    VERBO_P,			//29
    XERUNDIO_P			//30
} Categorias_pausas ;

#define MAX_NUM_CATEGORIAS_PAUSAS_CONSIDERADAS 31

typedef struct {
   //char	pausa;
   t_frase_separada * pal;
   Categorias_pausas categorias[ANCHO_VENTANA];
   int	n_silabas_pausa_ant;
   int	n_silabas_pausa_pos;
}t_pausas;

int pon_pausa_sintagma(unsigned char tipo);
void poner_pausas(t_frase_sintagmada *frase_sintagmada,t_frase_separada *f_sep);
int pon_pausa_segun_tipo(unsigned char tipo);

int
#ifdef _WIN32
__declspec(dllexport) 
#endif
crea_frase_pausas(t_frase_separada *p_sep, t_frase_sintagmada *p_sintagmada);

Categorias_pausas traduce_a_categorias_pausas(unsigned char categoria);
void cierra_ficheros_pausas(void);
int pausador_arbol(t_pausas *datos);
#endif

