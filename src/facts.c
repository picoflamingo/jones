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


static char* fact_str[] = {"FALSE", "TRUE", "OOPS", "?", NULL};
static int _iter = 1;

char *
jones_fact_str (int val)
{
  return fact_str[val];
}

int   
jones_fact_dump (FACT *f)
{
  int    i;

  if (!f) return -1;
  printf ("FACT : (%s) %s ", 
	  OBJ_ID(f->obj), OBJ_ID(f));
  /* Print objects */
  if (f->olist->n)
    {
      for (i = 0; i < f->olist->n - 1; i++)
	printf ("(%s),", OBJ_ID(f->olist->item[i]));
      printf ("(%s)", OBJ_ID(f->olist->item[i]));
    }
  printf (" -> %s (iter:%d)\n", fact_str[f->value], f->iter);
  /* print value */
  return 0;
}


FACT  *
jones_fact_new (char *id)
{
  FACT *f;

  if (!id) return NULL;

  if ((f = (FACT*) malloc (sizeof(FACT))) == NULL)
    {
      fprintf (stderr, "Cannot create fact '%s'\n", id);
      return NULL;
    }
  f->bi.id = strdup (id);
  f->obj = NULL;
  f->olist = nyx_list_new ("fact_obj_list", 10, sizeof(void*));
  f->value = FACT_UNKNOWN;
  f->fire = 0;
  f->iter = _iter - 1;
  return f;
}

int   
jones_fact_free (FACT *f)
{
  if (!f) return -1;

  if (f->olist) nyx_list_free (f->olist);
  if (f->bi.id) free (f->bi.id);
  free (f);

  return 0;
}

int   
jones_fact_set (FACT *f, int val)
{
  if (!f) return -1;

  f->value = val;

  return 0;
}

int   
jones_fact_set1 (FACT *f, int val)
{
  if (!f) return -1;
  if (f->iter < _iter)
    {
#ifdef DEBUG1
      printf ( "[%s] Setting ::", __FUNCTION__);
      jones_fact_dump (f);
#endif
      f->value = val;
    }
#ifdef DEBUG1
  else
    {
      printf ( "[%s] Skipping ::", __FUNCTION__);
      jones_fact_dump (f);
    }
#endif

  return 0;
}


int   
jones_fact_set_iter (FACT *f, int val)
{
  if (!f) return -1;

  f->iter = val;
  _iter = val;

  return 0;
}

int   
jones_fact_get (FACT *f)
{
  if (!f) return -1;

  return f->value;
}

int   
jones_fact_set_obj (FACT *f, _OBJ o)
{
  if (!f) return -1;
  if (!o) return -1;

  f->obj = o;

  return 0;
}

int   
jones_fact_add_obj (FACT *f, _OBJ o)
{
  if (!f) return -1;
  if (!o) return -1;

  if (!nyx_list_find_item (f->olist, OBJ_ID(o)) )
      nyx_list_add_item (f->olist, o);

  return 0;

}

int   
jones_fact_set_robj (FACT *f, _OBJ o, int i)
{
  if (!f) return -1;
  if (!o) return -1;

  if (i < f->olist->n)
    {
      f->olist->item[i] = o;
      return 0;
    }
  return -1;

}

_OBJ
jones_fact_get_robj (FACT *f, int i)
{
  if (!f) return -1;

  if (i < f->olist->n)
    {
      return f->olist->item[i];
    }
  return NULL;

}
