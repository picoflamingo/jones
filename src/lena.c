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
#include "facts.h"
#include "lena.h"

//#define DEBUG1


static char *str_op[] = {"OOPS", "VAL", "NOT", "AND", "OR ", "SET", 
			 "TRUE", "FALSE", "UNKNOWN", NULL};

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
  return 0;
}

int
jones_lena_free (LENA_EXPR *e)
{
  if (!e) return -1;

  if (e->i) free (e->i);
  free (e);

  return 0;
}

int        
jones_lena_set_id (LENA_EXPR *e, char *n)
{
  if (!e) return -1;
  if (!n) return -1;

  if (e->bi.id) free (e->bi.id);
  e->bi.id = strdup (n);

  return 0;
}

int       
jones_lena_expr_add_item (LENA_EXPR *e, int op, void *val)
{
  LENA_ITEM *aux;

  if (!e) return -1;

  if ((aux = realloc (e->i, sizeof(LENA_ITEM) * (e->n + 1))) == NULL)
    {
      fprintf (stderr, "Cannot reallocate expression\n");
      return -1;
    }
  e->i = aux;
  e->i[e->n].op = op;
  e->i[e->n].val = val;
  e->n++;

  return 0;

}

int       
jones_lena_run (LENA_EXPR *e)
{
  int       i, n, v, flag;
  LENA_ITEM *p;
  LENA_OP   o[1024];


  if (!e) return -1;

  n = e->n;
  p = e->i;

  memset (o, 0, sizeof(LENA_OP) * n);
#ifdef DEBUG1
  printf ("Stack is %d items long for rule %s\n", n, OBJ_ID(e));
#endif
  /* Check if the rule has to be fired... (FACT updated)*/

  for (i = 0; i < n; i++)
    {
      //if ((p[i].op == OP_VAL) && (((FACT*)p[i].val)->iter == 1)) 
      if ((p[i].op == OP_VAL) && (((FACT*)p[i].val)->iter > 0)) 
	{
#ifdef DEBUG1
	  printf ("FIRE: %d Fact: %s iter:%d\n", 
		  i, 
		  OBJ_ID(((FACT*)p[i].val)),
		  ((FACT*)p[i].val)->iter
		  );
#endif
	  break;
	}
      if ((i < (n - 1) && (p[i + 1].op == OP_SET))) 
	{
#ifdef DEBUG1
	  printf ("FIRE: OP_SET %d Fact: %s iter:%d\n", 
		  i, 
		  OBJ_ID(((FACT*)p[i].val)),
		  ((FACT*)p[i].val)->iter
		  );
#endif
	  break;
	}
    }

  if (i >= n - 2)
    {
#ifdef DEBUG1
      fprintf (stderr, "NOTE: RULE %s nothing to do\n", OBJ_ID(e));
#endif
      return 0;
    }
  /* First resolve facts values */
  flag = 0; // Deal with UNKNOWNS
  for (i = 0; i < n; i++)
    {
      o[i].op = p[i].op;

      if (p[i].op == OP_VAL)
	{
	  o[i].val = jones_fact_get (p[i].val);
	  if (o[i].val == FACT_UNKNOWN) flag = 1;
	}
      else if (p[i].op == OP_TRUE)
	{
	  o[i].val = FACT_TRUE;
	}
      else if (p[i].op == OP_FALSE)
	{
	  o[i].val = FACT_FALSE;
	}

      else
	o[i].val = 0;
      /* We can actually do the calculation here */
      /* This would probably end up in a different function */
      /* -------------------------------------------------- */
      switch (o[i].op)
	{
	case OP_NOT:
	  {

	    o[i].op = OP_VAL;
	    if (flag) o[i].val = FACT_UNKNOWN;
	    else
	      o[i].val = o[i - 1].val ? 0 : 1;
	    flag = 0;
	    break;
	  }
	case OP_AND:
	  {	    
	    v = o[i - 1].val + o[i - 2].val;
	    o[i].op = OP_VAL;

	    if (flag)
	      {
		if (v == CAN_RESOLVE_AND) // FALSE & ?? = FALSE
		  {
		    flag = o[i].val = 0;
		    break;
		  }
		else
		  return FACT_UNKNOWN; // TRUE & ?? = ??
	      }
	    o[i].val = o[i - 1].val && o[i - 2].val;
	    flag = 0;
	    break;
	  }
	case OP_OR:
	  {
	    v = o[i - 1].val + o[i - 2].val;
	    o[i].op = OP_VAL;
	    if (flag)
	      {
		if (v == CAN_RESOLVE_OR)
		  {
		    flag = o[i].val = 1;
		    break;
		  }
		else
		  return FACT_UNKNOWN;
	      }

	    o[i].val = o[i - 1].val || o[i - 2].val;
	    flag = 0;
	    break;
	  }
	case OP_SET:
	  {
	    o[i].op = OP_VAL;
	    v = jones_fact_get(p[i - 1] .val);
	    if (v != o[i -2].val)
	      {
		jones_fact_set (p[i - 1].val, o[i - 2].val);
		jones_fact_set_iter (p[i - 1].val, 2);
		printf ("RULE '%s': Sets '%s.%s' to '%s'\n",
			OBJ_ID(e), 
			OBJ_ID(((FACT*)p[i - 1].val)->obj),
			OBJ_ID(p[i - 1].val),
			jones_fact_str (o[i - 2].val));
#ifdef DEBUG1
		      jones_obj_dump ();
		      printf ("--------------------------\n");
#endif
	      }
	    flag = 0;
	    break;
	  }

	default:
	case OP_VAL:
	  break;
	  
	}
      
      /* -------------------------------------------------- */
#ifdef DEBUG1
      _dump_stack (o, n);
#endif
    }
#ifdef DEBUG1
  printf ( "Result %d\n", o[i - 1].val);
#endif

  /* FIXME: THis has to be done after processing all rules...*/
#if 0
  for (i = 0; i < n; i++)
    if ((p[i].op == OP_VAL)) 		
      jones_fact_set_iter (p[i].val, 0);
#endif  
  return o[i - 1].val;
}

