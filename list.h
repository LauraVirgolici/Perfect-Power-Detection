/*-- tlista.h --- LISTA SIMPLU INLANTUITA cu elemente de tip intreg ---*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _LISTA_SIMPLU_INLANTUITA_
#define _LISTA_SIMPLU_INLANTUITA_

typedef struct celula
{
  int info;
  struct celula *urm;
} TCelula, *TLista; /* tipurile Celula, Lista */

#define VidaL(L) ((L) == NULL)

/*-- operatii elementare --*/

TLista AlocCelula(int);   /* adresa celulei create sau NULL */
void DistrugeL(TLista *); /* elimina toate celulele din lista */
void AfisareL(TLista L);
int cmpfunc(const void *a, const void *b);
TLista addToList(TLista *L, int e);
void printListofLists(TLista *array, int nr_of_lists);
void swap(TLista a, TLista b) ;
void bubbleSort(TLista start) ;
void appendList(TLista *base, TLista list);
void removeDuplicates(TLista head);
int getCount(TLista head);

#endif
