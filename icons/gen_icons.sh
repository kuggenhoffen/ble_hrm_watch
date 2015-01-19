#!/bin/bash

for file in original/*.png
do
	newfile=${file//-/_}
	convert $file -background white -flatten -level 20%,20% xbm:${newfile//2x.png/icon.h}
	convert $file -background white -flatten xbm:${newfile//2x.png/icon.xbm}
	convert $file bmp:${newfile//2x.png/icon.bmp}
	sed -i 's/static char/static const uint8_t/g' ${newfile//2x.png/icon.h}
	cp original/*.h .
done
