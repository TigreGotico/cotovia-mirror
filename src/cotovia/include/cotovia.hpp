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
 


/** \file Cotovia.hpp
 
    <h1>Uso de la clase Cotovia</h1>


    <h2>Llamada a Cotovía</h2>

    <p>Las diferentes funcionalidades de la clase Cotovia se pueden
    seleccionar de dos formas: mediante la línea de comandos de llamada al
    programa, o internamente, por medio de la estructura de opciones.</p>

    <h3>Dentro de un programa</h3>

    <p>Otra forma de seleccionar la funcionalidad deseada del
    conversor consiste en rellenar los campos adecuados de la estructura de tipo 
    t_opciones que se le pasa a las funciones Carga y cotovia.</p>>
  
    <p>Por sencillez, se proporciona la macro opciones_por_defecto para inicializar
    una variable de ese tipo con los valores por defecto, es decir, síntesis
    de voz en gallego, a 16 KHz, con el locutor iago, y salida por el dispositivo
    de audio.</p>
  
    <p>Sus campos más destacados son los siguientes:</p>

    <ul>
    <li>et: escalado temporal, expresado en tanto por ciento.
    <li>ep: escalado de pitch, también en tanto por ciento.
    <li>idioma: 0 --> gallego, 1 --&gt castellano.
    <li>wav: 1 --> se genera un fichero wav, 0 -->  no.
    <li>audio: 1 --> se envía la voz sintética al
    dispositivo de sonido, 0 --> no.
    <li>locutores: nombres de los locutores, separados por
    espacios, que se desean emplear.
    <li>dirsalida: ruta del directorio de salida. Por defecto, el
    mismo en el que se encuentra el ejecutable.
    <li>dirbd: ruta del directorio data, en el que se encuentran
    las bases de datos. Por defecto, ``../data''.
    <li>entradaporfichero: 1 --> el texto que se desea
    sintetizar se encuentra almacenado en el fichero que se le pasa como
    primer parámetro.
    <li>0 -->  se sintetiza la cadena que se pasa como
    parámetro a la función cotovia de la clase.
    </ul>

    <h2>Flujo del programa</h2>

    <p>El uso normal de un objeto de la clase Cotovia es el siguiente:</p>

    <ul>
    <li>Llamada a la función Carga, con las opciones de funcionamiento deseadas.
    <li>Llamadas a la función cotovia.
    <li>Llamada a la función Descarga.
    </ul>

    <p>Los cambios en el comportamiento del objeto en las sucesivas
    llamadas a cotovia se realizan mediante la estructura de opciones. Así, por 
    ejemplo, para cambiar el locutor de una llamada a la siguiente llegaría con copiar
    su nombre en el campo locutores. No obstante, todos los locutores que un objeto
    vaya a emplear deben haber sido cargados previamente por dicho objeto mediante
    la función Carga.</p>

*/

#ifndef COTOVIA_HPP
#define COTOVIA_HPP


#define FREIRE           0
#define PAULINO          1
#define MONCHO           0
#define SONIA            1
#define TODAS_VOCES      2

#define TODOS_IDIOMAS    0
#define GALEGO           1
#define CASTELLANO       2

typedef struct
{
  char nombre_locutor[FILENAME_MAX];
  int numero_usuarios;
} Locutores_ya_cargados;

class Lengua {
public:	
  char * nombre;
  int numero_usuarios;
  unsigned char indice;
  unsigned char tipo;
  //int ya_cargada(char *l) {return !strcmp(nombre, l);};
  //Lengua(char *l, unsigned char tip){numero_usuarios = 1; nombre = l; tipo = tip;};
  //carga() {numero_usuarios++;};
  //descarga() {numero_usuarios--;};

};


