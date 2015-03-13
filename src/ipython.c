#include "ipython.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objs.h"
#include "facts.h"
#include "rules.h"



/* XXX: Check if we really need this */
static PyObject *pModule = NULL;


static PyObject*
jones_ipy_new_fact (PyObject *self, PyObject *args)
{
  FACT    *f;
  OBJECT  *o;

  char *obj_name, *fact_name, *fact_val;
     
  if(!PyArg_ParseTuple(args, "sss",
                       &obj_name, &fact_name, &fact_val
                       ))
    return NULL;
  printf ("Creating Fact: (%s) (%s) (%s)\n", obj_name, fact_name, fact_val);
  if (!(o = jones_obj_get (obj_name)))
    {
      fprintf (stderr, "Object (%s) not found\n", obj_name);
      return Py_BuildValue("d", 0);      
    }

  f = jones_fact_new (fact_name);
  if (fact_val[0] == 'T' || fact_val[0] == 't')
    jones_fact_set (f, FACT_TRUE);
  else if (fact_val[0] == 'F' || fact_val[0] == 'f')
    jones_fact_set (f, FACT_FALSE);
  else
    jones_fact_set (f, FACT_UNKNOWN);

  //jones_fact_add_obj (f, jones_obj_get(obj_name));
  jones_obj_add_fact (o, f);

  return Py_BuildValue("d", 0);
}


static PyObject*
jones_ipy_new_fact2 (PyObject *self, PyObject *args)
{
  FACT    *f;
  OBJECT  *o, *o1;

  char *obj_name, *obj_name1, *fact_name, *fact_val;
     
  if(!PyArg_ParseTuple(args, "ssss",
                       &obj_name, &fact_name, &obj_name1, &fact_val
                       ))
    return NULL;
  printf ("Creating Fact: (%s) (%s) (%s) (%s)\n", 
	  obj_name, fact_name, obj_name1, fact_val);

  if (!(o = jones_obj_get (obj_name)))
    {
      fprintf (stderr, "Object (%s) not found\n", obj_name);
      return Py_BuildValue("d", 0);      
    }
  if (!(o1 = jones_obj_get (obj_name1)))
    {
      fprintf (stderr, "Object (%s) not found\n", obj_name);
      return Py_BuildValue("d", 0);      
    }

  f = jones_fact_new (fact_name);
  if (fact_val[0] == 'T' || fact_val[0] == 't')
    jones_fact_set (f, FACT_TRUE);
  else if (fact_val[0] == 'F' || fact_val[0] == 'f')
    jones_fact_set (f, FACT_FALSE);
  else
    jones_fact_set (f, FACT_UNKNOWN);

  jones_fact_add_obj (f, o1);
  jones_obj_add_fact (o, f);


  return Py_BuildValue("d", 0);
}



int
rule_wrapper (RULE *r, OBJECT *o, FACT *f)
{
  PyObject *pFunc, *pValue, *pArgs;
      
  pFunc = PyObject_GetAttrString(pModule, r->bi.id); 
  printf ("%s:: %p %p\n", __FUNCTION__, pModule, pFunc);
  /* pFunc is a new reference */
  if (pFunc && PyCallable_Check(pFunc))
    {
      pArgs = PyTuple_New(3);
      // Parameters pased as opaque ids
      pValue = PyInt_FromLong(r);
      PyTuple_SetItem(pArgs, 0, pValue);

      pValue = PyInt_FromLong(o);
      PyTuple_SetItem(pArgs, 1, pValue);

      pValue = PyInt_FromLong(f);
      PyTuple_SetItem(pArgs, 2, pValue);

      pValue = PyObject_CallObject(pFunc, pArgs);
      if (PyErr_Occurred())
        PyErr_Print();

      Py_DECREF(pArgs);
      if (pValue != 0)
	{
	  Py_DECREF (pValue);
	}

    }
  else
    {
      if (PyErr_Occurred())
        PyErr_Print();
      fprintf(stderr, "Cannot find function \"%s\"\n", "populate_kb");
    }
  Py_XDECREF(pFunc);
    
  return 0;
}


static PyObject*
jones_ipy_new_rule (PyObject *self, PyObject *args)
{
  RULE    *r;
  char    *rule_name, *firing_fact;
     
  if(!PyArg_ParseTuple(args, "ss", &rule_name,  &firing_fact))
    return NULL;

  printf ("Creating Rule: (%s)\n", rule_name);

  r = jones_rule_new (rule_name, rule_wrapper);
  jones_rule_add_firing_fact (r, firing_fact);
  jones_rule_add (r);


  return Py_BuildValue("d", 0);
}




static PyObject*
jones_ipy_new_obj (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  char *obj_name;
     
  if(!PyArg_ParseTuple(args, "s", &obj_name))
    return NULL;

  printf ("Creating object: (%s)\n", obj_name);
  if ((o = jones_obj_get (obj_name)) != NULL)
    {
      fprintf (stderr, "Object (%s) already exists\n", obj_name);
      return Py_BuildValue("d", 0);      
    }
  jones_obj_add (jones_obj_new (obj_name));

  return Py_BuildValue("d", 0);
}




static PyMethodDef kb_access[] =
  {
    {"new_obj", jones_ipy_new_obj, METH_VARARGS,
     "Adds a object to Knowledge base."},

    {"new_fact", jones_ipy_new_fact, METH_VARARGS,
     "Adds a fact to Knowledge base."},

    {"new_fact2", jones_ipy_new_fact2, METH_VARARGS,
     "Adds a Binary fact to Knowledge base."},

    {"new_rule", jones_ipy_new_rule, METH_VARARGS,
     "Adds a Rule to Knowledge base."},
    

    {NULL, NULL, 0, NULL}
  };


int    
jones_python_init ()
{
  Py_Initialize();

  Py_InitModule("cjones", kb_access);

  /* register functions to access KB */
  PyRun_SimpleString ("print 'Hello from Python!'");

  return 0;
}

int    
jones_python_load_script (char *name)
{
  PyObject *pName;

  pName = PyString_FromString(name);
  pModule = PyImport_Import(pName);
  if (PyErr_Occurred())
    PyErr_Print();
  Py_DECREF(pName);
  printf ("+ Loading script (%p)\n", pModule);
  //PyRun_SimpleString ("import jones");
  return 0;
}

int
jones_python_populate ()
{
  PyObject *pFunc, *pValue;
      
  pFunc = PyObject_GetAttrString(pModule, "populate_kb"); 
  printf ("%s:: %p %p\n", __FUNCTION__, pModule, pFunc);
  /* pFunc is a new reference */
  if (pFunc && PyCallable_Check(pFunc))
    {
      pValue = PyObject_CallObject(pFunc, NULL);
      if (PyErr_Occurred())
        PyErr_Print();

    }
  else
    {
      if (PyErr_Occurred())
        PyErr_Print();
      fprintf(stderr, "Cannot find function \"%s\"\n", "populate_kb");
    }
  Py_XDECREF(pFunc);
    
  return 0;
}



/* Enough to invoke a python method */
int    
jones_python_run_str (char *pycode)
{
  PyRun_SimpleString (pycode);
  return 0;
}
