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
 

/* perfhash.c */

/* CALCULO DE UNA FUNCION DE HASH PERFECTA MULTIDIMENSIONAL,
   DE m CLAVES A n>m POSICIONES, conforme al algoritmo presentado en:

   Z.J.Czech, G.Havas, B.S.Majewski: "An Optimal algorithm for generating
   minimal perfect hash functions", Information Processing Letters 43 (1992),
   257--264.  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "perfhash.hpp"

/* entrada */
static unsigned int dim;       //dim de las claves [int]...[int]
static unsigned char *claves;  //buffer de claves
static unsigned int sep;       //separacion en bytes entre clave y clave.
static unsigned int m;         //num claves
static unsigned int n;         //num celdas (tamaño de g deseado) optimo=2*m
static int tipo_phash;         //STANDARD o BITRIGRAMAS

/*Notas: se define la zona de claves como un puntero a char para poder
acceder fácilmente a cualquier clave mediante aritmética de punteros
(ver macro "dame_clave").

Si las claves van todas seguidas, la "sep" entre claves es
obviamente (sizeof(int)*dim). Pero se deja flexible por si se desea combinar
las claves con otros datos, mediante estructuras del tipo:

struct nodo {
 int clave[dim];
 ...
 ...
} claves_y_mas[m];

En este caso debe fijarse sep=sizeof(struct nodo).
*/


/* salida */
static unsigned int *k1;    //[0..dim-1] para producto escalar con clave
static unsigned int *k2;    //[0..dim-1] para producto escalar con clave
static unsigned int *g;     //[0..n] define el grafo acíclico del hash


/* macros de apoyo */

//se utiliza para generar los vectores k1 y k2. 
#define indice_aleat(tam_arr) ((unsigned int)((float)(tam_arr)*rand()/(RAND_MAX+1.0)))

//claves es char *. Si no, no sirve esta aritmetica de punteros
// También tendríamos que cambiar estas macros, para que devolviesen char *

#define dame_clave(i) ((tipo_clave *)(claves+sep*i))
#define ph_dame_clave(ph,i) ((tipo_clave *)(ph->claves+ph->sep*i))


/*******************************************************************
 *                      ESTRUCTURA DE DATOS                        * 
 *******************************************************************/

/* Para construir el grafo, recorremos el conjunto de claves, y por
   cada clave calculamos dos nodos (f1(clave) y f2(clave)).
   Creamos dos transiciones entre esos 2 nodos (del uno al otro y viceversa) */
   
/* El array de transiciones tendrá tamaño 2*m y reservamos el espacio
   para el mismo antes de comenzar los cálculos, controlando la siguiente
   posición utilizable mediante la variable "transiciones_usadas". Al no
   tener quie hacer malloc en cada nodo, la velocidad es mayor.
   La variable "h" guarda el número de la clave (0..m-1). */

static struct transicion {
  unsigned int nodo_destino;
  struct transicion *sig;
  unsigned int h;
} *transiciones;
static int transiciones_usadas;


/* El array de nodos tiene tamaño n. Cada nodo tiene un conjunto de
   transiciones salientes (lista enlazada), y lleva asociado un valor de
   g=f(h), que se obtiene recorriendo el grafo una vez éste está completo y si
   es disjunto. La variable visita nos sirve para comprobar si es disjunto. */

static struct nodo {
  struct transicion *l_transiciones; /* lista enlazada */
  unsigned int visita;
} *nodos;

/* Valores para el campo visita */
enum visitas {NODO_NO_UTILIZADO, NODO_PENDIENTE, NODO_VISITADO};



/*******************************************************************
 *                      EVALUACION DEL HASH                        * 
 *******************************************************************/

/* Las funciones f1 y f2 calculan el producto escalar módulo n de
 k1 y k2 con una clave respectivamente */

static unsigned int f1(tipo_clave *clave) {

  if (tipo_phash==STANDARD) {

    /* original */
    int i;
    unsigned int *pk1=k1, kk1=0;
    
    for(i=dim;i--;)
      kk1+=(*pk1++)*(*clave++);
    return (kk1 % n);

  }

  else {

  /* inferente no bueno para maxlm
  int i;
  unsigned int kk1=0;
  for(i=0;i<dim;i++)
    kk1=(kk1+clave[i])*k1[i];

  return (kk1%n);
  */

  unsigned int kk1; // Antes estaba == 0;

  /*
  int i;
  for(i=dim-2;i>=0;i--)
    kk1=k1[i]*(clave[i]+kk1);
  kk1+=clave[dim-1];
  */

  /*
  if(clave[0])
    kk1 = k1[0]*(clave[0]+k1[1]*clave[1]) +clave[2];
  else
    kk1 = k1[1]*clave[1]+clave[2];
  */

  if(clave[0])
    //trigrama
    kk1 = clave[0]+k1[1]*(clave[2]+k1[0]*clave[1]);
  else
    kk1 = clave[1]+k1[0]*clave[2];

  return (kk1%n);

  }

}

