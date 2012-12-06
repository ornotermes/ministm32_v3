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
from PIL import Image, ImageDraw

print("// img2h - Convert image to header with a colormap in 16 bit RRRRRGGGGGGBBBBB")
print("// Command: %s" % " ".join(sys.argv))

def argParser():
    p = argparse.ArgumentParser()

    p.add_argument( "-c", "--colors", dest="colors", default="256", metavar="nnn", help="Number of colors if downscaling, max 256.")
    p.add_argument( "-d", "--dither", dest="dither", action="store_const", default=False, const=True, help="Diether if downscaling.")
    p.add_argument( "-f", "--file", dest="file", default="-", metavar="/file.png", help="")
    p.add_argument( "-n", "--name", dest="name", default="", metavar="file", help="Name of the output structure.")
    p.add_argument( "-o", "--output", dest="output", default="", metavar="file", help="Name of output file.")
    
    return vars(p.parse_args())

arg = argParser()
	
colors = int(arg["colors"])

srcImg = Image.open(arg["file"])
if arg["dither"]:	
	pltImg = srcImg.convert("RGB").convert("P", colors=colors, palette=Image.ADAPTIVE)
	tmpImg = srcImg.copy()
	tmpImg.palette = pltImg.palette
	newImg = tmpImg._new(tmpImg.im.convert('P', Image.FLOYDSTEINBERG))
else:
	newImg = srcImg.convert("RGB").convert("P", palette=Image.ADAPTIVE, colors=colors)
if(len(arg["output"]) > 0):
	newImg.save(arg["output"])

print ("static const uint16_t {0}_width = {1};".format(arg["name"], newImg.size[0]))
print ("static const uint16_t {0}_height = {1};".format(arg["name"], newImg.size[1]))
print ("static const uint16_t {0}_colors[] = {{".format(arg["name"]))

color_map = newImg.resize((colors, 1))
color_map.putdata(range(colors))
color_map = list(color_map.convert("RGB").getdata())
for color in color_map:
	print ("\t" + hex( ((color[0]&0xf8)<<8) | ((color[1]&0xfc)<<3) | ((color[2]&0xf8)>>3) )+", //R:%i G:%i B:#%i" % color)
print ("};");
	
print ("static const uint8_t {0}_data[] = {{".format(arg["name"]))
imgString=""
for pixel in list(newImg.getdata()):
	imgString += (str(pixel) + ", ")
	if (len(imgString) > 59):
		print ("\t" + imgString)
		imgString = ""
print ("\t" + imgString)
print ("};");
