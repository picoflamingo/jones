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

// export PYTHONPATH=.

#include "ipython.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objs.h"
#include "facts.h"
#include "rules.h"


#define VERSION "0.1"

static int auto_eval = 0;

int
parse (char *cmd)
{
  char p1[2048], p2[2048], p3[2048], p4[2048];

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
    }
  else if (!strncasecmp (cmd, "obj", strlen ("obj")))
    {
      sscanf (cmd + strlen ("obj "), "%s", p1);
      jones_obj_add (jones_obj_new (p1));
    }
  else if (!strncasecmp (cmd, "fact2", strlen ("fact2")))
    {
      sscanf (cmd + strlen ("fact2 "), "%s %s %s %s", p1, p2, p4, p3);
      printf ("  Adding fact (%s) to object (%s) with value '%s'\n", p1, p2, p3);
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
      jones_fact_add_obj (f, o1);
      //jones_obj_add_fact (o, f);

      //jones_obj_dump ();
    }
  else if (!strncasecmp (cmd, "fact", strlen ("fact")))
    {
      sscanf (cmd + strlen ("fact "), "%s %s %s", p1, p2, p3);
      printf (" < Adding/Setting  fact (%s) to object (%s) with value '%s'\n", p1, p2, p3);
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
      //jones_fact_add_obj (f, o);
      //jones_obj_add_fact (o, f);

      //jones_obj_dump ();
    }

  return 1;
}

int
main (int argc, char *argv[])
{
  RULE     *r;

  printf ("JONEs Version " VERSION "\n");
  printf ("+ JONES initialisation....");
  jones_obj_init ();
  jones_rule_init ();
  jones_python_init ();

  printf ("DONE\n");
  if (argc == 1)
    jones_python_load_script ("jones");
  else
    jones_python_load_script (argv[1]);
  
  printf ("......................................\n");
  jones_python_populate ();
  printf ("......................................\n");
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
  while (flag)
    {
      memset (cmd, 0, 2046);
      printf ("\033[1;32mjones > ");
      printf ("\033[1;33m");
      fgets (cmd, 2046, stdin);
      printf ("\033[0m");
      flag = parse (cmd);
      if (auto_eval)
	n = jones_rule_full_eval ();
    }

  return 0;
} 


