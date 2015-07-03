#ifndef FACTS_H
#define FACTS_H

//#include "list.h"
#include <nyx_list.h>


typedef void *_OBJ;

#define FACT_FALSE 0
#define FACT_TRUE  1
#define FACT_UNKNOWN 2

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
