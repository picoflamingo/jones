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
//#define DEBUG2

static char *str_op[] = {"OOPS", "VAL", "NOT", "AND", "OR ", "SET", 
			 "TRUE", "FALSE", "UNKNOWN", 
			 "STR", "MSG", "VAR",  "REF", "VREF", 
			 "CONST", NULL};

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

LENA_VAR*       
jones_lena_expr_add_var (LENA_EXPR *e, char *str)
{
  LENA_VAR *aux;
  char     *var;

  if (!e) return -1;
  if (!str) return -1;
  if ((var = strchr (str, '.')) == NULL)
    return NULL;
  *var = 0;
  var++;
  if ((aux = realloc (e->v, sizeof(LENA_VAR) * (e->npar + 1))) == NULL)
    {
      fprintf (stderr, "Cannot reallocate expression\n");
      return -1;
    }
  e->v = aux;
  e->v[e->npar].id = strdup (str);
  e->v[e->npar].fact = strdup (var);

  aux = &e->v[e->npar];
  fprintf (stderr, "Variable %d:: Id:'%s' Fact:'%s'\n", 
	   e->npar, aux->id, aux->fact);

  jones_lena_expr_add_item (e, OP_VAR, e->npar);
  e->npar++;



  return 0;

}




int        
jones_lena_run_with_par (KB *kb, LENA_EXPR *e, FACT *fact, int* result)
{
  int         i, n, v, flag, r, fire;
  LENA_ITEM   *p;
  LENA_OP     o[1024];

  if (!e) return -1;
  if (!e->npar && e->fired)
    {
      fprintf (stderr, "INFO: Rule '%s' already fired\n", e->str);
      return 0;
    }
  /* XXX: If expression does not have a parameter we may fallback
   * to jones_lena_run 
   */
  n = e->n;
  p = e->i;

  r = 0;
  memset (o, 0, sizeof(LENA_OP) * n); 
#ifdef DEBUG1
  printf ("Stack is %d items long for rule %s\n", n, OBJ_ID(e));
#endif

  /* XXX: For the time being we are calling this function with
   *      a FACT parameter so the rule is always fired 
   */

  /* XXX: First resolve facts values to deal with iter field
   *      Then execute stack
   */
  flag = 0; // Deal with UNKNOWNS
  fire = 0; // Shall we run the rule??
  for (i = 0; i < n; i++)
    {
      o[i].op = p[i].op;
      //o[i].val = p[i].val;

      if (o[i].op == OP_VAL)
	{
	  o[i].val = jones_fact_get (p[i].val);
	  if (o[i].val == FACT_UNKNOWN) flag = 1;
	  fire += ((FACT*)p[i].val)->iter;
	}
      else if (o[i].op == OP_CONST)
	o[i].val = p[i].val;
      else if (o[i].op == OP_REF)
	o[i].val = p[i].val;
      else if (o[i].op == OP_VAR)
	{
	  OBJECT  *_obj = NULL;

	  if (e->v[(int)p[i].val].id[0] == 'X')
	    _obj = fact->obj;
	  else 
	    {
	      fprintf (stderr, "Only X variable supported. Aborting...\n");
	      return -1;
	    }
	  /* FIXME: First check fact and then get iter....*/
	  if ((o[i].val = 
	       jones_obj_get_fact_val (_obj, 
				       e->v[(int)p[i].val].fact)) < 0)
	    return -1;
	  fire += jones_obj_get_fact (_obj, e->v[(int)p[i].val].fact)->iter;
	  if (o[i].val == FACT_UNKNOWN) flag = 1;
	}
      else if (o[i].op == OP_VREF)
	{
	  OBJECT  *_obj = NULL;
	  FACT    *f;

	  if (e->v[(int)p[i].val].id[0] == 'X')
	    _obj = fact->obj;
	  else 
	    {
	      fprintf (stderr, "Only X variable supported. Aborting...\n");
	      return -1;
	    }
	  o[i].val = jones_obj_get_or_create_fact (_obj, e->v[(int)p[i].val].fact,
					    FACT_UNKNOWN);
	}

      else
	o[i].val = 0;
    }

  if (fire == 0) 
    {
      fprintf (stderr, "INFO:Rule '%s' not fired. No involved fact updated\n", e->str);
      return 0;
    }
  e->fired = 1;
  for (i = 0; i < n; i++)
    {
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
		  {
		    // TRUE & ?? = ??
		    if(result) *result = FACT_UNKNOWN;
		    return 1; 
		  }
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
		  {
		    if (result) *result = FACT_UNKNOWN;
		    return 1;
		  }
	      }

	    o[i].val = o[i - 1].val || o[i - 2].val;
	    flag = 0;
	    break;
	  }
	case OP_MSG:
	  {
	    if (o[i - 2].val == FACT_TRUE)
	      printf ("MSG:%s\n", (char*) p[i - 1].val);

	    o[i].op = OP_VAL;
	    o[i].val = o[i - 2].val;
	    /* Rule fired */
	    r = 1;
	    break;
	  }
	case OP_SET:
	  {
	    o[i].op = OP_VAL;
	    v = jones_fact_get(o[i - 1] .val);
	    o[i].val = o[i -2].val;
	    if (v != o[i -2].val)
	      {
		if (((FACT*)o[i - 1].val)->iter == 2)
		  {
		    printf ("RULE '%s': [CONTRADICTION] Sets previously set "
			    "value '%s.%s' to '%s'\n",
			    OBJ_ID(e), 
			    OBJ_ID(((FACT*)o[i - 1].val)->obj),
			    OBJ_ID(o[i - 1].val),
			    jones_fact_str (o[i - 2].val));
		  }
		jones_fact_set (o[i - 1].val, o[i - 2].val);
		jones_fact_set_iter (o[i - 1].val, 2);

		printf ("RULE '%s': Sets '%s.%s' to '%s'\n",
			OBJ_ID(e), 
			OBJ_ID(((FACT*)o[i - 1].val)->obj),
			OBJ_ID(o[i - 1].val),

			jones_fact_str (o[i - 2].val));
		/* We signal that something has been changed */
		r = 1;

	      }
	    flag = 0;
	    break;
	  }

	default:
	case OP_STR:
	case OP_VAL:
	case OP_REF:
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

  if (result) *result = o[i - 1].val;
  return r;

}




