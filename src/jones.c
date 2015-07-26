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

#include "jones_const.h"

//#include "objs.h"
#include "kb.h"

static int auto_eval = 0;
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
  char p1[2048], p2[2048];
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
      jones_kb_dump_objects (_kb);
      jones_kb_dump_rules (_kb);
    }
  else if (!strncasecmp (cmd, "load", strlen ("load")))
    {
      char *str;

      str = cmd;
      str += strlen("load") + 1;
      str[strlen(str) - 1] = 0;

      cmd_load (str);

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

  return 1;
}

int
main (int argc, char *argv[])
{
  int  flag = 1;
  char cmd[2046];

  printf ("JONEs Version " JONES_VERSION "\n");

  _kb = jones_kb_init ("JONES");




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
	{
	  jones_kb_run (_kb);
	  jones_kb_dump_objects (_kb);
	}
    }

  return 0;
} 


