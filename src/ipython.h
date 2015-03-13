#ifndef IPYTHON_H
#define IPYTHON_H

#include <Python.h>


#ifdef __cplusplus
extern "C" {
#endif

  int    jones_python_init ();
  int    jones_python_load_script (char *name);
  int    jones_python_populate ();
  int    jones_python_run_str (char *name);

#ifdef __cplusplus
}
#endif

#endif
