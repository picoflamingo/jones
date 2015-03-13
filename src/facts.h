#ifndef FACTS_H
#define FACTS_H

#include "list.h"


typedef void *_OBJ;

#define FACT_FALSE 0
#define FACT_TRUE  1
#define FACT_UNKNOWN 2

typedef struct fact_t
{
  BASIC_ITEM bi;
  void    *obj;   /* Object this fact is related to */
  LIST    *olist; /* List of objects related to the fact.*/
  int     value;
  int     fire;
} FACT;

#ifdef __cplusplus
extern "C" {
#endif

  int   jones_fact_init ();
  int   jones_fact_dump (FACT *f);

  FACT* jones_fact_new (char *id);
  int   jones_fact_free (FACT *f);

  int   jones_fact_set (FACT *f, int val);
  int   jones_fact_get (FACT *f);
  int   jones_fact_set_obj (FACT *f, _OBJ o);
  int   jones_fact_add_obj (FACT *f, _OBJ o);

#ifdef __cplusplus
}
#endif


#endif
