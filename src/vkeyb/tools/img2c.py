#!/bin/python3
# Convert an image to a C array in RGB565 format.
import os
import sys
from PIL import Image

def rgba888ToRgb565(rgba):
    return ((rgba[0] >> 3) << 11) | ((rgba[1] >> 2) << 5) | (rgba[2] >> 3)

def rgba888ToAbgr1555(rgba):
    return ((rgba[2] >> 3) << 10) | ((rgba[1] >> 3) << 5) | (rgba[0] >> 3)

filename = sys.argv[1]
img = Image.open(filename).convert('RGB')
pixels = img.load()

c_var_base_name = (os.path.splitext(os.path.basename(filename))[0]).upper()

print("#define " + c_var_base_name + "_IMG_WIDTH " + str(img.width))
print("#define " + c_var_base_name + "_IMG_HEIGHT " + str(img.height))
print("")

print("#if defined(SUPPORT_ARGB1555)")
print("const uint16_t " + c_var_base_name + "_IMG_DATA[] =")
print("{")
for y in range(0, img.height):
    print("  ", end='')
    for x in range(0, img.width):
        print('0x{0:0{1}X}'.format(rgba888ToAbgr1555(pixels[x, y]), 4), end='')
        if ((y != img.height - 1) or (x != img.width - 1)):
           print(",", end='')
    print("")
print("};")

print("")

print("#else")
print("const uint16_t " + c_var_base_name + "_IMG_DATA[] =")
print("{")
for y in range(0, img.height):
    print("  ", end='')
    for x in range(0, img.width):
        print('0x{0:0{1}X}'.format(rgba888ToRgb565(pixels[x, y]), 4), end='')
        if ((y != img.height - 1) or (x != img.width - 1)):
           print(",", end='')
    print("")
print("};")
print("#endif")
