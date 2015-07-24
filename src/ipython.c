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
  int ret = 0;
  PyObject *pFunc, *pValue, *pArgs;
      
  pFunc = PyObject_GetAttrString(pModule, r->bi.id); 

  /* pFunc is a new reference */
  if (pFunc && PyCallable_Check(pFunc))
    {
      pArgs = PyTuple_New (3);

      /* Parameters pased as opaque ids (long) */
      pValue = PyInt_FromLong((long)r);
      PyTuple_SetItem(pArgs, 0, pValue);

      pValue = PyInt_FromLong((long)o);
      PyTuple_SetItem(pArgs, 1, pValue);

      pValue = PyInt_FromLong((long)f);
      PyTuple_SetItem(pArgs, 2, pValue);

      pValue = PyObject_CallObject(pFunc, pArgs);
      if (PyErr_Occurred())
        PyErr_Print();

      Py_DECREF(pArgs);
      if (pValue != 0)
	{
	  ret = (int)PyInt_AsLong(pValue);
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
    
  return ret;
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
#ifdef DEBUG1
      fprintf (stderr, "Object (%s) already exists\n", obj_name);
#endif
      return Py_BuildValue("d", 0);      
    }
  jones_obj_add (jones_obj_new (obj_name));

  return Py_BuildValue("d", 0);
}

static PyObject*
jones_ipy_fact_get (PyObject *self, PyObject *args)
{
  FACT    *f;
  long    v;
     
  if(!PyArg_ParseTuple(args, "l", &v))
    return NULL;

  f = (FACT*)v;

  v = jones_fact_get (f);

  return Py_BuildValue("d", v);
}


static PyObject*
jones_ipy_fact_set (PyObject *self, PyObject *args)
{
  FACT    *f;
  long    r, v;
     
  if(!PyArg_ParseTuple(args, "ll", &v, &r))
    return NULL;

  f = (FACT*)v;

  v = jones_fact_set1 (f, (int)r);

  return Py_BuildValue("d", v);
}



static PyObject*
jones_ipy_fact_get_obj (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    v, indx;
     
  if(!PyArg_ParseTuple(args, "ll", &v, &indx))
    return NULL;

  f = (FACT*)v;

  if (indx < f->olist->n )
    o = (OBJECT*) f->olist->item[indx];
  else
    o = NULL;

  return Py_BuildValue("l", (long)o);
}

static PyObject*
jones_ipy_fact_add_obj (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    p_o, v;
     
  if(!PyArg_ParseTuple(args, "ll", &v, &p_o))
    return NULL;

  f = (FACT*)v;
  o = (OBJECT*) p_o;
  
  jones_fact_add_obj (f, o);

  return Py_BuildValue("l", 0);
}

static PyObject*
jones_ipy_fact_set_robj (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    p_o, v, indx;
     
  if(!PyArg_ParseTuple(args, "lll", &v, &p_o, &indx))
    return NULL;

  f = (FACT*)v;
  o = (OBJECT*) p_o;
  
  jones_fact_set_robj (f, o, indx);

  return Py_BuildValue("l", 0);
}


static PyObject*
jones_ipy_fact_get_robj (PyObject *self, PyObject *args)
{
  OBJECT  *o, *o1;
  FACT    *f;
  long    p_o, indx;
  char    *fact_name;
     
  if(!PyArg_ParseTuple(args, "lsl", &p_o, &fact_name, &indx))
    return NULL;

  o = (OBJECT*) p_o;
  f = (FACT*)jones_obj_get_fact (o, fact_name);

  o1 = NULL;
  /* FIXME: Process errors */
  if (f)
    {
      o1 = jones_fact_get_robj (f, indx);
      return Py_BuildValue("l", (long)o1);
    }
  else
    return Py_BuildValue("l", 0);
}



/* DEBUG */
static PyObject*
jones_ipy_fact_dump (PyObject *self, PyObject *args)
{
  FACT    *f;
  long    v;
     
  if(!PyArg_ParseTuple(args, "l", &v))
    return NULL;

  f = (FACT*)v;

  jones_fact_dump (f);
  return Py_BuildValue("d", 0);
}



static PyObject*
jones_ipy_obj_get_or_create_fact (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    p_o, v;
  char    *fact_name;
     
  if(!PyArg_ParseTuple(args, "lsl", &p_o, &fact_name, &v))
    return NULL;
  o = (OBJECT*) p_o;

  f = jones_obj_get_or_create_fact1 (o, fact_name, v);

  return Py_BuildValue("l", (long)f);
}


static PyObject*
jones_ipy_obj_update_or_create_fact (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    p_o, v;
  char    *fact_name;
     
  if(!PyArg_ParseTuple(args, "lsl", &p_o, &fact_name, &v))
    return NULL;
  o = (OBJECT*) p_o;

  f = jones_obj_get_or_create_fact1 (o, fact_name, v);
  jones_fact_set1 (f, v);

  return Py_BuildValue("l", (long)f);
}


static PyObject*
jones_ipy_obj_get_fact_val (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  long    p_o, v;
  char    *fact_name;
     
  if(!PyArg_ParseTuple(args, "ls", &p_o, &fact_name))
    return NULL;
  o = (OBJECT*) p_o;

  v = jones_obj_get_fact_val (o, fact_name);

  return Py_BuildValue("l", (long)v);
}


static PyObject*
jones_ipy_obj_get_name (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  long    p_o;

     
  if(!PyArg_ParseTuple(args, "l", &p_o))
    return NULL;
  o = (OBJECT*) p_o;

  return Py_BuildValue("s",  OBJ_ID(o));
}



