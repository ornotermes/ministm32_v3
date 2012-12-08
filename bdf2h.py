#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#+	Copyright (c) 2012 Rikard Lindstrom <ornotermes@gmail.com>
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
import argparse

print("// bdf2h - Extract data from font to header format")
print("// Command: %s" % " ".join(sys.argv))

def argParser():
    p = argparse.ArgumentParser()

    p.add_argument( "-f", "--file", dest="file", default="-", metavar="/file.png", help="")
    p.add_argument( "-n", "--name", dest="name", default="", metavar="file", help="Name of the output structure.")
    p.add_argument( "-l", "--last", dest="last", default="255", metavar="n", help="Last charecter to convert.")
    
    return vars(p.parse_args())

arg = argParser()

font = open(arg["file"])

inChar = False
tempLine = ""
charNum = 0

def nextLine():
	return font.nextline()

for line in font:
	lineSplit = line.split(" ")
	if lineSplit[0] == "STARTCHAR":
		tempLine = "//" + lineSplit[1].rstrip("\n")
	if lineSplit[0] == "ENCODING":
		charNum = int(lineSplit[1].rstrip("\n"))
		if charNum <= int(arg["last"]):
			print tempLine + ": " + str(charNum)
	if line == "ENDCHAR\n":
		inChar = False
		if charNum <= int(arg["last"]):
			print tempLine
	if inChar:
		tempLine += "0x" + line[0] + line[1] + ", "
	if line == "BITMAP\n":
		if charNum <= int(arg["last"]):
			inChar = True
		tempLine = ""
