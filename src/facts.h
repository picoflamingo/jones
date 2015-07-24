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


#ifndef FACTS_H
#define FACTS_H

#include <nyx_list.h>


typedef void *_OBJ;

#define FACT_FALSE 0
#define FACT_TRUE  1
#define FACT_UNKNOWN 3

#define CAN_RESOLVE_AND  FACT_UNKNOWN
#define CAN_RESOLVE_OR   FACT_UNKNOWN + 1


typedef struct fact_t
{
  NYX_BASIC_ITEM bi;
  void           *obj;   /* Object this fact is related to */
  NYX_LIST       *olist; /* List of objects related to the fact.*/
  int            value;
  int            fire;
  int            iter;
} FACT;

#ifdef __cplusplus
extern "C" {
#endif


  int   jones_fact_dump (FACT *f);

  FACT* jones_fact_new (char *id);
  int   jones_fact_free (FACT *f);

  char *jones_fact_str (int val);
  int   jones_fact_set (FACT *f, int val);
  int   jones_fact_set1 (FACT *f, int val);
  int   jones_fact_set_iter (FACT *f, int val);
  int   jones_fact_get (FACT *f);
  int   jones_fact_set_obj (FACT *f, _OBJ o);
  int   jones_fact_add_obj (FACT *f, _OBJ o);
  int   jones_fact_set_robj (FACT *f, _OBJ o, int i);
  _OBJ  jones_fact_get_robj (FACT *f, int i);
#ifdef __cplusplus
}
#endif


#endif