static unsigned int f2(tipo_clave *clave) {

  if (tipo_phash==STANDARD) {

    /* original */
    int i;
    unsigned int *pk2=k2, kk2=0;
    
    for(i=dim;i--;)
      kk2+=(*pk2++)*(*clave++);
    return (kk2 % n);
  }

  else {

#ifdef CON_PRODUCTORIO
  int i;
  unsigned int kk2=1;
  for(i=0;i<dim;i++)
    kk2*=clave[i];
  return (kk2%n);
#else

  /* inferente no bueno para maxlm
  int i;
  unsigned int kk2=0;
  for(i=0;i<dim;i++)
    kk2=(kk2+clave[i])*k2[i];
  return (kk2%n);
  */

  unsigned int kk2; // Antes kk2 = 0;

  /*
  int i;
  for(i=dim-2;i>=0;i--)
    kk2=k2[i]*(clave[i]+kk2);
  kk2+=clave[dim-1];
  */

  /*
  if(clave[0])
    kk2 = k2[0]*(clave[0]+k2[1]*clave[1]) +clave[2];
  else
    kk2 = k2[1]*clave[1]+clave[2];
  */

  if(clave[0])
    //trigrama
    kk2 = clave[0]+k2[1]*(clave[2]+k2[0]*clave[1]);
  else
    kk2 = clave[1]+k2[0]*clave[2];



  return (kk2%n);
 
  }

#endif


}

/* la función hash() se está utilizando únicamente para verificar la
integridad del hash creado, ya que para que el usuario utilice un hash 
se emplea la función de interfaz "ph_usa_hash()" */

static unsigned int hash(tipo_clave *clave) {
  return ( (g[f1(clave)] + g[f2(clave)]) % m );
}




/*******************************************************************
 *             DISEÑO DEL HASH MEDIANTE GRAFO ACÍCLICO             * 
 *******************************************************************/


/* Añade 2 transiciones entre 2 nodos dados.
   Devuelve TRUE si la transición no es válida, lo cual se produce si
   la transición es de un nodo a sí mismo o si dichas transiciones ya
   existían. Puede que la transición introduzca un ciclo en el grafo,
   pero esto no lo podremos saber hasta que terminemos de construir
   el grafo y llamemos a "hay_ciclos" */

static int anadir_transicion(unsigned int nodo1, unsigned int nodo2, unsigned int h) {

  struct transicion *p_aux;

//  printf("  Añadiendo transicion entre %d y %d\n",nodo1,nodo2);

  /* Marcamos los nodos como utilizados */
  nodos[nodo1].visita=NODO_PENDIENTE;
  nodos[nodo2].visita=NODO_PENDIENTE;

  /* Si la transición es de un nodo a sí mismo el grafo no sirve */
  if (nodo1==nodo2) return 1;

  /* Si la transición ya existe el grafo no sirve. Solo hay que comprobar 1 */

  p_aux = nodos[nodo1].l_transiciones;
  while (p_aux) {
    if (p_aux->nodo_destino == nodo2) return 1;
    p_aux=p_aux->sig;
  }

  /* Añadimos una transición a la lista de transiciones de los nodos */

  /* De nodo1 a nodo2 */
  p_aux = &transiciones[transiciones_usadas++];
  p_aux->sig=nodos[nodo1].l_transiciones;
  nodos[nodo1].l_transiciones=p_aux;
  p_aux->nodo_destino=nodo2;
  p_aux->h=h;

  /* De nodo2 a nodo1 */
  p_aux = &transiciones[transiciones_usadas++];
  p_aux->sig=nodos[nodo2].l_transiciones;
  nodos[nodo2].l_transiciones=p_aux;
  p_aux->nodo_destino=nodo1;
  p_aux->h=h;

  return 0;

}


