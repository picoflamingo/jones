

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rules.h"

#include "ipython.h"

static LIST  *_rules = NULL;

int   
jones_rule_init ()
{
  _rules = list_new ("rules", 16, sizeof (RULE*));


  return 0;
}

int   
jones_rule_dump ()
{
  int   i, n;
  n = _rules->n;

  for (i = 0; i < n; i++)
    {
      printf ("RULE-%d : (%s) ", i, OBJ_ID(_rules->item[i]));
    }

  return 0;
}





int   
jones_rule_add (RULE *r)
{
  if (!r) return -1;

  list_add_item (_rules, r);
  return 0;
}




int   
jones_resolve ()
{
  fprintf (stderr, "Function %s not yet implemented\n", __FUNCTION__);
  return 0;
}



RULE  *
jones_rule_new (char *id, RULE_IMP f)
{
  RULE *r;

  if (!id) return NULL;
  if (!f) return NULL;
  
  if ((r = (RULE*) malloc (sizeof (RULE))) == NULL)
    {
      fprintf (stderr, "Cannot create memory for rule (%s)\n", id);
      return NULL;
    }
  r->bi.id = strdup (id);
  r->f = f;
  r->ffact = NULL;
  r->data = NULL;
  return r;
}

int   
jones_rule_add_firing_fact (RULE *r, char *fid)
{
  if (!r) return -1;
  if (!fid) return -1;

  if (r->ffact) free (r->ffact);
  r->ffact = strdup (fid);

  return 0;		     
}

int   
jones_rule_set_data (RULE *r, void *data)
{
  if (!r) return -1;
  r->data = data;
  return 0;
}
void* 
jones_rule_get_data (RULE *r)
{
  if (!r) return NULL;
  return r->data;
}

int   
jones_rule_eval (RULE *r, OBJECT *o)
{
  int   i, n;
  LIST  *facts;
  FACT  *f;

  if (!r) return -1;
  if (!o) return -1;

  /* Go through object FACTS and fire rule if FACT matches firing fact*/
  facts = o->facts;
  n = facts->n;
  for (i = 0; i < n; i++)
    {
      f = (FACT*) facts->item[i];
      if (!strcmp (OBJ_ID(f), r->ffact))
	r->f (r, o, f);
    }

  return 0;
}


int   
jones_rule_resolve_fact (OBJECT *o, char *fid)
{
  FACT   *f;
  int    i, n, j, n1;
  RULE   *r;
  OBJECT *o1;
  LIST   *obj;
  int    keep_going;

  if (!o) return -1;
  if (!fid) return -1;

  printf ("Resolving %s on object %s\n", fid, OBJ_ID(o)) ;

  /* Check if FACT exists and if it is known */
  if ((f = list_find_item (o->facts, fid)))
    {
      /* If fact exists check value */ 
      if (f->value != FACT_UNKNOWN)
	{
	  jones_fact_dump (f);
	  printf ("DONE\n");
	  /* We are done*/
	  return 0;
	}
    }
  else
    {
      // Let's add the fact we want to resolve 
      f = jones_fact_new (fid);
      jones_obj_add_fact (o, f);
    }
  /* If the fact is unknown or undefined we have to fire all rules 
   * Now, f contains the fact to check. We fire rules and check
   * if the state changes
   */
  printf ("-----------------------------------\n");
  jones_fact_dump (f);
  printf ("-----------------------------------\n");
  n = _rules->n;
  obj = jones_obj_get_list ();
  n1 = obj->n;
  printf ("Firing %d Rules....\n", n);
  keep_going = 0;
  do
    {
      for (i = 0; i < n; i++)
	{
	  r = (RULE*) _rules->item[i];
	  for (j = 0; j < n1; j++)
	    {
	      o1 = (OBJECT*) obj->item[j];
	      printf ("Firing Rule (%s) on object (%s)\n", OBJ_ID(r), OBJ_ID(o1));
	      keep_going += jones_rule_eval (r, o1);
	      jones_obj_dump();
	      
	      if (f->value != FACT_UNKNOWN) 
		{
		  printf ("*** SOLVED ***\n");
		  jones_fact_dump (f);
		  return 0;
		}
	    }
	}
      printf ("%d Rules fired in this iteraction\n", keep_going);
      printf ("**** NOT SOLVED ****\n");
    } while (keep_going);
  return 1;
}