static PyObject*
jones_ipy_imply (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  FACT    *f;
  long    p_o, v;
  char    *fact_name, *fact_name1, *f1, *f2;
  int     n1, n2;
     
  if(!PyArg_ParseTuple(args, "lss", &p_o, &fact_name, &fact_name1))
    return NULL;

  o = (OBJECT*) p_o;
  n1 = n2 =1;
  f1 = fact_name;
  f2 = fact_name1;

  if (fact_name[0] == '!') 
    {
      n1 = 0;
      f1 = fact_name + 1;
    }
  if (fact_name1[0] == '!')
    {
      n2 = 0;
      f2= fact_name1 + 1;
    }
  
  v = jones_obj_get_fact_val (o, f1);
  f = jones_obj_get_fact (o, f2);

  if (!f) 
    {
      f = jones_obj_get_or_create_fact1 (o, f2, !n2);
    }
  if (v == n1) jones_fact_set1 (f, n2);

  return Py_BuildValue("l", (long) (v == n1));
}


static PyObject*
jones_ipy_imply_ex (PyObject *self, PyObject *args)
{
  OBJECT  *o, *o1;
  FACT    *f;
  long    p_o, p_o1, v;
  char    *fact_name, *fact_name1, *f1, *f2;
  int     n1, n2;
     
  if(!PyArg_ParseTuple(args, "lsls", &p_o, &fact_name, &p_o1, &fact_name1))
    return NULL;
#ifdef DEBUG1
  printf ("Adding fact '%s' to object %ld (%p)\n", 
	  fact_name, p_o, (OBJECT*)p_o);
#endif
  o = (OBJECT*) p_o;
  o1 = (OBJECT*) p_o1;
  n1 = n2 =1;
  f1 = fact_name;
  f2 = fact_name1;

  if (fact_name[0] == '!') 
    {
      n1 = 0;
      f1 = fact_name + 1;
    }
  if (fact_name1[0] == '!')
    {
      n2 = 0;
      f2= fact_name1 + 1;
    }

  
  v = jones_obj_get_fact_val (o, f1);
  f = jones_obj_get_fact (o1, f2);
  if (!f) 
    {
      f = jones_obj_get_or_create_fact1 (o1, f2, !n2);
    }
  if (v == n1) jones_fact_set1 (f, n2);
  else jones_fact_set1 (f, !n2);
  return Py_BuildValue("l", (long) (v == n1));
}


static PyObject*
jones_ipy_find_obj (PyObject *self, PyObject *args)
{
  OBJECT  *o;
  char *obj_name;
     
  if(!PyArg_ParseTuple(args, "s", &obj_name))
    return NULL;

  if ((o = jones_obj_get (obj_name)) != NULL)
    {
#ifdef DEBUG1
      fprintf (stderr, "Object (%s) already exists\n", obj_name);
#endif
      return Py_BuildValue("l", (long)o);      
    }


  return Py_BuildValue("l", 0);
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
    {"fact_get", jones_ipy_fact_get, METH_VARARGS,
     "Gets value of given fact."},
    {"fact_set", jones_ipy_fact_set, METH_VARARGS,
     "Sets value of given fact."},
    {"fact_get_obj", jones_ipy_fact_get_obj, METH_VARARGS,
     "Gets object associated to fact."},
    {"fact_add_obj", jones_ipy_fact_add_obj, METH_VARARGS,
     "Gets object associated to fact."},
    {"fact_set_robj", jones_ipy_fact_set_robj, METH_VARARGS,
     "Gets object associated to fact."},
    {"fact_get_robj", jones_ipy_fact_get_robj, METH_VARARGS,
     "Gets object associated to fact."},
    {"find_obj", jones_ipy_find_obj, METH_VARARGS,
     "Finds and object by name."},

    {"obj_get_or_create_fact", jones_ipy_obj_get_or_create_fact, METH_VARARGS,
     "Gets a fact assocaited to an object and creates it if not exists."},
    {"obj_get_fact_val", jones_ipy_obj_get_fact_val, METH_VARARGS,
     "Gets the value associated to the given fact on the given object."},
    {"obj_update_or_create_fact", jones_ipy_obj_update_or_create_fact, METH_VARARGS,
     "Gets a fact assocaited to an object and creates it if not exists."},

    {"imply", jones_ipy_imply, METH_VARARGS,
     "DEBUG: RUn a simple if X -> Y."},
    {"imply_ex", jones_ipy_imply_ex, METH_VARARGS,
     "DEBUG: RUn a simple if X then Y else ! Y."},
    {"obj_get_name", jones_ipy_obj_get_name, METH_VARARGS,
     "DEBUG: Returns the name of the given object."},

    {"fact_dump", jones_ipy_fact_dump, METH_VARARGS,
     "DEBUG: Dumps the provided fact to the console."},
    
    {NULL, NULL, 0, NULL}
  };


int    
jones_python_init (void)
{
  Py_Initialize();

  Py_InitModule("cjones", kb_access);

  return 0;
}

int    
jones_python_load_script (char *name)
{
  PyObject *pName;

  pName = PyString_FromString(name);
  pModule = PyImport_Import(pName);

  if (PyErr_Occurred())
    {
      PyErr_Print();
      exit (1);
    }

  Py_DECREF(pName);
  printf ("+ Loading script: %s\n", name);
  return 0;
}

int
jones_python_populate (void)
{
  PyObject *pFunc;
      
  if (!pModule) return -1;
  pFunc = PyObject_GetAttrString(pModule, "populate_kb"); 
#ifdef DEBUG1
  printf ("%s:: %p %p\n", __FUNCTION__, pModule, pFunc);
#endif

  /* pFunc is a new reference */
  if (pFunc && PyCallable_Check(pFunc))
    {
      PyObject_CallObject(pFunc, NULL);
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
