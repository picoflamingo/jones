
#include "ipython.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rules.h"



//static LIST  *_rules = NULL;
static NYX_LIST  *_rules = NULL;
static int       _iter = 0;

int
jones_rule_get_iter ()
{
  return _iter;
}

int   
jones_rule_init (void)
{
  //_rules = list_new ("rules", 16, sizeof (RULE*));
  _rules = nyx_list_new ("rules", 16, sizeof (RULE*));
  _iter = 0;

  return 0;
}

int   
jones_rule_dump (void)
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

  //list_add_item (_rules, r);
  nyx_list_add_item (_rules, r);
  return 0;
}




int   
jones_resolve (void)
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
jones_rule_full_eval ()
{
  int       n_rules;
  int       n_obj;
  int       keep_going = 0;
  NYX_LIST  *obj;
  int       i, j;
  RULE      *r;
  OBJECT    *o;


  n_rules = _rules->n;
  obj = jones_obj_get_list ();
  n_obj = obj->n;
  keep_going = 0;

#ifdef DEBUG1
  printf ("Running full evaluation. %d rules on %d objects\n", n_rules, n_obj);  


  printf ("--[B] ------------------------------------------------\n");
  jones_obj_dump();
  printf ("--[%d]------------------------------------------------------\n", _iter);
#endif

  do
    {
      for (i = 0; i < n_rules; i++)
	{
	  r = (RULE*) _rules->item[i];
	  for (j = 0; j < n_obj; j++)
	    {
	      o = (OBJECT*) obj->item[j];
#ifdef DEBUG1
	      printf ("---> [%d] Firing Rule (%s) on object (%s)\n", 
		      _iter, OBJ_ID(r), OBJ_ID(o));
#endif
	      keep_going += jones_rule_eval (r, o);
	    }
	}

    } while (0);
  //printf ("%d Rules fired\n", keep_going);
  //printf ("--[A]------------------------------------------------\n");
  _iter ++;
  //jones_obj_dump();
  //printf ("-[%d]--------------------------------------------------------\n", _iter);


  return keep_going;

}

int   
jones_rule_eval (RULE *r, OBJECT *o)
{
  int       i, n, ret;
  NYX_LIST  *facts;
  FACT      *f;

  if (!r) return -1;
  if (!o) return -1;

  /* Go through object FACTS and fire rule if FACT matches firing fact*/
  facts = o->facts;
  n = facts->n;
  ret = 0;
  for (i = 0; i < n; i++)
    {
      f = (FACT*) facts->item[i];
      //if (f->iter >= _iter) continue;
      //f->iter = _iter;
      if (!strcmp (OBJ_ID(f), r->ffact))
	ret += r->f (r, o, f);
    }

  return ret;
}


int   
jones_rule_resolve_fact (OBJECT *o, char *fid)
{
  FACT       *f;
  int        i, n, j, n1;
  RULE       *r;
  OBJECT     *o1;
  NYX_LIST   *obj;
  int        keep_going;

  if (!o) return -1;
  if (!fid) return -1;

  /* XXX: Temporal... run full evaluation before trying to resolve fact */

  printf ("Resolving %s on object %s\n", fid, OBJ_ID(o)) ;

  /* Check if FACT exists and if it is known */
  //if ((f = list_find_item (o->facts, fid)))
  if ((f = nyx_list_find_item (o->facts, fid)))
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
  //printf ("Firing %d Rules....\n", n);
  keep_going = 0;
  do
    {
      for (i = 0; i < n; i++)
	{
	  r = (RULE*) _rules->item[i];
	  for (j = 0; j < n1; j++)
	    {
	      o1 = (OBJECT*) obj->item[j];
	      //printf ("Firing Rule (%s) on object (%s)\n", OBJ_ID(r), OBJ_ID(o1));
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


SIMPLE_RULE_DATA* 
jones_rule_add_srule_data (RULE *r, char *rule)
{
  char             f1[1024], f2[1024];
  SIMPLE_RULE_DATA *s;

  if (!rule) return NULL;
  
  if ((s = malloc (sizeof (SIMPLE_RULE_DATA))) == NULL)
    return NULL;

  /* Parse string */
  sscanf (rule, "%s -> %s", f1, f2);
  if (f1[0] == '!') 
    {
      s->n1 = 0; 
      s->f1 = strdup (f1 + 1);
    }
  else 
    {
      s->n1 = 1;
      s->f1 = strdup (f1);
    }

  if (f2[0] == '!') 
    {
      s->n2 = 0; 
      s->f2 = strdup (f2 + 1);
    }
  else 
    {
      s->n2 = 1;
      s->f2 = strdup (f2);
    }

  /* Store data */
  jones_rule_set_data (r, s);
  jones_rule_add_firing_fact (r, s->f1);
  jones_rule_add (r);
  return s;

}

int   
jones_rule_simple_rule (RULE*r, OBJECT *o, FACT *f)
{
  SIMPLE_RULE_DATA *s = jones_rule_get_data (r);
  FACT  *f1, *f2;
  
  if ((f1 = jones_obj_get_fact (o, s->f1)) == NULL)
    {
      fprintf (stderr, "SRULE (%s): Cannot find %s on object %s\n",
	       OBJ_ID(r), s->f1, OBJ_ID(o));
      return 0;
    }
  if (f1->value == s->n1)
    {
      if ((f2 = jones_obj_get_fact (o, s->f2)) == NULL)
	{
	  /* FIXME: We should create the fact if it does not exists...*/
	  fprintf (stderr, "SRULE (%s): Cannot find %s on object %s\n",
		   OBJ_ID(r), s->f2, OBJ_ID(o));
	  return 0;
	}
      jones_fact_set1 (f2, s->n2);
    }

  return 0;
}
