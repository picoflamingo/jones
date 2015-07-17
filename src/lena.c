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

#include "facts.h"
#include "lena.h"

static char *str_op[] = {"OOPS", "VAL", "NOT", "AND", "OR ", NULL};

int
_dump_stack (LENA_OP *o, int n)
{
  int i;
  for (i = 0; i < n; i++)
    {
      printf ("POS %02d: (%s:%d) %d\n",
	      i, str_op[o[i].op], o[i].op,
	      o[i].op == OP_VAL ? o[i].val : -1);

    }
  printf ("--------------------\n");
}

int       
jones_lena_expr_add_item (LENA_EXPR *e, int op, void *val)
{
  LENA_ITEM *aux;

  if (!e) return -1;
  //if (!val) return -1;

  if ((aux = realloc (e->i, sizeof(LENA_ITEM) * (e->n + 1))) == NULL)
    {
      fprintf (stderr, "Cannot reallocate expression\n");
      return -1;
    }
  e->i = aux;
  e->i[e->n].op = op;
  e->i[e->n].val = val;
  e->n++;
  printf ("Added %d(%p) . Total %d\n", op, val, e->n);
  return 0;

}

int       
jones_lena_run (LENA_EXPR *e)
{
  int       i, n, v, flag;
  LENA_ITEM *p;
  LENA_OP   o[1024];


  LENA_ITEM stack[100];
  if (!e) return -1;

  n = e->n;
  p = e->i;

  memset (o, 0, sizeof(LENA_OP) * n);
  printf ("Stack is %d items long\n", n);
  /* First resolve facts values */
  for (i = 0; i < n; i++)
    {
      o[i].op = p[i].op;

      if (p[i].op == OP_VAL)
	{
	  o[i].val = jones_fact_get (p[i].val);
	  /* If the fact is unknown we cannot evaluate the expression
	   *   We could recalculate for all possible values
	   */
	  if (o[i].val == FACT_UNKNOWN) return FACT_UNKNOWN;
	}
      else
	o[i].val == NULL;
      /* We can actually do the calculation here */
      /* This would probablty end up in a different function */
      /* -------------------------------------------------- */
      switch (o[i].op)
	{
	case OP_NOT:
	  {
	    o[i].op = OP_VAL;
	    o[i].val = o[i - 1].val ? 0 : 1;
	    break;
	  }
	case OP_AND:
	  {
	    o[i].op = OP_VAL;
	    o[i].val = o[i - 1].val && o[i - 2].val;
	    break;
	  }
	case OP_OR:
	  {
	    o[i].op = OP_VAL;
	    o[i].val = o[i - 1].val || o[i - 2].val;
	    break;
	  }

	default:
	case OP_VAL:
	  break;
	  
	}
      
      /* -------------------------------------------------- */
      _dump_stack (o, n);
    }

  
  return 0;
}

LENA_EXPR* 
jones_lena_parse (char *s)
{
  LENA_EXPR *e;
  FACT      *f;
  char      *str;
  char      *op1, *op2, *op;
  if (!str) return NULL;

  if ((e = malloc (sizeof(LENA_EXPR))) == NULL)
    {
      fprintf (stderr, "Cannot create lena expression parsin: '%s'\n", str);
      return NULL;
    }
  e->n = 0;
  e->i = NULL;
  str = strdup (s);
  /* Parse */
  op = strtok (str, " ");
  while (op)
    {
      printf ("Parsing '%s'\n",op);
      if (op[0] == '!')
	jones_lena_expr_add_item (e, OP_NOT, 0);
      else if (op[0] == '&')
	jones_lena_expr_add_item (e, OP_AND, 0);
      else if (op[0] == '|')
	jones_lena_expr_add_item (e, OP_OR, 0);
      else /* In any othercase this is a fact */
	{
	  /* Locate fact and add */
	  /* FIXME: For testing adding fake facts */
	  f = jones_fact_new (op);
	  jones_fact_set (f, 1);
	  jones_lena_expr_add_item (e, OP_VAL, f);
	}
      op = strtok (NULL, " ");
    }
  free (str);
  return e;
}