LENA_EXPR* 
jones_lena_parse (KB *kb, char *s)
{
  LENA_EXPR *e;
  FACT      *f;
  char      *str;
  char      *op;
  char      *txt = NULL;
  int       len;

  if (!s) return NULL;

  if ((e = malloc (sizeof(LENA_EXPR))) == NULL)
    {
      fprintf (stderr, "Cannot create lena expression parsin: '%s'\n", str);
      return NULL;
    }
  e->bi.id = NULL;
  e->n = 0;
  e->npar = 0;
  e->i = NULL;
  e->v = NULL;
  e->fired = 0;

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
	{
	  if (e->i[e->n - 1].op == OP_VAL)
	    e->i[e->n - 1].op = OP_REF;
	  else if (e->i[e->n - 1].op == OP_VAR)
	    e->i[e->n - 1].op = OP_VREF;
	  jones_lena_expr_add_item (e, OP_SET, 0);
	  /* Previously entered op has to be a reference */

	}
      else if (op[0] == 'T')
	  jones_lena_expr_add_item (e, OP_CONST, FACT_TRUE);
      else if (op[0] == 'F')
	jones_lena_expr_add_item (e, OP_CONST, FACT_FALSE);
      else if (op[0] == '$')
	{
	  /* Create variable object and add to expression */

	  jones_lena_expr_add_var (e, op + 1);
	  
	  
	  
	}
      else if (op[0] == '(')
	{
	  char *aux = op + 1;
	  op[strlen(op)] = ' ';
	  if ((aux = strchr (aux, ')')) == NULL)
	    {
	      fprintf (stderr, "String not finished...\n");
	      free (str);
	      free (e);
	      return NULL;
	    }

	  *aux = 0;
	  jones_lena_expr_add_item (e, OP_STR, strdup (op + 1));
	  op = strtok (aux + 1, " ");
	  continue;

	}
      else if (!strcasecmp (op, "say"))
	{
	  jones_lena_expr_add_item (e, OP_MSG, 0);
	}
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
  fprintf (stderr, "Rule uses %d variables\n", e->npar);
  free (str);
  return e;
}
