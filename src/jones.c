#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objs.h"
#include "facts.h"
#include "rules.h"

#define VERSION "0.1"



/* Rules */
int
is_sister_of (RULE *r, OBJECT *o, FACT *f)
{
  OBJECT  *o1;
  FACT    *f1;
  int     v;

  if (jones_fact_get (f))
    {
      jones_fact_dump (f);

      f1 = jones_obj_get_or_create_fact (o, "IS_FEMALE", FACT_TRUE);

      o1 = (OBJECT*) f->olist->item[0];

      if ((jones_obj_get_fact_val (o1, "IS_FEMALE")) == FACT_TRUE)
	{
	  // If target is a female then it is also a sister
	  f1 = jones_obj_get_or_create_fact (o1, "IS_SISTER_OF", FACT_TRUE);
	  jones_fact_add_obj (f1, o);
	}
      return RULE_FIRED;
    }

  return RULE_NOT_FIRED;
}

int
main (int argc, char *argv[])
{
  OBJECT   *o1, *o2, *o3;
  FACT     *f;
  RULE     *r;

  printf ("JONEs Version " VERSION "\n");
  jones_obj_init ();
  jones_rule_init ();

  /* Simple facts test */
  jones_obj_add (jones_obj_new ("John"));
  jones_obj_add (jones_obj_new ("Sally"));
  jones_obj_add (jones_obj_new ("Jane"));

  f = jones_fact_new ("IS_SISTER_OF");
  jones_fact_set (f, FACT_TRUE);
  jones_fact_add_obj (f, jones_obj_get ("Jane"));

  o1 = jones_obj_get ("Sally");
  jones_obj_add_fact (o1, f);
  f = jones_fact_new ("IS_FEMALE");
  jones_obj_add_fact (o1, f);

  jones_obj_get_or_create_fact (jones_obj_get("Jane"), "IS_FEMALE", FACT_TRUE);

  jones_obj_dump ();

  /* Add some rules */
  r = jones_rule_new ("RULE-00", is_sister_of);
  jones_rule_add_firing_fact (r, "IS_SISTER_OF");
  jones_rule_add (r);


  jones_rule_resolve_fact (jones_obj_get ("Jane"), 
			   "IS_SISTER_OF");

  return 0;
} 


// Simple objects test 
  /*
  jones_obj_dump ();
  jones_obj_add (jones_obj_new ("Test1"));
  jones_obj_add (jones_obj_new ("Test2"));
  jones_obj_add (jones_obj_new ("Test3"));
  jones_obj_dump ();

  o = jones_obj_get("Test2");
  printf ("Object '%s': %p\n", OBJ_ID(o), o);
  printf ("Object '%s': %p\n", o->bi.id, o);
  */
 
