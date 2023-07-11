from PIL import Image
from sys import argv
import os

def checkPixel(pixel: tuple[float, float, float]):

    return pixel[0] > 127 and pixel[1] > 127 and pixel[2] > 127

if len(argv) != 2:

    print("error| writeFont exactly takes 1 argument")
    quit()

image = Image.open(argv[1]).convert('RGB')

if image.width != 128 or image.height != 128:

    print("error| the dimensions of the image must be 128x128")
    quit()

outName = os.path.splitext(os.path.basename(argv[1]))[0]

outString = f"""
bits 32

    global {outName}Font

section .data

{outName}Font:
"""

for outerY in range(8):
    for outerX in range(16):
        currBytes = []

        for innerY in range(16):
            currByte = 0

            for innerX in range(8):

                currByte <<= 1

                if checkPixel(image.getpixel((outerX * 8 + innerX, outerY * 16 + innerY))):

                    currByte |= 1

            currBytes.append(currByte)

        outString += '    db      ' + ', '.join(map(lambda x: hex(x), currBytes)) + '\n'

outFileName = os.path.splitext(argv[1])[0] + '.asm'

with open(outFileName, 'w+') as file:

    file.truncate()

    file.write(outString)

    print(argv[1] + ' -> ' + outFileName)

