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

#ifndef LOGIC_ENG_H
#define LOGIC_ENG_H


#include "nyx_list.h"

typedef struct kb_t KB;

/* Logical Operators */
#define OP_VAL   1
#define OP_NOT   2
#define OP_AND   3
#define OP_OR    4
#define OP_SET   5
#define OP_TRUE  6
#define OP_FALSE 7
#define OP_UNKN  8
#define OP_STR   9
#define OP_MSG   10
#define OP_VAR   11



typedef struct lena_item_t
{
  int  op;
  void *val;
} LENA_ITEM;

typedef struct lena_op_t
{
  int  op;
  int  val;
} LENA_OP;


typedef struct lena_expr_t
{
  NYX_BASIC_ITEM    bi;
  LENA_ITEM *i;
  int        n;
  char      *str;
} LENA_EXPR;


#ifdef __cplusplus
extern "C" {
#endif

  LENA_EXPR* jones_lena_parse (KB *kb, char *str);
  int        jones_lena_free (LENA_EXPR *e);
  int        jones_lena_set_id (LENA_EXPR *e, char *n);
  int        jones_lena_run (LENA_EXPR *e, int* r);

  int        jones_lena_expr_add_item (LENA_EXPR *e, int op, void*val);

#ifdef __cplusplus
}
#endif


#endif
