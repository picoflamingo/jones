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

#ifndef LENA_H
#define LENA_H


#include <nyx_list.h>
#include <objs.h>

typedef struct kb_t KB;

/* Logical Operators */
#define OP_VAL   1
#define OP_NOT   2
#define OP_AND   3
#define OP_OR    4
#define OP_SET   5
#define OP_STR   9
#define OP_MSG   10
#define OP_VAR   11
#define OP_REF   12
#define OP_VREF  13
#define OP_CONST 14


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

typedef struct lena_var_t
{
  char  *id;
  char  *fact;
} LENA_VAR;


typedef struct lena_expr_t
{
  NYX_BASIC_ITEM    bi;
  LENA_ITEM *i;
  LENA_VAR  *v;
  int        n;
  char      *str;
  int        npar;
  int        fired;
} LENA_EXPR;


#ifdef __cplusplus
extern "C" {
#endif

  LENA_EXPR* jones_lena_parse (KB *kb, char *str);
  int        jones_lena_free (LENA_EXPR *e);
  int        jones_lena_set_id (LENA_EXPR *e, char *n);
  int        jones_lena_run_with_par (KB *kb, LENA_EXPR *e, FACT *o, int* r);

  int        jones_lena_expr_add_item (LENA_EXPR *e, int op, void*val);
  LENA_VAR*  jones_lena_expr_add_var  (LENA_EXPR *e, char *str);

#ifdef __cplusplus
}
#endif


#endif