/* Devuelve 1 si encuentra un ciclo */
static int recorrer_nodo(unsigned int num_nodo, unsigned int nodo_anterior) {

  struct transicion *p_transicion;
  unsigned int nodo_destino;
  int ajuste_mod=(n/m+1)*m;
  /* ajuste_mod = k.m >= n , para que funcione el modulo con nums. negativos */

  //printf("Visitando nodo %2d. (g=%d)\n",num_nodo,g[num_nodo]);


  /* Si llegamos a un nodo ya visitado es que hay ciclo */
  if(nodos[num_nodo].visita == NODO_VISITADO)
    return 1;

  nodos[num_nodo].visita = NODO_VISITADO;

  /* Recorremos recursivamente todas sus transiciones */

  p_transicion=nodos[num_nodo].l_transiciones;

  while(p_transicion) {

    /* No procesamos la que lleva al nodo del cual venimos */
    if ( (nodo_destino=p_transicion->nodo_destino) != nodo_anterior ) {

      /* Lo primero que hacemos es fijar la g del nodo destino en funcion
	 de la h de la transicion y la g del nodo actual */

      g[nodo_destino]=(ajuste_mod + p_transicion->h - g[num_nodo]) % m;
      
      /* Recorremos el nodo destino */

      if (recorrer_nodo(nodo_destino,num_nodo))
	return 1;
      
    }

    p_transicion=p_transicion->sig;
 
  }

  return 0;

}

/* Para recorrer el grafo, recorremos sólo los nodos utilizados pero no
   recorridos. Después de recorrer un nodo, todos sus "hijos" quedan marcados
   como recorridos y sólo restará por recorrer las partes disjuntas. */ 

static int hay_ciclos(void) {

  unsigned int i;

  for(i=0;i<n;i++) {

    switch (nodos[i].visita) {
    case NODO_VISITADO:
	//printf("No hace falta revisitar el nodo %d\n",i);
	break;
    case NODO_PENDIENTE:      
      if (recorrer_nodo(i,-1))
	return 1;
    }
  }
  return 0;

}


/* Cada vez que vemos que el grafo es disjunto, hay que comenzar
de nuevo, reinicializando todos los datos. Borramos los nodos utilizados
y liberamos las transiciones (sin free ->más rápido!!) */


static void reinicializar_grafo(void) {

  unsigned int i;

  /*Pongo todos los nodos a 0 y borro sus transiciones*/

  for(i=0;i<n;i++) {
    
    if(nodos[i].visita) {
      //printf("Borrando nodo %d\n",i);
      nodos[i].visita=NODO_NO_UTILIZADO;
      nodos[i].l_transiciones=NULL;
      g[i]=0;
    }
  }

  transiciones_usadas=0;

}


static void aleatorizar_tablas(void) {

  unsigned int i;
  for(i=0;i<(dim-1);i++) {
    k1[i]=indice_aleat(n);
    k2[i]=indice_aleat(n);
  }

  k1[dim-1]=1;
  k2[dim-1]=1;

}


static void construir_hash(void) {

  unsigned int i, valor_f1, valor_f2, num_iter=0;
  int grafo_ok;

  nodos= (nodo *)calloc(n,sizeof(struct nodo));
  transiciones=(transicion *) calloc(2*m,sizeof(struct transicion));
  
  while (1) {
    num_iter++;
    grafo_ok=-1;
    aleatorizar_tablas();
    reinicializar_grafo();

    if( (num_iter % 25==0))
      printf("%d intentos...\n",num_iter);

    for(i=0;i<m;i++) {

      valor_f1=f1(dame_clave(i));
      valor_f2=f2(dame_clave(i));

      /* Añado la transición a los nodos (doblemente enlazados) */
      if (anadir_transicion(valor_f1,valor_f2,i))
	{ 
	  /* El grafo ya no nos sirve */
	  //printf("Grafo no válido (transiciones ilegales)\n");
	  grafo_ok=0;
	  break;
	}
    }

    if(grafo_ok) {
      if(hay_ciclos()) {
	//printf("Grafo no válido (tiene ciclos)\n");
      }
      else {
	//printf("El grafo obtenido es válido (acíclico)\n");
	break;
      }
    }    
  }
  
  printf("Se obtuvo un grafo acíclico tras %d intentos\n",num_iter);

  printf("Vector k1: ");
  for(i=0;i<dim;i++)
    printf("%15d ",k1[i]);
  printf("\nVector K2: ");
  for(i=0;i<dim;i++)
    printf("%15d ",k2[i]);
  printf("\n");

  free(nodos);
  free(transiciones);

}

static int probar_integridad(void) {


  unsigned int i;

  for(i=0;i<m;i++) {
    if (i != hash(dame_clave(i))) {
      printf("Error. hash(clave %d) = %d != %d\n",
	     i, hash(dame_clave(i)), i
	     );
      return 1;
    }
  }
  printf("Hash creado con exito. hash(clave i) = i, i=0..(m-1)\n");
  return 0;

}



/*******************************************************************
 *                      FUNCIONES DE INTERFAZ                      * 
 *******************************************************************/



