/*
 * Jones: A basic rule-engine system
 * Copyright (c) 2015 David Martinez Oliveira
 *
 * This file is part of Jones
 *
 * Jones is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jones is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Jones.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nyx_list.h>

#include "facts.h"
#include "objs.h"


//static LIST *_obj_list = NULL;
static NYX_LIST *_obj_list = NULL;

NYX_LIST*
jones_obj_get_list (void)
{
  return _obj_list;
}


int    
jones_obj_init (void)
{
  _obj_list = nyx_list_new ("object_list", 1024, sizeof (OBJECT));

  return 0;
}

int     
jones_obj_dump (void)
{

  OBJECT  *o;
  int     i, j, n;

  for (i = 0; i < _obj_list->n; i++)
    {
      o = (OBJECT*)_obj_list->item[i];
      n = o->facts->n;
      if (n == 0) continue;
      printf ("OBJ %d: (%s) [%d FACTS]\n", i, OBJ_ID(o), n);


      for (j = 0; j < n; j++)
	{
	  jones_fact_dump (o->facts->item[j]);
	}
      printf ("\n");
    }
  //printf ("===============================================\n");

  return 0;
}

int     
jones_obj_fact_query (char *query)
{
  OBJECT  *o;
  FACT    *f;
  int     i, j, n, cnt;
  char    *copy, *fact;
  int     skip_value = 0;

  if (!query) return -1;
  copy = strdup (query);
  fact = copy;
  fact[strlen(fact) - 2] = 0;
  if (fact[0] == '!')
    {
      skip_value = 1;
      fact++;
    }
  cnt = 0;
  //printf ("Querying FACT '%s'\n", fact);
  for (i = 0; i < _obj_list->n; i++)
    {
      o = (OBJECT*)_obj_list->item[i];
      n = o->facts->n;
      if (n == 0) continue;
      for (j = 0; j < n; j++)
	{
	  f = (FACT*) o->facts->item[j];
	  if (f->value == skip_value) continue;

	  if (!strcmp (OBJ_ID(f), fact))
	    {
	      printf ("= Object '%s' -> ", OBJ_ID(o));
	      jones_fact_dump (f);
	      cnt++;
	    }
	}
    }
  printf ("         %d results for query '%s'\n", cnt, copy);
  //printf ("===============================================\n");
  free (copy);
  return 0;
}



OBJECT*
jones_obj_get (char *id)
{
  if (!_obj_list)
    {
      fprintf (stderr, "Object list not initialised...\n");
      return NULL;
    }
  if (!id) return NULL;
  return (OBJECT*) nyx_list_find_item (_obj_list, id);
  //return (OBJECT*) list_find_item (_obj_list, id);
}

int    
jones_obj_add (OBJECT *o)
{
  if (!_obj_list)
    {
      fprintf (stderr, "Object list not initialised...\n");
      return -1;
    }
  if (!o) return -1;
  //list_add_item (_obj_list, o);
  nyx_list_add_item (_obj_list, o);

  return 0;
}

int    
jones_obj_del (OBJECT *o)
{
  fprintf (stderr, "Function %s not yet implemented\n", __FUNCTION__);
  return 0;
}


OBJECT* 
jones_obj_new (char *id)
{
  NYX_BASIC_ITEM *bi;
  OBJECT         *o;

  if (!id) return NULL;

  if ((o = (OBJECT*) malloc (sizeof(OBJECT))) == NULL)
    {
      fprintf (stderr, "Cannot create object %s\n", id);
    }
  bi = (NYX_BASIC_ITEM*) o;
  bi->id = strdup (id);
  //o->facts = list_new ("obj_fact_list", 1, sizeof (FACT*));
  o->facts = nyx_list_new ("obj_fact_list", 1, sizeof (FACT*));
  return o;
}

int    
jones_obj_free (OBJECT *o)
{
  return 0;
}


int    
jones_obj_add_fact (OBJECT *o, FACT *f)
{
  FACT *f1;

  if (!o) return -1;
  if (!f) return -1;

  //if ((f1 = (FACT*) list_find_item (o->facts, f->bi.id)))
  if ((f1 = (FACT*) nyx_list_find_item (o->facts, f->bi.id)))
    {
      jones_fact_set (f1, jones_fact_get (f));
    }
  else
    {
      //list_add_item (o->facts, f);
      nyx_list_add_item (o->facts, f);
      jones_fact_set_obj (f, o);
    }
  return -1;

}

int    jones_obj_del_fact (OBJECT *o, int indx)
{
  return 0;
}

FACT*   
jones_obj_get_fact (OBJECT *o, char *id)
{
  if (!o) return NULL;
  if (!id) return NULL;

  //return (FACT*)list_find_item (o->facts, id);
  return (FACT*) nyx_list_find_item (o->facts, id);
}
 
FACT *  
jones_obj_get_fact_by_indx (OBJECT *o, int indx)
{
  return 0;
}

FACT*
jones_obj_get_or_create_fact (OBJECT *o, char *id, int val)
{
  FACT* f;

  if (!o) return NULL;
  if (!id) return NULL;

  if ((f = jones_obj_get_fact (o, id)) == NULL)
    {
      f = jones_fact_new (id);
      jones_fact_set_obj (f, o);
      jones_obj_add_fact (o, f);
      
    }
    jones_fact_set (f, val);
  return f;
}


FACT*
jones_obj_get_or_create_fact1 (OBJECT *o, char *id, int val)
{
  FACT* f;

  if (!o) return NULL;
  if (!id) return NULL;

  if ((f = jones_obj_get_fact (o, id)) == NULL)
    {
      f = jones_fact_new (id);
      jones_fact_set_obj (f, o);
      jones_obj_add_fact (o, f);
      jones_fact_set1 (f, val);
    }
  //jones_fact_set1 (f, val);
  return f;
}


int     
jones_obj_get_fact_val (OBJECT *o, char *id)
{
  FACT   *f;
  if (!o) return -1;
  if (!id) return -1;

  if ((f = jones_obj_get_fact (o, id)) == NULL)
    {
      return -1;
    }
  else
    return jones_fact_get (f);


}
