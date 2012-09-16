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
 






#define NUM_MODELOS              4  // Numero de modelos por infinitivo
#define NUM_INFINITIVO           4 // Numero de infinitivos por raiz
//fmendez
#define NUM_MODEL_DES           200  // Numero de modelos por desinencia
//fmendez
#ifndef VERBOS_HPP
   #define VERBOS_HPP

#include <gestor_busquedas_memoria.hpp>
#include <gbm_raices_verbales.hpp>
#include <gbm_desinencias.hpp>
/******************************************************************************
 *                            DECLARACION DE TIPOS                            *
 ******************************************************************************/

  typedef struct {
        char *palabras;
        t_conxuncion *conxuncions;
        int numero;
}  t_tabla_conxuncion;

typedef struct {
   char           desinencia[LONX_VERBA];
   unsigned char  cod_desinencia[NUM_MODEL_DES+1];
} t_posibles_desinencias;
// O nº. codigos desinencias: codigos des)*2 (modelos conx)+1(para NULL) = NUM_MODEL_DES+1



typedef  struct{                    // **** ESTRUCTURA DA INFORMACION DE VERBOS
//  char inf[LONX_VERBA];      // Infinitivo que ten como raiz: raiz[]
//   char inf[LONX_MAX_PALABRA];

        char *inf;


//ZA: Nota. Ahora la constante es LONX_VERBA, antes era LONX_MAX_PALABRA

   char grupos[NUM_MODELOS];        // Modelos de conxugacion asociados a un verbo
} modelos;                          // Tamanho max observado co xerador_taboas

typedef  struct{                    // **** ESTRUCTURA DOS VERBOS DA LISTA ****
//  char raiz[LONX_VERBA];     // Raiz verbal que poden ter varios verbos
//  char raiz[LONX_MAX_PALABRA];
//ZA: Idem. LONX_VERBA en vez de LONX_MAX_PALABRA
char *raiz;


   modelos modelo[NUM_INFINITIVO];  // Estructura con informacion de verbos
} t_raices_de_verbos;               // Tamanho max observado co xerador_taboas
typedef struct{                     // **** ESTRUCTURA DAS DESINENCIAS DA LISTA
   char *desinencia;                // Desinencia contida na estructura
   char info[NUM_MODEL_DES];        // Informacion das desinencias
}t_desinencias;                     // Tamanho max observado co xerador_taboas

typedef struct {                    // **** ESTRUCTURA DOS NODOS DA LISTA *****
      void *ptro_ant;               // Punteiro o nodo anterior da lista
      char verbo[LONX_VERBA];       // Infinitivo do verbo
      char raiz[LONX_VERBA];        // Raiz do verbo
      int modelo;                   // Modelo que ten o verbo
      void *ptro_sig;               // Punteiro o nodo seguinte da lista
} nodo;

/******************************************************************************
 *                  VARIABLES GLOBAIS QUE SE EXPORTAN.                        *
 ******************************************************************************/
//extern t_dic_enclitico pron_encliticos[];
//extern char matriz_cliticos_tiempos[26][217];





typedef char chapuza[217];

extern t_dic_enclitico *pron_encliticos;
extern chapuza *matriz_cliticos_tiempos;

extern int NU_PRON_ENCLITICOS;
int iniciaCliticos(char idioma);


class gbm_verbos: public gbm {

private:
  t_raices_de_verbos *raices_de_verbos;
  t_desinencias *desinencias;
  char idioma;
  gbm_raices_verbales *gbrv;
  gbm_desinencias *gbd;


public:
  gbm_verbos(char *,char *);
  gbm_verbos();
  vector <int> flexionar(char *lema);
  int getDatos2(int,t_frase_separada*);
  ~gbm_verbos();
  
//Sin estas funciones, el derivador no tirará bien.
   virtual char *devuelveLema() {return gbrv->devuelveLema();};
   virtual int buscar(char *nombre,t_frase_separada *frase_separada) {return gbrv->buscar(nombre,frase_separada);};


  /******************************************************************************
 *                    PROTOTIPOS DAS FUNCIONS DO MODULO                       *
 ******************************************************************************/
  int analizar_verbos(char *palabra,unsigned char* tempos_verdadeiros,t_infinitivo* inf, char idioma);
  void recompor_verbo_sen_segunda_forma(char *pal_analizada,char * pal_normal);
  int ditongo_decrecente(char * primeira_vocal);
  int acentua_graficamente(char * pos_acento);
  int acento_para_desfacer_ditongo(char * pal, char * rec);
  int recortar_e_acentuar_palabra(char * pal, const char * recorte, char idioma);
  int obter_conxugacion_de_codigo (unsigned char codigo);
  int copia_en_estructura(char *infinitivo, char *raiz,int modelo, t_infinitivo *inf);
  int comprobar_raiz_verbal(t_posibles_desinencias *lista_de_tempos_posibles,  char *verbo_e_desinencia,unsigned char * tempo_verdadeiro, t_infinitivo* info);
  int detectar_desinencias_verbais( char *pal_analizada,t_posibles_desinencias *lista_desinencias);
  int eliminar_encliticos(t_palabra_proc palabra_analizada, t_enclitico *enclitico_encontrado,int *orden,int eliminada_segunda_forma, char *tratar_de_eliminar_enclitico);
  int eliminar_segunda_forma( char *pal_analizada, char idioma);
  void decidir_entre_varias_posibilidades_de_verbos(t_posibles_desinencias *pos_desinencias, char *enclitico);
  int conjugacion(char *verb);
  int comprueba_validez_tiempo_conj(unsigned char *tempos_parciais, t_infinitivo *inf);
  int mete_enclitico(t_enclitico enclitico,t_infinitivo *inf,int n, char idioma);
  int mete_artigo(char *artigo,t_infinitivo *inf,int n);
  int excep_verbo(char *verbo);
  int comprueba_validez_tiempo_encl(unsigned char *tempos_parciais, t_infinitivo *inf);
  int diacritico(char *pal, char idioma);
  int reacentua(char * pal,char * pal_sal,char idioma);


  int comprobar_en_lista_de_verbos(char *pal, char idioma);
  int buscar_en_desinencias(char *pal, int tamanio, char idioma);


};
  




#endif
