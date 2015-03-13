#ifndef RULES_H
#define RULES_H

#include "list.h"
#include "facts.h"
#include "objs.h"

#define RULE_FIRED 1
#define RULE_NOT_FIRED 0

struct rule_t;

typedef int (*RULE_IMP) (struct rule_t*r, OBJECT *o, FACT *f);

typedef struct rule_t
{
  BASIC_ITEM  bi;
  char        *ffact;
  RULE_IMP    f;
  void        *data;
} RULE;

#ifdef __cplusplus
extern "C" {
#endif

  int   jones_rule_init ();
  int   jones_rule_dump ();
  int   jones_resolve ();

  int   jones_rule_add (RULE *r);
  int   jones_rule_set_data (RULE *r, void *data);
  void* jones_rule_get_data (RULE *r);

  RULE* jones_rule_new (char *id, RULE_IMP f);
  int   jones_rule_add_firing_fact (RULE *r, char *fid);
  int   jones_rule_eval (RULE *r, OBJECT *o);
  int   jones_rule_resolve_fact (OBJECT *o, char *fid);

#ifdef __cplusplus
}
#endif

#endif
