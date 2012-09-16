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
 
#ifndef SINTAGMA_H
#define SINTAGMA_H

#define adverbio(c)(c >= ADVE && c <=LOC_ADVE_DUBI )
#define es_contraccion(p) ( p==CONTR_CONX_ART_DET || \
           									p==CONTR_PREP_ART_DET || \
           									p==CONTR_INDEF_ART_DET || \
           									p==CONTR_PREP_ART_INDET || \
           									p==CONTR_PREP_INDEF || \
           									p==CONTR_PREP_INDEF_DET || \
           									p==CONTR_PREP_INDEF_PRON || \
           									p==CONTR_PREP_DEMO || \
           									p==CONTR_PREP_DEMO_DET || \
           									p==CONTR_PREP_DEMO_PRON || \
           									p==CONTR_DEMO_INDEF || \
           									p==CONTR_DEMO_INDEF_DET || \
           									p==CONTR_DEMO_INDEF_PRON || \
           									p==CONTR_PREP_DEMO_INDEF || \
           									p==CONTR_PREP_DEMO_INDEF_DET || \
           									p==CONTR_PREP_DEMO_INDEF_PRON || \
           									p==CONTR_PREP_PRON_PERS_TON || \
           									p==CONTR_PRON_PERS_AT_DAT_AC || \
           									p==CONTR_PRON_PERS_AT_DAT_DAT_AC || \
           									p==CONTR_PRON_PERS_AT || \
           									p==CONTR_CONX_COOR_COP_ART_DET)

int relativo_interrogativo_ou_exclamativo(t_frase_separada *recorredor);
//int comprobar_locucion_adversativa(t_frase_separada *frase_separada);
int locucion_prepositiva_mais_de(t_conxuncion *conxuncion_encontrada,t_frase_separada *frase_separada);
//ZA
int buscar_grupo_conxuntivo(t_frase_separada *frase_separada);
int comprobar_locucions_de_diccionario(t_frase_separada *frase_separada);
//int esta_palabra_ten_a_categoria_de(unsigned char categoria,t_frase_separada *pal_frase_separada);
#define esta_palabra_ten_a_categoria_de(a,b) (strchr((char *)(b)->cat_gramatical, (a)))
int esta_palabra_tivera_a_categoria_de(unsigned char categoria,t_frase_separada *pal_frase_separada);
int artigo_que_inicia_frase(t_frase_separada *pal_frase_separada);
int precede_a_palabra_inicio_de_sintagma_preposicional(t_frase_separada *pal_frase_separada);
int palabra_homografa_esta_rodeada_por_dous_sintagmas_nominais(t_frase_separada *punt_pal_actual);
int concorda_en_xenero_e_numero_coa_palabra_anterior(t_frase_separada * pal_frase_separada);
int hai_antes_adverbio_ou_pronome(t_frase_separada *pal_frase_separada);
int homografa_nome_verbo(t_frase_separada * item);
void cambiar_categoria_gramatical(t_frase_separada *punt_frase_separada,unsigned char cat_nova);
void desambiguar_se_non_hai_verbo_seguro_e_si_homografas(t_frase_separada *punt_recorre,char* num_verbos);
int contar_verbos_e_homografas(t_frase_separada * punt_fin,char *num_verbos,char *num_homografas);
void desambiguar_homografia_nome_verbo(t_frase_separada *frase_separada);
void desambiguar_categorias_gramaticais(t_frase_separada *frase_separada);
//int adverbio(unsigned char codigo);
int por_contexto_e_nome_e_morfoloxicamente_verbo(t_frase_separada *recorre_frase_separada);
t_frase_separada *comprobar_sintagma_nominal(t_frase_separada*frase_separada);
//        void identificar_grupos_conxuntivos(t_frase_separada *frase_separada);
t_frase_separada *adiantar_as_preposicions_ou_locucions_prepositivas(t_frase_separada *frase_separada);
int locucion(t_frase_separada *item);
int sintagma_de_puntuacion(unsigned char codigo);
t_frase_separada *clasificar_sintagma_funcion(t_frase_separada *recorre_frase_separada,t_frase_sintagmada *frase_sintagmada,int *cont);
int nexo_bipolar(unsigned char codigo);
int signo_fin_proposicion(unsigned char clase_signo);
int nexo_coordinativo(unsigned char codigo);
int nexo(unsigned char codigo);
int nexo_ou_pausa(unsigned char codigo);
unsigned char clasifica_segun_caracter_de_pausa_e_entonacion(unsigned char codigo);
int inicio_de_grupo(unsigned char codigo);
int signo_de_peche(unsigned char codigo);
int signo_de_apertura(unsigned char codigo);
int elimina_categoria(unsigned char categoria,t_frase_separada *item);
int asigna_categoria(unsigned char categoria,t_frase_separada *item);
int anade_categoria(unsigned char categoria,t_frase_separada *item);
inline int unica_categoria(unsigned char categoria,t_frase_separada *item);
int n_categorias(t_frase_separada *item);
int reglas_desam(t_frase_separada *item,int comienzo);
int pon_pausa_sintagma(unsigned char tipo);
//ZA
int buscarConxunciones(char* patron);
//t_conxuncion *refinaBusquedaLoc(char *patron);


class Sintagma {
	private:
		char idioma;
		t_frase_separada * frase_separada;
		t_frase_sintagmada * frase_sintagmada;
		t_frase_en_grupos * frase_en_grupos;

		int ultimo_grupo(t_frase_en_grupos *a_frase_en_grupos,int cont);
		void asignar_tonema_de_apertura_peche(char tipo,t_frase_en_grupos *a_frase_en_grupos,
				unsigned char signo);
		int grupo_entre_comas_ou_nexo_e_ou(t_frase_en_grupos *a_frase_en_grupos);
		void asignar_tonemas_en_enumeracions_ou_incisos(t_frase_en_grupos *frase_en_grupos);
		void asignacion_de_tonemas_final(t_frase_en_grupos *frase_en_grupos);
		void unir_subordinadas_dependentes(t_frase_en_grupos *a_frase_en_grupos);
		void identificar_tonemas(t_frase_en_grupos *frase_en_grupos);
		void identificar_grupos_entre_pausas(t_frase_sintagmada *frase_sintagmada,
				t_frase_en_grupos *frase_en_grupos);
		void cuenta_silabas_sintagma(t_frase_sintagmada *sint);
		void identificar_sintagmas();

	public:
		void
#ifdef _WIN32
__declspec(dllexport) 
#endif
		analise_sintagmatico(t_frase_separada *fseparada, t_frase_sintagmada *fsintagmada,
				t_frase_en_grupos *fgrupos, char pidioma);
};
#endif

