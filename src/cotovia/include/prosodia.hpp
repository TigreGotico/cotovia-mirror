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
 
#ifndef PROSODIA_H
   #define PROSODIA_H

#include "tip_var.hpp"



int alof_vocal_forte(char *c);
int alof_prosodia_vocalico(char *c);
int semivocal(t_prosodia * alofo);
int consoante_liquida_ou_nasal(char *c);
int consoante_xorda(char* c);
int consoante_sonora(char *c);
void adap_fon(char * ent,char *saida);
int obter_indice(char *alofo);

int vocal_inicial_de_grupo_entonativo(t_prosodia * f_prosodica,int i);



int onset_nucleo_coda(t_prosodia *f_prosodica);

unsigned char adapta_ti_prop_para_duracions(unsigned char in);

void preparar_datos_adicionais(t_prosodia *f_prosodica);

void posprocesar_prosodia(t_prosodia *f_prosodica);
void asignar_posicion_grupo_acentual_dentro_da_proposicion(t_prosodia *f_prosodica);
int empeza_grupo_prosodico_a_continuacion(char *f_fonetica);
void asignar_tipo_de_segmento_acentual(t_prosodia *f_prosodica, char *f_fonetica,int inicio_grupo_acentual,int fin_grupo_acentual,unsigned char *tipo_grupo_acentual);
void asignar_orde_silabica_inversa(t_prosodia *f_prosodica,int inicio_grupo_acentual,int fin_grupo_acentual,int pos_acento_grupo_acentual);
char *interpretar_espacio(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,int *inicio_grupo_acentual,int *fin_grupo_acentual,int *pos_acento_grupo_acentual,char *palabra_tonica,unsigned char * orde_silabica,
                          unsigned char *tipo_grupo_acentual);
void obter_alofono(char *f_fonetica,char *alofono);
char *interpretar_alofonos(char *f_fonetica,t_prosodia *f_prosodica,
                           int *pos_f_prosodica,unsigned char *orde_silabica,unsigned char tipo_proposicion,char *palabra_tonica,int *pos_grupo_acentual,unsigned char pos_grupo_prosodi);
char *interpretar_guion(char *f_fonetica,unsigned char *orde_silabica);
char * interpretar_pausa(char *f_fonetica,t_prosodia *f_prosodica,int *pos_f_prosodica,unsigned char num_grupo_prosodico,unsigned char tipo_proposicion);
char * interpretar_comentario(char *f_fonetica,unsigned char *tipo_prop,unsigned char *num_grupos_proso);
void encher_frase_prosodica(char *inicio_f_fonetica,t_prosodia *f_prosodica);
class Prosodia {
    private:

        FILE * fd;
        int bddur;
        char * fsalida;

        int encher_arquivo_de_prosodia(t_prosodia *f_prosodica);
        
        

    public:
        void xerar_prosodia(char *f_fonetica,t_prosodia *f_prosodica);
		void inicializa(t_opciones * opciones);
		void finaliza();
        unsigned char adaptar_grupo_prosodico(unsigned char codigo);        
};




#endif
