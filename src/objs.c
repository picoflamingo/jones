#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "facts.h"
#include "objs.h"


static LIST *_obj_list = NULL;

LIST*
jones_obj_get_list ()
{
  return _obj_list;
}


int    
jones_obj_init ()
{
  _obj_list = list_new ("object_list", 1024, sizeof (OBJECT));

  return 0;
}

int     
jones_obj_dump ()
{

  OBJECT  *o;
  int     i, j, n;

  for (i = 0; i < _obj_list->n; i++)
    {
      o = (OBJECT*)_obj_list->item[i];
      n = o->facts->n;
      printf ("OBJ %d: (%s) [%d FACTS]\n", i, OBJ_ID(o), n);


      for (j = 0; j < n; j++)
	{
	  jones_fact_dump (o->facts->item[j]);
	}
      printf ("\n");
    }
  printf ("===============================================\n");

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
  return (OBJECT*) list_find_item (_obj_list, id);
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
  list_add_item (_obj_list, o);

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
  BASIC_ITEM *bi;
  OBJECT     *o;

  if (!id) return NULL;

  if ((o = (OBJECT*)malloc (sizeof(OBJECT))) == NULL)
    {
      fprintf (stderr, "Cannot create object %s\n", id);
    }
  bi = (BASIC_ITEM*) o;
  bi->id = strdup (id);
  o->facts = list_new ("obj_fact_list", 1, sizeof (FACT*));
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

  if ((f1 = (FACT*) list_find_item (o->facts, f->bi.id)))
    {
      jones_fact_set (f1, jones_fact_get (f));
    }
  else
    {
      list_add_item (o->facts, f);
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

  return (FACT*)list_find_item (o->facts, id);
}
 
FACT *  jones_obj_get_fact_by_indx (OBJECT *o, int indx)
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
