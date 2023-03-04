/*-- functiiLista.c --*/
#include <ctype.h>
#include "list.h"

/*--- operatii de baza pentru lista simplu inlantuita ---*/

TLista AlocCelula(int e) /* adresa celulei create sau NULL */
{
	TLista aux = (TLista)malloc(sizeof(TCelula)); /* (1) incearca alocare */
	if (aux)									  /* aux contine adresa noii celule */
	{
		aux->info = e; /* (2,3) completeaza noua celula */
		aux->urm = NULL;
	}
	return aux; /* rezultatul este adresa noii celule sau NULL */
}

TLista addToList(TLista *L, int e)
{

	TLista aux, u;
	aux = AlocCelula(e);

	if (*L == NULL)
		*L = aux;
	else
	{
		u = *L;
		while (u->urm != NULL)
			u = u->urm;

		u->urm = aux;
	}
	return *L;
}


void printListofLists(TLista *array, int nr_of_lists){
	for(int i=0;i< nr_of_lists ;i++){
		AfisareL(array[i]);
	}
}





int cmpfunc(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

void AfisareL(TLista L)
/* afiseaza valorile elementelor din lista */
{
	printf("[");			  /* marcheaza inceput lista */
	for (; L != NULL; L = L->urm) /* pentru fiecare celula */
		printf("%d ", L->info);	  /* afiseaza informatie */
	printf("],");				  /* marcheaza sfarsit lista */
}

void DistrugeL(TLista *aL)
{
	TLista aux;
	while (*aL)
	{
		aux = *aL;
		*aL = aux->urm;
		free(aux);
	}
}





/* function to swap data of two nodes a and b*/
void swap(TLista a, TLista b) 
{ 
    int temp = a->info; 
    a->info = b->info; 
    b->info = temp; 
} 

void bubbleSort(TLista start) 
{ 
    int swapped, i; 
    TLista ptr1; 
    TLista lptr = NULL; 
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->urm != lptr) 
        { 
            if (ptr1->info > ptr1->urm->info) 
            { 
                swap(ptr1, ptr1->urm); 
                swapped = 1; 
            } 
            ptr1 = ptr1->urm; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 

void appendList(TLista *base, TLista list){

	TLista u= *base;

	if(u==NULL)
		{*base=list;
		return;}
	else 
		while(u->urm!=NULL)
			u=u->urm;
	
	u->urm=list;
}


void removeDuplicates(TLista head)
{
    /* Pointer to traverse the linked list */
    TLista current = head;
  
    /* Pointer to store the next pointer of a node to be deleted*/
    TLista next_next; 
    
    /* do nothing if the list is empty */
    if (current == NULL) 
       return; 
  
    /* Traverse the list till last node */
    while (current->urm != NULL) 
    {
       /* Compare current node with next node */
       if (current->info == current->urm->info) 
       {
           /* The sequence of steps is important*/               
           next_next = current->urm->urm;
           free(current->urm);
           current->urm = next_next;  
       }
       else /* This is tricky: only advance if no deletion */
       {
          current = current->urm; 
       }
    }
}

int getCount(TLista head)
{
    int count = 0; // Initialize count
    TLista current = head; // Initialize current
    while (current != NULL) {
        count++;
        current = current->urm;
    }
    return count;
}



  