/*
 * Jones: A basic rule-engine system
 * Copyright (c) 2015 David Mart.nez Oliveira
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
#ifndef IPYTHON_H
#define IPYTHON_H

#include <Python.h>


#ifdef __cplusplus
extern "C" {
#endif

  int    jones_python_init (void);
  int    jones_python_load_script (char *name);
  int    jones_python_populate (void);
  int    jones_python_run_str (char *name);

#ifdef __cplusplus
}
#endif

#endif
