/*
 * Jones: A basic rule-engine system
 * Copyright (c) 2015 David Mart.nez Oliveira
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

#include "ipython.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rules.h"


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
  printf ("--[A]------------------------------------------------\n");
  _iter ++;
  jones_obj_dump();
  printf ("-[%d]--------------------------------------------------------\n", _iter);


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