LENA_EXPR* 
jones_lena_parse (KB *kb, char *s)
{
  LENA_EXPR *e;
  FACT      *f;
  char      *str;
  char      *op;
  if (!s) return NULL;

  if ((e = malloc (sizeof(LENA_EXPR))) == NULL)
    {
      fprintf (stderr, "Cannot create lena expression parsin: '%s'\n", str);
      return NULL;
    }
  e->bi.id = NULL;
  e->n = 0;
  e->i = NULL;
  e->str = strdup (s);

  str = strdup (s);
  /* Parse */
  op = strtok (str, " ");
  while (op)
    {
      if (op[0] == '!')
	jones_lena_expr_add_item (e, OP_NOT, 0);
      else if (op[0] == '&')
	jones_lena_expr_add_item (e, OP_AND, 0);
      else if (op[0] == '|')
	jones_lena_expr_add_item (e, OP_OR, 0);
      else if (op[0] == '=')
	jones_lena_expr_add_item (e, OP_SET, 0);
      else if (op[0] == 'T')
	jones_lena_expr_add_item (e, OP_TRUE, 0);
      else if (op[0] == 'F')
	jones_lena_expr_add_item (e, OP_FALSE, 0);

      else /* In any othercase this is a fact */
	{
	  /* Locate fact and add */
	  if ((f = jones_kb_find_fact (kb, op)) == NULL)
	    {
	      
#if 0
	      /* If fact does not exoist produce a syntax error*/
	      fprintf (stderr, "Syntax error (%s)\n", op);
	      free (str);
	      return NULL;
#else
	      fprintf (stderr, "WARNING: Fact '%s' does not exist. It will be created\n", op);
	      f = jones_kb_add_fact (kb, op, FACT_UNKNOWN, NULL);
#endif
	    }
	  jones_lena_expr_add_item (e, OP_VAL, f);
	}
      op = strtok (NULL, " ");
    }
  free (str);
  return e;
}
