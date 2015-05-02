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
#ifndef RULES_H
#define RULES_H

#include <nyx_list.h>
#include "facts.h"
#include "objs.h"

#define RULE_FIRED 1
#define RULE_NOT_FIRED 0

struct rule_t;

typedef int (*RULE_IMP) (struct rule_t*r, OBJECT *o, FACT *f);

typedef struct rule_t
{
  NYX_BASIC_ITEM  bi;
  char            *ffact;
  RULE_IMP        f;
  void            *data;
} RULE;

#ifdef __cplusplus
extern "C" {
#endif

  int   jones_rule_init (void);
  int   jones_rule_dump (void);
  int   jones_resolve (void);

  int   jones_rule_add (RULE *r);
  int   jones_rule_set_data (RULE *r, void *data);
  void* jones_rule_get_data (RULE *r);

  RULE* jones_rule_new (char *id, RULE_IMP f);
  int   jones_rule_add_firing_fact (RULE *r, char *fid);
  int   jones_rule_eval (RULE *r, OBJECT *o);
  int   jones_rule_resolve_fact (OBJECT *o, char *fid);
  int   jones_rule_full_eval ();
#ifdef __cplusplus
}
#endif

#endif
