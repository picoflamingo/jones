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

#ifndef OBJECTS_H
#define OBJECTS_H

//#include "list.h"
#include "nyx_list.h"
#include "facts.h"


typedef struct object_t
{
  NYX_BASIC_ITEM    bi;
  NYX_LIST          *facts; /* List of associated facts*/
  // We might add properties/Class later.
} OBJECT;



#ifdef __cplusplus
extern "C" {
#endif

  int         jones_obj_init (void);
  int         jones_obj_dump (void);
  NYX_LIST*   jones_obj_get_list (void);
  OBJECT*     jones_obj_get (char *id);
  int         jones_obj_add (OBJECT *o);
  int         jones_obj_del (OBJECT *o);
  int         jones_obj_fact_query (char *query);

  OBJECT* jones_obj_new (char *id);
  int     jones_obj_free (OBJECT *o);

  int     jones_obj_add_fact (OBJECT *o, FACT *f);
  int     jones_obj_del_fact (OBJECT *o, int indx);
  FACT*    jones_obj_get_fact (OBJECT *o, char *id);
  FACT*    jones_obj_get_fact_by_indx (OBJECT *o, int indx);
  FACT*    jones_obj_get_or_create_fact (OBJECT *o, char*id, int val); 
  FACT*    jones_obj_get_or_create_fact1 (OBJECT *o, char*id, int val); 
  int     jones_obj_get_fact_val (OBJECT *o, char *id);
#ifdef __cplusplus
}
#endif


#endif
