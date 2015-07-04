JONES
----------------------------
(c) 2015 David Martinez Oliveira
See the end of the file for license conditions.


Description
------------
Jones is a very basic rule engine used to learn more about basic AI concepts. Rules are defined in python

Jones uses libnyx generic functions, so you need to install it first (https://github.com/picoflamingo/nyx)

Command. Line
-------------------------
In order to let Jones find the python Knowledge base you need to export the following environmental variable

export PYTHONPATH=.

When the application is executed without any parameter, the system will try to load the python script named "jones.py" in the current Python Path. Otherwise it will try to load the script provided in the command line. For example:

./jones test

*Note that the file extension is omitted. Command above loads file test.py*



Check doc/cmdline.txt for details

.

This file is part of Jones

Jones is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Jones is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jones.  If not, see <http://www.gnu.org/licenses/>.
