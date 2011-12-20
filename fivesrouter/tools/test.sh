#!/bin/sh

for fil in rulelists/* ; do 
	echo $fil
	./validator $fil 
	echo ; echo ; echo 
done