struct perfhash *ph_crea_hash(unsigned int ddim,
			      unsigned char *cclaves,
			      unsigned int sep_claves,
			      unsigned int mm,
			      unsigned int nn,
			      unsigned int ttipo_phash) {
  
  struct perfhash *ph;
  
  /* Copiamos los parámetros a las variables estáticas del módulo */

  n=(unsigned int)nn;
  m=(unsigned int)mm;
  dim=(unsigned int)ddim;
  claves=cclaves;
  sep=sep_claves;
  tipo_phash=ttipo_phash;

  k1=(unsigned int *)calloc(dim,sizeof(int));
  k2=(unsigned int *) calloc(dim,sizeof(int));
  g=(unsigned int *) calloc(n,sizeof(int));

 /* calculamos el hash */

  construir_hash();

  /* creamos la estructura de datos de salida y copiamos los parámetros */

  ph = (struct perfhash *)malloc(sizeof(struct perfhash));

  ph->n=nn;
  ph->m=mm;
  ph->dim=ddim;
  ph->claves=cclaves;
  ph->sep=sep;

  ph->k1=k1;
  ph->k2=k2;
  ph->g=g;

  probar_integridad();

  return ph;

}

void ph_destruye_hash(struct perfhash *ph) {

  free(ph->k1);
  free(ph->k2);
  free(ph->g);
  free(ph);

}


int ph_usa_hash(struct perfhash *ph, tipo_clave *clave) {

  int i, pos;
  int tamano;
  unsigned int kk1=0,kk2=0,*pk1, *pk2;
  //tipo_clave *punt1 = clave;

  // Tendríamos que definir clave y clave aux como char *.

  tipo_clave *clave_aux = clave;

  pk1=ph->k1;
  pk2=ph->k2;

  for(i=ph->dim;i--;) {
    kk1+=(*pk1++) * (*clave_aux);
    kk2+=(*pk2++) * (*clave_aux++);
  }

  pos=(ph->g[kk1%ph->n]+ph->g[kk2%ph->n])%ph->m;

  /* Vemos si realmente se corresponde con la clave */

  clave_aux = ph_dame_clave(ph, pos);
  tamano = ph->dim*sizeof(tipo_clave);

  for (i = 0; i < tamano; i++)
    //if (*punt1++ != *clave_aux++)
    if (*clave++ != *clave_aux++)
        return -1;
/*
  if (memcmp(clave, ph_dame_clave(ph, pos), ph->dim*sizeof(tipo_clave)))
    return -1;
*/

  return pos;
}


int ph_prueba_integridad(struct perfhash *ph) {

  unsigned int i;

  for(i=0;i<ph->m;i++) {
    if (ph_usa_hash(ph,ph_dame_clave(ph,i))<0) {
      return 1;
    }
  }
  return 0;

}


void ph_escribe_hash(Perfhash *ph, FILE *fichero) {

  /* Primero escribimos la estructura */

  fwrite(ph, sizeof(Perfhash), 1, fichero);

  
  /* Escribimos el array de claves */

  fwrite(ph->claves, 1, ph->m*ph->sep, fichero);

  /* Ahora escribimos los arrays k1, k2 y g*/

  fwrite(ph->k1, sizeof(unsigned int), ph->dim, fichero);
  fwrite(ph->k2, sizeof(unsigned int), ph->dim, fichero);
  fwrite(ph->g, sizeof(unsigned int), ph->n, fichero);
  
}

int ph_lee_hash(Perfhash **ph, FILE *fichero) {

  Perfhash *pph;
  int dimension;


  if ( (pph = (Perfhash *) malloc(sizeof(Perfhash))) == NULL) {
    puts("Error en ph_lee_hash, al asignar memoria.");
    return 1;
  }

  fread(pph, sizeof(Perfhash), 1, fichero);

  dimension = pph->dim;

  if ( (pph->claves = (unsigned char *) malloc(pph->sep*pph->m))
       == NULL) {
     puts("Error en ph_lee_hash, al asignar memoria.");
     return 1;
  }

  if ( (pph->k1 = (unsigned int *) malloc(dimension*sizeof(unsigned int)))
       == NULL) {
    puts("Error en ph_lee_hash, al asignar memoria.");
    return 1;
  }


  if ( (pph->k2 = (unsigned int *) malloc(dimension*sizeof(unsigned int)))
       == NULL) {
    puts("Error en ph_lee_hash, al asignar memoria.");
    return 1;
  }

  if ( (pph->g = (unsigned int *) malloc(pph->n*sizeof(unsigned int)))
       == NULL) {
    puts("Error en ph_lee_hash, al asignar memoria.");
    return 1;
  }

  fread(pph->claves, 1, pph->m*pph->sep, fichero);

  fread(pph->k1, sizeof(unsigned int), dimension, fichero);

  fread(pph->k2, sizeof(unsigned int), dimension, fichero);

  fread(pph->g, sizeof(unsigned int), pph->n, fichero);

  *ph = pph;
  
  return 0;
}


