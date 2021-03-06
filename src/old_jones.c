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


// export PYTHONPATH=.

#include "ipython.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objs.h"
#include "facts.h"
#include "rules.h"
#include "lena.h"
#include "kb.h"

/*FIXME: Get this from autools */
#define VERSION "0.2.0"

static int auto_eval = 0;
static int rule_id = 100;

static KB *_kb = NULL;

#define LINE_MAX_LEN 2048

/* Local Prototypes */
int parse (char *cmd);
int cmd_load (char *fname);


int
cmd_load (char *fname)
{
  int  line = 0;
  char buffer[LINE_MAX_LEN];
  FILE *f;

  if ((f = fopen (fname, "rt")) == NULL)
    {
      fprintf (stderr, "Cannot open file '%s'\n", fname);
      return -1;
    }

  while (!feof (f))
    {
      line++;
      memset (buffer, 0, LINE_MAX_LEN);
      if ((fgets (buffer, LINE_MAX_LEN, f)) == NULL)
	{
	  if (feof (f)) break;
	  else
	    fprintf (stderr, "%s: ERROR: reading file...\n", __FUNCTION__);
	}

      if (buffer[0] == '#') continue;
      if (strlen (buffer) == 0) continue;

      parse (buffer);

    }

  fclose (f);
  return 0;
}


int
parse (char *cmd)
{
  char p1[2048], p2[2048], p3[2048], p4[2048];
  int  v;

  if (!strncasecmp (cmd, "exit", strlen ("exit")) || cmd[0] == 'q')
    {
      printf ("Exiting...\n");
      return 0;
    }
  else if (!strncasecmp (cmd, "auto", strlen ("auto")))
    {
      auto_eval ^= 1;
      printf ("Auto evaluation: %s\n", auto_eval ? "ON" : "OFF");
    }

  else if (!strncasecmp (cmd, "list", strlen ("list")))
    {
      jones_obj_dump ();
      jones_kb_dump_rules (_kb);
    }
  else if (!strncasecmp (cmd, "obj", strlen ("obj")))
    {
      sscanf (cmd + strlen ("obj "), "%s", p1);
      jones_obj_add (jones_obj_new (p1));
      printf ("JONES: Object '%s' created\n", p1);
    }
  else if (!strncasecmp (cmd, "fact2", strlen ("fact2")))
    {
      sscanf (cmd + strlen ("fact2 "), "%s %s %s %s", p1, p2, p4, p3);
      printf ("JONES:Adding fact (%s) to object (%s) with value '%s'\n", 
	      p1, p2, p3);
      OBJECT *o = jones_obj_get (p1);
      if (!o)
	{
	  fprintf (stderr, "Object %s does not exit\n", p1);
	  return 1;
	}
      OBJECT *o1 = jones_obj_get (p4);
      if (!o)
	{
	  fprintf (stderr, "Object %s does not exit\n", p4);
	  return 1;
	}
      FACT *f = jones_obj_get_or_create_fact (o, p2, FACT_UNKNOWN);

      if (p3[0] == 'T' || p3[0] == 't') jones_fact_set (f, FACT_TRUE);
      else if (p3[0] == 'F' || p3[0] == 'f') jones_fact_set (f, FACT_FALSE);
      else jones_fact_set (f, FACT_UNKNOWN);

      jones_fact_set_iter (f, jones_rule_get_iter());
      if (f->olist->n == 0)
	jones_fact_add_obj (f, o1);
      else
	jones_fact_set_robj (f, o1,0);
    }
  else if (!strncasecmp (cmd, "fact", strlen ("fact")))
    {
      sscanf (cmd + strlen ("fact "), "%s %s %s", p1, p2, p3);
      printf ("JONES:Adding/Setting  fact (%s) to object (%s) with value '%s'\n", p1, p2, p3);
      OBJECT *o = jones_obj_get (p1);
      if (!o)
	{
	  fprintf (stderr, "Object %s does not exit\n", p1);
	  return 1;
	}
      FACT *f = jones_obj_get_or_create_fact (o, p2, FACT_UNKNOWN);

      if (p3[0] == 'T' || p3[0] == 't') jones_fact_set (f, FACT_TRUE);
      else if (p3[0] == 'F' || p3[0] == 'f') jones_fact_set (f, FACT_FALSE);
      else jones_fact_set (f, FACT_UNKNOWN);

      jones_fact_set_iter (f, jones_rule_get_iter());
    }
  else if (!strncasecmp (cmd, "ask", strlen ("ask")))
    {
      jones_rule_ask ();
      jones_obj_dump ();
    }
  else if (!strncasecmp (cmd, "load", strlen ("load")))
    {
      char *str;

      str = cmd;
      str += strlen("load") + 1;
      str[strlen(str) - 1] = 0;

      cmd_load (str);

    }
  else if (!strncasecmp (cmd, "srule", strlen ("srule")))
    {
      char rname[1024], *str;
      RULE *r;

      str = cmd;
      str += strlen("srule") + 1;
      str[strlen(str) - 1] = 0;
      snprintf (rname, 1024, "srule%03d", rule_id);

      rule_id++;
      r = jones_rule_new (rname, jones_rule_simple_rule);
      jones_rule_add_srule_data (r, str);
      printf ("JONES:New simple rule created with id '%s'\n", rname);

    }
  /* --------------------------------------------------*/ 
  /* XXX: New functions to migrate to the KB interface */
  else if (!strncasecmp (cmd, "lena", strlen ("lena")))
    {
      LENA_EXPR* e;
      cmd[strlen(cmd) - 1] = 0;

      if ((e = jones_kb_add_rule (_kb, cmd + strlen("lena "))))
	{
	  printf ("Rule %s created\n", OBJ_ID(e));
	}
    }
  else if (!strncasecmp (cmd, "set", strlen ("set")))
    {
      sscanf (cmd + strlen ("set "), "%s %s", p1, p2);
      if (p2[0] == 'T' || p2[0] == 't') v = FACT_TRUE;
      else if (p2[0] == 'F' || p2[0] == 'f') v = FACT_FALSE;
      else v = FACT_UNKNOWN;

      jones_kb_add_fact (_kb, p1, v, NULL);
    }

  else if (!strncasecmp (cmd, "run", strlen ("run")))
    {
      jones_kb_run (_kb);
    }
  else if (cmd[strlen(cmd) - 2] == '?') // FACT Query
    {
      jones_obj_fact_query (cmd);
    }

  return 1;
}

int
main (int argc, char *argv[])
{
  setenv ("PYTHONPATH", ".", 1);
  printf ("JONEs Version " VERSION "\n");
  printf ("+ JONES initialisation....");
  jones_obj_init ();
  jones_rule_init ();
  jones_python_init ();

  _kb = jones_kb_init ("JONES");

  printf ("DONE\n");
  if (argc > 1)
    jones_python_load_script (argv[1]);
  
  jones_python_populate ();
  jones_obj_dump ();

  /* Initial evaluation of rules */
  int n, n1;
  n1 = 0;
  do 
    {
      n = jones_rule_full_eval ();
      if (n == n1) break;
      n1 = n;
    } while (1);

  int flag = 1;
  char cmd[2046];


  /* Test endss */
  while (flag)
    {
      memset (cmd, 0, 2046);
      printf ("\033[1;32mjones > ");
      printf ("\033[1;33m");
      if ((fgets (cmd, 2046, stdin)) == NULL) exit (1);
      printf ("\033[0m");
      flag = parse (cmd);
      if (auto_eval)
	n = jones_rule_full_eval ();
    }

  return 0;
} 


