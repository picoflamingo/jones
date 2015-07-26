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
#ifndef KB_H
#define KB_H

#include <nyx_list.h>
#include "lena.h"
#include "facts.h"
#include "objs.h"

typedef struct kb_t
{
  NYX_BASIC_ITEM bi;
  NYX_LIST       *lena_rules;
  NYX_LIST       *obj;
  /* XXX: Add object list whenever object interface is updated */
} KB;

#ifdef __cplusplus
extern "C" {
#endif

  KB*        jones_kb_init (char *id);
  FACT*      jones_kb_find_fact (KB *kb, char *id);
  FACT*      jones_kb_add_fact (KB *kb, char *id, int val, void *data);
  LENA_EXPR* jones_kb_add_rule (KB *kb, char *str);
  int        jones_kb_run (KB *kb);

  int        jones_kb_dump_objects (KB *kb);
  int        jones_kb_dump_rules (KB *kb);
#ifdef __cplusplus
}
#endif

#endif
