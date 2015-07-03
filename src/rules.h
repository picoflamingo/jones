#ifndef RULES_H
#define RULES_H

//#include "list.h"
#include <nyx_list.h>
#include "facts.h"
#include "objs.h"

#define RULE_FIRED 1
#define RULE_NOT_FIRED 0

struct rule_t;

typedef int (*RULE_IMP) (struct rule_t*r, OBJECT *o, FACT *f);

typedef struct rule_t
{
  NYX_BASIC_ITEM  bi;
  char            *ffact;
  RULE_IMP        f;
  void            *data;
} RULE;

typedef struct simple_rule_data_t
{
  char *f1;
  char *f2;
  int  n1, n2;
} SIMPLE_RULE_DATA;

#ifdef __cplusplus
extern "C" {
#endif

  int   jones_rule_init (void);
  int   jones_rule_dump (void);
  int   jones_resolve (void);

  int   jones_rule_add (RULE *r);
  int   jones_rule_set_data (RULE *r, void *data);
  void* jones_rule_get_data (RULE *r);

  RULE* jones_rule_new (char *id, RULE_IMP f);
  int   jones_rule_add_firing_fact (RULE *r, char *fid);
  int   jones_rule_eval (RULE *r, OBJECT *o);
  int   jones_rule_resolve_fact (OBJECT *o, char *fid);
  int   jones_rule_full_eval ();

  SIMPLE_RULE_DATA* jones_rule_add_srule_data (RULE *r, char *rule);
  int   jones_rule_simple_rule (RULE*r, OBJECT *o, FACT *f);

#ifdef __cplusplus
}
#endif

#endif
