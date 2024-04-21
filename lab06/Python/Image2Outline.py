import numpy as np
from PIL import Image, ImageFilter
import sys
import cv2 as cv

FileName = "trudny_wariant.png"
if(len(sys.argv) > 1):
    FileName = sys.argv[1]


image = Image.open(FileName)
#image = image.crop((300, 700, 2500, 2900))
image = image.resize((image.size[0] // 2, image.size[1] // 2), Image.Resampling.NEAREST)
image = image.convert('RGBA')

pix = np.array(image)
for j in range(image.size[0]):
    for i in range(image.size[1]):
        # Different thresholds (depending on the image)
        #if sum(pix[i, j]) / (len(pix[i, j])) < 127 and pix[i, j][3] > 10:
        if sum(pix[i, j]) / (len(pix[i, j])) < 250 and pix[i, j][3] > 10:
        #if 0 <= i <= 1000 and 0 <= j <= 1000:
            pix[i, j][0] = 0
            pix[i, j][1] = 0
            pix[i, j][2] = 0
            pix[i, j][3] = 255
        else:
            pix[i, j][0] = 255
            pix[i, j][1] = 255
            pix[i, j][2] = 255
            pix[i, j][3] = 255

image = Image.fromarray(pix)
#image.show()
#image.save('Custom_MASK_rect.png')


image = image.filter(ImageFilter.CONTOUR)
image.show()
image.save("Outline_" + FileName)