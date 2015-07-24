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
static KB *_kb;
static int _rule_cnt = 1;

int
jones_kb_init (void)
{
  fprintf (stderr, "JONES Knowledge base initialising...\n");
  jones_obj_init (); /* XXX: FIXME allow multiple object list...
		      *      will have to go into KB structure */

  /* Create Lena expresions list -> Rules */
  /* XXX: Memory checks below */
  _kb = malloc (sizeof(struct kb_t));
  _kb->bi.id = strdup ("baseKB");
  _kb->lena_rules = nyx_list_new ("baseKB_lena_rules", 8, sizeof(LENA_EXPR));

  return 0;
}


int 
jones_kb_add_fact (char *id, int val, void *data)
{
  char      *on, *fn;
  OBJECT    *o;
  FACT      *f;

  if (!id) return -1;

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
  if ((o = jones_obj_get (on)) == NULL)
    {
      fprintf (stderr, "Object '%s' does not exists. Creating it\n", on);
      o = jones_obj_new (on);
      jones_obj_add (o);
    }

  /* Find Fact inside object */
  if ((f = jones_obj_get_fact (o, fn)) == NULL)
    {
      fprintf (stderr, "Fact '%s' does not exist for object '%s'. Creating it\n", 
	       fn, on);
      f = jones_obj_get_or_create_fact (o, fn, FACT_UNKNOWN);
    }
  jones_fact_set (f, val);
  jones_fact_set_iter (f, 1);
  return 0;

 clean_up:
  free (on);
  return -1;
}

LENA_EXPR*
jones_kb_add_rule (char *str)
{
  char id[1024];
  LENA_EXPR *e;
  
  if ((e = jones_lena_parse (str)) == NULL)
    return NULL;

  snprintf (id, 1024, "%s_%03d", OBJ_ID(_kb), _rule_cnt);
  _rule_cnt++;

  jones_lena_set_id (e, id);
  nyx_list_add_item (_kb->lena_rules, e);

  return e;
}

FACT* 
jones_kb_find_fact (char *id)
{
  char      *on, *fn;
  OBJECT    *o;
  FACT      *f;

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
  if ((o = jones_obj_get (on)) == NULL)
    {
      fprintf (stderr, "Unknown Object '%s'\n", on);
      goto clean_up;
    }
  /* Find Fact inside object */
  if ((f = jones_obj_get_fact (o, fn)) == NULL)
    {
      fprintf (stderr, "Unknown Fact '%s'\n", fn);
      goto clean_up;
    }
  return f;

 clean_up:
  free (on);
  return NULL;
}

int
jones_kb_run (void)
{
  int i, n;
  
  n = _kb->lena_rules->n;
  for (i = 0; i < n; i++)
    {
      jones_lena_run (_kb->lena_rules->item[i]);
    }
  return 0;
}
