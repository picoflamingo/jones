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

#include "kb.h"

/* FIXME: Let's support multiple KB... for the time being just 
 *        testing the concept
 */
//static KB *_kb;
static int _rule_cnt = 1;

KB*
jones_kb_init (char *id)
{
  KB    *_kb;
  char  str[1024];

  fprintf (stderr, "JONES Knowledge base initialising...\n");
  if ((_kb = malloc (sizeof(struct kb_t))) == NULL)
    {
      fprintf (stderr, "Cannot create a new knowledge base '%s' \n", id);
      return NULL;

    }
  _kb->bi.id = strdup (id);

  snprintf (str,1024, "%s_objs", id);
  _kb->obj = nyx_list_new (str, 8, sizeof (OBJECT));
  /* Create Lena expresions list -> Rules */
  snprintf (str,1024, "%s_rules", id);
  _kb->lena_rules = nyx_list_new (str, 8, sizeof(LENA_EXPR));

  return _kb;
}


FACT*
jones_kb_add_fact (KB *kb, char *id, int val, void *data)
{
  char      *on, *fn;
  OBJECT    *o;
  FACT      *f;

  if (!kb) return NULL;
  if (!id) return NULL;

  /* Parse id to retrieve name and fact name */
  on = strdup(id);
  if ((fn = strchr (on, '.')) == NULL)
    {
      fprintf (stderr, "Invalid identifier '%s'. Shall be OBJECT.FACT\n", id);
      goto clean_up;
    }
  *fn = 0;
  fn++;

  /* XXX: Object API to be updated */
  /* Find object */
  //if ((o = jones_obj_get (on)) == NULL)
  if ((o = (OBJECT*) nyx_list_find_item (kb->obj, on)) == NULL)
    {
      fprintf (stderr, "WARNING: Object '%s' does not exists. It will be created\n", on);
      o = jones_obj_new (on);
      //jones_obj_add (o);
      nyx_list_add_item (kb->obj, o);
    }

  /* Find Fact inside object */
  if ((f = jones_obj_get_fact (o, fn)) == NULL)
    {
      fprintf (stderr, 
	       "WARNING: Fact '%s' does not exist for object '%s'. It will be created\n", 
	       fn, on);
      f = jones_obj_get_or_create_fact (o, fn, FACT_UNKNOWN);
    }
  jones_fact_set (f, val);
  jones_fact_set_iter (f, 1);

  return f;

 clean_up:
  free (on);
  return NULL;
}

LENA_EXPR*
jones_kb_add_rule (KB *kb, char *str)
{
  char id[1024];
  LENA_EXPR *e;
  
  if (!kb) return NULL;
  if (!str) return NULL;

  if ((e = jones_lena_parse (kb, str)) == NULL)
    return NULL;

  snprintf (id, 1024, "%s_%03d", OBJ_ID(kb), _rule_cnt);
  _rule_cnt++;

  jones_lena_set_id (e, id);
  nyx_list_add_item (kb->lena_rules, e);

  return e;
}

FACT* 
jones_kb_find_fact (KB *kb, char *id)
{
  char      *on, *fn;
  OBJECT    *o;
  FACT      *f;

  if (!kb) return NULL;
  if (!id) return NULL;

  /* Parse id to retrieve name and fact name */
  on = strdup(id);
  if ((fn = strchr (on, '.')) == NULL)
    {
      fprintf (stderr, "Invalid identifier '%s'. Shall be OBJECT.FACT\n", id);
      goto clean_up;
    }
  *fn = 0;
  fn++;
  /* Find object */
  //if ((o = jones_obj_get (on)) == NULL)
  if ((o = (OBJECT*) nyx_list_find_item (kb->obj, on)) == NULL)
    {
      //fprintf (stderr, "Unknown Object '%s'\n", on);
      goto clean_up;
    }
  /* Find Fact inside object */
  if ((f = jones_obj_get_fact (o, fn)) == NULL)
    {
      //fprintf (stderr, "Unknown Fact '%s'\n", fn);
      goto clean_up;
    }
  return f;

 clean_up:
  free (on);
  return NULL;
}

int
jones_kb_run (KB *kb)
{
  int         i, j, n, fired, r;
  LENA_EXPR   *e;
  LENA_ITEM   *p;  

  if (!kb) return -1;


  fired = 0;
  n = kb->lena_rules->n;

  /* Force firing all rules */
#if 0
  for (i = 0; i < n; i++)
    {
      e = (LENA_EXPR*) kb->lena_rules->item[i];
      p = e->i;
      for (j = 0; j < e->n; j++)
	{
	  if ((p[j].op == OP_VAL)) ((FACT*)p[j].val)->iter = 1;
	}

    }

#endif
  for (i = 0; i < n; i++)
    {
      fired += jones_lena_run (kb->lena_rules->item[i], NULL);
    }

  printf ("KB(%s):%d rules fired out of %d\n", OBJ_ID(kb), fired, n);
  /* Mark all facts as processed */
  /* XXX: We should just keep a list of the fact that we have to re-arm*/
  for (i = 0; i < n; i++)
    {
      e = (LENA_EXPR*) kb->lena_rules->item[i];
      p = e->i;
      for (j = 0; j < e->n; j++)
	{
	  if ((p[j].op == OP_VAL)) ((FACT*)p[j].val)->iter = 0;
	}

    }

  return 0;
}


int        
jones_kb_dump_rules (KB *kb)
{
  int i, n;

  if (!kb) return -1;

  n = kb->lena_rules->n;
  printf ("KB(%s): %d rules\n", OBJ_ID(kb), n);
  printf ("-------------------------------------------------\n");
  for (i = 0; i < n; i++)
    {
      printf ("RULE%03d: %s -> [%s]\n", i, 
	      OBJ_ID(kb->lena_rules->item[i]),
	      ((LENA_EXPR*)kb->lena_rules->item[i])->str);
    }
  return 0;

}

int        
jones_kb_dump_objects (KB *kb)
{
  OBJECT  *o;
  int     i, j, n, n1, cnt;

  if (!kb) return -1;

  cnt = 0;
  n1 = kb->obj->n;
  printf ("KB(%s): %d objects\n", OBJ_ID(kb), n1);
  printf ("-------------------------------------------------\n");
  for (i = 0; i < n1; i++)
    {
      o = (OBJECT*) kb->obj->item[i];
      n = o->facts->n;
      cnt += n;
      if (n == 0) continue;
      printf ("OBJ%03d: (%s) [%d FACTS]\n", i, OBJ_ID(o), n);


      for (j = 0; j < n; j++)
	{
	  jones_fact_dump (o->facts->item[j]);
	}
      printf ("\n");
    }

  printf ("KB(%s): %d facts\n", OBJ_ID(kb), cnt);
  printf ("-------------------------------------------------\n");

  return 0;
}