#ifdef _WIN32
class __declspec(dllexport) Cotovia:Interfaz
#else
class Cotovia:Interfaz
#endif
				    {
				    protected:
				      static Locutores_ya_cargados *locutores_ya_cargados;

				      bool analisis_morfosintactico_cargado;
				      bool locutor_cargado;
				      bool audio_cargado;
				      bool variantes_cargado;



				      t_tipo_frase tipo_frase;

				      char *frase;
				      Token *f_tokenizada;
				      t_frase_separada *f_separada;
				      char *f_procesada;
				      char *f_silabas;
				      char *f_fonetica;
				      t_frase_sintagmada *f_sintagmada;
				      t_frase_en_grupos *f_en_grupos;
				      t_prosodia *f_prosodica;

				      int numero_de_tokens;
				      int numero_de_elementos_de_frase_separada;

				      char locutores_empleados[TAMANO_MAXIMO_CADENA_LOCUTORES];

				      FILE *flin;
				      FILE *f_analisis;
	
				      t_opciones opciones;

				      Leer_frase leer_frase;
				      Tokenizar tokenizador;
				      Preproceso preprocesador;
				      Transcripcion transcripcion;
				      Crea_vectores_descriptor crea_descriptores;
				      Procesado_senhal procesado_senhal;
				      Prosodia prosodia;
				      Trat_fon trat_fon;
				      Analisis_morfologico analisis_morfologico;
				      Analisis_morfosintactico analisis_morfosintactico;
				      Sintagma sintagma;
	//			      Sintesis_difonemas sintesis_difonemas;
				      Audio audio;

				      vector<Vector_descriptor_objetivo> cadena_unidades;
				      int numero_unidades;
				      Grupo_acentual_objetivo * acentuales_obj;
				      int numero_acentuales;

				      Frase_frontera frase_frontera_obj;
    	
				      static Locutor **locutor;
				      static int locutores_cargados;

				      Locutor *locutor_actual;
				      map <string, map<int, int> > map_unidades_escogidas;
				      Seleccion_unidades seleccion_unidades;


				      virtual void Reset(void);
				      virtual int Inicio(char *entrada);
				      virtual void Fin(void);
	

				      int anhade_locutor(char *nombre_locutor, bool * carga_locutor);
				      int libera_locutor(char *nombre_locutor, bool * libera_memoria, int *posicion);
				      int devuelve_posicion_locutor(char *nombre_locutores, int *posicion);
				      int selecciona_locutor(char *nombre_locutor, Locutor * &locutor_actual);
	
	
				      virtual int carga_frase(char * entrada, char *frase, bool &ultima) {
					return leer_frase.leer_frase(entrada, frase, ultima);
				      }
	
				      virtual void construye_frase_fonetica(void);
				      virtual int procesado_linguistico(void);
#ifdef _PARA_ELISA
				      virtual int generacion_prosodia(FILE * fichero_fonemas, FILE * fichero_silabas, estructura_duracion *duracion_silabas, int *numero_silabas, estructura_duracion *duracion_fonemas, int *numero_fonemas, char * nombre_sfs, char * nombre_fonemas, int correccion_manual);
#elif defined(_ESTADISTICA)
				      virtual int generacion_prosodia(Estadistica *fonema_est, Estadistica *semifonema_izquierdo_est, Estadistica *semifonema_derecho_est, Estadistica *trifonema_est, FILE *fichero_fonema_est, FILE *fichero_semi_izquierdo, FILE *fichero_semi_derecho, FILE *fichero_trifonema);

#elif defined(_CALCULA_DATOS_MODELO_ENERGIA)
				      virtual int generacion_prosodia(char * nombre_sonidos, char * nombre_fonemas, char *nombre_marcas, char * nombre_sfs, FILE * fichero_fonemas, FILE *fichero_marcas, FILE * fichero_sonido, int frecuencia_muestreo, Parametros_modelo_energia **cadena_datos_energia, int * numero_elementos_energia, int correccion_manual);
#elif defined(_CORPUS_PROSODICO)
				      virtual int generacion_prosodia(Estructura_prosodica **corpus_prosodico, int *numero_estructuras, unsigned char *corpus_incompleto, FILE *fichero_corpus, FILE *fichero_rechazo);
#else
				      virtual int generacion_prosodia(void);
#endif	
	
				      void libera_memoria_prosodia(void);
					  void print_hts_info(void);  // HTS

				      static map<string, Lengua> lenguas;
				      static map<unsigned char, char *> lang;
				      unsigned char numero_idiomas;
				    public:

				      Cotovia();
				      ~Cotovia();
				      int Descarga(void);
				      int Carga(t_opciones &opc);

				      //funcion principal

#if defined(_ESTADISTICA) ||					\
  defined(_GENERA_CORPUS) || defined(_CORPUS_PROSODICO) ||	\
  defined(_LEE_ENTONACION_DE_FICHERO) ||			\
  defined(_SECUENCIAS_FONETICAS)

				      int cotovia(int argc, char **argv, char *entrada, t_opciones opc);

#elif defined(_CALCULA_INDICES)

				      int cotovia(char *entrada, char *salida, t_opciones opc, float duracion,
						  float frecuencia);

#elif defined(_CALCULA_PENALIZACIONES_OPTIMAS)

				      int cotovia(char *entrada, t_opciones opc, float pen_4_gramas, float pen_3_gramas,
						  float pen_2_gramas);

#elif defined(_CALCULA_FACTORES)

				      int cotovia(char *entrada, char *salida_f0, char *salida_dur, char *salida_rafaga,
						  char *salida_concatena, char *salida_fonema_coart, char *salida_contextual,
						  char *salida_func_concatena, char *salida_ent_dur,
						  char *salida_pos_acentual, char *salida_pos_fonico,
						  char *salida_conc_acentual, char *salida_diferencias_prosodicas, t_opciones opc);

#else

				      int cotovia(char *entrada, t_opciones opc);

#endif

};

#endif

