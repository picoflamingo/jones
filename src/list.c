#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

LIST*  
list_new (char *id, int size, int item_size)
{
  LIST        *l;
  BASIC_ITEM  *bi;

  if (size < 0) return NULL;
  if ((l = (LIST*)malloc (sizeof(LIST))) == NULL)
    {
      fprintf (stderr, "Cannot allocate memory for list object\n");
      return NULL;
    }
  bi = (BASIC_ITEM*)l;
  bi->id = strdup (id);
  l->size = size;
  l->item_size = item_size;
  l->item_free = NULL;
  if ((l->item = malloc (sizeof(void*) * size)) == NULL)
    {
      fprintf (stderr, "Cannot allocate memory for list items\n");
      free (l);
      return NULL;
    }
  
  return l;
}

int   
list_free (LIST *l)
{
  BASIC_ITEM *bi;

  if (!l) return -1;
  bi = (BASIC_ITEM*) l;
  if (bi->id) free (bi->id);
  if (!l->item_free) free (l->item);
  else
    {
      /* Delete items in list */
      fprintf (stderr, "Free item function not yet supported\n");
      free (l->item);
    }
  free (l);
  return 0;
}

int   
list_dump (LIST *l)
{
  BASIC_ITEM *bi;
  int         i;

  bi = (BASIC_ITEM*)l;
  printf ("List: '%s' with %d items\n", bi->id, l->n);
  for (i = 0; i < l->n; i++)
    {
      bi = (BASIC_ITEM*) l->item[i];
      printf ("ITEM: %d (%s)\n", i, bi->id);
    }

  return 0;
} 

int   
list_add_item (LIST *l, void* item)
{
  void *aux;
  if (!l) return -1;
  if (!item) return -1;

  /* Reallocate buffer if required */
  if (l->n == l->size) 
    {
#if 0
      fprintf (stderr, "List full... re-allocating...(%d ->%d)\n", 
	       l->size, l->size + SIZE_INC);
#endif      
      if ((aux = realloc (l->item, sizeof(void*) * (l->size + SIZE_INC))) == NULL)
	{
	  fprintf (stderr, "Cannot add element to list\n");
	  return -1;
	}
      l->size += SIZE_INC;
      l->item = aux;
    }

  l->item[l->n] = item;
  l->n++;
  return 0;
}


void*   
list_find_item (LIST *l, char *id)
{
  BASIC_ITEM*  bi;
  int   i;

  if (!l) return NULL;
  if (!id) return NULL;

  for (i = 0; i < l->n; i++)
    {
      bi = (BASIC_ITEM *) l->item[i];
      if (bi && !strcmp (bi->id, id)) return l->item[i];
    }
  return NULL;
}

void* 
list_get_item (LIST *l, int i)
{
  return 0;
}
