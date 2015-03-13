#ifndef OBJECTS_H
#define OBJECTS_H

#include "list.h"
#include "facts.h"


typedef struct object_t
{
  BASIC_ITEM    bi;
  LIST          *facts; /* List of associated facts*/
  // We might add properties/Class later.
} OBJECT;



#ifdef __cplusplus
extern "C" {
#endif

  int     jones_obj_init ();
  int     jones_obj_dump ();
  LIST*   jones_obj_get_list ();
  OBJECT* jones_obj_get (char *id);
  int     jones_obj_add (OBJECT *o);
  int     jones_obj_del (OBJECT *o);


  OBJECT* jones_obj_new (char *id);
  int     jones_obj_free (OBJECT *o);

  int     jones_obj_add_fact (OBJECT *o, FACT *f);
  int     jones_obj_del_fact (OBJECT *o, int indx);
  FACT*    jones_obj_get_fact (OBJECT *o, char *id);
  FACT*    jones_obj_get_fact_by_indx (OBJECT *o, int indx);
  FACT*    jones_obj_get_or_create_fact (OBJECT *o, char*id, int val);
  int     jones_obj_get_fact_val (OBJECT *o, char *id);
#ifdef __cplusplus
}
#endif


#endif
